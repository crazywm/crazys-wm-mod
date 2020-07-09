/*
* Copyright 2009, 2010, The Pink Petal Development Team.
* The Pink Petal Devloment Team are defined as the game's coders
* who meet on http://pinkpetal.org
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
#include "InterfaceProcesses.h"
#include "buildings/cBrothel.h"
#include "scripting/cScriptManager.h"
#include "interface/cWindowManager.h"
#include "character/cPlayer.h"
#include "cScreenBrothelManagement.h"
#include "utils/FileList.h"
#include "Game.hpp"
#include "CLog.h"
#include "sStorage.hpp"
#include "scripting/GameEvents.h"
#include <sstream>
#include "sConfig.h"

extern bool g_CTRLDown;
extern cConfig cfg;

extern bool g_TryOuts;
extern bool g_TryCentre;
extern bool g_TryEr;
extern bool g_TryCast;

static std::string get_building_summary(const IBuilding& building);

// -------------------------------------------------------------------------------------------------

void IBuildingScreen::set_ids()
{
    buildinglabel_id = get_id("BuildingLabel", "Header");
    background_id    = get_id("Background");
    // TODO walk button does not exist for all building types.
    walk_id             = get_id("WalkButton");

    weeks_id         = get_id("Next Week", "Weeks");
    girls_id         = get_id("Girl Management", "Girls");
    staff_id         = get_id("Staff Management", "Staff");
    setup_id         = get_id("Setup", "SetUp");
    dungeon_id         = get_id("Dungeon");
    turns_id         = get_id("Turn Summary", "Turn");

    girlimage_id     = get_id("GirlImage");

    prevbrothel_id     = get_id("PrevButton", "Prev", "*Unused*");
    nextbrothel_id     = get_id("NextButton", "Next", "*Unused*");

    details_id       = get_id("BuildingDetails", "Details");
    town_id             = get_id("Visit Town");
    save_id             = get_id("Save");
    quit_id              = get_id("Quit");

    // set button callbacks
    // TODO walk button does not exist for all building types.
    SetButtonCallback(walk_id, [this]() { try_walk(); });
    SetButtonNavigation(town_id, "Town", false);
    SetButtonNavigation(dungeon_id, "Dungeon", false);
    SetButtonNavigation(setup_id,"Building Setup", false);
    SetButtonNavigation(turns_id, "Turn Summary", false);
    SetButtonNavigation(staff_id, "Gangs", false);
    SetButtonNavigation(girls_id, "Girl Management", false);
    SetButtonCallback(prevbrothel_id, [this]() {
        cycle_building(-1);
        replace_window("Building Management");
    });
    SetButtonCallback(nextbrothel_id, [this]() {
        cycle_building(1);
        replace_window("Building Management");
    });

    SetButtonCallback(weeks_id, [this]() {
        if (!g_CTRLDown) { AutoSaveGame(); }
        // need to switch the windows first, so that any new events will show up!
        push_window("Turn Summary");
        NextWeek();
    });
    SetButtonCallback(save_id, [this]() {
        SaveGame();
        g_Game->push_message("Game Saved", COLOR_GREEN);
    });
    SetButtonCallback(quit_id, [this]() {
        input_confirm([this]() {
            pop_to_window("Main Menu");
        });
    });
}

void IBuildingScreen::process()
{
    // TODO figure out when/how to enable the image
    if (girlimage_id != -1) HideWidget(girlimage_id, true);
}

IBuildingScreen::IBuildingScreen(const char * base_file, BuildingType building) :
        cGameWindow(base_file),
        m_Type(building)
{
}

void IBuildingScreen::init(bool back)
{
    Focused();

    if(active_building().type() != m_Type) {
        replace_window("Building Management");
        return;
    }

    EditTextItem(get_building_summary(active_building()), details_id);
    std::stringstream ss;

    ss << "Day: " << g_Game->date().day << " Month: " << g_Game->date().month << " Year: " << g_Game->date().year
       << " -- " << active_building().type_str() << ": " << active_building().name();
    EditTextItem(ss.str(), buildinglabel_id);
    DisableWidget(walk_id, !active_building().CanEncounter());
    SetImage(background_id, active_building().background_image());
}

void IBuildingScreen::try_walk()
{
    sGirl* girl = active_building().TryEncounter();
    if (girl == nullptr)                                                // if there's no girl, no meeting
    {
        return;
    }

    if (girlimage_id != -1)
    {
        PrepareImage(girlimage_id, girl, IMGTYPE_PROFILE, true, -1);
        HideWidget(girlimage_id, false);
    }
}

static std::string fame_text(const IBuilding* brothel)
{
    stringstream ss;
    /* */if (brothel->m_Fame >= 90)/*     */    ss << "World Renowned";
    else if (brothel->m_Fame >= 80)/*     */    ss << "Famous";
    else if (brothel->m_Fame >= 70)/*     */    ss << "Well Known";
    else if (brothel->m_Fame >= 60)/*     */    ss << "Talk of the town";
    else if (brothel->m_Fame >= 50)/*     */    ss << "Somewhat known";
    else if (brothel->m_Fame >= 30)/*     */    ss << "Mostly unknown";
    else/*                                */    ss << "Unknown";
    if (cfg.debug.log_show_numbers())/*   */    ss << " (" << (int)brothel->m_Fame << ")";
    return ss.str();
}

