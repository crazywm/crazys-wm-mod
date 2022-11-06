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
#include "events.h"
#include "cGirls.h"


extern cRng    g_Dice;

extern const char* const CarePointsBasicId = "CarePointsBasic";
extern const char* const CarePointsGoodId = "CarePointsGood";
extern const char* const DoctorInteractionId = "DoctorInteraction";

// // ----- Strut sClinic Create / destroy
sClinic::sClinic() : IBuilding(BuildingType::CLINIC, "Clinic")
{
    m_FirstJob = JOB_GETHEALING;
    m_LastJob = JOB_JANITOR;
    m_MatronJob = JOB_CHAIRMAN;
    m_MeetGirlData.Spawn = SpawnReason::CLINIC;
    m_MeetGirlData.Event = events::GIRL_MEET_CLINIC;

    declare_resource(CarePointsBasicId);
    declare_resource(CarePointsGoodId);
    declare_interaction(DoctorInteractionId);
}

sClinic::~sClinic()    = default;

// Run the shifts
void sClinic::UpdateGirls(bool is_night)    // Start_Building_Process_B
{
    // `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >> cClinic.cpp
    std::stringstream ss;
    std::string girlName;

    BeginShift(is_night);

    //  Do all the Clinic staff jobs.
    IterateGirls(is_night, {JOB_INTERN, JOB_NURSE, JOB_JANITOR, JOB_DOCTOR},
                 [is_night](auto& current) {
        g_Game->job_manager().handle_simple_job(current, is_night);
    });

    //  Do all the surgery jobs.
    IterateGirls(is_night, {JOB_GETHEALING, JOB_GETABORT, JOB_COSMETICSURGERY,
                            JOB_LIPO, JOB_BREASTREDUCTION, JOB_BOOBJOB, JOB_VAGINAREJUV, JOB_FACELIFT,
                            JOB_ASSJOB, JOB_TUBESTIED, JOB_CUREDISEASES, JOB_FERTILITY}, [is_night](auto& current) {
        g_Game->job_manager().do_job(current, is_night);
    });

    //  Finally, treat external patients. This depends on how many treatment points and free doctors we still have
    /*
    while(sGirl* doc = RequestInteraction(DoctorInteractionId)) {
        auto perf = doc->job_performance(JOB_DOCTOR, false);
        std::stringstream message;
        int earnings = 0;
        if(perf < 150) {
            // only let her work on simple surgeries
            int difficulty = g_Dice.in_range(25, 50 + perf / 2);
            // botched surgery
            if(difficulty > perf && g_Dice.percent(25)) {
                message << "${name} tried to perform a simple surgery, but she botched the job. ";
                if(TryConsumeResource(CarePointsGoodId, 4)) {
                    message << "Thankfully, your highly qualified nurses could prevent the worst.";
                } else {
                    message << "Her patient is now permanently crippled, and you had to pay them 500 gold as compensation.";
                    earnings = -500;
                }
            } else {
                earnings = 50 + perf/2 + ConsumeResource(CarePointsBasicId, 3) * 10;
                message << "${name} performed a simple surgery on an external patient, earning you " << earnings << " gold";
            }
        } else {
            // OK, high possibly high quality surgery
            if(!TryConsumeResource(CarePointsBasicId, 3)) {
                // Not enough nurses, we can only do the simple stuff
                earnings = 125 + ConsumeResource(CarePointsBasicId, 3) * 10;
                message << "${name} performed a simple surgery on an external patient, earning you " << earnings << " gold";
            } else {
                if (g_Dice.percent(25) && g_Dice.in_range(0, perf) < 100) {
                    message << "${name} performed a highly complicated surgery, which unfortunately wasn't successful.";
                } else {
                    earnings = 75 + perf / 2 + ConsumeResource(CarePointsBasicId, 4) * 12;
                    message << "${name} successfully performed a highly complicated surgery. The grateful family pays you "
                            << earnings << " gold";
                }
            }
        }
        doc->AddMessage(message.str(), EBaseImage::NURSE, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
        doc->m_Pay += earnings;
    }

    IterateGirls(is_night, {JOB_DOCTOR},
     [&](sGirl& girl) {
         bool refused = is_night ? girl.m_Refused_To_Work_Night : girl.m_Refused_To_Work_Day;

         CalculatePay(girl, girl.get_job(is_night));
         int totalPay  = girl.m_Pay;
         int totalTips = girl.m_Tips;

         // Summary Messages
         if (refused)
         {
             girl.AddMessage("${name} refused to work so she made no money.", EBaseImage::PROFILE, EVENT_SUMMARY);
         }
         else
         {
             girl.AddMessage(g_Game->job_manager().GirlPaymentText(this, girl, totalTips, totalPay, totalTips + totalPay, is_night),
                             EBaseImage::PROFILE, EVENT_SUMMARY);
         }
     });
*/
    EndShift(is_night);

    int total_doctor_actions = GetInteractionConsumed(DoctorInteractionId);
    int possible_doctor_actions = GetInteractionProvided(DoctorInteractionId);

    if(total_doctor_actions > possible_doctor_actions) {
        std::stringstream msg;
        msg << "You tried to get " << total_doctor_actions << " girls treated, but doctors could only take care of " << possible_doctor_actions << ".\n";
        AddMessage(msg.str(), EventType::EVENT_WARNING);
    } else if(possible_doctor_actions > 0) {
        std::stringstream msg;
        msg << "Your clinic has enough doctors for " << possible_doctor_actions << " patients. This week you requested treatment for " << total_doctor_actions << " girls.\n";
        AddMessage(msg.str(), EventType::EVENT_BUILDING);
    }

}

