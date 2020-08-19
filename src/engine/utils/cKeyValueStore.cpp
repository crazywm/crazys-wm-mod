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

#include "utils/cKeyValueStore.h"
#include "tinyxml2.h"
#include "xml/util.h"
#include "xml/getattr.h"
#include <utility>
#include "CLog.h"

cKeyValueBase::cKeyValueBase() {
}

int cKeyValueBase::get_integer(const char *name) const {
    return boost::get<int>(get_value(name));
}

bool cKeyValueBase::get_bool(const char *name) const {
    return boost::get<bool>(get_value(name));
}

const std::string& cKeyValueBase::get_str(const char* name) const {
    return boost::get<std::string>(get_value(name));
}


float cKeyValueBase::get_float(const char* name) const {
    const auto& val = get_value(name);
    if(val.type() == typeid(float)) {
        return boost::get<float>(val);
    } else {
        return (float)boost::get<sPercent>(val);
    }
}

sPercent cKeyValueBase::get_percent(const char* name) const {
    return boost::get<sPercent>(get_value(name));
}


void cKeyValueBase::set_value(const char *name, bool value) {
    auto& val = get_value(name);
    if(val.type() == typeid(bool)) {
        val = value;
    } else {
        throw std::logic_error("Cannot set non-bool setting to bool value");
    }
}

void cKeyValueBase::set_value(const char* name, int value) {
    auto& val = get_value(name);
    if(val.type() == typeid(int)) {
        val = value;
    } else {
        throw std::logic_error("Cannot set non-integer setting to integer value");
    }
}

void cKeyValueBase::set_value(const char* name, float value) {
    auto& val = get_value(name);
    if(val.type() == typeid(float)) {
        val = value;
    } else {
        throw std::logic_error("Cannot set non-float setting to float value");
    }
}

void cKeyValueBase::set_value(const char* name, std::string value) {
    auto& val = get_value(name);
    if(val.type() == typeid(std::string)) {
        val = std::move(value);
    } else {
        throw std::logic_error("Cannot set non-string setting to string value");
    }
}

void cKeyValueBase::set_value(const char* name, sPercent value) {
    auto& val = get_value(name);
    if(val.type() == typeid(sPercent)) {
        val = std::move(value);
    } else {
        throw std::logic_error("Cannot set non-string setting to string value");
    }
}

void cKeyValueBase::load_xml(const tinyxml2::XMLElement& root) {
    for(auto& element : IterateChildElements(root, "Entry")) {
        try {
            auto name = GetStringAttribute(element, "Key");
            auto& value = get_value(name);
            switch(value.which()) {
                case 0:         // bool
                    value = GetBoolAttribute(element, "Value");
                    break;
                case 1:         // int
                    value = GetIntAttribute(element, "Value");
                    break;
                case 2:         // float
                {
                    const char* val_str = GetStringAttribute(element, "Value");
                    float val = std::strtof(val_str, nullptr);
                    value = val;
                    break;
                }
                case 3:         // percent
                {
                    const char* val_str = GetStringAttribute(element, "Value");
                    float val = std::strtof(val_str, nullptr);
                    value = sPercent(val / 100.f);
                    break;
                }
                case 4:         // string
                default:
                    value = std::string(GetStringAttribute(element, "Value"));
                    break;
            }

        } catch( std::exception& error ) {
            g_LogFile.error("settings", "Could not read attribute from xml element on line ",
                            element.GetLineNum(), ": ", error.what());
            continue;
        }
    }
}

namespace {
    struct visitor {
        tinyxml2::XMLElement &t;

        template<class T>
        void operator()(T value) const {
            t.SetAttribute("Value", value);
        }

        void operator()(const std::string& value) const {
            t.SetAttribute("Value", value.c_str());
        }

        void operator()(sPercent value) const {
            t.SetAttribute("Value", (float)value * 100);
        }

        void operator()(boost::blank value) const {
        }

        using result_type = void;
    };
}

void cKeyValueBase::save_value_xml(tinyxml2::XMLElement& target, const settings_value_t& value) const {
    auto vis = visitor{target};
    value.apply_visitor(vis);
}

// ---------------------------------------------------------------------------------------------------------------------

const settings_value_t& cSimpleKeyValue::get_value(const char* tag) const {
    return const_cast<cSimpleKeyValue*>(this)->get_value(tag);
}

namespace {
    // https://stackoverflow.com/questions/31320857/how-to-determine-if-a-boostvariant-variable-is-empty
    struct is_blank_f : boost::static_visitor<bool> {
        bool operator()(boost::blank) const { return true; }

        template<typename T>
        bool operator()(T const&) const { return false; }
    };

    bool is_blank(settings_value_t const& v) {
        return boost::apply_visitor(is_blank_f(), v);
    }
}

settings_value_t& cSimpleKeyValue::get_value(const char* tag) {
    auto& ref = m_Settings.at(tag);
    if(is_blank(ref.value)) {
        return get_value(ref.fallback);
    }
    return ref.value;
}

void cSimpleKeyValue::save_xml(tinyxml2::XMLElement& target) const {
    for(auto& s : m_Settings) {
        auto& el = PushNewElement(target, "Entry");
        el.SetAttribute("Key", s.first.c_str());
        save_value_xml(el, s.second.value);
    }
}

void cSimpleKeyValue::add_setting(const char* tag, const char* name, settings_value_t default_value,
                                  const char* description, const char* fallback) {
    m_Settings[tag] = sKeyValueEntry{tag, name, description, fallback, std::move(default_value)};
}

void cSimpleKeyValue::add_setting(const char* tag, const char* name, const char* default_value, const char* description,
                                  const char* fallback) {
    add_setting(tag, name, std::string(default_value), description, fallback);
}
