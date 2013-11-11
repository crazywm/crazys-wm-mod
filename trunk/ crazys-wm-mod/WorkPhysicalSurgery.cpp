/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders 
 * who meet on http://pinkpetal.co.cc
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

bool cJobManager::WorkPhysicalSurgery(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";
	u_int job = 0;	

	// not for patient
	g_Girls.UnequipCombat(girl);

	bool hasDoctor = false;
	if(g_Clinic.GetNumGirlsOnJob(brothel->m_id, JOB_DOCTOR, true) >= 1 || g_Clinic.GetNumGirlsOnJob(brothel->m_id, JOB_DOCTOR, false) >= 1)
		hasDoctor = true;

	if (!hasDoctor)
	{
		string message = girl->m_Realname + gettext(" do nothing. You don't have any Doctor (require 1)");
		if(DayNight == 0)
			message += gettext("day");
		else
			message += gettext("night");
		message += gettext(" Shift.");

		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_WARNING);
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
	if (g_Clinic.GetNumGirlsOnJob(0,JOB_NURSE,false) == 1)
	{
	if(girl->m_WorkingDay == 3)
	{
		ss << "The physical surgery is a success. She is sad and has lost some health during the operation.";
		g_Girls.UpdateStat(girl, STAT_HAPPINESS, -20);
		g_Girls.UpdateStat(girl, STAT_SPIRIT, -5);
		g_Girls.UpdateStat(girl, STAT_HEALTH, -20);
		g_Girls.UpdateStat(girl, STAT_MANA, -20);
		g_Girls.UpdateStat(girl, STAT_BEAUTY, 40);
		g_Girls.UpdateStat(girl, STAT_CHARISMA, 40);
		if (g_Girls.HasTrait(girl, "Abnormally Large Boobs"))
		{
			g_Girls.RemoveTrait(girl, "Abnormally Large Boobs");
			girl->add_trait("Big Boobs", false);
			ss << "She loses Abnormally Large Boobs trait but gains Big Boobs traits.";
		}
		else if (g_Girls.HasTrait(girl, "Small Boobs"))
		{
			g_Girls.RemoveTrait(girl, "Small Boobs");
			girl->add_trait("Big Boobs", false);
			ss << "She loses Small Boobs trait but gains Big Boobs traits.";
		}
		else if (!g_Girls.HasTrait(girl, "Sexy Air"))
		{
			girl->add_trait("Sexy Air", false);
			ss << "She gains Sexy Air trait.";
		}
		else if (!g_Girls.HasTrait(girl, "Cute"))
		{
			girl->add_trait("Cute", false);
			ss << "She gains Cute trait.";
		}
		else if (!g_Girls.HasTrait(girl, "Great Figure"))
		{
			girl->add_trait("Great Figure", false);
			ss << "She gains Great Figure trait.";
		}
		girl->m_WorkingDay = 0;
	}
	else
	{
		ss << "The operation is in progess (" << (3 - girl->m_WorkingDay) << " day remaining).";
	}
	}
	else
		if(girl->m_WorkingDay == 5)
	{
		ss << "The physical surgery is a success. She is sad and has lost some health during the operation.";
		g_Girls.UpdateStat(girl, STAT_HAPPINESS, -20);
		g_Girls.UpdateStat(girl, STAT_SPIRIT, -5);
		g_Girls.UpdateStat(girl, STAT_HEALTH, -20);
		g_Girls.UpdateStat(girl, STAT_MANA, -20);
		g_Girls.UpdateStat(girl, STAT_BEAUTY, 40);
		g_Girls.UpdateStat(girl, STAT_CHARISMA, 40);
		if (g_Girls.HasTrait(girl, "Abnormally Large Boobs"))
		{
			g_Girls.RemoveTrait(girl, "Abnormally Large Boobs");
			girl->add_trait("Big Boobs", false);
			ss << "She loses Abnormally Large Boobs trait but gains Big Boobs traits.";
		}
		else if (g_Girls.HasTrait(girl, "Small Boobs"))
		{
			g_Girls.RemoveTrait(girl, "Small Boobs");
			girl->add_trait("Big Boobs", false);
			ss << "She loses Small Boobs trait but gains Big Boobs traits.";
		}
		else if (!g_Girls.HasTrait(girl, "Sexy Air"))
		{
			girl->add_trait("Sexy Air", false);
			ss << "She gains Sexy Air trait.";
		}
		else if (!g_Girls.HasTrait(girl, "Cute"))
		{
			girl->add_trait("Cute", false);
			ss << "She gains Cute trait.";
		}
		else if (!g_Girls.HasTrait(girl, "Great Figure"))
		{
			girl->add_trait("Great Figure", false);
			ss << "She gains Great Figure trait.";
		}
		girl->m_WorkingDay = 0;
	}
	else
	{
		ss << "The operation is in progess (" << (5 - girl->m_WorkingDay) << " day remaining).";
	}

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, DayNight);

	return false;
}
