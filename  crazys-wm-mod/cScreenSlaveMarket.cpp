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
#include <vector>
#include "cBrothel.h"
#include "main.h"
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
extern sGirl* MarketSlaveGirls[20];
extern int MarketSlaveGirlsDel[20];
extern bool g_GenGirls;
extern bool g_Cheats;
extern cTraits g_Traits;

extern cPlayer* The_Player;

extern	bool	g_LeftArrow;
extern	bool	g_RightArrow;
extern	bool	g_UpArrow;
extern	bool	g_DownArrow;
extern	bool	g_AltKeys;	// New hotkeys --PP
extern	bool	g_SpaceKey;
extern	bool	g_ShiftDown;
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

static string ReleaseGirlToWhere = "Br0";

bool cScreenSlaveMarket::ids_set = false;
cTariff tariff;

void cScreenSlaveMarket::set_ids()
{
	ids_set = true;
	back_id	= get_id("BackButton");
	more_id = get_id("ShowMoreButton");
	buy_slave_id = get_id("BuySlaveButton");
	cur_brothel_id = get_id("CurrentBrothel");
	slave_list_id = get_id("SlaveList");
	trait_list_id = get_id("TraitList");
	trait_list_text_id = get_id("TraitListT");
	details_id = get_id("SlaveDetails");
	trait_id = get_id("TraitDesc");
	girl_desc_id = get_id("GirlDesc");
	slave_image_id = get_id("SlaveImage");
	header_id = get_id("ScreenHeader");
	gold_id = get_id("Gold");
	slave_market_id = get_id("SlaveMarket");

	releaseto_id = get_id("ReleaseTo");
	roomsfree_id = get_id("RoomsFree");

	brothel0_id = get_id("Brothel0");
	brothel1_id = get_id("Brothel1");
	brothel2_id = get_id("Brothel2");
	brothel3_id = get_id("Brothel3");
	brothel4_id = get_id("Brothel4");
	brothel5_id = get_id("Brothel5");
	brothel6_id = get_id("Brothel6");
	house_id = get_id("House");
	clinic_id = get_id("Clinic");
	studio_id = get_id("Studio");
	arena_id = get_id("Arena");
	centre_id = get_id("Centre");
	farm_id = get_id("Farm");
	dungeon_id = get_id("Dungeon");
}

