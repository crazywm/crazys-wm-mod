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

// `J` Centre Job - Rehab_Job - Full_Time_Job
bool cJobManager::WorkRehab(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	girl->m_DayJob = girl->m_NightJob = JOB_REHAB;	// it is a full time job

	stringstream ss;
	string girlName = girl->m_Realname;
	cConfig cfg;

	g_Girls.UnequipCombat(girl);	// not for patient

	int enjoy = 0;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();
	int msgtype = Day0Night1, imagetype = IMGTYPE_PROFILE;

	ss << girlName << " underwent rehab for her addiction.\n\n";

	// if she was not in rehab yesterday, reset working days to 0 before proceding
	if (girl->m_YesterDayJob != JOB_REHAB) girl->m_PrevWorkingDay = girl->m_WorkingDay = 0;

	// `J` this will be taken care of in the centre reflow - leaving it in anyway
	if (!g_Girls.HasTrait(girl, "Fairy Dust Addict") &&		// `J` if the girl is not an addict
		!g_Girls.HasTrait(girl, "Shroud Addict") &&
		!g_Girls.HasTrait(girl, "Viras Blood Addict"))
	{
		ss.str("");
		ss << girlName << " is not addicted to anything so she was sent to the waiting room.";
		if (Day0Night1 == SHIFT_DAY)	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		girl->m_YesterDayJob = girl->m_YesterNightJob = JOB_CENTREREST;
		girl->m_DayJob = girl->m_NightJob = JOB_CENTREREST;
		girl->m_PrevWorkingDay = girl->m_WorkingDay = 0;
		return false;	// not refusing
	}

	if (g_Centre.GetNumGirlsOnJob(brothel->m_id, JOB_DRUGCOUNSELOR, true) < 1 || g_Centre.GetNumGirlsOnJob(brothel->m_id, JOB_DRUGCOUNSELOR, false) < 1)
	{
		ss << "She sits in rehab doing nothing. You must assign a drug counselor to treat her.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return false;	// not refusing
	}

	if (roll_a <= 50 && g_Girls.DisobeyCheck(girl, ACTION_WORKREHAB, brothel))
	{
		ss << "She fought with the counselor and did not make any progress this week.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKREHAB, -1, true);
		if (Day0Night1 == SHIFT_NIGHT) girl->m_WorkingDay--;
		return true;
	}

	if (Day0Night1 == SHIFT_DAY) girl->m_WorkingDay++;

	g_Girls.UpdateStat(girl, STAT_HAPPINESS, g_Dice % 30 - 20);
	g_Girls.UpdateStat(girl, STAT_SPIRIT, g_Dice % 5 - 10);
	g_Girls.UpdateStat(girl, STAT_MANA, g_Dice % 5 - 10);

	int healthmod = (g_Dice % 10) - 15;
	// `J` % chance a counselor will save her if she almost dies
	if (girl->health() + healthmod < 1 && g_Dice.percent(95 + (girl->health() + healthmod))
		&& (g_Centre.GetNumGirlsOnJob(brothel->m_id, JOB_DRUGCOUNSELOR, true) > 0 || g_Centre.GetNumGirlsOnJob(brothel->m_id, JOB_DRUGCOUNSELOR, false) > 0))
	{	// Don't kill the girl from rehab if a Drug Counselor is on duty
		g_Girls.SetStat(girl, STAT_HEALTH, 1);
		g_Girls.UpdateStat(girl, STAT_PCFEAR, 5);
		g_Girls.UpdateStat(girl, STAT_PCLOVE, -10);
		g_Girls.UpdateStat(girl, STAT_PCHATE, 10);
		ss << "She almost died in rehab but the Counselor saved her.\n";
		ss << "She hates you a little more for forcing this on her.\n\n";
		msgtype = EVENT_DANGER;
		enjoy -= 2;
	}
	else
	{
		g_Girls.UpdateStat(girl, STAT_HEALTH, healthmod);
		enjoy += (healthmod / 5) + 1;
	}

	if (girl->health() < 1)	// it should never get to this but have it here just in case.
	{
		ss << girlName << " died in rehab.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_DEATH, EVENT_DANGER);
		return false;
	}

	if (girl->m_WorkingDay >= 3 && Day0Night1 == SHIFT_NIGHT)
	{
		enjoy += g_Dice % 10;
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKCOUNSELOR, g_Dice%6-2, true);	// `J` She may want to help others with their problems
		g_Girls.UpdateStat(girl, STAT_HAPPINESS, g_Dice % 10);

		ss << "The rehab is a success.\n";
		msgtype = EVENT_GOODNEWS;
		if (g_Girls.HasTrait(girl, "Fairy Dust Addict"))
		{
			g_Girls.RemoveTrait(girl, "Fairy Dust Addict");
			ss << "She is no longer a fairy dust addict.\n";
		}
		else if (g_Girls.HasTrait(girl, "Shroud Addict"))
		{
			g_Girls.RemoveTrait(girl, "Shroud Addict");
			ss << "She is no longer a shroud addict.\n";
		}
		else if (g_Girls.HasTrait(girl, "Viras Blood Addict"))
		{
			g_Girls.RemoveTrait(girl, "Viras Blood Addict");
			ss << "She is no longer a viras blood addict.\n";
		}
		girl->m_PrevWorkingDay = girl->m_WorkingDay = 0;

		if (g_Girls.HasTrait(girl, "Fairy Dust Addict") || g_Girls.HasTrait(girl, "Shroud Addict") || g_Girls.HasTrait(girl, "Viras Blood Addict"))
		{
			// stay in rehab for another session
			ss << "\nShe should stay in rehab to treat her other addictions.";
		}
		else // get out of rehab
		{
			ss << "\nShe has been released from rehab.";
			girl->m_DayJob = girl->m_NightJob = JOB_CENTREREST;
			girl->m_PrevDayJob = girl->m_PrevNightJob = 255;
		}
	}
	else
	{
		ss << "The rehab is in progress (" << (3 - girl->m_WorkingDay) << " day remaining).";
	}

	girl->m_Events.AddMessage(ss.str(), imagetype, msgtype);

	// Improve girl
	int libido = 1;
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	g_Girls.UpdateEnjoyment(girl, ACTION_WORKREHAB, enjoy, true);
	return false;
}