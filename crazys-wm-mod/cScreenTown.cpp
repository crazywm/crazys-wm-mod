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
#include "cMovieStudio.h"
#include "cArena.h"
#include "cCentre.h"
#include "cFarm.h"
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
#include "FileList.h"

extern bool						g_InitWin;
extern int						g_CurrBrothel;
extern int						g_CurrClinic;
extern int						g_CurrStudio;
extern int						g_CurrArena;
extern int						g_CurrCentre;
extern int						g_CurrFarm;
extern int						g_CurrHouse;
extern cGold					g_Gold;
extern cBrothelManager			g_Brothels;
extern cClinicManager			g_Clinic;
extern cMovieStudioManager		g_Studios;
extern cArenaManager			g_Arena;
extern cCentreManager			g_Centre;
extern cFarmManager				g_Farm;
extern cWindowManager			g_WinManager;
extern cInterfaceEventManager	g_InterfaceEvents;
extern sGirl *selected_girl;
extern bool						g_WalkAround;
extern bool						g_Cheats;
extern bool						eventrunning;
extern string					g_ReturnText;
extern cGangManager				g_Gangs;
extern bool						g_AllTogle;
extern int						g_CurrentScreen;
extern int						g_Building;
extern string					ReadTextFile(DirPath path, string file);

static int lastNum = -1;
static int ImageNum = -1;
bool cScreenTown::ids_set = false;

cScreenTown::cScreenTown()
{
	DirPath dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "town_screen.xml";
	m_filename = dp.c_str();
	BuyBrothel = -1;
	BuyClinic = -1;
	BuyArena = -1;
	BuyStudio = -1;
	BuyCentre = -1;
	BuyFarm = -1;
	GetName = false;
	m_first_walk = true;
}
cScreenTown::~cScreenTown() {}


void cScreenTown::set_ids()
{
	ids_set			/**/ = true;
	g_LogFile.write("set_ids in cScreenTown");

	back_id			/**/ = get_id("BackButton", "Back");
	gold_id			/**/ = get_id("Gold", "*Unused*");//
	walk_id			/**/ = get_id("WalkButton");
	girlimage_id	/**/ = get_id("GirlImage");
	curbrothel_id	/**/ = get_id("CurrentBrothel");
	slavemarket_id	/**/ = get_id("SlaveMarket");
	shop_id			/**/ = get_id("Shop");
	mayor_id		/**/ = get_id("MayorsOffice");
	bank_id			/**/ = get_id("Bank");
	house_id		/**/ = get_id("House");
	clinic_id		/**/ = get_id("Clinic");
	studio_id		/**/ = get_id("Studio");
	arena_id		/**/ = get_id("Arena");
	centre_id		/**/ = get_id("Centre");
	farm_id			/**/ = get_id("Farm");
	prison_id		/**/ = get_id("Prison");
	brothel0_id		/**/ = get_id("Brothel0");
	brothel1_id		/**/ = get_id("Brothel1");
	brothel2_id		/**/ = get_id("Brothel2");
	brothel3_id		/**/ = get_id("Brothel3");
	brothel4_id		/**/ = get_id("Brothel4");
	brothel5_id		/**/ = get_id("Brothel5");
	brothel6_id		/**/ = get_id("Brothel6");
	setup_id		/**/ = get_id("SetUp", "*Unused*");//
}

// stats of each brothel: price to buy, starting rooms, maximum rooms, required # of businesses owned
static static_brothel_data brothel_data[] = {
	{ 0, 20, 200, 0 },
	{ 10000, 25, 250, 30 },
	{ 30000, 30, 300, 70 },
	{ 70000, 35, 350, 100 },
	{ 150000, 40, 400, 140 },
	{ 300000, 50, 500, 170 },
	{ 1000000, 80, 600, 220 }
};
static static_brothel_data centre_data[] = {
	{ 5000, 20, 200, 5 }
	//{000, 10, 0 }
};
static static_brothel_data farm_data[] = {
	{ 10000, 20, 200, 10 }
	//{000, 10, 0 }
};
static static_brothel_data arena_data[] = {
	{ 15000, 20, 200, 15 }
	//{000, 10, 0 }
};
static static_brothel_data studio_data[] = {
	{ 20000, 20, 200, 20 }
	//{000, 10, 0 }
};
static static_brothel_data clinic_data[] = {
	{ 25000, 20, 200, 25 }
	//{000, 10, 0, 0 }
};

