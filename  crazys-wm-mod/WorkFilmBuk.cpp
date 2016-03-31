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
extern sGang g_Gang;
extern cPlayer* The_Player;



//Useful fn
void AndAction(stringstream *, string, bool);

// Job Movie Studio - Evil Jobs - Bukakke
bool cJobManager::WorkFilmBuk(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKMOVIE;
	// No film crew.. then go home	// `J` this will be taken care of in building flow, leaving it in for now
	if (g_Studios.GetNumGirlsOnJob(0, JOB_CAMERAMAGE, SHIFT_NIGHT) == 0 || g_Studios.GetNumGirlsOnJob(0, JOB_CRYSTALPURIFIER, SHIFT_NIGHT) == 0)
	{
		girl->m_Events.AddMessage("There was no crew to film the scene, so she took the day off", IMGTYPE_PROFILE, EVENT_NOWORK);
		return false;
	}

	stringstream ss;
	string girlName = girl->m_Realname;
	int wages = 50;
	int enjoy = 0, bonus = 0;
	bool tied = false;
	double jobperformance = JP_FilmBuk(girl, false);


	g_Girls.UnequipCombat(girl);	// not for actress (yet)

	int roll = g_Dice.d100();
	if (g_Girls.HasTrait(girl, "Cum Addict"))
	{
		ss << gettext("Cum-craving ") << girlName << gettext(" couldn't wait to get sticky in this bukkake scene, and was sucking guys off before the lighting was even set up.");
		bonus += 10;
	}
	else if (roll <= 10 && !g_Girls.HasTrait(girl, "Mind Fucked") && g_Girls.DisobeyCheck(girl, ACTION_WORKMOVIE, brothel))
	{
		ss << girlName << gettext(" refused to have any part in this");
		if (girl->is_slave())
		{
			if (The_Player->disposition() > 30)  //Nice
			{
				ss << gettext(" \"filthy\" bukkake scene.\nShe was clearly upset so you allowed her the day off.");
				g_Girls.UpdateStat(girl, STAT_PCLOVE, 2);
				g_Girls.UpdateStat(girl, STAT_PCHATE, -1);
				girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
				return true;
			}
			else if (The_Player->disposition() > -30) //pragmatic
			{
				ss << gettext(" \"filthy\" bukkake scene.\nShe was clearly upset so you had your men drug her and tie her down for action.");
				g_Girls.UpdateStat(girl, STAT_PCLOVE, -1);
				g_Girls.UpdateStat(girl, STAT_PCHATE, 1);
				g_Girls.UpdateStat(girl, STAT_PCFEAR, +1);
				The_Player->disposition(-1);
				tied = true;
				enjoy -= 2;
			}
			else if (The_Player->disposition() > -30)
			{
				ss << gettext(" \"filthy\" bukkake scene.\nShe was clearly upset so you had your men whip some sense into her before the scene and tie her down for action.");
				g_Girls.UpdateStat(girl, STAT_PCLOVE, -2);
				g_Girls.UpdateStat(girl, STAT_PCHATE, +2);
				g_Girls.UpdateStat(girl, STAT_PCFEAR, +4);
				The_Player->disposition(-2);
				enjoy -= 6;
				tied = true;
			}
		}
		else //not a slave
		{
			ss << " \"disgusting\" bukkake scene and left the set.";
			girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
			return true;
		}
	}
	else ss << girlName << gettext(" was filmed being splattered with cum in bukkake scenes.");


	ss << gettext("\n");

	//Lights, camera...
	AndAction(&ss, girlName, tied);


	if (jobperformance >= 350)
	{
		ss << gettext("It was an excellent scene.");
		bonus += 12;
	}
	else if (jobperformance >= 245)
	{
		ss << gettext("It was mostly an excellent scene.");
		bonus += 6;
	}
	else if (jobperformance >= 185)
	{
		ss << gettext("Overall, it was an solid scene.");
		bonus += 4;
	}
	else if (jobperformance >= 145)
	{
		ss << gettext("Overall, it wasn't a bad scene.");
		bonus += 2;
	}
	else if (jobperformance >= 100)
	{
		ss << gettext("It wasn't a great scene.");
		bonus ++;
	}
	else
	{
		ss << gettext("It was a poor scene.");
	}
	ss << gettext("\n");


	//Enjoyed? If she's deranged, she'd should have enjoyed it.
	if (g_Girls.HasTrait(girl, "Mind Fucked"))
	{
		enjoy += 16;
		ss << gettext("Being completely mind fucked, ") << girlName << gettext(" really gets off on the depravity.\n");
	}
	else if (g_Girls.HasTrait(girl, "Masochist"))
	{
		enjoy += 13;
		ss << girlName << gettext(" enjoys this. She knows it's what she deserves.\n");
	}
	else if (g_Girls.HasTrait(girl, "Cum Addict"))
	{
		enjoy += 13;
		ss << girlName << gettext(" enjoys this, and spends a while licking cum off her body.\n");
	}
	else if (g_Girls.HasTrait(girl, "Broken Will") || g_Girls.HasTrait(girl, "Dependant"))
	{
		enjoy += 11;
		ss << girlName << gettext(" accepts this. It is Master's will.\n");
	}
	else if (g_Girls.HasTrait(girl, "Iron Will") || g_Girls.HasTrait(girl, "Fearless"))
	{
		enjoy -= 5;
		ss << girlName << gettext(" endures in stoic silence, determined not to let you see her suffer.\n");
	}
	
	//For final calc
	bonus = bonus + enjoy;

	//Now we've got that done, most girls shouldn't like this...
	enjoy -= 10;

	// remaining modifiers are in the AddScene function --PP
	int finalqual = g_Studios.AddScene(girl, JOB_FILMORAL, bonus);
	ss << "Her scene is valued at: " << finalqual << " gold.\n";

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_ORAL, Day0Night1);

	// work out the pay between the house and the girl
	if (girl->is_slave() && !cfg.initial.slave_pay_outofpocket())
	{
		wages = 0;	// You own her so you don't have to pay her.
	}
	else
	{
		wages += finalqual * 2;
	}
	girl->m_Pay = wages;

	// Improve stats
	int xp = 10, skill = 3, libido = 1;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_PERFORMANCE, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_BDSM, g_Dice%skill + 1);
	g_Girls.UpdateStat(girl, STAT_CONFIDENCE, -(g_Dice%skill + 1));
	g_Girls.UpdateStat(girl, STAT_SPIRIT, -(g_Dice%skill + 1));
	g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, libido);

	g_Girls.UpdateEnjoyment(girl, ACTION_WORKMOVIE, enjoy);
	g_Girls.UpdateStat(girl, STAT_PCLOVE, enjoy);
	g_Girls.UpdateStat(girl, STAT_PCFEAR, enjoy);
	g_Girls.UpdateStat(girl, STAT_PCHATE, -enjoy);


	g_Girls.PossiblyGainNewTrait(girl, "Masochist", 75, ACTION_SEX, girlName + " has turned into a Masochist from all the degradation.", Day0Night1);
	g_Girls.PossiblyGainNewTrait(girl, "Cum Addict", 80, ACTION_WORKMOVIE, girlName + " has become addicted to cum.", Day0Night1);
	if (tied)
	{
		g_Girls.PossiblyGainNewTrait(girl, "Mind Fucked", 75, ACTION_SEX, girlName + " has become Mind Fucked from the forced degradation.", Day0Night1);
	}
	//lose
	if (tied) g_Girls.PossiblyLoseExistingTrait(girl, "Iron Will", 80, ACTION_SEX, girlName + "'s unwilling degradation has shattered her iron will.", Day0Night1);


	//Evil job bonus-------------------------------------------------------
	//BONUS - evil jobs damage her body, break her spirit and make her hate you

	int MrEvil = g_Dice % 8, MrNasty = g_Dice % 8;
	MrEvil = (MrEvil + MrNasty) / 2;				//Should come out around 3 most of the time.

	g_Girls.UpdateStat(girl, STAT_CONFIDENCE, -MrEvil);
	g_Girls.UpdateStat(girl, STAT_SPIRIT, -MrEvil);
	g_Girls.UpdateStat(girl, STAT_DIGNITY, -MrEvil);
	g_Girls.UpdateStat(girl, STAT_PCLOVE, -MrEvil);
	g_Girls.UpdateStat(girl, STAT_PCHATE, MrEvil);
	g_Girls.UpdateStat(girl, STAT_PCFEAR, MrEvil);
	The_Player->disposition(-MrEvil);

	//----------------------------------------------------------------------

	return false;
}

