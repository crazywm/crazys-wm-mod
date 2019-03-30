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
#include <main.h>
#include <InterfaceGlobals.h>
#include "cScriptManager.h"
#include "cScreenGetInput.h"
#include "InterfaceProcesses.h"
#include "buildings/cBrothel.h"
#include "cScreenBrothelManagement.h"
#include "FileList.h"
#include "Game.hpp"
#include "sStorage.hpp"

extern string ReadTextFile(DirPath path, string file);

extern bool eventrunning;
extern bool g_Cheats;
extern bool g_CTRLDown;


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
    walk_id			 = get_id("WalkButton");

    weeks_id		 = get_id("Next Week", "Weeks");
    girls_id		 = get_id("Girl Management", "Girls");
    staff_id		 = get_id("Staff Management", "Staff");
    setup_id		 = get_id("Setup", "SetUp");
    dungeon_id		 = get_id("Dungeon");
    turns_id		 = get_id("Turn Summary", "Turn");

    girlimage_id	 = get_id("GirlImage");

    prevbrothel_id	 = get_id("PrevButton", "Prev", "*Unused*");
    nextbrothel_id	 = get_id("NextButton", "Next", "*Unused*");

    details_id       = get_id("BuildingDetails", "Details");
    town_id			 = get_id("Visit Town");
    save_id			 = get_id("Save");
    quit_id		 	 = get_id("Quit");

    // set button callbacks
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
        NextWeek();
        push_window("Turn Summary");
    });
    SetButtonCallback(save_id, [this]() {
        SaveGame(g_CTRLDown);
        g_Game.push_message("Game Saved", COLOR_GREEN);
    });
    SetButtonCallback(quit_id, [this]() {
        input_confirm([this]() {
            g_WinManager.PopToWindow("Main Menu");
            ResetInterface();
        });
    });
}

void IBuildingScreen::process()
{
    if (girlimage_id != -1 && !eventrunning)	HideImage(girlimage_id, true);
}

IBuildingScreen::IBuildingScreen(const char * base_file, BuildingType building, bool * has_walked) :
        cInterfaceWindowXML(base_file),
        m_Type(building), m_HasDoneWalkThisWeek(has_walked)
{
}

void IBuildingScreen::init(bool back)
{
    Focused();

    if(active_building().type() != m_Type)
        throw std::logic_error("Invalid building type for this screen!");

    EditTextItem(get_building_summary(active_building()), details_id);
    stringstream ss;

    ss << "Day: " << g_Game.date().day << " Month: " << g_Game.date().month << " Year: " << g_Game.date().year
       << " -- " << active_building().type_str() << ": " << active_building().name();
    EditTextItem(ss.str(), buildinglabel_id);

    if(m_HasDoneWalkThisWeek)
        DisableButton(walk_id, *m_HasDoneWalkThisWeek);

    SetImage(background_id, g_Graphics.LoadBrothelImage(active_building().background_image()));
}

IBuildingScreen::~IBuildingScreen()
{
    g_LogFile.write("Shutdown Building Screen");
}

void IBuildingScreen::try_walk()
{
    if(!m_HasDoneWalkThisWeek)
        return;

    if (*m_HasDoneWalkThisWeek)
    {
        g_Game.push_message("You can only do this once per week.", COLOR_RED);
        return;
    }

    if (!g_Cheats) *m_HasDoneWalkThisWeek = true;

    sGirl* girl = meet_girl();
    if (girl == nullptr)												// if there's no girl, no meeting
    {
        g_Game.push_message(walk_no_luck(), COLOR_RED);
        return;
    }

    // most of the time, you're not going to find anyone unless you're cheating, of course.
    if (!g_Dice.percent(cfg.initial.girl_meet()) && !g_Cheats)
    {
        g_Game.push_message(walk_no_luck(), COLOR_BLUE);
        return;
    }

    if (girlimage_id != -1)
    {
        PrepareImage(girlimage_id, girl, IMGTYPE_PROFILE, true, -1);
        HideImage(girlimage_id, false);
    }

    do_walk(girl);
}

sGirl* IBuildingScreen::meet_girl() const
{
    return g_Game.GetRandomGirl();
}

std::string IBuildingScreen::walk_no_luck()
{
    return "NOT IMPLEMENTED! AT " __FILE__;
}

