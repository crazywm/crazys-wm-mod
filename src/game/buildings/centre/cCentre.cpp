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
#include "IGame.h"
#include "buildings/queries.h"
#include "cJobManager.h"
#include "character/predicates.h"

extern cRng             g_Dice;

// // ----- Strut sCentre Create / destroy
sCentre::sCentre() : IBuilding(BuildingType::CENTRE, "Centre")
{
    m_FirstJob = JOB_CENTREMANAGER;
    m_LastJob = JOB_THERAPY;
    m_MatronJob = JOB_DIRECTOR;
}

sCentre::~sCentre()    = default;

// Run the shifts
void sCentre::UpdateGirls(bool is_night)
{
    //  Handle the start of shift stuff for all girls.  //
    BeginShift(is_night);

    IterateGirls(is_night, {JOB_FEEDPOOR, JOB_COMUNITYSERVICE, JOB_CLEANCENTRE, JOB_COUNSELOR}, [&](auto& current)
    {
        g_Game->job_manager().handle_simple_job(current, is_night);
    });

    IterateGirls(is_night, {JOB_REHAB, JOB_ANGER, JOB_EXTHERAPY, JOB_THERAPY}, [&](auto& current)
    {
        g_Game->job_manager().do_job(current, is_night);
    });

    EndShift(is_night);
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
        ss << "work doing community service.";
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

