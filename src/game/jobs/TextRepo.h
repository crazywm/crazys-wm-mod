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

#ifndef WM_TEXTREPO_H
#define WM_TEXTREPO_H

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace tinyxml2 {
    class XMLElement;
}

struct sTextRecord {
    sTextRecord(std::string text, std::vector<std::string> conds, int priority, int chance) :
            Conditions(std::move(conds)), Text(std::move(text)), Priority(priority), Chance(chance) {}
    std::vector<std::string> Conditions;
    std::string Text;
    int         Priority;
    int         Chance;
};

class cTextRepository {
public:
    void load(const tinyxml2::XMLElement& root);
    const std::string& get_text(const std::string& prompt, const std::function<bool(const std::string&)>& check);
private:
    std::unordered_map<std::string, std::vector<sTextRecord>> m_Texts;
};

#endif //WM_TEXTREPO_H
