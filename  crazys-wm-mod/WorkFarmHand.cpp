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
#include "cRng.h"
#include "CLog.h"
#include "cMessageBox.h"
#include "cGold.h"
#include "cBrothel.h"
#include "cFarm.h"

extern CLog g_LogFile;
extern cMessageQue g_MessageQue;
extern cRng g_Dice;
extern cGold g_Gold;
extern cBrothelManager g_Brothels;
extern cFarmManager g_Farm;

// `J` Job Farm - Staff - job_is_cleaning
bool cJobManager::WorkFarmHand(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKFARM; int actiontype2 = ACTION_WORKCLEANING;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();
	if (roll_a <= 50 && (girl->disobey_check(actiontype, brothel) || girl->disobey_check(actiontype2, brothel)))
	{
		ss << " refused to work on the farm.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked cleaning and repairing the farm.\n \n";

	g_Girls.UnequipCombat(girl);	// put that shit away

	double CleanAmt = JP_FarmHand(girl, false);
	int enjoy = 0, enjoyC = 0, enjoyF = 0;
	int wages = 0;
	int tips = 0;
	int imagetype = IMGTYPE_MAID;
	int msgtype = Day0Night1;
	bool playtime = false;

	if (roll_a <= 10)
	{
		enjoyC -= g_Dice % 3; enjoyF -= g_Dice % 3;
		CleanAmt = CleanAmt * 0.8;
		/* */if (roll_b < 30)	ss << "She spilled a bucket of something unpleasant all over herself.";
		else if (roll_b < 60)	ss << "She stepped in something unpleasant.";
		else /*            */	ss << "She did not like working on the farm today.";
	}
	else if (roll_a >= 90)
	{
		enjoyC += g_Dice % 3; enjoyF += g_Dice % 3;
		CleanAmt = CleanAmt * 1.1;
		/* */if (roll_b < 50)	ss << "She cleaned the building while humming a pleasant tune.";
		else /*            */	ss << "She had a great time working today.";
	}
	else
	{
		enjoyC += g_Dice % 2; enjoyF += g_Dice % 2;
		ss << "The shift passed uneventfully.";
	}
	ss << "\n \n";

	// slave girls not being paid for a job that normally you would pay directly for do less work
	if ((girl->is_slave() && !cfg.initial.slave_pay_outofpocket()))
	{
		CleanAmt = CleanAmt * 0.9;
		wages = 0;
	}
	else
	{
		wages = int(CleanAmt); // `J` Pay her based on how much she cleaned
	}

	// `J` if she can clean more than is needed, she has a little free time after her shift
	if (brothel->m_Filthiness < CleanAmt / 2) playtime = true;
	ss << "\n \nCleanliness rating improved by " << int(CleanAmt);
	if (playtime)	// `J` needs more variation
	{
		ss << "\n \n" << girlName << " finished her cleaning early so she ";
		if (Day0Night1 == SHIFT_DAY && roll_c % 3 == 1)	// 33% chance she will watch the sunset when working day shift
		{
			ss << "sat beneath an oak tree and watched the sunset.";
			girl->tiredness(-((g_Dice % 5) + 2));
		}
		else if (roll_c < 25)
		{
			ss << "played with the baby animals a bit.";
			girl->animalhandling((g_Dice % 2) + 1);
		}
		else if (roll_c < 50)
		{
			ss << "played in the dirt a bit.";
			girl->farming((g_Dice % 2));
		}
		else
		{
			ss << "sat in a rocking chair on the farm house front porch whittling.";
			girl->crafting((g_Dice % 3));
			girl->tiredness(-(g_Dice % 3));
		}
		girl->happiness((g_Dice % 4) + 2);
	}

#if 0

	// `J` Farm Bookmark - adding in items that can be created in the farm















#endif




	// do all the output
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_MAID, Day0Night1);
	brothel->m_Filthiness -= int(CleanAmt);

	// Money
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);

	// Improve girl
	int xp = 5, libido = 1, skill = 3;

	if (enjoyC + enjoyF > 2)							{ xp += 1; skill += 1; }
	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }

	girl->exp((g_Dice % xp) + 1);
	girl->upd_temp_stat(STAT_LIBIDO, libido);

	// primary (+2 for single or +1 for multiple)
	girl->service((g_Dice % skill));
	// secondary (-1 for one then -2 for others)
	girl->crafting(max(0, (g_Dice % skill) - 1));
	girl->farming(max(0, (g_Dice % skill) - 2));
	girl->strength(max(0, (g_Dice % skill) - 2));

	girl->upd_Enjoyment(actiontype, enjoyF);
	girl->upd_Enjoyment(actiontype2, enjoyC);
	// Gain Traits
	if (g_Dice.percent(girl->service()))
		g_Girls.PossiblyGainNewTrait(girl, "Maid", 90, actiontype2, girlName + " has cleaned enough that she could work professionally as a Maid anywhere.", Day0Night1);
	// Lose Traits
	if (g_Dice.percent(girl->service()))
		g_Girls.PossiblyLoseExistingTrait(girl, "Clumsy", 30, actiontype2, "It took her spilling hundreds of buckets, and just as many reprimands, but " + girl->m_Realname + " has finally stopped being so Clumsy.", Day0Night1);

	return false;
}


double cJobManager::JP_FarmHand(sGirl* girl, bool estimate)
{
	return JP_Cleaning(girl, estimate);		// just use cleaning so there is 1 less thing to edit
}
