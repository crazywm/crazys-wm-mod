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

// `J` Job Movie Studio - Crew
bool cJobManager::WorkFilmPromoter(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKMOVIE;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll = g_Dice.d100();
	if (roll <= 20 && girl->disobey_check(ACTION_WORKMOVIE, brothel))
	{
		ss << " refused to work as a promoter today.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked to promote the sales of the studio's films.\n \n";
	bool movies = brothel->m_NumMovies > 0;
	if (!movies)	ss << "There were no movies for her to promote, so she just promoted the studio in general.\n \n";

	g_Girls.UnequipCombat(girl);	// not for studio crew

	int wages = 50, tips = 0;
	int enjoy = 0;

	if (roll <= 10 || (!movies && roll <= 15))
	{
		enjoy -= g_Dice % 3 + 1;
		ss << "She had difficulties working with advertisers and theater owners" << (movies ? "" : " without movies to sell them");
	}
	else if (roll >= 90)
	{
		enjoy += g_Dice % 3 + 1;
		ss << "She found it easier " << (movies ? "selling the movies" : "promoting the studio") << " today";
	}
	else
	{
		enjoy += g_Dice % 2;
	}
	ss << ".\n \n";
	double jobperformance = JP_FilmPromoter(girl, false);
	jobperformance += enjoy * 2;

	// slave girls not being paid for a job that normally you would pay directly for do less work
	if ((girl->is_slave() && !cfg.initial.slave_pay_outofpocket()))
	{
		jobperformance *= 0.9;
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
		/* */if (jobperformance > 0)	ss << " She helped promote the studio's movies, increasing sales " << (int)jobperformance << "%. \n";
		else if (jobperformance < 0)	ss << " She did a bad job today, she hurt film sales " << (int)jobperformance << "%. \n";
		else /*                   */	ss << " She did not really help film sales.\n";
	}
	else
	{	// `J` zzzzzz - need some effects for this
		/* */if (jobperformance > 0)	ss << " She helped promote the studio. \n";
		else if (jobperformance < 0)	ss << " She did a bad job today, she hurt reputation of the studio. \n";
		else /*                   */	ss << " She did not really help promote the studio.\n";
	}

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, SHIFT_NIGHT);
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);

	g_Studios.m_PromoterBonus = (double)(brothel->m_AdvertisingBudget / 10) + jobperformance;


	// Improve girl
	int xp = 10, skill = 3, libido = 1;

	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }

	girl->exp(xp);
	girl->charisma(g_Dice%skill);
	girl->service(g_Dice%skill);
	girl->upd_temp_stat(STAT_LIBIDO, libido);

	return false;
}

