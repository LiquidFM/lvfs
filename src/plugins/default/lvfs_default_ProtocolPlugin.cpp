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

#include "lvfs_default_ProtocolPlugin.h"
#include "lvfs_default_DefaultFile.h"


namespace LVFS {

ProtocolPlugin::ProtocolPlugin()
{}

ProtocolPlugin::~ProtocolPlugin()
{}

Interface::Holder ProtocolPlugin::open(const char *uri) const
{
    return DefaultFile::open(uri, m_error);
}

const Error &ProtocolPlugin::lastError() const
{
    return m_error;
}

}
