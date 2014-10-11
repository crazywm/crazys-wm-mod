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

// `J` Clinic Job - Surgery
bool cJobManager::WorkHealing(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";

	// not for patient
	g_Girls.UnequipCombat(girl);

	bool hasDoctor = false;
	if (g_Clinic.GetNumGirlsOnJob(brothel->m_id, JOB_DOCTOR, true) > 0 || g_Clinic.GetNumGirlsOnJob(brothel->m_id, JOB_DOCTOR, false) > 0)
		hasDoctor = true;

	if (!hasDoctor)
	{
		message = girl->m_Realname + gettext(" does nothing. You don't have any Doctor (require 1) ");
		(DayNight == 0) ? message += gettext("day") : message += gettext("night"); message += gettext(" Shift.");
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_WARNING);
		return true;
	}

	if (g_Girls.HasTrait(girl, "Construct"))
	{
		message = girl->m_Realname + gettext(" has no biological parts so she was sent to the repair shop.");
		if(DayNight == 0) girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_WARNING);
		girl->m_DayJob = girl->m_NightJob = JOB_GETREPAIRS;
		return true;
	}

	int numdocs = g_Clinic.GetNumGirlsOnJob(0, JOB_DOCTOR, DayNight);
	int numnurse = g_Clinic.GetNumGirlsOnJob(0, JOB_NURSE, DayNight);
	
	message = gettext("She does nothing while the Doctor");if (numdocs > 1) message += gettext("s");

	if (g_Girls.HasTrait(girl, "Half-Construct") && girl->m_DayJob == JOB_GETHEALING && girl->m_NightJob == JOB_GETHEALING)
	{
		g_Girls.UpdateStat(girl, STAT_HEALTH, 30);	// Total 60 healing per day, heals less because Doctor only fixes living tissue.
	}
	else
	{
		g_Girls.UpdateStat(girl, STAT_HEALTH, 40);
	}
	if (numnurse > 0)
	{
		message += gettext(" and Nurse");if (numnurse > 1) message += gettext("s");
		g_Girls.UpdateStat(girl, STAT_TIREDNESS, -30);
		g_Girls.UpdateStat(girl, STAT_HAPPINESS, 10);
		g_Girls.UpdateStat(girl, STAT_MANA, 40);
	}
	else 
	{
		g_Girls.UpdateStat(girl, STAT_TIREDNESS, -20);
		g_Girls.UpdateStat(girl, STAT_MANA, 30);
	}
	if (numdocs + numnurse >= 4 && DayNight == 1)	// lots of people making sure she is in good health
	{
		g_Girls.UpdateStat(girl, STAT_CONSTITUTION, 1);
	}
	((numdocs > 2 && numnurse < 1) || numnurse > 1) ? message += gettext(" take ") : message += gettext(" takes ");
	message += gettext("care of her.");
	girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, DayNight);

	// Improve girl
	int libido = 1;
	if (g_Girls.HasTrait(girl, "Lesbian"))		libido += numnurse;
	if (g_Girls.HasTrait(girl, "Masochist"))	libido += 1;
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))	libido += 2;
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);
	if (g_Dice % 10 == 0)
		g_Girls.UpdateSkill(girl, SKILL_MEDICINE, 1);	// `J` she watched what the doctors and nurses were doing

	return false;
}
