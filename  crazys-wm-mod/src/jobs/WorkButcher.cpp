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
#include "cRng.h"
#include "src/buildings/cBrothel.h"
#include "cInventory.h"
#include "src/Game.hpp"

#pragma endregion

// `J` Job Farm - Producers
bool cJobManager::WorkButcher(sGirl* girl, bool Day0Night1, string& summary, cRng& rng)
{
    auto brothel = girl->m_Building;
#pragma region //	Job setup				//
	int actiontype = ACTION_WORKCOOKING;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = rng.d100(), roll_b = rng.d100(), roll_c = rng.d100();
	if (girl->disobey_check(actiontype, JOB_BUTCHER))			// they refuse to work
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked as a butcher on the farm.\n \n";

	cGirls::UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	int wages = 20, tips = 0;
	int enjoy = 0;
	int imagetype = IMGTYPE_COOK;
	int msgtype = Day0Night1;

#pragma endregion
#pragma region //	Job Performance			//

	double jobperformance = JP_Butcher(girl, false);
	if (jobperformance >= 245)
	{
		ss << " She must be the perfect at this.";
		wages += 155;
	}
	else if (jobperformance >= 185)
	{
		ss << " She's unbelievable at this.";
		wages += 95;
	}
	else if (jobperformance >= 145)
	{
		ss << " She's good at this job.";
		wages += 55;
	}
	else if (jobperformance >= 100)
	{
		ss << " She made a few mistakes but overall she is okay at this.";
		wages += 15;
	}
	else if (jobperformance >= 70)
	{
		ss << " She was nervous and made a few mistakes. She isn't that good at this.";
		wages -= 5;
	}
	else
	{
		ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.";
		wages -= 15;
	}
	ss << "\n \n";

#pragma endregion
#pragma region	//	Enjoyment and Tiredness		//


	int roll = rng.d100();
#if 1
	//enjoyed the work or not
	if (roll <= 5)
	{
		ss << "\nSome of the patrons abused her during the shift.";
		enjoy -= 1;
	}
	else if (roll <= 25)
	{
		ss << "\nShe had a pleasant time working.";
		enjoy += 3;
	}
	else
	{
		ss << "\nOtherwise, the shift passed uneventfully.";
		enjoy += 1;
	}
#else
	if (roll_a <= 10)
	{
		enjoyC -= rng % 3; enjoyF -= rng % 3;
		CleanAmt = int(CleanAmt * 0.8);
		/* */if (roll_b < 30)	ss << "She spilled a bucket of something unpleasant all over herself.";
		else if (roll_b < 60)	ss << "She stepped in something unpleasant.";
		else /*            */	ss << "She did not like working on the farm today.";
	}
	else if (roll_a >= 90)
	{
		enjoyC += rng % 3; enjoyF += rng % 3;
		CleanAmt = int(CleanAmt * 1.1);
		/* */if (roll_b < 50)	ss << "She cleaned the building while humming a pleasant tune.";
		else /*            */	ss << "She had a great time working today.";
	}
	else
	{
		enjoyC += rng % 2; enjoyF += rng % 2;
		ss << "The shift passed uneventfully.";
	}
	ss << "\n \n";
#endif


#pragma endregion
#pragma region	//	Create Items				//


	// `J` Farm Bookmark - adding in items that can be created in the farm

	string itemmade;
	sInventoryItem* item = nullptr;
	int totalitemsmade = 1;
	if (rng.percent(min(90.0, jobperformance / 2)))
	{
		int chooseitem = rng % (girl->magic() < 50 ? 50 : 60);	// limit some of the more magical items

		/* */if (chooseitem < 10)	itemmade = "C.G. Burger";
		else if (chooseitem < 20)	itemmade = "Premium Hot Dogs";
		else if (chooseitem < 28)	itemmade = "C.G. Cheese Burger";
		else if (chooseitem < 36)	itemmade = "C.G. Bacon Cheese Burger";
		else if (chooseitem < 43)	itemmade = "Turducken";
		else if (chooseitem < 50)	itemmade = "Cold Turkey";
		else if (chooseitem < 52)	itemmade = "Cold Turkey ";
		else if (chooseitem < 54)	itemmade = "Cold Turkey  ";
		else if (chooseitem < 56)	itemmade = "Cold  Turkey";
		else if (chooseitem < 58)	itemmade = "Cold  Turkey ";
		else /*                */	itemmade = "Aoshima BEEF!!";

		if (chooseitem < 40) totalitemsmade += rng % 3;
		item = g_Game->inventory_manager().GetItem(itemmade);
	}
	if (item)
	{
		msgtype = EVENT_GOODNEWS;
		ss << "\n \n" << girlName << " made ";
		if (totalitemsmade == 1) ss << "one ";
		else ss << totalitemsmade << " ";
		ss << itemmade << " for you.";
		g_Game->player().inventory().add_item(item, totalitemsmade);
	}



#pragma endregion
#pragma region	//	Money					//


#pragma endregion
#pragma region	//	Finish the shift			//


	girl->upd_Enjoyment(actiontype, enjoy);
	girl->m_Events.AddMessage(ss.str(), imagetype, msgtype);


	int roll_max = (girl->beauty() + girl->charisma());
	roll_max /= 4;
	wages += 10 + rng%roll_max;

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
	girl->animalhandling((rng % skill) + 1);
	girl->strength((rng % skill) + 1);
	// secondary (-1 for one then -2 for others)
	girl->medicine(max(0, (rng % skill) - 1));
	girl->intelligence(max(0, (rng % skill) - 2));
	girl->cooking(max(0, (rng % skill) - 2));

#pragma endregion
	return false;
}

double cJobManager::JP_Butcher(sGirl* girl, bool estimate)// not used
{
	double jobperformance =
		// primary - first 100
		((girl->animalhandling() + girl->strength()) / 2) +
		// secondary - second 100
		((girl->medicine() + girl->intelligence() + girl->cooking()) / 3) +
		// level bonus
		girl->level();
	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}

    jobperformance += girl->get_trait_modifier("work.butcher");

	return jobperformance;
}
