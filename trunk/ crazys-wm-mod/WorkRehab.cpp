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

bool cJobManager::WorkRehab(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";
	u_int job = 0;	
	int msgtype = DayNight;


	if (girl->m_YesterDayJob != JOB_REHAB)	// if she was not in rehab yesterday, 
	{
		girl->m_WorkingDay = 0;				// rest working days to 0 before proceding
		girl->m_PrevWorkingDay = 0;
	}
	
	// not for patient
	g_Girls.UnequipCombat(girl);

	bool hasDoctor = false;
	if(g_Centre.GetNumGirlsOnJob(brothel->m_id, JOB_DRUGCOUNSELOR, true) >= 1 || g_Centre.GetNumGirlsOnJob(brothel->m_id, JOB_DRUGCOUNSELOR, false) >= 1)
		hasDoctor = true;

	if (!hasDoctor)
	{
		string message = girl->m_Realname + gettext(" you must have a drug counselor (require 1)");
		if(DayNight == 0)	message += gettext("day");
		else				message += gettext("night");
		message += gettext(" Shift.");

		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_WARNING);
		return true;
	}
	if (!g_Girls.HasTrait(girl, "Fairy Dust Addict") &&		// `J` if the girl is not an addict
		!g_Girls.HasTrait(girl, "Shroud Addict") &&
		!g_Girls.HasTrait(girl, "Viras Blood Addict"))
	{
		message = girl->m_Realname + gettext(" is not addicted to anything so she was sent to the waiting room.");
		if (DayNight == 0)	girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_WARNING);
		girl->m_YesterDayJob = girl->m_YesterNightJob = JOB_CENTREREST;
		girl->m_DayJob = girl->m_NightJob = JOB_CENTREREST;
		girl->m_PrevWorkingDay = girl->m_WorkingDay = 0;
		return true;
	}


	if(DayNight == 0)
	{
		girl->m_WorkingDay++;
		job	= girl->m_DayJob;
	}
	else
	{
		job	= girl->m_NightJob;
	}

	stringstream ss;
	if(girl->m_WorkingDay == 3)
	{
		g_Girls.UpdateStat(girl, STAT_HAPPINESS, -20);
		g_Girls.UpdateStat(girl, STAT_SPIRIT, -5);
		if (girl->health() - 20 < 1 && (g_Centre.GetNumGirlsOnJob(brothel->m_id, JOB_DRUGCOUNSELOR, true) >= 1 || g_Centre.GetNumGirlsOnJob(brothel->m_id, JOB_DRUGCOUNSELOR, false) >= 1))
		{	// Don't kill the girl from rehab if a Drug Counselor is on duty
			g_Girls.SetStat(girl, STAT_HEALTH, 1);
			g_Girls.UpdateStat(girl, STAT_PCFEAR, 5);
			g_Girls.UpdateStat(girl, STAT_PCLOVE, -10);
			g_Girls.UpdateStat(girl, STAT_PCHATE, 10);
			ss << "She almost died in rehab but the Counselor saved her.\n";
			ss << "She hates you a little more for forcing this on her.";
			girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_DANGER);
		}
		else
		{
			g_Girls.UpdateStat(girl, STAT_HEALTH, -20);
		}
		g_Girls.UpdateStat(girl, STAT_MANA, -20);

		if (girl->health()< 1)
		{
			ss << "She died in rehab.";
			msgtype = EVENT_DANGER;
		}
		else
		{
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
			girl->m_DayJob = JOB_CENTREREST;
			girl->m_NightJob = JOB_CENTREREST;
		}
	}
	else
	{
		ss << "The rehab is in progress (" << (3 - girl->m_WorkingDay) << " day remaining).";
	}

	// Improve girl
	int libido = 1;

	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, msgtype);

	return false;
}