void cScreenSlaveMarket::init()
{
	g_CurrentScreen = SCREEN_SLAVEMARKET;
	if (!g_InitWin) return;
	Focused();
	g_InitWin = false;

	//buttons enable/disable
	DisableButton(more_id, true);
	DisableButton(buy_slave_id, true);
	selection = -1;
	
	if (cfg.debug.log_girls()) g_LogFile.os() << "setting up slave market: genGirls = " << g_GenGirls << endl;

	ImageNum = -1;
	stringstream ss;
	if (cur_brothel_id >= 0)
	{
		ss << "Current Brothel: " << g_Brothels.GetName(g_CurrBrothel);
		EditTextItem(ss.str(), cur_brothel_id);
	}

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
	else if (sub == "Du") releaseto = g_Brothels.GetBrothel(0);

	ss.str("");
	ss << "Send Girl to: " << (sub == "Du" ? "The Dungeon" : releaseto->m_Name);
	EditTextItem(ss.str(), releaseto_id);
	ss.str("");
	if (sub != "Du") ss << "Room for " << releaseto->free_rooms() << " more girls.";
	EditTextItem(ss.str(), roomsfree_id);

	HideButton(brothel1_id, g_Brothels.GetNumBrothels() < 2 || g_Brothels.GetBrothel(1) == 0);
	HideButton(brothel2_id, g_Brothels.GetNumBrothels() < 3 || g_Brothels.GetBrothel(2) == 0);
	HideButton(brothel3_id, g_Brothels.GetNumBrothels() < 4 || g_Brothels.GetBrothel(3) == 0);
	HideButton(brothel4_id, g_Brothels.GetNumBrothels() < 5 || g_Brothels.GetBrothel(4) == 0);
	HideButton(brothel5_id, g_Brothels.GetNumBrothels() < 6 || g_Brothels.GetBrothel(5) == 0);
	HideButton(brothel6_id, g_Brothels.GetNumBrothels() < 7 || g_Brothels.GetBrothel(6) == 0);
	HideButton(clinic_id, g_Clinic.GetNumBrothels()		< 1 || g_Clinic.GetBrothel(0) == 0);
	HideButton(studio_id, g_Studios.GetNumBrothels()	< 1 || g_Studios.GetBrothel(0) == 0);
	HideButton(arena_id, g_Arena.GetNumBrothels()		< 1 || g_Arena.GetBrothel(0) == 0);
	HideButton(centre_id, g_Centre.GetNumBrothels()		< 1 || g_Centre.GetBrothel(0) == 0);
	HideButton(farm_id, g_Farm.GetNumBrothels()			< 1 || g_Farm.GetBrothel(0) == 0);

	ClearListBox(slave_list_id);	// clear the list

	int numgirls = g_Dice.bell(cfg.slavemarket.slavesnewweeklymin(), cfg.slavemarket.slavesnewweeklymax());

	// Check if any slave girls
	for (int i = 0; i < 20; i++)
	{
		bool unique = false;
		// easiest case is if the girs have already been generated
		if (g_GenGirls != false)
		{
			// first of all, if there isn't a girl in this slot the reset doesn't matter much
			if (MarketSlaveGirls[i] == 0) continue;
			/*
			* I'm going to assume that -1 here means "OK to delete". Which means non -1 means
			* "do not delete", from which we can infer that the girl is unique.
			*/
			int col = COLOR_BLUE;
			if (MarketSlaveGirlsDel[i] != -1)
			{
				col = COLOR_RED;
				unique = true;
			}
			AddToListBox(slave_list_id, i, MarketSlaveGirls[i]->m_Realname, col);
			continue;
		}
		/*
		*		So at this point, g_GenGirls is true which means we need to regenerate the slave market
		*		I think I'd be happier with more of a slow, incremental change rather than a full regen each week.
		*		Still, that's for the future.
		*		For now: is there a girl in the current slot?
		*/
		if (MarketSlaveGirls[i] != 0)
		{
			// Yes there is. Is it OK to delete her. If so, do it
			if (MarketSlaveGirlsDel[i] == -1) delete MarketSlaveGirls[i];
			// in any case, mark the slot as empty
			MarketSlaveGirls[i] = 0;
		}
		if (i >= numgirls) continue;
		// now try and generate a unique girl for the slot
		generate_unique_girl(i, unique);
		// if the id for this slot is zero, then we didn't make a unique girl so we need a random one
		if (MarketSlaveGirls[i] == 0)
		{
			int n = 0;
			sGirl* testgirl = 0;
			while (testgirl == 0&& n<20)
			{
				testgirl = g_Girls.CreateRandomGirl(0, false, true);
				for (int j = 0; j < 20; j++)
				{
					if (MarketSlaveGirls[j] == 0) continue;
					else if (string(testgirl->m_Name) == string(MarketSlaveGirls[j]->m_Name))
					{
						testgirl = 0;
						break;
					}
				}
				n++;
			}
			if (testgirl == 0)
			{
				testgirl = g_Girls.CreateRandomGirl(0, false, true);
			}

			MarketSlaveGirls[i] = testgirl;
			MarketSlaveGirlsDel[i] = -1;
		}
		/*
		* if the slot is still zero, we couldn't get a girl at all (can this happen? it probably should...)
		* in which case there's nothing more for this time round.  (and arguably for the loop...)
		*/
		if (MarketSlaveGirls[i] == 0) continue;
		// decide the display color based on whether the girl is unique
		int col = unique ? COLOR_RED : COLOR_BLUE;
		// and display
		AddToListBox(slave_list_id, i, MarketSlaveGirls[i]->m_Realname, col);
	}

	selection = 0;

	/*
	*	if we're cheating, set the list as OK to be regenerated
	*	when we next display the screen.
	*
	*	if we're not, flag the market as regenerated for this turn
	*/
	g_GenGirls = (g_Cheats) ? false : true;

	if (header_id >= 0)
	{
		ss.str("");
		ss << "Slave Market, " << g_Gold.sval() << gettext(" gold");
		EditTextItem(ss.str(), header_id);
	}
	if (gold_id >= 0)
	{
		ss.str("");
		ss << "Gold:  " << g_Gold.sval();
		EditTextItem(ss.str(), gold_id);
	}
	
	// Finds the first girl in the selection, so she is highlighted. This stops the No girl selected that was normal before. --PP
	for (int i = numgirls-1; i > -1; i--)
	{
		if (MarketSlaveGirls[i] == 0) continue;
		selection = i;
	}

	// if there is still as selection (a non empty slave list) then highlight the current selection
	if (selection >= 0) SetSelectedItemInList(slave_list_id, selection, true);
	// now we need to populate the trait box
	if (trait_list_id >= 0) ClearListBox(trait_list_id);
	if (trait_list_text_id >= 0) EditTextItem("Traits:", trait_list_text_id);
	if (girl_desc_id >= 0)	EditTextItem("", girl_desc_id);
	int tmp = GetLastSelectedItemFromList(slave_list_id);
	// if the last item was -1 I assume the list is empty so we can go home early (and probably should have earlier still)
	if (tmp == -1) return;
	// get the girl under the cursor. If she's null, something funny is going on, so splurge a diagnostic before exiting the func
	sGirl* g = MarketSlaveGirls[tmp];
	if (g == 0)
	{
		g_LogFile.os() << "error: null pointer for cursor entry in market" << endl;
		return;
	}
	preparescreenitems(g);
}

