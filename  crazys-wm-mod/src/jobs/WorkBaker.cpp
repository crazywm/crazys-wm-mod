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
#include "cGold.h"
#include "src/buildings/cBrothel.h"
#include "cInventory.h"
#include "src/Game.hpp"


extern cRng g_Dice;

#pragma endregion

// `J` Job Farm - Producers
bool cJobManager::WorkBaker(sGirl* girl, bool Day0Night1, string& summary)
{
    auto brothel = girl->m_Building;
#pragma region //	Job setup				//
	int actiontype = ACTION_WORKCOOKING;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();
	if (girl->disobey_check(actiontype, JOB_BAKER))			// they refuse to work
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked as a baker on the farm.\n \n";

	cGirls::UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	int wages = 20, tips = 0;
	int enjoy = 0;
	int imagetype = IMGTYPE_COOK;
	int msgtype = Day0Night1;

#pragma endregion
#pragma region //	Job Performance			//

	double jobperformance = JP_Baker(girl, false);
	double craftpoints = jobperformance;

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


	int roll = g_Dice.d100();
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
		enjoy -= g_Dice % 3;
		/* */if (roll_b < 30)	ss << "She spilled a bucket of something unpleasant all over herself.";
		else if (roll_b < 60)	ss << "She stepped in something unpleasant.";
		else /*            */	ss << "She did not like working on the farm today.";
	}
	else if (roll_a >= 90)
	{
		enjoy += g_Dice % 3;
		/* */if (roll_b < 50)	ss << "She cleaned the building while humming a pleasant tune.";
		else /*            */	ss << "She had a great time working today.";
	}
	else
	{
		enjoy += g_Dice % 2;
		ss << "The shift passed uneventfully.";
	}
	ss << "\n \n";
#endif

#pragma endregion
#pragma region	//	Create Items				//
    if (craftpoints > 0)
    {
        int points_remaining = (int)craftpoints;
        int numitems = 0;

        while (points_remaining > 0 && numitems < (1 + girl->crafting() / 15))
        {
            auto item = g_Game.inventory_manager().GetCraftableItem(*girl, JOB_BAKER, points_remaining);
            if(!item) {
                // try something easier. Get craftable item does not return items which need less than
                // points_remaining / 3 crafting points
                item = g_Game.inventory_manager().GetCraftableItem(*girl, JOB_BAKER, points_remaining / 2);
            }
            if(!item) {
                points_remaining -= 10;
                continue;
            }

            points_remaining -= item->m_Crafting.craft_cost();
            girl->mana(-item->m_Crafting.mana_cost());
            msgtype = EVENT_GOODNEWS;
            if (numitems == 0)	ss << "\n \n" << girlName << " made:";
            ss << "\n" << item->m_Name;
            g_Game.player().inventory().add_item(item);
            numitems++;
        }
    }

#pragma endregion
#pragma region	//	Money					//


#pragma endregion
#pragma region	//	Finish the shift			//



	// `J` - Finish the shift - Baker

	// Push out the turn report
	girl->m_Events.AddMessage(ss.str(), imagetype, msgtype);

	// Money
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);

	// Base Improvement and trait modifiers
	int xp = 5, libido = 1, skill = 3;
	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }
	// EXP and Libido
	girl->exp((g_Dice%xp)+1);
	girl->upd_temp_stat(STAT_LIBIDO, libido);

	// primary improvement (+2 for single or +1 for multiple)
	girl->cooking((g_Dice % skill) + 2);
	// secondary improvement (-1 for one then -2 for others)
	girl->service(max(0,(g_Dice%skill)-1));
	girl->intelligence(max(0,(g_Dice%skill)-2));
	girl->herbalism(max(0,(g_Dice%skill)-2));

	// Update Enjoyment
	girl->upd_Enjoyment(actiontype, enjoy);
	// Gain Traits
	cGirls::PossiblyGainNewTrait(girl, "Chef", 70, actiontype, girlName + " has prepared enough food to qualify as a Chef.", Day0Night1);

#pragma endregion
	return false;
}

double cJobManager::JP_Baker(sGirl* girl, bool estimate)// 'Mute' used inside of cJobManager::WorkBaker
{
	double jobperformance =
		// primary - first 100
		girl->cooking() +
		// secondary - second 100
		((girl->service() + girl->intelligence() + girl->herbalism()) / 3) +
		// level bonus
		girl->level();

	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}

	//good traits
	if (girl->has_trait( "Quick Learner"))  jobperformance += 5;
	if (girl->has_trait( "Psychic"))		  jobperformance += 10;
	if (girl->has_trait( "Mixologist"))	  jobperformance += 20;	//Good with measures
	if (girl->has_trait( "Chef"))			  jobperformance += 30;


	//bad traits
	if (girl->has_trait( "Dependant"))	jobperformance -= 50; // needs others to do the job
	if (girl->has_trait( "Clumsy")) 		jobperformance -= 20; //spills food and breaks things often
	if (girl->has_trait( "Aggressive")) 	jobperformance -= 20; //gets mad easy
	if (girl->has_trait( "Nervous"))		jobperformance -= 30; //don't like to be around people
	if (girl->has_trait( "Meek"))			jobperformance -= 20;


	return jobperformance;
}
