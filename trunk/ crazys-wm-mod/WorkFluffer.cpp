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

bool cJobManager::WorkFluffer(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";

	// No film crew.. then go home
	if (g_Studios.GetNumGirlsOnJob(0,JOB_CAMERAMAGE,false) == 0|| g_Studios.GetNumGirlsOnJob(0,JOB_CRYSTALPURIFIER,false) == 0)
		{
		message = "There was no crew to film the scene, so she took the day off";
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_NOWORK);
		return false;
		}

	char buffer[1000];
	int jobperformance = 0;

	// put that shit away, you'll scare off the customers!
	g_Girls.UnequipCombat(girl);

	// TODO need better dialog
	int roll = g_Dice%100;
	string girlName = girl->m_Realname;
	message = girlName;
		if (roll <= 10 && g_Girls.DisobeyCheck(girl, ACTION_WORKMOVIE, brothel))
	{
		message = girl->m_Realname + gettext(" refused to suck dick as a fluffer today");

		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	if(roll <= 15)
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKMOVIE, -3, true);
		message += " didn't like having so many dicks in her mouth today.\n\n";
		jobperformance += -5;
	}
	else if(roll >=90)
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKMOVIE, +3, true);
		message += " loved sucking cock today.\n\n";
		jobperformance += +5;
	}
	else
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKMOVIE, +1, true);
		message += " had a pleasant day giving blowjobs.\n\n";
	}

	jobperformance += (g_Girls.GetSkill(girl, SKILL_ORALSEX) + g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_SPIRIT)) / 30;
	jobperformance += g_Girls.GetStat(girl, STAT_LEVEL);
	jobperformance += g_Dice%4 - 1;	// should add a -1 to +3 random element --PP
	g_Studios.m_FlufferQuality += jobperformance;
		
	if(jobperformance > 0)
	{
	message += girlName + gettext(" helped improve the scene ");
						_itoa(jobperformance,buffer,10);
						message += buffer;
						message += "% by keeping the actors happy with her mouth. \n";
	}
	else if(jobperformance < 0)
	{
	message += girlName + gettext(" performed poorly, she reduced the scene quality ");
					_itoa(jobperformance,buffer,10);
					message += buffer;
					message += "% with her lack of passion while sucking dicks. \n";
	}
	else
		message += girlName + gettext(" did not really effect the scene quality.\n");

	girl->m_Events.AddMessage(message,IMGTYPE_ORAL,DayNight);
	// Improve girl
	int xp = 3, libido = 1, skill = 1;

	if (g_Girls.HasTrait(girl, "Quick Learner"))
	{
		skill += 1;
		xp += 2;
	}
	else if (g_Girls.HasTrait(girl, "Slow Learner"))
	{
		skill -= 1;
		xp -= 2;
	}

	if (g_Girls.HasTrait(girl, "Nymphomaniac"))
		libido += 2;

	girl->m_Pay += 55 + jobperformance;
	g_Gold.staff_wages(55 + jobperformance);  // wages come from you
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, skill);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	return false;
}
