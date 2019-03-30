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

#include <sstream>
#include "cClinic.h"
#include "cGangs.h"
#include "strnatcmp.h"
#include "src/Game.hpp"

extern cRng    g_Dice;

// // ----- Strut sClinic Create / destroy
sClinic::sClinic() : IBuilding(BuildingType::CLINIC, "Clinic")
{
    m_RestJob = JOB_CLINICREST;
    m_FirstJob = JOB_GETHEALING;
    m_LastJob = JOB_JANITOR;
}

sClinic::~sClinic()	= default;

// Run the shifts
void sClinic::UpdateGirls(bool is_night)	// Start_Building_Process_B
{
	// `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >> cClinic.cpp
	stringstream ss;
	string summary, girlName;
	u_int sw = 0, psw = 0;

	int totalPay = 0, totalTips = 0, totalGold = 0;
	int sum = EVENT_SUMMARY;

	int numDoctors = 0, numNurses = 0;

	//////////////////////////////////////////////////////
	//  Handle the start of shift stuff for all girls.  //
	//////////////////////////////////////////////////////
    for(auto& current : girls())
    {
		if (current->is_dead())		// skip dead girls
		{
			continue;
		}
        sum = EVENT_SUMMARY; summary = ""; ss.str("");

        cGirls::UseItems(current);				// Girl uses items she has
        cGirls::CalculateGirlType(current);		// update the fetish traits
        cGirls::CalculateAskPrice(current, true);	// Calculate the girls asking price

        if (current->has_trait("AIDS") &&
            (current->m_DayJob == JOB_DOCTOR || current->m_DayJob == JOB_INTERN || current->m_DayJob == JOB_NURSE
            || current->m_NightJob == JOB_DOCTOR || current->m_NightJob == JOB_INTERN || current->m_NightJob == JOB_NURSE))
        {
            ss << "Health laws prohibit anyone with AIDS from working in the Medical profession so " << girlName << " was sent to the waiting room.";
            current->m_DayJob = current->m_NightJob = JOB_CLINICREST;
            sum = EVENT_WARNING;
        }
        if (ss.str().length() > 0) current->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, sum);

	}

	////////////////////////////////////////////////////////
	//  Process Matron first incase she refuses to work.  //
	////////////////////////////////////////////////////////
    bool matron = SetupMatron(is_night, "Chairman");
    HandleRestingGirls(is_night, matron, "Chairman");

	////////////////////////////////////////////////////////
	//  JOB_DOCTOR needs to be checked before all others  //
	////////////////////////////////////////////////////////
    for(auto& current : girls())
    {
		sw = (is_night ? current->m_NightJob : current->m_DayJob);
		if (current->is_dead() || sw != JOB_DOCTOR)
		{	// skip dead girls and anyone who is not a doctor
			continue;
		}
		totalPay = totalTips = totalGold = 0;
		sum = EVENT_SUMMARY; summary = ""; ss.str("");
		girlName = current->m_Realname;

		if (current->has_trait("AIDS"))
		{
			ss << girlName << " has AIDS! She has to get it treated before she can go back to work as a Doctor.";
			current->m_DayJob = current->m_NightJob = JOB_CUREDISEASES;
		}
		else if (current->is_slave())
		{
			ss << "Doctors can not be slaves so " << girlName << " was demoted to Nurse.";
			current->m_DayJob = current->m_NightJob = JOB_NURSE;
		}
		else if (current->intelligence() < 50 || current->medicine() < 50)
		{
			ss << girlName << " is not qualified to be a Doctor so she was sent back to being an Intern.";
			current->m_DayJob = current->m_NightJob = JOB_NURSE;
		}
		else if (current->disobey_check(ACTION_WORKDOCTOR, JOB_DOCTOR))
		{
			(is_night ? current->m_Refused_To_Work_Night = true : current->m_Refused_To_Work_Day = true);
			m_Fame -= current->fame();
			ss << girlName << " refused to work as a Doctor so made no money.";
			sum = EVENT_NOWORK;
		}
		else
		{
			numDoctors++;
		}
		if (ss.str().length() > 0) current->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, sum);
	}

	////////////////////////////////////////////////////////////////
	//  Interns and Nurses can be promoted to doctor if need be.  //
	////////////////////////////////////////////////////////////////
	if(matron)
	{
        for (auto& current : girls()) {
            if (numDoctors >= 1)
                break;

            sw = (is_night ? current->m_NightJob : current->m_DayJob);
            if (current->is_dead() || sw != JOB_INTERN || current->is_slave() || current->intelligence() < 50 ||
                current->medicine() <
                50) {    // skip dead girls and anyone who is not an intern and make sure they are qualified to be a doctor
                continue;
            }
            sum = EVENT_SUMMARY;
            summary = "";
            ss.str("");
            girlName = current->m_Realname;

            if (!current->disobey_check(ACTION_WORKDOCTOR, JOBS(sw))) {
                numDoctors++;
                ss << "There was no Doctor available to work so " << girlName << " was promoted to Doctor.";
                current->m_DayJob = current->m_NightJob = JOB_DOCTOR;
            }
            if (ss.str().length() > 0) current->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, sum);
        }

        for (auto& current : girls()) {
            if (numDoctors >= 1)
                break;

            sw  = (is_night ? current->m_NightJob : current->m_DayJob);
            if (current->is_dead() || sw != JOB_NURSE || current->is_slave() || current->intelligence() < 50 ||
                current->medicine() < 50) {    // skip dead girls and anyone who is not a nurse and make sure they
                // are qualified to be a doctor
                continue;
            }
            sum = EVENT_SUMMARY;
            summary = "";
            ss.str("");
            girlName = current->m_Realname;

            if (!current->disobey_check(ACTION_WORKDOCTOR, JOBS(sw))) {
                numDoctors++;
                ss << "There was no Doctor available to work so " << girlName << " was promoted to Doctor.";
                current->m_DayJob = current->m_NightJob = JOB_DOCTOR;
            }
            if (ss.str().length() > 0) current->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, sum);
        }
    }

	/////////////////////////////////////
	//  Do all the Clinic staff jobs.  //
	/////////////////////////////////////
    for (auto& current : girls())
    {
		sw = (is_night ? current->m_NightJob : current->m_DayJob);
		if (current->is_dead() || (sw != JOB_INTERN && sw != JOB_NURSE && sw != JOB_JANITOR && sw != JOB_MECHANIC && sw != JOB_DOCTOR) ||
			// skip dead girls and anyone who is not staff
			(sw == JOB_DOCTOR && ((is_night == SHIFT_DAY && current->m_Refused_To_Work_Day)||(is_night == SHIFT_NIGHT && current->m_Refused_To_Work_Night))))
		{	// and skip doctors who refused to work in the first check
			continue;
		}
		totalPay = totalTips = totalGold = 0;
		sum = EVENT_SUMMARY; summary = ""; ss.str("");
		girlName = current->m_Realname;

		if (current->m_NightJob == JOB_DOCTOR) summary = "SkipDisobey";
		// do their job
		bool refused = g_Game.job_manager().JobFunc[sw](current, is_night, summary);

		totalPay += current->m_Pay;
		totalTips += current->m_Tips;
		totalGold += current->m_Pay + current->m_Tips;
		CalculatePay(*current, sw);

		//		Summary Messages
		if (refused)
		{
			m_Fame -= current->fame();
			ss << girlName << " refused to work so made no money.";
		}
		else
		{
			ss << g_Game.job_manager().GirlPaymentText(this, current, totalTips, totalPay, totalGold, is_night);
			if (totalGold < 0) sum = EVENT_DEBUG;

			m_Fame += current->fame();
		}
		if (ss.str().length() > 0) current->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, sum);
	}


	///////////////////////////////////////////////////////////////////////
	//  Do all the surgery jobs. Not having a doctor is in all of them.  //
	///////////////////////////////////////////////////////////////////////
    for (auto& current : girls())
	{
		sw = (is_night ? current->m_NightJob : current->m_DayJob);
		if (current->is_dead() || (sw != JOB_GETHEALING && sw != JOB_GETREPAIRS && sw != JOB_GETABORT
			&& sw != JOB_COSMETICSURGERY && sw != JOB_LIPO && sw != JOB_BREASTREDUCTION && sw != JOB_BOOBJOB
			&& sw != JOB_VAGINAREJUV && sw != JOB_FACELIFT && sw != JOB_ASSJOB && sw != JOB_TUBESTIED
			&& sw != JOB_CUREDISEASES && sw != JOB_FERTILITY))
		{	// skip dead girls and anyone not a patient
			continue;
		}
		summary = "";

		// do their surgery
		g_Game.job_manager().JobFunc[sw](current, is_night, summary);
	}

    EndShift("Chairman", is_night, matron);
}

