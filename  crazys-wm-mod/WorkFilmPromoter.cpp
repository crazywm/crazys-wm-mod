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
extern cMovieStudioManager g_Studios;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;

bool cJobManager::WorkFilmPromoter(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";
	char buffer[1000];
	int jobperformance = 0;
	string girlName = girl->m_Realname;

	g_Girls.UnequipCombat(girl);
	
	girl->m_Pay += 80;
	
	message = girlName;	
	message += gettext(" worked to promote the sales of the studio's films.\n\n");
	
	int roll = g_Dice%100;
	if (roll <= 10 && g_Girls.DisobeyCheck(girl, ACTION_WORKMOVIE, brothel))
	{
		message = girl->m_Realname + gettext(" refused to work as a promoter today.");

		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else if(roll <= 15) {
		message += gettext(" She had difficulties working with advertisers and theater owners.\n\n");
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKMOVIE, -1, true);
		jobperformance += -5;
	}
	else if(roll >=90)
	{
		message += gettext(" She found it easier selling the movies today.\n\n");
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKMOVIE, +3, true);
		jobperformance += 5;
	}
	else
	{
		message += gettext(" Otherwise, the shift passed uneventfully.\n\n");
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKMOVIE, +1, true);
	}

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
		multiplier += (cval / 4);  // add 25% of fame to multiplier
	}
	cval = g_Girls.GetStat(girl, STAT_LEVEL);
	if (cval > 0)
	{
		multiplier += (cval / 2);  // add 50% of level to multiplier
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

	multiplier += jobperformance;
	jobperformance = (int)multiplier;

	g_Studios.m_PromoterBonus = (double)(brothel->m_AdvertisingBudget / 10) + multiplier;
	
/*
 *	work out the pay between the house and the girl
 *
 *	the original calc took the average of beauty and charisma and halved it
 */
	int roll_max = girl->spirit() + girl->intelligence();
	roll_max /= 4;
	girl->m_Pay += 10 + g_Dice%roll_max;


	if(jobperformance > 0)
	{
	message += girlName + gettext(" helped promote the studio's movies, increasing sales ");
						_itoa(jobperformance,buffer,10);
						message += buffer;
						message += "%. \n";
	}
	else if(jobperformance < 0)
	{
	message += girlName + gettext(" did a bad job today, she hurt film sales ");
					_itoa(jobperformance,buffer,10);
					message += buffer;
					message += "%. \n";
	}
	else
		message += girlName + gettext(" did not really help film sales.\n");

	girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, SHIFT_NIGHT);

	// now to boost the brothel's advertising level accordingly

	// Improve girl
	int xp = 5, skill = 3;

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

	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, skill);

	return false;
}
