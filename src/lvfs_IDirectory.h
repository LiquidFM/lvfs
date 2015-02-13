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

#ifndef LVFS_IDIRECTORY_H_
#define LVFS_IDIRECTORY_H_

#include <cstddef>
#include <iterator>
#include <platform/utils.h>
#include <lvfs/Interface>
#include <lvfs/IEntry>
#include <lvfs/Error>


namespace LVFS {

/**
 * XXX: This is not a directory on your HDD/SSD.
 * It's just a list of entries.
 */
class PLATFORM_MAKE_PUBLIC IDirectory
{
    DECLARE_INTERFACE(LVFS::IDirectory)

public:
    class const_iterator
    {
    public:
        typedef std::forward_iterator_tag iterator_category;
        typedef std::ptrdiff_t            difference_type;
        typedef Interface::Holder         value_type;
        typedef const value_type *        pointer;
        typedef const value_type &        reference;

    public:
        inline const_iterator();

        inline bool isValid() const;
        inline reference operator*() const;
        inline pointer operator->() const;
        inline const_iterator &operator++();
        inline bool operator==(const const_iterator &x) const;
        inline bool operator!=(const const_iterator &x) const;

    protected:
        class Implementation;
        typedef ::EFC::Holder<Implementation> Holder;

        inline const_iterator(Implementation *imp);

    protected:
        class Implementation : public Holder::Data
        {
            PLATFORM_MAKE_NONCOPYABLE(Implementation)
            PLATFORM_MAKE_NONMOVEABLE(Implementation)

        public:
            Implementation();
            virtual ~Implementation();

            virtual bool isEqual(const Holder &other) const = 0;
            virtual reference asReference() const = 0;
            virtual pointer asPointer() const = 0;
            virtual void next() = 0;
        };

    private:
        Holder m_data;
    };

    struct Progress
    {
        void *arg;
        void (*function)(void *arg, off64_t processed);
        const volatile bool &aborted;
    };

public:
    virtual ~IDirectory();

    virtual const_iterator begin() const = 0;
    virtual const_iterator end() const = 0;

    virtual bool exists(const char *name) const = 0;
    virtual Interface::Holder entry(const char *name, const IType *type, bool create = false) = 0;
    virtual Interface::Holder copy(const Progress &callback, const Interface::Holder &file, bool move = false) = 0;

    virtual bool rename(const Interface::Holder &file, const char *name) = 0;
    virtual bool remove(const Interface::Holder &file) = 0;

    virtual const Error &lastError() const = 0;
};


IDirectory::const_iterator::const_iterator() :
    m_data()
{}

bool IDirectory::const_iterator::isValid() const
{ return m_data != NULL; }

IDirectory::const_iterator::reference IDirectory::const_iterator::operator*() const
{ return m_data->asReference(); }

IDirectory::const_iterator::pointer IDirectory::const_iterator::operator->() const
{ return m_data->asPointer(); }

IDirectory::const_iterator &IDirectory::const_iterator::operator++()
{ m_data->next(); return *this; }

bool IDirectory::const_iterator::operator==(const const_iterator &x) const
{ return m_data == x.m_data || m_data->isEqual(x.m_data); }

bool IDirectory::const_iterator::operator!=(const const_iterator &x) const
{ return m_data != x.m_data && !m_data->isEqual(x.m_data); }

IDirectory::const_iterator::const_iterator(Implementation *imp) :
    m_data(imp)
{}

}

#endif /* LVFS_IDIRECTORY_H_ */
