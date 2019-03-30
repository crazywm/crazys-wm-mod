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
#include <functional>
#include <main.h>
#include "cScreenTurnSummary.h"

#include "buildings/cBrothel.h"

#include "cGangs.h"
#include "cScreenGirlDetails.h"
#include "InterfaceProcesses.h"
#include "cScreenBrothelManagement.h"

#include "widgets/cTextItem.h"
#include "widgets/cImageItem.h"
#include "FileList.h"
#include "Game.hpp"

extern cScreenBrothelManagement* g_BrothelManagement;
extern	cScreenGirlDetails*		g_GirlDetails;
extern	bool					g_Cheats;
extern	bool					eventrunning;
extern	bool					g_AllTogle;

extern	string					pic_types[];

extern	sGirl*					g_selected_girl;

extern	bool	g_UpArrow;
extern	bool	g_DownArrow;
extern	bool	g_AltKeys;	// New hotkeys --PP
extern	bool	g_EnterKey;
extern	bool	g_W_Key;
extern	bool	g_S_Key;
extern	bool	g_O_Key;
extern	bool	g_CTRLDown;
extern  bool    g_EnterKey; // More Hotkeys

#pragma endregion
#pragma region //	Local Variables			//

static int	Item = -1;
static bool	Item_Change = false;
static int	Event = -1;
static int	Image = -1;
static int	Image_Type = -1;
static bool	Image_Change = false;
static int summarysortorder = 0;	// the order girls get sorted in the summary lists
#pragma endregion

cScreenTurnSummary::cScreenTurnSummary() : cInterfaceWindowXML("TurnSummary.xml")
{
}

void cScreenTurnSummary::set_ids()
{
	back_id			= get_id("BackButton", "Back");
	brothel_id		= get_id("CurrentBrothel");
	category_id		= get_id("Category");
	gold_id			= get_id("Gold", "*Unused*");//
	labelitem_id	= get_id("LabelItem");
	item_id			= get_id("Item");
	event_id		= get_id("Event");
	labeldesc_id	= get_id("LabelDesc");
	goto_id			= get_id("GoTo");
	nextweek_id		= get_id("NextWeek");
	prev_id			= get_id("PrevButton","Prev");
	next_id			= get_id("NextButton","Next");
	image_id		= get_id("Background");
	imagename_id	= get_id("ImageName", "*Unused*");//

	SetButtonNavigation(back_id, "<back>");
	SetButtonCallback(next_id, [this]() {
        cycle_building(1);
        change_category(m_ActiveCategory);
	});
    SetButtonCallback(prev_id, [this]() {
        cycle_building(-1);
        change_category(m_ActiveCategory);
    });
    SetButtonCallback(nextweek_id, [this]() {
        init(false);
        if (!g_CTRLDown) { AutoSaveGame(); }
        NextWeek();
    });

    SetListBoxHotKeys(category_id, SDLK_e, SDLK_q);
    SetListBoxSelectionCallback(category_id, [this](int selection) {
        change_category((SummaryCategory)selection);
    });

    SetListBoxHotKeys(item_id, SDLK_a, SDLK_d);
    SetListBoxSelectionCallback(item_id, [this](int selection) {
       Item = selection;
       change_item();
    });

    SetListBoxHotKeys(event_id, SDLK_w, SDLK_s);
    SetListBoxSelectionCallback(event_id, [this](int selection) {
        change_event();
    });

    SetButtonCallback(goto_id, [this]() { goto_selected(); });
    SetButtonHotKey(goto_id, SDLK_SPACE);
}

