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
#pragma region //	Includes and Externs			//
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

#pragma endregion

// `J` Job Farm - Laborers
bool cJobManager::WorkMilker(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
#pragma region //	Job setup				//
	int actiontype = ACTION_WORKFARM;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();
	if (girl->disobey_check(actiontype, brothel))			// they refuse to work
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked as a milker on the farm.\n \n";

	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	int wages = 20, tips = 0;
	int enjoy = 0;
	int imagetype = IMGTYPE_FARM;
	int msgtype = Day0Night1;

#pragma endregion
#pragma region //	Job Performance			//

	double jobperformance = JP_Milker(girl, false);
	double drinks = jobperformance / 2;

	if (jobperformance >= 245)
	{
		ss << "Her milk bucket practically fills itself as she walks down the rows of cows.";
		drinks *= 5; roll_a += 10; roll_b += 25;
	}
	else if (jobperformance >= 185)
	{
		ss << "Her hands moved like lightning as she gracefully milks the cows teats.";
		drinks *= 4; roll_a += 5; roll_b += 18;
	}
	else if (jobperformance >= 145)
	{
		ss << "She knows exactly when the cows are ready to be milked and how to best milk them.";
		drinks *= 3; roll_a += 2; roll_b += 10;
	}
	else if (jobperformance >= 100)
	{
		ss << "She can milk the cows without spilling much.";
		drinks *= 2;
	}
	else if (jobperformance >= 70)
	{
		ss << "She isn't very good at aiming the teats into the bucket.";
		roll_a -= 2; roll_b -= 5;
	}
	else
	{
		ss << "She can't seem to get the hang of this.";
		wages -= 10; drinks *= 0.8; roll_a -= 5; roll_b -= 10;
	}
	ss << "\n \n";

#pragma endregion
#pragma region	//	Enjoyment and Tiredness		//

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

#pragma endregion
#pragma region	//	Money					//

	// slave girls not being paid for a job that normally you would pay directly for do less work
	if ((girl->is_slave() && !cfg.initial.slave_pay_outofpocket()))
	{
		drinks *= 0.9;
		wages = 0;
	}
	else
	{
		wages += (int)drinks / 100; // `J` Pay her based on how much she brought in
	}

#pragma endregion
#pragma region	//	Create Items				//

	int milk = int(drinks / 10);	// plain milk is made here, breast milk from girls is made in WorkMilk
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




#pragma endregion
#pragma region	//	Finish the shift			//

	ss << "\n" << girlName;
	if ((int)drinks > 0)
	{
		g_Brothels.add_to_drinks((int)drinks);
		ss << " brought in " << (int)drinks << " units of milk.";
	}
	else { ss << " was unable to collect any milk."; }

	girl->m_Events.AddMessage(ss.str(), imagetype, msgtype);

	// Money
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);

	// Improve stats
	int xp = 5, libido = 1, skill = 3;

	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }

	girl->exp((g_Dice % xp) + 1);
	girl->upd_temp_stat(STAT_LIBIDO, libido);

	// primary (+2 for single or +1 for multiple)
	girl->animalhandling((g_Dice % skill) + 2);
	// secondary (-1 for one then -2 for others)
	girl->handjob(max(0, (g_Dice % skill) - 1));
	girl->farming(max(0, (g_Dice % skill) - 2));
	girl->intelligence(max(0, (g_Dice % skill) - 2));

	girl->upd_Enjoyment(actiontype, enjoy);


#pragma endregion
	return false;
}

double cJobManager::JP_Milker(sGirl* girl, bool estimate)// not used
{
	double jobperformance =
		// primary - first 100
		((girl->animalhandling() + girl->handjob()) / 2) +
		// secondary - second 100
		((girl->farming() + girl->intelligence()) / 2) +
		// level bonus
		girl->level();
	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}



	//good traits
	if (girl->has_trait( "Quick Learner"))		jobperformance += 5;
	if (girl->has_trait( "Psychic"))				jobperformance += 10;
	if (girl->has_trait( "Farmers Daughter"))		jobperformance += 30;
	if (girl->has_trait( "Country Gal"))			jobperformance += 10;


	//bad traits
	if (girl->has_trait( "Dependant"))	jobperformance -= 50; // needs others to do the job
	if (girl->has_trait( "Clumsy")) 		jobperformance -= 20; //spills food and breaks things often
	if (girl->has_trait( "Aggressive")) 	jobperformance -= 20; //gets mad easy
	if (girl->has_trait( "Nervous"))		jobperformance -= 30; //don't like to be around people
	if (girl->has_trait( "Meek"))			jobperformance -= 20;

	if (girl->has_trait( "One Arm"))		jobperformance -= 40;
	if (girl->has_trait( "One Foot"))		jobperformance -= 40;
	if (girl->has_trait( "One Hand"))		jobperformance -= 30;
	if (girl->has_trait( "One Leg"))		jobperformance -= 60;
	if (girl->has_trait( "No Arms"))		jobperformance -= 125;
	if (girl->has_trait( "No Feet"))		jobperformance -= 60;
	if (girl->has_trait( "No Hands"))		jobperformance -= 50;
	if (girl->has_trait( "No Legs"))		jobperformance -= 100;
	if (girl->has_trait( "Blind"))		jobperformance -= 30;
	if (girl->has_trait( "Deaf"))			jobperformance -= 15;
	if (girl->has_trait( "Retarded"))		jobperformance -= 60;
	if (girl->has_trait( "Smoker"))		jobperformance -= 10;	//would need smoke breaks

	if (girl->has_trait( "Alcoholic"))			jobperformance -= 25;
	if (girl->has_trait( "Fairy Dust Addict"))	jobperformance -= 25;
	if (girl->has_trait( "Shroud Addict"))		jobperformance -= 25;
	if (girl->has_trait( "Viras Blood Addict"))	jobperformance -= 25;

	return jobperformance;
}
