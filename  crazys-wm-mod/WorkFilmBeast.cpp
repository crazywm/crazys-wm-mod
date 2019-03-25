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
extern cPlayer* The_Player;

// `J` Job Movie Studio - Actress
bool cJobManager::WorkFilmBeast(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKMOVIE;
	stringstream ss;	string girlName = girl->m_Realname;
	// Taken care of in building flow, leaving it in for robustness
	if (g_Studios.GetNumGirlsOnJob(0, JOB_CAMERAMAGE, SHIFT_NIGHT) == 0 || g_Studios.GetNumGirlsOnJob(0, JOB_CRYSTALPURIFIER, SHIFT_NIGHT) == 0)
	{
		girl->m_Events.AddMessage("There was no crew to film the scene, so she took the day off", IMGTYPE_PROFILE, EVENT_NOWORK);
		return false;
	}
	// no beasts = no scene
	if (g_Brothels.GetNumBeasts() < 1)
	{
		girl->m_Events.AddMessage("You have no beasts for this scene, so she had the day off.", IMGTYPE_PROFILE, EVENT_NOWORK);
		return false;
	}

	int wages = 50, tips = 0;
	int enjoy = 0, bonus = 0;
	double jobperformance = JP_FilmBeast(girl, false);
	bool tied = false;

	g_Girls.UnequipCombat(girl);	// not for actress (yet)


	int roll = g_Dice.d100();
	if (girl->health() < 20)
	{
		ss << "The crew refused to film a Bestiality scene with " << girlName << " because she is not healthy enough.\n\"She could get hurt.\"";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else if (girl->has_trait( "Nymphomaniac"))
	{
		ss << "\"Bestiality? Aren't we all animals?!\"\nAs long as it will fuck her, sex addict " << girlName << " really doesn't care WHAT it is.";
	}
	else if (roll <= 10 && !girl->has_trait( "Mind Fucked") && girl->disobey_check(ACTION_WORKMOVIE, brothel))
	{
		ss << girlName << " refused to be fucked by animals on film.";
		if (girl->is_slave())
		{
			if (The_Player->disposition() > 30)  // nice
			{
				ss << " She was so passionate that you allowed her the day off.\n";
				girl->pclove(2);
				girl->pchate(-1);
				girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
				return true;
			}
			else if (The_Player->disposition() > -30) //pragmatic
			{
				ss << " As her owner, you over-ruled and gave her consent.";
				ss << " Your crew readied the cameras, while your men tied her arms behind her back and feet behind her head. \n\"Release the beasts!\"";
				girl->pclove(-1);
				girl->pchate(1);
				girl->pcfear(+1);
				The_Player->disposition(-1);
				tied = true;
				enjoy -= 2;
			}
			else if (The_Player->disposition() > -30)
			{
				ss << " Amused, you have your men flog this slave for a while to remind her of her place.";
				ss << " You offer the film-crew first choice of your more exotic beasts.";
				girl->pclove(-2);
				girl->pchate(+2);
				girl->pcfear(+4);
				girl->spirit(-1);
				The_Player->disposition(-2);
				enjoy -= 6;
				tied = true;
			}
		}
		else // not a slave
		{
			ss << " She left the set.";
			girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
			return true;
		}
	}
	else ss << girlName << " was filmed being fucked by animals.\n \n";

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


	if (girl->check_virginity())
	{
		girl->lose_virginity();
		jobperformance += 50;
		ss << "She is no longer a virgin.\n";
	}

	// remaining modifiers are in the AddScene function --PP
	int finalqual = g_Studios.AddScene(girl, JOB_FILMBEAST, bonus);
	ss << "Her scene is valued at: " << finalqual << " gold.\n";

	// mod: added check for number of beasts owned; otherwise, fake beasts could somehow inseminate the girl
	if (g_Brothels.GetNumBeasts() > 0)
	{
		if (!girl->calc_insemination(*g_Girls.GetBeast(), false, 1.0))
			g_MessageQue.AddToQue(girl->m_Realname + " has gotten inseminated", 0);
	}

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_BEAST, Day0Night1);

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
	girl->beastiality(g_Dice%skill + 1);

	girl->upd_Enjoyment(ACTION_SEX, enjoy);
	girl->upd_Enjoyment(ACTION_WORKMOVIE, enjoy);
	g_Girls.PossiblyGainNewTrait(girl, "Fake Orgasm Expert", 50, ACTION_SEX, "She has become quite the faker.", Day0Night1);
	g_Girls.PossiblyGainNewTrait(girl, "Porn Star", 80, ACTION_WORKMOVIE, "She has performed in enough sex scenes that she has become a well known Porn Star.", Day0Night1);

	//Evil job bonus-------------------------------------------------------
	//BONUS - evil jobs damage her body, break her spirit and make her hate you

	int MrEvil = g_Dice % 8, MrNasty = g_Dice % 8;
	MrEvil = (MrEvil + MrNasty) / 2;				//Should come out around 3 most of the time.

	girl->confidence(-MrEvil);
	girl->spirit(-MrEvil);
	girl->dignity(-MrEvil);
	girl->pclove(-MrEvil);
	girl->pchate(MrEvil);
	girl->pcfear(MrEvil);
	The_Player->disposition(-MrEvil);

	//----------------------------------------------------------------------

	return false;
}

double cJobManager::JP_FilmBeast(sGirl* girl, bool estimate)
{
	double jobperformance =
		((girl->charisma() + girl->beauty()/2)
		+ girl->animalhandling() + girl->beastiality());

	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}

	//Good
	if (girl->has_trait( "Mind Fucked"))					jobperformance += 100;	//Enjoyment and craziness
	if (girl->has_trait( "Masochist"))					jobperformance += 35;	//
	if (girl->has_trait( "Broken Will"))					jobperformance += 30;	//
	if (girl->has_trait( "Dependant"))					jobperformance += 25;	//
	if (girl->has_trait( "Twisted"))						jobperformance += 25;	//
	if (girl->has_trait( "Goddess"))						jobperformance += 60;	//High-status degraded
	if (girl->has_trait( "Angel"))						jobperformance += 50;	//
	if (girl->has_trait( "Queen"))						jobperformance += 50;	//
	if (girl->has_trait( "Princess"))						jobperformance += 40;	//
	if (girl->has_trait( "Noble"))						jobperformance += 15;	//
	if (girl->has_trait( "Idol"))							jobperformance += 25;	//
	if (girl->has_trait( "Priestess"))					jobperformance += 25;	//
	if (girl->has_trait( "Heroine"))						jobperformance += 15;	//
	if (girl->has_trait( "Teacher"))						jobperformance += 15;	//
	if (girl->has_trait( "Tsundere"))						jobperformance += 30;	//beaten customers wanna see this!
	if (girl->has_trait( "Yandere"))						jobperformance += 25;	//

	//Bad
	if (girl->has_trait( "Iron Will"))					jobperformance += 40;	//Try not to put on a show
	if (girl->has_trait( "Fearless"))						jobperformance += 25;	//


	return jobperformance;
}
