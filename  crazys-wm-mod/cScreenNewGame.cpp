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
#include "cScreenNewGame.h"
#include "cGetStringScreenManager.h"
#include "cScriptManager.h"
#include "cWindowManager.h"
#include "cBrothel.h"

extern void NewGame();
extern void LoadGameScreen();
extern void GetString();

extern cWindowManager g_WinManager;
extern cBrothelManager  g_Brothels;

extern bool g_InitWin;
extern int g_CurrentScreen;

extern bool g_UpArrow;
extern bool g_DownArrow;
extern bool g_LeftArrow;
extern bool g_RightArrow;
extern bool g_HomeKey;
extern bool g_EndKey;
extern bool g_PageUpKey;
extern bool g_PageDownKey;
extern bool g_TabKey;
extern bool g_EscapeKey;

extern string monthnames[13];
extern string g_ReturnText;

int currentbox=0;

bool cScreenNewGame::ids_set = false;

void cScreenNewGame::set_ids()
{
	ids_set = true;
	ok_id = get_id("Ok");
	cancel_id = get_id("Cancel");
	brothel_id = get_id("BrothelName");
	pname_id = get_id("PlayerName");
	psname_id = get_id("PlayerSurname");
	pbm_id = get_id("PlayerBirthMonth");
	pbm1_id = get_id("PlayerBirthMonthNum");
	pbd_id = get_id("PlayerBirthDay");
	pbd1_id = get_id("PlayerBirthDayNum");


}

void cScreenNewGame::init()
{
	g_CurrentScreen = SCREEN_NEWGAME;
	if (g_InitWin)
	{
		Focused();
		g_InitWin = false;
		g_Girls.GetImgManager()->LoadList("Default");
	}

	if (!g_InitWin) return;

	Focused();
	g_InitWin = false;


}

void cScreenNewGame::process()
{
	// we need to make sure the ID variables are set
	if (!ids_set) set_ids();

	// handle arrow keys
	if (check_keys()) return;

	// set up the window if needed
	init();

	// check to see if there's a button event needing handling
	check_events();
}

void cScreenNewGame::check_events()
{
	// no events means we can go home
	if (g_InterfaceEvents.GetNumEvents() == 0) return;

	if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, cancel_id))
	{
		g_InitWin = true;
		g_WinManager.Pop();
		return;
	}

	if (g_InterfaceEvents.CheckSlider(pbd_id))
	{
		stringstream ss;
		g_Brothels.GetPlayer()->SetBirthDay(SliderValue(pbd_id));
		SliderValue(pbd_id, g_Brothels.GetPlayer()->BirthDay());
		ss << g_Brothels.GetPlayer()->BirthDay();
		EditTextItem(ss.str(), pbd1_id);
		return;
	}
	if (g_InterfaceEvents.CheckSlider(pbm_id))
	{
		stringstream ss;
		g_Brothels.GetPlayer()->SetBirthMonth(SliderValue(pbm_id));
		SliderValue(pbm_id, g_Brothels.GetPlayer()->BirthMonth());
		ss << monthnames[g_Brothels.GetPlayer()->BirthMonth()];
		EditTextItem(ss.str(), pbm1_id);
		return;
	}


	if (g_InterfaceEvents.CheckButton(ok_id)) 
	{
		string b = GetEditBoxText(brothel_id);
		string p = GetEditBoxText(pname_id);
		string s = GetEditBoxText(psname_id);
		if (b.size() < 1 || p.size() < 1 || s.size() < 1)
		{
			g_MessageQue.AddToQue("You must enter a name in all 3 boxes.", COLOR_RED);
			return;
		}
		else	// ready to start the game now
		{
			g_ReturnText = b;
			g_Brothels.GetPlayer()->m_FirstName = p;
			g_Brothels.GetPlayer()->m_Surname = s;
			g_Brothels.GetPlayer()->m_RealName = p+" "+s;

			g_InitWin = true;
			g_WinManager.Pop();
			NewGame();
		}

	}
}

bool cScreenNewGame::check_keys()
{
	bool mod = true;
//	g_UpArrow;
//	g_DownArrow;
//	g_LeftArrow;
//	g_RightArrow;

	if (g_TabKey || g_DownArrow)
	{
		mod = g_DownArrow = g_TabKey = false;
		currentbox++;
	}
	if (g_TabKey || g_UpArrow)
	{
		mod = g_UpArrow = false;
		currentbox--;
	}
	
	if (g_EscapeKey)
	{
		g_EscapeKey = false;
		m_EditBoxes[currentbox]->ClearText();
		return true;
	}
	if (g_HomeKey || (currentbox == 4 && g_RightArrow))
	{
		mod = g_RightArrow = g_HomeKey = false;
		g_Brothels.GetPlayer()->BirthDay(1);
	}
	if (g_EndKey || (currentbox == 4 && g_LeftArrow))
	{
		mod = g_LeftArrow = g_EndKey = false;
		g_Brothels.GetPlayer()->BirthDay(-1);
	}
	if (g_PageUpKey || (currentbox == 3 && g_RightArrow))
	{
		mod = g_RightArrow = g_PageUpKey = false;
		g_Brothels.GetPlayer()->BirthMonth(1);
	}
	if (g_PageDownKey || (currentbox == 3 && g_LeftArrow))
	{
		mod = g_LeftArrow = g_PageDownKey = false;
		g_Brothels.GetPlayer()->BirthMonth(-1);
	}

	if (!mod)
	{
		stringstream ss;
		SliderValue(pbd_id, g_Brothels.GetPlayer()->BirthDay());
		ss << g_Brothels.GetPlayer()->BirthDay();
		EditTextItem(ss.str(), pbd1_id);
		ss.str("");
		SliderValue(pbm_id, g_Brothels.GetPlayer()->BirthMonth());
		ss << monthnames[g_Brothels.GetPlayer()->BirthMonth()];
		EditTextItem(ss.str(), pbm1_id);

		if (currentbox >= (int)m_EditBoxes.size() + (int)m_Sliders.size()) currentbox = 0;
		if (currentbox < 0) currentbox = (int)m_EditBoxes.size() + (int)m_Sliders.size();

		for (int i = 0; i < (int)m_EditBoxes.size(); i++) m_EditBoxes[i]->m_HasFocus = false;
		if (currentbox<3)	m_EditBoxes[currentbox]->m_HasFocus = true;

		m_Sliders[0]->IsActive(currentbox == 3);
		m_Sliders[1]->IsActive(currentbox == 4);


		return true;
	}





	return false;
}