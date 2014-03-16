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
#include "cScreenSlaveMarket.h"
#include "cWindowManager.h"
#include "cGold.h"
#include "sFacilityList.h"
#include "cGetStringScreenManager.h"
#include "libintl.h"

extern	bool			g_InitWin;
extern	int			g_CurrBrothel;
extern	cGold			g_Gold;
extern	cBrothelManager		g_Brothels;
extern	cWindowManager		g_WinManager;
extern	cInterfaceEventManager	g_InterfaceEvents;
extern sGirl* MarketSlaveGirls[8];
extern int MarketSlaveGirlsDel[8];
extern bool g_GenGirls;
extern bool g_Cheats;
extern cTraits g_Traits;

extern	bool	g_LeftArrow;
extern	bool	g_RightArrow;
extern	bool	g_UpArrow;
extern	bool	g_DownArrow;
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

bool cScreenSlaveMarket::ids_set = false;

void cScreenSlaveMarket::set_ids()
{
	ids_set = true;
	back_id	= get_id("BackButton");
	more_id = get_id("ShowMoreButton");
	buy_slave_id = get_id("BuySlaveButton");
	cur_brothel_id = get_id("CurrentBrothel");
	slave_list_id = get_id("SlaveList");
	trait_list_id = get_id("TraitList");
	details_id = get_id("SlaveDetails");
	trait_id = get_id("TraitDesc");
	slave_image_id = get_id("SlaveImage");
	header_id = get_id("ScreenHeader");
}

