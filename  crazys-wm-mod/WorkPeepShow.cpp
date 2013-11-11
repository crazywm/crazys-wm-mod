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

bool cJobManager::WorkPeepShow(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";
	if(Preprocessing(ACTION_SEX, girl, brothel, DayNight, summary, message))
		return true;

	// put that shit away, you'll scare off the customers!
	g_Girls.UnequipCombat(girl);

	int roll = g_Dice%100;
	int looks = (g_Girls.GetStat(girl, STAT_CHARISMA) + g_Girls.GetStat(girl, STAT_BEAUTY))/2;
	int jobperformance = (looks + g_Girls.GetSkill(girl, SKILL_STRIP));

	int wages = g_Girls.GetStat(girl, STAT_ASKPRICE)+10;
	message += "She let the customer watch her get naked.";


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


	if(jobperformance < 35)
		{
			message += " She was nervous and constantly making mistakes. She really isn't very good at this job.\n\n";
			wages -= 15;
		}
	else if(jobperformance < 65)
		{
			message += " She was nervous and made a few mistakes. She isn't that good at this.\n\n";
			wages -= 5;
		}
	else if(jobperformance < 85)
		{
			message += " She made a few mistakes but overall she is okay at this.\n\n";
			wages += 15;
		}
	else if(jobperformance < 135)
		{
			message += " She's good at this job and knows a few tricks to drive the customers wild.\n\n";
			wages += 55;
		}
	else if(jobperformance < 185)
		{
			message += " She's unbelievable at this and is always finding new ways to please the customer.\n\n";
			wages += 95;
		}
	else if(jobperformance < 245)
		{
			message += " She must be the perfect teaser she never goes to fast but never to slow she knows just when to do things and the customers can't get enough of her.\n\n";
			wages += 155;
		}

	if(g_Girls.GetStat(girl, STAT_LIBIDO) > 80)
	{
		message += "She was horney and ended up masturbating for the customer making them very happy.";
		sCustomer cust;
		GetMiscCustomer(brothel, cust);
		brothel->m_Happiness += 100;
		g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -40);

		// work out the pay between the house and the girl
		wages += 60;
		girl->m_Pay = wages;
		girl->m_Events.AddMessage(message, IMGTYPE_MAST, DayNight);
		g_Girls.UpdateEnjoyment(girl, ACTION_SEX, +3, true);
	}
	else
	{
		brothel->m_Happiness += (g_Dice%70)+30;
		brothel->m_MiscCustomers++;

		// work out the pay between the house and the girl
		girl->m_Pay = wages;
		girl->m_Events.AddMessage(message, IMGTYPE_NUDE, DayNight);
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


	//lose
	g_Girls.PossiblyLoseExistingTrait(girl, "Nervous", 30, ACTION_SEX, girl->m_Realname + " has had so many people see her naked she is no longer nervous about anything.", DayNight != 0);

	return false;
}
