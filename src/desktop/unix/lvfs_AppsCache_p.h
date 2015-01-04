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

#include <lvfs/IApplication>
#include <lvfs/IApplications>
#include <lvfs/Module>

#include <efc/List>
#include <efc/Vector>
#include <xdg/xdg.h>

#include <cstring>
#include <cstdlib>

#include <paths.h>
#include <sys/statvfs.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>


namespace LVFS {
namespace {

class App : public Implements<IApplication>
{
public:
    App(const char *name, const char *description, const char *exec, const Interface::Holder &icon) :
        m_name(name ? ::strdup(name) : NULL),
        m_description(description ? ::strdup(description) : NULL),
        m_exec(exec ? ::strdup(exec) : NULL),
        m_icon(icon)
    {}

    virtual ~App()
    {
        if (m_name)
            ::free(m_name);

        if (m_description)
            ::free(m_description);

        if (m_exec)
            ::free(m_exec);
    }

    virtual const char *name() const { return m_name; }
    virtual const char *description() const { return m_description; }
    virtual const Interface::Holder &icon() const { return m_icon; }

    virtual bool open(const IEntry *entry) const
    {
        char buffer[PATH_MAX];

        if (::strlen(m_exec) > sizeof(buffer) - 1)
            return false;

        ::strcpy(buffer, m_exec);

        for (char *pos = buffer, *pos2 = NULL;
             (pos2 = strstr(pos, "%d")) ||
             (pos2 = strstr(pos, "%D")) ||
             (pos2 = strstr(pos, "%n")) ||
             (pos2 = strstr(pos, "%N")) ||
             (pos2 = strstr(pos, "%k")) ||
             (pos2 = strstr(pos, "%v")) ||
             (pos2 = strstr(pos, "%m"));
             pos = pos2)
        {
            ::memmove(pos2, pos2 + 2, ::strlen(pos2 + 2) + 1);
        }

        char *pos;
        EFC::List<char *> args;

        for (char *pos2 = pos = buffer; pos2 = ::strchr(pos, ' '); pos = pos2 + 1)
        {
            *pos2 = 0;

            if ((pos2 - pos) > 1 && ::strchr(pos, '=') == NULL && ::strstr(pos, "env") == NULL)
                args.push_back(pos);
        }

        args.push_back(pos);

        EFC::Vector<char *> args2;
        args2.reserve(args.size() + 1);

        for (auto i : args)
            if (::strstr(i, "%i"))
            {
                static const char icon[] = "--icon";
                size_t len = sizeof(icon) + 1 + ::strlen(m_icon->as<IEntry>()->location());

                pos = static_cast<char *>(::malloc(len));

                if (UNLIKELY(pos == NULL))
                {
                    for (auto i : args2)
                        ::free(i);

                    return false;
                }

                ::snprintf(pos, len, "%s %s", icon, m_icon->as<IEntry>()->location());

                args2.push_back(pos);
            }
            else if (::strstr(i, "%f") ||
                     ::strstr(i, "%F") ||
                     ::strstr(i, "%u") ||
                     ::strstr(i, "%U"))
            {
                size_t len = ::strlen(entry->location()) + 1;

                pos = static_cast<char *>(::malloc(len));

                if (UNLIKELY(pos == NULL))
                {
                    for (auto i : args2)
                        ::free(i);

                    return false;
                }

                ::strcpy(pos, entry->location());

                args2.push_back(pos);
            }
            else if (::strstr(i, "%c"))
            {
                size_t len = ::strlen(m_name) + 1;

                pos = static_cast<char *>(::malloc(len));

                if (UNLIKELY(pos == NULL))
                {
                    for (auto i : args2)
                        ::free(i);

                    return false;
                }

                ::strcpy(pos, m_name);

                args2.push_back(pos);
            }
            else
                args2.push_back(::strdup(i));

        if (pid_t pid = ::fork())
        {
            for (auto i : args2)
                ::free(i);

            if (pid < 0)
            {
//                error = Info::codec()->toUnicode(::strerror(errno));
                return false;
            }
        }
        else
        {
            args2.push_back(NULL);

            ::strcpy(buffer, entry->location());
            *::strrchr(buffer, '/') = 0;

            ::setsid();
            ::chdir(buffer);
            ::execvp(args2[0], args2.data());
            ::exit(EXIT_FAILURE);
        }

        return true;
    }

private:
    char *m_name;
    char *m_description;
    char *m_exec;
    Interface::Holder m_icon;
};


class Apps : public Implements<IApplications>
{
public:
    typedef ::EFC::List<Interface::Holder> Container;

public:
    Apps()
    {}