void cScreenTown::init()
{
	g_CurrentScreen = SCREEN_TOWN;

	if (gold_id >= 0)
	{
		stringstream ss; ss << "Gold: " << g_Gold.ival();
		EditTextItem(ss.str(), gold_id);
	}

	if (BuyClinic != -1)
	{
		(g_ChoiceManager.GetChoice(0) == 0 ? GetClinic = true : BuyClinic = -1);
		g_ChoiceManager.Free();
	}
	if (GetClinic)
	{
		static_brothel_data *bpt = clinic_data + BuyClinic;
		g_Gold.brothel_cost(bpt->price);
		g_Clinic.NewBrothel(bpt->rooms, bpt->maxrooms);
		g_Clinic.SetName(0, "Clinic");
		GetClinic = false;
		BuyClinic = -1;
	}
	if (BuyCentre != -1)
	{
		(g_ChoiceManager.GetChoice(0) == 0 ? GetCentre = true : BuyCentre = -1);
		g_ChoiceManager.Free();
	}
	if (GetCentre)
	{
		static_brothel_data *bpt = centre_data + BuyCentre;
		g_Gold.brothel_cost(bpt->price);
		g_Centre.NewBrothel(bpt->rooms, bpt->maxrooms);
		g_Centre.SetName(0, "Centre");
		GetCentre = false;
		BuyCentre = -1;
	}
	if (BuyArena != -1)
	{
		(g_ChoiceManager.GetChoice(0) == 0 ? GetArena = true : BuyArena = -1);
		g_ChoiceManager.Free();
	}
	if (GetArena)
	{
		static_brothel_data *bpt = arena_data + BuyArena;

		g_Gold.brothel_cost(bpt->price);
		g_Arena.NewBrothel(bpt->rooms, bpt->maxrooms);
		g_Arena.SetName(0, ("Arena"));

		GetArena = false;
		BuyArena = -1;
	}
	if (BuyStudio != -1)
	{
		(g_ChoiceManager.GetChoice(0) == 0 ? GetStudio = true : BuyStudio = -1);
		g_ChoiceManager.Free();
	}
	if (GetStudio)
	{
		static_brothel_data *bpt = studio_data + BuyStudio;
		g_Gold.brothel_cost(bpt->price);
		g_Studios.NewBrothel(bpt->rooms, bpt->maxrooms);
		g_Studios.SetName(0, ("Studio"));
		GetStudio = false;
		BuyStudio = -1;
	}
	if (BuyFarm != -1)
	{
		(g_ChoiceManager.GetChoice(0) == 0 ? GetFarm = true : BuyFarm = -1);
		g_ChoiceManager.Free();
	}
	if (GetFarm)
	{
		static_brothel_data *bpt = farm_data + BuyFarm;
		g_Gold.brothel_cost(bpt->price);
		g_Farm.NewBrothel(bpt->rooms, bpt->maxrooms);
		g_Farm.SetName(0, ("Farm"));
		GetFarm = false;
		BuyFarm = -1;
	}
	if (GetName)
	{
		if (g_ReturnText != "")
		{
			if (g_Brothels.GetObjective() && g_Brothels.GetObjective()->m_Objective == OBJECTIVE_GETNEXTBROTHEL)
				g_Brothels.PassObjective();
			static_brothel_data *bpt = brothel_data + BuyBrothel;
			g_Gold.brothel_cost(bpt->price);
			g_Brothels.NewBrothel(bpt->rooms, bpt->maxrooms);
			g_Brothels.SetName(g_Brothels.GetNumBrothels() - 1, g_ReturnText);
			g_InitWin = true;
		}
		GetName = false;
		BuyBrothel = -1;
	}
	if (BuyBrothel != -1)
	{
		if (g_ChoiceManager.GetChoice(0) == 0)
		{
			g_MessageQue.AddToQue("Enter a name for your new brothel.", 0);
			GetName = true;
			g_InitWin = true;
			g_WinManager.Push(GetString, &g_GetString);
		}
		else BuyBrothel = -1;
		g_ChoiceManager.Free();
		if (!GetName) BuyBrothel = -1;
		return;
	}
	g_CurrentScreen = SCREEN_TOWN;
	if (!g_InitWin) return;

	Focused();
	g_InitWin = false;

	// buttons enable/disable
	DisableButton(walk_id, g_WalkAround);

	HideButton(brothel2_id, g_Brothels.GetNumBrothels() < 2 || g_Brothels.GetBrothel(1) == 0);
	HideButton(brothel3_id, g_Brothels.GetNumBrothels() < 3 || g_Brothels.GetBrothel(2) == 0);
	HideButton(brothel4_id, g_Brothels.GetNumBrothels() < 4 || g_Brothels.GetBrothel(3) == 0);
	HideButton(brothel5_id, g_Brothels.GetNumBrothels() < 5 || g_Brothels.GetBrothel(4) == 0);
	HideButton(brothel6_id, g_Brothels.GetNumBrothels() < 6 || g_Brothels.GetBrothel(5) == 0);


	string brothel = "Current Brothel: ";
	brothel += g_Brothels.GetName(g_CurrBrothel);
	EditTextItem(brothel, curbrothel_id);
}

