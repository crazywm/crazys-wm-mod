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
#include "cJobManager.h"
#include "cBrothel.h"
#include "cArena.h"
#include "cCustomers.h"
#include "cRng.h"
#include "cInventory.h"
#include "sConfig.h"
#include "cRival.h"
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
extern cArenaManager g_Arena;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;

// `J` Arena Job - Staff - Matron_Job - Full_Time_Job
bool cJobManager::WorkDoctore(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	girl->m_DayJob = girl->m_NightJob = JOB_DOCTORE;	// it is a full time job
	if (DayNight == 1) return false;
	cTariff tariff;
	cConfig cfg;
	string message = "";
	if (Preprocessing(ACTION_WORKMATRON, girl, brothel, DayNight, summary, message)) return true;

	// Complications
	int check = g_Dice % 100;
	if (check < 10)
	{
		if (brothel->m_NumGirls > girl->get_skill(SKILL_SERVICE) * 2)
		{
			g_Girls.UpdateEnjoyment(girl, ACTION_WORKMATRON, -10, true);
			message = girl->m_Realname;
			message += gettext(" was overwhelmed by the number of girls she was required to manage and broke down crying.");
			g_Girls.UpdateStat(girl, STAT_HAPPINESS, -10);
			g_Girls.UpdateStat(girl, STAT_CONFIDENCE, -5);
			girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, DayNight);
		}
		else
		{
			g_Girls.UpdateEnjoyment(girl, ACTION_WORKMATRON, -3, true);
			message = gettext("Had trouble dealing with some of the girls.");
			g_Girls.UpdateStat(girl, STAT_HAPPINESS, -3);
			g_Girls.UpdateStat(girl, STAT_CONFIDENCE, -1);
			girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, DayNight);
		}
	}
	else if (check > 90)
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKMATRON, +3, true);
		message = gettext("Enjoyed helping the girls with their lives.");
		g_Girls.UpdateStat(girl, STAT_HAPPINESS, 3);
		g_Girls.UpdateStat(girl, STAT_CONFIDENCE, 1);
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, DayNight);
	}
	else
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKMATRON, +1, true);
		message = gettext("Went about her day as usual.");
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, DayNight);
	}


	// Improve girl
	int numgirls = brothel->m_NumGirls;
	int xp = 5 + (numgirls / 10), libido = 1, skill = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 5; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 5; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			libido += 2;
	if (g_Girls.HasTrait(girl, "Lesbian"))				libido += numgirls / 20;

	girl->m_Pay = int(float(100.0 + (((girl->get_skill(SKILL_SERVICE) + girl->get_stat(STAT_CHARISMA) + girl->get_stat(STAT_INTELLIGENCE) + girl->get_stat(STAT_CONFIDENCE) + girl->get_skill(SKILL_MEDICINE) + 50) / 50)*numgirls) * cfg.out_fact.matron_wages()));
	g_Girls.UpdateStat(girl, STAT_EXP, g_Dice%xp + 5);
	g_Girls.UpdateSkill(girl, SKILL_MEDICINE, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, g_Dice%skill + 2);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, g_Dice%libido);

	g_Girls.PossiblyGainNewTrait(girl, "Charismatic", 30, ACTION_WORKMATRON, gettext("She has worked as a matron long enough that she has learned to be more Charismatic."), DayNight != 0);
	g_Girls.PossiblyGainNewTrait(girl, "Psychic", 60, ACTION_WORKMATRON, gettext("She has learned to handle the girls so well that you'd almost think she was Psychic."), DayNight != 0);

	return false;
}