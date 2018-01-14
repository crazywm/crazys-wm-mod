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
#include <string>
#include <sstream>
#include <algorithm>
#include "InterfaceIDs.h"
#include "InterfaceGlobals.h"
#include "cScreenDungeon.h"
#include "cScriptManager.h"
#include "cBrothel.h"
#include "main.h"
#include "DirPath.h"
#include "cTariff.h"
#include "cGirlGangFight.h"
#include "cGirlTorture.h"
#include "cScreenGirlDetails.h"

extern cScreenGirlDetails	g_GirlDetails;
extern sInterfaceIDs		g_interfaceid;
extern bool					g_InitWin;
extern cRng					g_Dice;
extern int					g_TalkCount;
extern bool					g_Cheats;
extern cPlayer*				The_Player;
extern sGirl*				selected_girl;
extern vector<int>			cycle_girls;
extern int					cycle_pos;
extern bool					eventrunning;
extern int					g_CurrentScreen;

extern bool					g_UpArrow;
extern bool					g_DownArrow;
extern bool					g_AltKeys;	// New hotkeys --PP
extern bool					g_EnterKey;
extern bool					g_SpaceKey;
extern bool					g_Q_Key;
extern bool					g_W_Key;
extern bool					g_E_Key;
extern bool					g_A_Key;
extern bool					g_S_Key;
extern bool					g_D_Key;
extern bool					g_Z_Key;
extern bool					g_X_Key;
extern bool					g_C_Key;

static cDungeon*			dungeon = g_Brothels.GetDungeon();
static cTariff				tariff;
static string				ReleaseGirlToWhere = "Br0";
static int					lastNum = -1;
static int					ImageNum = -1;
static vector<int>			select_girls;
static stringstream ss;

bool cScreenDungeon::ids_set = false;
cScreenDungeon::cScreenDungeon()
{
	selection = -1;
	DirPath dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "dungeon_screen.xml";
	m_filename = dp.c_str();
}
cScreenDungeon::~cScreenDungeon() {}

void cScreenDungeon::set_ids()
{
	ids_set			/**/ = true;
	g_LogFile.write("set_ids in cScreenDungeon");

	back_id 		/**/ = get_id("BackButton", "Back");
	header_id 		/**/ = get_id("DungeonHeader");
	gold_id			/**/ = get_id("Gold");
	girllist_id 	/**/ = get_id("GirlList");
	girlimage_id 	/**/ = get_id("GirlImage");
	brandslave_id 	/**/ = get_id("BrandSlaveButton");
	release_id 		/**/ = get_id("ReleaseButton");
	allowfood_id 	/**/ = get_id("AllowFoodButton");
	torture_id 		/**/ = get_id("TortureButton");
	stopfood_id 	/**/ = get_id("StopFeedingButton");
	interact_id 	/**/ = get_id("InteractButton");
	interactc_id	/**/ = get_id("InteractCount");
	releaseall_id 	/**/ = get_id("ReleaseAllButton");
	releasecust_id 	/**/ = get_id("ReleaseCustButton");
	viewdetails_id 	/**/ = get_id("DetailsButton");
	sellslave_id 	/**/ = get_id("SellButton");

	releaseto_id 	/**/ = get_id("ReleaseTo");
	roomsfree_id 	/**/ = get_id("RoomsFree");
	brothel0_id 	/**/ = get_id("Brothel0");
	brothel1_id 	/**/ = get_id("Brothel1");
	brothel2_id 	/**/ = get_id("Brothel2");
	brothel3_id 	/**/ = get_id("Brothel3");
	brothel4_id 	/**/ = get_id("Brothel4");
	brothel5_id 	/**/ = get_id("Brothel5");
	brothel6_id 	/**/ = get_id("Brothel6");
	house_id 		/**/ = get_id("House");
	clinic_id 		/**/ = get_id("Clinic");
	studio_id 		/**/ = get_id("Studio");
	arena_id 		/**/ = get_id("Arena");
	centre_id 		/**/ = get_id("Centre");
	farm_id 		/**/ = get_id("Farm");

	//Set the default sort order for columns, so listbox knows the order in which data will be sent
	SortColumns(girllist_id, m_ListBoxes[girllist_id]->m_ColumnName, m_ListBoxes[girllist_id]->m_ColumnCount);

	if (cfg.debug.log_debug())	g_LogFile.write("cScreenDungeon::set_ids > Done");
}