void cScreenTown::process()
{
	if (!ids_set)set_ids();								// we need to make sure the ID variables are set
	if (girlimage_id != -1 && !eventrunning)	HideImage(girlimage_id, true);
	init();
	if (g_InterfaceEvents.GetNumEvents() == 0) return;	// no events means we can go home

	/*
	*	otherwise, compare event IDs
	*
	*	if it's the back button, pop the window off the stack
	*	and we're done
	*/
	/* */if (g_InterfaceEvents.CheckButton(back_id))		{ g_InitWin = true;		g_WinManager.Pop();					return; }
	else if (g_InterfaceEvents.CheckButton(slavemarket_id)) { g_InitWin = true;		g_WinManager.push("Slave Market");	return; }
	else if (g_InterfaceEvents.CheckButton(prison_id))		{ g_InitWin = true;		g_WinManager.push("Prison");		return; }
	else if (g_InterfaceEvents.CheckButton(house_id))		{ g_Building = BUILDING_HOUSE;	g_CurrHouse = 0;	g_InitWin = true;	g_WinManager.push("Player House");	return; }
	else if (g_InterfaceEvents.CheckButton(clinic_id))		{ check_clinic(0);		g_InitWin = true;			return; }
	else if (g_InterfaceEvents.CheckButton(studio_id))		{ check_studio(0);		g_InitWin = true;			return; }
	else if (g_InterfaceEvents.CheckButton(arena_id))		{ check_arena(0);		g_InitWin = true;			return; }
	else if (g_InterfaceEvents.CheckButton(centre_id))		{ check_centre(0);		g_InitWin = true;			return; }
	else if (g_InterfaceEvents.CheckButton(farm_id))		{ check_farm(0);		g_InitWin = true;			return; }
	else if (g_InterfaceEvents.CheckButton(mayor_id))		{ g_InitWin = true;		g_WinManager.push("Mayor");	return; }
	else if (g_InterfaceEvents.CheckButton(bank_id))		{ g_InitWin = true;		g_WinManager.push("Bank");	return; }
	else if (g_InterfaceEvents.CheckButton(shop_id))		{ g_InitWin = true;		g_AllTogle = false;	g_WinManager.push("Item Management");	return; }
	else if (g_InterfaceEvents.CheckButton(brothel0_id))	{ check_brothel(0);		g_InitWin = true;	return; }
	else if (g_InterfaceEvents.CheckButton(brothel1_id))	{ check_brothel(1);		g_InitWin = true;	return; }
	else if (g_InterfaceEvents.CheckButton(brothel2_id))	{ check_brothel(2);		g_InitWin = true;	return; }
	else if (g_InterfaceEvents.CheckButton(brothel3_id))	{ check_brothel(3);		g_InitWin = true;	return; }
	else if (g_InterfaceEvents.CheckButton(brothel4_id))	{ check_brothel(4);		g_InitWin = true;	return; }
	else if (g_InterfaceEvents.CheckButton(brothel5_id))	{ check_brothel(5);		g_InitWin = true;	return; }
	else if (g_InterfaceEvents.CheckButton(brothel6_id))	{ check_brothel(6);		g_InitWin = true;	return; }
	else if (g_InterfaceEvents.CheckButton(walk_id))		{ do_walk();			if (!g_Cheats) g_WalkAround = true;	g_InitWin = true; }
	else if (g_InterfaceEvents.CheckButton(setup_id))		{ g_InitWin = true;		g_WinManager.push("Property Management");		return; }	// `J` added

}

