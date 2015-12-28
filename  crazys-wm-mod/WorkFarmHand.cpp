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
	if (roll_a <= 50 && (g_Girls.DisobeyCheck(girl, actiontype, brothel) || g_Girls.DisobeyCheck(girl, actiontype2, brothel)))
	{
		ss << " refused to work on the farm.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked cleaning and repairing the farm.\n\n";

	

	g_Girls.UnequipCombat(girl);	// put that shit away

	double CleanAmt = ((g_Girls.GetSkill(girl, SKILL_SERVICE) / 10) + 5) * 10;
	CleanAmt += JP_FarmHand(girl, false);

	int enjoyC = 0, enjoyF = 0;
	double wages = 0, tips = 0;
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
	ss << "\n\n";

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
	ss << "\n\nCleanliness rating improved by " << int(CleanAmt);
	if (playtime)	// `J` needs more variation
	{
		ss << "\n\n" << girlName << " finished her cleaning early so she ";
		if (Day0Night1 == SHIFT_DAY && roll_c % 3 == 1)	// 33% chance she will watch the sunset when working day shift
		{
			ss << "sat beneath an oak tree and watched the sunset.";
			g_Girls.UpdateStat(girl, STAT_TIREDNESS, -((g_Dice % 5) + 2));
		}
		else if (roll_c < 25)
		{
			ss << "played with the baby animals a bit.";
			g_Girls.UpdateSkill(girl, SKILL_ANIMALHANDLING, (g_Dice % 2) + 1);
		}
		else if (roll_c < 50)
		{
			ss << "played in the dirt a bit.";
			g_Girls.UpdateSkill(girl, SKILL_FARMING, (g_Dice % 2));
		}
		else
		{
			ss << "sat in a rocking chair on the farm house front porch whittling.";
			g_Girls.UpdateSkill(girl, SKILL_CRAFTING, (g_Dice % 3));
			g_Girls.UpdateStat(girl, STAT_TIREDNESS, -(g_Dice % 3));
		}
		g_Girls.UpdateStat(girl, STAT_HAPPINESS, (g_Dice % 4) + 2);
	}

#if 0

	// `J` Farm Bookmark - adding in items that can be created in the farm















#endif




	// do all the output
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_MAID, Day0Night1);
	brothel->m_Filthiness -= int(CleanAmt);

	// Money
	if (wages < 0)	wages = 0;	girl->m_Pay = (int)wages;
	if (tips < 0)	tips = 0;	girl->m_Tips = (int)tips;

	// Improve girl
	int xp = 5, libido = 1, skill = 3;

	if (enjoyC + enjoyF > 2)							{ xp += 1; skill += 1; }
	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_EXP, (g_Dice % xp) + 1);
	g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, libido);

	// primary (+2 for single or +1 for multiple)
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, (g_Dice % skill));
	// secondary (-1 for one then -2 for others)
	g_Girls.UpdateSkill(girl, SKILL_CRAFTING, max(0, (g_Dice % skill) - 1));
	g_Girls.UpdateSkill(girl, SKILL_FARMING, max(0, (g_Dice % skill) - 2));
	g_Girls.UpdateStat(girl, STAT_STRENGTH, max(0, (g_Dice % skill) - 2));

	g_Girls.UpdateEnjoyment(girl, actiontype, enjoyF);
	g_Girls.UpdateEnjoyment(girl, actiontype2, enjoyC);
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
