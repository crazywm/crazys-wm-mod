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
#include "cClinic.h"
#include "cMovieStudio.h"
#include "cHouse.h"
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
#include "libintl.h"

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cHouseManager g_House;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;
extern int g_Building;

bool cJobManager::WorkPersonalBedWarmer(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	cTariff tariff;
	string message = "";
	int tex = g_Dice%4;


	// put that shit away, not needed for sex training
	g_Girls.UnequipCombat(girl);

	int roll = g_Dice%100;
	int HateLove = 0;
	HateLove = g_Girls.GetStat(girl, STAT_PCLOVE) - g_Girls.GetStat(girl, STAT_PCHATE);
	int wages = 150;

	message += "You tell her she is going to warm your bed tonight.";

	if(girl->m_States&(1<<STATUS_SLAVE))
	{
	if(HateLove < 0)
					{
					if(HateLove > -20)
						message += "She finds you to be annoying but you own her.\n\n";
					else if(HateLove > -40)
						message += "She finds you annoying but knows she must listen.\n\n";
					else if(HateLove > -60)
						message += "She doesn't like you but she is your slave and does what she is told.\n\n";
					else if(HateLove > -80)
						message += "She hates you but knows she must listen.\n\n";
					else 
						message += "She hates you more then anything but you own her so she does what she is told.\n\n";
					}
					else
					{
					 if (HateLove < 20)
						message += "She finds you be a decent master.\n\n";
					else if (HateLove < 40)
						message += "She finds you be a good master.\n\n";
					else if (HateLove < 60)
						message += "She finds you to be attractive.\n\n";
					else if (HateLove < 80)
						message += "She has really strong feelings for you.\n\n";
					else
						message += "She loves you more then anything.\n\n";
					}
	}
	else
{
	if(HateLove < 0)
					{
					if(HateLove > -20)
					{
						message += "She finds you to be annoying so she wants extra for the job.\n\n";
						wages = +125;
					}
					else if(HateLove > -40)
					{
						message += "She finds you annoying so she wants extra for the job.\n\n";
						wages = +175;
					}
					else if(HateLove > -60)
					{
						message += "She doesn't like you so she wants extra for the job.\n\n";
						wages = +205;
					}
					else if(HateLove > -80)
					{
						message += "She don't like you at all and wants more money to lay with you at night .\n\n";
						wages = +225;
					}
					else 
						message += "She can't stand the sight of you and demands way more money to lay with you at night.\n\n";
						wages = +250;
					}
					else
					{
		if (HateLove < 20)
						message += "She finds you to be okay.\n\n";
					else if (HateLove < 40)
					{
						message += "She finds you to be okay.\n\n";
						wages = -25;
					}
					else if (HateLove < 60)
					{
						message += "She finds you attractive so she gives you a discount.\n\n";
						wages = -25;
					}
					else if (HateLove < 80)
					{
						message += "Shes has really strong feelings for you so she lays with you for less money.\n\n";
						wages = -25;
					}
					else
					{
						message += "She is totally in love with you and doesn't want as much money.\n\n";
						wages = -25;
					}

if (roll <= 100 && g_Girls.DisobeyCheck(girl, ACTION_WORKHAREM, brothel))
	{
		message = girl->m_Realname + gettext(" refused to lay with you today.");
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else if(roll <= 15) {
		message += gettext(" \nYou did something to piss her off.\n\n");
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKHAREM, -1, true);
		g_Girls.UpdateStat(girl, STAT_PCLOVE, -1);
	}
	else if(roll >=90)
	{
		message += gettext(" \nShe had a pleasant time with you.\n\n");
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKHAREM, +3, true);
		g_Girls.UpdateStat(girl, STAT_PCLOVE, 2);
	}
	else
	{
		message += gettext(" \nOtherwise, nothing of note happened.\n\n");
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKHAREM, +1, true);
	}

	
					}
	}
	

	if(g_Girls.GetStat(girl, STAT_LIBIDO) > 60)
	{
		if (HateLove < 40)
		{
		message += "She was horney but she doesn't care for you much so she just Masturbated.\n\n";
		g_Girls.UpdateEnjoyment(girl, ACTION_SEX, +1, true);	
		g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -35);
		girl->m_Events.AddMessage(message, IMGTYPE_MAST, DayNight);
	}
		else 
		{
	message += "She was horney and she likes you so she ";
	if (roll <= 25)
		{
			message += "decied to suck your cock.\n\n";
			g_Girls.UpdateSkill(girl, SKILL_ORALSEX, 1);
			girl->m_Events.AddMessage(message, IMGTYPE_ORAL, DayNight);
		}
		else if (roll <= 50)
		{
			message += "lets you tie her up.\n\n";
			g_Girls.UpdateSkill(girl, SKILL_BDSM, 1);
			girl->m_Events.AddMessage(message, IMGTYPE_BDSM, DayNight);
			if(girl->m_Virgin)
				{
					girl->m_Virgin = false;
					message += "She was a virgin.\n";
				}
			g_Building = BUILDING_HOUSE;
			if(!girl->calc_pregnancy(g_Brothels.GetPlayer(), false, 1.0)) {
			g_MessageQue.AddToQue("She has gotten pregnant", 0);
		}
	}
		else if (roll <= 75)
		{
			message += "lets you use her ass.\n\n";
			g_Girls.UpdateSkill(girl, SKILL_ANAL, 1);
			girl->m_Events.AddMessage(message, IMGTYPE_ANAL, DayNight);
		}
		else
		{
			message += "has sex with you.\n\n";
			g_Girls.UpdateSkill(girl, SKILL_NORMALSEX, 1);
			girl->m_Events.AddMessage(message, IMGTYPE_SEX, DayNight);
			if(girl->m_Virgin)
				{
					girl->m_Virgin = false;
					message += "She was a virgin.\n";
				}
			g_Building = BUILDING_HOUSE;
			if(!girl->calc_pregnancy(g_Brothels.GetPlayer(), false, 1.0)) {
			g_MessageQue.AddToQue("She has gotten pregnant", 0);
		}
		}
		g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -35);
		g_Girls.UpdateEnjoyment(girl, ACTION_SEX, +3, true);
	}
	}
	else
	{
	girl->m_Events.AddMessage(message, IMGTYPE_ECCHI, DayNight);
	}

	if(wages < 0)
		wages = 50;

	g_Gold.girl_support(wages);  // matron wages come from you
	girl->m_Pay += wages;

	// Improve stats
	int xp = 10, libido = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))
	{
		xp += 3;
	}
	else if (g_Girls.HasTrait(girl, "Slow Learner"))
	{
		xp -= 3;
	}

	if (g_Girls.HasTrait(girl, "Nymphomaniac"))
		libido += 2;

	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	return false;
}
