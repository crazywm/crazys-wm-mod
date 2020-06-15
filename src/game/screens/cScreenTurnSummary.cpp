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
#pragma region //    Includes and Externs            //
#include <iostream>
#include <functional>
#include <main.h>
#include "cScreenTurnSummary.h"

#include "buildings/cBrothel.h"

#include "cGangs.h"
#include "cScreenGirlDetails.h"
#include "InterfaceProcesses.h"
#include "cScreenBrothelManagement.h"

#include "widgets/cImageItem.h"
#include "utils/FileList.h"
#include "Game.hpp"
#include "interface/cSurface.h"
#include "cJobManager.h"
#include "sConfig.h"

extern cConfig cfg;

extern    cScreenGirlDetails*        g_GirlDetails;

extern    bool    g_CTRLDown;

#pragma endregion
#pragma region //    Local Variables            //

static bool    Item_Change = false;
static int    Event = -1;
static int    Image = -1;
static int    Image_Type = -1;
static bool    Image_Change = false;
static int summarysortorder = 0;    // the order girls get sorted in the summary lists
#pragma endregion

cScreenTurnSummary::cScreenTurnSummary() : cGameWindow("TurnSummary.xml")
{
}

void cScreenTurnSummary::set_ids()
{
    back_id            = get_id("BackButton", "Back");
    brothel_id        = get_id("CurrentBrothel");
    category_id        = get_id("Category");
    gold_id            = get_id("Gold", "*Unused*");//
    labelitem_id    = get_id("LabelItem");
    item_id            = get_id("Item");
    event_id        = get_id("Event");
    labeldesc_id    = get_id("LabelDesc");
    goto_id            = get_id("GoTo");
    nextweek_id        = get_id("NextWeek");
    prev_id            = get_id("PrevButton","Prev");
    next_id            = get_id("NextButton","Next");
    image_id        = get_id("Background");
    imagename_id    = get_id("ImageName", "*Unused*");//

    SetButtonNavigation(back_id, "<back>");
    /*
    SetButtonCallback(next_id, [this]() {
        cycle_building(1);
        change_category(m_ActiveCategory);
    });
    SetButtonCallback(prev_id, [this]() {
        cycle_building(-1);
        change_category(m_ActiveCategory);
    });
    */
    SetButtonCallback(nextweek_id, [this]() {
        if (!g_CTRLDown) { AutoSaveGame(); }
        NextWeek();
        init(false);
    });

    SetListBoxHotKeys(category_id, SDLK_e, SDLK_q);
    SetListBoxSelectionCallback(category_id, [this](int selection) {
        int sc = selection % 10;
        if(sc == Summary_GIRLS) {
            set_active_building( &g_Game->buildings().get_building(selection / 10) );
        }
        change_category((SummaryCategory)sc);
    });

    SetListBoxHotKeys(item_id, SDLK_a, SDLK_d);
    SetListBoxSelectionCallback(item_id, [this](int selection) {
        change_item(selection);
    });

    SetListBoxHotKeys(event_id, SDLK_w, SDLK_s);
    SetListBoxSelectionCallback(event_id, [this](int selection) {
        change_event(selection);
    });

    SetButtonCallback(goto_id, [this]() { goto_selected(); });
    SetButtonHotKey(goto_id, SDLK_SPACE);

    AddKeyCallback(SDLK_o, [this]() {
        summarysortorder = summarysortorder == 0 ? 1 : 0;
        change_category(m_ActiveCategory);
    });

    AddKeyCallback(SDLK_RETURN, [this]() {
        if (cfg.resolution.next_turn_enter())
        {
            if (!g_CTRLDown) { g_CTRLDown = false; AutoSaveGame(); }
            NextWeek();
            init(false);
        }
    });
}

void cScreenTurnSummary::init(bool back)
{
    Focused();

    if (selected_girl())
    {
        if (selected_girl()->m_DayJob == JOB_INDUNGEON)    m_ActiveCategory = Summary_DUNGEON;
        else {
            m_ActiveCategory = Summary_GIRLS;
            set_active_building(selected_girl()->m_Building);
        }
    }

    EditTextItem(active_building().name(), brothel_id);

    if (gold_id >= 0)
    {
        stringstream ss; ss << "Gold: " << g_Game->gold().ival();
        EditTextItem(ss.str(), gold_id);
    }

    // Clear everything
    ClearListBox(category_id);
    ClearListBox(item_id);
    ClearListBox(event_id);
    EditTextItem("", labeldesc_id);

    AddToListBox(category_id, Summary_BUILDINGS, "BUILDINGS");
    AddToListBox(category_id, Summary_GANGS, "GANGS");
    AddToListBox(category_id, Summary_DUNGEON, "DUNGEON");
    for(int i = 0; i < g_Game->buildings().num_buildings(); ++i) {
        AddToListBox(category_id, Summary_GIRLS + 10 * i, g_Game->buildings().get_building(i).name());
    }
    change_category(m_ActiveCategory);
}