void sClinic::auto_assign_job(sGirl* target, std::stringstream& message, bool is_night)
{
    std::stringstream& ss = message;
    // if they have no job at all, assign them a job
    ss << "The Chairman assigns " << target->m_Realname << " to ";
    if (target->has_trait("AIDS"))										// anyone with AIDS
    {
        target->m_DayJob = target->m_NightJob = JOB_CUREDISEASES;		//  needs to get it treated right away
        ss << "get her AIDS treated right away.";
    }
    else if ((target->is_free() &&										// assign any free girl
             (target->intelligence() > 70 && target->medicine() > 70))	// who is well qualified
             || (num_girls_on_job(JOB_DOCTOR, is_night) < 1 &&			// or if there are no doctors yet
                 target->intelligence() >= 50 && target->medicine() >= 50))	// assign anyone who qualifies
    {
        target->m_DayJob = target->m_NightJob = JOB_DOCTOR;			// as a Doctor
        ss << "work as a Doctor.";
    }
    else if (num_girls_on_job(JOB_NURSE, is_night) < 1)			// make sure there is at least 1 Nurse
    {
        target->m_DayJob = target->m_NightJob = JOB_NURSE;
        ss << "work as a Nurse.";
    }
    else if (target->has_disease())									// treat anyone with a disease
    {
        target->m_DayJob = target->m_NightJob = JOB_CUREDISEASES;
        vector<const char*> diseases;
        if (target->has_trait("Herpes"))		    diseases.emplace_back("Herpes");
        if (target->has_trait("Chlamydia"))	    diseases.emplace_back("Chlamydia");
        if (target->has_trait("Syphilis"))		diseases.emplace_back("Syphilis");
        if (target->has_trait("AIDS"))			diseases.emplace_back("AIDS");
        int numdiseases = diseases.size();
        ss << "get her " << (numdiseases > 1 ? "diseases" : diseases[0]) << " treated.";
    }
        // then make sure there is at least 1 Janitor and 1 Mechanic
    else if (num_girls_on_job(JOB_MECHANIC, is_night) < 1)
    {
        target->m_DayJob = target->m_NightJob = JOB_MECHANIC;
        ss << "work as a Mechanic.";
    }
    else if (num_girls_on_job(JOB_JANITOR, is_night) < 1)
    {
        target->m_DayJob = target->m_NightJob = JOB_JANITOR;
        ss << "work as a Janitor.";
    }
        // then add more of each job as numbers permit
    else if (target->medicine() > 30 && num_girls_on_job(JOB_NURSE, is_night) < num_girls() / 10)
    {
        target->m_DayJob = target->m_NightJob = JOB_NURSE;
        ss << "work as a Nurse.";
    }
    else if (num_girls_on_job(JOB_MECHANIC, is_night) < num_girls() / 20)
    {
        target->m_DayJob = target->m_NightJob = JOB_MECHANIC;
        ss << "work as a Mechanic.";
    }
    else if (num_girls_on_job(JOB_JANITOR, is_night) < num_girls() / 20)
    {
        target->m_DayJob = target->m_NightJob = JOB_JANITOR;
        ss << "work as a Janitor.";
    }
    else	// assign anyone else to Internship
    {
        target->m_DayJob = target->m_NightJob = JOB_INTERN;
        ss << "work as an Intern.";
    }
}

