/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders 
 * who meet on http://pinkpetal.co.cc
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
extern cGold g_Gold;

bool cJobManager::WorkTorturer(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	if(DayNight == SHIFT_NIGHT)  // Do this only once a day
		return false;

	string message = "";
	if(Preprocessing(ACTION_WORKTORTURER, girl, brothel, DayNight, summary, message))
		return true;




	// ready armor and weapons!
	g_Girls.EquipCombat(girl);

	// Complications
	if(g_Dice%100 <= 10)
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKTORTURER, -3, true);
		message = girl->m_Realname + gettext(" hurt herself while torturing someone.");
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, DayNight);
	}
	else
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKTORTURER, +3, true);
		message = girl->m_Realname + gettext(" enjoyed her job working in the dungeon.");
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, DayNight);
	}
	
	// Improve girl
	int xp = 5, libido = 5, skill = 1;

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

	if (g_Girls.HasTrait(girl, "Nymphomaniac"))
		libido += 2;

	girl->m_Pay += 65;
	g_Gold.staff_wages(65);  // wages come from you
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_BDSM, skill);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	// WD: Update flag
	g_Brothels.TortureDone(true);	

	// Check for new traits
	g_Girls.PossiblyGainNewTrait(girl, "Sadistic", 30, ACTION_WORKTORTURER, girl->m_Realname + gettext(" has come to enjoy her job so much that she has become rather Sadistic."), DayNight != 0);
	g_Girls.PossiblyGainNewTrait(girl, "Merciless", 50, ACTION_WORKTORTURER, girl->m_Realname + gettext(" extensive experience with torture has made her absolutely Merciless."), DayNight != 0);

	return false;
}
