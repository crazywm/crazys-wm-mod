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

// `J` Job Centre - Therapy - Full_Time_Job
bool cJobManager::WorkRehab(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
#pragma region //	Job setup				//
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int actiontype = ACTION_WORKREHAB;
	// if she was not in rehab yesterday, reset working days to 0 before proceding
	if (girl->m_YesterDayJob != JOB_REHAB) girl->m_PrevWorkingDay = girl->m_WorkingDay = 0;
	girl->m_DayJob = girl->m_NightJob = JOB_REHAB;	// it is a full time job

	if (!girl->has_trait( "Fairy Dust Addict")	&&	!girl->has_trait( "Alcoholic") &&
		!girl->has_trait( "Shroud Addict")		&&	!girl->has_trait( "Cum Addict") &&
		!girl->has_trait( "Viras Blood Addict")	&&	!girl->has_trait( "Smoker"))
	{
		ss << " is not addicted to anything so she was sent to the waiting room.";
		if (Day0Night1 == SHIFT_DAY)	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		girl->m_PrevDayJob = girl->m_PrevNightJob = girl->m_YesterDayJob = girl->m_YesterNightJob = girl->m_DayJob = girl->m_NightJob = JOB_CENTREREST;
		girl->m_PrevWorkingDay = girl->m_WorkingDay = 0;
		return false;	// not refusing
	}
	bool hasCounselor = g_Centre.GetNumGirlsOnJob(0, JOB_COUNSELOR, Day0Night1) > 0;
	if (!hasCounselor)
	{
		ss << " sits in rehab doing nothing. You must assign a counselor to treat her.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return false;	// not refusing
	}

	if (g_Dice.percent(50) && girl->disobey_check(actiontype, brothel))
	{
		ss << " fought with her counselor and did not make any progress this week.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		girl->upd_Enjoyment(actiontype, -1);
		if (Day0Night1) girl->m_WorkingDay--;
		return true;
	}
	ss << " underwent rehab for her addiction.\n \n";

	g_Girls.UnequipCombat(girl);	// not for patient

	int enjoy = 0;
	int msgtype = Day0Night1, imagetype = IMGTYPE_PROFILE;

#pragma endregion
#pragma region //	Count the Days				//

	if (!Day0Night1) girl->m_WorkingDay++;

	girl->happiness(g_Dice % 30 - 20);
	girl->spirit(g_Dice % 5 - 10);
	girl->mana(g_Dice % 5 - 10);

	int healthmod = (g_Dice % 10) - 15;
	// `J` % chance a counselor will save her if she almost dies
	if (girl->health() + healthmod < 1 && g_Dice.percent(95 + (girl->health() + healthmod)) &&
		(g_Centre.GetNumGirlsOnJob(brothel->m_id, JOB_COUNSELOR, true) > 0 || g_Centre.GetNumGirlsOnJob(brothel->m_id, JOB_COUNSELOR, false) > 0))
	{	// Don't kill the girl from rehab if a Counselor is on duty
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

	if (girl->health() < 1)	// it should never get to this but have it here just in case.
	{
		ss << girlName << " died in rehab.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_DEATH, EVENT_DANGER);
		return false;
	}

	if (girl->m_WorkingDay >= 3 && Day0Night1)
	{
		girl->m_PrevWorkingDay = girl->m_WorkingDay = 0;
		enjoy += g_Dice % 10;
		girl->upd_Enjoyment(ACTION_WORKCOUNSELOR, g_Dice.bell(-1, 4));	// `J` She may want to help others with their problems
		girl->happiness(g_Dice % 10);

		ss << "The rehab is a success.\n";
		msgtype = EVENT_GOODNEWS;

		bool cured = false;
		int tries = 10;
		while (!cured && tries > -2)
		{
			tries--;
			int t = max(0, g_Dice % tries);
			switch (t)
			{
			case 0:
				if (girl->has_trait( "Viras Blood Addict"))
				{
					girl->remove_trait("Viras Blood Addict", true);
					ss << "She is no longer a viras blood addict.\n";
					cured = true; break;
				}
			case 1:
				if (girl->has_trait( "Shroud Addict"))
				{
					girl->remove_trait("Shroud Addict", true);
					ss << "She is no longer a shroud addict.\n";
					cured = true; break;
				}
			case 2:
				if (girl->has_trait( "Fairy Dust Addict"))
				{
					girl->remove_trait("Fairy Dust Addict", true);
					ss << "She is no longer a fairy dust addict.\n";
					cured = true; break;
				}
			case 3:
				if (girl->has_trait( "Alcoholic"))
				{
					girl->remove_trait("Alcoholic", true);
					ss << "She is no longer an alcoholic.\n";
					cured = true; break;
				}
			case 4:
				if (girl->has_trait( "Cum Addict"))
				{
					girl->remove_trait("Cum Addict", true);
					ss << "She is no longer a cum addict.\n";
					cured = true; break;
				}
			case 5:
			default:
				if (girl->has_trait( "Smoker"))
				{
					girl->remove_trait("Smoker", true);
					ss << "She is no longer a smoker.\n";
					cured = true; break;
				}
			}
		}

		girl->add_trait("Former Addict", 40);

		if (girl->has_trait( "Fairy Dust Addict") || girl->has_trait( "Shroud Addict") || girl->has_trait( "Cum Addict") ||
			girl->has_trait( "Viras Blood Addict") || girl->has_trait( "Alcoholic") || girl->has_trait( "Smoker"))
		{
			ss << "\nShe should stay in rehab to treat her other addictions.";
		}
		else // get out of rehab
		{
			ss << "\nShe has been released from rehab.";
			girl->m_PrevDayJob = girl->m_PrevNightJob = girl->m_YesterDayJob = girl->m_YesterNightJob = girl->m_DayJob = girl->m_NightJob = JOB_CENTREREST;
		}
	}
	else
	{
		ss << "The rehab is in progress (" << (3 - girl->m_WorkingDay) << " day remaining).";
	}

#pragma endregion
#pragma region	//	Finish the shift			//

	girl->m_Events.AddMessage(ss.str(), imagetype, msgtype);

	// Improve girl
	int libido = 1;
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }
	girl->upd_temp_stat(STAT_LIBIDO, libido);
	girl->upd_Enjoyment(actiontype, enjoy);

#pragma endregion
	return false;
}

double cJobManager::JP_Rehab(sGirl* girl, bool estimate)// not used
{
	double jobperformance = 110;
	if (girl->has_trait( "Smoker"))				jobperformance += 40;	// if she has 1 = C
	if (girl->has_trait( "Cum Addict"))			jobperformance += 40;	// if she has 2 = B
	if (girl->has_trait( "Fairy Dust Addict"))	jobperformance += 40;	// if she has 3 = A
	if (girl->has_trait( "Shroud Addict"))		jobperformance += 40;	// if she has 4 = S
	if (girl->has_trait( "Alcoholic"))			jobperformance += 40;	// if she has 5 = S
	if (girl->has_trait( "Viras Blood Addict"))	jobperformance += 40;	// if she has 6 = I

	if (jobperformance == 110)	return -1000;			// X - does not need it

	return jobperformance;
}
