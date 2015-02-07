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
#include "cMovieStudio.h"
#include "cCastingTry.h"
#include "cWindowManager.h"
#include "cGold.h"
#include "sFacilityList.h"
#include "cGetStringScreenManager.h"
#include "InterfaceProcesses.h"
#include "cScriptManager.h"
#include <iostream>
#include <locale>
#include <sstream>
#include "cGangs.h"

extern	bool			g_InitWin;
extern	int			g_CurrBrothel;
extern	cGold			g_Gold;
extern	cBrothelManager		g_Brothels;
extern	cMovieStudioManager  g_Studios;
extern	cWindowManager		g_WinManager;
extern	cInterfaceEventManager	g_InterfaceEvents;
extern bool g_TryCast;
extern bool g_Cheats;
extern	bool	eventrunning;
extern string g_ReturnText;
extern cGangManager g_Gangs;
extern bool g_AllTogle;
extern int g_Building;

bool cCastingTry::ids_set = false;

void cCastingTry::set_ids()
{
	ids_set = true;
	back_id = get_id("BackButton");
	walk_id = get_id("WalkButton");
	curbrothel_id = get_id("CurrentBrothel");
	clinic_id = get_id("Clinic");
}

void cCastingTry::init()
{

	/*
	*	buttons enable/disable
	*/
	DisableButton(walk_id, g_TryCast);

}

void cCastingTry::process()
{
	/*
	*	we need to make sure the ID variables are set
	*/
	if (!ids_set) {
		set_ids();
	}

	init();
	/*
	*	no events means we can go home
	*/
	if (g_InterfaceEvents.GetNumEvents() == 0) {
		return;
	}

	/*
	*	otherwise, compare event IDs
	*
	*	if it's the back button, pop the window off the stack
	*	and we're done
	*/
	if (g_InterfaceEvents.CheckButton(back_id)) {
		g_InitWin = true;
		g_WinManager.Pop();
		return;
	}
	else if (g_InterfaceEvents.CheckButton(walk_id)) {
		do_walk();
		if (!g_Cheats) g_TryCast = true;
		g_InitWin = true;
	}
}

string cCastingTry::walk_no_luck()
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
		"There is not much going on here in the studio.";
	case 3: return
		"Married. Married. Bodyguard. Already works for you. Married. "
		"Hideous. Not a woman. Married. Escorted. Married... "
		"Might as well go home, there's nothing happening here."
		;
	case 4: return
		"It's not a bad life, if you can get paid to try pretty girls "
		"out before they start filming. But somedays there isn't"
		"a single decent prospect in the bunch of them. "
		;
	case 5: return
		"All seemed perfect she was pretty really wanting to be "
		"an actress...  Then you told her what kinda movies you "
		"planned to make and she stormed off cursing at you."
		;
	case 6: return
		"When the weather is bad people just don't show up for this "
		"kinda thing. "
		;
	case 7: return
		"There's a bit of skirt over there with a lovely "
		"figure, and had a face that was pretty, ninety "
		"years ago. Over yonder, a sweet young thing but she's "
		"got daddy's gold.  Looks like nothing to gain here today. "
		;
	}
	/*
	*	I don't think this should happen, hence the overly dramatic prose
	*/
	return	"The sky is filled with thunder, and portals are opening all "
		"over Crossgate. You've seen five rains of frogs so far and "
		"three madmen speaking in tongues. In the marketplace a "
		"mechanical contraption materialised which walked like a man "
		"and declaimed alien poetry for half an hour before bursting "
		"into flames and collapsing. And the only thing all day that "
		"looked female and and attractive turned out to be some "
		"otherwordly species of carnivorous plant, and had to be "
		"destroyed by the town guard. The only good thing about this "
		"day is that it's over. It's time to go home."
		;
}

void cCastingTry::do_walk()
{
	if (g_TryCast)
	{
		g_MessageQue.AddToQue("You can only do this once per week.", 2);
		return;
	}

	sGirl *girl = g_Girls.GetRandomGirl();	//let's get a girl for the player to meet
	if (girl == 0)							// if there's no girl, no meeting
	{
		g_MessageQue.AddToQue(walk_no_luck(), COLOR_RED);
		return;
	}
	/*
	*	most of the time, you're not going to find anyone
	*	unless you're cheating, of course.
	*/
	cConfig cfg;
	int meet_chance = cfg.initial.girl_meet();
	if (!g_Dice.percent(meet_chance) && !g_Cheats) {
		g_MessageQue.AddToQue(walk_no_luck(), COLOR_RED);
		return;
	}
	/*
	*	I'd like to move this to the handler script
	*
	*	once scripts are stable
	*/
	string message = "";
	int pre = g_Dice % 2;
	if (pre == 1)	message = "You need a new girl for your next film. You set up a public casting call.";
	else			message = "You hold an open casting call to try to get a new actress for your movies.";
	g_MessageQue.AddToQue(message, COLOR_BLUE);
	int v[2] = { 0, -1 };
	cTrigger* trig = 0;
	g_Building = BUILDING_STUDIO;
	DirPath dp;
	string filename;
	cScriptManager sm;
	/*
	*	is there a girl specific talk script?
	*/
	if (!(trig = girl->m_Triggers.CheckForScript(TRIGGER_MEET, false, v))) {
		// no, so trigger the default one
		dp = DirPath() << "Resources" << "Scripts" << "MeetCastingTry.script";
	}
	else {
		// trigger the girl-specific one
		cConfig cfg;
		if (cfg.folders.configXMLch())
			dp = DirPath() << cfg.folders.characters() << girl->m_Name << trig->m_Script;
		else
			dp = DirPath() << "Resources" << "Characters" << girl->m_Name << trig->m_Script;
	}
	eventrunning = true;
	sm.Load(dp, girl);
	//g_Building = BUILDING_BROTHEL;
}


