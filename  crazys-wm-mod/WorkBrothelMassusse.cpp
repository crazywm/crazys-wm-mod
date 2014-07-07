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

bool cJobManager::WorkBrothelMasseuse(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";
	string girlName = girl->m_Realname;
	if(Preprocessing(ACTION_WORKMASSUSSE, girl, brothel, DayNight, summary, message))
		return true;

	// put that shit away, you'll scare off the customers!
	g_Girls.UnequipCombat(girl);

	int roll = g_Dice%100;
	int jobperformance = (	g_Girls.GetStat(girl, STAT_CHARISMA) / 2 + 
							g_Girls.GetStat(girl, STAT_BEAUTY) / 2  +
							g_Girls.GetSkill(girl, SKILL_MEDICINE) / 2 +
							g_Girls.GetSkill(girl, SKILL_SERVICE) / 2);
	int wages = g_Girls.GetStat(girl, STAT_ASKPRICE)+40;

	message += "She massaged a customer.";


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


	if (jobperformance >= 245)
		{
			message += " She must be the perfect massusse she never goes to hard but never to soft she knows just what to do and the customers can't get enough of her.\n\n";
			wages += 155;

			if (roll <= 20)
			{
				message += girlName + " hands are the best thing that could touch a man. Customers left big tips after getting a massage from her.\n";
			}
		else if (roll <= 40)
			{
				message += "She is well known and have a steady base of faithful customers.\n";
			}
		else if (roll <= 60)
			{
				message += "A client wanted that " + girlName + " focus on the tight region. Doing as he asked, she left him with a boner.\n";
			}
		else if (roll <= 80)
			{
				message +=  "Every man leave girlsname room with a wide smile.\n";
			}
		else
			{
				message += "Being asked to perfume more sensual massage, " + girlName + " massage the customer with her breasts for some extra coin.\n";
			}
		}
	else if (jobperformance >= 185)
		{
			message += " She's unbelievable at this and is always finding new ways to please the customer.\n\n";
			wages += 95;

		if (roll <= 20)
			{
				message += "Knowing most of masseurs techniques, she is one of the best.\n";
			}
		else if (roll <= 40)
			{
				message += "One of her customers wanted something more than the usual service. She declined and banned him from her customers list.\n";
			}
		else if (roll <= 60)
			{
				message += "Trying a new technique, " + girlName + " performed a massage with her feet. The customer really liked it and suggested she should put this on the regular menu.\n";
			}
		else if (roll <= 80)
			{
				message +=  "She certainly knows how and where to  press on a man’s body. Today, customers moans was heard on the corridor.\n";
			}
		else
			{
				message += "Today she used some new oils. The customers liked them and paid some extra for the service.\n";
			}
		}
	else if (jobperformance >= 145)
		{
			message += " She's good at this job and knows a few tricks to drive the customers wild.\n\n";
			wages += 55;

		if (roll <= 20)
			{
				message += girlName + " helped a customer with a really painful back pain. The man bowed several times, thanking her for the treatment.\n";
			}
		else if (roll <= 40)
			{
				message += "She's making less mistakes with every day and earning good money.\n";
			}
		else if (roll <= 60)
			{
				message += "When massaging a customer, she had a pleasant conversation with him, leaving a very good impression.\n";
			}
		else if (roll <= 80)
			{
				message +=  "She certainly knows how to do her job and is getting better at it every day.\n";
			}
		else
			{
				message += "A female customer requested a face massage. " + girlName + " managed to meet her expectations.\n";
			}
		}
	else if (jobperformance >= 100)
		{
			message += " She made a few mistakes but overall she is okay at this.\n\n";
			wages += 15;

		if (roll <= 20)
			{
				message += "Maybe she isn’t the best, but at least she won’t kill anyone.\n";
			}
		else if (roll <= 40)
			{
				message += "She got the job done. No complains was noted.\n";
			}
		else if (roll <= 60)
			{
				message += "She was good. A few of " + girlName + "'s customers praised her.\n";
			}
		else if (roll <= 80)
			{
				message +=  "She made few mistakes, but manged not to hurt anyone today.\n";
			}
		else
			{
				message += "Only few customers came today. She didn’t earn much.\n";
			}
		}
	else if (jobperformance >= 70)
		{
			message += " She was nervous and made a few mistakes. She isn't that good at this.\n\n";
			wages -= 5;

		if (roll <= 20)
			{
				message += girlName + " almost killed a customer today!\n";
			}
		else if (roll <= 40)
			{
				message += "Her performance was bad. The customer left a complaint and didn’t pay for the service.\n";
			}
		else if (roll <= 60)
			{
				message += "Thing that she did today could be called a backrub, not a massage.\n";
			}
		else if (roll <= 80)
			{
				message +=  "She argued with a client and kicked him out on the street wearing only a towel around his waist.\n";
			}
		else
			{
				message += "Not knowing what to do, she tried her best and failed.\n";
			}
		}
	else
		{
			message += " She was nervous and constantly making mistakes. She really isn't very good at this job.\n\n";
			wages -= 15;

		if (roll <= 20)
			{
				message += girlName + " massaged a client. He could barely walk out after her service.\n";
			}
		else if (roll <= 40)
			{
				message += "Getting kicked by a horse or getting a massage from " + girlName + ". Equally deadly but the first you could get for free. \n";
			}
		else if (roll <= 60)
			{
				message += "A loud scream was heard in your building short after " + girlName + " started her shift.\n";
			}
		else if (roll <= 80)
			{
				message +=  "Not having any experience in this kind of work she fails miserably.\n";
			}
		else
			{
				message += "She tries to massage a client in the best way she knows. After ten minutes he leaves without paying and frustrated.\n";
			}
		}

	if(g_Girls.GetStat(girl, STAT_LIBIDO) > 90)
	{
		u_int n;
		message += "She massaged and ended up fucking the customer as well, making them very happy.\n\n";
		sCustomer cust;
		GetMiscCustomer(brothel, cust);
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
		else if(n == SKILL_HANDJOB)
			imageType = IMGTYPE_HAND;
		g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -45);

		// work out the pay between the house and the girl
		wages += 225;
		girl->m_Pay = wages;
		g_Girls.UpdateEnjoyment(girl, ACTION_SEX, +3, true);
		girl->m_Events.AddMessage(message, imageType, DayNight);
	}
	else
	{
		brothel->m_Happiness += (g_Dice%70)+30;
		brothel->m_MiscCustomers++;

		// work out the pay between the house and the girl
		girl->m_Pay = wages;
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, DayNight);
	}

	//enjoyed the work or not
	if(roll <= 5)
	{
		message += " \nSome of the patrons abused her during the shift.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKMASSUSSE, -1, true);
	}
	else if(roll <= 25) {
		message += " \nShe had a pleasant time working.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKMASSUSSE, +3, true);
	}
	else
	{
		message += " \nOtherwise, the shift passed uneventfully.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKMASSUSSE, +1, true);
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
	g_Girls.UpdateSkill(girl, SKILL_MEDICINE, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, g_Dice%skill+1);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	return false;
}
