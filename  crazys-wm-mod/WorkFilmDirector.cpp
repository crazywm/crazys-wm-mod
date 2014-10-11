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

// `J` Movie Studio Job - Crew - Job_is_Matron
bool cJobManager::WorkFilmDirector(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	cConfig cfg;
	stringstream ss;
	string girlName = girl->m_Realname;
	g_Studios.m_DirectorName = girl->m_Realname;
	int wages = 50;
	int enjoy = 0;
	int jobperformance = 0;
	int numgirls = brothel->m_NumGirls;

	g_Girls.UnequipCombat(girl);	// not for studio crew

	ss << girlName << " worked as a film director.\n\n";

	int roll = g_Dice.d100();
	if (roll <= 10 && g_Girls.DisobeyCheck(girl, ACTION_WORKMOVIE, brothel))
	{
		ss << "She refused to work as a director today.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}

	/* */if (roll <= 10) { enjoy -= g_Dice % 3 + 1; ss << "She did not like working in the studio today.\n\n"; }
	else if (roll >= 90) { enjoy += g_Dice % 3 + 1; ss << "She had a great time working today.\n\n"; }
	else /*    */{ enjoy += max(0, g_Dice % 3 - 1); ss << "Otherwise, the shift passed uneventfully.\n\n"; }
	jobperformance = enjoy * 2;
	
	jobperformance += (girl->spirit() - 50) / 10;
	jobperformance += (girl->intelligence() - 50) / 10;
	jobperformance += g_Girls.GetSkill(girl, SKILL_SERVICE) / 10;
	jobperformance /= 3;
	jobperformance += g_Girls.GetStat(girl, STAT_LEVEL);
	jobperformance += g_Girls.GetStat(girl, STAT_FAME) / 10;
	jobperformance += g_Dice % 4 - 1;	// should add a -1 to +3 random element --PP

	/* */if (jobperformance > 0)	ss << "She helped improve the scene " << jobperformance << "% with her Directing skills. \n";
	else if (jobperformance < 0)	ss << "She did a bad job today, she reduced the scene quality " << jobperformance << "% with her poor performance. \n";
	else /*                   */	ss << "She did not really help the scene quality.\n";

	girl->m_Pay = int(float(100.0 + (((girl->get_skill(SKILL_SERVICE) + girl->get_stat(STAT_CHARISMA) + girl->get_stat(STAT_INTELLIGENCE) + girl->get_stat(STAT_CONFIDENCE) + girl->get_skill(SKILL_MEDICINE) + 50) / 50)*numgirls) * cfg.out_fact.matron_wages()));
	g_Studios.m_DirectorQuality += jobperformance;
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, SHIFT_NIGHT);

	// Improve girl
	int xp = 5 + (numgirls / 10), libido = 1, skill = 3, fame = jobperformance / 50;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 5; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 5; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			libido += 2;
	if (g_Girls.HasTrait(girl, "Lesbian"))				libido += numgirls / 20;

	g_Girls.UpdateStat(girl, STAT_EXP, g_Dice%xp + 5);
	g_Girls.UpdateStat(girl, STAT_FAME, g_Dice%fame);
	g_Girls.UpdateStat(girl, STAT_CHARISMA, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, g_Dice%skill + 2);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, g_Dice%libido);

	g_Girls.PossiblyGainNewTrait(girl, "Charismatic", 30, ACTION_WORKMATRON, gettext("She has worked as a matron long enough that she has learned to be more Charismatic."), DayNight != 0);
	g_Girls.PossiblyGainNewTrait(girl, "Psychic", 90, ACTION_WORKMATRON, gettext("She has learned to handle the girls so well that you'd almost think she was Psychic."), DayNight != 0);

	return false;
}
