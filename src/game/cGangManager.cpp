#include "cGangManager.hpp"

#include "buildings/cBuildingManager.h"
#include "cRival.h"
#include "cTariff.h"
#include "gang_missions.h"
#include "IGame.h"
#include <fstream>
#include <utils/DirPath.h>
#include <utils/string.hpp>
#include <xml/util.h>
#include <CLog.h>
#include <utils/algorithms.hpp>

extern cRng g_Dice;

namespace settings {
    extern const char* GANG_MAX_RECRUIT_LIST;
    extern const char* GANG_MAX_START_MEMBERS;
    extern const char* GANG_MIN_START_MEMBERS;
    extern const char* GANG_MAX_RIVAL_MEMBERS;
    extern const char* GANG_MIN_RIVAL_MEMBERS;
    extern const char* GANG_REMOVE_CHANCE;
    extern const char* GANG_MIN_WEEKLY_NEW;
    extern const char* GANG_MAX_WEEKLY_NEW;
    extern const char* GANG_MIN_RIVAL_SKILL;
    extern const char* GANG_MAX_RIVAL_SKILL;
}

cGangManager::cGangManager() {
    m_BusinessesExtort = 0;
    m_Gang_Gets_Girls = m_Gang_Gets_Items = m_Gang_Gets_Beast = 0;

    // load gang names
    std::ifstream in;
    // WD: Typecast to resolve ambiguous call in VS 2010
    DirPath dp = DirPath() << "Resources" << "Data" << "HiredGangNames.txt";
    in.open(dp.c_str());
    in.seekg(0);
    readline(in);         // ignore the first line (it's a line count)
    while(in.good())      // read until EOF
        {
        std::string name = readline(in);
        if(!name.empty())
            m_GangNames.emplace_back(std::move(name));
        }

    m_Missions.resize(MISS_COUNT);
    m_Missions[MISS_GUARDING] = std::make_unique<cMissionGuarding>();
    m_Missions[MISS_SABOTAGE] = std::make_unique<cMissionSabotage>();
    m_Missions[MISS_SPYGIRLS] = std::make_unique<cMissionSpyGirls>();
    m_Missions[MISS_CAPTUREGIRL] = std::make_unique<cMissionRecapture>();
    m_Missions[MISS_EXTORTION] = std::make_unique<cMissionExtortion>();
    m_Missions[MISS_PETYTHEFT] = std::make_unique<cMissionPettyTheft>();
    m_Missions[MISS_GRANDTHEFT] = std::make_unique<cMissionGrandTheft>();
    m_Missions[MISS_KIDNAPP] = std::make_unique<cMissionKidnap>();
    m_Missions[MISS_CATACOMBS] = std::make_unique<cMissionCatacombs>();
    m_Missions[MISS_TRAINING] = std::make_unique<cMissionTraining>();
    m_Missions[MISS_RECRUIT] = std::make_unique<cMissionRecruiting>();
    m_Missions[MISS_SERVICE] = std::make_unique<cMissionService>();
    m_Missions[MISS_DUNGEON] = nullptr;
}

cGangManager::~cGangManager() = default;

bool cGangManager::LoadGangsXML(const tinyxml2::XMLElement* pGangManager)
{
    if (pGangManager == nullptr) return false;

    // load goons and goon missions
    auto* pGangs = pGangManager->FirstChildElement("Gangs");
    if (pGangs)
    {
        for (auto* pGang = pGangs->FirstChildElement("Gang"); pGang != nullptr; pGang = pGang->NextSiblingElement("Gang"))
        {
            auto gang = std::make_unique<sGang>();
            if (gang->LoadGangXML(*pGang)) {
                AddGang(std::move(gang));
            }
        }
    }
    // load hireable goons
    auto* pHireables = pGangManager->FirstChildElement("Hireables");
    if (pHireables)
    {
        for (auto* pGang = pHireables->FirstChildElement("Gang"); pGang != nullptr; pGang = pGang->NextSiblingElement("Gang"))
        {
            auto gang = std::make_unique<sGang>();
            if (gang->LoadGangXML(*pGang)) {
                m_HireableGangs.push_back(std::move(gang));
            }
        }
    }

    pGangManager->QueryIntAttribute("BusinessesExtort", &m_BusinessesExtort);
    pGangManager->QueryIntAttribute("NumHealingPotions", &m_NumHealingPotions);
    pGangManager->QueryIntAttribute("NumNets", &m_NumNets);
    pGangManager->QueryIntAttribute("KeepHealStocked", &m_KeepHealStocked);
    pGangManager->QueryIntAttribute("KeepNetsStocked", &m_KeepNetsStocked);

    // `J` added for .06.01.10
    pGangManager->QueryIntAttribute("Gang_Gets_Girls", &m_Gang_Gets_Girls);
    pGangManager->QueryIntAttribute("Gang_Gets_Items", &m_Gang_Gets_Items);
    pGangManager->QueryIntAttribute("Gang_Gets_Beast", &m_Gang_Gets_Beast);
    if ((m_Gang_Gets_Girls == 0 && m_Gang_Gets_Items == 0 && m_Gang_Gets_Beast == 0) ||
    m_Gang_Gets_Girls + m_Gang_Gets_Items + m_Gang_Gets_Beast != 100)
    {
        m_Gang_Gets_Items = 34;
        m_Gang_Gets_Beast = 33;
        m_Gang_Gets_Girls = 33;
    }

    return true;
}

