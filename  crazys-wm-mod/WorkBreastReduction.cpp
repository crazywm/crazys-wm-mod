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
#include "cJobManager.h"
#include "cBrothel.h"
#include "cClinic.h"
#include "cCustomers.h"
#include "cRng.h"
#include "cInventory.h"
#include "sConfig.h"
#include "cRival.h"
#include <sstream>
#include "CLog.h"
#include "cTrainable.h"
#include "cTariff.h"
#include "cGold.h"
#include "cGangs.h"
#include "cMessageBox.h"
#include "libintl.h"

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cClinicManager g_Clinic;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;

#pragma endregion

// `J` Job Clinic - Surgery
bool cJobManager::WorkBreastReduction(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
#pragma region //	Job setup				//
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	// if she was not in surgery last turn, reset working days to 0 before proceding
	if (girl->m_YesterDayJob != JOB_BREASTREDUCTION) { girl->m_WorkingDay = girl->m_PrevWorkingDay = 0; }
	girl->m_DayJob = girl->m_NightJob = JOB_BREASTREDUCTION;	// it is a full time job

	if (girl->has_trait( "Flat Chest"))
	{
		ss << " already has a Flat Chest so she was sent to the waiting room.";
		if (Day0Night1 == SHIFT_DAY)	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		girl->m_PrevDayJob = girl->m_PrevNightJob = girl->m_YesterDayJob = girl->m_YesterNightJob = girl->m_DayJob = girl->m_NightJob = JOB_CLINICREST;
		girl->m_WorkingDay = girl->m_PrevWorkingDay = 0;
		return false;	// not refusing
	}
	bool hasDoctor = g_Clinic.GetNumGirlsOnJob(0, JOB_DOCTOR, Day0Night1) > 0;
	int numnurse = g_Clinic.GetNumGirlsOnJob(0, JOB_NURSE, Day0Night1);
	if (!hasDoctor)
	{
		ss << " does nothing. You don't have any Doctors working. (require 1) ";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		return false;	// not refusing
	}
	ss << " is in the Clinic to get her breasts reduced.\n \n";

	int msgtype = Day0Night1;
	g_Girls.UnequipCombat(girl);	// not for patient

#pragma endregion
#pragma region //	Count the Days				//

	if (Day0Night1 == SHIFT_DAY)	// the Doctor works on her durring the day
	{
		girl->m_WorkingDay++;
	}
	else	// and if there are nurses on duty, they take care of her at night
	{
		girl->m_WorkingDay = girl->m_PrevWorkingDay = 0;
		ss << "The surgery is a success.\n";
		msgtype = EVENT_GOODNEWS;

		ss << g_Girls.AdjustTraitGroupBreastSize(girl, -1, false) << "\n \n";

		if (numnurse > 2)
		{
			ss << "The Nurses kept her healthy and happy during her recovery.\n";
			girl->health(g_Dice.bell(0, 20));
			girl->happiness(g_Dice.bell(0, 10));
			girl->spirit(g_Dice.bell(0, 10));
			girl->mana(g_Dice.bell(0, 20));
			girl->beauty(g_Dice.bell(0, 2));
			girl->charisma(g_Dice.bell(0, 2));
		}
		else if (numnurse > 0)
		{
			ss << "The Nurse" << (numnurse > 1 ? "s" : "") << " helped her during her recovery.\n";
			girl->health(g_Dice.bell(0, 10));
			girl->happiness(g_Dice.bell(0, 5));
			girl->spirit(g_Dice.bell(0, 5));
			girl->mana(g_Dice.bell(0, 10));
			girl->beauty(g_Dice % 2);
			girl->charisma(g_Dice % 2);
		}
		else
		{
			ss << "She is sad and has lost some health during the operation.\n";
			girl->health(g_Dice.bell(-20, 2));
			girl->happiness(g_Dice.bell(-10, 1));
			girl->spirit(g_Dice.bell(-5, 1));
			girl->mana(g_Dice.bell(-20, 3));
			girl->beauty(g_Dice.bell(-1, 1));
			girl->charisma(g_Dice.bell(-1, 1));
		}

		if (girl->has_trait( "Flat Chest"))
		{
			ss << girlName << "'s breasts are as small as they can get so she was sent to the waiting room.";
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
	if (g_Dice % 10 == 0)
		girl->medicine(1);	// `J` she watched what the doctors and nurses were doing

#pragma endregion
	return false;
}

double cJobManager::JP_BreastReduction(sGirl* girl, bool estimate)
{
	double jobperformance = 0.0;
	if (estimate)	// for third detail string - how much do they need this?
	{
		/* */if (girl->has_trait( "Flat Chest"))				return -1000;
		else if (girl->has_trait( "Petite Breasts"))			return 0;
		else if (girl->has_trait( "Small Boobs"))				return 70;
		else if (girl->has_trait( "Busty Boobs"))				return 100;
		else if (girl->has_trait( "Big Boobs"))				return 150;
		else if (girl->has_trait( "Giant Juggs"))				return 200;
		else if (girl->has_trait( "Massive Melons"))			return 250;
		else if (girl->has_trait( "Abnormally Large Boobs"))	return 350;
		else if (girl->has_trait( "Titanic Tits"))			return 400;
		else /*                                                  */ return 100;
	}
	return jobperformance;
}
