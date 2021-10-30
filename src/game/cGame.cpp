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

#include "cGame.h"
#include "CLog.h"
#include "sConfig.h"
#include "cJobManager.h"
#include "cGold.h"
#include "cShop.h"
#include "cGangs.h"
#include "cGangManager.hpp"
#include "cRival.h"
#include "cInventory.h"
#include "cObjectiveManager.hpp"
#include "sStorage.h"
#include "buildings/cDungeon.h"
#include "buildings/cBuildingManager.h"
#include "character/cPlayer.h"
#include "character/cCustomers.h"
#include "character/traits/ITraitsManager.h"
#include "buildings/studio/manager.h"
#include "scripting/cScriptManager.h"
#include "utils/DirPath.h"
#include "utils/FileList.h"
#include "xml/util.h"

#include "interface/fwd.hpp"
#include "interface/cWindowManager.h"

#include <chrono>
#include "character/cSkillCap.h"

extern cConfig cfg;

namespace settings {
    extern const char* TAXES_RATE;
    extern const char* TAXES_MINIMUM;
    extern const char* TAXES_LAUNDRY;
    extern const char* SLAVE_MARKET_MIN;
    extern const char* SLAVE_MARKET_MAX;
    extern const char* SLAVE_MARKET_UNIQUE_CHANCE;
    extern const char* SLAVE_MARKET_TURNOVER_RATE;
    extern const char* PLAYER_TALK_DEFAULT;
}

struct cGame::sScriptEventStack {
    scripting::sAsyncScriptHandle ActiveScript;
    std::deque<scripting::sEventID> EventStack;
};

cGame::cGame() :
        m_EventStack( std::make_unique<sScriptEventStack>() ) {
    // load the default event mapping
    auto path = DirPath() << "Resources" << "Scripts" << "DefaultEvents.xml";
    script_manager().LoadEventMapping(*script_manager().GetGlobalEventMapping(), path.str());

    // and default girl events
    path = DirPath() << "Resources" << "Scripts" << "DefaultGirl.xml";
    auto ge = script_manager().CreateEventMapping("DefaultGirl", "");
    script_manager().LoadEventMapping(*ge, path.str());
    script_manager().RegisterEventMapping(std::move(ge));
}

std::unique_ptr<IGame> IGame::CreateGame() {
    return std::make_unique<cGame>();
}

// ---------------------------------------------------------------------------------------------------------------------
//                                              Saving and Loading
// ---------------------------------------------------------------------------------------------------------------------
void cGame::NewGame(const std::function<void(std::string)>& callback) {
    g_LogFile.info("prepare", "Loading Game Data");
    // setup gold
    gold().reset();

    // jobs
    g_LogFile.info("prepare", "Setup Jobs");
    job_manager().Setup();

    g_LogFile.info("prepare", "Resetting Player");
    for(int i = 0; i < NUM_STATS; ++i) player().set_stat(i, 60);
    for(auto skill : SkillsRange) player().set_skill_direct(skill, 10);
    player().SetToZero();

    // traits
    callback("Loading Traits");
    g_LogFile.info("prepare", "Loading Traits");
    LoadTraitFiles(DirPath() << "Resources" << "Data" << "Traits");

    // load skills
    callback("Loading Skills");
    g_LogFile.info("prepare", "Loading Skills");
    DirPath caps;
    caps << "Resources" << "Data" << "Skills.xml";
    m_SkillCaps->load_from_xml(*LoadXMLDocument(caps.c_str())->RootElement());

    callback("Loading Items");
    g_LogFile.info("prepare", "Loading Items");
    for(const auto& path : DirPath::split_search_path(cfg.items()))
        LoadItemFiles(DirPath::expand_path(path).c_str());

    callback("Loading Girls");
    g_LogFile.info("prepare", "Loading Girl Files");
    for(const auto& path : DirPath::split_search_path(cfg.characters()))
        LoadGirlFiles(DirPath::expand_path(path).c_str(), callback);

    g_LogFile.info("prepare", "Update Shop");
    shop().RestockShop();

    callback("Update Slave Market");
    g_LogFile.info("prepare", "Update Slave Market");
    UpdateMarketSlaves();

    m_TalkCount = settings().get_integer(settings::PLAYER_TALK_DEFAULT);
    m_WalkAround = false;
}