bool cScreenSlaveMarket::check_keys()
{
	if (g_UpArrow || (g_AltKeys && g_A_Key))
	{
		g_UpArrow = false; g_A_Key = false;
		selection = ArrowUpListBox(slave_list_id);
		g_GenGirls = true;
		return true;
	}
	if (g_DownArrow || (g_AltKeys && g_D_Key))
	{
		g_DownArrow = false; g_D_Key = false;
		selection = ArrowDownListBox(slave_list_id);
		g_GenGirls = true;
		return true;
	}
	if (g_AltKeys && g_S_Key)
	{
		sGirl *girl = MarketSlaveGirls[selection];
		g_S_Key = false;
		if (g_ShiftDown)
		{
			DetailLevel = 2;
			EditTextItem(g_Girls.GetThirdDetailsString(girl), details_id);
		}
		else
		{
			if (DetailLevel == 0)		{ DetailLevel = 1; EditTextItem(g_Girls.GetMoreDetailsString(girl, true), details_id); }
			else						{ DetailLevel = 0; EditTextItem(g_Girls.GetDetailsString(girl, true), details_id); }
		}
		return true;
	}
	if (g_SpaceKey)
	{
		return buy_slaves();
	}
	return false;
}

bool cScreenSlaveMarket::check_events()
{
	sGirl *girl = MarketSlaveGirls[selection];

	if (g_InterfaceEvents.GetNumEvents() == 0) return true;	// no events means we can go home

	// if it's the back button, pop the window off the stack and we're done
	if (g_InterfaceEvents.CheckButton(back_id))
	{
		girl = 0;
		g_InitWin = true;
		g_WinManager.Pop();
		return true;
	}

	if (g_InterfaceEvents.CheckButton(buy_slave_id))
	{
		return buy_slaves();
	}
	if (g_InterfaceEvents.CheckButton(more_id))
	{
		if (DetailLevel == 0)		{ DetailLevel = 1; EditTextItem(g_Girls.GetMoreDetailsString(girl, true), details_id); }
		else if (DetailLevel == 1)	{ DetailLevel = 2; EditTextItem(g_Girls.GetThirdDetailsString(girl), details_id); }
		else						{ DetailLevel = 0; EditTextItem(g_Girls.GetDetailsString(girl, true), details_id); }
		return true;
	}
	if (g_InterfaceEvents.CheckListbox(trait_list_id))
	{
		int tmp = GetLastSelectedItemFromList(trait_list_id);
		selection = GetLastSelectedItemFromList(slave_list_id);
		if (tmp != -1 && selection != -1 && MarketSlaveGirls[selection]->m_NumTraits > 0)
		{
			if (tmp > MarketSlaveGirls[selection]->m_NumTraits)	tmp = 0;
			if (trait_id >= 0)	EditTextItem(MarketSlaveGirls[selection]->m_Traits[tmp]->m_Desc, trait_id);
		}
		else if (trait_id >= 0)	EditTextItem("", trait_id);
		return true;
	}
	if (g_InterfaceEvents.CheckListbox(slave_list_id)) return change_selected_girl();


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
	else if (g_InterfaceEvents.CheckButton(dungeon_id))		change_release("Du0");

	return false;
}