bool sClinic::handle_back_to_work(sGirl& girl, std::stringstream& ss, bool is_night)
{
    u_int firstjob = JOB_GETHEALING;
    u_int lastjob = JOB_JANITOR;

    unsigned psw = (is_night ? girl.m_PrevNightJob : girl.m_PrevDayJob);
    bool backtowork = false;
    if (psw == JOB_DOCTOR || psw == JOB_NURSE || psw == JOB_INTERN || psw == JOB_MECHANIC)
    {
        if (girl.has_trait("AIDS"))
        {
            girl.m_DayJob = girl.m_NightJob = m_RestJob;
            ss << girl.m_Realname << " could not go back to work as a";
            if (psw == JOB_DOCTOR)	ss << " Doctor";
            if (psw == JOB_NURSE)	ss << " Nurse";
            if (psw == JOB_INTERN)	ss << "n Intern";
            ss << " because she has AIDS. Instead ";
        }
        else if (psw == JOB_DOCTOR && girl.is_slave())
        {
            girl.m_DayJob = girl.m_NightJob = m_RestJob;
            ss << girl.m_Realname << " could not go back to work as a Doctor because she is a slave. Instead ";
        }
        else if (psw == JOB_INTERN)		// intern is a part time job
        {
            girl.m_DayJob = girl.m_PrevDayJob;
            girl.m_NightJob = girl.m_PrevNightJob;
            backtowork = true;
        }
        else							// the others are fulltime
        {
            girl.m_DayJob = girl.m_NightJob = psw;
            backtowork = true;
        }
    }
    else if (psw != m_RestJob && psw >= firstjob && psw<=lastjob)
    {	// if she had a previous job that shift, put her back to work.
        if (g_Game.job_manager().FullTimeJob(psw))
        {
            girl.m_DayJob = girl.m_NightJob = psw;
        }
        else if (is_night)	// checking night job
        {
            if (girl.m_DayJob == m_RestJob && girl.m_PrevDayJob != m_RestJob && girl.m_PrevDayJob != 255)
                girl.m_DayJob = girl.m_PrevDayJob;
            girl.m_NightJob = psw;
        }
        else					// checking day job
        {
            girl.m_DayJob = psw;
            if (girl.m_NightJob == m_RestJob && girl.m_PrevNightJob != m_RestJob && girl.m_PrevNightJob != 255)
                girl.m_NightJob = girl.m_PrevNightJob;
        }
        backtowork = true;
    }
    if (backtowork)	ss << "The Chairman puts " << girl.m_Realname << " back to work.\n";
    return true;
}

