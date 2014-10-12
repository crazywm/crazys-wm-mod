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
#include "cMovieStudio.h"

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cMovieStudioManager g_Studios;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;

// `J` Movie Studio Job - Crew - job_is_cleaning
bool cJobManager::WorkFilmStagehand(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	cConfig cfg;
	stringstream ss;
	string girlName = girl->m_Realname;
	int wages = 50;
	int enjoy = 0;
	int jobperformance = 0;
	int CleanAmt = ((g_Girls.GetSkill(girl, SKILL_SERVICE) / 10) + 5) * 5;
	bool filming = true;
	bool playtime = false;
	u_int imagetype = IMGTYPE_PROFILE;

	g_Girls.UnequipCombat(girl);	// not for studio crew

	ss << girlName << " worked as a stagehand.\n\n";

	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();

	if (roll_a <= 10 && g_Girls.DisobeyCheck(girl, ACTION_WORKMOVIE, brothel))
	{
		ss << "She refused to work as a stagehand today.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}

	if (g_Studios.GetNumGirlsOnJob(0, JOB_CAMERAMAGE, SHIFT_NIGHT) == 0 ||
		g_Studios.GetNumGirlsOnJob(0, JOB_CRYSTALPURIFIER, SHIFT_NIGHT) == 0 ||
		g_Studios.Num_Actress(0) < 1)
	{
		ss << "There were no scenes being filmed, so she just cleaned the set.\n\n";
		filming = false;
		CleanAmt *= 2;
	}


	/* */if (roll_a <= 10) { enjoy -= g_Dice % 3 + 1; ss << "She did not like working in the studio today.\n\n"; }
	else if (roll_a >= 90) { enjoy += g_Dice % 3 + 1; ss << "She had a great time working today.\n\n"; }
	else /*      */{ enjoy += max(0, g_Dice % 3 - 1); ss << "Otherwise, the shift passed uneventfully.\n\n"; }
	jobperformance = enjoy * 2;


	if (filming)
	{
		jobperformance += (girl->spirit() - 50) / 10;
		jobperformance += (girl->intelligence() - 50) / 10;
		jobperformance += g_Girls.GetSkill(girl, SKILL_SERVICE) / 10;
		jobperformance /= 3;
		jobperformance += g_Girls.GetStat(girl, STAT_LEVEL);
		jobperformance += g_Dice % 4 - 1;	// should add a -1 to +3 random element --PP

		if (jobperformance < 0) jobperformance = max(-1, jobperformance / 10);
		else if (jobperformance > 0) jobperformance = min(1, jobperformance / 10);

		/* */if (jobperformance > 0)	ss << "She helped improve the scene " << jobperformance << "% with her production skills.\n";
		else if (jobperformance < 0)	ss << "She did a bad job today, reduceing the scene quality " << jobperformance << "% with her poor performance.\n";
		else /*                   */	ss << "She did not really help the scene quality.\n";
	}


	// slave girls not being paid for a job that normally you would pay directly for do less work
	if ((girl->is_slave() && !cfg.initial.slave_pay_outofpocket()))
	{
		CleanAmt = int(CleanAmt * 0.9);
		wages = 0;
	}
	else if (filming)
	{
		wages += CleanAmt + jobperformance;
	}
	else
	{
		wages += CleanAmt;
	}

	ss << gettext("\n\nCleanliness rating improved by ") << CleanAmt;

	if (!filming && brothel->m_Filthiness < CleanAmt / 2) // `J` needs more variation
	{
		ss << "\n\n" << girlName << " finished her cleaning early so she hung out around the Studio a bit.";
		g_Girls.UpdateTempStat(girl, STAT_LIBIDO, g_Dice % 3 + 1);
		g_Girls.UpdateStat(girl, STAT_HAPPINESS, g_Dice % 3 + 1);
	}
	imagetype = IMGTYPE_MAID;


	girl->m_Events.AddMessage(ss.str(), imagetype, SHIFT_NIGHT);
	if (filming) g_Studios.m_StagehandQuality += jobperformance;
	brothel->m_Filthiness -= CleanAmt;
	girl->m_Pay += wages;


	// Improve girl
	int xp = 10, skill = 3, libido = 1;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, skill);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	if (filming) g_Girls.UpdateEnjoyment(girl, ACTION_WORKMOVIE, enjoy, true);
	g_Girls.UpdateEnjoyment(girl, ACTION_WORKCLEANING, enjoy, true);

	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Clumsy", 30, ACTION_WORKCLEANING, "It took her spilling hundreds of buckets, and just as many reprimands, but " + girl->m_Realname + " has finally stopped being so Clumsy.", DayNight != 0);
	
	return false;
}