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

#include <cstdio>
#include <cstddef>
#include <cstdint>
#include <lvfs/Interface>
#include <lvfs/Error>


namespace LVFS {

/**
 * XXX: This is a minimal interface
 * to read/write data from/to IEntry
 */
class PLATFORM_MAKE_PUBLIC IFile
{
    DECLARE_INTERFACE(LVFS::IFile)

public:
    enum Mode
    {
        Read,
        Write,
        ReadWrite
    };

    enum Whence
    {
        FromBeginning,
        FromCurrent,
        FromEnd
    };

    enum Advise
    {
        /** The application has no specific advice to give on this range of the file. It should be treated as normal. */
        Normal,
        /** The application intends to access the data in the specified range in a random (nonsequential) order. */
        Random,
        /** The application intends to access the data in the specified range sequentially, from lower to higher addresses. */
        Sequential,
        /** The application intends to access the data in the specified range in the near future. */
        WillNeed,
        /** The application intends to access the data in the specified range in the near future, but only once. */
        NoReuse,
        /** The application does not intend to access the pages in the specified range in the near future. */
        DontNeed
    };

public:
    virtual ~IFile();

    virtual size_t read(void *buffer, size_t size) = 0;
    virtual size_t write(const void *buffer, size_t size) = 0;
    virtual bool advise(off64_t offset, off64_t len, Advise advise) = 0;
    virtual bool seek(off64_t offset, Whence whence = FromCurrent) = 0;
    virtual bool flush() = 0;

    virtual const Error &lastError() const = 0;
};

}

#endif /* LVFS_IFILE_H_ */
