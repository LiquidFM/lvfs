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

#ifndef LVFS_SETTINGS_INSTANCE_H_
#define LVFS_SETTINGS_INSTANCE_H_

#include <cstring>
#include <efc/Map>


namespace LVFS {
namespace Settings {

class Scope;


class PLATFORM_MAKE_PUBLIC Instance
{
    PLATFORM_MAKE_NONCOPYABLE(Instance)
    PLATFORM_MAKE_NONMOVEABLE(Instance)
    PLATFORM_MAKE_STACK_ONLY

public:
    Instance(const char *file);
    virtual ~Instance();

    bool manage(Scope *scope);
    virtual void load();
    virtual void save();

protected:
    struct String
    {
        String() :
            data("")
        {}

        String(const char *data) :
            data(data)
        {}

        String(const String &other) :
            data(other.data)
        {}

        bool operator<(const String &other) const
        { return ::strcmp(data, other.data) < 0; }

        bool operator==(const String &other) const
        { return ::strcmp(data, other.data) == 0; }

        const char *data;
    };

    typedef EFC::Map<String, Scope *> Container;

    virtual void load(const Container &container) const;
    virtual void save(const Container &container) const;

private:
    const char *m_file;
    Container m_options;
};

}}

#endif /* LVFS_SETTINGS_INSTANCE_H_ */