string cScreenTown::walk_no_luck()
{
	if (m_first_walk)
	{
		m_first_walk = false;
		return	"Your father once called this \"talent spotting\" - wandering though town, looking for new girls. \"Willing, desperate or vulnerable\" is how he put it. Not that any of those qualities are in evidence today, reminding you of another of your father's sayings - the one about patience. It's probably the only time he ever used the word \"virtue\".";
	}
	switch (g_Dice % 8)
	{
	case 1: return  "Married. Married. Bodyguard. Already works for you. Married. Hideous. Not a woman. Married. Escorted. Married... Might as well go home, there's nothing happening out here.";
	case 2: return	"It's not a bad life, if you can get paid for hanging around on street corners and eyeing up the pretty girls. Not a single decent prospect in the bunch of them, mind. Every silver lining has a cloud...";
	case 3: return	"You've walked and walked and walked, and the prettiest thing you've seen all day turned out not to be female. It's time to go home...";
	case 4: return	"When the weather is bad, the hunting is good. Get them cold and wet enough and girls too proud to spread their legs suddenly can't get their knickers off fast enough, if the job only comes with room and board. The down side is that you spend far too much time walking in the rain when everyone sane is warm inside. Time to head home for a mug of cocoa and a nice hot trollop.";
	case 5: return	"There's a bit of skirt over there with a lovely figure, and had a face that was pretty, ninety years ago. Over yonder, a sweet young thing frolicking through the marketplace. She's being ever so daring, spending her daddy's gold, and hasn't yet realised that there's a dozen of her daddy's goons keeping a discreet eye on her.  It's like that everywhere today. Maybe tomorrow will be better.";
	default:return	"The city is quiet. The same old streets; the same old faces.";
	}
	// I don't think this should happen, hence the overly dramatic prose
	return "The sky is filled with thunder, and portals are opening all over Crossgate. You've seen five rains of frogs so far and three madmen speaking in tongues. In the marketplace a mechanical contraption materialised which walked like a man and declaimed alien poetry for half an hour before bursting into flames and collapsing. And the only thing all day that looked female and and attractive turned out to be some otherwordly species of carnivorous plant, and had to be destroyed by the town guard. The only good thing about this day is that it's over. It's time to go home.";
}

void cScreenTown::do_walk()
{
	if (g_WalkAround)
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
			PrepareImage(girlimage_id, girl, IMGTYPE_PROFILE, true, ImageNum);
			HideImage(girlimage_id, false);
		}
		else HideImage(girlimage_id, true);
	}

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
		introfile = "MeetTownDefault.script.intro";
		intro = DirPath() << "Resources" << "Scripts";
		dp = DirPath() << "Resources" << "Scripts" << "MeetTownDefault.script";			// no, so trigger the default one
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
		message = "You go out searching around town for any new girls. You notice a potential new girl and walk up to her.";
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

