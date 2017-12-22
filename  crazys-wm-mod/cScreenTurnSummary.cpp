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
#pragma region //	Includes and Externs			//
#include <iostream>
#include <locale>
#include <sstream>
#include "cScreenTurnSummary.h"

#include "cBrothel.h"
#include "cMovieStudio.h"
#include "cArena.h"
#include "cCentre.h"
#include "cClinic.h"
#include "cFarm.h"
#include "cHouse.h"

#include "cGangs.h"
#include "cScreenGirlDetails.h"
#include "InterfaceProcesses.h"
#include "cScriptManager.h"
#include "cGetStringScreenManager.h"
#include "cGold.h"
#include "cScreenBrothelManagement.h"

#include "cWindowManager.h"
#include "sFacilityList.h"
#include "FileList.h"

extern	bool					g_InitWin;
extern	int						g_CurrBrothel;
extern	int						g_CurrClinic;
extern	int						g_CurrStudio;
extern	int						g_CurrArena;
extern	int						g_CurrCentre;
extern	int						g_CurrFarm;
extern	int						g_CurrHouse;
extern	int						g_CurrentScreen;
extern	cBrothelManager			g_Brothels;
extern	cClinicManager			g_Clinic;
extern	cMovieStudioManager		g_Studios;
extern	cArenaManager			g_Arena;
extern	cCentreManager			g_Centre;
extern	cFarmManager			g_Farm;
extern	cHouseManager			g_House;
extern	int						g_Building;

extern	cGangManager			g_Gangs;
extern	cGold					g_Gold;
extern cScreenBrothelManagement g_BrothelManagement;
extern	cWindowManager			g_WinManager;
extern	cInterfaceEventManager	g_InterfaceEvents;
extern	cScreenGirlDetails		g_GirlDetails;
extern	bool					g_Cheats;
extern	bool					eventrunning;
extern	bool					g_AllTogle;

extern	CSurface*				g_BrothelImages[];
extern	string					pic_types[];

extern	void					GetString();
extern	cInterfaceWindow		g_GetString;
extern	sGirl*					selected_girl;

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
extern	bool	g_O_Key;
extern	bool	g_CTRLDown;
extern  bool    g_EnterKey; // More Hotkeys

#pragma endregion
#pragma region //	Local Variables			//
bool cScreenTurnSummary::ids_set = false;
enum{
	Summary_GIRLS,
	Summary_GANGS,
	Summary_BROTHELS,
	Summary_DUNGEON,
	Summary_STUDIO,
	Summary_ARENA,
	Summary_CENTRE,
	Summary_CLINIC,
	Summary_FARM,
	Summary_HOUSE
};

static int	Category = -1;
static int	Category_Last = -1;
static bool	Category_Change = false;
static int	Item = -1;
static int	Item_Last = -1;
static bool	Item_Change = false;
static int	Event = -1;
static int	Event_Last = -1;
static bool	Event_Change = false;
static int	Image = -1;
static int	Image_Last = -1;
static int	Image_Type = -1;
static bool	Image_Change = false;
static int summarysortorder = 0;	// the order girls get sorted in the summary lists
#pragma endregion

cScreenTurnSummary::cScreenTurnSummary()
{
	DirPath dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "TurnSummary.xml";
	m_filename = dp.c_str();
}
cScreenTurnSummary::~cScreenTurnSummary() {}

void cScreenTurnSummary::set_ids()
{
	ids_set			/**/ = true;
	g_LogFile.write("set_ids in cScreenTurnSummary");

	back_id			/**/ = get_id("BackButton", "Back");
	brothel_id		/**/ = get_id("CurrentBrothel");
	category_id		/**/ = get_id("Category");
	gold_id			/**/ = get_id("Gold", "*Unused*");//
	labelitem_id	/**/ = get_id("LabelItem");
	item_id			/**/ = get_id("Item");
	event_id		/**/ = get_id("Event");
	labeldesc_id	/**/ = get_id("LabelDesc");
	goto_id			/**/ = get_id("GoTo");
	nextweek_id		/**/ = get_id("NextWeek");
	prev_id			/**/ = get_id("PrevButton","Prev");
	next_id			/**/ = get_id("NextButton","Next");
	image_id		/**/ = get_id("Background");
	imagename_id	/**/ = get_id("ImageName", "*Unused*");//
}

