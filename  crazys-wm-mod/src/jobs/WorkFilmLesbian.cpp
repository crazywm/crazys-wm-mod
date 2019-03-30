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
#include "src/buildings/cBrothel.h"
#include "cRng.h"
#include "src/buildings/cMovieStudio.h"
#include "src/Game.hpp"
#include <sstream>

extern cRng g_Dice;

// `J` Job Movie Studio - Actress
bool cJobManager::WorkFilmLesbian(sGirl* girl, bool Day0Night1, string& summary)
{
    auto brothel = dynamic_cast<sMovieStudio*>(girl->m_Building);

	int actiontype = ACTION_WORKMOVIE;
	// No film crew.. then go home	// `J` this will be taken care of in building flow, leaving it in for now
	if (brothel->num_girls_on_job(JOB_CAMERAMAGE, SHIFT_NIGHT) == 0 || brothel->num_girls_on_job(JOB_CRYSTALPURIFIER, SHIFT_NIGHT) == 0)
	{
		girl->m_Events.AddMessage("There was no crew to film the scene, so she took the day off", IMGTYPE_PROFILE, EVENT_NOWORK);
		return false;
	}

	stringstream ss;
	string girlName = girl->m_Realname;
	int wages = 50, tips = 0;
	int enjoy = 0;
	int jobperformance = 0;
	int bonus = 0;

	cGirls::UnequipCombat(girl);	// not for actress (yet)

	ss << girlName << " worked as an actress filming lesbian scenes.\n \n";

	int roll = g_Dice.d100();
	if (roll <= 10 && girl->disobey_check(ACTION_WORKMOVIE, JOB_FILMLESBIAN))
	{
		ss << "She refused to film a lesbian scene today.\n";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else if (roll <= 10) { enjoy -= g_Dice % 3 + 1;	ss << "She didn't like doing it, but but she made the actress come.\n \n"; }
	else if (roll >= 90) { enjoy += g_Dice % 3 + 1;	ss << "She was amazed how good it felt to have her pussy eaten, and gave as good in return.\n \n"; }
	else /*            */{ enjoy += g_Dice % 2;		ss << "She had a pleasant day playing with her girlfriend.\n \n"; }
	jobperformance = enjoy * 2;

	if (girl->check_virginity())
	{
		jobperformance += 20;
		ss << "No 'Man' has been with her, She is still a virgin.\n";
	}
	// remaining modifiers are in the AddScene function --PP
	int finalqual = brothel->AddScene(girl, JOB_FILMLESBIAN, bonus);
	ss << "Her scene is valued at: " << finalqual << " gold.\n";

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_LESBIAN, Day0Night1);

	// work out the pay between the house and the girl
	if (girl->is_slave() && !cfg.initial.slave_pay_outofpocket())
	{
		wages = 0;	// You own her so you don't have to pay her.
	}
	else
	{
		wages += finalqual * 2;
	}
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);

	// Improve stats
	int xp = 10, skill = 3;

	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }

	girl->exp(xp);
	girl->performance(g_Dice%skill);
	girl->lesbian(g_Dice%skill + 1);

	girl->upd_Enjoyment(ACTION_SEX, enjoy);
	girl->upd_Enjoyment(ACTION_WORKMOVIE, enjoy);
	cGirls::PossiblyGainNewTrait(girl, "Fake Orgasm Expert", 50, ACTION_SEX, "She has become quite the faker.", Day0Night1);
	cGirls::PossiblyGainNewTrait(girl, "Porn Star", 80, ACTION_WORKMOVIE, "She has performed in enough sex scenes that she has become a well known Porn Star.", Day0Night1);

	return false;
}

double cJobManager::JP_FilmLesbian(sGirl* girl, bool estimate)// not used
{
	return 0;
}
