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
#include "libintl.h"

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;

bool cJobManager::WorkBarmaid(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";
	string girlName = girl->m_Realname;
	int tex = g_Dice%4;

	if(Preprocessing(ACTION_WORKBAR, girl, brothel, DayNight, summary, message))	// they refuse to have work in the bar
		return true;

	// put that shit away, you'll scare off the customers!
	g_Girls.UnequipCombat(girl);

	int wages = 15;
	message += "She worked as a barmaid.";

	int roll = g_Dice%100;
	int jobperformance = (g_Girls.GetStat(girl, STAT_INTELLIGENCE) + g_Girls.GetSkill(girl, SKILL_SERVICE));

	//good traits
	if (g_Girls.HasTrait(girl, "Charismatic"))  //
		jobperformance += 20;
	if (g_Girls.HasTrait(girl, "Sexy Air"))  //
		jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Cool Person"))  //people love to be around her
		jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Cute"))  //
		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Charming"))  //people like charming people
		jobperformance += 15;
	if (g_Girls.HasTrait(girl, "Quick Learner"))  //people like charming people
		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Psychic"))
		jobperformance += 10;

	//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))  //needs others to do the job
		jobperformance -= 50;
	if (g_Girls.HasTrait(girl, "Clumsy"))  //spills food and breaks things often
		jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Aggressive"))  //gets mad easy and may attack people
		jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Nervous"))  //don't like to be around people
		jobperformance -= 30;
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
				message += girlName + " was giving orders to the wrong patrons and letting a lot people walk out without paying there tab.\n";
			}
			else
			{
				message += girlName + " spilled drinks all over the place and mixed the wrong stuff when trying to make drinks for people.\n";
			}
		}
	else if(jobperformance < 70)
		{
			message += " She was nervous and made a few mistakes. She isn't that good at this.\n\n";
			wages -= 5;
		if (roll <= 50)
			{
				message += girlName + " mixed up peoples drink orders..  When she only had four patrons drinking.\n";
			}
			else
			{
				message += girlName + " gave someone a drink she mixed that made them sick.  It was nothing but coke and ice so who knows how she did it.\n";
			}
		}
	else if(jobperformance < 100)
		{
			message += " She made a few mistakes but overall she is okay at this.\n\n";
			wages += 15;
		if (roll <= 50)
			{
				message += girlName + " mixed up a few drink orders..  But they order the same drink so it didn't matter to much.\n";
			}
			else
			{
				message += girlName + " wasted a few drinks by forgetting to ice them but wasn't major.\n";
			}
		}
	else if(jobperformance < 145)
		{
			message += " She's good at this job and gets praised by the customers often.\n\n";
			wages += 55;
		if (roll <= 50)
			{
				message += girlName + " didn't mix up any orders and kept the patrons drunk and happy.\n";
			}
			else
			{
				message += girlName + " had some regualers come in.  She knows just how to keep them happy and spending gold.\n";
			}
		}
	else if(jobperformance < 185)
		{
			message += " She's unbelievable at this and is always getting praised by the customers for her work.\n\n";
			wages += 95;
		if (roll <= 50)
			{
				message += girlName + " had the bar filled with happy drunks.  She didn't miss a beat all shift.\n";
			}
			else
			{
				message += "People love seeing " + girl->m_Realname + " work and they pour into the bar during her shift.  She mixes wonderful drinks and doesn't mess orders up so they couldn't be happier.\n";
			}
		}
	else if(jobperformance < 245)
		{
			message += " She must be the perfect bar tender customers go on and on about her and always come to see her when she works.\n\n";
			wages += 155;
		if (roll <= 50)
			{
				message += girlName + " was sliding drinks all over the bar without spilling a drop she put on quite a show for the patrons.\n";
			}
			else
			{
				message += girlName + " mixed up what some patrons called the perfect drink.  It got them drunk faster then anything they had before.\n";
			}
		}


	//try and add randomness here
	if (g_Girls.GetStat(girl, STAT_BEAUTY) >85)
		if((g_Dice%101) < 20)
		{
			message += " Stunned by her beauty a customer left her a great tip.\n\n";
			wages += 25;
		}

	if (g_Girls.HasTrait(girl, "Clumsy"))
		if((g_Dice%101) < 15)
		{
			message += " Her clumsy nature caused her to spill a drink on a custmoer resulting in them storming off without paying.\n";
			wages -= 15;
		}

	if (g_Girls.HasTrait(girl, "Pessimist"))
		if((g_Dice%101) < 5)
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
		if((g_Dice%101) < 5)
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

	if (g_Girls.HasTrait(girl, "Big Boobs") || g_Girls.HasTrait(girl, "Abnormally Large Boobs"))
		if((g_Dice%101) < 15)
		{
			if(jobperformance < 150)
			{
				message += " A patron was staring obviously at her large breasts. But she had no ideal how to take advantage of it.\n";
			}
			else
			{
				message += " A patron was staring obviously at her large breasts. So she over charged them for drinks while they drooled not paying any mind to the price.\n";
				wages += 15;
			}
		}

	if (g_Girls.HasTrait(girl, "Psychic"))
		if((g_Dice%101) < 20)
		{
			message += "She used her Psychic skills to know excatally what the patrons wanted to order and when to refill there mugs keeping them happy and increasing tips.\n";
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
				message += " A patron pissed her off but she was able to keep her cool as she is getting use to this kinda thing.\n";
			}
		}

	if (g_Girls.HasTrait(girl, "Horrific Scars"))
		if((g_Dice%101) < 15)
		{
			if(jobperformance < 150)
			{
				message += " A patron gasped at her Horrific Scars making her sad.  But they didn't feel sorry for her.\n";
			}
			else
			{
				message += " A patron gasped at her Horrific Scars making her sad.  Feeling bad about it as she did a wonderful job they left a good tip.\n";
				wages += 15;
			}
		}

		if(wages < 0)
			wages = 0;



	/*if (roll <= 50 && g_Girls.DisobeyCheck(girl, ACTION_WORKBAR, brothel))
	{
		message = girl->m_Realname + gettext(" refused to as a barmaid today.");
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else if(roll <= 15) {
		message += gettext(" \nSome of the patrons abused her during the shift.");
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKBAR, -1, true);
	}
	else if(roll >=90)
	{
		message += gettext(" \nShe had a pleasant time working.");
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKBAR, +3, true);
	}
	else
	{
		message += gettext(" \nOtherwise, the shift passed uneventfully.");
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKBAR, +1, true);
	}*/



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
	int roll_max = (g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetSkill(girl, SKILL_SERVICE));
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
	g_Girls.PossiblyGainNewTrait(girl, "Charismatic", 60, ACTION_WORKBAR, "Dealing with customers at the bar and talking with them about their problems has made " + girl->m_Realname + " more Charismatic.", DayNight != 0);

	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Nervous", 20, ACTION_WORKBAR, girl->m_Realname + " seems to finally be getting over her shyness. She's not always so Nervous anymore.", DayNight != 0);
	return false;
}