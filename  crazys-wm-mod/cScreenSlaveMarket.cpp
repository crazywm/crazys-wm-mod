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
static cPlayer* m_Player = g_Brothels.GetPlayer();

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
	if (!g_InitWin) return;
	Focused();
	g_InitWin = false;

	//buttons enable/disable
	DisableButton(more_id, true);
	DisableButton(buy_slave_id, true);
	selection = -1;
	cConfig cfg;
	if (cfg.debug.log_girls()) g_LogFile.os() << "setting up slave market: genGirls = " << g_GenGirls << endl;

	ImageNum = -1;
	string brothel = gettext("Current Brothel: ");
	brothel += g_Brothels.GetName(g_CurrBrothel);
	EditTextItem(brothel, cur_brothel_id);

	// clear the list
	ClearListBox(slave_list_id);

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

	string message = gettext("Slave Market, ");
	message += g_Gold.sval();
	message += gettext(" gold");
	EditTextItem(message, header_id);

	// Finds the first girl in the selection, so she is highlighted. This stops the No girl selected that was normal before. --PP
	for (int i = numgirls-1; i > -1; i--)
	{
		if (MarketSlaveGirls[i] == 0) continue;
		selection = i;
	}

	// if there is still as selection (a non empty slave list) then highlight the current selection
	if (selection >= 0) SetSelectedItemInList(slave_list_id, selection, true);
	// now we need to populate the trait box
	ClearListBox(trait_list_id);
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
	// loop through her traits, populating the box
	for (int i = 0; i < MAXNUM_TRAITS; i++)
	{
		if (!g->m_Traits[i]) continue;
		AddToListBox(trait_list_id, i, g_Traits.GetTranslateName(g->m_Traits[i]->m_Name));
	}
	// and finally, highlight the selected entry?
	SetSelectedItemInList(trait_list_id, 0);
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
		cTariff tariff;
		sGirl *girl = MarketSlaveGirls[selection];
		g_SpaceKey = false;
		for (selection = multi_slave_first(); selection != -1; selection = multi_slave_next())
		{
			girl = MarketSlaveGirls[selection];
			int cost = tariff.slave_buy_price(girl);
			cerr << "Selection = " << selection << ", girl = " << girl->m_Realname << endl;
			// can the player afford this particular playmate?
			if (g_Gold.slave_cost(cost) == false)
			{
				string text = "You don't have enough money to purchase ";
				text += girl->m_Realname;
				g_MessageQue.AddToQue(text, 0);
				break;
			}

			sBrothel* brothel = g_Brothels.GetBrothel(g_CurrBrothel);
			string text = get_buy_slave_string(girl);
			if (g_Girls.GetRebelValue(girl, false) >= 35 || (brothel->m_NumRooms - brothel->m_NumGirls) == 0)
			{
				g_MessageQue.AddToQue(text, 0);
				g_Brothels.GetDungeon()->AddGirl(girl, DUNGEON_NEWSLAVE);
			}
			else
			{
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
	if (!ids_set) set_ids();						// we need to make sure the ID variables are set
	if (check_keys()) return;						// handle arrow keys
	init();											// set up the window if needed
	check_events();									// check to see if there's a button event needing handling
	HideImage(slave_image_id, (selection == -1));	// hide/show image based on whether a girl is selected
	if (selection == -1)							// if no girl is selected, clear girl info
	{
		EditTextItem(gettext("No girl selected"), details_id);
		EditTextItem("", trait_id);
	}
	// nothing selected == nothing further to do
	int index = selected_item();
	if (index == -1) return;
	sGirl *girl;
	girl = MarketSlaveGirls[index];
	if (!girl)
	{
		g_LogFile.os() << "... no girl at index " << index << "- returning " << endl;
		return;
	}
}

void cScreenSlaveMarket::generate_unique_girl(int i, bool &unique)
{
	cConfig cfg;

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
	ClearListBox(trait_list_id);
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
	for (int i = 0; i < MAXNUM_TRAITS; i++)						// whizz down the girl's trait list
	{
		sTrait *trait = girl->m_Traits[i];
		if (!trait) continue;							// skip any that are absent
		AddToListBox(trait_list_id, i, trait->m_Name);	// mention the rest in the trait listbox
	}

	SetImage(slave_image_id, g_Girls.GetImageSurface(girl, IMGTYPE_PROFILE, true, ImageNum));
	if (g_Girls.IsAnimatedSurface(girl, IMGTYPE_PROFILE, ImageNum))
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
		for (selection = multi_slave_first(); selection != -1; selection = multi_slave_next())
		{
			girl = MarketSlaveGirls[selection];
			int cost = tariff.slave_buy_price(girl);
			cerr << "Selection = " << selection << ", girl = " << girl->m_Realname << endl;
			// can the player afford this particular playmate?
			if (g_Gold.slave_cost(cost) == false)
			{
				string text = gettext("You don't have enough money to purchase ");
				text += girl->m_Realname;
				g_MessageQue.AddToQue(text, 0);
				break;
			}

			sBrothel* brothel = g_Brothels.GetBrothel(g_CurrBrothel);
			string text = get_buy_slave_string(girl);
			if (g_Girls.GetRebelValue(girl, false) >= 35 || (brothel->m_NumRooms - brothel->m_NumGirls) == 0)
			{
				g_MessageQue.AddToQue(text, 0);
				g_Brothels.GetDungeon()->AddGirl(girl, DUNGEON_NEWSLAVE);
			}
			else
			{
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
		if (tmp != -1 && selection != -1) EditTextItem(MarketSlaveGirls[selection]->m_Traits[tmp]->m_Desc, trait_id);
		return true;
	}
	if (g_InterfaceEvents.CheckListbox(slave_list_id)) return change_selected_girl();
	return false;
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
		if (m_Player->disposition() >= 80)				//Benevolent
		{
			text += " accepting her own flaws that to needed be corrected, she goes to the dungeon where she will be waiting for your guidance.";
		}
		else if (m_Player->disposition() >= 50)			//Nice
		{
			text += " in your opinion needs to work on her attitude, she has been guided to the dungeon.";
		}
		else if (m_Player->disposition() >= 10)			//Pleasant
		{
			text += " as your newest investment, she was sent to the dungeon to work on her rebellious nature.";
		}
		else if (m_Player->disposition() >= -10)			//Neutral
		{
			text += " has been sent to your dungeon, as she is rebellious and poorly trained.";
		}
		else if (m_Player->disposition() >= -50)			//Not nice
		{
			text += " as your newest investment that needs your special touch, she was sent to the dungeon.";
		}
		else if (m_Player->disposition() >= -80)			//Mean
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
		if (m_Player->disposition() >= 80)				//Benevolent
		{
			if (t == 1)	text += " went to your current brothel with a smile on her face happy that such a nice guy bought her.";
			else		text += " smiled as you offered her your arm, surprised to find such a kindness waiting for her. Hopeing such kindness would continue, she went happily with you as her owner.";
		}
		else if (m_Player->disposition() >= 50)			//Nice
		{
			if (t == 1)	text += ", having heard about her new owner's reputation, was guided to your current brothel without giving any trouble.";
			else		text += " looked up at you hopefully as you refused the use of a retainer or delivery, instead finding herself taken into your retinue for the day and given a chance to enjoy the fresh air before you both return home.";
		}
		else if (m_Player->disposition() >= 10)			//Pleasant
		{
			if (t == 1)	text += " was sent to your current brothel, knowing that she could have been bought by a lot worse owner.";
			else		text += " was escorted home by one of your slaves who helped her settle in. She seems rather hopeful of a good life in your care.";
		}
		else if (m_Player->disposition() >= -10)			//Neutral
		{
			if (t == 1)	text += " as your newest investment, she was sent to your current brothel.";
			else		text += " has been sent to your establishment under the supervision of your most trusted slaves.";
		}
		else if (m_Player->disposition() >= -50)			//Not nice
		{
			if (t == 1)	text += " not being very happy about her new owner, was escorted to your current brothel.";
			else		text += " struggled as her hands were shackled in front of her. Her eyes locked on the floor, tears gathering in the corners of her eyes, as she was sent off to your brothel.";
		}
		else if (m_Player->disposition() >= -80)			//Mean
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