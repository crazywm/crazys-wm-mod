/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders 
 * who meet on http://pinkpetal.co.cc
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
#include "cMovieStudio.h"
#include "cArena.h"
#include "cCentre.h"
#include "cScreenTown.h"
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
#include "libintl.h"

extern	bool			g_InitWin;
extern	int			g_CurrBrothel;
extern	int g_CurrClinic;
extern	int g_CurrStudio;
extern	int g_CurrArena;
extern	int g_CurrCentre;
extern	cGold			g_Gold;
extern	cBrothelManager		g_Brothels;
extern	cClinicManager		g_Clinic;
extern	cMovieStudioManager		g_Studios;
extern	cArenaManager		g_Arena;
extern	cCentreManager		g_Centre;
extern	cWindowManager		g_WinManager;
extern	cInterfaceEventManager	g_InterfaceEvents;
extern bool g_WalkAround;
extern bool g_Cheats;
extern	bool	eventrunning;
extern string g_ReturnText;
extern cGangManager g_Gangs;
extern bool g_AllTogle;
extern	int		g_CurrentScreen;

extern void GetString();
extern cInterfaceWindow g_GetString;

bool cScreenTown::ids_set = false;

void cScreenTown::set_ids()
{
	ids_set = true;
	back_id =		get_id("BackButton");
	walk_id =		get_id("WalkButton");
	curbrothel_id =	get_id("CurrentBrothel");
	slavemarket_id =get_id("SlaveMarket");
	shop_id =		get_id("Shop");
	mayor_id =		get_id("MayorsOffice");
	bank_id =		get_id("Bank");
	house_id =		get_id("House");
	clinic_id =		get_id("Clinic");
	studio_id =		get_id("Studio");
	arena_id =		get_id("Arena");
	centre_id =		get_id("Centre");
	prison_id =		get_id("Prison");
	brothel0_id =	get_id("Brothel0");
	brothel1_id =	get_id("Brothel1");
	brothel2_id =	get_id("Brothel2");
	brothel3_id =	get_id("Brothel3");
	brothel4_id =	get_id("Brothel4");
	brothel5_id =	get_id("Brothel5");
	brothel6_id =	get_id("Brothel6");
}

struct static_brothel_data {
	int	price;
	int	rooms;
	int business;
};

// stats of each brothel: price to buy, starting rooms, required # of businesses owned
static static_brothel_data brothel_data[] = {
	{ 0, 0, 0 },
//	{ 100, 25, 1 },	// testing code, lower requirements
//	{ 200, 30, 2 },
//	{ 300, 35, 3 },
//	{ 400, 40, 4 },
//	{ 500, 50, 5 },
	//{ 0, 25, 0 },
	{ 10000, 25, 30 },
	{ 30000, 30, 70 },
	{ 70000, 35, 100 },
	{ 150000, 40, 140 },
	{ 300000, 50, 170 },
	{ 1000000, 80, 220 }
};

static static_brothel_data clinic_data[] = {
	{ 10000, 20, 10 }
	//{000, 10, 0 }
};

void cScreenTown::init()
{
	if(BuyClinic != -1)
	{
		if(g_ChoiceManager.GetChoice(0) == 0)
		{
			GetClinic = true;
		}
		g_ChoiceManager.Free();
	}
	if(GetClinic)
	{
		static_brothel_data *bpt = clinic_data + BuyClinic;

		g_Gold.brothel_cost(bpt->price);
		g_Clinic.NewBrothel(bpt->rooms);
		g_Clinic.SetName(0, gettext("Clinic"));

		GetClinic = false;
		BuyClinic = -1;
	}
	if(GetName)
	{
		if(g_ReturnText != "")
		{
			if(g_Brothels.GetObjective())
			{
				if(g_Brothels.GetObjective()->m_Objective == OBJECTIVE_GETNEXTBROTHEL)
					g_Brothels.PassObjective();
			}

			static_brothel_data *bpt = brothel_data + BuyBrothel;

			g_Gold.brothel_cost(bpt->price);
			g_Brothels.NewBrothel(bpt->rooms);
			g_Brothels.SetName(g_Brothels.GetNumBrothels() - 1, g_ReturnText);
			
			g_InitWin = true;
		}
		GetName = false;
		BuyBrothel = -1;
	}
	if(BuyBrothel != -1)
	{
		if(g_ChoiceManager.GetChoice(0) == 0)
		{
			g_MessageQue.AddToQue("Enter a name for your new brothel.", 0);
			GetName = true;
			g_InitWin = true;
			g_WinManager.Push(GetString, &g_GetString);
		}
		g_ChoiceManager.Free();
		if(!GetName)
			BuyBrothel= -1;
		return;
	}
	g_CurrentScreen = SCREEN_TOWN;
	if(!g_InitWin) {
		return;
	}
	Focused();
	g_InitWin = false;

/*
 *	buttons enable/disable
 */
	DisableButton(walk_id, g_WalkAround);

	HideButton(brothel2_id, (g_Brothels.GetBrothel(1)==0));
	HideButton(brothel3_id, (g_Brothels.GetBrothel(2)==0));
	HideButton(brothel4_id, (g_Brothels.GetBrothel(3)==0));
	HideButton(brothel5_id, (g_Brothels.GetBrothel(4)==0));
	HideButton(brothel6_id, (g_Brothels.GetBrothel(5)==0));

	string brothel = "Current Brothel: ";
	brothel += g_Brothels.GetName(g_CurrBrothel);
	EditTextItem(brothel, curbrothel_id);
}

