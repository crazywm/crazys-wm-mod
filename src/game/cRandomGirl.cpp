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

#include "cRandomGirl.h"
#include "cGirls.h"
#include "CLog.h"
#include "IGame.h"
#include "character/sGirl.h"
#include "cInventory.h"
#include "xml/getattr.h"
#include "xml/util.h"
#include "character/traits/ITraitsCollection.h"
#include "character/predicates.h"
#include "character/cPlayer.h"
#include "cNameList.h"
#include "utils/streaming_random_selection.hpp"

namespace settings {
    extern const char* USER_ACCOMODATION_FREE;
    extern const char* USER_ACCOMODATION_SLAVE;
}

extern cNameList g_GirlNameList;
extern cNameList g_BoysNameList;
extern cNameList g_SurnameList;

sRandomGirl::sRandomGirl()
{
    Name = "";
    Desc = "-";

    SpawnWeights.fill(0);

    //assigning defaults
    for (auto stat: StatsRange)
    {
        switch (stat)
        {
        case STAT_HAPPINESS:
        case STAT_HEALTH:
            MinStats[stat] = MaxStats[stat] = 100;
            break;
        case STAT_TIREDNESS:
        case STAT_FAME:
        case STAT_LEVEL:
        case STAT_EXP:
        case STAT_PCFEAR:
        case STAT_PCLOVE:
        case STAT_ASKPRICE:
            MinStats[stat] = MaxStats[stat] = 0;
            break;
        case STAT_AGE:
            MinStats[stat] = 17; MaxStats[stat] = 25;
            break;
        case STAT_MORALITY:
        case STAT_REFINEMENT:
        case STAT_DIGNITY:
        case STAT_SANITY:
            MinStats[stat] = -10; MaxStats[stat] = 10;
            break;
        case STAT_LACTATION:
            MinStats[stat] = -20; MaxStats[stat] = 20;
            break;
        default:
            MinStats[stat] = 30; MaxStats[stat] = 60;
            break;
        }
    }
    MinSkills.fill(0);
    MaxSkills.fill(30);

    // now for a few overrides
    MinMoney = 0;
    MaxMoney = 10;
}

const sRandomGirl* cRandomGirls::get_spec(SpawnReason reason, int age)
{
    // If we do not have any girls to choose from, return a hardcoded "Error Girl"
    if (m_RandomGirls.empty()) {
        return nullptr;
    }

    RandomSelector<const sRandomGirl> selector;
    int index = static_cast<int>(reason);
    for(const auto& girl : m_RandomGirls) {
        if(girl.SpawnWeights[index] > 0 &&
                // does she fit into the right age bracket?
                (age < 0 || (girl.MinStats[STAT_AGE] <= age && age <= girl.MaxStats[STAT_AGE])) ) {
            selector.process(&girl, float(girl.SpawnWeights[index]));
        }
    }

    return selector.selection();
}

