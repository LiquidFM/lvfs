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

#ifndef LVFS_DEFAULT_IPERMISSIONS_H_
#define LVFS_DEFAULT_IPERMISSIONS_H_

#include <lvfs/Interface>


namespace LVFS {

class IPermissions
{
    DECLARE_INTERFACE(LVFS::IPermissions)

public:
    enum
    {
        Read  = 0x1,
        Write = 0x1 << 1,
        Exec  = 0x1 << 2
    };

public:
    virtual ~IPermissions();

    virtual int permissions() const = 0;
};

}

#endif /* LVFS_DEFAULT_IPERMISSIONS_H_ */