void cGame::LoadGame(const tinyxml2::XMLElement& source, const std::function<void(std::string)>& callback) {
    g_LogFile.info("prepare", "Loading Game Data");

    // jobs
    g_LogFile.info("prepare", "Setup Jobs");
    job_manager().Setup();

    // traits
    callback("Loading Traits");
    g_LogFile.info("prepare", "Loading Traits");
    LoadTraitFiles(DirPath() << "Resources" << "Data" << "Traits");

    // load skills
    callback("Loading Skills");
    g_LogFile.info("prepare", "Loading Skills");
    DirPath caps;
    caps << "Resources" << "Data" << "Skills.xml";
    m_SkillCaps->load_from_xml(*LoadXMLDocument(caps.c_str())->RootElement());

    ReadGameAttributesXML(source);

    callback("Loading Items");
    auto start_time_items = std::chrono::steady_clock::now();
    g_LogFile.info("prepare", "Loading Items");
    for(const auto& path : DirPath::split_search_path(cfg.items()))
        LoadItemFiles(DirPath::expand_path(path).c_str());
    int duration = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::steady_clock::now() - start_time_items ).count();
    g_LogFile.info("prepare", "Loaded items in ", duration, "ms");
    callback("Loaded items in " + std::to_string(duration) + "ms");

    // girl file list
    auto start_time_girls = std::chrono::steady_clock::now();
    g_LogFile.info("prepare", "Loading Girl List");
    auto gf = source.FirstChildElement("GirlFiles");
    if(gf) {
        for(auto& file : IterateChildElements(*gf, "File")) {
            m_GirlFiles.insert(file.GetText());
        }
    }

    callback("Loading Girl Files");
    g_LogFile.info("prepare", "Loading Girl Files");
    for(const auto& path : DirPath::split_search_path(cfg.characters()))
        LoadGirlFiles(DirPath::expand_path(path).c_str(), callback);

    duration = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::steady_clock::now() - start_time_girls ).count();
    g_LogFile.info("prepare", "Loaded girls in ", duration, "ms");
    callback("Loaded girls in " + std::to_string(duration) + "ms");

    g_LogFile.log(ELogLevel::NOTIFY, "Loading Rivals");
    callback("Loading Rivals");
    /// TODO fix error handling;
    rivals().LoadRivalsXML(source.FirstChildElement("Rival_Manager"));

    gold().loadGoldXML(source.FirstChildElement("Gold"));            // load player gold

    source.QueryAttribute("Year", &m_Date.year);
    source.QueryAttribute("Month", &m_Date.month);
    source.QueryAttribute("Day", &m_Date.day);

    auto storage_el = source.FirstChildElement("Storage");
    if(storage_el) {
        storage().load_from_xml(*storage_el);
    }

    callback("Loading Player");
    g_LogFile.log(ELogLevel::NOTIFY, "Loading Player");
    player().LoadPlayerXML(source.FirstChildElement("Player"));

    g_LogFile.log(ELogLevel::NOTIFY, "Loading Dungeon");
    dungeon().LoadDungeonDataXML(source.FirstChildElement("Dungeon"));

    // initialize new market
    auto* slave_market = source.FirstChildElement("SlaveMarket");
    if(slave_market) {
        GetSlaveMarket().LoadXML(*slave_market);
    }

    // load the buildings!
    g_LogFile.log(ELogLevel::NOTIFY, "Loading Buildings");
    callback("Loading Buildings");
    buildings().LoadXML(source);

    // load the settings
    auto setting_el = source.FirstChildElement("Settings");
    if(setting_el)
        settings().load_xml(*setting_el);

    callback("Loading Girls.");
    g_LogFile.log(ELogLevel::NOTIFY, "Loading Girls");
    girl_pool().LoadGirlsXML(source.FirstChildElement("Girls"));

    callback("Loading Gangs.");
    g_LogFile.log(ELogLevel::NOTIFY, "Loading Gangs");
    gang_manager().LoadGangsXML(source.FirstChildElement("Gang_Manager"));

    auto* mvel = source.FirstChildElement("Movies");
    if(mvel) {
        movie_manager().load_xml(*mvel);
    } else {
        g_LogFile.log(ELogLevel::WARNING, "Could not find <Movies> element in save game");
    }

    // TODO save the shop, and load items again here
    shop().RestockShop();
}


