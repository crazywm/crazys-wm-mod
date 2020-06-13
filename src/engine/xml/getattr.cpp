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

#include "xml/util.h"
#include <tinyxml2.h>
#include <cassert>
#include "CLog.h"


template<class T>
auto Query(const tinyxml2::XMLElement& element, const char*name, T& target) {
    return element.QueryAttribute(name, &target);
}

auto Query(const tinyxml2::XMLElement& element, const char*name, const char*& target) {
    auto result = element.Attribute(name);
    if(!result) {
        return tinyxml2::XML_NO_ATTRIBUTE;
    } else {
        target = result;
        return tinyxml2::XML_SUCCESS;
    }
}


template<class T>
T GetBasicTAttribute(const tinyxml2::XMLElement& element, const char* name, T* fallback = nullptr)
{
    T value;
    auto result = Query(element, name, value);
    switch (result) {
    case tinyxml2::XML_SUCCESS:
        return value;
    case tinyxml2::XML_NO_ATTRIBUTE: {
        std::stringstream error;
        error << "Could not find attribute '" << name << "' in element ' " << element.Value() << "'";
        if(fallback) {
            g_LogFile.log(ELogLevel::WARNING, error.str());
            return *fallback;
        } else {
            throw std::runtime_error(error.str());
        }
    }
    case tinyxml2::XML_WRONG_ATTRIBUTE_TYPE: {
        std::stringstream error;
        error << "Could not convert attribute '" << name << "' with value '" << element.Attribute(name)
              << "' to `" << typeid(T).name() << "` in element ' " << element.Value() << "'";
        if(fallback) {
            g_LogFile.log(ELogLevel::WARNING, error.str());
            return *fallback;
        } else {
            throw std::runtime_error(error.str());
        }
    }
    }
    assert(0);
}

int GetIntAttribute(const tinyxml2::XMLElement& element, const char* name, int min, int max)
{
    int target;
    auto result = element.QueryIntAttribute(name, &target);
    switch(result) {
    case tinyxml2::XML_SUCCESS:
        if(target < min || target > max) {
            std::stringstream error;
            error << "Value '" << target << "' of attribute '" << name << "' in element ' " << element.Value() << "' is not in valid range [" << min << ", " << max <<"]";
            throw std::runtime_error(error.str());
        }
        return target;
    case tinyxml2::XML_NO_ATTRIBUTE: {
        std::stringstream error;
        error << "Could not find attribute '" << name << "' in element ' " << element.Value() << "'";
        throw std::runtime_error(error.str());
    }
    case tinyxml2::XML_WRONG_ATTRIBUTE_TYPE: {
        std::stringstream error;
        error << "Could not convert attribute '" << name << "' with value '" << element.Attribute(name) << "' to integer in element ' " << element.Value() << "'";
        throw std::runtime_error(error.str());
    }
    }
    assert(0);
}

int GetIntAttribute(const tinyxml2::XMLElement& element, const char* name)
{
    return GetBasicTAttribute<int>(element, name);
}

bool GetBoolAttribute(const tinyxml2::XMLElement& element, const char* name) {
    return GetBasicTAttribute<bool>(element, name);
}

const char* GetStringAttribute(const tinyxml2::XMLElement& element, const char* name)
{
    return GetBasicTAttribute<const char*>(element, name);
}

int GetFallbackIntAttribute(const tinyxml2::XMLElement& element, const char* name, int fallback) noexcept {
    return GetBasicTAttribute(element, name, &fallback);
}

bool GetFallbackBoolAttribute(const tinyxml2::XMLElement& element, const char* name, bool fallback) {
    return GetBasicTAttribute(element, name, &fallback);
}

const char* GetDefaultedStringAttribute(const tinyxml2::XMLElement& element, const char* name, const char* default_value) {
    auto val = element.Attribute(name);
    if(val) return val;
    return default_value;
}