void cScreenTurnSummary::process()
{
	if (!ids_set) set_ids();		// we need to make sure the ID variables are set
	init();							// set up the window if needed
	if (!check_events()) return;	// check to see if there's a button event needing handling

	if (Category_Change)
	{
		ClearListBox(item_id);
		ClearListBox(event_id);
		/* */if (Category == Summary_GANGS)		Fill_Items_GANGS();
		else if (Category == Summary_BROTHELS)	Fill_Items_BROTHELS();
		else if (Category == Summary_DUNGEON)	Fill_Items_DUNGEON();
		else if (Category == Summary_STUDIO)	Fill_Items_STUDIO();
		else if (Category == Summary_ARENA)		Fill_Items_ARENA();
		else if (Category == Summary_CENTRE)	Fill_Items_CENTRE();
		else if (Category == Summary_CLINIC)	Fill_Items_CLINIC();
		else if (Category == Summary_FARM)		Fill_Items_FARM();
		else if (Category == Summary_HOUSE)		Fill_Items_HOUSE();
		else { Category = Summary_GIRLS;		Fill_Items_GIRLS(); }

		SetSelectedItemInList(category_id, Category);
		if (Item >= GetListBoxSize(item_id) || Item < 0)	Item = 0;
		SetSelectedItemInList(item_id, Item);

		Category_Change = false;
		Item_Change = true;
	}
	if (Item_Change)
	{
		ClearListBox(event_id);
		/* */if (Category == Summary_GANGS)		Fill_Events_GANGS();
		else if (Category == Summary_BROTHELS)	Fill_Events_BROTHELS();
		else if (GetSelectedTextFromList(item_id) == "") selected_girl = 0;
		else
		{
			switch (Category)
			{
			case Summary_DUNGEON:
				if (g_Brothels.GetDungeon()->GetGirlByName(GetSelectedTextFromList(item_id)))
					selected_girl = g_Brothels.GetDungeon()->GetGirlByName(GetSelectedTextFromList(item_id))->m_Girl;
				break;
			case Summary_STUDIO:	selected_girl = g_Studios.GetGirlByName(0, GetSelectedTextFromList(item_id));				break;
			case Summary_ARENA:		selected_girl = g_Arena.GetGirlByName(0, GetSelectedTextFromList(item_id));					break;
			case Summary_CENTRE:	selected_girl = g_Centre.GetGirlByName(0, GetSelectedTextFromList(item_id));				break;
			case Summary_CLINIC:	selected_girl = g_Clinic.GetGirlByName(0, GetSelectedTextFromList(item_id));				break;
			case Summary_FARM:		selected_girl = g_Farm.GetGirlByName(0, GetSelectedTextFromList(item_id));					break;
			case Summary_HOUSE:		selected_girl = g_House.GetGirlByName(0, GetSelectedTextFromList(item_id));					break;
			case Summary_GIRLS:		selected_girl = g_Brothels.GetGirlByName(g_CurrBrothel, GetSelectedTextFromList(item_id));	break;
			default:				selected_girl = 0;
			}

			Fill_Events(selected_girl);
		}

		Event = 0;
		SetSelectedItemInList(event_id, Event);
		Item_Change = false;
		Event_Change = true;
		Image_Change = true;
	}
	if (Event_Change && Event >= 0)
	{
		string text = "";
		if (selected_girl && (Category == Summary_DUNGEON || Category == Summary_STUDIO || Category == Summary_ARENA
			|| Category == Summary_CENTRE || Category == Summary_CLINIC || Category == Summary_FARM
			|| Category == Summary_HOUSE || Category == Summary_GIRLS))
		{
			if (!selected_girl->m_Events.IsEmpty())
			{
				text = selected_girl->m_Events.GetMessage(Event).m_Message;
				Image_Type = selected_girl->m_Events.GetMessage(Event).m_MessageType;
			}
			Image_Change = true;
		}
		else if (Category == Summary_GANGS)
		{
			if (g_Gangs.GetGang(Item) && !g_Gangs.GetGang(Item)->m_Events.IsEmpty())
				text = g_Gangs.GetGang(Item)->m_Events.GetMessage(Event).m_Message;
		}
		else if (Category == Summary_BROTHELS)
		{
			if (g_Brothels.GetBrothel(Item) && !g_Brothels.GetBrothel(Item)->m_Events.IsEmpty())
				text = g_Brothels.GetBrothel(Item)->m_Events.GetMessage(Event).m_Message;
		}
		EditTextItem(text, labeldesc_id);
		Item_Last = -1;
		Event_Change = false;
	}

	EditTextItem(g_Brothels.GetName(g_CurrBrothel), brothel_id);

	// Draw the image
	if (Category == Summary_BROTHELS)
	{
		SetImage(image_id, g_BrothelImages[Item]);
		if (imagename_id >= 0)	m_TextItems[imagename_id]->SetText("");
	}
	else if (Category == Summary_GANGS)
	{
		SetImage(image_id, g_BrothelImages[g_CurrBrothel]);
		if (imagename_id >= 0)	m_TextItems[imagename_id]->SetText("");
	}
	else if (selected_girl && Image_Change)
	{
		Image_Change = false;
		bool random = true;
		if ((selected_girl->m_newRandomFixed >= 0) && (Image_Type == IMGTYPE_PROFILE))
		{
			random = false;
			Image = selected_girl->m_newRandomFixed;
		}
		PrepareImage(image_id, selected_girl, Image_Type, random, Image);
		if (imagename_id >= 0)
		{
			string t = "";
			if (m_Images[image_id]) t = m_Images[image_id]->m_Image->GetFilename();
			m_TextItems[imagename_id]->SetText(t);
		}
	}
	else if (Image_Change)
	{
		m_Images[image_id]->m_Image = new CSurface(ImagePath("blank.png"));
		m_Images[image_id]->m_AnimatedImage = 0;
		m_Images[image_id]->m_Image->m_Message = "";
		if (imagename_id >= 0)	m_TextItems[imagename_id]->SetText("");
	}

}
void cScreenTurnSummary::init()
{
	g_CurrentScreen = SCREEN_TURNSUMMARY;
	if (!g_InitWin) return;
	g_InitWin = false;
	Focused();

	if (selected_girl)
	{
		if (selected_girl->m_DayJob == JOB_INDUNGEON)	Category = Summary_DUNGEON;
		else if (selected_girl->m_InClinic)/*      */	Category = Summary_CLINIC;
		else if (selected_girl->m_InStudio)/*      */	Category = Summary_STUDIO;
		else if (selected_girl->m_InArena)/*       */	Category = Summary_ARENA;
		else if (selected_girl->m_InCentre)/*      */	Category = Summary_CENTRE;
		else if (selected_girl->m_InHouse)/*       */	Category = Summary_HOUSE;
		else if (selected_girl->m_InFarm)/*        */	Category = Summary_FARM;
		else /*                                    */	Category = Summary_GIRLS;
	}

	EditTextItem(g_Brothels.GetName(g_CurrBrothel), brothel_id);

	if (Category < 0)	Category = 0;
	if (Category_Last == Category) Item = GetSelectedItemFromList(item_id);
	else { Item = 0; Category_Last = Category; }

	string sorttext = "ITEM";
	if (summarysortorder == 1)
	{
		if (Category == Summary_STUDIO)	{ sorttext += " (Jobs)"; }
		if (Category == Summary_CLINIC)	{ sorttext += " (Triage)"; }
	}
	EditTextItem(sorttext, item_id);

	if (gold_id >= 0)
	{
		stringstream ss; ss << "Gold: " << g_Gold.ival();
		EditTextItem(ss.str(), gold_id);
	}

	// Clear everything
	ClearListBox(category_id);
	ClearListBox(item_id);
	ClearListBox(event_id);
	EditTextItem("", labeldesc_id);

	AddToListBox(category_id, 0, "GIRLS");
	AddToListBox(category_id, 1, "GANGS");
	AddToListBox(category_id, 2, "BROTHELS");
	AddToListBox(category_id, 3, "DUNGEON");
	if (g_Studios.GetNumBrothels() > 0)	AddToListBox(category_id, 4, "STUDIO");
	if (g_Arena.GetNumBrothels() > 0)	AddToListBox(category_id, 5, "ARENA");
	if (g_Centre.GetNumBrothels() > 0)	AddToListBox(category_id, 6, "CENTRE");
	if (g_Clinic.GetNumBrothels() > 0)	AddToListBox(category_id, 7, "CLINIC");
	if (g_Farm.GetNumBrothels() > 0)	AddToListBox(category_id, 8, "FARM");
	AddToListBox(category_id, 9, "HOUSE");
	SetSelectedItemInList(category_id, Category);
	Category_Change = true;

	int size = GetListBoxSize(item_id);
	if (Item >= size)	Item = size;
	SetSelectedItemInList(item_id, Item);

}
bool cScreenTurnSummary::check_events()
{
	if (g_Q_Key || g_W_Key || g_E_Key || g_A_Key || g_S_Key || g_D_Key || g_Z_Key || g_X_Key || g_C_Key || g_O_Key ||
		g_LeftArrow || g_RightArrow || g_UpArrow || g_DownArrow || g_SpaceKey || g_EnterKey){
	}	// if one of these keys is pressed then continue
	else if (g_InterfaceEvents.GetNumEvents() == 0)	return false;							// no events means we can go home

	if (g_InterfaceEvents.CheckButton(back_id))		// if it's the back button, pop the window off the stack and we're done
	{
		g_InitWin = true;
		g_WinManager.Pop();
		return false;
	}
	if (g_InterfaceEvents.CheckButton(nextweek_id))
	{
		g_InitWin = true;
		if (!g_CTRLDown) { g_CTRLDown = false; AutoSaveGame(); }
		NextWeek();
		return false;
	}

	// Change category
	if (g_InterfaceEvents.CheckListbox(category_id) || g_E_Key || g_Q_Key)
	{
		Category_Last = Category;
		if (g_E_Key)		//Q and E scrolls through the Catagories list
		{
			g_E_Key = false;
			Category++;
			if (Category == Summary_STUDIO	&& g_Studios.GetNumBrothels() < 1)	Category++;
			if (Category == Summary_ARENA	&& g_Arena.GetNumBrothels() < 1)	Category++;
			if (Category == Summary_CENTRE	&& g_Centre.GetNumBrothels() < 1)	Category++;
			if (Category == Summary_CLINIC	&& g_Clinic.GetNumBrothels() < 1)	Category++;
			if (Category == Summary_FARM	&& g_Farm.GetNumBrothels() < 1)		Category++;
			if (Category > 9) Category = 0;
		}
		else if (g_Q_Key)
		{
			g_Q_Key = false;
			Category--;
			if (Category < 0) Category = 9;
			if (Category == Summary_FARM	&& g_Farm.GetNumBrothels() < 1)		Category--;
			if (Category == Summary_CLINIC	&& g_Clinic.GetNumBrothels() < 1)	Category--;
			if (Category == Summary_CENTRE	&& g_Centre.GetNumBrothels() < 1)	Category--;
			if (Category == Summary_ARENA	&& g_Arena.GetNumBrothels() < 1)	Category--;
			if (Category == Summary_STUDIO	&& g_Studios.GetNumBrothels() < 1)	Category--;
		}
		else
		{
			Category = GetSelectedItemFromList(category_id);
		}
		if (Category >= 0 && Category == Category_Last)	return true;	// no change
		Category_Change = true;
		return true;
	}
	if (g_InterfaceEvents.CheckButton(next_id))
	{
		g_CurrBrothel++;
		if (g_CurrBrothel >= g_Brothels.GetNumBrothels()) g_CurrBrothel = 0;
		Category_Change = true;
		return true;
	}
	if (g_InterfaceEvents.CheckButton(prev_id))
	{
		g_CurrBrothel--;
		if (g_CurrBrothel < 0) g_CurrBrothel = g_Brothels.GetNumBrothels() - 1;
		Category_Change = true;
		return true;
	}

	// Change item
	if (g_InterfaceEvents.CheckListbox(item_id) || g_LeftArrow || g_RightArrow || g_A_Key || g_D_Key)
	{
		Item_Last = Item;
		if (g_LeftArrow || g_A_Key)		// W and S scroll through the events list
		{
			g_LeftArrow = g_A_Key = false;
			Item = ArrowUpListBox(item_id);
		}
		else if (g_RightArrow || g_D_Key)
		{
			g_RightArrow = g_D_Key = false;
			Item = ArrowDownListBox(item_id);
		}
		else
		{
			Item = GetSelectedItemFromList(item_id);
		}
		if (Item >= 0 && Item == Item_Last)	return true;	// no change
		Item_Change = true;
		return true;
	}

	// change event
	if (g_InterfaceEvents.CheckListbox(event_id) || g_UpArrow || g_DownArrow || g_W_Key || g_S_Key)
	{
		Event_Last = Event;
		if (g_UpArrow || g_W_Key)
		{
			g_UpArrow = g_W_Key = false;
			Event = ArrowUpListBox(event_id);
		}
		else if (g_DownArrow || g_S_Key)
		{
			g_DownArrow = g_S_Key = false;
			Event = ArrowDownListBox(event_id);
		}
		else
		{
			Event = GetSelectedItemFromList(event_id);
		}
		if (Event >= 0 && Event == Event_Last)	return true;	// no change
		Event_Change = true;
		return true;
	}

	if (g_EnterKey)
	{
		g_EnterKey = false;
		if (cfg.resolution.next_turn_enter())
		{
			g_InitWin = true;
			if (!g_CTRLDown) { g_CTRLDown = false; AutoSaveGame(); }
			NextWeek();
		}
		return false;
	}

	// change sort order
	if (g_O_Key)
	{
		g_O_Key = false;
		if (summarysortorder == 0) summarysortorder = 1;
		else summarysortorder = 0;
		Category_Last = -1;
		Category_Change = true;
		return true;
	}

	// goto selected
	if (g_InterfaceEvents.CheckButton(goto_id) || g_SpaceKey)
	{
		g_SpaceKey = false;
		g_GirlDetails.lastsexact = -1;
		string selectedName = GetSelectedTextFromList(item_id);
		g_InitWin = true;
		switch (Category)
		{
		case Summary_GANGS:
		{
			g_CurrentScreen = SCREEN_GANGMANAGEMENT;
			g_WinManager.push("Gangs");
		}break;
		case Summary_GIRLS:
		{
			selected_girl = g_Brothels.GetGirlByName(g_CurrBrothel, selectedName);
			if (selected_girl)
			{
				g_CurrentScreen = SCREEN_GIRLDETAILS;
				g_WinManager.push("Girl Details");
			}
			else
			{
				g_Building = BUILDING_BROTHEL;
				if (Item < 0 || Item > g_Brothels.GetNumBrothels()) g_CurrBrothel = 0;
				else g_CurrBrothel = Item;
				g_CurrentScreen = SCREEN_GIRLMANAGEMENT;
				g_WinManager.PopToWindow(&g_BrothelManagement);
			}
		}break;
		case Summary_DUNGEON:
		{
			sDungeonGirl* dg = g_Brothels.GetDungeon()->GetGirlByName(selectedName);
			if (dg) selected_girl = g_Brothels.GetDungeon()->GetGirlByName(selectedName)->m_Girl;
			if (selected_girl)
			{
				g_CurrentScreen = SCREEN_GIRLDETAILS;
				g_WinManager.push("Girl Details");
			}
			else
			{
				g_CurrentScreen = SCREEN_DUNGEON;
				g_WinManager.push("Dungeon");
			}
		}break;
		case Summary_CLINIC:
		{
			selected_girl = g_Clinic.GetGirlByName(0, selectedName);
			if (selected_girl)
			{
				g_CurrentScreen = SCREEN_GIRLDETAILS;
				g_WinManager.push("Girl Details");
			}
			else
			{
				g_CurrentScreen = SCREEN_CLINIC;
				g_WinManager.push("Clinic Screen");
			}
		}break;
		case Summary_STUDIO:
		{
			selected_girl = g_Studios.GetGirlByName(0, selectedName);
			if (selected_girl)
			{
				g_CurrentScreen = SCREEN_GIRLDETAILS;
				g_WinManager.push("Girl Details");
			}
			else
			{
				g_CurrentScreen = SCREEN_STUDIO;
				g_WinManager.push("Movie Screen");
			}
		}break;
		case Summary_ARENA:
		{
			selected_girl = g_Arena.GetGirlByName(0, selectedName);
			if (selected_girl)
			{
				g_CurrentScreen = SCREEN_GIRLDETAILS;
				g_WinManager.push("Girl Details");
			}
			else
			{
				g_CurrentScreen = SCREEN_ARENA;
				g_WinManager.push("Arena Screen");
			}
		}break;
		case Summary_CENTRE:
		{
			selected_girl = g_Centre.GetGirlByName(0, selectedName);
			if (selected_girl)
			{
				g_CurrentScreen = SCREEN_GIRLDETAILS;
				g_WinManager.push("Girl Details");
			}
			else
			{
				g_CurrentScreen = SCREEN_CENTRE;
				g_WinManager.push("Centre Screen");
			}
		}break;
		case Summary_HOUSE:
		{
			selected_girl = g_House.GetGirlByName(0, selectedName);
			if (selected_girl)
			{
				g_CurrentScreen = SCREEN_GIRLDETAILS;
				g_WinManager.push("Girl Details");
			}
			else
			{
				g_CurrentScreen = SCREEN_HOUSE;
				g_WinManager.push("Player House");
			}
		}break;
		case Summary_FARM:
		{
			selected_girl = g_Farm.GetGirlByName(0, selectedName);
			if (selected_girl)
			{
				g_CurrentScreen = SCREEN_GIRLDETAILS;
				g_WinManager.push("Girl Details");
			}
			else
			{
				g_CurrentScreen = SCREEN_FARM;
				g_WinManager.push("Farm Screen");
			}
		}break;
		case Summary_BROTHELS:
		default:
		{
			g_Building = BUILDING_BROTHEL;
			if (Item < 0 || Item > g_Brothels.GetNumBrothels()) g_CurrBrothel = 0;
			else g_CurrBrothel = Item;
			g_CurrentScreen = SCREEN_GIRLMANAGEMENT;
			g_WinManager.PopToWindow(&g_BrothelManagement);
		}break;
		}
	}
	return false;
}

