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
extern cGold g_Gold;

bool cJobManager::WorkHallXXXEntertainer(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";
	string girlName = girl->m_Realname;
	if(Preprocessing(ACTION_WORKHALL, girl, brothel, DayNight, summary, message))	// they refuse to have work in the hall
		return true;

	// put that shit away, you'll scare off the customers!
	g_Girls.UnequipCombat(girl);

	int roll = g_Dice%100;
	int looks = (g_Girls.GetStat(girl, STAT_CHARISMA) + g_Girls.GetStat(girl, STAT_BEAUTY))/2;
	int jobperformance = (looks + g_Girls.GetSkill(girl, SKILL_SERVICE));
	int wages = 25;

	message = "She worked as a sexual entertainer in the gambling hall.";

	//good traits
	if (g_Girls.HasTrait(girl, "Charismatic"))  //
		jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Sexy Air"))  //
		jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Cool Person"))  //people love to be around her
		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Cute"))  //
		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Charming"))  //people like charming people
		jobperformance += 5;
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
		jobperformance -= 10;
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
			message += " She must be the perfect entertainer customers go on and on about her and always come to see her when she works.\n\n";
			wages += 155;
		if (roll <= 20)
			{
				if (g_Girls.HasTrait(girl, "Nymphomaniac"))
				{
					message += girlName + " could do this all day with a genuine smile on her face, pussy dripping onto the stained boards beneath her legs.\n";
				}
				else
				{
				message += girlName + " has a large devoted group of fans, who come in every night to watch her.\n";
				}
			}
		else if (roll <= 40)
			{
				message +=  "A simple smile towards the audience from " + girlName + " makes every gambler howl and cheer.\n";
			}
		else if (roll <= 60)
			{
				message += "It's amazing how many tricks " + girlName + "knows, to tease her audience but never quite push them over the point where they'd lose interest.\n";
			}
		else if (roll <= 80)
			{
				message += girlName + " stripped in a unique way tonight, dancing with a razor in each hand.  With those razors, she peeled her clothes away from her lovely body, without nicking herself a single time.\n";
			}
		else
			{
				if (g_Girls.HasTrait(girl, "Sexy Air"))
				{
					message += girlName + " could be dressed in a sack and still make a fortune masturbating on stage.\n";
				}
				else
				{
				message +=  girlName + "'s shows are always sold out, and her fame is spreading across the city.\n";
				}
			}
		}
else if (jobperformance >= 185)
		{
			message += " She's unbelievable at this and is always getting praised by the customers for her work.\n\n";
			wages += 95;
		if (roll <= 20)
			{
				if (g_Girls.HasTrait(girl, "Nymphomaniac"))
				{
					message += girlName + " loves her job, you can tell by the zeal she employs as she penetrates her mouth and pussy with dildoes on stage, moaning throughout.\n";
				}
				else
				{
				message += "From start to finish, every move " + girlName + " makes practically sweats sexuallity.\n";
				}
			}
		else if (roll <= 40)
			{
				message += girlName + " moved off the stage halfway through the act and walked amongst the audience, to their joy.\n";
			}
		else if (roll <= 60)
			{
				message += "As clothing slowly fell from her body and her hand descended towards her core, " + girlName + " smiled seductively at the audience.\n";
			}
		else if (roll <= 80)
			{
				message += "All male members of " + girlName + "'s audience had trouble standing after she fellated a dildo on stage.\n";
			}
		else
			{
				if (g_Girls.HasTrait(girl, "Sexy Air"))
				{
					message +=  "The skill of " + girlName + "'s dancing coupled with her sexual air just pack the customers in.\n";
				}
				else
				{
				message +=  "Amusingly, one of the female members of " + girlName + "'s audience threw her panties on the stage, where " + girlName + " used them as an impromptu prop.\n";
				}
			}
		}
else if (jobperformance >= 145)
		{
			message += " She's good at this job and gets praised by the customers often.\n\n";
			wages += 55;
		if (roll <= 20)
			{
				message +=  "A lucky gambler almost fainted when " + girlName + " gave him her freshly removed skirt as a present.\n";
			}
		else if (roll <= 40)
			{
				if (g_Girls.HasTrait(girl, "Long Legs"))
				{
					message += girlName + "'s erotic dances are accentuated by her perfect legs.\n";
				}
				else
				{
				message += girlName + " has a small, but devoted fanbase, who are willing to pay gate fees just to watch her dance.\n";
				}
			}
		else if (roll <= 60)
			{
				message += "After letting " + girlName + " chose the music that she danced to, her erotic dances have improved markedly.\n";
			}
		else if (roll <= 80)
			{
				message += "You watched with amusement as members of her audience made a disproportionate number of trips to the bathroom after her performance.\n";
			}
		else
			{
				if (g_Girls.HasTrait(girl, "Sexy Air"))
				{
					message += girlName + " is by no measure bad at her job, but she gets way more customers then could be expected.\n";
				}
				else
				{
				message +=  "A man made an offer to buy " + girlName + " today, which you turned down.  She's popular with the patrons.\n";
				}
			}
		}
