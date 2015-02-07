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
#include "cMovieStudio.h"
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
extern cMovieStudioManager g_Studios;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;

// `J` Movie Studio Job - Crew
bool cJobManager::WorkFluffer(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKMOVIE;
	bool SkipDisobey = (summary == "SkipDisobey");	// not used for this job yet
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;

	// No film crew.. then go home	// `J` this will be taken care of in building flow, leaving it in for now
	if (g_Studios.GetNumGirlsOnJob(0, JOB_CAMERAMAGE, SHIFT_NIGHT) == 0 || g_Studios.GetNumGirlsOnJob(0, JOB_CRYSTALPURIFIER, SHIFT_NIGHT) == 0)
	{
		girl->m_Events.AddMessage("There was no crew to film the scene, so she took the day off", IMGTYPE_PROFILE, EVENT_NOWORK);
		return false;	// not refusing
	}
	else if (g_Studios.Num_Actress(0) < 1)
	{
		girl->m_Events.AddMessage("There were no actresses to film, so she took the day off", IMGTYPE_PROFILE, EVENT_NOWORK);
		return false;	// not refusing
	}
	int roll = g_Dice.d100();
	if (roll <= 10 && g_Girls.DisobeyCheck(girl, ACTION_WORKMOVIE, brothel))
	{
		ss << " refused to work as a fluffer today";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked as a fluffer.\n\n";

	cConfig cfg;
	g_Girls.UnequipCombat(girl);	// not for studio crew

	int wages = 50;
	int enjoy = 0;

	if (roll <= 10)
	{
		enjoy -= g_Dice % 3 + 1;
		ss << "She didn't like having so many dicks in her mouth today.\n\n";
	}
	else if (roll >= 90)
	{
		enjoy += g_Dice % 3 + 1;
		ss << "She loved sucking cock today.\n\n";
	}
	else
	{
		enjoy += max(0, g_Dice % 3 - 1);
		ss << "She had a pleasant day giving blowjobs.\n\n";
	}
	double jobperformance = JP_Fluffer(girl, false);
	jobperformance += enjoy * 2;

	// slave girls not being paid for a job that normally you would pay directly for do less work
	if ((girl->is_slave() && !cfg.initial.slave_pay_outofpocket()))
	{
		jobperformance *= 0.9;
		wages = 0;
	}
	else
	{
		wages += (int)jobperformance;
	}

	/* */if (jobperformance > 0)	ss << "She helped improve the scene " << (int)jobperformance << "% by keeping the actors happy with her mouth. \n";
	else if (jobperformance < 0)	ss << "She performed poorly, she reduced the scene quality " << (int)jobperformance << "% with her lack of passion while sucking dicks. \n";
	else /*                   */	ss << "She did not really effect the scene quality.\n";

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_ORAL, Day0Night1);
	g_Studios.m_FlufferQuality += (int)jobperformance;
	girl->m_Pay = wages;

	// Improve girl
	int xp = 10, libido = 1, skill = 1;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, skill);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	return false;
}


double cJobManager::JP_Fluffer(sGirl* girl, bool estimate)// not used
{
	double jobperformance = 0.0;
	if (estimate)// for third detail string
	{
		jobperformance +=
			(girl->oralsex() / 2) +
			(girl->beauty() / 2) +
			(girl->spirit() / 2) +
			(girl->service() / 2) +
			girl->level();
	}
	else// for the actual check
	{
		jobperformance += (g_Girls.GetSkill(girl, SKILL_ORALSEX) + g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_SPIRIT)) / 30;
		jobperformance += g_Girls.GetSkill(girl, SKILL_SERVICE) / 10;
		jobperformance += g_Girls.GetStat(girl, STAT_LEVEL);
		jobperformance += g_Dice % 4 - 1;       // should add a -1 to +3 random element --PP
	}
	return jobperformance;
}
