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
#include "buildings/cMovieStudio.h"
#include "buildings/queries.hpp"
#include <sstream>


// `J` Job Movie Studio - Crew
bool WorkFluffer(sGirl& girl, bool Day0Night1, cRng& rng)
{
    auto brothel = dynamic_cast<sMovieStudio*>(girl.m_Building);

    stringstream ss;

    // No film crew.. then go home    // `J` this will be taken care of in building flow, leaving it in for now
    if (brothel->num_girls_on_job(JOB_CAMERAMAGE, SHIFT_NIGHT) == 0 || brothel->num_girls_on_job(JOB_CRYSTALPURIFIER, SHIFT_NIGHT) == 0)
    {
        girl.AddMessage("There was no crew to film the scene, so she took the day off", IMGTYPE_PROFILE, EVENT_NOWORK);
        return false;    // not refusing
    }
    else if (Num_Actress(*brothel) < 1)
    {
        girl.AddMessage("There were no actresses to film, so she took the day off", IMGTYPE_PROFILE, EVENT_NOWORK);
        return false;    // not refusing
    }
    int roll = rng.d100();
    if (roll <= 10 && girl.disobey_check(ACTION_WORKMOVIE, JOB_FLUFFER))
    {
        ss << "${name} refused to work as a fluffer today";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    }
    ss << "${name} worked as a fluffer.\n \n";


    cGirls::UnequipCombat(girl);    // not for studio crew

    int wages = 50, tips = 0;
    int enjoy = 0;

    if (roll <= 10)
    {
        enjoy -= rng % 3 + 1;
        ss << "She didn't like having so many dicks in her mouth today.\n \n";
    }
    else if (roll >= 90)
    {
        enjoy += rng % 3 + 1;
        ss << "She loved sucking cock today.\n \n";
    }
    else
    {
        enjoy += rng % 2;
        ss << "She had a pleasant day keeping the actors ready to work.\n \n";
    }
    double jobperformance = girl.job_performance(JOB_FLUFFER, false);
    jobperformance += enjoy * 2;

    // slave girls not being paid for a job that normally you would pay directly for do less work
    if (girl.is_unpaid())
    {
        jobperformance *= 0.9;
        wages = 0;
    }
    else
    {
        wages += (int)jobperformance;
    }

    /* */if (jobperformance > 0)    ss << "She helped improve the scene " << (int)jobperformance << "% by keeping the actors happy with her mouth. \n";
    else if (jobperformance < 0)    ss << "She performed poorly, she reduced the scene quality " << (int)jobperformance << "% with her lack of passion while sucking dicks. \n";
    else /*                   */    ss << "She did not really effect the scene quality.\n";

    girl.AddMessage(ss.str(), IMGTYPE_ORAL, Day0Night1);
    brothel->m_FlufferQuality += (int)jobperformance;
    girl.m_Tips = max(0, tips);
    girl.m_Pay = max(0, wages);

    // Improve girl
    int xp = 10, skill = 1;

    if (girl.has_active_trait("Quick Learner"))        { skill += 1; xp += 3; }
    else if (girl.has_active_trait("Slow Learner"))    { skill -= 1; xp -= 3; }

    girl.exp(xp);
    girl.service(skill);

    return false;
}


double JP_Fluffer(const sGirl& girl, bool estimate)// not used
{
    double jobperformance = 0.0;
    if (estimate)// for third detail string
    {
        jobperformance +=
            (girl.oralsex() / 2) +
            (girl.beauty() / 2) +
            (girl.spirit() / 2) +
            (girl.service() / 2) +
            girl.level();
    }
    else// for the actual check
    {
        int t = girl.tiredness() - 80;
        if (t > 0)
            jobperformance -= t / 4;
        jobperformance += (girl.oralsex() + girl.beauty() + girl.spirit()) / 30;
        jobperformance += girl.service() / 10;
        jobperformance += girl.level();
        jobperformance += g_Dice % 4 - 1;       // should add a -1 to +3 random element --PP
    }
    return jobperformance;
}