tinyxml2::XMLElement& cGangManager::SaveGangsXML(tinyxml2::XMLElement& elRoot)
{
    auto& elGangManager = PushNewElement(elRoot, "Gang_Manager");

    auto& elGangs = PushNewElement(elGangManager, "Gangs");
    for(auto& gang : m_PlayersGangs)
    {
        gang->SaveGangXML(elGangs);
    }
    auto& elHireables =  PushNewElement(elGangManager, "Hireables");
    for(auto& gang : m_HireableGangs)
    {
        gang->SaveGangXML(elHireables);
    }

    elGangManager.SetAttribute("BusinessesExtort", m_BusinessesExtort);
    elGangManager.SetAttribute("NumHealingPotions", m_NumHealingPotions);
    elGangManager.SetAttribute("NumNets", m_NumNets);
    elGangManager.SetAttribute("KeepHealStocked", m_KeepHealStocked);
    elGangManager.SetAttribute("KeepNetsStocked", m_KeepNetsStocked);

    // `J` added for .06.01.10
    if (m_Gang_Gets_Girls == 0 && m_Gang_Gets_Items == 0 && m_Gang_Gets_Beast == 0)
    {
        m_Gang_Gets_Items = 34;
        m_Gang_Gets_Beast = 33;
        m_Gang_Gets_Girls = 33;

    }
    elGangManager.SetAttribute("Gang_Gets_Girls", m_Gang_Gets_Girls);
    elGangManager.SetAttribute("Gang_Gets_Items", m_Gang_Gets_Items);
    elGangManager.SetAttribute("Gang_Gets_Beast", m_Gang_Gets_Beast);
    return elGangManager;
}


// ----- Hire fire

void cGangManager::HireGang(int gangID)
{
    auto gang = std::move(m_HireableGangs.at(gangID));
    m_HireableGangs.erase(m_HireableGangs.begin() + gangID);
    gang->m_Combat = gang->m_AutoRecruit = false;
    gang->m_LastMissID = -1;
    if (gang->m_Num <= 5) gang->m_MissionID = MISS_RECRUIT;
    AddGang(std::move(gang));
}

void cGangManager::FireGang(int gangID)
{
    auto gang = std::move(m_PlayersGangs.at(gangID));
    m_PlayersGangs.erase(m_PlayersGangs.begin() + gangID);
    if (m_HireableGangs.size() < g_Game->settings().get_integer(settings::GANG_MAX_RECRUIT_LIST))
    {
        gang->m_Combat = gang->m_AutoRecruit = false;
        gang->m_LastMissID = -1;
        m_HireableGangs.push_back(std::move(gang));
    }
}

// ----- Add remove

