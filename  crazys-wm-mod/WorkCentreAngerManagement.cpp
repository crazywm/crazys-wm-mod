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
bool cJobManager::WorkCentreAngerManagement(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
#pragma region //	Job setup				//
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int actiontype = ACTION_WORKTHERAPY;
	// if she was not in thearpy yesterday, reset working days to 0 before proceding
	if (girl->m_YesterDayJob != JOB_ANGER) { girl->m_WorkingDay = girl->m_PrevWorkingDay = 0; }
	girl->m_DayJob = girl->m_NightJob = JOB_ANGER;	// it is a full time job

	if (!girl->has_trait( "Aggressive") &&		// if the girl doesnt need this
		!girl->has_trait( "Tsundere") &&
		!girl->has_trait( "Yandere"))
	{
		ss << " doesn't need anger management so she was sent to the waiting room.";
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
		return false;	// not refusing
	}
	if (g_Dice.percent(20) || girl->disobey_check(actiontype, brothel))	// `J` - yes, OR, not and.
	{
		girl->upd_Enjoyment(actiontype, -1);
		if (Day0Night1) girl->m_WorkingDay--;
		if (g_Dice.percent(10))
		{
			girl->upd_Enjoyment(actiontype, -5);
			bool runaway = false;
			// if there is no counselor, it should not get to here
			sGirl* counselor = g_Centre.GetRandomGirlOnJob(0, JOB_COUNSELOR, Day0Night1);
			ss << "\n \n" << girlName << " fought hard with her counselor " << counselor->m_Realname;
			int winner = g_Girls.girl_fights_girl(girl, counselor);
			if (winner == 1)	// the patient won
			{
				ss << " and won.\n \n";
				girl->upd_Enjoyment(ACTION_COMBAT, 5);
				counselor->upd_Enjoyment(ACTION_WORKCOUNSELOR, -5);
				counselor->upd_Enjoyment(ACTION_COMBAT, -2);

				if (g_Dice.percent(10))	// and ran away
				{
					runaway = true;
					stringstream smess;
					smess << girlName << " fought with her counselor and ran away.\nSend your goons after her to attempt recapture.\nShe will escape for good after 6 weeks.\n";
					g_MessageQue.AddToQue(smess.str(), COLOR_RED);

					g_Centre.RemoveGirl(0, girl, false);
					girl->m_RunAway = 6;
					girl->m_NightJob = girl->m_DayJob = JOB_RUNAWAY;
					g_Brothels.AddGirlToRunaways(girl);
					return true;
				}
			}
			else	// the counselor won
			{
				ss << " and lost.\n \n";
				girl->upd_Enjoyment(actiontype, -5);
				girl->upd_Enjoyment(ACTION_COMBAT, -5);
				counselor->upd_Enjoyment(ACTION_WORKCOUNSELOR, -1);
				counselor->upd_Enjoyment(ACTION_COMBAT, 2);
			}
			stringstream ssc;
			ssc << counselor->m_Realname << " had to defend herself from " << girlName << " who she was counceling.\n";
			if (runaway) ss << girlName << " ran out of the Counceling Centre and has not been heard from since.";
			counselor->m_Events.AddMessage(ssc.str(), IMGTYPE_COMBAT, EVENT_WARNING);
		}
		else
		{
			ss << " fought with her counselor and did not make any progress this week.";
		}
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
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
	int healthmod = (g_Dice % 15) - 11;
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
		ss << "She died in anger management.";
		msgtype = EVENT_DANGER;
	}

	if (girl->m_WorkingDay >= 3 && Day0Night1)
	{
		enjoy += g_Dice % 10;
		girl->upd_Enjoyment(ACTION_WORKCOUNSELOR, g_Dice % 6 - 2);	// `J` She may want to help others with their problems
		girl->happiness(g_Dice % 10);

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
				if (girl->has_trait( "Aggressive"))
				{
					girl->remove_trait("Aggressive");
					ss << "She is no longer Aggressive.\n";
					cured = true; break;
				}
			case 1:
				if (girl->has_trait( "Tsundere"))
				{
					girl->remove_trait("Tsundere");
					ss << "She is no longer a Tsundere.\n";
					cured = true; break;
				}
			case 2:
			default:
				if (girl->has_trait( "Yandere"))
				{
					girl->remove_trait("Yandere");
					ss << "She is no longer a Yandere.\n";
					cured = true; break;
				}
			}
		}

		if (girl->has_trait( "Aggressive") || girl->has_trait( "Tsundere") || girl->has_trait( "Yandere"))
		{
			ss << "\nShe should stay in anger management to treat her other anger issues.";
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


double cJobManager::JP_CentreAngerManagement(sGirl* girl, bool estimate)
{
	double jobperformance = 100;
	if (girl->has_trait( "Aggressive"))	jobperformance += 100;	// if she has 1 = A
	if (girl->has_trait( "Tsundere"))		jobperformance += 100;	// if she has 2 = S
	if (girl->has_trait( "Yandere"))		jobperformance += 100;	// if she has 3 = I

	if (jobperformance == 100)	return -1000;			// X - does not need it

	return jobperformance;
}
