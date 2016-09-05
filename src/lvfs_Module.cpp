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

#include "lvfs_Module.h"

#include <lvfs/IEntry>
#include <lvfs/plugins/IPackage>
#include <lvfs/plugins/IContentPlugin>
#include <lvfs/plugins/IProtocolPlugin>
#include <brolly/assert.h>

#include <cstring>
#include <cstdio>

#include <dlfcn.h>
#include <linux/limits.h>


WARN_UNUSED_RETURN_OFF

namespace LVFS {
namespace {
    static Module *s_instance;
}

const char Module::SchemaDelimiter[] = "://";
const char Module::DirectoryTypeName[] = "inode/directory";


Module::Module(Settings::Instance &settings) :
    m_settings(settings)
{
    ASSERT((MaxUriLength - MaxSchemaLength - SchemaDelimiterLength) <= PATH_MAX);
    ASSERT(s_instance == NULL);
    s_instance = this;

    processPlugin("/media/WORKSPACE/github.com/qfm/workspace/root/build/lvfs-core/liblvfs-core.so");
    processPlugin("/media/WORKSPACE/github.com/qfm/workspace/root/build/lvfs-db/liblvfs-db.so");
    processPlugin("/media/WORKSPACE/github.com/qfm/workspace/root/build/lvfs-arc/liblvfs-arc.so");
    processPlugin("/media/WORKSPACE/github.com/qfm/workspace/root/build/lvfs-qimg/liblvfs-qimg.so");
    processPlugin("/media/WORKSPACE/github.com/qfm/workspace/root/build/lvfs-bits/liblvfs-bits.so");
    processPlugin("/media/WORKSPACE/github.com/qfm/workspace/root/build/lvfs-qplayer/liblvfs-qplayer.so");
}

Module::~Module()
{
    ASSERT(s_instance == this);
    s_instance = NULL;

    m_contentPlugins.clear();
    m_protocolPlugins.clear();

    for (auto i : m_plugins)
        dlclose(i.handle);
}

const Desktop &Module::desktop()
{
    ASSERT(s_instance != NULL);
    return s_instance->m_desktop;
}

Interface::Holder Module::open(const char *uri, Error &error)
{
    ASSERT(s_instance != NULL);
    return s_instance->internalOpen(uri, error);
}

Interface::Holder Module::open(const Interface::Holder &file)
{
    ASSERT(s_instance != NULL);
    return s_instance->internalOpen(file);
}

Interface::Holder Module::internalOpen(const char *uri, Error &error)
{
    char buffer[MaxSchemaLength] = { 'f', 'i', 'l', 'e' };
    Interface::Holder res;

    if (const char *delim = strstr(uri, SchemaDelimiter))
        if (delim - uri < MaxSchemaLength)
        {
            memcpy(buffer, uri, delim - uri);
            buffer[delim - uri] = 0;
            uri = delim + SchemaDelimiterLength;
        }
        else
            return res;

    auto root = m_protocolPlugins.find(buffer);

    if (root == m_protocolPlugins.end())
        return res;
    else
        for (auto i : (*root).second)
        {
            res = i->open(uri);

            if (res.isValid())
                break;
        }

    if (res.isValid())
        res = internalOpen(res);

    return res;
}

Interface::Holder Module::internalOpen(const Interface::Holder &file)
{
    Interface::Holder res;
    Interface::Adaptor<IEntry> entry(file);
    ASSERT(entry.isValid());

    auto plugin = m_contentPlugins.find(entry->type()->name());

    if (plugin != m_contentPlugins.end())
        for (auto i : (*plugin).second)
        {
            res = i->open(file);

            if (res.isValid())
                return res;
        }

    return file;
}

void Module::processPlugin(const char *fileName)
{
    Plugin plugin = { dlopen(fileName, RTLD_LAZY | RTLD_LOCAL), NULL };

    if (plugin.handle)
    {
        /* Clear any existing error */
        dlerror();

        if (plugin.package = reinterpret_cast<PackageFunction>(dlsym(plugin.handle, "lvfs_package")))
        {
            const IPackage *package = plugin.package().as<IPackage>();

            if (UNLIKELY(package == NULL))
                fprintf(stderr, "Package \"%s\" does not implement IPackage interface\n", fileName);
            else
            {
                if (const IPackage::Plugin **p = package->contentPlugins())
                    for (const IPackage::Plugin *pl = *p; pl != NULL; pl = *++p)
                        m_contentPlugins[pl->type].push_back(pl->plugin.as<IContentPlugin>());

                if (const IPackage::Plugin **p = package->protocolPlugins())
                    for (const IPackage::Plugin *pl = *p; pl != NULL; pl = *++p)
                        m_protocolPlugins[pl->type].push_back(pl->plugin.as<IProtocolPlugin>());

                m_plugins.push_back(plugin);
            }
        }
        else
            fprintf(stderr, "%s\n", dlerror());
    }
    else
        fprintf(stderr, "%s\n", dlerror());
}


bool Module::String::operator<(const String &other) const
{
    return strcmp(m_string, other.m_string) < 0;
}

bool Module::String::operator==(const String &other) const
{
    return strcmp(m_string, other.m_string) == 0;
}

}

WARN_UNUSED_RETURN_ON
