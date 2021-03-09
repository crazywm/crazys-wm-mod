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

#include "data.h"
#include <cassert>
#include "xml/getattr.h"
#include "xml/util.h"
#include "tinyxml2.h"
#include "utils/string.hpp"
#include "utils/lookup.h"

namespace {
    SceneCategory scene_category_from_string(const std::string& name) {
        if (iequals(name, "TEASE")) {
            return SceneCategory::TEASE;
        } else if (iequals(name, "SOFT")) {
            return SceneCategory::SOFT;
        } else if (iequals(name, "HARD")) {
            return SceneCategory::HARD;
        } else if (iequals(name, "EXTREME")) {
            return SceneCategory::EXTREME;
        }
        assert(false);
    }

    const std::array<const char*, (int)SceneType::COUNT>& get_scene_type_names() {
        static std::array<const char*, (int)SceneType::COUNT> names {
            "Action", "Chef", "Music", "Tease",
            "Titty", "Strip", "Hand", "Foot", "Mast",
            "Oral", "Anal", "Sex", "Les", "Group",
            "Beast", "BDSM", "Bukkake", "Public BDSM", "Throat"
        };
        return names;
    }

    const id_lookup_t<SceneType>& get_scene_type_lookup() {
        static auto lookup = create_lookup_table<SceneType>(get_scene_type_names());
        return lookup;
    }

    std::array<const char*, (int)SceneCategory::NUM_TYPES> CategoryNames = {
            "Tease", "Soft", "Hard", "Extreme"
    };

    struct sSceneTypeData {
        SceneCategory Category;
        int FluffNeeded;            // Number of Fluffer points needed
        int StageHandNeeded;        // Amount of StageHand points needed for the scene
    };

    std::array<sSceneTypeData, (int)SceneType::COUNT> TypeToCategory = {
            sSceneTypeData{SceneCategory::TEASE, 0, 10},                  // Action
            {SceneCategory::TEASE, 0, 10},                  // Chef
            {SceneCategory::TEASE, 0, 10},                  // Music
            {SceneCategory::TEASE, 0, 5},                  // Tease
            {SceneCategory::SOFT, 5, 5},                   // Titty
            {SceneCategory::SOFT, 0, 10},                   // Strip
            {SceneCategory::SOFT, 5, 5},                   // Hand
            {SceneCategory::SOFT, 5, 5},                   // Foot
            {SceneCategory::SOFT, 0, 5},                   // Mast
            {SceneCategory::HARD, 5, 5},                   // Oral
            {SceneCategory::HARD, 5, 5},                   // Anal
            {SceneCategory::HARD, 5, 5},                   // Sex
            {SceneCategory::HARD, 0, 5},                   // Les
            {SceneCategory::HARD, 15, 10},                  // Group
            {SceneCategory::EXTREME, 10, 15},               // Beast
            {SceneCategory::EXTREME, 5, 10},                // BDSM
            {SceneCategory::EXTREME, 20, 10},               // Bukkake
            {SceneCategory::EXTREME, 0, 10},                // Public BDSM
            {SceneCategory::EXTREME, 5, 5},                // Throat
    };
}

SceneCategory get_category(SceneType type) {
    return TypeToCategory.at(int(type)).Category;
}

int get_fluffer_required(SceneType type) {
    return TypeToCategory.at(int(type)).FluffNeeded;
}

int get_stage_points_required(SceneType type) {
    return TypeToCategory.at(int(type)).StageHandNeeded;
}

SceneType scene_type_from_string(const std::string& name) {
    return lookup_with_error(get_scene_type_lookup(), name, "Trying to get invalid Scene Type");
}

const char* get_name(SceneType type) {
    return get_scene_type_names().at((int)type);
}

