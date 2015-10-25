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

#include "lvfs_settings_Instance.h"
#include "lvfs_settings_Visitor.h"
#include "lvfs_settings_Scope.h"
#include "lvfs_settings_List.h"
#include "lvfs_settings_IntOption.h"
#include "lvfs_settings_StringOption.h"

#include <lvfs/Module>
#include <efc/ScopedPointer>

#include <libxml/encoding.h>
#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

#include <cstring>
#include <cassert>


namespace LVFS {
namespace Settings {

namespace {

class ListScope : public Scope
{
public:
    ListScope(const char *id, Option *parent = 0) :
        Scope(id, parent)
    {}

    virtual ~ListScope()
    {
        for (Container::iterator i = m_options.begin(); i != m_options.end(); i = m_options.erase(i))
            delete *i;
    }
};


class DefaultListScopeLoader : public Visitor
{
public:
    DefaultListScopeLoader(xmlTextReaderPtr reader, Scope *scope) :
        m_scope(scope),
        m_name(NULL),
        m_value(NULL),
        m_reader(reader)
    {}

    virtual void visit(List &option)
    {}

    virtual void visit(Scope &option)
    {
        EFC::ScopedPointer<ListScope> value(new (std::nothrow) ListScope(option.id()));

        if (UNLIKELY(value.get() == NULL))
            return;

        Scope *scope = m_scope;
        m_scope = value.get();

        for (auto i : option.options())
            i->accept(*this);

        m_scope = scope;
        m_scope->manage(value.release());
    }

    virtual void visit(IntOption &option)
    {
        EFC::ScopedPointer<IntOption> value(new (std::nothrow) IntOption(option.id(), option.defaultValue()));

        if (UNLIKELY(value.get() == NULL))
            return;

        m_scope->manage(value.release());
    }

    virtual void visit(StringOption &option)
    {
        EFC::ScopedPointer<StringOption> value(new (std::nothrow) StringOption(option.id(), option.defaultValue()));

        if (UNLIKELY(value.get() == NULL))
            return;

        m_scope->manage(value.release());
    }

private:
    Scope *m_scope;
    const xmlChar *m_name;
    const xmlChar *m_value;
    xmlTextReaderPtr m_reader;
};


class Loader : public Visitor
{
public:
    struct State
    {
        enum Type
        {
            Loader,
            ListLoader,
            ListScopeLoader
        };
    };

public:
    Loader(xmlTextReaderPtr reader) :
        m_valid(true),
        m_list(NULL),
        m_scope(NULL),
        m_state(State::Loader),
        m_name(NULL),
        m_value(NULL),
        m_reader(reader)
    {}

    virtual void visit(List &option)
    {
        switch (m_state)
        {
            case State::Loader:
                loader(option);
                break;

            case State::ListLoader:
                listLoader(option);
                break;

            case State::ListScopeLoader:
                listScopeLoader(option);
                break;
        }
    }

    virtual void visit(Scope &option)
    {
        switch (m_state)
        {
            case State::Loader:
                loader(option);
                break;

            case State::ListLoader:
                listLoader(option);
                break;

            case State::ListScopeLoader:
                listScopeLoader(option);
                break;
        }
    }

    virtual void visit(IntOption &option)
    {
        switch (m_state)
        {
            case State::Loader:
                loader(option);
                break;

            case State::ListLoader:
                listLoader(option);
                break;

            case State::ListScopeLoader:
                listScopeLoader(option);
                break;
        }
    }

    virtual void visit(StringOption &option)
    {
        switch (m_state)
        {
            case State::Loader:
                loader(option);
                break;

            case State::ListLoader:
                listLoader(option);
                break;

            case State::ListScopeLoader:
                listScopeLoader(option);
                break;
        }
    }

private:
    inline void loader(List &option)
    {
        m_name = xmlTextReaderConstName(m_reader);

        if (m_name != NULL &&
            ::strcmp("list", reinterpret_cast<const char *>(m_name)) == 0 &&
            (m_name = xmlTextReaderGetAttribute(m_reader, BAD_CAST "name")) != NULL &&
            ::strcmp(option.id(), reinterpret_cast<const char *>(m_name)) == 0 &&
            xmlTextReaderRead(m_reader) == 1)
        {
            m_state = State::ListLoader;
            m_list = &option;

            do
            {
                option.value()->accept(*this);

                if (!m_valid)
                    break;

                m_name = xmlTextReaderConstName(m_reader);

                if (m_name == NULL)
                    break;
                else if (::strcmp("list", reinterpret_cast<const char *>(m_name)) == 0)
                {
                    xmlTextReaderRead(m_reader);
                    break;
                }
            }
            while (true);

            m_state = State::Loader;
        }
    }

