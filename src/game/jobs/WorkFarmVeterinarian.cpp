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
#pragma region //    Includes and Externs            //
#include "cRng.h"
#include "buildings/cBrothel.h"
#include <sstream>

#pragma endregion

// `J` Job Farm - Staff
bool WorkFarmVeterinarian(sGirl& girl, bool Day0Night1, cRng& rng)
{
#pragma region //    Job setup                //
    Action_Types actiontype = ACTION_WORKFARM;
    stringstream ss;
    int roll_a = rng.d100();
    if (girl.disobey_check(actiontype, JOB_VETERINARIAN))            // they refuse to work
    {
        ss << "${name} refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    }
    ss << "${name} worked as a Veterinarian on the farm.";

    cGirls::UnequipCombat(&girl);    // put that shit away, you'll scare off the customers!

    int wages = 20, tips = 0;
    int enjoy = 0;

#pragma endregion
#pragma region //    Job Performance            //

    int fame = 0;
    double jobperformance = girl.job_performance(JOB_VETERINARIAN, false);


    if (jobperformance >= 245)
    {
        wages += 155;    fame += 2;
        ss << " She must be the perfect at this.\n \n";
    }
    else if (jobperformance >= 185)
    {
        wages += 95;    fame += 1;
        ss << " She's unbelievable at this.\n \n";
    }
    else if (jobperformance >= 145)
    {
        wages += 55;
        ss << " She's good at this job.\n \n";
    }
    else if (jobperformance >= 100)
    {
        wages += 15;
        ss << " She made a few mistakes but overall she is okay at this.\n \n";
    }
    else if (jobperformance >= 70)
    {
        ss << " She was nervous and made a few mistakes. She isn't that good at this.\n \n";
        wages -= 5;
    }
    else
    {
        ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.\n \n";
        wages -= 15;
    }


#pragma endregion
#pragma region    //    Enjoyment and Tiredness        //

    //enjoyed the work or not
    if (roll_a <= 5)
    {
        ss << "\nSome of the patrons abused her during the shift.";
        enjoy -= 1;
    }
    else if (roll_a <= 25)
    {
        ss << "\nShe had a pleasant time working.";
        enjoy += 3;
    }
    else
    {
        ss << "\nOtherwise, the shift passed uneventfully.";
        enjoy += 1;
    }

#pragma endregion
#pragma region    //    Money                    //


#pragma endregion
#pragma region    //    Create Items                //



#if 0

    // `J` Farm Bookmark - adding in items that can be created in the farm




#endif


#pragma endregion
#pragma region    //    Finish the shift            //


    girl.upd_Enjoyment(ACTION_WORKFARM, enjoy);
    girl.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);

    // Money
    girl.m_Tips = max(0, tips);
    girl.m_Pay = max(0, wages);

    // Improve stats
    int xp = 10, skill = 3;

    if (girl.has_active_trait("Quick Learner"))        { skill += 1; xp += 3; }
    else if (girl.has_active_trait("Slow Learner"))    { skill -= 1; xp -= 3; }

    girl.fame(fame);

    girl.exp((rng % xp) + 1);

    // primary (+2 for single or +1 for multiple)
    girl.medicine((rng % skill) + 1);
    girl.animalhandling((rng % skill) + 1);
    // secondary (-1 for one then -2 for others)
    girl.intelligence(max(0, (rng % skill) - 1));
    girl.charisma(max(0, (rng % skill) - 2));
    girl.beastiality(max(0, (rng % skill) - 2));

#pragma endregion
    return false;
}

double JP_FarmVeterinarian(const sGirl& girl, bool estimate)// not used
{
    double jobperformance =
        // primary - first 100
        ((girl.medicine() + girl.animalhandling()) / 2) +
        // secondary - second 100
        ((girl.intelligence() + girl.charisma() + girl.beastiality()) / 3) +
        // level bonus
        girl.level();
    if (!estimate)
    {
        int t = girl.tiredness() - 80;
        if (t > 0)
            jobperformance -= (t + 2) * (t / 3);
    }

    jobperformance += girl.get_trait_modifier("work.veterinarian");

    return jobperformance;
}
