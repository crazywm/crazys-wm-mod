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

// `J` Farm Job - Staff - job_is_cleaning
bool cJobManager::WorkFarmHand(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKFARM;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();
	if (roll_a <= 50 && (g_Girls.DisobeyCheck(girl, actiontype, brothel) || g_Girls.DisobeyCheck(girl, ACTION_WORKCLEANING, brothel)))
	{
		ss << " refused to work on the farm.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked cleaning and repairing the farm.\n\n";

	cConfig cfg;

	g_Girls.UnequipCombat(girl);	// put that shit away

	double CleanAmt = ((g_Girls.GetSkill(girl, SKILL_SERVICE) / 10) + 5) * 10;
	CleanAmt += JP_FarmHand(girl, false);

	int enjoyC = 0, enjoyF = 0;
	int wages = 0;
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
	girl->m_Pay = wages;

	// Improve girl
	int xp = 5, libido = 1, skill = 3;
	if (enjoyC + enjoyF > 2)							{ xp += 1; skill += 1; }
	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_EXP, (g_Dice % xp) + 2);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, (g_Dice % skill));
	g_Girls.UpdateSkill(girl, SKILL_CRAFTING, (g_Dice % 2));
	g_Girls.UpdateSkill(girl, SKILL_FARMING, (g_Dice % 2));
	g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, libido);

	g_Girls.UpdateEnjoyment(girl, actiontype, enjoyF, true);
	g_Girls.UpdateEnjoyment(girl, ACTION_WORKCLEANING, enjoyC, true);
	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Clumsy", 30, ACTION_WORKCLEANING, "It took her spilling hundreds of buckets, and just as many reprimands, but " + girl->m_Realname + " has finally stopped being so Clumsy.", Day0Night1);

	return false;
}


double cJobManager::JP_FarmHand(sGirl* girl, bool estimate)
{
	double CleanAmt = 0;
	if (estimate)		// for third detail string
	{
		CleanAmt += girl->service() * 2;
	}
	if (g_Girls.HasTrait(girl, "Maid"))						CleanAmt += 20;
	if (g_Girls.HasTrait(girl, "Powerful Magic"))			CleanAmt += 10;
	if (g_Girls.HasTrait(girl, "Strong Magic"))				CleanAmt += 5;
	if (g_Girls.HasTrait(girl, "Handyman"))					CleanAmt += 5;
	if (g_Girls.HasTrait(girl, "Waitress"))					CleanAmt += 5;
	if (g_Girls.HasTrait(girl, "Agile"))					CleanAmt += 5;
	if (g_Girls.HasTrait(girl, "Fleet of Foot"))			CleanAmt += 2;
	if (g_Girls.HasTrait(girl, "Strong"))					CleanAmt += 5;
	if (g_Girls.HasTrait(girl, "Assassin"))					CleanAmt += 1;
	if (g_Girls.HasTrait(girl, "Psychic"))					CleanAmt += 2;
	if (g_Girls.HasTrait(girl, "Manly"))					CleanAmt += 1;
	if (g_Girls.HasTrait(girl, "Tomboy"))					CleanAmt += 2;
	if (g_Girls.HasTrait(girl, "Optimist"))					CleanAmt += 1;
	if (g_Girls.HasTrait(girl, "Sharp-Eyed"))				CleanAmt += 1;
	if (g_Girls.HasTrait(girl, "Giant"))					CleanAmt += 2;
	if (g_Girls.HasTrait(girl, "Prehensile Tail"))			CleanAmt += 3;
	if (g_Girls.HasTrait(girl, "Cow Girl"))					CleanAmt += 1;
	if (g_Girls.HasTrait(girl, "Equine"))					CleanAmt += 1;

	if (g_Girls.HasTrait(girl, "Blind"))					CleanAmt -= 20;
	if (g_Girls.HasTrait(girl, "Queen"))					CleanAmt -= 20;
	if (g_Girls.HasTrait(girl, "Princess"))					CleanAmt -= 10;
	if (g_Girls.HasTrait(girl, "Mind Fucked"))				CleanAmt -= 10;
	if (g_Girls.HasTrait(girl, "Bimbo"))					CleanAmt -= 5;
	if (g_Girls.HasTrait(girl, "Retarded"))					CleanAmt -= 5;
	if (g_Girls.HasTrait(girl, "Smoker"))					CleanAmt -= 1;
	if (g_Girls.HasTrait(girl, "Clumsy"))					CleanAmt -= 5;
	if (g_Girls.HasTrait(girl, "Delicate"))					CleanAmt -= 1;
	if (g_Girls.HasTrait(girl, "Elegant"))					CleanAmt -= 5;
	if (g_Girls.HasTrait(girl, "Malformed"))				CleanAmt -= 1;
	if (g_Girls.HasTrait(girl, "Massive Melons"))			CleanAmt -= 1;
	if (g_Girls.HasTrait(girl, "Abnormally Large Boobs"))	CleanAmt -= 3;
	if (g_Girls.HasTrait(girl, "Titanic Tits"))				CleanAmt -= 5;
	if (g_Girls.HasTrait(girl, "Broken Will"))				CleanAmt -= 5;
	if (g_Girls.HasTrait(girl, "Pessimist"))				CleanAmt -= 1;
	if (g_Girls.HasTrait(girl, "Meek"))						CleanAmt -= 2;
	if (g_Girls.HasTrait(girl, "Nervous"))					CleanAmt -= 2;
	if (g_Girls.HasTrait(girl, "Dependant"))				CleanAmt -= 5;
	if (g_Girls.HasTrait(girl, "Bad Eyesight"))				CleanAmt -= 5;

	return CleanAmt;
}
