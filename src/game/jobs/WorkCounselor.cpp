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
#include "buildings/cBrothel.h"
#include "cRng.h"
#include <sstream>
#include "cJobManager.h"

// `J` Job Centre - Rehab_Job - Full_Time_Job
bool WorkCounselor(sGirl& girl, bool Day0Night1, cRng& rng)
{    // `J` changed "Drug Counselor" to just "Counselor" so she can help the other therapy patients

    auto brothel = girl.m_Building;

    Action_Types actiontype = ACTION_WORKCOUNSELOR;
    bool SkipDisobey = true;
    girl.m_DayJob = girl.m_NightJob = JOB_COUNSELOR;    // it is a full time job

    stringstream ss;
    int roll_a = rng.d100();
    if (!SkipDisobey)    // `J` skip the disobey check because it has already been done in the building flow
    {
        if (roll_a <= 50 && girl.disobey_check(actiontype, JOB_COUNSELOR))
        {
            ss << "${name} refused to counsel anyone.";
            girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
            girl.upd_Enjoyment(ACTION_WORKREHAB, -1);
            return true;
        }
    }
    ss << "${name} counceled patients.\n \n";



    cGirls::UnequipCombat(&girl);    // not for doctor

    int wages = 25;
    int tips = 0;
    int enjoy = 0;

    /* */if (roll_a <= 10)    { enjoy -= rng % 3 + 1;    ss << "The addicts hasseled her."; }
    else if (roll_a >= 90)    { enjoy += rng % 3 + 1;    ss << "She had a pleasant time working."; }
    else /*             */    { enjoy += rng % 2;        ss << "Otherwise, the shift passed uneventfully."; }

    girl.AddMessage(ss.str(), IMGTYPE_TEACHER, Day0Night1);

    int rehabers = brothel->num_girls_on_job(JOB_REHAB, Day0Night1);
    // work out the pay between the house and the girl
    int roll_max = girl.spirit() + girl.intelligence();
    roll_max /= 4;
    wages += 10 + rng%roll_max;
    wages += 5 * rehabers;    // `J` pay her 5 for each patient you send to her
    girl.m_Tips = max(0, tips);
    girl.m_Pay = max(0, wages);

    // Improve stats
    int xp = 5 + (rehabers / 2), skill = 2 + (rehabers / 2);

    if (girl.has_active_trait("Quick Learner"))        { skill += 1; xp += 3; }
    else if (girl.has_active_trait("Slow Learner"))    { skill -= 1; xp -= 3; }

    girl.exp(xp);
    girl.charisma((rng%skill) + 1);
    girl.service((rng%skill) + 1);

    girl.upd_Enjoyment(actiontype, enjoy);
    //gain traits
    cGirls::PossiblyGainNewTrait(&girl, "Charismatic", 60, actiontype, "Dealing with patients and talking with them about their problems has made ${name} more Charismatic.", Day0Night1 == SHIFT_NIGHT);
    //lose traits
    cGirls::PossiblyLoseExistingTrait(&girl, "Nervous", 30, actiontype, "${name} seems to finally be getting over her shyness. She's not always so Nervous anymore.", Day0Night1 == SHIFT_NIGHT);

    return false;
}

double JP_Counselor(const sGirl& girl, bool estimate)// not used
{
    double jobperformance = 0.0;
    if (estimate)// for third detail string
    {
        jobperformance += girl.charisma() +
            (girl.intelligence() / 2) +
            (girl.confidence() / 4) +
            (girl.morality() / 4) +
            girl.level();
    }
    else// for the actual check
    {
    }

    jobperformance += girl.get_trait_modifier("work.counselor");

    return jobperformance;
}
