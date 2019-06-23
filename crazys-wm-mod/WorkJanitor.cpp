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
#include "cCustomers.h"
#include "cGangs.h"
#include "cGold.h"
#include "cInventory.h"
#include "cJobManager.h"
#include "CLog.h"
#include "cMessageBox.h"
#include "cRival.h"
#include "cRng.h"
#include "cTariff.h"
#include "cTrainable.h"
#include "libintl.h"
#include "sConfig.h"
#include <sstream>
#include "cBrothel.h"
#include "cClinic.h"

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cClinicManager g_Clinic;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;

// `J` Job Clinic - Staff - job_is_cleaning
bool cJobManager::WorkJanitor(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKCLEANING;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();
	if (roll_a <= 50 && girl->disobey_check(actiontype, brothel))
	{
		ss << " refused to clean the Clinic.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked cleaning the Clinic.\n \n";

	g_Girls.UnequipCombat(girl);	// put that shit away

	double CleanAmt = JP_Janitor(girl, false);
	int enjoy = 0;
	int wages = 0;
	int tips = 0;
	int imagetype = IMGTYPE_MAID;
	int msgtype = Day0Night1;
	bool playtime = false;

	if (roll_a <= 10)
	{
		enjoy -= g_Dice % 3 + 1;
		CleanAmt *= 0.8;
		if (roll_b < 50)	ss << "She spilled a bucket of something unpleasant all over herself.";
		else				ss << "She did not like cleaning the Clinic today.";
	}
	else if (roll_a >= 90)
	{
		enjoy += g_Dice % 3 + 1;
		CleanAmt *= 1.1;
		if (roll_b < 50)	ss << "She cleaned the building while humming a pleasant tune.";
		else				ss << "She had a great time working today.";
	}
	else
	{
		enjoy += g_Dice % 2;
		ss << "The shift passed uneventfully.";
	}
	ss << "\n \n";

	// slave girls not being paid for a job that normally you would pay directly for do less work
	if ((girl->is_slave() && !cfg.initial.slave_pay_outofpocket()))
	{
		CleanAmt *= 0.9;
		wages = 0;
	}
	else
	{
		wages = min(30, int(30 + (CleanAmt / 10))); // `J` Pay her based on how much she cleaned
	}

	// `J` if she can clean more than is needed, she has a little free time after her shift
	if (brothel->m_Filthiness < CleanAmt / 2) playtime = true;
	ss << "\n \nCleanliness rating improved by " << (int)CleanAmt;
	if (playtime)	// `J` needs more variation
	{
		ss << "\n \n" << girlName << " finished her cleaning early so she ";
		if (girl->is_pregnant() && girl->health() < 90)
		{
			ss << "got a quick checkup and made sure her unborn baby was doing OK.";
			girl->health(10);
			girl->happiness((g_Dice % 4) + 2);
		}
		else if (girl->health() < 80)
		{
			ss << "got a quick checkup.";
			girl->health(10);
		}
		else if (girl->is_pregnant() || g_Dice.percent(40))
		{
			ss << "hung out in the maternity ward watching the babies.";
			girl->happiness((g_Dice % 6) - 2);
		}
		else if (girl->tiredness() > 50 && brothel->free_rooms() > 10)
		{
			ss << "found an empty room and took a nap.";
			girl->tiredness(-((g_Dice % 10) + 5));
		}
		else
		{
			int d = g_Clinic.GetNumGirlsOnJob(brothel->m_id, JOB_DOCTOR, Day0Night1);
			int n = g_Clinic.GetNumGirlsOnJob(brothel->m_id, JOB_NURSE, Day0Night1);
			ss << "watched the ";
			if (d + n < 1)
			{
				ss << "people wander about the Clinic.";
				girl->happiness((g_Dice % 3) - 1);
			}
			else
			{
				if (d > 0)			ss << "doctor" << (d > 1 ? "s" : "");
				if (d > 0 && n > 0)	ss << " and ";
				if (n > 0)			ss << "nurse" << (n > 1 ? "s" : "");
				ss << " do their job" << (d + n > 1 ? "s" : "");
				girl->medicine((g_Dice % 2));
			}
		}
	}

	// do all the output
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_MAID, Day0Night1);
	brothel->m_Filthiness -= (int)CleanAmt;
	// Money
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);

	// Improve girl
	int xp = 5, libido = 1, skill = 3;
	if (enjoy > 1)										{ xp += 1; skill += 1; }
	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }

	girl->exp((g_Dice % xp) + 2);
	girl->service((g_Dice % skill) + 2);
	girl->upd_temp_stat(STAT_LIBIDO, libido);

	girl->upd_Enjoyment(actiontype, enjoy);
	// Gain Traits
	if (g_Dice.percent(girl->service()))
		g_Girls.PossiblyGainNewTrait(girl, "Maid", 70, actiontype, girlName + " has cleaned enough that she could work professionally as a Maid anywhere.", Day0Night1);
	// Lose Traits
	if (g_Dice.percent(girl->service()))
		g_Girls.PossiblyLoseExistingTrait(girl, "Clumsy", 30, actiontype, "It took her spilling hundreds of buckets, and just as many reprimands, but " + girl->m_Realname + " has finally stopped being so Clumsy.", Day0Night1);

	return false;
}

double cJobManager::JP_Janitor(sGirl* girl, bool estimate)// not used
{
	return JP_Cleaning(girl, estimate);		// just use cleaning so there is 1 less thing to edit
}
