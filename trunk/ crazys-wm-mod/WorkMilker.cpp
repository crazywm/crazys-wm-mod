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
#include "cRng.h"
#include "CLog.h"
#include "cMessageBox.h"
#include "cGold.h"
#include "cBrothel.h"
#include "cFarm.h"

extern CLog g_LogFile;
extern cMessageQue g_MessageQue;
extern cRng g_Dice;
extern cGold g_Gold;
extern cBrothelManager g_Brothels;
extern cFarmManager g_Farm;
extern cInventory g_InvManager;

// `J` Job Farm - Laborers
bool cJobManager::WorkMilker(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKFARM;
	stringstream ss; string girlName = girl->m_Realname; ss << girl->m_Realname;
	if (g_Girls.DisobeyCheck(girl, actiontype, brothel))			// they refuse to work 
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked as a milker on the farm.\n\n";

	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	int wages = 15, enjoy = 0;
	int roll_a = g_Dice.d100();
	double jobperformance = JP_Milker(girl, false);

	double drinks = 1.0 + jobperformance + g_Dice.randomd(jobperformance);

	if (jobperformance >= 245)
	{
		ss << "She must be the perfect at this.";
		roll_a += 10;
	}
	else if (jobperformance >= 185)
	{
		ss << "She's unbelievable at this.";
		roll_a += 5;
	}
	else if (jobperformance >= 145)
	{
		ss << "She's good at this job.";
		roll_a += 1;
	}
	else if (jobperformance >= 100)
	{
		ss << "She made a few mistakes but overall she is okay at this.";
	}
	else if (jobperformance >= 70)
	{
		ss << "She was nervous and made a few mistakes. She isn't that good at this.";
		roll_a -= 1;
	}
	else
	{
		ss << "She was nervous and constantly making mistakes. She really isn't very good at this job.";
		roll_a -= 5;
	}
	ss << "\n\n";


	// Complications
	if (roll_a <= 10)
	{
		enjoy -= g_Dice % 3 + 1;
		ss << "The animals were uncooperative and some didn't even let her get near them.\n";
		drinks *= 0.8;
		if (g_Dice.percent(20))
		{
			enjoy--;
			ss << "Several animals kicked over the milking buckets and soaked " << girlName << ".\n";
			girl->happiness(-(1 + g_Dice % 5));
			drinks -= (5 + g_Dice % 6);
		}
		if (g_Dice.percent(20))
		{
			enjoy--;
			ss << "One of the animals urinated on " << girlName << " and contaminated the milk she had collected.\n";
			girl->happiness(-(1 + g_Dice % 3));
			drinks -= (5 + g_Dice % 6);
		}
		if (g_Dice.percent(20))
		{
			enjoy--;
			int healthmod = g_Dice % 10 + 1;
			girl->health(-healthmod);
			girl->happiness(-(healthmod + g_Dice % healthmod));
			ss << "One of the animals kicked " << girlName << " and ";
			if (girl->health() < 1)
			{
				ss << "killed her.\n";
				g_MessageQue.AddToQue(girlName + " was killed when an animal she was milking kicked her in the head.", COLOR_RED);
				return false;	// not refusing, she is dead
			}
			else ss << (healthmod > 5 ? "" : "nearly ") << "broke her arm.\n";
			drinks -= (5 + g_Dice % 6);
		}
	}
	else if (roll_a >= 90)
	{
		enjoy += g_Dice % 3 + 1;
		ss << "The animals were pleasant and cooperative today.\n";
		drinks *= 1.2;
	}
	else
	{
		enjoy += g_Dice % 2;
		ss << "She had an uneventful day milking.\n";
	}


#if 1
	// `J` Farm Bookmark - adding in items that can be created in the farm

	int milk = int(drinks / 50);	// plain milk is made here, breast milk from girls is made in WorkMilk
	if (milk > 0)
	{
		int milkmade[5] = { 0, 0, 0, 0, 0 };	// (total,gallon,quart,pint,shots}
		// check if the milk items exist and only do the checks if at least one of them does
		sInventoryItem* milkitems[4] = { g_InvManager.GetItem("Milk"), g_InvManager.GetItem("Milk (pt)"), g_InvManager.GetItem("Milk (qt)"), g_InvManager.GetItem("Milk (g)") };
		if (milkitems[0] != 0 || milkitems[1] != 0 || milkitems[2] != 0 || milkitems[3] != 0)
		{
			while (milk > 0)	// add milk
			{
				sInventoryItem* item = NULL;
				string itemname = "";
				/* */if (milkitems[3] && milk > 3 && g_Dice.percent(30))
				{
					milk -= 4;
					milkmade[0]++;
					milkmade[4]++;
					g_Brothels.AddItemToInventory(milkitems[3]);
				}
				else if (milkitems[2] && milk > 2 && g_Dice.percent(50))
				{
					milk -= 3;
					milkmade[0]++;
					milkmade[3]++;
					g_Brothels.AddItemToInventory(milkitems[2]);
				}
				else if (milkitems[1] && milk > 1 && g_Dice.percent(70))
				{
					milk -= 2;
					milkmade[0]++;
					milkmade[2]++;
					g_Brothels.AddItemToInventory(milkitems[1]);
				}
				else if (milkitems[0])
				{
					milk -= 1;
					milkmade[0]++;
					milkmade[1]++;
					g_Brothels.AddItemToInventory(milkitems[0]);
				}
				else milk--;	// add a reducer just in case.
			}
		}
		if (milkmade[0] > 0)
		{
			ss << girlName << " produced " << milkmade[0] << " bottles of milk for you, \n";
			if (milkmade[4] > 0) ss << milkmade[4] << " gallons\n";
			if (milkmade[3] > 0) ss << milkmade[3] << " quarts\n";
			if (milkmade[2] > 0) ss << milkmade[2] << " pints\n";
			if (milkmade[1] > 0) ss << milkmade[1] << " shots\n";
		}
	}

	// `J` zzzzzz - need to add the girl milking herself - can be done easier after WorkMilk is done


#endif

	
	ss << "\n" << girlName;
	if (drinks < 1) { drinks = 0; ss << " was unable to collect any milk."; }
	else ss << " was able to collect enough milk for " << (int)drinks << " drinks.";

#if 0
	// `J` in the future, food and drinks will be stored for use elsewhere
	g_Brothels.add_to_drinks(int(drinks));
#else
	// `J` but for now just sell them
	double earned = drinks * 2;	// you sell the milk for 2 gold per glass
	brothel->m_Finance.farm_income(earned);
#endif

	g_Girls.UpdateEnjoyment(girl, actiontype, enjoy, true);
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);

	wages += int(drinks / 5);

	if (wages < 0) wages = 0;
	girl->m_Pay = wages;


	// Improve stats
	int xp = 5, libido = 1, skill = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_EXP, (g_Dice % xp) + 1);
	g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, libido);

	// primary (+2 for single or +1 for multiple)
	g_Girls.UpdateSkill(girl, SKILL_ANIMALHANDLING, (g_Dice % skill) + 2);
	// secondary (-1 for one then -2 for others)
	g_Girls.UpdateSkill(girl, SKILL_FARMING, max(0, (g_Dice % skill) - 1));
	g_Girls.UpdateStat(girl, STAT_INTELLIGENCE, max(0, (g_Dice % skill) - 2));

	return false;
}

double cJobManager::JP_Milker(sGirl* girl, bool estimate)// not used
{
	double jobperformance =
		// primary - first 100
		girl->animalhandling() +
		// secondary - second 100
		((girl->farming() + girl->intelligence()) / 2) +
		// level bonus
		girl->level();
	// traits modifiers



	//good traits
	if (g_Girls.HasTrait(girl, "Quick Learner"))  jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Psychic"))		  jobperformance += 10;


	//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))	jobperformance -= 50; //needs others to do the job
	if (g_Girls.HasTrait(girl, "Clumsy")) 		jobperformance -= 20; //spills food and breaks things often
	if (g_Girls.HasTrait(girl, "Aggressive")) 	jobperformance -= 20; //gets mad easy
	if (g_Girls.HasTrait(girl, "Nervous"))		jobperformance -= 30; //don't like to be around people	
	if (g_Girls.HasTrait(girl, "Meek"))			jobperformance -= 20;
	return jobperformance;
}
