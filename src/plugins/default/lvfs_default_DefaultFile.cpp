/**
 * This file is part of lvfs.
 *
 * Copyright (C) 2011-2014 Dmitriy Vilkov, <dav.daemon@gmail.com>
 *
 * lvfs is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * lvfs is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with lvfs. If not, see <http://www.gnu.org/licenses/>.
 */

#include "lvfs_default_DefaultFile.h"
#include "lvfs_default_File.h"
#include "lvfs_default_Directory.h"

#include <brolly/assert.h>
#include <cstring>

#include <linux/limits.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>


namespace {
using namespace LVFS;

class Iterator : public DefaultFile::const_iterator
{
public:
    Iterator() :
        DefaultFile::const_iterator(new (std::nothrow) Imp())
    {}

    Iterator(const char *path) :
        DefaultFile::const_iterator(new (std::nothrow) Imp(path))
    {}

protected:
    union Buffer
    {
        struct dirent d;
        char b[offsetof(struct dirent, d_name) + NAME_MAX + 1];
    };

    class Imp : public DefaultFile::const_iterator::Implementation
    {
        PLATFORM_MAKE_NONCOPYABLE(Imp)
        PLATFORM_MAKE_NONMOVEABLE(Imp)

    public:
        Imp() :
            m_dir(NULL),
            m_entry(NULL)
        {
            memset(&m_path, 0, sizeof(m_path));
        }

        Imp(const char *path) :
            m_dir(NULL),
            m_entry(NULL)
        {
            memset(&m_buffer, 0, sizeof(m_buffer));
            strncpy(m_path, path, sizeof(m_path));

            if (m_dir = opendir(m_path))
                next();
        }

        virtual ~Imp()
        {
            closedir(m_dir);
        }

        virtual bool isEqual(const Holder &other) const
        {
            return (m_entry == NULL && other.as<Imp>()->m_entry == NULL) ||
                   (
                           (m_entry != NULL && other.as<Imp>()->m_entry != NULL) &&
                           strcmp(m_path, other.as<Imp>()->m_path) == 0 &&
                           strcmp(m_entry->d_name, other.as<Imp>()->m_entry->d_name) == 0
                   );
        }

        virtual reference asReference() const
        {
            return m_file;
        }

        virtual pointer asPointer() const
        {
            return &m_file;
        }

        virtual void next()
        {
            m_file.reset();

            while (readdir_r(m_dir, &m_buffer.d, &m_entry) == 0 && m_entry)
                if (m_entry->d_type == DT_DIR || m_entry->d_type == DT_UNKNOWN)
                {
                    if (strcmp(m_entry->d_name, ".") == 0 || strcmp(m_entry->d_name, "..") == 0)
                        continue;
                    else
                    {
                        char path[PATH_MAX];

                        if (UNLIKELY(std::snprintf(path, sizeof(path), "%s/%s", m_path, m_entry->d_name) < 0))
                        {
                            m_entry = NULL;
                            break;
                        }

                        m_file.reset(new (std::nothrow) Directory(path));
                        break;
                    }
                }
                else
                {
                    char path[PATH_MAX];

                    if (UNLIKELY(std::snprintf(path, sizeof(path), "%s/%s", m_path, m_entry->d_name) < 0))
                    {
                        m_entry = NULL;
                        break;
                    }

                    m_file.reset(new (std::nothrow) File(path));
                    break;
                }
        }

    private:
        DIR *m_dir;
        Buffer m_buffer;
        struct dirent *m_entry;
        Interface::Holder m_file;
        char m_path[PATH_MAX];
    };
};

}


