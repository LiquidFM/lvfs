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

#ifndef LVFS_DEFAULT_FILE_H_
#define LVFS_DEFAULT_FILE_H_

#include "lvfs_default_DefaultFile.h"


namespace LVFS {

class File : public Implements<IPermissions, IFile, IEntry>
{
public:
    File(const char *fileName);
    File(const char *fileName, const struct stat &st);
    virtual ~File();

    /* IPermissions */

    virtual int permissions() const;

    /* IFile */

    virtual bool open();
    virtual size_t read(void *buffer, size_t size);
    virtual size_t write(const void *buffer, size_t size);
    virtual bool seek(long offset, Whence whence);
    virtual bool flush();
    virtual void close();

    virtual uint64_t size() const;
    virtual size_t position() const;
    virtual const Error &lastError() const;

    /* IEntry */

    virtual const char *title() const;
    virtual const char *type() const;

private:
    DefaultFile m_file;
};

}

#endif /* LVFS_DEFAULT_FILE_H_ */