void sClinic::auto_assign_job(sGirl& target, std::stringstream& message, bool is_night)
{
    std::stringstream& ss = message;
    // if they have no job at all, assign them a job
    ss << "The Chairman assigns " << target.FullName() << " to ";
    if (target.has_active_trait(traits::AIDS))                                        // anyone with AIDS
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
        if (target.has_active_trait(traits::HERPES))          diseases.emplace_back(traits::HERPES);
        if (target.has_active_trait(traits::CHLAMYDIA))       diseases.emplace_back(traits::CHLAMYDIA);
        if (target.has_active_trait(traits::SYPHILIS))        diseases.emplace_back(traits::SYPHILIS);
        if (target.has_active_trait(traits::AIDS))            diseases.emplace_back(traits::AIDS);
        int numdiseases = diseases.size();
        ss << "get her " << (numdiseases > 1 ? "diseases" : diseases[0]) << " treated.";
    }
        // then make sure there is at least 1 Janitor and 1 Mechanic
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
    if (psw == JOB_DOCTOR || psw == JOB_NURSE || psw == JOB_INTERN)
    {
        if (girl.has_active_trait(traits::AIDS))
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
        ss << "The Chairman admits " << girl.FullName() << " to get healed.\n";
        girl.m_DayJob = girl.m_NightJob = JOB_GETHEALING;
        return true;
    }
    else if ((girl.health() < 40 || girl.tiredness() > 60) && g_Dice.percent(girl.intelligence()))
    {
        ss << girl.FullName() << " checks herself in to get healed.\n";
        girl.m_DayJob = girl.m_NightJob = JOB_GETHEALING;
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

void sClinic::GirlBeginShift(sGirl& girl, bool is_night) {
    IBuilding::GirlBeginShift(girl, is_night);

    if (girl.has_active_trait(traits::AIDS) && (
            is_in((JOBS)girl.m_DayJob, {JOB_DOCTOR, JOB_INTERN, JOB_NURSE}) ||
            is_in((JOBS)girl.m_NightJob, {JOB_DOCTOR, JOB_INTERN, JOB_NURSE})))
    {
        girl.m_DayJob = girl.m_NightJob = JOB_RESTING;
        girl.AddMessage("Health laws prohibit anyone with AIDS from working in the Medical profession so ${name} was sent to the waiting room.",
                        EImageBaseType::PROFILE, EVENT_WARNING);
    }
}
