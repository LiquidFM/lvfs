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

#include <cstddef>


namespace LVFS {

class IEntry
{
public:
    virtual ~IEntry();

    virtual const char *title() const = 0;
    virtual const char *type() const = 0;

    virtual bool open() = 0;
    virtual size_t read(void *buffer, size_t size) = 0;
    virtual size_t write(const void *buffer, size_t size) = 0;
    virtual void close() = 0;

    virtual bool rename(const char *fileName) = 0;
};

}

#endif /* LVFS_IENTRY_H_ */