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

// `J` House Job - General
bool cJobManager::WorkPersonalBedWarmer(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	cTariff tariff;
	string message = "";
	stringstream ss;
	g_Building = BUILDING_HOUSE;

	// put that shit away, not needed for sex training
	g_Girls.UnequipCombat(girl);

	int roll_a = g_Dice % 100, roll_b = g_Dice % 100, roll_c = g_Dice % 100, roll_d = g_Dice % 100;
	int HateLove = 0;
	HateLove = g_Girls.GetStat(girl, STAT_PCLOVE) - g_Girls.GetStat(girl, STAT_PCHATE);
	int wages = 150;

	ss << gettext("You tell her she is going to warm your bed tonight.\n");

	if (girl->m_States&(1 << STATUS_SLAVE))
	{
		if (HateLove < -80)	ss << gettext("She hates you more then anything but you own her so she does what she is told.\n\n");
		else if (HateLove < -60)	ss << gettext("She hates you but knows she must listen.\n\n");
		else if (HateLove < -40)	ss << gettext("She doesn't like you but she is your slave and does what she is told.\n\n");
		else if (HateLove < -20)	ss << gettext("She finds you annoying but knows she must listen.\n\n");
		else if (HateLove < 0)	ss << gettext("She finds you to be annoying but you own her.\n\n");
		else if (HateLove < 20)	ss << gettext("She finds you be a decent master.\n\n");
		else if (HateLove < 40)	ss << gettext("She finds you be a good master.\n\n");
		else if (HateLove < 60)	ss << gettext("She finds you to be attractive.\n\n");
		else if (HateLove < 80)	ss << gettext("She has really strong feelings for you.\n\n");
		else					ss << gettext("She loves you more then anything.\n\n");
	}
	else
	{
		if (HateLove < -80)	{ wages += 150;	ss << gettext("She can't stand the sight of you and demands way more money to lay with you at night.\n\n"); }
		else if (HateLove < -60)	{ wages += 100;	ss << gettext("She don't like you at all and wants more money to lay with you at night .\n\n"); }
		else if (HateLove < -40)	{ wages += 75;	ss << gettext("She doesn't like you so she wants extra for the job.\n\n"); }
		else if (HateLove < -20)	{ wages += 50;	ss << gettext("She finds you annoying so she wants extra for the job.\n\n"); }
		else if (HateLove < 0)	{ wages += 25;	ss << gettext("She finds you to be annoying so she wants extra for the job.\n\n"); }
		else if (HateLove < 20)	{ ss << gettext("She finds you to be okay.\n\n"); }
		else if (HateLove < 40)	{ wages -= 20;	ss << gettext("She finds you to be nice so she gives you a discount.\n\n"); }
		else if (HateLove < 60)	{ wages -= 40;	ss << gettext("She finds you attractive so she gives you a discount.\n\n"); }
		else if (HateLove < 80)	{ wages -= 60;	ss << gettext("Shes has really strong feelings for you so she lays with you for less money.\n\n"); }
		else					{ wages -= 80;	ss << gettext("She is totally in love with you and doesn't want as much money.\n\n"); }
	}

	if (roll_a <= 100 && g_Girls.DisobeyCheck(girl, ACTION_WORKHAREM, brothel))
	{
		message = girl->m_Realname + gettext(" refused to lay with you today.");
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else if (roll_a <= 15)
	{
		message += gettext(" \nYou did something to piss her off.\n\n");
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKHAREM, -1, true);
		g_Girls.UpdateStat(girl, STAT_PCLOVE, -1);
	}
	else if (roll_a >= 90)
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


	if (HateLove >= 80) //loves you
	{
		if (g_Girls.CheckVirginity(girl))		// 25% decline
		{
			ss << gettext("She is a virgin so you ask her if she wants to let you be her first.\nShe loves you greatly ");
			if (roll_d <= 25)	{ ss << gettext("but says she isn't ready so instead she "); roll_d *= 2; }
			else				{ ss << gettext("and agrees so she ");	roll_d = 77; } // normal sex
		}
		else
		{
			ss << gettext("She loves you greatly so she ");
		}
	}
	else if (HateLove >= 60)			//find u attractive
	{
		if (g_Girls.CheckVirginity(girl))		// 50 % decline
		{
			ss << gettext("She is a virgin so you ask her if she wants to let you be her first.\nShe finds you to be attractive ");
			if (roll_d <= 50)		{ ss << gettext("but declines so instead she ");	roll_d *= 2; }
			else					{ ss << gettext("so she agrees and she ");	roll_d = 77; } // normal sex
		}
		else
		{
			ss << gettext("She finds you to be attractive so she ");
		}
	}
	else if (HateLove > 20)			//your okay
	{
		if (g_Girls.CheckVirginity(girl))		// 70% decline
		{
			ss << gettext("She is a virgin so you ask her if she wants to let you be her first.\nShe finds you to be an okay guy ");
			if (roll_d <= 70)	{ ss << gettext("but declines anyway. So she "); roll_d = 100; }
			else				{ ss << gettext("so she agrees and she ");	roll_d = 77; } // normal sex
		}
		else
		{
			ss << gettext("She finds you to be okay so she ");
		}
	}
	else if (HateLove >= -20)			//annoying
	{
		if (g_Girls.CheckVirginity(girl))		// 80% decline
		{
			ss << gettext("She is a virgin so you ask her if she wants to let you be her first.\nShe finds you to be rather annoying ");
			if (roll_d <= 80)	{ ss << gettext("so she declines and she "); roll_d = 100; }
			else				{ ss << gettext("but agrees none the less. She ");	roll_d = 77; } // normal sex
		}
		else
		{
			ss << gettext("She finds you to be annoying so she ");
		}
	}
	else if (HateLove > -60)			//dont like u at all
	{
		if (g_Girls.CheckVirginity(girl))		// 95% decline
		{
			ss << gettext("She is a virgin so you ask her if she wants to let you be her first.\nShe doesn't really like you ");
			if (roll_d <= 95)	{ ss << gettext("and declines so she "); roll_d = 100; }
			else				{ ss << gettext("but for whatever reason she agrees so she ");	roll_d = 77; } // normal sex
		}
		else
			ss << gettext("She don't like you at all so she ");
		if (g_Girls.GetStat(girl, STAT_LIBIDO) > 60)
		{
			roll_d = 88;
		}
		else
		{
			roll_d = 100;
		}
	}
	else											//cant stand the site of u
	{
		if (g_Girls.CheckVirginity(girl))		// 100% decline
		{
			ss << gettext("She is a virgin so you ask her if she wants to let you be her first.\nShe ");
			ss << gettext("laughs hard at the thought of her letting you touch her.\nShe ");
			if (g_Girls.GetStat(girl, STAT_LIBIDO) > 60)
			{
				roll_d = 88;
			}
			else
			{
				roll_d = 100;
			}
		}
		else
			ss << gettext("She can't stand the sight of you so she ");
		if (g_Girls.GetStat(girl, STAT_LIBIDO) > 60)
		{
			roll_d = 88;
		}
		else
		{
			roll_d = 100;
		}
	}




	if (roll_d <= 11)
	{
		g_Girls.UpdateSkill(girl, SKILL_STRIP, 2);
		ss << gettext("does a little strip show for you.\n\n");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_STRIP, DayNight);
	}
	else if (roll_d <= 22 && is_sex_type_allowed(SKILL_TITTYSEX, brothel))
	{
		g_Girls.UpdateSkill(girl, SKILL_TITTYSEX, 2);
		ss << gettext("uses her tits on you.\n\n");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_TITTY, DayNight);
	}
	else if (roll_d <= 33 && is_sex_type_allowed(SKILL_HANDJOB, brothel))
	{
		g_Girls.UpdateSkill(girl, SKILL_HANDJOB, 2);
		ss << gettext("gives you a hand job.\n\n");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_HAND, DayNight);
	}
	else if (roll_d <= 44 && is_sex_type_allowed(SKILL_ORALSEX, brothel))
	{
		g_Girls.UpdateSkill(girl, SKILL_ORALSEX, 2);
		ss << gettext("decided to suck your cock.\n\n");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_ORAL, DayNight);
	}
	else if (roll_d <= 55 && is_sex_type_allowed(SKILL_ANAL, brothel))
	{
		g_Girls.UpdateSkill(girl, SKILL_ANAL, 2);
		ss << gettext("lets you use her ass.\n\n");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_ANAL, DayNight);
	}
	else if (roll_d <= 66 && is_sex_type_allowed(SKILL_BDSM, brothel))
	{
		g_Girls.UpdateSkill(girl, SKILL_BDSM, 2);
		ss << gettext("lets you tie her up.\n\n");
		if (g_Girls.CheckVirginity(girl))
		{
			g_Girls.LoseVirginity(girl);	// `J` updated for trait/status
			ss << gettext("She is no longer a virgin.\n");
		}
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_BDSM, DayNight);
		if (!girl->calc_pregnancy(g_Brothels.GetPlayer(), false, 1.0))
		{
			message = girl->m_Realname;
			message += " has gotten pregnant";
			g_MessageQue.AddToQue(message, 0);
		}
	}
	else if (roll_d <= 77 && is_sex_type_allowed(SKILL_NORMALSEX, brothel))
	{
		g_Girls.UpdateSkill(girl, SKILL_NORMALSEX, 2);
		ss << gettext("has sex with you.\n\n");
		if (g_Girls.CheckVirginity(girl))
		{
			g_Girls.LoseVirginity(girl);	// `J` updated for trait/status
			ss << gettext("She is no longer a virgin.\n");
		}
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_SEX, DayNight);
		if (!girl->calc_pregnancy(g_Brothels.GetPlayer(), false, 1.0))
		{
			message = girl->m_Realname;
			message += " has gotten pregnant";
			g_MessageQue.AddToQue(message, 0);
		}
	}
	else if (roll_d <= 88)
	{
		if (HateLove > 20)
		{
			ss << gettext("let you watch her Masturbate.\n\n");
		}
		else
		{
			ss << gettext("was horney so she just Masturbated.\n\n");
		}
		g_Girls.UpdateEnjoyment(girl, ACTION_SEX, +1, true);
		g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -15);
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_MAST, DayNight);
	}
	else
	{
		if (HateLove > 20)
		{
			ss << gettext("is to tried to fool around so she goes to sleep.\n\n");
		}
		else
		{
			ss << gettext("just goes to sleep.\n\n");
		}
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_ECCHI, DayNight);
	}



	//if(g_Girls.GetStat(girl, STAT_LIBIDO) > 60)
	//{
	//	if (HateLove < 40)
	//	{
	//		message += "She was horney but she doesn't care for you much so she just Masturbated.\n\n";
	//		g_Girls.UpdateEnjoyment(girl, ACTION_SEX, +1, true);	
	//		g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -35);
	//		girl->m_Events.AddMessage(message, IMGTYPE_MAST, DayNight);
	//	}
	//	else 
	//	{
	//		message += "She was horney and she likes you so she ";
	//		if (roll_b <= 25)
	//		{
	//			message += "decided to suck your cock.\n\n";
	//			g_Girls.UpdateSkill(girl, SKILL_ORALSEX, 2);
	//			girl->m_Events.AddMessage(message, IMGTYPE_ORAL, DayNight);
	//		}
	//		else if (roll_b <= 50)
	//		{
	//			message += "lets you tie her up.\n\n";
	//			g_Girls.UpdateSkill(girl, SKILL_BDSM, 2);
	//			girl->m_Events.AddMessage(message, IMGTYPE_BDSM, DayNight);
	//			if(g_Girls.CheckVirginity(girl))
	//			{
	//				g_Girls.LoseVirginity(girl);	// `J` updated for trait/status
	//				message += "She was a virgin.\n";
	//			}
	//			g_Building = BUILDING_HOUSE;
	//			if(!girl->calc_pregnancy(g_Brothels.GetPlayer(), false, 1.0)) 
	//			{
	//				g_MessageQue.AddToQue("She has gotten pregnant", 0);
	//			}
	//		}
	//		else if (roll_b <= 75)
	//		{
	//			message += "lets you use her ass.\n\n";
	//			g_Girls.UpdateSkill(girl, SKILL_ANAL, 2);
	//			girl->m_Events.AddMessage(message, IMGTYPE_ANAL, DayNight);
	//		}
	//		else
	//		{
	//			message += "has sex with you.\n\n";
	//			g_Girls.UpdateSkill(girl, SKILL_NORMALSEX, 2);
	//			girl->m_Events.AddMessage(message, IMGTYPE_SEX, DayNight);
	//			if(g_Girls.CheckVirginity(girl))
	//			{
	//				g_Girls.LoseVirginity(girl);	// `J` updated for trait/status
	//				message += "She was a virgin.\n";
	//			}
	//			g_Building = BUILDING_HOUSE;
	//			if(!girl->calc_pregnancy(g_Brothels.GetPlayer(), false, 1.0)) 
	//			{
	//				g_MessageQue.AddToQue("She has gotten pregnant", 0);
	//			}
	//		}
	//		g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -35);
	//		g_Girls.UpdateEnjoyment(girl, ACTION_SEX, +3, true);
	//	}
	//}
	//else
	//{
	//	girl->m_Events.AddMessage(message, IMGTYPE_ECCHI, DayNight);
	//}

	if (wages < 0)
		wages = 0;

	g_Gold.girl_support(wages);  // wages come from you
	girl->m_Pay += wages;

	// Improve stats
	int xp = 10, libido = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	return false;
}