static std::string happiness_text(const IBuilding* brothel)
{
    stringstream ss;
    /* */if (brothel->m_Happiness >= 80)/* */    ss << "High";
    else if (brothel->m_Happiness < 40)/*  */    ss << "Low";
    else /*                                */    ss << "Medium";
    if (cfg.debug.log_show_numbers())            ss << " (" << brothel->m_Happiness << ")";
    return ss.str();
}

static std::string get_building_summary(const IBuilding& building)
{
    stringstream ss;
    long profit = building.m_Finance.total_profit();
    /*
    *    format the summary into one big string, and return it
    */
    ss << "Customer Happiness: " << happiness_text(&building);
    ss << "\nFame: " << fame_text(&building) << endl;
    ss << "\nRooms (available/current): " << building.free_rooms() << " / " << building.num_rooms();
    ss << "\nThis brothel's Profit: " << profit;
    ss << "\nYour Gold: " << g_Game->gold().ival();
    ss << "\nSecurity Level: " << building.security();
    ss << "\nDisposition: " << g_Game->player().disposition_text();
    ss << "\nSuspicion: " << g_Game->player().suss_text();
    ss << "\nFilthiness: " << building.filthiness();
    ss << "\nBeasts Housed Here: " << g_Game->storage().beasts();

    if(building.type() == BuildingType::FARM) {
        ss << "\nFood Stored: " << g_Game->storage().food()
           << "\nDrink Stored: " << g_Game->storage().drinks()
           << "\nGoods Stored: " << g_Game->storage().goods()
           << "\nAlchemy Items: " << g_Game->storage().alchemy();
    }
    return ss.str();
}



// ---------------------------------------------------------------------------------------------------------------------


cScreenBrothelManagement::cScreenBrothelManagement() : IBuildingScreen("brothel_management.xml",
                                                                       BuildingType::BROTHEL)
{
}

// ---------------------------------------------------------------------------------------------------------------------
cScreenArena::cScreenArena() : IBuildingScreen("arena_screen.xml", BuildingType::ARENA)
{
}


cScreenCentre::cScreenCentre() : IBuildingScreen("centre_screen.xml", BuildingType::CENTRE)
{
    //
}


cScreenClinic::cScreenClinic() : IBuildingScreen("clinic_screen.xml", BuildingType::CLINIC)
{
}

cScreenFarm::cScreenFarm(): IBuildingScreen("farm_screen.xml", BuildingType::FARM)
{

}

cScreenHouse::cScreenHouse() : IBuildingScreen("playerhouse_screen.xml", BuildingType::HOUSE)
{
}

void cScreenHouse::set_ids()
{
    IBuildingScreen::set_ids();
    house_id        = get_id("House");
    SetButtonNavigation(house_id,"House", false);
}

cMovieScreen::cMovieScreen() : IBuildingScreen("movie_screen.xml", BuildingType::STUDIO)
{
}

void cMovieScreen::set_ids()
{
    IBuildingScreen::set_ids();
    createmovie_id = get_id("CreateMovieButton", "*Unused*");
    SetButtonNavigation(createmovie_id, "Movie Maker", false);
}


void CBuildingScreenDispatch::init(bool back)
{
    auto bt = window_manager().GetActiveBuilding()->type();
    switch(bt) {
    case BuildingType ::BROTHEL:
        replace_window("Brothel Management");
        break;
    case BuildingType::STUDIO:
        replace_window("Movie Screen");
        break;
    case BuildingType::CLINIC:
        replace_window("Clinic Screen");
        break;
    case BuildingType::ARENA:
        replace_window("Arena Screen");
        break;
    case BuildingType::CENTRE:
        replace_window("Centre Screen");
        break;
    case BuildingType::HOUSE:
        replace_window("Player House");
        break;
    case BuildingType::FARM:
        replace_window("Farm Screen");
        break;
    }
}

CBuildingScreenDispatch::CBuildingScreenDispatch() : cInterfaceWindow("BuildingScreenDispatch") {

}
