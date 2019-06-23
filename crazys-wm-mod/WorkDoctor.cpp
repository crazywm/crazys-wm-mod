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

// `J` Job Clinic - Staff
bool cJobManager::WorkDoctor(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKDOCTOR;
	bool SkipDisobey = (summary == "SkipDisobey");
	stringstream ss; string girlName = girl->m_Realname;
	if (girl->has_trait( "AIDS"))
	{
		ss << "Health laws prohibit anyone with AIDS from working in the Medical profession so " << girlName << " was sent to the waiting room.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		girl->m_PrevDayJob = girl->m_PrevNightJob = girl->m_DayJob = girl->m_NightJob = JOB_CLINICREST;
		return false;
	}
	if (girl->is_slave())
	{
		ss << "Slaves are not allowed to be Doctors so " << girlName << " was reassigned to being a Nurse.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		girl->m_PrevDayJob = girl->m_PrevNightJob = girl->m_DayJob = girl->m_NightJob = JOB_NURSE;
		return false;
	}
	ss << girlName;
	if (girl->medicine() < 50 || girl->intelligence() < 50)
	{
		ss << " does not have enough training to work as a Doctor. She has been reassigned to Internship so she can learn what she needs.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		girl->m_PrevDayJob = girl->m_PrevNightJob = girl->m_DayJob = girl->m_NightJob = JOB_INTERN;
		return false;
	}
	if (!SkipDisobey)	// `J` skip the disobey check because it has already been done in the building flow
	{
		if (girl->disobey_check(actiontype, brothel))			// they refuse to work
		{
			ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
			girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
			return true;
		}
	}
	ss << " worked as a Doctor.\n";


	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the patients!

	int enjoy = 0, wages = 100, tips = 0;

	// this will be added to the clinic's code eventually - for now it is just used for her pay
	int patients = 0;			// `J` how many patients the Doctor can see in a shift


	// Doctor is a full time job now
	girl->m_DayJob = girl->m_NightJob = JOB_DOCTOR;


	double jobperformance = JP_Doctor(girl, false);

	//enjoyed the work or not
	int roll = g_Dice.d100();
	if (roll <= 10)
	{
		enjoy -= g_Dice % 3 + 1;
		jobperformance *= 0.9;
		ss << "Some of the patients abused her during the shift.\n";
	}
	else if (roll >= 90)
	{
		enjoy += g_Dice % 3 + 1;
		jobperformance *= 1.1;
		ss << "She had a pleasant time working.\n";
	}
	else
	{
		enjoy += g_Dice % 2;
		ss << "Otherwise, the shift passed uneventfully.\n";
	}

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);
	patients += (int)(jobperformance / 10);		// `J` 1 patient per 10 point of performance
	g_Clinic.m_Doctor_Patient_Time += patients;

	/* `J` this will be a place holder until a better payment system gets done
	*  this does not take into account any of your girls in surgery
	*/
	int earned = 0;
	for (int i = 0; i < patients; i++)
	{
		earned += g_Dice % 50 + 50; // 50-100 gold per customer
	}
	brothel->m_Finance.clinic_income(earned);
	ss.str("");
	ss << girlName << " earned " << earned << " gold from taking care of " << patients << " patients.\n";
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);

	wages += (patients * 10);
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);

	// Improve stats
	int xp = 10 + (patients / 2), libido = 1, skill = 1 + (patients / 3);

	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }
	if (girl->has_trait( "Lesbian"))				{ libido += patients / 2; }

	girl->exp(xp);
	girl->intelligence(skill);
	girl->medicine(skill);
	girl->service(1);
	girl->upd_temp_stat(STAT_LIBIDO, libido);


	girl->upd_Enjoyment(actiontype, enjoy);

	return false;
}

double cJobManager::JP_Doctor(sGirl* girl, bool estimate)// not used
{
	double jobperformance =
		(girl->intelligence() +
		girl->medicine() +
		girl->level() / 5);
	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}

	if (girl->has_trait( "Charismatic"))		jobperformance += 20;
	if (girl->has_trait( "Sexy Air"))			jobperformance += 10;
	if (girl->has_trait( "Cool Person"))		jobperformance += 10;
	if (girl->has_trait( "Cute"))				jobperformance += 5;
	if (girl->has_trait( "Charming"))			jobperformance += 15;
	if (girl->has_trait( "Nerd"))				jobperformance += 30;
	if (girl->has_trait( "Quick Learner"))	jobperformance += 10;
	if (girl->has_trait( "Psychic"))			jobperformance += 20;	// Don't have to ask "Where does it hurt?"
	if (girl->has_trait( "Doctor"))			jobperformance += 50;
	if (girl->has_trait( "Goddess"))			jobperformance += 10; //might be able to heal people easier.. they are a goddess after all
	if (girl->has_trait( "Optimist"))			jobperformance += 10;
	if (girl->has_trait( "Priestess"))		jobperformance += 10;

	//bad traits
	if (girl->has_trait( "Dependant"))		jobperformance -= 50;
	if (girl->has_trait( "Clumsy"))			jobperformance -= 20;
	if (girl->has_trait( "Aggressive"))		jobperformance -= 20;
	if (girl->has_trait( "Nervous"))			jobperformance -= 50;
	if (girl->has_trait( "Retarded"))			jobperformance -= 100;
	if (girl->has_trait( "Meek"))				jobperformance -= 20;
	if (girl->has_trait( "Mind Fucked"))		jobperformance -= 50;
	if (girl->has_trait( "Pessimist"))		jobperformance -= 10;
	if (girl->has_trait( "Sadistic"))			jobperformance -= 20;
	if (girl->has_trait( "Zombie"))			jobperformance -= 100;

	if (girl->has_trait( "One Arm"))		jobperformance -= 40;
	if (girl->has_trait( "One Foot"))		jobperformance -= 40;
	if (girl->has_trait( "One Hand"))		jobperformance -= 30;
	if (girl->has_trait( "One Leg"))		jobperformance -= 60;
	if (girl->has_trait( "No Arms"))		jobperformance -= 150;
	if (girl->has_trait( "No Feet"))		jobperformance -= 60;
	if (girl->has_trait( "No Hands"))		jobperformance -= 90;
	if (girl->has_trait( "No Legs"))		jobperformance -= 150;
	if (girl->has_trait( "Blind"))		jobperformance -= 75;
	if (girl->has_trait( "Deaf"))			jobperformance -= 15;
	if (girl->has_trait( "Retarded"))		jobperformance -= 60;
	if (girl->has_trait( "Smoker"))		jobperformance -= 10;	//would need smoke breaks

	if (girl->has_trait( "Former Addict"))		jobperformance -= 25; // not good idea to let former addict around all those drugs
	if (girl->has_trait( "Alcoholic"))			jobperformance -= 25;
	if (girl->has_trait( "Fairy Dust Addict"))	jobperformance -= 25;
	if (girl->has_trait( "Shroud Addict"))		jobperformance -= 25;
	if (girl->has_trait( "Viras Blood Addict"))	jobperformance -= 25;

	return jobperformance;
}
