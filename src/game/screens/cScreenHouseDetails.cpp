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
#include "cScreenHouseDetails.h"
#include "buildings/cBuildingManager.h"
#include "interface/cWindowManager.h"
#include "cGangs.h"
#include "cObjectiveManager.hpp"
#include "Game.hpp"
#include <sstream>

extern    int    g_TalkCount;

static std::string fmt_objective(std::stringstream &ss, std::string desc, int limit, int sofar = -1)
{
    ss << desc;
    if (limit != -1) { ss << " in " << limit << " weeks"; }
    if (sofar > -1) { ss << ", " << sofar << " acquired so far"; }
    ss << ".";
    return ss.str();
}

cScreenHouseDetails::cScreenHouseDetails() : cInterfaceWindowXML("house_screen.xml")
{
}

void cScreenHouseDetails::set_ids()
{
    header_id        = get_id("ScreenHeader");
    interact_id      = get_id("InteractText");
    interactb_id     = get_id("BuyInteract");
    interactb10_id   = get_id("BuyInteract10");
    slavedate_id     = get_id("SlaveDate", "*Unused*");
    details_id       = get_id("BuildingDetails");

    SetButtonCallback(interactb_id, [this]() {buy_interactions(1); });
    SetButtonCallback(interactb10_id, [this]() {buy_interactions(10); });

    SetButtonCallback(get_id("Settings"), [this](){
        push_window("UserSettings");
    });
}

void cScreenHouseDetails::buy_interactions(int num)
{
    if (g_Game->gold().misc_debit(1000 * num)) g_TalkCount += num;
    init(false);
}

void cScreenHouseDetails::init(bool back)
{
    Focused();

    std::stringstream ss;
    ss << "CURRENT OBJECTIVE: ";
    sObjective* obj = g_Game->get_objective();
    if (obj)
    {
        switch (obj->m_Objective)
        {
        case OBJECTIVE_REACHGOLDTARGET:
            ss << "End the week with " << obj->m_Target << " gold in the bank";
            if (obj->m_Limit != -1) ss << " within " << obj->m_Limit << " weeks";
            ss << ", " << g_Game->GetBankMoney() << " gathered so far.";
            break;
        case OBJECTIVE_GETNEXTBROTHEL:
            fmt_objective(ss, "Purchase the next brothel", obj->m_Limit);
            break;
            /*----
            case OBJECTIVE_PURCHASENEWGAMBLINGHALL:
            fmt_objective(ss, "Purchase a gambling hall", obj->m_Limit);
            break;
            case OBJECTIVE_PURCHASENEWBAR:
            fmt_objective(ss, "Purchase a bar", obj->m_Limit);
            break;
            ----*/
        case OBJECTIVE_LAUNCHSUCCESSFULATTACK:
            fmt_objective(ss, "Launch a successful attack", obj->m_Limit);
            break;
        case OBJECTIVE_HAVEXGOONS:
            ss << "Have " << obj->m_Target << " gangs";
            fmt_objective(ss, "", obj->m_Limit);
            break;
        case OBJECTIVE_STEALXAMOUNTOFGOLD:
            ss << "Steal " << obj->m_Target << " gold";
            fmt_objective(ss, "", obj->m_Limit, obj->m_SoFar);
            break;
        case OBJECTIVE_CAPTUREXCATACOMBGIRLS:
            ss << "Capture " << obj->m_Target << " girls from the catacombs";
            fmt_objective(ss, "", obj->m_Limit, obj->m_SoFar);
            break;
        case OBJECTIVE_HAVEXMONSTERGIRLS:
            ss << "Have a total of " << obj->m_Target << " monster (non-human) girls";
            fmt_objective(ss, "", obj->m_Limit, get_total_player_monster_girls());
            break;
        case OBJECTIVE_KIDNAPXGIRLS:
            ss << "Kidnap " << obj->m_Target << " girls from the streets";
            fmt_objective(ss, "", obj->m_Limit, obj->m_SoFar);
            break;
        case OBJECTIVE_EXTORTXNEWBUSINESS:
            ss << "Control " << obj->m_Target << " city business";
            fmt_objective(ss, "", obj->m_Limit, obj->m_SoFar);
            break;
        case OBJECTIVE_HAVEXAMOUNTOFGIRLS:
            ss << "Have a total of " << obj->m_Target << " girls";
            fmt_objective(ss, "", obj->m_Limit, get_total_player_girls());
            break;
        }
    }
    else ss << "NONE\n";

    ss << "\nCurrent gold: " << g_Game->gold().ival()
        << "\nBank account: " << g_Game->GetBankMoney()
        << "\nBusinesses controlled: " << g_Game->gang_manager().GetNumBusinessExtorted()
        << "\n \nCurrent number of runaways: " << g_Game->GetNumRunaways() << "\n";
    //    `J` added while loop to add runaway's names to the list 
    if (g_Game->GetNumRunaways() > 0)
    {
        bool first = true;
        for(const auto& rgirl : g_Game->GetRunaways())
        {
            if (!first)    ss << " ,   ";
            first = false;
            ss << rgirl->FullName() << " (" << rgirl->m_RunAway << ")";
        }
    }
    EditTextItem(ss.str(), details_id);
    if (interact_id >= 0)
    {
        ss.str(""); ss << "Interactions Left: ";
        if (g_Game->allow_cheats()) ss << "\nInfinate Cheat";
        else ss << g_TalkCount << "\nBuy more for 1000 each.";
        EditTextItem(ss.str(), interact_id);
    }
    if (interactb_id >= 0) DisableWidget(interactb_id, g_Game->allow_cheats() || g_Game->gold().ival() < 1000);
    if (interactb10_id >= 0) DisableWidget(interactb10_id, g_Game->allow_cheats() || g_Game->gold().ival() < 10000);
}
