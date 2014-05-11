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

bool cJobManager::WorkAdvertising(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";
	if(Preprocessing(ACTION_WORKADVERTISING, girl, brothel, DayNight, summary, message))
		return true;

	// put that shit away, you'll scare off the customers!
	g_Girls.UnequipCombat(girl);

	// How much will she help stretch your advertising budget? Let's find out
	double cval, multiplier = 0.0;

	cval = g_Girls.GetSkill(girl, SKILL_SERVICE);
	if (cval > 0)
	{
		cval = g_Dice%int(cval) + (cval / 2);  // random 50%-150% range
		multiplier += (cval / 3);  // add ~33% of service skill to multiplier
	}
	cval = g_Girls.GetStat(girl, STAT_CHARISMA);
	if (cval > 0)
	{
		cval = g_Dice%int(cval) + (cval / 2);  // random 50%-150% range
		multiplier += (cval / 6);  // add ~17% of charisma to multiplier
	}
	cval = g_Girls.GetStat(girl, STAT_BEAUTY);
	if (cval > 0)
	{
		cval = g_Dice%int(cval) + (cval / 2);  // random 50%-150% range
		multiplier += (cval / 10);  // add 10% of beauty to multiplier
	}
	cval = g_Girls.GetStat(girl, STAT_INTELLIGENCE);
	if (cval > 0)
	{
		cval = g_Dice%int(cval) + (cval / 2);  // random 50%-150% range
		multiplier += (cval / 6);  // add ~17% of intelligence to multiplier
	}
	cval = g_Girls.GetStat(girl, STAT_CONFIDENCE);
	if (cval > 0)
	{
		cval = g_Dice%int(cval) + (cval / 2);  // random 50%-150% range
		multiplier += (cval / 10);  // add 10% of confidence to multiplier
	}
	cval = g_Girls.GetStat(girl, STAT_FAME);
	if (cval > 0)
	{
		cval = g_Dice%int(cval) + (cval / 2);  // random 50%-150% range
		multiplier += (cval / 10);  // add 10% of fame to multiplier
	}

	// useful traits
	if(girl->has_trait("Psychic"))
		multiplier += 10;  // add 10 to multiplier
	if(girl->has_trait("Cool Person"))
		multiplier += 10;  // add 10 to multiplier
	if(girl->has_trait("Sexy Air"))
		multiplier += 10;  // add 10 to multiplier
	if(girl->has_trait("Charismatic"))
		multiplier += 10;  // add 10 to multiplier
	if(girl->has_trait("Charming"))
		multiplier += 10;  // add 10 to multiplier

	// unhelpful traits
	if(girl->has_trait("Nervous"))
		multiplier -= 5;  // subtract 5 from multiplier
	if(girl->has_trait("Clumsy"))
		multiplier -= 5;  // subtract 5 from multiplier
	if(girl->has_trait("Retarded"))
		multiplier -= 20;  // subtract 20 from multiplier
	if(girl->has_trait("Malformed"))
		multiplier -= 20;  // subtract 20 from multiplier

	// Complications
	if(g_Dice%100 <= 10)
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKADVERTISING, -1, true);
		message = gettext("She was harassed and made fun of while advertising.");
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, DayNight);
		multiplier *= 0.8;
	}
	else
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKADVERTISING, +3, true);
		message = gettext("She made sure many people were interested in the buildings facilities.");
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, DayNight);
	}

	stringstream ss;
	ss.str("");
	ss << gettext("She managed to stretch the effectiveness of your advertising budget by about ") << int(multiplier) << gettext("%.");
	ss << gettext(" You paid her 70 gold for her advertising efforts.");
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_SUMMARY);

	// now to boost the brothel's advertising level accordingly
	brothel->m_AdvertisingLevel += (multiplier / 100);

	// Improve girl
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

	girl->m_Pay += 70;
	g_Gold.advertising_costs(70);  // wages come from you
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, skill);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	return false;
}
