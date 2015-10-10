/**
 * This file is part of lvfs.
 *
 * Copyright (C) 2011-2015 Dmitriy Vilkov, <dav.daemon@gmail.com>
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

#ifndef LVFS_SETTINGS_SCOPE_H_
#define LVFS_SETTINGS_SCOPE_H_

#include <efc/List>
#include <lvfs/settings/Option>


namespace LVFS {
namespace Settings {

class PLATFORM_MAKE_PUBLIC Scope : public Option
{
public:
    typedef EFC::List<Option *> Container;

public:
    Scope(const char *id, Option *parent = 0) :
        Option(id, parent)
    {}
    virtual ~Scope();

    const Container &options() const { return m_options; }

    virtual void manage(Option *option);
    virtual void accept(Visitor &visitor);

private:
    Container m_options;
};

}}

#endif /* LVFS_SETTINGS_SCOPE_H_ */
