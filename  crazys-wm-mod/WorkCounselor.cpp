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

// `J` Job Centre - Rehab_Job - Full_Time_Job
bool cJobManager::WorkCounselor(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{	// `J` changed "Drug Counselor" to just "Counselor" so she can help the other therapy patients

	int actiontype = ACTION_WORKCOUNSELOR;
	bool SkipDisobey = (summary == "SkipDisobey");
	girl->m_DayJob = girl->m_NightJob = JOB_COUNSELOR;	// it is a full time job

	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = g_Dice.d100();
	if (!SkipDisobey)	// `J` skip the disobey check because it has already been done in the building flow
	{
		if (roll_a <= 50 && girl->disobey_check(actiontype, brothel))
		{
			ss << " refused to counsel anyone.";
			girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
			girl->upd_Enjoyment(ACTION_WORKREHAB, -1);
			return true;
		}
	}
	ss << " counceled patients.\n \n";



	g_Girls.UnequipCombat(girl);	// not for doctor

	int wages = 25;
	int tips = 0;
	int enjoy = 0;

	/* */if (roll_a <= 10)	{ enjoy -= g_Dice % 3 + 1;	ss << "The addicts hasseled her."; }
	else if (roll_a >= 90)	{ enjoy += g_Dice % 3 + 1;	ss << "She had a pleasant time working."; }
	else /*             */	{ enjoy += g_Dice % 2;		ss << "Otherwise, the shift passed uneventfully."; }

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_TEACHER, Day0Night1);

	int rehabers = g_Centre.GetNumGirlsOnJob(0, JOB_REHAB, Day0Night1);
	// work out the pay between the house and the girl
	int roll_max = girl->spirit() + girl->intelligence();
	roll_max /= 4;
	wages += 10 + g_Dice%roll_max;
	wages += 5 * rehabers;	// `J` pay her 5 for each patient you send to her
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);

	// Improve stats
	int xp = 5 + (rehabers / 2), skill = 2 + (rehabers / 2), libido = 1;

	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }

	girl->exp(xp);
	girl->charisma((g_Dice%skill) + 1);
	girl->service((g_Dice%skill) + 1);

	girl->upd_Enjoyment(actiontype, enjoy);
	//gain traits
	g_Girls.PossiblyGainNewTrait(girl, "Charismatic", 60, actiontype, "Dealing with patients and talking with them about their problems has made " + girl->m_Realname + " more Charismatic.", Day0Night1 == SHIFT_NIGHT);
	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Nervous", 30, actiontype, girl->m_Realname + " seems to finally be getting over her shyness. She's not always so Nervous anymore.", Day0Night1 == SHIFT_NIGHT);

	return false;
}

double cJobManager::JP_Counselor(sGirl* girl, bool estimate)// not used
{
	double jobperformance = 0.0;
	if (estimate)// for third detail string
	{
		jobperformance += girl->charisma() +
			(girl->intelligence() / 2) +
			(girl->confidence() / 4) +
			(girl->morality() / 4) +
			girl->level();
	}
	else// for the actual check
	{
	}
	return jobperformance;
}