void cGangManager::AddNewGang(bool boosted)
{
    int name_index = -1;
    while (true)
    {
        name_index = g_Dice%m_GangNames.size();
        const std::string& name = m_GangNames[name_index];
        if(std::any_of(begin(m_PlayersGangs), end(m_PlayersGangs),
                       [&](auto& gang) { return gang->name() == name; })) {
            continue;
        }

        if(std::any_of(begin(m_HireableGangs), end(m_HireableGangs),
                       [&](auto& gang) { return gang->name() == name; })) {
            continue;
        }
        break;
    }

    auto newGang = std::make_unique<sGang>(m_GangNames[name_index]);

    int max_members = g_Game->settings().get_integer(settings::GANG_MAX_START_MEMBERS);
    int min_members = g_Game->settings().get_integer(settings::GANG_MIN_START_MEMBERS);
    newGang->m_Num = g_Dice.closed_uniform(min_members, max_members);
    if (boosted) newGang->m_Num = std::min(sGang::max_members(), newGang->m_Num + 5);

    int new_val;
    for (int & m_Skill : newGang->m_Skills)
    {
        new_val = (g_Dice % 30) + 21;
        if (g_Dice % 5 == 1)    new_val += 1 + g_Dice % 10;
        if (boosted)            new_val += 10 + g_Dice % 11;
        m_Skill = new_val;
    }
    for (int & m_Stat : newGang->m_Stats)
    {
        new_val = (g_Dice % 30) + 21;
        if (g_Dice % 5 == 1)    new_val += g_Dice % 10;
        if (boosted)            new_val += 10 + g_Dice % 11;
        m_Stat = new_val;
    }
    newGang->m_Stats[STAT_HEALTH] = 100;
    newGang->m_Stats[STAT_HAPPINESS] = 100;

    g_LogFile.info("gangs", "Added new recruitable gang ", newGang->name());
    m_HireableGangs.push_back(std::move(newGang));
}

void cGangManager::AddGang(std::unique_ptr<sGang> newGang)
{
    m_PlayersGangs.push_back(std::move(newGang));
}

void cGangManager::RemoveGang(sGang* gang)
{
    auto iter = std::find_if(m_PlayersGangs.begin(), m_PlayersGangs.end(), [gang](auto& e) { return e.get() == gang; });
    if(iter != m_PlayersGangs.end()) {
        m_PlayersGangs.erase(iter);
    }
}

// ----- Get

int cGangManager::GetNumGangs()
{
    return m_PlayersGangs.size();
}

int cGangManager::GetMaxNumGangs()
{
    m_MaxNumGangs = 7 + g_Game->buildings().num_buildings(BuildingType::BROTHEL);
    return m_MaxNumGangs;
}

int cGangManager::GetNumHireableGangs()
{
    return m_HireableGangs.size();
}

// `J` added temp gang mod - base strength + mod
sGang cGangManager::GetTempGang(int mod)
{
    sGang newGang;
    newGang.m_Num = g_Dice.closed_uniform(g_Game->settings().get_integer(settings::GANG_MIN_RIVAL_MEMBERS),
                                    g_Game->settings().get_integer(settings::GANG_MAX_RIVAL_MEMBERS));
    auto random_value = [mod]() {
        int value = g_Dice.closed_uniform(g_Game->settings().get_integer(settings::GANG_MIN_RIVAL_SKILL),
                                          g_Game->settings().get_integer(settings::GANG_MAX_RIVAL_SKILL) + mod);
        if (value < 1)    return 1;
        if (value > 100)  return 100;
        return value;
    };
    for (int& m_Skill : newGang.m_Skills) {
        m_Skill = random_value();
    }
    for (int & m_Stat : newGang.m_Stats) {
        m_Stat = random_value();
    }
    newGang.m_Stats[STAT_HEALTH] = g_Dice.closed_uniform(50, 100);
    newGang.m_Stats[STAT_HAPPINESS] = 100;
    newGang.set_weapon_level( g_Dice.closed_uniform(1, 3) );

    return newGang;
}

sGang* cGangManager::GetGang(int gangID)
{
    if(gangID >= m_PlayersGangs.size() || gangID < 0)
        return nullptr;
    else
        return m_PlayersGangs.at(gangID).get();
}

// ----- Combat

// MYR: This is similar to GangCombat, but instead of one of the players gangs
//      fighting the girl, some random gang attacks her.  This random gang
//      doesn't have healing potions and the weapon levels of a player gang.
//      ATM only the new security code uses it.
//      This will also be needed to be updated to the new way of doing combat.
// true means the girl won

// ----- Mission related