void cGame::LoadGirlFiles(const DirPath& location,
                          const std::function<void(const std::string&)>& error_handler) {
    g_LogFile.debug("girls", "Loading girl files from ", location.c_str());
    // first, load unique girls. Process only those that are not
    // already present in the current game.
    FileList girlfiles(location, "*.girlsx");
    for (int i = 0; i < girlfiles.size(); i++)
    {
        if (m_GirlFiles.count(girlfiles[i].leaf()) > 0)
        {
            continue;
        }
        m_GirlFiles.insert(girlfiles[i].leaf());
        try {
            girl_pool().LoadGirlsXML(girlfiles[i].full(), location.str(), error_handler);
        } catch (const std::runtime_error& error) {
            error_handler(std::string("ERROR: Could not read girls in file '") + girlfiles[i].full() + "': " + error.what() );
        }
    }

    // load all random girls.
    FileList rgirlfiles(location, "*.rgirlsx");
    for (int i = 0; i < rgirlfiles.size(); i++)
    {
        try {
            girl_pool().LoadRandomGirl(rgirlfiles[i].full(), location.str(), error_handler);
        } catch (const std::runtime_error& error) {
            error_handler(std::string("ERROR: Could not read girls in file '") + rgirlfiles[i].full() + "': " + error.what() );
        }
    }

    // and recurse into subdirectories
    auto subdirs = FileList::ListSubdirs(location.str());
    for(auto& dir : subdirs) {
        LoadGirlFiles(DirPath(location) << dir.c_str(), error_handler);
    }
}

void cGame::LoadTraitFiles(DirPath location) {
    FileList fl_t(location, "*.xml");                // get a file list
    if (fl_t.size() > 0)
    {
        for (int i = 0; i < fl_t.size(); i++)                // loop over the list, loading the files
        {
            g_LogFile.info("traits", "Loading traits from file '", fl_t[i].full(), '\'');
            traits().load_xml(*LoadXMLDocument(fl_t[i].full())->RootElement());
        }
    } else {
        g_LogFile.error("traits", "Could not find any trait files in '", location.c_str(), '\'');
    }
}


void cGame::LoadItemFiles(DirPath location) {
    FileList fl(location, "*.itemsx");
    g_LogFile.log(ELogLevel::NOTIFY, "Found ", fl.size(), " itemsx files");
    fl.scan("*.itemsx");
    // Iterate over the map and print out all key/value pairs. kudos: wikipedia
    g_LogFile.debug("items", "walking map...");
    for (int i = 0; i < fl.size(); i++)
    {
        try {
            g_LogFile.debug("items", "\t\tLoading xml Item from", fl[i].full());
            inventory_manager().LoadItemsXML(fl[i].full());
        } catch (std::runtime_error& error) {
            g_LogFile.error("items", "Could not load items from '", fl[i].full(), "': ", error.what());
        }
    }
}

void cGame::ReadGameAttributesXML(const tinyxml2::XMLElement& el) {
    // load cheating
    m_IsCheating = el.IntAttribute("Cheat", 0);

    m_TalkCount = el.IntAttribute("TalkCount", 0);
    m_WalkAround = el.IntAttribute("WalkAround", false);

    // load supply shed level, other goodies
    el.QueryIntAttribute("SupplyShedLevel", &m_SupplyShedLevel);

    // load bribe rate and bank
    el.QueryAttribute("BribeRate", &m_BribeRate);
    el.QueryAttribute("Bank", &m_Bank);

    // load prison
    auto pPrisonGirls = el.FirstChildElement("PrisonGirls");
    if (pPrisonGirls)
    {
        GetPrison().LoadXML(*pPrisonGirls);
    }

    // load runaways
    auto pRunaways = el.FirstChildElement("Runaways");
    if (pRunaways)
    {
        for (auto pGirl = pRunaways->FirstChildElement("Girl"); pGirl != nullptr; pGirl = pGirl->NextSiblingElement("Girl"))
        {    // load each girl and add her
            auto rgirl = std::make_shared<sGirl>(false);
            bool success = rgirl->LoadGirlXML(pGirl);
            if (success) { AddGirlToRunaways(rgirl); }
        }
    }

    // load objective. This does not belong here, but is currently kept for backwards compatibility
    objective_manager().LoadFromXML(el);
}

