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

#include "lvfs_default_Directory.h"
#include "lvfs_default_Entry.h"
#include "lvfs_default_File.h"

#include <lvfs/Module>
#include <brolly/assert.h>

#include <cstring>
#include <cstdlib>

#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>


namespace {
using namespace LVFS;

class Iterator : public Directory::const_iterator
{
public:
    Iterator() :
        Directory::const_iterator(new (std::nothrow) Imp())
    {}

    Iterator(const char *path) :
        Directory::const_iterator(new (std::nothrow) Imp(path))
    {}

protected:
    union Buffer
    {
        struct dirent d;
        char b[offsetof(struct dirent, d_name) + NAME_MAX + 1];
    };

    class Imp : public Directory::const_iterator::Implementation
    {
        PLATFORM_MAKE_NONCOPYABLE(Imp)
        PLATFORM_MAKE_NONMOVEABLE(Imp)

    public:
        Imp() :
            m_dir(NULL),
            m_entry(NULL)
        {
            ::memset(&m_path, 0, sizeof(m_path));
        }

        Imp(const char *path) :
            m_dir(NULL),
            m_entry(NULL)
        {
            ::memset(&m_buffer, 0, sizeof(m_buffer));
            ::strncpy(m_path, path, sizeof(m_path));

            if (m_dir = ::opendir(m_path))
            {
                if (strcmp(m_path, "/") == 0)
                    m_path[0] = 0;

                next();
            }
        }

        virtual ~Imp()
        {
            ::closedir(m_dir);
        }

        virtual bool isEqual(const Holder &other) const
        {
            return m_entry == other.as<Imp>()->m_entry;
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
            Module::Error error;
            Interface::Holder file;

            m_file.reset();

            while (::readdir_r(m_dir, &m_buffer.d, &m_entry) == 0 && m_entry)
                if (::strcmp(m_entry->d_name, ".") == 0 || ::strcmp(m_entry->d_name, "..") == 0)
                    continue;
                else
                {
                    char path[PATH_MAX];

                    if (UNLIKELY(std::snprintf(path, sizeof(path), "%s/%s", m_path, m_entry->d_name) < 0))
                    {
                        m_entry = NULL;
                        return;
                    }

                    file = Entry::open(path, error);

                    if (file.isValid())
                        m_file = Module::open(file);

                    if (!m_file.isValid())
                        m_file = file;

                    break;
                }

            if (!m_file.isValid())
                m_entry = NULL;
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

Directory::Directory(const char *fileName, const struct stat &st) :
    m_filePath(::strdup(fileName)),
    m_fileName(::strrchr(m_filePath, '/') + 1),
    m_type(Module::desktop().typeOfDirectory()),
    m_cTime(st.st_ctime),
    m_mTime(st.st_mtime),
    m_aTime(st.st_atime),
    m_permissions(File::translatePermissions(st))
{}

Directory::~Directory()
{
    ::free(m_filePath);
}

const char *Directory::title() const
{
    return m_fileName;
}

const char *Directory::schema() const
{
    return "file";
}

const char *Directory::location() const
{
    return m_filePath;
}

const IType *Directory::type() const
{
    return m_type;
}

Interface::Holder Directory::open(IFile::Mode mode) const
{
    return Interface::Holder();
}

Directory::const_iterator Directory::begin() const
{
    return Iterator(m_filePath);
}

Directory::const_iterator Directory::end() const
{
    return Iterator();
}

bool Directory::exists(const char *name) const
{
    return false;
}

Interface::Holder Directory::entry(const char *name, const IType *type, bool create)
{
    ASSERT(name != NULL);
    ASSERT(type != NULL);
    return Interface::Holder();
}

Interface::Holder Directory::copy(const Progress &callback, const Interface::Holder &file, bool move)
{
    ASSERT(file.isValid());

    if (::strcmp(file->as<IEntry>()->type()->name(), Module::DirectoryTypeName) == 0)
        return Interface::Holder();

    char path[PATH_MAX];

    if (UNLIKELY(std::snprintf(path, sizeof(path), "%s/%s", m_filePath, file->as<IEntry>()->title()) < 0))
        return Interface::Holder();

    if (move)
    {
        if (::link(file->as<IEntry>()->location(), path) == 0 && ::unlink(file->as<IEntry>()->location()) == 0)
            return Entry::open(path, m_lastError);
        else
            if (errno == EEXIST &&
                ::unlink(path) == 0 &&
                ::link(file->as<IEntry>()->location(), path) == 0 &&
                ::unlink(file->as<IEntry>()->location()) == 0)
                return Entry::open(path, m_lastError);

        m_lastError = errno;
    }

    enum { BufferSize = 65536 };
    char *buffer = new (std::nothrow) char [BufferSize];

    if (UNLIKELY(buffer == NULL))
        return Interface::Holder();

    int dest_file = ::creat(path, S_IRUSR | S_IWUSR);

    if (dest_file == -1)
    {
        m_lastError = errno;
        delete [] buffer;
        return Interface::Holder();
    }

    Interface::Adaptor<IFile> src_file(file->as<IEntry>()->open());

    if (!src_file.isValid())
    {
        ::close(dest_file);
        ::unlink(path);
        m_lastError = EIO;
        delete [] buffer;
        return Interface::Holder();
    }

    off64_t read;

    while ((read = src_file->read(buffer, BufferSize)) && !callback.aborted)
        if (::write(dest_file, buffer, read) == read)
            callback.function(callback.arg, read);
        else
        {
            ::close(dest_file);
            ::unlink(path);
            m_lastError = EIO;
            delete [] buffer;
            return Interface::Holder();
        }

    ::close(dest_file);
    delete [] buffer;

    return Entry::open(path, m_lastError);
}

bool Directory::rename(const Interface::Holder &file, const char *name)
{
    return false;
}

bool Directory::remove(const Interface::Holder &file)
{
    ASSERT(file.isValid());
    char path[PATH_MAX];

#if PLATFORM_OS(WINDOWS)
    DWORD attr = GetFileAttributesW((const wchar_t*)filePath.utf16());
    if (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_READONLY) == FILE_ATTRIBUTE_READONLY)
        SetFileAttributesW((const wchar_t*)filePath.utf16(), attr &= ~FILE_ATTRIBUTE_READONLY);
#endif

    if (UNLIKELY(std::snprintf(path, sizeof(path), "%s/%s", m_filePath, file->as<IEntry>()->title()) < 0))
        return false;

    struct stat st;

    if (::stat(path, &st) == 0)
        if (S_ISDIR(st.st_mode))
        {
            if (::rmdir(path) == 0)
                return true;
        }
        else
            if (::unlink(path) == 0)
                return true;

    m_lastError = errno;
    return false;
}

const Error &Directory::lastError() const
{
    return m_lastError;
}

off64_t Directory::size() const
{
    return 0;
}

time_t Directory::cTime() const
{
    return m_cTime;
}

time_t Directory::mTime() const
{
    return m_mTime;
}

time_t Directory::aTime() const
{
    return m_aTime;
}

int Directory::permissions() const
{
    return m_permissions;
}

}
