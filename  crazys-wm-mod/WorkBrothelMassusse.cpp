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

bool cJobManager::WorkBrothelMasseuse(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";
	if(Preprocessing(ACTION_SEX, girl, brothel, DayNight, summary, message))
		return true;

	// put that shit away, you'll scare off the customers!
	g_Girls.UnequipCombat(girl);

	int roll = g_Dice%100;
	int looks = (g_Girls.GetStat(girl, STAT_CHARISMA) + g_Girls.GetStat(girl, STAT_BEAUTY))/2;
	int jobperformance = (looks + g_Girls.GetSkill(girl, SKILL_SERVICE));
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
		}
	else if (jobperformance >= 185)
		{
			message += " She's unbelievable at this and is always finding new ways to please the customer.\n\n";
			wages += 95;
		}
	else if (jobperformance >= 145)
		{
			message += " She's good at this job and knows a few tricks to drive the customers wild.\n\n";
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
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, skill);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	return false;
}