double cJobManager::JP_FilmPromoter(sGirl* girl, bool estimate)// not used
{
	double jobperformance = 0.0;
	if (estimate)// for third detail string
	{
		jobperformance =
			girl->performance() / 6.0 +
			girl->service() / 6.0 +
			girl->charisma() / 6.0 +
			girl->beauty() / 10.0 +
			girl->intelligence() / 6.0 +
			girl->confidence() / 10.0 +
			girl->fame() / 10.0;

		if (girl->is_slave()) jobperformance -= 1000;
	}
	else// for the actual check
	{
		// How much will she help stretch your advertising budget? Let's find out
		double cval = 0.0;
		cval = girl->service();
		if (cval > 0)
		{
			cval = g_Dice % (int)cval + (cval / 2);  // random 50%-150% range
			jobperformance += (cval / 3);  // add ~33% of service skill to jobperformance
		}
		cval = girl->charisma();
		if (cval > 0)
		{
			cval = g_Dice % (int)cval + (cval / 2);  // random 50%-150% range
			jobperformance += (cval / 6);  // add ~17% of charisma to jobperformance
		}
		cval = girl->beauty();
		if (cval > 0)
		{
			cval = g_Dice % (int)cval + (cval / 2);  // random 50%-150% range
			jobperformance += (cval / 10);  // add 10% of beauty to jobperformance
		}
		cval = girl->intelligence();
		if (cval > 0)
		{
			cval = g_Dice % (int)cval + (cval / 2);  // random 50%-150% range
			jobperformance += (cval / 6);  // add ~17% of intelligence to jobperformance
		}
		cval = girl->confidence();
		if (cval > 0)
		{
			cval = g_Dice % (int)cval + (cval / 2);  // random 50%-150% range
			jobperformance += (cval / 10);  // add 10% of confidence to jobperformance
		}
		cval = girl->fame();
		if (cval > 0)
		{
			cval = g_Dice % (int)cval + (cval / 2);  // random 50%-150% range
			jobperformance += (cval / 4);  // add 25% of fame to jobperformance
		}
		cval = girl->level();
		if (cval > 0)
		{
			jobperformance += (cval / 2);  // add 50% of level to jobperformance
		}
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}


	// useful traits
	if (girl->has_trait("Actress"))				jobperformance += 10;	//
	if (girl->has_trait("Charismatic"))			jobperformance += 10;
	if (girl->has_trait("Charming"))			jobperformance += 10;
	if (girl->has_trait("Cool Person"))			jobperformance += 10;
	if (girl->has_trait("Elegant"))				jobperformance += 5;	//
	if (girl->has_trait("Exhibitionist"))		jobperformance += 15;	// Advertising topless
	if (girl->has_trait("Fearless"))			jobperformance += 5;	//
	if (girl->has_trait("Flexible"))			jobperformance += 5;	//
	if (girl->has_trait("Former Official"))		jobperformance += 10;	//
	if (girl->has_trait("Great Arse"))			jobperformance += 5;
	if (girl->has_trait("Great Figure"))		jobperformance += 5;
	if (girl->has_trait("Idol"))				jobperformance += 20;	//
	if (girl->has_trait("Natural Pheromones"))	jobperformance += 10;
	if (girl->has_trait("Optimist"))			jobperformance += 5;	//
	if (girl->has_trait("Porn Star"))			jobperformance += 20;	//
	if (girl->has_trait("Priestess"))			jobperformance += 10;	// used to preaching to the masses
	if (girl->has_trait("Princess"))			jobperformance += 5;	//
	if (girl->has_trait("Psychic"))				jobperformance += 10;
	if (girl->has_trait("Queen"))				jobperformance += 10;	//
	if (girl->has_trait("Quick Learner"))		jobperformance += 5;
	if (girl->has_trait("Sexy Air"))			jobperformance += 10;
	if (girl->has_trait("Shape Shifter"))		jobperformance += 20;	// she can show what is playing
	if (girl->has_trait("Singer"))				jobperformance += 10;	//
	if (girl->has_trait("Slut"))				jobperformance += 10;	//
	if (girl->has_trait("Your Daughter"))		jobperformance += 20;	//
	if (girl->has_trait("Your Wife"))			jobperformance += 20;	//


	// unhelpful traits
	if (girl->has_trait("Alcoholic"))			jobperformance -= 15;
	if (girl->has_trait("Aggressive"))			jobperformance -= 10; //gets mad easy and may attack people
	if (girl->has_trait("Blind"))				jobperformance -= 20;
	if (girl->has_trait("Broken Will"))			jobperformance -= 50;
	if (girl->has_trait("Clumsy"))				jobperformance -= 5;
	if (girl->has_trait("Deaf"))				jobperformance -= 10;
	if (girl->has_trait("Dependant"))			jobperformance -= 50; // needs others to do the job
	if (girl->has_trait("Emprisoned Customer"))	jobperformance -= 30;	// she may be warning the other customers
	if (girl->has_trait("Fairy Dust Addict"))	jobperformance -= 15;
	if (girl->has_trait("Horrific Scars"))		jobperformance -= 10;	//
	if (girl->has_trait("Kidnapped"))			jobperformance -= 40;	// she may try to run away or get help
	if (girl->has_trait("Malformed"))			jobperformance -= 20;
	if (girl->has_trait("Meek"))				jobperformance -= 10;
	if (girl->has_trait("Mute"))				jobperformance -= 10;	//
	if (girl->has_trait("Nervous"))				jobperformance -= 5;
	if (girl->has_trait("No Arms"))				jobperformance -= 40;
	if (girl->has_trait("No Feet"))				jobperformance -= 40;
	if (girl->has_trait("No Hands"))			jobperformance -= 30;
	if (girl->has_trait("No Legs"))				jobperformance -= 40;
	if (girl->has_trait("One Arm"))				jobperformance -= 10;
	if (girl->has_trait("One Foot"))			jobperformance -= 15;
	if (girl->has_trait("One Hand"))			jobperformance -= 5;
	if (girl->has_trait("One Leg"))				jobperformance -= 20;
	if (girl->has_trait("Pessimist"))			jobperformance -= 5;	//
	if (girl->has_trait("Retarded"))			jobperformance -= 20;
	if (girl->has_trait("Skeleton"))			jobperformance -= 50;	//
	if (girl->has_trait("Shy"))					jobperformance -= 10;
	if (girl->has_trait("Slow Learner"))		jobperformance -= 5;
	if (girl->has_trait("Smoker"))				jobperformance -= 10;	//would need smoke breaks
	if (girl->has_trait("Shroud Addict"))		jobperformance -= 15;
	if (girl->has_trait("Viras Blood Addict"))	jobperformance -= 15;
	if (girl->has_trait("Zombie"))				jobperformance -= 50;	//


	return jobperformance;
}
