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

    Human = true;
    Catacomb = Arena = YourDaughter = IsDaughter = false;

    //assigning defaults
    for (int i = 0; i < NUM_STATS; i++)
    {
        // `J` When modifying Stats or Skills, search for "J-Change-Stats-Skills"  :  found in >> cGirls.h > sRandomGirl
        switch (i)
        {
        case STAT_HAPPINESS:
        case STAT_HEALTH:
            MinStats[i] = MaxStats[i] = 100;
            break;
        case STAT_TIREDNESS:
        case STAT_FAME:
        case STAT_LEVEL:
        case STAT_EXP:
        case STAT_PCFEAR:
        case STAT_PCLOVE:
        case STAT_PCHATE:
        case STAT_ASKPRICE:
            MinStats[i] = MaxStats[i] = 0;
            break;
        case STAT_AGE:
            MinStats[i] = 17; MaxStats[i] = 25;
            break;
        case STAT_MORALITY:
        case STAT_REFINEMENT:
        case STAT_DIGNITY:
        case STAT_SANITY:
            MinStats[i] = -10; MaxStats[i] = 10;
            break;
        case STAT_LACTATION:
            MinStats[i] = -20; MaxStats[i] = 20;
            break;
        default:
            MinStats[i] = 30; MaxStats[i] = 60;
            break;
        }
    }
    for (int i = 0; i < NUM_SKILLS; i++)// Changed from 10 to NUM_SKILLS so that it will always set the proper number of defaults --PP
    {
        MinSkills[i] = 0;                // Changed from 30 to 0, made no sense for all skills to be a default of 30.
        MaxSkills[i] = 30;
    }
    // now for a few overrides
    MinMoney = 0;
    MaxMoney = 10;
}

sRandomGirl::~sRandomGirl() = default;

sRandomGirl
cRandomGirls::GetRandomGirlSpec(bool Human0Monster1, bool arena, bool daughter, const std::string& findbyname)
{
    // If we do not have any girls to choose from, return a hardcoded "Error Girl"
    if (m_RandomGirls.empty()) {
        sRandomGirl hard_coded;
        hard_coded.Desc = "Hard Coded Random Girl\n(The game did not find a valid .rgirlsx file)";
        hard_coded.Name = "Default";
        hard_coded.Human = (Human0Monster1 == 0);
        hard_coded.Arena = arena;
        hard_coded.YourDaughter = daughter;

        return hard_coded;
    }

    // 1. The most direct check is to try to find a girl by name.
    if (!findbyname.empty())
    {
        auto candidate = find_random_girl_by_name(findbyname);
        if(candidate)
            return *candidate;
    }

    // 2. Next we see if you are looking for your own daughter
    if (daughter &&    m_NumRandomYourDaughterGirls > 0)
    {
        bool monstergirl = Human0Monster1;
        if (m_NumNonHumanRandomYourDaughterGirls < 1)
            monstergirl = false;
        // if there are no monster girls we will accept a human

        int offset = g_Dice % m_RandomGirls.size();    // pick a random stating point
        for(unsigned i = 0; i < m_RandomGirls.size(); ++i) {
            auto& candidate = m_RandomGirls.at((i + offset) % m_RandomGirls.size());
            if (!candidate.YourDaughter)
                continue;

            if ((bool)candidate.Human == !monstergirl) {
                return candidate;
            }
        }
    }

    int offset = g_Dice % m_RandomGirls.size();    // pick a random stating point
    for(unsigned i = 0; i < m_RandomGirls.size(); ++i) {
        auto& candidate = m_RandomGirls.at((i + offset) % m_RandomGirls.size());
        if (Human0Monster1 == (candidate.Human == 0))    {            // test for humanity
            return candidate;
        }
    }

    // if we couldn't find a girl that fits the specs, so we just take a random one and set the flags as we want
    // them. We make a copy here since we modify data.
    auto candidate = m_RandomGirls.at( g_Dice % m_RandomGirls.size() );
    candidate.Human = (Human0Monster1 == 0);
    candidate.Arena = arena;
    candidate.YourDaughter = daughter;
    return candidate;
}