void cScreenTurnSummary::process()
{
    // process a change of the selected item within the category, e.g. selecting a different girl.
    if (Item_Change)
    {
        change_item(0);
        Item_Change = false;
    }

    EditTextItem(active_building().name(), brothel_id);

    // Draw the image
    if (m_ActiveCategory == Summary_BUILDINGS)
    {
        SetImage(image_id, active_building().background_image());
        if (imagename_id >= 0)    EditTextItem("", imagename_id);
    }
    else if (m_ActiveCategory == Summary_GANGS)
    {
        SetImage(image_id, active_building().background_image());
        if (imagename_id >= 0)    EditTextItem("", imagename_id);
    }
    else {
        cImageItem * image_item = GetImage(image_id);
        if (selected_girl() && Image_Change)
        {
            Image_Change = false;
            PrepareImage(image_id, selected_girl(), Image_Type, true, Image);
            if (imagename_id >= 0)
            {
                string t;
                if (image_item) t = image_item->m_Image.GetFileName();
                EditTextItem(t, imagename_id);
            }
        }
        else if (Image_Change)
        {
            SetImage(image_id, "blank.png");
            image_item->m_Message       = "";
            if (imagename_id >= 0)    EditTextItem("", imagename_id);
        }
    }

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
        Fill_Items_GIRLS(&active_building());
        break;
    }

    // if the category has not changed, keep the selected item
    int new_item = 0;
    if (m_ActiveCategory == new_category) {
        new_item = GetSelectedItemFromList(item_id);
    }
    if (new_item >= GetListBoxSize(item_id) || new_item < 0) new_item = 0;
    SetSelectedItemInList(item_id, new_item);

    m_ActiveCategory = new_category;

    string sorttext = "ITEM";
    if (summarysortorder == 1 && m_ActiveCategory == Summary_GIRLS)
    {
        if (active_building().type() == BuildingType::STUDIO)    { sorttext += " (Jobs)"; }
        if (active_building().type() == BuildingType::CLINIC)    { sorttext += " (Triage)"; }
    }
    EditTextItem(sorttext, labelitem_id);

    change_item(0);
}

void cScreenTurnSummary::change_event(int selection)
{
    Event = selection;
    if(selection < 0)
        return;

    m_ActiveReport = nullptr;

    std::string text;
    if (selected_girl() && (m_ActiveCategory == Summary_DUNGEON || m_ActiveCategory == Summary_GIRLS)) {
        if (!selected_girl()->m_Events.IsEmpty()) {
            const CEvent& event = selected_girl()->m_Events.GetMessage(selection);
            text       = event.GetMessage();
            Image_Type = event.GetMessageType();
            if(event.IsCombat()) {
                m_ActiveReport = event.GetReport();
            }
        }
        Image_Change = true;
    } else if (m_ActiveCategory == Summary_GANGS) {
        int active_gang = GetSelectedItemFromList(item_id);
        auto* gang = g_Game->gang_manager().GetGang(active_gang);
        if (gang && !gang->m_Events.IsEmpty()) {
            const CEvent& event = gang->m_Events.GetMessage(selection);
            text = event.GetMessage();
            if(event.IsCombat()) {
                m_ActiveReport = event.GetReport();
            }
        }
    } else if (m_ActiveCategory == Summary_BUILDINGS) {
        auto building_id = GetSelectedItemFromList(item_id);
        if(building_id < 0 || building_id >= g_Game->buildings().num_buildings())
            building_id = 0;
        IBuilding * brothel = &g_Game->buildings().get_building(building_id);
        if (!brothel->m_Events.IsEmpty()) {
            text = brothel->m_Events.GetMessage(selection).GetMessage();
            EditTextItem(brothel->name(), brothel_id);
        }
    }
    EditTextItem(text, labeldesc_id);
}

void cScreenTurnSummary::change_item(int selection)
{
    ClearListBox(event_id);
    switch(m_ActiveCategory) {
    case Summary_GANGS:
        Fill_Events_Gang(selection);
        break;
    case Summary_BUILDINGS:
        Fill_Events_Buildings(selection);
        break;
    case Summary_DUNGEON:
        if (g_Game->dungeon().GetGirlByName(GetSelectedTextFromList(item_id))) {
            set_active_girl(g_Game->dungeon().GetGirlByName(GetSelectedTextFromList(item_id))->m_Girl.get());
        }
        break;
    case Summary_GIRLS:
        set_active_girl(active_building().find_girl_by_name(GetSelectedTextFromList(item_id)));
        Fill_Events(selected_girl());
        break;
    }

    Event = 0;
    SetSelectedItemInList(event_id, Event);
    change_event(0);
    Image_Change = true;
}

