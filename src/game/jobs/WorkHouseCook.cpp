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
#include "cRng.h"
#include <sstream>
#include "buildings/cBuildingManager.h"

// `J` Job House - Cook
bool WorkHouseCook(sGirl& girl, bool Day0Night1, cRng& rng)
{
    auto brothel = girl.m_Building;

    Action_Types actiontype = ACTION_WORKCOOKING;
    stringstream ss;
    int roll_a = rng.d100();
    if (roll_a <= 50 && girl.disobey_check(actiontype, JOB_HOUSECOOK))
    {
        ss << "${name} refused to cook for your house.";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    }
    ss << "${name} worked cooking for your house.\n \n";

    int enjoy = 0;
    int wages = 30, tips = 0;
    double jobperformance = girl.job_performance(JOB_HOUSECOOK, false);


    cGirls::UnequipCombat(girl);    // put that shit away

    #pragma endregion
#pragma region //    Job Performance            //
    if (jobperformance >= 245)
    {
        ss << " She must be the perfect at this.";
        brothel->update_all_girls_stat(STAT_HAPPINESS, 3);
        wages += 20;
    }
    else if (jobperformance >= 185)
    {
        ss << " She's unbelievable at this.";
        brothel->update_all_girls_stat(STAT_HAPPINESS, 2);
        wages += 15;
    }
    else if (jobperformance >= 145)
    {
        ss << " She's good at this job.";
        brothel->update_all_girls_stat(STAT_HAPPINESS, 1);
        wages += 10;
    }
    else if (jobperformance >= 100)
    {
        ss << " She made a few mistakes but overall she is okay at this.";
        wages += 5;
    }
    else if (jobperformance >= 70)
    {
        ss << " She was nervous and made a few mistakes. She isn't that good at this.";
        brothel->update_all_girls_stat(STAT_HAPPINESS, -1);
        wages -= 5;
    }
    else
    {
        ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.";
        brothel->update_all_girls_stat(STAT_HAPPINESS, -2);
        wages -= 15;
    }
    ss << "\n \n";

#pragma endregion
#pragma region    //    Enjoyment and Tiredness        //


    //enjoyed the work or not
    int roll = rng.d100();
    if (roll <= 5)
    {
        ss << "Some of the girls made fun of her cooking during the shift.";
        enjoy -= 1;
    }
    else if (roll <= 25)
    {
        ss << "She had a pleasant time working.";
        enjoy += 3;
    }
    else
    {
        ss << "Otherwise, the shift passed uneventfully.";
        enjoy += 1;
    }


#pragma endregion
#pragma region    //    Money                    //


    // slave girls not being paid for a job that normally you would pay directly for do less work
    if (girl.is_unpaid())
    {
        wages = 0;
    }
    else
    {
        // Money
        girl.m_Tips = max(0, tips);
        girl.m_Pay = max(0, wages);
    }
#pragma endregion


    // do all the output
    girl.AddMessage(ss.str(), IMGTYPE_COOK, Day0Night1);


    // Improve girl
    int xp = 15, skill = 3;
    if (enjoy > 1)                                        { xp += 1; skill += 1; }
    if (girl.has_active_trait("Quick Learner"))        { skill += 1; xp += 3; }
    else if (girl.has_active_trait("Slow Learner"))    { skill -= 1; xp -= 3; }

    girl.exp((rng % xp) + 2);
    girl.cooking((rng % skill) + 2);

    girl.upd_Enjoyment(actiontype, enjoy);

    return false;
}

double JP_HouseCook(const sGirl& girl, bool estimate)// not used
{
    double jobperformance =
        // primary - first 100
        girl.cooking() +
        // secondary - second 100
        ((girl.intelligence() + girl.confidence()) / 2) +
        // level bonus
        girl.level();
    if (!estimate)
    {
        int t = girl.tiredness() - 80;
        if (t > 0)
            jobperformance -= (t + 2) * (t / 3);
    }

    jobperformance += girl.get_trait_modifier("work.housecook");

    return jobperformance;
}
