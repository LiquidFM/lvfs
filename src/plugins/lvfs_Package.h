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

#include <platform/utils.h>
#include <lvfs/plugins/IContentPlugin>
#include <lvfs/plugins/IProtocolPlugin>


#define DECLARE_PLUGIN(CLASS)                    \
    extern "C" PLATFORM_MAKE_PUBLIC              \
    const ::LVFS::Package *lvfs_plugin_package() \
    {                                            \
        static const CLASS package;              \
        return &package;                         \
    }


namespace LVFS {
namespace Settings {
    class Scope;
}

class PLATFORM_MAKE_PUBLIC Package
{
    PLATFORM_MAKE_NONCOPYABLE(Package)
    PLATFORM_MAKE_NONMOVEABLE(Package)
    PLATFORM_MAKE_STACK_ONLY

public:
    typedef const Package *(*PluginFunction)();

    struct Plugin
    {
        const char *type;
        const Interface &plugin;
    };

public:
    Package();
    virtual ~Package();

    virtual const char *name() const = 0;
    virtual Settings::Scope *settings() const = 0;
    virtual const Plugin **contentPlugins() const = 0;
    virtual const Plugin **protocolPlugins() const = 0;
};

}

#endif /* LVFS_PACKAGE_H_ */
