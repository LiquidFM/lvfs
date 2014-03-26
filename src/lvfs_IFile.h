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

#ifndef LVFS_IFILE_H_
#define LVFS_IFILE_H_

#include <cstddef>
#include <cstdint>
#include <lvfs/Interface>
#include <lvfs/Error>


namespace LVFS {

/**
 * XXX: This is not a file on your HDD/SSD.
 * It's just a readable/writeable entry.
 */
class IFile
{
    DECLARE_INTERFACE(LVFS::IFile)

public:
    enum Whence
    {
        FromBeginning,
        FromCurrent,
        FromEnd
    };

public:
    virtual ~IFile();

    virtual bool open() = 0;
    virtual size_t read(void *buffer, size_t size) = 0;
    virtual size_t write(const void *buffer, size_t size) = 0;
    virtual bool seek(long offset, Whence whence = FromCurrent) = 0;
    virtual bool flush() = 0;
    virtual void close() = 0;

    virtual uint64_t size() const = 0;
    virtual size_t position() const = 0;
    virtual const Error &lastError() const = 0;
};

}

#endif /* LVFS_IFILE_H_ */
