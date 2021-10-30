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

#ifndef WM_CSKILLCAP_H
#define WM_CSKILLCAP_H

#include <vector>
#include "Constants.h"
#include <boost/variant/variant.hpp>

class ICharacter;

namespace tinyxml2 {
    class XMLElement;
}

using StatSkill = boost::variant<STATS, SKILLS>;

class cSkillCap {
public:
    int operator()(const ICharacter& character) const;
    void load_from_xml(const tinyxml2::XMLElement& element);
private:
    struct sFactor {
        StatSkill Source;
        int FactorInPercent;
    };

    struct sSingleCap {
        int Offset;
        std::vector<sFactor> Factors;
    };
    std::vector<sSingleCap> m_Caps;
};

class cSkillCapManager {
public:
    cSkillCapManager() = default;
    void load_from_xml(const tinyxml2::XMLElement& element);
    int get_cap(SKILLS target, const ICharacter& character) const;
private:
    std::array<cSkillCap, NUM_SKILLS> m_SkillCaps;
};

#endif //WM_CSKILLCAP_H
