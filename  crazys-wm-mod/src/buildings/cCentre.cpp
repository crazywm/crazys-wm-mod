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

#include "cCentre.h"
#include "cGangs.h"
#include "strnatcmp.h"
#include "Game.hpp"
#include "queries.hpp"

extern cRng             g_Dice;

// // ----- Strut sCentre Create / destroy
sCentre::sCentre() : IBuilding(BuildingType::CENTRE, "Centre")
{
    m_RestJob = JOB_CENTREREST;
    m_FirstJob = JOB_CENTREREST;
    m_LastJob = JOB_THERAPY;
}

sCentre::~sCentre()	= default;

// Run the shifts
void sCentre::UpdateGirls(bool is_night)
{
	// `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >> cCentre.cpp
	stringstream ss;
	string summary, girlName;
	u_int sw = 0, psw = 0;

	int totalPay = 0, totalTips = 0, totalGold = 0;
	int sum = EVENT_SUMMARY;

	bool counselor = false;

	bool refused = false;

	//////////////////////////////////////////////////////
	//  Handle the start of shift stuff for all girls.  //
	//////////////////////////////////////////////////////
    BeginShift();
    bool matron = SetupMatron(is_night, "Centre Manager");

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//  Now If there is a matron and she is not refusing to work, then she can delegate the girls in this building.  //
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    HandleRestingGirls(is_night, matron, "Centre Manager");

	//////////////////////////////////////////////////////////
	//  JOB_COUNSELOR needs to be checked before all others //
	//////////////////////////////////////////////////////////
    for(auto& current : girls())
    {
		sw = (is_night ? current->m_NightJob : current->m_DayJob);
		if (current->is_dead() || sw != JOB_COUNSELOR)
		{	// skip dead girls and anyone who is not a counselor
			continue;
		}
		totalPay = totalTips = totalGold = 0;
		sum = EVENT_SUMMARY; summary = ""; ss.str("");
		girlName = current->m_Realname;
		
		if (current->disobey_check(ACTION_WORKCOUNSELOR, (JOBS)sw))
		{
			(is_night ? current->m_Refused_To_Work_Night = true : current->m_Refused_To_Work_Day = true);
			m_Fame -= current->fame();
			ss << girlName << " refused to work so made no money.";
		}
		else
		{
			counselor = true;
		}
		if (ss.str().length() > 0) current->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, sum);
	}

	/////////////////////////////////////////////////////////////////////////////////
	//  Anyone not in the Therapy Cantre can be assigned to counselor if need be.  //
	/////////////////////////////////////////////////////////////////////////////////
	if(matron) {
        for(auto& current : girls())
        {
            if(!(!counselor && Num_Patients(*this, is_night) > 0))
                break;
            sw  = (is_night ? current->m_NightJob : current->m_DayJob);
            if (current->is_dead() || (sw != JOB_FEEDPOOR && sw != JOB_COMUNITYSERVICE && sw != JOB_CLEANCENTRE)) {
                // skip dead girls
               continue;
            }
            sum = EVENT_SUMMARY;
            summary = "";
            ss.str("");
            girlName = current->m_Realname;

            if (!current->disobey_check(ACTION_WORKCOUNSELOR, (JOBS)sw)) {
                counselor = true;
                ss << "There was no Counselor available to work so " << girlName << " was assigned to do it.";
                current->m_DayJob = current->m_NightJob = JOB_COUNSELOR;
            }
            if (ss.str().length() > 0) current->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, sum);
        }


        /////////////////////////////////////////////////////////////////////////////
        //  Anyone in the Therapy Cantre can be assigned to counselor if need be.  //
        //  Try them in order of who can better go without their therapy.          //
        /////////////////////////////////////////////////////////////////////////////
        for(auto& current : girls())
        {
            if(!(!counselor && Num_Patients(*this, is_night) > 0))
                break;

            sw = (is_night ? current->m_NightJob : current->m_DayJob);
            if (current->is_dead() || sw != JOB_THERAPY)
            {
                continue;
            }
            sum = EVENT_SUMMARY; summary = ""; ss.str("");
            girlName = current->m_Realname;

            if (!current->disobey_check(ACTION_WORKCOUNSELOR, (JOBS)sw))
            {
                counselor = true;
                ss << "There was no Counselor available to work so " << girlName << " was assigned to do it.";
                current->m_DayJob = current->m_NightJob = JOB_COUNSELOR;
            }
            if (ss.str().length() > 0) current->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, sum);
        }


        for(auto& current : girls())
        {
            if(!(!counselor && Num_Patients(*this, is_night) > 0))
                break;

            sw = (is_night ? current->m_NightJob : current->m_DayJob);
            if (current->is_dead() || (sw != JOB_ANGER && sw != JOB_EXTHERAPY))
            {
                continue;
            }
            sum = EVENT_SUMMARY; summary = ""; ss.str("");
            girlName = current->m_Realname;

            if (!current->disobey_check(ACTION_WORKCOUNSELOR, (JOBS)sw))
            {
                counselor = true;
                ss << "There was no Counselor available to work so " << girlName << " was assigned to do it.";
                current->m_DayJob = current->m_NightJob = JOB_COUNSELOR;
            }
            if (ss.str().length() > 0) current->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, sum);
        }

        for(auto& current : girls())
        {
            if(!(!counselor && Num_Patients(*this, is_night) > 0))
                break;
            sw = (is_night ? current->m_NightJob : current->m_DayJob);
            if (current->is_dead() || sw != JOB_REHAB)
            {
                continue;
            }
            sum = EVENT_SUMMARY; summary = ""; ss.str("");
            girlName = current->m_Realname;

            if (!current->disobey_check(ACTION_WORKCOUNSELOR, (JOBS)sw))
            {
                counselor = true;
                ss << "There was no Counselor available to work so " << girlName << " was assigned to do it.";
                current->m_DayJob = current->m_NightJob = JOB_COUNSELOR;
            }
            if (ss.str().length() > 0) current->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, sum);
        }
    }

	/////////////////////////////////////
	//  Do all the Centre staff jobs.  //
	/////////////////////////////////////
    for(auto& current : girls())
    {
		sw = (is_night ? current->m_NightJob : current->m_DayJob);
		if (current->is_dead() || (sw != JOB_FEEDPOOR && sw != JOB_COMUNITYSERVICE && sw != JOB_CLEANCENTRE && sw != JOB_COUNSELOR) ||
			// skip dead girls and anyone who is not staff
			(sw == JOB_COUNSELOR && ((is_night == SHIFT_DAY && current->m_Refused_To_Work_Day) || (is_night == SHIFT_NIGHT && current->m_Refused_To_Work_Night))))
		{
		    continue;
		}
		totalPay = totalTips = totalGold = 0;
		sum = EVENT_SUMMARY; summary = ""; ss.str("");
		girlName = current->m_Realname;

		if (current->m_NightJob == JOB_COUNSELOR) summary = "SkipDisobey";
		// do their job
		refused = g_Game.job_manager().JobFunc[sw](current, is_night, summary);

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


	////////////////////////////////////////////////////////////////////
	//  Do Rehab and therapy last if there is a counselor available.  //
	////////////////////////////////////////////////////////////////////
    for(auto& current : girls())
    {
		sw = (is_night ? current->m_NightJob : current->m_DayJob);
		if (current->is_dead() || (sw != JOB_REHAB && sw != JOB_ANGER && sw != JOB_EXTHERAPY && sw != JOB_THERAPY))
		{
		    continue;
		}
		summary = "";
		g_Game.job_manager().JobFunc[sw](current, is_night, summary);
	}

    EndShift("Centre Manager", is_night, matron);
}

