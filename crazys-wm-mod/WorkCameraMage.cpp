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

// `J` Job Movie Studio - Crew
bool cJobManager::WorkCameraMage(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKMOVIE;
	bool SkipDisobey = (summary == "SkipDisobey");
	stringstream ss; string girlName = girl->m_Realname;

	// No film crew.. then go home	// `J` this will be taken care of in building flow, leaving it in for now
	if (g_Studios.GetNumGirlsOnJob(0, JOB_CAMERAMAGE, SHIFT_NIGHT) == 0 || g_Studios.GetNumGirlsOnJob(0, JOB_CRYSTALPURIFIER, SHIFT_NIGHT) == 0)
	{
		ss << "There was no crew to film the scene, so " << girlName << " took the day off";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return false;	// not refusing
	}
	else if (g_Studios.Num_Actress(0) < 1)
	{
		ss << "There were no actresses to film, so " << girlName << " took the day off";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return false;	// not refusing
	}

	// `J` added this to allow the Director to assign someone to this job without making it permanent
	if (girl->m_DayJob == JOB_FILMFREETIME)	// the director sets the old job to dayjob when changing night job
	{
		ss << girlName << " was assigned to work as a cameramage";
	}
	else
	{
		ss << "The Director assigned " << girlName << "to run the camera for the week";
	}


	g_Girls.UnequipCombat(girl);	// not for studio crew

	int roll = g_Dice.d100();
	if (!SkipDisobey)	// `J` skip the disobey check because it has already been done in the building flow
	{
		if (roll <= 10 && girl->disobey_check(actiontype, brothel))
		{
			if (girl->m_DayJob == JOB_FILMFREETIME)
			{
				ss << " but she refused to work.";
				girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
			}
			else
			{
				ss << " but " << girlName;
				if (g_Studios.is_Actress_Job(girl->m_DayJob))	ss << " wanted to be in front of the camera rather than behind it.\n";
				else if (girl->m_DayJob == JOB_PROMOTER)		ss << " preferred to sell the movies instead of make them.\n";
				else if (girl->m_DayJob == JOB_FLUFFER)			ss << " was having a bad hair day and needed some cum in it to make it cooperate.\n";
				else if (girl->m_DayJob == JOB_STAGEHAND)		ss << " wanted to move around more than the camera would allow her.\n";
				else if (girl->m_DayJob == JOB_CRYSTALPURIFIER) ss << " preferred to edit the scenes rather than makeing new ones.\n";
				girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_BACKTOWORK);
			}
			return true;
		}
	}
	ss << ".\n \n";

	int wages = 50, tips = 0;
	int enjoy = 0;

	if (roll <= 10)
	{
		enjoy -= g_Dice % 3 + 1;
		ss << "She did not like working in the studio today.\n \n";
	}
	else if (roll >= 90)
	{
		enjoy += g_Dice % 3 + 1;
		ss << "She had a great time working today.\n \n";
	}
	else
	{
		enjoy += g_Dice % 2;
		ss << "Otherwise, the shift passed uneventfully.\n \n";
	}
	double jobperformance = JP_CameraMage(girl, false);
	jobperformance += enjoy * 2;

	// slave girls not being paid for a job that normally you would pay directly for do less work
	if ((girl->is_slave() && !cfg.initial.slave_pay_outofpocket()))
	{
		jobperformance *= 0.9;
		wages = 0;
	}
	else	// work out the pay between the house and the girl
	{
		// `J` zzzzzz - need to change pay so it better reflects how well she filmed the films
		wages += 20;
		int roll_max = girl->spirit() + girl->intelligence();
		roll_max /= 4;
		wages += 10 + g_Dice%roll_max;
	}

	/* */if (jobperformance > 0)	ss << "She helped improve the scene " << (int)jobperformance << "% with her camera skills. \n";
	else if (jobperformance < 0)	ss << "She did a bad job today, she reduced the scene quality " << (int)jobperformance << "% with her poor performance. \n";
	else /*                   */	ss << "She did not really effect the scene quality.\n";

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);
	g_Studios.m_CameraQuality += (int)jobperformance;
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);

	// Improve stats
	int xp = 5, skill = 3, libido = 1;
	if (jobperformance > 5)	skill += 1;

	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }

	if (g_Dice % 2 == 1)
		girl->intelligence(g_Dice%skill);
	girl->service(g_Dice%skill + 1);
	girl->exp(xp);
	girl->upd_temp_stat(STAT_LIBIDO, libido);

	return false;
}

double cJobManager::JP_CameraMage(sGirl* girl, bool estimate)// not used
{
	double jobperformance = 0.0;
	if (estimate)// for third detail string
	{
		jobperformance +=
			(girl->intelligence() / 2) +
			(girl->spirit() / 2) +
			(girl->fame() / 2) +
			(girl->service() / 2) +
			girl->level();

	}
	else// for the actual check
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= t / 3;

		jobperformance += (girl->spirit() - 50) / 10;
		jobperformance += (girl->intelligence() - 50) / 10;
		jobperformance += girl->service() / 10;
		jobperformance /= 3;
		jobperformance += girl->level();
		jobperformance += girl->fame() / 10;
		jobperformance += g_Dice % 4 - 1;	// should add a -1 to +3 random element --PP

	}
	return jobperformance;
}
