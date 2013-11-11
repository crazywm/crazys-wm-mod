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
#include "cScreenHouse.h"
#include "cWindowManager.h"
#include "cGold.h"
#include "cGetStringScreenManager.h"
#include "InterfaceGlobals.h"
#include "cGangs.h"
#include "libintl.h"

extern bool g_InitWin;
extern int g_CurrBrothel;
extern cGold g_Gold;
extern cBrothelManager g_Brothels;
extern cWindowManager g_WinManager;
extern cInterfaceEventManager g_InterfaceEvents;
extern long g_IntReturn;
extern cGangManager g_Gangs;

extern void GetString();
extern cInterfaceWindow g_GetString;
extern void GetInt();
extern cInterfaceWindow g_GetInt;

extern	int		g_CurrentScreen;

static string fmt_objective(stringstream &ss, string desc, int limit, int sofar=-1)
{
	ss << desc;
	if(limit != -1) {
		ss << gettext(" in ") << limit << gettext(" weeks");
	}
	if(sofar > -1) {
		ss << gettext(", ") << sofar << gettext(" acquired so far");
	}
	ss << gettext(".");
	return ss.str();
}


bool cScreenHouse::ids_set = false;

void cScreenHouse::set_ids()
{
	ids_set = true;
	back_id	= get_id("BackButton");
	details_id = get_id("HouseDetails");
	header_id = get_id("ScreenHeader");
}

void cScreenHouse::init()
{
	g_CurrentScreen = SCREEN_HOUSE;
	if(!g_InitWin) {
		return;
	}
	Focused();
	g_InitWin = false;

	locale syslocale("");
	stringstream ss;
	ss.imbue(syslocale);

	ss << gettext("CURRENT OBJECTIVE: ");
	sObjective* obj = g_Brothels.GetObjective();
	if(obj)
	{
		switch(obj->m_Objective) {
		case OBJECTIVE_REACHGOLDTARGET:
			ss << gettext("Gather ") << obj->m_Target << gettext(" gold");
			if(obj->m_Limit != -1) {
				ss << gettext(" in ") << obj->m_Limit << gettext(" weeks");
			}
			ss << gettext(", ") << g_Gold.ival() << gettext(" gathered so far.");
			break;
		case OBJECTIVE_GETNEXTBROTHEL:
			fmt_objective(ss, gettext("Purchase the next brothel"), obj->m_Limit);
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
			fmt_objective(ss, gettext("Launch a successful attack"), obj->m_Limit);
			break;
		case OBJECTIVE_HAVEXGOONS:
			ss << gettext("Have ") << obj->m_Target << gettext(" gangs");
			fmt_objective(ss, "", obj->m_Limit);
			break;
		case OBJECTIVE_STEALXAMOUNTOFGOLD:
			ss << gettext("Steal ") << obj->m_Target << gettext(" gold");
			fmt_objective(ss, "", obj->m_Limit, obj->m_SoFar);
			break;
		case OBJECTIVE_CAPTUREXCATACOMBGIRLS:
			ss << gettext("Capture ") << obj->m_Target << gettext(" girls from the catacombs");
			fmt_objective(ss, "", obj->m_Limit, obj->m_SoFar);
			break;
		case OBJECTIVE_HAVEXMONSTERGIRLS:
			ss << gettext("Have a total of ") << obj->m_Target << gettext(" monster (non-human) girls");
			fmt_objective(ss, "", obj->m_Limit, g_Brothels.GetTotalNumGirls(true));
			break;
		case OBJECTIVE_KIDNAPXGIRLS:
			ss << gettext("Kidnap ") << obj->m_Target << gettext(" girls from the streets");
			fmt_objective(ss, "", obj->m_Limit, obj->m_SoFar);
			break;
		case OBJECTIVE_EXTORTXNEWBUSINESS:
			ss << gettext("Control ") << obj->m_Target << gettext(" city business");
			fmt_objective(ss, "", obj->m_Limit, obj->m_SoFar);
			break;
		case OBJECTIVE_HAVEXAMOUNTOFGIRLS:
			ss << gettext("Have a total of ") << obj->m_Target << gettext(" girls");
			fmt_objective(ss, "", obj->m_Limit, g_Brothels.GetTotalNumGirls(false));
			break;
		}
	}
	else ss << gettext("NONE\n");

	ss << gettext("\nCurrent number of runaways: ")
	   << g_Brothels.GetNumRunaways()
	   << gettext("\n")
	;

	ss << gettext("\n")
	   << gettext("Current gold: ") << g_Gold.ival() << gettext("\n")
	   << gettext("Bank account: ") << g_Brothels.GetBankMoney() << gettext("\n")
	   << gettext("Businesses controlled: ")
	   << g_Gangs.GetNumBusinessExtorted()
	   << gettext("\n")
	;

	EditTextItem(ss.str(), details_id);
	obj = 0;
}

void cScreenHouse::process()
{
/*
 *	we need to make sure the ID variables are set
 */
	if(!ids_set)
		set_ids();

/*
 *	set up the window if needed
 */
	init();

/*
 *	check to see if there's a button event needing handling
 */
	check_events();
}


void cScreenHouse::check_events()
{
/* 
 *	no events means we can go home
 */
	if(g_InterfaceEvents.GetNumEvents() == 0)
		return;

/*
 *	if it's the back button, pop the window off the stack
 *	and we're done
 */
	if(g_InterfaceEvents.CheckButton(back_id)) {
		g_InitWin = true;
		g_WinManager.Pop();
		return;
	}

}
