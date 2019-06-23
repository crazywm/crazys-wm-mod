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
bool cJobManager::WorkShepherd(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
#pragma region //	Job setup				//
	int actiontype = ACTION_WORKFARM;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();
	if (girl->disobey_check(actiontype, brothel))
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked as a shepherd in the farm.\n \n";

	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	int wages = 20, tips = 0;
	int enjoy = 0;
	int imagetype = IMGTYPE_HERD;
	int msgtype = Day0Night1;

#pragma endregion
#pragma region //	Job Performance			//

	double jobperformance = JP_Shepherd(girl, false);

	// brings in food and rarely beasts
	double beasts = jobperformance / 100;
	double food = jobperformance;

	if (jobperformance >= 245)
	{
		ss << "The animals come to " << girlName << " when they are ready for market.";
		beasts *= 2.0; food *= 2.0; roll_a += 10; roll_b += 25;
	}
	else if (jobperformance >= 185)
	{
		ss << girlName << " seems to know just when to choose the best animals to send to market.";
		beasts *= 1.6; food *= 1.6; roll_a += 5; roll_b += 18;
	}
	else if (jobperformance >= 145)
	{
		ss << girlName << " has little trouble finding animals to send to market.";
		beasts *= 1.2; food *= 1.2; roll_a += 2; roll_b += 10;
	}
	else if (jobperformance >= 100)
	{
		ss << girlName << " usually sends the right animals to market.";
	}
	else if (jobperformance >= 70)
	{
		ss << girlName << " doesn't know what animals to send to the market so she just guesses.";
		beasts *= 0.8; food *= 0.8; roll_a -= 2; roll_b -= 5;
	}
	else
	{
		ss << girlName << " has no idea what she is doing.";
		beasts *= 0.5; food *= 0.5; wages -= 10; roll_a -= 5; roll_b -= 10;
	}
	ss << "\n \n";

#pragma endregion
#pragma region	//	Enjoyment and Tiredness		//

	// Complications
	if (roll_a <= 10)
	{
		enjoy -= g_Dice % 3 + 1;
		ss << "The animals were uncooperative and some didn't even let her get near them.\n";
		if (g_Dice.percent(20))
		{
			enjoy--;
			ss << "Several animals got out and " << girlName << " had to chase them down.\n";
			girl->happiness(-(1 + g_Dice % 5));
			girl->tiredness(1 + g_Dice % 15);
			beasts *= 0.8;
			food *= 0.9;
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
			beasts *= 0.9;
			food *= 0.8;
		}
	}
	else if (roll_a >= 90)
	{
		enjoy += g_Dice % 3 + 1;
		ss << "The animals were pleasant and cooperative today.\n";
		beasts *= 1.1;
		food *= 1.1;
	}
	else
	{
		enjoy += g_Dice % 2;
		ss << "She had an uneventful day tending the animals.\n";
	}

#pragma endregion
#pragma region	//	Create Items				//




	// `J` Farm Bookmark - adding in items that can be created in the farm
















#pragma endregion
#pragma region	//	Money					//

	// slave girls not being paid for a job that normally you would pay directly for do less work
	if ((girl->is_slave() && !cfg.initial.slave_pay_outofpocket()))
	{
		beasts *= 0.9;
		food *= 0.9;
		wages = 0;
	}
	else	// `J` Pay her based on how much she brought in
	{
		if (food > 0)		wages += (int)food / 100;
		if (beasts > 0)		wages += (int)beasts;
	}

#pragma endregion
#pragma region	//	Finish the shift			//

	ss << "\n" << girlName;
	if ((int)beasts > 0)
	{
		g_Brothels.add_to_beasts((int)beasts);
		ss << " brought " << (int)beasts << " beasts to work in the brothels";
	}
	if ((int)beasts > 0 && (int)food > 0)
	{
		ss << " and";
	}
	if ((int)food > 0)
	{
		g_Brothels.add_to_food((int)food);
		ss << " sent " << (int)food << " units of food worth of animals to slaughter";
	}
	if ((int)beasts <= 0 && (int)food <= 0)
	{
		ss << " did not bring in any animals";
	}
	ss << ".";

	girl->upd_Enjoyment(actiontype, enjoy);
	girl->m_Events.AddMessage(ss.str(), imagetype, msgtype);

	// Money
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);

	// Improve stats
	int xp = 5, libido = 1, skill = 3;

	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }

	girl->upd_temp_stat(STAT_LIBIDO, libido);

	// primary (+2 for single or +1 for multiple)
	// secondary (-1 for one then -2 for others)

#pragma endregion
	return false;
}

double cJobManager::JP_Shepherd(sGirl* girl, bool estimate)// not used
{
	double jobperformance =
		// primary - first 100
		girl->animalhandling() +
		// secondary - second 100
		((girl->charisma() + girl->intelligence() + girl->confidence()) / 3) +
		// level bonus
		girl->level();
	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}


	//good traits
	if (girl->has_trait( "Quick Learner"))	jobperformance += 5;
	if (girl->has_trait( "Psychic"))			jobperformance += 10;
	if (girl->has_trait( "Farmers Daughter"))	jobperformance += 30;
	if (girl->has_trait( "Country Gal"))		jobperformance += 10;

	//bad traits
	if (girl->has_trait( "Dependant"))		jobperformance -= 50;	// needs others to do the job
	if (girl->has_trait( "Clumsy"))			jobperformance -= 20;	//spills food and breaks things often
	if (girl->has_trait( "Aggressive"))		jobperformance -= 20;	//gets mad easy
	if (girl->has_trait( "Nervous"))			jobperformance -= 30;	//don't like to be around people
	if (girl->has_trait( "Meek"))				jobperformance -= 20;

	if (girl->has_trait( "One Arm"))		jobperformance -= 40;
	if (girl->has_trait( "One Foot"))		jobperformance -= 40;
	if (girl->has_trait( "One Hand"))		jobperformance -= 30;
	if (girl->has_trait( "One Leg"))		jobperformance -= 60;
	if (girl->has_trait( "No Arms"))		jobperformance -= 125;
	if (girl->has_trait( "No Feet"))		jobperformance -= 60;
	if (girl->has_trait( "No Hands"))		jobperformance -= 50;
	if (girl->has_trait( "No Legs"))		jobperformance -= 150;
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
