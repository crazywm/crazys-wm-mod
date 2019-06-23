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
#include "cWindowManager.h"
#include "cScriptManager.h"

#include "InterfaceProcesses.h"
#include "cMovieStudio.h"
#include "cScreenMovie.h"
#include "FileList.h"

extern cInterfaceEventManager g_InterfaceEvents;
extern cWindowManager g_WinManager;
extern cBrothelManager g_Brothels;
extern cMovieStudioManager g_Studios;


extern int g_Building;
extern int g_CurrBrothel;
extern int g_CurrentScreen;
extern string ReadTextFile(DirPath path, string file);
extern bool g_TryCast;
extern bool eventrunning;
extern bool g_InitWin;
extern bool g_Cheats;



extern bool g_CTRLDown;

bool cMovieScreen::ids_set = false;

void cMovieScreen::set_ids()
{
	ids_set			/**/ = true;
	g_LogFile.write("set_ids in cMovieScreen");

	buildinglabel_id/**/ = get_id("BuildingLabel", "Header");
	background_id	/**/ = get_id("Background", "Movies");
	walk_id			/**/ = get_id("WalkButton");

	weeks_id		/**/ = get_id("Next Week", "Weeks");
	moviedetails_id	/**/ = get_id("BuildingDetails", "Details", "MovieDetails");
	girls_id		/**/ = get_id("Girl Management", "Girls");
	staff_id		/**/ = get_id("Staff Management", "Staff");
	setup_id		/**/ = get_id("Setup", "SetUp");
	dungeon_id		/**/ = get_id("Dungeon");
	turns_id		/**/ = get_id("Turn Summary", "Turn");

	girlimage_id	/**/ = get_id("GirlImage");
	back_id			/**/ = get_id("BackButton", "Back");

	nextbrothel_id	/**/ = get_id("PrevButton", "Prev", "*Unused*");
	prevbrothel_id	/**/ = get_id("NextButton", "Next", "*Unused*");
	movie_id		/**/ = get_id("Movie", "*Unused*");//
	createmovie_id	/**/ = get_id("CreateMovieButton", "*Unused*");//
}
cMovieScreen::cMovieScreen()
{
	DirPath dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "movie_screen.xml";
	m_filename = dp.c_str();
	GetName = false;
	m_first_walk = true;
}
cMovieScreen::~cMovieScreen() { g_LogFile.write("Studio Shutdown"); }

void cMovieScreen::init()
{
	g_CurrentScreen = SCREEN_STUDIO;
	g_Building = BUILDING_STUDIO;
	if (g_InitWin)
	{
		Focused();

		EditTextItem(g_Studios.GetBrothelString(0), moviedetails_id);
		g_InitWin = false;
	}


	DisableButton(walk_id, g_TryCast);
}

void cMovieScreen::process()
{
	if (!ids_set) set_ids();	// we need to make sure the ID variables are set
	if (girlimage_id != -1 && !eventrunning)	HideImage(girlimage_id, true);
	init();
	if (g_InterfaceEvents.GetNumEvents() != 0)	check_events();
}

void cMovieScreen::check_events()
{
	g_InitWin = true;
	if (g_InterfaceEvents.CheckButton(back_id))				{ g_WinManager.Pop(); }
	else if (g_InterfaceEvents.CheckButton(girls_id))		{ g_WinManager.push("Studio"); }
	else if (g_InterfaceEvents.CheckButton(staff_id))		{ g_WinManager.push("Gangs"); }
	else if (g_InterfaceEvents.CheckButton(turns_id))		{ g_WinManager.push("Turn Summary"); }
	else if (g_InterfaceEvents.CheckButton(setup_id))		{ g_WinManager.push("Building Setup"); }
	else if (g_InterfaceEvents.CheckButton(dungeon_id))		{ g_WinManager.push("Dungeon"); }
	else if (g_InterfaceEvents.CheckButton(createmovie_id))	{ g_WinManager.push("Movie Maker"); }
	else if (g_InterfaceEvents.CheckButton(walk_id))
	{
		do_walk();
		if (!g_Cheats) g_TryCast = true;
	}
	else if (g_InterfaceEvents.CheckButton(weeks_id))
	{
		if (!g_CTRLDown) { g_CTRLDown = false; AutoSaveGame(); }
		NextWeek();
		g_WinManager.push("Turn Summary");
	}
}

string cMovieScreen::walk_no_luck()
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
	// I don't think this should happen, hence the overly dramatic prose
	return "The sky is filled with thunder, and portals are opening all over Crossgate. You've seen five rains of frogs so far and three madmen speaking in tongues. In the marketplace a mechanical contraption materialised which walked like a man and declaimed alien poetry for half an hour before bursting into flames and collapsing. And the only thing all day that looked female and and attractive turned out to be some otherwordly species of carnivorous plant, and had to be destroyed by the town guard. The only good thing about this day is that it's over. It's time to go home.";
}

void cMovieScreen::do_walk()
{
	if (g_TryCast)
	{
		g_MessageQue.AddToQue("You can only do this once per week.", COLOR_RED);
		return;
	}
	sGirl *girl = g_Girls.GetRandomGirl();						// let's get a girl for the player to meet
	if (girl == 0)												// if there's no girl, no meeting
	{
		g_MessageQue.AddToQue(walk_no_luck(), COLOR_RED);
		return;
	}
	// most of the time, you're not going to find anyone unless you're cheating, of course.
	if (!g_Dice.percent(cfg.initial.girl_meet()) && !g_Cheats)
	{
		g_MessageQue.AddToQue(walk_no_luck(), COLOR_BLUE);
		return;
	}
	if (girlimage_id != -1)
	{
		if (girl)
		{
			PrepareImage(girlimage_id, girl, IMGTYPE_PROFILE, true, -1);
			HideImage(girlimage_id, false);
		}
		else HideImage(girlimage_id, true);
	}

	g_Building = BUILDING_STUDIO;
	int v[2] = { 0, -1 };
	cTrigger* trig = 0;
	DirPath dp;
	DirPath intro;
	string introfile = "";
	string message = "";
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
	if (message.size() > 0) g_MessageQue.AddToQue(message, COLOR_BLUE);

	eventrunning = true;
	sm.Load(dp, girl);
}