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
	int actiontype = ACTION_WORKMOVIE;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;

	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();
	if (roll_a <= 50 && (g_Girls.DisobeyCheck(girl, ACTION_WORKMOVIE, brothel) || g_Girls.DisobeyCheck(girl, ACTION_WORKCLEANING, brothel)))
	{
		ss << " refused to work as a stagehand today.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked as a stagehand.\n\n";

	cConfig cfg;
	g_Girls.UnequipCombat(girl);	// not for studio crew

	int wages = 50;
	int enjoyc = 0, enjoym = 0;
	bool filming = true;
	bool playtime = false;
	int imagetype = IMGTYPE_PROFILE;


	// `J` - jobperformance and CleanAmt need to be worked out specially for this job.
	int jobperformance = 0;
	int CleanAmt = ((g_Girls.GetSkill(girl, SKILL_SERVICE) / 10) + 5) * 5;

	if (g_Studios.GetNumGirlsOnJob(0, JOB_CAMERAMAGE, SHIFT_NIGHT) == 0 ||
		g_Studios.GetNumGirlsOnJob(0, JOB_CRYSTALPURIFIER, SHIFT_NIGHT) == 0 ||
		g_Studios.Num_Actress(0) < 1)
	{
		ss << "There were no scenes being filmed, so she just cleaned the set.\n\n";
		filming = false;
		CleanAmt *= 2;
		imagetype = IMGTYPE_MAID;
	}

	if (g_Girls.HasTrait(girl, "Director"))					{ CleanAmt -= 10;	jobperformance += 15; }
	if (g_Girls.HasTrait(girl, "Actress"))					{ CleanAmt += 0;	jobperformance += 10; }
	if (g_Girls.HasTrait(girl, "Porn Star"))				{ CleanAmt += 0;	jobperformance += 5; }
	if (g_Girls.HasTrait(girl, "Flight"))					{ CleanAmt += 20;	jobperformance += 10; }
	if (g_Girls.HasTrait(girl, "Maid"))						{ CleanAmt += 20;	jobperformance += 2; }
	if (g_Girls.HasTrait(girl, "Powerful Magic"))			{ CleanAmt += 10;	jobperformance += 10; }
	if (g_Girls.HasTrait(girl, "Strong Magic"))				{ CleanAmt += 5;	jobperformance += 5; }
	if (g_Girls.HasTrait(girl, "Handyman"))					{ CleanAmt += 5;	jobperformance += 10; }
	if (g_Girls.HasTrait(girl, "Waitress"))					{ CleanAmt += 5;	jobperformance += 5; }
	if (g_Girls.HasTrait(girl, "Agile"))					{ CleanAmt += 5;	jobperformance += 10; }
	if (g_Girls.HasTrait(girl, "Fleet of Foot"))			{ CleanAmt += 2;	jobperformance += 5; }
	if (g_Girls.HasTrait(girl, "Strong"))					{ CleanAmt += 5;	jobperformance += 10; }
	if (g_Girls.HasTrait(girl, "Assassin"))					{ CleanAmt += 1;	jobperformance += 1; }
	if (g_Girls.HasTrait(girl, "Psychic"))					{ CleanAmt += 2;	jobperformance += 5; }
	if (g_Girls.HasTrait(girl, "Manly"))					{ CleanAmt += 1;	jobperformance += 1; }
	if (g_Girls.HasTrait(girl, "Tomboy"))					{ CleanAmt += 2;	jobperformance += 2; }
	if (g_Girls.HasTrait(girl, "Optimist"))					{ CleanAmt += 1;	jobperformance += 1; }
	if (g_Girls.HasTrait(girl, "Sharp-Eyed"))				{ CleanAmt += 1;	jobperformance += 5; }
	if (g_Girls.HasTrait(girl, "Giant"))					{ CleanAmt += 2;	jobperformance += 2; }
	if (g_Girls.HasTrait(girl, "Prehensile Tail"))			{ CleanAmt += 3;	jobperformance += 3; }

	if (g_Girls.HasTrait(girl, "Blind"))					{ CleanAmt -= 20;	jobperformance -= 20; }
	if (g_Girls.HasTrait(girl, "Queen"))					{ CleanAmt -= 20;	jobperformance -= 10; }
	if (g_Girls.HasTrait(girl, "Princess"))					{ CleanAmt -= 10;	jobperformance -= 5; }
	if (g_Girls.HasTrait(girl, "Mind Fucked"))				{ CleanAmt -= 10;	jobperformance -= 5; }
	if (g_Girls.HasTrait(girl, "Bimbo"))					{ CleanAmt -= 5;	jobperformance -= 5; }
	if (g_Girls.HasTrait(girl, "Retarded"))					{ CleanAmt -= 5;	jobperformance -= 5; }
	if (g_Girls.HasTrait(girl, "Smoker"))					{ CleanAmt -= 1;	jobperformance -= 1; }
	if (g_Girls.HasTrait(girl, "Clumsy"))					{ CleanAmt -= 5;	jobperformance -= 5; }
	if (g_Girls.HasTrait(girl, "Delicate"))					{ CleanAmt -= 1;	jobperformance -= 1; }
	if (g_Girls.HasTrait(girl, "Elegant"))					{ CleanAmt -= 5;	jobperformance -= 1; }
	if (g_Girls.HasTrait(girl, "Malformed"))				{ CleanAmt -= 1;	jobperformance -= 1; }
	if (g_Girls.HasTrait(girl, "Massive Melons"))			{ CleanAmt -= 1;	jobperformance -= 1; }
	if (g_Girls.HasTrait(girl, "Abnormally Large Boobs"))	{ CleanAmt -= 3;	jobperformance -= 1; }
	if (g_Girls.HasTrait(girl, "Titanic Tits"))				{ CleanAmt -= 5;	jobperformance -= 1; }
	if (g_Girls.HasTrait(girl, "Broken Will"))				{ CleanAmt -= 5;	jobperformance -= 10; }
	if (g_Girls.HasTrait(girl, "Pessimist"))				{ CleanAmt -= 1;	jobperformance -= 1; }
	if (g_Girls.HasTrait(girl, "Meek"))						{ CleanAmt -= 2;	jobperformance -= 2; }
	if (g_Girls.HasTrait(girl, "Nervous"))					{ CleanAmt -= 2;	jobperformance -= 3; }
	if (g_Girls.HasTrait(girl, "Dependant"))				{ CleanAmt -= 5;	jobperformance -= 5; }
	if (g_Girls.HasTrait(girl, "Bad Eyesight"))				{ CleanAmt -= 5;	jobperformance -= 5; }


	if (roll_a <= 10)
	{
		enjoyc -= g_Dice % 3 + 1; if (filming) enjoym -= g_Dice % 3 + 1;
		CleanAmt = int(CleanAmt * 0.8);
		ss << "She did not like working in the studio today.";
	}
	else if (roll_a >= 90)
	{
		enjoyc += g_Dice % 3 + 1; if (filming) enjoym += g_Dice % 3 + 1;
		CleanAmt = int(CleanAmt * 1.1);
		ss << "She had a great time working today.";
	}
	else
	{
		enjoyc += max(0, g_Dice % 3 - 1); if (filming) enjoym += max(0, g_Dice % 3 - 1);
		ss << "Otherwise, the shift passed uneventfully.";
	}
	jobperformance += enjoyc + enjoym;
	ss << "\n\n";

	if (filming)
	{
		jobperformance += (((girl->spirit() - 50) / 10) + ((girl->intelligence() - 50) / 10) + (girl->service() / 10)) / 3;
		jobperformance += g_Girls.GetStat(girl, STAT_LEVEL);
		jobperformance += g_Dice % 4 - 1;	// should add a -1 to +3 random element --PP

		if (jobperformance > 0)
		{
			jobperformance = max(1, jobperformance / 10);
			ss << "She helped improve the scene " << (int)jobperformance << "% with her production skills.";
		}
		else if (jobperformance < 0)
		{
			jobperformance = min(-1, jobperformance / 10);
			ss << "She did a bad job today, reduceing the scene quality " << (int)jobperformance << "% with her poor performance.";
		}
		else ss << "She did not really help the scene quality.";
		ss << "\n\n";
	}


	// slave girls not being paid for a job that normally you would pay directly for do less work
	if ((girl->is_slave() && !cfg.initial.slave_pay_outofpocket()))
	{
		CleanAmt = int(CleanAmt * 0.9);
		wages = 0;
	}
	else if (filming)
	{
		wages += CleanAmt + jobperformance;
	}
	else
	{
		wages += CleanAmt;
	}

	ss << gettext("Cleanliness rating improved by ") << CleanAmt;

	if (!filming && brothel->m_Filthiness < CleanAmt / 2) // `J` needs more variation
	{
		ss << "\n\n" << girlName << " finished her cleaning early so she hung out around the Studio a bit.";
		g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, g_Dice % 3 + 1);
		g_Girls.UpdateStat(girl, STAT_HAPPINESS, g_Dice % 3 + 1);
	}


	girl->m_Events.AddMessage(ss.str(), imagetype, SHIFT_NIGHT);
	if (filming) g_Studios.m_StagehandQuality += jobperformance;
	brothel->m_Filthiness -= CleanAmt;
	girl->m_Pay = wages;


	// Improve girl
	int xp = filming ? 10 : 5, skill = 3, libido = 1;
	if (enjoyc + enjoym > 2)							{ xp += 1; skill += 1; }
	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, (g_Dice % skill) + 2);
	g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, libido);

	if (filming) g_Girls.UpdateEnjoyment(girl, ACTION_WORKMOVIE, enjoym);
	g_Girls.UpdateEnjoyment(girl, ACTION_WORKCLEANING, enjoyc);
	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Clumsy", 30, ACTION_WORKCLEANING, "It took her spilling hundreds of buckets, and just as many reprimands, but " + girl->m_Realname + " has finally stopped being so Clumsy.", Day0Night1);

	return false;
}

double cJobManager::JP_FilmStagehand(sGirl* girl, bool estimate)
{
	// this is hard to use because the job does both cleaning and adds to film quality

	int CleanAmt = 0;
	if (estimate)	// for third detail string
	{
		CleanAmt = girl->service() * 2;
	}


	return CleanAmt;
}