void cGame::SaveGame(tinyxml2::XMLElement& root) {
    auto& el = root;

    // safe cheat
    el.SetAttribute("Cheat", m_IsCheating);

    el.SetAttribute("TalkCount", m_TalkCount);
    el.SetAttribute("WalkAround", m_WalkAround);

    // save bribe rate and bank
    el.SetAttribute("BribeRate", m_BribeRate);
    el.SetAttribute("Bank", m_Bank);

    // girl file list
    auto& gf = PushNewElement(el, "GirlFiles");
    for(auto& file : m_GirlFiles) {
        PushNewElement(gf, "File").SetText(file.c_str());
    }

    // save prison
    auto& elPrison = PushNewElement(el, "PrisonGirls");
    GetPrison().SaveXML(elPrison);

    // save runaways
    auto& elRunaways = PushNewElement(el, "Runaways");
    for(auto&& rgirl : GetRunaways())
    {
        rgirl->m_DayJob = rgirl->m_NightJob = JOB_RUNAWAY;
        rgirl->SaveGirlXML(elRunaways);
    }

    g_LogFile.log(ELogLevel::NOTIFY, "Saving Rivals");
    rivals().SaveRivalsXML(el);


    g_LogFile.log(ELogLevel::NOTIFY, "Saving Player");
    player().SavePlayerXML(el);

    g_LogFile.log(ELogLevel::NOTIFY, "Saving Dungeon");
    dungeon().SaveDungeonDataXML(&el);

    // output player gold
    gold().saveGoldXML(el);

    // output girls
    girl_pool().SaveGirlsXML(el);    // this is all the girls that have not been acquired

    auto& market = PushNewElement(el, "SlaveMarket");
    GetSlaveMarket().SaveXML(market);

    // output gangs
    gang_manager().SaveGangsXML(el);

    // save objectives.
    objective_manager().SaveToXML(el);

    // output year, month and day
    el.SetAttribute("Year", date().year);
    el.SetAttribute("Month", date().month);
    el.SetAttribute("Day", date().day);

    buildings().SaveXML(el);

    settings().save_xml(el);

    movie_manager().save_xml(PushNewElement(el, "Movies"));
}


// ---------------------------------------------------------------------------------------------------------------------
//                                       Game Progression
// ---------------------------------------------------------------------------------------------------------------------

