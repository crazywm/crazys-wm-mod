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

// `J` Brothel Job - Sleazy Bar
bool cJobManager::WorkBarStripper(sGirl* girl, sBrothel* brothel, int Day0Night1, string& summary)
{
	string message = ""; string girlName = girl->m_Realname;

	if(Preprocessing(ACTION_WORKSTRIP, girl, brothel, Day0Night1, summary, message))	// they refuse to have work in the bar
		return true;

	// put that shit away, you'll scare off the customers!
	g_Girls.UnequipCombat(girl);

	int roll = g_Dice%100;
	int jobperformance = (	g_Girls.GetStat(girl, STAT_CHARISMA) / 2 +
							g_Girls.GetStat(girl, STAT_BEAUTY) / 2 +
							g_Girls.GetSkill(girl, SKILL_PERFORMANCE) / 2 +
							g_Girls.GetSkill(girl, SKILL_STRIP) / 2);
	int lapdance = (	g_Girls.GetStat(girl, STAT_INTELLIGENCE)/2 + 
						g_Girls.GetSkill(girl, SKILL_PERFORMANCE)/2 +
						g_Girls.GetSkill(girl, SKILL_STRIP)		)/2;
	//int drinks;

	int wages = 30, work = 0;
	message += "She worked as a stripper in the club.";

	//good traits
	if (g_Girls.HasTrait(girl, "Charismatic"))    jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Sexy Air"))		  jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Cool Person"))    jobperformance += 10;//people love to be around her
	if (g_Girls.HasTrait(girl, "Cute"))			  jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Charming")) 	  jobperformance += 10; //people like charming people
	if (g_Girls.HasTrait(girl, "Great Figure"))   jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Great Arse"))	  jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Quick Learner"))  jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Psychic"))		  jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Long Legs"))	  jobperformance += 10;
	if (g_Girls.GetStat(girl, STAT_FAME) >85)	  jobperformance += 10;

	//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))		jobperformance -= 50; //needs others to do the job	
	if (g_Girls.HasTrait(girl, "Clumsy"))			jobperformance -= 20; //spills food and breaks things often	
	if (g_Girls.HasTrait(girl, "Aggressive"))		jobperformance -= 20; //gets mad easy and may attack people
	if (g_Girls.HasTrait(girl, "Nervous"))			jobperformance -= 30; //don't like to be around people
	if (g_Girls.HasTrait(girl, "Meek"))				jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Shy"))				jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Slow Learner"))		jobperformance -= 10;
	if (g_Girls.HasTrait(girl, "Horrific Scars"))   jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Small Scars"))		jobperformance -= 5;


	if (jobperformance >= 245)
		{
			message += " She must be the perfect stripper, customers go on and on about her and always come to see her when she works.\n\n";
			wages += 155;
		if (roll <= 20)
			{
				message += girlName + " use of the pole amazes the patrons. They truely have no idea how she does the things she does.\n";
				brothel->m_Happiness += 10; brothel->m_Fame += 5;
			}
		else if (roll <= 40)
			{
				message += girlName + "'s throws her bra into the crowd causing a fight to erupt over who got to take it home. They just love her.\n";
				brothel->m_Happiness += 10;
			}
		else if (roll <= 60)
			{
				message += "People push and shove to get the front of the stage for her dance set.\n";
				brothel->m_Fame += 10;
			}
		else if (roll <= 80)
			{
				message += girlName + " smile is enough to bring in the tips. You sometimes wonder if she even needs to take her clothes off.\n";
				brothel->m_Happiness += 10;
				wages += 15;
			}
		else
			{
				message += "Somehow, " + girlName + " managed to be so sexy that you thought you might need to close just to clean up the mess the crowd made.\n";
				brothel->m_Happiness += 10;
				brothel->m_Filthiness += 5;
				wages += 10;
				}
		}
	else if (jobperformance >= 185)
		{
			message += " She's unbelievable at this and is always getting praised by the customers for her work.\n\n";
			wages += 95;
		if (roll <= 20)
			{
				message += girlName + " use of the pole makes every head in the place turn to watch.\n";
				brothel->m_Fame += 5;
			}
		else if (roll <= 40)
			{
				message += girlName + "'s throws her bra into the crowd causing an uproar of hooting and screaming.  She knows how to drive them crazy.\n";
				brothel->m_Happiness += 10;
			}
		else if (roll <= 60)
			{
				message += girlName + " draws just as many females as males to the club when its her shift.  It seems everyone has noticed her skill.\n";
				brothel->m_Fame += 10;
			}
		else if (roll <= 80)
			{
				message += "All male members of " + girlName + "'s audience had trouble standing after she finished her set.\n";
				brothel->m_Happiness += 10;
			}
		else
			{
				message += "From start to finish, every move " + girlName + " makes practically sweats sexuallity.\n";
				brothel->m_Happiness += 10;
				}
		}
	else if (jobperformance >= 145)
		{
			message += " She's good at this job and gets praised by the customers often.\n\n";
			wages += 55;
		if (roll <= 20)
			{
				message += girlName + " can spin on the pole in a way that excites the crowd.\n";
				brothel->m_Happiness += 5;
			}
		else if (roll <= 40)
			{
				message += girlName + "'s sly smile and slow striptease drives the crowd crazy.\n";
				brothel->m_Happiness += 5;
			}
		else if (roll <= 60)
			{
				message += "Seeing the large crowd waiting outside, " + girlName + " smirked and slowly walked on stage knowing she was going get good tips today.\n";
				wages += 10;
			}
		else if (roll <= 80)
			{
				message += "After letting " + girlName + " chose the music that she danced to, her erotic dances have improved markedly.\n";
			}
		else
			{
				message += "You watched with amusement as members of her audience made a disproportionate number of trips to the bathroom after her performance.\n";
				}
		}
	else if (jobperformance >= 100)
		{
			message += " She made a few mistakes but overall she is okay at this.\n\n";
			wages += 15;
		if (roll <= 20)
			{
				message += "While she won't win any contests, " + girlName + " isn't a terrible striper.\n";
			}
		else if (roll <= 40)
			{
				message += girlName + " is not the best erotic dancer you've ever seen, but the drunks enjoy the eyecandy.\n";
			}
		else if (roll <= 60)
			{
				message += girlName + "'s striptease draws a decent crowd each night.\n";
			}
		else if (roll <= 80)
			{
				message += girlName + " is able to use the pole without making any mistakes even if she isn't that great on it.\n";
			}
		else
			{
				message += "Somehow, " + girlName + " managed to get a decent amount of tips as she slowly took her clothes of during her set on stage.\n";
				}
		}
	else if (jobperformance >= 70)
		{
			message += " She was nervous and made a few mistakes. She isn't that good at this.\n\n";
			wages -= 5;
		if (roll <= 20)
			{
				message += girlName + " spinned around and around on the pole getting dizzy causing her to fall after getting of the pole.\n";
				brothel->m_Fame -= 5;
			}
		else if (roll <= 40)
			{
				message += girlName + "'s hair got tangled up in her shirt when she tried to take it off, making her reel about the stage like a drunk.\n";
			}
		else if (roll <= 60)
			{
				message += "For all the reaction she's getting, you could probably replace " + girlName + " with a simple mannequin.\n";
			}
		else if (roll <= 80)
			{
				message += girlName + " atleast got all her clothes off this time.\n";
			}
		else
			{
				message += girlName + " isn't qualified as a striper, which might explain why she couldn't successfully detach her bra on stage.\n";
				brothel->m_Happiness -= 10;
				}
		}
	else
		{
			message += " She was nervous and constantly making mistakes. She really isn't very good at this job.\n\n";
			wages -= 15;
		if (roll <= 20)
			{
				message += girlName + " slipped of the pole and fell flat on her ass.\n";
			}
		else if (roll <= 40)
			{
				message += girlName + "'s hair got tangled up in her shirt when she tried to take it off, making her reel about the stage like a drunk.\n";
				brothel->m_Fame -= 10;
				brothel->m_Happiness += 5;  //would be funny to see
			}
		else if (roll <= 60)
			{
				message += "Seeing the large crowd waiting outside, " + girlName + "'s nerve broke and she wouldn't take the stage.\n";
			}
		else if (roll <= 80)
			{
				message += "Somehow, " + girlName + " forgot that she was a stripper and finished her set with her clothes still on.\n";
				brothel->m_Fame -= 5;
			}
		else
			{
				message += "Somehow, " + girlName + " managed to be so unsexy that the audience barely seemed to notice her presence.\n";
				brothel->m_Happiness -= 5;
				}
		}


	//try and add randomness here
	if (g_Girls.GetStat(girl, STAT_BEAUTY) >85 && g_Dice.percent(20))
	{ message += "Stunned by her beauty a customer left her a great tip.\n\n"; wages += 25; }

	if (g_Girls.HasTrait(girl, "Clumsy") && g_Dice.percent(5))
		{ message += " Her clumsy nature caused her to slide off the pole causing her to have to stop stripping for a few hours.\n"; wages -= 15; }

	if (g_Girls.HasTrait(girl, "Pessimist") && g_Dice.percent(5))
	{
		if (jobperformance < 125)
			{ message += " Her pessimistic mood depressed the customers making them tip less.\n"; wages -= 10; }
		else
			{ message += girlName + " was in a poor mood so the patrons gave her a bigger tip to try and cheer her up.\n"; wages += 10; }
	}

	if (g_Girls.HasTrait(girl, "Optimist") && g_Dice.percent(5))
	{
		if (jobperformance < 125)
			{ message += girlName + " was in a cheerful mood but the patrons thought she needed to work more on her stripping.\n"; wages -= 10; }
		else
			{ message += " Her optimistic mood made patrons cheer up increasing the amount they tip.\n"; wages += 10; }
	}

	if (g_Girls.HasTrait(girl, "Great Figure") && g_Dice.percent(20))
	{
		if (jobperformance < 125)
			{ message += girlName + " has a great figure so she draws a few extra patrons even if she needed to work more on her stripping.\n"; wages += 5; }
		else
			{ message += girlName + "'s great figure draws a large crowed to the stage and her skill at stripping makes them pay up to see the show up close.\n"; wages += 15; }
	}
		

	// lap dance code.. just test stuff for now
	if (lapdance >= 90)
		{
			message += girlName + " doesn't have to try to sell private dances the patrons beg her to buy one off her.\n";
			if (roll < 5)
				{
					message += "She sold a champagne dance.";
					wages += 250;
				}
			if (roll < 20)
				{
					message += "She sold a shower dance.";
					wages += 125;
				}
			if (roll < 40)
				{
					message += "She was able to sell a few VIP dances.";
					wages += 160;
				}
			if (roll < 60)
				{
					message += "She sold a VIP dance.";
					wages += 75;
				}
			else
				{
					message += "She sold several lap dances.";
					wages += 85;
				}
			}
	else if (lapdance >= 65)
		{
			message += girlName + "'s skill at selling private dances is impressive.\n";
			if (roll < 5)
				{
					message += "She convinced a patron to buy a shower dance.";
					wages += 75;
				}
			if (roll < 20)
				{
					message += "Sold a VIP dance to a patron.";
					wages += 70;
				}
			else
				{
					message += "Sold a few lap dance.";
					wages += 65;
				}
			}
	else if (lapdance >= 40)
		{
			message += girlName + " tried to sell private dances and ";
			if (roll < 5)
				{
					message += "was able to sell a vip dance againts all odds.";
					wages += 50;
				}
			if (roll < 20)
				{
					message += "was able to sell a lap dance.";
					wages += 25;
				}
			else
				{
					message += "wasn't able to sell any.";
				}
			}
	else 
		{
			message += girlName + "'s doesn't seem to understand the real money in stripping is selling private dances.\n";
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


	g_Girls.UpdateEnjoyment(girl, ACTION_WORKSTRIP, work , true);
	girl->m_Events.AddMessage(message, IMGTYPE_STRIP, Day0Night1);


	int roll_max = (g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_CHARISMA));
	roll_max /= 4;
	wages += 10 + g_Dice%roll_max;
	girl->m_Pay = wages;

	// Improve stats
	int xp = 15, libido = 1, skill = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_FAME, 1);
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_PERFORMANCE, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_STRIP, g_Dice%skill + 2);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	//gained
	g_Girls.PossiblyGainNewTrait(girl, "Sexy Air", 70, ACTION_WORKSTRIP, girlName + " has been stripping and having to be sexy for so long she now reeks of sexyness.", Day0Night1 == SHIFT_NIGHT);

	//lose
	g_Girls.PossiblyLoseExistingTrait(girl, "Nervous", 20, ACTION_WORKSTRIP, girlName + " has had so many people see her naked she is no longer nervous about anything.", Day0Night1 == SHIFT_NIGHT);
	if (jobperformance > 150 && g_Girls.GetStat(girl, STAT_CONFIDENCE) > 65) { g_Girls.PossiblyLoseExistingTrait(girl, "Shy", 60, ACTION_WORKSTRIP, girlName + " has been stripping for so long now that her confidence is super high and she is no longer Shy.", Day0Night1 == SHIFT_NIGHT); }

	return false;
}