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

bool cJobManager::WorkBrothelStripper(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";
	if(Preprocessing(ACTION_SEX, girl, brothel, DayNight, summary, message))
		return true;

	// put that shit away, you'll scare off the customers!
	g_Girls.UnequipCombat(girl);

	int roll = g_Dice%100;
	int looks = (g_Girls.GetStat(girl, STAT_CHARISMA) + g_Girls.GetStat(girl, STAT_BEAUTY))/2;
	int jobperformance = (looks + g_Girls.GetSkill(girl, SKILL_STRIP));
	int lapdance = (g_Girls.GetStat(girl, STAT_INTELLIGENCE) + g_Girls.GetSkill(girl, SKILL_STRIP))/2;
	int mast = false;
	int sex = false;
	int wages = 30;

	message = "She stripped for a customer.";

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
		jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Great Figure"))  //
		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Great Arse"))  //
		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Quick Learner")) 
		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Psychic"))  //knows what people want to hear
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
	if (g_Girls.HasTrait(girl, "Horrific Scars"))
		jobperformance -= 20;


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
	if (lapdance >= 100)
		{
			message += girl->m_Realname + " doesn't have to try to sell private dances the patrons beg her to buy one off her.\n";
			if (roll < 5)
				{
					message += "She sold a champagne dance.";
					wages += 250;
				}
			if (roll < 20)
				{
					message += "She sold a shower dance.";
					wages += 75;
					if(g_Girls.GetStat(girl, STAT_LIBIDO) > 70)
						{
							message += "She was in the mood so she put on quite a show for them.";
							g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -40);
							wages += 50;
							mast = true;
							}
				}
			if (roll < 40)
				{
					message += "She was able to sell a few VIP dances.";
					wages += 75;
				}
			if (roll < 60)
				{
					message += "She sold a VIP dance.";
					wages += 50;
				}
			else
				{
				message += "She sold several lap dances.";
				wages += 50;
			}
		}
	else if (lapdance >= 75)
			{
				message += girl->m_Realname + "'s skill at selling private dances is impressive.\n";
			if (roll < 5)
				{
					message += "She convinced a patron to buy a shower dance.";
					wages += 75;
					if(g_Girls.GetStat(girl, STAT_LIBIDO) > 70)
						{
							message += "She was in the mood so she put on quite a show for them.";
							g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -40);
							wages += 50;
							mast = true;
							}
						}
			if (roll < 20)
				{
					message += "Sold a VIP dance to a patron.";
					{
	/*u_int action = (roll <7);
	if(action == 0)
	{
		u_int n = 0;
		message += "She stripped for and ended up fucking the customer as well, making them very happy.\n\n";
		sCustomer cust;
		while (true)
		{
		GetMiscCustomer(brothel, cust);
		if (is_sex_type_allowed(cust.m_SexPref, brothel))
			break;
		}
		g_Girls.GirlFucks(girl, DayNight, &cust, false,message,n);
		brothel->m_Happiness += 100;
		int imageType = IMGTYPE_SEX;
		if(n == SKILL_ANAL)
			imageType = IMGTYPE_ANAL;
		else if(n == SKILL_BDSM)
			imageType = IMGTYPE_BDSM;
		else if(n == SKILL_NORMALSEX)
			imageType = IMGTYPE_SEX;
		else if(n == SKILL_BEASTIALITY)
			imageType = IMGTYPE_BEAST;
		else if(n == SKILL_GROUP)
			imageType = IMGTYPE_GROUP;
		else if(n == SKILL_LESBIAN)
			imageType = IMGTYPE_LESBIAN;
		else if(n == SKILL_ORALSEX)
			imageType = IMGTYPE_ORAL;
		else if(n == SKILL_TITTYSEX)
			imageType = IMGTYPE_TITTY;
		g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -2);
		g_Girls.UpdateEnjoyment(girl, ACTION_SEX, +3, true);
		sex = true;
							}*/
						wages += 25;

						}
			}
			else
				{
					message += "Sold a few lap dance.";
					wages += 25;
					}
				}
	else if (lapdance >= 50)
				{
					message += girl->m_Realname + " tried to sell private dances and ";
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
						message += girl->m_Realname + "'s doesn't seem to understand the real money in stripping is selling private dances.\n";
				
		}

	u_int action = g_Dice%7;
	if(action == 0)
	{
		u_int n = 0;
		message += "She stripped for and ended up fucking the customer as well, making them very happy.\n\n";
		sCustomer cust;
		while (true)
		{
		GetMiscCustomer(brothel, cust);
		if (is_sex_type_allowed(cust.m_SexPref, brothel))
			break;
		}
		g_Girls.GirlFucks(girl, DayNight, &cust, false,message,n);
		brothel->m_Happiness += 100;
		int imageType = IMGTYPE_SEX;
		if(n == SKILL_ANAL)
			imageType = IMGTYPE_ANAL;
		else if(n == SKILL_BDSM)
			imageType = IMGTYPE_BDSM;
		else if(n == SKILL_NORMALSEX)
			imageType = IMGTYPE_SEX;
		else if(n == SKILL_BEASTIALITY)
			imageType = IMGTYPE_BEAST;
		else if(n == SKILL_GROUP)
			imageType = IMGTYPE_GROUP;
		else if(n == SKILL_LESBIAN)
			imageType = IMGTYPE_LESBIAN;
		else if(n == SKILL_ORALSEX)
			imageType = IMGTYPE_ORAL;
		else if(n == SKILL_TITTYSEX)
			imageType = IMGTYPE_TITTY;
		g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -2);
		g_Girls.UpdateEnjoyment(girl, ACTION_SEX, +3, true);

	/*if (sex)
	{*/
		// work out the pay between the house and the girl
		wages += 30;
		wages += g_Girls.GetStat(girl, STAT_ASKPRICE);
		int roll_max = (g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_CHARISMA));
		roll_max /= 4;
		wages += 10 + g_Dice%roll_max;
		girl->m_Pay = wages;
		girl->m_Events.AddMessage(message, imageType, DayNight);
	}
	else if (mast) 
	{
		brothel->m_Happiness += (g_Dice%70)+60;
		// work out the pay between the house and the girl
		int roll_max = (g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_CHARISMA));
		roll_max /= 4;
		wages += 10 + g_Dice%roll_max;
		girl->m_Pay = wages;
		girl->m_Events.AddMessage(message, IMGTYPE_MAST, DayNight);
			}
	else
	{
		brothel->m_Happiness += (g_Dice%70)+30;
		// work out the pay between the house and the girl
		int roll_max = (g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_CHARISMA));
		roll_max /= 4;
		wages += 10 + g_Dice%roll_max;
		girl->m_Pay = wages;
		girl->m_Events.AddMessage(message, IMGTYPE_STRIP, DayNight);
	}
	

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
	g_Girls.PossiblyGainNewTrait(girl, "Sexy Air", 80, ACTION_SEX, girl->m_Realname + " has been stripping and having to be sexy for so long she now reeks of sexyness.", DayNight != 0);

	//lose
	g_Girls.PossiblyLoseExistingTrait(girl, "Nervous", 30, ACTION_SEX, girl->m_Realname + " has had so many people see her naked she is no longer nervous about anything.", DayNight != 0);

	return false;
}