void cGame::NextWeek()
{
    auto start_time_turn = std::chrono::steady_clock::now();
    m_TalkCount = settings().get_integer(settings::PLAYER_TALK_DEFAULT);
    m_WalkAround = false;

    g_LogFile.info("turn", "Start processing next week");
    gang_manager().GangStartOfShift();

    g_LogFile.info("turn", "Processing buildings");
    for(auto& building : buildings().buildings()) {
        try {
            building->Update();
        } catch (std::exception& exception) {
            g_LogFile.error("girls", "Error when processing building ", building->name(), ": ", exception.what());
            g_Game->error("Error when processing building " + building->name() + ": " + exception.what());
        }
    }

    // clear the events of dungeon girls
    dungeon().ClearDungeonGirlEvents();

    g_LogFile.info("turn", "Money Processing");
    UpdateBribeInfluence();

    // Update the bribe rate
    gold().bribes(GetBribeRate());

    if (GetBankMoney() > 0)                                    // incraese the bank gold by 02%
    {
        int amount = (int)(GetBankMoney()*0.002f);
        m_Bank += amount;
        /*
        *        bank iterest isn't added to the gold value
        *        but it can be recorded for reporting purposes
        */
        gold().bank_interest(amount);
    }

    HandleTaxes();

    g_LogFile.info("turn", "Rivals");
    rivals().check_rivals();

    g_LogFile.info("turn", "Prison");
    if(GetPrison().num() > 0)
    {
        if (g_Dice.percent(10))    // 10% chance of someone being released
        {
            // TODO shouldn't the player get the girl back???
            auto girl = GetPrison().TakeGirl(0);
            girl_pool().AddGirl(girl);
        }
    }

    g_LogFile.info("turn", "Runaways");
    UpdateRunaways();

    // Update the time
    m_Date.day += 7;
    if (m_Date.day > 30)
    {
        m_Date.day -= 30;
        m_Date.month++;
        if (m_Date.month > 12)
        {
            m_Date.month = 1;
            m_Date.year++;
        }
    }

    // keep gravitating player suspicion to 0
    if (player().suspicion() > 0)         player().suspicion(-1);
    else if (player().suspicion() < 0)    player().suspicion(1);
    if (player().suspicion() > 20)        CheckRaid();    // is the player under suspision by the authorities

    // get money from currently extorted businesses
    std::stringstream ss;
    int num_businesses_extortet = gang_manager().GetNumBusinessExtorted();
    if (num_businesses_extortet > 0)
    {
        if (g_Dice.percent(6.7))
        {
            auto girl = girl_pool().CreateRandomGirl(17);
            ss << "A man cannot pay so he sells you his daughter " << girl->FullName() << " to clear his debt to you.\n";
            girl->AddMessage("${name}'s father could not pay his debt to you so he gave her to you as payment.", IMGTYPE_PROFILE, EVENT_DUNGEON);
            dungeon().AddGirl(std::move(girl), DUNGEON_NEWGIRL);
            gang_manager().NumBusinessExtorted(-1);
        }
        int gained_gold = num_businesses_extortet * INCOME_BUSINESS;
        ss << "You gain " << gained_gold << " gold from the " << num_businesses_extortet << " businesses under your control.\n";
        gold().extortion(gained_gold);
        push_message(ss.str(), COLOR_GREEN);
    }

    ss.str("");
    long totalProfit = gold().total_profit();
    if (totalProfit < 0)
    {
        ss << "Your brothel had an overall deficit of " << -totalProfit << " gold.";
        push_message(ss.str(), COLOR_RED);
    }
    else if (totalProfit > 0)
    {
        ss << "You made a overall profit of " << totalProfit << " gold.";
        push_message(ss.str(), COLOR_GREEN);
    }
    else
    {
        ss << "You are breaking even (made as much money as you spent)";
        push_message(ss.str(), COLOR_DARKBLUE);
    }

    // `J` added loss of security if not enough businesses held.

    int num_brothels = buildings().num_buildings(BuildingType::BROTHEL);
    if (num_businesses_extortet < 40 && num_brothels >= 2)
    {
        buildings().building_with_type(BuildingType::BROTHEL, 1)->m_SecurityLevel -= (40 - num_businesses_extortet) * 2;
    }

    if (num_businesses_extortet < 70 && num_brothels >= 3)
    {
        buildings().building_with_type(BuildingType::BROTHEL, 2)->m_SecurityLevel -= (70 - num_businesses_extortet) * 2;
    }

    if (num_businesses_extortet < 100 && num_brothels >= 4)
    {
        buildings().building_with_type(BuildingType::BROTHEL, 3)->m_SecurityLevel -= (100 - num_businesses_extortet) * 2;
    }

    if (num_businesses_extortet < 140 && num_brothels >= 5)
    {
        buildings().building_with_type(BuildingType::BROTHEL, 4)->m_SecurityLevel -= (140 - num_businesses_extortet) * 2;
    }

    if (num_businesses_extortet < 170 && num_brothels >= 6)
    {
        buildings().building_with_type(BuildingType::BROTHEL, 5)->m_SecurityLevel -= (170 - num_businesses_extortet) * 2;
    }

    if (num_businesses_extortet < 220 && num_brothels >= 7)
    {
        buildings().building_with_type(BuildingType::BROTHEL, 6)->m_SecurityLevel -= (220 - num_businesses_extortet) * 2;
    }

    g_LogFile.info("turn", "Update Slave Market");
    // update slave market
    UpdateMarketSlaves();

    // go through and update all the gang-related data (send them on missions, etc.)
    g_LogFile.info("turn", "Update Gangs");
    gang_manager().UpdateGangs();

    g_LogFile.info("turn", "Update Dungeon");
    dungeon().Update();    // update the people in the dungeon

    g_LogFile.info("turn", "Update Objectives");
    // update objectives or maybe create a new one
    if (get_objective()) objective_manager().UpdateObjective();
    else if (g_Dice.percent(45)) objective_manager().CreateNewObjective();

    g_LogFile.info("turn", "Update Customers");
    // go through and update the population base
    customers().ChangeCustomerBase();

    // TODO Free customers?

    // update the players gold
    g_LogFile.info("turn", "Update Gold");
    gold().week_end();

    // Process Triggers
    // RunEvent(EDefaultEvent::NEXT_WEEK);

    // go ahead and handle pregnancies for girls not controlled by player
    girl_pool().UncontrolledPregnancies();

    // Update the shop. This happens very late in the week update, because then we can guarantee that the
    // player always sees a full shop.
    g_LogFile.info("turn", "Update Shop");
    shop().RestockShop();

    // cheat gold
    if (m_IsCheating)    gold().cheat();
    long duration = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::steady_clock::now() - start_time_turn ).count();
    g_LogFile.info("turn", "End Turn. Processing took ", duration, "ms");
}

