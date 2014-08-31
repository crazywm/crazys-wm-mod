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
extern cGold g_Gold;

bool cJobManager::WorkIntern(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	if (g_Girls.HasTrait(girl, "AIDS"))
	{
		stringstream ss;
		ss << "Health laws prohibit anyone with AIDS from working in the Medical profession so " <<
			girl->m_Realname << " was sent to the waiting room.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		girl->m_DayJob = girl->m_NightJob = JOB_CLINICREST;
		return true;
	}

	cTariff tariff;
	string message = "";

	if(Preprocessing(ACTION_WORKNURSE, girl, brothel, DayNight, summary, message))
		return true;
	stringstream ss;
	ss.str(message);

	double roll_a = g_Dice % 100; //this is used to determine gain amount
	int skill = 0;
	     if (roll_a <= 5){ skill = 5; }
	else if (roll_a <= 15){ skill = 4; }
	else if (roll_a <= 30){ skill = 3; }
	else if (roll_a <= 60){ skill = 2; }
	else                  { skill = 1; }
	if (g_Girls.HasTrait(girl, "Quick Learner"))	{ skill += 1; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; }

	ss << gettext("She trains to become a nurse for the day.\n\n");
	int train = 0;
	int tmed = g_Girls.GetSkill(girl, SKILL_MEDICINE);		// train = 1
	int tint = g_Girls.GetStat(girl, STAT_INTELLIGENCE);		// train = 2
	int tcha = g_Girls.GetStat(girl, STAT_CHARISMA);	// train = 3
	double roll_b = g_Dice % 100;
	do{		// `J` New method of selecting what job to do
		     if (roll_b < 30 && tmed < 100)	train = 1;
		else if (roll_b < 60 && tint < 100)	train = 2;
		else if (roll_b < 80 && tcha < 100)	train = 3;
		roll_b -= 30;
	} while (train == 0 && roll_b > 0);

	if (train == 0)	// no training today
	{
		ss << gettext("There is nothing more she can learn here so she takes the rest of the day off.");
		girl->m_NightJob = girl->m_DayJob = JOB_CLINICREST;
	}

	if (train == 1)
	{
		ss << gettext("She learns how to work with medicine better.\n");
		ss << gettext("She managed to gain ") << skill << gettext(" medicine skill.\n\n");
		g_Girls.UpdateSkill(girl, SKILL_MEDICINE, skill);
	}
	if (train == 2)
	{
		ss << gettext("She got smarter today.\n");
		ss << gettext("She managed to gain ") << skill << gettext(" Intelligence.\n\n");
		g_Girls.UpdateStat(girl, STAT_INTELLIGENCE, skill);
	}
	if (train == 3)
	{
		ss << gettext("She got more charismatic today.\n\n");
		ss << gettext("She managed to gain ") << skill << gettext(" Charisma.\n\n");
		g_Girls.UpdateStat(girl, STAT_CHARISMA, skill);
	}
	
	int roll = g_Dice%100;
	//enjoyed the work or not
	if (roll <= 5)
	{
		message += " \nSome of the patrons abused her during the shift.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKNURSE, -1, true);
	}
	else if (roll <= 25) {
		message += " \nShe had a pleasant time working.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKNURSE, +3, true);
	}
	else
	{
		message += " \nOtherwise, the shift passed uneventfully.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKNURSE, +1, true);
	}

	
		
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, DayNight);
	girl->m_Pay = 25 + (skill*5); // `J` Pay her more if she learns more

	// Improve stats
	int xp = 5 + skill, libido = int(1 + skill/2);

	if (g_Girls.HasTrait(girl, "Quick Learner"))
	{
		xp += 2;
	}
	else if (g_Girls.HasTrait(girl, "Slow Learner"))
	{
		xp -= 2;
	}
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))
		libido += 2;

	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	//gain traits
	g_Girls.PossiblyGainNewTrait(girl, "Charismatic", 60, ACTION_WORKNURSE, "Dealing with patients and talking with them about their problems has made " + girl->m_Realname + " more Charismatic.", DayNight != 0);

	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Nervous", 20, ACTION_WORKNURSE, girl->m_Realname + " seems to finally be getting over her shyness. She's not always so Nervous anymore.", DayNight != 0);

	return false;
}
