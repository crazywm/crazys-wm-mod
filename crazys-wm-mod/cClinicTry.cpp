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
#include "cClinic.h"
#include "cClinicTry.h"
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
#include "FileList.h"

extern	bool					g_InitWin;
extern	int						g_CurrBrothel;
extern	cGold					g_Gold;
extern	cBrothelManager			g_Brothels;
extern	cClinicManager			g_Clinic;
extern	cWindowManager			g_WinManager;
extern	cInterfaceEventManager	g_InterfaceEvents;
extern	bool					g_TryEr;
extern	bool					g_Cheats;
extern	bool					eventrunning;
extern	cGangManager			g_Gangs;
extern	bool					g_AllTogle;
extern	int						g_CurrentScreen;
extern	int						g_Building;
extern	string					ReadTextFile(DirPath path, string file);

bool cClinicTry::ids_set = false;

void cClinicTry::set_ids()
{
	ids_set = true;
	back_id =		get_id("BackButton");
	walk_id =		get_id("WalkButton");
	curbrothel_id =	get_id("CurrentBrothel");
	clinic_id =		get_id("Clinic");
}

void cClinicTry::init()
{

/*
 *	buttons enable/disable
 */
	DisableButton(walk_id, g_TryEr);

}

void cClinicTry::process()
{
/*
 *	we need to make sure the ID variables are set
 */
	if(!ids_set) {
		set_ids();
	}

	init();
/* 
 *	no events means we can go home
 */
	if(g_InterfaceEvents.GetNumEvents() == 0) {
		return;
	}

/*
 *	otherwise, compare event IDs 
 *
 *	if it's the back button, pop the window off the stack
 *	and we're done
 */
	if(g_InterfaceEvents.CheckButton(back_id)) {
		g_InitWin = true;
		g_WinManager.Pop();
		return;
	}
	else if(g_InterfaceEvents.CheckButton(walk_id)) {
		do_walk();
		if(!g_Cheats) g_TryEr = true;
		g_InitWin = true;
	}
}

string cClinicTry::walk_no_luck()
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

void cClinicTry::do_walk()
{
	if (g_TryEr)
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

	g_Building = BUILDING_CLINIC;
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
	if (message.size() > 0) g_MessageQue.AddToQue(message, COLOR_BLUE);

	eventrunning = true;
	sm.Load(dp, girl);
	return;
}