void cScreenDungeon::init()
{
	if (cfg.debug.log_debug())	g_LogFile.write("cScreenDungeon::init > Start");
	Focused();
	g_CurrentScreen = SCREEN_DUNGEON;
	ClearListBox(girllist_id);				// clear the lists
	g_LogFile.write("::init: Dungeon\n");	// `J`
	vector<string> columnNames;				//get a list of all the column names, so we can find which data goes in that column
	m_ListBoxes[girllist_id]->GetColumnNames(columnNames);
	int numColumns = columnNames.size();

	if (gold_id >= 0)
	{
		ss.str(""); ss << "Gold: " << g_Gold.ival();
		EditTextItem(ss.str(), gold_id);
	}

	if (cfg.debug.log_debug())	g_LogFile.write("cScreenDungeon::init > 1");
	ss.str("");	ss << "Your Dungeon where " << dungeon->GetNumDied() << " people have died.";
	EditTextItem(ss.str(), header_id);
	// Fill the list box
	selection = dungeon->GetNumGirls() > 0 ? 0 : -1;
	string* Data = new string[numColumns];
	for (int i = 0; i < dungeon->GetNumGirls(); i++)												// add girls
	{
		sGirl *girl = dungeon->GetGirl(i)->m_Girl;													// get the i-th girl
		if (selected_girl == girl) selection = i;													// if selected_girl is this girl, update selection
		girl->m_DayJob = girl->m_NightJob = JOB_INDUNGEON;
		int col = ((girl->health() <= 30) || (girl->happiness() <= 30)) ? COLOR_RED : COLOR_BLUE;	// if she's low health or unhappy, flag her entry to display in red // Anon21
		dungeon->OutputGirlRow(i, Data, columnNames);												// add her to the list
		AddToListBox(girllist_id, i, Data, numColumns, col);
	}
	// now add the customers
	int offset = dungeon->GetNumGirls();
	for (int i = 0; i < dungeon->GetNumCusts(); i++)	// add customers
	{
		int col = (dungeon->GetCust(i)->m_Health <= 30) ? COLOR_RED : COLOR_BLUE;
		dungeon->OutputCustRow(i, Data, columnNames);
		AddToListBox(girllist_id, i + offset, Data, numColumns, col);
	}
	delete[] Data;

	if (cfg.debug.log_debug())	g_LogFile.write("cScreenDungeon::init > 2");
	// disable some buttons
	DisableButton(allowfood_id);
	DisableButton(stopfood_id);
	DisableButton(interact_id);
	if (interactc_id >= 0)
	{
		ss.str(""); ss << "Interactions Left: ";
		if (g_Cheats) ss << "Infinate Cheat";
		else if (g_TalkCount <= 0) ss << "0 (buy in House screen)";
		else ss << g_TalkCount;
		EditTextItem(ss.str(), interactc_id);
	}
	DisableButton(release_id);
	DisableButton(brandslave_id);
	DisableButton(torture_id);
	DisableButton(sellslave_id);
	//	cerr << "::init: disabling torture" << endl;	// `J` commented out
	DisableButton(viewdetails_id);
	DisableButton(releaseall_id, (dungeon->GetNumGirls() <= 0));	// only enable "release all girls" if there are girls to release
	DisableButton(releasecust_id, (dungeon->GetNumCusts() <= 0));	// similarly...

	HideImage(girlimage_id, dungeon->GetNumGirls() <= 0);

	string sub = ReleaseGirlToWhere.substr(0, 2);
	sBrothel *releaseto = g_Brothels.GetBrothel(g_CurrBrothel);
	char a = ReleaseGirlToWhere[2]; char b = "0"[0]; int sendtonum = a - b;	// `J` cheap fix to get brothel number
	/* */if (sub == "Br") releaseto = g_Brothels.GetBrothel(sendtonum);
	else if (sub == "Ho") releaseto = g_House.GetBrothel(sendtonum);
	else if (sub == "Cl") releaseto = g_Clinic.GetBrothel(sendtonum);
	else if (sub == "St") releaseto = g_Studios.GetBrothel(sendtonum);
	else if (sub == "Ar") releaseto = g_Arena.GetBrothel(sendtonum);
	else if (sub == "Ce") releaseto = g_Centre.GetBrothel(sendtonum);
	else if (sub == "Fa") releaseto = g_Farm.GetBrothel(sendtonum);

	ss.str("");	ss << "Release Girl to: " << releaseto->m_Name;
	EditTextItem(ss.str(), releaseto_id);
	ss.str("");	ss << "Room for " << releaseto->free_rooms() << " more girls.";
	EditTextItem(ss.str(), roomsfree_id);

	HideButton(brothel1_id, g_Brothels.GetNumBrothels() < 2 || g_Brothels.GetBrothel(1) == 0);
	HideButton(brothel2_id, g_Brothels.GetNumBrothels() < 3 || g_Brothels.GetBrothel(2) == 0);
	HideButton(brothel3_id, g_Brothels.GetNumBrothels() < 4 || g_Brothels.GetBrothel(3) == 0);
	HideButton(brothel4_id, g_Brothels.GetNumBrothels() < 5 || g_Brothels.GetBrothel(4) == 0);
	HideButton(brothel5_id, g_Brothels.GetNumBrothels() < 6 || g_Brothels.GetBrothel(5) == 0);
	HideButton(brothel6_id, g_Brothels.GetNumBrothels() < 7 || g_Brothels.GetBrothel(6) == 0);
	HideButton(clinic_id, g_Clinic.GetNumBrothels() < 1 || g_Clinic.GetBrothel(0) == 0);
	HideButton(studio_id, g_Studios.GetNumBrothels() < 1 || g_Studios.GetBrothel(0) == 0);
	HideButton(arena_id, g_Arena.GetNumBrothels() < 1 || g_Arena.GetBrothel(0) == 0);
	HideButton(centre_id, g_Centre.GetNumBrothels() < 1 || g_Centre.GetBrothel(0) == 0);
	HideButton(farm_id, g_Farm.GetNumBrothels() < 1 || g_Farm.GetBrothel(0) == 0);

	g_InitWin = false;

	if (cfg.debug.log_debug())	g_LogFile.write("cScreenDungeon::init > 3");
	// if a selection of girls is stored, try to re-select them
	if (!select_girls.empty())
	{
		selection = select_girls.back();
		for (int i = 0; i < (int)select_girls.size(); i++)
		{
			SetSelectedItemInList(girllist_id, select_girls[i], (select_girls[i] == select_girls.back()), false);
		}
		select_girls.clear();
	}
	else if (selection >= 0) SetSelectedItemInList(girllist_id, selection);
	if (cfg.debug.log_debug())	g_LogFile.write("cScreenDungeon::init > Done");
}

