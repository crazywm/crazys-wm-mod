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
#include "cRng.h"
#include "CLog.h"
#include "cMessageBox.h"
#include "cGold.h"
#include "cBrothel.h"
#include "cFarm.h"

extern CLog g_LogFile;
extern cMessageQue g_MessageQue;
extern cRng g_Dice;
extern cGold g_Gold;
extern cBrothelManager g_Brothels;
extern cFarmManager g_Farm;

// `J` Job Farm - Staff - Matron_Job - Full_Time_Job
bool cJobManager::WorkFarmManager(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKMATRON;
	// DisobeyCheck is done in the building flow.
	girl->m_DayJob = girl->m_NightJob = JOB_FARMMANGER;	// it is a full time job

	stringstream ss; string girlName = girl->m_Realname; ss << "Farm Manager  " << girlName << "  ";


	int numgirls = brothel->m_NumGirls;
	int wages = 0, tips = 0;
	int enjoy = 0;
	int conf = 0;
	int happy = 0;

	// Complications
	int check = g_Dice.d100();
	if (check < 10 && numgirls >(girl->service() + girl->confidence()) * 3)
	{
		enjoy -= (g_Dice % 6 + 5);
		conf -= 5; happy -= 10;
		ss << "was overwhelmed by the number of girls she was required to manage and broke down crying.";
	}
	else if (check < 10)
	{
		enjoy -= (g_Dice % 3 + 1);
		conf -= -1; happy -= -3;
		ss << "had trouble dealing with some of the girls.";
	}
	else if (check > 90)
	{
		enjoy += (g_Dice % 3 + 1);
		conf += 1; happy += 3;
		ss << "enjoyed helping the girls with their lives.";
	}
	else
	{
		enjoy += (g_Dice % 3 - 1);
		ss << "went about her day as usual.";
	}

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);

#if 0
	// `J` Farm Bookmark - adding in items that can be created in the farm
	// possibly add a non-human girl born on the farm

#endif

	// Improve girl
	int xp = numgirls / 10, libido = 1, skill = 3;

	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 5; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 5; }
	if (girl->has_trait( "Nymphomaniac"))			libido += 2;
	if (girl->has_trait( "Lesbian"))				libido += numgirls / 20;

	wages += int(float(100.0 + (((girl->get_skill(SKILL_SERVICE) + girl->get_stat(STAT_CHARISMA) + girl->get_stat(STAT_INTELLIGENCE) + girl->get_stat(STAT_CONFIDENCE) + girl->get_skill(SKILL_MEDICINE) + 50) / 50)*numgirls) * cfg.out_fact.matron_wages()));
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);

	if (conf>-1) conf += g_Dice%skill;
	girl->confidence(conf);
	girl->happiness(happy);

	girl->exp((g_Dice % xp) + 1);
	girl->upd_temp_stat(STAT_LIBIDO, libido);

	// primary (+2 for single or +1 for multiple)
	girl->medicine(g_Dice%skill);
	// secondary (-1 for one then -2 for others)
	girl->service(g_Dice%skill + 2);

	girl->upd_Enjoyment(actiontype, enjoy);
	g_Girls.PossiblyGainNewTrait(girl, "Charismatic", 30, actiontype, "She has worked as a matron long enough that she has learned to be more Charismatic.", Day0Night1);
	g_Girls.PossiblyGainNewTrait(girl, "Psychic", 60, actiontype, "She has learned to handle the girls so well that you'd almost think she was Psychic.", Day0Night1);

	return false;
}

double cJobManager::JP_FarmManager(sGirl* girl, bool estimate)// not used
{
	double jobperformance = 0.0;
	if (estimate)// for third detail string
	{		// `J` estimate - needs work
		jobperformance = ((girl->charisma() + girl->confidence() + girl->spirit()) / 3) +
			((girl->service() + girl->intelligence() + girl->medicine()) / 3) +
			girl->level();

		if (girl->is_slave()) jobperformance -= 1000;

	}
	else// for the actual check
	{
	}
	return jobperformance;
}
