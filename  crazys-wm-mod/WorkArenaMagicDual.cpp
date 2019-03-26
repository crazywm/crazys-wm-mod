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

#if 0
#include "cJobManager.h"
#include "cBrothel.h"
#include "cArena.h"
#include "cCustomers.h"
#include "cRng.h"
#include "cInventory.h"
#include "sConfig.h"
#include "cRival.h"
#include <sstream>
#include "CLog.h"
#include "cTrainable.h"
#include "cTariff.h"
#include "cGold.h"
#include "cGangs.h"
#include "cMessageBox.h"
#include "libintl.h"

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cArenaManager g_Arena;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;

// `J` Job Arena - Fighting
bool cJobManager::WorkArenaMagicDual(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_COMBAT;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	if (girl->disobey_check(actiontype, brothel))			// they refuse to work
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}

	int imagetype = IMGTYPE_MAGIC;
	int roll = g_Dice.d100();

	enum magicdualchoice
		{
			MD_FireWorks,
			MD_Fight,
			MD_Nothing,			// do nothing special


			MD_NumberOfMagicDualChoices	// Leave this as the last thing on the list to allow for random choices.
		};								// When the choice gets handled, the switch will use the "MD_name" as the case.

		// the test for if the girl can act on that choice is done next
		int choice = 0;	bool choicemade = false;

		choice = g_Dice % MD_NumberOfMagicDualChoices;	// randomly choose from all of the choices
		switch (choice)
		{
			// these don't need a test
			case MD_Nothing:
				choicemade = true;	// ready so continue
					break;

			case MD_Nothing:
			default:
			choice = MD_Nothing;
			choicemade = true;	// ready so continue
			break;
		}

		case MD_FireWorks:
			break;	// end FT_Quest

		case MD_Fight:
			break;	// end FT_Quest

		case MD_Nothing:
			break;	// end FT_Quest


	return false;
}

double cJobManager::JP_ArenaMagicDual(sGirl* girl, bool estimate)// not used
{
	double jobperformance = 0.0;

	if (estimate)// for third detail string
	{
	}
	else// for the actual check
	{
	}
	return jobperformance;
}
#endif