void cScreenSlaveMarket::process()
{
	if (!ids_set) set_ids();						// we need to make sure the ID variables are set
	if (check_keys()) return;						// handle arrow keys
	init();											// set up the window if needed
	check_events();									// check to see if there's a button event needing handling
	HideImage(slave_image_id, (selection == -1));	// hide/show image based on whether a girl is selected
	if (selection == -1)							// if no girl is selected, clear girl info
	{
		EditTextItem("No girl selected", details_id);
		if (trait_id >= 0) EditTextItem("", trait_id);
	}
	// nothing selected == nothing further to do
	int index = selected_item();
	if (index == -1) return;
	sGirl* girl = MarketSlaveGirls[index];
	if (!girl) g_LogFile.os() << "... no girl at index " << index << "- returning " << endl;
}

bool cScreenSlaveMarket::buy_slaves()
{
	g_SpaceKey = false;
	stringstream ss;
	stringstream sendtotext;
	// set the brothel
	sBrothel *brothel = g_Brothels.GetBrothel(g_CurrBrothel);
	string sub = ReleaseGirlToWhere.substr(0, 2);
	char a = ReleaseGirlToWhere[2]; char b = "0"[0]; int sendtonum = a - b;	// `J` cheap fix to get brothel number
	if (sub == "Du") sendtotext << "the Dungeon";
	else
	{
		/* */if (sub == "Br")	{ brothel = g_Brothels.GetBrothel(sendtonum);	sendtotext << "your brothel: " << brothel->m_Name;	}
		else if (sub == "Ho")	{ brothel = g_House.GetBrothel(sendtonum);		sendtotext << "your House";							}
		else if (sub == "Cl")	{ brothel = g_Clinic.GetBrothel(sendtonum);		sendtotext << "the Clinic";							}
		else if (sub == "St")	{ brothel = g_Studios.GetBrothel(sendtonum);	sendtotext << "the Studio";							}
		else if (sub == "Ar")	{ brothel = g_Arena.GetBrothel(sendtonum);		sendtotext << "the Arena";							}
		else if (sub == "Ce")	{ brothel = g_Centre.GetBrothel(sendtonum);		sendtotext << "the Community Centre";				}
		else if (sub == "Fa")	{ brothel = g_Farm.GetBrothel(sendtonum);		sendtotext << "the Farm";							}
	}

	// set the girls
	sGirl *girl = MarketSlaveGirls[selection];
	vector<string> girlsnames;

	int totalcost = 0;
	for (int sel = multi_slave_first(); sel != -1; sel = multi_slave_next())
	{
		girl = MarketSlaveGirls[sel];
		totalcost += tariff.slave_buy_price(girl);
		girlsnames.push_back(girl->m_Realname);
	}

	// Check if there is enough room where we want to send her
	if (sub != "Du")	// Dungeon has no limit so don't bother checking if sending them there.
	{
		if (brothel->free_rooms() < 0)
		{
			g_MessageQue.AddToQue("The current building has no more room.\nChoose another building to send them to.", 0);
			return false;
		}
		if (brothel->free_rooms() < (int)girlsnames.size())
		{
			g_MessageQue.AddToQue("The current building does not have enough room for all the girls you want to send there.\nChoose another building or select fewer girls at a time to buy.", 0);
			return false;
		}
	}

	// `J` check if we can afford all the girls selected
	if (!g_Gold.slave_cost(totalcost))	// this pays for them if you can afford them
	{									// otherwise this part runs and returns a fail message.
		stringstream text;
		if (girlsnames.size() > 4 && g_Gold.ival() < totalcost / 2) text << "Calm down!  ";
		text << "You don't have enough money to purchase ";
		if (girlsnames.size() == 1) text << girlsnames[0];
		else
		{
			if (girlsnames.size() == 2) text << "these two";
			else if (girlsnames.size() == 3) text << "these three";
			else text << girlsnames.size();
			text << " girls";
		}
		text << ".";
		g_MessageQue.AddToQue(text.str(), 0);
		return false;
	}

	// `J` we could afford the girls so lets get to it
	ss << "You buy ";

	/* */if (girlsnames.size() == 1)	ss << "a girl,   " << girlsnames[0] << "   and send her to " << sendtotext.str();
	else if (girlsnames.size() == 2)	ss << "two girls,   " << girlsnames[0] << "   and   " << girlsnames[1] << ". You send them to " << sendtotext.str();
	else /*                       */	ss << girlsnames.size() << " girls and send them to " << sendtotext.str();
	ss << ".\n\n";

	// `J` zzzzzz - add in flavor texts here
	
	











	// `J` end flavor texts
	
	// `J` send them where they need to go
	for (int sel = multi_slave_first(); sel != -1; sel = multi_slave_next())
	{
		girl = MarketSlaveGirls[sel];
		/* */if (sub == "Br") g_Brothels.AddGirl(brothel->m_id, girl);
		else if (sub == "Ho") g_House.AddGirl(brothel->m_id, girl);
		else if (sub == "Cl") g_Clinic.AddGirl(brothel->m_id, girl);
		else if (sub == "St") g_Studios.AddGirl(brothel->m_id, girl);
		else if (sub == "Ar") g_Arena.AddGirl(brothel->m_id, girl);
		else if (sub == "Ce") g_Centre.AddGirl(brothel->m_id, girl);
		else if (sub == "Fa") g_Farm.AddGirl(brothel->m_id, girl);
		else	// if something fails this will catch it and send them to the dungeon
		{
			g_Brothels.GetDungeon()->AddGirl(girl, DUNGEON_NEWSLAVE);
		}
		MarketSlaveGirls[sel] = 0;
	}

	// `J` now tell the player what happened
	if (girlsnames.size() <= 0)	g_MessageQue.AddToQue("Error, no girls names in the list", 0);
	else g_MessageQue.AddToQue(ss.str(), 0);

	// finish it
	EditTextItem("", details_id);
	selection = -1;
	g_InitWin = true;
	return true;
}