namespace LVFS {

static uid_t userId = getuid();
static gid_t groupId = getgid();

int translatePermissions(const struct stat &st)
{
    int res = 0;

    if ((st.st_mode & S_IROTH) ||
        (st.st_uid == userId && (st.st_mode & S_IRUSR)) ||
        (st.st_gid == groupId && (st.st_mode & S_IRGRP)))
        res |= DefaultFile::Read;

    if ((st.st_mode & S_IWOTH) ||
        (st.st_uid == userId  && (st.st_mode & S_IWUSR)) ||
        (st.st_gid == groupId && (st.st_mode & S_IWGRP)))
        res |= DefaultFile::Write;

    if ((st.st_mode & S_IXOTH) ||
        (st.st_uid == userId  && (st.st_mode & S_IXUSR)) ||
        (st.st_gid == groupId && (st.st_mode & S_IXGRP)))
        res |= DefaultFile::Exec;

    return res;
}


DefaultFile::DefaultFile(const char *fileName) :
    m_file(NULL),
    m_isDir(false),
    m_isFile(false),
    m_isLink(false),
    m_exists(true),
    m_permissions(0),
    m_size(0),
    m_lastModified(0),
    m_filePath(strdup(fileName)),
    m_fileName(strrchr(m_filePath, '/') + 1)
{
    struct stat st;

    if (m_exists = (::lstat(m_filePath, &st) == 0))
        if (m_isFile = S_ISREG(st.st_mode))
        {
            m_size = st.st_size;
            m_permissions = translatePermissions(st);
            m_lastModified = st.st_mtime;
        }
        else if (m_isDir = S_ISDIR(st.st_mode))
        {
            m_permissions = translatePermissions(st);
            m_lastModified = st.st_mtime;
        }
        else
            if (m_isLink = S_ISLNK(st.st_mode))
            {
                char buff[PATH_MAX];

                if (::readlink(m_filePath, buff, PATH_MAX) == 0)
                    if (char *realName = ::canonicalize_file_name(buff))
                    {
                        if (m_exists = (stat(realName, &st) == 0))
                            if ((m_isFile = S_ISREG(st.st_mode)) || (m_isDir = S_ISDIR(st.st_mode)))
                            {
                                m_size = st.st_size;
                                m_permissions = translatePermissions(st);
                                m_lastModified = st.st_mtime;
                            }

                        free(realName);
                    }
            }
}

DefaultFile::DefaultFile(const char *fileName, const struct stat &st) :
    m_file(NULL),
    m_isDir(S_ISDIR(st.st_mode)),
    m_isFile(S_ISREG(st.st_mode)),
    m_isLink(S_ISLNK(st.st_mode)),
    m_exists(true),
    m_permissions(translatePermissions(st)),
    m_size(!m_isDir ? st.st_size : 0),
    m_lastModified(st.st_mtime),
    m_filePath(strdup(fileName)),
    m_fileName(strrchr(m_filePath, '/') + 1)
{}

DefaultFile::~DefaultFile()
{
    close();
    free(m_filePath);
}

Interface::Holder DefaultFile::open(const char *uri, Error &error)
{
    ASSERT(uri != NULL);
    char file[PATH_MAX];

    if (uri[0] == '/')
        strncpy(file, uri, sizeof(file));
    else
        if (getcwd(file, sizeof(file)))
        {
            if (std::snprintf(file + strlen(file), sizeof(file) - strlen(file) - 1, "/%s", uri) < 0)
            {
                error = errno;
                return Interface::Holder();
            }
        }
        else
            strncpy(file, uri, sizeof(file));

    struct stat st;

    if (::lstat(file, &st) != 0)
        error = errno;
    else
        if (S_ISREG(st.st_mode))
            return Interface::Holder(new (std::nothrow) File(file, st));
        else if (S_ISDIR(st.st_mode))
            return Interface::Holder(new (std::nothrow) Directory(file, st));
        else if (S_ISLNK(st.st_mode))
        {
            char buff[PATH_MAX] = {};
            struct stat st2;

            if (::readlink(file, buff, PATH_MAX) != 0)
                error = errno;
            else
                if (char *realName = ::canonicalize_file_name(buff))
                {
                    Interface::Holder res;

                    if (::stat(realName, &st2) != 0)
                        return Interface::Holder(new (std::nothrow) File(file, st));
                    else
                        if (S_ISREG(st2.st_mode))
                            res = Interface::Holder(new (std::nothrow) File(file, st2));
                        else if (S_ISDIR(st2.st_mode))
                            res = Interface::Holder(new (std::nothrow) Directory(file, st2));
                        else
                            error = ENOENT;

                    free(realName);
                    return res;
                }
                else
                    error = errno;
        }

    return Interface::Holder();
}

int DefaultFile::permissions() const
{
    return m_permissions;
}

bool DefaultFile::open()
{
    if (m_file != NULL)
        return true;
    else
        if ((m_file = fopen(m_filePath, "r+")) || (m_file = fopen(m_filePath, "r")))
            return true;
        else
            m_lastError = errno;

    return false;
}

size_t DefaultFile::read(void *buffer, size_t size)
{
    ASSERT(m_file != NULL);

    size_t res = fread(buffer, 1, size, m_file);
    m_lastError = errno;
    return res;
}

size_t DefaultFile::write(const void *buffer, size_t size)
{
    ASSERT(m_file != NULL);

    size_t res = fwrite(buffer, 1, size, m_file);
    m_lastError = errno;
    return res;
}

bool DefaultFile::seek(long offset, Whence whence)
{
    static const int system[FromEnd + 1] = { SEEK_SET, SEEK_CUR, SEEK_END };
    ASSERT(m_file != NULL);

    if (LIKELY(fseek(m_file, offset, system[whence]) == 0))
        return true;

    m_lastError = errno;
    return false;
}

bool DefaultFile::flush()
{
    ASSERT(m_file != NULL);

    if (LIKELY(fflush(m_file) == 0))
        return true;

    m_lastError = errno;
    return false;
}

void DefaultFile::close()
{
    if (m_file)
    {
        fclose(m_file);
        m_file = NULL;
    }
}

uint64_t DefaultFile::size() const
{
    return m_size;
}

size_t DefaultFile::position() const
{
    long res = ftell(m_file);
    m_lastError = errno;
    return res;
}

DefaultFile::const_iterator DefaultFile::begin() const
{
    return Iterator(m_filePath);
}

DefaultFile::const_iterator DefaultFile::end() const
{
    return Iterator();
}

Interface::Holder DefaultFile::entry(const char *name) const
{
    return Interface::Holder();
}

bool DefaultFile::rename(const Interface::Holder &file, const char *name)
{
    return false;
}

bool DefaultFile::remove(const Interface::Holder &file)
{
    return false;
}

const char *DefaultFile::title() const
{
    return m_fileName;
}

const char *DefaultFile::type() const
{
    return "";
}

const Error &DefaultFile::lastError() const
{
    return m_lastError;
}

}
