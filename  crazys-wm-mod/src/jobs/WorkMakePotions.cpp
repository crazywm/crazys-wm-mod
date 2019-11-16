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

// `J` Job Farm - Producers - updated 1/29/15
bool cJobManager::WorkMakePotions(sGirl* girl, bool Day0Night1, string& summary, cRng& rng)
{
    auto brothel = girl->m_Building;
#pragma region //	Job setup				//
	int actiontype = ACTION_WORKMAKEPOTIONS;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = rng.d100(), roll_b = rng.d100(), roll_c = rng.d100();
	if (girl->disobey_check(actiontype, JOB_MAKEPOTIONS))			// they refuse to work
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked as a potions maker on the farm.\n \n";

	cGirls::UnequipCombat(girl);	// weapons and armor can get in the way

	int wages = 20, tips = 0;
	int enjoy = 0;
	int imagetype = IMGTYPE_CRAFT;
	int msgtype = Day0Night1;

#pragma endregion
#pragma region //	Job Performance			//


	double jobperformance = JP_MakePotions(girl, false);

	if (jobperformance >= 245)
	{
		ss << " She must be the perfect at this.";
		wages += 155;
		roll_a -= 20;
	}
	else if (jobperformance >= 185)
	{
		ss << " She's unbelievable at this.";
		wages += 95;
		roll_a -= 10;
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
		roll_a += 5;
	}
	else if (jobperformance >= 70)
	{
		ss << " She was nervous and made a few mistakes. She isn't that good at this.";
		wages -= 5;
		roll_a += 10;
	}
	else
	{
		ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.";
		wages -= 15;
		roll_a += 20;
	}
	ss << "\n \n";

#pragma endregion
#pragma region	//	Enjoyment and Tiredness		//


	//enjoyed the work or not
	if (roll_a <= 10)
	{
		enjoy += rng % 3 + 1;
		ss << "She had a great time making potions today.";
	}
	else if (roll_a >= 90)
	{
		enjoy -= (rng % 5 + 1);
		ss << "Some potions blew up in her face today.";
		girl->health(-(rng % 10));
		girl->happiness(-(rng % 20));
		girl->beauty(-(rng % 3));

	}
	else if (roll_a >= 80)
	{
		enjoy -= (rng % 3 + 1);
		ss << "She did not like making potions today.";
	}
	else
	{
		enjoy += rng % 2;
		ss << "The shift passed uneventfully.";
	}
	ss << "\n \n";


#pragma endregion
#pragma region	//	Create Items				//


	// `J` Farm Bookmark - adding in potions that can be created in the farm

    int points_remaining =  (rng % ((int)jobperformance / 2) + ((int)jobperformance / 2));
    // more girls working can help out a bit, but too many can hurt so limit it to 10
    points_remaining += min(10, (brothel->num_girls_on_job(JOB_MAKEPOTIONS, Day0Night1) - 1));
    int numitems = 0;

    while (points_remaining > 0 && numitems < (1 + girl->crafting() / 15))
    {
        auto item = g_Game->inventory_manager().GetCraftableItem(*girl, JOB_MAKEPOTIONS, points_remaining);
        if(!item) {
            // try something easier. Get craftable item does not return items which need less than
            // points_remaining / 3 crafting points
            item = g_Game->inventory_manager().GetCraftableItem(*girl, JOB_MAKEPOTIONS, points_remaining / 2);

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
        g_Game->player().inventory().add_item(item);
        numitems++;
    }

#pragma endregion
#pragma region	//	Money					//


#pragma endregion
#pragma region	//	Finish the shift			//



	girl->upd_Enjoyment(actiontype, enjoy);
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_CRAFT, msgtype);

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
	girl->brewing((rng % skill) + 1);
	girl->herbalism((rng % skill) + 1);
	// secondary (-1 for one then -2 for others)
	girl->intelligence(max(0, (rng % skill) - 1));
	girl->cooking(max(0, (rng % skill) - 2));
	girl->magic(max(0, (rng % skill) - 2));

#pragma endregion
	return false;
}

double cJobManager::JP_MakePotions(sGirl* girl, bool estimate)// not used
{
	double jobperformance =
		// primary - first 100
		((girl->brewing() + girl->herbalism()) / 2) +
		// secondary - second 100
		((girl->intelligence() + girl->cooking() + girl->magic()) / 3) +
		// level bonus
		girl->level();
	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}

    jobperformance += girl->get_trait_modifier("work.makepotions");

    return jobperformance;
}
