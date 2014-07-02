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

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cMovieStudioManager g_Studios;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;

bool cJobManager::WorkFilmMast(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";

	// No film crew.. then go home
	if (g_Studios.GetNumGirlsOnJob(0,JOB_CAMERAMAGE,false) == 0|| g_Studios.GetNumGirlsOnJob(0,JOB_CRYSTALPURIFIER,false) == 0)
		{
		message = "There was no crew to film the scene, so she took the day off";
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_NOWORK);
		return false;
		}

	string girlName = girl->m_Realname;
	int jobperformance = 0;
	
	// not for actress
	g_Girls.UnequipCombat(girl);

	girl->m_Pay += 30;
	message = girlName;
	message += (" worked as an actress filming Masturbation scenes.\n\n");
	
	int roll = g_Dice%100;

	if(roll <= 10 && g_Girls.DisobeyCheck(girl, ACTION_WORKMOVIE, brothel)) 
	{
		message = girlName + " refused to masturbate on film today.\n";

		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else if(roll <= 15)
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKMOVIE, -3, true);
		message += girlName + " didn't want to make the film, but the director persuaded her.\n\n";
		jobperformance += -5;
	}
	else if(roll >=90)
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKMOVIE, +3, true);
		message += girlName + " had intense orgasms while playing with her pussy!\n\n";
		jobperformance += +5;
	}
	else
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKMOVIE, +1, true);
		message += girlName + " spent the afternoon fingering herself.\n\n";
	}

	if(girl->m_Virgin)
	{
		jobperformance += 20;
		message += "She is a virgin.\n";
	}
	jobperformance += g_Girls.GetSkill(girl, SKILL_PERFORMANCE) / 10;
	jobperformance += g_Dice % 4 - 1;	// should add a -1 to +3 random element --PP
	jobperformance += -10; // Modifier for what kind of sex scene it is.. normal sex is the baseline at +0
	// remaining modifiers are in the AddScene function --PP
	string finalqual = g_Studios.AddScene(girl, SKILL_SERVICE, jobperformance);
	message += "Her scene us valued at: " + finalqual + " gold.\n";

	girl->m_Events.AddMessage(message, IMGTYPE_MAST, DayNight);

/*
 *	work out the pay between the house and the girl
 *
 *	the original calc took the average of beauty and charisma and halved it
 */
	int roll_max = girl->beauty() + girl->charisma();
	roll_max /= 4;
	girl->m_Pay += 10 + g_Dice%roll_max;

	// Improve stats
	int xp = 5, skill = 3;

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
	g_Girls.UpdateSkill(girl, SKILL_PERFORMANCE, g_Dice%skill + 1);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, g_Dice%skill);

	g_Girls.PossiblyGainNewTrait(girl, "Fake Orgasm Expert", 15, ACTION_WORKMOVIE, "She has become quite the faker.", DayNight != 0);

	return false;
}
