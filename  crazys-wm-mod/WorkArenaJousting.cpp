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
#include "cArena.h"
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
extern cArenaManager g_Arena;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;

bool cJobManager::WorkArenaJousting(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";
	string girlName = girl->m_Realname;

	if(Preprocessing(ACTION_COMBAT, girl, brothel, DayNight, summary, message))
		return true;

	int roll = g_Dice%100;
	int wages = 50;
	int jobperformance = ((g_Girls.GetStat(girl, STAT_INTELLIGENCE) / 2) + (g_Girls.GetSkill(girl, SKILL_ANIMALHANDLING) / 2) + g_Girls.GetSkill(girl, SKILL_COMBAT));


	message += "She worked as a jouster in the arena.";

	//CRAZY
	/*Jousting plans
		General plan is to at some point make it to where u have a jousting tourment ever week.  More girls the better
		have them place and 1st place thru 3rd get the most money.  How they place would be based off job performance.
		If they have the same lvl of jobperformance then have a random roll to see who wins.  More girls in it mean more
		people show up to watch it and would mean more money.  Not sure how to do this yet but its more or less the basic
		ideal.*/ 


	//good traits
	if (g_Girls.HasTrait(girl, "Charismatic"))   jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Aggressive"))    jobperformance += 20;
	if (g_Girls.HasTrait(girl, "Long Legs"))     jobperformance += 5;  //be able to ride better
	if (g_Girls.HasTrait(girl, "Quick Learner")) jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Psychic"))		 jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Strong"))		 jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Strong Magic"))  jobperformance += 5; //use magic to cheat
	if (g_Girls.HasTrait(girl, "Assassin"))		 jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Sadistic"))		 jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Manly"))		 jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Merciless"))	 jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Fearless"))		 jobperformance += 5;


	//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))   jobperformance -= 50;
	if (g_Girls.HasTrait(girl, "Clumsy"))      jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Nervous"))     jobperformance -= 30;
	if (g_Girls.HasTrait(girl, "Meek"))		   jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Fragile"))	   jobperformance -= 10;
	if (g_Girls.HasTrait(girl, "Abnormally Large Boobs"))  jobperformance -= 10; //boobs get in the way
	if (g_Girls.HasTrait(girl, "Slow Learner"))jobperformance -= 5;
	if (g_Girls.HasTrait(girl, "One Eye"))     jobperformance -= 15;
	if (g_Girls.HasTrait(girl, "Eye Patch"))   jobperformance -= 15;
	if (g_Girls.HasTrait(girl, "Elegant"))     jobperformance -= 5;  //wouldnt want to do this


	if (jobperformance >= 245)
		{
			message += " She must be perfect at this.\n\n";
			wages += 155;
		}
	else if (jobperformance >= 185)
		{
			message += " She's unbelievable at this and is always getting praised by people for her work.\n\n";
			wages += 95;
		}
	else if (jobperformance >= 145)
		{
			message += " She's good at this job and gets praised by the customers often.\n\n";
			wages += 55;
		}
	else if (jobperformance >= 100)
		{
			message += " She made a few mistakes but overall she is okay at this.\n\n";
			wages += 15;
		}
	else if (jobperformance >= 70)
		{
			message += " She was nervous and made a few mistakes. She isn't that good at this.\n\n";
			wages -= 5;
		}
	else
		{
			message += " She was nervous and constantly making mistakes. She really isn't very good at this job.\n\n";
			wages -= 15;
		}

	if(wages < 0)
			wages = 0;


		//enjoyed the work or not
	if(roll <= 5)
	{
		message += " \nSome of the patrons abused her during the shift.";
		g_Girls.UpdateEnjoyment(girl, ACTION_COMBAT, -1, true);
	}
	else if(roll <= 25) {
		message += " \nShe had a pleasant time working.";
		g_Girls.UpdateEnjoyment(girl, ACTION_COMBAT, +3, true);
	}
	else
	{
		message += " \nOtherwise, the shift passed uneventfully.";
		g_Girls.UpdateEnjoyment(girl, ACTION_COMBAT, +1, true);
	}

	girl->m_Events.AddMessage(message, IMGTYPE_COMBAT, DayNight);
	int roll_max = (g_Girls.GetStat(girl, STAT_FAME) + g_Girls.GetStat(girl, STAT_CHARISMA));
	roll_max /= 4;
	wages += 10 + g_Dice%roll_max;
	girl->m_Pay = wages;


	// Improve stats
	int xp = 15, libido = 1, skill = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3;}
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3;}
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_FAME, 1);
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_ANIMALHANDLING, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_COMBAT, g_Dice%skill + 2);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	//gained
	

	//lose
	g_Girls.PossiblyLoseExistingTrait(girl, "Nervous", 30, ACTION_COMBAT, girl->m_Realname + " has went into battle so many times she is no longer nervous.", DayNight != 0);

	return false;
}