void cScreenTurnSummary::goto_selected()
{
    if(m_ActiveReport) {
        for(auto& turn : m_ActiveReport->rounds) {
            push_message(turn, 0);
        }
        return;
    }
    string selectedName = GetSelectedTextFromList(item_id);
    switch (m_ActiveCategory)
    {
    case Summary_GANGS:
        push_window("Gangs");
        break;

    case Summary_GIRLS: {
        auto girl = active_building().find_girl_by_name(selectedName);
        if (girl) {
            set_active_girl(girl);
            push_window("Girl Details");
        }
        break;
    }
    case Summary_DUNGEON: {
        sDungeonGirl * dg   = g_Game->dungeon().GetGirlByName(selectedName);
        if (dg) {
            auto* girl = g_Game->dungeon().GetGirlByName(selectedName)->m_Girl.get();
            if(girl) {
                set_active_girl(girl);
                push_window("Girl Details");
            }
        } else {
            push_window("Dungeon");
        }
    }
    break;
    case Summary_BUILDINGS: {
        // set active building?
        auto bld = &g_Game->buildings().get_building(GetSelectedItemFromList(item_id));
        set_active_building(bld);
        push_window("Building Management");
    }
    }
}

void cScreenTurnSummary::Fill_Items_GANGS()
{
    for (int i = 0; i < g_Game->gang_manager().GetNumGangs(); i++)
        AddToListBox(item_id, i, g_Game->gang_manager().GetGang(i)->name());
}

void cScreenTurnSummary::Fill_Items_BUILDINGS()
{
    for (int i = 0; i < g_Game->buildings().num_buildings(); i++) {
        IBuilding& next = g_Game->buildings().get_building(i);
        AddToListBox(item_id, i, next.name());
    }
}

void cScreenTurnSummary::Fill_Events(sGirl* girl)
{
    if (girl == nullptr) return;

    Image_Change = true;
    Image_Type = IMGTYPE_PROFILE;
    if (!girl->m_Events.IsEmpty())
    {
        girl->m_Events.DoSort();        // Sort Events to put Warnings & Dangers first.
        for (int l = 0; l < girl->m_Events.GetNumEvents(); l++)
        {
            string            sTitle = girl->m_Events.GetMessage(l).TitleText();
            unsigned int    uiListboxColour = girl->m_Events.GetMessage(l).ListboxColour();
            AddToListBox(event_id, l, sTitle, uiListboxColour);
        }
    }
    if (GetListBoxSize(event_id) > 0) SetSelectedItemInList(event_id, 0);
}

void cScreenTurnSummary::Fill_Events_Gang(int gang_id)
{
    sGang* gang = g_Game->gang_manager().GetGang(gang_id);
    if (gang == nullptr) return;
    if (!gang->m_Events.IsEmpty())
    {
        gang->m_Events.DoSort();                        // Sort Events to put Warnings & Dangers first.

        for (int l = 0; l < gang->m_Events.GetNumEvents(); l++)
        {
            string            sTitle = gang->m_Events.GetMessage(l).TitleText();
            unsigned int    uiListboxColour = gang->m_Events.GetMessage(l).ListboxColour();
            AddToListBox(event_id, l, sTitle, uiListboxColour);
        }
    }
    if (GetListBoxSize(event_id) > 0) SetSelectedItemInList(event_id, 0);
}
void cScreenTurnSummary::Fill_Events_Buildings(int building_id)
{
    auto& pSelectedBrothel = g_Game->buildings().get_building(building_id);
    if (!pSelectedBrothel.m_Events.IsEmpty())
    {
        pSelectedBrothel.m_Events.DoSort();                        // Sort Events to put Warnings & Dangers first.

        for (int l = 0; l < pSelectedBrothel.m_Events.GetNumEvents(); l++)
        {
            string            sTitle = pSelectedBrothel.m_Events.GetMessage(l).TitleText();
            unsigned int    uiListboxColour = pSelectedBrothel.m_Events.GetMessage(l).ListboxColour();
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

    if (!g.m_Events.HasUrgent() && sexjob)    {
        return {1.0, COLOR_BLUE};
    }
    else if (!g.m_Events.HasUrgent())        { return {2.0, COLOR_BLUE}; }
    else if (g.m_Events.HasDanger())        { return {5.0, COLOR_RED}; }
    else if (g.m_Events.HasGoodNews())        { return {4.0, COLOR_GREEN}; }
    else                                        { return {3.0, COLOR_DARKBLUE}; }
}




void cScreenTurnSummary::Fill_Items_GIRLS(IBuilding * building)
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
        AddToListBox(item_id, ID, girl->FullName(), rating_fn(*girl).color);
        if (selected_girl() == girl) {
            SetSelectedItemInList(item_id, ID);
        }
        ID++;
    }
}

void cScreenTurnSummary::Fill_Items_DUNGEON()
{
    cDungeon* pDungeon = &g_Game->dungeon();
    std::vector<sGirl*> all_girls;
    all_girls.reserve(pDungeon->GetNumGirls());
    for(auto& girl : pDungeon->girls()) {
        all_girls.push_back(girl.m_Girl.get());
    }

    std::function<EventRating(const sGirl& g)> rating_fn = default_rating;

    std::sort(begin(all_girls), end(all_girls), [&](sGirl* a, sGirl* b) {
        return rating_fn(*a).ordering > rating_fn(*b).ordering;
    });

    int ID = 0;
    for(auto& girl : all_girls) {
        AddToListBox(item_id, ID, girl->FullName(), rating_fn(*girl).color);
        if (selected_girl() == girl) {
            SetSelectedItemInList(item_id, ID);
        }
        ID++;
    }
}
