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

#include "lvfs_settings_StringOption.h"
#include "lvfs_settings_Visitor.h"

#include <cstring>
#include <cstdlib>


namespace LVFS {
namespace Settings {

StringOption::StringOption(const char *id, const char *defaultValue, Option *parent) :
    Option(id, parent),
    m_value(::strdup(defaultValue)),
    m_defaultValue(defaultValue)
{}

StringOption::~StringOption()
{
    ::free(m_value);
}

void StringOption::setValue(const char *value)
{
    ::free(m_value);
    m_value = ::strdup(value);
}

void StringOption::accept(Visitor &visitor)
{
    visitor.visit(*this);
}

}}
