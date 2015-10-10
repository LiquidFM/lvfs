/**
 * This file is part of lvfs_.
 *
 * Copyright (C) 2011-2015 Dmitriy Vilkov, <dav.daemon@gmail.com>
 *
 * lvfs_ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * lvfs_ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with lvfs_. If not, see <http://www.gnu.org/licenses/>.
 */

#include "lvfs_settings_IntOption.h"
#include "lvfs_settings_Visitor.h"


namespace LVFS {
namespace Settings {

IntOption::~IntOption()
{}

void IntOption::accept(Visitor &visitor)
{
    visitor.visit(*this);
}

}}
