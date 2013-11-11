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

bool cJobManager::WorkBar(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	/* WD:	Added missing SEX_ACTION filter
	 *
	 *		Add customer happiness 
	 *		Fixed income - need to decrement loop counter
	 *		when customer is rejected
	 */
	string message = "";
	int tex = g_Dice%4;

	if(Preprocessing(ACTION_WORKBAR, girl, brothel, DayNight, summary, message))	// they refuse to have work in the bar
		return true;

	// put that shit away, you'll scare off the customers!
	g_Girls.UnequipCombat(girl);

	// Four different random jobs
	if(tex == 0)
	{
		girl->m_Pay += 5;
		message += "She worked as a barmaid.";
	}
	else if(tex == 1)
	{
		girl->m_Pay += 15;
		message += "She worked as a waitress.";
	}
	else if(tex == 2)
	{
		girl->m_Pay += 30;
		message += "She worked as a stripper.";
	}
	else if(tex == 3)
	{
		//string message = "";
		u_int n = 0;
		int num = (g_Dice%2)+1;
		girl->m_Pay += g_Girls.GetStat(girl, STAT_ASKPRICE)*num;
		stringstream sstemp;
		sstemp << "She worked as a whore and fucked " << num << " customers.";
		message += sstemp.str();
		for(int i=0; i<num; i++)
		{
			sCustomer Cust;
			//GetMiscCustomer(brothel, Cust);

			// WD:	Create Customer
			g_Customers.GetCustomer(Cust, brothel);

			// WD:	Rejected unwanted sex types
			if(!is_sex_type_allowed(Cust.m_SexPref, brothel))
			{
				i--;
				continue; 
			}

			// WD:	Set the customers begining happiness/satisfaction
			Cust.m_Stats[STAT_HAPPINESS] = 42 + g_Dice%10 + g_Dice%10; // WD: average 51 range 42 to 60

			g_Girls.GirlFucks(girl, DayNight, &Cust, false,message,n);
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
			girl->m_Events.AddMessage(message, imageType, DayNight);
			g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -2);

			// WD:	 record number of customers
			brothel->m_MiscCustomers += 1;
		}
	}

/*
 *	Did she enjoy working this job?
 *
 *	Hmmm... with a 95% chance she enjoyed the work, that +3 is going to predominate
 *
 *	I can accept that jobs like barmaid might be cushy numbers, but this applies
 *	whatever the girl does
 */
	int roll = g_Dice%100;
	if(roll <= 5)
	{
		message += " Some of the patrons abused her during the shift.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKBAR, -3, true);
	}
/* 
 *	so the bonus is a third of that for a bad experience, but four times more likely
 *	so on the whole, they'll still like bar work
 */
	else if(roll <= 25) {
		message += " She had a pleasant time working.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKBAR, +1, true);
	}
	else
	{
		message += " Otherwise, the shift passed uneventfully.";
	}

	if(tex == 3)
			girl->m_Events.AddMessage(message, IMGTYPE_SEX, DayNight);
		else
			girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, DayNight);

/*
 *	work out the pay between the house and the girl
 *
 *	the original calc took the average of beauty and charisma and halved it
 */
	int roll_max = girl->beauty() + girl->charisma();
	roll_max /= 4;
	girl->m_Pay += 10 + g_Dice%roll_max;

	// Improve stats
	int xp = 5, libido = 1, skill = 3;

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
