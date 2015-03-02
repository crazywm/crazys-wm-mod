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
#include "cScreenGangs.h"
#include "cWindowManager.h"
#include "cGold.h"
#include "InterfaceProcesses.h"
#include "cGangs.h"
#include "libintl.h"

extern bool g_InitWin;
extern int g_CurrBrothel;
extern cGold g_Gold;
extern cWindowManager g_WinManager;
extern cInterfaceEventManager	g_InterfaceEvents;
extern cGangManager g_Gangs;

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

static cTariff tariff;

static int selection = -1;
static int sel_recruit = -1;

bool cScreenGangs::ids_set = false;

void cScreenGangs::set_ids()
{
	ids_set = true;
	back_id = get_id("BackButton");
	ganghire_id = get_id("GangHireButton");
	gangfire_id = get_id("GangFireButton");
	totalcost_id = get_id("TotalCost");
	ganglist_id = get_id("GangList");
	missionlist_id = get_id("MissionList");
	gangdesc_id = get_id("GangDescription");
	missiondesc_id = get_id("MissionDescription");
	weaponlevel_id = get_id("WeaponDescription");
	weaponup_id = get_id("WeaponUpButton");
	netdesc_id = get_id("NetDescription");
	netbuy_id = get_id("BuyNetsButton");
	netbuy10_id = get_id("BuyNetsButton10");
	netbuy20_id = get_id("BuyNetsButton20");
	netautobuy_id = get_id("AutoBuyNetsToggle");
	healdesc_id = get_id("HealPotDescription");
	healbuy_id = get_id("BuyHealPotButton");
	healbuy10_id = get_id("BuyHealPotButton10");
	healbuy20_id = get_id("BuyHealPotButton20");
	healautobuy_id = get_id("AutoBuyHealToggle");
	recruitlist_id = get_id("RecruitList");

	//Set the default sort order for columns, so listboxes know the order in which data will be sent
	string RecruitColumns[] = { "GangName", "Number", gettext("Combat"), gettext("Magic"), gettext("Intelligence"), gettext("Agility"), gettext("Constitution"), gettext("Charisma") };
	SortColumns(recruitlist_id, RecruitColumns, 8);
	string GangColumns[] = { "GangName", "Number", "Mission", gettext("Combat"), gettext("Magic"), gettext("Intelligence"), gettext("Agility"), gettext("Constitution"), gettext("Charisma") };
	SortColumns(ganglist_id, GangColumns, 9);
}

