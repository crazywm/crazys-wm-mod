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

#include "cArena.h"
#include "cGangs.h"
#include "IBuilding.hpp"
#include "Game.hpp"
#include "sStorage.hpp"

extern cRng             g_Dice;

// // ----- Strut sArena Create / destroy
sArena::sArena() : IBuilding(BuildingType::ARENA, "Arena")
{
    m_RestJob = JOB_ARENAREST;
    m_FirstJob = JOB_FIGHTBEASTS;
    m_LastJob = JOB_CLEANARENA;
}

sArena::~sArena() = default;

// Run the shifts
void sArena::UpdateGirls(bool is_night)	// Start_Building_Process_B
{
	// `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >> cArena.cpp
	stringstream ss;
	string summary, girlName;
	u_int sw = 0, psw = 0;

	int totalPay = 0, totalTips = 0, totalGold = 0;
	int sum = EVENT_SUMMARY;

	//////////////////////////////////////////////////////
	//  Handle the start of shift stuff for all girls.  //
	//////////////////////////////////////////////////////
	BeginShift();
    bool matron = SetupMatron(is_night, "Doctore");
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//  Now If there is a matron and she is not refusing to work, then she can delegate the girls in this building.  //
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    HandleRestingGirls(is_night, matron, "Doctore");

	////////////////////////////////////////
	//  Run any races the girls can run.  //
	////////////////////////////////////////
//	current = brothel->m_Girls;
//	while (current)
//	{
//		sw = (Day0Night1 ? current->m_NightJob : current->m_DayJob);
//		if (current->is_dead() || sw != JOB_RACING)
//		{	// skip dead girls, and anyone not racing
//			if (current->m_Next) { current = current->m_Next; continue; }
//			else { current = 0; break; }
//		}
//		ss.str("");
//		// do their job
//		refused = m_JobManager.JobFunc[sw](current, brothel, Day0Night1, summary);
//
//		totalPay += current->m_Pay;
//		totalTips += current->m_Tips;
//		totalGold += current->m_Pay + current->m_Tips;
//		g_Brothels.CalculatePay(brothel, current, sw);
//
//		//		Summary Messages
//		if (refused)
//		{
//			brothel->m_Fame -= current->fame();
//			ss << girlName << " refused to work so made no money.";
//		}
//		else
//		{
//			ss << m_JobManager.GirlPaymentText(brothel, current, totalTips, totalPay, totalGold, Day0Night1);
//			if (totalGold < 0) sum = EVENT_DEBUG;
//
//			brothel->m_Fame += current->fame();
//		}
//		if (ss.str().length() > 0) current->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, sum);
//
//		current = current->m_Next; // Next Girl
//	}



	/////////////////////////////////////////////////////////////////
	//  All other Jobs in the Arena can be done at the same time.  //
	/////////////////////////////////////////////////////////////////
	/* `J` zzzzzz - Need to split up the jobs
	Done - JOB_ARENAREST, JOB_DOCTORE

	JOB_CLEANARENA

	JOB_FIGHTBEASTS
	JOB_FIGHTARENAGIRLS
	JOB_FIGHTTRAIN
	JOB_CITYGUARD

	//*/
	for(auto& current : girls())
	{
		sw = (is_night ? current->m_NightJob : current->m_DayJob);
		if (current->is_dead() || sw == m_RestJob || sw == m_MatronJob)// || sw == JOB_RACING)
		{	// skip dead girls, resting girls and the matron and racers
			continue;
		}
		totalPay = totalTips = totalGold = 0;
		sum = EVENT_SUMMARY; summary = ""; ss.str("");
		girlName = current->m_Realname;

		// fight beasts so if there is no beasts dont want them doing nothing
		if (sw == JOB_FIGHTBEASTS && g_Game->storage().beasts() < 1)
		{
			stringstream ssc;
			ssc << "There are no beasts to fight so " << girlName << " was sent to ";

			if (current->health() < 50)
			{
				ssc << "rest and heal";
				sw = m_RestJob;
			}
			else if (current->combat() > 90 && current->magic() > 90 && current->agility() > 90 && current->constitution() > 90 && current->health() > 90)
			{
				ssc << "fight other girls";
				sw = JOB_FIGHTARENAGIRLS;
			}
			else
			{
				ssc << "train for combat";
				sw = JOB_FIGHTTRAIN;
			}
			ssc << " instead.\n"<<"\n";
			current->m_Events.AddMessage(ssc.str(), IMGTYPE_PROFILE, is_night);

		}

		// do their job
		bool refused = g_Game->job_manager().JobFunc[sw](current, is_night, summary, g_Dice);

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
			ss << g_Game->job_manager().GirlPaymentText(this, current, totalTips, totalPay, totalGold, is_night);
			if (totalGold < 0) sum = EVENT_DEBUG;

			m_Fame += current->fame();
		}
		if (ss.str().length() > 0) current->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, sum);
	}

	EndShift("Doctore", is_night, matron);
}