void cScreenSlaveMarket::init()
{
	g_CurrentScreen = SCREEN_SLAVEMARKET;
	if(!g_InitWin) {
		return;
	}
	Focused();
	g_InitWin = false;

/*
 *	buttons enable/disable
 */
	DisableButton(more_id, true);
	DisableButton(buy_slave_id, true);
	selection=-1;

	g_LogFile.os() << "setting up slave market: genGirls = " << g_GenGirls << endl;

	ImageNum = -1;
	string brothel = gettext("Current Brothel: ");
	brothel += g_Brothels.GetName(g_CurrBrothel);
	EditTextItem(brothel, cur_brothel_id);

	// clear the list
	ClearListBox(slave_list_id);

	// Check if any slave girls
	for(int i=0; i<8; i++)
	{
		bool unique = false;
/*
 *		easiest case is if the girs have already been generated
 */
		if(g_GenGirls != false) {
/*
 *			first of all, if there isn't a girl in this slot
 *			the rest doesn't matter much
 */
			if(MarketSlaveGirls[i] == 0) {
				continue;
			}
/*
 *			I'm going to assume that -1 here means
 *			"OK to delete". Which means non -1 means
 *			"do not delete", from which we can infer
 *			that the girl is unique.
 *
 *			which means we colour the slot red
 */
			int col = LISTBOX_BLUE;
			if(MarketSlaveGirlsDel[i] != -1) {
				col = LISTBOX_RED;
				unique = true;
			}

			AddToListBox(slave_list_id, i, MarketSlaveGirls[i]->m_Realname, col);
			continue;
		}

/*
 *		So at this point, g_GenGirls is true
 *		which means we need to regenerate the slave market
 *
 *		I think I'd be happier with more of a slow, incremental
 *		change rather than a full regen each week.
 *		Still, that's for the future.
 *	
 *		For now: is there a girl in the current slot?
 */
		if(MarketSlaveGirls[i] != 0) {
/*
 *			Yes there is. Is it OK to delete her.
 *			If so, do it
 */
			if(MarketSlaveGirlsDel[i] == -1) {
				delete MarketSlaveGirls[i];	// doing it
			}
/*
 *			in any case, mark the slot as empty
 */
			MarketSlaveGirls[i] = 0;
		}

/*
 *		now try and generate a unique girl for the slot
 */
		generate_unique_girl(i, unique);
/*
 *		if the id for this slot is zero,
 *		then we didn't make a unique girl
 *
 *		so we need a random one
 */
		if(MarketSlaveGirls[i] == 0) {
			MarketSlaveGirls[i] = g_Girls.CreateRandomGirl(0, false, "", true);
			MarketSlaveGirlsDel[i] = -1;
		}
/*
 *		if the slot is still zero, we couldn't get a girl at all
 *		(can this happen? it probably should...)
 *
 *		in which case there's nothing more for this time round.
 *		(and arguably for the loop...)
 */
		if(MarketSlaveGirls[i] == 0) {
			continue;
		}
/*
 *		decide the display color based on whether the girl is unique
 */
		int col = unique ? LISTBOX_RED : LISTBOX_BLUE;
/*
 *		and display
 */
		AddToListBox(slave_list_id, i, MarketSlaveGirls[i]->m_Realname, col);
	}

	selection = 0;

/*
 *	if we're cheating, set the list as OK to be regenerated
 *	when we next display the screen.
 *
 *	if we're not, flag the market as regenerated for this turn
 */
	if(g_Cheats)
		g_GenGirls = false;
	else
		g_GenGirls = true;

	string message = gettext("Slave Market, ");
	message += g_Gold.sval();
	message += gettext(" gold");
	EditTextItem(message, header_id);

/*				*** Disabled next section, this was not doing anything, since selection has nothing to do with the size of the list,
 *				*** and the selection always defaulted to zero as soon as you made a purchase.. then would error out if you purchased
 *				*** the first girl in list, causing no girl to be selected. --PP
 *	if the selection is off the end of the now possible shorter list
 *	whizz back up until we hit data
 *
 *	(can we not just set it to the last entry? We know the size...)
 
	if(selection >= 0)
	{
		while(selection > GetListBoxSize(slave_list_id) && selection != -1)
			selection--;
	}
*/ 

	
	// Finds the first girl in the selection, so she is highlighted. This stops the No girl selected that was normal before. --PP
	for (int i=7; i>-1; i--)
	{
		if (MarketSlaveGirls[i] == 0)
			continue;
		selection = i;
	}

/*
 *	if there is still as selection (a non empty slave list)
 *	then highlight the current selection
 */
	if(selection >= 0) {
		SetSelectedItemInList(slave_list_id, selection, true);
	}
/*
 *	now we need to populate the trait box
 */
	ClearListBox(trait_list_id);
	int tmp = GetLastSelectedItemFromList(slave_list_id);
/*
 *	if the last item was -1 I assume the list is empty
 *	so we can go home early (and probably should have earlier still)
 */
	if(tmp == -1) {
		return;
	}
/*
 *	get the girl under the cursor. If she's null,
 *	something funny is going on, so splurge a 
 *	diagnostic before exiting the func
 */
	sGirl* g = MarketSlaveGirls[tmp];
	if(g == 0) {
		g_LogFile.os() << "error: null pointer for cursor entry in market" << endl;
		return;
	}
/*
 *	loop through her traits, populating the box
 */
	for(int i=0; i<30; i++)
	{
		if(!g->m_Traits[i]) {
			continue;
		}
		AddToListBox(trait_list_id, i, g_Traits.GetTranslateName(g->m_Traits[i]->m_Name));
	}
/*
 *	and finally, highlight the selected entry?
 */
	SetSelectedItemInList(trait_list_id, 0);
}

