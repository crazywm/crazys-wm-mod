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

// `J` Clinic Job - Staff
bool cJobManager::WorkDoctor(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	bool SkipDisobey = (summary == "SkipDisobey");
	stringstream ss; string girlName = girl->m_Realname;

	if (g_Girls.HasTrait(girl, "AIDS"))
	{
		ss << "Health laws prohibit anyone with AIDS from working in the Medical profession so " << girlName << " was sent to the waiting room.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		girl->m_DayJob = girl->m_NightJob = JOB_CLINICREST;
		return false;
	}
	if (girl->medicine() < 50 || girl->intelligence() < 50)
	{
		ss << girlName << " does not have enough training to work as a Doctor. She has been reassigned to Internship so she can learn what she needs.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		girl->m_DayJob = girl->m_NightJob = JOB_INTERN;
		return false;
	}
	if (girl->is_slave())
	{
		ss << "Slaves are not allowed to be Doctors so " << girlName << " was reassigned to being a Nurse.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		girl->m_DayJob = girl->m_NightJob = JOB_NURSE;
		return false;
	}

	if (!SkipDisobey)	// `J` skip the disobey check because it has already been done in the building flow
	{
		if (Preprocessing(ACTION_WORKDOCTOR, girl, brothel, Day0Night1, summary, ss.str())) return true;
	}
	cConfig cfg;

	int enjoy = 0, wages = 100;

	// this will be added to the clinic's code eventually - for now it is just used for her pay
	int patients = 0;			// `J` how many patients the Doctor can see in a shift

	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the patients!

	// Doctor is a full time job now
	girl->m_DayJob = girl->m_NightJob = JOB_DOCTOR;

	ss << gettext("She worked as a Doctor.\n");

	int roll = g_Dice.d100();
	int jobperformance = (g_Girls.GetStat(girl, STAT_INTELLIGENCE) +
		g_Girls.GetSkill(girl, SKILL_MEDICINE) +
		g_Girls.GetStat(girl, STAT_LEVEL) / 5);

	if (g_Girls.HasTrait(girl, "Charismatic"))		jobperformance += 20;
	if (g_Girls.HasTrait(girl, "Sexy Air"))			jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Cool Person"))		jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Cute"))				jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Charming"))			jobperformance += 15;
	if (g_Girls.HasTrait(girl, "Nerd"))				jobperformance += 30;
	if (g_Girls.HasTrait(girl, "Quick Learner"))	jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Psychic"))			jobperformance += 20;	// Don't have to ask "Where does it hurt?"

	//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))		jobperformance -= 50;
	if (g_Girls.HasTrait(girl, "Clumsy"))			jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Aggressive"))		jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Nervous"))			jobperformance -= 50;
	if (g_Girls.HasTrait(girl, "Retarded"))			jobperformance -= 100;
	if (g_Girls.HasTrait(girl, "Meek"))				jobperformance -= 20;




	//enjoyed the work or not
	if (roll <= 10)
	{
		enjoy -= g_Dice % 3 + 1;
		jobperformance = int(jobperformance * 0.9);
		ss << "Some of the patients abused her during the shift.\n";
	}
	else if (roll >= 90)
	{
		enjoy += g_Dice % 3 + 1;
		jobperformance = int(jobperformance * 1.1);
		ss << "She had a pleasant time working.\n";
	}
	else
	{
		enjoy += g_Dice % 2;
		ss << "Otherwise, the shift passed uneventfully.\n";
	}

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);
	patients = jobperformance / 10;		// `J` 1 patient per 10 point of performance
	g_Clinic.m_Doctor_Patient_Time += patients;

	/* `J` this will be a place holder until a better payment system gets done
	*  this does not take into account any of your girls in surgery  
	*/
	int earned = 0;
	for (int i = 0; i < patients; i++)
	{
		earned += g_Dice % 150 + 50; // 50-200 gold per customer
	}
	brothel->m_Finance.clinic_income(earned);
	ss.str("");
	ss << girlName << " earned " << earned << " gold from taking care of " << patients << " patients.\n";
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);

	girl->m_Pay += wages + (patients * 10);

	// Improve stats
	int xp = 10 + (patients/2), libido = 1, skill = 1 + (patients / 3);

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }
	if (g_Girls.HasTrait(girl, "Lesbian"))				{ libido += patients / 2; }

	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateStat(girl, STAT_INTELLIGENCE, skill);
	g_Girls.UpdateSkill(girl, SKILL_MEDICINE, skill);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, 1);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);


	g_Girls.UpdateEnjoyment(girl, ACTION_WORKDOCTOR, enjoy, true);

	return false;
}
