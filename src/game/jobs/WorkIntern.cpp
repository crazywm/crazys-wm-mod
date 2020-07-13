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
#include "cJobManager.h"
#include "cRng.h"
#include "CLog.h"
#include "cGold.h"
#include "buildings/cBuildingManager.h"
#include "cJobManager.h"

// `J` Job Clinic - Staff - Learning_Job
bool WorkIntern(sGirl& girl, bool Day0Night1, cRng& rng)
{
    auto brothel = girl.m_Building;

    Action_Types actiontype = ACTION_WORKTRAINING;
    stringstream ss;
    if (girl.has_active_trait("AIDS"))
    {
        ss << "Health laws prohibit anyone with AIDS from working in the Medical profession so ${name} was sent to the waiting room.";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
        girl.FullJobReset(JOB_CLINICREST);
        return false;
    }
    if (girl.medicine() + girl.intelligence() + girl.charisma() >= 300)
    {
        ss << "There is nothing more she can learn here so she is promoted to ";
        if (girl.is_slave())    { ss << "Nurse.";    girl.m_DayJob = girl.m_NightJob = JOB_NURSE; }
        else /*            */    { ss << "Doctor.";    girl.m_DayJob = girl.m_NightJob = JOB_DOCTOR; }
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);
        return false;    // not refusing
    }

    if (girl.disobey_check(actiontype, JOB_INTERN))            // they refuse to work
    {
        ss << "${name} refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    }
    ss << "${name} trains in the Medical field.\n \n";

    cGirls::UnequipCombat(girl);    // put that shit away

    int enjoy = 0;                                                //
    int wages = 0;                                                //
    int tips = 0;                                                //
    int train = 0;                                                // main skill trained
    int tmed = girl.medicine();                                // Starting level - train = 1
    int tint = girl.intelligence();                            // Starting level - train = 2
    int tcha = girl.charisma();                                // Starting level - train = 3
    bool gaintrait = false;                                        // posibility of gaining a trait
    bool promote = false;                                        // posibility of getting promoted to Doctor or Nurse
    int skill = 0;                                                // gian for main skill trained
    int dirtyloss = brothel->m_Filthiness / 100;                // training time wasted with bad equipment
    int sgMed = 0, sgInt = 0, sgCha = 0;                        // gains per skill
    int roll_a = rng.d100();                                    // roll for main skill gain
    int roll_b = rng.d100();                                    // roll for main skill trained
    int roll_c = rng.d100();                                    // roll for enjoyment





    /* */if (roll_a <= 5)    skill = 7;
    else if (roll_a <= 15)    skill = 6;
    else if (roll_a <= 30)    skill = 5;
    else if (roll_a <= 60)    skill = 4;
    else /*             */    skill = 3;
    /* */if (girl.has_active_trait("Quick Learner"))    { skill += 1; }
    else if (girl.has_active_trait("Slow Learner"))    { skill -= 1; }
    skill -= dirtyloss;
    ss << "The Clinic is ";
    if (dirtyloss <= 0) ss << "clean and tidy";
    if (dirtyloss == 1) ss << "dirty and the equipment has not been put back in its place";
    if (dirtyloss == 2) ss << "messy. The equipment is damaged and strewn about the building";
    if (dirtyloss == 3) ss << "filthy and some of the equipment is broken";
    if (dirtyloss >= 4) ss << "in complete disarray and the equipment barely usable";
    ss << ".\n \n";
    if (skill < 1) skill = 1;    // always at least 1

    do{        // `J` New method of selecting what job to do
        /* */if (roll_b < 40  && tmed < 100)    train = 1;    // medicine
        else if (roll_b < 70  && tint < 100)    train = 2;    // intelligence
        else if (roll_b < 100 && tcha < 100)    train = 3;    // charisma
        roll_b -= 10;
    } while (train == 0 && roll_b > 0);
    if (train == 0 || rng.percent(5)) gaintrait = true;
    if (train == 0 && girl.medicine() > 70 && girl.intelligence() > 70)    promote = true;

    if (train == 1) { sgMed = skill; ss << "She learns how to work with medicine better.\n"; }    else sgMed = rng % 3;
    if (train == 2) { sgInt = skill; ss << "She got smarter today.\n"; }                        else sgInt = rng % 2;
    if (train == 3) { sgCha = skill; ss << "She got more charismatic today.\n"; }                else sgCha = rng % 2;

    if (sgMed + sgInt + sgCha > 0)
    {
        ss << "She managed to gain:\n";
        if (sgMed > 0) { ss << sgMed << " Medicine.\n";        girl.medicine(sgMed); }
        if (sgInt > 0) { ss << sgInt << " Intelligence.\n";    girl.intelligence(sgInt); }
        if (sgCha > 0) { ss << sgCha << " Charisma.\n";        girl.charisma(sgCha); }
    }

    int trycount = 10;
    while (gaintrait && trycount > 0)    // `J` Try to add a trait
    {
        trycount--;
        switch (rng % 10)
        {
        case 0:
            if (girl.lose_trait( "Nervous"))
            {
                ss << "She seems to be getting over her Nervousness with her training.";
                gaintrait = false;
            }
            break;
        case 1:
            if (girl.lose_trait( "Meek"))
            {
                ss << "She seems to be getting over her Meakness with her training.";
                gaintrait = false;
            }
            break;
        case 2:
            if (girl.lose_trait( "Dependant"))
            {
                ss << "She seems to be getting over her Dependancy with her training.";
                gaintrait = false;
            }
            break;
        case 3:
            if (girl.gain_trait( "Charismatic"))
            {
                ss << "Dealing with patients and talking with them about their problems has made ${name} more Charismatic.";
                gaintrait = false;
            }
            break;
        case 4:
            break;
        case 5:

            break;
        case 6:

            break;

        default:    break;    // no trait gained
        }
    }



    //enjoyed the work or not
    /* */if (roll_c <= 10)    { enjoy -= rng % 3 + 1;    ss << "Some of the patrons abused her during the shift."; }
    else if (roll_c >= 90)    { enjoy += rng % 3 + 1;    ss << "She had a pleasant time working."; }
    else /*             */    { enjoy += rng % 2;        ss << "Otherwise, the shift passed uneventfully."; }
    girl.upd_Enjoyment(actiontype, enjoy);

    girl.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);

    if (girl.is_unpaid()) { wages = 0; }
    else { wages = 25 + (skill * 5); } // `J` Pay her more if she learns more
    girl.m_Tips = max(0, tips);
    girl.m_Pay = max(0, wages);

    // Improve stats
    int xp = 5 + skill;

    if (girl.has_active_trait("Quick Learner"))        { xp += 2; }
    else if (girl.has_active_trait("Slow Learner"))    { xp -= 2; }

    girl.exp((rng % xp) + 1);
    girl.upd_temp_stat(STAT_LIBIDO, skill / 2);

    if (girl.medicine() + girl.intelligence() + girl.charisma() >= 300) promote = true;
    if (promote)
    {
        ss.str("");
        ss << "${name} has completed her Internship and has been promoted to ";
        if (girl.is_slave())    { ss << "Nurse.";    girl.m_DayJob = girl.m_NightJob = JOB_NURSE; }
        else /*            */    { ss << "Doctor.";    girl.m_DayJob = girl.m_NightJob = JOB_DOCTOR; }
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);
    }

    return false;
}

double JP_Intern(const sGirl& girl, bool estimate)// not used
{
    double jobperformance = 0.0;
    if (estimate)// for third detail string
    {
        jobperformance +=
            (100 - girl.medicine()) +
            (100 - girl.intelligence()) +
            (100 - girl.charisma());

        // traits she could gain/lose
        if (girl.has_active_trait("Nervous")) jobperformance += 20;
        if (!girl.has_active_trait("Charismatic")) jobperformance += 20;

    }
    else// for the actual check
    {
    }
    return jobperformance;
}
