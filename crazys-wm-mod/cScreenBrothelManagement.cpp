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
#include "cScreenGetInput.h"
#include "InterfaceProcesses.h"
#include "cBrothel.h"
#include "cScreenBrothelManagement.h"
#include "FileList.h"

extern cInterfaceEventManager g_InterfaceEvents;
extern cWindowManager g_WinManager;
extern cBrothelManager g_Brothels;
extern CSurface*		g_BrothelImages[];
extern cMessageQue		g_MessageQue;

extern int g_Building;
extern int g_CurrBrothel;
extern int g_CurrentScreen;
extern string ReadTextFile(DirPath path, string file);
extern bool g_TryCentre;
extern bool eventrunning;
extern bool g_InitWin;
extern bool g_Cheats;
extern unsigned long	g_Day, g_Month, g_Year;
extern cScreenGetInput	g_GetInput;

extern bool g_CTRLDown;

bool cScreenBrothelManagement::id_set = false;

void cScreenBrothelManagement::set_ids()
{
	id_set				/**/ = true;
	g_LogFile.write("set_ids in cScreenBrothelManagement");

	buildinglabel_id	/**/ = get_id("BuildingLabel", "Header");
	background_id		/**/ = get_id("Background");
	walk_id				/**/ = get_id("WalkButton", "*Unused*");

	weeks_id			/**/ = get_id("Next Week", "Weeks");
	buildingdetails_id	/**/ = get_id("BuildingDetails", "Details");
	girls_id			/**/ = get_id("Girl Management", "Girls");
	staff_id			/**/ = get_id("Staff Management");
	setup_id			/**/ = get_id("Setup", "SetUp");
	dungeon_id			/**/ = get_id("Dungeon");
	turns_id			/**/ = get_id("Turn Summary", "Turn");

	town_id				/**/ = get_id("Visit Town");
	save_id				/**/ = get_id("Save");
	quit_id				/**/ = get_id("Quit");
	prevbrothel_id		/**/ = get_id("PrevButton", "Prev");
	nextbrothel_id		/**/ = get_id("NextButton", "Next");


}
cScreenBrothelManagement::cScreenBrothelManagement()
{
	DirPath dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "brothel_management.xml";
	m_filename = dp.c_str();


}
cScreenBrothelManagement::~cScreenBrothelManagement() { g_LogFile.write("Brothel Shutdown"); }

void cScreenBrothelManagement::init()
{
	g_CurrentScreen = SCREEN_BROTHEL;
	g_Building = BUILDING_BROTHEL;
	if (g_InitWin)
	{
		Focused();
		stringstream ss;
		ss << "Day: " << g_Day << " Month: " << g_Month << " Year: " << g_Year << " -- Brothel: " << g_Brothels.GetName(g_CurrBrothel);
		EditTextItem(ss.str(), buildinglabel_id);
		EditTextItem(g_Brothels.GetBrothelString(g_CurrBrothel), buildingdetails_id);
		SetImage(background_id, g_BrothelImages[g_CurrBrothel]);
		g_InitWin = false;
	}
}

void cScreenBrothelManagement::process()
{
	if (!id_set)		set_ids();
	if (check_keys())	return;
	init();
	if (g_InterfaceEvents.GetNumEvents() != 0)	check_events();
}

void cScreenBrothelManagement::check_events()
{
	g_InitWin = true;
	if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, girls_id))		{ g_WinManager.push("Girl Management"); }
	else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, staff_id))	{ g_WinManager.push("Gangs"); }
	else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, setup_id))	{ g_WinManager.push("Building Setup"); }
	else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, dungeon_id))	{ g_WinManager.push("Dungeon"); }
	else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, town_id))	{ g_WinManager.push("Town"); }
	else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, turns_id))	{ g_WinManager.push("Turn Summary"); }
	else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, weeks_id))
	{
		if (!g_CTRLDown) { g_CTRLDown = false; AutoSaveGame(); }
		NextWeek();
		g_WinManager.push("Turn Summary");
	}
	else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, save_id))
	{
		SaveGame(g_CTRLDown);
		g_MessageQue.AddToQue("Game Saved", COLOR_GREEN);
	}
	else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, quit_id))
	{
		g_GetInput.ModeConfirmExit();
		g_WinManager.push("GetInput");
	}
	else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, nextbrothel_id))
	{
		g_CurrBrothel++;
		if (g_CurrBrothel >= g_Brothels.GetNumBrothels())
			g_CurrBrothel = 0;
	}
	else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, prevbrothel_id))
	{
		g_CurrBrothel--;
		if (g_CurrBrothel < 0)
			g_CurrBrothel = g_Brothels.GetNumBrothels() - 1;
	}
}

bool cScreenBrothelManagement::check_keys()
{
	/*if (g_LeftArrow)
	{
	g_CurrBrothel--;
	if (g_CurrBrothel < 0)
	g_CurrBrothel = g_Brothels.GetNumBrothels() - 1;
	g_InitWin = true;
	g_LeftArrow = false;
	return;
	}
	else if (g_RightArrow)
	{
	g_CurrBrothel++;
	if (g_CurrBrothel >= g_Brothels.GetNumBrothels())
	g_CurrBrothel = 0;
	g_InitWin = true;
	g_RightArrow = false;
	return;
	}*/

	return false;
}