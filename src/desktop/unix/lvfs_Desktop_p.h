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

#include "lvfs_Cache_p.h"
#include "lvfs_IconCache_p.h"

namespace LVFS {
    static IconCache iconCache;
    static const char *theme;
}

#include "lvfs_AppsCache_p.h"

namespace LVFS { static AppsCache appsCache; }

#include "../lvfs_MimeType.h"

#include <brolly/assert.h>
#include <efc/StateMachine>

#include <platform/platform.h>
#define PLATFORM_DE(PLATFORM_FEATURE) PLATFORM_IS_SET(PLATFORM_DE_##PLATFORM_FEATURE)

#if PLATFORM_DE(KDE)
#   include "kde/desktop_kde_p.h"
#elif PLATFORM_DE(GNOME) || PLATFORM_DE(XFCE4)
#   include "gtk/desktop_gtk_p.h"
#else
#error Unknown desktop environment!
#endif

#include <sys/stat.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <X11/Xlib.h>
#include <X11/Xatom.h>


namespace LVFS {

namespace {
    class Parser
    {
    public:
        enum State
        {
            Lang,
            Country,
            Modifier,
            Encoding,
            Stoped
        };

    public:
        Parser(const char *string) :
            m_valid(false),
            m_lang(NULL),
            m_country(NULL),
            m_modifier(NULL),
            m_encoding(NULL)
        {
            typedef RULE(Lang,     '_', Country,  Parser, lang)        Rule1;
            typedef RULE(Country,  '@', Modifier, Parser, country)     Rule2;
            typedef RULE(Modifier, '.', Encoding, Parser, modifier)    Rule3;
            typedef RULE(Country,  '.', Encoding, Parser, country)     Rule4;
            typedef RULE(Encoding,   0, Stoped,   Parser, encoding)    Rule5;
            typedef LOKI_TYPELIST_5(Rule1, Rule2, Rule3, Rule4, Rule5) Rules;

            m_valid = ::EFC::StateMachine<Rules>(this).process(string) == Stoped;
        }

    private:
        void lang(const char *string, int size)
        {
            m_lang = string;
            const_cast<char *>(string)[size] = 0;
        }

        void country(const char *string, int size)
        {
            m_country = string;
            const_cast<char *>(string)[size] = 0;
        }

        void modifier(const char *string, int size)
        {
            m_modifier = string;
            const_cast<char *>(string)[size] = 0;
        }

        void encoding(const char *string, int size)
        {
            m_encoding = string;
            const_cast<char *>(string)[size] = 0;
        }

