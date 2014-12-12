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

// `J` Brothel Job - Brothel
bool cJobManager::WorkBrothelStripper(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	string message = ""; string girlName = girl->m_Realname;
	if(Preprocessing(ACTION_WORKSTRIP, girl, brothel, Day0Night1, summary, message))
		return true;

	// put that shit away, you'll scare off the customers!
	g_Girls.UnequipCombat(girl);

	int roll = g_Dice%100;
	int jobperformance = (	g_Girls.GetStat(girl, STAT_CHARISMA) / 4 + 
							g_Girls.GetStat(girl, STAT_BEAUTY) / 4 +
							g_Girls.GetSkill(girl, SKILL_PERFORMANCE) / 2 +
							g_Girls.GetSkill(girl, SKILL_STRIP));
	int lapdance = (g_Girls.GetStat(girl, STAT_INTELLIGENCE) / 2 +
					g_Girls.GetSkill(girl, SKILL_PERFORMANCE) / 2 +
					g_Girls.GetSkill(girl, SKILL_STRIP)) / 2;
	int mast = false, sex = false;
	int wages = 45, work = 0;

	message = "She stripped for a customer.";

	//good traits
	if (g_Girls.HasTrait(girl, "Charismatic"))   jobperformance += 15;
	if (g_Girls.HasTrait(girl, "Sexy Air"))		 jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Cool Person"))   jobperformance += 10; //people love to be around her
	if (g_Girls.HasTrait(girl, "Cute"))			 jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Charming"))		 jobperformance += 10; //people like charming people
	if (g_Girls.HasTrait(girl, "Great Figure"))	 jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Great Arse"))	 jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Quick Learner")) jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Psychic"))		 jobperformance += 10; //knows what people want
	if (g_Girls.HasTrait(girl, "Long Legs"))	 jobperformance += 10;
	if (g_Girls.GetStat(girl, STAT_FAME) >85)	 jobperformance += 10; //more people willing to see her
		

	//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))		jobperformance -= 50; //needs others to do the job
	if (g_Girls.HasTrait(girl, "Clumsy"))			jobperformance -= 20; //spills food and breaks things often
	if (g_Girls.HasTrait(girl, "Aggressive"))		jobperformance -= 20; //gets mad easy and may attack people
	if (g_Girls.HasTrait(girl, "Nervous"))			jobperformance -= 30; //don't like to be around people
	if (g_Girls.HasTrait(girl, "Meek"))				jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Shy"))				jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Slow Learner"))		jobperformance -= 10;
	if (g_Girls.HasTrait(girl, "Horrific Scars"))	jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Small Scars"))		jobperformance -= 5;


	if (jobperformance >= 245)
		{
			message += " She must be the perfect stripper customers go on and on about her and always come to see her when she works.\n\n";
			wages += 155;
		}
	else if (jobperformance >= 185)
		{
			message += " She's unbelievable at this and is always getting praised by the customers for her work.\n\n";
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
					message += "She sold a shower dance.\n";
					wages += 125;
					if(g_Girls.GetStat(girl, STAT_LIBIDO) > 70)
						{
							message += "She was in the mood so she put on quite a show for them.";
							g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -20);
							wages += 50; mast = true;
						}
				}
			if (roll < 40)
				{
					message += "She was able to sell a few VIP dances.\n";
					wages += 160;
					if (g_Dice.percent(20))
						{ sex = true; }
				}
			if (roll < 60)
				{
					message += "She sold a VIP dance.\n";
					wages += 75;
					if (g_Dice.percent(15))
						{ sex = true; }
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
			if (roll < 10)
				{
					message += "She convinced a patron to buy a shower dance.\n";
					wages += 75;
					if(g_Girls.GetStat(girl, STAT_LIBIDO) > 70)
						{
							message += "She was in the mood so she put on quite a show for them.";
							g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -20);
							wages += 50; mast = true;
						}
				}
			if (roll < 40)
				{
					message += "Sold a VIP dance to a patron.\n";
					wages += 75;
					if (g_Dice.percent(20))
					{ sex = true; }
				}
			else
				{ message += "Sold a few lap dance."; wages += 65; }
				}
	else if (lapdance >= 40)
			{
				message += girlName + " tried to sell private dances and ";
				if (roll < 5)
						{
							message += "was able to sell a vip dance againts all odds.\n"; wages += 75;
							if (g_Dice.percent(10))
							{ sex = true; }
						}
					if (roll < 20)
						{ message += "was able to sell a lap dance."; wages += 25; }
					else
						{ message += "wasn't able to sell any."; }
			}
		else 
				{ message += girlName + "'s doesn't seem to understand the real money in stripping is selling private dances.\n"; }

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


	if (sex)
	{
		u_int n;
		message += "She stripped for and ended up ";
		sCustomer cust;
		brothel->m_Happiness += 100;
		GetMiscCustomer(brothel, cust);
		int imageType = IMGTYPE_SEX;
		if (cust.m_IsWoman) n = SKILL_LESBIAN, message += "licking the customer pussy until she got off";
		else
		{
			switch (g_Dice % 10)
			{
			case 0:        n = SKILL_ORALSEX;   message += "sucking the customer off";					break;
			case 1:        n = SKILL_TITTYSEX;  message += "using her tits to get the customer off";    break;
			case 2:        n = SKILL_HANDJOB;   message += "using her hand to get the customer off";    break;
			case 3:        n = SKILL_ANAL;      message += "letting the customer use her ass";			break;
			case 4:        n = SKILL_FOOTJOB;   message += "using her feet to get the customer off";    break;
			default:	   n = SKILL_NORMALSEX; message += "fucking the customer as well";				break;
			}
		}
		/* */if (n == SKILL_LESBIAN)	imageType = IMGTYPE_LESBIAN;
		else if (n == SKILL_ORALSEX)	imageType = IMGTYPE_ORAL;
		else if (n == SKILL_TITTYSEX)	imageType = IMGTYPE_TITTY;
		else if (n == SKILL_HANDJOB)	imageType = IMGTYPE_HAND;
		else if (n == SKILL_FOOTJOB)	imageType = IMGTYPE_FOOT;
		else if (n == SKILL_ANAL)		imageType = IMGTYPE_ANAL;
		else if (n == SKILL_NORMALSEX)	imageType = IMGTYPE_SEX;
		g_Girls.UpdateSkill(girl, n, 2);
		g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -25);
		message += ", making them very happy.\n";
		g_Girls.UpdateEnjoyment(girl, ACTION_SEX, +1, true);
		// work out the pay between the house and the girl
		wages += g_Girls.GetStat(girl, STAT_ASKPRICE);
		int roll_max = (g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_CHARISMA));
		roll_max /= 4;
		wages += 50 + g_Dice%roll_max;
		girl->m_Pay = wages;
		girl->m_Events.AddMessage(message, imageType, Day0Night1);
	}
	else if (mast) 
	{
		brothel->m_Happiness += (g_Dice%70)+60;
		// work out the pay between the house and the girl
		int roll_max = (g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_CHARISMA));
		roll_max /= 4;
		wages += 50 + g_Dice%roll_max;
		girl->m_Pay = wages;
		girl->m_Events.AddMessage(message, IMGTYPE_MAST, Day0Night1);
	}
	else
	{
		brothel->m_Happiness += (g_Dice%70)+30;
		// work out the pay between the house and the girl
		int roll_max = (g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_CHARISMA));
		roll_max /= 4;
		wages += 10 + g_Dice%roll_max;
		girl->m_Pay = wages;
		girl->m_Events.AddMessage(message, IMGTYPE_STRIP, Day0Night1);
	}
	

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
	g_Girls.PossiblyGainNewTrait(girl, "Sexy Air", 80, ACTION_WORKSTRIP, girlName + " has been stripping and having to be sexy for so long she now reeks of sexyness.", Day0Night1 == SHIFT_NIGHT);

	//lose
	g_Girls.PossiblyLoseExistingTrait(girl, "Nervous", 30, ACTION_WORKSTRIP, girlName + " has had so many people see her naked she is no longer nervous about anything.", Day0Night1 == SHIFT_NIGHT);
	if (jobperformance > 150 && g_Girls.GetStat(girl, STAT_CONFIDENCE) > 65) { g_Girls.PossiblyLoseExistingTrait(girl, "Shy", 60, ACTION_WORKSTRIP, girlName + " has been stripping for so long now that her confidence is super high and she is no longer Shy.", Day0Night1 == SHIFT_NIGHT); }

	return false;
}