void cScreenDungeon::selection_change()
{
	selection = GetLastSelectedItemFromList(girllist_id);
	// if nothing is selected, then we just need to disable some buttons and we're done
	if (selection == -1)
	{
		selected_girl = 0;
		DisableButton(brandslave_id);
		DisableButton(allowfood_id);
		DisableButton(stopfood_id);
		DisableButton(interact_id);
		DisableButton(release_id);
		DisableButton(torture_id);
		//		cerr << "selection = " << selection << " (-1) - disabling torture" << endl;	// `J` commented out
		DisableButton(viewdetails_id);
		DisableButton(sellslave_id);
		selection = -1;		// can this have changed?
		return;
	}
	// otherwise, we need to enable some buttons...
	DisableButton(sellslave_id);
	DisableButton(torture_id, !torture_possible());
	//	cerr << "selection = " << selection << " - enabling torture" << endl;	// `J` commented out
	DisableButton(interact_id, g_TalkCount == 0);
	EnableButton(release_id);
	DisableButton(brandslave_id);
	// and then decide if this is a customer selected, or a girl customer is easiest, so we do that first
	if ((selection - dungeon->GetNumGirls()) >= 0)
	{
		// It's a customer! All we need to do is toggle some buttons
		cerr << "Player selecting Dungeon Customer #" << selection << endl;	// `J` rewrote to reduce confusion
		int num = (selection - dungeon->GetNumGirls());
		sDungeonCust* cust = dungeon->GetCust(num);
		DisableButton(viewdetails_id);
		DisableButton(allowfood_id, cust->m_Feeding);
		DisableButton(stopfood_id, !cust->m_Feeding);
		return;
	}
	// Not a customer then. Must be a girl...
	cerr << "Player selecting Dungeon Girl #" << selection << endl;	// `J` rewrote to reduce confusion
	int num = selection;
	sDungeonGirl* dgirl = dungeon->GetGirl(num);
	sGirl * girl = dgirl->m_Girl;
	// again, we're just enabling and disabling buttons
	EnableButton(viewdetails_id);
	
	DisableButton(allowfood_id, dgirl->m_Feeding);
	DisableButton(stopfood_id, !dgirl->m_Feeding);
	// some of them partly depend upon whether she's a slave or not
	if (girl->is_slave())
	{
		EnableButton(sellslave_id);
		DisableButton(brandslave_id);
	}
	else
	{
		EnableButton(brandslave_id);
		DisableButton(sellslave_id);
	}
	selected_girl = girl;
}

int cScreenDungeon::view_girl()
{
	selection = GetSelectedItemFromList(girllist_id);

	if (selection == -1) return Continue;							// nothing selected, nothing to do.
	if ((selection - dungeon->GetNumGirls()) >= 0) return Continue;	// if this is a customer, we're not interested
	sGirl *girl = dungeon->GetGirl(selection)->m_Girl;				// if we can't find the girl, there's nothing we can do
	if (!girl) return Continue;
	if (girl->health() > 0)
	{
		//load up the cycle_girls vector with the ordered list of girl IDs
		FillSortedIDList(girllist_id, &cycle_girls, &cycle_pos);
		// we don't want customers or dead girls in this list
		for (int i = cycle_girls.size(); i-- > 0;)
		{
			if (cycle_girls[i] >= dungeon->GetNumGirls() || dungeon->GetGirl(cycle_girls[i])->m_Girl->is_dead())
				cycle_girls.erase(cycle_girls.begin() + i);
		}
		g_CurrentScreen = SCREEN_GIRLDETAILS;
		g_InitWin = true;
		g_WinManager.push("Girl Details");
		return Return;
	}
	// can't ... resist ...
	g_MessageQue.AddToQue("This is a dead girl. She has ceased to be.", COLOR_RED);
	// Furthermore, she's shuffled off this mortal coil and joined the bleeding choir invisible!
	return Return;
}

