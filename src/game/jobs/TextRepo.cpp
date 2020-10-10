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

#include "TextRepo.h"
#include "cRng.h"
#include "xml/util.h"
#include "xml/getattr.h"
#include <tinyxml2.h>
#include "utils/streaming_random_selection.hpp"

extern cRng g_Dice;

const std::string& cTextRepository::get_text(const std::string& prompt,
                                             const std::function<bool(const std::string&)>& check) {
    auto& texts = m_Texts.at(prompt);
    if(texts.empty()) {
        throw std::runtime_error("No text available for prompt.");
    }

    // this tracks whether we have found a constrained candidate
    bool constraint = false;

    RandomSelector<const std::string> choice;
    for(auto& candidate : texts) {
        // if we are looking for constrained candidates, we skip all without a condition
        if(constraint && candidate.Condition.empty())
            continue;

        // otherwise, we check those with a condition
        if(!candidate.Condition.empty()) {
            if(!check(candidate.Condition))
                continue;
            // first time that condition was true -- reset
            if(!constraint) {
                choice.reset();
            }
            constraint = true;
        }
        choice.process(&candidate.Text);
    }
    return *choice.selection();
}

void cTextRepository::load(const tinyxml2::XMLElement& root) {
    for(auto& entry : IterateChildElements(root, "Entry")) {
        std::string name = GetStringAttribute(entry, "Name");
        auto result = m_Texts.emplace(name, std::vector<sTextRecord>{});
        for(auto& text : IterateChildElements(entry, "Text")) {
            result.first->second.emplace_back(GetDefaultedStringAttribute(text, "Condition", ""), text.GetText());
        }
    }
}
