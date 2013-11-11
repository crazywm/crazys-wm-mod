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

bool cJobManager::WorkMilk(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";
	int num_items = 0;
	void AddItem(sInventoryItem* item);
	sInventoryItem* GetItem(string name);

	if(Preprocessing(ACTION_GENERAL, girl, brothel, DayNight, summary, message))	// they refuse to have work
		return true;

	// put that shit away, you'll scare off the customers!
	g_Girls.UnequipCombat(girl);

	girl->m_Pay += 15;
		message = " She let her breasts be milked.";

	if (g_Girls.HasTrait(girl, "Small Boobs"))
	{
		message += " She has rather small breasts so you got less milk.";
		girl->m_Pay += 25;
	}
	else if (g_Girls.HasTrait(girl, "Big Boobs"))
		{
		message += " She has rather big breasts so you got more milk.";
		girl->m_Pay += 35;
	}
	else if (g_Girls.HasTrait(girl, "Abnormally Large Boobs"))
		{
		message += "She has abnormally large breasts so you got a lot more milk.";
		girl->m_Pay += 40;
	}
	else
	{
		message += " She has normal sized breasts so you get a decent amount of milk.";
		girl->m_Pay += 30;
	}
	if (girl->m_States&(1<<STATUS_PREGNANT) || girl->m_States&(1<<STATUS_PREGNANT_BY_PLAYER))
	{
		message += " Cause she is pregnant so she is able to produce a lot more milk.";
		girl->m_Pay += 100;
	}

	girl->m_Events.AddMessage(message, IMGTYPE_MILK, DayNight);

	// Improve stats
	int xp = 5, libido = 1, skill = 3;

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

	if (g_Girls.HasTrait(girl, "Nymphomaniac"))
		libido += 2;

	g_Girls.UpdateStat(girl, STAT_FAME, 1);
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, skill);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);
	
return false;
}