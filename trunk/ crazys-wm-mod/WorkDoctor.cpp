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
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;

bool cJobManager::WorkDoctor(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";
	int tex = g_Dice%4;

	g_Girls.AddTiredness(girl);

	// not for doctor
	g_Girls.UnequipCombat(girl);

	if(DayNight)
		girl->m_DayJob = JOB_DOCTOR;
	else
		girl->m_NightJob = JOB_DOCTOR;

	girl->m_Pay += 100;
	message += gettext("She worked as a doctor.");
	
	int roll = g_Dice%100;

	if(roll <= 25) {
		message += gettext(" She had a pleasant time working.");
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKDOCTOR, +1, true);
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
	girl->m_Pay += 10 + g_Dice%roll_max;

	// Improve stats
	int xp = 15, skill = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))
	{
		skill += 1;
		xp += 3;
	}
	else if (g_Girls.HasTrait(girl, "Slow Learner"))
	{
		skill -= 1;
		xp -= 3;
	}

	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, DayNight);

	return false;
}