int cScreenDungeon::enslave_customer(int girls_removed, int custs_removed)
{
	/*
	*	mod - docclox - nerfed the cash for selling a customer.
	*	a fat smelly brothel creeper probably shouldn't raise as much as
	*	a sexy young slavegirl. Feel free to un-nerf if you disagree.
	*/
	long gold = (g_Dice % 200) + 63;
	g_Gold.slave_sales(gold);
	ss.str("");	ss << "You force the customer into slavery lawfully for committing a crime against your business and sell them for " << gold << " gold.";
	g_MessageQue.AddToQue(ss.str(), 0);
	The_Player->evil(1);
	int customer_index = selection - dungeon->GetNumGirls();	// get the index of the about-to-be-sold customer
	customer_index += girls_removed;
	customer_index -= custs_removed;
	sDungeonCust* cust = dungeon->GetCust(customer_index);		// get the customer record
	dungeon->RemoveCust(cust);		// remove the customer from the dungeon room for an overload here
	return 0;
}

void cScreenDungeon::set_slave_stats(sGirl *girl)
{
	girl->set_slave();
	girl->obedience(-10);
	girl->pcfear(5);
	girl->pclove(-10);
	girl->pchate(5);
	girl->m_Stats[STAT_HOUSE] = cfg.initial.slave_house_perc();
	girl->m_AccLevel = cfg.initial.slave_accom();
}

int cScreenDungeon::enslave()
{
	int numCustsRemoved = 0;
	int numGirlsRemoved = 0;
	int pos = 0, deadcount = 0;

	store_selected_girls();
	// roll on vectors!
	for (selection = GetNextSelectedItemFromList(girllist_id, 0, pos); selection != -1; selection = GetNextSelectedItemFromList(girllist_id, pos + 1, pos))
	{
		if ((selection - (dungeon->GetNumGirls() + numGirlsRemoved)) >= 0)	// it is a customer
		{
			enslave_customer(numGirlsRemoved, numCustsRemoved);
			numCustsRemoved++;
			continue;
		}
		// it is a girl
		sDungeonGirl* dgirl = dungeon->GetGirl(selection - numGirlsRemoved);
		sGirl *girl = dgirl->m_Girl;
		if (girl->is_slave()) continue;					// nothing to do if she's _already_ enslaved
		if (girl->is_dead()) { deadcount++; continue; }	// likewise, dead girls can't be enslaved
		cGirlGangFight ggf(girl);						// This is much simpler if she just submits...

		if (ggf.girl_submits())
		{
			set_slave_stats(girl);
			ss.str(""); ss << girl->m_Realname << " submits the the enchanted slave tattoo being placed upon her.";
			g_MessageQue.AddToQue(ss.str(), 0);
			continue;
		}

		if (!ggf.player_won())	// did the player need to step in
		{
			// adjust the girl's stats to reflect her new status and then evil up the player because he forced her into slavery
			The_Player->evil(5);
			set_slave_stats(girl);
			ss.str(""); ss << girl->m_Realname << " breaks free from your goons' control. You restrain her personally while the slave tattoo placed upon her.";
			g_MessageQue.AddToQue(ss.str(), COLOR_RED);
			continue;
		}

		if (ggf.girl_lost())		// there was a gang, and some of them are still with us
		{
			The_Player->evil(5);	// evil up the player for doing a naughty thing and adjust the girl's stats
			set_slave_stats(girl);
			ss.str(""); ss << girl->m_Realname << " puts up a fight " << "but your goons control her as the enchanted slave tattoo is placed upon her.";
			g_MessageQue.AddToQue(ss.str(), COLOR_RED);	// and queue the message
			continue;
		}
		// we just did the "lost" case this is the girl wins case
		ss.str(""); ss << girl->m_Realname << " puts up a fight and ";
		if (ggf.wipeout()) ss << " the gang is wiped out and";			// if there is a gang, but it has no members

		// If girl wins she escapes and leaves the brothel
		ss << "And after defeating you as well she escapes to the outside world.\n";
		dungeon->RemoveGirl(girl);
		numGirlsRemoved++;
		girl->run_away();
		The_Player->suspicion(15);					// suspicion goes up
		The_Player->evil(15);						// so does evil
		g_MessageQue.AddToQue(ss.str(), COLOR_RED);	// add to the message queue
	}
	if (deadcount > 0) g_MessageQue.AddToQue("There's not much point in using a slave tattoo on a dead body.", 0);
	g_InitWin = true;
	return Return;
}

