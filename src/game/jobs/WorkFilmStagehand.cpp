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
#include "cRng.h"
#include "buildings/cMovieStudio.h"
#include "buildings/queries.h"
#include <sstream>
#include "cJobManager.h"
#include "cGirls.h"

// `J` Job Movie Studio - Crew - job_is_cleaning
bool WorkFilmStagehand(sGirl& girl, bool Day0Night1, cRng& rng)
{
    auto brothel = dynamic_cast<sMovieStudio*>(girl.m_Building);

    Action_Types actiontype = ACTION_MOVIECREW; Action_Types actiontype2 = ACTION_WORKCLEANING;
    std::stringstream ss;
    int roll_a = rng.d100();
    if (roll_a <= 50 && (girl.disobey_check(actiontype, JOB_STAGEHAND) || girl.disobey_check(actiontype2, JOB_STAGEHAND)))
    {
        ss << "${name} refused to work as a stagehand today.";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    }
    ss << "${name} worked as a stagehand.\n \n";

    cGirls::UnequipCombat(girl);    // not for studio crew
    int enjoyc = 0, enjoym = 0;
    int wages = 50;
    int tips = 0;
    int imagetype = IMGTYPE_PROFILE;
    bool filming = true;


    // `J` - jobperformance and CleanAmt need to be worked out specially for this job.
    double jobperformance = 0;
    double CleanAmt = ((girl.service() / 10.0) + 5) * 5;
    CleanAmt += girl.get_trait_modifier("work.stagehand.clean-amount");
    jobperformance += girl.get_trait_modifier("work.stagehand.performance");

    if (brothel->num_girls_on_job(JOB_CAMERAMAGE, SHIFT_NIGHT) == 0 ||
        brothel->num_girls_on_job(JOB_CRYSTALPURIFIER, SHIFT_NIGHT) == 0 ||
        Num_Actress(*brothel) < 1)
    {
        ss << "There were no scenes being filmed, so she just cleaned the set.\n \n";
        filming = false;
        imagetype = IMGTYPE_MAID;
    }

    if (roll_a <= 10)
    {
        enjoyc -= rng % 3 + 1; if (filming) enjoym -= rng % 3 + 1;
        CleanAmt *= 0.8;
        ss << "She did not like working in the studio today.";
    }
    else if (roll_a >= 90)
    {
        enjoyc += rng % 3 + 1; if (filming) enjoym += rng % 3 + 1;
        CleanAmt *= 1.1;
        ss << "She had a great time working today.";
    }
    else
    {
        enjoyc += std::max(0, rng % 3 - 1); if (filming) enjoym += std::max(0, rng % 3 - 1);
        ss << "Otherwise, the shift passed uneventfully.";
    }
    jobperformance += enjoyc + enjoym;
    ss << "\n \n";

    CleanAmt = std::min((int)CleanAmt, brothel->m_Filthiness);

    if (filming)
    {
        jobperformance += (girl.crafting() / 5) + (girl.constitution() / 10) + (girl.service() / 10);
        jobperformance += girl.level();
        jobperformance += rng % 4 - 1;    // should add a -1 to +3 random element --PP

        // Cleaning reduces the points remaining for actual stage hand work
        jobperformance -= CleanAmt / 2;
        if(jobperformance < 0 && CleanAmt > 0) {
            ss << "Your studio was so messy that ${name} spent the entire shift cleaning up, and had no time to "
                  "assist in movie production. She improved the cleanliness rating by " << (int)CleanAmt << ".";
            jobperformance = 0;
        } else {
            ss << "She assisted the crew in movie production and provided " << (int)jobperformance << " stage hand points.";
        }
    }


    // slave girls not being paid for a job that normally you would pay directly for do less work
    if (girl.is_unpaid())
    {
        CleanAmt *= 0.9;
        wages = 0;
    }
    else if (filming)
    {
        wages += int(CleanAmt + jobperformance);
    }
    else
    {
        wages += int(CleanAmt);
    }

    if (!filming && brothel->m_Filthiness < CleanAmt / 2) // `J` needs more variation
    {
        ss << "\n \n${name} finished her cleaning early so she hung out around the Studio a bit.";
        girl.upd_temp_stat(STAT_LIBIDO, rng % 3 + 1, true);
        girl.happiness(rng % 3 + 1);
    }


    girl.AddMessage(ss.str(), imagetype, EVENT_NIGHTSHIFT);

    brothel->m_StageHandPoints += int(jobperformance);
    brothel->m_Filthiness = std::max(0, brothel->m_Filthiness - int(CleanAmt));
    girl.m_Tips = std::max(0, tips);
    girl.m_Pay = std::max(0, wages);


    // Improve girl
    int xp = filming ? 10 : 5, skill = 3;
    if (enjoyc + enjoym > 2)                            { xp += 1; skill += 1; }
    if (girl.has_active_trait("Quick Learner"))        { skill += 1; xp += 3; }
    else if (girl.has_active_trait("Slow Learner"))    { skill -= 1; xp -= 3; }

    girl.exp(xp);
    girl.service((rng % skill) + 2);

    if (filming) girl.upd_Enjoyment(actiontype, enjoym);
    girl.upd_Enjoyment(actiontype2, enjoyc);
    // Gain Traits
    if (rng.percent(girl.service()))
        cGirls::PossiblyGainNewTrait(girl, "Maid", 90, actiontype2, "${name} has cleaned enough that she could work professionally as a Maid anywhere.", Day0Night1);
    //lose traits
    cGirls::PossiblyLoseExistingTrait(girl, "Clumsy", 30, actiontype2, "It took her spilling hundreds of buckets, and just as many reprimands, but ${name} has finally stopped being so Clumsy.", Day0Night1);

    return false;
}
