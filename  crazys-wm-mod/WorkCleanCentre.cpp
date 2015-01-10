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

// `J` Centre Job - General - job_is_cleaning
bool cJobManager::WorkCleanCentre(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	stringstream ss; string girlName = girl->m_Realname;
	cConfig cfg;

	g_Girls.UnequipCombat(girl);	// put that shit away
	
	int CleanAmt = ((g_Girls.GetSkill(girl, SKILL_SERVICE) / 10) + 5) * 10;
	int enjoy = 0;
	int wages = 0;
	int jobperformance = 0;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();
	bool playtime = false;

	ss << girlName << " worked cleaning the centre.\n\n";

	if (roll_a <= 50 && g_Girls.DisobeyCheck(girl, ACTION_WORKCLEANING, brothel))
	{
		ss << "She refused to clean the centre.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
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
	if (g_Girls.HasTrait(girl, "Sharp - Eyed"))				CleanAmt += 1;
	if (g_Girls.HasTrait(girl, "Giant"))					CleanAmt += 2;
	if (g_Girls.HasTrait(girl, "Prehensile Tail"))			CleanAmt += 3;



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

	if (roll_a <= 10)
	{
		enjoy -= g_Dice % 3 + 1;
		CleanAmt = int(CleanAmt * 0.8);
		if (roll_b < 50)	ss << "She spilled a bucket of something unpleasant all over herself.";
		else				ss << "She did not like cleaning the centre today.";
	
	}
	else if (roll_a >= 90)
	{
		enjoy += g_Dice % 3 + 1;
		CleanAmt = int(CleanAmt * 1.1);
		if (roll_b < 50)	ss << "She cleaned the building while humming a pleasant tune.";
		else				ss << "She had a great time working today.";
	}
	else
	{
		enjoy += g_Dice % 2;
		ss << "The shift passed uneventfully.";
	}
	ss << "\n\n";

	// slave girls not being paid for a job that normally you would pay directly for do less work
	if ((girl->is_slave() && !cfg.initial.slave_pay_outofpocket()))
	{
		CleanAmt = int(CleanAmt * 0.9);
		wages = 0;
	}
	else
	{
		wages = CleanAmt; // `J` Pay her based on how much she cleaned
	}

	// `J` if she can clean more than is needed, she has a little free time after her shift
	if (brothel->m_Filthiness < CleanAmt / 2) playtime = true;
	ss << gettext("\n\nCleanliness rating improved by ") << CleanAmt;
	if (playtime)	// `J` needs more variation
	{
		ss << "\n\n" << girlName << " finished her cleaning early so she played with the children a bit.";
		g_Girls.UpdateStat(girl, STAT_HAPPINESS, (g_Dice % 5) + 3);
	}

	// do all the output
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_MAID, Day0Night1);
	brothel->m_Filthiness -= CleanAmt;
	girl->m_Pay = wages;



	

	// Improve girl
	int xp = 5, libido = 1, skill = 3;
	if (enjoy > 1)										{ xp += 1; skill += 1; }
	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_EXP, (g_Dice % xp) + 2);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, (g_Dice % skill) + 2);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	g_Girls.UpdateEnjoyment(girl, ACTION_WORKCLEANING, enjoy, true);
	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Clumsy", 30, ACTION_WORKCLEANING, "It took her spilling hundreds of buckets, and just as many reprimands, but " + girl->m_Realname + " has finally stopped being so Clumsy.", Day0Night1 == SHIFT_NIGHT);

	return false;
}