    inline void loader(Scope &option)
    {
        if ((m_name = xmlTextReaderConstName(m_reader)) != NULL &&
            ::strcmp(option.id(), reinterpret_cast<const char *>(m_name)) == 0 &&
            xmlTextReaderRead(m_reader) == 1)
        {
            Visitor::visit(option);
            xmlTextReaderRead(m_reader);
        }
    }

    inline void loader(IntOption &option)
    {
        if ((m_name = xmlTextReaderConstName(m_reader)) != NULL &&
            ::strcmp(option.id(), reinterpret_cast<const char *>(m_name)) == 0 &&
            xmlTextReaderRead(m_reader) == 1)
        {
            m_value = xmlTextReaderConstValue(m_reader);

            if (m_value != NULL && xmlTextReaderRead(m_reader) == 1)
            {
                option.setValue(::strtol(reinterpret_cast<const char *>(m_value), NULL, 10));
                xmlTextReaderRead(m_reader);
            }
        }
    }

    inline void loader(StringOption &option)
    {
        if ((m_name = xmlTextReaderConstName(m_reader)) != NULL &&
            ::strcmp(option.id(), reinterpret_cast<const char *>(m_name)) == 0 &&
            xmlTextReaderRead(m_reader) == 1)
        {
            m_value = xmlTextReaderConstValue(m_reader);

            if (m_value != NULL && xmlTextReaderRead(m_reader) == 1)
            {
                option.setValue(reinterpret_cast<const char *>(m_value));
                xmlTextReaderRead(m_reader);
            }
        }
    }

    inline void listLoader(List &option)
    {
        m_name = xmlTextReaderConstName(m_reader);

        if (m_name == NULL)
            m_valid = false;
        else if (::strcmp("list", reinterpret_cast<const char *>(m_name)) != 0)
            m_valid = false;
        else if ((m_name = xmlTextReaderGetAttribute(m_reader, BAD_CAST "name")) == NULL || ::strcmp(option.id(), reinterpret_cast<const char *>(m_name)) != 0)
            m_valid = false;
        else if (xmlTextReaderRead(m_reader) != 1)
            m_valid = false;
        else
        {
            EFC::ScopedPointer<List> value(new (std::nothrow) List(option.value()));

            if (UNLIKELY(value.get() == NULL))
                m_valid = false;
            else
            {
                List *list = m_list;
                m_list = value.get();

                do
                {
                    option.value()->accept(*this);

                    if (!m_valid)
                    {
                        m_list = list;
                        return;
                    }

                    m_name = xmlTextReaderConstName(m_reader);

                    if (m_name == NULL)
                    {
                        m_list = list;
                        return;
                    }
                    else if (::strcmp("list", reinterpret_cast<const char *>(m_name)) == 0)
                    {
                        m_valid = xmlTextReaderRead(m_reader) == 1;
                        break;
                    }
                }
                while (true);

                m_list = list;
                m_list->add(value.release());
            }
        }
    }

    inline void listLoader(Scope &option)
    {
        m_name = xmlTextReaderConstName(m_reader);

        if (m_name == NULL)
            m_valid = false;
        else if (::strcmp(option.id(), reinterpret_cast<const char *>(m_name)) != 0)
            m_valid = false;
        else if (xmlTextReaderRead(m_reader) != 1)
            m_valid = false;
        else
        {
            EFC::ScopedPointer<ListScope> value(new (std::nothrow) ListScope(option.id()));

            if (UNLIKELY(value.get() == NULL))
                m_valid = false;
            else
            {
                Scope *scope = m_scope;
                m_scope = value.get();
                m_state = State::ListScopeLoader;

                for (auto i : option.options())
                {
                    i->accept(*this);

                    if (!m_valid)
                    {
                        m_scope = scope;
                        return;
                    }
                }

                m_scope = scope;
                m_state = State::ListLoader;

                m_list->add(value.release());
                m_valid = xmlTextReaderRead(m_reader) == 1;
            }
        }
    }

    inline void listLoader(IntOption &option)
    {
        m_name = xmlTextReaderConstName(m_reader);

        if (m_name == NULL)
            m_valid = false;
        else if (::strcmp(option.id(), reinterpret_cast<const char *>(m_name)) != 0)
            m_valid = false;
        else if (xmlTextReaderRead(m_reader) != 1)
            m_valid = false;
        else
        {
            m_value = xmlTextReaderConstValue(m_reader);

            if (m_value == NULL)
                m_valid = false;
            else if (xmlTextReaderRead(m_reader) != 1)
                m_valid = false;
            else
            {
                EFC::ScopedPointer<IntOption> value(new (std::nothrow) IntOption(option.id(), option.defaultValue()));

                if (UNLIKELY(value.get() == NULL))
                    m_valid = false;
                else
                {
                    value->setValue(::strtol(reinterpret_cast<const char *>(m_value), NULL, 10));
                    m_list->add(value.release());
                    m_valid = xmlTextReaderRead(m_reader) == 1;
                }
            }
        }
    }

