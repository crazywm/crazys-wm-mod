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
#include "buildings/cBrothel.h"
#include "cScreenPrison.h"
#include "interface/cWindowManager.h"
#include "cGold.h"
#include "Game.hpp"

static int ImageNum = -1;

cScreenPrison::cScreenPrison() : cInterfaceWindowXML("prison_screen.xml")
{
	selection = -1;
}

void cScreenPrison::init(bool back)
{
	stringstream ss;
	Focused();

    DisableWidget(more_id, true);
    DisableWidget(release_id, true);
	selection = -1;
	update_details();

	int i = 0;
	ClearListBox(prison_list_id);
	for(sGirl* pgirls : g_Game->GetPrison())
	{
		stringstream ss;
		int cost = PrisonReleaseCost(pgirls);
		ss << pgirls->m_Realname << "  (release cost: " << cost << " gold)";
		AddToListBox(prison_list_id, i, ss.str());
		i++;
	}
}

void cScreenPrison::set_ids()
{
	header_id			/**/ = get_id("ScreenHeader");
	back_id				/**/ = get_id("BackButton","Back");
	more_id				/**/ = get_id("ShowMoreButton");
	release_id			/**/ = get_id("ReleaseButton");
	prison_list_id		/**/ = get_id("PrisonList");
	girl_desc_id		/**/ = get_id("GirlDescription");
	girlimage_id		/**/ = get_id("GirlImage", "*Unused*");//
	DetailLevel			/**/ = 0;

	SetButtonNavigation(back_id, "<back>");
	SetButtonCallback(more_id, [this]() { more_button(); });
	SetButtonCallback(release_id, [this]() {
	    release_button();
        init(false);
	});

	SetListBoxSelectionCallback(prison_list_id, [this](int sel) { selection_change(0); });
    SetListBoxHotKeys(prison_list_id, SDLK_a,  SDLK_d);
}

void cScreenPrison::selection_change(int selection)
{
    DisableWidget(more_id, (selection == -1));
    DisableWidget(release_id, (selection == -1));
	update_details();
}

void cScreenPrison::update_details()
{
	EditTextItem("No Prisoner Selected", girl_desc_id);
	if (selection == -1) return;
	sGirl* pgirls = get_selected_girl();
	if (!pgirls) return;

	if (DetailLevel == 1)		EditTextItem(cGirls::GetMoreDetailsString(pgirls, true), girl_desc_id);
	else if (DetailLevel == 2)	EditTextItem(cGirls::GetThirdDetailsString(pgirls), girl_desc_id);
	else						EditTextItem(cGirls::GetDetailsString(pgirls, true), girl_desc_id);
}

sGirl* cScreenPrison::get_selected_girl()
{
	if (selection == -1) return nullptr;
	int i = 0;
    for(sGirl* pgirls : g_Game->GetPrison()) {
		if (i == selection) return pgirls;
		i++;
	}
	return nullptr;
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
	if (!g_Game->gold().afford((double)cost))
	{
		push_message("You don't have enough gold", 0);
		return;
	}
	g_Game->gold().item_cost((double)cost);
	g_Game->gold().girl_support((double)cost);
	g_Game->RemoveGirlFromPrison(pgirls);
    IBuilding& bld = active_building();
    if (bld.free_rooms() < 1)
	{
		string text = pgirls->m_Realname;
		text += " has been sent to your dungeon, since current brothel is full.";
		g_Game->push_message(text, 0);
		g_Game->dungeon().AddGirl(pgirls, DUNGEON_NEWGIRL);
	}
	else
	{
		string text = pgirls->m_Realname;
		text += " has been sent to your current brothel.";
		g_Game->push_message(text, 0);
        bld.add_girl(pgirls);
	}
}

int cScreenPrison::PrisonReleaseCost(sGirl* girl)
{
	cGirls::CalculateAskPrice(girl, false);
	int cost = girl->askprice() * 15;
	cost += cGirls::GetSkillWorth(girl);
	if (girl->check_virginity()) cost += int(cost / 2);	//	`J` fixed virgin adds half cost more
	cost *= 2;
	return cost;
}

void cScreenPrison::update_image()
{
	if (get_selected_girl() && !IsMultiSelected(prison_list_id))//This may need fixed CRAZY
	{
		PrepareImage(girlimage_id, get_selected_girl(), IMGTYPE_JAIL, true, ImageNum);
        HideWidget(girlimage_id, false);
	}
	else HideWidget(girlimage_id, true);
}