void cScreenTown::process()
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
	else if(g_InterfaceEvents.CheckButton(slavemarket_id))
	{
		g_InitWin = true;
		g_WinManager.push("Slave Market");
		return;
	}
	else if(g_InterfaceEvents.CheckButton(prison_id))
	{
		g_InitWin = true;
		g_WinManager.push("Prison");
		return;
	}
	else if(g_InterfaceEvents.CheckButton(house_id))
	{
		g_InitWin = true;
		g_WinManager.push("Player House");
		return;
	}
	else if(g_InterfaceEvents.CheckButton(clinic_id))
	{
		check_clinic(0);
		g_InitWin = true;
		return;
	}
	else if(g_InterfaceEvents.CheckButton(studio_id))
	{
		g_InitWin = true;
		g_WinManager.push("Movie Screen");
		return;
	}
	else if(g_InterfaceEvents.CheckButton(arena_id))
	{
		g_InitWin = true;
		g_WinManager.push("Arena Screen");
		return;
	}
	else if(g_InterfaceEvents.CheckButton(centre_id))
	{
		g_InitWin = true;
		g_WinManager.push("Centre Screen");
		return;
	}
	else if(g_InterfaceEvents.CheckButton(mayor_id))
	{
		g_InitWin = true;
		g_WinManager.push("Mayor");
		return;
	}
	else if(g_InterfaceEvents.CheckButton(bank_id))
	{
		g_InitWin = true;
		g_WinManager.push("Bank");
		return;
	}
	else if(g_InterfaceEvents.CheckButton(shop_id))
	{
		g_InitWin = true;
		g_AllTogle = false;
		g_WinManager.push("Item Management");
		return;
	}
	else if(g_InterfaceEvents.CheckButton(brothel0_id))
	{
		g_CurrBrothel = 0;
		g_InitWin = true;
		g_WinManager.Pop();
		return;
	}
	else if(g_InterfaceEvents.CheckButton(brothel1_id))
	{
		check_brothel(1);
		g_InitWin = true;
		return;
	}
	else if(g_InterfaceEvents.CheckButton(brothel2_id))
	{
		check_brothel(2);
		g_InitWin = true;
		return;
	}
	else if(g_InterfaceEvents.CheckButton(brothel3_id))
	{
		check_brothel(3);
		g_InitWin = true;
		return;
	}
	else if(g_InterfaceEvents.CheckButton(brothel4_id))
	{
		check_brothel(4);
		g_InitWin = true;
		return;
	}
	else if(g_InterfaceEvents.CheckButton(brothel5_id))
	{
		check_brothel(5);
		g_InitWin = true;
		return;
	}
	else if(g_InterfaceEvents.CheckButton(brothel6_id))
	{
		check_brothel(6);
		g_InitWin = true;
		return;
	}
	else if(g_InterfaceEvents.CheckButton(walk_id)) {
		do_walk();
		if(!g_Cheats) g_WalkAround = true;
		g_InitWin = true;
	}
}