void cScreenTurnSummary::process()
{
	// process a change of the selected item within the category, e.g. selecting a different girl.
	if (Item_Change)
	{
        change_item();
        Item_Change = false;
    }

    EditTextItem(active_building().name(), brothel_id);

	// Draw the image
	if (m_ActiveCategory == Summary_BUILDINGS)
	{
        SetImage(image_id, g_Graphics.LoadBrothelImage(active_building().background_image()));
		if (imagename_id >= 0)	m_TextItems[imagename_id]->SetText("");
	}
	else if (m_ActiveCategory == Summary_GANGS)
	{
		SetImage(image_id, g_Graphics.LoadBrothelImage(active_building().background_image()));
		if (imagename_id >= 0)	m_TextItems[imagename_id]->SetText("");
	}
	else if (g_selected_girl && Image_Change)
	{
		Image_Change = false;
		bool random = true;
		if ((g_selected_girl->m_newRandomFixed >= 0) && (Image_Type == IMGTYPE_PROFILE))
		{
			random = false;
			Image = g_selected_girl->m_newRandomFixed;
		}
		PrepareImage(image_id, g_selected_girl, Image_Type, random, Image);
		if (imagename_id >= 0)
		{
			string t;
			if (m_Images[image_id]) t = m_Images[image_id]->m_Image->GetFilename();
			m_TextItems[imagename_id]->SetText(t);
		}
	}
	else if (Image_Change)
	{
		m_Images[image_id]->m_Image = new CSurface(ImagePath("blank.png"));
		m_Images[image_id]->m_AnimatedImage = nullptr;
		m_Images[image_id]->m_Image->m_Message = "";
		if (imagename_id >= 0)	m_TextItems[imagename_id]->SetText("");
	}

}

void cScreenTurnSummary::change_event()
{
    if(Event < 0)
        return;

    string text;
    if (g_selected_girl && (m_ActiveCategory == Summary_DUNGEON || m_ActiveCategory == Summary_GIRLS)) {
        if (!g_selected_girl->m_Events.IsEmpty()) {
            text       = g_selected_girl->m_Events.GetMessage(Event).m_Message;
            Image_Type = g_selected_girl->m_Events.GetMessage(Event).m_MessageType;
        }
        Image_Change = true;
    } else if (m_ActiveCategory == Summary_GANGS) {
        if (g_Game.gang_manager().GetGang(Item) && !g_Game.gang_manager().GetGang(Item)->m_Events.IsEmpty())
            text = g_Game.gang_manager().GetGang(Item)->m_Events.GetMessage(Event).m_Message;
    } else if (m_ActiveCategory == Summary_BUILDINGS) {
        IBuilding * brothel = &g_Game.buildings().get_building(Item);
        if (!brothel->m_Events.IsEmpty()) {
            text = brothel->m_Events.GetMessage(Event).m_Message;
            EditTextItem(brothel->name(), brothel_id);
        }
    }
    EditTextItem(text, labeldesc_id);
}

void cScreenTurnSummary::change_item()
{
    ClearListBox(event_id);
    switch(m_ActiveCategory) {
    case Summary_GANGS:
        Fill_Events_GANGS();
        break;
    case Summary_BUILDINGS:
        Fill_Events_BROTHELS();
        break;
    case Summary_DUNGEON:
        if (g_Game.dungeon().GetGirlByName(GetSelectedTextFromList(item_id)))
            g_selected_girl = g_Game.dungeon().GetGirlByName(GetSelectedTextFromList(item_id))->m_Girl.get();
        break;
    case Summary_GIRLS:
        g_selected_girl = active_building().find_girl_by_name(GetSelectedTextFromList(item_id));
        Fill_Events(g_selected_girl);
        break;
    }

    Event = 0;
    SetSelectedItemInList(event_id, Event);
    change_event();
    Image_Change = true;
}

void cScreenTurnSummary::change_category(SummaryCategory new_category)
{
    ClearListBox(item_id);
    ClearListBox(event_id);
    switch(new_category) {
    case Summary_GANGS:
        Fill_Items_GANGS();
        break;
    case Summary_BUILDINGS:
        Fill_Items_BUILDINGS();
        break;
    case Summary_DUNGEON:
        Fill_Items_DUNGEON();
        break;
    case Summary_GIRLS:
        Fill_Items_Building(&active_building());
        break;
    }

    if (m_ActiveCategory == new_category) Item = GetSelectedItemFromList(item_id);
    else { Item = 0; }
    if (Item >= GetListBoxSize(item_id) || Item < 0) Item = 0;
    SetSelectedItemInList(item_id, Item);

    m_ActiveCategory = new_category;

    string sorttext = "ITEM";
    if (summarysortorder == 1 && m_ActiveCategory == Summary_GIRLS)
    {
        if (active_building().type() == BuildingType::STUDIO)	{ sorttext += " (Jobs)"; }
        if (active_building().type() == BuildingType::CLINIC)	{ sorttext += " (Triage)"; }
    }
    EditTextItem(sorttext, item_id);

    change_item();
}

