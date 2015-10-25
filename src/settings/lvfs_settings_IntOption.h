/**
 * This file is part of lvfs_.
 *
 * Copyright (C) 2011-2015 Dmitriy Vilkov, <dav.daemon@gmail.com>
 *
 * lvfs_ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * lvfs_ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with lvfs_. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LVFS_SETTINGS_INTOPTION_H_
#define LVFS_SETTINGS_INTOPTION_H_

#include <lvfs/settings/Option>


namespace LVFS {
namespace Settings {

class PLATFORM_MAKE_PUBLIC IntOption : public Option
{
public:
    IntOption(const char *id, int defaultValue, Option *parent = 0) :
        Option(id, parent),
        m_value(defaultValue),
        m_defaultValue(defaultValue)
    {}
    virtual ~IntOption();

    inline int value() const { return m_value; }
    inline void setValue(int value) { m_value = value; }
    inline int defaultValue() const { return m_defaultValue; }

    virtual void accept(Visitor &visitor);

private:
    int m_value;
    const int m_defaultValue;
};

}}

#endif /* LVFS_SETTINGS_INTOPTION_H_ */
