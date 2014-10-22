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

// `J` Movie Studio Job - Crew
bool cJobManager::WorkCrystalPurifier(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	bool SkipDisobey = (summary == "SkipDisobey");
	// No film crew.. then go home	// `J` this will be taken care of in building flow, leaving it in for now
	if (g_Studios.GetNumGirlsOnJob(0, JOB_CAMERAMAGE, SHIFT_NIGHT) == 0 || g_Studios.GetNumGirlsOnJob(0, JOB_CRYSTALPURIFIER, SHIFT_NIGHT) == 0)
	{
		girl->m_Events.AddMessage("There was no crew to film the scene, so she took the day off", IMGTYPE_PROFILE, EVENT_NOWORK);
		return false;
	}
	else if (g_Studios.Num_Actress(0) < 1)
	{
		girl->m_Events.AddMessage("There were no actresses to film, so she took the day off", IMGTYPE_PROFILE, EVENT_NOWORK);
		return false;
	}

	cConfig cfg;
	stringstream ss;
	string girlName = girl->m_Realname;
	int wages = 50;
	int enjoy = 0;
	int jobperformance = 0;

	g_Girls.UnequipCombat(girl);	// not for studio crew

	// `J` added this to allow the Director to assign someone to this job without making it permanent
	if (girl->m_DayJob == JOB_FILMFREETIME)	// the director sets the old job to dayjob when changing night job
	{
		ss << girlName << " worked as a crystal purifier.\n\n";
	}
	else
	{
		ss << "The Director assigned " << girlName << "to edit the scenes for the week.\n\n";
	}

	int roll = g_Dice.d100();
	if (!SkipDisobey)	// `J` skip the disobey check because it has already been done in the building flow
	{
		if (roll <= 10 && g_Girls.DisobeyCheck(girl, ACTION_WORKMOVIE, brothel))
		{
			if (girl->m_DayJob == JOB_FILMFREETIME)
			{
				ss << "She refused to work as a crystal purifier today.";
				girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
			}
			else
			{
				if (g_Studios.is_Actress_Job(girl->m_DayJob))	ss << "but she wanted to be part of the action instead of just watching it.\n";
				else if (girl->m_DayJob == JOB_PROMOTER)		ss << "but she preferred to sell the movies rather than edit them.\n";
				else if (girl->m_DayJob == JOB_FLUFFER)			ss << "but she wanted to see the action live instead of watching it afterwards.\n";
				else if (girl->m_DayJob == JOB_STAGEHAND)		ss << "but she wanted to clean instead of editing scenes.\n";
				else if (girl->m_DayJob == JOB_CAMERAMAGE)		ss << "but she preferred to film the scenes rather than edit them.\n";
				girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_BACKTOWORK);
			}
			return true;
		}
	}

	/* */if (roll <= 10) { enjoy -= g_Dice % 3 + 1; ss << "She did not like working in the studio today.\n\n"; }
	else if (roll >= 90) { enjoy += g_Dice % 3 + 1; ss << "She had a great time working today.\n\n"; }
	else /*    */{ enjoy += max(0, g_Dice % 3 - 1); ss << "Otherwise, the shift passed uneventfully.\n\n"; }
	jobperformance = enjoy * 2;


	jobperformance += (girl->spirit() - 50) / 10;
	jobperformance += (girl->intelligence() - 50) / 10;
	jobperformance += g_Girls.GetSkill(girl, SKILL_SERVICE) / 10;
	jobperformance /= 3;
	jobperformance += g_Girls.GetStat(girl, STAT_LEVEL);
	jobperformance += g_Girls.GetStat(girl, STAT_FAME) / 20;
	jobperformance += g_Dice % 4 - 1;	// should add a -1 to +3 random element --PP

	// slave girls not being paid for a job that normally you would pay directly for do less work
	if ((girl->is_slave() && !cfg.initial.slave_pay_outofpocket()))
	{
		jobperformance = int(jobperformance * 0.9);
		wages = 0;
	}
	else	// work out the pay between the house and the girl
	{		
		// `J` zzzzzz - need to change pay so it better reflects how well she promoted the films
		wages += 20;
		int roll_max = girl->spirit() + girl->intelligence();
		roll_max /= 4;
		wages += 10 + g_Dice%roll_max;
	}

	/* */if (jobperformance > 0)	ss << "She helped improve the scene " << jobperformance << "% with her production skills. \n";
	else if (jobperformance < 0)	ss << "She did a bad job today, she reduced the scene quality " << jobperformance << "% with her poor performance. \n";
	else /*                   */	ss << "She did not really help the scene quality.\n";
	
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, DayNight);
	g_Studios.m_PurifierQaulity += jobperformance;
	girl->m_Pay = wages;

	// Improve stats
	int xp = 10, skill = 3, libido = 1;
	if (jobperformance > 5)	skill += 1;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	if (g_Dice % 2 == 1)
		g_Girls.UpdateStat(girl, STAT_INTELLIGENCE, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, g_Dice%skill + 1);
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	return false;
}