void cScreenDungeon::release_all_customers()
{
	// loop until we run out of customers
	while (dungeon->GetNumCusts() > 0)
	{
		sDungeonCust* cust = dungeon->GetCust(0);		// get the first customer in the list
		dungeon->RemoveCust(cust);						// remove from brothel
		// de-evil the player for being nice suspicion drops too
		The_Player->evil(-5);
		The_Player->suspicion(-5);
	}
	selection = -1;
	g_InitWin = true;
}

void cScreenDungeon::sell_slaves()
{
	int paid = 0, count = 0, deadcount = 0;
	vector<int> girl_array;
	get_selected_girls(&girl_array);  // get and sort array of girls/customers
	vector<string> girl_names;
	vector<int> sell_gold;
	for (int i = girl_array.size(); i-- > 0;)
	{
		selection = girl_array[i];
		if ((selection - dungeon->GetNumGirls()) >= 0) continue;	// if this is a customer, we skip to the next list entry

		sDungeonGirl* dgirl = dungeon->GetGirl(selection);			// get the index of the girl, get the DungeonGirl entry
		sGirl *girl = dgirl->m_Girl;								// and get the sGirl

		if (girl->is_slave() == false) continue;					// if she's not a slave, the player isn't allowed to sell her
		if (girl->is_dead())										// likewise, dead slaves can't be sold
		{
			deadcount++;
			continue;
		}
		// she's a living slave, she's out of here
		g_Girls.CalculateAskPrice(girl, false);
		int cost = tariff.slave_sell_price(girl);					// get the sell price of the girl. This is a little on the occult side
		g_Gold.slave_sales(cost);
		paid += cost;
		count++;
		girl = dungeon->RemoveGirl(dungeon->GetGirl(selection));	// remove her from the dungeon, add her back into the general pool
		girl_names.push_back(girl->m_Realname);
		sell_gold.push_back(cost);
		if (girl->m_Realname.compare(girl->m_Name) == 0)
		{
			g_Girls.AddGirl(girl);									// add unique girls back to main pool
		}
		else { girl = 0; }											// random girls simply get removed from the game
		g_Brothels.RemoveGirl(g_CurrBrothel, girl, true);			// `J` added to fix "dungeon screen to brothel screen crash"
	}
	if (deadcount > 0) g_MessageQue.AddToQue("Nobody is currently in the market for dead girls.", COLOR_YELLOW);
	if (count <= 0) return;

	ss.str(""); ss << "You sold ";
	if (count == 1)		{ ss << girl_names[0] << " for " << sell_gold[0] << " gold."; }
	else
	{
		ss << count << " slaves:";
		for (int i = 0; i < count; i++)
		{
			ss << "\n    " << girl_names[i] << "   for " << sell_gold[i] << " gold";
		}
		ss << "\nFor a total of " << paid << " gold.";
	}
	g_MessageQue.AddToQue(ss.str(), 0);
	selection = -1;
	g_InitWin = true;
}

void cScreenDungeon::release_all_girls()
{
	string sub = ReleaseGirlToWhere.substr(0, 2);
	sBrothel *brothel = g_Brothels.GetBrothel(g_CurrBrothel);
	char a = ReleaseGirlToWhere[2]; char b = "0"[0]; int sendtonum = a - b;	// `J` cheap fix to get brothel number
	/* */if (sub == "Br") brothel = g_Brothels.GetBrothel(sendtonum);
	else if (sub == "Ho") brothel = g_House.GetBrothel(sendtonum);
	else if (sub == "Cl") brothel = g_Clinic.GetBrothel(sendtonum);
	else if (sub == "St") brothel = g_Studios.GetBrothel(sendtonum);
	else if (sub == "Ar") brothel = g_Arena.GetBrothel(sendtonum);
	else if (sub == "Ce") brothel = g_Centre.GetBrothel(sendtonum);
	else if (sub == "Fa") brothel = g_Farm.GetBrothel(sendtonum);

	if (brothel->free_rooms() <= 0) 	// Is there room?
	{
		g_MessageQue.AddToQue("The current building has no more room.\nBuild more rooms, get rid of some girls, or change the building you are currently releasing girls to.", COLOR_YELLOW);
		return;
	}
	while (dungeon->GetNumGirls() > 0) 		// loop until there are no more girls to release
	{
		if (brothel->free_rooms() > 0) 		// make sure there's room for another girl
		{
			sGirl* girl = dungeon->RemoveGirl(dungeon->GetGirl(0));
			/* */if (sub == "Br") g_Brothels.AddGirl(sendtonum, girl);
			else if (sub == "Ho") g_House.AddGirl(sendtonum, girl);
			else if (sub == "Cl") g_Clinic.AddGirl(sendtonum, girl);
			else if (sub == "St") g_Studios.AddGirl(sendtonum, girl);
			else if (sub == "Ar") g_Arena.AddGirl(sendtonum, girl);
			else if (sub == "Ce") g_Centre.AddGirl(sendtonum, girl);
			else if (sub == "Fa") g_Farm.AddGirl(sendtonum, girl);
			continue;
		}
		// we only get here if we run out of space
		g_MessageQue.AddToQue("There is no more room in the current building.\nBuild more rooms, get rid of some girls, or change the building you are currently releasing girls to.", 0);
		break;
	}
}