static std::string fame_text(const IBuilding* brothel)
{
    stringstream ss;
    /* */if (brothel->m_Fame >= 90)/*     */	ss << "World Renowned";
    else if (brothel->m_Fame >= 80)/*     */	ss << "Famous";
    else if (brothel->m_Fame >= 70)/*     */	ss << "Well Known";
    else if (brothel->m_Fame >= 60)/*     */	ss << "Talk of the town";
    else if (brothel->m_Fame >= 50)/*     */	ss << "Somewhat known";
    else if (brothel->m_Fame >= 30)/*     */	ss << "Mostly unknown";
    else/*                                */	ss << "Unknown";
    if (cfg.debug.log_show_numbers())/*   */	ss << " (" << (int)brothel->m_Fame << ")";
    return ss.str();
}

static std::string happiness_text(const IBuilding* brothel)
{
    stringstream ss;
    /* */if (brothel->m_Happiness >= 80)/* */	ss << "High";
    else if (brothel->m_Happiness < 40)/*  */	ss << "Low";
    else /*                                */	ss << "Medium";
    if (cfg.debug.log_show_numbers())			ss << " (" << brothel->m_Happiness << ")";
    return ss.str();
}

static std::string get_building_summary(const IBuilding& building)
{
    stringstream ss;
    long profit = building.m_Finance.total_profit();
    /*
    *	format the summary into one big string, and return it
    */
    ss << "Customer Happiness: " << happiness_text(&building);
    ss << "\nFame: " << fame_text(&building) << endl;
    ss << "\nRooms (available/current): " << (building.free_rooms() - building.num_girls()) << " / " << building.num_rooms();
    ss << "\nThis brothel's Profit: " << profit;
    ss << "\nYour Gold: " << g_Game.gold().ival();
    ss << "\nSecurity Level: " << building.security();
    ss << "\nDisposition: " << g_Game.player().disposition_text();
    ss << "\nSuspicion: " << g_Game.player().suss_text();
    ss << "\nFilthiness: " << building.filthiness();
    ss << "\nBeasts Housed Here: " << g_Game.storage().beasts();

    if(building.type() == BuildingType::FARM) {
        ss << "\nFood Stored: " << g_Game.storage().food()
           << "\nDrink Stored: " << g_Game.storage().drinks()
           << "\nGoods Stored: " << g_Game.storage().goods()
           << "\nAlchemy Items: " << g_Game.storage().alchemy();
    }
    return ss.str();
}



// ---------------------------------------------------------------------------------------------------------------------


cScreenBrothelManagement::cScreenBrothelManagement() : IBuildingScreen("brothel_management.xml",
                                                                       BuildingType::BROTHEL, nullptr)
{
}

// ---------------------------------------------------------------------------------------------------------------------
cScreenArena::cScreenArena() : IBuildingScreen("arena_screen.xml", BuildingType::ARENA, &g_TryOuts)
{
}

void cScreenArena::do_walk(sGirl* girl)
{
    int v[2] = { 3, -1 };
    cTrigger* trig = nullptr;
    DirPath dp;
    DirPath intro;
    string introfile;
    string message;
    cScriptManager sm;

    // is there a girl specific talk script?
    if (!(trig = girl->m_Triggers.CheckForScript(TRIGGER_MEET, false, v)))
    {
        // no, so trigger the default one
        introfile = "MeetArenaTry.script.intro";
        intro = DirPath() << "Resources" << "Scripts";
        dp = DirPath() << "Resources" << "Scripts" << "MeetArenaTry.script";
    }
    else
    {
        // trigger the girl-specific one
        introfile = trig->m_Script + ".intro";
        intro = DirPath(cfg.folders.characters().c_str()) << girl->m_Name;
        dp = DirPath(cfg.folders.characters().c_str()) << girl->m_Name << trig->m_Script;
    }

    FileList abstest(intro, introfile.c_str());
    if (abstest.size() == 0)
    {
        message = "You hold open try outs to all girls willing to step into the arena and fight for their life.";
    }
    else
    {
        message = ReadTextFile(intro, introfile);
    }
    if (!message.empty()) g_Game.push_message(message, COLOR_BLUE);

    eventrunning = true;
    sm.Load(dp, girl);
}