void cScreenSlaveMarket::change_release(string towhere)
{
	ReleaseGirlToWhere = towhere;
	stringstream ss;
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
	else if (sub == "Du") releaseto = g_Brothels.GetBrothel(0);

	ss.str("");
	ss << "Send Girl to: " << (sub == "Du" ? "The Dungeon" : releaseto->m_Name);
	EditTextItem(ss.str(), releaseto_id);
	ss.str("");
	if (sub != "Du") ss << "Room for " << releaseto->free_rooms() << " more girls.";
	EditTextItem(ss.str(), roomsfree_id);


}

void cScreenSlaveMarket::generate_unique_girl(int i, bool &unique)
{
	

	if (g_Girls.GetNumSlaveGirls() <= 0) return;				// if there are no unique slave girls left then we can do no more here
	if (!g_Dice.percent(cfg.slavemarket.unique_market())) return;	// otherwise - 35% chance of a unique girl. `J` added config.xml customization
	int g = g_Dice%g_Girls.GetNumSlaveGirls();					// randomly select a slavegirl from the list
	sGirl *gpt = g_Girls.GetGirl(g_Girls.GetSlaveGirl(g));		// try and get a struct for the girl in question
	if (!gpt) return;											// if we can't, we go home
	/*
	 *	whizz down the list of girls we have already
	 *	and see if the new girl is already in the list
	 *
	 *	if she is, we need do nothing more
	 */
	for (int j = 0; j < 20; j++)
	{
		if (MarketSlaveGirls[j] == gpt) return;
	}
	gpt->m_Stats[STAT_HOUSE] = cfg.initial.slave_house_perc();
	// add the girl to the slave market girls and mark her as not-to-be-deleted
	MarketSlaveGirls[i] = gpt;
	MarketSlaveGirlsDel[i] = g;
	unique = true;
}