void cScreenTurnSummary::Fill_Items_GIRLS()
{
	int ID = 0, nNumGirls = g_Brothels.GetNumGirls(g_CurrBrothel);
	vector<sGirl*> tmpSexGirls, tmpServiceGirls, tmpGoodNewsGirls, tmpDangerGirls, tmpWarningGirls;
	tmpSexGirls.clear(); tmpServiceGirls.clear(); tmpGoodNewsGirls.clear(); tmpDangerGirls.clear(); tmpWarningGirls.clear();
	sGirl* pTmpGirl;
	bool sexjob = false;

	// Major change in the way this is handled... want to be able to list sex related jobs together. 
	// Disabled and replaced the enire section dealing with populating the listboxes here. --PP
	// Find out which girls have sex type jobs.
	for (int i = 0; i<nNumGirls; i++)
	{
		pTmpGirl = g_Brothels.GetGirl(g_CurrBrothel, i);
		sexjob = false;
		switch (pTmpGirl->m_DayJob)
		{
		case JOB_XXXENTERTAINMENT:
		case JOB_PEEP:
		case JOB_BARSTRIPPER:
		case JOB_BROTHELSTRIPPER:
		case JOB_MASSEUSE:
		case JOB_BARWHORE:
		case JOB_WHOREGAMBHALL:
		case JOB_WHOREBROTHEL:
		case JOB_WHORESTREETS:
		case JOB_ESCORT:
			sexjob = true;
			break;
		default:
			break;
		}
		switch (pTmpGirl->m_NightJob)
		{
		case JOB_XXXENTERTAINMENT:
		case JOB_PEEP:
		case JOB_BARSTRIPPER:
		case JOB_BROTHELSTRIPPER:
		case JOB_MASSEUSE:
		case JOB_BARWHORE:
		case JOB_WHOREGAMBHALL:
		case JOB_WHOREBROTHEL:
		case JOB_WHORESTREETS:
		case JOB_ESCORT:
			sexjob = true;
			break;
		default:
			break;
		}
		// Sort the girls into 4 catagories... sex jobs, service jobs, warning, and danger
		// `J` added 5th catagory... goodnews
		// If we want to we could add a seperate catagory for each job and order the list even further, but it will make this clunkier.
		/* */if (!pTmpGirl->m_Events.HasUrgent() && sexjob)	tmpSexGirls.push_back(pTmpGirl);
		else if (!pTmpGirl->m_Events.HasUrgent())			tmpServiceGirls.push_back(pTmpGirl);
		else if (pTmpGirl->m_Events.HasDanger())			tmpDangerGirls.push_back(pTmpGirl);
		else if (pTmpGirl->m_Events.HasGoodNews())			tmpGoodNewsGirls.push_back(pTmpGirl);
		else /*                                 */			tmpWarningGirls.push_back(pTmpGirl);
	}
	// Put the catagories into the List Boxes... to change what order they are listed in, just swap these for-next loops. --PP
	//Girls with Danger events
	for (u_int i = 0; i < tmpDangerGirls.size(); i++)
	{
		string tname = tmpDangerGirls[i]->m_Realname;
		AddToListBox(item_id, ID, tname, COLOR_RED);
		if (selected_girl == tmpDangerGirls[i]) Item = ID;
		ID++;
	}
	//Girls with GoodNews events
	for (u_int i = 0; i < tmpGoodNewsGirls.size(); i++)
	{
		string tname = tmpGoodNewsGirls[i]->m_Realname;
		AddToListBox(item_id, ID, tname, COLOR_GREEN);
		if (selected_girl == tmpGoodNewsGirls[i]) Item = ID;
		ID++;
	}
	//Girls wih Warnings
	for (u_int i = 0; i < tmpWarningGirls.size(); i++)
	{
		string tname = tmpWarningGirls[i]->m_Realname;
		AddToListBox(item_id, ID, tname, COLOR_DARKBLUE);
		if (selected_girl == tmpWarningGirls[i]) Item = ID;
		ID++;
	}
	//ServiceJob Girls
	for (u_int i = 0; i < tmpServiceGirls.size(); i++)
	{
		string tname = tmpServiceGirls[i]->m_Realname;
		AddToListBox(item_id, ID, tname);
		if (selected_girl == tmpServiceGirls[i]) Item = ID;
		ID++;
	}
	//SexJob girls
	for (u_int i = 0; i < tmpSexGirls.size(); i++)
	{
		string tname = tmpSexGirls[i]->m_Realname;
		AddToListBox(item_id, ID, tname);
		if (selected_girl == tmpSexGirls[i]) Item = ID;
		ID++;
	}

}
void cScreenTurnSummary::Fill_Items_GANGS()
{
	for (int i = 0; i<g_Gangs.GetNumGangs(); i++)
		AddToListBox(item_id, i, g_Gangs.GetGang(i)->m_Name);
}
void cScreenTurnSummary::Fill_Items_BROTHELS()
{
	for (int i = 0; i<g_Brothels.GetNumBrothels(); i++)
		AddToListBox(item_id, i, g_Brothels.GetBrothel(i)->m_Name);
	Item = g_CurrBrothel;
}
void cScreenTurnSummary::Fill_Items_DUNGEON()
{
	// Fill the list box
	cDungeon* pDungeon = g_Brothels.GetDungeon();
	int ID = 0, nNumGirls = pDungeon->GetNumGirls();
	vector<sGirl*> tmpGoodNewsGirls, tmpDangerGirls, tmpWarningGirls, tmpOtherGirls;
	tmpGoodNewsGirls.clear(); tmpDangerGirls.clear(); tmpWarningGirls.clear(); tmpOtherGirls.clear();
	sGirl* pTmpGirl;

	for (int i = 0; i < nNumGirls; i++)
	{
		pTmpGirl = pDungeon->GetGirl(i)->m_Girl;

		/* */if (!pTmpGirl->m_Events.HasUrgent())	tmpOtherGirls.push_back(pTmpGirl);
		else if (pTmpGirl->m_Events.HasDanger())	tmpDangerGirls.push_back(pTmpGirl);
		else if (pTmpGirl->m_Events.HasGoodNews())	tmpGoodNewsGirls.push_back(pTmpGirl);
		else /*                                */	tmpWarningGirls.push_back(pTmpGirl);
	}

	//Girls with Danger events
	for (u_int i = 0; i < tmpDangerGirls.size(); i++)
	{
		string tname = tmpDangerGirls[i]->m_Realname;
		AddToListBox(item_id, ID, tname, COLOR_RED);
		if (selected_girl == tmpDangerGirls[i]) Item = ID;
		ID++;
	}
	//Girls with GoodNews events
	for (u_int i = 0; i < tmpGoodNewsGirls.size(); i++)
	{
		string tname = tmpGoodNewsGirls[i]->m_Realname;
		AddToListBox(item_id, ID, tname, COLOR_GREEN);
		if (selected_girl == tmpGoodNewsGirls[i]) Item = ID;
		ID++;
	}
	//Girls wih Warnings
	for (u_int i = 0; i < tmpWarningGirls.size(); i++)
	{
		string tname = tmpWarningGirls[i]->m_Realname;
		AddToListBox(item_id, ID, tname, COLOR_DARKBLUE);
		if (selected_girl == tmpWarningGirls[i]) Item = ID;
		ID++;
	}
	//ServiceJob Girls
	for (u_int i = 0; i < tmpOtherGirls.size(); i++)
	{
		string tname = tmpOtherGirls[i]->m_Realname;
		AddToListBox(item_id, ID, tname);
		if (selected_girl == tmpOtherGirls[i]) Item = ID;
		ID++;
	}
}// End of New dungeon code
void cScreenTurnSummary::Fill_Items_STUDIO()
{
	if (summarysortorder == 1)	// Sort by Job
	{
		int nNumGirlsStudio = g_Studios.GetNumGirls(g_CurrStudio);
		int ID = 0;
		vector<sGirl*> tmpStudioDirector, tmpStudioCrew, tmpStudioStaff, tmpStudioActress, tmpStudioFree;
		tmpStudioDirector.clear(); tmpStudioCrew.clear(); tmpStudioStaff.clear(); tmpStudioActress.clear(); tmpStudioFree.clear();
		sGirl* pTmpGirl;
		// Sort the girls by their job groups
		for (int i = 0; i < nNumGirlsStudio; i++)
		{
			pTmpGirl = g_Studios.GetGirl(0, i);
			switch (pTmpGirl->m_NightJob)
			{
			case JOB_DIRECTOR:
				tmpStudioDirector.push_back(pTmpGirl);
				break;
			case JOB_CAMERAMAGE:
			case JOB_CRYSTALPURIFIER:
				tmpStudioCrew.push_back(pTmpGirl);
				break;
			case JOB_PROMOTER:
			case JOB_FLUFFER:
			case JOB_STAGEHAND:
				tmpStudioStaff.push_back(pTmpGirl);
				break;
			case JOB_FILMFREETIME:
				tmpStudioFree.push_back(pTmpGirl);
				break;
			default:	// all others get sorted as actress
				tmpStudioActress.push_back(pTmpGirl);
				break;
			}
		}

		for (u_int i = 0; i < tmpStudioDirector.size(); i++)
		{
			string tname = tmpStudioDirector[i]->m_Realname;
			AddToListBox(item_id, ID, tname, COLOR_RED);
			if (selected_girl == tmpStudioDirector[i])
				Item = ID;
			ID++;
		}
		for (u_int i = 0; i < tmpStudioCrew.size(); i++)
		{
			string tname = tmpStudioCrew[i]->m_Realname;
			AddToListBox(item_id, ID, tname, COLOR_DARKBLUE);
			if (selected_girl == tmpStudioCrew[i])
				Item = ID;
			ID++;
		}
		for (u_int i = 0; i < tmpStudioStaff.size(); i++)
		{
			string tname = tmpStudioStaff[i]->m_Realname;
			AddToListBox(item_id, ID, tname, COLOR_BLUE);
			if (selected_girl == tmpStudioStaff[i])
				Item = ID;
			ID++;
		}
		for (u_int i = 0; i < tmpStudioActress.size(); i++)
		{
			string tname = tmpStudioActress[i]->m_Realname;
			AddToListBox(item_id, ID, tname, COLOR_GREEN);
			if (selected_girl == tmpStudioActress[i])
				Item = ID;
			ID++;
		}
		for (u_int i = 0; i < tmpStudioFree.size(); i++)
		{
			string tname = tmpStudioFree[i]->m_Realname;
			AddToListBox(item_id, ID, tname, COLOR_RED);
			if (selected_girl == tmpStudioFree[i])
				Item = ID;
			ID++;
		}
	}
	else	// start normal sorting
	{
		int nNumGirlsStudio = g_Studios.GetNumGirls(g_CurrStudio);
		int ID = 0;
		vector<sGirl*> tmpSexGirls;
		vector<sGirl*> tmpServiceGirls;
		vector<sGirl*> tmpGoodNewsGirls;
		vector<sGirl*> tmpDangerGirls;
		vector<sGirl*> tmpWarningGirls;
		tmpSexGirls.clear();
		tmpServiceGirls.clear();
		tmpGoodNewsGirls.clear();
		tmpDangerGirls.clear();
		tmpWarningGirls.clear();
		sGirl* pTmpGirl;
		bool sexjob = false;

		// Major change in the way this is handled... want to be able to list sex related jobs together. 
		// Disabled and replaced the enire section dealing with populating the listboxes here. --PP
		// Find out which girls have sex type jobs.
		for (int i = 0; i < nNumGirlsStudio; i++)
		{
			pTmpGirl = g_Studios.GetGirl(0, i); // `J` changed g_CurrBrothel to 0
			sexjob = false;
			switch (pTmpGirl->m_NightJob)
			{
			case JOB_FILMBEAST:
			case JOB_FILMSEX:
			case JOB_FILMANAL:
			case JOB_FILMLESBIAN:
			case JOB_FILMBONDAGE:
			case JOB_FILMGROUP:
			case JOB_FILMORAL:
			case JOB_FILMMAST:
			case JOB_FILMTITTY:
			case JOB_FILMSTRIP:
			case JOB_FILMRANDOM:
			case JOB_FLUFFER:
				sexjob = true;
				break;
			default:
				break;
			}
			if (!pTmpGirl->m_Events.HasUrgent() && sexjob)	{ tmpSexGirls.push_back(pTmpGirl); }
			else if (!pTmpGirl->m_Events.HasUrgent())		{ tmpServiceGirls.push_back(pTmpGirl); }
			else if (pTmpGirl->m_Events.HasDanger())		{ tmpDangerGirls.push_back(pTmpGirl); }
			else if (pTmpGirl->m_Events.HasGoodNews())		{ tmpGoodNewsGirls.push_back(pTmpGirl); }
			else /*                                */		{ tmpWarningGirls.push_back(pTmpGirl); }
		}
		// Put the catagories into the List Boxes... to change what order they are listed in, just swap these for-next loops. --PP

		//Girls with Danger events
		for (u_int i = 0; i < tmpDangerGirls.size(); i++)
		{
			string tname = tmpDangerGirls[i]->m_Realname;
			AddToListBox(item_id, ID, tname, COLOR_RED);
			if (selected_girl == tmpDangerGirls[i])
				Item = ID;
			ID++;
		}
		//Girls with GoodNews events
		for (u_int i = 0; i < tmpGoodNewsGirls.size(); i++)
		{
			string tname = tmpGoodNewsGirls[i]->m_Realname;
			AddToListBox(item_id, ID, tname, COLOR_GREEN);
			if (selected_girl == tmpGoodNewsGirls[i])
				Item = ID;
			ID++;
		}
		//Girls wih Warnings
		for (u_int i = 0; i < tmpWarningGirls.size(); i++)
		{
			string tname = tmpWarningGirls[i]->m_Realname;
			AddToListBox(item_id, ID, tname, COLOR_DARKBLUE);
			if (selected_girl == tmpWarningGirls[i])
				Item = ID;
			ID++;
		}
		//ServiceJob Girls
		for (u_int i = 0; i < tmpServiceGirls.size(); i++)
		{
			string tname = tmpServiceGirls[i]->m_Realname;
			AddToListBox(item_id, ID, tname);
			if (selected_girl == tmpServiceGirls[i])
				Item = ID;
			ID++;
		}
		//SexJob girls
		for (u_int i = 0; i < tmpSexGirls.size(); i++)
		{
			string tname = tmpSexGirls[i]->m_Realname;
			AddToListBox(item_id, ID, tname);
			if (selected_girl == tmpSexGirls[i])
				Item = ID;
			ID++;
		}
	}
}
void cScreenTurnSummary::Fill_Items_ARENA()
{
	int nNumGirlsArena = g_Arena.GetNumGirls(g_CurrArena);
	int ID = 0;

	// `J` Girls with GoodNews events first
	for (int h = 0; h<nNumGirlsArena; h++)
	{
		sGirl* pTmpGirl = g_Arena.GetGirl(0, h);
		if (pTmpGirl->m_Events.HasGoodNews())
		{
			string tname = pTmpGirl->m_Realname;
			AddToListBox(item_id, ID, tname, COLOR_GREEN);
			if (selected_girl == pTmpGirl)
				Item = ID;
			ID++;
		}
	}
	// MYR: Girls with danger events first
	for (int i = 0; i<nNumGirlsArena; i++)
	{
		sGirl* pTmpGirl = g_Arena.GetGirl(0, i);
		if (pTmpGirl->m_Events.HasDanger() && !pTmpGirl->m_Events.HasGoodNews())
		{
			string tname = pTmpGirl->m_Realname;
			AddToListBox(item_id, ID, tname, COLOR_RED);
			if (selected_girl == pTmpGirl)
				Item = ID;
			ID++;
		}
	}

	// Girls with warning events next
	for (int j = 0; j<nNumGirlsArena; j++)
	{
		sGirl* pTmpGirl = g_Arena.GetGirl(0, j);
		if (pTmpGirl->m_Events.HasWarning() && !pTmpGirl->m_Events.HasDanger() && !pTmpGirl->m_Events.HasGoodNews())
		{
			string temp = pTmpGirl->m_Realname;
			AddToListBox(item_id, ID, temp, COLOR_DARKBLUE);
			if (selected_girl == pTmpGirl)
				Item = ID;
			ID++;
		}
	}

	// Arena girl
	for (int k = 0; k<nNumGirlsArena; k++)
	{
		sGirl* pTmpGirl = g_Arena.GetGirl(0, k);
		if (!pTmpGirl->m_Events.HasUrgent())
		{
			string temp = pTmpGirl->m_Realname;
			AddToListBox(item_id, ID, temp);
			if (selected_girl == pTmpGirl)
				Item = ID;
			ID++;
		}
	}
} // End of arena
void cScreenTurnSummary::Fill_Items_CENTRE()
{
	int nNumGirlsCentre = g_Centre.GetNumGirls(g_CurrCentre);
	int ID = 0;

	// MYR: Girls with danger events first
	for (int i = 0; i<nNumGirlsCentre; i++)
	{
		sGirl* pTmpGirl = g_Centre.GetGirl(0, i);
		if (pTmpGirl->m_Events.HasDanger())
		{
			string tname = pTmpGirl->m_Realname;
			AddToListBox(item_id, ID, tname, COLOR_RED);
			if (selected_girl == pTmpGirl)
				Item = ID;
			ID++;
		}
	}

	// `J` Girls with GoodNews events first
	for (int h = 0; h<nNumGirlsCentre; h++)
	{
		sGirl* pTmpGirl = g_Centre.GetGirl(0, h);
		if (pTmpGirl->m_Events.HasGoodNews() && !pTmpGirl->m_Events.HasDanger())
		{
			string tname = pTmpGirl->m_Realname;
			AddToListBox(item_id, ID, tname, COLOR_GREEN);
			if (selected_girl == pTmpGirl)
				Item = ID;
			ID++;
		}
	}
	// Girls with warning events next
	for (int j = 0; j<nNumGirlsCentre; j++)
	{
		sGirl* pTmpGirl = g_Centre.GetGirl(0, j);
		if (pTmpGirl->m_Events.HasWarning() && !pTmpGirl->m_Events.HasDanger() && !pTmpGirl->m_Events.HasGoodNews())
		{
			string temp = pTmpGirl->m_Realname;
			AddToListBox(item_id, ID, temp, COLOR_DARKBLUE);
			if (selected_girl == pTmpGirl)
				Item = ID;
			ID++;
		}
	}

	// Centre girl
	for (int k = 0; k<nNumGirlsCentre; k++)
	{
		sGirl* pTmpGirl = g_Centre.GetGirl(0, k);
		if (!pTmpGirl->m_Events.HasUrgent())
		{
			string temp = pTmpGirl->m_Realname;
			AddToListBox(item_id, ID, temp);
			if (selected_girl == pTmpGirl)
				Item = ID;
			ID++;
		}
	}
} // End of Centre
void cScreenTurnSummary::Fill_Items_CLINIC()
{
	if (summarysortorder == 1)	//	Sort by Triage
	{
		// `J` because the clinic runs the girls differently, we sort the girls differently
		int ID = 0;
		int nNumGirlsClinic = g_Clinic.GetNumGirls(0);
		vector<sGirl*> tmpTriage5, tmpTriage4, tmpTriage3, tmpTriage2, tmpTriage1, tmpTriage0, tmpDoctors, tmpNurses, tmpStaff;
		tmpTriage5.clear(); tmpTriage4.clear(); tmpTriage3.clear(); tmpTriage2.clear(); tmpTriage1.clear(); tmpTriage0.clear(); tmpDoctors.clear();  tmpNurses.clear(); tmpStaff.clear();
		sGirl* pTmpGirl;

		for (int i = 0; i < nNumGirlsClinic; i++)
		{
			pTmpGirl = g_Clinic.GetGirl(0, i);

			// `J` Doctors first
			/* */if (pTmpGirl->m_DayJob == JOB_DOCTOR || pTmpGirl->m_NightJob == JOB_DOCTOR)
				tmpDoctors.push_back(pTmpGirl);
			// `J` Girls in surgery, the lower on the list, the longer she will be in surgery
			else if ((g_Clinic.is_Surgery_Job(pTmpGirl->m_DayJob) || g_Clinic.is_Surgery_Job(pTmpGirl->m_NightJob))
				&& pTmpGirl->m_WorkingDay >= 5)	tmpTriage5.push_back(pTmpGirl);
			else if ((g_Clinic.is_Surgery_Job(pTmpGirl->m_DayJob) || g_Clinic.is_Surgery_Job(pTmpGirl->m_NightJob))
				&& pTmpGirl->m_WorkingDay >= 4)	tmpTriage4.push_back(pTmpGirl);
			else if ((g_Clinic.is_Surgery_Job(pTmpGirl->m_DayJob) || g_Clinic.is_Surgery_Job(pTmpGirl->m_NightJob))
				&& pTmpGirl->m_WorkingDay >= 3)	tmpTriage3.push_back(pTmpGirl);
			else if ((g_Clinic.is_Surgery_Job(pTmpGirl->m_DayJob) || g_Clinic.is_Surgery_Job(pTmpGirl->m_NightJob))
				&& pTmpGirl->m_WorkingDay >= 2)	tmpTriage2.push_back(pTmpGirl);
			else if ((g_Clinic.is_Surgery_Job(pTmpGirl->m_DayJob) || g_Clinic.is_Surgery_Job(pTmpGirl->m_NightJob))
				&& pTmpGirl->m_WorkingDay >= 1)	tmpTriage1.push_back(pTmpGirl);
			// `J` all other patients
			else if (g_Clinic.is_Surgery_Job(pTmpGirl->m_DayJob) || g_Clinic.is_Surgery_Job(pTmpGirl->m_NightJob)
				|| pTmpGirl->m_DayJob == JOB_GETHEALING || pTmpGirl->m_NightJob == JOB_GETHEALING
				|| pTmpGirl->m_DayJob == JOB_GETREPAIRS || pTmpGirl->m_NightJob == JOB_GETREPAIRS)
				tmpTriage0.push_back(pTmpGirl);
			// `J` Nurses after patients
			else if (pTmpGirl->m_DayJob == JOB_NURSE || pTmpGirl->m_NightJob == JOB_NURSE)
				tmpNurses.push_back(pTmpGirl);
			// `J` the rest of the Staff last
			else tmpStaff.push_back(pTmpGirl);
		}
		for (u_int i = 0; i < tmpTriage5.size(); i++)
		{
			string tname = tmpTriage5[i]->m_Realname;
			AddToListBox(item_id, ID, tname, COLOR_RED);
			if (selected_girl == tmpTriage5[i]) Item = ID;
			ID++;
		}
		for (u_int i = 0; i < tmpTriage4.size(); i++)
		{
			string tname = tmpTriage4[i]->m_Realname;
			AddToListBox(item_id, ID, tname, COLOR_RED);
			if (selected_girl == tmpTriage4[i]) Item = ID;
			ID++;
		}
		for (u_int i = 0; i < tmpTriage3.size(); i++)
		{
			string tname = tmpTriage3[i]->m_Realname;
			AddToListBox(item_id, ID, tname, COLOR_RED);
			if (selected_girl == tmpTriage3[i]) Item = ID;
			ID++;
		}
		for (u_int i = 0; i < tmpTriage2.size(); i++)
		{
			string tname = tmpTriage2[i]->m_Realname;
			AddToListBox(item_id, ID, tname, COLOR_RED);
			if (selected_girl == tmpTriage2[i]) Item = ID;
			ID++;
		}
		for (u_int i = 0; i < tmpTriage1.size(); i++)
		{
			string tname = tmpTriage1[i]->m_Realname;
			AddToListBox(item_id, ID, tname, COLOR_RED);
			if (selected_girl == tmpTriage1[i]) Item = ID;
			ID++;
		}
		for (u_int i = 0; i < tmpTriage0.size(); i++)
		{
			string tname = tmpTriage0[i]->m_Realname;
			AddToListBox(item_id, ID, tname);
			if (selected_girl == tmpTriage0[i]) Item = ID;
			ID++;
		}
		for (u_int i = 0; i < tmpDoctors.size(); i++)
		{
			string tname = tmpDoctors[i]->m_Realname;
			AddToListBox(item_id, ID, tname);
			if (selected_girl == tmpDoctors[i]) Item = ID;
			ID++;
		}
		for (u_int i = 0; i < tmpNurses.size(); i++)
		{
			string tname = tmpNurses[i]->m_Realname;
			AddToListBox(item_id, ID, tname);
			if (selected_girl == tmpNurses[i]) Item = ID;
			ID++;
		}
		for (u_int i = 0; i < tmpStaff.size(); i++)
		{
			string tname = tmpStaff[i]->m_Realname;
			AddToListBox(item_id, ID, tname);
			if (selected_girl == tmpStaff[i]) Item = ID;
			ID++;
		}
	}		// end triage sorting
	else	// start normal sorting
	{
		int ID = 0, nNumGirlsClinic = g_Clinic.GetNumGirls(0);
		vector<sGirl*> tmpServiceGirls, tmpGoodNewsGirls, tmpDangerGirls, tmpWarningGirls;
		tmpServiceGirls.clear(); tmpGoodNewsGirls.clear(); tmpDangerGirls.clear(); tmpWarningGirls.clear();
		sGirl* pTmpGirl;
		for (int i = 0; i < nNumGirlsClinic; i++)
		{
			pTmpGirl = g_Clinic.GetGirl(0, i);
			/* */if (!pTmpGirl->m_Events.HasUrgent())			tmpServiceGirls.push_back(pTmpGirl);
			else if (pTmpGirl->m_Events.HasDanger())			tmpDangerGirls.push_back(pTmpGirl);
			else if (pTmpGirl->m_Events.HasGoodNews())			tmpGoodNewsGirls.push_back(pTmpGirl);
			else /*                                 */			tmpWarningGirls.push_back(pTmpGirl);
		}
		for (u_int i = 0; i < tmpDangerGirls.size(); i++)
		{
			string tname = tmpDangerGirls[i]->m_Realname;
			AddToListBox(item_id, ID, tname, COLOR_RED);
			if (selected_girl == tmpDangerGirls[i]) Item = ID;
			ID++;
		}
		//Girls with GoodNews events
		for (u_int i = 0; i < tmpGoodNewsGirls.size(); i++)
		{
			string tname = tmpGoodNewsGirls[i]->m_Realname;
			AddToListBox(item_id, ID, tname, COLOR_GREEN);
			if (selected_girl == tmpGoodNewsGirls[i]) Item = ID;
			ID++;
		}
		//Girls wih Warnings
		for (u_int i = 0; i < tmpWarningGirls.size(); i++)
		{
			string tname = tmpWarningGirls[i]->m_Realname;
			AddToListBox(item_id, ID, tname, COLOR_DARKBLUE);
			if (selected_girl == tmpWarningGirls[i]) Item = ID;
			ID++;
		}
		//ServiceJob Girls
		for (u_int i = 0; i < tmpServiceGirls.size(); i++)
		{
			string tname = tmpServiceGirls[i]->m_Realname;
			AddToListBox(item_id, ID, tname);
			if (selected_girl == tmpServiceGirls[i]) Item = ID;
			ID++;
		}
	}		// end normal sorting
} // End of clinic
void cScreenTurnSummary::Fill_Items_FARM()
{
	int nNumGirlsFarm = g_Farm.GetNumGirls(g_CurrFarm);
	int ID = 0;

	// MYR: Girls with danger events first
	for (int i = 0; i<nNumGirlsFarm; i++)
	{
		sGirl* pTmpGirl = g_Farm.GetGirl(0, i);
		if (pTmpGirl->m_Events.HasDanger())
		{
			string tname = pTmpGirl->m_Realname;
			AddToListBox(item_id, ID, tname, COLOR_RED);
			if (selected_girl == pTmpGirl)
				Item = ID;
			ID++;
		}
	}
	// `J` Girls with GoodNews events first
	for (int h = 0; h<nNumGirlsFarm; h++)
	{
		sGirl* pTmpGirl = g_Farm.GetGirl(0, h);
		if (pTmpGirl->m_Events.HasGoodNews() && !pTmpGirl->m_Events.HasDanger())
		{
			string tname = pTmpGirl->m_Realname;
			AddToListBox(item_id, ID, tname, COLOR_GREEN);
			if (selected_girl == pTmpGirl)
				Item = ID;
			ID++;
		}
	}

	// Girls with warning events next
	for (int j = 0; j<nNumGirlsFarm; j++)
	{
		sGirl* pTmpGirl = g_Farm.GetGirl(0, j);
		if (pTmpGirl->m_Events.HasWarning() && !pTmpGirl->m_Events.HasDanger() && !pTmpGirl->m_Events.HasGoodNews())
		{
			string temp = pTmpGirl->m_Realname;
			AddToListBox(item_id, ID, temp, COLOR_DARKBLUE);
			if (selected_girl == pTmpGirl)
				Item = ID;
			ID++;
		}
	}

	// Farm girl
	for (int k = 0; k<nNumGirlsFarm; k++)
	{
		sGirl* pTmpGirl = g_Farm.GetGirl(0, k);
		if (!pTmpGirl->m_Events.HasUrgent())
		{
			string temp = pTmpGirl->m_Realname;
			AddToListBox(item_id, ID, temp);
			if (selected_girl == pTmpGirl)
				Item = ID;
			ID++;
		}
	}
} // End of Farm
void cScreenTurnSummary::Fill_Items_HOUSE()
{
	int nNumGirlsHouse = g_House.GetNumGirls(g_CurrHouse);
	int ID = 0;

	// MYR: Girls with danger events first
	for (int i = 0; i<nNumGirlsHouse; i++)
	{
		sGirl* pTmpGirl = g_House.GetGirl(0, i);
		if (pTmpGirl->m_Events.HasDanger())
		{
			string tname = pTmpGirl->m_Realname;
			AddToListBox(item_id, ID, tname, COLOR_RED);
			if (selected_girl == pTmpGirl)
				Item = ID;
			ID++;
		}
	}
	// `J` Girls with GoodNews events first
	for (int h = 0; h<nNumGirlsHouse; h++)
	{
		sGirl* pTmpGirl = g_House.GetGirl(0, h);
		if (pTmpGirl->m_Events.HasGoodNews() && !pTmpGirl->m_Events.HasDanger())
		{
			string tname = pTmpGirl->m_Realname;
			AddToListBox(item_id, ID, tname, COLOR_GREEN);
			if (selected_girl == pTmpGirl)
				Item = ID;
			ID++;
		}
	}

	// Girls with warning events next
	for (int j = 0; j<nNumGirlsHouse; j++)
	{
		sGirl* pTmpGirl = g_House.GetGirl(0, j);
		if (pTmpGirl->m_Events.HasWarning() && !pTmpGirl->m_Events.HasDanger() && !pTmpGirl->m_Events.HasGoodNews())
		{
			string temp = pTmpGirl->m_Realname;
			AddToListBox(item_id, ID, temp, COLOR_DARKBLUE);
			if (selected_girl == pTmpGirl)
				Item = ID;
			ID++;
		}
	}

	// House girl
	for (int k = 0; k<nNumGirlsHouse; k++)
	{
		sGirl* pTmpGirl = g_House.GetGirl(0, k);
		if (!pTmpGirl->m_Events.HasUrgent())
		{
			string temp = pTmpGirl->m_Realname;
			AddToListBox(item_id, ID, temp);
			if (selected_girl == pTmpGirl)
				Item = ID;
			ID++;
		}
	}
} // End of House