void cGame::CheckRaid()
{
    cRival *rival = nullptr;
    cRivalManager& rival_mgr = rivals();
    /*
    *    If the player's influence can shield him
    *    it only follows that the influence of his rivals
    *    can act to stitch him up
    *
    *    see if there exists a rival with infulence
    */
    if (!rival_mgr.player_safe())
    {
        rival = rival_mgr.get_influential_rival();
    }
    /*
    *    chance is based on how much suspicion is leveled at
    *    the player, less his influence at city hall.
    *
    *    And then modified back upwards by rival influence
    */
    int pc = player().suspicion() - GetInfluence();
    if (rival)
    {
        pc += rival->m_Influence / 4;
    }
    /*
    *    pc gives us the % chance of a raid
    *    let's do the "not raided" case first
    */
    if (!g_Dice.percent(pc)) {
        /*
        *        you are clearly a model citizen, sir
        *        and are free to go
        */
        return;
    }
    /*
    *    OK, the raid is on. Start formatting a message
    */
    std::stringstream ss;
    ss << "The local authorities perform a bust on your operations: ";
    /*
    *    if we make our influence check, the guard captain will be under
    *    orders from the mayor to let you off.
    *
    *    Let's make sure the player can tell
    */
    if (g_Dice.percent(GetInfluence()))
    {
        ss << "the guard captain lectures you on the importance of crime prevention, whilst also passing on the Mayor's heartfelt best wishes.";
        player().suspicion(-5);
        push_message(ss.str(), COLOR_GREEN);
        return;
    }
    /*
    *    if we have a rival influencing things, it might not matter
    *    if the player is squeaky clean
    */

    if (rival && player().disposition() > 0 && g_Dice.percent(rival->m_Influence / 2))
    {
        int fine = (g_Dice % 1000) + 150;
        gold().fines(fine);
        ss << "the guard captain condemns your operation as a hotbed of criminal activity and fines you " << fine << " gold for 'living without due care and attention'.";
        /*
        *        see if there's a girl using drugs he can nab
        */
        check_druggy_girl(ss);
        /*
        *        make sure the player knows why the captain is
        *        being so blatantly unfair
        */
        ss << "On his way out the captain smiles and says that the " << rival->m_Name << " send their regards.";
        push_message(ss.str(), COLOR_RED);
        return;
    }
    /*
    *    if the player is basically a goody-goody type
    *    he's unlikely to have anything incriminating on
    *    the premises. 20 disposition should see him
    */
    if (g_Dice.percent(player().disposition() * 5))
    {
        ss << "they pronounce your operation to be entirely in accordance with the law.";
        player().suspicion(-5);
        push_message(ss.str(), COLOR_GREEN);
        return;
    }
    int nPlayer_Disposition = player().disposition();
    if (nPlayer_Disposition > -10)
    {
        int fine = (g_Dice % 100) + 20;
        gold().fines(fine);
        ss << "they find you in technical violation of some health and safety ordinances, and they fine you " << fine << " gold.";
    }
    else if (nPlayer_Disposition > -30)
    {
        int fine = (g_Dice % 300) + 40;
        gold().fines(fine);
        ss << "they find some minor criminalities and fine you " << fine << " gold.";
    }
    else if (nPlayer_Disposition > -50)
    {
        int fine = (g_Dice % 600) + 100;
        gold().fines(fine);
        ss << "they find evidence of dodgy dealings and fine you " << fine << " gold.";
    }
    else if (nPlayer_Disposition > -70)
    {
        int fine = (g_Dice % 1000) + 150;
        int bribe = (g_Dice % 300) + 100;
        gold().fines(fine + bribe);
        ss << "they find a lot of illegal activities and fine you " << fine << " gold, it also costs you an extra " << bribe << " to pay them off from arresting you.";
    }
    else if (nPlayer_Disposition > -90)
    {
        int fine = (g_Dice % 1500) + 200;
        int bribe = (g_Dice % 600) + 100;
        gold().fines(fine + bribe);
        ss << "they find enough dirt to put you behind bars for life. " << "It costs you " << bribe << " to stay out of prison, plus another " << fine << " in fines on top of that";
    }
    else
    {
        int fine = (g_Dice % 2000) + 400;
        int bribe = (g_Dice % 800) + 150;
        gold().fines(fine + bribe);
        ss << "the captain declares your premises to be a sinkhole of the utmost vice and depravity, and it is only with difficulty that you dissuade him from seizing all your property on the spot. You pay " << fine << " gold in fines, but only after slipping the captain " << bribe << " not to drag you off to prison.";
    }
    /*
    *    check for a drug-using girl they can arrest
    */
    check_druggy_girl(ss);
    push_message(ss.str(), COLOR_RED);
}

