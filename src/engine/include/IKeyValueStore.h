/*
* Copyright 2009, 2010, The Pink Petal Development Team.
* The Pink Petal Devloment Team are defined as the game's coders
* who meet on http://pinkpetal.org
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef WM_IKEYVALUESTORE_H
#define WM_IKEYVALUESTORE_H

#include <string>
#include "utils/sPercent.h"

namespace tinyxml2 {
    class XMLElement;
}

class IKeyValueStore {
public:
    virtual ~IKeyValueStore() = default;

    // access
    virtual int   get_integer(const char* name) const = 0;
    virtual bool  get_bool(const char* name) const    = 0;
    virtual float get_float(const char* name) const   = 0;
    virtual const std::string& get_str(const char* name) const = 0;
    virtual sPercent get_percent(const char* name) const = 0;

    virtual void set_value(const char* name, bool value) = 0;
    virtual void set_value(const char* name, int value)  = 0;
    virtual void set_value(const char* name, std::string value) = 0;
    virtual void set_value(const char* name, float value) = 0;
    virtual void set_value(const char* name, sPercent value) = 0;

    // load/save
    virtual void load_xml(const tinyxml2::XMLElement& source) = 0;
    virtual void save_xml(tinyxml2::XMLElement& target) const = 0;
};


#endif //WM_IKEYVALUESTORE_H
