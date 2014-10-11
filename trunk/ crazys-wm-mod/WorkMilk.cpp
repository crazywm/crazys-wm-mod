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

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;

// `J` Farm Job - Laborers
bool cJobManager::WorkMilk(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";
	string girlName = girl->m_Realname;
	int num_items = 0;
	void AddItem(sInventoryItem* item);
	sInventoryItem* GetItem(string name);

	if(Preprocessing(ACTION_WORKMILK, girl, brothel, DayNight, summary, message))	// they refuse to have work
		return true;

	// put that shit away, you'll scare off the customers!
	g_Girls.UnequipCombat(girl);

	girl->m_Pay += 15;
		message = "She let her breasts be milked.\n\n";

	if (g_Girls.HasTrait(girl, "Small Boobs"))
	{
		if (girl->m_States&(1<<STATUS_PREGNANT) || girl->m_States&(1<<STATUS_PREGNANT_BY_PLAYER))
		{
			message += girl->m_Realname + " has small breasts, but her body still gives plenty of milk in anticipation of nursing!.";
			girl->m_Pay += 125;
		}
		else
		{
		message += girl->m_Realname + " has small breasts, which only yield a small amount of milk.";
		girl->m_Pay += 25;
		}
	}
	else if (g_Girls.HasTrait(girl, "Big Boobs"))
		{
			if (girl->m_States&(1<<STATUS_PREGNANT) || girl->m_States&(1<<STATUS_PREGNANT_BY_PLAYER))
			{
			message += girl->m_Realname + "'s already sizable breasts have become fat and swollen with milk in preparation for her child.";
			girl->m_Pay += 135;
		}
		else
		{
		message += girl->m_Realname + " has large breasts, that yield a good amount of milk to the suction machine even without pregnancy.";
		girl->m_Pay += 35;
		}
	}
	else if (g_Girls.HasTrait(girl, "Abnormally Large Boobs"))
		{
		if (girl->m_States&(1<<STATUS_PREGNANT) || girl->m_States&(1<<STATUS_PREGNANT_BY_PLAYER))
			{
			message += girl->m_Realname + " has ridiculously large breasts, even without a baby in development.  With a bun in the oven, her tits are each larger than her head, and leak milk near continuously.";
			girl->m_Pay += 140;
		}
		else
		{
		message += girl->m_Realname + "'s massive globes don't need pregnancy to yield a profitable quantity of milk!";
		girl->m_Pay += 40;
			}
		}
	else
	{
		if (girl->m_States&(1<<STATUS_PREGNANT) || girl->m_States&(1<<STATUS_PREGNANT_BY_PLAYER))
		{
			message += girl->m_Realname + " has average sized breasts, which yield a fair amount of milk with the help of pregnancy.";
			girl->m_Pay += 130;
		}
		else
		{
		message += girl->m_Realname + " has average sized breasts, perfect handfuls, which yield an okay amount of milk.";
		girl->m_Pay += 30;
		}
	}

	girl->m_Events.AddMessage(message, IMGTYPE_MILK, DayNight);

	// Improve stats
	int xp = 15, libido = 1, skill = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_FAME, 1);
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, skill);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);
	
return false;
}