string cScreenTown::walk_no_luck()
{
	stringstream ss;
	if(m_first_walk) {
		m_first_walk = false;
		ss << gettext("Your father once called this 'talent spotting' - ") 
			<< gettext("wandering though town, looking for new girls. ")
			<< gettext("'Willing, desperate or vulnerable' is how he put it. ")
			<< gettext("Not that any of those qualities are in evidence ")
			<< gettext("today, reminding you of another of your father's ")
			<< gettext("sayings - the one about patience. It's probably the ")
			<< gettext("only time he ever used the word 'virtue'.")
		;
		return	ss.str();
	}
	switch(g_Dice % 8) {
	case 0:
	case 1:
	case 2: return
		gettext("The city is quiet. The same old streets; the same old faces.");
	case 3: 
		ss << gettext("Married. Married. Bodyguard. Already works for you. Married. ")
		<< gettext("Hideous. Not a woman. Married. Escorted. Married... ")
		<< gettext("Might as well go home, there's nothing happening out here.")
		;
		return ss.str();
	case 4: 
		ss << gettext("It's not a bad life, if you can get paid for hanging around ")
		<< gettext("on street corners and eyeing up the pretty girls. Not a ")
		<< gettext("single decent prospect in the bunch of them, mind. ")
		<< gettext("Every silver lining has a cloud...");
		return ss.str();
	case 5: 
		ss << gettext("You've walked and walked and walked, and the prettiest ")
		<< gettext("thing you've seen all day turned out not to be female. ")
		<< gettext("It's time to go home...")
		;
		return ss.str();
		
	case 6: 
		ss << gettext("When the weather is bad, the hunting is good. Get them cold ")
		<< gettext("and wet enough and girls too proud to spread their legs ")
		<< gettext("suddenly can't get their knickers off fast enough, if the job ")
		<< gettext("only comes with room and board. The down side is that you ")
		<< gettext("spend far too much time walking in the rain when everyone ")
		<< gettext("sane is warm inside. Time to head home for a mug of cocoa ")
		<< gettext("and a nice hot trollop.")
		;
		return ss.str();
	case 7: 
		ss << gettext("There's a bit of skirt over there with a lovely ")
		<< gettext("figure, and had a face that was pretty, ninety ")
		<< gettext("years ago. Over yonder, a sweet young thing frolicking ")
		<< gettext("through the marketplace. She's being ever so daring, ")
		<< gettext("spending her daddy's gold, and hasn't yet realised ")
		<< gettext("that there's a dozen of her daddy's goons keeping ")
		<< gettext("a discreet eye on her.  It's like that everywhere ")
		<< gettext("today. Maybe tomorrow will be better.")
		;
		return ss.str();
	}
/*
 *	I don't think this should happen, hence the overly dramatic prose
 */
	ss << gettext("The sky is filled with thunder, and portals are opening all ")
		<< gettext("over Crossgate. You've seen five rains of frogs so far and ")
		<< gettext("three madmen speaking in tongues. In the marketplace a ")
		<< gettext("mechanical contraption materialised which walked like a man ")
		<< gettext("and declaimed alien poetry for half an hour before bursting ")
		<< gettext("into flames and collapsing. And the only thing all day that ")
		<< gettext("looked female and and attractive turned out to be some ")
		<< gettext("otherwordly species of carnivorous plant, and had to be ")
		<< gettext("destroyed by the town guard. The only good thing about this ")
		<< gettext("day is that it's over. It's time to go home.");
	return	ss.str();
	;
}

void cScreenTown::do_walk()
{
	if(g_WalkAround) {
		g_MessageQue.AddToQue(gettext("You can only do this once per week."), 2);
		return;
	}
/*
 *	let's get a girl for the player to meet
 */
	sGirl *girl = g_Girls.GetRandomGirl();
/*
 *	if there's no girl, no meeting
 */
	if(girl == 0) {
		g_MessageQue.AddToQue(walk_no_luck(), 0);
		return;
	}
/*
 *	most of the time, you're not going to find anyone
 *	unless you're cheating, of course.
 */
	cConfig cfg;
	int meet_chance = cfg.initial.girl_meet();
	if(!g_Dice.percent(meet_chance) && !g_Cheats) {
		g_MessageQue.AddToQue(walk_no_luck(), 1);
		return;
	}
/*
 *	I'd like to move this to the handler script
 *
 *	once scripts are stable
 */
	string message = gettext("You go out searching around town for any new girls, ");
	message += gettext("and you notice a potential new girl and walk up to her.");
	g_MessageQue.AddToQue(message, 2);
	int v[2] = {0,-1};
	cTrigger* trig = 0;

	DirPath dp;
	string filename;
	cScriptManager sm;
/*
 *	is there a girl specific talk script?
 */
	if(!(trig = girl->m_Triggers.CheckForScript(TRIGGER_MEET, false, v))) {
		// no, so trigger the default one
		dp = DirPath()
			<< "Resources" << "Scripts" << "MeetTownDefault.script"
		;
	}
	else {
		// trigger the girl-specific one
		dp = DirPath()
			<< "Resources" << "Characters" << girl->m_Name
			<< trig->m_Script
		;
	}
	eventrunning = true;
	sm.Load(dp, girl);
	return;
}

