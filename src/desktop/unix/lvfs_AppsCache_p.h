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

#include <lvfs/IApplication>
#include <lvfs/IApplications>
#include <lvfs/Module>

#include <efc/List>
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

    virtual bool exec() const
    {
        return false;
    }

//    virtual bool exec(const IFileContainer *container, const IFileInfo *file, char *error) const
//    {
//        typedef QList<QByteArray> List;
//        QByteArray workingDirectory = container->location();
//        QByteArray fileName(file->fileName());
//
//        List arguments = QByteArray(m_exec).
//                replace("%d", QByteArray()).
//                replace("%D", QByteArray()).
//                replace("%n", QByteArray()).
//                replace("%N", QByteArray()).
//                replace("%k", QByteArray()).
//                replace("%v", QByteArray()).
//                replace("%m", QByteArray()).
//                trimmed().
//                split(' ');
//
//        for (List::size_type i = 0, size = arguments.size(); i < size;)
//            if (arguments.at(i).indexOf('=') != -1)
//            {
//                arguments.removeAt(i);
//                --size;
//            }
//            else
//            {
//                arguments[i] = arguments.at(i).trimmed();
//
//                if (arguments.at(i).indexOf("%i") != -1)
//                {
//                    arguments[i].replace("%i", m_iconName);
//                    arguments.insert(i, QByteArray("--icon"));
//                    ++i;
//                    ++size;
//                }
//                else
//                    arguments[i].
//                        replace("%f", fileName).
//                        replace("%F", fileName).
//                        replace("%u", fileName).
//                        replace("%U", fileName).
//                        replace("%c", ::Desktop::Locale::current()->codec()->fromUnicode(m_name));
//
//                ++i;
//            }
//
//        if (pid_t pid = fork())
//        {
//            if (pid < 0)
//            {
//                error = Info::codec()->toUnicode(::strerror(errno));
//                return false;
//            }
//        }
//        else
//        {
//            QVarLengthArray<char *, 8> argv(arguments.size() + 1);
//
//            for (List::size_type i = 0, size = arguments.size(); i < size; ++i)
//                argv.data()[i] = arguments[i].data();
//
//            argv.data()[arguments.size()] = NULL;
//
//            setsid();
//            chdir(workingDirectory);
//            execvp(argv.data()[0], argv.data());
//            exit(EXIT_FAILURE);
//        }
//
//        return true;
//    }

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
