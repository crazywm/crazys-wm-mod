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
#include "src/buildings/cBrothel.h"
#include "src/Game.hpp"
#include "src/sStorage.hpp"


#pragma endregion

// `J` Job Farm - Laborers
bool cJobManager::WorkRancher(sGirl* girl, bool Day0Night1, string& summary, cRng& rng)
{
    auto brothel = girl->m_Building;
#pragma region //	Job setup				//
	int actiontype = ACTION_WORKFARM;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = rng.d100(), roll_b = rng.d100(), roll_c = rng.d100();
	if (girl->disobey_check(actiontype, JOB_RANCHER))			// they refuse to work
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked as a rancher on the farm.";

	cGirls::UnequipCombat(girl);	// put that shit away

	int wages = 20, tips = 0;
	int enjoy = 0;
	int imagetype = IMGTYPE_FARM;
	int msgtype = Day0Night1;

#pragma endregion
#pragma region //	Job Performance			//

	double jobperformance = JP_Rancher(girl, false);

	// brings in beasts and food
	double beasts = jobperformance / 50;
	double food = jobperformance / 5;

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
		enjoy -= rng % 3 + 1;
		ss << "The animals were uncooperative and some didn't even let her get near them.\n";
		if (rng.percent(20))
		{
			enjoy--;
			ss << "Several animals got out and " << girlName << " had to chase them down.\n";
			girl->happiness(-(1 + rng % 5));
			girl->tiredness(1 + rng % 15);
			beasts *= 0.8;
			food *= 0.9;
		}
		if (rng.percent(20))
		{
			enjoy--;
			int healthmod = rng % 10 + 1;
			girl->health(-healthmod);
			girl->happiness(-(healthmod + rng % healthmod));
			ss << "One of the animals kicked " << girlName << " and ";
			if (girl->health() < 1)
			{
				ss << "killed her.\n";
				g_Game->push_message(girlName + " was killed when an animal she was milking kicked her in the head.", COLOR_RED);
				return false;	// not refusing, she is dead
			}
			else ss << (healthmod > 5 ? "" : "nearly ") << "broke her arm.\n";
			beasts *= 0.9;
			food *= 0.8;
		}
	}
	else if (roll_a >= 90)
	{
		enjoy += rng % 3 + 1;
		ss << "The animals were pleasant and cooperative today.\n";
		beasts *= 1.1;
		food *= 1.1;
	}
	else
	{
		enjoy += rng % 2;
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
		g_Game->storage().add_to_beasts((int)beasts);
		ss << " brought " << (int)beasts << " beasts to work in the brothels";
	}
	if ((int)beasts > 0 && (int)food > 0)
	{
		ss << " and";
	}
	if ((int)food > 0)
	{
		g_Game->storage().add_to_food((int)food);
		ss << " sent " << (int)food << " units of food worth of animals to slaughter";
	}
	if ((int)beasts <= 0 && (int)food <= 0)
	{
		ss << " did not bring in any animals";
	}
	ss << ".";

	girl->m_Events.AddMessage(ss.str(), imagetype, msgtype);

	// Money
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);

	// Improve stats
	int xp = 5, libido = 1, skill = 3;

	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }

	girl->exp((rng % xp) + 1);
	girl->upd_temp_stat(STAT_LIBIDO, libido);

	// primary (+2 for single or +1 for multiple)
	girl->animalhandling((rng % skill) + 2);
	// secondary (-1 for one then -2 for others)
	girl->confidence(max(0, (rng % skill) - 1));
	girl->charisma(max(0, (rng % skill) - 2));
	girl->intelligence(max(0, (rng % skill) - 2));

	girl->upd_Enjoyment(actiontype, enjoy);

#pragma endregion
	return false;
}

double cJobManager::JP_Rancher(sGirl* girl, bool estimate)// not used
{
	double jobperformance =
		// primary - first 100
		girl->animalhandling() +
		// secondary - second 100
		((girl->confidence() + girl->charisma() + girl->intelligence()) / 3) +
		// level bonus
		girl->level();
	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}
    jobperformance += girl->get_trait_modifier("work.rancher");

    return jobperformance;
}
