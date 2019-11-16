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
#include "cCustomers.h"
#include "src/Game.hpp"
#include <sstream>

// `J` Job Movie Studio - Actress
bool cJobManager::WorkFilmBondage(sGirl* girl, bool Day0Night1, string& summary, cRng& rng)
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
	double jobperformance = JP_FilmBondage(girl, false);

	cGirls::UnequipCombat(girl);	// not for actress (yet)

#if 0
	//OLD version
	ss << girlName << " worked as an actress filming BDSM scenes.\n \n";

	int roll = rng.d100();
	if (roll <= 10 && girl->disobey_check(ACTION_WORKMOVIE, brothel))
	{
		ss << "She refused to get beaten on film today.\n";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else if (roll <= 10) { enjoy -= rng % 3 + 1;	ss << "She did not enjoy getting tied up and hurt today.\n \n"; }
	else if (roll >= 90) { enjoy += rng % 3 + 1;	ss << "She had a great time getting spanked and whipped.\n \n"; }
	else /*            */{ enjoy += rng % 2;		ss << "She had just another day in the dungeon.\n \n"; }
	jobperformance = enjoy * 2;
#else
	int bonus = 0;
	int roll = rng.d100();


	if (girl->health() < 40)
	{
		ss << "The crew refused to film a dungeon scene with " << girlName << " because she is not healthy enough.\n\"We are NOT filming snuff.\"";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else if (girl->is_pregnant())
	{
		ss << "The crew refused to do a BDSM scene with " << girlName << " due to her pregnancy.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else if (girl->has_trait( "Mind Fucked"))
	{
		ss << "Mind fucked " << girlName << " seemed at home in the dungeon, and started collecting together tools she should be punished with.\n";
		bonus += 10;
	}
	else if (girl->has_trait( "Masochist"))
	{
		ss << "Masochist " << girlName << " was pleased to in the dungeon. It is her place.\n";
		bonus += 6;
	}
	else if (roll <= 10 && girl->disobey_check(ACTION_WORKMOVIE, JOB_FILMBONDAGE))
	{
		ss << girlName << " was horrified and refused to be beaten and sexually tortured in this ";
		if (girl->is_slave())
		{
			if (g_Game->player().disposition() > 30)  // nice
			{
				ss << "\"monstrous\" place.\nShe was starting to panic, so you allowed her the day off.\n";
				girl->pclove(2);
				girl->pchate(-1);
				girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
				return true;
			}
			else if (g_Game->player().disposition() > -30) //pragmatic
			{
				ss << "\"monstrous\" place.\nShe was starting to panic, so you ordered your men to grab her and bind her for action.\n";
				girl->pclove(-1);
				girl->pchate(2);
				girl->pcfear(2);
				g_Game->player().disposition(-1);
				enjoy -= 2;
			}
			else if (g_Game->player().disposition() > -30)
			{
				ss << "\"monstrous\" place.\nShe was starting to panic so you ordered your men to quickly grab, strip and bind her. Finally, ";
				if (girl->has_trait( "Pierced Nipples"))
				{
					ss << "noticing her pierced nipples";
					if (girl->has_trait( "Pierced Clit"))
					{
						ss << " and clit";
					}
					ss << ", ";
				}
				else if (girl->has_trait( "Pierced Clit")) ss << "noticing her clittoral piercing, ";
				ss << "you personally selected some 'fun tools' for your actor, instructing him to train your slave in humility and obedience.\n\"Master her. Intimately.\"";
				girl->pclove(-4);
				girl->pchate(+5);
				girl->pcfear(+5);
				g_Game->player().disposition(-2);
				enjoy -= 6;
			}
		}
		else // not a slave
		{
			ss << " \"monstrous\" place.\n";
			girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
			return true;
		}
	}
	else ss << girlName << " was taken for bondage and torture scenes in your dungeon.\n \n";

	//Qual
	if (jobperformance >= 350)
	{
		ss << "It was an excellent scene.";
		bonus += 12;
	}
	else if (jobperformance >= 245)
	{
		ss << "It was mostly an excellent scene.";
		bonus += 6;
	}
	else if (jobperformance >= 185)
	{
		ss << "Overall, it was an solid scene.";
		bonus += 4;
	}
	else if (jobperformance >= 145)
	{
		ss << "Overall, it wasn't a bad scene.";
		bonus += 2;
	}
	else if (jobperformance >= 100)
	{
		ss << "It wasn't a great scene.";
		bonus++;
	}
	else
	{
		ss << "It was a poor scene.";
	}
	ss << "\n";


	//Enjoyed? If she's deranged, she'd should have enjoyed it.
	if (girl->has_trait( "Mind Fucked"))
	{
		enjoy += 16;
		ss << "Being completely mind fucked, " << girlName << " actually gets off on this.\n";
	}
	else if (girl->has_trait( "Masochist"))
	{
		enjoy += 10;
		ss << girlName << " enjoys this. It's what she deserves.\n";
	}
	else if (girl->has_trait( "Broken Will") || girl->has_trait( "Dependant"))
	{
		enjoy += 5;
		ss << girlName << " accepts this. It is Master's will.\n";
	}
	else if (girl->has_trait( "Iron Will") || girl->has_trait( "Fearless"))
	{
		enjoy -= 5;
		ss << girlName << " endures in stoic silence, determined not to let you see her suffer.\n";
	}
	else if (girl->has_trait( "Nymphomaniac"))
	{
		enjoy += 2;
		ss << girlName << " doesn't much like the pain, but loves the sex and attention.\n";
	}

	//Feedback enjoyment
	if (enjoy > 10)
	{
		ss << "She won't say it, but you suspect she secretly gets off on the attention, sin and degradation.\n \n";
	}
	else if (enjoy > 0)
	{
		ss << "She's only a little traumatised.\n \n";
	}
	else
	{
		ss << "From the way she's crouched, rocking in a corner and silently weeping... you figure she didn't enjoy this.\n \n";
	}
#endif

	if (girl->check_virginity())
	{
		girl->lose_virginity();	// `J` updated for trait/status
		jobperformance += 50;
		ss << "She is no longer a virgin.\n";
	}
	sCustomer Cust = g_Game->GetCustomer(*brothel);
	Cust.m_Amount = 1;
	if (Cust.m_IsWoman)	// FemDom
	{
		jobperformance += 20;
        /* */if (girl->has_trait("Lesbian"))	jobperformance += 20;
        else if (girl->has_trait("Straight"))	jobperformance -= 20;
	}
	else
	{
		if (!girl->calc_pregnancy(Cust, false, 0.75))
			g_Game->push_message(girl->m_Realname + " has gotten pregnant", 0);
        /* */if (girl->has_trait("Lesbian"))	jobperformance -= 10;
        else if (girl->has_trait("Straight"))	jobperformance += 10;
	}

	//Evil pays more and costs about the same...
	int finalqual = brothel->AddScene(girl, JOB_FILMBONDAGE, bonus);
	ss << "Her scene is valued at: " << finalqual << " gold.\n";

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_BDSM, Day0Night1);

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
	girl->performance(rng%skill);
	girl->bdsm(rng%skill + 1);

	girl->upd_Enjoyment(ACTION_SEX, enjoy);
	girl->upd_Enjoyment(ACTION_WORKMOVIE, enjoy);
	//gain
	cGirls::PossiblyGainNewTrait(girl, "Fake Orgasm Expert", 50, ACTION_SEX, "She has become quite the faker.", Day0Night1);
	cGirls::PossiblyGainNewTrait(girl, "Masochist", 65, ACTION_SEX, girlName + " has turned into a Masochist from filming so many BDSM scenes.", Day0Night1);
	cGirls::PossiblyGainNewTrait(girl, "Slut", 80, ACTION_SEX, girlName + " has turned into quite a slut.", Day0Night1);
	cGirls::PossiblyGainNewTrait(girl, "Porn Star", 80, ACTION_WORKMOVIE, "She has performed in enough sex scenes that she has become a well known Porn Star.", Day0Night1);
	//lose

	//Evil job bonus-------------------------------------------------------
	//BONUS - evil jobs damage her body, break her spirit and make her hate you

	int MrEvil = rng % 8, MrNasty = rng % 8;
	MrEvil = (MrEvil + MrNasty) / 2;				//Should come out around 3 most of the time.

	girl->confidence(-MrEvil);
	girl->spirit(-MrEvil);
	girl->dignity(-MrEvil);
	girl->pclove(-MrEvil);
	girl->pchate(MrEvil);
	girl->pcfear(MrEvil);
	g_Game->player().disposition(-MrEvil);

	//----------------------------------------------------------------------
	return false;
}

double cJobManager::JP_FilmBondage(sGirl* girl, bool estimate)// not used
{
	double jobperformance =
		(girl->charisma() + girl->beauty()
		+ girl->dignity());

	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}

	if (girl->check_virginity())						jobperformance += 20;	//

    jobperformance += girl->get_trait_modifier("work.film.bondage");

    return jobperformance;
}