bool sClinic::handle_resting_girl(sGirl& girl, bool is_night, bool has_matron, std::stringstream& ss)
{
    if (has_matron && (girl.health() < 80 || girl.tiredness() > 20))	// if she is not healthy enough to go back to work
    {
        ss << "The Chairman admits " << girl.m_Realname << " to get ";
        if (girl.has_trait("Construct"))	{ ss << "repaired";				girl.m_DayJob = girl.m_NightJob = JOB_GETREPAIRS; }
        else if (girl.has_trait("Half-Construct"))	{ ss << "healed and repaired";	girl.m_DayJob = JOB_GETHEALING;	girl.m_NightJob = JOB_GETREPAIRS; }
        else	{ ss << "healed";				girl.m_DayJob = girl.m_NightJob = JOB_GETHEALING; }
        ss << ".\n";
        return true;
    }
    else if ((girl.health() < 40 || girl.tiredness() > 60) && g_Dice.percent(girl.intelligence()))
    {
        ss << girl.m_Realname << " checks herself in to get ";
        if (girl.has_trait("Construct"))	{ ss << "repaired";				girl.m_DayJob = girl.m_NightJob = JOB_GETREPAIRS; }
        else if (girl.has_trait("Half-Construct"))	{ ss << "healed and repaired";	girl.m_DayJob = JOB_GETHEALING;	girl.m_NightJob = JOB_GETREPAIRS; }
        else	{ ss << "healed";				girl.m_DayJob = girl.m_NightJob = JOB_GETHEALING; }
        ss << ".\n";
        return true;
    }
    return false;
}