void cScreenDungeon::stop_feeding()
{
	// and then loop using multi_first() and multi_next()
	for (int selection = multi_first(); selection != -1; selection = multi_next())
	{
		int num_girls = dungeon->GetNumGirls();
		// if the selection is more than than the number of girls it has to be a customer
		if ((selection - num_girls) >= 0)	// it is a customer
		{
			int num = (selection - num_girls);
			sDungeonCust* cust = dungeon->GetCust(num);
			cust->m_Feeding = true;
		}
		else
		{
			sDungeonGirl* girl = dungeon->GetGirl(selection);
			girl->m_Feeding = true;
		}
	}
}

void cScreenDungeon::start_feeding()
{
	int pos = 0;
	selection = GetNextSelectedItemFromList(girllist_id, 0, pos);
	while (selection != -1)
	{
		if ((selection - dungeon->GetNumGirls()) >= 0)	// it is a customer
		{
			int num = (selection - dungeon->GetNumGirls());
			sDungeonCust* cust = dungeon->GetCust(num);
			cust->m_Feeding = false;
		}
		else
		{
			int num = selection;
			sDungeonGirl* girl = dungeon->GetGirl(num);
			girl->m_Feeding = false;
		}
		selection = GetNextSelectedItemFromList(girllist_id, pos + 1, pos);
	}
}

void cScreenDungeon::torture_customer(int girls_removed)
{
	ss.str("Customer: ");
	int cust_index = selection - dungeon->GetNumGirls() + girls_removed;	// get the index number for the customer
	sDungeonCust* cust = dungeon->GetCust(cust_index);						// get the customer record from the dungeon

	if (!cust) return;
	if (cust->m_Tort && !g_Cheats) 		// don't let the PL torture more than once a day (unless cheating is enabled)
	{
		ss << "You may only torture someone once per week.";
		g_MessageQue.AddToQue(ss.str(), 0);
		return;
	}
	cust->m_Tort = true;		// flag the customer as tortured, decrement his health
	cust->m_Health -= 6;
	ss << "Screams fill the dungeon ";
	if (cust->m_Health > 0)
	{
		ss << "until the customer is battered, bleeding and bruised.\nYou leave them sobbing "
			<< (cust->m_Health >= 30 ? "uncontrollably." : "and near to death.");
	}
	else
	{
		cust->m_Health = 0;
		ss<<" gradually growing softer until it stops completely.\nThey are dead.";
		The_Player->evil(2);
	}
	g_MessageQue.AddToQue(ss.str(), 0);
}

/*
* If we have a multiple selection, then the torture button
* should be enabled if just one of the selected rows can
* be tortured
*/
bool cScreenDungeon::torture_possible()
{
	int nSelection;		// don't use selection for the loop - its a class global and can mess things up elsewhere
	int	nPosition = 0;
	int nNumGirls = dungeon->GetNumGirls();
	for (nSelection = GetNextSelectedItemFromList(girllist_id, 0, nPosition); nSelection != -1; nSelection = GetNextSelectedItemFromList(girllist_id, nPosition + 1, nPosition))
	{
		bool not_yet_tortured;
		// get the customer or girl under selection and find out if they've been tortured this turn
		if (nSelection >= nNumGirls)
		{
			sDungeonCust* dcust = dungeon->GetCust(nSelection - nNumGirls);
			not_yet_tortured = !dcust->m_Tort;
		}
		else
		{
			sDungeonGirl* dgirl = dungeon->GetGirl(nSelection);
			not_yet_tortured = !dgirl->m_Girl->m_Tort;
		}
		if (not_yet_tortured) return true;	// we only need one torturable prisoner so if we found one, we can go home
	}
	return false;							// we only get here if no-one in the list was torturable
}

void cScreenDungeon::torture()
{
	int pos = 0;
	int numGirlsRemoved = 0;
	store_selected_girls();

	for (selection = GetNextSelectedItemFromList(girllist_id, 0, pos); selection != -1; selection = GetNextSelectedItemFromList(girllist_id, pos + 1, pos))
	{
		// if it's a customer, we have a separate routine
		if ((selection - (dungeon->GetNumGirls() + numGirlsRemoved)) >= 0)
		{
			The_Player->evil(5);
			torture_customer(numGirlsRemoved);
			continue;
		}
		// If we get here, it's a girl
		sDungeonGirl* dgirl = dungeon->GetGirl(selection - numGirlsRemoved);
		cGirlTorture gt(dgirl);
	}
}

void cScreenDungeon::change_release(string towhere)
{
	ReleaseGirlToWhere = towhere;
	g_InitWin = true;
}

