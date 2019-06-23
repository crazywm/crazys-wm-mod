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
#include "cScreenGetInput.h"
#include "cWindowManager.h"
#include "cScreenMainMenu.h"

extern cWindowManager g_WinManager;

extern string g_ReturnText;
extern cScreenMainMenu g_MainMenu;
extern int g_CurrentScreen;
extern long g_IntReturn;
extern bool g_EnterKey;
extern bool g_InitWin;

bool g_ReturnConfirm;

void ResetInterface();

cScreenGetInput::cScreenGetInput()
{
	DirPath dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "getInput.xml";
	m_filename = dp.c_str();
}
cScreenGetInput::~cScreenGetInput() {}

void cScreenGetInput::set_ids()
{
	ids_set			/**/ = true;
	g_LogFile.write("set_ids in cScreenGetInput");

	id_btn_ok		/**/ = get_id("Ok");
	id_btn_cancel	/**/ = get_id("Cancel");
	id_label		/**/ = get_id("Label");
	id_textfield	/**/ = get_id("TextField");
}

void cScreenGetInput::process()
{
	g_CurrentScreen = SCREEN_BROTHEL;
	if (!ids_set)		set_ids();
	if (check_keys())	return;
	init();
	check_events();
}

void cScreenGetInput::init()
{
	g_CurrentScreen = SCREEN_GETINPUT;
	if (!g_InitWin) return;
	Focused();
	g_InitWin = false;
	g_IntReturn = 0;
	g_EnterKey = false;

	switch (m_profile)
	{
	case MODE_STRING:
	{
		HideEditBox(id_textfield, false);
		EditTextItem("Enter Text:", id_textfield);
		break;
	}
	case MODE_INT:
	{
		HideEditBox(id_textfield, false);
		EditTextItem("Enter Value:", id_textfield);
		break;
	}
	case MODE_CONFIRM_EXIT:
	case MODE_CONFIRM:
	{
		HideEditBox(id_textfield, true);
		EditTextItem("Confirm?", id_textfield);
		break;
	}
	}
}

void cScreenGetInput::check_events()
{
	if (g_InterfaceEvents.GetNumEvents() != 0 || g_EnterKey)
	{
		switch (m_profile)
		{
		case (MODE_CONFIRM_EXIT) :
			CheckEvents_ConfirmExit();
			break;
		default:
			if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, id_btn_ok))
			{
				g_EnterKey = false;
				g_ReturnText = GetEditBoxText(id_textfield);
				g_IntReturn = atol(g_ReturnText.c_str());

				g_InitWin = true;
				g_WinManager.Pop();
				return;
			}
			else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, id_btn_cancel))
				g_WinManager.Pop();
		}
	}
}

bool cScreenGetInput::check_keys()
{
	return false;
}

void cScreenGetInput::CheckEvents_ConfirmExit()
{
	if (g_InitWin)
	{
		Focused();
		g_InitWin = false;
	}
	if (g_InterfaceEvents.GetNumEvents() == 0 && !g_EnterKey)	return;
	if (g_InterfaceEvents.CheckButton(id_btn_cancel))
	{
		g_ReturnText = "";
		g_InitWin = true;
		g_WinManager.Pop();
		return;
	}
	if (g_InterfaceEvents.CheckButton(id_btn_ok) || g_EnterKey)
	{
		g_EnterKey = false;
		g_ReturnText = "";
		g_InitWin = true;
		g_WinManager.PopToWindow(&g_MainMenu);
		ResetInterface();

		// Schedule Quit Event -- To quit
		//SDL_Event ev;
		//ev->type = SDL_QUIT;
		//if (!SDL_PushEvent(ev))
		//	g_LogFile.write("SDL Quit Re-Scheduled!");
	}
}