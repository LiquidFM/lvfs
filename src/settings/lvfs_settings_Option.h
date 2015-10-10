/**
 * This file is part of lvfs.
 *
 * Copyright (C) 2011-2012 Dmitriy Vilkov, <dav.daemon@gmail.com>
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

#ifndef LVFS_SETTINGS_OPTION_H_
#define LVFS_SETTINGS_OPTION_H_

#include <platform/utils.h>


namespace LVFS {
namespace Settings {

class Visitor;


class PLATFORM_MAKE_PUBLIC Option
{
public:
	Option(const char *id, Option *parent = 0) :
	    m_id(id),
		m_parent(parent)
	{}
	virtual ~Option();

    const char *id() const { return m_id; }
	Option *parent() const { return m_parent; }

	virtual void accept(Visitor &visitor) = 0;

    template <typename R> inline
    R *as() { return static_cast<R *>(this); }

    template <typename R> inline
    const R *as() const { return static_cast<const R *>(this); }

private:
    const char *m_id;
	Option *m_parent;
};

}}

#endif /* LVFS_SETTINGS_OPTION_H_ */
