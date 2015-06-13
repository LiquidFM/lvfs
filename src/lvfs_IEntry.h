/**
 * This file is part of qfm.
 *
 * Copyright (C) 2011-2014 Dmitriy Vilkov, <dav.daemon@gmail.com>
 *
 * qfm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * qfm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with qfm. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LVFS_IENTRY_H_
#define LVFS_IENTRY_H_

#include <lvfs/IType>
#include <lvfs/IStream>


namespace LVFS {

class PLATFORM_MAKE_PUBLIC IEntry
{
    DECLARE_INTERFACE(LVFS::IEntry)

public:
    virtual ~IEntry();

    virtual const char *title() const = 0;
    virtual const char *schema() const = 0;
    virtual const char *location() const = 0;
    virtual const IType *type() const = 0;
    virtual Interface::Holder open(IStream::Mode mode = IStream::Read) const = 0;
};

}

#endif /* LVFS_IENTRY_H_ */
