/*
* Copyright 2022, The Pink Petal Development Team.
* The Pink Petal Development Team are defined as the game's coders
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

#include "cScreenInfoTraits.h"
#include "IGame.h"
#include "character/traits/ITraitsManager.h"
#include "character/traits/ITraitSpec.h"
#include "character/traits/ITraitsCollection.h"
#include <algorithm>
#include "utils/string.hpp"
#include "interface/cWindowManager.h"
#include "character/sGirl.h"

namespace {
    const std::string help_text =
            "Traits convey characteristics of a girl not captured by her Stats and Skills.\n\n"
            "A trait can be attached to a girl in three different ways: \n"
            " 1) As an inherent trait. This is a trait she has been born with, and which she will"
            " pass on to her children.\n"
            " 2) As an acquired trait. These are traits that she gained during her life, which will"
            " generally not passed to her children.\n"
            " 3) As a dynamic trait. Such traits stem from temporary status effects or equipped items,"
            " and generally go away again if the item is unequipped or the status stops.\n"
            "In addition, inherent traits can be in a dormant state, which means that the trait has no "
            "effect, but can be passed on to children (i.e. like a recessive gene). Dormant traits are "
            "generally invisible to the player.\n"
            "Not all traits can be active at the same time, e.g. Optimist and Pessimist exclude one another."
            "If a new trait is acquired that excludes an existing inherent trait, than that inherent trait "
            "will become dormant.\n\n"
            "Apart from influencing dialog and events, traits can have alter a girls Stats and Skills, her "
            "Enjoyment of different activities and how attractive she is to customers with different Fetishes, as well "
            "as how much happiness they gain from sex with her. Further, traits can also give a bonus or malus to "
            "a girls performance in a particular job.\n"
            ;
}

cScreenInfoTraits::cScreenInfoTraits() : cGameWindow("info_traits_screen.xml") {

}

void cScreenInfoTraits::set_ids() {
    trait_list_id     = get_id("TraitsList");
    effects_list_id   = get_id("EffectsList");
    description_id    = get_id("Description");
    exclusion_list_id = get_id("ExclusionList");
    property_list_id  = get_id("PropertyList");
    property_expl_id  = get_id("PropExplanation");
    help_text_id      = get_id("HelpText");

    SetListBoxSelectionCallback(trait_list_id, [&](int id){
        load_effects(id);
    });

    SetListBoxSelectionCallback(property_list_id, [&](int id) {
        show_property_explanation(id);
    });
}

void cScreenInfoTraits::init(bool back) {
    cInterfaceWindow::init(back);
    show_property_explanation(-1);

    EditTextItem(help_text, help_text_id);

    int counter = 0;
    m_SortedTraits = g_Game->traits().get_all_traits();
    std::sort(m_SortedTraits.begin(), m_SortedTraits.end());

    auto active_girl = window_manager().GetActiveGirl();

    for(auto& trait: m_SortedTraits) {
        int color = COLOR_NEUTRAL;
        if(active_girl && active_girl->has_active_trait(trait.c_str())) {
            color = COLOR_NEUTRAL2;
        }
        AddToListBox(trait_list_id, counter, g_Game->traits().lookup(trait.c_str())->display_name(), color);
        ++counter;
    }
}

void cScreenInfoTraits::load_effects(int trait_id) {
    std::string trait_name = m_SortedTraits.at(trait_id);
    auto trait_data = g_Game->traits().lookup(trait_name.c_str());
    if(!trait_data) {
        g_Game->error("Could not find trait " + trait_name + " something went very wrong!");
        return;
    }

    EditTextItem(trait_data->desc(), description_id);

    auto collection = g_Game->traits().create_collection();
    collection->add_permanent_trait(trait_data);
    collection->update();

    ClearListBox(effects_list_id);

    for(auto stat : StatsRange) {
        int value = collection->stat_effects()[stat];
        if(value != 0) {
            std::vector<FormattedCellData> data = {mk_text("Stat"), mk_text(get_stat_name(stat)), mk_num(value)};
            AddToListBox(effects_list_id, -1, std::move(data));
        }
    }

    for(auto skill : SkillsRange) {
        int value = collection->skill_effects()[skill];
        if(value != 0) {
            std::vector<FormattedCellData> data = {mk_text("Skill"), mk_text(get_skill_name(skill)), mk_num(value)};
            AddToListBox(effects_list_id, -1, std::move(data));
        }
        value = collection->skill_cap_effects()[skill];
        if(value != 0) {
            std::vector<FormattedCellData> data = {mk_text("Skill Cap"), mk_text(get_skill_name(skill)), mk_num(value)};
            AddToListBox(effects_list_id, -1, std::move(data));
        }
    }

    std::vector<std::pair<std::string, int>> modifiers(collection->get_all_modifiers().begin(), collection->get_all_modifiers().end());
    std::sort(begin(modifiers), end(modifiers));
    for(const auto& mod: modifiers) {
        if(starts_with(mod.first, "fetish:")) {
            Fetishs fetish = get_fetish_id(mod.first.substr(7));
            std::vector<FormattedCellData> data = {mk_text("Fetish"), mk_text(get_fetish_name(fetish)), mk_num(mod.second)};
            AddToListBox(effects_list_id, -1, std::move(data));
        } else if(starts_with(mod.first, "enjoy:")) {
            Action_Types action = get_action_id(mod.first.substr(6));
            std::vector<FormattedCellData> data = {mk_text("Enjoyment"), mk_text(get_action_name(action)), mk_num(mod.second)};
            AddToListBox(effects_list_id, -1, std::move(data));
        } else if(starts_with(mod.first, "sex:")) {
            std::vector<FormattedCellData> data = {mk_text("Sex"), mk_text(mod.first.substr(4)), mk_num(mod.second)};
            AddToListBox(effects_list_id, -1, std::move(data));
        } else if(starts_with(mod.first, "work.")) {
            std::vector<FormattedCellData> data = {mk_text("Job"), mk_text(mod.first.substr(5)), mk_num(mod.second)};
            AddToListBox(effects_list_id, -1, std::move(data));
        }  else if(starts_with(mod.first, "tag:")) {
            std::vector<FormattedCellData> data = {mk_text("Tag"), mk_text(mod.first.substr(4)), mk_num(mod.second)};
            AddToListBox(effects_list_id, -1, std::move(data));
        } else {
            // do not present these for now
            /*std::vector<FormattedCellData> data = {mk_text("?"), mk_text(mod.first), mk_num(mod.second)};
            AddToListBox(effects_list_id, -1, std::move(data));
            */
        }
    }

    ClearListBox(exclusion_list_id);

    /// TODO figure out why we have duplicates here!
    std::unordered_set<std::string> exclusions(trait_data->exclusions().begin(), trait_data->exclusions().end());
    for(auto& exclusion: exclusions) {
        AddToListBox(exclusion_list_id, 0, exclusion);
    }

    auto& props = trait_data->get_properties();
    ClearListBox(property_list_id);

    m_SortedProperties = props.keys();
    std::sort(begin(m_SortedProperties), end(m_SortedProperties));

    for(int i = 0; i < m_SortedProperties.size(); ++i) {
        auto key = m_SortedProperties[i].c_str();
        std::vector<FormattedCellData> data = {mk_text(props.get_display_name(key)),
                                               mk_percent(trait_data->get_properties().get_percent(key).as_percentage())};
        AddToListBox(property_list_id, i, std::move(data));
    }
}

void cScreenInfoTraits::show_property_explanation(int property_id) {
    if(property_id < 0) {
        EditTextItem("Select a property to get a short explanation of its meaning.", property_expl_id);
        return;
    }

    auto prop = m_SortedProperties.at(property_id).c_str();
    auto& props = g_Game->traits().lookup(traits::AIDS)->get_properties();
    EditTextItem(props.get_description(prop), property_expl_id);
}