void sArena::auto_assign_job(sGirl* target, std::stringstream& message, bool is_night)
{
    // shortcut
    std::stringstream& ss = message;

    ss << "The Doctore assigns " << target->m_Realname << " to ";

    // need at least 1 guard and 1 cleaner (because guards must be free, they get assigned first)
    if (target->is_free() && num_girls_on_job(JOB_CITYGUARD, is_night) < 1)
    {
        target->m_DayJob = target->m_NightJob = JOB_CITYGUARD;
        ss << "work helping the city guard.";
    }
    else if (num_girls_on_job(JOB_CLEANARENA, is_night) < 1)
    {
        target->m_DayJob = target->m_NightJob = JOB_CLEANARENA;
        ss << "work cleaning the arena.";
    }
    else if (num_girls_on_job(JOB_BLACKSMITH, is_night) < 1)
    {
        target->m_DayJob = target->m_NightJob = JOB_BLACKSMITH;
        ss << "work making weapons and armor.";
    }
    else if (num_girls_on_job(JOB_COBBLER, is_night) < 1)
    {
        target->m_DayJob = target->m_NightJob = JOB_COBBLER;
        ss << "work making shoes and leather items.";
    }
    else if (num_girls_on_job(JOB_JEWELER, is_night) < 1)
    {
        target->m_DayJob = target->m_NightJob = JOB_JEWELER;
        ss << "work making jewelery.";
    }

        // next assign more guards and cleaners if there are a lot of girls to choose from
    else if (target->is_free() && num_girls_on_job(JOB_CITYGUARD, is_night) < num_girls() / 20)
    {
        target->m_DayJob = target->m_NightJob = JOB_CITYGUARD;
        ss << "work helping the city guard.";
    }
    else if (num_girls_on_job(JOB_CLEANARENA, is_night) < num_girls() / 20)
    {
        target->m_DayJob = target->m_NightJob = JOB_CLEANARENA;
        ss << "work cleaning the arena.";
    }
    else if (num_girls_on_job(JOB_BLACKSMITH, is_night) < num_girls() / 20)
    {
        target->m_DayJob = target->m_NightJob = JOB_BLACKSMITH;
        ss << "work making weapons and armor.";
    }
    else if (num_girls_on_job(JOB_COBBLER, is_night) < num_girls() / 20)
    {
        target->m_DayJob = target->m_NightJob = JOB_COBBLER;
        ss << "work making shoes and leather items.";
    }
    else if (num_girls_on_job(JOB_JEWELER, is_night) < num_girls() / 20)
    {
        target->m_DayJob = target->m_NightJob = JOB_JEWELER;
        ss << "work making jewelery.";
    }

        // Assign fighters - 50+ combat

        /*	Only fight beasts if there are 10 or more available
        *		and 1 girl per 10 beasts so they don't get depleted too fast.
        *	You can manually assign more if you want but I prefer to save beasts for the brothel
        *		until each building has their own beast supply.
        *	The farm will supply them when more work gets done to it
        */
    else if (target->combat() > 60 && g_Game->storage().beasts() >= 10 &&
            num_girls_on_job(JOB_FIGHTBEASTS, is_night) < g_Game->storage().beasts() / 10)
    {
        target->m_DayJob = target->m_NightJob = JOB_FIGHTBEASTS;
        ss << "work fighting beast in the arena.";
    }
        // if there are not enough beasts, have the girls fight other girls
    else if (target->combat() > 60 && num_girls_on_job(JOB_FIGHTARENAGIRLS, is_night) < 1)
    {
        target->m_DayJob = target->m_NightJob = JOB_FIGHTARENAGIRLS;
        ss << "work fighting other girls in the arena.";
    }

    else	// assign anyone else to Training
    {
        target->m_DayJob = target->m_NightJob = JOB_FIGHTTRAIN;
        ss << "train for the arena.";
    }
}