void cScreenGangs::init()
{
	stringstream ss;
	g_CurrentScreen = SCREEN_GANGMANAGEMENT;
	if (!g_InitWin) return;

	Focused();
	g_InitWin = false;
	cConfig cfg;

	selection = GetLastSelectedItemFromList(ganglist_id);
	sel_recruit = GetLastSelectedItemFromList(recruitlist_id);

	ClearListBox(missionlist_id);
	AddToListBox(missionlist_id, 0, gettext("GUARDING"));
	AddToListBox(missionlist_id, 1, gettext("SABOTAGE"));
	AddToListBox(missionlist_id, 2, gettext("SPY ON GIRLS"));
	AddToListBox(missionlist_id, 3, gettext("RECAPTURE"));
	AddToListBox(missionlist_id, 4, gettext("ACQUIRE TERRITORY"));
	AddToListBox(missionlist_id, 5, gettext("PETTY THEFT"));
	AddToListBox(missionlist_id, 6, gettext("GRAND THEFT"));
	AddToListBox(missionlist_id, 7, gettext("KIDNAPPING"));
	AddToListBox(missionlist_id, 8, gettext("CATACOMBS"));
	AddToListBox(missionlist_id, 9, gettext("TRAINING"));
	AddToListBox(missionlist_id, 10, gettext("RECRUITING"));

	SetCheckBox(netautobuy_id, (g_Gangs.GetNetRestock() > 0));
	SetCheckBox(healautobuy_id, (g_Gangs.GetHealingRestock() > 0));

	// weapon upgrades
	int *wlev = g_Gangs.GetWeaponLevel();
	ss.str("");
	ss << gettext("Weapon Level: ") << *wlev;
	if ((*wlev) < 4)
	{
		EnableButton(weaponup_id);
		ss << gettext(" Next: ") << tariff.goon_weapon_upgrade(*wlev) << gettext("g");
	}
	else DisableButton(weaponup_id);
	cerr << "weapon text = '" << ss.str() << "'" << endl;
	EditTextItem(ss.str(), weaponlevel_id);

	ss.str("");
	int *nets = g_Gangs.GetNets();
	ss << gettext("Nets (") << tariff.nets_price(1) << gettext("g each): ") << *nets;
	EditTextItem(ss.str(), netdesc_id);
	DisableButton(netbuy_id, *nets >= 60);
	DisableButton(netbuy10_id, *nets >= 60);
	DisableButton(netbuy20_id, *nets >= 60);
	DisableCheckBox(netautobuy_id, *nets < 1);

	ss.str("");
	int *potions = g_Gangs.GetHealingPotions();
	ss << gettext("Heal Potions (") << tariff.healing_price(1) << gettext("g each): ") << *potions;
	EditTextItem(ss.str(), healdesc_id);
	DisableButton(healbuy_id, *potions >= 200);
	DisableButton(healbuy10_id, *potions >= 200);
	DisableButton(healbuy20_id, *potions >= 200);
	DisableCheckBox(healautobuy_id, *potions < 1);

	ss.str("");
	int cost = 0;
	if (g_Gangs.GetNumGangs() > 0)
	{
		for (int i = 0; i < g_Gangs.GetNumGangs(); i++)
		{
			sGang* g = g_Gangs.GetGang(i);
			if (g == 0) g = g_Gangs.GetGang(i - 1);
			cost += tariff.goon_mission_cost(g->m_MissionID);
		}
	}
	ss << gettext("Weekly Cost: ") << cost;
	EditTextItem(ss.str(), totalcost_id);

	ClearListBox(ganglist_id);
	int num = 0;
	sGang* current = g_Gangs.GetGang(0);

	// loop through the gangs, populating the list box
	cerr << "Setting gang mission descriptions" << endl;
	for (current = g_Gangs.GetGang(0); current; current = current->m_Next)
	{
		// format the string with the gang name, mission and number of men
		string Data[9];
		ss.str("");
		ss << current->m_Name;
		Data[0] = ss.str();
		ss.str("");
		ss << current->m_Num;
		Data[1] = ss.str();
		ss.str("");
		ss << short_mission_desc(current->m_MissionID);
		Data[2] = ss.str();
		ss.str("");
		ss << current->m_Skills[SKILL_COMBAT] << "%";
		Data[3] = ss.str();
		ss.str("");
		ss << current->m_Skills[SKILL_MAGIC] << "%";
		Data[4] = ss.str();
		ss.str("");
		ss << current->m_Stats[STAT_INTELLIGENCE] << "%";
		Data[5] = ss.str();
		ss.str("");
		ss << current->m_Stats[STAT_AGILITY] << "%";
		Data[6] = ss.str();
		ss.str("");
		ss << current->m_Stats[STAT_CONSTITUTION] << "%";
		Data[7] = ss.str();
		ss.str("");
		ss << current->m_Stats[STAT_CHARISMA] << "%";
		Data[8] = ss.str();

		//		cerr << "Gang:\t" << Data[0] << "\t" << Data[1] << "\t" << Data[2]
		//			<< "\t" << Data[3] << "\t" << Data[4] << "\t" << Data[5] << "\t" << Data[6] << endl;

		/*
		*			add the box to the list; red highlight gangs that are low on numbers
		*/
		AddToListBox(ganglist_id, num++, Data, 9, (current->m_Num < 6 ? COLOR_RED : COLOR_BLUE));
	}

	ClearListBox(recruitlist_id);
	num = 0;
	current = g_Gangs.GetHireableGang(0);

	// loop through the gangs, populating the list box
	cerr << "Setting recruitable gang info" << endl;
	for (current = g_Gangs.GetHireableGang(0); current; current = current->m_Next)
	{
		// format the string with the gang name, mission and number of men
		string Data[8];
		ss.str("");
		ss << current->m_Name;
		Data[0] = ss.str();
		ss.str("");
		ss << current->m_Num;
		Data[1] = ss.str();
		ss.str("");
		ss << current->m_Skills[SKILL_COMBAT] << "%";
		Data[2] = ss.str();
		ss.str("");
		ss << current->m_Skills[SKILL_MAGIC] << "%";
		Data[3] = ss.str();
		ss.str("");
		ss << current->m_Stats[STAT_INTELLIGENCE] << "%";
		Data[4] = ss.str();
		ss.str("");
		ss << current->m_Stats[STAT_AGILITY] << "%";
		Data[5] = ss.str();
		ss.str("");
		ss << current->m_Stats[STAT_CONSTITUTION] << "%";
		Data[6] = ss.str();
		ss.str("");
		ss << current->m_Stats[STAT_CHARISMA] << "%";
		Data[7] = ss.str();

		//		cerr << "Recruitable\t" << Data[0] << "\t" << Data[1] << "\t" << Data[2]
		//			<< "\t" << Data[3] << "\t" << Data[4] << "\t" << Data[5] << endl;

		/*
		*			add the box to the list
		*/
		AddToListBox(recruitlist_id, num++, Data, 8, (current->m_Num < 6 ? COLOR_RED : COLOR_BLUE));
	}

	if (selection == -1 && GetListBoxSize(ganglist_id) >= 1) selection = 0;

	if (selection >= 0)
	{
		while (selection > GetListBoxSize(ganglist_id) && selection != -1) selection--;
	}
	if (selection >= 0) SetSelectedItemInList(ganglist_id, selection);
	if (sel_recruit == -1 && GetListBoxSize(recruitlist_id) >= 1) sel_recruit = 0;
	if (sel_recruit >= 0) SetSelectedItemInList(recruitlist_id, sel_recruit);

	DisableButton(ganghire_id, (g_Gangs.GetNumHireableGangs() <= 0) || (g_Gangs.GetNumGangs() >= g_Gangs.GetMaxNumGangs()) || (sel_recruit == -1));
	DisableButton(gangfire_id, (g_Gangs.GetNumGangs() <= 0) || (selection == -1));

	potions = wlev = nets = 0;
}

