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
#include "libintl.h"

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;

// `J` Brothel Job - General
bool cJobManager::WorkTorturer(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKTORTURER;
	if (Day0Night1) return false;		// Do this only once a day
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	if (g_Girls.DisobeyCheck(girl, actiontype, brothel))
	{
		ss << " refused to torture anyone.";
		girl->morality(1);
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " is assigned to torture people in the dungeon.";
	girl->morality(-1);

	g_Girls.EquipCombat(girl);	// ready armor and weapons!

	// Complications
#if 0
	if (g_Dice.percent(10))
	{
		g_Girls.UpdateEnjoyment(girl, actiontype, -3, true);
		ss << girlName << gettext(" hurt herself while torturing someone.");
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, Day0Night1);
	}
	else
	{
		g_Girls.UpdateEnjoyment(girl, actiontype, +3, true);
		ss << girlName << gettext(" enjoyed her job working in the dungeon.");
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, Day0Night1);
	}
#else
	//SIN: bit more variety for the above
	int roll(g_Dice % 5);
	bool forFree = false;
	if (g_Dice.percent(10))
	{
		g_Girls.UpdateEnjoyment(girl, actiontype, -3, true);
		if (g_Girls.HasTrait(girl, "Sadistic") || g_Girls.HasTrait(girl, "Merciless") || g_Girls.GetStat(girl, STAT_MORALITY) < 30)
			ss << girlName << gettext(" hurt herself while torturing someone.\n");
		else
		{
			switch (roll)
			{
			case 0:		ss << girlName << gettext(" felt bad torturing people as she could easily see herself in the victim.\n"); break;
			case 1:		ss << girlName << gettext(" doesn't like this as she feels it is wrong to torture people.\n"); break;
			case 2:		ss << girlName << gettext(" feels like a bitch after one of her torture victims wept the entire time and kept begging her to stop.\n"); break;
			case 3:		ss << girlName << gettext(" feels awful after accidentally whipping someone in an excruciating place.\n"); break;
			case 4:		ss << girlName << gettext(" didn't enjoy this as she felt sorry for the victim.\n"); break;
			default:	ss << girlName << gettext(" didn't enjoy this for some illogical reason. [error]\n"); break; //shouldn't happen
			}
			//And a little randomness
			if (g_Dice.percent(40))
			{
				roll = g_Dice % 3;
				switch (roll)
				{
				case 0:
					ss << gettext("She hates you for making her do this today.\n");
					g_Girls.UpdateStat(girl, STAT_PCLOVE, -(g_Dice % 2));
					g_Girls.UpdateStat(girl, STAT_PCHATE, g_Dice % 2);
					break;
				case 1:
					ss << girlName << gettext(" is terrified that you treat people like this.\n");
					g_Girls.UpdateStat(girl, STAT_PCFEAR, g_Dice % 6);
					g_Girls.UpdateStat(girl, STAT_OBEDIENCE, g_Dice % 2);
					break;
				case 2:
					ss << gettext("She learned a bit about medicine while trying to stop the pain.\n");
					g_Girls.UpdateSkill(girl, SKILL_MEDICINE, g_Dice % 10);
					break;
				default:
					ss << girlName << gettext(" did something completely unexpected. [error]");
					break;
				}
			}
		}
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);
	}
	else
	{
		g_Girls.UpdateEnjoyment(girl, actiontype, +3, true);
		switch (roll)
		{
		case 0:		ss << girlName << gettext(" enjoyed her job working in the dungeon.\n"); break;
		case 1:		ss << girlName << gettext(" is turned on by the power of torturing people.\n"); break;
		case 2:		ss << girlName << gettext(" enjoyed trying out different torture devices and watching the effects on the victim.\n"); break;
		case 3:		ss << girlName << gettext(" spent her time in the dungeon whipping her victim in time to music to make amusing sound effects.\n"); break;
		case 4:		ss << girlName << gettext(" uses the victim's cries and screams to to figure out the 'best' areas to torture.\n"); break;
		default:	ss << girlName << gettext(" enjoyed this for some illogical reason. [error]\n"); break;
		}

		//And a little randomness
		if ((g_Girls.GetStat(girl, STAT_MORALITY) < 20 || g_Girls.HasTrait(girl, "Sadistic")) && g_Dice.percent(20))
		{
			ss << girlName << gettext(" loved this so much she wouldn't accept any money, as long as you promise she can do it again soon.\n");
			g_Girls.UpdateEnjoyment(girl, actiontype, +3, true);
			forFree = true;
		}
		if (g_Dice.percent(20))
		{
			roll = g_Dice % 4;
			switch (roll)
			{
			case 0:
				ss << girlName << gettext(" put so much energy into this it seems to have improved her fitness.\n");
				g_Girls.UpdateStat(girl, STAT_CONSTITUTION, g_Dice % 3);
				break;
			case 1:
				ss << girlName << gettext(" went way too far, creating a hell of a mess. Still it looks like she had fun - she hasn't stopped smiling.\n");
				g_Girls.UpdateStat(girl, STAT_HAPPINESS, g_Dice % 5);
				g_Girls.UpdateEnjoyment(girl, actiontype, +1, true);
				brothel->m_Filthiness += 15;
				break;
			case 2:
				ss << girlName << gettext(" over-exerted herself.");
				g_Girls.UpdateStat(girl, STAT_HEALTH, -(g_Dice % 5));
				g_Girls.UpdateStat(girl, STAT_TIREDNESS, g_Dice % 5);
				break;
			case 3:
				ss << girlName << gettext(" appreciates that you entrust her with this kind of work.");
				g_Girls.UpdateStat(girl, STAT_PCLOVE, g_Dice % 2);
				g_Girls.UpdateStat(girl, STAT_PCHATE, -(g_Dice % 2));
				break;
			default:
				ss << girlName << gettext(" did something completely unexpected. [error]");
				break;
			}
		}
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);
	}
#endif

	// Improve girl
	int xp = 15, libido = 5, skill = 1;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	if (!forFree)
	{
		girl->m_Pay += 65;
		g_Gold.staff_wages(65);  // wages come from you
	}

	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateStat(girl, STAT_MORALITY, -2);
	g_Girls.UpdateSkill(girl, SKILL_BDSM, skill);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	// WD: Update flag
	g_Brothels.TortureDone(true);

	// Check for new traits
	g_Girls.PossiblyGainNewTrait(girl, "Sadistic", 30, actiontype, girl->m_Realname + gettext(" has come to enjoy her job so much that she has become rather Sadistic."), Day0Night1);
	g_Girls.PossiblyGainNewTrait(girl, "Merciless", 50, actiontype, girl->m_Realname + gettext(" extensive experience with torture has made her absolutely Merciless."), Day0Night1);

	return false;
}

double cJobManager::JP_Torturer(sGirl* girl, bool estimate)		// not used
{
	double jobperformance = 0.0;
	if (estimate)	// for third detail string
	{

	}
	else			// for the actual security check
	{

	}

	return jobperformance;
}