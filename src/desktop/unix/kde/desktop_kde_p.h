/**
 * This file is part of QFM.
 *
 * Copyright (C) 2011-2012 Dmitriy Vilkov, <dav.daemon@gmail.com>
 *
 * QFM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * QFM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with QFM. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef DESKTOP_KDE_P_H_
#define DESKTOP_KDE_P_H_

#include <cstdlib>
#include <linux/limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


namespace LVFS {
namespace {

struct DesktopPrivate
{
    struct String
    {
        String() :
            data(NULL),
            shouldFree(false)
        {}

        ~String()
        {
            if (shouldFree)
                ::free(data);
        }

        char *data;
        bool shouldFree;
    };


    static const char *iconThemeName(int version)
    {
        const char *result = version >= 4 ? "oxygen" : "crystalsvg";
//        QSettings settings(kdeHome(version) + QString::fromLatin1("/share/config/kdeglobals"), QSettings::IniFormat);
//
//        settings.beginGroup(QString::fromLatin1("Icons"));
//        result = settings.value(QString::fromLatin1("Theme"), result).toString();

        return result;
    }

    static const char *kdeHome(int version)
    {
        static String kdeHomePath;

        if (kdeHomePath.data == NULL)
        {
            if (const char *var = ::getenv("KDEHOME"))
                kdeHomePath.data = const_cast<char *>(var);

            if (kdeHomePath.data == NULL)
            {
                if (const char *home = ::getenv("HOME"))
                {
                    char path[PATH_MAX];
                    struct stat st;

                    if (version == 4)
                    {
                        ::strncpy(path, home, PATH_MAX - 1);
                        ::strncat(path, "/.kde4", PATH_MAX - strlen(path) - 1);

                        if (::stat(path, &st) == 0)
                        {
                            kdeHomePath.data = ::strdup(path);
                            kdeHomePath.shouldFree = true;
                        }
                    }

                    if (kdeHomePath.data == NULL)
                    {
                        ::strncpy(path, home, PATH_MAX - 1);
                        ::strncat(path, "/.kde", PATH_MAX - strlen(path) - 1);

                        kdeHomePath.data = ::strdup(path);
                        kdeHomePath.shouldFree = true;
                    }
                }
            }
        }

        return kdeHomePath.data;
    }
};

}}

#endif /* DESKTOP_KDE_P_H_ */
