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

#ifndef LVFS_MIMETYPE_H_
#define LVFS_MIMETYPE_H_

#include <lvfs/IType>


namespace LVFS {

class PLATFORM_MAKE_PRIVATE MimeType : public Implements<IType>
{
public:
    MimeType(const char *name, const char *description);
    MimeType(const char *name, const char *description, const Interface::Holder &icon);
    virtual ~MimeType();

    /* IType */

    virtual const char *name() const;
    virtual const char *description() const;

    virtual Interface::Holder icon() const;
    void setIcon(const Interface::Holder &icon) { m_icon = icon; }

private:
    char *m_name;
    char *m_description;
    Interface::Holder m_icon;
};

}

#endif /* LVFS_MIMETYPE_H_ */
