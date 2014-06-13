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

bool cJobManager::WorkFilmDirector(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	cTariff tariff;
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
	string girlName = girl->m_Realname;
	g_Studios.m_DirectorName = girl->m_Realname;

	g_Girls.UnequipCombat(girl);
	
	girl->m_Pay += 100;
	
	message = girlName;	
	message += gettext(" worked as a film director.\n\n");
	
	int roll = g_Dice%100;
	if (roll <= 10 && g_Girls.DisobeyCheck(girl, ACTION_WORKMOVIE, brothel))
	{
		message = girl->m_Realname + gettext(" refused to work as a director today.");

		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else if(roll <= 15) {
		message += gettext(" She did not like working in the studio today.\n\n");
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKMOVIE, -1, true);
		jobperformance += -5;
	}
	else if(roll >=90)
	{
		message += gettext(" She had a great time working today.\n\n");
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKMOVIE, +3, true);
		jobperformance += 5;
	}
	else
	{
		message += gettext(" Otherwise, the shift passed uneventfully.\n\n");
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKMOVIE, +1, true);
	}

		int roll_max = girl->spirit() + girl->intelligence();
	roll_max /= 4;
	girl->m_Pay += 10 + g_Dice%roll_max;
	g_Gold.girl_support(girl->m_Pay);  // matron wages come from you

	jobperformance += (girl->spirit() - 50) / 10;
	jobperformance += (girl->intelligence() - 50) / 10;
	jobperformance += g_Girls.GetSkill(girl, SKILL_SERVICE) / 10;
	jobperformance /= 3;
	jobperformance += g_Girls.GetStat(girl, STAT_LEVEL);
	jobperformance += g_Girls.GetStat(girl, STAT_FAME) / 10;
	jobperformance += g_Dice % 4 - 1;	// should add a -1 to +3 random element --PP
	
	g_Studios.m_DirectorQuality += jobperformance;

	if(jobperformance > 0)
	{
	message += girlName + gettext(" helped improve the scene ");
						_itoa(jobperformance,buffer,10);
						message += buffer;
						message += "% with her Directing skills. \n";
	}
	else if(jobperformance < 0)
	{
	message += girlName + gettext(" did a bad job today, she reduced the scene quality ");
					_itoa(jobperformance,buffer,10);
					message += buffer;
					message += "% with her poor performance. \n";
	}
	else
		message += girlName + gettext(" did not really help the scene quality.\n");

	girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, SHIFT_NIGHT);

	// Improve girl
	int xp = 10, skill = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))
	{
		skill += 1;
		xp += 5;
	}
	else if (g_Girls.HasTrait(girl, "Slow Learner"))
	{
		skill -= 1;
		xp -= 5;
	}

	g_Girls.UpdateStat(girl, STAT_FAME, 1);
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, skill);
	
	g_Girls.PossiblyGainNewTrait(girl, "Charismatic", 30, ACTION_WORKMOVIE, gettext("She has been a director long enough that she is has become Charismatic."), DayNight != 0);
	g_Girls.PossiblyGainNewTrait(girl, "Psychic", 60, ACTION_WORKMOVIE, gettext("She has learned to handle the girls so well that you'd almost think she was Psychic."), DayNight != 0);

	return false;
}