std::shared_ptr<sGirl> cRandomGirls::create_from_template(const sRandomGirl& template_, SpawnReason reason, int age) const {
    auto newGirl = std::make_shared<sGirl>(false);
    newGirl->m_AccLevel = g_Game->settings().get_integer(settings::USER_ACCOMODATION_FREE);
    newGirl->SetImageFolder(template_.ImageDirectory);

    newGirl->m_Desc = template_.Desc;
    newGirl->m_Name = template_.Name;

    // set all jobs to null
    newGirl->m_DayJob = newGirl->m_NightJob = JOB_UNSET;
    newGirl->m_WorkingDay = newGirl->m_PrevWorkingDay = 0;

    newGirl->m_Money = g_Dice.closed_uniform(template_.MinMoney, template_.MaxMoney);

    // skills
    for (auto skill : SkillsRange) {
        newGirl->set_skill_direct(skill, g_Dice.closed_uniform(template_.MinSkills[skill], template_.MaxSkills[skill]));
    }
    for (auto skill : SkillsRange) {
        newGirl->upd_skill(skill, 0);
    }

    // stats
    for (auto stat : StatsRange) {
        newGirl->set_stat(stat, g_Dice.closed_uniform(template_.MinStats[stat], template_.MaxStats[stat]));
    }

    // add the traits
    for (int i = 0; i < template_.TraitNames.size(); i++)
    {
        std::string name = template_.TraitNames[i];
        // TODO (traits) inherent/permanent traits
        newGirl->gain_trait(name.c_str(), template_.TraitChance[i]);
    }

    for (auto& item_candidate : template_.Inventory)
    {
        if (!g_Dice.percent(item_candidate.Chance)) {
            continue;
        }
        newGirl->add_item(item_candidate.Item);
        if (item_candidate.Item->m_Type != sInventoryItem::Food && item_candidate.Item->m_Type != sInventoryItem::Makeup)
        {
            newGirl->equip(item_candidate.Item, false);
        }
    }

    // if a fixed age was requested, ensure that we get it
    if (age >= 18)    newGirl->set_stat(STAT_AGE, age);

    if (newGirl->age() < 18)    newGirl->set_stat(STAT_AGE, 18);    // `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live
    if (g_Dice.percent(5))   newGirl->gain_trait("Former Addict");
    else
    {
        newGirl->gain_trait("Smoker", 5);
        newGirl->gain_trait("Alcoholic", 4);
        newGirl->gain_trait("Fairy Dust Addict", 2);
        newGirl->gain_trait("Shroud Addict", 1);
        newGirl->gain_trait("Viras Blood Addict", 0.5);
    }

    newGirl->set_default_house_percent();

    switch (reason) {
        case SpawnReason::ARENA:
            newGirl->set_status(STATUS_ARENA);
        break;
        case SpawnReason::PLAYER_DAUGHTER:
            newGirl->raw_traits().add_inherent_trait("Your Daughter");
        break;
        case SpawnReason::SLAVE_MARKET:
            newGirl->set_status(STATUS_SLAVE);
            break;
        // this girl has been taken against her will so make her rebellious
        case SpawnReason::KIDNAPPED:
        {
            newGirl->add_temporary_trait("Kidnapped", std::max(5, g_Dice.bell(0, 25)));        // 5-25 turn temp trait
            int spirit = g_Dice.bell(50, 125);
            int conf = g_Dice.bell(50, 125);
            int obey = g_Dice.bell(-50, 50);
            int hate = g_Dice.bell(0, 100);

            newGirl->set_stat(STAT_SPIRIT, spirit);
            newGirl->set_stat(STAT_CONFIDENCE, conf);
            newGirl->set_stat(STAT_OBEDIENCE,  obey);
            newGirl->set_stat(STAT_PCLOVE, -hate);
        }
            break;
        case SpawnReason::MEETING:
        case SpawnReason::STUDIO:
        case SpawnReason::CLINIC:
        case SpawnReason::CATACOMBS:
        case SpawnReason::REWARD:
        case SpawnReason::RECRUITED:
        case SpawnReason::BIRTH:
        case SpawnReason::CUSTOMER:
        case SpawnReason::COUNT:
        break;
    }

    if(newGirl->is_slave()) {
        newGirl->m_AccLevel = g_Game->settings().get_integer(settings::USER_ACCOMODATION_SLAVE);
    } else {
        newGirl->m_AccLevel = g_Game->settings().get_integer(settings::USER_ACCOMODATION_FREE);
    }
    newGirl->raw_traits().update();

    //
    std::string first_name = g_GirlNameList.random();
    std::string surname;
    if (is_your_daughter(*newGirl)) {
        surname = g_Game->player().Surname();    // give her your last name
    } else if (g_Dice.percent(90)) {
        surname = g_SurnameList.random();
    }
    newGirl->SetName(std::move(first_name), "", std::move(surname));

    // load triggers
    for(auto& trigger : template_.Triggers) {
        newGirl->m_EventMapping->SetEventHandler(trigger.Event, trigger.Script, trigger.Function);
    }

    // `J` more usefull log for rgirl
    g_LogFile.log(ELogLevel::NOTIFY, "Random girl ", newGirl->FullName(), " created from template ", newGirl->m_Name, ".rgirlsx");

    cGirls::CalculateGirlType(*newGirl);
    return newGirl;
}

