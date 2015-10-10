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
#include "lvfs_settings_IntOption.h"

#include <lvfs/Module>

#include <libxml/encoding.h>
#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

#include <cstring>


namespace LVFS {
namespace Settings {

namespace {

class DefaultInitializer : public Visitor
{
public:
    virtual void visit(IntOption &option)
    {
        option.setValue(option.defaultValue());
    }
};

class Loader : public Visitor
{
public:
    Loader(xmlTextReaderPtr reader) :
        m_name(NULL),
        m_value(NULL),
        m_reader(reader)
    {}

    virtual void visit(Scope &option)
    {
        m_name = xmlTextReaderConstName(m_reader);

        if (m_name == NULL)
            option.accept(m_default);
        else if (::strcmp(option.id(), reinterpret_cast<const char *>(m_name)) != 0)
            option.accept(m_default);
        else
            if (xmlTextReaderRead(m_reader) == 1)
                Visitor::visit(option);
            else
                option.accept(m_default);
    }

    virtual void visit(IntOption &option)
    {
        m_name = xmlTextReaderConstName(m_reader);

        if (m_name == NULL)
            option.accept(m_default);
        else if (::strcmp(option.id(), reinterpret_cast<const char *>(m_name)) != 0)
            option.accept(m_default);
        else if (xmlTextReaderRead(m_reader) == 1)
        {
            m_value = xmlTextReaderConstValue(m_reader);

            if (m_value == NULL)
                option.accept(m_default);
            else
                option.setValue(::strtol(reinterpret_cast<const char *>(m_value), NULL, 10));
        }
        else
            option.accept(m_default);
    }

private:
    const xmlChar *m_name;
    const xmlChar *m_value;
    xmlTextReaderPtr m_reader;
    DefaultInitializer m_default;
};

class Saver : public Visitor
{
public:
    Saver(xmlTextWriterPtr writer) :
        m_valid(true),
        m_writer(writer)
    {}

    bool isValid() const { return m_valid; }

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
    else
    {
        DefaultInitializer visitor;

        for (Container::const_iterator i = container.begin(), end = container.end(); i != end; ++i)
            i->second->accept(visitor);
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
