std::shared_ptr<sGirl>
cRandomGirls::CreateRandomGirl(int age, bool slave, bool undead, bool Human0Monster1, bool kidnapped, bool arena,
                               bool your_daughter, bool is_daughter, const std::string& find_by_name)
                         {

    g_LogFile.debug("girls", "cGirls::CreateRandomGirl");
    auto girl_template = GetRandomGirlSpec(Human0Monster1, arena, your_daughter, find_by_name);

    auto newGirl = std::make_shared<sGirl>(false);
    newGirl->m_AccLevel = g_Game->settings().get_integer(settings::USER_ACCOMODATION_FREE);
    newGirl->SetImageFolder(girl_template.ImageDirectory);

    newGirl->m_Desc = girl_template.Desc;
    newGirl->m_Name = girl_template.Name;


    // set all jobs to null
    newGirl->m_DayJob = newGirl->m_NightJob = JOB_UNSET;
    newGirl->m_WorkingDay = newGirl->m_PrevWorkingDay = 0;

    newGirl->m_Money = (g_Dice % (girl_template.MaxMoney - girl_template.MinMoney)) + girl_template.MinMoney;    // money

    // skills
    for (auto skill : SkillsRange) {
        newGirl->set_skill_direct(skill, g_Dice.closed_uniform(girl_template.MinSkills[skill], girl_template.MaxSkills[skill]));
    }
    for (auto skill : SkillsRange) {
        newGirl->upd_skill(skill, 0);
    }

    // stats
    for (auto stat : StatsRange) {
        newGirl->set_stat(stat, g_Dice.closed_uniform(girl_template.MinStats[stat], girl_template.MaxStats[stat]));
    }

    // add the traits
    for (int i = 0; i < girl_template.TraitNames.size(); i++)
    {
        std::string name = girl_template.TraitNames[i];
        // TODO (traits) inherent/permanent traits
        newGirl->gain_trait(name.c_str(), girl_template.TraitChance[i]);
    }

    for (auto& item_candidate : girl_template.Inventory)
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

    if (girl_template.Human == 0)           newGirl->raw_traits().add_inherent_trait("Not Human");
    if (girl_template.YourDaughter == 1)    newGirl->raw_traits().add_inherent_trait("Your Daughter");

    newGirl->set_stat(STAT_FAME, 0);
    if (age != 0)    newGirl->set_stat(STAT_AGE, age);
    newGirl->set_stat(STAT_HEALTH, 100);
    newGirl->set_stat(STAT_HAPPINESS, 100);
    newGirl->set_stat(STAT_TIREDNESS, 0);

    if (kidnapped)    // this girl has been taken against her will so make her rebelious
        {
        newGirl->add_temporary_trait("Kidnapped", std::max(5, g_Dice.bell(0, 25)));        // 5-25 turn temp trait
        int spirit = g_Dice.bell(50, 125);
        int conf = g_Dice.bell(50, 125);
        int obey = g_Dice.bell(-50, 50);
        int hate = g_Dice.bell(0, 100);

        newGirl->set_stat(STAT_SPIRIT, spirit);
        newGirl->set_stat(STAT_CONFIDENCE, conf);
        newGirl->set_stat(STAT_OBEDIENCE,  obey);
        newGirl->set_stat(STAT_PCHATE, hate);
        }

    if (newGirl->age() < 18) newGirl->set_stat(STAT_AGE, 18);    // `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live
    if (g_Dice.percent(5))        newGirl->gain_trait("Former Addict");
    else
    {
        newGirl->gain_trait("Smoker", 5);
        newGirl->gain_trait("Alcoholic", 4);
        newGirl->gain_trait("Fairy Dust Addict", 2);
        newGirl->gain_trait("Shroud Addict", 1);
        newGirl->gain_trait("Viras Blood Addict", 0.5);
    }

    newGirl->set_default_house_percent();

    // If the girl is a slave or arena.. then make her more obedient.
    if (slave || newGirl->is_slave())
    {
        newGirl->set_status(STATUS_SLAVE);
        newGirl->m_AccLevel = g_Game->settings().get_integer(settings::USER_ACCOMODATION_SLAVE);
        newGirl->m_Money = 0;
        newGirl->upd_base_stat(STAT_OBEDIENCE, 20);
    }
    if (arena || newGirl->has_status(STATUS_ARENA))
    {
        newGirl->set_status(STATUS_ARENA);
        newGirl->m_AccLevel = g_Game->settings().get_integer(settings::USER_ACCOMODATION_SLAVE);
        newGirl->m_Money = 0;
        newGirl->upd_base_stat(STAT_OBEDIENCE, 20);
    }
    if (your_daughter || is_your_daughter(*newGirl))    // `J` if she is your daughter...
        {
        newGirl->m_AccLevel = 9;            // pamper her
        newGirl->m_Money = 1000;
        newGirl->house(0);    // your daughter gets to keep all she gets
        newGirl->raw_traits().add_inherent_trait("Your Daughter");
        newGirl->set_stat(STAT_OBEDIENCE, std::max(newGirl->obedience(), 80));    // She starts out obedient
        your_daughter = true;
        }
    if (is_daughter || newGirl->has_status(STATUS_ISDAUGHTER))
    {
        newGirl->set_status(STATUS_ISDAUGHTER);
        newGirl->m_Money = 0;
        newGirl->upd_base_stat(STAT_OBEDIENCE, 20);
    }

    newGirl->raw_traits().update();

    //
    std::string first_name = g_GirlNameList.random();
    std::string surname;
    if (your_daughter) {
        surname = g_Game->player().Surname();    // give her your last name
    } else if (g_Dice.percent(90)) {
        surname = g_SurnameList.random();
    }
    newGirl->SetName(std::move(first_name), "", std::move(surname));

    // load triggers
    for(auto& trigger : girl_template.Triggers) {
        newGirl->m_EventMapping->SetEventHandler(trigger.Event, trigger.Script, trigger.Function);
    }

    // `J` more usefull log for rgirl
    g_LogFile.log(ELogLevel::NOTIFY, "Random girl ", newGirl->FullName(), " created from template ", newGirl->m_Name, ".rgirlsx");

    cGirls::CalculateGirlType(*newGirl);
    return newGirl;
}

