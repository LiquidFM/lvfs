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

#include "lvfs_Singleton.h"
#include "plugins/default/lvfs_default_ProtocolPlugin.h"

#include <lvfs/IEntry>
#include <brolly/assert.h>
#include <cstring>
#include <cstdio>

#include <dlfcn.h>
#include <linux/limits.h>


namespace LVFS {
namespace {
    static Singleton *s_instance;
    static LVFS::ProtocolPlugin defaultPlugin;
}
WARN_UNUSED_RETURN_OFF


Singleton::Error::Error()
{}

Singleton::Error::Error(int code) :
    ::LVFS::Error(code)
{}

Singleton::Error::~Error()
{}


Singleton::Singleton()
{
    ASSERT(s_instance == NULL);
    s_instance = this;

    processPlugin("/media/WORKSPACE/github.com/qfm/workspace/root/build/lvfs-arc/liblvfs-arc.so");
}

Singleton::~Singleton()
{
    ASSERT(s_instance == this);
    s_instance = NULL;

    m_contentPlugins.clear();
    m_protocolPlugins.clear();

    for (auto i : m_plugins)
        dlclose(i.handle);
}

const Desktop &Singleton::desktop()
{
    ASSERT(s_instance != NULL);
    return s_instance->m_desktop;
}

Interface::Holder Singleton::open(const char *uri, Error &error)
{
    ASSERT(s_instance != NULL);
    return s_instance->internalOpen(uri, error);
}

Interface::Holder Singleton::internalOpen(const char *uri, Error &error)
{
    enum { SchemaLength = 128 };
    static const char schema_delim[] = "://";
    char buffer[SchemaLength];
    Interface::Holder res;

    if (const char *delim = strstr(uri, schema_delim))
        if (delim - uri < SchemaLength)
        {
            memcpy(buffer, uri, delim - uri);
            buffer[delim - uri] = 0;
            uri = delim + sizeof(schema_delim) - 1;

            auto root = m_protocolPlugins.find(buffer);

            if (root != m_protocolPlugins.end())
                for (auto i : (*root).second)
                {
                    res = i->open(uri);

                    if (res.isValid())
                        break;
                }
        }
        else
            return Interface::Holder();

    if (!res.isValid())
        res = defaultPlugin.open(uri);

    if (res.isValid())
    {
        Interface::Holder res2;
        Interface::Adaptor<IEntry> entry(res);
        ASSERT(entry.isValid());

        auto plugin = m_contentPlugins.find(entry->type()->name());

        if (plugin != m_contentPlugins.end())
            for (auto i : (*plugin).second)
            {
                res2 = i->open(res);

                if (res2.isValid())
                    return res2;
            }
    }

    return res;
}

void Singleton::processPlugin(const char *fileName)
{
    Plugin plugin = { dlopen(fileName, RTLD_LAZY | RTLD_LOCAL), NULL };

    if (plugin.handle)
    {
        /* Clear any existing error */
        dlerror();

        Package::PluginFunction func;

        if (*(void **) (&func) = dlsym(plugin.handle, "lvfs_plugin_package"))
        {
            plugin.package = func();

            if (LIKELY(plugin.package != NULL))
            {
                if (const Package::Plugin **p = plugin.package->contentPlugins())
                    for (const Package::Plugin *pl = *p; pl != NULL; pl = *++p)
                        m_contentPlugins[pl->type].push_back(pl->plugin.as<IContentPlugin>());

                if (const Package::Plugin **p = plugin.package->protocolPlugins())
                    for (const Package::Plugin *pl = *p; pl != NULL; pl = *++p)
                        m_protocolPlugins[pl->type].push_back(pl->plugin.as<IProtocolPlugin>());

                m_plugins.push_back(plugin);
            }
            else
            {
                dlclose(plugin.handle);
            }
        }
        else
        {
            fprintf(stderr, "%s\n", dlerror());
        }
    }
    else
    {
        fprintf(stderr, "%s\n", dlerror());
    }
}


bool Singleton::String::operator<(const String &other) const
{
    return strcmp(m_string, other.m_string) < 0;
}

bool Singleton::String::operator==(const String &other) const
{
    return strcmp(m_string, other.m_string) == 0;
}


WARN_UNUSED_RETURN_ON
}
