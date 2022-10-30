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

#include "cObjectiveManager.hpp"
#include "Constants.h"
#include "buildings/cDungeon.h"
#include "character/cPlayer.h"
#include "cGangs.h"
#include "cGangManager.hpp"
#include "cRival.h"
#include "IGame.h"
#include "cInventory.h"
#include <sstream>
#include "xml/util.h"
#include "Inventory.h"
#include "buildings/cBuildingManager.h"

extern cRng                    g_Dice;

// ----- Objective
void cObjectiveManager::UpdateObjective()
{
    if (m_Objective)
    {
        if (m_Objective->m_Limit > -1) m_Objective->m_Limit--;

        switch (m_Objective->m_Objective)
        {
        case OBJECTIVE_REACHGOLDTARGET:
            if (g_Game->GetBankMoney() >= m_Objective->m_Target)    PassObjective();    // `J` changed to bank instead of cash to clear up issues
            break;
        case OBJECTIVE_HAVEXGOONS:
            if (g_Game->gang_manager().GetNumGangs() >= m_Objective->m_Target)        PassObjective();
            break;
        case OBJECTIVE_STEALXAMOUNTOFGOLD:
        case OBJECTIVE_CAPTUREXCATACOMBGIRLS:
        case OBJECTIVE_KIDNAPXGIRLS:
        case OBJECTIVE_EXTORTXNEWBUSINESS:
            if (m_Objective->m_SoFar >= m_Objective->m_Target)        PassObjective();
            break;
        case OBJECTIVE_HAVEXMONSTERGIRLS:
            if (get_total_player_monster_girls() >= m_Objective->m_Target)    PassObjective();
            break;
        case OBJECTIVE_HAVEXAMOUNTOFGIRLS:
            if (get_total_player_girls() >= m_Objective->m_Target)        PassObjective();
            break;

            // note that OBJECTIVE_GETNEXTBROTHEL has PassObjective() call in cScreenTown when passed.
        }

        // `J` moved to the end and fixed so if the objective is passed (thus deleted), failure is not returned
        if (m_Objective != nullptr && m_Objective->m_Limit == 0)
        {
            std::stringstream ss;
            if (!m_Objective->m_FailText.empty())        ss << "You have failed your objective:\n" << m_Objective->m_FailText;
            else if (m_Objective->m_Text.empty())    ss << "You have failed an objective.";
            else ss << "You have failed your objective to " << m_Objective->m_Text;
            g_Game->push_message(ss.str(), COLOR_WARNING);
            m_Objective.reset();
        }
    }
}

sObjective* cObjectiveManager::GetObjective(){ return m_Objective.get(); }