    inline void listLoader(StringOption &option)
    {
        m_name = xmlTextReaderConstName(m_reader);

        if (m_name == NULL)
            m_valid = false;
        else if (::strcmp(option.id(), reinterpret_cast<const char *>(m_name)) != 0)
            m_valid = false;
        else if (xmlTextReaderRead(m_reader) != 1)
            m_valid = false;
        else
        {
            m_value = xmlTextReaderConstValue(m_reader);

            if (m_value == NULL)
                m_valid = false;
            else if (xmlTextReaderRead(m_reader) != 1)
                m_valid = false;
            else
            {
                EFC::ScopedPointer<StringOption> value(new (std::nothrow) StringOption(option.id(), option.defaultValue()));

                if (UNLIKELY(value.get() == NULL))
                    m_valid = false;
                else
                {
                    value->setValue(reinterpret_cast<const char *>(m_value));
                    m_list->add(value.release());
                    m_valid = xmlTextReaderRead(m_reader) == 1;
                }
            }
        }
    }

    inline void listScopeLoader(List &option)
    {
        EFC::ScopedPointer<List> value(new (std::nothrow) List(option.value()));

        if (UNLIKELY(value.get() == NULL))
        {
            m_valid = false;
            return;
        }

        m_name = xmlTextReaderConstName(m_reader);

        if (m_name == NULL)
            m_valid = false;
        else if (::strcmp("list", reinterpret_cast<const char *>(m_name)) != 0)
            m_valid = false;
        else if ((m_name = xmlTextReaderGetAttribute(m_reader, BAD_CAST "name")) == NULL || ::strcmp(option.id(), reinterpret_cast<const char *>(m_name)) != 0)
            m_valid = false;
        else if (xmlTextReaderRead(m_reader) != 1)
            m_valid = false;
        else
        {
            List *list = m_list;
            m_list = value.get();
            m_state = State::ListLoader;

            do
            {
                option.value()->accept(*this);

                if (!m_valid)
                {
                    m_valid = false;
                    break;
                }

                m_name = xmlTextReaderConstName(m_reader);

                if (m_name == NULL)
                {
                    m_valid = false;
                    break;
                }
                else if (::strcmp("list", reinterpret_cast<const char *>(m_name)) == 0)
                {
                    m_valid = xmlTextReaderRead(m_reader) == 1;
                    break;
                }
            }
            while (true);

            m_list = list;
            m_state = State::ListScopeLoader;
        }

        m_scope->manage(value.release());
    }

    inline void listScopeLoader(Scope &option)
    {
        EFC::ScopedPointer<ListScope> value(new (std::nothrow) ListScope(option.id()));

        if (UNLIKELY(value.get() == NULL))
        {
            m_valid = false;
            return;
        }

        m_name = xmlTextReaderConstName(m_reader);

        if (m_name == NULL)
        {
            DefaultListScopeLoader loader(m_reader, value.get());

            for (auto i : option.options())
                i->accept(loader);

            m_valid = false;
        }
        else if (::strcmp(option.id(), reinterpret_cast<const char *>(m_name)) != 0)
        {
            DefaultListScopeLoader loader(m_reader, value.get());

            for (auto i : option.options())
                i->accept(loader);

            m_valid = false;
        }
        else
            if (xmlTextReaderRead(m_reader) == 1)
            {
                Scope *scope = m_scope;
                m_scope = value.get();

                for (auto i : option.options())
                {
                    i->accept(*this);

                    if (!m_valid)
                    {
                        m_scope = scope;
                        return;
                    }
                }

                m_scope = scope;
                m_valid = xmlTextReaderRead(m_reader) == 1;
            }
            else
            {
                DefaultListScopeLoader loader(m_reader, value.get());

                for (auto i : option.options())
                    i->accept(loader);

                m_valid = false;
            }

        m_scope->manage(value.release());
    }

    inline void listScopeLoader(IntOption &option)
    {
        EFC::ScopedPointer<IntOption> value(new (std::nothrow) IntOption(option.id(), option.defaultValue()));

        if (UNLIKELY(value.get() == NULL))
        {
            m_valid = false;
            return;
        }

        m_name = xmlTextReaderConstName(m_reader);

        if (m_name == NULL)
            m_valid = false;
        else if (::strcmp(value->id(), reinterpret_cast<const char *>(m_name)) != 0)
            m_valid = false;
        else if (xmlTextReaderRead(m_reader) != 1)
            m_valid = false;
        else
        {
            m_value = xmlTextReaderConstValue(m_reader);

            if (m_value == NULL)
                m_valid = false;
            else if (xmlTextReaderRead(m_reader) != 1)
                m_valid = false;
            else
            {
                value->setValue(::strtol(reinterpret_cast<const char *>(m_value), NULL, 10));
                m_valid = xmlTextReaderRead(m_reader) == 1;
            }
        }

        m_scope->manage(value.release());
    }

