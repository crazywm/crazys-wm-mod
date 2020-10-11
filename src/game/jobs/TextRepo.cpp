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
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include "utils/streaming_random_selection.hpp"

namespace {
    std::string empty_string;
}

const std::string& cTextRepository::get_text(const std::string& prompt,
                                             const std::function<bool(const std::string&)>& check) {
    auto& texts = m_Texts.at(prompt);
    if(texts.empty()) {
        throw std::runtime_error("No text available for prompt.");
    }

    // this tracks whether we have found a constrained candidate
    int priority = 0;

    RandomSelector<const std::string> choice;
    for(auto& candidate : texts) {
        // skip all candidates with lower priority than what we have already found
        if(candidate.Priority < priority)
            continue;

        // skip randomly disabled candidates
        if(candidate.Chance < 100 && !g_Dice.percent(candidate.Chance))
            continue;

        // otherwise, we check those with a condition
        if(!candidate.Conditions.empty()) {
            bool passed = false;
            for(auto& cond : candidate.Conditions) {
                passed |= check(cond);
            }
            if(!passed)
                continue;
        }

        // if we've found a new element with higher priority, reset the selection process
        if(candidate.Priority > priority)
            choice.reset();

        choice.process(&candidate.Text);
    }

    if(choice.selection())
        return *choice.selection();
    return empty_string;
}

namespace {
    std::vector<std::string> parse_condition(const char* source) {
        if(!source)
            return {};
        std::vector<std::string> conditions;
        boost::split(conditions, source, [](const char c){ return c == '|';});
        return conditions;
    };
}

void cTextRepository::load(const tinyxml2::XMLElement& root) {
    for(auto& entry : IterateChildElements(root, "Message")) {
        std::string name = GetStringAttribute(entry, "Name");
        auto result = m_Texts.emplace(name, std::vector<sTextRecord>{});
        for(auto& text : IterateChildElements(entry, "Text")) {
            int priority = text.IntAttribute("Priority", -1);
            if(priority == -1) {
                priority = text.Attribute("Condition") ? 1 : 0;
            }
            int chance = text.IntAttribute("Chance", 100);
            std::string content = text.GetText();
            // TODO this does not convert \n -> ' '
            boost::algorithm::trim_all(content);
            result.first->second.emplace_back(std::move(content),
                                              parse_condition(text.Attribute("Condition")),
                                              priority, chance);
        }
    }
}