// Missions done here - Updated for .06.01.09
void cGangManager::UpdateGangs()
{
    std::stringstream ss;

    // maintain recruitable gangs list, potentially pruning some old ones
    auto remove_chance = g_Game->settings().get_percent(settings::GANG_REMOVE_CHANCE);
    erase_if(m_HireableGangs,  [remove_chance](auto& gang) { return g_Dice.percent(remove_chance); });

    // maybe add some new gangs to the recruitable list
    int add_min = g_Game->settings().get_integer(settings::GANG_MIN_WEEKLY_NEW);
    int add_max = g_Game->settings().get_integer(settings::GANG_MAX_WEEKLY_NEW);
    int add_recruits = g_Dice.bell(add_min, add_max);
    for (int i = 0; i < add_recruits; i++)
    {
        if (m_HireableGangs.size() >= g_Game->settings().get_integer(settings::GANG_MAX_RECRUIT_LIST)) break;
        AddNewGang(false);
    }

    // now, deal with player controlled gangs on missions
    for(auto& currentGang : m_PlayersGangs) {
        switch (currentGang->m_MissionID) {
            case MISS_GUARDING: // these are handled in GangStartOfShift()
            case MISS_SPYGIRLS:
                break;
            case MISS_CAPTUREGIRL:
                if (g_Game->GetNumRunaways() > 0) {
                    m_Missions[MISS_CAPTUREGIRL]->run(*currentGang);
                    break;
                }
                else
                    currentGang->AddMessage(
                            "This gang was sent to look for runaways but there are none so they went looking for any girl to kidnap instead.");
            case MISS_SABOTAGE:
            case MISS_EXTORTION:
            case MISS_PETYTHEFT:
            case MISS_GRANDTHEFT:
            case MISS_KIDNAPP:
            case MISS_CATACOMBS:
            case MISS_TRAINING:
            case MISS_RECRUIT:
            case MISS_SERVICE:
                m_Missions[currentGang->m_MissionID]->run(*currentGang);
                break;
            default: {
                std::stringstream sse;
                g_LogFile.log(ELogLevel::ERROR, "no mission set or mission not found : ", currentGang->m_MissionID);
                sse << "Error: no mission set or mission not found : " << currentGang->m_MissionID;
                currentGang->AddMessage(sse.str());
                }
                break;
        }
    }

    // remove lost gangs
    erase_if(m_PlayersGangs,  [this](auto& gang) { return this->losegang(*gang); });

    // recruitment
    for(auto& gang : m_PlayersGangs) {
        if (!gang->m_Combat && gang->m_Num < sGang::max_members()) gang->m_Num++;
        check_gang_recruit(*gang);
    }
    g_Game->rivals().Update(m_BusinessesExtort);    // Update the rivals

    RestockNetsAndPots();

    // healing
    for(auto& gang : m_PlayersGangs) {
        if(gang->m_MedicalCost > 0) {
            gang->AddMessage("Some of your goons have been hurt in the line of duty. You pay "
            + std::to_string(gang->m_MedicalCost) + " gold so a doctor keeps them fit for work.");
            g_Game->gold().goon_wages( gang->m_MedicalCost );
            gang->m_MedicalCost = 0;
        }
    }
}

// `J` restock at the start and end of the gang shift - Added for .06.01.09
void cGangManager::RestockNetsAndPots()
{
    g_LogFile.log(ELogLevel::DEBUG,
                  "Time to restock heal potions and nets\n"
                  , "Heal Flag    = ", bool(m_KeepHealStocked > 0), "\n"
                  , "Heal Target  = ", m_KeepHealStocked, "\n"
                  , "Heal Current = ", m_KeepHealStocked, "\n"
                  , "Nets Flag    = ", bool(m_KeepNetsStocked > 0), "\n"
                  , "Nets Target  = ", m_KeepNetsStocked, "\n"
                  , "Nets Current = ", m_KeepNetsStocked);
    if (m_KeepHealStocked > 0 && m_KeepHealStocked > m_NumHealingPotions)
    {
        int diff = m_KeepHealStocked - m_NumHealingPotions;
        m_NumHealingPotions = m_KeepHealStocked;
        g_Game->gold().consumable_cost(g_Game->tariff().healing_price(diff));
    }
    if (m_KeepNetsStocked > 0 && m_KeepNetsStocked > m_NumNets)
    {
        int diff = m_KeepNetsStocked - m_NumNets;
        m_NumNets = m_KeepNetsStocked;
        g_Game->gold().consumable_cost(g_Game->tariff().nets_price(diff));
    }
}

sGang* cGangManager::GetGangOnMission(int missID)
{
    auto found = std::find_if(begin(m_PlayersGangs), end(m_PlayersGangs),
                              [missID](auto& gang) {
        return gang->m_Num > 0 && gang->m_MissionID == missID;
    });
    if(found == m_PlayersGangs.end()) {
        return nullptr;
    }
    return found->get();
}

sGang* cGangManager::GetRandomGangOnMission(int missID)
{
    RandomSelector<sGang> selector;
    for(auto& gang : m_PlayersGangs) {
        if(gang->m_MissionID == missID && gang->m_Num > 0) {
            selector.process(gang.get());
        }
    }
    return selector.selection();
}

