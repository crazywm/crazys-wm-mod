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
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;

bool cJobManager::WorkFilmAnal(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";
	// No film crew.. then go home
	if (g_Studios.GetNumGirlsOnJob(0,JOB_CAMERAMAGE,false) == 0|| g_Studios.GetNumGirlsOnJob(0,JOB_CRYSTALPURIFIER,false) == 0)
		{
		message = "There was no crew to film the scene, so she took the day off";
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_NOWORK);
		return false;
		}

	string girlName = girl->m_Realname;
	int jobperformance = 0;

	// not for actress
	g_Girls.UnequipCombat(girl);
	/*
	//  ~J~ started 4/28/14
	// attempt to add check for if the girl wants to film a scene with weapon or armor
	double equip_check;
	equip_check = g_Girls.GetSkill(girl, SKILL_COMBAT) + g_Girls.GetSkill(girl, SKILL_MAGIC);
	equip_check = g_Dice%int(equip_check);  // random combat check
	
	int Armor = -1, Weap1 = -1, Weap2 = -1;
	for (int i = 0; i<40; i++)
	{
		if (girl->m_Inventory[i] != 0)
		{
			if (girl->m_Inventory[i]->m_Type == INVWEAPON)
			{
				if (Weap1 == -1)
					Weap1 = i;
				else if (Weap2 == -1)
					Weap2 = i;
				else if (girl->m_Inventory[i]->m_Cost > girl->m_Inventory[Weap1]->m_Cost)
				{
					Weap2 = Weap1;
					Weap1 = i;
				}
				else if (girl->m_Inventory[i]->m_Cost > girl->m_Inventory[Weap2]->m_Cost)
					Weap2 = i;
			}
			if (girl->m_Inventory[i]->m_Type == INVARMOR)
			{
				g_InvManager.Unequip(girl, i);
				if (Armor == -1)
					Armor = i;
				else if (girl->m_Inventory[i]->m_Cost > girl->m_Inventory[Armor]->m_Cost)
					Armor = i;
			}
		}
	}
	if (Armor > -1)
		g_InvManager.Equip(girl, Armor, false);
	if (Weap1 > -1)
		g_InvManager.Equip(girl, Weap1, false);
	if (Weap2 > -1)
		g_InvManager.Equip(girl, Weap2, false);



	if(equip_check_c <= 10)
	{
	message = girlName + " took off her armor before starting the scene.\n";
	message = girlName + " took off her armor before starting the scene.\n";
	message = girlName + " took off her armor before starting the scene.\n";
	message = girlName + " wanted to film her scene with her armor on.\n";
	g_Girls.EquipCombat(girl);

	girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_NOWORK);
	return true;
	}


	g_Girls.EquipCombat(girl);

	*/

	girl->m_Pay += 60;
	message = girlName;
	message += (" worked as an actress filming anal scenes.\n\n");
	
	int roll = g_Dice%100;

	if(roll <= 10 && g_Girls.DisobeyCheck(girl, ACTION_WORKMOVIE, brothel)) 
	{
		message = girlName + " refused to do anal on film today.\n";

		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else if(roll <= 15)
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKMOVIE, -3, true);
		message += girlName + " didn't like having a cock up her ass today.\n\n";
		jobperformance += -5;
	}
	else if(roll >=90)
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKMOVIE, +3, true);
		message += girlName + " loved having her ass pounded today.\n\n";
		jobperformance += +5;
	}
	else
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKMOVIE, +1, true);
		message += girlName + " had a pleasant day letting her co-star slip his cock into her butt.\n\n";
	}
	if(girl->m_Virgin)
	{
		jobperformance += 50;
		message += "She is a virgin.\n";
	}
	jobperformance += g_Dice%4 - 1;	// should add a -1 to +3 random element --PP
	jobperformance += 5; // Modifier for what kind of sex scene it is.. normal sex is the baseline at +0
	// remaining modifiers are in the AddScene function --PP
	string finalqual = g_Studios.AddScene(girl, SKILL_ANAL, jobperformance);
	message += "Her scene us valued at: " + finalqual + " gold.\n";
	g_Girls.UpdateSkill(girl, SKILL_ANAL, 2);

/*
 *	work out the pay between the house and the girl
 *
 *	the original calc took the average of beauty and charisma and halved it
 */
	int roll_max = girl->beauty() + girl->charisma();
	roll_max /= 4;
	girl->m_Pay += 10 + g_Dice%roll_max;

	// Improve stats
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
	girl->m_Events.AddMessage(message, IMGTYPE_ANAL, DayNight);

	g_Girls.PossiblyGainNewTrait(girl, "Fake orgasm expert", 15, ACTION_WORKMOVIE, "She has become quite the faker.", DayNight != 0);

	return false;
}
