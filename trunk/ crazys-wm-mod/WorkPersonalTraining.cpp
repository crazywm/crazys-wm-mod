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
#include "cHouse.h"
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
extern cHouseManager g_House;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;
extern int g_Building;

bool cJobManager::WorkPersonalTraining(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";
	int tex = g_Dice%4;

	if(Preprocessing(ACTION_SEX, girl, brothel, DayNight, summary, message))	// they refuse to have work
		return true;

	// put that shit away, not needed for sex training
	g_Girls.UnequipCombat(girl);

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
		else if (roll <= 55)
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

	message = "You over see her traning for the day.\n\n";

	if (roll <= 20)
		{
			message += "You decied to teach her the art of sucking a cock.\n\n";
			g_Girls.UpdateSkill(girl, SKILL_ORALSEX, skill);
			girl->m_Events.AddMessage(message, IMGTYPE_ORAL, DayNight);
		}
		else if (roll <= 40)
		{
			message += "You decied to over see her skill in a gang bang.\n\n";
			g_Girls.UpdateSkill(girl, SKILL_GROUP, skill);
			girl->m_Events.AddMessage(message, IMGTYPE_GROUP, DayNight);
			if(girl->m_Virgin)
				{
					girl->m_Virgin = false;
					message += "She was a virgin.\n";
				}
			g_Building = BUILDING_HOUSE;
			if(!girl->calc_group_pregnancy(g_Brothels.GetPlayer(), false, 1.0)) {
			g_MessageQue.AddToQue("She has gotten pregnant", 0);
		}
		}
		else if (roll <= 60)
		{
			message += "You decied to teach her the fine art of BDSM.\n\n";
			g_Girls.UpdateSkill(girl, SKILL_BDSM, skill);
			girl->m_Events.AddMessage(message, IMGTYPE_BDSM, DayNight);
			if(girl->m_Virgin)
				{
					girl->m_Virgin = false;
					message += "She was a virgin.\n";
				}
			g_Building = BUILDING_HOUSE;
			if(!girl->calc_pregnancy(g_Brothels.GetPlayer(), false, 1.0)) {
			g_MessageQue.AddToQue("She has gotten pregnant", 0);
	}
		}
		else if (roll <= 80)
		{
			message += "You decied to teach her how to use her ass.\n\n";
			g_Girls.UpdateSkill(girl, SKILL_ANAL, skill);
			girl->m_Events.AddMessage(message, IMGTYPE_ANAL, DayNight);
		}
		else
		{
			message += "You decied to teach her how to ride a dick like a pro.\n\n";
			g_Girls.UpdateSkill(girl, SKILL_NORMALSEX, skill);
			girl->m_Events.AddMessage(message, IMGTYPE_SEX, DayNight);
			if(girl->m_Virgin)
				{
					girl->m_Virgin = false;
					message += "She was a virgin.\n";
				}
			g_Building = BUILDING_HOUSE;
			if(!girl->calc_pregnancy(g_Brothels.GetPlayer(), false, 1.0)) {
			g_MessageQue.AddToQue("She has gotten pregnant", 0);
		}
		}
		

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
}