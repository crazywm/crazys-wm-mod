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
#include "cWindowManager.h"
#include "cMessageBox.h"
#include "InterfaceProcesses.h"
#include "InterfaceGlobals.h"
#include "cScreenBrothelManagement.h"
#include "cScreenGetInput.h"
#include "libintl.h"

extern CSurface *g_BrothelImages[];

extern cBrothelManager g_Brothels;
extern cWindowManager g_WinManager;
extern cMessageQue g_MessageQue;

extern int g_CurrentScreen;
extern int g_CurrBrothel;
extern unsigned long g_Day, g_Month, g_Year;
extern bool g_InitWin;

extern cScreenGetInput g_GetInput;

extern void Turnsummary();
extern void SaveGameXML();

extern bool g_CTRLDown;

bool cScreenBrothelManagement::id_set = false;

void cScreenBrothelManagement::set_ids()
{
	id_set = true;

	id_header = get_id("Header");
	id_details = get_id("Details");
	id_image = get_id("Image");
	id_girls = get_id("Girl Management");
	id_staff = get_id("Staff Management");
	id_setup = get_id("Setup");
	id_dungeon = get_id("Dungeon");
	id_town = get_id("Visit Town");
	id_week = get_id("Next Week");
	id_save = get_id("Save");
	id_turn = get_id("Turn Summary");
	id_quit = get_id("Quit");
	id_prev = get_id("Prev");
	id_next = get_id("Next");
}

cScreenBrothelManagement::cScreenBrothelManagement()
{
	
	DirPath dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "brothel_management.xml";
	m_filename = dp.c_str();
}

cScreenBrothelManagement::~cScreenBrothelManagement()
{
}

void cScreenBrothelManagement::init()
{
	if (g_InitWin)
	{
		Focused();

		stringstream ss;
		ss << gettext("Day: ") << g_Day << gettext(" Month: ") << g_Month << gettext(" Year: ") << g_Year << gettext(" -- Brothel: ") << g_Brothels.GetName(g_CurrBrothel);
		EditTextItem(ss.str(), id_header);
		EditTextItem(g_Brothels.GetBrothelString(g_CurrBrothel), id_details);
		g_InitWin = false;
		// selected_girl = 0;

		SetImage(id_image, g_BrothelImages[g_CurrBrothel]);
	}
}

void cScreenBrothelManagement::process()
{
	g_CurrentScreen = SCREEN_BROTHEL;

	if (!id_set)
		set_ids();

	if (check_keys())
		return;

	init();

	check_events();
}

void cScreenBrothelManagement::check_events()
{
	if (g_InterfaceEvents.GetNumEvents() != 0)
	{
		if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, id_girls))
		{
			g_InitWin = true;
			g_WinManager.push("Girl Management");
			return;
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, id_staff))
		{
			g_InitWin = true;
			g_WinManager.push("Gangs");
			return;
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, id_setup))
		{
			g_InitWin = true;
			g_WinManager.push("Building Setup");
			return;
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, id_dungeon))
		{
			g_InitWin = true;
			g_WinManager.push("Dungeon");
			return;
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, id_town))
		{
			g_InitWin = true;
			g_WinManager.push("Town");
			return;
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, id_save))
		{
			SaveGame(g_CTRLDown);
			g_MessageQue.AddToQue("Game Saved", COLOR_GREEN);
			return;
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, id_week))
		{
			g_InitWin = true;
			AutoSaveGame();
			NextWeek();
			g_WinManager.Push(Turnsummary, &g_Turnsummary);
			return;
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, id_turn))
		{
			g_InitWin = true;
			g_CurrentScreen = SCREEN_TURNSUMMARY;
			g_WinManager.Push(Turnsummary, &g_Turnsummary);
			return;
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, id_quit))
		{
			g_InitWin = true;
			g_GetInput.ModeConfirmExit();
			g_WinManager.push("GetInput");
			return;
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, id_next))
		{
			g_CurrBrothel++;
			if (g_CurrBrothel >= g_Brothels.GetNumBrothels())
				g_CurrBrothel = 0;
			g_InitWin = true;
			return;
		}
		else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, id_prev))
		{
			g_CurrBrothel--;
			if (g_CurrBrothel < 0)
				g_CurrBrothel = g_Brothels.GetNumBrothels() - 1;
			g_InitWin = true;
			return;
		}
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