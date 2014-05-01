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

#ifndef LVFS_IAPPLICATION_H_
#define LVFS_IAPPLICATION_H_

#include <lvfs/Interface>


namespace LVFS {

class PLATFORM_MAKE_PUBLIC IApplication
{
    DECLARE_INTERFACE(LVFS::IApplication)

public:
    virtual ~IApplication();

    virtual const char *name() const = 0;
    virtual const char *description() const = 0;
    virtual const Interface::Holder &icon() const = 0;

    virtual bool exec() const = 0;
};

}

#endif /* LVFS_IAPPLICATION_H_ */