    private:
        friend class Desktop::Locale;
        bool m_valid;
        const char *m_lang;
        const char *m_country;
        const char *m_modifier;
        const char *m_encoding;
    };
}


enum Environment
{
    DE_Cde,
    DE_Kde,
    DE_4Dwm,
    DE_Gnome,
    DE_Win32,
    DE_Unknown
};

enum X11Atom
{
    DTWM_IS_RUNNING,
    _DT_SAVE_MODE,
    _SGI_DESKS_MANAGER,
    NAtoms
};

static const char *x11_atomnames[NAtoms] =
{
    "DTWM_IS_RUNNING",
    "_DT_SAVE_MODE",
    "_SGI_DESKS_MANAGER"
};


#if PLATFORM_DE(KDE)
    static int kde_version = 0;
#endif


static Desktop *desktop;
static Environment environment = DE_Unknown;


Desktop::Locale::Locale() :
    m_lang(NULL),
    m_country(NULL),
    m_encoding(NULL),
    m_modifier(NULL)
{
    m_buffer[MaxLenghtOfLocale - 1] = 0;

    if (const char *locale = ::getenv("LANG"))
        strncpy(m_buffer, locale, MaxLenghtOfLocale - 1);
    else
        strcpy(m_buffer, "en_GB.UTF-8");

    Parser parser(m_buffer);

    if (parser.m_valid)
    {
        m_lang = parser.m_lang;
        m_country = parser.m_country;
        m_encoding = parser.m_encoding;
        m_modifier = parser.m_modifier;
    }
    else
    {
        m_lang = "en";
        m_country = "GB";
        m_encoding = "UTF-8";
    }
}


Desktop::Desktop()
{
    ASSERT(desktop == NULL);

    if (Display *display = ::XOpenDisplay(NULL))
    {
        int rc;
        Atom type;
        int format;
        const char *var;
        unsigned long length, after;
        unsigned char *data = 0;
        Atom atoms[NAtoms];

#if defined(XlibSpecificationRelease) && (XlibSpecificationRelease >= 6)
        ::XInternAtoms(display, const_cast<char **>(x11_atomnames), NAtoms, False, atoms);
#else
        for (int i = 0; i = 0; i < NAtoms; ++i)
            atoms[i] = ::XInternAtom(display, const_cast<char *>(x11_atomnames[i]), False);
#endif

        do
        {
            if (var = ::getenv("KDE_FULL_SESSION"))
            {
                environment = DE_Kde;

#if PLATFORM_DE(KDE)
                if (var = ::getenv("KDE_SESSION_VERSION"))
                    kde_version = atoi(var);
#endif
                break;
            }

            if (var = ::getenv("DESKTOP_SESSION"))
                if (!::strcmp(var, "gnome"))
                {
                    environment = DE_Gnome;
                    break;
                }

            // GNOME_DESKTOP_SESSION_ID is deprecated, but still check it
            if (var = ::getenv("GNOME_DESKTOP_SESSION_ID"))
            {
                environment = DE_Gnome;
                break;
            }

            rc = ::XGetWindowProperty(display, RootWindow(display, DefaultScreen(display)),
                                    atoms[_DT_SAVE_MODE],
                                    0, 2, False, XA_STRING, &type, &format, &length,
                                    &after, &data);
            if (rc == Success && length)
            {
                if (!::strcmp(reinterpret_cast<const char *>(data), "xfce4"))
                {
                    environment = DE_Gnome;
                    break;
                }

                ::XFree(data);
                data = 0;
            }

            rc = ::XGetWindowProperty(display, RootWindow(display, DefaultScreen(display)),
                                    atoms[DTWM_IS_RUNNING],
                                    0, 1, False, AnyPropertyType, &type, &format, &length,
                                    &after, &data);
            if (rc == Success && length)
            {
                environment = DE_Cde;
                break;
            }

            rc = ::XGetWindowProperty(display, RootWindow(display, DefaultScreen(display)),
                                    atoms[_SGI_DESKS_MANAGER], 0, 1, False, XA_WINDOW,
                                    &type, &format, &length, &after, &data);
            if (rc == Success && length)
            {
                environment = DE_4Dwm;
                break;
            }
        }
        while (true);

        if (data)
            ::XFree(reinterpret_cast<char *>(data));

        ::XCloseDisplay(display);
    }

#if PLATFORM_DE(KDE)
    theme = DesktopPrivate::iconThemeName(kde_version);
#else
    theme = DesktopPrivate::iconThemeName();
#endif

    ::xdg_init();
    desktop = this;
}

Desktop::~Desktop()
{
    ASSERT(desktop != NULL);

    ::xdg_shutdown();
    desktop = NULL;
}

Interface::Holder Desktop::applications(const IType *type) const
{
    return appsCache.findApplications(type);
}

Interface::Holder Desktop::typeOfFile(IFile *file, const char *fileName, IconType iconType) const
{
    ASSERT(file != NULL && fileName != NULL);
    const char *mimeType = xdg_mime_get_mime_type_from_file_name(fileName);

    if (mimeType == XDG_MIME_TYPE_UNKNOWN && file->open())
    {
        int len = xdg_mime_get_max_buffer_extents();
        ::EFC::ScopedPointer<char> buffer(new (std::nothrow) char [len]);

        if (LIKELY(buffer != NULL))
            mimeType = xdg_mime_get_mime_type_for_data(buffer.get(), len, NULL);

        file->close();
    }

    if (strcmp(mimeType, XDG_MIME_TYPE_UNKNOWN) == 0 ||
        strcmp(mimeType, XDG_MIME_TYPE_EMPTY) == 0)
    {
        mimeType = XDG_MIME_TYPE_TEXTPLAIN;
    }

    MimeType *type;
    Interface::Holder res(type = new (std::nothrow) MimeType(mimeType, mimeType));

    if (LIKELY(res != NULL))
        switch (iconType)
        {
            case OnlyAppIcon:
            {
                Interface::Holder apps(applications(type));
                const IApplications *apps2 = apps->as<IApplications>();
                ASSERT(apps2 != NULL);

                IApplications::const_iterator app(apps2->begin());

                if (app != apps2->end())
                {
                    const IApplication *app2 = (*app)->as<IApplication>();
                    ASSERT(app2 != NULL);

                    type->setIcon(app2->icon());
                }
                else
                    type->setIcon(iconCache.findMimeIcon(XDG_MIME_TYPE_TEXTPLAIN, SmallIcon, theme));

                break;
            }

            case OnlyTypeIcon:
            {
                type->setIcon(iconCache.findMimeIcon(mimeType, XDG_MIME_TYPE_TEXTPLAIN, SmallIcon, theme));
                break;
            }

            case AppIconIfNoTypeIcon:
            {
                Interface::Holder icon = iconCache.findMimeIcon(mimeType, SmallIcon, theme);

                if (icon.isValid())
                    type->setIcon(icon);
                else
                {
                    Interface::Holder apps(applications(type));
                    const IApplications *apps2 = apps->as<IApplications>();
                    ASSERT(apps2 != NULL);

                    IApplications::const_iterator app(apps2->begin());

                    if (app != apps2->end())
                    {
                        const IApplication *app2 = (*app)->as<IApplication>();
                        ASSERT(app2 != NULL);

                        type->setIcon(app2->icon());
                    }
                    else
                        type->setIcon(iconCache.findMimeIcon(XDG_MIME_TYPE_TEXTPLAIN, SmallIcon, theme));
                }

                break;
            }
        }

    return res;
}

Interface::Holder Desktop::typeOfDirectory() const
{
    return Interface::Holder(new (std::nothrow) MimeType("inode/directory", "File system directory", iconCache.findMimeIcon("inode/directory", SmallIcon, theme)));
}

}
