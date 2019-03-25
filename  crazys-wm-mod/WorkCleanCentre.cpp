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
#include <sstream>
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
#include "cBrothel.h"
#include "cCentre.h"

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cCentreManager g_Centre;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;

// `J` Job Centre - General - job_is_cleaning
bool cJobManager::WorkCleanCentre(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKCLEANING;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();
	if (roll_a <= 50 && girl->disobey_check(actiontype, brothel))
	{
		ss << " refused to clean the Centre.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked cleaning the Centre.\n \n";

	g_Girls.UnequipCombat(girl);	// put that shit away

	double CleanAmt = JP_CleanCentre(girl, false);
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
		else				ss << "She did not like cleaning the Centre today.";
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

	// slave girls not being paid for a job that normally you would pay directly for, do less work
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
		if (roll_a < 20 && roll_b < 60 && roll_c < 50)
		{
			ss << "\n \n" << girlName << " finished her cleaning early so took a long bath to clean herself off.";
			girl->happiness((g_Dice % 3) + 1);
			girl->tiredness(-(g_Dice % 3));
			imagetype = IMGTYPE_BATH;
		}
		else if (roll_c < 50)
		{
			ss << "\n \n" << girlName << " finished her cleaning early so she played with the children a bit.";
			girl->happiness((g_Dice % 5) + 3);
			girl->tiredness(g_Dice % 3);
			girl->morality(g_Dice % 2);
		}
		else
		{
			ss << "\n \n" << girlName << " finished her cleaning early so she took nap.";
			girl->happiness(g_Dice % 3);
			girl->tiredness(-1 - (g_Dice % 10));
			girl->morality(-(g_Dice % 2));
		}

	}

	brothel->m_Filthiness -= (int)CleanAmt;

	// `J` - Finish the shift - CleanCentre

	// Push out the turn report
	girl->m_Events.AddMessage(ss.str(), imagetype, msgtype);
	// Money
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);

	// Base Improvement and trait modifiers
	int xp = 5, libido = 1, skill = 3;
	/* */if (girl->has_trait("Quick Learner"))	{ skill += 1; xp += 3; }
	else if (girl->has_trait("Slow Learner"))	{ skill -= 1; xp -= 3; }
	/* */if (girl->has_trait("Nymphomaniac"))	{ libido += 2; }
	// EXP and Libido
	girl->exp((g_Dice % xp) + 1);
	girl->upd_temp_stat(STAT_LIBIDO, libido,false);


	// primary improvement (+2 for single or +1 for multiple)
	girl->service((g_Dice % skill) + 2);
	// secondary improvement (-1 for one then -2 for others)
	girl->morality(max(-1, (g_Dice % skill) - 1));		// possibly go down but mostly go up
	girl->refinement(max(-1, (g_Dice % skill) - 2));	// possibly go up or down


	// Update Enjoyment
	girl->upd_Enjoyment(actiontype, enjoy);
	// Gain Traits
	if (g_Dice.percent(girl->service()))
		g_Girls.PossiblyGainNewTrait(girl, "Maid", 70, actiontype, girlName + " has cleaned enough that she could work professionally as a Maid anywhere.", Day0Night1);
	// Lose Traits
	if (g_Dice.percent(girl->service()))
		g_Girls.PossiblyLoseExistingTrait(girl, "Clumsy", 30, actiontype, "It took her spilling hundreds of buckets, and just as many reprimands, but " + girl->m_Realname + " has finally stopped being so Clumsy.", Day0Night1);

	return false;
}

double cJobManager::JP_CleanCentre(sGirl* girl, bool estimate)// not used
{
	return JP_Cleaning(girl, estimate);		// just use cleaning so there is 1 less thing to edit
}