sGirl* cScreenArena::meet_girl() const
{
    // let's get a girl for the player to meet was to get arena.. dont think this should happen this is tryouts arena girl should be ready to fight. CRAZY
    return g_Game.GetRandomGirl(false, false, true);
}

std::string cScreenArena::walk_no_luck()
{
    if (m_first_walk) {
        m_first_walk = false;
        return	"Your father once called this 'talent spotting' - "
                  "and looking these girls over you see no talent for "
                  "anything."
                ;
    }
    switch (g_Dice % 8) {
    case 0:
    case 1:
    case 2: return
                "The city is quiet and no one shows up.";
    case 3: return
                "Married. Married. Bodyguard. Already works for you. Married. "
                "Hideous. Not a woman. Married. Escorted. Married... "
                "Might as well go home, there's nothing happening out here."
                ;
    case 4: return
                "It's not a bad life, if you can get paid for hanging around "
                "on street corners and eyeing up the pretty girls. Not a "
                "single decent prospect in the bunch of them, mind. "
                "Every silver lining has a cloud..."
                ;
    case 5: return
                "You've walked and walked and walked, and the prettiest "
                "thing you've seen all day turned out not to be female. "
                "It's time to go home..."
                ;
    case 6: return
                "When the weather is bad, the hunting is good. Get them cold "
                "and wet enough and girls too proud to spread their legs "
                "suddenly can't get their knickers off fast enough, if the job "
                "only comes with room and board. The down side is that you "
                "spend far too much time walking in the rain when everyone "
                "sane is warm inside. Time to head home for a mug of cocoa "
                "and a nice hot trollop."
                ;
    case 7: return
                "There's a bit of skirt over there with a lovely "
                "figure, and had a face that was pretty, ninety "
                "years ago. Over yonder, a sweet young thing frolicking "
                "through the marketplace. She's being ever so daring, "
                "spending her daddy's gold, and hasn't yet realised "
                "that there's a dozen of her daddy's goons keeping "
                "a discreet eye on her.  It's like that everywhere "
                "today. Maybe tomorrow will be better."
                ;
    }
}

cScreenCentre::cScreenCentre() : IBuildingScreen("centre_screen.xml", BuildingType::CENTRE, &g_TryCentre)
{
    //
}


cScreenClinic::cScreenClinic() : IBuildingScreen("clinic_screen.xml", BuildingType::CLINIC,
                                                 &g_TryEr)
{
}

void cScreenClinic::do_walk(sGirl* girl)
{
    int v[2] = { 0, -1 };
    cTrigger* trig = nullptr;
    DirPath dp;
    DirPath intro;
    string introfile;
    string message;
    cScriptManager sm;

    // is there a girl specific talk script?
    if (!(trig = girl->m_Triggers.CheckForScript(TRIGGER_MEET, false, v)))
    {
        // no, so trigger the default one
        introfile = "MeetClinicTry.script.intro";
        intro = DirPath() << "Resources" << "Scripts";
        dp = DirPath() << "Resources" << "Scripts" << "MeetClinicTry.script";
    }
    else
    {
        // trigger the girl-specific one
        introfile = trig->m_Script + ".intro";
        intro = DirPath(cfg.folders.characters().c_str()) << girl->m_Name;
        dp = DirPath(cfg.folders.characters().c_str()) << girl->m_Name << trig->m_Script;
    }

    FileList abstest(intro, introfile.c_str());
    if (abstest.size() == 0)
    {
        message = "You go to your clinic in the hopes that your men have brought in a potential new girl as per your orders.";
    }
    else
    {
        message = ReadTextFile(intro, introfile);
    }
    if (!message.empty()) g_Game.push_message(message, COLOR_BLUE);

    eventrunning = true;
    sm.Load(dp, girl);
}

