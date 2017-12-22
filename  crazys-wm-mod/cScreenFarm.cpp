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
#include "cWindowManager.h"
#include "cScriptManager.h"
#include "InterfaceProcesses.h"
#include "cFarm.h"
#include "cScreenFarm.h"
#include "FileList.h"

extern cInterfaceEventManager g_InterfaceEvents;
extern cWindowManager g_WinManager;
extern cBrothelManager g_Brothels;
extern cFarmManager g_Farm;

extern int g_Building;
extern int g_CurrBrothel;
extern int g_CurrentScreen;
extern string ReadTextFile(DirPath path, string file);

extern bool eventrunning;
extern bool g_InitWin;
extern bool g_Cheats;

extern bool g_CTRLDown;

bool cScreenFarm::ids_set = false;

void cScreenFarm::set_ids()
{
	ids_set			/**/ = true;
	g_LogFile.write("set_ids in cScreenFarm");


	buildinglabel_id/**/ = get_id("BuildingLabel","Header");
	background_id	/**/ = get_id("Background", "Farm");
	walk_id			/**/ = get_id("WalkButton");

	weeks_id		/**/ = get_id("Next Week","Weeks");
	farmdetails_id	/**/ = get_id("BuildingDetails", "Details", "FarmDetails");
	girls_id		/**/ = get_id("Girl Management","Girls");
	staff_id		/**/ = get_id("Staff Management","Staff");
	setup_id		/**/ = get_id("Setup", "SetUp");
	dungeon_id		/**/ = get_id("Dungeon");
	turns_id		/**/ = get_id("Turn Summary","Turn");

	girlimage_id	/**/ = get_id("GirlImage");
	back_id			/**/ = get_id("BackButton","Back");

	nextbrothel_id	/**/ = get_id("PrevButton","Prev","*Unused*");
	prevbrothel_id	/**/ = get_id("NextButton","Next","*Unused*");
	farm_id			/**/ = get_id("Farm", "*Unused*");

}

void cScreenFarm::init()
{
	g_CurrentScreen = SCREEN_FARM;
	g_Building = BUILDING_FARM;

//	DisableButton(walk_id, g_TryOuts);
}

void cScreenFarm::process()
{
	if (!ids_set) set_ids();	// we need to make sure the ID variables are set
	if (girlimage_id != -1 && !eventrunning)	HideImage(girlimage_id, true);
	init();

	if (g_InitWin)
	{
		stringstream fp; fp << g_Farm.GetBrothelString(0) << "\nFood Stored: " << g_Brothels.GetNumFood() << "\nDrink Stored: " << g_Brothels.GetNumDrinks() << "\nGoods Stored: " << g_Brothels.GetNumGoods() << "\nAlchemy Items: " << g_Brothels.GetNumAlchemy();
		EditTextItem(fp.str(), farmdetails_id);
		g_InitWin = false;
	}
	if (g_InterfaceEvents.GetNumEvents() == 0)	return;	// no events means we can go home

	// otherwise, compare event IDs
	if (g_InterfaceEvents.CheckButton(back_id))			// if it's the back button, pop the window off the stack and we're done
	{
		g_CurrentScreen = SCREEN_TOWN;
		g_InitWin = true;
		g_WinManager.Pop();
		return;
	}
	else if (g_InterfaceEvents.CheckButton(walk_id))
	{
//		do_walk();
//		if (!g_Cheats) g_TryOuts = true;
		g_InitWin = true;
		return;
	}
	else if (g_InterfaceEvents.CheckButton(girls_id))
	{
		g_InitWin = true;
		g_WinManager.push("Farm");
		return;
	}
	else if (g_InterfaceEvents.CheckButton(staff_id))
	{
		g_InitWin = true;
		g_WinManager.push("Gangs");
		return;
	}
	else if (g_InterfaceEvents.CheckButton(turns_id))
	{
		g_InitWin = true;
		g_WinManager.push("Turn Summary");
		return;
	}
	else if (g_InterfaceEvents.CheckButton(setup_id))
	{
		g_Building = BUILDING_FARM;
		g_InitWin = true;
		g_WinManager.push("Building Setup");
		return;
	}
	else if (g_InterfaceEvents.CheckButton(dungeon_id))
	{
		g_InitWin = true;
		g_WinManager.push("Dungeon");
		return;
	}
	else if (g_InterfaceEvents.CheckButton(weeks_id))
	{
		g_InitWin = true;
		if (!g_CTRLDown) { g_CTRLDown = false; AutoSaveGame(); }
		NextWeek();
		g_WinManager.push("Turn Summary");
		return;
	}
}