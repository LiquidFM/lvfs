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

#ifndef LVFS_DEFAULT_DEFAULTFILE_H_
#define LVFS_DEFAULT_DEFAULTFILE_H_

#include <cstdio>
#include <lvfs/IType>
#include <lvfs/IFile>
#include <lvfs/IFsFile>
#include <lvfs/IDirectory>
#include <lvfs/Interface>
#include <lvfs/Error>


struct stat;


namespace LVFS {

class PLATFORM_MAKE_PRIVATE DefaultFile : public IFsFile, public IFile, public IDirectory, public IEntry
{
public:
    DefaultFile(const char *fileName);
    DefaultFile(const char *fileName, const struct stat &st);
    virtual ~DefaultFile();

    static Interface::Holder open(const char *uri, Error &error);

    /* IFsFile */

    virtual time_t cTime() const;
    virtual time_t mTime() const;
    virtual time_t aTime() const;

    virtual int permissions() const;
    virtual bool setPermissions(int value);

    /* IFile */

    virtual bool open();
    virtual size_t read(void *buffer, size_t size);
    virtual size_t write(const void *buffer, size_t size);
    virtual bool seek(long offset, Whence whence);
    virtual bool flush();
    virtual void close();

    virtual uint64_t size() const;
    virtual size_t position() const;

    /* IDirectory */

    virtual const_iterator begin() const;
    virtual const_iterator end() const;
    virtual Interface::Holder entry(const char *name) const;

    virtual bool rename(const Interface::Holder &file, const char *name);
    virtual bool remove(const Interface::Holder &file);

    /* IEntry */

    virtual const char *title() const;
    virtual const char *location() const;
    virtual const IType *type() const;

    /* COMMON */

    virtual const Error &lastError() const;

private:
    FILE *m_file;
    bool m_isDir;
    bool m_isFile;
    bool m_isLink;
    bool m_exists;
    int m_permissions;
    uint64_t m_size;
    uint64_t m_lastModified;
    char *m_filePath;
    char *m_fileName;
    const IType *m_type;
    Interface::Holder m_typeHolder;
    mutable Error m_lastError;
};

}

#endif /* LVFS_DEFAULT_DEFAULTFILE_H_ */