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
#include "cJobManager.h"
#include "cBrothel.h"
#include "cClinic.h"
#include "cMovieStudio.h"
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
extern cMovieStudioManager g_Studios;
extern cArenaManager g_Arena;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;

bool cJobManager::WorkCleanArena(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";
	
	
	char buffer[1000];
	string girlName = girl->m_Realname;

	g_Girls.UnequipCombat(girl);
	
	girl->m_Pay += 30;
	
	message = girlName;	
	message += gettext(" worked cleaning your house.\n\n");

	
	int roll = g_Dice%100;
	if (roll <= 10 && g_Girls.DisobeyCheck(girl, ACTION_WORKCLEANING, brothel))
	{
		message = girl->m_Realname + gettext(" refused to clean your house.");
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else if(roll <= 15) {
		message += gettext(" She did not like cleaning your house today.\n\n");
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKCLEANING, -1, true);
		girl->m_Events.AddMessage(message, IMGTYPE_MAID, DayNight);
	}
	else if(roll >=90)
	{
		message += gettext(" She had a great time working today.\n\n");
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKCLEANING, +3, true);
		girl->m_Events.AddMessage(message, IMGTYPE_MAID, DayNight);
	}
	else
	{
		message += gettext(" Otherwise, the shift passed uneventfully.\n\n");
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKCLEANING, +1, true);
		girl->m_Events.AddMessage(message, IMGTYPE_MAID, DayNight);
	}
	
/*
 *	work out the pay between the house and the girl
 */
	int roll_max = girl->spirit() + girl->intelligence();
	roll_max /= 4;
	girl->m_Pay += 10 + g_Dice%roll_max;
	g_Gold.building_upkeep(girl->m_Pay);  // wages come from you

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
	int xp = 5, skill = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))
	{
		skill += 1;
		xp += 3;
	}
	else if (g_Girls.HasTrait(girl, "Slow Learner"))
	{
		skill -= 1;
		xp -= 3;
	}

	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, skill);

	g_Girls.PossiblyLoseExistingTrait(girl, "Clumsy", 20, ACTION_WORKCLEANING, "It took her spilling hundreds of buckets, and just as many reprimands, but " + girl->m_Realname + " has finally stopped being so Clumsy.", DayNight != 0);
	return false;
}
