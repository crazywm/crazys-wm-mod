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

// `J` Job Clinic - Surgery
bool cJobManager::WorkHealing(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_GENERAL;
	stringstream ss; string girlName = girl->m_Realname;
	g_Girls.UnequipCombat(girl);	// not for patients

	if (girl->has_trait( "Construct"))
	{
		ss << girlName << " has no biological parts so she was sent to the repair shop.";
		if (Day0Night1 == SHIFT_DAY) girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		if (girl->m_DayJob == JOB_GETHEALING)	girl->m_DayJob = JOB_GETREPAIRS;
		if (girl->m_NightJob == JOB_GETHEALING)	girl->m_NightJob = JOB_GETREPAIRS;
		return false;	// not refusing
	}

	int numdocs = g_Clinic.GetNumGirlsOnJob(0, JOB_DOCTOR, Day0Night1);
	int numnurse = g_Clinic.GetNumGirlsOnJob(0, JOB_NURSE, Day0Night1);

	// `J` base recovery copied freetime recovery
	int health = 10 + (girl->constitution() / 10);
	int tiredness = 10 + g_Dice % 21;	// build up as positive then apply as negative
	int happy = 10 + g_Dice % 11;
	int mana = 5 + (girl->magic() / 5);
	int libido = (girl->has_trait( "Nymphomaniac") ? 15 : 5);

	if (numdocs + numnurse < 1)
	{
		ss << "You don't have any Doctors or Nurses on duty so " << girlName << " just rests in a hospital bed.";
	}
	else
	{
		ss << girlName << " rests while the";
	}
	if (numdocs > 0)
	{
		ss << " Doctor" << (numdocs > 1 ? "s" : "");
		if (girl->has_trait( "Half-Construct") && girl->m_DayJob == JOB_GETHEALING && girl->m_NightJob == JOB_GETHEALING)
		{	// if fixed by Doctor for both shifts.
			health += 20;	// Total 40 healing per day, heals less because Doctor only fixes living tissue.
		}
		else
		{
			health += 30;
		}
	}
	if (numdocs > 0 && numnurse > 0) { ss << " and"; }
	if (numnurse > 0)
	{
		ss << " Nurse" << (numnurse > 1 ? "s" : "");
		health += 10;
		tiredness += 10;
		happy += 10;
		mana += (girl->magic() / 5);
	}
	if (numdocs + numnurse >= 4 && g_Dice.percent(50))	// lots of people making sure she is in good working order
	{
		girl->constitution(1);
	}
	ss << (((numdocs > 1 && numnurse < 1) || numnurse > 1) ? " take" : " takes") << " care of her.";

	// Improve girl
	if (girl->has_trait( "Lesbian"))		libido += numnurse;
	if (girl->has_trait( "Masochist"))	libido += 1;
	if (girl->has_trait( "Nymphomaniac"))	libido += 2;

	girl->upd_stat(STAT_HEALTH, health, false);
	girl->upd_stat(STAT_TIREDNESS, -tiredness, false);
	girl->happiness(happy);
	girl->mana(mana);
	girl->upd_temp_stat(STAT_LIBIDO, libido);
	if (g_Dice % 10 == 0) girl->medicine(1);	// `J` she watched what the doctors and nurses were doing

	girl->exp(1);   // Just because!

	// send her to the waiting room when she is healthy
	if (girl->health() > 90 && girl->tiredness() < 10)
	{
		if (numdocs + numnurse < 1)	ss << "\n \nShe wanders out of the Clinic when she is feeling better.";
		else						ss << "\n \nShe has been released from the Clinic.";
		if (girl->m_DayJob == JOB_GETHEALING)	girl->m_DayJob = JOB_CLINICREST;
		if (girl->m_NightJob == JOB_GETHEALING)	girl->m_NightJob = JOB_CLINICREST;
		if (girl->m_DayJob == JOB_GETREPAIRS)	girl->m_DayJob = JOB_CLINICREST;
		if (girl->m_NightJob == JOB_GETREPAIRS)	girl->m_NightJob = JOB_CLINICREST;
	}

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);

	return false;
}

double cJobManager::JP_Healing(sGirl* girl, bool estimate)
{
	if (girl->has_trait( "Construct")) return -1000;
	double jobperformance = 1.0;
	jobperformance += (100 - girl->health());
	jobperformance += (100 - girl->happiness());
	jobperformance += girl->tiredness();
	return jobperformance;
}