void cScreenGangs::process()
{
	if (!ids_set) set_ids();	// we need to make sure the ID variables are set
	if (check_keys()) return;	// handle arrow keys
	init();						// set up the window if needed
	check_events();				// check to see if there's a button event needing handling
}

bool cScreenGangs::check_keys()
{
	if (g_UpArrow) 
	{
		selection = ArrowUpListBox(ganglist_id);
		g_UpArrow = false;
		return true;
	}
	if (g_DownArrow) 
	{
		selection = ArrowDownListBox(ganglist_id);
		g_DownArrow = false;
		return true;
	}
	if (g_AltKeys)
	{
		if (g_A_Key)
		{
			selection = ArrowUpListBox(ganglist_id);
			g_A_Key = false;
			return true;
		}
		if (g_D_Key)
		{
			selection = ArrowDownListBox(ganglist_id);
			g_D_Key = false;
			return true;
		}
		if (g_W_Key)
		{
			selection = ArrowUpListBox(missionlist_id);
			g_W_Key = false;
			return true;
		}
		if (g_S_Key)
		{
			selection = ArrowDownListBox(missionlist_id);
			g_S_Key = false;
			return true;
		}
		if (g_Q_Key)
		{
			selection = ArrowUpListBox(recruitlist_id);
			g_Q_Key = false;
			return true;
		}
		if (g_E_Key)
		{
			selection = ArrowDownListBox(recruitlist_id);
			g_E_Key = false;
			return true;
		}
	}
	if (g_SpaceKey)
	{
		g_SpaceKey = false;
		hire_recruitable();
		return true;
	}
	return false;
}