void cObjectiveManager::CreateNewObjective()
{
    m_Objective.reset(new sObjective());
    if (m_Objective)
    {
        std::stringstream ss;
        std::stringstream ssf;
        std::stringstream ssp;
        std::stringstream sst;

        sst << "You have a new objective:\n";
        bool done = false;
        m_Objective->m_Difficulty = std::max(0, ((int)g_Game->date().year - 1209));
        m_Objective->m_SoFar = 0;
        m_Objective->m_Reward = g_Dice%NUM_REWARDS;
        m_Objective->m_Limit = -1;
        m_Objective->m_Target = 0;
        m_Objective->m_Text = "";
        m_Objective->m_FailText = "";
        m_Objective->m_PassText = "";

        while (!done)
        {
            m_Objective->m_Objective = g_Dice%NUM_OBJECTIVES;
            switch (m_Objective->m_Objective)
            {
            case OBJECTIVE_REACHGOLDTARGET:
            {
                if (true)
                {
                    ss << "Acquire ";
                    if (m_Objective->m_Difficulty >= 3)
                    {
                        m_Objective->m_Limit = (g_Dice % 20) + 10;
                        m_Objective->m_Target = m_Objective->m_Limit * 1000;
                        ss << m_Objective->m_Target << " gold within " << m_Objective->m_Limit << " weeks.";
                    }
                    else
                    {
                        m_Objective->m_Target = ((g_Dice % 20) + 1) * 200;
                        ss << m_Objective->m_Target << " gold.";
                    }
                }
                else
                {
                    m_Objective->m_Target = ((g_Dice % 20) + 1) * ((m_Objective->m_Difficulty + 2) * 100) * (m_Objective->m_Difficulty + 1);
                    ss << "The bank has requested that you deposit " << m_Objective->m_Target << " gold into your account";
                    if (m_Objective->m_Difficulty >= 3)
                    {
                        m_Objective->m_Limit = std::max(3, (g_Dice % 10) - m_Objective->m_Difficulty);
                        ss << " within " << m_Objective->m_Limit << " weeks.";
                    }
                    ssf << "The bank has determined that you are not a serious investor and has reduced your interest rate. (Not implemented yet)";
                    ssp << "The bank is pleased to hold on to your gold for you";
                }
                done = true;
            }break;

            case OBJECTIVE_LAUNCHSUCCESSFULATTACK:
            {
                cRivalManager r;
                if (r.GetNumRivals() > 0)
                {
                    if (g_Game->gang_manager().GetNumGangs() > 0)
                    {
                        ss << "Your gang" << (g_Game->gang_manager().GetNumGangs() > 1 ? "s are" : " is") << " getting restless and itching for a fight. ";
                    }
                    ss << "Launch a successful attack mission within ";
                    m_Objective->m_Limit = (m_Objective->m_Difficulty >= 3 ? (g_Dice % 5) + 3 : (g_Dice % 10) + 10);
                    ss << m_Objective->m_Limit << " weeks.";
                    done = true;
                }
            }break;

            case OBJECTIVE_HAVEXGOONS:
            {
                if (g_Game->gang_manager().GetNumGangs() < g_Game->gang_manager().GetMaxNumGangs())
                {
                    m_Objective->m_Target = g_Game->gang_manager().GetNumGangs() + ((g_Dice % 3) + 1);
                    if (m_Objective->m_Target > g_Game->gang_manager().GetMaxNumGangs()) m_Objective->m_Target = g_Game->gang_manager().GetMaxNumGangs();
                    m_Objective->m_Limit = (m_Objective->m_Difficulty >= 3 ? (g_Dice % 4) + 3 : (g_Dice % 7) + 6);
                    ss << "Have " << m_Objective->m_Target << " gangs within " << m_Objective->m_Limit << " weeks.";
                    done = true;
                }
            }break;

            case OBJECTIVE_STEALXAMOUNTOFGOLD:
            {
                if (m_Objective->m_Difficulty >= 2)
                {
                    m_Objective->m_Limit = (g_Dice % 20) + 13;
                    m_Objective->m_Target = m_Objective->m_Limit * 1300;
                    ss << "Steal " << m_Objective->m_Target << " gold within " << m_Objective->m_Limit << " weeks.";
                }
                else
                {
                    m_Objective->m_Target = ((g_Dice % 20) + 1) * 200;
                    ss << "Steal " << m_Objective->m_Target << " gold.";
                }
                done = true;
            }break;

            case OBJECTIVE_CAPTUREXCATACOMBGIRLS:
            {
                ss << "Capture ";
                if (m_Objective->m_Difficulty >= 2)
                {
                    m_Objective->m_Limit = (g_Dice % 5) + 1;
                    m_Objective->m_Target = (g_Dice % (m_Objective->m_Limit - 1)) + 1;
                    ss << m_Objective->m_Target << " girls from the catacombs within " << m_Objective->m_Limit << " weeks.";
                }
                else
                {
                    m_Objective->m_Target = (g_Dice % 5) + 1;
                    ss << m_Objective->m_Target << " girls from the catacombs.";
                }
                done = true;
            }break;

            case OBJECTIVE_HAVEXMONSTERGIRLS:
            {
                ss << "Have a total of ";
                if (m_Objective->m_Difficulty >= 2)
                {
                    m_Objective->m_Limit = (g_Dice % 8) + 3;
                    m_Objective->m_Target = get_total_player_monster_girls() + (g_Dice % (m_Objective->m_Limit - 1)) + 1;
                    ss << m_Objective->m_Target << " monster (non-human) girls within " << m_Objective->m_Limit << " weeks.";
                }
                else
                {
                    m_Objective->m_Target = get_total_player_monster_girls() + (g_Dice % 8) + 1;
                    ss << m_Objective->m_Target << " monster (non-human) girls.";
                }
                done = true;
            }break;

            case OBJECTIVE_KIDNAPXGIRLS:
            {
                ss << "Kidnap ";
                if (m_Objective->m_Difficulty >= 2)
                {
                    m_Objective->m_Limit = (g_Dice % 5) + 1;
                    m_Objective->m_Target = (g_Dice % (m_Objective->m_Limit - 1)) + 1;
                    ss << m_Objective->m_Target << " girls from the streets within " << m_Objective->m_Limit << " weeks.";
                }
                else
                {
                    m_Objective->m_Target = (g_Dice % 5) + 1;
                    ss << m_Objective->m_Target << " girls from the streets.";
                }
                done = true;
            }break;

            case OBJECTIVE_EXTORTXNEWBUSINESS:
            {    // `J` if there are not enough available businesses, don't use this one
                if (TOWN_NUMBUSINESSES > g_Game->gang_manager().GetNumBusinessExtorted() + 5)
                {
                    ss << "Gain control of ";
                    if (m_Objective->m_Difficulty >= 2)
                    {
                        m_Objective->m_Limit = (g_Dice % 5) + 1;
                        m_Objective->m_Target = (g_Dice % (m_Objective->m_Limit - 1)) + 1;
                        ss << m_Objective->m_Target << " new businesses within " << m_Objective->m_Limit << " weeks.";
                    }
                    else
                    {
                        m_Objective->m_Target = (g_Dice % 5) + 1;
                        ss << m_Objective->m_Target << " new businesses.";
                    }
                    done = true;
                }
            }break;

            case OBJECTIVE_HAVEXAMOUNTOFGIRLS:
            {
                ss << "Have a total of ";
                if (m_Objective->m_Difficulty >= 2)
                {
                    m_Objective->m_Limit = (g_Dice % 8) + 3;
                    m_Objective->m_Target = get_total_player_girls() + (g_Dice % (m_Objective->m_Limit - 1)) + 1;
                    ss << m_Objective->m_Target << " girls within " << m_Objective->m_Limit << " weeks.";
                }
                else
                {
                    m_Objective->m_Target = get_total_player_girls() + (g_Dice % 8) + 1;
                    ss << m_Objective->m_Target << " girls.";
                }
                done = true;
            }break;

            case OBJECTIVE_GETNEXTBROTHEL:
            {
                if (g_Game->buildings().num_buildings(BuildingType::BROTHEL) < 6)
                {
//                    ss << "The seller of a brothel is offering a bonus mystery prize to whoever buys it";
                    ss << "Purchase a new brothel";
                    if (m_Objective->m_Difficulty >= 2)
                    {
                        m_Objective->m_Limit = (g_Dice % 10) + 10;
                        ss << " within " << m_Objective->m_Limit << " weeks";
                    }
                    ss << ".";
                    done = true;
                }
            }break;
            }
        }

        sst << ss.str();
        m_Objective->m_Text = ss.str();
        m_Objective->m_FailText = ssf.str();
        m_Objective->m_PassText = ssp.str();

        if (sst.str().length() > 0)
        {
            g_Game->push_message(sst.str(), COLOR_EMPHASIS);
            // TODO give objectives their own message category
            g_Game->buildings().get_building(0).AddMessage(sst.str(), EVENT_GOODNEWS);
        }
    }
}

