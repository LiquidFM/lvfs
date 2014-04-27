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


namespace LVFS {
namespace {

class IconCache : public Cache
{
public:
    IconCache()
    {}

    ~IconCache()
    {}

    Interface::Holder findIcon(const char *name, int iconSize, Context context, const char *theme)
    {
        Index index(name, iconSize, context);
        Interface::Holder icon = lockedRead(index);

        if (!icon.isValid())
        {
            WriteLocker lock(m_cacheLock);
            icon = read(index);

            if (!icon.isValid())
            {
                if (char *icon_path = ::xdg_icon_lookup(name, iconSize, context, theme))
                {
                    icon = write(std::move(index), icon_path);
                    ::free(icon_path);
                }
                else
                    write(std::move(index));
            }
        }

        return icon;
    }

    Interface::Holder findMimeIcon(const char *mimeType, int iconSize, const char *theme)
    {
        Index index(mimeType, iconSize, XdgThemeMimeTypes);
        Interface::Holder icon = lockedRead(index);

        if (!icon.isValid())
        {
            WriteLocker lock(m_cacheLock);
            icon = read(index);

            if (!icon.isValid())
            {
                if (char *icon_path = ::xdg_mime_type_icon_lookup(mimeType, iconSize, theme))
                {
                    icon = write(std::move(index), icon_path);
                    ::free(icon_path);
                }
                else
                    write(std::move(index));
            }
        }

        return icon;
    }

    Interface::Holder findMimeIcon(const char *mimeType, const char *fallbackMimeType, int iconSize, const char *theme)
    {
        Index index(mimeType, iconSize, XdgThemeMimeTypes);
        Interface::Holder icon = lockedRead(index);

        if (!icon.isValid())
        {
            WriteLocker lock(m_cacheLock);
            icon = read(index);

            if (!icon.isValid())
            {
                if (char *icon_path = ::xdg_mime_type_icon_lookup(mimeType, iconSize, theme))
                {
                    icon = write(std::move(index), icon_path);
                    ::free(icon_path);
                }
                else
                    if (icon_path = ::xdg_mime_type_icon_lookup(fallbackMimeType, iconSize, theme))
                    {
                        icon = write(std::move(index), icon_path);
                        ::free(icon_path);
                    }
                    else
                        write(std::move(index));
            }
        }

        return icon;
    }
};

}}
