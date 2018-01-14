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
#include "cScreenHouseDetails.h"
#include "cWindowManager.h"
#include "cGold.h"
#include "cGetStringScreenManager.h"
#include "InterfaceGlobals.h"
#include "cGangs.h"

extern bool g_InitWin;
extern int g_CurrBrothel;
extern cGold g_Gold;
extern cBrothelManager g_Brothels;
extern cWindowManager g_WinManager;
extern cInterfaceEventManager g_InterfaceEvents;
extern long g_IntReturn;
extern cGangManager g_Gangs;
extern bool g_Cheats;
extern	int	g_TalkCount;

extern	int		g_CurrentScreen;

static string fmt_objective(stringstream &ss, string desc, int limit, int sofar = -1)
{
	ss << desc;
	if (limit != -1) { ss << " in " << limit << " weeks"; }
	if (sofar > -1) { ss << ", " << sofar << " acquired so far"; }
	ss << ".";
	return ss.str();
}

bool cScreenHouseDetails::ids_set = false;
cScreenHouseDetails::cScreenHouseDetails()
{
	DirPath dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "house_screen.xml";
	m_filename = dp.c_str();
}
cScreenHouseDetails::~cScreenHouseDetails() {}

void cScreenHouseDetails::set_ids()
{
	ids_set			/**/ = true;
	g_LogFile.write("set_ids in cScreenHouseDetails");
	header_id		/**/ = get_id("ScreenHeader");
	interact_id		/**/ = get_id("InteractText");
	interactb_id	/**/ = get_id("BuyInteract");
	interactb10_id	/**/ = get_id("BuyInteract10");
	slavedate_id	/**/ = get_id("SlaveDate", "*Unused*");//
	details_id		/**/ = get_id("HouseDetails");
	back_id			/**/ = get_id("BackButton", "Back");
}

void cScreenHouseDetails::process()
{
	if (!ids_set) set_ids();										// we need to make sure the ID variables are set
	init();															// set up the window if needed
	if (g_InterfaceEvents.GetNumEvents() != 0)	check_events();		// check to see if there's a button event needing handling
}

void cScreenHouseDetails::init()
{
	g_CurrentScreen = SCREEN_HOUSE;
	if (!g_InitWin) return;
	Focused();
	g_InitWin = false;

	stringstream ss;
	ss << "CURRENT OBJECTIVE: ";
	sObjective* obj = g_Brothels.GetObjective();
	if (obj)
	{
		switch (obj->m_Objective)
		{
		case OBJECTIVE_REACHGOLDTARGET:
			ss << "End the week with " << obj->m_Target << " gold in the bank";
			if (obj->m_Limit != -1) ss << " within " << obj->m_Limit << " weeks";
			ss << ", " << g_Brothels.GetBankMoney() << " gathered so far.";
			break;
		case OBJECTIVE_GETNEXTBROTHEL:
			fmt_objective(ss, "Purchase the next brothel", obj->m_Limit);
			break;
			/*----
			case OBJECTIVE_PURCHASENEWGAMBLINGHALL:
			fmt_objective(ss, "Purchase a gambling hall", obj->m_Limit);
			break;
			case OBJECTIVE_PURCHASENEWBAR:
			fmt_objective(ss, "Purchase a bar", obj->m_Limit);
			break;
			----*/
		case OBJECTIVE_LAUNCHSUCCESSFULATTACK:
			fmt_objective(ss, "Launch a successful attack", obj->m_Limit);
			break;
		case OBJECTIVE_HAVEXGOONS:
			ss << "Have " << obj->m_Target << " gangs";
			fmt_objective(ss, "", obj->m_Limit);
			break;
		case OBJECTIVE_STEALXAMOUNTOFGOLD:
			ss << "Steal " << obj->m_Target << " gold";
			fmt_objective(ss, "", obj->m_Limit, obj->m_SoFar);
			break;
		case OBJECTIVE_CAPTUREXCATACOMBGIRLS:
			ss << "Capture " << obj->m_Target << " girls from the catacombs";
			fmt_objective(ss, "", obj->m_Limit, obj->m_SoFar);
			break;
		case OBJECTIVE_HAVEXMONSTERGIRLS:
			ss << "Have a total of " << obj->m_Target << " monster (non-human) girls";
			fmt_objective(ss, "", obj->m_Limit, g_Brothels.GetTotalNumGirls(true));
			break;
		case OBJECTIVE_KIDNAPXGIRLS:
			ss << "Kidnap " << obj->m_Target << " girls from the streets";
			fmt_objective(ss, "", obj->m_Limit, obj->m_SoFar);
			break;
		case OBJECTIVE_EXTORTXNEWBUSINESS:
			ss << "Control " << obj->m_Target << " city business";
			fmt_objective(ss, "", obj->m_Limit, obj->m_SoFar);
			break;
		case OBJECTIVE_HAVEXAMOUNTOFGIRLS:
			ss << "Have a total of " << obj->m_Target << " girls";
			fmt_objective(ss, "", obj->m_Limit, g_Brothels.GetTotalNumGirls(false));
			break;
		}
	}
	else ss << "NONE\n";

	ss << "\nCurrent gold: " << g_Gold.ival()
		<< "\nBank account: " << g_Brothels.GetBankMoney()
		<< "\nBusinesses controlled: " << g_Gangs.GetNumBusinessExtorted()
		<< "\n \nCurrent number of runaways: " << g_Brothels.GetNumRunaways() << "\n";
	//	`J` added while loop to add runaway's names to the list 
	if (g_Brothels.GetNumRunaways() > 0)
	{
		sGirl* rgirl = g_Brothels.m_Runaways;
		while (rgirl)
		{
			ss << rgirl->m_Realname << " (" << rgirl->m_RunAway << ")";
			rgirl = rgirl->m_Next;
			if (rgirl)	ss << " ,   ";
		}
	}
	EditTextItem(ss.str(), details_id);
	if (interact_id >= 0)
	{
		ss.str(""); ss << "Interactions Left: ";
		if (g_Cheats) ss << "\nInfinate Cheat";
		else ss << g_TalkCount << "\nBuy more for 1000 each.";
		EditTextItem(ss.str(), interact_id);
	}
	if (interactb_id >= 0)		DisableButton(interactb_id, g_Cheats || g_Gold.ival() < 1000);
	if (interactb10_id >= 0)	DisableButton(interactb10_id, g_Cheats || g_Gold.ival() < 10000);
	obj = 0;
}

void cScreenHouseDetails::check_events()
{
	if (g_InterfaceEvents.CheckButton(back_id))			{ g_InitWin = true; g_WinManager.Pop(); return; }
	if (g_InterfaceEvents.CheckButton(interactb_id))	{ if (g_Gold.misc_debit(1000))	g_TalkCount++; }
	if (g_InterfaceEvents.CheckButton(interactb10_id))	{ if (g_Gold.misc_debit(10000))	g_TalkCount += 10; }
	g_InitWin = true;
}