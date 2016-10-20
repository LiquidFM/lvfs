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

#ifndef LVFS_DESKTOP_H_
#define LVFS_DESKTOP_H_

#include <platform/utils.h>
#include <lvfs/Interface>
#include <lvfs/Error>


namespace LVFS {

class IType;
class IEntry;


class PLATFORM_MAKE_PUBLIC Desktop
{
public:
    enum IconType
    {
        OnlyAppIcon,
        OnlyTypeIcon,
        AppIconIfNoTypeIcon
    };

    enum IconSize
    {
        SmallIcon = 16
    };

    class Locale
    {
    public:
        Locale();

        const char *lang() const { return m_lang; }
        const char *country() const { return m_country; }
        const char *encoding() const { return m_encoding; }
        const char *modifier() const { return m_modifier; }

    private:
        enum { MaxLenghtOfLocale = 24 };

    private:
        const char *m_lang;
        const char *m_country;
        const char *m_encoding;
        const char *m_modifier;
        char m_buffer[MaxLenghtOfLocale];
    };

    class Theme
    {
    public:
        struct Actions
        {
            enum Enum
            {
                Refresh,
                Exit,
                Copy,
                Cut,
                Paste,
                Properties,
                Pack,
                Extract,
                Search,
                Open
            };
        };

        struct Status
        {
            enum Enum
            {
                Missing
            };
        };

        struct Devices
        {
            enum Enum
            {
                HardDisk,
                RemovableMedia,
                RemovableMediaUsb,
                RemovableMediaUsbPendrive,
                Flash,
                FlashMemoryStick,
                FlashSdMmc,
                FlashSmartMedia,
                Floppy,
                Optical,
                OpticalAudio,
                OpticalBluRay,
                OpticalData,
                OpticalDvdVideo,
                OpticalDvd,
                OpticalMixedCd,
                OpticalRecordable,
                OpticalVideo,
                Tape
            };
        };

        Interface::Holder icon(Actions::Enum context, IconSize iconSize = SmallIcon) const;
        Interface::Holder icon(Status::Enum context, IconSize iconSize = SmallIcon) const;
        Interface::Holder icon(Devices::Enum context, IconSize iconSize = SmallIcon) const;
        Interface::Holder icon(const char *context, IconSize iconSize = SmallIcon) const;
    };

public:
    Desktop();
    ~Desktop();

    const Locale &locale() const { return m_locale; }
    const Theme &theme() const { return m_theme; }

    Interface::Holder applications(const IType *type) const;
    Interface::Holder typeOfFile(const char *filename, IconType iconType = AppIconIfNoTypeIcon) const;
    Interface::Holder typeOfFile(const IEntry *file, IconType iconType = AppIconIfNoTypeIcon) const;
    Interface::Holder typeOfDirectory() const;
    Interface::Holder typeOfUnknownFile() const;

    const Error &lastError() const { return m_lastError; }

private:
    Interface::Holder loadMimeType(const char *mimeType, IconType iconType) const;

private:
    Locale m_locale;
    Theme m_theme;
    Error m_lastError;
};

}

#endif /* LVFS_DESKTOP_H_ */
