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

#ifndef WM_CKEYVALUESTORE_H
#define WM_CKEYVALUESTORE_H

#include <boost/variant.hpp>
#include <string>
#include <unordered_map>
#include "IKeyValueStore.h"

namespace tinyxml2 {
    class XMLElement;
}

using settings_value_t = boost::variant<bool, int, float, sPercent, std::string, boost::blank>;

/*!
    \brief Base class for implementing Key-Value stores.
    \details This class provides basic implementations for the typed getters and setters
            by reducing them to just two methods that concrete implementations need to provide.

            If the types do not match, this implementation throws.
 */
class cKeyValueBase : public IKeyValueStore {
public:
    cKeyValueBase(const char* element_name, const char* key_name, const char* value_name) :
            m_ElementName(element_name ? element_name : "Entry"), m_KeyName(key_name ? key_name : "Key"), m_ValueName(value_name ? value_name : "Value")
    {};
    virtual const settings_value_t & get_value(const char* name) const = 0;

    // access
    int   get_integer(const char* name) const override;
    bool  get_bool(const char* name) const override;
    float get_float(const char* name) const override;
    const std::string& get_str(const char* name) const override;
    sPercent get_percent(const char* name) const override;

    // update
    void set_value(const char* name, bool value) override;
    void set_value(const char* name, int value) override;
    void set_value(const char* name, std::string value) override;
    void set_value(const char* name, float value) override;
    void set_value(const char* name, sPercent value) override;

    // load/save
    void load_xml(const tinyxml2::XMLElement& source) override;
    // cannot implement saving efficiently, because we currently have no way of iterating all entries
protected:
    virtual settings_value_t& get_value(const char* name) = 0;
    void save_value_xml(tinyxml2::XMLElement& target, const settings_value_t& value) const;

    const char* m_ElementName;
    const char* m_KeyName;
    const char* m_ValueName;
};

class cSimpleKeyValue : public cKeyValueBase {
public:
    using cKeyValueBase::cKeyValueBase;

    const settings_value_t& get_value(const char* tag) const override;

    // io
    void save_xml(tinyxml2::XMLElement& target) const override;

protected:
    void add_setting(const char* tag, const char* name, settings_value_t default_value,
            const char* description=nullptr, const char* fallback=nullptr);
    // we need this explicit overload to prevent char* being converted to bool
    void add_setting(const char* tag, const char* name, const char* default_value,
            const char* description=nullptr, const char* fallback=nullptr);


    settings_value_t & get_value(const char* tag) override;
    struct sKeyValueEntry {
        const char* tag;
        const char* name;
        const char* description;
        const char* fallback;
        settings_value_t value;
    };

    std::unordered_map<std::string, sKeyValueEntry> m_Settings;
};


#endif //WM_CKEYVALUESTORE_H
