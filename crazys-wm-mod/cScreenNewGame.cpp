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
#include "cWindowManager.h"
#include "cBrothel.h"

extern cWindowManager g_WinManager;
extern cBrothelManager  g_Brothels;
extern bool g_InitWin;
extern int g_CurrentScreen;
extern string monthnames[13];
extern string g_ReturnText;
extern int g_ReturnInt;
extern cPlayer* The_Player;

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

extern bool g_1_Key;
extern bool g_2_Key;
extern bool g_3_Key;
extern bool g_4_Key;
extern bool g_5_Key;
extern bool g_6_Key;
extern bool g_7_Key;
extern bool g_8_Key;
extern bool g_9_Key;
extern bool g_0_Key;

extern bool g_F1_Key;
extern bool g_F2_Key;
extern bool g_F3_Key;
extern bool g_F4_Key;
extern bool g_F5_Key;
extern bool g_F6_Key;
extern bool g_F7_Key;
extern bool g_F8_Key;
extern bool g_F9_Key;
extern bool g_F10_Key;
extern bool g_F11_Key;
extern bool g_F12_Key;

int currentbox=0;

bool cScreenNewGame::ids_set = false;

cScreenNewGame::cScreenNewGame()
{
	DirPath dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "NewGame.xml";
	m_filename = dp.c_str();
}

void cScreenNewGame::set_ids()
{
	ids_set			/**/ = true;
	g_LogFile.write("set_ids in cScreenNewGame");

	ok_id			/**/ = get_id("Ok");
	cancel_id		/**/ = get_id("Cancel");
	brothel_id		/**/ = get_id("BrothelName");
	pname_id		/**/ = get_id("PlayerName");
	psname_id		/**/ = get_id("PlayerSurname");
	pbm_id			/**/ = get_id("PlayerBirthMonth");
	pbm1_id			/**/ = get_id("PlayerBirthMonthNum");
	pbd_id			/**/ = get_id("PlayerBirthDay");
	pbd1_id			/**/ = get_id("PlayerBirthDayNum");
	phn_id			/**/ = get_id("PlayerHoroscope");
}

void cScreenNewGame::init()
{
	g_CurrentScreen = SCREEN_NEWGAME;
	if (g_InitWin) { Focused(); g_InitWin = false; }
	if (!g_InitWin) return;
	Focused();
	g_InitWin = false;
}

void cScreenNewGame::process()
{
	if (!ids_set) set_ids();	// we need to make sure the ID variables are set
	if (check_keys()) return;	// handle arrow keys
	init();						// set up the window if needed
	check_events();				// check to see if there's a button event needing handling
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

	if (g_InterfaceEvents.CheckSlider(pbd_id) || g_InterfaceEvents.CheckSlider(pbm_id))
	{
		stringstream ss;
		The_Player->SetBirthDay(SliderValue(pbd_id));
		SliderValue(pbd_id, The_Player->BirthDay());
		The_Player->SetBirthMonth(SliderValue(pbm_id));
		SliderValue(pbm_id, The_Player->BirthMonth());
		ss << The_Player->BirthDay();
		EditTextItem(ss.str(), pbd1_id);
		ss.str(""); ss << monthnames[The_Player->BirthMonth()];
		EditTextItem(ss.str(), pbm1_id);
		EditTextItem(g_Girls.GetHoroscopeName(The_Player->BirthMonth(), The_Player->BirthDay()), phn_id);
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
			g_ReturnInt = 1;
			g_ReturnText = b;
			The_Player->SetFirstName(p);
			The_Player->SetSurname(s);
			The_Player->SetRealName(p + " " + s);

			g_InitWin = true;
			g_WinManager.Pop();
			g_WinManager.push("Preparing Game");
			return;
		}
	}
}

