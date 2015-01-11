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
#include "cScreenMainMenu.h"
#include "cGetStringScreenManager.h"
#include "cScriptManager.h"
#include "cWindowManager.h"

extern void NewGame();
extern void LoadGameScreen();
extern void GetString();

extern cWindowManager g_WinManager;

extern bool g_InitWin;
extern int g_CurrentScreen;

bool cScreenMainMenu::ids_set = false;

void cScreenMainMenu::set_ids()
{
	ids_set = true;
	new_id = get_id("New Game");
	load_id = get_id("Load Game");
	quit_id = get_id("Quit Game");
}

void cScreenMainMenu::init()
{
	g_CurrentScreen = SCREEN_MAINMENU;
	if (g_InitWin)
	{
		Focused();
		g_InitWin = false;
		g_Girls.GetImgManager()->LoadList("Default");
	}
}

void cScreenMainMenu::process()
{
	// we need to make sure the ID variables are set
	if (!ids_set)
		set_ids();

	// handle arrow keys
	if (check_keys())
		return;

	// set up the window if needed
	init();

	// check to see if there's a button event needing handling
	check_events();
}

void cScreenMainMenu::check_events()
{
	// no events means we can go home
	if (g_InterfaceEvents.GetNumEvents() == 0)
		return;

	if (g_InterfaceEvents.CheckButton(new_id))
	{
#if 0
		// the old new game code
		cGetStringScreenManager gssm;
		gssm.empty_allowed(false);
		gssm.set_handler(NewGame);
		g_WinManager.Push(GetString, &g_GetString);
		g_MessageQue.AddToQue("Enter a name for your first brothel.", 0);
#else
		// the new new game code
		g_WinManager.push("New Game");
#endif
		g_InitWin = true;
		return;
	}

	if (g_InterfaceEvents.CheckButton(load_id))
	{
		g_WinManager.Push(LoadGameScreen, &g_LoadGame);
		g_InitWin = true;
		return;
	}

	if (g_InterfaceEvents.CheckButton(quit_id))
	{
		SDL_Event evn;
		evn.type = SDL_QUIT;
		SDL_PushEvent(&evn);
	}
}

bool cScreenMainMenu::check_keys()
{
	return false;
}