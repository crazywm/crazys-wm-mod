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
#include "buildings/cBrothel.h"
#include "cRival.h"
#include "cScreenMayor.h"
#include "cGold.h"
#include "Game.hpp"
#include <sstream>

cScreenMayor::cScreenMayor() : cInterfaceWindowXML("mayor_screen.xml")
{
}

void cScreenMayor::set_ids()
{
    back_id            = get_id("BackButton", "Back");
    bribe_id        = get_id("BribeButton");
    details_id        = get_id("MayorDetails");
    header_id        = get_id("ScreenHeader");

    // setup event handlers
    SetButtonNavigation(back_id, "<back>");
    SetButtonCallback(bribe_id, [this](){
        input_integer([](int amount){
            if (amount >= 0) { g_Game->SetBribeRate(amount); }
        });
    });
}

void cScreenMayor::init(bool back)
{
    Focused();
    stringstream ss;
    ss << "Influence Details\n";

    g_Game->UpdateBribeInfluence();

    int PlayersInfluence = g_Game->GetInfluence();
    auto& rivals = g_Game->rivals().GetRivals();
    if (!rivals.empty())
    {
        long top[4];    // the top 4 rival influences
        for (int i = 0; i < 4; i++) top[i] = -1;
        int r = 0;
        for(auto& rival : rivals)    // find the top 4 rival influences of the authorities
        {
            for (int i = 0; i<4; i++)
            {
                if (rival->m_Influence > top[i])
                {
                    if (i + 3 < 4) top[i + 3] = top[i + 2];
                    if (i + 2 < 4) top[i + 2] = top[i + 1];
                    if (i + 1 < 4) top[i + 1] = top[i];
                    top[i] = r;
                    break;
                }
            }
            r++;
        }
        ss << "Your influence: " << PlayersInfluence << "% costing " << g_Game->GetBribeRate() << " gold per week.";

        for (long i : top)
        {
            if (i != -1)
            {
                ss << "\n" << g_Game->rivals().GetRival(i)->m_Name << " : " << g_Game->rivals().GetRival(i)->m_Influence << "% influence";
            }
        }
    }
    else
    {
        ss << "Your influence: " << PlayersInfluence << "%\nNo Rivals";
    }
    ss << "\n \nNumber of girls in prison: " << g_Game->GetNumInPrison();
    EditTextItem(ss.str(), details_id);
}
