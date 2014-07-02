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

bool cJobManager::WorkFilmGroup(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";

	// No film crew.. then go home
	if (g_Studios.GetNumGirlsOnJob(0,JOB_CAMERAMAGE,false) == 0|| g_Studios.GetNumGirlsOnJob(0,JOB_CRYSTALPURIFIER,false) == 0)
		{
		message = "There was no crew to film the scene, so she took the day off";
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_NOWORK);
		return false;
		}

	int jobperformance = 0;
	string girlName = girl->m_Realname;
	char buffer[1000];

	// not for actress
	g_Girls.UnequipCombat(girl);

	girl->m_Pay += 50;
	int guys = g_Dice%100;
	if (guys < 50)
	{
		guys = 2;
		jobperformance += 5;
		message += girlName + (" worked as an actress filming a three-way.\n\n");
	}
	else if (guys < 95)
	{
		jobperformance += 10;
		guys = g_Dice%100;
		if (guys < 40)
			guys = 3;
		else if (guys < 60)
			guys = 4;
		else if (guys < 75)
			guys = 5;
		else if (guys < 85)
			guys = 6;
		else if (guys < 90)
			guys = 7;
		else if (guys < 94)
			guys = 8;
		else if (guys < 98)
			guys = 9;
		else
			guys = 10;
		message += girlName + (" worked in a gang-bang film with ");
		_itoa(guys, buffer, 10);
			message += buffer;
			message += (" other people\n\n");
	}
	else
	{
		jobperformance += 20;
		guys = g_Dice%80 + 20;
		message += girlName + (" worked in a orgy scene with ");
		_itoa(guys, buffer, 10);
			message += buffer;
			message += (" other people\n\n");
	}
	
	girl->m_Pay += (guys * 10);	// Extra pay per guy
	
	for (int i = 0; i < guys; i += 3)	// Extra tiredness per guy
		g_Girls.AddTiredness(girl);
	
	int roll = g_Dice%100;

if(roll <= 10 && g_Girls.DisobeyCheck(girl, ACTION_WORKMOVIE, brothel)) 
	{
		message = girlName + " refused to do an orgy on film today.\n";

		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else if(roll <= 15)
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKMOVIE, -3, true);
		message += girlName + " found it unpleasant fucking that many people.\n\n";
		jobperformance += -5;
	}
	else if(roll >=90)
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKMOVIE, +3, true);
		message += girlName + " loved getting so much action, and wants more!\n\n";
		jobperformance += +5;
	}
	else
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKMOVIE, +1, true);
		message += girlName + " wasn't really into having so much sex today, but managed to get through.\n\n";
	}


	if(girl->m_Virgin)
	{
		g_Girls.LoseVirginity(girl);	// `J` updated for trait/status
		jobperformance += 50;
		message += "She is no longer a virgin.\n";
	}

	if(!girl->calc_group_pregnancy(g_Brothels.GetPlayer(), false, 1.0)) {
			g_MessageQue.AddToQue("She has gotten pregnant", 0);
		}

	jobperformance += g_Girls.GetSkill(girl, SKILL_PERFORMANCE) / 10;
	jobperformance += g_Dice % 4 - 1;	// should add a -1 to +3 random element --PP
	jobperformance += 5; // Modifier for what kind of sex scene it is.. normal sex is the baseline at +0
	// remaining modifiers are in the AddScene function --PP
	string finalqual = g_Studios.AddScene(girl, SKILL_GROUP, jobperformance);
	message += "Her scene us valued at: " + finalqual + " gold.\n";

	girl->m_Events.AddMessage(message, IMGTYPE_GROUP, DayNight);

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
	g_Girls.UpdateSkill(girl, SKILL_PERFORMANCE, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_GROUP, g_Dice%skill + 1);

	g_Girls.PossiblyGainNewTrait(girl, "Fake Orgasm Expert", 15, ACTION_WORKMOVIE, "She has become quite the faker.", DayNight != 0);

	return false;
}