void cScreenTurnSummary::Fill_Events(sGirl* girl)
{
	if (girl == 0) return;
	Event_Change = true;
	Image_Change = true;
	Image_Type = IMGTYPE_PROFILE;
	if (!girl->m_Events.IsEmpty())
	{
		girl->m_Events.DoSort();		// Sort Events to put Warnings & Dangers first.
		for (int l = 0; l < girl->m_Events.GetNumEvents(); l++)
		{
			string			sTitle = girl->m_Events.GetMessage(l).TitleText();
			unsigned int	uiListboxColour = girl->m_Events.GetMessage(l).ListboxColour();
			AddToListBox(event_id, l, sTitle, uiListboxColour);
		}
	}
	if (GetListBoxSize(event_id) > 0) SetSelectedItemInList(event_id, 0);
}
void cScreenTurnSummary::Fill_Events_GANGS()
{
	sGang* gang = g_Gangs.GetGang(Item);
	if (gang == 0) return;
	if (!gang->m_Events.IsEmpty())
	{
		gang->m_Events.DoSort();						// Sort Events to put Warnings & Dangers first.

		for (int l = 0; l < gang->m_Events.GetNumEvents(); l++)
		{
			string			sTitle = gang->m_Events.GetMessage(l).TitleText();
			unsigned int	uiListboxColour = gang->m_Events.GetMessage(l).ListboxColour();
			AddToListBox(event_id, l, sTitle, uiListboxColour);
		}
	}
	if (GetListBoxSize(event_id) > 0) SetSelectedItemInList(event_id, 0);
}
void cScreenTurnSummary::Fill_Events_BROTHELS()
{
	if (Item < 0) Item = 0;
	if (Item >= g_Brothels.GetNumBrothels()) Item = g_Brothels.GetNumBrothels()-1;
	sBrothel* pSelectedBrothel = g_Brothels.GetBrothel(Item);
	if (!pSelectedBrothel->m_Events.IsEmpty())
	{
		pSelectedBrothel->m_Events.DoSort();						// Sort Events to put Warnings & Dangers first.

		for (int l = 0; l < g_Brothels.GetBrothel(Item)->m_Events.GetNumEvents(); l++)
		{
			string			sTitle = pSelectedBrothel->m_Events.GetMessage(l).TitleText();
			unsigned int	uiListboxColour = pSelectedBrothel->m_Events.GetMessage(l).ListboxColour();
			AddToListBox(event_id, l, sTitle, uiListboxColour);
		}
	}
}
