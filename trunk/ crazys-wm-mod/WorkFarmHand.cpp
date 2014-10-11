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
#include "cFarm.h"

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cFarmManager g_Farm;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;

// `J` Farm Job - Staff
bool cJobManager::WorkFarmHand(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";
	string girlName = girl->m_Realname;
	stringstream ss;
	cConfig cfg;

	g_Girls.UnequipCombat(girl);	// put that shit away
	
	int CleanAmt = ((g_Girls.GetSkill(girl, SKILL_SERVICE) / 10) + 5) * 10;
	int enjoyC = 0, enjoyF = 0;
	int wages = 0;
	int jobperformance = 0;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();
	bool playtime = false;

	message = girlName + gettext(" worked cleaning and repairing the farm.\n\n");
	
	if (roll_a <= 10 && g_Girls.DisobeyCheck(girl, ACTION_WORKFARM, brothel))
	{
		message = girl->m_Realname + gettext(" refused to work on the farm.");
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else if (roll_a <= 10)
	{
		enjoyC -= g_Dice % 3;
		enjoyF -= g_Dice % 3;
		CleanAmt = int(CleanAmt * 0.8);
		/* */if (roll_b < 30)	message += gettext("She spilled a bucket of something unpleasant all over herself.");
		else if (roll_b < 60)	message += gettext("She stepped in something unpleasant.");
		else /*            */	message += gettext("She did not like working on the farm today.");
	}
	else if (roll_a <= 20)
	{
		enjoyC += g_Dice % 3;
		enjoyF += g_Dice % 3;
		CleanAmt = int(CleanAmt * 1.1);
		/* */if (roll_b < 50)	message += gettext("She cleaned the building while humming a pleasant tune.");
		else /*            */	message += gettext("She had a great time working today.");
	}
	else
	{
		enjoyC += g_Dice % 2;
		enjoyF += g_Dice % 2;
		message += gettext("The shift passed uneventfully.");
	}

	if (brothel->m_Filthiness < CleanAmt / 2) playtime = true;
	brothel->m_Filthiness -= CleanAmt;
	ss << gettext("\n\nCleanliness rating improved by ") << CleanAmt;
	if (playtime)	// `J` needs more variation
	{
		ss << "\n\n" << girlName << " finished her cleaning early so she ";
		if (DayNight == SHIFT_DAY && roll_c % 3 == 1)	// 33% chance she will watch the sunset when working day shift
		{
			ss << "sat beneath an oak tree and watched the sunset.";
		}
		else if (roll_c < 25)
		{
			ss << "played with the baby animals a bit.";
			g_Girls.UpdateSkill(girl, SKILL_ANIMALHANDLING, (g_Dice % 2)+1);
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
		}
		g_Girls.UpdateStat(girl, STAT_HAPPINESS, (g_Dice % 4) + 2);
	}
	message += ss.str();

	g_Girls.UpdateEnjoyment(girl, ACTION_WORKFARM, enjoyF, true);
	g_Girls.UpdateEnjoyment(girl, ACTION_WORKCLEANING, enjoyC, true);
	girl->m_Events.AddMessage(message, IMGTYPE_MAID, DayNight);


	wages = CleanAmt; // `J` Pay her based on how much she cleaned
	if (girl->is_slave() && !cfg.initial.slave_pay_outofpocket()) wages = 0;	// You own her so you don't have to pay her.
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
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Clumsy", 30, ACTION_WORKCLEANING, "It took her spilling hundreds of buckets, and just as many reprimands, but " + girl->m_Realname + " has finally stopped being so Clumsy.", DayNight != 0);

	return false;
}