bool cScreenSlaveMarket::check_keys()
{
	if(g_UpArrow) {
		selection = ArrowUpListBox(slave_list_id);
		g_UpArrow = false;
		//g_InitWin = true;
		g_GenGirls = true;
		//SetSelectedItemInList(slave_list_id, selection);
		return true;
	}
	if(g_DownArrow) {
		selection = ArrowDownListBox(slave_list_id);
		g_DownArrow = false;
		//g_InitWin = true;
		g_GenGirls = true;
		//SetSelectedItemInList(slave_list_id, selection);
		return true;
	}
	if(g_AltKeys)
	{
	if(g_A_Key) {
		selection = ArrowUpListBox(slave_list_id);
		g_A_Key = false;
		//g_InitWin = true;
		g_GenGirls = true;
		//SetSelectedItemInList(slave_list_id, selection);
		return true;
	}
	if(g_D_Key) {
		selection = ArrowDownListBox(slave_list_id);
		g_D_Key = false;
		//g_InitWin = true;
		g_GenGirls = true;
		//SetSelectedItemInList(slave_list_id, selection);
		return true;
	}
	if(g_S_Key) {
		sGirl *girl = MarketSlaveGirls[selection];
		if(DetailLevel == 0)
		{
			DetailLevel = 1;
			EditTextItem(g_Girls.GetMoreDetailsString(girl), details_id);
		}
		else
		{
			DetailLevel = 0;
			EditTextItem(g_Girls.GetDetailsString(girl,true), details_id);
		}
		return true;
	}
	}
	if(g_SpaceKey)
	{
	cTariff tariff;
	sGirl *girl = MarketSlaveGirls[selection];
	g_SpaceKey = false;
		for(selection = multi_slave_first();
			selection != -1;
			selection = multi_slave_next()
		){
			girl = MarketSlaveGirls[selection];
			int cost = tariff.slave_buy_price(girl);
			cerr << "Selection = " << selection 
			     << ", girl = " << girl->m_Realname
			     << endl
			;
/*
 *			can the player afford this particular playmate?
 */
			if(g_Gold.slave_cost(cost) == false) {
				string text = gettext("You don't have enough money to purchase ");
				text += girl->m_Realname;
				g_MessageQue.AddToQue(text, 0);
				break;
			}

			sBrothel* brothel = g_Brothels.GetBrothel(g_CurrBrothel);
			if(g_Girls.GetRebelValue(girl, false) >= 35)
			{
				string text = girl->m_Realname;
				text += gettext(" has been sent to your dungeon, as she is rebellious and poorly trained.");
				g_MessageQue.AddToQue(text, 0);
				g_Brothels.GetDungeon()->AddGirl(girl, DUNGEON_NEWSLAVE);
			}
			else if((brothel->m_NumRooms - brothel->m_NumGirls) == 0)
			{
				string text = girl->m_Realname;
				text += gettext(" has been sent to your dungeon, since your current brothel is full.");
				g_MessageQue.AddToQue(text, 0);
				g_Brothels.GetDungeon()->AddGirl(girl, DUNGEON_NEWSLAVE);
			}
			else
			{
				string text = girl->m_Realname;
				text += gettext(" has been sent to your current brothel.");
				g_MessageQue.AddToQue(text, 0);
				g_Brothels.AddGirl(g_CurrBrothel, girl);
			}
			EditTextItem("", details_id);
			MarketSlaveGirls[selection] = 0;
		}
		selection = -1;
		g_InitWin = true;
		return true;
	}
	return false;
}

void cScreenSlaveMarket::process()
{
/*
 *	we need to make sure the ID variables are set
 */
	if(!ids_set) {
		set_ids();
	}
/*
 *	handle arrow keys
 */
	//g_LogFile.os() << "... checking arrows" << endl;
 	if(check_keys())
		return;

/*
 *	set up the window if needed
 */
	init();

	//g_LogFile.os() << "Slave Market called" << endl;

/*
 *	check to see if there's a button event needing handling
 */
	//g_LogFile.os() << "... checking buttons" << endl;
	check_events();

/*
 *	hide/show image based on whether a girl is selected
 */
	HideImage(slave_image_id, (selection == -1));
/*
 *	if no girl is selected, clear girl info
 */
	if (selection == -1)
	{
		EditTextItem(gettext("No girl selected"), details_id);
		EditTextItem("", trait_id);
	}

/*
 *	nothing selected == nothing further to do
 */
	int index = selected_item();
	if(index == -1) {
		return;
	}
	sGirl *girl;
	girl = MarketSlaveGirls[index];
	if(!girl) {
		g_LogFile.os() << "... no girl at index "
		               << index
			       << "- returning "
			       << endl
		;
		return;
	}
}

