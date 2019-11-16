/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders
 * who meet on http://pinkpetal.org     // old site: http://pinkpetal .co.cc
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

#ifndef CRAZYS_WM_MOD_GETATTR_H
#define CRAZYS_WM_MOD_GETATTR_H

namespace tinyxml2
{
	class XMLElement;
	class XMLDocument;
}

/*
 * Get*Attribute functions: They each take as first argument a reference to the XML Element from which to get the
 * attribute, as second argument the attribute name. If the attribute is not found, or cannot be converted to the
 * target type, a runtime_error is thrown.
 */

int GetIntAttribute(const tinyxml2::XMLElement& element, const char* name);
bool GetBoolAttribute(const tinyxml2::XMLElement& element, const char* name);
int GetIntAttribute(const tinyxml2::XMLElement& element, const char* name, int min, int max);
const char* GetStringAttribute(const tinyxml2::XMLElement& element, const char* name);

/*
 * GetFallback*Attribute functions: Like Get*Attribute, but take an additional parameter for the default value. The
 * intention of these methods is to be used with required values, whose absence should issue a warning.
 */
int GetFallbackIntAttribute(const tinyxml2::XMLElement& element, const char* name, int fallback) noexcept;
bool GetFallbackBoolAttribute(const tinyxml2::XMLElement& element, const char* name, bool fallback) noexcept;

// For strings, there is no convenient way of getting with default (that I know of)
const char* GetDefaultedStringAttribute(const tinyxml2::XMLElement& element, const char* name, const char* default_value);


#endif //CRAZYS_WM_MOD_GETATTR_H