void cScreenTurnSummary::init(bool back)
{
	Focused();

	if (g_selected_girl)
	{
		if (g_selected_girl->m_DayJob == JOB_INDUNGEON)	m_ActiveCategory = Summary_DUNGEON;
		else {
            m_ActiveCategory = Summary_GIRLS;
		}
	}

	EditTextItem(active_building().name(), brothel_id);

	if (gold_id >= 0)
	{
		stringstream ss; ss << "Gold: " << g_Game.gold().ival();
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
	change_category(m_ActiveCategory);

	int size = GetListBoxSize(item_id);
	if (Item >= size)	Item = size;
	SetSelectedItemInList(item_id, Item);

}

void cScreenTurnSummary::goto_selected()
{
    /*
    g_GirlDetails->lastsexact = -1;
    string selectedName = GetSelectedTextFromList(item_id);
    switch (Category) {
    case Summary_GANGS: {
        g_CurrentScreen = SCREEN_GANGMANAGEMENT;
        push_window("Gangs");
    }
        break;
    case Summary_GIRLS: {
        auto girl = active_building().find_girl_by_name(selectedName);
        if (girl) {
            g_CurrentScreen = SCREEN_GIRLDETAILS;
            g_GirlDetails->SetSelectedGirl(girl);
            push_window("Girl Details");
        } else {
// TODO figure out target screen here!
            if (Item < 0 || Item > g_Game.buildings().num_buildings(BuildingType::BROTHEL)) g_CurrBrothel = 0;
            else g_CurrBrothel = Item;
            g_CurrentScreen    = SCREEN_GIRLMANAGEMENT;
            g_WinManager.PopToWindow(g_BrothelManagement);
        }
    }
        break;
    case Summary_DUNGEON: {
        sDungeonGirl * dg   = g_Game.dungeon().GetGirlByName(selectedName);
        sGirl        * girl = nullptr;
        if (dg) girl = g_Game.dungeon().GetGirlByName(selectedName)->m_Girl.get();
        if (girl) {
            g_CurrentScreen = SCREEN_GIRLDETAILS;
            g_GirlDetails->SetSelectedGirl(girl);
            push_window("Girl Details");
        } else {
            g_CurrentScreen = SCREEN_DUNGEON;
            push_window("Dungeon");
        }
    }
        break;
    case Summary_BUILDINGS: {
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//    TODO: FIX THIS: Select correct girl and use current building screen!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Clinic, Farm, House, Arena, Centre
        auto girl = active_building().find_girl_by_name(selectedName);
        if (girl) {
            g_GirlDetails->SetSelectedGirl(girl);
            push_window("Girl Details");
        } else {
            push_window("Building Management");
        }
    }

    }
    */
}

void cScreenTurnSummary::Fill_Items_GANGS()
{
	for (int i = 0; i < g_Game.gang_manager().GetNumGangs(); i++)
		AddToListBox(item_id, i, g_Game.gang_manager().GetGang(i)->name());
}
void cScreenTurnSummary::Fill_Items_BUILDINGS()
{
    auto& active = active_building();
	for (int i = 0; i < g_Game.buildings().num_buildings(); i++) {
        IBuilding& next = g_Game.buildings().get_building(i);
        if(&active == &next) {
            Item = i;
        }
        AddToListBox(item_id, i, next.name());
    }
}
void cScreenTurnSummary::Fill_Items_DUNGEON()
{
	// Fill the list box
	cDungeon* pDungeon = &g_Game.dungeon();
	int ID = 0, nNumGirls = pDungeon->GetNumGirls();
	vector<sGirl*> tmpGoodNewsGirls, tmpDangerGirls, tmpWarningGirls, tmpOtherGirls;
	tmpGoodNewsGirls.clear(); tmpDangerGirls.clear(); tmpWarningGirls.clear(); tmpOtherGirls.clear();
	sGirl* pTmpGirl;

	for (int i = 0; i < nNumGirls; i++)
	{
		pTmpGirl = pDungeon->GetGirl(i)->m_Girl.get();

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
		if (g_selected_girl == tmpDangerGirls[i]) Item = ID;
		ID++;
	}
	//Girls with GoodNews events
	for (u_int i = 0; i < tmpGoodNewsGirls.size(); i++)
	{
		string tname = tmpGoodNewsGirls[i]->m_Realname;
		AddToListBox(item_id, ID, tname, COLOR_GREEN);
		if (g_selected_girl == tmpGoodNewsGirls[i]) Item = ID;
		ID++;
	}
	//Girls wih Warnings
	for (u_int i = 0; i < tmpWarningGirls.size(); i++)
	{
		string tname = tmpWarningGirls[i]->m_Realname;
		AddToListBox(item_id, ID, tname, COLOR_DARKBLUE);
		if (g_selected_girl == tmpWarningGirls[i]) Item = ID;
		ID++;
	}
	//ServiceJob Girls
	for (u_int i = 0; i < tmpOtherGirls.size(); i++)
	{
		string tname = tmpOtherGirls[i]->m_Realname;
		AddToListBox(item_id, ID, tname);
		if (g_selected_girl == tmpOtherGirls[i]) Item = ID;
		ID++;
	}
}

void cScreenTurnSummary::Fill_Events(sGirl* girl)
{
	if (girl == nullptr) return;
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
	sGang* gang = g_Game.gang_manager().GetGang(Item);
	if (gang == nullptr) return;
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
    int num_brothels = g_Game.buildings().num_buildings(BuildingType::BROTHEL);
    if (Item >= num_brothels) Item = num_brothels - 1;
	auto& pSelectedBrothel = *g_Game.buildings().building_with_type(BuildingType::BROTHEL, Item);
	if (!pSelectedBrothel.m_Events.IsEmpty())
	{
		pSelectedBrothel.m_Events.DoSort();						// Sort Events to put Warnings & Dangers first.

		for (int l = 0; l < pSelectedBrothel.m_Events.GetNumEvents(); l++)
		{
			string			sTitle = pSelectedBrothel.m_Events.GetMessage(l).TitleText();
			unsigned int	uiListboxColour = pSelectedBrothel.m_Events.GetMessage(l).ListboxColour();
			AddToListBox(event_id, l, sTitle, uiListboxColour);
		}
	}
}

struct EventRating {
    double ordering;
    int color;
};

// ordering methods
//  default
EventRating default_rating(const sGirl& g) {
    if (g.m_Events.HasDanger())
    {
        return {3.0, COLOR_RED};
    }
    else if (g.m_Events.HasGoodNews())
    {
        return {2.0, COLOR_GREEN};
    }
    else if (g.m_Events.HasWarning())
    {
        return {1.0, COLOR_DARKBLUE};
    }

    return {0.0, COLOR_BLUE};
}

bool is_sex_job(JOBS job) {
    switch (job)
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
        return true;
    default:
        return false;
    }
}