void sTargetGroup::load_xml(const tinyxml2::XMLElement& root) {
    Name = GetStringAttribute(root, "Name");
    Amount = GetIntAttribute(root, "Amount");
    ForcedIsTurnOff = GetFloatAttribute(root, "ForcedIsTurnOff");
    RequiredScore = GetFloatAttribute(root, "RequiredScore");
    SpendingPower = GetFloatAttribute(root, "SpendingPower");
    Saturation = GetIntAttribute(root, "Saturation");
    Knowledge = GetIntAttribute(root, "Knowledge");

    TurnOffs.fill(0.f);
    for(const auto& turnoff : IterateChildElements(root, "TurnOff")) {
        float val = GetFloatAttribute(turnoff, "Value");
        SceneCategory cat = scene_category_from_string(GetStringAttribute(turnoff, "Category"));
        TurnOffs[(int)cat] = val;
    }

    Desires.fill(1.f);
    for(const auto& desire : IterateChildElements(root, "Desire")) {
        float val = GetFloatAttribute(desire, "Value");
        SceneCategory cat = scene_category_from_string(GetStringAttribute(desire, "Category"));
        Desires[(int)cat] = val;
    }

    Favourite = scene_type_from_string(GetStringAttribute(root, "Favourite"));
}

void sTargetGroup::save_xml(tinyxml2::XMLElement& target) const {
    target.SetAttribute("Name", Name.c_str());
    target.SetAttribute("Amount", Amount);
    target.SetAttribute("ForcedIsTurnOff", ForcedIsTurnOff);
    target.SetAttribute("RequiredScore", RequiredScore);
    target.SetAttribute("SpendingPower", SpendingPower);
    target.SetAttribute("Saturation", Saturation);
    target.SetAttribute("Knowledge", Knowledge);
    for(int i = 0; i < TurnOffs.size(); ++i) {
        auto& elem = PushNewElement(target, "TurnOff");
        elem.SetAttribute("Category", CategoryNames[i]);
        elem.SetAttribute("Value", TurnOffs[i]);
    }

    for(int i = 0; i < Desires.size(); ++i) {
        auto& elem = PushNewElement(target, "Desire");
        elem.SetAttribute("Category", CategoryNames[i]);
        elem.SetAttribute("Value", Desires[i]);
    }

    target.SetAttribute("Favourite", get_scene_type_names()[(int)Favourite]);
}

FormattedCellData sTargetGroup::get_formatted(const std::string& name, bool all) const {
    int lookup_desires = -1;
    if(name == "Group") {
        return mk_text(Name);
    } else if(name == "Size") {
        if(all || Knowledge >= KnowledgeForSize) {
            return mk_num(Amount);
        } else {
            return {0, "???"};
        }
    } else if(name == "Saturation") {
        if(all || Knowledge >= KnowledgeForSaturation) {
            return mk_percent(100 * Saturation / Amount);
        } else {
            return {0, "???"};
        }
    } else if(name == "FavScene") {
        if(all || Knowledge >= KnowledgeForFavScene) {
            return mk_text(get_name(Favourite));
        }else {
            return mk_text("???");
        }
    } else if(name == "Score") {
        if(all || Knowledge >= KnowledgeForReqScore) {
            return mk_num((int) RequiredScore);
        } else {
            return {0, "???"};
        }
    } else if(name == "SpendingPower") {
        if(all || Knowledge >= KnowledgeForSpendingPower) {
            return mk_num((int)SpendingPower);
        } else {
            return {0, "???"};
        }
    } else if(name == "Desire_Tease") {
        lookup_desires = 0;
    } else if(name == "Desire_Soft") {
        lookup_desires = 1;
    } else if(name == "Desire_Hard") {
        lookup_desires = 2;
    } else if(name == "Desire_Extreme") {
        lookup_desires = 3;
    }
    assert(lookup_desires >= 0);

    if(all || Knowledge >= KnowledgeForDesires) {
        return mk_num(Desires[lookup_desires] * 100);
    } else {
        return {0, "???"};
    }
}

void MovieScene::load_xml(const tinyxml2::XMLElement& root) {
    // TODO Category is derived from Type! we shouldn't store and load it
    Category = scene_category_from_string(GetStringAttribute(root, "Category"));
    Type = scene_type_from_string(GetStringAttribute(root, "Type"));
    ContentQuality = GetIntAttribute(root, "ContentQuality");
    TechnicalQuality = GetIntAttribute(root, "TechnicalQuality");
    WasForced = GetBoolAttribute(root, "WasForced");

    Actress = GetStringAttribute(root, "Actress");
    Director = GetStringAttribute(root, "Director");
    CameraMage = GetStringAttribute(root, "CameraMage");
    CrystalPurifier = GetStringAttribute(root, "CrystalPurifier");

    Budget = GetIntAttribute(root, "Budget");
}

