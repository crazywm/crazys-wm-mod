/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders 
 * who meet on http://pinkpetal.co.cc
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

bool cJobManager::WorkBarWaitress(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";
	string girlName = girl->m_Realname;
	int tex = g_Dice%4;

	if(Preprocessing(ACTION_WORKBAR, girl, brothel, DayNight, summary, message))	// they refuse to have work in the bar
		return true;

	// put that shit away, you'll scare off the customers!
	g_Girls.UnequipCombat(girl);

	int wages = 15;
	message += "She worked as a waitress in the bar.";

	int roll = g_Dice%100;
	int jobperformance = (g_Girls.GetStat(girl, STAT_INTELLIGENCE) + g_Girls.GetSkill(girl, SKILL_SERVICE));

	//good traits
	if (g_Girls.HasTrait(girl, "Charismatic"))  //
		jobperformance += 15;
	if (g_Girls.HasTrait(girl, "Sexy Air"))  //
		jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Cool Person"))  //people love to be around her
		jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Cute"))  //
		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Charming"))  //people like charming people
		jobperformance += 20;
	if (g_Girls.HasTrait(girl, "Quick Learner"))  //people like charming people
		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Psychic"))
		jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Fleet of Foot")) //faster at taking orders and droping them off
		jobperformance += 5;

	//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))  //needs others to do the job
		jobperformance -= 50;
	if (g_Girls.HasTrait(girl, "Clumsy"))  //spills food and breaks things often
		jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Aggressive"))  //gets mad easy and may attack people
		jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Nervous"))  //don't like to be around people
		jobperformance -= 30;
	if (g_Girls.HasTrait(girl, "Abnormally Large Boobs"))  //boobs are to big and get in the way
		jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Meek"))
		jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Slow Learner"))
		jobperformance -= 10;


	if(jobperformance < 45)
		{
			message += " She was nervous and constantly making mistakes. She really isn't very good at this job.\n\n";
			wages -= 15;
		if (roll <= 50)
			{
				message += girlName + " was taking orders to the wrong tables and letting a lot people walk out without paying there tab.\n";
			}
			else
			{
				message += girlName + " spilled food all over the place and mixed orders up constantly.\n";
			}
		}
	else if(jobperformance < 70)
		{
			message += " She was nervous and made a few mistakes. She isn't that good at this.\n\n";
			wages -= 5;
		if (roll <= 50)
			{
				message += girlName + " wrote down the wrong orders for a few patrons resulting in them leaving.\n";
			}
			else
			{
				message += girlName + " sneezed in an order she had just sat down on a table.  Needless to say the patron was mad and left.\n";
			}

		if (g_Girls.HasTrait(girl, "Great Arse"))
			if(roll <= 15)
		{
			message += " A patron reached out and grabed her ass. She was startled and ended up dropping a whole order\n";
			wages -= 15;
		}
		}
	else if(jobperformance < 100)
		{
			message += " She made a few mistakes but overall she is okay at this.\n\n";
			wages += 15;

		if (roll <= 50)
			{
				message += girlName + " forgot to take an order to a table for over a hour.  But they were in a forgiving mood and stuck around.\n";
			}
			else
			{
				message += girlName + " sneezed in an order she had just sat down on a table.  Needless to say the patron was mad and left.\n";
			}

			if (g_Girls.HasTrait(girl, "Great Arse"))
			if(roll <= 15)
		{
			message += " A patron reached out and grabed her ass. She was startled and ended up dropping half an order.\n";
			wages -= 10;
		}
		}
	else if(jobperformance < 145)
		{
			message += " She's good at this job and gets praised by the customers often.\n\n";
			wages += 55;

		if (roll <= 50)
			{
				if (g_Girls.HasTrait(girl, "Big Boobs") || g_Girls.HasTrait(girl, "Abnormally Large Boobs"))
				{
					message += "The patron love been served by " + girl->m_Realname + ".  Due to the fact she's good at her job and they love staring at her Big Boobs.\n";
				}
				else
				{
				message += girlName + " didn't mess up any order this shift.  Keeping the patrons happy.\n";
				}
			}
			else
			{
				message += girlName + " had some regulars come in.  She knows there order by heart and put it in as soon as she seen them walk in making them happy.\n";
			}

			if (g_Girls.HasTrait(girl, "Great Arse"))
			if(roll <= 15)
		{
			message += " A patron reached out and grabed her ass. She's use to this and skilled enough so she didn't drop anything\n";
		}
		}
	else if(jobperformance < 185)
		{
			message += " She's unbelievable at this and is always getting praised by the customers for her work.\n\n";
			wages += 95;

		if (roll <= 50)
			{
				message += girlName + " bounced from table to table taking orders and recommending items to help you make more money.\n";
			}
			else
			{
				message += girlName + " is a town favriote and knows most of the patrons by name and what they order.\n";
			}

			if (g_Girls.HasTrait(girl, "Great Arse"))
			if(roll <= 15)
		{
			message += " A patron reached out to grab her ass. But she skillfully avoided it with a laugh and told him that her ass wasn't on the menu.  He laughed so hard he increased her tip\n";
			wages += 15;
		}
		}
	else if(jobperformance < 245)
		{
			message += " She must be the perfect waitress customers go on and on about her and always come to see her when she works.\n\n";
			wages += 155;

		if (roll <= 50)
			{
				message += girlName + " danced around the bar dropping orders off as if she dosen't even have to think to do this anymore.\n";
			}
			else
			{
				message += "People came in from everywhere to see " + girl->m_Realname + " work.  She bounces all around the bar laughing and keeping the patrons happy without messing anything up.\n";
			}

			if (g_Girls.HasTrait(girl, "Great Arse"))
			if(roll <= 15)
		{
			message += " A patron reached out to grab her ass. But she skillfully avoided it with a laugh and told him that her ass wasn't on the menu.  He laughed so hard he increased her tip\n";
			wages += 15;
		}
		}

	//try and add randomness here
	if (g_Girls.GetStat(girl, STAT_BEAUTY) >85)
		if((g_Dice%100) < 20)
		{
			message += " Stunned by her beauty a customer left her a great tip.\n";
			wages += 25;
		}

	if (g_Girls.HasTrait(girl, "Clumsy"))
		if((g_Dice%100) < 15)
		{
			message += " Her clumsy nature cause her to spill food on a custmoer resulting in them storming off without paying.\n";
			wages -= 25;
		}

	if (g_Girls.HasTrait(girl, "Pessimist"))
		if((g_Dice%100) < 5)
		{
			if(jobperformance < 125)
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
		if((g_Dice%100) < 5)
		{
			if(jobperformance < 125)
			{
				message += girl->m_Realname + " was in a cheerful mood but the patrons thought she needed to work more on her services.\n";
				wages -= 10;
			}
			else
			{
			message += " Her optimistic mood made patrons cheer up increasing the amount they tip.\n";
			wages += 10;
			}
		}

	if (g_Girls.HasTrait(girl, "Psychic"))
		if((g_Dice%101) < 20)
		{
			message += "She used her Psychic skills to know excatally what the patrons wanted to order making them happy and increasing her tips.\n";
			wages += 15;
		}

	if (g_Girls.HasTrait(girl, "Assassin"))
		if((g_Dice%101) < 5)
		{
			if(jobperformance < 150)
			{
				message += " A patron pissed her off and using her Assassin skills she killed him before even thinking about it resulting in patrons storming out without paying.\n";
				wages -= 50;
			}
			else
			{
				message += " A patron pissed her off but she just gave them a death stare and walked away.\n";
			}
		}

	if (g_Girls.HasTrait(girl, "Horrific Scars"))
		if((g_Dice%101) < 15)
		{
			if(jobperformance < 150)
			{
				message += " A patron gasped at her Horrific Scars making her uneasy.  But they didn't feel sorry for her.\n";
			}
			else
			{
				message += " A patron gasped at her Horrific Scars making her sad.  Feeling bad about it as she did a wonderful job they left a good tip.\n";
				wages += 15;
			}
		}

	if (g_Brothels.GetNumGirlsOnJob(0,JOB_BARMAID,false) == 1)
		if((g_Dice%100) < 25)
		{
			if(jobperformance < 125)
			{
				message += girl->m_Realname + " wasn't good enough at her job to use the barmaid to her advantage.\n";
			}
			else
			{
			message += girl->m_Realname + " used the barmaid to great effect speeding up her work and increasing her tips.\n";
			wages += 25;
			}
		}

		if(wages < 0)
			wages = 0;



	//enjoyed the work or not
	if(roll <= 5)
	{
		message += " \nSome of the patrons abused her during the shift.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKBAR, -1, true);
	}
	else if(roll <= 25) {
		message += " \nShe had a pleasant time working.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKBAR, +3, true);
	}
	else
	{
		message += " \nOtherwise, the shift passed uneventfully.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKBAR, +1, true);
	}

	girl->m_Events.AddMessage(message, IMGTYPE_WAIT, DayNight);

	int roll_max = (g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_CHARISMA));
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
	if(g_Dice%2)
		g_Girls.UpdateStat(girl, STAT_INTELLIGENCE, 1);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, skill);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);
	
	//gain traits
	g_Girls.PossiblyGainNewTrait(girl, "Charming", 70, ACTION_WORKBAR, girl->m_Realname + " has been flirting with customers to try to get better tips. Enough practice at it has made her quite Charming.", DayNight != 0);

	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Clumsy", 30, ACTION_WORKBAR, "It took her break hundreds of dishes, and just as many reprimands, but " + girl->m_Realname + " has finally stopped being so Clumsy.", DayNight != 0);
	return false;
}