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

static cDungeon* m_Dungeon = g_Brothels.GetDungeon();

bool cJobManager::WorkRecruiter(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{

	cTariff tariff;
	string message = "";

	if(Preprocessing(ACTION_WORKRECRUIT, girl, brothel, DayNight, summary, message))	// they refuse to have work in the bar
		return true;


	// put that shit away, not needed for sex training
	g_Girls.UnequipCombat(girl);

	int roll = g_Dice%100;
	int HateLove = 0;
	HateLove = g_Girls.GetStat(girl, STAT_PCLOVE) - g_Girls.GetStat(girl, STAT_PCHATE);
	int jobperformance = (HateLove + g_Girls.GetStat(girl, STAT_CHARISMA));
	int wages = 100;

	message += "She worked trying to recruit girls for you.";

	if      (HateLove < -80)	message += " She hates you more then anything so she doesn't try that hard.\n\n";
	else if (HateLove < -60)	message += " She hates you.\n\n";
	else if (HateLove < -40)	message += " She doesn't like you.\n\n";
	else if (HateLove < -20)	message += " She finds you to be annoying.\n\n";
	else if (HateLove <   0)	message += " She finds you to be annoying.\n\n";
	else if (HateLove <  20)	message += " She finds you to be decent.\n\n";
	else if (HateLove <  40)	message += " She finds you to be a good person.\n\n";
	else if (HateLove <  60)	message += " She finds you to be a good person.\n\n";
	else if (HateLove <  80)	message += " She has really strong feelings for you so she trys really hard for you.\n\n";
	else					message += " She loves you more then anything so she gives it her all.\n\n";

	//good traits
	if (g_Girls.HasTrait(girl, "Charismatic"))  //
		jobperformance += 20;
	if (g_Girls.HasTrait(girl, "Cool Person"))  //people love to be around her
		jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Charming"))  //people like charming people
		jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Psychic"))  //knows what people want to hear
		jobperformance += 20;

	//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))  //needs others to do the job
		jobperformance -= 50;
	if (g_Girls.HasTrait(girl, "Clumsy"))  //
		jobperformance -= 5;
	if (g_Girls.HasTrait(girl, "Aggressive"))  //gets mad easy and may attack people
		jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Nervous"))  //don't like to be around people
		jobperformance -= 30;
	if (g_Girls.HasTrait(girl, "Meek"))
		jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Broken Will"))
		jobperformance -= 50;


	int findchance = 0;

	if (jobperformance >= 245)
	{
		message += " She must be the perfect recruiter.\n\n";
		findchance = 30;
	}
	else if (jobperformance  >= 185)
	{
		message += " She's unbelievable at this.\n\n";
		findchance = 20;
	}
	else if (jobperformance >= 135)
	{
		message += " She's good at this job.\n\n";
		findchance = 16;
	}
	else if (jobperformance >= 85)
	{
		message += " She made a few mistakes but overall she is okay at this.\n\n";
		findchance = 12;
	}
	else if (jobperformance >= 65)
	{
		message += " She was nervous and made a few mistakes. She isn't that good at this.\n\n";
		findchance = 8;
	}
	else
	{
		message += " She was nervous and constantly making mistakes. She really isn't very good at this job.\n\n";
		findchance = 4;
	}
	if ((g_Dice % 101)<findchance)
	{
		sGirl* girl = g_Girls.GetRandomGirl();
		if (girl)
		{
			/* MYR: For some reason I can't figure out, a number of girl's house percentages
			are at zero or set to zero when they are sent to the dungeon. I'm not sure
			how to fix it, so I'm explicitly setting the percentage to 60 here */
			girl->m_Stats[STAT_HOUSE] = 60;
			message += gettext("She finds a girl, ");
			message += girl->m_Name;
			message += gettext(" and convinces her that she should work for you.");
			m_Dungeon->AddGirl(girl, DUNGEON_NEWGIRL);
		}
	}
	else
	{
		message += "But was unable to find anyone to join.";
	}



//enjoyed the work or not
	if(roll <= 5)
	{
		message += " \nSome people abused her during the shift.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKRECRUIT, -1, true);
	}
	else if(roll <= 25) {
		message += " \nShe had a pleasant time working.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKRECRUIT, +3, true);
	}
	else
	{
		message += " \nOtherwise, the shift passed uneventfully.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKRECRUIT, +1, true);
	}

	girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, DayNight);
	int roll_max = (g_Girls.GetStat(girl, STAT_CHARISMA) + g_Girls.GetSkill(girl, SKILL_SERVICE));
	roll_max /= 4;
	wages += 10 + g_Dice%roll_max;
	girl->m_Pay = wages;
	

	// Improve stats
	int xp = 10, libido = 1, skill = 3;

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

	g_Girls.UpdateStat(girl, STAT_FAME, 1);
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	if (g_Dice % 2)	g_Girls.UpdateSkill(girl, SKILL_LESBIAN, 1);
	else			g_Girls.UpdateStat(girl, STAT_INTELLIGENCE, 1);
	if (g_Dice % 2)	g_Girls.UpdateStat(girl, STAT_CHARISMA, skill);
	else			g_Girls.UpdateSkill(girl, SKILL_SERVICE, skill);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	//gain traits
	g_Girls.PossiblyGainNewTrait(girl, "Charismatic", 60, ACTION_WORKRECRUIT, "Dealing with people all day has made " + girl->m_Realname + " more Charismatic.", DayNight != 0);

	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Nervous", 20, ACTION_WORKRECRUIT, girl->m_Realname + " seems to finally be getting over her shyness. She's not always so Nervous anymore.", DayNight != 0);
	return false;
	}
