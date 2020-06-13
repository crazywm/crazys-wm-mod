/*
* Copyright 2009, 2010, The Pink Petal Development Team.
* The Pink Petal Devloment Team are defined as the game's coders
* who meet on http://pinkpetal.org
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
#pragma region //    Includes and Externs            //
#include "cRng.h"
#include "buildings/cBrothel.h"
#include "Game.hpp"
#include "sStorage.hpp"
#include "cInventory.h"
#include <sstream>
#include "Inventory.hpp"


#pragma endregion

// `J` Job Farm - Laborers
bool WorkMilker(sGirl& girl, bool Day0Night1, cRng& rng)
{
#pragma region //    Job setup                //
    Action_Types actiontype = ACTION_WORKFARM;
    stringstream ss;
    int roll_a = rng.d100(), roll_b = rng.d100();
    if (girl.disobey_check(actiontype, JOB_MILKER))            // they refuse to work
    {
        ss << "${name} refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    }
    ss << "${name} worked as a milker on the farm.\n \n";

    cGirls::UnequipCombat(&girl);    // put that shit away, you'll scare off the customers!

    int wages = 20, tips = 0;
    int enjoy = 0;
    int imagetype = IMGTYPE_FARM;
    int msgtype = Day0Night1;

#pragma endregion
#pragma region //    Job Performance            //

    double jobperformance = girl.job_performance(JOB_MILKER, false);
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
#pragma region    //    Enjoyment and Tiredness        //

    // Complications
    if (roll_a <= 10)
    {
        enjoy -= rng % 3 + 1;
        ss << "The animals were uncooperative and some didn't even let her get near them.\n";
        drinks *= 0.8;
        if (rng.percent(20))
        {
            enjoy--;
            ss << "Several animals kicked over the milking buckets and soaked ${name}.\n";
            girl.happiness(-(1 + rng % 5));
            drinks -= (5 + rng % 6);
        }
        if (rng.percent(20))
        {
            enjoy--;
            ss << "One of the animals urinated on ${name} and contaminated the milk she had collected.\n";
            girl.happiness(-(1 + rng % 3));
            drinks -= (5 + rng % 6);
        }
        if (rng.percent(20))
        {
            enjoy--;
            int healthmod = rng % 10 + 1;
            girl.health(-healthmod);
            girl.happiness(-(healthmod + rng % healthmod));
            ss << "One of the animals kicked ${name} and ";
            if (girl.health() < 1)
            {
                ss << "killed her.\n";
                g_Game->push_message(girl.FullName() + " was killed when an animal she was milking kicked her in the head.", COLOR_RED);
                return false;    // not refusing, she is dead
            }
            else ss << (healthmod > 5 ? "" : "nearly ") << "broke her arm.\n";
            drinks -= (5 + rng % 6);
        }
    }
    else if (roll_a >= 90)
    {
        enjoy += rng % 3 + 1;
        ss << "The animals were pleasant and cooperative today.\n";
        drinks *= 1.2;
    }
    else
    {
        enjoy += rng % 2;
        ss << "She had an uneventful day milking.\n";
    }

#pragma endregion
#pragma region    //    Money                    //

    // slave girls not being paid for a job that normally you would pay directly for do less work
    if (girl.is_unpaid())
    {
        drinks *= 0.9;
        wages = 0;
    }
    else
    {
        wages += (int)drinks / 100; // `J` Pay her based on how much she brought in
    }

#pragma endregion
#pragma region    //    Create Items                //

    int milk = int(drinks / 10);    // plain milk is made here, breast milk from girls is made in WorkMilk
    if (milk > 0)
    {
        int milkmade[5] = { 0, 0, 0, 0, 0 };    // (total,gallon,quart,pint,shots}
        // check if the milk items exist and only do the checks if at least one of them does
        sInventoryItem* milkitems[4] = { g_Game->inventory_manager().GetItem("Milk"), g_Game->inventory_manager().GetItem("Milk (pt)"), g_Game->inventory_manager().GetItem("Milk (qt)"), g_Game->inventory_manager().GetItem("Milk (g)") };
        if (milkitems[0] != nullptr || milkitems[1] != nullptr || milkitems[2] != nullptr || milkitems[3] != nullptr)
        {
            while (milk > 0)    // add milk
            {
                string itemname;
                /* */if (milkitems[3] && milk > 3 && rng.percent(30))
                {
                    milk -= 4;
                    milkmade[0]++;
                    milkmade[4]++;
                    g_Game->player().add_item(milkitems[3]);
                }
                else if (milkitems[2] && milk > 2 && rng.percent(50))
                {
                    milk -= 3;
                    milkmade[0]++;
                    milkmade[3]++;
                    g_Game->player().add_item(milkitems[2]);
                }
                else if (milkitems[1] && milk > 1 && rng.percent(70))
                {
                    milk -= 2;
                    milkmade[0]++;
                    milkmade[2]++;
                    g_Game->player().add_item(milkitems[1]);
                }
                else if (milkitems[0])
                {
                    milk -= 1;
                    milkmade[0]++;
                    milkmade[1]++;
                    g_Game->player().add_item(milkitems[0]);
                }
                else milk--;    // add a reducer just in case.
            }
        }
        if (milkmade[0] > 0)
        {
            ss << "${name} produced " << milkmade[0] << " bottles of milk for you, \n";
            if (milkmade[4] > 0) ss << milkmade[4] << " gallons\n";
            if (milkmade[3] > 0) ss << milkmade[3] << " quarts\n";
            if (milkmade[2] > 0) ss << milkmade[2] << " pints\n";
            if (milkmade[1] > 0) ss << milkmade[1] << " shots\n";
        }
    }

    // `J` zzzzzz - need to add the girl milking herself - can be done easier after WorkMilk is done




#pragma endregion
#pragma region    //    Finish the shift            //

    ss << "\n${name}";
    if ((int)drinks > 0)
    {
        g_Game->storage().add_to_drinks((int)drinks);
        ss << " brought in " << (int)drinks << " units of milk.";
    }
    else { ss << " was unable to collect any milk."; }

    girl.AddMessage(ss.str(), imagetype, msgtype);

    // Money
    girl.m_Tips = max(0, tips);
    girl.m_Pay = max(0, wages);

    // Improve stats
    int xp = 5, skill = 3;

    if (girl.has_active_trait("Quick Learner"))        { skill += 1; xp += 3; }
    else if (girl.has_active_trait("Slow Learner"))    { skill -= 1; xp -= 3; }

    girl.exp((rng % xp) + 1);

    // primary (+2 for single or +1 for multiple)
    girl.animalhandling((rng % skill) + 2);
    // secondary (-1 for one then -2 for others)
    girl.handjob(max(0, (rng % skill) - 1));
    girl.farming(max(0, (rng % skill) - 2));
    girl.intelligence(max(0, (rng % skill) - 2));

    girl.upd_Enjoyment(actiontype, enjoy);


#pragma endregion
    return false;
}

double JP_Milker(const sGirl& girl, bool estimate)// not used
{
    double jobperformance =
        // primary - first 100
        ((girl.animalhandling() + girl.handjob()) / 2) +
        // secondary - second 100
        ((girl.farming() + girl.intelligence()) / 2) +
        // level bonus
        girl.level();
    if (!estimate)
    {
        int t = girl.tiredness() - 80;
        if (t > 0)
            jobperformance -= (t + 2) * (t / 3);
    }

    jobperformance += girl.get_trait_modifier("work.milker");

    return jobperformance;
}
