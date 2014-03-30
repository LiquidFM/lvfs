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

#ifndef LVFS_SINGLETON_H_
#define LVFS_SINGLETON_H_

#include <platform/utils.h>

#include <efc/Map>
#include <efc/List>

#include <lvfs/Error>
#include <lvfs/Interface>
#include <lvfs/plugins/Package>


namespace LVFS {

class PLATFORM_MAKE_PUBLIC Singleton
{
    PLATFORM_MAKE_NONCOPYABLE(Singleton)
    PLATFORM_MAKE_NONMOVEABLE(Singleton)
    PLATFORM_MAKE_STACK_ONLY

public:
    class Error : public ::LVFS::Error
    {
    public:
        Error();
        Error(int code);
        virtual ~Error();
    };

public:
    Singleton();
    ~Singleton();

    static Interface::Holder open(const char *uri, Error &error);

private:
    Interface::Holder internalOpen(const char *uri, Error &error);
    void processPlugin(const char *fileName);

private:
    struct Plugin
    {
        void *handle;
        const Package *package;
    };

    class String
    {
    public:
        String() :
            m_string("")
        {}

        String(const char *string) :
            m_string(string)
        {}

        ~String()
        {}

        bool operator<(const String &other) const;
        bool operator==(const String &other) const;

    private:
        const char *m_string;
    };

private:
    ::EFC::List<Plugin> m_plugins;
    ::EFC::Map<String, ::EFC::List<const IDataPlugin *>> m_dataPlugins;
    ::EFC::Map<String, ::EFC::List<const IRootPlugin *>> m_rootPlugins;
};

}

#endif /* LVFS_SINGLETON_H_ */
