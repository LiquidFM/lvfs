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

#ifndef LVFS_DEFAULT_DIRECTORY_H_
#define LVFS_DEFAULT_DIRECTORY_H_

#include <lvfs/IFile>
#include <lvfs/IDirectory>
#include <lvfs/IProperties>


struct stat;


namespace LVFS {

class PLATFORM_MAKE_PRIVATE Directory : public Implements<IEntry, IDirectory, IProperties>
{
public:
    Directory(const char *fileName, const struct stat &st);
    virtual ~Directory();

    /* IEntry */

    virtual const char *title() const;
    virtual const char *schema() const;
    virtual const char *location() const;
    virtual const IType *type() const;
    virtual Interface::Holder open(IFile::Mode mode) const;

    /* IDirectory */

    virtual const_iterator begin() const;
    virtual const_iterator end() const;

    virtual bool exists(const char *name) const;
    virtual Interface::Holder entry(const char *name, const IType *type = NULL, bool create = false);

    virtual bool copy(const Progress &callback, const Interface::Holder &file, bool move = false);
    virtual bool rename(const Interface::Holder &file, const char *name);
    virtual bool remove(const Interface::Holder &file);

    virtual const Error &lastError() const;

    /* IFsFile */

    virtual off64_t size() const;
    virtual time_t cTime() const;
    virtual time_t mTime() const;
    virtual time_t aTime() const;
    virtual int permissions() const;

private:
    char *m_filePath;
    const char *m_fileName;
    Interface::Adaptor<IType> m_type;

private:
    time_t m_cTime;
    time_t m_mTime;
    time_t m_aTime;
    int m_permissions;
    Error m_lastError;
};

}

#endif /* LVFS_DEFAULT_DIRECTORY_H_ */