void MovieScene::save_xml(tinyxml2::XMLElement& target) const {
    target.SetAttribute("Category", CategoryNames[int(Category)]);
    target.SetAttribute("Type", get_scene_type_names()[(int)Type]);
    target.SetAttribute("ContentQuality", ContentQuality);
    target.SetAttribute("TechnicalQuality", TechnicalQuality);
    target.SetAttribute("WasForced", WasForced);

    target.SetAttribute("Actress", Actress.c_str());
    target.SetAttribute("Director", Director.c_str());
    target.SetAttribute("CameraMage", CameraMage.c_str());
    target.SetAttribute("CrystalPurifier", CrystalPurifier.c_str());

    target.SetAttribute("Budget", Budget);
}

namespace {
    void load_minmax(int& min, int& max, const tinyxml2::XMLElement& root, const char* element) {
        auto* el = root.FirstChildElement(element);
        if(!el) {
            throw std::runtime_error("Could not find element");
        }

        min = GetIntAttribute(*el, "Min");
        max = GetIntAttribute(*el, "Max");
    }
}

void sTargetGroupSpec::load_xml(const tinyxml2::XMLElement& root) {
    Name = GetStringAttribute(root, "Name");
    load_minmax(MinAmount, MaxAmount, root, "Amount");
    load_minmax(SpendingPowerMin, SpendingPowerMax, root, "SpendingPower");
    load_minmax(RequiredScoreMin, RequiredScoreMax, root, "RequiredScore");

    ForcedIsTurnOff = GetFloatAttribute(root, "ForcedIsTurnOff");

    TurnOffs.fill(-1.f);
    for(const auto& turnoff : IterateChildElements(root, "TurnOff")) {
        float val = GetFloatAttribute(turnoff, "Value");
        SceneCategory cat = scene_category_from_string(GetStringAttribute(turnoff, "Category"));
        TurnOffs[(int)cat] = val;
    }

    DesiresMin.fill(0.f);
    DesiresMax.fill(1.f);
    for(const auto& turnoff : IterateChildElements(root, "Desire")) {
        float min = GetFloatAttribute(turnoff, "Min");
        float max = GetFloatAttribute(turnoff, "Max");
        SceneCategory cat = scene_category_from_string(GetStringAttribute(turnoff, "Category"));
        DesiresMin[(int)cat] = min;
        DesiresMax[(int)cat] = max;
    }

    if(root.FirstChildElement("Favourite") == nullptr) {
        for(int i = 0; i < (int)SceneType::COUNT; ++i) {
            Favourites.push_back(static_cast<SceneType>(i));
        }
    } else {
        for (const auto& fav : IterateChildElements(root, "Favourite")) {
            Favourites.push_back((SceneType) scene_type_from_string(fav.GetText()));
        }
    }
}

void Movie::save_xml(tinyxml2::XMLElement& target) const {
    for(auto& scene: Scenes) {
        auto& elem = PushNewElement(target, "Scene");
        scene.save_xml(elem);
    }
    target.SetAttribute("Name", Name.c_str());
    target.SetAttribute("TicketPrice", TicketPrice);
    target.SetAttribute("Hype", Hype);
    target.SetAttribute("Age", Age);
    target.SetAttribute("TotalEarnings", TotalEarnings);
    target.SetAttribute("EarningsLastWeek", EarningsLastWeek);
    target.SetAttribute("TotalCost", TotalCost);
}

void Movie::load_xml(const tinyxml2::XMLElement& root) {
    for(auto& elem : IterateChildElements(root, "Scene")) {
        Scenes.emplace_back();
        Scenes.back().load_xml(elem);
    }

    Name = GetStringAttribute(root, "Name");
    TicketPrice = GetIntAttribute(root, "TicketPrice");
    Hype = GetFloatAttribute(root, "Hype");
    Age = GetIntAttribute(root, "Age");
    TotalEarnings = GetIntAttribute(root, "TotalEarnings");
    EarningsLastWeek = GetIntAttribute(root, "EarningsLastWeek");
    TotalCost = GetIntAttribute(root, "TotalCost");
}