double cJobManager::JP_FilmBuk(sGirl* girl, bool estimate)// used
{
	double jobperformance =
		(g_Girls.GetStat(girl, STAT_CHARISMA) + g_Girls.GetStat(girl, STAT_BEAUTY));

	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}

	//Good
	if (g_Girls.HasTrait(girl, "Mind Fucked"))					jobperformance += 100;	//Enjoyment and craziness
	if (g_Girls.HasTrait(girl, "Masochist"))					jobperformance += 35;	//
	if (g_Girls.HasTrait(girl, "Broken Will"))					jobperformance += 30;	//
	if (g_Girls.HasTrait(girl, "Dependant"))					jobperformance += 25;	//
	if (g_Girls.HasTrait(girl, "Twisted"))						jobperformance += 25;	//
	if (g_Girls.HasTrait(girl, "Cum Addict"))					jobperformance += 50;	//Her kind of job
	if (g_Girls.HasTrait(girl, "Goddess"))						jobperformance += 60;	//High-status degraded
	if (g_Girls.HasTrait(girl, "Angel"))						jobperformance += 50;	//
	if (g_Girls.HasTrait(girl, "Queen"))						jobperformance += 50;	//
	if (g_Girls.HasTrait(girl, "Princess"))						jobperformance += 40;	//
	if (g_Girls.HasTrait(girl, "Noble"))						jobperformance += 15;	//	
	if (g_Girls.HasTrait(girl, "Idol"))							jobperformance += 25;	//
	if (g_Girls.HasTrait(girl, "Priestess"))					jobperformance += 25;	//	
	if (g_Girls.HasTrait(girl, "Heroine"))						jobperformance += 15;	//	
	if (g_Girls.HasTrait(girl, "Teacher"))						jobperformance += 15;	//	
	if (g_Girls.HasTrait(girl, "Tsundere"))						jobperformance += 30;	//beaten customers wanna see this!
	if (g_Girls.HasTrait(girl, "Yandere"))						jobperformance += 25;	//	
	
	//Bad
	if (g_Girls.HasTrait(girl, "Iron Will"))					jobperformance += 40;	//Try not to put on a show
	if (g_Girls.HasTrait(girl, "Fearless"))						jobperformance += 25;	//	
	

	return jobperformance;
}


