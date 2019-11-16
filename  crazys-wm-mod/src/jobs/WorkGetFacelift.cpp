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
#pragma region //	Includes and Externs			//
#include "src/buildings/cBrothel.h"
#include "cRng.h"
#include <sstream>

#pragma endregion

// `J` Job Clinic - Surgery
bool cJobManager::WorkGetFacelift(sGirl* girl, bool Day0Night1, string& summary, cRng& rng)
{
    auto brothel = girl->m_Building;
#pragma region //	Job setup				//
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	// if she was not in surgery last turn, reset working days to 0 before proceding
	if (girl->m_YesterDayJob != JOB_FACELIFT) { girl->m_WorkingDay = girl->m_PrevWorkingDay = 0; }
	girl->m_DayJob = girl->m_NightJob = JOB_FACELIFT;	// it is a full time job

	if (girl->age() <= 21)
	{
		ss << " is too young to get a Face Lift so she was sent to the waiting room.";
		if (Day0Night1 == SHIFT_DAY)	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		girl->m_PrevDayJob = girl->m_PrevNightJob = girl->m_YesterDayJob = girl->m_YesterNightJob = girl->m_DayJob = girl->m_NightJob = JOB_CLINICREST;
		girl->m_WorkingDay = girl->m_PrevWorkingDay = 0;
		return false;	// not refusing
	}
	bool hasDoctor = brothel->num_girls_on_job(JOB_DOCTOR, Day0Night1) > 0;
	int numnurse = brothel->num_girls_on_job(JOB_NURSE, Day0Night1);
	if (!hasDoctor)
	{
		ss << " does nothing. You don't have any Doctors working. (require 1) ";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		return false;	// not refusing
	}
	ss << " is in the Clinic to get a face lift.\n \n";

	int msgtype = Day0Night1;
	cGirls::UnequipCombat(girl);	// not for patient

#pragma endregion
#pragma region //	Count the Days				//

	if (Day0Night1 == SHIFT_DAY)	// the Doctor works on her durring the day
	{
		girl->m_WorkingDay++;
	}
	else	// and if there are nurses on duty, they take care of her at night
	{
		if (brothel->num_girls_on_job(JOB_NURSE, 1) > 0)
		{
			girl->m_WorkingDay++;
			girl->health(10);
			girl->happiness(10);
			girl->mana(10);
		}
	}

#pragma endregion
#pragma region //	In Progress				//

	if (girl->m_WorkingDay < 5 || Day0Night1 == SHIFT_DAY)
	{
		int wdays = (5 - girl->m_WorkingDay);
		if (brothel->num_girls_on_job(JOB_NURSE, 1) > 0)
		{
			if (wdays >= 3)		{ wdays = 3; }
			else if (wdays > 1)	{ wdays = 2; }
			else				{ wdays = 1; }
		}
		ss << "The operation is in progress (" << wdays << " day remaining).\n";
		if (brothel->num_girls_on_job(JOB_NURSE, 1) > 1)		{ ss << "The Nurses are taking care of her at night."; }
		else if (brothel->num_girls_on_job(JOB_NURSE, 1) > 0){ ss << "The Nurse is taking care of her at night."; }
		else							{ ss << "Having a Nurse on duty will speed up her recovery."; }
	}

#pragma endregion
#pragma region //	Surgery Finished			//

	else
	{
		ss << "The surgery is a success.\nShe looks a few years younger.\n";
		msgtype = EVENT_GOODNEWS;
		girl->m_WorkingDay = girl->m_PrevWorkingDay = 0;

		if (numnurse > 2)
		{
			ss << "The Nurses kept her healthy and happy during her recovery.\n";
			girl->health(rng.bell(0, 20));
			girl->happiness(rng.bell(0, 10));
			girl->spirit(rng.bell(0, 10));
			girl->mana(rng.bell(0, 20));
			girl->beauty(rng.bell(8, 16));
			girl->charisma(rng.bell(0, 2));
			girl->age(rng.bell(-4, -1));
		}
		else if (numnurse > 0)
		{
			ss << "The Nurse" << (numnurse > 1 ? "s" : "") << " helped her during her recovery.\n";
			girl->health(rng.bell(0, 10));
			girl->happiness(rng.bell(0, 5));
			girl->spirit(rng.bell(0, 5));
			girl->mana(rng.bell(0, 10));
			girl->beauty(rng.bell(6, 12));
			girl->charisma(rng % 2);
			girl->age(rng.bell(-3, -1));
		}
		else
		{
			ss << "She is sad and has lost some health during the operation.\n";
			girl->health(rng.bell(-20, 2));
			girl->happiness(rng.bell(-10, 1));
			girl->spirit(rng.bell(-5, 1));
			girl->mana(rng.bell(-20, 3));
			girl->beauty(rng.bell(4, 10));
			girl->charisma(rng.bell(-1, 1));
			girl->age(rng.bell(-2, -1));
		}

		if (girl->m_Stats[STAT_AGE] <= 18) girl->m_Stats[STAT_AGE] = 18;
		if (girl->age() <= 21)
		{
			ss << "\n \nShe has been released from the Clinic.";
			girl->m_PrevDayJob = girl->m_PrevNightJob = girl->m_YesterDayJob = girl->m_YesterNightJob = girl->m_DayJob = girl->m_NightJob = JOB_CLINICREST;
		}
	}

#pragma endregion
#pragma region	//	Finish the shift			//

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, msgtype);

	// Improve girl
	int libido = 1;
	if (girl->has_trait( "Lesbian"))		libido += numnurse;
	if (girl->has_trait( "Masochist"))	libido += 1;
	if (girl->has_trait( "Nymphomaniac"))	libido += 2;
	girl->upd_temp_stat(STAT_LIBIDO, libido);
	if (rng % 10 == 0)
		girl->medicine(1);	// `J` she watched what the doctors and nurses were doing

#pragma endregion
	return false;
}

double cJobManager::JP_GetFacelift(sGirl* girl, bool estimate)
{
	double jobperformance = 0.0;
	if (estimate)	// for third detail string - how much do they need this?
	{
		if (girl->age() <= 21)	return -1000;			// X - not needed
		if (girl->age() == 100) return 0;				// E - unknown age?

		// this probably needs to be reworked
		jobperformance += 50 + girl->age() * 5;
		jobperformance -= girl->charisma();
		jobperformance -= girl->beauty();
	}
	return jobperformance;
}
