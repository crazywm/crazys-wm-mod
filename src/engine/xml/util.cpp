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
#include "CLog.h"
#include "interface/sColor.h"
#include "utils/piecewise_linear.h"
#include "xml/getattr.h"

using namespace xmlutil;

pXMLDocument LoadXMLDocument(const char* file_name) {
    auto doc = std::make_unique<tinyxml2::XMLDocument>();
    if(doc->LoadFile(file_name) != tinyxml2::XML_SUCCESS) {
        g_LogFile.log(ELogLevel::ERROR, "Can't load XML '", file_name, "': ", doc->ErrorStr());
        throw std::runtime_error(doc->ErrorStr());
    }

    g_LogFile.log(ELogLevel::DEBUG, "Loaded XML File ", file_name);

    return doc;
}

void GetColorAttribute(tinyxml2::XMLElement& element, sColor& target)
{
    std::uint8_t r = GetIntAttribute(element, "R", 0, 255);
    std::uint8_t g = GetIntAttribute(element, "G", 0, 255);
    std::uint8_t b = GetIntAttribute(element, "B", 0, 255);
    target = {r, g, b};
}

tinyxml2::XMLElement& PushNewElement(tinyxml2::XMLElement& root, const char* name) {
    auto el = root.GetDocument()->NewElement(name);
    root.InsertEndChild(el);
    return *el;
}

PiecewiseLinearFunction LoadLinearFunction(const XMLElement& source, const char* x_attr, const char* y_attr) {
    auto result = PiecewiseLinearFunction();
    for(auto& pt : IterateChildElements(source, "Point")) {
        float x = GetFloatAttribute(pt, x_attr);
        float y = GetFloatAttribute(pt, y_attr);
        result.add_vertex(x, y);
    }
    return result;
}

tinyxml2::XMLElement* xmlutil::sibling(tinyxml2::XMLElement* el, const char* name) {
    return el->NextSiblingElement(name);
}

const tinyxml2::XMLElement* xmlutil::sibling(const tinyxml2::XMLElement* el, const char* name) {
    return el->NextSiblingElement(name);
}

tChildrenIterator<XMLElement> xmlutil::begin(const sChildRange<XMLElement>& r) {
    return tChildrenIterator<XMLElement>{r.root->FirstChildElement(r.name), r.name};
}

tChildrenIterator<const XMLElement> xmlutil::begin(const sChildRange<const XMLElement>& r) {
    return tChildrenIterator<const XMLElement>{r.root->FirstChildElement(r.name), r.name};
}