void cScreenTown::check_brothel(int BrothelNum)
{	// player clicked on one of the brothels
	if(g_Brothels.GetNumBrothels() == BrothelNum)
	{	// player doesn't own this brothel... can he buy it? 
		static_brothel_data *bck = brothel_data + BrothelNum;
		locale syslocale("");
		stringstream ss;
		ss.imbue(syslocale);

		if(!g_Gold.afford(bck->price) || g_Gangs.GetNumBusinessExtorted() < bck->business)
		{	// can't buy it
			ss << gettext("This building costs ") << bck->price << gettext(" gold and you need to control at least ") << bck->business << gettext(" businesses.");
			if(!g_Gold.afford(bck->price))
				ss << "\n" << gettext("You need ") << (bck->price - g_Gold.ival()) << gettext(" more gold to afford it.");
			if(g_Gangs.GetNumBusinessExtorted() < bck->business)
				ss << "\n" << gettext("You need to control ") << (bck->business - g_Gangs.GetNumBusinessExtorted()) << gettext(" more businesses.");
			g_MessageQue.AddToQue(ss.str(), 0);
		}
		else
		{	// can buy it
			ss << gettext("Do you wish to purchase this building for ") << bck->price << gettext(" gold? It has ") << bck->rooms << gettext(" rooms.");
			g_MessageQue.AddToQue(ss.str(), 2);
			g_ChoiceManager.CreateChoiceBox(224, 112, 352, 384, 0, 2, 32, 8);
			g_ChoiceManager.AddChoice(0, gettext("Buy It"), 0);
			g_ChoiceManager.AddChoice(0, gettext("Don't Buy It"), 1);
			g_ChoiceManager.SetActive(0);
			BuyBrothel = BrothelNum;
		}
	}
	else
	{	// player owns this brothel... go to it
		g_CurrBrothel = BrothelNum;
		g_WinManager.Pop();
	}
}

void cScreenTown::check_clinic(int ClinicNum)
{	// player clicked on one of the brothels
	if(g_Clinic.GetNumBrothels() == ClinicNum)
	{	// player doesn't own this brothel... can he buy it? 
		static_brothel_data *bck = clinic_data + ClinicNum;
		locale syslocale("");
		stringstream ss;
		ss.imbue(syslocale);

		if(!g_Gold.afford(bck->price) || g_Gangs.GetNumBusinessExtorted() < bck->business)
		{	// can't buy it
			ss << gettext("This building costs ") << bck->price << gettext(" gold and you need to control at least ") << bck->business << gettext(" businesses.");
			if(!g_Gold.afford(bck->price))
				ss << "\n" << gettext("You need ") << (bck->price - g_Gold.ival()) << gettext(" more gold to afford it.");
			if(g_Gangs.GetNumBusinessExtorted() < bck->business)
				ss << "\n" << gettext("You need to control ") << (bck->business - g_Gangs.GetNumBusinessExtorted()) << gettext(" more businesses.");
			g_MessageQue.AddToQue(ss.str(), 0);
		}
		else
		{	// can buy it
			ss << gettext("Do you wish to purchase this building for ") << bck->price << gettext(" gold? It has ") << bck->rooms << gettext(" rooms.");
			g_MessageQue.AddToQue(ss.str(), 2);
			g_ChoiceManager.CreateChoiceBox(224, 112, 352, 384, 0, 2, 32, 8);
			g_ChoiceManager.AddChoice(0, gettext("Buy It"), 0);
			g_ChoiceManager.AddChoice(0, gettext("Don't Buy It"), 1);
			g_ChoiceManager.SetActive(0);
			BuyClinic = ClinicNum;
		}
	}
	else
	{	// player owns this brothel... go to it
		g_CurrClinic = ClinicNum;
		g_WinManager.push("Clinic Screen");
	}
}