std::shared_ptr<sGirl> cRandomGirls::spawn(SpawnReason reason, int age) {
    // don't allow age < 18
    if(age < 18)
        age = -1;

    g_LogFile.debug("girls", "cGirls::CreateRandomGirl");

    // first, try to find a girl that matches human and age
    auto attempt = get_spec(reason, age);
    if(!attempt) attempt = get_spec(reason);

    auto new_girl = std::make_shared<sGirl>(false);

    // last call -- just accept any girl
    if(!attempt && !m_RandomGirls.empty()) {
        attempt = &m_RandomGirls[g_Dice.random(m_RandomGirls.size())];
        g_LogFile.warning("girl", "Could not find a fitting random girl template! Spawn reason: ", get_spawn_name(reason));
    }

    if(!attempt) {
        new_girl->m_Desc = "Hard Coded Random Girl\n(The game did not find a valid .rgirlsx file)";
        new_girl->SetName("Default", "", "Girl");
        g_Game->error("Could not find a valid random girl template!");
        g_LogFile.error("girl", "Could not find a valid random girl template!");
        return new_girl;
    }

    return create_from_template(*attempt, reason, age);
}

void cRandomGirls::load_from_file(const std::string& filename, const std::string& base_path,
                                  const std::function<void(const std::string&)>& error_handler)
{
    try {
        load_random_girl_imp(filename, base_path, error_handler);
    } catch (const std::exception& error) {
        g_LogFile.error("girls", "Could not load random girl file '", filename, "': ", error.what());
        if(error_handler)
            error_handler("ERROR: Could not load random girl file " + filename + ": " + error.what());
    }
}

namespace {
    void process_trait_xml(sRandomGirl& target, const tinyxml2::XMLElement& el, bool is_v0)
    {
        std::string trait_name = GetStringAttribute(el, "Name");
        if(is_v0 && trait_name == "Dependant") {
            trait_name = "Dependent";
            g_LogFile.warning("traits", "Found misspelled trait `Dependant` for random girl ", target.Name);
        }
        target.TraitNames.emplace_back(trait_name);
        target.TraitChance.emplace_back(el.IntAttribute("Percent", 100));
    }

    void process_item_xml(sRandomGirl& target, const tinyxml2::XMLElement& el)
    {
        std::string item_name = GetStringAttribute(el, "Name");
        const sInventoryItem *item = g_Game->inventory_manager().GetItem(item_name);
        if (!item)
        {
            g_LogFile.log(ELogLevel::ERROR, "Can't find Item: '", item_name, "' - skipping it.");
            return;        // do as much as we can without crashing
        }

        sPercent chance(el.IntAttribute("Percent", 100));
        target.Inventory.push_back(sRandomGirl::sItemRecord{item, chance});
    }

    void process_stat_xml(sRandomGirl& target, const tinyxml2::XMLElement& el, bool is_v0)
    {
        int index;
        std::string stat_name = GetStringAttribute(el, "Name");

        // TODO remove this check at some point
        if(is_v0) {
            if(stat_name == "House") {
                g_LogFile.warning("girl", "Girl ", target.Name, " specified House stat, which is obsolete.");
                return;
            } else if(stat_name == "NPCLove") {
                g_LogFile.warning("girl", "Girl ", target.Name, " specified NPCLove stat, which is obsolete.");
                return;
            } else if(stat_name == "PCHate") {
                g_LogFile.warning("girl", "Girl ", target.Name, " specified PCHate stat, which is obsolete.");
                return;
            }
        }
        try {
            index = get_stat_id(stat_name);
        } catch (const std::out_of_range& e) {
            g_LogFile.error("girls", "Invalid stat name '", stat_name, "' encountered");
            return;        // do as much as we can without crashing
        }

        el.QueryAttribute("Min", &target.MinStats[index]);
        el.QueryAttribute("Max", &target.MaxStats[index]);
    }

