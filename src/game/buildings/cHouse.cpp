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

#include "cHouse.h"
#include "cGangs.h"
#include "Game.hpp"
#include "cJobManager.h"
#include "character/predicates.h"
#include "cGirls.h"
extern cRng             g_Dice;

// // ----- Strut sHouse Create / destroy
sHouse::sHouse() : IBuilding(BuildingType::HOUSE, "House")
{
    m_FirstJob = JOB_HEADGIRL;
    m_LastJob = JOB_HOUSEPET;
    m_MatronJob = JOB_HEADGIRL;
}

sHouse::~sHouse() = default;

// Run the shifts
void sHouse::UpdateGirls(bool is_night)    // Start_Building_Process_B
{
    // `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >> cHouse.cpp
    std::stringstream ss;

    BeginShift(is_night);

    //  Do all Personal Bed Warmers together.  //
    m_Girls->apply([&](auto& current) {
        auto sw = current.get_job(is_night);
        if (current.is_dead() || sw != JOB_PERSONALBEDWARMER)
        {
            return;
        }
        g_Game->job_manager().handle_simple_job(current, is_night);
    });

    /////////////////////////////////////////////////////////////////////
    //  All the orher Jobs in the House can be done at the same time.  //
    /////////////////////////////////////////////////////////////////////
    /* `J` zzzzzz - Need to split up the jobs
    Done - JOB_HOUSEREST, JOB_HEADGIRL, JOB_PERSONALBEDWARMER

    JOB_CLEANHOUSE
    JOB_RECRUITER
    JOB_PERSONALTRAINING

    //*/
    m_Girls->apply([&](auto& current) {
        auto sw = current.get_job(is_night);;
        if (current.is_dead() || sw == JOB_RESTING || sw == m_MatronJob ||    // skip dead girls, resting girls and the matron
            sw == JOB_PERSONALBEDWARMER ||
            (is_night && sw == JOB_RECRUITER))                            // skip recruiters on the night shift
        {
            return;
        }

        g_Game->job_manager().handle_simple_job(current, is_night);
    });

    EndShift(is_night);
}

void sHouse::auto_assign_job(sGirl& target, std::stringstream& message, bool is_night)
{
    std::stringstream& ss = message;
    // if they have no job at all, assign them a job
    ss << "The Head Girl assigns " << target.FullName() << " to ";

    // Set any free girls who would do well at recruiting to recruit for you
    if (target.is_free() && target.job_performance(JOB_RECRUITER) >= 185)
    {
        target.m_DayJob = target.m_NightJob = JOB_RECRUITER;
        ss << "work recruiting girls for you.";
    }
        // assign a slave to clean
    else if (target.is_slave() && num_girls_on_job(JOB_CLEANHOUSE, is_night) < std::max(1, num_girls() / 20))
    {
        target.m_DayJob = target.m_NightJob = JOB_CLEANHOUSE;
        ss << "work cleaning the house.";
    }
        // and a free girl to recruit for you
    else if (target.is_free() && num_girls_on_job(JOB_RECRUITER, is_night) < 1)
    {
        target.m_DayJob = target.m_NightJob = JOB_RECRUITER;
        ss << "work recruiting girls for you.";
    }
        // set at least 1 bed warmer
    else if (num_girls_on_job(JOB_PERSONALBEDWARMER, is_night) < 1
             && !is_virgin(target))    // Crazy added this so that it wont set virgins to this
    {
        target.m_DayJob = target.m_NightJob = JOB_PERSONALBEDWARMER;
        ss << "work warming your bed.";
    }
        // assign 1 cleaner per 20 girls
    else if (num_girls_on_job(JOB_CLEANHOUSE, is_night) < std::max(1, num_girls() / 20))
    {
        target.m_DayJob = target.m_NightJob = JOB_CLEANHOUSE;
        ss << "work cleaning the house.";
    }
        // Put every one else who is not a virgin and needs some training
    else if (!is_virgin(target) && cGirls::GetAverageOfSexSkills(target) < 99)
    {
        target.m_DayJob = target.m_NightJob = JOB_PERSONALTRAINING;
        ss << "get personal training from you.";
    }
        // Set any other free girls to recruit for you
    else if (target.is_free())
    {
        target.m_DayJob = target.m_NightJob = JOB_RECRUITER;
        ss << "work recruiting girls for you.";
    }
    else
    {
        ss << "do nothing because this part of the code has not been finished yet.";
    }
}
