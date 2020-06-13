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

#ifndef WM_CGAMESETTINGS_H
#define WM_CGAMESETTINGS_H

#include <boost/variant.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#include "utils/cKeyValueStore.h"


struct sKeyValueEntry {
    const char* tag;
    const char* name;
    const char* description;
    settings_value_t value;
};

/*
 * A specialized key-value store that attaches a human-readable name and description to each entry.
 */
class cGameSettings : public cKeyValueBase {
public:
    cGameSettings();
    const settings_value_t & get_value(const char* name) const override;

    // list all settings
    std::vector<const sKeyValueEntry*> list_all_settings() const;

    // io
    void save_xml(tinyxml2::XMLElement& target) const override;
private:
    settings_value_t & get_value(const char* name) override;
    void add_setting(const char* tag, const char* name, const char* desc, settings_value_t default_value);
    std::unordered_map<std::string, sKeyValueEntry> m_Settings;
};

#endif //WM_CGAMESETTINGS_H
