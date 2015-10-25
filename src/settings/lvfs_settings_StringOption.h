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

#ifndef LVFS_SETTINGS_STRINGOPTION_H_
#define LVFS_SETTINGS_STRINGOPTION_H_

#include <lvfs/settings/Option>


namespace LVFS {
namespace Settings {

class PLATFORM_MAKE_PUBLIC StringOption : public Option
{
public:
    StringOption(const char *id, const char *defaultValue, Option *parent = 0);
    virtual ~StringOption();

    inline const char *value() const { return m_value; }
    void setValue(const char *value);
    inline const char *defaultValue() const { return m_defaultValue; }

    virtual void accept(Visitor &visitor);

private:
    char *m_value;
    const char *m_defaultValue;
};

}}

#endif /* LVFS_SETTINGS_STRINGOPTION_H_ */