void cGame::HandleTaxes() {
    double taxRate      = settings().get_float(settings::TAXES_RATE);
    double min_tax_rate = settings().get_float(settings::TAXES_MINIMUM);
    double laundry_rate = settings().get_float(settings::TAXES_LAUNDRY);
    if (m_Influence > 0)    // can you influence it lower
    {
        int lowerBy = m_Influence / 20;
        float amount = (float)(lowerBy / 100);
        taxRate = std::max(min_tax_rate, taxRate - amount);
    }
    // check for money laundering and apply tax
    int earnings = gold().total_earned();

    if (earnings <= 0)
    {
        push_message("You didn't earn any money so didn't get taxed.", COLOR_BLUE);
        return;
    }
    /*
    *    money laundering: nice idea - I had no idea it was
    *    in the game.
    *
    *    Probably we should make the player work for this.
    *    invest a little in businesses to launder through.
    */
    int laundry = g_Dice.random(int(earnings * laundry_rate));
    long tax = long((earnings - laundry) * taxRate);
    /*
    *    this should not logically happen unless we
    *    do something very clever with the money laundering
    */
    if (tax <= 0)
    {
        push_message("Thanks to a clever accountant, none of your income turns out to be taxable", COLOR_BLUE);
        return;
    }
    gold().tax(tax);
    std::stringstream ss;
    /*
    *    Let's report the laundering, at least.
    *    Otherwise, it just makes the tax rate wobble a bit
    */
    ss << "You were taxed " << tax << " gold. You managed to launder " << laundry << " through various local businesses.";
    if (m_Influence < 0)    // can you influence it lower
    {
        ss << "\nConsider bribing city officials to get a lower tax rate.";
    }
    push_message(ss.str(), COLOR_BLUE);
}

void cGame::UpdateBribeInfluence()
{
    m_Influence = GetBribeRate();
    auto& rival_list = rivals().GetRivals();
    if (!rival_list.empty())
    {
        long total = m_BribeRate;
        total += TOWN_OFFICIALSWAGES;    // this is the amount the government controls

        for(auto& rival : rival_list)  // get the total for all bribes
        {
            total += rival->m_BribeRate;
        }

        for(auto& rival : rival_list)    // get the total for all bribes
        {
            if (rival->m_BribeRate > 0 && total != 0)
                rival->m_Influence = (int)(((float)rival->m_BribeRate / (float)total)*100.0f);
            else
                rival->m_Influence = 0;
        }

        if (m_BribeRate != 0 && total != 0)
            m_Influence = (int)(((float)m_BribeRate / (float)total)*100.0f);
        else
            m_Influence = 0;
    }
    else
    {
        if (m_BribeRate <= 0)
            m_Influence = 0;
        else
            m_Influence = (int)(((float)m_BribeRate / (float)((float)TOWN_OFFICIALSWAGES + (float)m_BribeRate))*100.0f);
    }
}