void cScreenDungeon::release()
{
	string sub = ReleaseGirlToWhere.substr(0, 2);
	sBrothel *brothel = g_Brothels.GetBrothel(g_CurrBrothel);
	char a = ReleaseGirlToWhere[2]; char b = "0"[0]; int sendtonum = a - b;	// `J` cheap fix to get brothel number
	/* */if (sub == "Br") brothel = g_Brothels.GetBrothel(sendtonum);
	else if (sub == "Ho") brothel = g_House.GetBrothel(sendtonum);
	else if (sub == "Cl") brothel = g_Clinic.GetBrothel(sendtonum);
	else if (sub == "St") brothel = g_Studios.GetBrothel(sendtonum);
	else if (sub == "Ar") brothel = g_Arena.GetBrothel(sendtonum);
	else if (sub == "Ce") brothel = g_Centre.GetBrothel(sendtonum);
	else if (sub == "Fa") brothel = g_Farm.GetBrothel(sendtonum);

	vector<int> girl_array;
	get_selected_girls(&girl_array);			// get and sort array of girls/customers
	for (int i = girl_array.size(); i--> 0;)
	{
		selection = girl_array[i];
		// check in case its a customer
		if ((selection - dungeon->GetNumGirls()) >= 0)
		{
			int num = selection - dungeon->GetNumGirls();
			sDungeonCust* cust = dungeon->GetCust(num);
			dungeon->RemoveCust(cust);
			// player did a nice thing: suss and evil go down :)
			The_Player->suspicion(-5);
			The_Player->evil(-5);
			continue;
		}

		// if there's room, remove her from the dungeon and add her to the current brothel
		int num = selection;
		if ((brothel->free_rooms()) > 0)
		{
			sGirl* girl = dungeon->RemoveGirl(dungeon->GetGirl(num));
			/* */if (sub == "Br") g_Brothels.AddGirl(sendtonum, girl);
			else if (sub == "Ho") g_House.AddGirl(sendtonum, girl);
			else if (sub == "Cl") g_Clinic.AddGirl(sendtonum, girl);
			else if (sub == "St") g_Studios.AddGirl(sendtonum, girl);
			else if (sub == "Ar") g_Arena.AddGirl(sendtonum, girl);
			else if (sub == "Ce") g_Centre.AddGirl(sendtonum, girl);
			else if (sub == "Fa") g_Farm.AddGirl(sendtonum, girl);
			continue;
		}
		// if we run out of space
		g_MessageQue.AddToQue("The current building has no more room.\nBuy a new one, get rid of some girls, or change the building you are currently releasing girls to.", 0);
		break;		// no point in running round the loop any further we're out of space
	}
}

void cScreenDungeon::talk()
{
	if (g_TalkCount <= 0) return;	// if we have no talks left, we can go home
	int v[2] = { 0, -1 };
	// customers are always last in the list, so we can determine if this is a customer by simple aritmetic
	if ((selection - g_Brothels.GetDungeon()->GetNumGirls()) >= 0) return;		// it is a customer

	// OK, it wasn't a customer
	int num = selection;
	sDungeonGirl* girl = g_Brothels.GetDungeon()->GetGirl(num);
	/*
	*	is she dead? that would make life simpler.
	*
	*	(actually, I'd like to be able to view her stats in read-only mode
	*	after she dies, just so I can do a post-mortem. But for now...)
	*/
	if (girl->m_Girl->is_dead())
	{
		g_MessageQue.AddToQue("Though you have many skills, speaking to the dead is not one of them.", 1);
		return;
	}
	// she's still alive. I guess we'll have to talk to her
	cTrigger* trig = 0;		// is there a girl specific script for this interaction?
	DirPath dp;
	eventrunning = true;
	if (!(trig = girl->m_Girl->m_Triggers.CheckForScript(TRIGGER_TALK, false, v)))
	{	// no, so trigger the default one
		dp = dp << "Resources" << "Scripts" << "DefaultInteractDungeon.script";
	}
	else
	{	// yes, so trigger the custom one
		dp = DirPath(cfg.folders.characters().c_str()) << girl->m_Girl->m_Name << trig->m_Script << "DefaultInteractDungeon.script";
	}
	cScriptManager script_manager;
	script_manager.Load(dp, girl->m_Girl);
	if (!g_Cheats) g_TalkCount--;
}

void cScreenDungeon::update_image()
{
	if ((selection - dungeon->GetNumGirls()) >= 0)	// Makes it so when on a customer it doesnt keep the last girls pic up
	{
		HideImage(girlimage_id, true);
	}
	else if ((selected_girl) && !IsMultiSelected(girllist_id))
	{
		PrepareImage(girlimage_id, selected_girl, selected_girl->m_Tort ? IMGTYPE_TORTURE : IMGTYPE_JAIL, true, ImageNum);
		HideImage(girlimage_id, false);
	}
	else
	{
		HideImage(girlimage_id, true);
	}
}