// `J` - Added for .06.02.18
sGang* cGangManager::GetGangNotFull(int roomfor, bool recruiting)
{
    if (recruiting)
    {
        int missions[5] = { MISS_RECRUIT, MISS_TRAINING, MISS_SPYGIRLS, MISS_GUARDING, MISS_SERVICE };
        for(auto& gang: m_PlayersGangs) {
            if (gang->m_Num + roomfor <= sGang::max_members())
            {
                for (auto mission : missions) if (gang->m_MissionID == mission)    return gang.get();
            }
        }
    }
    else
    {
        for(auto& gang: m_PlayersGangs)
        {
            if (gang->m_Num < sGang::max_members()) return gang.get();
        }
    }
    return nullptr;
}

// `J` - Added for .06.01.09
sGang* cGangManager::GetGangRecruitingNotFull(int roomfor)
{
    // first try to get a gang that can hold all that are being sent to them
    sGang* rec = GetGangNotFull(roomfor, true);
    if(rec) return rec;
    // if none are found then get a gang that has room for at least 1
    return GetGangNotFull(1, true);
}

// Get a vector with all the gangs doing MISS_FOO
std::vector<sGang*> cGangManager::gangs_on_mission(int mission_id)
{
    std::vector<sGang*> v; // loop through the gangs
    for(auto& gang : m_PlayersGangs)
    {
        // if they're not doing the job we are looking for, disregard them
        if (gang->m_MissionID != mission_id) continue;
        v.push_back(gang.get());
    }
    return v;
}

// `J` - Added for .06.01.09
std::vector<sGang*> cGangManager::gangs_watching_girls()
{
    std::vector<sGang*> v; // loop through the gangs
    for(auto& gang : m_PlayersGangs)
    {
        // if they're not doing the job we are looking for, disregard them
        if (gang->m_MissionID == MISS_GUARDING || gang->m_MissionID == MISS_SPYGIRLS) v.push_back(gang.get());
    }
    return v;
}

/*
*    two objectives here:
*    A: multiple squads spying on the girls improves the chance of catching thieves
*    B: The intelligence of the girl and the goons affects the result
*/
int cGangManager::chance_to_catch(const sGirl& girl)
{
    int pc = 0;
    std::vector<sGang*> gvec = gangs_on_mission(MISS_SPYGIRLS);    // get a vector containing all the spying gangs
    for (const auto& gang: gvec)        // whizz down the vector adding probability as we go
        {
        /*
        *        now then: the basic chance is 5 * number of goons
        *        but I want to modify that for the intelligence
        *        of the girl, and that of the squad
        */
        float mod = 1.f + float(gang->intelligence() - girl.intelligence()) / 100.f;
        /*
        *        that should give us a multiplier that can
        *        at one extreme, double the chances of the sqaud
        *        catching her, and at the other, reduce it to zero
        */
        pc += int(5 * gang->m_Num * mod);

        gang->BoostStat(STAT_INTELLIGENCE, 1);
        }
    if (pc > 100) pc = 100;
    return pc;
}

// `J` - Added for .06.01.09
bool cGangManager::losegang(const sGang& gang)
{
    if (gang.m_Num <= 0)
    {
        std::stringstream ss;
        int mission = gang.m_MissionID;
        ss << gang.name() << " was lost while ";
        switch (mission)
        {
            case MISS_GUARDING:         ss << "guarding.";                              break;
            case MISS_SABOTAGE:         ss << "attacking your rivals.";                 break;
            case MISS_SPYGIRLS:         ss << "spying on your girls?";                  break;
            case MISS_CAPTUREGIRL:      ss << "trying to recapture a runaway.";         break;
            case MISS_EXTORTION:        ss << "trying to extort new businesses.";       break;
            case MISS_PETYTHEFT:        ss << "performing petty crimes.";               break;
            case MISS_GRANDTHEFT:       ss << "performing major crimes.";               break;
            case MISS_KIDNAPP:          ss << "trying to kidnap girls.";                break;
            case MISS_CATACOMBS:        ss << "exploring the catacombs.";               break;
            case MISS_TRAINING:         ss << "training?";                              break;
            case MISS_RECRUIT:          ss << "recruiting?";                            break;
            case MISS_SERVICE:          ss << "helping the community.";                 break;
            default:                    ss << "on a mission.";                          break;
        }
        g_Game->push_message(ss.str(), COLOR_RED);
        return true;
    }
    return false;
}

