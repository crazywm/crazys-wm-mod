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

bool cJobManager::WorkCombatTraining(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	cTariff tariff;

	string message = "";
	if(Preprocessing(ACTION_COMBAT, girl, brothel, DayNight, summary, message))
		return true;
	stringstream ss;
	ss.str(message);

	int roll = g_Dice%100;
	int skill = 0;
	{
		if (roll <= 15)
		{
			skill = 5;
		}
		else if (roll <= 35)
		{
			skill = 4;
		}
		else if (roll <= 60)
		{
			skill = 3;
		}
		else
		{
			skill = 2;
		}

	if (g_Girls.HasTrait(girl, "Quick Learner"))
	{
		skill += 1;
	}
	else if (g_Girls.HasTrait(girl, "Slow Learner"))
	{
		skill -= 1;
	}

	ss << gettext("She trains in combat for the day.\n\n");
	//message = "She trains in combat for the day.\n\n";

	if (roll <= 33 && (int)girl->m_Skills[SKILL_COMBAT] < 100)
		{
			ss << gettext("She learns how to fight better with her weapons.\n");
			ss << gettext("She managed to gain ") << skill << gettext(" Combat.\n\n");
			g_Girls.UpdateSkill(girl, SKILL_COMBAT, skill);
		}
		else if (roll <= 66 && (int)girl->m_Skills[SKILL_MAGIC] < 100)
		{
			ss << gettext("She learns how to cast better magic.\n");
			ss << gettext("She managed to gain ") << skill << gettext(" Magic.\n\n");
			g_Girls.UpdateSkill(girl, SKILL_MAGIC, skill);
		}
		else if (roll <= 95 && (int)girl->m_Stats[STAT_AGILITY] < 100)
		{
			ss << gettext("She worked her speed today.\n\n");
			ss << gettext("She managed to gain ") << skill << gettext(" Agility.\n\n");
			g_Girls.UpdateStat(girl, STAT_AGILITY, skill);
		}
		else if (roll > 95 && (int)girl->m_Stats[STAT_CONSTITUTION] < 100)
		{
			ss << gettext("She has gotten tougher from the training.\n\n");
			ss << gettext("She managed to gain ") << skill << gettext(" Constituion.\n\n");
			g_Girls.UpdateStat(girl, STAT_CONSTITUTION, skill);
		}
		}
		
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_COMBAT, DayNight);
	girl->m_Pay = 25;

	// Improve stats
	int xp = 10;

	if (g_Girls.HasTrait(girl, "Quick Learner"))
	{
		xp += 3;
	}
	else if (g_Girls.HasTrait(girl, "Slow Learner"))
	{
		xp -= 3;
	}

	g_Girls.UpdateStat(girl, STAT_EXP, xp);



	return false;
}
