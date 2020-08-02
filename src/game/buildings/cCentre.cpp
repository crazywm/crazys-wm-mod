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

#include "cCentre.h"
#include "cGangs.h"
#include "Game.hpp"
#include "queries.h"
#include "cJobManager.h"
#include "character/predicates.h"

extern cRng             g_Dice;

// // ----- Strut sCentre Create / destroy
sCentre::sCentre() : IBuilding(BuildingType::CENTRE, "Centre")
{
    m_RestJob = JOB_CENTREREST;
    m_FirstJob = JOB_CENTREREST;
    m_LastJob = JOB_THERAPY;
}

sCentre::~sCentre()    = default;

// Run the shifts
void sCentre::UpdateGirls(bool is_night)
{
    // `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >> cCentre.cpp
    std::stringstream ss;
    std::string girlName;

    bool counselor = false;

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
    m_Girls->apply([&](auto& current)
    {
        auto sw = current.get_job(is_night);
        if (current.is_dead() || sw != JOB_COUNSELOR)
        {    // skip dead girls and anyone who is not a counselor
           return;
        }
        ss.str("");

        if (current.disobey_check(ACTION_WORKCOUNSELOR, (JOBS)sw))
        {
           (is_night ? current.m_Refused_To_Work_Night = true : current.m_Refused_To_Work_Day = true);
           m_Fame -= current.fame();
           ss << "${name} refused to work so made no money.";
        }
        else
        {
           counselor = true;
        }
        if (ss.str().length() > 0) current.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_SUMMARY);
    });

    /////////////////////////////////////////////////////////////////////////////////
    //  Anyone not in the Therapy Cantre can be assigned to counselor if need be.  //
    /////////////////////////////////////////////////////////////////////////////////
    if(matron && Num_Patients(*this, is_night) > 0) {
        /////////////////////////////////////////////////////////////////////////////
        //  Anyone in the Therapy Cantre can be assigned to counselor if need be.  //
        //  Try them in order of who can better go without their therapy.          //
        /////////////////////////////////////////////////////////////////////////////

        counselor = FindCounselor(is_night, {JOB_FEEDPOOR, JOB_COMUNITYSERVICE, JOB_CLEANCENTRE});
        if(!counselor) {
            counselor = FindCounselor(is_night, {JOB_THERAPY});
        }
        if(!counselor) {
            counselor = FindCounselor(is_night, {JOB_ANGER, JOB_EXTHERAPY});
        }
        if(!counselor) {
            counselor = FindCounselor(is_night, {JOB_REHAB});
        }
    }

    /////////////////////////////////////
    //  Do all the Centre staff jobs.  //
    /////////////////////////////////////
    m_Girls->apply([&](auto& current)
    {
        auto sw = current.get_job(is_night);
        if (current.is_dead() || (sw != JOB_FEEDPOOR && sw != JOB_COMUNITYSERVICE && sw != JOB_CLEANCENTRE && sw != JOB_COUNSELOR) ||
            // skip dead girls and anyone who is not staff
            (sw == JOB_COUNSELOR && ((is_night == SHIFT_DAY && current.m_Refused_To_Work_Day) || (is_night == SHIFT_NIGHT && current.m_Refused_To_Work_Night))))
        {
            return;
        }

        g_Game->job_manager().handle_simple_job(current, is_night);
    });


    ////////////////////////////////////////////////////////////////////
    //  Do Rehab and therapy last if there is a counselor available.  //
    ////////////////////////////////////////////////////////////////////
    m_Girls->apply([&](auto& current){
        auto sw = current.get_job(is_night);
        if (current.is_dead() || (sw != JOB_REHAB && sw != JOB_ANGER && sw != JOB_EXTHERAPY && sw != JOB_THERAPY))
        {
            return;
        }
        g_Game->job_manager().do_job(current, is_night);
    });

    EndShift("Centre Manager", is_night, matron);
}

void sCentre::auto_assign_job(sGirl& target, std::stringstream& message, bool is_night)
{
    std::stringstream& ss = message;

    // if they have no job at all, assign them a job
    ss << "The Centre Manager assigns " << target.FullName() << " to ";
    // first send any addicts to rehab
    if (is_addict(target))
    {
        target.m_DayJob = target.m_NightJob = JOB_REHAB;
        ss << "go to Rehab.";
    }
        // Make sure there is at least 1 counselor on duty
    else if (target.is_free() && num_girls_on_job(JOB_COUNSELOR, is_night) < 1)
    {
        target.m_DayJob = target.m_NightJob = JOB_COUNSELOR;
        ss << "work as a Counselor.";
    }
        // assign 1 cleaner per 20 girls
    else if (num_girls_on_job(JOB_CLEANCENTRE, is_night) < std::max(1, num_girls() / 20))
    {
        target.m_DayJob = target.m_NightJob = JOB_CLEANCENTRE;
        ss << "clean the Centre.";
    }
        // assign 1 counselor per 20 girls
    else if (target.is_free() && num_girls_on_job(JOB_COUNSELOR, is_night) < num_girls() / 20)
    {
        target.m_DayJob = target.m_NightJob = JOB_COUNSELOR;
        ss << "work as a Counselor.";
    }
        // split all the rest between JOB_COMUNITYSERVICE and JOB_FEEDPOOR
    else if (num_girls_on_job(JOB_COMUNITYSERVICE, is_night) < num_girls_on_job(JOB_FEEDPOOR, is_night))
    {
        target.m_DayJob = target.m_NightJob = JOB_COMUNITYSERVICE;
        ss << "work doing comunity service.";
    }
    else
    {
        target.m_DayJob = target.m_NightJob = JOB_FEEDPOOR;
        ss << "work feeding the poor.";
    }
}

bool sCentre::handle_back_to_work(sGirl& girl, std::stringstream& ss, bool is_night)
{
    JOBS psw = (is_night ? girl.m_PrevNightJob : girl.m_PrevDayJob);
    if (psw == JOB_COUNSELOR && girl.is_free())
    {
        girl.m_DayJob = girl.m_NightJob = JOB_COUNSELOR;
        ss << "The Centre Manager puts " << girl.FullName() << " back to work.\n";
        return true;
    }
    else if (psw == JOB_REHAB)
    {
        girl.m_DayJob = girl.m_NightJob = psw;
        ss << "The Centre Manager puts " << girl.FullName() << " back into Rehab.\n";
        return true;
    }
    else if (psw == JOB_ANGER || psw == JOB_EXTHERAPY || psw == JOB_THERAPY)
    {
        girl.m_DayJob = girl.m_NightJob = psw;
        ss << "The Centre Manager puts " << girl.FullName() << " back into Therapy.\n";
        return true;
    }
    return false;
}

bool sCentre::FindCounselor(bool is_night, std::initializer_list<JOBS> current_job) {
    auto found = m_Girls->get_first_girl([&](auto& current){
        auto sw = current.get_job(is_night);
        // skip dead girls
        if (current.is_dead()) {
            return false;
        }

        // skip anyone with a different job
        bool right_job = std::any_of(begin(current_job), end(current_job), [sw](JOBS job){ return sw == job; });
        if(!right_job)
            return false;

        //  we really need a const disobey_check
        return !const_cast<sGirl&>(current).disobey_check(ACTION_WORKCOUNSELOR, (JOBS)sw);
    });

    if(found) {
        std::string message = "There was no Counselor available to work so {name} was assigned to do it.";
        found->m_DayJob = found->m_NightJob = JOB_COUNSELOR;
        found->AddMessage(message, IMGTYPE_PROFILE, EVENT_SUMMARY);
    }

    return found;
}

