/**
 * This file is part of lvfs.
 *
 * Copyright (C) 2011-2016 Dmitriy Vilkov, <dav.daemon@gmail.com>
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

#include <lvfs/IEntry>

#include <efc/Map>
#include <efc/ScopedPointer>

#include <xdg/xdg.h>

#include <cstring>
#include <pthread.h>


namespace LVFS {
namespace {

class CacheEntry : public Implements<IEntry, IType>
{
public:
    CacheEntry(const char *fileName) :
        m_filePath(::strdup(fileName)),
        m_fileName(::strrchr(m_filePath, '/') + 1)
    {}

    virtual ~CacheEntry()
    {
        ::free(m_filePath);
    }

    virtual const char *title() const { return m_fileName; }
    virtual const char *schema() const { return "file"; }
    virtual const char *location() const { return m_filePath; }
    virtual const IType *type() const { return this; }
    virtual Interface::Holder open(IStream::Mode mode) const { return Interface::Holder(); }

    virtual const char *name() const { return XDG_MIME_TYPE_UNKNOWN; }
    virtual Interface::Holder icon() const { return Interface::Holder(); }
    virtual const char *description() const { return XDG_MIME_TYPE_UNKNOWN; }

private:
    char *m_filePath;
    char *m_fileName;
};


class Cache
{
protected:
    struct Index
    {
        Index() :
            name(NULL),
            size(0),
            context(XdgThemeActions)
        {}

        Index(Index &&other) :
            name(other.name),
            size(other.size),
            context(other.context)
        {
            other.name = NULL;
        }

        Index(const char *name, Context context) :
            name(::strdup(name)),
            size(0),
            context(context)
        {}

        Index(const char *name, int size, Context context) :
            name(::strdup(name)),
            size(size),
            context(context)
        {}

        ~Index()
        {
            if (name)
                ::free(name);
        }

        bool operator==(const Index &other) const
        {
            return context == other.context && size == other.size && strcmp(name, other.name) == 0;
        }

        bool operator<(const Index &other) const
        {
            return context < other.context || size < other.size || strcmp(name, other.name) < 0;
        }

        char *name;
        int size;
        Context context;

    private:
        Index(const Index &other);
    };

    class ReadLocker
    {
    public:
        ReadLocker(pthread_rwlock_t &lock) :
            m_lock(lock)
        {
            ::pthread_rwlock_rdlock(&m_lock);
        }
        ~ReadLocker()
        {
            ::pthread_rwlock_unlock(&m_lock);
        }

    private:
        pthread_rwlock_t &m_lock;
    };

    class WriteLocker
    {
    public:
        WriteLocker(pthread_rwlock_t &lock) :
            m_lock(lock)
        {
            ::pthread_rwlock_wrlock(&m_lock);
        }
        ~WriteLocker()
        {
            ::pthread_rwlock_unlock(&m_lock);
        }

    private:
        pthread_rwlock_t &m_lock;
    };

public:
    Cache() :
        m_cacheLock(PTHREAD_RWLOCK_INITIALIZER)
    {}

    ~Cache()
    {}

protected:
    Interface::Holder read(const Index &index)
    {
        ::EFC::Map<Index, Interface::Holder>::const_iterator i = m_cache.find(index);

        if (i != m_cache.end())
            return (*i).second;
        else
            return Interface::Holder();
    }

    Interface::Holder lockedRead(const Index &index)
    {
        ReadLocker lock(m_cacheLock);
        return read(index);
    }

    void write(Index &&index)
    {
        m_cache[std::move(index)] = Interface::Holder();
    }

    Interface::Holder write(Index &&index, const char *fileName)
    {
        return write(std::move(index), Interface::Holder(new (std::nothrow) CacheEntry(fileName)));
    }

    Interface::Holder write(Index &&index, Interface::Holder &&item)
    {
        return (m_cache[std::move(index)] = std::move(item));
    }

    void write(Index &&index, const Interface::Holder &item)
    {
        m_cache[std::move(index)] = item;
    }

protected:
    pthread_rwlock_t m_cacheLock;

private:
    ::EFC::Map<Index, Interface::Holder> m_cache;
};

}}
