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

int cKeyValueBase::get_integer(const char *name) const {
    return boost::get<sIntWithBounds>(get_value(name)).value;
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
    if(val.type() == typeid(sIntWithBounds)) {
        boost::get<sIntWithBounds>(val).assign_checked(value);
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
    bool found_entry = false;
    for(auto& element : IterateChildElements(root, m_ElementName)) {
        found_entry = true;
        try {
            auto name = GetStringAttribute(element, m_KeyName);
            auto& value = get_value(name);
            switch(value.which()) {
                case 0:         // bool
                    value = GetBoolAttribute(element, m_ValueName);
                    break;
                case 1:         // IntWithBounds
                {
                    auto& bounded = boost::get<sIntWithBounds>(value);
                    bounded.assign_checked(GetIntAttribute(element, m_ValueName, bounded.Min, bounded.Max));
                }
                    break;
                case 2:         // float
                {
                    const char* val_str = GetStringAttribute(element, m_ValueName);
                    float val = std::strtof(val_str, nullptr);
                    value = val;
                    break;
                }
                case 3:         // percent
                {
                    const char* val_str = GetStringAttribute(element, m_ValueName);
                    float val = std::strtof(val_str, nullptr);
                    value = sPercent(val / 100.f);
                    break;
                }
                case 4:         // string
                default:
                    value = std::string(GetStringAttribute(element, m_ValueName));
                    break;
            }

        } catch( std::exception& error ) {
            g_LogFile.error("settings", "Could not read attribute from xml element on line ",
                            element.GetLineNum(), ": ", error.what());
            continue;
        }
    }

    if(!found_entry && root.FirstChildElement() != nullptr) {
        // if there is content, but no matching elements, chances are there is a typo/wrong name somewhere
        g_LogFile.error("xml", "Reading a value set with expected Tag <", m_ElementName, ">, did not find ",
                        "any matching data. Root element <", root.Value(), "> given on line ", root.GetLineNum());
    }
}

namespace {
    struct visitor {
        const char* AttributeName;
        tinyxml2::XMLElement &t;

        template<class T>
        void operator()(T value) const {
            t.SetAttribute(AttributeName, value);
        }

        void operator()(const std::string& value) const {
            t.SetAttribute(AttributeName, value.c_str());
        }

        void operator()(sPercent value) const {
            t.SetAttribute(AttributeName, (float)value * 100);
        }

        void operator()(sIntWithBounds value) const {
            t.SetAttribute(AttributeName, value.value);
        }

        void operator()(boost::blank value) const {
        }

        using result_type = void;
    };
}

void cKeyValueBase::save_value_xml(tinyxml2::XMLElement& target, const settings_value_t& value) const {
    auto vis = visitor{m_ValueName, target};
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
    auto ref_iter = m_Settings.find(tag);
    if(ref_iter == m_Settings.end()) {
        throw std::out_of_range(std::string("Key ") + tag + " not found");
    }
    auto& ref = ref_iter->second;
    if(is_blank(ref.value)) {
        return get_value(ref.fallback);
    }
    return ref.value;
}

void cSimpleKeyValue::save_xml(tinyxml2::XMLElement& target) const {
    for(auto& s : m_Settings) {
        auto& el = PushNewElement(target, m_ElementName);
        el.SetAttribute(m_KeyName, s.first.c_str());
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

void sIntWithBounds::assign(int new_value) noexcept(true) {
    if(new_value < Min) {
        value = Min;
        g_LogFile.warning("key-value", "Assigned value ", new_value, " less than lower bound ", Min);
    } else if(new_value > Max) {
        value = Max;
        g_LogFile.warning("key-value", "Assigned value ", new_value, " larger than lower bound ", Max);
    } else {
        value = new_value;
    }
}


void sIntWithBounds::assign_checked(int new_value) noexcept(false) {
    if(new_value < Min) {
        throw std::invalid_argument("Assigned value below lower bound");
    } else if(new_value > Max) {
        throw std::invalid_argument("Assigned value above upper bound");
    } else {
        value = new_value;
    }
}
