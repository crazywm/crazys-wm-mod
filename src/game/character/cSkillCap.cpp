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

#include "cSkillCap.h"
#include "ICharacter.h"
#include "xml/getattr.h"
#include "xml/util.h"
#include "tinyxml2.h"
#include <iostream>

int cSkillCap::operator()(const ICharacter& character) const {
    int cap = std::numeric_limits<int>::max();
    for(auto& current : m_Caps) {
        int value = 0;
        for(auto& factor : current.Factors) {
            value += factor.FactorInPercent *  character.get_attribute(factor.Source);
        }
        value /= 100;
        value += current.Offset;
        cap = std::min(cap, value);
    }
    return cap;
}

void cSkillCap::load_from_xml(const tinyxml2::XMLElement& element) {
    for(auto& cap : IterateChildElements(element, "Cap")) {
        m_Caps.emplace_back();
        m_Caps.back().Offset = 0;
        cap.QueryIntAttribute("Offset", &m_Caps.back().Offset);
        for(auto& contrib : IterateChildElements(cap, "Factor")) {
            StatSkill attribute = get_stat_skill_id(GetStringAttribute(contrib, "Attribute"));
            int value = GetIntAttribute(contrib, "Factor", 0, std::numeric_limits<int>::max());
            m_Caps.back().Factors.push_back(sFactor{attribute, value});
        }
    }
}

int cSkillCapManager::get_cap(SKILLS target, const ICharacter& character) const {
    int base_cap = m_SkillCaps[target](character);
    int trait_mod = character.get_trait_skill_cap_modifier(target);
    return std::max(0, base_cap + trait_mod);
}

void cSkillCapManager::load_from_xml(const tinyxml2::XMLElement& element) {
    for(auto& cap : IterateChildElements(element, "Skill")) {
        std::string name = GetStringAttribute(cap, "Name");
        m_SkillCaps[get_skill_id(name)].load_from_xml(cap);
    }
}
