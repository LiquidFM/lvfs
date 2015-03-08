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

#ifndef LVFS_ERROR_H_
#define LVFS_ERROR_H_

#include <platform/utils.h>


namespace LVFS {

class PLATFORM_MAKE_PUBLIC Error
{
public:
    Error();
    Error(int code);
    virtual ~Error();

    inline bool isOk() const { return m_code == 0; }
    inline int code() const { return m_code; }

    virtual const char *description() const;

private:
    int m_code;
};

}

#endif /* LVFS_ERROR_H_ */
