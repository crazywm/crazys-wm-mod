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

bool cJobManager::WorkBarStripper(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";
	string girlName = girl->m_Realname;
	int tex = g_Dice%4;

	if(Preprocessing(ACTION_WORKCLUB, girl, brothel, DayNight, summary, message))	// they refuse to have work in the bar
		return true;

	// put that shit away, you'll scare off the customers!
	g_Girls.UnequipCombat(girl);

	int roll = g_Dice%100;
	int looks = (g_Girls.GetStat(girl, STAT_CHARISMA) + g_Girls.GetStat(girl, STAT_BEAUTY))/2;
	int jobperformance = (looks + g_Girls.GetSkill(girl, SKILL_STRIP));
	int lapdance = (g_Girls.GetStat(girl, STAT_INTELLIGENCE) + g_Girls.GetSkill(girl, SKILL_STRIP))/2;
	//int drinks;

	int wages = 30;
	message += "She worked as a stripper in the club.";

	//good traits
	if (g_Girls.HasTrait(girl, "Charismatic"))  //
		jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Sexy Air"))  //
		jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Cool Person"))  //people love to be around her
		jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Cute"))  //
		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Charming"))  //people like charming people
		jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Great Figure"))  //
		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Great Arse"))  //
		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Quick Learner"))  
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


	if (jobperformance >= 245)
		{
			message += " She must be the perfect stripper customers go on and on about her and always come to see her when she works.\n\n";
			wages += 155;
		/*if (roll <= 20)
			{
				message += girlName + " slipped on a fresh smear of ejaculate halfway through her dance, and fell on her ass.\n";
			}
		else if (roll <= 40)
			{
				message += girlName + "'s hair got tangled up in her shirt when she tried to take it off, making her reel about the stage like a drunk.\n";
			}
		else if (roll <= 60)
			{
				message += "Seeing the large crowd waiting outside, " + girlName + "'s nerve broke and she ran back out of the gambling hall.\n";
			}
		else if (roll <= 80)
			{
				message += girlName + " was drunk when she stumbled out at the beginning of shift, and fell asleep almost as soon as she reached the stage.\n";
			}
		else
			{
				message += "Somehow, " + girlName + " managed to be so unsexy that the audience barely seemed to notice her presence.\n";
				}*/
		}
	else if (jobperformance >= 185)
		{
			message += " She's unbelievable at this and is always getting praised by the customers for her work.\n\n";
			wages += 95;
		/*if (roll <= 20)
			{
				message += girlName + " slipped on a fresh smear of ejaculate halfway through her dance, and fell on her ass.\n";
			}
		else if (roll <= 40)
			{
				message += girlName + "'s hair got tangled up in her shirt when she tried to take it off, making her reel about the stage like a drunk.\n";
			}
		else if (roll <= 60)
			{
				message += "Seeing the large crowd waiting outside, " + girlName + "'s nerve broke and she ran back out of the gambling hall.\n";
			}
		else if (roll <= 80)
			{
				message += girlName + " was drunk when she stumbled out at the beginning of shift, and fell asleep almost as soon as she reached the stage.\n";
			}
		else
			{
				message += "Somehow, " + girlName + " managed to be so unsexy that the audience barely seemed to notice her presence.\n";
				}*/
		}
	else if (jobperformance >= 145)
		{
			message += " She's good at this job and gets praised by the customers often.\n\n";
			wages += 55;
		/*if (roll <= 20)
			{
				message += girlName + " slipped on a fresh smear of ejaculate halfway through her dance, and fell on her ass.\n";
			}
		else if (roll <= 40)
			{
				message += girlName + "'s hair got tangled up in her shirt when she tried to take it off, making her reel about the stage like a drunk.\n";
			}
		else if (roll <= 60)
			{
				message += "Seeing the large crowd waiting outside, " + girlName + "'s nerve broke and she ran back out of the gambling hall.\n";
			}
		else if (roll <= 80)
			{
				message += girlName + " was drunk when she stumbled out at the beginning of shift, and fell asleep almost as soon as she reached the stage.\n";
			}
		else
			{
				message += "Somehow, " + girlName + " managed to be so unsexy that the audience barely seemed to notice her presence.\n";
				}*/
		}
	else if (jobperformance >= 100)
		{
			message += " She made a few mistakes but overall she is okay at this.\n\n";
			wages += 15;
		/*if (roll <= 20)
			{
				message += girlName + " slipped on a fresh smear of ejaculate halfway through her dance, and fell on her ass.\n";
			}
		else if (roll <= 40)
			{
				message += girlName + "'s hair got tangled up in her shirt when she tried to take it off, making her reel about the stage like a drunk.\n";
			}
		else if (roll <= 60)
			{
				message += "Seeing the large crowd waiting outside, " + girlName + "'s nerve broke and she ran back out of the gambling hall.\n";
			}
		else if (roll <= 80)
			{
				message += girlName + " was drunk when she stumbled out at the beginning of shift, and fell asleep almost as soon as she reached the stage.\n";
			}
		else
			{
				message += "Somehow, " + girlName + " managed to be so unsexy that the audience barely seemed to notice her presence.\n";
				}*/
		}
	else if (jobperformance >= 70)
		{
			message += " She was nervous and made a few mistakes. She isn't that good at this.\n\n";
			wages -= 5;
		/*if (roll <= 20)
			{
				message += girlName + " slipped on a fresh smear of ejaculate halfway through her dance, and fell on her ass.\n";
			}
		else if (roll <= 40)
			{
				message += girlName + "'s hair got tangled up in her shirt when she tried to take it off, making her reel about the stage like a drunk.\n";
			}
		else if (roll <= 60)
			{
				message += "Seeing the large crowd waiting outside, " + girlName + "'s nerve broke and she ran back out of the gambling hall.\n";
			}
		else if (roll <= 80)
			{
				message += girlName + " was drunk when she stumbled out at the beginning of shift, and fell asleep almost as soon as she reached the stage.\n";
			}
		else
			{
				message += "Somehow, " + girlName + " managed to be so unsexy that the audience barely seemed to notice her presence.\n";
				}*/
		}
	else
		{
			message += " She was nervous and constantly making mistakes. She really isn't very good at this job.\n\n";
			wages -= 15;
		/*if (roll <= 20)
			{
				message += girlName + " slipped on a fresh smear of ejaculate halfway through her dance, and fell on her ass.\n";
			}
		else if (roll <= 40)
			{
				message += girlName + "'s hair got tangled up in her shirt when she tried to take it off, making her reel about the stage like a drunk.\n";
			}
		else if (roll <= 60)
			{
				message += "Seeing the large crowd waiting outside, " + girlName + "'s nerve broke and she ran back out of the gambling hall.\n";
			}
		else if (roll <= 80)
			{
				message += girlName + " was drunk when she stumbled out at the beginning of shift, and fell asleep almost as soon as she reached the stage.\n";
			}
		else
			{
				message += "Somehow, " + girlName + " managed to be so unsexy that the audience barely seemed to notice her presence.\n";
				}*/
		}


	//try and add randomness here
	if (g_Girls.GetStat(girl, STAT_BEAUTY) >85)
		if((g_Dice%100) < 15)
		{
			message += " Stunned by her beauty a customer left her a great tip.\n";
			wages += 15;
		}

	if (g_Girls.HasTrait(girl, "Clumsy"))
		if((g_Dice%100) < 5)
		{
			message += " Her clumsy nature caused her to slide off the pole causing her to have to stop stripping for a few hours.\n";
			wages -= 15;
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
				message += girl->m_Realname + " was in a cheerful mood but the patrons thought she needed to work more on her stripping.\n";
				wages -= 10;
			}
			else
			{
			message += " Her optimistic mood made patrons cheer up increasing the amount they tip.\n";
			wages += 10;
			}
		}

	if (g_Girls.HasTrait(girl, "Great Figure"))
		if((g_Dice%100) < 20)
		{
			if(jobperformance < 125)
			{
				message += girl->m_Realname + " has a great figure so she draws a few extra patrons even if she needed to work more on her stripping.\n";
				wages += 5;
			}
			else
			{
			message += girl->m_Realname + "'s great figure draws a large crowed to the stage and her skill at stripping makes them pay up to see the show up close.\n";
			wages += 15;
			}
		}
		

	// lap dance code.. just test stuff for now
	if (lapdance >= 100)
		{
			message += girl->m_Realname + " doesn't try to sell private dances the patrons beg her to buy one off her.\n";
			if (roll < 5)
				{
		message += "She sold a champagne dance.";
		}
			if (roll < 20)
				{
		message += "She sold a shower dance.";
		}
			if (roll < 40)
				{
		message += "She was able to sell a few VIP dances.";
		}
			if (roll < 60)
				{
		message += "She sold a VIP dance.";
		}
			else
				{
		message += "She sold several lap dances.";
		}
	}
	else if (lapdance >= 75)
		{
			message += girl->m_Realname + "'s skill at selling private dances is impressive.\n";
			if (roll < 5)
				{
		message += "She convinced a patron to buy a shower dance.";
		}
			if (roll < 20)
				{
		message += "Sold a VIP dance to a patron.";
		}
			else
				{
		message += "Sold a few lap dance.";
		}
	}
	else if (lapdance >= 50)
		{
			message += girl->m_Realname + " tried to sell private dances and ";
			if (roll < 5)
				{
		message += "was able to sell a vip dance againts all odds.";
		}
			if (roll < 20)
				{
		message += "was able to sell a lap dance.";
		}
			else
				{
		message += "wasn't able to sell any.";
		}
	}
	else 
		{
			message += girl->m_Realname + "'s doesn't seem to understand the real money in stripping is selling private dances.\n";
	}

		if(wages < 0)
			wages = 0;



	//enjoyed the work or not
	if(roll <= 5)
	{
		message += " \nSome of the patrons abused her during the shift.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKCLUB, -1, true);
	}
	else if(roll <= 25) {
		message += " \nShe had a pleasant time working.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKCLUB, +3, true);
	}
	else
	{
		message += " \nOtherwise, the shift passed uneventfully.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKCLUB, +1, true);
	}

	girl->m_Events.AddMessage(message, IMGTYPE_STRIP, DayNight);


	int roll_max = (g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_CHARISMA));
	roll_max /= 4;
	wages += 10 + g_Dice%roll_max;
	girl->m_Pay = wages;

	// Improve stats
	int xp = 15, libido = 1, skill = 3;

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
	g_Girls.UpdateSkill(girl, SKILL_STRIP, skill);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	//gained
	g_Girls.PossiblyGainNewTrait(girl, "Sexy Air", 70, ACTION_WORKCLUB, girl->m_Realname + " has been stripping and having to be sexy for so long she now reeks of sexyness.", DayNight != 0);

	//lose
	g_Girls.PossiblyLoseExistingTrait(girl, "Nervous", 20, ACTION_WORKCLUB, girl->m_Realname + " has had so many people see her naked she is no longer nervous about anything.", DayNight != 0);

	return false;
}