    void process_skill_xml(sRandomGirl& target, const tinyxml2::XMLElement& el)
    {
        int index = get_skill_id(GetStringAttribute(el, "Name"));
        el.QueryAttribute("Min", &target.MinSkills[index]);
        el.QueryAttribute("Max", &target.MaxSkills[index]);
    }

    void process_cash_xml(sRandomGirl& target, const tinyxml2::XMLElement& el)
    {
        el.QueryAttribute("Min", &target.MinMoney);
        el.QueryAttribute("Max", &target.MaxMoney);
    }

    void process_trigger_xml(sRandomGirl& target, const tinyxml2::XMLElement& el) {
        std::string event = GetStringAttribute(el, "Name");
        std::string script = GetStringAttribute(el, "Script");
        std::string function = GetStringAttribute(el, "Function");
        target.Triggers.push_back(sRandomGirl::sTriggerData{event, script, function});
    }

    void process_spawn_xml(sRandomGirl& target, const tinyxml2::XMLElement& el) {
        SpawnReason reason = get_spawn_id(GetStringAttribute(el, "Reason"));
        int weight = GetIntAttribute(el, "Weight");
    }

    void load_from_xml_v0(sRandomGirl& target, const tinyxml2::XMLElement& el)
    {
        // name and description are easy
        target.Name = GetStringAttribute(el, "Name");
        g_LogFile.log(ELogLevel::NOTIFY, "Loading Legacy Rgirl : ", target.Name);
        target.Desc = GetDefaultedStringAttribute(el, "Desc", "-");

        auto get_yesno_attr = [&](const char* attribute){
            if (auto pt = el.Attribute(attribute)) {
                return (strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0);
            } else {
                return false;
            }
        };

        auto set_from_attr = [&](const char* attribute, int& target, int true_val=100, int false_val=0) {
            target = get_yesno_attr(attribute) ? true_val : false_val;
        };
        // DQ - new random type ...
        set_from_attr("Catacomb", target.SpawnWeights[static_cast<int>(SpawnReason::CATACOMBS)]);
        set_from_attr("Arena", target.SpawnWeights[static_cast<int>(SpawnReason::ARENA)]);
        // is this even valid xml?
        set_from_attr("Your Daughter", target.SpawnWeights[static_cast<int>(SpawnReason::PLAYER_DAUGHTER)]);

        if(!get_yesno_attr("Human")) {
            target.TraitNames.emplace_back("Not Human");
            target.TraitChance.push_back(100);
        }

        // if no spawn weights are set, make a default of 1 for each except the ones that can be set with this old code
        if(std::all_of(begin(target.SpawnWeights), end(target.SpawnWeights), [](int w){ return w == 0; })) {
            target.SpawnWeights.fill(1);
            target.SpawnWeights[static_cast<int>(SpawnReason::CATACOMBS)] = 0;
            target.SpawnWeights[static_cast<int>(SpawnReason::ARENA)] = 0;
            target.SpawnWeights[static_cast<int>(SpawnReason::PLAYER_DAUGHTER)] = 0;
        }

        // loop through children
        for (auto& child : IterateChildElements(el))
        {
            std::string tag = child.Value();
            try {
                if (tag == "Gold") {
                    process_cash_xml(target, child);
                } else if (tag == "Stat") {
                    process_stat_xml(target, child, true);
                } else if (tag == "Skill") {
                    process_skill_xml(target, child);
                } else if (tag == "Trait") {
                    process_trait_xml(target, child, true);
                } else if (tag == "Item") {
                    process_item_xml(target, child);
                } else {
                    // None of the above? Better ask for help then.
                    g_LogFile.warning("girl", "Unexpected tag: ", tag,
                                      "    don't know what do to, ignoring");
                }
            } catch (const std::exception& ex) {
                g_LogFile.error("girl", "Error when processing element ", tag, " on line ", child.GetLineNum(), ": ",
                                ex.what());
            }
        }
    }