void cScreenGangs::check_events()
{
	int buynets = 0;
	int buypots = 0;
	stringstream ss;

	if (g_InterfaceEvents.GetNumEvents() == 0) return;	// no events means we can go home

	// if it's the back button, pop the window off the stack and we're done
	if (g_InterfaceEvents.CheckButton(back_id))
	{
		g_InitWin = true;
		g_WinManager.Pop();
		return;
	}
	if (g_InterfaceEvents.CheckButton(ganghire_id))
	{
		hire_recruitable();
		return;
	}
	if (g_InterfaceEvents.CheckButton(weaponup_id))
	{
		int cost = 0;
		int *wlev = g_Gangs.GetWeaponLevel();
		cost = tariff.goon_weapon_upgrade(*wlev);
		if (g_Gold.item_cost(cost) == true)
		{
			*wlev += 1;
			g_InitWin = true;
		}
		wlev = 0;
		return;
	}

	if (g_InterfaceEvents.CheckButton(netbuy_id))	buynets = 1;
	if (g_InterfaceEvents.CheckButton(netbuy10_id))	buynets = 10;
	if (g_InterfaceEvents.CheckButton(netbuy20_id))	buynets = 20;
	if (buynets > 0)
	{
		int cost = 0;
		int amount = buynets;
		int *nets = g_Gangs.GetNets();
		if (((*nets) + buynets) > 60) amount = 60 - (*nets);
		cost = tariff.nets_price(amount);
		if (g_Gold.item_cost(cost) == true)
		{
			*nets += amount;
			if (IsCheckboxOn(netautobuy_id)) g_Gangs.KeepNetStocked(*nets);
			g_InitWin = true;
		}
		nets = 0;
		buynets = 0;
		return;
	}

	if (g_InterfaceEvents.CheckButton(healbuy_id))		buypots = 1;
	if (g_InterfaceEvents.CheckButton(healbuy10_id))	buypots = 10;
	if (g_InterfaceEvents.CheckButton(healbuy20_id))	buypots = 20;
	if (buypots > 0)
	{
		int cost = 0;
		int amount = buypots;
		int *potions = g_Gangs.GetHealingPotions();
		if (((*potions) + buypots) > 200)	amount = 200 - (*potions);
		cost = tariff.healing_price(amount);
		if (g_Gold.item_cost(cost) == true)
		{
			*potions += amount;
			if (IsCheckboxOn(healautobuy_id)) g_Gangs.KeepHealStocked(*potions);
			g_InitWin = true;
		}
		potions = 0;
		buypots = 0;
		return;
	}
	if (g_InterfaceEvents.CheckCheckbox(netautobuy_id))
	{
		int *nets = g_Gangs.GetNets();
		g_Gangs.KeepNetStocked(IsCheckboxOn(netautobuy_id) ? *nets : 0);
	}
	if (g_InterfaceEvents.CheckCheckbox(healautobuy_id))
	{
		int *potions = g_Gangs.GetHealingPotions();
		g_Gangs.KeepHealStocked(IsCheckboxOn(healautobuy_id) ? *potions : 0);
	}
	if (g_InterfaceEvents.CheckButton(gangfire_id))
	{
		selection = GetLastSelectedItemFromList(ganglist_id);
		if (selection != -1)
		{
			g_Gangs.FireGang(selection);
			g_InitWin = true;
		}
		return;
	}
	if (g_InterfaceEvents.CheckListbox(recruitlist_id))
	{
		string ClickedHeader = HeaderClicked(recruitlist_id);
		if (ClickedHeader != "")
		{
			cerr << "User clicked \"" << ClickedHeader << "\" column header on Recruit listbox" << endl;
			return;
		}

		cerr << "selected recruitable gang changed" << endl;
		sel_recruit = GetLastSelectedItemFromList(recruitlist_id);

		if (ListDoubleClicked(recruitlist_id))
		{
			cerr << "User double-clicked recruitable gang! Hiring if possible." << endl;
			hire_recruitable();
			return;
		}
		//		g_InitWin = true;
	}
	// this is what gets called it you change the selected gang
	if (g_InterfaceEvents.CheckListbox(ganglist_id))
	{
		string ClickedHeader = HeaderClicked(ganglist_id);
		if (ClickedHeader != "")
		{
			cerr << "User clicked \"" << ClickedHeader << "\" column header on Gangs listbox" << endl;
			return;
		}

		cerr << "selected gang changed" << endl;
		selection = GetLastSelectedItemFromList(ganglist_id);
		if (selection != -1)
		{
			sGang* gang = g_Gangs.GetGang(selection);

			ss.str("");
			ss << gettext("Name: ") << gang->m_Name << "\n"
				<< gettext("Number: ") << gang->m_Num << "\n"
				<< gettext("Combat: ") << gang->m_Skills[SKILL_COMBAT] << "%\n"
				<< gettext("Magic: ") << gang->m_Skills[SKILL_MAGIC] << "%\n"
				<< gettext("Intelligence: ") << gang->m_Stats[STAT_INTELLIGENCE] << "%\n";
			EditTextItem(ss.str(), gangdesc_id);

			SetSelectedItemInList(missionlist_id, gang->m_MissionID, false);
			/*
			*				set the long description for the mission
			*/
			set_mission_desc(gang->m_MissionID);
		}
	}
	if (g_InterfaceEvents.CheckListbox(missionlist_id))
	{
		/*
		*			get the index into the missions list
		*/
		int mission_id = GetLastSelectedItemFromList(missionlist_id);
		cerr << "selchange: mid = " << mission_id << endl;
		/*
		*			set the textfield with the long description and price
		*			for this mission
		*/
		set_mission_desc(mission_id);

		cerr << "selection change: rebuilding gang list box" << endl;

		for (int	selection = multi_first();
			selection != -1;
			selection = multi_next()
			) {
			sGang* gang = g_Gangs.GetGang(selection);
			/*
			*				make sure we found the gang - pretty catastrophic
			*				if not, so log it if we do
			*/
			if (gang == 0)
			{
				g_LogFile.ss() << "Error: No gang for index " << selection;
				g_LogFile.ssend();
				continue;
			}
			/*
			*				if the mission id is -1, nothing else to do
			*				(moving this to before the recruitment check
			*				since -1 most likely means nothing selected in
			*				the missions list)
			*/
			if (mission_id == -1) { continue; }
			/*
			*				if the gang is already doing <whatever>
			*				then let them get on with it
			*/
			if (gang->m_MissionID == u_int(mission_id)) { continue; }
			/*
			*				if they were recruiting, turn off the
			*				auto-recruit flag
			*/
			if (gang->m_MissionID == MISS_RECRUIT && gang->m_AutoRecruit)
			{
				gang->m_AutoRecruit = false;
				gang->m_LastMissID = -1;
			}
			gang->m_MissionID = mission_id;
			/*
			*				format the display line
			*/
			string Data[6];
			ss.str("");	ss << gang->m_Name;								Data[0] = ss.str();
			ss.str("");	ss << gang->m_Num;								Data[1] = ss.str();
			ss.str("");	ss << short_mission_desc(mission_id);			Data[2] = ss.str();
			ss.str("");	ss << gang->m_Skills[SKILL_COMBAT] << "%";		Data[3] = ss.str();
			ss.str("");	ss << gang->m_Skills[SKILL_MAGIC] << "%";		Data[4] = ss.str();
			ss.str("");	ss << gang->m_Stats[STAT_INTELLIGENCE] << "%";	Data[5] = ss.str();

			//		cerr << "Gang:\t" << Data[0] << "\t" << Data[1] << "\t" << Data[2]
			//			<< "\t" << Data[3] << "\t" << Data[4] << "\t" << Data[5] << "\t" << Data[6] << endl;
			//
			//			cerr << "        index " << mission_id << ": " << ss.str() << endl;
			/*
			*				and add it to the list
			*/
			SetSelectedItemText(ganglist_id, selection, Data, 6);
		}

		ss.str("");
		int cost = 0;
		if (g_Gangs.GetNumGangs() > 0)
		{
			for (int i = 0; i < g_Gangs.GetNumGangs(); i++)
			{
				sGang* g = g_Gangs.GetGang(i);
				cost += tariff.goon_mission_cost(g->m_MissionID);
			}
		}
		ss << gettext("Weekly Cost: ") << cost;
		EditTextItem(ss.str(), totalcost_id);
	}
}


