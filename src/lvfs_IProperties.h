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

#ifndef LVFS_IPROPERTIES_H_
#define LVFS_IPROPERTIES_H_

#include <time.h>
#include <sys/types.h>
#include <lvfs/Error>
#include <lvfs/Interface>


namespace LVFS {

class PLATFORM_MAKE_PUBLIC IProperties
{
    DECLARE_INTERFACE(LVFS::IProperties)

public:
    enum
    {
        Read  = 0x1,
        Write = 0x1 << 1,
        Exec  = 0x1 << 2
    };

public:
    virtual ~IProperties();

    virtual off64_t size() const = 0;
    virtual time_t cTime() const = 0;
    virtual time_t mTime() const = 0;
    virtual time_t aTime() const = 0;
    virtual int permissions() const = 0;
};

}

#endif /* LVFS_IPROPERTIES_H_ */
