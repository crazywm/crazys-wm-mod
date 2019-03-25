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

// `J` Job Movie Studio - Crew - Matron_Job - Full_Time_Job
bool cJobManager::WorkFilmDirector(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKMATRON;
	// DisobeyCheck is done in the building flow.
	stringstream ss; string girlName = girl->m_Realname;
	g_Studios.m_DirectorName = girl->m_Realname;

	ss << girlName << " worked as a Film Director.\n \n";

	g_Girls.UnequipCombat(girl);	// not for studio crew

	int wages = 50;
	int tips = 0;
	int enjoy = 0;
	int numgirls = brothel->m_NumGirls;

	int roll = g_Dice.d100();
	if (roll <= 10)
	{
		enjoy -= g_Dice % 3 + 1;
		ss << "She did not like working in the Studio today.\n \n";
	}
	else if (roll >= 90)
	{
		enjoy += g_Dice % 3 + 1;
		ss << "She had a great time working today.\n \n";
	}
	else
	{
		enjoy += g_Dice % 2;
	}

	double jobperformance = JP_FilmDirector(girl, false);
	jobperformance += enjoy * 2;

	/* */if (jobperformance > 0)	ss << "She helped improve the scene " << (int)jobperformance << "% with her Directing skills. \n";
	else if (jobperformance < 0)	ss << "She did a bad job today, she reduced the scene quality " << (int)jobperformance << "% with her poor performance. \n";
	else /*                   */	ss << "She did not really help the scene quality.\n";

	wages += int(float(100.0 + (((girl->get_skill(SKILL_SERVICE) + girl->get_stat(STAT_CHARISMA) + girl->get_stat(STAT_INTELLIGENCE) + girl->get_stat(STAT_CONFIDENCE) + girl->get_skill(SKILL_MEDICINE) + 50) / 50)*numgirls) * cfg.out_fact.matron_wages()));
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);
	g_Studios.m_DirectorQuality += (int)jobperformance;
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, SHIFT_NIGHT);

	// Improve girl
	int xp = numgirls / 10, libido = 1, skill = 3, fame = (int)(jobperformance / 50);

	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 5; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 5; }
	if (girl->has_trait( "Nymphomaniac"))			libido += 2;
	if (girl->has_trait( "Lesbian"))				libido += numgirls / 20;

	girl->exp(g_Dice%xp + 5);
	girl->fame(g_Dice%fame);
	girl->charisma(g_Dice%skill);
	girl->service(g_Dice%skill + 2);
	girl->upd_temp_stat(STAT_LIBIDO, g_Dice%libido);

	g_Girls.PossiblyGainNewTrait(girl, "Charismatic", 30, actiontype, "She has worked as a matron long enough that she has learned to be more Charismatic.", Day0Night1);
	g_Girls.PossiblyGainNewTrait(girl, "Psychic", 90, actiontype, "She has learned to handle the girls so well that you'd almost think she was Psychic.", Day0Night1);

	return false;
}

double cJobManager::JP_FilmDirector(sGirl* girl, bool estimate)// not used
{
	double jobperformance = 0.0;
	if (estimate)// for third detail string
	{
		jobperformance +=
			(girl->intelligence() / 2) +
			(girl->spirit() / 2) +
			(girl->fame() / 2) +
			(girl->service() / 2) +
			girl->level();

	}
	else// for the actual check
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= t / 4;

		jobperformance += (girl->spirit() - 50) / 10;
		jobperformance += (girl->intelligence() - 50) / 10;
		jobperformance += girl->service() / 10;
		jobperformance /= 3;
		jobperformance += girl->level();
		jobperformance += girl->fame() / 10;
		jobperformance += g_Dice % 4 - 1;	// should add a -1 to +3 random element --PP

	}
	return jobperformance;
}
