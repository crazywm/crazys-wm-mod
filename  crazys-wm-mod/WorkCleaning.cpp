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
#include "cJobManager.h"
#include "cBrothel.h"
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
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;

bool cJobManager::WorkCleaning(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";
	if(Preprocessing(ACTION_WORKCLEANING, girl, brothel, DayNight, summary, message))
		return true;

	// put that shit away, you'll scare off the customers!
	g_Girls.UnequipCombat(girl);

	// Complications
	if(g_Dice%100 <= 10)
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKCLEANING, -2, true);
		message = gettext("Spilled a bucket of something unpleasant all over herself.");
		girl->m_Events.AddMessage(message, IMGTYPE_MAID, DayNight);
	}
	else
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKCLEANING, +3, true);
		message = gettext("Cleaned the building while humming a pleasant tune.");
		girl->m_Events.AddMessage(message, IMGTYPE_MAID, DayNight);
	}
	
	// cleaning is a service skill
	int CleanAmt;
	if(g_Girls.GetSkill(girl, SKILL_SERVICE) >= 10)
		CleanAmt = ((g_Girls.GetSkill(girl, SKILL_SERVICE)/10)+5) * 10;
	else
	   CleanAmt = 50;

	brothel->m_Filthiness -= CleanAmt;
	stringstream sstemp;
    sstemp << gettext("Cleanliness rating improved by ") << CleanAmt;
	girl->m_Events.AddMessage(sstemp.str(), IMGTYPE_MAID, DayNight);

	// Improve girl
	int xp = 5, libido = 1, skill = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	int pay = 50;
	if (CleanAmt >= 125)		{ pay += 100; }
	else if (CleanAmt >= 60)	{ pay += 50; }

	girl->m_Pay += pay;
	g_Gold.building_upkeep(pay);  // wages come from you
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, skill);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Clumsy", 20, ACTION_WORKCLEANING, "It took her spilling hundreds of buckets, and just as many reprimands, but " + girl->m_Realname + " has finally stopped being so Clumsy.", DayNight != 0);
	
	return false;
}