string cScreenGangs::mission_desc(int mid)
{
	switch (mid) {
	case MISS_GUARDING:		return gettext("Your men will guard your property.");
	case MISS_SABOTAGE:		return gettext("Your men will move about town and destroy and loot enemy businesses.");
	case MISS_SPYGIRLS:		return gettext("Your men will spy on your working girls, looking for the ones who take extra for themselves.");
	case MISS_CAPTUREGIRL:	return gettext("Your men will set out to re-capture any girls who have recently run away.");
	case MISS_EXTORTION:	return gettext("Sends your men out to force local gangs out of their areas of town.");
	case MISS_PETYTHEFT:	return gettext("Your men will mug people in the street.");
	case MISS_GRANDTHEFT:	return gettext("Your men will attempt to rob a bank or other risky place with high rewards.");
	case MISS_KIDNAPP:		return gettext("Your men will kidnap beggar, homeless or lost girls from the street and also lure other girls into working for you.");
	case MISS_CATACOMBS:	return gettext("Your men will explore the catacombs for treasure.");
	case MISS_TRAINING:		return gettext("Your men will improve their skills slightly.");
	case MISS_RECRUIT:		return gettext("Your men will replace their missing men (up to 15).");
	default:
		break;
	}

	stringstream ss;
	ss << "Error: unexpected mission ID: " << mid;
	return ss.str();
}