int cScreenDungeon::process_events()
{
	if (g_InterfaceEvents.GetNumEvents() == 0 && !g_SpaceKey) return Continue;
	if (g_InterfaceEvents.CheckButton(back_id))		// if it's the back button, pop the window off the stack and we're done
	{
		g_InitWin = true;
		g_WinManager.Pop();
		return Return;
	}
	if (g_InterfaceEvents.CheckListbox(girllist_id))
	{
		if (ListDoubleClicked(girllist_id))		// If double-clicked, try to bring up girl details
		{
			g_GirlDetails.lastsexact = -1;
			return view_girl();
		}
		selection_change();
		if (IsMultiSelected(girllist_id)) 		// disable buttons based on multiselection
		{
			DisableButton(interact_id, true);
			DisableButton(viewdetails_id, true);
		}
		update_image();
		return Return;
	}
	if (g_InterfaceEvents.CheckButton(viewdetails_id) || g_SpaceKey)
	{
		g_SpaceKey = false;
		g_GirlDetails.lastsexact = -1;
		return view_girl();
	}
	if (g_InterfaceEvents.CheckButton(brandslave_id))
	{
		return enslave();
	}
	if (g_InterfaceEvents.CheckButton(releasecust_id))
	{
		release_all_customers();
		return Return;
	}
	if (g_InterfaceEvents.CheckButton(sellslave_id))
	{
		sell_slaves();
		return Return;
	}
	if (g_InterfaceEvents.CheckButton(releaseall_id))
	{
		release_all_girls();
		selection = -1;
		g_InitWin = true;
		return Return;
	}
	if (g_InterfaceEvents.CheckButton(allowfood_id))
	{
		stop_feeding();
		selection = -1;
		g_InitWin = true;
		return Return;
	}
	if (g_InterfaceEvents.CheckButton(stopfood_id))
	{
		start_feeding();
		selection = -1;
		g_InitWin = true;
		return Return;
	}
	if (g_InterfaceEvents.CheckButton(interact_id))
	{
		if (selection == -1) return Return;
		talk();
		g_InitWin = true;
		return Return;
	}
	if (g_InterfaceEvents.CheckButton(torture_id))
	{
		torture();
		selection = -1;
		g_InitWin = true;
		return Return;
	}
	if (g_InterfaceEvents.CheckButton(release_id))
	{
		release();
		selection = -1;
		g_InitWin = true;
		return Return;
	}

	/* */if (g_InterfaceEvents.CheckButton(brothel0_id))	change_release("Br0");
	else if (g_InterfaceEvents.CheckButton(brothel1_id))	change_release("Br1");
	else if (g_InterfaceEvents.CheckButton(brothel2_id))	change_release("Br2");
	else if (g_InterfaceEvents.CheckButton(brothel3_id))	change_release("Br3");
	else if (g_InterfaceEvents.CheckButton(brothel4_id))	change_release("Br4");
	else if (g_InterfaceEvents.CheckButton(brothel5_id))	change_release("Br5");
	else if (g_InterfaceEvents.CheckButton(brothel6_id))	change_release("Br6");
	else if (g_InterfaceEvents.CheckButton(house_id))		change_release("Ho0");
	else if (g_InterfaceEvents.CheckButton(clinic_id))		change_release("Cl0");
	else if (g_InterfaceEvents.CheckButton(studio_id))		change_release("St0");
	else if (g_InterfaceEvents.CheckButton(arena_id))		change_release("Ar0");
	else if (g_InterfaceEvents.CheckButton(centre_id))		change_release("Ce0");
	else if (g_InterfaceEvents.CheckButton(farm_id))		change_release("Fa0");

	return Continue;
}

void cScreenDungeon::process()
{
	static int selection = -1;
	if (!ids_set) set_ids();	// we need to make sure the ID variables are set
	if (g_InitWin) init();
	if (process_events() == Return) return;

	if (g_UpArrow || g_DownArrow || (g_AltKeys && (g_A_Key || g_D_Key)))
	{
		selection = (g_UpArrow || g_A_Key) ? ArrowUpListBox(girllist_id) : ArrowDownListBox(girllist_id);
		g_UpArrow = g_DownArrow = g_A_Key = g_D_Key = false;
		return;
	}
}

void cScreenDungeon::get_selected_girls(vector<int> *girl_array)
{  // take passed vector and fill it with sorted list of girl/customer IDs
	int pos = 0;
	int GSelection = GetNextSelectedItemFromList(girllist_id, 0, pos);
	while (GSelection != -1)
	{
		girl_array->push_back(GSelection);
		GSelection = GetNextSelectedItemFromList(girllist_id, pos + 1, pos);
	}
	sort(girl_array->begin(), girl_array->end());
}

void cScreenDungeon::store_selected_girls()
{  // save list of multi-selected girls
	select_girls.clear();
	get_selected_girls(&select_girls);
	if (select_girls.empty()) return;

	// we're not really interested in customers here
	while (select_girls.back() >= dungeon->GetNumGirls())
	{
		select_girls.pop_back();
		if (select_girls.empty()) break;
	}
}
