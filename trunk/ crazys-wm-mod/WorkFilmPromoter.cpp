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

// `J` Movie Studio Job - Crew
bool cJobManager::WorkFilmPromoter(sGirl* girl, sBrothel* brothel, int Day0Night1, string& summary)
{
	cConfig cfg;
	stringstream ss;
	string girlName = girl->m_Realname;
	int wages = 50;
	int enjoy = 0;
	int jobperformance = 0;
	bool movies = g_Studios.m_NumMovies > 0;

	g_Girls.UnequipCombat(girl);	// not for studio crew

	ss << girlName << " worked to promote the sales of the studio's films.\n\n";

	int roll = g_Dice.d100();
	if (roll <= 10 && g_Girls.DisobeyCheck(girl, ACTION_WORKMOVIE, brothel))
	{
		ss << "She refused to work as a promoter today.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}

	if (!movies)
	{
		ss << "There were no movies for her to promote, so she just promoted the studio in general.\n\n";
	}

	if (roll <= 10 || (!movies && roll <= 15))
	{
		enjoy -= g_Dice % 3 + 1;
		ss << "She had difficulties working with advertisers and theater owners" << (movies ? "" : " without movies to sell them") << ".\n\n";
	}
	else if (roll >= 90) { enjoy += g_Dice % 3 + 1; ss << "She found it easier " << (movies ? "selling the movies" : "promoting the studio") << " today.\n\n"; }
	else /*    */{ enjoy += max(0, g_Dice % 3 - 1); ss << "Otherwise, the shift passed uneventfully.\n\n"; }
	jobperformance = enjoy * 2;

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
	jobperformance += (int)multiplier;

	// useful traits
	if (girl->has_trait("Psychic"))			jobperformance += 10;
	if (girl->has_trait("Cool Person"))		jobperformance += 10;
	if (girl->has_trait("Sexy Air"))		jobperformance += 10;
	if (girl->has_trait("Charismatic"))		jobperformance += 10;
	if (girl->has_trait("Charming"))		jobperformance += 10;
	// unhelpful traits
	if (girl->has_trait("Nervous"))			jobperformance -= 5; 
	if (girl->has_trait("Clumsy"))			jobperformance -= 5; 
	if (girl->has_trait("Retarded"))		jobperformance -= 20;
	if (girl->has_trait("Malformed"))		jobperformance -= 20;


	// slave girls not being paid for a job that normally you would pay directly for do less work
	if ((girl->is_slave() && !cfg.initial.slave_pay_outofpocket()))
	{
		jobperformance = int(jobperformance * 0.9);
		wages = 0;
	}
	else	// work out the pay between the house and the girl
	{
		// `J` zzzzzz - need to change pay so it better reflects how well she promoted the films
		int roll_max = girl->spirit() + girl->intelligence();
		roll_max /= 4;
		wages += 10 + g_Dice%roll_max;
	}

	if (movies)
	{
		/* */if (jobperformance > 0)	ss << " She helped promote the studio's movies, increasing sales " << jobperformance << "%. \n";
		else if (jobperformance < 0)	ss << " She did a bad job today, she hurt film sales " << jobperformance << "%. \n";
		else /*                   */	ss << " She did not really help film sales.\n";
	}
	else
	{	// `J` zzzzzz - need some effects for this
		/* */if (jobperformance > 0)	ss << " She helped promote the studio. \n";
		else if (jobperformance < 0)	ss << " She did a bad job today, she hurt reputation of the studio. \n";
		else /*                   */	ss << " She did not really help promote the studio.\n";
	}

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, SHIFT_NIGHT);
	girl->m_Pay = wages;
	g_Studios.m_PromoterBonus = (double)(brothel->m_AdvertisingBudget / 10) + jobperformance;


	// Improve girl
	int xp = 10, skill = 3, libido = 1;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateStat(girl, STAT_CHARISMA, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, g_Dice%skill);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	return false;
}