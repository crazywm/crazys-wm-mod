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

// `J` Centre Job - Rehab
bool cJobManager::WorkDrugCounselor(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";

	g_Girls.AddTiredness(girl);

	// not for doctor
	g_Girls.UnequipCombat(girl);

	girl->m_DayJob = girl->m_NightJob = JOB_DRUGCOUNSELOR;	// it is a full time job

	int wages = 25;
	message += gettext("She worked as a drug counselor.");
	
	int roll = g_Dice%100;

	if (roll <= 25) {
		message += gettext(" She had a pleasant time working.");
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKCOUNSELOR, +1, true);
	}
	else if (roll >= 90) {
		message += gettext(" The addicts hasseled her.");
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKCOUNSELOR, -3, true);
	}
	else
	{
		message += gettext(" Otherwise, the shift passed uneventfully.");
	}

/*
 *	work out the pay between the house and the girl
 *
 *	the original calc took the average of beauty and charisma and halved it
 */

	int roll_max = girl->spirit() + girl->intelligence();
	roll_max /= 4;
	wages += 10 + g_Dice%roll_max;
	wages += 5 * g_Centre.GetNumGirlsOnJob(0, JOB_REHAB, DayNight);	// `J` pay her 5 for each patient you send to her
	girl->m_Pay = wages;

	// Improve stats
	int xp = 15, skill = 2, libido = 1;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateStat(girl, STAT_CHARISMA, skill);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, skill);
	girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, DayNight);

	//gain traits
	g_Girls.PossiblyGainNewTrait(girl, "Charismatic", 60, ACTION_WORKCOUNSELOR, "Dealing with patients and talking with them about their problems has made " + girl->m_Realname + " more Charismatic.", DayNight != 0);

	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Nervous", 20, ACTION_WORKCOUNSELOR, girl->m_Realname + " seems to finally be getting over her shyness. She's not always so Nervous anymore.", DayNight != 0);

	return false;
}