else if (jobperformance >= 100)
		{
			message += " She made a few mistakes but overall she is okay at this.\n\n";
			wages += 15;
		if (roll <= 20)
			{
				if (g_Girls.HasTrait(girl, "Sexy Air"))
				{
					message += girlName + "'s dances are mechanical, but her aurora of sexuality make clients break out in sweats.\n";
				}
				else
				{
				message += girlName + "'s striptease draws a decent crowd each night.\n";
				}
			}
		else if (roll <= 40)
			{
				message += girlName + " is not the best erotic dancer you've ever seen, but the gamblers enjoy the eyecandy.\n";
			}
		else if (roll <= 60)
			{
				message += "The cleaners always hate having to clean up after " + girlName + "'s audience.\n";
			}
		else if (roll <= 80)
			{
				message += girlName + " got a round of applause when she bowed, and an even bigger one when her ";
					if (g_Girls.HasTrait(girl, "Big Boobs") || g_Girls.HasTrait(girl, "Abnormally Large Boobs"))
						message += "large breasts \"accidentally\" flopped out of her top.\n";
					else if (g_Girls.HasTrait(girl, "Small Boobs"))
						message += "shirt \"accidentally\" exposed her small and perkey breasts.\n";
					else
						message += "breasts \"accidentally\" fell out of her top.\n";
			}
		else
			{
				if (g_Girls.HasTrait(girl, "Long Legs"))
				{
					message += "Although " + girlName + " is no great shake at this job, her legs lend her a significant popularity boost.\n";
				}
				else
				{
				message += girlName + " has some talent at this job, but she needs to develop it.\n";
				}
			}
		}
else if (jobperformance >= 70)
		{
			message += " She was nervous and made a few mistakes. She isn't that good at this.\n\n";
			wages -= 5;
		if (roll <= 20)
			{
				if (g_Girls.HasTrait(girl, "Sexy Air"))
				{
					message += "Even though a block of wood could dance better, + " + girlName + " still draws the eye.\n";
				}
				else
				{
				message += girlName + " is still incredibly sexy, despite her lack of anything resembling an active customer appeal.\n";
				}
			}
		else if (roll <= 40)
			{
				message += girlName + "'s distaste for the crowd was evident by her curled lip and perpetual scowl.\n";
			}
		else if (roll <= 60)
			{
				message += "The only person who seemed to find " + girlName + "'s striptease appetizing was a random drunk.\n";
			}
		else if (roll <= 80)
			{
				if (g_Girls.HasTrait(girl, "Long Legs"))
				{
					message += "The perfectly sculpted flesh of her leg drew attention away from the stupid look on " + girlName + "'s face.\n";
				}
				else
				{
				message += girlName + "'s hips twitched in a decidedly half-assed way.\n";
				}
			}
		else
			{
				message += girlName + " isn't qualified as a striper, which might explain why she couldn't successfully detach her bra on stage.\n";
			}
		}
else
		{
			message += " She was nervous and constantly making mistakes. She really isn't very good at this job.\n\n";
			wages -= 15;
		if (roll <= 20)
			{
				if (g_Girls.HasTrait(girl, "Long Legs"))
					{
					message += "Customers just can't tear their eyes away from " + girlName + "'s long, tasty legs, even as she tripped over her discarded top.\n";
					}
				else
				{
				message += girlName + " slipped on a fresh smear of ejaculate halfway through her dance, and fell on her ass.\n";
				}
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
				if (g_Girls.HasTrait(girl, "Sexy Air"))
					{
					message += "Even though a block of wood could dance better, + " + girlName + " still draws the eye.\n";
					}
				else
					{
				message += "Somehow, " + girlName + " managed to be so unsexy that the audience barely seemed to notice her presence.\n";
				}
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
			message += " Her clumsy nature caused her to spill a custmoers drink on them resulting in them storming off without paying.\n";
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
				message += girl->m_Realname + " was in a cheerful mood but the patrons thought she needed to work more on her services.\n";
				wages -= 10;
			}
			else
			{
			message += "Her optimistic mood made patrons cheer up increasing the amount they tip.\n";
			wages += 10;
			}
		}

	if(g_Girls.GetStat(girl, STAT_LIBIDO) > 90)
	{
		message += "She was horney and ended up masturbating for the customers making them very happy.";
		sCustomer cust;
		GetMiscCustomer(brothel, cust);
		brothel->m_Happiness += 100;
		g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -40);

		// work out the pay between the house and the girl
		wages += g_Girls.GetStat(girl, STAT_ASKPRICE)+60;
		girl->m_Events.AddMessage(message, IMGTYPE_MAST, DayNight);
	}

	if(wages < 0)
			wages = 0;

	//enjoyed the work or not
	if(roll <= 5)
	{
		message += " \nSome of the patrons abused her during the shift.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKHALL, -1, true);
	}
	else if(roll <= 25) {
		message += " \nShe had a pleasant time working.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKHALL, +3, true);
	}
	else
	{
		message += " \nOtherwise, the shift passed uneventfully.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKHALL, +1, true);
	}
	girl->m_Events.AddMessage(message, IMGTYPE_ECCHI, DayNight);
	

	// work out the pay between the house and the girl
	wages += (g_Dice%((int)(((g_Girls.GetStat(girl, STAT_BEAUTY)+g_Girls.GetStat(girl, STAT_CHARISMA))/2)*0.5f)))+10;
	girl->m_Pay = wages;
	string pay = "";


	// Improve girl
	int xp = 10, libido = 1, skill = 2;

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

	g_Girls.UpdateStat(girl, STAT_FAME, 1);
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, skill);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	//gain traits
	g_Girls.PossiblyGainNewTrait(girl, "Nymphomaniac", 75, ACTION_WORKHALL, "Having to preform sexual entertainment for patrons every day has made " + girl->m_Realname + " quite the nympho.", DayNight != 0);

	return false;
}