void cScreenSlaveMarket::generate_unique_girl(int i, bool &unique)
{
	cConfig cfg;
/*
 *	if there are no unique slave girls left (?)
 *	then we can do no more here
 */
	if(g_Girls.GetNumSlaveGirls() <= 0) {
		return;
	}
/*
 *	otherwise - 20% chance of a unique girl.
 */
	if(1 + (g_Dice % 100) >= 35) {
		return;
	}
/*
 *	randomly select a slavegirl from the list
 */
	int g = g_Dice%g_Girls.GetNumSlaveGirls();
/*
 *	try and get a struct for the girl in question
 */
 	sGirl *gpt = g_Girls.GetGirl(g_Girls.GetSlaveGirl(g));
/*
 *	if we can't, we go home
 */
	if(!gpt) {
		return;
	}
/*
 *	whizz down the list of girls we have already
 *	and see if the new girl is already in the list
 *
 *	if she is, we need do nothing more
 */
	for(int j=0; j<8; j++) {
		if(MarketSlaveGirls[j] == gpt) {
			return;
		}
	}
	gpt->m_Stats[STAT_HOUSE]=cfg.initial.slave_house_perc();
/*
 *	add the girl to the slave market girls
 *	and mark her as not-to-be-deleted
 */
	MarketSlaveGirls[i] = gpt;
	MarketSlaveGirlsDel[i] = g;
	unique = true;
}

bool cScreenSlaveMarket::change_selected_girl()
{
	ImageNum=-1;
/*
 *	Since this is a multiselect box, GetLastSelectedItemFromList
 *	returns the last clicked list item, even if it's deselected.
 *	So, we'll check for that and show first truly selected item
 *	if the last clicked one is actually deselected.
 */
	selection = GetSelectedItemFromList(slave_list_id);
	bool MatchSel = false;
	int i;

	for(i = multi_slave_first();
		i != -1;
		i = multi_slave_next()
	){
		if (i == selection)
		{
			MatchSel = true;
			break;
		}
	}
	if (!MatchSel)
		selection = multi_slave_first();

/*
 *	if the player selected an empty slot
 *	make that into "nothing selected" and return
 */
	if(MarketSlaveGirls[selection] == 0)
		selection = -1;

/*
 *	disable/enable buttons based on whether a girl is selected
 */
	DisableButton(more_id, (selection == -1));
	DisableButton(buy_slave_id, (selection == -1));

	ClearListBox(trait_list_id);
/*
 *	selection of -1 means nothing selected
 *	so we get to go home early
 */
	if(selection == -1)
		return true;
/*
 *	otherwise, we have (potentially) a new girl:
 *	set the global girl pointer
 *
 *	if we can't find the pointer. log an error and go home
 */
 	sGirl *girl = MarketSlaveGirls[selection];
	if(!girl) {
		g_LogFile.ss()
			<< "Warning: cScreenSlaveMarket::change_selected_girl"
		        << "can't find girl data for selection"
		;
		g_LogFile.ssend();
		return true;
	}
	string detail;

	if(DetailLevel == 0)
		detail = g_Girls.GetDetailsString(girl,true);
	else
		detail = g_Girls.GetMoreDetailsString(girl);
	EditTextItem(detail, details_id);
/*
 *	I don't understand where this is used...
 */
	ImageNum = -1;
/*
 *	I don't see the point of this next bit. "selection" already
 *	holds this value. We haven't changed it, haven't
 *	passed it to anything the might have changed it by reference
 *	haven't even called any class methods that might
 *	have altered it.
 *
 *	commenting it out, just in case
 *
	int tmp = g_SlaveMarket.GetLastSelectedItemFromList(g_interfaceid.LIST_SLGIRLS);
	if(tmp == -1) {
		return true;
	}
 */
 /*
  *	whizz down the girl's trait list
  */
	for(int i=0; i<30; i++) {
		sTrait *trait = girl->m_Traits[i];
/*
 *		skip any that are absent
 */
		if(!trait) {
			continue;
		}
/*
 *		mention the rest in the trait listbox
 */
		AddToListBox(trait_list_id, i, trait->m_Name);
	}
/*
 *	Hmm... Here's a thing: Nothing in the SlaveMarket code sets ImageNum
 *	to anything other than -1. That's not just my refactoring, either.
 *	the original code was the same
 *
 *	which more or less makes sense, since that would trigger random selection
 *
 *	except that g_Girls.GetImageSurface sets the image number.
 *	I had to make image_num() supply a reference so it would compile
 */
// 	g_LogFile.os() << "... setting image: " << ImageNum << endl;

	SetImage(slave_image_id, g_Girls.GetImageSurface(girl, IMGTYPE_PROFILE, true, ImageNum));
	if(g_Girls.IsAnimatedSurface(girl, IMGTYPE_PROFILE, ImageNum))
		SetImage(slave_image_id, g_Girls.GetAnimatedSurface(girl, IMGTYPE_PROFILE, ImageNum));

/*
 *	set the trait list to the first entry
 *
 *	(what happens if there are no traits at all?)
 */
	SetSelectedItemInList(trait_list_id, 0);
	return true;
}

