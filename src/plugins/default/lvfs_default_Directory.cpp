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


namespace LVFS {

Directory::Directory(const char *fileName) :
    m_file(fileName)
{}

Directory::Directory(const char *fileName, const struct stat &st) :
    m_file(fileName, st)
{}

Directory::~Directory()
{}

time_t Directory::cTime() const
{
    return m_file.cTime();
}

time_t Directory::mTime() const
{
    return m_file.mTime();
}

time_t Directory::aTime() const
{
    return m_file.aTime();
}

int Directory::permissions() const
{
    return m_file.permissions();
}

bool Directory::setPermissions(int value)
{
    return m_file.setPermissions(value);
}

Directory::const_iterator Directory::begin() const
{
    return m_file.begin();
}

Directory::const_iterator Directory::end() const
{
    return m_file.end();
}

Interface::Holder Directory::entry(const char *name) const
{
    return Interface::Holder();
}

bool Directory::rename(const Interface::Holder &file, const char *name)
{
    return m_file.rename(file, name);
}

bool Directory::remove(const Interface::Holder &file)
{
    return m_file.remove(file);
}

const char *Directory::title() const
{
    return m_file.title();
}

const char *Directory::schema() const
{
    return m_file.schema();
}

const char *Directory::location() const
{
    return m_file.location();
}

const IType *Directory::type() const
{
    return m_file.type();
}

const Error &Directory::lastError() const
{
    return m_file.lastError();
}

}