bool cScreenSlaveMarket::change_selected_girl()
{
	ImageNum = -1;
	/*
	 *	Since this is a multiselect box, GetLastSelectedItemFromList
	 *	returns the last clicked list item, even if it's deselected.
	 *	So, we'll check for that and show first truly selected item
	 *	if the last clicked one is actually deselected.
	 */
	selection = GetSelectedItemFromList(slave_list_id);
	bool MatchSel = false;
	int i;

	for (i = multi_slave_first(); i != -1; i = multi_slave_next())
	{
		if (i == selection)
		{
			MatchSel = true;
			break;
		}
	}
	if (!MatchSel) selection = multi_slave_first();
	// if the player selected an empty slot make that into "nothing selected" and return
	if (MarketSlaveGirls[selection] == 0) selection = -1;
	// disable/enable buttons based on whether a girl is selected
	DisableButton(more_id, (selection == -1));
	DisableButton(buy_slave_id, (selection == -1));
	if (trait_list_id >= 0) ClearListBox(trait_list_id);
	if (trait_list_text_id >= 0) EditTextItem("Traits:", trait_list_text_id);
	if (girl_desc_id >= 0)	EditTextItem("", girl_desc_id);
	// selection of -1 means nothing selected so we get to go home early
	if (selection == -1) return true;
	/*
	 *	otherwise, we have (potentially) a new girl:
	 *	set the global girl pointer
	 *
	 *	if we can't find the pointer. log an error and go home
	 */
	sGirl *girl = MarketSlaveGirls[selection];
	if (!girl)
	{
		g_LogFile.ss() << "Warning: cScreenSlaveMarket::change_selected_girl" << "can't find girl data for selection";
		g_LogFile.ssend();
		return true;
	}
	string detail;

	if (DetailLevel == 0)		detail = g_Girls.GetDetailsString(girl, true);
	else if (DetailLevel == 1)	detail = g_Girls.GetMoreDetailsString(girl, true);
	else						detail = g_Girls.GetThirdDetailsString(girl);
	EditTextItem(detail, details_id);
	ImageNum = -1;										// I don't understand where this is used...

	preparescreenitems(girl);
	PrepareImage(slave_image_id, girl, IMGTYPE_PRESENTED, true, ImageNum);

	return true;
}

