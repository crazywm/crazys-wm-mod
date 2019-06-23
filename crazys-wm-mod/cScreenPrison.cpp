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
#include "cScreenPrison.h"
#include "cWindowManager.h"
#include "cGold.h"
#include "sFacilityList.h"
#include "cGetStringScreenManager.h"

#ifdef LINUX
#include "linux.h"
#endif

extern	bool			g_InitWin;
extern	int			g_CurrBrothel;
extern	cGold			g_Gold;
extern	cBrothelManager		g_Brothels;
extern	cWindowManager		g_WinManager;
extern	cInterfaceEventManager	g_InterfaceEvents;

extern	bool	g_LeftArrow;
extern	bool	g_RightArrow;
extern	bool	g_UpArrow;
extern	bool	g_DownArrow;
extern	bool	g_AltKeys;	// New hotkeys --PP
extern	bool	g_EnterKey;
extern	bool	g_SpaceKey;
extern	bool	g_Q_Key;
extern	bool	g_W_Key;
extern	bool	g_E_Key;
extern	bool	g_A_Key;
extern	bool	g_S_Key;
extern	bool	g_D_Key;
extern	bool	g_Z_Key;
extern	bool	g_X_Key;
extern	bool	g_C_Key;
extern	int		g_CurrentScreen;

static int lastNum = -1;
static int ImageNum = -1;
extern sGirl *selected_girl;

cScreenPrison::cScreenPrison()
{
	DirPath dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "prison_screen.xml";
	m_filename = dp.c_str();
	selection = -1;
}
cScreenPrison::~cScreenPrison() {}

void cScreenPrison::init()
{
	stringstream ss;
	g_CurrentScreen = SCREEN_JAIL;
	if (!g_InitWin) { return; }
	Focused();
	g_InitWin = false;

	DisableButton(more_id, true);
	DisableButton(release_id, true);
	selection = -1;
	update_details();

	int i = 0;
	ClearListBox(prison_list_id);
	sGirl* pgirls = g_Brothels.GetPrison();
	while (pgirls)
	{
		stringstream ss;
		int cost = PrisonReleaseCost(pgirls);
		ss << pgirls->m_Realname << "  (release cost: " << cost << " gold)";
		AddToListBox(prison_list_id, i, ss.str());
		i++;
		pgirls = pgirls->m_Next;
	}
}

bool cScreenPrison::ids_set = false;

void cScreenPrison::set_ids()
{
	ids_set			/**/ = true;
	g_LogFile.write("set_ids in cScreenPrison");

	header_id			/**/ = get_id("ScreenHeader");
	back_id				/**/ = get_id("BackButton","Back");
	more_id				/**/ = get_id("ShowMoreButton");
	release_id			/**/ = get_id("ReleaseButton");
	prison_list_id		/**/ = get_id("PrisonList");
	girl_desc_id		/**/ = get_id("GirlDescription");
	girlimage_id		/**/ = get_id("GirlImage", "*Unused*");//
	DetailLevel			/**/ = 0;
}

bool cScreenPrison::check_keys()
{
	if (g_UpArrow || g_DownArrow || (g_AltKeys && (g_A_Key || g_D_Key)))
	{
		selection = ((g_UpArrow || g_A_Key) ? ArrowUpListBox(prison_list_id) : ArrowDownListBox(prison_list_id));
		g_UpArrow = g_A_Key = g_DownArrow = g_D_Key = false;
		SetSelectedItemInList(prison_list_id, selection);
		return true;
	}
	return false;
}

void cScreenPrison::process()
{
	if (!ids_set)		set_ids();						// we need to make sure the ID variables are set
	if (check_keys())	return;							// handle arrow keys
	init();
	if (g_InterfaceEvents.GetNumEvents() == 0) return;	// no events means we can go home

	if (g_InterfaceEvents.CheckButton(back_id))			// if it's the back button, pop the window off the stack and we're done
	{
		g_InitWin = true;
		g_WinManager.Pop();
		return;
	}
	if (g_InterfaceEvents.CheckButton(more_id))			// The More button to switch between girl details
	{
		more_button();
		return;
	}
	if (g_InterfaceEvents.CheckButton(release_id))		// The Release button to release selected girl from prison
	{
		release_button();
		g_InitWin = true;
		return;
	}
	if (g_InterfaceEvents.CheckListbox(prison_list_id))	// catch a selection change event for the listbox
	{
		selection = GetSelectedItemFromList(prison_list_id);
		selection_change();
		return;
	}
}

void cScreenPrison::selection_change()
{
	DisableButton(more_id, (selection == -1));
	DisableButton(release_id, (selection == -1));
	update_details();
}

void cScreenPrison::update_details()
{
	EditTextItem("No Prisoner Selected", girl_desc_id);
	if (selection == -1) return;
	sGirl* pgirls = get_selected_girl();
	if (!pgirls) return;

	if (DetailLevel == 1)		EditTextItem(g_Girls.GetMoreDetailsString(pgirls, true), girl_desc_id);
	else if (DetailLevel == 2)	EditTextItem(g_Girls.GetThirdDetailsString(pgirls), girl_desc_id);
	else						EditTextItem(g_Girls.GetDetailsString(pgirls, true), girl_desc_id);
}

sGirl* cScreenPrison::get_selected_girl()
{
	if (selection == -1) return 0;
	sGirl* pgirls = g_Brothels.GetPrison();
	int i = 0;
	while (pgirls)
	{
		if (i == selection) break;
		i++;
		pgirls = pgirls->m_Next;
	}
	return pgirls;
}

void cScreenPrison::more_button()
{
	if (DetailLevel == 0)		DetailLevel = 1;
	else if (DetailLevel == 1)	DetailLevel = 2;
	else						DetailLevel = 0;
	update_details();
}

void cScreenPrison::release_button()
{
	if (selection == -1) return;
	sGirl* pgirls = get_selected_girl();
	if (!pgirls) return;
	int cost = PrisonReleaseCost(pgirls);
	if (!g_Gold.afford((double)cost))
	{
		g_MessageQue.AddToQue("You don't have enough gold", 0);
		return;
	}
	g_Gold.item_cost((double)cost);
	g_Gold.girl_support((double)cost);
	g_Brothels.RemoveGirlFromPrison(pgirls);
	if (g_Brothels.GetFreeRooms(g_CurrBrothel) < 1)
	{
		string text = pgirls->m_Realname;
		text += " has been sent to your dungeon, since current brothel is full.";
		g_MessageQue.AddToQue(text, 0);
		g_Brothels.GetDungeon()->AddGirl(pgirls, DUNGEON_NEWGIRL);
	}
	else
	{
		string text = pgirls->m_Realname;
		text += " has been sent to your current brothel.";
		g_MessageQue.AddToQue(text, 0);
		g_Brothels.AddGirl(g_CurrBrothel, pgirls);
	}
}

int cScreenPrison::PrisonReleaseCost(sGirl* girl)
{
	g_Girls.CalculateAskPrice(girl, false);
	int cost = girl->askprice() * 15;
	cost += g_Girls.GetSkillWorth(girl);
	if (girl->check_virginity()) cost += int(cost / 2);	//	`J` fixed virgin adds half cost more
	cost *= 2;
	return cost;
}

void cScreenPrison::update_image()
{
	if ((selected_girl) && !IsMultiSelected(prison_list_id))//This may need fixed CRAZY
	{
		PrepareImage(girlimage_id, selected_girl, IMGTYPE_JAIL, true, ImageNum);
		HideImage(girlimage_id, false);
	}
	else HideImage(girlimage_id, true);
}