string cScreenGangs::short_mission_desc(int mid)
{
	cerr << "short_mission_desc(" << mid << ")" << endl;
	switch (mid)
	{
	case MISS_GUARDING:		return gettext("Guarding");
	case MISS_SABOTAGE:		return gettext("Sabotaging");
	case MISS_SPYGIRLS:		return gettext("Watching Girls");
	case MISS_CAPTUREGIRL:	return gettext("Finding escaped girls");
	case MISS_EXTORTION:	return gettext("Acquiring Territory");
	case MISS_PETYTHEFT:	return gettext("Mugging people");
	case MISS_GRANDTHEFT:	return gettext("Robbing places");
	case MISS_KIDNAPP:		return gettext("Kidnapping Girls");
	case MISS_CATACOMBS:	return gettext("Exploring Catacombs");
	case MISS_TRAINING:		return gettext("Training Skills");
	case MISS_RECRUIT:		return gettext("Recruiting Men");
	default:				return gettext("Error: Unknown");
	}
}

int cScreenGangs::set_mission_desc(int mid)
{
	stringstream ss;
	/*
	*	OK: get the difficulty-adjusted price for this mission
	*/
	int price = tariff.goon_mission_cost(mid);
	/*
	*	and get a description of the mission
	*/
	string desc = mission_desc(mid);
	/*
	*	stick 'em both together ...
	*/
	ss.str("");
	ss << desc << " (" << price << "g)";
	/*
	*	... and set the text field
	*/
	EditTextItem(ss.str(), missiondesc_id);
	/*
	*	return the mission price
	*/
	return price;
}

void cScreenGangs::hire_recruitable()
{
	if ((g_Gangs.GetNumGangs() >= g_Gangs.GetMaxNumGangs()) || (sel_recruit == -1)) return;
	g_Gangs.HireGang(sel_recruit);
	g_InitWin = true;
}