bool cScreenSlaveMarket::check_events()
{
	cTariff tariff;
	sGirl *girl = MarketSlaveGirls[selection];
/* 
 *	no events means we can go home
 */
	if(g_InterfaceEvents.GetNumEvents() == 0)
		return true;

/*
 *	if it's the back button, pop the window off the stack
 *	and we're done
 */
	if(g_InterfaceEvents.CheckButton(back_id)) {
		girl = 0;
		g_InitWin = true;
		g_WinManager.Pop();
		return true;
	}

	if(g_InterfaceEvents.CheckButton(buy_slave_id))
	{
		for(selection = multi_slave_first();
			selection != -1;
			selection = multi_slave_next()
		){
			girl = MarketSlaveGirls[selection];
			int cost = tariff.slave_buy_price(girl);
			cerr << "Selection = " << selection 
			     << ", girl = " << girl->m_Realname
			     << endl
			;
/*
 *			can the player afford this particular playmate?
 */
			if(g_Gold.slave_cost(cost) == false) {
				string text = gettext("You don't have enough money to purchase ");
				text += girl->m_Realname;
				g_MessageQue.AddToQue(text, 0);
				break;
			}

			sBrothel* brothel = g_Brothels.GetBrothel(g_CurrBrothel);
			if(g_Girls.GetRebelValue(girl, false) >= 35)
			{
				string text = girl->m_Realname;
				text += gettext(" has been sent to your dungeon, as she is rebellious and poorly trained.");
				g_MessageQue.AddToQue(text, 0);
				g_Brothels.GetDungeon()->AddGirl(girl, DUNGEON_NEWSLAVE);
			}
			else if((brothel->m_NumRooms - brothel->m_NumGirls) == 0)
			{
				string text = girl->m_Realname;
				text += gettext(" has been sent to your dungeon, since your current brothel is full.");
				g_MessageQue.AddToQue(text, 0);
				g_Brothels.GetDungeon()->AddGirl(girl, DUNGEON_NEWSLAVE);
			}
			else
			{
				string text = girl->m_Realname;
				text += gettext(" has been sent to your current brothel.");
				g_MessageQue.AddToQue(text, 0);
				g_Brothels.AddGirl(g_CurrBrothel, girl);
			}
			EditTextItem("", details_id);
			MarketSlaveGirls[selection] = 0;
		}
		selection = -1;
		g_InitWin = true;
		return true;
	}
	if(g_InterfaceEvents.CheckButton(more_id))
	{
		if(DetailLevel == 0)
		{
			DetailLevel = 1;
			EditTextItem(g_Girls.GetMoreDetailsString(girl), details_id);
		}
		else
		{
			DetailLevel = 0;
			EditTextItem(g_Girls.GetDetailsString(girl,true), details_id);
		}
		return true;
	}
	if(g_InterfaceEvents.CheckListbox(trait_list_id))
	{
		int tmp = GetLastSelectedItemFromList(trait_list_id);
		selection = GetLastSelectedItemFromList(slave_list_id);
		if(tmp != -1 && selection != -1)
			EditTextItem(MarketSlaveGirls[selection]->m_Traits[tmp]->m_Desc, trait_id);
		return true;
	}
	if(g_InterfaceEvents.CheckListbox(slave_list_id))
	{
		return change_selected_girl();
	}
	return false;
}

