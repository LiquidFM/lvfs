/**
 * This file is part of lvfs.
 *
 * Copyright (C) 2011-2016 Dmitriy Vilkov, <dav.daemon@gmail.com>
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

#ifndef LVFS_IPACKAGE_H_
#define LVFS_IPACKAGE_H_

#include <lvfs/Interface>


namespace LVFS {
namespace Settings {
    class Scope;
}

class PLATFORM_MAKE_PUBLIC IPackage
{
    DECLARE_INTERFACE(LVFS::IPackage)

public:
    struct Plugin
    {
        const char *type;
        const Interface &plugin;
    };

public:
    virtual ~IPackage();

    virtual const char *name() const = 0;
    virtual Settings::Scope *settings() const = 0;
    virtual const Plugin **contentPlugins() const = 0;
    virtual const Plugin **protocolPlugins() const = 0;
};

}

#endif /* LVFS_IPACKAGE_H_ */
