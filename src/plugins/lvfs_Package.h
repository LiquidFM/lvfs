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

#ifndef LVFS_PACKAGE_H_
#define LVFS_PACKAGE_H_

#include <lvfs/plugins/IDataPlugin>
#include <lvfs/plugins/IRootPlugin>


namespace LVFS {

class Package
{
    PLATFORM_MAKE_NONCOPYABLE(Package)
    PLATFORM_MAKE_NONMOVEABLE(Package)
    PLATFORM_MAKE_STACK_ONLY

public:
    struct DataPlugin
    {
        const char *type;
        const IDataPlugin *plugin;
    };

    struct RootPlugin
    {
        const char *schema;
        const IRootPlugin *plugin;
    };

public:
    Package();
    virtual ~Package();

    virtual const char *name() const = 0;
    virtual const DataPlugin *dataPlugins() const = 0;
    virtual const RootPlugin *rootPlugins() const = 0;
};

}

#endif /* LVFS_PACKAGE_H_ */
