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

#include <sstream>
#include "cClinic.h"
#include "cGangs.h"
#include "IGame.h"
#include "cJobManager.h"
#include "utils/algorithms.hpp"
#include "character/predicates.h"
#include "scripting/GameEvents.h"
#include "cGirls.h"


extern cRng    g_Dice;

// // ----- Strut sClinic Create / destroy
sClinic::sClinic() : IBuilding(BuildingType::CLINIC, "Clinic")
{
    m_FirstJob = JOB_GETHEALING;
    m_LastJob = JOB_JANITOR;
    m_MatronJob = JOB_CHAIRMAN;
    m_MeetGirlData.Event = EDefaultEvent::MEET_GIRL_CLINIC;
}

sClinic::~sClinic()    = default;

// Run the shifts
void sClinic::UpdateGirls(bool is_night)    // Start_Building_Process_B
{
    // `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >> cClinic.cpp
    std::stringstream ss;
    std::string girlName;

    int numDoctors = 0;

    BeginShift(is_night);

    ////////////////////////////////////////////////////////
    //  JOB_DOCTOR needs to be checked before all others  //
    ////////////////////////////////////////////////////////
    m_Girls->apply([&](auto& current) {
        auto sw = current.get_job(is_night);
        if (current.is_dead() || sw != JOB_DOCTOR)
        {    // skip dead girls and anyone who is not a doctor
            return;
        }
        auto sum = EVENT_SUMMARY;
        ss.str("");

        if (current.has_active_trait("AIDS"))
        {
            ss << "${name} has AIDS! She has to get it treated before she can go back to work as a Doctor.";
            current.m_DayJob = current.m_NightJob = JOB_CUREDISEASES;
        }
        else if (current.is_slave())
        {
            ss << "Doctors can not be slaves so ${name} was demoted to Nurse.";
            current.m_DayJob = current.m_NightJob = JOB_NURSE;
        }
        else if (current.intelligence() < 50 || current.medicine() < 50)
        {
            ss << "${name} is not qualified to be a Doctor so she was sent back to being an Intern.";
            current.m_DayJob = current.m_NightJob = JOB_NURSE;
        }
        else if (current.disobey_check(ACTION_WORKDOCTOR, JOB_DOCTOR))
        {
            (is_night ? current.m_Refused_To_Work_Night = true : current.m_Refused_To_Work_Day = true);
            m_Fame -= current.fame();
            ss << "${name} refused to work as a Doctor so made no money.";
            sum = EVENT_NOWORK;
        }
        else
        {
            numDoctors++;
        }
        if (ss.str().length() > 0) current.AddMessage(ss.str(), IMGTYPE_PROFILE, sum);
    });

    ////////////////////////////////////////////////////////////////
    //  Interns and Nurses can be promoted to doctor if need be.  //
    ////////////////////////////////////////////////////////////////
    if(get_active_matron())
    {
        if(numDoctors < 1)
            m_Girls->apply([&](sGirl& current){
                if (numDoctors >= 1)
                    return;
                if(promote_to_doctor(current, JOB_INTERN, is_night))
                    numDoctors += 1;
            });
        if(numDoctors < 1)
            m_Girls->apply([&](sGirl& current){
                if (numDoctors >= 1)
                    return;
                if(promote_to_doctor(current, JOB_NURSE, is_night))
                    numDoctors += 1;
            });
    }

    /////////////////////////////////////
    //  Do all the Clinic staff jobs.  //
    /////////////////////////////////////
    m_Girls->apply([is_night](auto& current) {
        auto sw = current.get_job(is_night);
        if (current.is_dead() || (sw != JOB_INTERN && sw != JOB_NURSE && sw != JOB_JANITOR && sw != JOB_MECHANIC && sw != JOB_DOCTOR) ||
            // skip dead girls and anyone who is not staff
            (sw == JOB_DOCTOR && ((is_night == SHIFT_DAY && current.m_Refused_To_Work_Day)||(is_night == SHIFT_NIGHT && current.m_Refused_To_Work_Night))))
        {    // and skip doctors who refused to work in the first check
            return;
        }

        g_Game->job_manager().handle_simple_job(current, is_night);
    });


    ///////////////////////////////////////////////////////////////////////
    //  Do all the surgery jobs. Not having a doctor is in all of them.  //
    ///////////////////////////////////////////////////////////////////////
    m_Girls->apply([is_night](auto& current) {
        auto sw = current.get_job(is_night);
        if (current.is_dead() || !is_in((JOBS)sw, {JOB_GETHEALING, JOB_GETREPAIRS, JOB_GETABORT, JOB_COSMETICSURGERY,
                                                    JOB_LIPO, JOB_BREASTREDUCTION, JOB_BOOBJOB, JOB_VAGINAREJUV, JOB_FACELIFT,
                                                    JOB_ASSJOB, JOB_TUBESTIED, JOB_CUREDISEASES, JOB_FERTILITY}))
        {    // skip dead girls and anyone not a patient
            return;
        }

        // do their surgery
        g_Game->job_manager().do_job(current, is_night);
    });

    EndShift(is_night);
}

