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

bool cJobManager::WorkRepairShop(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";

	// not for patient
	g_Girls.UnequipCombat(girl);

	bool hasMechanic = false;
	if (g_Clinic.GetNumGirlsOnJob(brothel->m_id, JOB_MECHANIC, true) >= 1 || g_Clinic.GetNumGirlsOnJob(brothel->m_id, JOB_MECHANIC, false) >= 1)
		hasMechanic = true;

	if (!hasMechanic)
	{
		message = girl->m_Realname + gettext(" does nothing. You don't have a Mechanic (require 1) ");
		(DayNight == 0) ? message += gettext("day") : message += gettext("night"); message += gettext(" Shift.");
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_WARNING);
		return true;
	}

	if (!g_Girls.HasTrait(girl, "Construct") || !g_Girls.HasTrait(girl, "Half-Construct"))
	{
		message = girl->m_Realname + gettext(" has no artificial parts so she was sent to the Healing center.");
		if (DayNight == 0)	girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_WARNING);
		girl->m_DayJob = girl->m_NightJob = JOB_GETHEALING;
		return true;
	}

	int nummecs = g_Clinic.GetNumGirlsOnJob(0, JOB_MECHANIC, DayNight == 0);
	int numnurse = g_Clinic.GetNumGirlsOnJob(0, JOB_NURSE, DayNight == 0);

	if (g_Girls.HasTrait(girl, "Construct"))
	{
		g_Girls.UpdateStat(girl, STAT_HEALTH, 400); // constructs heal at 10% of normal so 400=40 to her
	}
	else if (g_Girls.HasTrait(girl, "Half-Construct"))
	{
		if (girl->m_DayJob == JOB_GETREPAIRS && girl->m_NightJob == JOB_GETREPAIRS)	// Fixed by Mechanic for both shifts.
		{
			g_Girls.UpdateStat(girl, STAT_HEALTH, 30);	// Total 60 healing per day, heals less because Mechanic does not fix living tissue.
		}
		else 
		{
			g_Girls.UpdateStat(girl, STAT_HEALTH, 40);	// Total 80 healing per day with Doctor doing the other half.
		}
	}
	else // "But you're not a construct? Whatever, hop on the table and I'll see what I can do."
	{	
		g_Girls.UpdateStat(girl, STAT_HEALTH, 10);	
	}

	message = girl->m_Realname + gettext(" was repaired.");
	message += gettext("She does nothing while the Mechanic");if (nummecs > 1)message += gettext("s");
	if (numnurse > 0)
	{
		message += gettext(" and Nurse");if (numnurse > 1)message += gettext("s");
		g_Girls.UpdateStat(girl, STAT_TIREDNESS, -30);
		g_Girls.UpdateStat(girl, STAT_HAPPINESS, 10);
		g_Girls.UpdateStat(girl, STAT_MANA, 40);
	}
	else
	{
		g_Girls.UpdateStat(girl, STAT_TIREDNESS, -20);
		g_Girls.UpdateStat(girl, STAT_MANA, 30);
	}
	if (nummecs + numnurse >= 4 && DayNight == 1)	// lots of people making sure she is in good working order
	{
		g_Girls.UpdateStat(girl, STAT_CONSTITUTION, 1);
	}
	
	((nummecs > 2 && numnurse < 1) || numnurse > 1) ? message += gettext(" take ") : message += gettext(" takes ");
	message += gettext("care of her.");

	girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, DayNight);

	// Improve girl
	int libido = 1;
	if (g_Girls.HasTrait(girl, "Lesbian"))		libido += numnurse;
	if (g_Girls.HasTrait(girl, "Masochist"))	libido += 1;
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))	libido += 2;
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	return false;
}