EventRating brothel_rating(const sGirl& g) {
    bool sexjob = is_sex_job((JOBS)g.m_DayJob) || is_sex_job((JOBS)g.m_NightJob);

    if(!g.m_Events.HasUrgent() && sexjob) {
        return {0.0, COLOR_BLUE};
    } else if (g.m_Events.HasDanger()) {
        return {4.0, COLOR_RED};
    } else if (g.m_Events.HasGoodNews()) {
        return {3.0, COLOR_GREEN};
    } else if (g.m_Events.HasWarning()) {
        return {2.0, COLOR_DARKBLUE};
    }

    return {1.0, COLOR_BLUE};
}



//  clinic
EventRating clinic_rating(const sGirl& g)
{
    if (g.m_DayJob == JOB_DOCTOR || g.m_NightJob == JOB_DOCTOR) {
        return {-1.0, COLOR_DARKBLUE};
    }

    bool surgery_job = cJobManager::is_Surgery_Job(g.m_DayJob) ||
                       cJobManager::is_Surgery_Job(g.m_NightJob);
    if (surgery_job && g.m_WorkingDay >= 1) {
        return {(double) g.m_WorkingDay, COLOR_YELLOW};
    } else if (surgery_job || g.m_DayJob == JOB_GETHEALING || g.m_NightJob == JOB_GETHEALING
               || g.m_DayJob == JOB_GETREPAIRS || g.m_NightJob == JOB_GETREPAIRS) {
        return {0.0, COLOR_BLUE};
    } else if (g.m_DayJob == JOB_NURSE || g.m_NightJob == JOB_NURSE) {
        return {-2.0, COLOR_BLUE};
    }
    return {-3.0, COLOR_BLUE};
}