void sCentre::auto_assign_job(sGirl* target, std::stringstream& message, bool is_night)
{
    std::stringstream& ss = message;

    // if they have no job at all, assign them a job
    ss << "The Centre Manager assigns " << target->m_Realname << " to ";
    // first send any addicts to rehab
    if (target->is_addict())
    {
        target->m_DayJob = target->m_NightJob = JOB_REHAB;
        ss << "go to Rehab.";
    }
        // Make sure there is at least 1 counselor on duty
    else if (target->is_free() && num_girls_on_job(JOB_COUNSELOR, is_night) < 1)
    {
        target->m_DayJob = target->m_NightJob = JOB_COUNSELOR;
        ss << "work as a Counselor.";
    }
        // assign 1 cleaner per 20 girls
    else if (num_girls_on_job(JOB_CLEANCENTRE, is_night) < max(1, num_girls() / 20))
    {
        target->m_DayJob = target->m_NightJob = JOB_CLEANCENTRE;
        ss << "clean the Centre.";
    }
        // assign 1 counselor per 20 girls
    else if (target->is_free() && num_girls_on_job(JOB_COUNSELOR, is_night) < num_girls() / 20)
    {
        target->m_DayJob = target->m_NightJob = JOB_COUNSELOR;
        ss << "work as a Counselor.";
    }
        // split all the rest between JOB_COMUNITYSERVICE and JOB_FEEDPOOR
    else if (num_girls_on_job(JOB_COMUNITYSERVICE, is_night) < num_girls_on_job(JOB_FEEDPOOR, is_night))
    {
        target->m_DayJob = target->m_NightJob = JOB_COMUNITYSERVICE;
        ss << "work doing comunity service.";
    }
    else
    {
        target->m_DayJob = target->m_NightJob = JOB_FEEDPOOR;
        ss << "work feeding the poor.";
    }
}

bool sCentre::handle_back_to_work(sGirl& girl, std::stringstream& ss, bool is_night)
{
    int psw = (is_night ? girl.m_PrevNightJob : girl.m_PrevDayJob);
    if (psw == JOB_COUNSELOR && girl.is_free())
    {
        girl.m_DayJob = girl.m_NightJob = JOB_COUNSELOR;
        ss << "The Centre Manager puts " << girl.m_Realname << " back to work.\n";
        return true;
    }
    else if (psw == JOB_REHAB)
    {
        girl.m_DayJob = girl.m_NightJob = psw;
        ss << "The Centre Manager puts " << girl.m_Realname << " back into Rehab.\n";
        return true;
    }
    else if (psw == JOB_ANGER || psw == JOB_EXTHERAPY || psw == JOB_THERAPY)
    {
        girl.m_DayJob = girl.m_NightJob = psw;
        ss << "The Centre Manager puts " << girl.m_Realname << " back into Therapy.\n";
        return true;
    }
    return false;
}