std::string cScreenClinic::walk_no_luck()
{
    if(m_first_walk) {
        m_first_walk = false;
        return	"Your father once called this 'talent spotting' - "
                  "and looking these girls over you see no talent for "
                  "anything."
                ;
    }
    switch(g_Dice % 8) {
    case 0:
    case 1:
    case 2: return
                "The Clinic is quite not much going on here.";
    case 3: return
                "Married. Married. Bodyguard. Already works for you. Married. "
                "Hideous. Not a woman. Married. Escorted. Married... "
                "Might as well go home, there's nothing happening here."
                ;
    case 4: return
                "It's not a bad life, if you can get paid for hanging in the "
                "clinic eyeing up the pretty girls that might be brought in."
                "Not a single decent prospect in the bunch of them. "
                ;
    case 5: return
                "You've walked and walked and walked, and the prettiest "
                "thing you've seen all day turned out not to be female. "
                "It's time to go home..."
                ;
    case 6: return
                "When the weather is bad, the hunting is good. The cold brings "
                "in the sick. But nothing of note today. "
                ;
    case 7: return
                "There's a bit of skirt over there with a lovely "
                "figure, and had a face that was pretty, ninety "
                "years ago. Over yonder, a sweet young thing but she's "
                "got daddy's gold.  Looks like nothing to gain here today. "
                ;
    }
}


cScreenFarm::cScreenFarm(): IBuildingScreen("farm_screen.xml", BuildingType::FARM, nullptr)
{

}

cScreenHouse::cScreenHouse() : IBuildingScreen("playerhouse_screen.xml",
                                               BuildingType::HOUSE, nullptr)
{
}

void cScreenHouse::set_ids()
{
    IBuildingScreen::set_ids();
    house_id		= get_id("House");
    SetButtonNavigation(house_id,"House", false);
}

cMovieScreen::cMovieScreen() : IBuildingScreen("movie_screen.xml", BuildingType::STUDIO, &g_TryCast)
{
}

void cMovieScreen::do_walk(sGirl* girl)
{
    int v[2] = { 0, -1 };
    cTrigger* trig = nullptr;
    DirPath dp;
    DirPath intro;
    string introfile;
    string message;
    cScriptManager sm;

    // is there a girl specific talk script?
    if (!(trig = girl->m_Triggers.CheckForScript(TRIGGER_MEET, false, v)))
    {
        // no, so trigger the default one
        introfile = "MeetCastingTry.script.intro";
        intro = DirPath() << "Resources" << "Scripts";
        dp = DirPath() << "Resources" << "Scripts" << "MeetCastingTry.script";
    }
    else
    {
        // trigger the girl-specific one
        introfile = trig->m_Script + ".intro";
        intro = DirPath(cfg.folders.characters().c_str()) << girl->m_Name;
        dp = DirPath(cfg.folders.characters().c_str()) << girl->m_Name << trig->m_Script;
    }

    FileList abstest(intro, introfile.c_str());
    if (abstest.size() == 0)
    {
        int pre = g_Dice % 2;
        if (pre == 1)	message = "You need a new girl for your next film. You set up a public casting call.";
        else			message = "You hold an open casting call to try to get a new actress for your movies.";
    }
    else
    {
        message = ReadTextFile(intro, introfile);
    }
    if (!message.empty()) g_Game.push_message(message, COLOR_BLUE);

    eventrunning = true;
    sm.Load(dp, girl);
}

void cMovieScreen::set_ids()
{
    IBuildingScreen::set_ids();
    createmovie_id = get_id("CreateMovieButton", "*Unused*");
    SetButtonNavigation(createmovie_id, "Movie Maker", false);
}

std::string cMovieScreen::walk_no_luck()
{
    if (m_first_walk)
    {
        m_first_walk = false;
        return	"Your father once called this 'talent spotting' - and looking these girls over you see no talent for anything.";
    }
    switch (g_Dice % 8)
    {
    case 0:	return "Married. Married. Bodyguard. Already works for you. Married. Hideous. Not a woman. Married. Escorted. Married...\nMight as well go home, there's nothing happening here.";
    case 1:	return "It's not a bad life, if you can get paid to try pretty girls out before they start filming. But somedays there isn't a single decent prospect in the bunch of them.";
    case 2:	return "All seemed perfect she was pretty really wanting to be an actress...  Then you told her what kinda movies you planned to make and she stormed off cursing at you.";
    case 3:	return "When the weather is bad people just don't show up for this kinda thing.";
    case 4:	return "There's a bit of skirt over there with a lovely figure, and had a face that was pretty, ninety years ago. Over yonder, a sweet young thing but she's got daddy's gold.  Looks like nothing to gain here today. ";
    default:return "There is not much going on here in the studio.";
    }
}

void CBuildingScreenDispatch::init(bool back)
{
    auto bt = active_building().type();
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