string cScreenSlaveMarket::get_buy_slave_string(sGirl* girl)
{
	sBrothel* brothel = g_Brothels.GetBrothel(g_CurrBrothel);
	string text = girl->m_Realname;

	if ((brothel->m_NumRooms - brothel->m_NumGirls) == 0)
	{
		text += " has been sent to your dungeon, since your current brothel is full.";
	}
	else if (g_Girls.GetRebelValue(girl, false) >= 35)
	{
		if (The_Player->disposition() >= 80)				//Benevolent
		{
			text += " accepting her own flaws that to needed be corrected, she goes to the dungeon where she will be waiting for your guidance.";
		}
		else if (The_Player->disposition() >= 50)			//Nice
		{
			text += " in your opinion needs to work on her attitude, she has been guided to the dungeon.";
		}
		else if (The_Player->disposition() >= 10)			//Pleasant
		{
			text += " as your newest investment, she was sent to the dungeon to work on her rebellious nature.";
		}
		else if (The_Player->disposition() >= -10)			//Neutral
		{
			text += " has been sent to your dungeon, as she is rebellious and poorly trained.";
		}
		else if (The_Player->disposition() >= -50)			//Not nice
		{
			text += " as your newest investment that needs your special touch, she was sent to the dungeon.";
		}
		else if (The_Player->disposition() >= -80)			//Mean
		{
			text += " still had some spirit in her eyes left that you decided to stub out. She was dragged to a dungeon cell.";
		}
		else											//Evil
		{
			text += " put up a fight. She was beaten and dragged to your dungeon where you can have some private fun time with her.";
		}
	}
	else
	{
		int t = g_Dice % 2;	// `J` because there are currently only 2 text options per disposition this should be ok
		if (The_Player->disposition() >= 80)				//Benevolent
		{
			if (t == 1)	text += " went to your current brothel with a smile on her face happy that such a nice guy bought her.";
			else		text += " smiled as you offered her your arm, surprised to find such a kindness waiting for her. Hopeing such kindness would continue, she went happily with you as her owner.";
		}
		else if (The_Player->disposition() >= 50)			//Nice
		{
			if (t == 1)	text += ", having heard about her new owner's reputation, was guided to your current brothel without giving any trouble.";
			else		text += " looked up at you hopefully as you refused the use of a retainer or delivery, instead finding herself taken into your retinue for the day and given a chance to enjoy the fresh air before you both return home.";
		}
		else if (The_Player->disposition() >= 10)			//Pleasant
		{
			if (t == 1)	text += " was sent to your current brothel, knowing that she could have been bought by a lot worse owner.";
			else		text += " was escorted home by one of your slaves who helped her settle in. She seems rather hopeful of a good life in your care.";
		}
		else if (The_Player->disposition() >= -10)			//Neutral
		{
			if (t == 1)	text += " as your newest investment, she was sent to your current brothel.";
			else		text += " has been sent to your establishment under the supervision of your most trusted slaves.";
		}
		else if (The_Player->disposition() >= -50)			//Not nice
		{
			if (t == 1)	text += " not being very happy about her new owner, was escorted to your current brothel.";
			else		text += " struggled as her hands were shackled in front of her. Her eyes locked on the floor, tears gathering in the corners of her eyes, as she was sent off to your brothel.";
		}
		else if (The_Player->disposition() >= -80)			//Mean
		{
			if (t == 1)	text += " didn't wanted to provoke you in any possible way. She went quietly to your current brothel, without any resistance.";
			else		text += " was dragged away to your brothel crying, one of your guards slapping her face as she tried to resist. ";
		}
		else											//Evil
		{
			if (t == 1)	text += " was dragged crying and screaming to your current brothel afraid of what you might do to her as her new owner.";
			else		text += " looked up at you in fear as you order for her to be taken to your brothel. A hint of some emotion hidden in her eyes draws your attention for a moment before she unconsciously looked away, no doubt afraid of what you'd do to her if she met your gaze.";
		}
	}
	return text;
}

void cScreenSlaveMarket::preparescreenitems(sGirl* girl)
{

	int trait_count = 0;
	stringstream traits_text;
	traits_text << "Traits:      ";

	// loop through her traits, populating the box
	for (int i = 0; i < MAXNUM_TRAITS; i++)
	{
		if (!girl->m_Traits[i]) continue;
		trait_count++;
		if (trait_list_id >= 0) AddToListBox(trait_list_id, i, g_Traits.GetTranslateName(girl->m_Traits[i]->m_Name));
		if (trait_list_text_id)
		{
			if (trait_count > 1) traits_text << ",   ";
			traits_text << g_Traits.GetTranslateName(girl->m_Traits[i]->m_Name);
		}
	}
	// and finally, highlight the selected entry?
	if (trait_list_id >= 0) SetSelectedItemInList(trait_list_id, 0);
	if (trait_list_text_id >= 0) EditTextItem(traits_text.str(), trait_list_text_id);
	if (girl_desc_id >= 0)	EditTextItem(girl->m_Desc, girl_desc_id);



}