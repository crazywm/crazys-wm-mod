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
static cPlayer* m_Player = g_Brothels.GetPlayer();

bool cJobManager::WorkPersonalTraining(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";
	g_Building = BUILDING_HOUSE;
	int tex = g_Dice % 4;

	if(Preprocessing(ACTION_SEX, girl, brothel, DayNight, summary, message))	// they refuse to have work
		return true;
	stringstream ss;
	ss.str(message);

	// put that shit away, not needed for sex training
	g_Girls.UnequipCombat(girl);

	double roll_a = g_Dice % 100; //this is used to determine gain amount

	int skill = 0;
	     if (roll_a <= 15){skill = 5;}
	else if (roll_a <= 35){skill = 4;}
	else if (roll_a <= 60){skill = 3;}
	else                  {skill = 2;}
	     if (g_Girls.HasTrait(girl, "Quick Learner"))	{skill += 1;}
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{skill -= 1;}

	ss << gettext("You over see her traning for the day.\n\n");
	double roll_b = g_Dice % 100; //this is used to determine what skill is trained
// roll_b random from 1-100 then is modified by player disposition and if is less than:
// 7 strip | 15 les | 30 tit | 50 oral | 70 normal | 80 anal | 90 group | 100 bdsm | +beast
// the nicer the player, the lower the roll, meaner is higher, only evil will do beast
// will also skip down the list if the girl has 100 in the skill
	if (m_Player->disposition() >= 80)				//Benevolent
	{
		if (girl->m_Virgin)		// 25% decline
		{
			ss << gettext("She is a virgin so you ask her if she wants to let you be her first.\nShe ");
			if (roll_b <= 25)	{ ss << gettext("declines so "); roll_b *= 2; }
			else				{	ss << gettext("agrees so ");	roll_b = 60;	} // normal sex
		}
		else if (roll_b > 75) roll_b -= 8;
	}
	else if (m_Player->disposition() >= 50)			//Nice
	{
		if (girl->m_Virgin)		// 50 % decline
		{
			ss << gettext("She is a virgin so you ask her if she wants to let you be her first.\nShe ");
			if (roll_b <= 50)		{	ss << gettext("declines so ");	}
			else					{	ss << gettext("agrees so ");	roll_b = 60;	} // normal sex
		}
		else if (roll_b > 90) roll_b -= 3;
	}
	else if (m_Player->disposition() > 10)			//Pleasant
	{
		if (girl->m_Virgin)		// 70% decline
		{
			ss << gettext("She is a virgin so you ask her if she wants to let you be her first.\n");
			if (roll_b <= 50)		{	ss << gettext("She declines so ");						}
			else if (roll_b <= 80)	{	ss << gettext("She agrees so ");			roll_b = 60;} // normal sex
			else					{	ss << gettext("She declines so ");			roll_b = 75;} // anal sex
		}
		else if (roll_b > 95) roll_b *= 0.98;
	}
	else if (m_Player->disposition() >= -10)			//Neutral
	{
		if (girl->m_Virgin)		// 80% decline
		{
			ss << gettext("She is a virgin so you ask her if she wants to let you be her first.\n");
			if (roll_b <= 50)		{ ss << gettext("She declines so "); }
			else if (roll_b <= 70)	{ ss << gettext("She agrees so ");			roll_b = 60; } // normal sex
			else if (roll_b <= 90)	{ ss << gettext("She declines so ");			roll_b = 75; } // anal sex
			else					{ ss << gettext("She refuses so instead ");	roll_b = 95; } // BDSM
		}
	}
	else if (m_Player->disposition() > -50)			//Not nice
	{
		if (girl->m_Virgin)
		{
			ss << gettext("She is a virgin, but not for long.\n");
			if (roll_b <= 70)		{ ss << gettext("Wanting her for yourself, "); roll_b = 60; } // normal
			else if (roll_b <= 85)	{ ss << gettext("Wanting to break in all her holes, ");			roll_b = 85; } // group
			else					{ ss << gettext("Wanting her to know who the boss is, ");		roll_b = 95; } // bdsm
		}
		else if (roll_b < 30) roll_b *= 2;
	}
	else if (m_Player->disposition() > -80)			//Mean
	{
		if (girl->m_Virgin)
		{
			ss << gettext("She is a virgin, but not for long.\n");
			if (roll_b <= 60)		{ ss << gettext("Wanting her for yourself, "); roll_b = 60; } // normal
			else if (roll_b <= 80)	{ ss << gettext("Wanting to break in all her holes, ");			roll_b = 85; } // group
			else					{ ss << gettext("Wanting her to know who the boss is, ");		roll_b = 95; } // bdsm
		}
		else if (roll_b < 50) roll_b *= 2;
	}
	else											//Evil
	{
		if (girl->m_Virgin)
		{
			ss << gettext("She is a virgin, but not for long.\n");
			if (roll_b <= 50)		{ ss << gettext("Wanting her for yourself, "); roll_b = 60; } // normal
			else if (roll_b <= 70)	{ ss << gettext("Wanting to break in all her holes, ");			roll_b = 85; } // group
			else if (roll_b <= 90)	{ ss << gettext("Wanting her to know who the boss is, ");		roll_b = 95; } // bdsm
			else					{ ss << gettext("Wanting her to know she is just an animal to you, ");	roll_b = 105; } // beast
		}
		else if (roll_b < 70) roll_b *= 2;
	}


	if (roll_b <= 10 && (int)girl->m_Skills[SKILL_STRIP] < 100)
	{
		g_Girls.UpdateSkill(girl, SKILL_STRIP, skill); 
		ss << gettext("You decide to have her strip for you.\n\n");
		ss << gettext("She managed to gain ") << skill << gettext(" Strip.\n\n");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_STRIP, DayNight);
	}
	else if (roll_b <= 20 && (int)girl->m_Skills[SKILL_LESBIAN] < 100)
	{
		g_Girls.UpdateSkill(girl, SKILL_LESBIAN, skill);
		ss << gettext("You decide to have her strip for you.\n\n");
		ss << gettext("She managed to gain ") << skill << gettext(" Lesbian.\n\n");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_LESBIAN, DayNight);
	}
	else if (roll_b <= 35 && (int)girl->m_Skills[SKILL_TITTYSEX] < 100)
	{
		g_Girls.UpdateSkill(girl, SKILL_TITTYSEX, skill);
		ss << gettext("You decide to have her use her tits on you.\n\n");
		ss << gettext("She managed to gain ") << skill << gettext(" Titty.\n\n");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_TITTY, DayNight);
	}
	else if (roll_b <= 50 && (int)girl->m_Skills[SKILL_ORALSEX] < 100)
	{
		g_Girls.UpdateSkill(girl, SKILL_ORALSEX, skill);
		ss << gettext("You decide to teach her the art of sucking a cock.\n\n");
		ss << gettext("She managed to gain ") << skill << gettext(" Oral.\n\n");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_ORAL, DayNight);
	}
	else if (roll_b <= 70 && (int)girl->m_Skills[SKILL_NORMALSEX] < 100)
	{
		g_Girls.UpdateSkill(girl, SKILL_NORMALSEX, skill);
		ss << gettext("You decide to teach her how to ride a dick like a pro.\n\n");
		ss << gettext("She managed to gain ") << skill << gettext(" Normal Sex.\n\n");
		if (girl->m_Virgin){girl->m_Virgin = false;ss << gettext("She is no longer a virgin.\n");}
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_SEX, DayNight);
		if (!girl->calc_pregnancy(g_Brothels.GetPlayer(), false, 1.0))
		{
			g_MessageQue.AddToQue("She has gotten pregnant", 0);
		}
	}
	else if (roll_b <= 80 && (int)girl->m_Skills[SKILL_ANAL] < 100)
	{
		g_Girls.UpdateSkill(girl, SKILL_ANAL, skill);
		ss << gettext("You decide to teach her how to use her ass.\n\n");
		ss << gettext("She managed to gain ") << skill << gettext(" Anal Sex.\n\n");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_ANAL, DayNight);
	}
	else if (roll_b <= 90 && (int)girl->m_Skills[SKILL_GROUP] < 100)
	{
		g_Girls.UpdateSkill(girl, SKILL_GROUP, skill);
		ss << gettext("You decide to over see her skill in a gang bang.\n\n");
		ss << gettext("She managed to gain ") << skill << gettext(" Group Sex.\n\n");
		if (girl->m_Virgin){ girl->m_Virgin = false; ss << gettext("She is no longer a virgin.\n"); }
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_GROUP, DayNight);
		if (!girl->calc_group_pregnancy(g_Brothels.GetPlayer(), false, 1.0))
		{
			g_MessageQue.AddToQue("She has gotten pregnant", 0);
		}
	}
	else if (roll_b <= 100 && (int)girl->m_Skills[SKILL_BDSM] < 100)
	{
		g_Girls.UpdateSkill(girl, SKILL_BDSM, skill);
		ss << gettext("You decide to teach her the fine art of BDSM.\n\n");
		ss << gettext("She managed to gain ") << skill << gettext(" BDSM.\n\n");
		if (girl->m_Virgin){ girl->m_Virgin = false; ss << gettext("She is no longer a virgin.\n)"); }
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_BDSM, DayNight);
		if (!girl->calc_pregnancy(g_Brothels.GetPlayer(), false, 1.0))
		{
			g_MessageQue.AddToQue("She has gotten pregnant", 0);
		}
	}
	else if ((int)girl->m_Skills[SKILL_BEASTIALITY] < 100)
	{
		g_Girls.UpdateSkill(girl, SKILL_BEASTIALITY, skill);
		ss << gettext("You decide to have her get acquainted with some animals.\n\n");
		ss << gettext("She managed to gain ") << skill << gettext(" Beastiality.\n\n");
		if (girl->m_Virgin && g_Dice%2){ girl->m_Virgin = false; ss << gettext("She is no longer a virgin.\n"); }
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_BEAST, DayNight);
	}
	else															
	{
		if (g_Dice % 2)g_Girls.UpdateSkill(girl, SKILL_STRIP, 1);
		if (g_Dice % 2)g_Girls.UpdateSkill(girl, SKILL_ORALSEX, 1);
		if (g_Dice % 2)g_Girls.UpdateSkill(girl, SKILL_TITTYSEX, 1);
		if (g_Dice % 2)g_Girls.UpdateSkill(girl, SKILL_LESBIAN, 1);
		if (g_Dice % 2)g_Girls.UpdateSkill(girl, SKILL_NORMALSEX, 1);
		if (g_Dice % 2)g_Girls.UpdateSkill(girl, SKILL_ANAL, 1);
		if (g_Dice % 2)g_Girls.UpdateSkill(girl, SKILL_BDSM, 1);
		ss << gettext("You couldn't decide what to teach her so you just fooled around with her.\n\n");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_ECCHI, DayNight);
	}


	// Improve stats
	int xp = 10;

	if (g_Girls.HasTrait(girl, "Quick Learner"))
	{
		xp += 3;
	}
	else if (g_Girls.HasTrait(girl, "Slow Learner"))
	{
		xp -= 3;
	}

	g_Girls.UpdateStat(girl, STAT_EXP, xp);

	return false;
}