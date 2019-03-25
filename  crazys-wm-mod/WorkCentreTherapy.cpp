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
#include "cCentre.h"
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
extern cCentreManager g_Centre;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;

#pragma endregion

// `J` Job Centre - Therapy
bool cJobManager::WorkCentreTherapy(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
#pragma region //	Job setup				//
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int actiontype = ACTION_WORKTHERAPY;
	// if she was not in thearpy yesterday, reset working days to 0 before proceding
	if (girl->m_YesterDayJob != JOB_THERAPY) { girl->m_WorkingDay = girl->m_PrevWorkingDay = 0; }
	girl->m_DayJob = girl->m_NightJob = JOB_THERAPY;	// it is a full time job

	if (!girl->has_trait( "Nervous") &&		//  if the girl doesnt need this
		!girl->has_trait( "Dependant") &&
		!girl->has_trait( "Pessimist"))
	{
		ss << " doesn't need therapy for anything so she was sent to the waiting room.";
		if (Day0Night1 == 0)	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		girl->m_PrevDayJob = girl->m_PrevNightJob = girl->m_YesterDayJob = girl->m_YesterNightJob = girl->m_DayJob = girl->m_NightJob = JOB_CENTREREST;
		girl->m_PrevWorkingDay = girl->m_WorkingDay = 0;
		return false; // not refusing
	}
	bool hasCounselor = g_Centre.GetNumGirlsOnJob(0, JOB_COUNSELOR, Day0Night1) > 0;
	if (!hasCounselor)
	{
		ss << " has no counselor to help her on the " << (Day0Night1 ? "night" : "day") << " Shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		return false; // not refusing
	}
	if (g_Dice.percent(10) && girl->disobey_check(actiontype, brothel))
	{
		ss << " fought with her counselor and did not make any progress this week.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		girl->upd_Enjoyment(actiontype, -1);
		if (Day0Night1) girl->m_WorkingDay--;
		return true;
	}
	ss << " underwent therapy for mental issues.\n \n";

	g_Girls.UnequipCombat(girl);	// not for patient

	int enjoy = 0;
	int msgtype = Day0Night1;

#pragma endregion
#pragma region //	Count the Days				//

	if (!Day0Night1) girl->m_WorkingDay++;

	girl->happiness(g_Dice % 30 - 20);
	girl->spirit(g_Dice % 5 - 10);
	girl->mana(g_Dice % 5 - 10);

	// `J` % chance a counselor will save her if she almost dies
	int healthmod = (g_Dice % 6) - 4;
	if (girl->health() + healthmod < 1 && g_Dice.percent(95 + (girl->health() + healthmod)) &&
		(g_Centre.GetNumGirlsOnJob(brothel->m_id, JOB_COUNSELOR, true) > 0 || g_Centre.GetNumGirlsOnJob(brothel->m_id, JOB_COUNSELOR, false) > 0))
	{	// Don't kill the girl from therapy if a Counselor is on duty
		g_Girls.SetStat(girl, STAT_HEALTH, 1);
		girl->pcfear(5);
		girl->pclove(-10);
		girl->pchate(10);
		ss << "She almost died in rehab but the Counselor saved her.\n";
		ss << "She hates you a little more for forcing this on her.\n \n";
		msgtype = EVENT_DANGER;
		enjoy -= 2;
	}
	else
	{
		girl->health(healthmod);
		enjoy += (healthmod / 5) + 1;
	}

	if (girl->health() < 1)
	{
		ss << "She died in therapy.";
		msgtype = EVENT_DANGER;
	}

	if (girl->m_WorkingDay >= 3 && Day0Night1)
	{
		girl->m_PrevWorkingDay = girl->m_WorkingDay = 0;
		enjoy += g_Dice % 5;
		girl->upd_Enjoyment(ACTION_WORKCOUNSELOR, g_Dice % 6 - 2);	// `J` She may want to help others with their problems
		girl->happiness(g_Dice % 5);

		ss << "The therapy is a success.\n";
		msgtype = EVENT_GOODNEWS;

		bool cured = false;
		int tries = 5;
		while (!cured && tries > -2)
		{
			tries--;
			int t = max(0, g_Dice % tries);
			switch (t)
			{
			case 0:

				if (girl->has_trait( "Nervous"))
				{
					girl->remove_trait("Nervous");
					ss << "She is no longer nervous all the time.\n";
					cured = true; break;
				}
			case 1:
				if (girl->has_trait( "Dependant"))
				{
					girl->remove_trait("Dependant");
					ss << "She is no longer Dependant on others.\n";
					cured = true; break;
				}
			case 2:
			default:
				if (girl->has_trait( "Pessimist"))
				{
					girl->remove_trait("Pessimist");
					ss << "She is no longer a Pessimist about everything.\n";
					cured = true; break;
				}
			}
		}

		if (girl->has_trait( "Nervous") || girl->has_trait( "Dependant") || girl->has_trait( "Pessimist"))
		{
			ss << "\nShe should stay in therapy to treat her other disorders.";
		}
		else // get out of therapy
		{
			ss << "\nShe has been released from therapy.";
			girl->m_PrevDayJob = girl->m_PrevNightJob = girl->m_YesterDayJob = girl->m_YesterNightJob = girl->m_DayJob = girl->m_NightJob = JOB_CENTREREST;
			girl->m_PrevWorkingDay = girl->m_WorkingDay = 0;
		}
	}
	else
	{
		ss << "The therapy is in progress (" << (3 - girl->m_WorkingDay) << " day remaining).";
	}

#pragma endregion
#pragma region	//	Finish the shift			//

	// Improve girl
	int libido = 1;

	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }

	girl->upd_temp_stat(STAT_LIBIDO, libido);
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, msgtype);
	girl->upd_Enjoyment(actiontype, enjoy);

#pragma endregion
	return false;
}

double cJobManager::JP_CentreTherapy(sGirl* girl, bool estimate)
{
	double jobperformance = 100;
	if (girl->has_trait( "Nervous"))		jobperformance += 100;	// if she has 1 = A
	if (girl->has_trait( "Dependant"))	jobperformance += 100;	// if she has 2 = S
	if (girl->has_trait( "Pessimist"))	jobperformance += 100;	// if she has 3 = I

	if (jobperformance == 100)	return -1000;			// X - does not need it

	return jobperformance;
}