bool cScreenTown::buy_building(static_brothel_data* bck)
{
	locale syslocale("");
	stringstream ss;
	ss.imbue(syslocale);

	if (!g_Gold.afford(bck->price) || g_Gangs.GetNumBusinessExtorted() < bck->business)
	{	// can't buy it
		ss << ("This building costs ") << bck->price << (" gold and you need to control at least ") << bck->business << (" businesses.");
		if (!g_Gold.afford(bck->price))
			ss << "\n" << ("You need ") << (bck->price - g_Gold.ival()) << (" more gold to afford it.");
		if (g_Gangs.GetNumBusinessExtorted() < bck->business)
			ss << "\n" << ("You need to control ") << (bck->business - g_Gangs.GetNumBusinessExtorted()) << (" more businesses.");
		g_MessageQue.AddToQue(ss.str(), 0);
		return false;
	}
	else	// can buy it
	{
		ss << ("Do you wish to purchase this building for ") << bck->price << (" gold? It has ") << bck->rooms << (" rooms.");
		g_MessageQue.AddToQue(ss.str(), 2);
		g_ChoiceManager.CreateChoiceBox(224, 112, 352, 384, 0, 2, 32, 8, 16);
		g_ChoiceManager.AddChoice(0, ("Buy It"), 0);
		g_ChoiceManager.AddChoice(0, ("Don't Buy It"), 1);
		g_ChoiceManager.SetActive(0);
		return true;
	}
	return false;
}

// player clicked on one of the brothels
void cScreenTown::check_brothel(int BrothelNum)
{
	if (g_Brothels.GetNumBrothels() == BrothelNum)	// player doesn't own this brothel... can he buy it?
	{
		static_brothel_data *bck = brothel_data + BrothelNum;
		if (buy_building(bck)) BuyBrothel = BrothelNum;

	}
	else	// player owns this brothel... go to it
	{
		g_Building = BUILDING_BROTHEL;
		g_CurrBrothel = BrothelNum;
		g_WinManager.Pop();
	}
}

void cScreenTown::check_clinic(int ClinicNum)
{	// player clicked on one of the brothels
	if (g_Clinic.GetNumBrothels() == ClinicNum)
	{	// player doesn't own this brothel... can he buy it?
		static_brothel_data *bck = clinic_data + ClinicNum;
		if (buy_building(bck)) BuyClinic = ClinicNum;
	}
	else
	{	// player owns this brothel... go to it
		g_Building = BUILDING_CLINIC;
		g_CurrClinic = ClinicNum;
		g_WinManager.push("Clinic Screen");
	}
}

void cScreenTown::check_centre(int CentreNum)
{	// player clicked on one of the brothels
	if (g_Centre.GetNumBrothels() == CentreNum)
	{	// player doesn't own this brothel... can he buy it?
		static_brothel_data *bck = centre_data + CentreNum;
		if (buy_building(bck)) 			BuyCentre = CentreNum;
	}
	else
	{	// player owns this brothel... go to it
		g_Building = BUILDING_CENTRE;
		g_CurrCentre = CentreNum;
		g_WinManager.push("Centre Screen");
	}
}

void cScreenTown::check_arena(int ArenaNum)
{	// player clicked on one of the brothels
	if (g_Arena.GetNumBrothels() == ArenaNum)
	{	// player doesn't own this brothel... can he buy it?
		static_brothel_data *bck = arena_data + ArenaNum;
		if (buy_building(bck)) 			BuyArena = ArenaNum;
	}
	else
	{	// player owns this brothel... go to it
		g_Building = BUILDING_ARENA;
		g_CurrArena = ArenaNum;
		g_WinManager.push("Arena Screen");
	}
}

void cScreenTown::check_studio(int StudioNum)
{	// player clicked on one of the brothels
	if (g_Studios.GetNumBrothels() == StudioNum)
	{	// player doesn't own this Studio... can he buy it?
		static_brothel_data *bck = studio_data + StudioNum;
		if (buy_building(bck)) 			BuyStudio = StudioNum;
	}
	else
	{	// player owns this brothel... go to it
		g_Building = BUILDING_STUDIO;
		g_CurrStudio = StudioNum;
		g_WinManager.push("Movie Screen");
	}
}

void cScreenTown::check_farm(int FarmNum)
{	// player clicked on one of the brothels
	if (g_Farm.GetNumBrothels() == FarmNum)
	{	// player doesn't own this Studio... can he buy it?
		static_brothel_data *bck = farm_data + FarmNum;
		if (buy_building(bck)) 			BuyFarm = FarmNum;
	}
	else
	{	// player owns this brothel... go to it
		g_Building = BUILDING_FARM;
		g_CurrFarm = FarmNum;
		g_WinManager.push("Farm Screen");
	}
}