void cObjectiveManager::PassObjective()
{
    if (m_Objective)
    {
        // `J` fix for REWARD_RIVALHINDER so it does not have to recall PassObjective()
        cRival* rival = nullptr;
        if (m_Objective->m_Reward == REWARD_RIVALHINDER)
        {
            rival = g_Game->random_rival();
            if (!rival) m_Objective->m_Reward = REWARD_GOLD;
        }

        std::stringstream ss;
        if (m_Objective->m_Text.empty())    ss << "You have completed your objective and you";
        else ss << "You have completed your objective to " << m_Objective->m_Text <<"\nYou";


        switch (m_Objective->m_Reward)
        {
        case REWARD_GOLD:
        {
            long gold = (g_Dice % 200) + 33;
            if (m_Objective->m_Difficulty > 0) gold *= m_Objective->m_Difficulty;

            // `J` if you had a time limit you get extra gold for the unused time
            int mod = m_Objective->m_Target;
            if (m_Objective->m_Objective == OBJECTIVE_REACHGOLDTARGET || m_Objective->m_Objective == OBJECTIVE_STEALXAMOUNTOFGOLD)
                mod = std::min(1, m_Objective->m_Target / 100);
            if (m_Objective->m_Limit > 0) gold += mod * m_Objective->m_Limit;

            ss << " get " << gold << " gold.";
            g_Game->gold().objective_reward(gold);
        }break;

        case REWARD_GIRLS:
        {
            int girls = 1;
            if (m_Objective->m_Difficulty > 0) girls *= m_Objective->m_Difficulty;

            // `J` throw in a few extra girls if your mission was to get more girls
            int div = 0;
            int bonus = std::min(5, m_Objective->m_Limit < 4 ? 1 : m_Objective->m_Limit / 2);
            if (m_Objective->m_Objective == OBJECTIVE_CAPTUREXCATACOMBGIRLS || m_Objective->m_Objective == OBJECTIVE_KIDNAPXGIRLS)
                div = 10;
            if (m_Objective->m_Objective == OBJECTIVE_HAVEXMONSTERGIRLS || m_Objective->m_Objective == OBJECTIVE_HAVEXAMOUNTOFGIRLS)
                div = 20;
            if (bonus > 0 && div > 0) girls += std::min(bonus, m_Objective->m_Target / div);


            ss << " get " << girls << " slave girl" << (girls > 1 ? "s" : "") << ":\n";
            while (girls > 0)
            {
                auto girl = g_Game->CreateRandomGirl(SpawnReason::REWARD);
                std::stringstream ssg;
                ss << girl->FullName() << "\n";
                ssg << girl->FullName() << " was given to you as a reward for completing your objective.";
                girl->AddMessage(ssg.str(), EImageBaseType::PROFILE, EVENT_DUNGEON);
                g_Game->dungeon().AddGirl(std::move(girl), DUNGEON_NEWGIRL);
                girls--;
            }
        }break;

        case REWARD_RIVALHINDER:
        {
            long gold = (rival->m_Gold > 10 ? (g_Dice % (rival->m_Gold / 2)) + 1 : 436);
            rival->m_Gold -= gold;
            g_Game->gold().objective_reward(gold);
            ss << " get to steal " << gold << " gold from the " << rival->m_Name << ".";

            // `J` added
            bool building = false;
            if (rival->m_NumBrothels > 0 && g_Dice.percent(10))
            {
                ss << "\nOne of their Brothels ";
                building = true;
                rival->m_NumBrothels--;
            }
            else if (rival->m_NumGamblingHalls > 0 && g_Dice.percent(25))
            {
                ss << "\nOne of their Gambling Halls ";
                building = true;
                rival->m_NumGamblingHalls--;
            }
            else if (rival->m_NumBars > 0 && g_Dice.percent(50))
            {
                ss << "\nOne of their Bars ";
                building = true;
                rival->m_NumBars--;
            }
            if (building)
            {
                switch (g_Dice % 5)
                {
                case 0: ss << "is closed down by the health department."; break;
                case 1: ss << "is bombed by an unknown party."; break;
                case 2: ss << "vanishes."; break;
                case 3: ss << "falls into a sinkhole."; break;
                default: ss << "mysteriously burns to the ground."; break;
                }
            }

            if (rival->m_NumGirls > 0 && g_Dice.percent(30))
            {
                int num = 1;
                rival->m_NumGirls--;
                while (rival->m_NumGirls > 0 && g_Dice.percent(50))
                {
                    num++;
                    rival->m_NumGirls--;
                }
                ss << "\n" << num << " of their girls ";
                switch (g_Dice % 5)
                {
                case 0: ss << "were arrested for various crimes."; break;
                case 1: ss << "were killed."; break;
                case 2: ss << "vanished."; break;
                case 3: ss << "disappeared."; break;
                default: ss << "were kidnapped."; break;
                }
            }
        }break;

        case REWARD_ITEM:
        {
            int numItems = std::max(1, m_Objective->m_Difficulty);
            int tries = numItems * 10;
            std::vector<std::string> itemnames;
            while (numItems > 0 && tries > 0)
            {
                tries--;
                const sInventoryItem* item = g_Game->inventory_manager().GetRandomItem();
                if(!item)
                   break;       // supplier is all out :(
                else if (item->m_Rarity < Item_Rarity::SCRIPTONLY)
                {
                    if(g_Game->player().inventory().add_item(item)) {
                        itemnames.push_back(item->m_Name);
                        numItems--;
                    }
                    else
                    {
                        numItems = 0;
                        ss << "r inventory is full so instead you";
                    }
                }
            }

            if (numItems > 0)
            {
                ss << " get one item:\n" << itemnames[0];
            }
            else if (numItems > 1)
            {
                ss << " get " << numItems << " items:";
                for (int i = 0; i < numItems; i++)
                {
                    ss << "\n" << itemnames[i];
                }
            }
            else        // no items so get gold instead
            {
                long gold = (g_Dice % 200) + 33;
                if (m_Objective->m_Difficulty > 0) gold *= m_Objective->m_Difficulty;
                ss << " get " << gold << " gold.";
                g_Game->gold().objective_reward(gold);
            }
        }break;
        }

        if (ss.str().length() > 0)
        {
            g_Game->push_message(ss.str(), COLOR_POSITIVE);
            g_Game->buildings().get_building(0).AddMessage(ss.str(), EVENT_GOODNEWS);
        }

        m_Objective.reset();
    }
}