bool cScreenNewGame::check_keys()
{
	bool mod = true;
	if (g_TabKey || g_DownArrow)
	{
		mod = g_DownArrow = g_TabKey = false;
		currentbox++;
	}
	if (g_UpArrow)
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
	if (g_PageDownKey || (currentbox == 3 && g_LeftArrow))
	{
		mod = g_LeftArrow = g_PageDownKey = false;
		The_Player->BirthMonth(-1);
	}
	if (g_PageUpKey || (currentbox == 3 && g_RightArrow))
	{
		mod = g_RightArrow = g_PageUpKey = false;
		The_Player->BirthMonth(1);
	}
	if (g_EndKey || (currentbox == 4 && g_LeftArrow))
	{
		mod = g_LeftArrow = g_EndKey = false;
		The_Player->BirthDay(-1);
	}
	if (g_HomeKey || (currentbox == 4 && g_RightArrow))
	{
		mod = g_RightArrow = g_HomeKey = false;
		The_Player->BirthDay(1);
	}
	if (currentbox == 4 && (g_1_Key || g_2_Key || g_3_Key || g_4_Key || g_5_Key || g_6_Key || g_7_Key || g_8_Key || g_9_Key || g_0_Key))
	{
		// `J` I'm sure this can be done better but this will do for now.
		mod = false;
		int tmp = 0;
		int cur = The_Player->BirthDay();
		int fin = 0;

		if (g_1_Key)	{ g_1_Key = false; tmp = 1; }
		if (g_2_Key)	{ g_2_Key = false; tmp = 2; }
		if (g_3_Key)	{ g_3_Key = false; tmp = 3; }
		if (g_4_Key)	{ g_4_Key = false; tmp = 4; }
		if (g_5_Key)	{ g_5_Key = false; tmp = 5; }
		if (g_6_Key)	{ g_6_Key = false; tmp = 6; }
		if (g_7_Key)	{ g_7_Key = false; tmp = 7; }
		if (g_8_Key)	{ g_8_Key = false; tmp = 8; }
		if (g_9_Key)	{ g_9_Key = false; tmp = 9; }
		if (g_0_Key)	{ g_0_Key = false; tmp = 0; }

		/* */if (cur == 0)	fin = tmp;
		else if (cur > 9)	fin = ((cur % 10) * 10) + tmp;
		else if (cur < 10)	fin = (cur * 10) + tmp;
		if (fin>30)fin = 30;

		The_Player->SetBirthDay(fin);
	}

	if (g_F1_Key)	{ mod = g_F1_Key = false; The_Player->SetBirthMonth(1); }
	if (g_F2_Key)	{ mod = g_F2_Key = false; The_Player->SetBirthMonth(2); }
	if (g_F3_Key)	{ mod = g_F3_Key = false; The_Player->SetBirthMonth(3); }
	if (g_F4_Key)	{ mod = g_F4_Key = false; The_Player->SetBirthMonth(4); }
	if (g_F5_Key)	{ mod = g_F5_Key = false; The_Player->SetBirthMonth(5); }
	if (g_F6_Key)	{ mod = g_F6_Key = false; The_Player->SetBirthMonth(6); }
	if (g_F7_Key)	{ mod = g_F7_Key = false; The_Player->SetBirthMonth(7); }
	if (g_F8_Key)	{ mod = g_F8_Key = false; The_Player->SetBirthMonth(8); }
	if (g_F9_Key)	{ mod = g_F9_Key = false; The_Player->SetBirthMonth(9); }
	if (g_F10_Key)	{ mod = g_F10_Key = false;  The_Player->SetBirthMonth(10); }
	if (g_F11_Key)	{ mod = g_F11_Key = false;  The_Player->SetBirthMonth(11); }
	if (g_F12_Key)	{ mod = g_F12_Key = false;  The_Player->SetBirthMonth(12); }

	if (!mod)
	{
		stringstream ss;
		SliderValue(pbd_id, The_Player->BirthDay());
		ss << The_Player->BirthDay();
		EditTextItem(ss.str(), pbd1_id);
		ss.str("");
		SliderValue(pbm_id, The_Player->BirthMonth());
		ss << monthnames[The_Player->BirthMonth()];
		EditTextItem(ss.str(), pbm1_id);

		if (currentbox >= (int)m_EditBoxes.size() + (int)m_Sliders.size()) currentbox = 0;
		if (currentbox < 0) currentbox = (int)m_EditBoxes.size() + (int)m_Sliders.size();

		for (int i = 0; i < (int)m_EditBoxes.size(); i++) m_EditBoxes[i]->m_HasFocus = false;
		if (currentbox<3)	m_EditBoxes[currentbox]->m_HasFocus = true;

		m_Sliders[0]->IsActive(currentbox == 3);
		m_Sliders[1]->IsActive(currentbox == 4);
		EditTextItem(g_Girls.GetHoroscopeName(The_Player->BirthMonth(), The_Player->BirthDay()), phn_id);
		return true;
	}
	return false;
}