// `J` - Added for .06.01.09
void cGangManager::check_gang_recruit(sGang& gang)
{
    std::stringstream ss;
    if (gang.m_MissionID == MISS_SERVICE || gang.m_MissionID == MISS_TRAINING){}    // `J` service and training can have as few as 1 member doing it.
    else if (gang.m_Num <= 5 && gang.m_MissionID != MISS_RECRUIT)
    {
        ss << "Gang   " << gang.name() << "   were set to recruit due to low numbers";
        gang.AddMessage(ss.str(), EVENT_WARNING);
        gang.m_AutoRecruit = true;
        gang.m_LastMissID = gang.m_MissionID;
        gang.m_MissionID = MISS_RECRUIT;
    }
    else if (gang.m_MissionID == MISS_RECRUIT && gang.m_Num >= sGang::max_members())
    {
        if (gang.m_AutoRecruit)
        {
            ss << "Gang   " << gang.name() << "   were placed back on their previous mission now that their numbers are back to normal.";
            gang.m_MissionID = gang.m_LastMissID;
            gang.m_AutoRecruit = false;
        }
        else
        {
            ss << "Gang   " << gang.name() << "   were placed on guard duty from recruitment as their numbers are full.";
            gang.m_MissionID = MISS_GUARDING;
        }
        gang.AddMessage(ss.str(), EVENT_WARNING);
    }
}

// `J` - Added for .06.01.09
void cGangManager::GangStartOfShift()
{
    std::stringstream ss;

    RestockNetsAndPots();

    // update goons for the start of the turn
    int cost = 0;
    // remove lost gangs
    erase_if(m_PlayersGangs,  [&,this](auto& gang) {
        if (gang->m_Num <= 0)    // clear dead
            {
            ss << "All of the men in gang " << gang->name() << " have died.";
            g_Game->push_message(ss.str(), COLOR_RED);
            return true;
            }
        return false;
    });

    if (m_PlayersGangs.empty()) return;    // no gangs

    for(auto& gang: m_PlayersGangs)
    {
        gang->m_Combat = false;
        gang->GetEvents().Clear();
        cost += g_Game->tariff().goon_mission_cost(gang->m_MissionID);    // sum up the cost of all the goon missions

        check_gang_recruit(*gang);

        if (gang->m_MissionID == MISS_SPYGIRLS)    gang->AddMessage("Gang   " + gang->name() + "   is spying on your girls.");
        if (gang->m_MissionID == MISS_GUARDING)    gang->AddMessage("Gang   " + gang->name() + "   is guarding.");
    }
    g_Game->gold().goon_wages(cost);

    // give out nets
    while(m_NumNets > 0) {
        int last_nets = m_NumNets;
        for(auto& gang: m_PlayersGangs) {
            // if only a few nets are left, only give out to gangs who need them for their job.
            if(m_NumNets < 10 && !m_Missions[gang->m_MissionID]->requires_nets())
                continue;
            m_NumNets -= gang->give_nets(1);
        }

        // if no gang needs any more nets, we can stop
        if(m_NumNets == last_nets)
            break;
    }

    // give out potions
    while(m_NumNets > 0) {
        int last_potions = m_NumHealingPotions;
        for(auto& gang: m_PlayersGangs) {
            // if only a few nets are left, only give out to gangs who need them for their job.
            if(m_NumHealingPotions < 10 && !m_Missions[gang->m_MissionID]->requires_potions())
                continue;
            m_NumHealingPotions -= gang->give_potions(1);
        }

        // if no gang needs any more nets, we can stop
        if(m_NumHealingPotions == last_potions)
            break;
    }
}

int cGangManager::BuyNets(int amount, bool autobuy)
{
    amount = std::min(amount, 60 - m_NumNets);

    int cost = g_Game->tariff().nets_price(amount);
    if (g_Game->gold().item_cost(cost))
    {
        m_NumNets += amount;
    }

    if(autobuy) {
        KeepNetStocked(m_NumNets);
    }

    return amount;
}

int cGangManager::BuyHealingPotions(int amount, bool autobuy)
{
    amount = std::min(amount, 200 - m_NumHealingPotions);

    int cost = g_Game->tariff().healing_price(amount);
    if (g_Game->gold().item_cost(cost))
    {
        m_NumHealingPotions += amount;
    }

    if(autobuy) {
        KeepHealStocked(m_NumHealingPotions);
    }

    return amount;
}