    inline void listScopeLoader(StringOption &option)
    {
        EFC::ScopedPointer<StringOption> value(new (std::nothrow) StringOption(option.id(), option.defaultValue()));

        if (UNLIKELY(value.get() == NULL))
        {
            m_valid = false;
            return;
        }

        m_name = xmlTextReaderConstName(m_reader);

        if (m_name == NULL)
            m_valid = false;
        else if (::strcmp(value->id(), reinterpret_cast<const char *>(m_name)) != 0)
            m_valid = false;
        else if (xmlTextReaderRead(m_reader) != 1)
            m_valid = false;
        else
        {
            m_value = xmlTextReaderConstValue(m_reader);

            if (m_value == NULL)
                m_valid = false;
            else if (xmlTextReaderRead(m_reader) != 1)
                m_valid = false;
            else
            {
                value->setValue(reinterpret_cast<const char *>(m_value));
                m_valid = xmlTextReaderRead(m_reader) == 1;
            }
        }

        m_scope->manage(value.release());
    }

private:
    bool m_valid;
    List *m_list;
    Scope *m_scope;
    State::Type m_state;
    const xmlChar *m_name;
    const xmlChar *m_value;
    xmlTextReaderPtr m_reader;
};


class Saver : public Visitor
{
public:
    Saver(xmlTextWriterPtr writer) :
        m_valid(true),
        m_writer(writer)
    {}

    bool isValid() const { return m_valid; }

    virtual void visit(List &option)
    {
        if (xmlTextWriterStartElement(m_writer, BAD_CAST "list") < 0)
            m_valid = false;
        else if (xmlTextWriterWriteFormatAttribute(m_writer, BAD_CAST "name", "%s", option.id()) < 0)
            m_valid = false;
        else
        {
            for (auto i : option.values())
            {
                i->accept(*this);

                if (!m_valid)
                    return;
            }

            m_valid = xmlTextWriterEndElement(m_writer) >= 0;
        }
    }

    virtual void visit(Scope &option)
    {
        if (xmlTextWriterStartElement(m_writer, BAD_CAST option.id()) < 0)
            m_valid = false;
        else
        {
            Visitor::visit(option);

            if (m_valid)
                m_valid = xmlTextWriterEndElement(m_writer) >= 0;
        }
    }

    virtual void visit(IntOption &option)
    {
        m_valid = xmlTextWriterWriteFormatElement(m_writer, BAD_CAST option.id(), "%d", option.value()) >= 0;
    }

    virtual void visit(StringOption &option)
    {
        m_valid = xmlTextWriterWriteFormatElement(m_writer, BAD_CAST option.id(), "%s", option.value()) >= 0;
    }

private:
    bool m_valid;
    xmlTextWriterPtr m_writer;
};

}


Instance::Instance(const char *file) :
    m_file(file)
{
    LIBXML_TEST_VERSION
}

Instance::~Instance()
{}

bool Instance::manage(Scope *scope)
{
    Container::iterator lb = m_options.lower_bound(scope->id());

    if (lb != m_options.end() && !(m_options.key_comp()(scope->id(), lb->first)))
        return false;
    else
        m_options.insert(lb, Container::value_type(scope->id(), scope));

    return true;
}

void Instance::load()
{
    load(m_options);
}

void Instance::save()
{
    save(m_options);
}

void Instance::load(const Container &container) const
{
    const xmlChar *name;
    xmlTextReaderPtr reader = xmlReaderForFile(m_file, NULL, 0);

    if (reader != NULL &&
        xmlTextReaderRead(reader) == 1 &&
        (name = xmlTextReaderConstName(reader)) != NULL &&
        ::strcmp("LVFS", reinterpret_cast<const char *>(name)) == 0 &&
        xmlTextReaderRead(reader) == 1)
    {
        Loader loader(reader);

        for (Container::const_iterator i = container.begin(), end = container.end(); i != end; ++i)
            i->second->accept(loader);
    }

    xmlFreeTextReader(reader);
}

void Instance::save(const Container &container) const
{
    xmlTextWriterPtr writer = xmlNewTextWriterFilename(m_file, 0);

    if (writer == NULL)
        return;

    if (xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL) < 0)
        return;

    if (xmlTextWriterStartElement(writer, BAD_CAST "LVFS") < 0)
        return;

    Saver saver(writer);

    for (Container::const_iterator i = container.begin(), end = container.end(); i != end; ++i)
    {
        i->second->accept(saver);

        if (!saver.isValid())
        {
            xmlFreeTextWriter(writer);
            return;
        }
    }

    xmlTextWriterEndDocument(writer);
    xmlFreeTextWriter(writer);
}

}}
