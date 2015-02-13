/**
 * This file is part of lvfs.
 *
 * Copyright (C) 2011-2015 Dmitriy Vilkov, <dav.daemon@gmail.com>
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

#define _FILE_OFFSET_BITS 64

#include "lvfs_default_File.h"

#include <brolly/assert.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>


namespace LVFS {

static uid_t userId = getuid();
static gid_t groupId = getgid();


File::File(const char *fileName, Mode mode) :
    m_file(::open(fileName, 0)),
    m_size(0),
    m_cTime(0),
    m_mTime(0),
    m_aTime(0),
    m_permissions(0)
{
    if (isValid())
    {
        struct stat st;

        if (LIKELY(::fstat(m_file, &st) != -1))
        {
            m_size = st.st_size;
            m_cTime = st.st_ctime;
            m_mTime = st.st_mtime;
            m_aTime = st.st_atime;
            m_permissions = translatePermissions(st);
        }
    }
}

File::~File()
{
    if (isValid())
        ::close(m_file);
}

bool File::isValid() const
{
    return m_file != -1;
}

size_t File::read(void *buffer, size_t size)
{
    ASSERT(isValid());
    return ::read(m_file, buffer, size);
}

size_t File::write(const void *buffer, size_t size)
{
    ASSERT(isValid());
    return ::write(m_file, buffer, size);
}

bool File::advise(off64_t offset, off64_t len, Advise advise)
{
    ASSERT(isValid());
    static const int posix_advice[] = { POSIX_FADV_NORMAL,
                                        POSIX_FADV_RANDOM,
                                        POSIX_FADV_SEQUENTIAL,
                                        POSIX_FADV_WILLNEED,
                                        POSIX_FADV_NOREUSE,
                                        POSIX_FADV_DONTNEED };

    return ::posix_fadvise64(m_file, offset, len, posix_advice[advise]) == 0;
}

bool File::seek(off64_t offset, Whence whence)
{
    ASSERT(isValid());
    static const int system[FromEnd + 1] = { SEEK_SET, SEEK_CUR, SEEK_END };

    return ::lseek64(m_file, offset, system[whence]) != (off64_t)-1;
}

bool File::flush()
{
    ASSERT(isValid());
#if _GNU_SOURCE
    ::syncfs(m_file);
#endif
    return true;
}

const Error &File::lastError() const
{
    return m_lastError;
}

off64_t File::size() const
{
    ASSERT(isValid());
    return m_size;
}

time_t File::cTime() const
{
    ASSERT(isValid());
    return m_cTime;
}

time_t File::mTime() const
{
    ASSERT(isValid());
    return m_mTime;
}

time_t File::aTime() const
{
    ASSERT(isValid());
    return m_aTime;
}

int File::permissions() const
{
    ASSERT(isValid());
    return m_permissions;
}

int File::translatePermissions(const struct stat &st)
{
    int res = 0;

    if ((st.st_mode & S_IROTH) ||
        (st.st_uid == userId && (st.st_mode & S_IRUSR)) ||
        (st.st_gid == groupId && (st.st_mode & S_IRGRP)))
        res |= IProperties::Read;

    if ((st.st_mode & S_IWOTH) ||
        (st.st_uid == userId  && (st.st_mode & S_IWUSR)) ||
        (st.st_gid == groupId && (st.st_mode & S_IWGRP)))
        res |= IProperties::Write;

    if ((st.st_mode & S_IXOTH) ||
        (st.st_uid == userId  && (st.st_mode & S_IXUSR)) ||
        (st.st_gid == groupId && (st.st_mode & S_IXGRP)))
        res |= IProperties::Exec;

    return res;
}

}
