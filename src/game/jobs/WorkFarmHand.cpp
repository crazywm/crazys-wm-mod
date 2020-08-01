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
#include "buildings/cBuildingManager.h"
#include <sstream>

// `J` Job Farm - Staff - job_is_cleaning
bool WorkFarmHand(sGirl& girl, bool Day0Night1, cRng& rng)
{
    auto brothel = girl.m_Building;

    Action_Types actiontype = ACTION_WORKFARM; Action_Types actiontype2 = ACTION_WORKCLEANING;
    stringstream ss;
    int roll_a = rng.d100(), roll_b = rng.d100(), roll_c = rng.d100();
    if (roll_a <= 50 && (girl.disobey_check(actiontype, JOB_FARMHAND) || girl.disobey_check(actiontype2, JOB_FARMHAND)))
    {
        ss << "${name} refused to work on the farm.";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    }
    ss << "${name} worked cleaning and repairing the farm.\n \n";

    cGirls::UnequipCombat(girl);    // put that shit away

    double CleanAmt = girl.job_performance(JOB_FARMHAND, false);
    int enjoyC = 0, enjoyF = 0;
    int wages = 0;
    int tips = 0;
    bool playtime = false;

    if (roll_a <= 10)
    {
        enjoyC -= rng % 3; enjoyF -= rng % 3;
        CleanAmt = CleanAmt * 0.8;
        /* */if (roll_b < 30)    ss << "She spilled a bucket of something unpleasant all over herself.";
        else if (roll_b < 60)    ss << "She stepped in something unpleasant.";
        else /*            */    ss << "She did not like working on the farm today.";
    }
    else if (roll_a >= 90)
    {
        enjoyC += rng % 3; enjoyF += rng % 3;
        CleanAmt = CleanAmt * 1.1;
        /* */if (roll_b < 50)    ss << "She cleaned the building while humming a pleasant tune.";
        else /*            */    ss << "She had a great time working today.";
    }
    else
    {
        enjoyC += rng % 2; enjoyF += rng % 2;
        ss << "The shift passed uneventfully.";
    }
    ss << "\n \n";

    // slave girls not being paid for a job that normally you would pay directly for do less work
    if (girl.is_unpaid())
    {
        CleanAmt = CleanAmt * 0.9;
        wages = 0;
    }
    else
    {
        wages = int(CleanAmt); // `J` Pay her based on how much she cleaned
    }

    // `J` if she can clean more than is needed, she has a little free time after her shift
    if (brothel->m_Filthiness < CleanAmt / 2) playtime = true;
    ss << "\n \nCleanliness rating improved by " << int(CleanAmt);
    if (playtime)    // `J` needs more variation
    {
        ss << "\n \n${name} finished her cleaning early so she ";
        if (Day0Night1 == SHIFT_DAY && roll_c % 3 == 1)    // 33% chance she will watch the sunset when working day shift
        {
            ss << "sat beneath an oak tree and watched the sunset.";
            girl.tiredness(-((rng % 5) + 2));
        }
        else if (roll_c < 25)
        {
            ss << "played with the baby animals a bit.";
            girl.animalhandling((rng % 2) + 1);
        }
        else if (roll_c < 50)
        {
            ss << "played in the dirt a bit.";
            girl.farming((rng % 2));
        }
        else
        {
            ss << "sat in a rocking chair on the farm house front porch whittling.";
            girl.crafting((rng % 3));
            girl.tiredness(-(rng % 3));
        }
        girl.happiness((rng % 4) + 2);
    }

#if 0

    // `J` Farm Bookmark - adding in items that can be created in the farm















#endif




    // do all the output
    girl.AddMessage(ss.str(), IMGTYPE_MAID, Day0Night1 ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
    brothel->m_Filthiness -= int(CleanAmt);

    // Money
    girl.m_Tips = max(0, tips);
    girl.m_Pay = max(0, wages);

    // Improve girl
    int xp = 5, libido = 1, skill = 3;

    if (enjoyC + enjoyF > 2)                            { xp += 1; skill += 1; }
    if (girl.has_active_trait("Quick Learner"))        { skill += 1; xp += 3; }
    else if (girl.has_active_trait("Slow Learner"))    { skill -= 1; xp -= 3; }
    if (girl.has_active_trait("Nymphomaniac"))            { libido += 2; }

    girl.exp((rng % xp) + 1);
    girl.upd_temp_stat(STAT_LIBIDO, libido);

    // primary (+2 for single or +1 for multiple)
    girl.service((rng % skill));
    // secondary (-1 for one then -2 for others)
    girl.crafting(max(0, (rng % skill) - 1));
    girl.farming(max(0, (rng % skill) - 2));
    girl.strength(max(0, (rng % skill) - 2));

    girl.upd_Enjoyment(actiontype, enjoyF);
    girl.upd_Enjoyment(actiontype2, enjoyC);
    // Gain Traits
    if (rng.percent(girl.service()))
        cGirls::PossiblyGainNewTrait(girl, "Maid", 90, actiontype2, "${name} has cleaned enough that she could work professionally as a Maid anywhere.", Day0Night1);
    // Lose Traits
    if (rng.percent(girl.service()))
        cGirls::PossiblyLoseExistingTrait(girl, "Clumsy", 30, actiontype2, "It took her spilling hundreds of buckets, and just as many reprimands, but ${name} has finally stopped being so Clumsy.", Day0Night1);

    return false;
}