void AndAction(stringstream *TheAction, string TheHo, bool TiedUp)
{
	int roll = g_Dice.d100();
	//Some consts for later
	int const STARTS = 6;
	int const STUDS = 6;
	int const ADJECTIVES = 6;
	int const ACTIONS = 6;
	int const OUTCOMES = 7;

	*TheAction << TheHo << gettext(" was ");
	switch (roll%STARTS)
	{
	case 0:
		*TheAction << gettext("dutifully gangbanged");
		break;
	case 1:
		*TheAction << gettext("deeply probed");
		break;
	case 2:
		*TheAction << gettext("effectively raped");
		break;
	case 3:
		*TheAction << gettext("uncomfortably filled");
		break;
	case 4:
		*TheAction << gettext("clumsily penetrated");
		break;
	case 5:
		*TheAction << gettext("roughly used");
		break;
	default:
		*TheAction << gettext("was mysteriously acted on");
		break;
	}

	*TheAction << gettext(" by ");

	switch (roll%STUDS)
	{
	case 0:
		*TheAction << gettext("40-year-old virgins");
		break;
	case 1:
		*TheAction << gettext("a large gang of men");
		break;
	case 2:
		*TheAction << gettext("unenthusiastic gay men");
		break;
	case 3:
		*TheAction << gettext("straight-jacketed psychopaths");
		break;
	case 4:
		*TheAction << gettext("real-life ogres");
		break;
	case 5:
		*TheAction << gettext("drunken soldiers");
		break;
	default:
		*TheAction << gettext("unknown creatures from the planet Bug");
		break;
	}

	*TheAction << gettext(" who ");

	switch (roll%ADJECTIVES)
	{
	case 0:
		*TheAction << gettext("vigorously");
		break;
	case 1:
		*TheAction << gettext("stoutly");
		break;
	case 2:
		*TheAction << gettext("casually");
		break;
	case 3:
		*TheAction << gettext("lugubriously");
		break;
	case 4:
		*TheAction << gettext("excitedly");
		break;
	case 5:
		*TheAction << gettext("deliberately");
		break;
	default:
		*TheAction << gettext("ludicrously");
		break;
	}

	*TheAction << gettext(" ");

	switch (roll%ACTIONS)
	{
	case 0:
		*TheAction << gettext("ejaculated their seed");
		break;
	case 1:
		*TheAction << gettext("splooged");
		break;
	case 2:
		*TheAction << gettext("fired hot cum");
		break;
	case 3:
		*TheAction << gettext("shot semen");
		break;
	case 4:
		*TheAction << gettext("nutted");
		break;
	case 5:
		*TheAction << gettext("came");
		break;
	default:
		*TheAction << gettext("spazzed");
		break;
	}

	if (TiedUp)
	{
		switch (roll%OUTCOMES)
		{
		case 0:
			*TheAction << gettext(" all over her tied, trembling body.");
			break;
		case 1:
			*TheAction << gettext(" into her pried-open mouth.");
			break;
		case 2:
			*TheAction << gettext(" on her face, breasts and inside her spread-eagle cunt.");
			break;
		case 3:
			*TheAction << gettext(" over her bound, naked body until she was nicely 'glazed'.");
			break;
		case 4:
			*TheAction << gettext(" in her tied down, squirming face.");
			break;
		case 5:
			*TheAction << gettext(" into her speculum-spread vagina.");
			break;
		case 6:
			*TheAction << gettext(" in her eyes and up her nose.");
			break;
		default:
			*TheAction << gettext(" somewhere.");
			break;
		}
	}
	else
	{
		switch (roll%OUTCOMES)
		{
		case 0:
			*TheAction << gettext(" all over her naked body.");
			break;
		case 1:
			*TheAction << gettext(" into her waiting mouth.");
			break;
		case 2:
			*TheAction << gettext(" simultaneously in her ass, mouth and cunt.");
			break;
		case 3:
			*TheAction << gettext(" over her until she was nicely 'glazed'.");
			break;
		case 4:
			*TheAction << gettext(" in her hair and on her face.");
			break;
		case 5:
			*TheAction << gettext(" over her stomach and breasts.");
			break;
		case 6:
			*TheAction << gettext(" all in her eyes and up her nose.");
			break;
		default:
			*TheAction << gettext(" somewhere.");
			break;
		}
	}
	*TheAction << gettext("\n\n");
}