void cObjectiveManager::LoadFromXML(const tinyxml2::XMLElement& root)
{
    auto pObjective = root.FirstChildElement("Objective");
    if (pObjective)
    {
        m_Objective = std::make_unique<sObjective>();
        pObjective->QueryIntAttribute("Difficulty", &m_Objective->m_Difficulty);
        pObjective->QueryIntAttribute("Limit", &m_Objective->m_Limit);
        pObjective->QueryAttribute("Objective", &m_Objective->m_Objective);
        pObjective->QueryIntAttribute("Reward", &m_Objective->m_Reward);
        pObjective->QueryIntAttribute("SoFar", &m_Objective->m_SoFar);
        pObjective->QueryIntAttribute("Target", &m_Objective->m_Target);

        // `J` added
        if (pObjective->Attribute("Text")) { m_Objective->m_Text =pObjective->Attribute("Text"); } else m_Objective->m_Text = "";
        // `J` added .06.03.01
        if (pObjective->Attribute("FailText")) { m_Objective->m_FailText = pObjective->Attribute("FailText"); }    else m_Objective->m_FailText = "";
        if (pObjective->Attribute("PassText")) { m_Objective->m_PassText = pObjective->Attribute("PassText"); }    else m_Objective->m_PassText = "";
    }
}

void cObjectiveManager::SaveToXML(tinyxml2::XMLElement& root) const
{
    if (m_Objective)
    {
        auto& elObjective = PushNewElement(root, "Objective");
        elObjective.SetAttribute("Difficulty", m_Objective->m_Difficulty);
        elObjective.SetAttribute("Limit", m_Objective->m_Limit);
        elObjective.SetAttribute("Objective", m_Objective->m_Objective);
        elObjective.SetAttribute("Reward", m_Objective->m_Reward);
        elObjective.SetAttribute("SoFar", m_Objective->m_SoFar);
        elObjective.SetAttribute("Target", m_Objective->m_Target);
        elObjective.SetAttribute("Text", m_Objective->m_Text.c_str());
        elObjective.SetAttribute("FailText", m_Objective->m_FailText.c_str());
        elObjective.SetAttribute("PassText", m_Objective->m_PassText.c_str());
    }
}
