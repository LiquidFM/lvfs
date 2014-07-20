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

#include "lvfs_default_File.h"


namespace LVFS {

File::File(const char *fileName) :
    m_file(fileName)
{}

File::File(const char *fileName, const struct stat &st) :
    m_file(fileName, st)
{}

File::~File()
{}

const char *File::title() const
{
    return m_file.title();
}

const char *File::schema() const
{
    return m_file.schema();
}

const char *File::location() const
{
    return m_file.location();
}

const IType *File::type() const
{
    return m_file.type();
}

bool File::open()
{
    return m_file.open();
}

size_t File::read(void *buffer, size_t size)
{
    return m_file.read(buffer, size);
}

size_t File::write(const void *buffer, size_t size)
{
    return m_file.write(buffer, size);
}

bool File::seek(long offset, Whence whence)
{
    return m_file.seek(offset, whence);
}

bool File::flush()
{
    return m_file.flush();
}

void File::close()
{
    m_file.close();
}

uint64_t File::size() const
{
    return m_file.size();
}

size_t File::position() const
{
    return m_file.position();
}

const Error &File::lastError() const
{
    return m_file.lastError();
}

time_t File::cTime() const
{
    return m_file.cTime();
}

time_t File::mTime() const
{
    return m_file.mTime();
}

time_t File::aTime() const
{
    return m_file.aTime();
}

int File::permissions() const
{
    return m_file.permissions();
}

bool File::setPermissions(int value)
{
    return m_file.setPermissions(value);
}

}
