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
#include "libintl.h"

extern cScreenGirlDetails g_GirlDetails;

extern sInterfaceIDs g_interfaceid;
extern	bool	g_InitWin;
extern	cRng	g_Dice;
extern	int		g_TalkCount;
extern	bool	g_Cheats;
extern	bool	g_UpArrow;
extern	bool	g_DownArrow;
extern	bool	eventrunning;
extern	bool	g_AltKeys;	// New hotkeys --PP
extern	bool	g_SpaceKey;
extern	bool	g_Q_Key;
extern	bool	g_W_Key;
extern	bool	g_E_Key;
extern	bool	g_A_Key;
extern	bool	g_S_Key;
extern	bool	g_D_Key;
extern	bool	g_Z_Key;
extern	bool	g_X_Key;
extern	bool	g_C_Key;
extern	int		g_CurrentScreen;

static cPlayer* player = g_Brothels.GetPlayer();
static cDungeon* dungeon = g_Brothels.GetDungeon();
static cTariff tariff;

static int lastNum = -1;
static int ImageNum = -1;

extern sGirl *selected_girl;
extern vector<int> cycle_girls;
extern int cycle_pos;

static vector<int> select_girls;

bool cScreenDungeon::ids_set = false;


void cScreenDungeon::set_ids()
{
	ids_set = true;
	back_id = get_id("BackButton");
	header_id = get_id("DungeonHeader");
	girllist_id = get_id("GirlList");
	girlimage_id = get_id("GirlImage");
	brandslave_id = get_id("BrandSlaveButton");
	release_id = get_id("ReleaseButton");
	allowfood_id = get_id("AllowFoodButton");
	torture_id = get_id("TortureButton");
	stopfood_id = get_id("StopFeedingButton");
	interact_id = get_id("InteractButton");
	releaseall_id = get_id("ReleaseAllButton");
	releasecust_id = get_id("ReleaseCustButton");
	viewdetails_id = get_id("DetailsButton");
	sellslave_id = get_id("SellButton");

	//Set the default sort order for columns, so listbox knows the order in which data will be sent
	SortColumns(girllist_id, m_ListBoxes[girllist_id]->m_ColumnName, m_ListBoxes[girllist_id]->m_ColumnCount);
}

void cScreenDungeon::init()
{
	Focused();
	g_CurrentScreen = SCREEN_DUNGEON;
	// clear the lists
	ClearListBox(girllist_id);
	cerr << "::init: Dungeon" << endl;	// `J`

	//get a list of all the column names, so we can find which data goes in that column
	vector<string> columnNames;
	m_ListBoxes[girllist_id]->GetColumnNames(columnNames);
	int numColumns = columnNames.size();

/*
 *	Display the text: let's use a stringstream for this
 */
	stringstream ss;
	ss << gettext("Your Dungeon where ") << dungeon->GetNumDied() << gettext(" people have died.");
	EditTextItem(ss.str(), header_id);
	// Fill the list box
	string* Data = new string[numColumns];
	for(int i = 0; i < dungeon->GetNumGirls(); i++)	// add girls
	{
/*
 *		get the i-th girl
 */
		sGirl *girl = dungeon->GetGirl(i)->m_Girl;
/*
 *		if selected_girl is this girl, update selection
 */
		if (selected_girl == girl)
			selection = i;
/*
 *		if she's low health or unhappy, flag her entry to display in red
 */
		int col = ((girl->health() <= 30) || (girl->happiness() <= 30)) ? COLOR_RED : COLOR_BLUE; // Anon21

/*
 *		add her to the list
 */
		dungeon->OutputGirlRow(i, Data, columnNames);
		AddToListBox(girllist_id, i, Data, numColumns, col);
	}
/*
 *	now add the customers
 */
	int offset = dungeon->GetNumGirls();
	for(int i=0; i<dungeon->GetNumCusts(); i++)	// add customers
	{
		int col = (dungeon->GetCust(i)->m_Health <= 30) ? COLOR_RED : COLOR_BLUE;
		dungeon->OutputCustRow(i, Data, columnNames);
		AddToListBox(girllist_id, i+offset, Data, numColumns, col);
	}
	delete [] Data;
/*
 *	disable some buttons
 */
	DisableButton(allowfood_id);
	DisableButton(stopfood_id);
	DisableButton(interact_id);
	DisableButton(release_id);
	DisableButton(brandslave_id);
	DisableButton(torture_id);
	DisableButton(sellslave_id);
//	cerr << "::init: disabling torture" << endl;	// `J` commented out
	DisableButton(viewdetails_id);
/*
 *	only enable "release all girls" if there are girls to release
 */
	DisableButton(releaseall_id, (dungeon->GetNumGirls() <= 0));
/*
 *	similarly...
 */
	DisableButton(releasecust_id, (dungeon->GetNumCusts() <= 0));

	g_InitWin = false;

/*
 *	if a selection of girls is stored, try to re-select them
 */
	if (!select_girls.empty())
	{
		selection = select_girls.back();
		for (int i=0; i < (int)select_girls.size(); i++)
		{
			SetSelectedItemInList(girllist_id, select_girls[i], (select_girls[i] == select_girls.back()), false);
		}
		select_girls.clear();
	}
	else if(selection >= 0) 
		SetSelectedItemInList(girllist_id, selection);
}