    void load_from_xml_v1(sRandomGirl& target, const tinyxml2::XMLElement& el)
    {
        // name and description are easy
        target.Name = GetStringAttribute(el, "Name");
        g_LogFile.log(ELogLevel::NOTIFY, "Loading random girl : ", target.Name);
        target.Desc = GetDefaultedStringAttribute(el, "Desc", "-");

        // loop through children
        for (auto& child : IterateChildElements(el))
        {
            std::string tag = child.Value();
            try {
                if (tag == "Gold") {
                    process_cash_xml(target, child);
                } else if (tag == "Stat") {
                    process_stat_xml(target, child, false);
                } else if (tag == "Skill") {
                    process_skill_xml(target, child);
                } else if (tag == "Trait") {
                    process_trait_xml(target, child, false);
                } else if (tag == "Item") {
                    process_item_xml(target, child);
                } else if (tag == "Trigger") {
                    process_trigger_xml(target, child);
                } else if (tag == "Spawn") {
                    process_spawn_xml(target, child);
                } else {
                    // None of the above? Better ask for help then.
                    g_LogFile.warning("girl", "Unexpected tag: ", tag,
                                      "    don't know what do to, ignoring");
                }
            } catch (const std::exception& ex) {
                g_LogFile.error("girl", "Error when processing element ", tag, " on line ", child.GetLineNum(), ": ",
                                ex.what());
            }
        }
    }
}

const sRandomGirl* cRandomGirls::get_spec(const std::string& name)
{
    for(auto& rg : m_RandomGirls) {
        if(rg.Name == name) {
            return &rg;
        }
    }
    return nullptr;
}

void cRandomGirls::load_random_girl_imp(const std::string& filename, const std::string& base_path,
                                        const std::function<void(const std::string&)>& error_handler) {
    auto doc = LoadXMLDocument(filename);
    g_LogFile.log(ELogLevel::NOTIFY, "Loading File ::: ", filename);

    auto root_element = doc->RootElement();

    int version = 0;
    root_element->QueryIntAttribute("Version", &version);

    auto load = [&]() -> std::function<void(sRandomGirl&, const tinyxml2::XMLElement&)> {
        switch(version) {
            case 0:
                return [](auto&& girl, auto&& el){
                    load_from_xml_v0(girl, el);
                };
            case 1:
                return [](auto&& girl, auto&& el) {
                    load_from_xml_v1(girl, el);
                };
            default:
                throw std::runtime_error("Invalid version specified in random girl file '" + filename + "': " + std::to_string(version));
        }
    }();

    for (auto& el : IterateChildElements(*root_element))
    {
        m_RandomGirls.emplace_back();
        auto& girl = m_RandomGirls.back();
        try {
            load(girl, el);
            girl.ImageDirectory = DirPath(base_path.c_str()) << girl.Name;
        } catch (const std::exception& error) {
            g_LogFile.error("girls", "Could not load rgirl from file '", filename, "': ", error.what());
            if(error_handler)
                error_handler("ERROR: Could not load rgirl from file " + filename + ": " + error.what());
            // remove the invalid girl again
            m_RandomGirls.pop_back();
        }
    }
}

std::shared_ptr<sGirl> cRandomGirls::spawn(SpawnReason reason, int age, const std::string& name) {
    auto lookup = get_spec(name);
    if(lookup) {
        return create_from_template(*lookup, reason, age);
    }
    return {};
}