void sClinic::auto_assign_job(sGirl& target, std::stringstream& message, bool is_night)
{
    std::stringstream& ss = message;
    // if they have no job at all, assign them a job
    ss << "The Chairman assigns " << target.FullName() << " to ";
    if (target.has_active_trait("AIDS"))                                        // anyone with AIDS
    {
        target.m_DayJob = target.m_NightJob = JOB_CUREDISEASES;        //  needs to get it treated right away
        ss << "get her AIDS treated right away.";
    }
    else if ((target.is_free() &&                                        // assign any free girl
             (target.intelligence() > 70 && target.medicine() > 70))    // who is well qualified
             || (num_girls_on_job(JOB_DOCTOR, is_night) < 1 &&            // or if there are no doctors yet
                 target.intelligence() >= 50 && target.medicine() >= 50))    // assign anyone who qualifies
    {
        target.m_DayJob = target.m_NightJob = JOB_DOCTOR;            // as a Doctor
        ss << "work as a Doctor.";
    }
    else if (num_girls_on_job(JOB_NURSE, is_night) < 1)            // make sure there is at least 1 Nurse
    {
        target.m_DayJob = target.m_NightJob = JOB_NURSE;
        ss << "work as a Nurse.";
    }
    else if (has_disease(target))                                    // treat anyone with a disease
    {
        target.m_DayJob = target.m_NightJob = JOB_CUREDISEASES;
        std::vector<const char*> diseases;
        if (target.has_active_trait("Herpes"))          diseases.emplace_back("Herpes");
        if (target.has_active_trait("Chlamydia"))       diseases.emplace_back("Chlamydia");
        if (target.has_active_trait("Syphilis"))        diseases.emplace_back("Syphilis");
        if (target.has_active_trait("AIDS"))            diseases.emplace_back("AIDS");
        int numdiseases = diseases.size();
        ss << "get her " << (numdiseases > 1 ? "diseases" : diseases[0]) << " treated.";
    }
        // then make sure there is at least 1 Janitor and 1 Mechanic
    else if (num_girls_on_job(JOB_MECHANIC, is_night) < 1)
    {
        target.m_DayJob = target.m_NightJob = JOB_MECHANIC;
        ss << "work as a Mechanic.";
    }
    else if (num_girls_on_job(JOB_JANITOR, is_night) < 1)
    {
        target.m_DayJob = target.m_NightJob = JOB_JANITOR;
        ss << "work as a Janitor.";
    }
        // then add more of each job as numbers permit
    else if (target.medicine() > 30 && num_girls_on_job(JOB_NURSE, is_night) < num_girls() / 10)
    {
        target.m_DayJob = target.m_NightJob = JOB_NURSE;
        ss << "work as a Nurse.";
    }
    else if (num_girls_on_job(JOB_MECHANIC, is_night) < num_girls() / 20)
    {
        target.m_DayJob = target.m_NightJob = JOB_MECHANIC;
        ss << "work as a Mechanic.";
    }
    else if (num_girls_on_job(JOB_JANITOR, is_night) < num_girls() / 20)
    {
        target.m_DayJob = target.m_NightJob = JOB_JANITOR;
        ss << "work as a Janitor.";
    }
    else    // assign anyone else to Internship
    {
        target.m_DayJob = target.m_NightJob = JOB_INTERN;
        ss << "work as an Intern.";
    }
}

bool sClinic::handle_back_to_work(sGirl& girl, std::stringstream& ss, bool is_night)
{
    JOBS firstjob = JOB_GETHEALING;
    JOBS lastjob = JOB_JANITOR;

    JOBS psw = (is_night ? girl.m_PrevNightJob : girl.m_PrevDayJob);
    bool backtowork = false;
    if (psw == JOB_DOCTOR || psw == JOB_NURSE || psw == JOB_INTERN || psw == JOB_MECHANIC)
    {
        if (girl.has_active_trait("AIDS"))
        {
            girl.m_DayJob = girl.m_NightJob = JOB_RESTING;
            ss << girl.FullName() << " could not go back to work as a";
            if (psw == JOB_DOCTOR)    ss << " Doctor";
            if (psw == JOB_NURSE)    ss << " Nurse";
            if (psw == JOB_INTERN)    ss << "n Intern";
            ss << " because she has AIDS. Instead ";
        }
        else if (psw == JOB_DOCTOR && girl.is_slave())
        {
            girl.m_DayJob = girl.m_NightJob = JOB_RESTING;
            ss << girl.FullName() << " could not go back to work as a Doctor because she is a slave. Instead ";
        }
        else if (psw == JOB_INTERN)        // intern is a part time job
        {
            girl.m_DayJob = girl.m_PrevDayJob;
            girl.m_NightJob = girl.m_PrevNightJob;
            backtowork = true;
        }
        else                            // the others are fulltime
        {
            girl.m_DayJob = girl.m_NightJob = psw;
            backtowork = true;
        }
    }
    else if (psw != JOB_RESTING && psw >= firstjob && psw<=lastjob)
    {    // if she had a previous job that shift, put her back to work.
        if (g_Game->job_manager().FullTimeJob(psw))
        {
            girl.m_DayJob = girl.m_NightJob = psw;
        }
        else if (is_night)    // checking night job
        {
            if (girl.m_DayJob == JOB_RESTING && girl.m_PrevDayJob != JOB_RESTING && girl.m_PrevDayJob != 255)
                girl.m_DayJob = girl.m_PrevDayJob;
            girl.m_NightJob = psw;
        }
        else                    // checking day job
        {
            girl.m_DayJob = psw;
            if (girl.m_NightJob == JOB_RESTING && girl.m_PrevNightJob != JOB_RESTING && girl.m_PrevNightJob != 255)
                girl.m_NightJob = girl.m_PrevNightJob;
        }
        backtowork = true;
    }
    if (backtowork)    ss << "The Chairman puts " << girl.FullName() << " back to work.\n";
    return true;
}