void cScreenDungeon::selection_change()
{
	selection = GetLastSelectedItemFromList(girllist_id);
/*
 *	if nothing is selected, then we just need to disable some buttons
 *	and we're done
 */
	if(selection == -1) {
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
/*
 *	otherwise, we need to enable some buttons...
 */
	DisableButton(sellslave_id);
	DisableButton(torture_id, !torture_possible());
//	cerr << "selection = " << selection << " - enabling torture" << endl;	// `J` commented out
	DisableButton(interact_id, g_TalkCount == 0);
	EnableButton(release_id);
	DisableButton(brandslave_id);
/*
 *	and then decide if this is a customer selected, or a girl
 *	customer is easiest, so we do that first
 */
	if((selection - dungeon->GetNumGirls()) >= 0) {
/*
 *		It's a customer! All we need to do is toggle some buttons
 */
		cerr << "Player selecting Dungeon Customer #" << selection << endl;	// `J` rewrote to reduce confusion
		DisableButton(viewdetails_id);
		EnableButton(allowfood_id);
		DisableButton(stopfood_id);
		return;
	}
/*
 *	Not a customer then. Must be a girl...
 */
	cerr << "Player selecting Dungeon Girl #" << selection << endl;	// `J` rewrote to reduce confusion
	int num = selection;
	sDungeonGirl* dgirl = dungeon->GetGirl(num);
	sGirl * girl = dgirl->m_Girl;
/*
 *	again, we're just enabling and disabling buttons
 */
	EnableButton(viewdetails_id);
	EnableButton(allowfood_id);
	EnableButton(stopfood_id);
/*
 *	some of them partly depend upon whether she's a slave or not
 */
	if(girl->is_slave())
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
/*
 *	nothing selected, nothing to do.
 */
	if(selection == -1) {
		return Continue;
	}
/*
 *	if this is a customer, we're not interested
 */
	if((selection - dungeon->GetNumGirls()) >= 0) {
		return Continue;
	} 
/*
 *	if we can't find the girl, there's nothing we can do
 */
	sGirl *girl = dungeon->GetGirl(selection)->m_Girl;
	if(!girl) {
		return Continue;
	}
	if(girl->health() > 0)
	{
		//load up the cycle_girls vector with the ordered list of girl IDs
		FillSortedIDList(girllist_id, &cycle_girls, &cycle_pos);
		// we don't want customers or dead girls in this list
		for(int i = cycle_girls.size(); i --> 0; )
		{
			if(cycle_girls[i] >= dungeon->GetNumGirls())
				cycle_girls.erase(cycle_girls.begin()+i);
			else if(dungeon->GetGirl(cycle_girls[i])->m_Girl->health() <= 0)
				cycle_girls.erase(cycle_girls.begin()+i);
		}
		g_CurrentScreen = SCREEN_GIRLDETAILS;
		g_InitWin = true;
		g_WinManager.push("Girl Details");
		return Return;
	}
/*
 *	can't ... resist ...
 */
	g_MessageQue.AddToQue(gettext("This is a dead girl. She has ceased to be."), COLOR_RED);
/*
 *	Furthermore, she's shuffled off this mortal coil and joined the bleeding
 *	choir invisible!
 */
	return Return;
}

int cScreenDungeon::enslave_customer(int girls_removed, int custs_removed)
{
/*
 *	mod - docclox - nerfed the cash for selling a customer.
 *	a fat smelly brothel creeper probably shouldn't raise as much as 
 *	a sexy young slavegirl. Feel free to un-nerf if you disagree.
 */
	long gold = (g_Dice%200)+63;
	g_Gold.slave_sales(gold);
/*
 *	format the message
 */
	stringstream ss;
	ss << "You force the customer into slavery lawfully for committing a crime against your business and sell them for " << gold << " gold.";
	g_MessageQue.AddToQue(ss.str(), 0);
/*
 *	get the index of the about-to-be-sold customer
 */
	int customer_index = selection - dungeon->GetNumGirls();
	customer_index += girls_removed;
	customer_index -= custs_removed;
/*
 *	get the customer record
 */
	sDungeonCust* cust = dungeon->GetCust(customer_index);
/*
 *	remove the customer from the dungeon
 *	room for an overload here
 */
	dungeon->RemoveCust(cust);
	return 0;
}

void cScreenDungeon::set_slave_stats(sGirl *girl)
{
	cConfig cfg;
	girl->set_slave();
	girl->obedience(-10);
	girl->pcfear(5);
	girl->pclove(-10);
	girl->pchate(5);
	girl->m_Stats[STAT_HOUSE] = cfg.initial.slave_house_perc();
	girl->m_AccLevel = 0;
}

int cScreenDungeon::enslave()
{
	string message = "";
	int numCustsRemoved = 0;
	int numGirlsRemoved = 0;
	int pos = 0, deadcount = 0;
	cPlayer* player = g_Brothels.GetPlayer();

	store_selected_girls();
/*
 *	roll on vectors!
 */
	for(
		selection = GetNextSelectedItemFromList(girllist_id, 0, pos);
		selection != -1;
		selection = GetNextSelectedItemFromList(girllist_id, pos+1, pos)
	) {
		message = "";
		if((selection - (dungeon->GetNumGirls()+numGirlsRemoved)) >= 0)	// it is a customer
		{
			enslave_customer(numGirlsRemoved, numCustsRemoved);
			numCustsRemoved ++;
			continue;
		}
/*
 *		it is a girl
 */
		sDungeonGirl* dgirl = dungeon->GetGirl(
			selection - numGirlsRemoved
		);
		sGirl *girl = dgirl->m_Girl;
/*
 *		nothing to do if she's _already_ enslaved
 */
		if(girl->is_slave()) {
			continue;
		}
/*
 *		likewise, dead girls can't be enslaved
 */
		if(girl->health() <= 0)
		{
			deadcount++;
			continue;
		}
/*
 *		This is much simpler if she just submits...
 */

		cGirlGangFight ggf(girl);

		if(ggf.girl_submits()) {
			message += girl->m_Realname;
			message += gettext(" submits the the enchanted slave tattoo being placed upon her.");
			set_slave_stats(girl);
			g_MessageQue.AddToQue(message, 0);
			continue;
		}

/*
 *		did the player need to step in
 */
		if(!ggf.player_won()) {
/*
 *			adjust the girl's stats to reflect her new status
 *			and then evil up the player because he forced her
 *			into slavery
 */
			player->evil(5);
			set_slave_stats(girl);
			message += girl->m_Realname;
			message += gettext(" breaks free from your goons' control. You restrain her personally while the slave tattoo placed upon her.");
			g_MessageQue.AddToQue(message, COLOR_RED);
			continue;
			}

		if (ggf.girl_lost())
		{
			message += girl->m_Realname;
			message += gettext(" puts up a fight ");	// there was a gang, and some of them are still with us
			message += gettext("but your goons control her as the enchanted slave tattoo is placed upon her.");
			player->evil(5);				// evil up the player for doing a naughty thing and adjust the girl's stats
			set_slave_stats(girl);
			g_MessageQue.AddToQue(message, COLOR_RED);	// and queue the message
			continue;
		}
/*
 *		we just did the "lost" case
 *		this is the girl wins case
 */
		message += girl->m_Realname;
		message += " puts up a fight and ";
/*
 *		if there is a gang, but it has no members
 */
		if(ggf.wipeout()) {
			message += " the gang is wiped out and";
		}
/*
 *		If girl wins she escapes and leaves the brothel
 */
		message += "And after defeating you as well she escapes to the outside world.\n";
		dungeon->RemoveGirl(girl);
		numGirlsRemoved++;
		girl->run_away();
/*
 *		suspicion goes up, so does evil
 */
		player->suspicion(15);
		player->evil(15);
/*
 *		add to the message queue
 */
		g_MessageQue.AddToQue(message, 1);

	}

	if(deadcount > 0)
		g_MessageQue.AddToQue("There's not much point in using a slave tattoo on a dead body.", 0);

	g_InitWin = true;
	return Return;
}

void cScreenDungeon::release_all_customers()
{
	cPlayer* player = g_Brothels.GetPlayer();
/*
 *	loop until we run out of customers
 */
	while(dungeon->GetNumCusts() > 0) {
/*
 *		get the first customer in the list
 */
		sDungeonCust* cust = dungeon->GetCust(0);
/*
 *		remove from brothel
 */
		dungeon->RemoveCust(cust);
/*
 *		de-evil the player for being nice
 *		suspicion drops too
 */
		player->evil(-5);
		player->suspicion(-5);
	}
	selection = -1;
	g_InitWin = true;
}

void cScreenDungeon::sell_slaves()
{
	int paid = 0, count = 0, deadcount = 0;
	vector<int> girl_array;
	get_selected_girls(&girl_array);  // get and sort array of girls/customers

	for(int i = girl_array.size(); i-- > 0; )
	{
		selection = girl_array[i];
/*
 *		if this is a customer, we skip to the next list entry
 */
		if((selection - dungeon->GetNumGirls()) >= 0)
			continue;
/*
 *		get the index of the girl,
 *		get the DungeonGirl entry
 *		and get the sGirl
 */
		sDungeonGirl* dgirl = dungeon->GetGirl(selection);
		sGirl *girl = dgirl->m_Girl;
/*
 *		if she's not a slave, the player isn't allowed to sell her
 */
		if(girl->is_slave() == false)
			continue;
/*
 *		likewise, dead slaves can't be sold
 */
		if(girl->health() <= 0)
		{
			deadcount++;
			continue;
		}
/* 
 *		she's a living slave, she's out of here
 */
		g_Girls.CalculateAskPrice(girl, false);
/*
 *		get the sell price of the girl. This is a little on the
 *		occult side
 */
		int cost = tariff.slave_sell_price(girl);
		g_Gold.slave_sales(cost);
		paid += cost;
		count++;
/*
 *		remove her from the dungeon, add her back into the general pool
 */
		girl = dungeon->RemoveGirl(dungeon->GetGirl(selection));
		if (girl->m_Realname.compare(girl->m_Name) == 0)
		{
			g_Girls.AddGirl(girl);  // add unique girls back to main pool
		}
		else
		{  // random girls simply get removed from the game
//			delete girl;
			girl = 0;
		}
		g_Brothels.RemoveGirl(g_CurrBrothel, girl, true);	// `J` added to fix "dungeon screen to brothel screen crash"
	}

	if(deadcount > 0)
		g_MessageQue.AddToQue("Nobody is currently in the market for dead girls.", 0);

	if(count <= 0)
		return;

	stringstream ss;
	ss.str("");
	ss << "You sold ";
	if(count > 1)
		ss << count << " slave girls";
	else
		ss << "the slave girl";
	ss << " for " << paid << " gold.";
	g_MessageQue.AddToQue(ss.str(), 0);
	selection = -1;
	g_InitWin = true;
}

void cScreenDungeon::release_all_girls()
{
	sBrothel *brothel = g_Brothels.GetBrothel(g_CurrBrothel);
/*
 *	Is there room?
 */
	if(brothel->free_rooms() <= 0) {
		g_MessageQue.AddToQue(
			"The current brothel has no more room.\n"
			"Buy a new one, get rid of some girls, or "
			"change the brothel you are currently "
			"managing.", 0
		);
		return;
	}
/*
 *	loop until there are no more girls to release
 */
 	while(dungeon->GetNumGirls() > 0) {
/*
 *		make sure there's room for another girl
 */
		if(brothel->free_rooms() > 0) {
			sGirl* girl = dungeon->RemoveGirl(dungeon->GetGirl(0));
			g_Brothels.AddGirl(g_CurrBrothel, girl);
			continue;
		}
/*
 *		we only get here if we run out of space
 */
		g_MessageQue.AddToQue(
			"There is no more room in the current brothel.\n"
			"Buy a new one, get rid of some girls, or "
			"change the brothel you are currently "
			"managing.", 0
		);
		break;
	}
}

void cScreenDungeon::stop_feeding()
{
/*
 *	and then loop using multi_first() and multi_next()
 */
	for(int	selection = multi_first();
		selection != -1;
		selection = multi_next()
	) {
		int num_girls = dungeon->GetNumGirls();
/*
 *		if the selection is more than than the number of girls
 *		it has to be a customer
 */
		if((selection - num_girls) >= 0)	// it is a customer
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
	while(selection != -1)
	{
		if((selection - dungeon->GetNumGirls()) >= 0)	// it is a customer
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
		selection = GetNextSelectedItemFromList(girllist_id, pos+1, pos);
	}
}

void cScreenDungeon::torture_customer(int girls_removed)
{
	string message = "Customer: ";
/*
 *	get the index number for the customer
 */
	int cust_index = selection - dungeon->GetNumGirls() + girls_removed;
/*
 *	get the customer record from the dungeon
 */
	sDungeonCust* cust = dungeon->GetCust(cust_index);

	if (!cust)
		return;
/*
 *	don't let the PL torture more than once a da7
 *	(unless cheating is enabled)
 */
	if(cust->m_Tort && !g_Cheats) {
		message += "You may only torture someone once per week.";
		g_MessageQue.AddToQue(message, 0);
		return;
	}
/*
 *	flag the customer as tortured, decrement his health
 */
	cust->m_Tort = true;
	cust->m_Health -= 6;
/*
 *	if he's in good health, queue a message and depart
 */
	if(cust->m_Health >= 30) {
		message += "Screams fill the dungeon until the "
			   "customer is battered, bleeding and "
			   "bruised.\n"
			   "You leave them sobbing uncontrollably."
		;
		g_MessageQue.AddToQue(message, 0);
		return;
	}

	if(cust->m_Health > 0) {
		message += "Screams fill the dungeon until the "
			   "customer is battered, bleeding and "
			   "bruised.\n"
			   "You leave them sobbing and near to "
			   "death."
		;
		g_MessageQue.AddToQue(message, 0);
		return;
	}

	cust->m_Health = 0;
	message += "They are dead.";
	g_MessageQue.AddToQue(message, 0);
}

#if 0	// WD	Replaced by Doclox's cGirlTorture and cGirlGangFight code.
bool cScreenDungeon::girl_fight_torture(sGirl *girl, string &message, bool &fight)
{
	cGirlGangFight ggf(girl);

	if(ggf.girl_submits()) {
		fight = false;
		message += "She doesn't put up a fight so ";
		return GirlLoses;
	}
	fight = true;

	if(ggf.player_won()) {
		message +=
			"She puts up a fight. Only your personal "
			"intervention prevents her escape."
		;
		return GirlLoses;
	}

	if(ggf.girl_lost()) {
		message += "Your goons defeat her and ";
		return GirlLoses;
	}
	message += "She puts up a fight and ";

	if(ggf.wipeout()) {
		message += " your men are wiped out";
	}
/*
 *	If girl wins she escapes and leaves the brothel
 */
	message += "and after defeating you as well "
		   "she escapes to the outside world.\n"
	;
/*
 *	Add her to the "escaped" list. I want a word with her...
 */
	dungeon->RemoveGirl(girl);
	girl->run_away();
/*
 *	Suss goes up: makes sense, she escapes to tell terrible
 *	stories about the Whoremaster's dungeons.
 *
 *	Original code has evil dropping. I don't see that one:
 *	it's not like the player released her.
 *
 *	On the other hand, he probably doesn't get any more
 *	evil added than he got for trying to totrture her
 *	in the first place
 */
	player->suspicion(15);
	g_MessageQue.AddToQue(message, 0);
	return GirlWins;
}
#endif

/*
 * If we have a multiple selection, then the torture button
 * should be enabled if just one of the selected rows can
 * be tortured
 */
bool cScreenDungeon::torture_possible()
{
	int nSelection;
	int	nPosition	= 0;
	int nNumGirls	= dungeon->GetNumGirls();
/*
 *	don't use selection for the loop - 
 *	it's a class global and can mess things up elsewhere
 */
	for(
		nSelection = GetNextSelectedItemFromList(girllist_id, 0, nPosition);
		nSelection != -1;
		nSelection = GetNextSelectedItemFromList(girllist_id, nPosition+1, nPosition)
	) {
		bool not_yet_tortured;
/*
 *		get the customer or girl under selection and
 *		find out if they've been tortured this turn
 */
		if(nSelection >= nNumGirls) {
			sDungeonCust* dcust = dungeon->GetCust(nSelection - nNumGirls);
			not_yet_tortured = !dcust->m_Tort;
		}
		else {
			sDungeonGirl* dgirl = dungeon->GetGirl(nSelection);
			not_yet_tortured = !dgirl->m_Girl->m_Tort;
		}
/*
 *		we only need one torturable prisoner
 *		so if we found one, we can go home
 */
		if(not_yet_tortured) {
			return true;
		}
	}
/*
 *	we only get here if no-one in the list was
 *	torturable
 */
	return false;
}

void cScreenDungeon::torture()
{
	int pos = 0;
	int numGirlsRemoved = 0;

	store_selected_girls();

	for(
		selection = GetNextSelectedItemFromList(girllist_id, 0, pos);
		selection != -1;
		selection = GetNextSelectedItemFromList(girllist_id, pos+1, pos)
	) {

/*
 *		if it's a customer, we have a separate routine 
 */
		if((selection - (dungeon->GetNumGirls()+numGirlsRemoved)) >= 0)	{
			player->evil(5);
			torture_customer(numGirlsRemoved);
			continue;
		}
/*
 *		If we get here, it's a girl
 */
		sDungeonGirl* dgirl = dungeon->GetGirl(selection-numGirlsRemoved);
//		cGirlTorture gt(dgirl->m_Girl);
		cGirlTorture gt(dgirl);
	}
}

void cScreenDungeon::release()
{
	cPlayer* player = g_Brothels.GetPlayer();
	sBrothel *brothel = g_Brothels.GetBrothel(g_CurrBrothel);

	vector<int> girl_array;
	get_selected_girls(&girl_array);  // get and sort array of girls/customers

	for(int i = girl_array.size(); i --> 0; )
	{
		selection = girl_array[i];
/*
 *		check in case its a customer
 */
		if((selection - dungeon->GetNumGirls()) >= 0)
		{
			int num = selection - dungeon->GetNumGirls();
			sDungeonCust* cust = dungeon->GetCust(num);
			dungeon->RemoveCust(cust);
/*
 *			player did a nice thing: suss and evil go down :)
 */
			player->suspicion(-5);
			player->evil(-5);
			continue;
		}
/* 
 *		if there's room, remove her from the dungeon
 *		and add her to the current brothel
 */
		int num = selection;
		if((brothel->m_NumRooms - brothel->m_NumGirls) > 0)
		{
			sGirl* girl = dungeon->RemoveGirl(dungeon->GetGirl(num));
			g_Brothels.AddGirl(g_CurrBrothel, girl);
			continue;
		}
/*
 *		if we run out of space
 */
		g_MessageQue.AddToQue(
			"The current brothel has no more room.\n"
			"Buy a new one, get rid of some girls, or "
			"change the brothel you are currently managing.", 0
		);
/*
 *		no point in running round the loop any further
 *		we're out of space
 */
		break;
	}
}

void cScreenDungeon::talk()
{
/*
 *	if we have no talks left, we can go home
 */
	if(g_TalkCount <= 0)
	{
		return;
	}
	int v[2] = {0, -1};
/*
 *	customers are always last in the list,
 *	so we can determine if this is a customer by simple 
 *	aritmetic
 */
	if((selection - g_Brothels.GetDungeon()->GetNumGirls()) >= 0)	// it is a customer
	{
		return;
	}
/*
 *	OK, it wasn't a customer
 */
	int num = selection;
	sDungeonGirl* girl = g_Brothels.GetDungeon()->GetGirl(num);
/*
 *	is she dead? that would make life simpler.
 *
 *	(actually, I'd like to be able to view her stats in read-only mode
 *	after she dies, just so I can do a post-mortem. But for now...)
 */
	if(g_Girls.GetStat(girl->m_Girl, STAT_HEALTH) <= 0) {
		g_MessageQue.AddToQue("Though you have many skills, speaking to the dead is not one of them.", 1);
		return;
	}
/*
 *	she's still alive. I guess we'll have to talk to her
 */
	cTrigger* trig = 0;
/*
 *	is there a girl specific script for this interaction?
 */
	DirPath dp;
	eventrunning = true;
	if(!(trig = girl->m_Girl->m_Triggers.CheckForScript(TRIGGER_TALK, false, v)))
	{
		// no, so trigger the default one
		dp = dp << "Resources" << "Scripts" << "DefaultInteractDungeon.script";
	}
	else
	{
		// yes, so trigger the custom one
		dp = dp	<< "Resources"
			<< "Characters"
			<< girl->m_Girl->m_Name
			<< trig->m_Script
			<< "DefaultInteractDungeon.script"
		;
	}
	cScriptManager script_manager;
	script_manager.Load(dp, girl->m_Girl);
	if(!g_Cheats) g_TalkCount--;
}
void cScreenDungeon::update_image()
{
	if((selected_girl) && !IsMultiSelected(girllist_id))
	{
		bool Rand = true;
		if (selected_girl->m_Tort)
		{
			SetImage(girlimage_id, g_Girls.GetImageSurface(selected_girl, IMGTYPE_TORTURE, Rand, ImageNum));

			if (g_Girls.IsAnimatedSurface(selected_girl, IMGTYPE_TORTURE, ImageNum))
				SetImage(girlimage_id, g_Girls.GetAnimatedSurface(selected_girl, IMGTYPE_TORTURE, ImageNum));
		}
		else
		{
			SetImage(girlimage_id, g_Girls.GetImageSurface(selected_girl, IMGTYPE_PROFILE, Rand, ImageNum));

			if (g_Girls.IsAnimatedSurface(selected_girl, IMGTYPE_PROFILE, ImageNum))
				SetImage(girlimage_id, g_Girls.GetAnimatedSurface(selected_girl, IMGTYPE_PROFILE, ImageNum));
		}

		HideImage(girlimage_id, false); 
	}
	else
	{		
		HideImage(girlimage_id, true); 
	}
}

int cScreenDungeon::process_events()
{
	if(g_InterfaceEvents.GetNumEvents() == 0) {
		return Continue;
	}
	// if it's the back button, pop the window off the stack and we're done
	if(g_InterfaceEvents.CheckButton(back_id)) {
		g_InitWin = true;
		g_WinManager.Pop();
		return Return;
	}
	if(g_InterfaceEvents.CheckListbox(girllist_id)) {
		// If double-clicked, try to bring up girl details
		if(ListDoubleClicked(girllist_id))
		{
			g_GirlDetails.lastsexact = -1;
			return view_girl();
		}

		selection_change();
		// disable buttons based on multiselection
		if(IsMultiSelected(girllist_id)) {
			DisableButton(interact_id, true);
			DisableButton(viewdetails_id, true);
		}
		update_image();
		return Return;
	}
	if(g_InterfaceEvents.CheckButton(viewdetails_id))
	{
		g_GirlDetails.lastsexact = -1;
		return view_girl();
	}
	if(g_InterfaceEvents.CheckButton(brandslave_id))
	{
		return enslave();
	}
	if(g_InterfaceEvents.CheckButton(releasecust_id))
	{
		release_all_customers();
		return Return;
	}
	if(g_InterfaceEvents.CheckButton(sellslave_id))
	{
		sell_slaves();
		return Return;
	}
	if(g_InterfaceEvents.CheckButton(releaseall_id))
	{
		release_all_girls();
		selection = -1;
		g_InitWin = true;
		return Return;
	}
	if(g_InterfaceEvents.CheckButton(allowfood_id))
	{
		stop_feeding();
		selection = -1;
		g_InitWin = true;
		return Return;
	}
	if(g_InterfaceEvents.CheckButton(stopfood_id))
	{
		start_feeding();
		selection = -1;
		g_InitWin = true;
		return Return;
	}
	if(g_InterfaceEvents.CheckButton(interact_id))
	{
		if(selection == -1) return Return;
		talk();
		g_InitWin = true;
		return Return;
	}
	if(g_InterfaceEvents.CheckButton(torture_id))
	{
		torture();
		selection = -1;
		g_InitWin = true;
		return Return;
	}
	if(g_InterfaceEvents.CheckButton(release_id))
	{
		release();
		selection = -1;
		g_InitWin = true;
		return Return;
	}
	return Continue;
}

void cScreenDungeon::process()
{
	static int selection = -1;

	// we need to make sure the ID variables are set
	if(!ids_set)
		set_ids();

	if(g_InitWin) {
		init();
	}

	if(process_events() == Return) {
		return;
	}

	if(g_UpArrow)
	{
		selection = ArrowUpListBox(girllist_id);
		g_UpArrow = false;
		return;
	}
	else if(g_DownArrow)
	{
		selection = ArrowDownListBox(girllist_id);
		g_DownArrow = false;
		return;
	}
	if(g_AltKeys)
	{
	if(g_A_Key)
	{
		selection = ArrowUpListBox(girllist_id);
		g_A_Key = false;
		return;
	}
	else if(g_D_Key)
	{
		selection = ArrowDownListBox(girllist_id);
		g_D_Key = false;
		return;
	}
	}
}

void cScreenDungeon::get_selected_girls(vector<int> *girl_array)
{  // take passed vector and fill it with sorted list of girl/customer IDs
	int pos = 0;
	int GSelection = GetNextSelectedItemFromList(girllist_id, 0, pos);
	while(GSelection != -1)
	{
		girl_array->push_back(GSelection);
		GSelection = GetNextSelectedItemFromList(girllist_id, pos+1, pos);
	}
	sort(girl_array->begin(), girl_array->end());
}

void cScreenDungeon::store_selected_girls()
{  // save list of multi-selected girls
	select_girls.clear();

	get_selected_girls(&select_girls);

	if(select_girls.empty())
		return;

	// we're not really interested in customers here
	while(select_girls.back() >= dungeon->GetNumGirls())
	{
		select_girls.pop_back();
		if(select_girls.empty())
			break;
	}
}