void cGame::UpdateRunaways() {
    for(auto it = m_Runaways.begin(); it != m_Runaways.end(); )
    {
        auto rgirl = *it;
        if (rgirl->m_RunAway > 0)
        {
            // there is a chance the authorities will catch her if she is branded a slave
            if (rgirl->is_slave() && g_Dice.percent(5))
            {
                // girl is recaptured and returned to you
                it = m_Runaways.erase(it);
                dungeon().AddGirl(rgirl, DUNGEON_GIRLRUNAWAY);
                push_message("A runnaway slave has been recaptured by the authorities and returned to you.", COLOR_GREEN);
                continue;
            }
            rgirl->m_RunAway--;
        }
        else    // add her back to girls available to reacquire
        {
            rgirl->m_NightJob = rgirl->m_DayJob = JOB_RESTING;
            it = m_Runaways.erase(it);
            girl_pool().AddGirl(rgirl);
            continue;
        }

        // regular loop update
        ++it;
    }
}


void cGame::UpdateMarketSlaves()
{
    g_LogFile.debug("game", "Updating slave market");
    int numgirls = g_Dice.bell(settings().get_integer(settings::SLAVE_MARKET_MIN),
                               settings().get_integer(settings::SLAVE_MARKET_MAX));

    // first, we may copy some girls over
    g_LogFile.debug("game", "Removing slaves from market");
    sPercent turnover = settings().get_percent(settings::SLAVE_MARKET_TURNOVER_RATE);
    for(int i = 0; i < GetSlaveMarket().num(); ++i) {
        if(g_Dice.percent(turnover)) {
            auto taken = GetSlaveMarket().TakeGirl(i);
            girl_pool().GiveGirl(std::move(taken));
            --i;  // need to decrement the iterator because the market has just lost a girl
        }
    }

    if (numgirls > 20)    numgirls = 20;
    if (numgirls < 1)    numgirls = 1;

    g_LogFile.debug("game", "Filling up slave market again");
    // next, fill up
    for(unsigned i = GetSlaveMarket().num(); i < numgirls; ++i) {

        if (g_Dice.percent(settings().get_percent(settings::SLAVE_MARKET_UNIQUE_CHANCE)))
        {
            auto girl = girl_pool().GetRandomGirl(true, false, false, false, false, true);
            if(girl) {
                GetSlaveMarket().AddGirl(girl);
                continue;
            }
            g_LogFile.warning("girl", "Tried to create a unique girl for the market, but could not find one.");
        }

        // we didn't make a unique girl so we need a random one
        // try to generate a new random girl. Don't allow name duplicates
        for(int n = 0; n < 20; ++n) {
            auto girl = girl_pool().CreateRandomGirl(0, true);
            // still valid?
            if(girl) {
                GetSlaveMarket().AddGirl(girl);
                break;
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------


void cGame::PushEvent(const scripting::sEventID& event) {
    if(m_EventStack->ActiveScript) {
        m_EventStack->EventStack.push_back(event);
    } else {
        RunNextEvent(event);
    }
}

void cGame::RunNextEvent(const scripting::sEventID& event) {
    m_EventStack->ActiveScript = script_manager().GetGlobalEventMapping()->RunAsync(event, {});
    if(!m_EventStack->ActiveScript)
        return;
    m_EventStack->ActiveScript->SetDoneCallback([this](const scripting::sScriptValue& v){
        // if done, start next script if one is available
        if(!m_EventStack->EventStack.empty()) {
            // handle the next event
            RunNextEvent(m_EventStack->EventStack.front());
            m_EventStack->EventStack.pop_front();
        } else {
            // otherwise, we no longer have an active script
            m_EventStack->ActiveScript = nullptr;
        }
    });
}

void cGame::error(std::string message) {
    for(auto& ctx : m_ErrorContextStack) {
        message = ctx + " >> " + message;
    }
    window_manager().PushError(std::move(message));
}


void cGame::push_message(std::string text, int color)
{
    window_manager().PushMessage(std::move(text), color);
}


cErrorContext cGame::push_error_context(std::string message) {
    m_ErrorContextStack.push_back(std::move(message));
    return cErrorContext(this, [this]() {
        m_ErrorContextStack.pop_back();
    });
}