bool sClinic::handle_resting_girl(sGirl& girl, bool is_night, bool has_matron, std::stringstream& ss)
{
    if (has_matron && (girl.health() < 80 || girl.tiredness() > 20))    // if she is not healthy enough to go back to work
    {
        ss << "The Chairman admits " << girl.FullName() << " to get ";
        if (girl.has_active_trait("Construct"))    { ss << "repaired";                girl.m_DayJob = girl.m_NightJob = JOB_GETREPAIRS; }
        else if (girl.has_active_trait("Half-Construct"))    { ss << "healed and repaired";    girl.m_DayJob = JOB_GETHEALING;    girl.m_NightJob = JOB_GETREPAIRS; }
        else    { ss << "healed";                girl.m_DayJob = girl.m_NightJob = JOB_GETHEALING; }
        ss << ".\n";
        return true;
    }
    else if ((girl.health() < 40 || girl.tiredness() > 60) && g_Dice.percent(girl.intelligence()))
    {
        ss << girl.FullName() << " checks herself in to get ";
        if (girl.has_active_trait("Construct"))    { ss << "repaired";                girl.m_DayJob = girl.m_NightJob = JOB_GETREPAIRS; }
        else if (girl.has_active_trait("Half-Construct"))    { ss << "healed and repaired";    girl.m_DayJob = JOB_GETHEALING;    girl.m_NightJob = JOB_GETREPAIRS; }
        else    { ss << "healed";                girl.m_DayJob = girl.m_NightJob = JOB_GETHEALING; }
        ss << ".\n";
        return true;
    }
    return false;
}

std::string sClinic::meet_no_luck() const {
    return g_Dice.select_text(
            {
                "Your father once called this 'talent spotting' - "
                "and looking these girls over you see no talent for "
                "anything.",
                "The Clinic is quite not much going on here.",
                "Married. Married. Bodyguard. Already works for you. Married. "
                "Hideous. Not a woman. Married. Escorted. Married... "
                "Might as well go home, there's nothing happening here.",
                "It's not a bad life, if you can get paid for hanging in the "
                "clinic eyeing up the pretty girls that might be brought in."
                "Not a single decent prospect in the bunch of them. ",
                "You've walked and walked and walked, and the prettiest "
                "thing you've seen all day turned out not to be female. "
                "It's time to go home...",
                "When the weather is bad, the hunting is good. The cold brings "
                "in the sick. But nothing of note today. ",
                "There's a bit of skirt over there with a lovely "
                "figure, and had a face that was pretty, ninety "
                "years ago. Over yonder, a sweet young thing but she's "
                "got daddy's gold.  Looks like nothing to gain here today. "
            }
    );
}

bool sClinic::promote_to_doctor(sGirl& current, JOBS job, bool is_night) {
    auto sw = current.get_job(is_night);
    if (current.is_dead() || sw != job || current.is_slave() || current.intelligence() < 50 ||
        current.medicine() < 50) {
        // skip dead girls and anyone who is not an intern and make sure they are qualified to be a doctor
        return false;
    }
    auto sum = EVENT_SUMMARY;

    if (!current.disobey_check(ACTION_WORKDOCTOR, JOBS(sw))) {
        current.m_DayJob = current.m_NightJob = JOB_DOCTOR;
        current.AddMessage("There was no Doctor available to work so ${name} was promoted to Doctor.", IMGTYPE_PROFILE, sum);
        return true;
    }
    return false;
}

void sClinic::GirlBeginShift(sGirl& girl, bool is_night) {
    IBuilding::GirlBeginShift(girl, is_night);

    if (girl.has_active_trait("AIDS") && (
            is_in((JOBS)girl.m_DayJob, {JOB_DOCTOR, JOB_INTERN, JOB_NURSE}) ||
            is_in((JOBS)girl.m_NightJob, {JOB_DOCTOR, JOB_INTERN, JOB_NURSE})))
    {
        girl.m_DayJob = girl.m_NightJob = JOB_RESTING;
        girl.AddMessage("Health laws prohibit anyone with AIDS from working in the Medical profession so ${name} was sent to the waiting room.",
                        IMGTYPE_PROFILE, EVENT_WARNING);
    }
}
