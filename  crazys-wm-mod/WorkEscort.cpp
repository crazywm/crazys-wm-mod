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
#include <algorithm>

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;

bool cJobManager::WorkEscort(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";
	string girlName = girl->m_Realname;

	// put that shit away, you'll scare off the customers!
	g_Girls.UnequipCombat(girl);

	int roll_a = g_Dice % 100; //what lvl of customer
	int roll_b = g_Dice % 100;
	int roll_c = g_Dice % 100;
	int roll_d = g_Dice % 100;
	int agl = (g_Girls.GetStat(girl, STAT_AGILITY));

	int pass_a = false;
	int pass_b = false;

	//customer type
	int cust_a = false; //rich
	int cust_b = false; //middle
	int cust_c = false; //poor

	int jobperformance = 0;
	int wages = g_Girls.GetStat(girl, STAT_ASKPRICE)+40;

	if (roll_a <= 60)
	{
		cust_a = true;
	}
	else if (roll_a <= 85)
	{
		cust_b = true;
	}
	else
	{
		cust_c = true;
	}


	message = "She worked as an escort ";

	//Doing test stuff for now. 


	if (cust_a)//rich
	{
		message = "She sees a rich customer and.";
		if (agl >= 90)
			{
			message += girlName + " arrived early.";
			jobperformance += 20;
			}
		else if (agl >= 75)
			{
			message += girlName + " was on time.";
			jobperformance += 10;
			}
		else if (agl >= 50)
			{
			message += girlName + " was a few minutes late.";
			jobperformance += 5;
			}
		else
			{
				message += girlName + " was very late.";
				jobperformance = 0;	
			}

		if (jobperformance >= 10)
		{
			pass_a = true;
		}

	if (pass_a)
		{
			message += girlName + " passed test a.";
		}
	else
		{
			message += girlName + " fail test a.";
		}
	}

	if (cust_b)//middle
	{
		message = "She sees a middle class customer and ";
		if (agl >= 90)
			{
			message += girlName + " arrived early.";
			jobperformance += 20;
			}
		else if (agl >= 75)
			{
			message += girlName + " was on time.";
			jobperformance += 10;
			}
		else if (agl >= 50)
			{
			message += girlName + " was a few minutes late.";
			jobperformance += 5;
			}
		else
			{
				message += girlName + " was very late.";
				jobperformance = 0;	
			}

		if (jobperformance >= 5)
		{
			pass_a = true;
		}

	if (pass_a)
		{
			message += girlName + " passed test a.";
		}
	else
		{
			message += girlName + " fail test a.";
		}
	}

	if (cust_c)//poor
	{
		message = "She sees a porr customer and ";
		if (agl >= 90)
			{
			message += girlName + " arrived early.";
			jobperformance += 20;
			}
		else if (agl >= 75)
			{
			message += girlName + " was on time.";
			jobperformance += 10;
			}
		else if (agl >= 50)
			{
			message += girlName + " was a few minutes late.";
			jobperformance += 5;
			}
		else
			{
				message += girlName + " was very late.";
				jobperformance = 0;	
			}

		if (jobperformance >= 0)
		{
			pass_a = true;
		}

	if (pass_a)
		{
			message += girlName + " passed test a.";
		}
	else
		{
			message += girlName + " fail test a.";
		}
	}



	// work out the pay between the house and the girl
	girl->m_Pay = wages;
	girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, DayNight);

	return false;
}