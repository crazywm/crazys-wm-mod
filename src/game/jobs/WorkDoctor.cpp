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
#include "buildings/cBuildingManager.h"
#include "cRng.h"
#include <sstream>

// `J` Job Clinic - Staff
bool WorkDoctor(sGirl& girl, bool Day0Night1, cRng& rng)
{
    auto brothel = girl.m_Building;

    Action_Types actiontype = ACTION_WORKDOCTOR;
    bool SkipDisobey = true; // summary == "SkipDisobey");
    stringstream ss;
    if (girl.has_active_trait("AIDS"))
    {
        ss << "Health laws prohibit anyone with AIDS from working in the Medical profession so ${name} was sent to the waiting room.";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
        girl.m_PrevDayJob = girl.m_PrevNightJob = girl.m_DayJob = girl.m_NightJob = JOB_CLINICREST;
        return false;
    }
    if (girl.is_slave())
    {
        ss << "Slaves are not allowed to be Doctors so ${name} was reassigned to being a Nurse.";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
        girl.m_PrevDayJob = girl.m_PrevNightJob = girl.m_DayJob = girl.m_NightJob = JOB_NURSE;
        return false;
    }
    if (girl.medicine() < 50 || girl.intelligence() < 50)
    {
        ss << "${name} does not have enough training to work as a Doctor. She has been reassigned to Internship so she can learn what she needs.";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
        girl.m_PrevDayJob = girl.m_PrevNightJob = girl.m_DayJob = girl.m_NightJob = JOB_INTERN;
        return false;
    }
    if (!SkipDisobey)    // `J` skip the disobey check because it has already been done in the building flow
    {
        if (girl.disobey_check(actiontype, JOB_DOCTOR))            // they refuse to work
        {
            ss << "${name} refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
            girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
            return true;
        }
    }
    ss << "${name} worked as a Doctor.\n";


    cGirls::UnequipCombat(&girl);    // put that shit away, you'll scare off the patients!

    int enjoy = 0, wages = 100, tips = 0;

    // this will be added to the clinic's code eventually - for now it is just used for her pay
    int patients = 0;            // `J` how many patients the Doctor can see in a shift


    // Doctor is a full time job now
    girl.m_DayJob = girl.m_NightJob = JOB_DOCTOR;


    double jobperformance = girl.job_performance(JOB_DOCTOR, false);

    //enjoyed the work or not
    int roll = rng.d100();
    if (roll <= 10)
    {
        enjoy -= rng % 3 + 1;
        jobperformance *= 0.9;
        ss << "Some of the patients abused her during the shift.\n";
    }
    else if (roll >= 90)
    {
        enjoy += rng % 3 + 1;
        jobperformance *= 1.1;
        ss << "She had a pleasant time working.\n";
    }
    else
    {
        enjoy += rng % 2;
        ss << "Otherwise, the shift passed uneventfully.\n";
    }

    girl.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);
    patients += (int)(jobperformance / 10);        // `J` 1 patient per 10 point of performance

    /* `J` this will be a place holder until a better payment system gets done
    *  this does not take into account any of your girls in surgery
    */
    int earned = 0;
    for (int i = 0; i < patients; i++)
    {
        earned += rng % 50 + 50; // 50-100 gold per customer
    }
    brothel->m_Finance.clinic_income(earned);
    ss.str("");
    ss << "${name} earned " << earned << " gold from taking care of " << patients << " patients.\n";
    girl.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);

    wages += (patients * 10);
    girl.m_Tips = max(0, tips);
    girl.m_Pay = max(0, wages);

    // Improve stats
    int xp = 10 + (patients / 2),  skill = 1 + (patients / 3);

    if (girl.has_active_trait("Quick Learner"))        { skill += 1; xp += 3; }
    else if (girl.has_active_trait("Slow Learner"))    { skill -= 1; xp -= 3; }
    if (girl.has_active_trait("Lesbian"))                { girl.upd_temp_stat(STAT_LIBIDO, patients / 2); }

    girl.exp(xp);
    girl.intelligence(skill);
    girl.medicine(skill);
    girl.service(1);

    girl.upd_Enjoyment(actiontype, enjoy);

    return false;
}

double JP_Doctor(const sGirl& girl, bool estimate)// not used
{
    double jobperformance =
        (girl.intelligence() +
        girl.medicine() +
        girl.level() / 5);
    if (!estimate)
    {
        int t = girl.tiredness() - 80;
        if (t > 0)
            jobperformance -= (t + 2) * (t / 3);
    }

    jobperformance += girl.get_trait_modifier("work.doctor");

    return jobperformance;
}