void cRandomGirls::LoadRandomGirlXML(const std::string& filename, const std::string& base_path,
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
    void process_trait_xml(sRandomGirl& target, const tinyxml2::XMLElement& el)
    {
        target.TraitNames.emplace_back(GetStringAttribute(el, "Name"));
        target.TraitChance.emplace_back(el.IntAttribute("Percent", 100));
    }

    void process_item_xml(sRandomGirl& target, const tinyxml2::XMLElement& el)
    {
        std::string item_name = GetStringAttribute(el, "Name");
        sInventoryItem *item = g_Game->inventory_manager().GetItem(item_name);
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

    void load_from_xml_v0(sRandomGirl& target, const tinyxml2::XMLElement& el)
    {
        // name and description are easy
        target.Name = GetStringAttribute(el, "Name");
        g_LogFile.log(ELogLevel::NOTIFY, "Loading Legacy Rgirl : ", target.Name);
        target.Desc = GetDefaultedStringAttribute(el, "Desc", "-");
        // DQ - new random type ...
        if (auto pt = el.Attribute("Human")) target.Human = strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0;
        if (auto pt = el.Attribute("Catacomb")) target.Catacomb = strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0;
        if (auto pt = el.Attribute("Arena")) target.Arena = strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0;
        if (auto pt = el.Attribute("Your Daughter")) target.YourDaughter = strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0;
        if (auto pt = el.Attribute("Is Daughter")) target.IsDaughter = strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0;

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
                    process_trait_xml(target, child);
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
        // DQ - new random type ...
        if (auto pt = el.Attribute("Human")) target.Human = strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0;
        if (auto pt = el.Attribute("Catacomb")) target.Catacomb = strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0;
        if (auto pt = el.Attribute("Arena")) target.Arena = strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0;
        if (auto pt = el.Attribute("Your Daughter")) target.YourDaughter = strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0;
        if (auto pt = el.Attribute("Is Daughter")) target.IsDaughter = strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0;

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
                    process_trait_xml(target, child);
                } else if (tag == "Item") {
                    process_item_xml(target, child);
                } else if (tag == "Trigger") {
                    process_trigger_xml(target, child);
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

sRandomGirl* cRandomGirls::find_random_girl_by_name(const std::string& name)
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
            if (girl.YourDaughter)
            {
                m_NumRandomYourDaughterGirls++;
                if (girl.Human)        m_NumHumanRandomYourDaughterGirls++;
                if (!girl.Human)       m_NumNonHumanRandomYourDaughterGirls++;
            }
            else
            {
                if (girl.Human)        m_NumHumanRandomGirls++;
                if (!girl.Human)       m_NumNonHumanRandomGirls++;
            }

        } catch (const std::exception& error) {
            g_LogFile.error("girls", "Could not load rgirl from file '", filename, "': ", error.what());
            if(error_handler)
                error_handler("ERROR: Could not load rgirl from file " + filename + ": " + error.what());
            // remove the invalid girl again
            m_RandomGirls.pop_back();
        }
    }
}