//  studio
EventRating studio_rating_job(const sGirl& g)
{
    if (g.m_NightJob == JOB_DIRECTOR) {
        return {5.0, COLOR_RED};
    } else if (g.m_NightJob == JOB_CAMERAMAGE || g.m_NightJob == JOB_CRYSTALPURIFIER) {
        return {4.0, COLOR_DARKBLUE};
    } else if (g.m_NightJob == JOB_PROMOTER || g.m_NightJob == JOB_FLUFFER || g.m_NightJob == JOB_STAGEHAND) {
        return {3.0, COLOR_BLUE};
    } else if (g.m_NightJob == JOB_FILMFREETIME) {
        return {0.0, COLOR_RED};
    }
    return {0.0, COLOR_GREEN};
}

EventRating studio_rating_default(const sGirl& g)
{
    bool sexjob = false;
    switch (g.m_NightJob)
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

    if (!g.m_Events.HasUrgent() && sexjob)	{
        return {1.0, COLOR_BLUE};
    }
    else if (!g.m_Events.HasUrgent())		{ return {2.0, COLOR_BLUE}; }
    else if (g.m_Events.HasDanger())		{ return {5.0, COLOR_RED}; }
    else if (g.m_Events.HasGoodNews())		{ return {4.0, COLOR_GREEN}; }
    else                                        { return {3.0, COLOR_DARKBLUE}; }
}




void cScreenTurnSummary::Fill_Items_Building(IBuilding * building)
{
    std::vector<sGirl*> all_girls;
    all_girls.reserve(building->num_girls());
    for(auto& girl : building->girls()) {
        all_girls.push_back(girl);
    }

    std::function<EventRating(const sGirl& g)> rating_fn = default_rating;
    if (summarysortorder == 1 && building->type() == BuildingType::CLINIC) {
        rating_fn = clinic_rating;
    } else if (building->type() == BuildingType::STUDIO) {
        rating_fn = summarysortorder == 1 ? studio_rating_job : studio_rating_default;
    } else if (building->type() == BuildingType::BROTHEL) {
        rating_fn = brothel_rating;
    }

    std::sort(begin(all_girls), end(all_girls), [&](sGirl* a, sGirl* b) {
        return rating_fn(*a).ordering > rating_fn(*b).ordering;
    });

    int ID = 0;
    for(auto& girl : all_girls) {
        AddToListBox(item_id, ID, girl->m_Realname, rating_fn(*girl).color);
        if (g_selected_girl == girl) Item = ID;
        ID++;
    }
}

void cScreenTurnSummary::OnKeyPress(SDL_keysym key)
{
    if(key.sym == SDLK_RETURN) {
        if (cfg.resolution.next_turn_enter())
        {
            if (!g_CTRLDown) { g_CTRLDown = false; AutoSaveGame(); }
            NextWeek();
            init(false);
        }
    } else if(key.sym == SDLK_o) {
        if (summarysortorder == 0) summarysortorder = 1;
        else summarysortorder = 0;
        change_category(m_ActiveCategory);
    }
}
