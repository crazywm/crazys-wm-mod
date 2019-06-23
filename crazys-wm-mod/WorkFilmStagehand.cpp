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
#include "cCustomers.h"
#include "cGangs.h"
#include "cGold.h"
#include "cInventory.h"
#include "cJobManager.h"
#include "CLog.h"
#include "cMessageBox.h"
#include "cRival.h"
#include "cRng.h"
#include "cTariff.h"
#include "cTrainable.h"
#include "libintl.h"
#include "sConfig.h"
#include <sstream>
#include "cBrothel.h"
#include "cMovieStudio.h"

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cMovieStudioManager g_Studios;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;

// `J` Job Movie Studio - Crew - job_is_cleaning
bool cJobManager::WorkFilmStagehand(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKMOVIE; int actiontype2 = ACTION_WORKCLEANING;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();
	if (roll_a <= 50 && (girl->disobey_check(actiontype, brothel) || girl->disobey_check(actiontype2, brothel)))
	{
		ss << " refused to work as a stagehand today.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked as a stagehand.\n \n";

	g_Girls.UnequipCombat(girl);	// not for studio crew
	int enjoyc = 0, enjoym = 0;
	int wages = 50;
	int tips = 0;
	int imagetype = IMGTYPE_PROFILE;
	int msgtype = Day0Night1;
	bool playtime = false;
	bool filming = true;


	// `J` - jobperformance and CleanAmt need to be worked out specially for this job.
	double jobperformance = 0;
	double CleanAmt = ((girl->service() / 10.0) + 5) * 5;

	if (girl->has_trait( "Director"))					{ CleanAmt -= 10;	jobperformance += 15; }
	if (girl->has_trait( "Actress"))					{ CleanAmt += 0;	jobperformance += 10; }
	if (girl->has_trait( "Porn Star"))				{ CleanAmt += 0;	jobperformance += 5; }
	if (girl->has_trait( "Flight"))					{ CleanAmt += 20;	jobperformance += 10; }
	if (girl->has_trait( "Maid"))						{ CleanAmt += 20;	jobperformance += 2; }
	if (girl->has_trait( "Powerful Magic"))			{ CleanAmt += 10;	jobperformance += 10; }
	if (girl->has_trait( "Strong Magic"))				{ CleanAmt += 5;	jobperformance += 5; }
	if (girl->has_trait( "Handyman"))					{ CleanAmt += 5;	jobperformance += 10; }
	if (girl->has_trait( "Waitress"))					{ CleanAmt += 5;	jobperformance += 5; }
	if (girl->has_trait( "Agile"))					{ CleanAmt += 5;	jobperformance += 10; }
	if (girl->has_trait( "Fleet of Foot"))			{ CleanAmt += 2;	jobperformance += 5; }
	if (girl->has_trait( "Strong"))					{ CleanAmt += 5;	jobperformance += 10; }
	if (girl->has_trait( "Assassin"))					{ CleanAmt += 1;	jobperformance += 1; }
	if (girl->has_trait( "Psychic"))					{ CleanAmt += 2;	jobperformance += 5; }
	if (girl->has_trait( "Manly"))					{ CleanAmt += 1;	jobperformance += 1; }
	if (girl->has_trait( "Tomboy"))					{ CleanAmt += 2;	jobperformance += 2; }
	if (girl->has_trait( "Optimist"))					{ CleanAmt += 1;	jobperformance += 1; }
	if (girl->has_trait( "Sharp-Eyed"))				{ CleanAmt += 1;	jobperformance += 5; }
	if (girl->has_trait( "Giant"))					{ CleanAmt += 2;	jobperformance += 2; }
	if (girl->has_trait( "Prehensile Tail"))			{ CleanAmt += 3;	jobperformance += 3; }

	if (girl->has_trait( "Blind"))					{ CleanAmt -= 20;	jobperformance -= 20; }
	if (girl->has_trait( "Queen"))					{ CleanAmt -= 20;	jobperformance -= 10; }
	if (girl->has_trait( "Princess"))					{ CleanAmt -= 10;	jobperformance -= 5; }
	if (girl->has_trait( "Mind Fucked"))				{ CleanAmt -= 10;	jobperformance -= 5; }
	if (girl->has_trait( "Bimbo"))					{ CleanAmt -= 5;	jobperformance -= 5; }
	if (girl->has_trait( "Retarded"))					{ CleanAmt -= 5;	jobperformance -= 5; }
	if (girl->has_trait( "Smoker"))					{ CleanAmt -= 1;	jobperformance -= 1; }
	if (girl->has_trait( "Clumsy"))					{ CleanAmt -= 5;	jobperformance -= 5; }
	if (girl->has_trait( "Delicate"))					{ CleanAmt -= 1;	jobperformance -= 1; }
	if (girl->has_trait( "Elegant"))					{ CleanAmt -= 5;	jobperformance -= 1; }
	if (girl->has_trait( "Malformed"))				{ CleanAmt -= 1;	jobperformance -= 1; }
	if (girl->has_trait( "Massive Melons"))			{ CleanAmt -= 1;	jobperformance -= 1; }
	if (girl->has_trait( "Abnormally Large Boobs"))	{ CleanAmt -= 3;	jobperformance -= 1; }
	if (girl->has_trait( "Titanic Tits"))				{ CleanAmt -= 5;	jobperformance -= 1; }
	if (girl->has_trait( "Broken Will"))				{ CleanAmt -= 5;	jobperformance -= 10; }
	if (girl->has_trait( "Pessimist"))				{ CleanAmt -= 1;	jobperformance -= 1; }
	if (girl->has_trait( "Meek"))						{ CleanAmt -= 2;	jobperformance -= 2; }
	if (girl->has_trait( "Nervous"))					{ CleanAmt -= 2;	jobperformance -= 3; }
	if (girl->has_trait( "Dependant"))				{ CleanAmt -= 5;	jobperformance -= 5; }
	if (girl->has_trait( "Bad Eyesight"))				{ CleanAmt -= 5;	jobperformance -= 5; }


	if (g_Studios.GetNumGirlsOnJob(0, JOB_CAMERAMAGE, SHIFT_NIGHT) == 0 ||
		g_Studios.GetNumGirlsOnJob(0, JOB_CRYSTALPURIFIER, SHIFT_NIGHT) == 0 ||
		g_Studios.Num_Actress(0) < 1)
	{
		ss << "There were no scenes being filmed, so she just cleaned the set.\n \n";
		filming = false;
		imagetype = IMGTYPE_MAID;
	}
	else	// she worked on the film so only cleaned half as much
	{
		CleanAmt *= 0.5;
	}

	if (roll_a <= 10)
	{
		enjoyc -= g_Dice % 3 + 1; if (filming) enjoym -= g_Dice % 3 + 1;
		CleanAmt *= 0.8;
		ss << "She did not like working in the studio today.";
	}
	else if (roll_a >= 90)
	{
		enjoyc += g_Dice % 3 + 1; if (filming) enjoym += g_Dice % 3 + 1;
		CleanAmt *= 1.1;
		ss << "She had a great time working today.";
	}
	else
	{
		enjoyc += max(0, g_Dice % 3 - 1); if (filming) enjoym += max(0, g_Dice % 3 - 1);
		ss << "Otherwise, the shift passed uneventfully.";
	}
	jobperformance += enjoyc + enjoym;
	ss << "\n \n";

	if (filming)
	{
		jobperformance += (((girl->spirit() - 50) / 10) + ((girl->intelligence() - 50) / 10) + (girl->service() / 10)) / 3;
		jobperformance += girl->level();
		jobperformance += g_Dice % 4 - 1;	// should add a -1 to +3 random element --PP

		if (jobperformance > 0)
		{
			jobperformance = max(1.0, jobperformance / 10);
			ss << "She helped improve the scene " << (int)jobperformance << "% with her production skills.";
		}
		else if (jobperformance < 0)
		{
			jobperformance = min(-1.0, jobperformance / 10);
			ss << "She did a bad job today, reduceing the scene quality " << (int)jobperformance << "% with her poor performance.";
		}
		else ss << "She did not really help the scene quality.";
		ss << "\n \n";
	}


	// slave girls not being paid for a job that normally you would pay directly for do less work
	if ((girl->is_slave() && !cfg.initial.slave_pay_outofpocket()))
	{
		CleanAmt *= 0.9;
		wages = 0;
	}
	else if (filming)
	{
		wages += int(CleanAmt + jobperformance);
	}
	else
	{
		wages += int(CleanAmt);
	}

	ss << "Cleanliness rating improved by " << (int)CleanAmt;

	if (!filming && brothel->m_Filthiness < CleanAmt / 2) // `J` needs more variation
	{
		ss << "\n \n" << girlName << " finished her cleaning early so she hung out around the Studio a bit.";
		girl->upd_temp_stat(STAT_LIBIDO, g_Dice % 3 + 1, true);
		girl->happiness(g_Dice % 3 + 1);
	}


	girl->m_Events.AddMessage(ss.str(), imagetype, SHIFT_NIGHT);
	if (filming) g_Studios.m_StagehandQuality += int(jobperformance);
	brothel->m_Filthiness -= int(CleanAmt);
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);


	// Improve girl
	int xp = filming ? 10 : 5, skill = 3, libido = 1;
	if (enjoyc + enjoym > 2)							{ xp += 1; skill += 1; }
	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }

	girl->exp(xp);
	girl->service((g_Dice % skill) + 2);
	girl->upd_temp_stat(STAT_LIBIDO, libido);

	if (filming) girl->upd_Enjoyment(actiontype, enjoym);
	girl->upd_Enjoyment(actiontype2, enjoyc);
	// Gain Traits
	if (g_Dice.percent(girl->service()))
		g_Girls.PossiblyGainNewTrait(girl, "Maid", 90, actiontype2, girlName + " has cleaned enough that she could work professionally as a Maid anywhere.", Day0Night1);
	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Clumsy", 30, actiontype2, "It took her spilling hundreds of buckets, and just as many reprimands, but " + girl->m_Realname + " has finally stopped being so Clumsy.", Day0Night1);

	return false;
}

double cJobManager::JP_FilmStagehand(sGirl* girl, bool estimate)
{
	return JP_Cleaning(girl, estimate);		// just use cleaning so there is 1 less thing to edit
}
