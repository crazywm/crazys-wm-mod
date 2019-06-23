/*
* Copyright 2009, 2010, The Pink Petal Development Team.
* The Pink Petal Devloment Team are defined as the game's coders
* who meet on http://pinkpetal.org     // old site: http://pinkpetal .co.cc
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
#include "cBrothel.h"
#include "cScreenMayor.h"
#include "cWindowManager.h"
#include "cGold.h"
#include "cGetStringScreenManager.h"
#include "cScreenGetInput.h"
#include "InterfaceGlobals.h"

#ifdef LINUX
#include "linux.h"
#endif

extern bool g_InitWin;
extern int g_CurrBrothel;
extern cGold g_Gold;
extern cBrothelManager g_Brothels;
extern cWindowManager g_WinManager;
extern cInterfaceEventManager g_InterfaceEvents;
extern long g_IntReturn;
extern cScreenGetInput g_GetInput;
extern int g_CurrentScreen;

bool cScreenMayor::ids_set = false;
cScreenMayor::cScreenMayor()
{
	DirPath dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "mayor_screen.xml";
	m_filename = dp.c_str();
	SetBribe = false;
}
cScreenMayor::~cScreenMayor() {}

void cScreenMayor::set_ids()
{
	ids_set			/**/ = true;
	g_LogFile.write("set_ids in cScreenMayor");

	back_id			/**/ = get_id("BackButton", "Back");
	bribe_id		/**/ = get_id("BribeButton");
	details_id		/**/ = get_id("MayorDetails");
	header_id		/**/ = get_id("ScreenHeader");
}

void cScreenMayor::init()
{
	g_CurrentScreen = SCREEN_MAYOR;
	if (!g_InitWin) { return; }
	Focused();
	g_InitWin = false;
	stringstream ss;
	ss << "Influence Details\n";
	if (SetBribe)
	{
		if (g_IntReturn >= 0)
		{
			g_Brothels.SetBribeRate(g_IntReturn);
			SetBribe = false;
		}
	}

	g_Brothels.UpdateBribeInfluence();

	int PlayersInfluence = g_Brothels.GetInfluence();
	cRival* rival = g_Brothels.GetRivals();
	if (rival)
	{
		long top[4];	// the top 4 rival influences
		for (int i = 0; i < 4; i++) top[i] = -1;
		int r = 0;
		while (rival)	// find the top 4 rival influences of the authorities
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
			rival = rival->m_Next;
		}
		ss << "Your influence: " << PlayersInfluence << "% costing " << g_Brothels.GetBribeRate() << " gold per week.";

		for (int i = 0; i < 4; i++)
		{
			if (top[i] != -1)
			{
				ss << "\n" << g_Brothels.GetRivalManager()->GetRival(top[i])->m_Name << " : " << g_Brothels.GetRivalManager()->GetRival(top[i])->m_Influence << "% influence";
			}
		}
	}
	else
	{
		ss << "Your influence: " << PlayersInfluence << "%\nNo Rivals";
	}
	ss << "\n \nNumber of girls in prison: " << g_Brothels.GetNumInPrison();
	EditTextItem(ss.str(), details_id);
}

void cScreenMayor::process()
{
	if (!ids_set) set_ids();		// we need to make sure the ID variables are set
	init();							// set up the window if needed
	check_events();					// check to see if there's a button event needing handling
}

void cScreenMayor::check_events()
{
	if (g_InterfaceEvents.GetNumEvents() == 0) return;	// no events means we can go home

	if (g_InterfaceEvents.CheckButton(back_id)) 		// if it's the back button, pop the window off the stack and we're done
	{
		g_InitWin = true;
		g_WinManager.Pop();
		return;
	}
	if (g_InterfaceEvents.CheckButton(bribe_id))
	{
		SetBribe = true;
		g_GetInput.ModeGetInt();
		g_WinManager.push("GetInput");
		g_InitWin = true;
		return;
	}
}