    virtual ~Apps()
    {}

    virtual const_iterator begin() const;
    virtual const_iterator end() const;

    bool push_back(Interface::Holder &&app)
    {
        return m_list.push_back(std::move(app));
    }

private:
    Container m_list;
};


class Iterator : public Apps::const_iterator
{
public:
    Iterator(Apps::Container::const_iterator &&i) :
        Apps::const_iterator(new (std::nothrow) Imp(std::move(i)))
    {}

protected:
    class Imp : public Apps::const_iterator::Implementation
    {
        PLATFORM_MAKE_NONCOPYABLE(Imp)
        PLATFORM_MAKE_NONMOVEABLE(Imp)

    public:
        Imp(Apps::Container::const_iterator &&i) :
            m_i(std::move(i))
        {}

        virtual ~Imp()
        {}

        virtual bool isEqual(const Holder &other) const
        {
            return m_i == other.as<Imp>()->m_i;
        }

        virtual reference asReference() const
        {
            return (*m_i);
        }

        virtual pointer asPointer() const
        {
            return &(*m_i);
        }

        virtual void next()
        {
            ++m_i;
        }

    private:
        Apps::Container::const_iterator m_i;
    };
};


Apps::const_iterator Apps::begin() const
{
    return Iterator(std::move(m_list.begin()));
}

Apps::const_iterator Apps::end() const
{
    return Iterator(std::move(m_list.end()));
}


class AppsCache : public Cache
{
public:
    AppsCache()
    {}

    ~AppsCache()
    {}

    Interface::Holder findApplications(const IType *type)
    {
        Index index(type->name(), XdgThemeApplications);
        Interface::Holder apps = lockedRead(index);

        if (!apps.isValid())
        {
            WriteLocker lock(m_cacheLock);
            apps = read(index);

            if (!apps.isValid())
            {
                Apps *list;
                apps.reset(list = new (std::nothrow) Apps());

                if (LIKELY(apps != NULL))
                {
                    const XdgJointListItem *tmp_apps;

                    if (tmp_apps = ::xdg_apps_lookup(type->name()))
                        fill(list, tmp_apps);

                    if (tmp_apps = ::xdg_known_apps_lookup(type->name()))
                        fill(list, tmp_apps);

                    write(std::move(index), apps);
                }
            }
        }

        return apps;
    }

private:
    void fill(Apps *list, const XdgJointListItem *apps)
    {
        const char *name = NULL;
        const char *exec = NULL;
        const char *gen_name = NULL;
        const char *comment = NULL;
        const char *icon_name = NULL;
        Interface::Holder icon;

        const XdgListItem *values;
        const XdgAppGroup *group;
        const Desktop::Locale &locale = Module::desktop().locale();

        do
            if (group = ::xdg_app_group_lookup(::xdg_joint_list_item_app(apps), "Desktop Entry"))
            {
                if (values = ::xdg_app_localized_entry_lookup(group, "Name", locale.lang(), locale.country(), locale.modifier()))
                    name = ::xdg_list_item_app_group_entry_value(values);
                else
                    name = NULL;

                if (values = ::xdg_app_localized_entry_lookup(group, "GenericName", locale.lang(), locale.country(), locale.modifier()))
                    gen_name = ::xdg_list_item_app_group_entry_value(values);
                else
                    gen_name = NULL;

                if (values = ::xdg_app_localized_entry_lookup(group, "Comment", locale.lang(), locale.country(), locale.modifier()))
                    comment = ::xdg_list_item_app_group_entry_value(values);
                else
                    comment = NULL;

                if (values = ::xdg_app_localized_entry_lookup(group, "Exec", locale.lang(), locale.country(), locale.modifier()))
                    exec = ::xdg_list_item_app_group_entry_value(values);
                else
                    exec = NULL;

                if (values = ::xdg_app_localized_entry_lookup(group, "Icon", locale.lang(), locale.country(), locale.modifier()))
                    icon_name = ::xdg_list_item_app_group_entry_value(values);
                else
                    icon_name = NULL;

                icon = iconCache.findIcon(icon_name, Desktop::SmallIcon, XdgThemeApplications, theme);

                if (!icon.isValid())
                    icon = iconCache.findMimeIcon(XDG_MIME_TYPE_UNKNOWN, Desktop::SmallIcon, theme);

                list->push_back(Interface::Holder(new (std::nothrow) App(name, comment ? comment : gen_name, exec, icon)));
            }
        while (apps = ::xdg_joint_list_next(apps));
    }
};

}}
