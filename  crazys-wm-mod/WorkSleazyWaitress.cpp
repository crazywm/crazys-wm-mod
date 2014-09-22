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

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;

bool cJobManager::WorkSleazyWaitress(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = ""; string girlName = girl->m_Realname;
	int tex = g_Dice%4;

	if(Preprocessing(ACTION_WORKCLUB, girl, brothel, DayNight, summary, message))	// they refuse to have work in the bar
		return true;

	// put that shit away, you'll scare off the customers!
	g_Girls.UnequipCombat(girl);

	int roll = g_Dice%100;
	int jobperformance = ( (g_Girls.GetStat(girl, STAT_CHARISMA) +
							g_Girls.GetStat(girl, STAT_BEAUTY) +
							g_Girls.GetSkill(girl, SKILL_PERFORMANCE)) / 3 +
							g_Girls.GetSkill(girl, SKILL_SERVICE));

	int wages = 25, work = 0;
	int imagetype = IMGTYPE_ECCHI;
	message += "She worked as a waitress in the strip club.";


	//good traits
	if (g_Girls.HasTrait(girl, "Charismatic"))		jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Sexy Air"))			jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Cool Person"))		jobperformance += 10; //people love to be around her	
	if (g_Girls.HasTrait(girl, "Cute"))				jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Charming"))			jobperformance += 15; //people like charming people
	if (g_Girls.HasTrait(girl, "Great Figure"))		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Great Arse"))		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Quick Learner"))	jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Psychic"))			jobperformance += 10; //knows what people want to hear
	if (g_Girls.HasTrait(girl, "Fleet of Foot") || g_Girls.HasTrait(girl, "Fleet Of Foot")) jobperformance += 5;  //faster at taking orders and droping them off


	//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))	jobperformance -= 50; //needs others to do the job	
	if (g_Girls.HasTrait(girl, "Clumsy"))		jobperformance -= 20; //spills food and breaks things often	
	if (g_Girls.HasTrait(girl, "Aggressive"))	jobperformance -= 20;  //gets mad easy and may attack people
	if (g_Girls.HasTrait(girl, "Nervous"))		jobperformance -= 20; //don't like to be around people
	if (g_Girls.HasTrait(girl, "Abnormally Large Boobs"))  jobperformance -= 20;  //boobs are to big and get in the way
	if (g_Girls.HasTrait(girl, "Meek"))			jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Slow Learner"))	jobperformance -= 10;
	if (g_Girls.HasTrait(girl, "One Eye"))		jobperformance -= 10;


	if (jobperformance >= 245)
		{
			message += " She must be the perfect waitress customers go on and on about her and always come to see her when she works.\n\n";
			wages += 155;
		}
	else if (jobperformance >= 185)
		{
			message += " She's unbelievable at this and is always getting praised by the customers for her work.\n\n";
			wages += 95;
		}
	else if (jobperformance >= 135)
		{
			message += " She's good at this job and gets praised by the customers often.\n\n";
			wages += 55;
		}
	else if (jobperformance >= 85)
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


	//try and add randomness here
	if (g_Girls.GetStat(girl, STAT_BEAUTY) >85)
		if ((g_Dice%100) < 20)
		{
			message += " Stunned by her beauty a customer left her a great tip.\n";
			wages += 25;
		}

	if (g_Girls.HasTrait(girl, "Clumsy"))
		if ((g_Dice%100) < 15)
		{
			message += " Her clumsy nature cause her to spill food on a custmoer resulting in them storming off without paying.\n";
			wages -= 25;
		}

	if (g_Girls.HasTrait(girl, "Pessimist"))
		if ((g_Dice%100) < 5)
		{
			if (jobperformance < 125)
			{
			message += " Her pessimistic mood depressed the customers making them tip less.\n";
			wages -= 10;
			}
			else
			{
				message += girl->m_Realname + " was in a poor mood so the patrons gave her a bigger tip to try and cheer her up.\n";
				wages += 10;
			}
		}

	if (g_Girls.HasTrait(girl, "Optimist"))
		if ((g_Dice%100) < 5)
		{
			if (jobperformance < 125)
			{
				message += girlName + " was in a cheerful mood but the patrons thought she needed to work more on her services.\n";
				wages -= 10;
			}
			else
			{
			message += " Her optimistic mood made patrons cheer up increasing the amount they tip.\n";
			wages += 10;
			}
		}

	if (g_Girls.HasTrait(girl, "Great Arse"))
	{
		if((g_Dice%100) < 15)
		{
		if (jobperformance >= 185) //great
			{
				message += " A patron reached out to grab her ass. But she skillfully avoided it with a laugh and told him that her ass wasn't on the menu.  He laughed so hard he increased her tip\n";
				wages += 15;
			}
		else if (jobperformance >= 135) //decent or good
			{
				message += " A patron reached out and grabed her ass. She's use to this and skilled enough so she didn't drop anything\n";
			}
		else if (jobperformance >= 85) //bad
			{
				message += " A patron reached out and grabed her ass. She was startled and ended up dropping half an order.\n";
				wages -= 10;
			}
		else  //very bad
			{
				message += " A patron reached out and grabed her ass. She was startled and ended up dropping a whole order\n";
				wages -= 15;
			}
		}

	if (g_Girls.HasTrait(girl, "Great Figure"))
		if ((g_Dice%100) <= 25)
		{
			if (jobperformance < 125)
			{
				message += girlName + "'s amazing figure wasn't enough to keep the patrons happy when her servies was so bad.\n";
				wages -= 10;
			}
			else
			{
			message += " Not only does she have an amazing figure but she is also an amazing waitress the patrons really love her and her tips prove it.\n";
			wages += 10;
			}
		}

	if (g_Girls.HasTrait(girl, "Meek") || g_Girls.HasTrait(girl, "Shy"))
		if ((g_Dice%100) < 5)
			{
				message += girlName + " was taking an order from a rather rude patron when he decide to grope her.  She ins't the kind of girl to resist this and had a bad day at work because of this.\n";
				work -=5;
			}


		if(wages < 0)
			wages = 0;


	//enjoyed the work or not
	if (roll <= 5)
	{ message += " \nSome of the patrons abused her during the shift."; work -= 1; }
	else if (roll <= 25) 
	{ message += " \nShe had a pleasant time working."; work += 3; }
	else
	{ message += " \nOtherwise, the shift passed uneventfully."; work += 1; }


	g_Girls.UpdateEnjoyment(girl, ACTION_WORKCLUB, work , true);
	girl->m_Events.AddMessage(message, imagetype, DayNight);


	int roll_max = (g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_CHARISMA));
	roll_max /= 4;
	wages += 10 + g_Dice%roll_max;
	girl->m_Pay += wages;

	// Improve stats
	int xp = 15, libido = 1, skill = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_FAME, 1);
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_PERFORMANCE, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, g_Dice%skill + 1);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	//gained traits
	g_Girls.PossiblyGainNewTrait(girl, "Charming", 70, ACTION_WORKCLUB, girlName + " has been flirting with customers to try to get better tips. Enough practice at it has made her quite Charming.", DayNight != 0);
	if (jobperformance > 150 || g_Girls.GetStat(girl, STAT_CONSTITUTION) > 65) { g_Girls.PossiblyGainNewTrait(girl, "Fleet Of Foot", 60, ACTION_WORKBAR, girlName + " has been doding bewteen tables and avoiding running into customers for so long she has become Fleet Of Foot.", DayNight != 0); }

	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Clumsy", 30, ACTION_WORKCLUB, "It took her break hundreds of dishes, and just as many reprimands, but " + girlName + " has finally stopped being so Clumsy.", DayNight != 0);

	return false;
}