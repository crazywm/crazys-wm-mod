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

///! This file contains several low-level utility functions for working with xml
#ifndef CRAZYS_WM_MOD_XML_UTIL_H
#define CRAZYS_WM_MOD_XML_UTIL_H

#include <string>
#include <memory>

namespace tinyxml2
{
    class XMLElement;
    class XMLDocument;
}

class sColor;
class PiecewiseLinearFunction;

using pXMLDocument = std::unique_ptr<tinyxml2::XMLDocument>;

pXMLDocument LoadXMLDocument(const char* file_name) noexcept(false);
inline pXMLDocument LoadXMLDocument(const std::string& file_name) noexcept(false) { return LoadXMLDocument(file_name.c_str()); }

namespace xmlutil
{
    using namespace tinyxml2;

    // helpers that allow to move implementation from .h to .cpp
    XMLElement* sibling(XMLElement* el, const char* name);
    const XMLElement* sibling(const XMLElement* el, const char* name);

    /// This is an iterator that iterates over all child elements of a a given element. Incrementing
    /// It means switching to the next sibling element. Templated to allow both const and non-const
    /// usage.
    template<class T>
    class tChildrenIterator : public std::iterator<std::input_iterator_tag, T> {
    public:
        explicit tChildrenIterator(T* ptr, const char* name=nullptr) : m_Element(ptr), m_ElementName(name) { }

        T& operator*() { return *m_Element; }
        T* operator->() { return m_Element; }

        tChildrenIterator& operator++() {
            m_Element = sibling(m_Element, m_ElementName);
            return *this;
        }

        tChildrenIterator operator++(int)
        {
            tChildrenIterator tmp(*this); operator++(); return tmp;
        }

        bool operator==(const tChildrenIterator& rhs) { return m_Element == rhs.m_Element; }
        bool operator!=(const tChildrenIterator& rhs) { return m_Element != rhs.m_Element; }

    private:
        T* m_Element;
        const char* m_ElementName;
    };

    template<class Element>
    struct sChildRange {
        Element *root;
        const char *name;
    };

    tChildrenIterator<XMLElement> begin(const sChildRange<XMLElement>& r);
    tChildrenIterator<const XMLElement> begin(const sChildRange<const XMLElement>& r);

    template<class T>
    auto end(const sChildRange<T>&) {
        return tChildrenIterator<T>{nullptr, nullptr};
    }

    using sConstChildRange = sChildRange<const tinyxml2::XMLElement>;
    using sMutableChildRange = sChildRange<tinyxml2::XMLElement>;
}

inline xmlutil::sMutableChildRange IterateChildElements(tinyxml2::XMLElement& element, const char* name = nullptr) {
    return {&element, name};
}

inline xmlutil::sConstChildRange IterateChildElements(const tinyxml2::XMLElement& element, const char* name = nullptr) {
    return {&element, name};
}

tinyxml2::XMLElement& PushNewElement(tinyxml2::XMLElement& root, const char* name);
inline tinyxml2::XMLElement& PushNewElement(tinyxml2::XMLElement& root, const std::string& name) { return PushNewElement(root, name.c_str()); }

void GetColorAttribute(tinyxml2::XMLElement& element, sColor& target);
PiecewiseLinearFunction LoadLinearFunction(const tinyxml2::XMLElement& source, const char* x_attr, const char* y_attr);

#endif //CRAZYS_WM_MOD_XML_UTIL_H
