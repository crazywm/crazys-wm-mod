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

// BSIN: Job Movie Studio - Actress - Teaser
bool cJobManager::WorkFilmTease(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKMOVIE;
	//BSIN - Leaving this check in for robustness, in case jobflow changed later
	if (g_Studios.GetNumGirlsOnJob(0, JOB_CAMERAMAGE, SHIFT_NIGHT) == 0 || g_Studios.GetNumGirlsOnJob(0, JOB_CRYSTALPURIFIER, SHIFT_NIGHT) == 0)
	{
		girl->m_Events.AddMessage("There was no crew to film the scene, so she took the day off.", IMGTYPE_PROFILE, EVENT_NOWORK);
		return false;
	}

	stringstream ss;
	string item;
	string girlName = girl->m_Realname;
	int wages = 40, tips = 0;
	int enjoy = 0;
	double jobperformance = JP_FilmTease(girl, false);
	int bonus = 0;

	g_Girls.UnequipCombat(girl);	// not for actress (yet)
	/**/ if (g_Girls.HasItem(girl, "Cow-Print Lingerie") != -1)		item = gettext("Cow-Print Lingerie"), bonus = 6;
	else if (g_Girls.HasItem(girl, "Designer Lingerie") != -1)		item = gettext("Designer Lingerie"), bonus = 10;
	else if (g_Girls.HasItem(girl, "Sequin Lingerie") != -1)		item = gettext("Sequin Lingerie"), bonus = 7;
	else if (g_Girls.HasItem(girl, "Sexy X-Mas Lingerie") != -1)	item = gettext("Sexy X-Mas Lingerie"), bonus = 7;
	else if (g_Girls.HasItem(girl, "Leather Lingerie") != -1)		item = gettext("Leather Lingerie"), bonus = 6;
	else if (g_Girls.HasItem(girl, "Leopard Lingerie") != -1)		item = gettext("Leopard Lingerie"), bonus = 5;
	else if (g_Girls.HasItem(girl, "Organic Lingerie") != -1)		item = gettext("Organic Lingerie"), bonus = 5;
	else if (g_Girls.HasItem(girl, "Sheer Lingerie") != -1)			item = gettext("Sheer Lingerie"), bonus = 5;
	else if (g_Girls.HasItem(girl, "Lace Lingerie") != -1)			item = gettext("Lace Lingerie"), bonus = 3;
	else if (g_Girls.HasItem(girl, "Silk Lingerie") != -1)			item = gettext("Silk Lingerie"), bonus = 2;
	else if (g_Girls.HasItem(girl, "Plain Lingerie") != -1)			item = gettext("Plain Lingerie"), bonus = 1;

	int roll = g_Dice.d100();
	ss << girlName;
	if (roll <= 10 && g_Girls.DisobeyCheck(girl, ACTION_WORKMOVIE, brothel))
	{
		ss << gettext(" refused to make a teaser clip today.\n");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else ss << gettext(" worked filming teaser scenes to arouse viewers.\n\n");

	if (bonus)
	{
		ss << gettext("To improve the scene, ") << girlName << gettext(" wore her ") << item << gettext(".\n");
		jobperformance += (2 * bonus);
	}

	if (jobperformance >= 350)
	{
		ss << gettext("She created an outstanding teaser scene. She's so stunning and flirtatious that it's somehow hotter that she never gets naked.");
		g_Girls.UpdateStat(girl, STAT_FAME, 4);
		g_Girls.UpdateStat(girl, STAT_CHARISMA, 2);
		bonus = 12;
	}
	else if (jobperformance >= 245)
	{
		ss << gettext("She created an amazing teaser scene. Somehow she makes the few little she has on look hotter than being naked.");
		g_Girls.UpdateStat(girl, STAT_FAME, 2);
		g_Girls.UpdateStat(girl, STAT_CHARISMA, 1);
		bonus = 6;
	}
	else if (jobperformance >= 185)
	{
		ss << gettext("She created a decent teaser scene. Not the best, but still arousing.");
		g_Girls.UpdateStat(girl, STAT_FAME, 1);
		g_Girls.UpdateStat(girl, STAT_CHARISMA, 1);
		bonus = 4;
	}
	else if (jobperformance >= 145)
	{
		ss << gettext("It wasn't a great teaser scene. By the end you wish she'd get naked and do something.");
		bonus = 2;
	}
	else if (jobperformance >= 100)
	{
		ss << gettext("It was a bad teaser scene. Could someone just fuck her already?");
		bonus = 1;
		ss << gettext("\nThe Studio Director advised ") << girlName << gettext(" how to spice up the scene");
		if (g_Dice.percent(40))
		{
			ss << gettext(" and improve her performance. The scene definitely got better after this.");
			bonus++;
		}
		else
		{
			ss << gettext(", but she wouldn't listen.");
		}
	}
	else
	{
		ss << gettext("That was nothing 'teasing' about this awkward, charmless performance. Even the CameraMage seemed to lose interest.");
		if (g_Brothels.GetNumGirlsOnJob(0, JOB_DIRECTOR, Day0Night1) > 0)
		{
			ss << gettext("\nThe Studio Director stepped in and took control ");
			if (g_Dice.percent(50))
			{
				ss << gettext("significantly improving ") << girlName << gettext("'s performance in the scene.");
				bonus++;
			}
			else
			{
				ss << gettext("but wasn't able to save this trainwreck of a scene.");
			}
		}
	}
	ss << gettext("\n");

	//Enjoyed? If she performed well, she'd should have enjoyed it.
	if (jobperformance >= 200)
	{
		enjoy += (g_Dice % 3 + 1);
		ss << gettext("She loved flirting with the camera today.\n\n");
	}
	else if (jobperformance >= 100)
	{
		enjoy += g_Dice % 2;
		ss << gettext("She enjoyed this performance.\n\n");
	}
	else
	{
		enjoy -= (g_Dice % 3 + 2);
		ss << gettext("She didn't really get what she was supposed to do, and did not enjoy making this scene.\n\n");
	}
	bonus = bonus + enjoy;


	int finalqual = g_Studios.AddScene(girl, JOB_FILMTEASE, bonus);
	ss << "Her scene is valued at: " << finalqual << " gold.\n";

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_ECCHI, Day0Night1);

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
	int xp = 10, skill = 3, libido = 1;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_PERFORMANCE, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_STRIP, g_Dice%skill + 1);
	g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, libido);

	g_Girls.UpdateEnjoyment(girl, ACTION_WORKSTRIP, enjoy);
	g_Girls.UpdateEnjoyment(girl, ACTION_WORKMOVIE, enjoy);
	
	//gain traits
	g_Girls.PossiblyGainNewTrait(girl, "Charming", 80, ACTION_WORKMOVIE, "Flirting and seducing on film has made her Charming.", Day0Night1);
	g_Girls.PossiblyGainNewTrait(girl, "Actress", 80, ACTION_WORKSTRIP, "All this flirting and teasing with a camera in a studio has improved her acting skills.", Day0Night1);
	if (jobperformance >= 140 && g_Dice.percent(25))
	{
		g_Girls.PossiblyGainNewTrait(girl, "Sexy Air", 80, ACTION_WORKSTRIP, girlName + " has been having to be sexy for so long she now reeks sexiness.", Day0Night1);
	}

	// nice job bonus-------------------------------------------------------
	//BONUS - Nice jobs show off her assets and make her happy, increasing fame and love.

	int MrNiceGuy = g_Dice % 6, MrFair = g_Dice % 6;
	MrNiceGuy = (MrNiceGuy + MrFair) / 3;				//Should come out around 1-2 most of the time.

	g_Girls.UpdateStat(girl, STAT_HAPPINESS, MrNiceGuy);
	g_Girls.UpdateStat(girl, STAT_FAME, MrNiceGuy);
	g_Girls.UpdateStat(girl, STAT_PCLOVE, MrNiceGuy);
	g_Girls.UpdateStat(girl, STAT_PCHATE, -MrNiceGuy);
	g_Girls.UpdateStat(girl, STAT_PCFEAR, -MrNiceGuy);
	The_Player->disposition(MrNiceGuy);

	//----------------------------------------------------------------------

	return false;
}

double cJobManager::JP_FilmTease(sGirl* girl, bool estimate)
{
	double jobperformance =
		((g_Girls.GetStat(girl, STAT_CHARISMA) + g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_CONFIDENCE)) / 3
		+ (g_Girls.GetSkill(girl, SKILL_PERFORMANCE) + g_Girls.GetSkill(girl, SKILL_STRIP) / 2));

	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}


	//good traits
	if (g_Girls.HasTrait(girl, "Charismatic"))		jobperformance += 25;	//can tease
	if (g_Girls.HasTrait(girl, "Sexy Air"))			jobperformance += 25;	//can tease
	if (g_Girls.HasTrait(girl, "Porn Star"))		jobperformance += 25;	//plays to camera well
	if (g_Girls.HasTrait(girl, "Cool Person"))		jobperformance += 10;	//people love to be around her
	if (g_Girls.HasTrait(girl, "Charming"))			jobperformance += 10;	//
	if (g_Girls.HasTrait(girl, "Social Drinker"))	jobperformance += 5;	//relaxed
	if (g_Girls.HasTrait(girl, "Exhibitionist"))	jobperformance += 30;	//Loves showing off body
	if (g_Girls.HasTrait(girl, "Cute"))				jobperformance += 15;	//Attractive to some
	if (g_Girls.HasTrait(girl, "Lolita"))			jobperformance += 10;	//
	if (g_Girls.HasTrait(girl, "Elegant"))			jobperformance += 15;	//
	if (g_Girls.HasTrait(girl, "Exotic"))			jobperformance += 10;	//
	if (g_Girls.HasTrait(girl, "Beauty Mark"))		jobperformance += 5;	//
	if (g_Girls.HasTrait(girl, "Great Figure"))		jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Great Arse"))		jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Agile"))			jobperformance += 15;	//moves well
	
		
	//bad traits
	if (g_Girls.HasTrait(girl, "Nervous"))					jobperformance -= 30;	//weakens performance
	if (g_Girls.HasTrait(girl, "Meek"))						jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Shy"))						jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Broken Will"))				jobperformance -= 80;	//too messed up
	if (g_Girls.HasTrait(girl, "Mind Fucked"))				jobperformance -= 80;	//too messed up
	if (g_Girls.HasTrait(girl, "Branded on the Ass"))		jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Branded on the Forehead"))	jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "One Arm"))					jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "One Foot"))					jobperformance -= 10;
	if (g_Girls.HasTrait(girl, "One Hand"))					jobperformance -= 10;
	if (g_Girls.HasTrait(girl, "One Leg"))					jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "No Arms"))					jobperformance -= 50;	//hard to strip
	if (g_Girls.HasTrait(girl, "No Feet"))					jobperformance -= 60;	//hard to strip or move
	if (g_Girls.HasTrait(girl, "No Hands"))					jobperformance -= 50;
	if (g_Girls.HasTrait(girl, "No Legs"))					jobperformance -= 75;
	if (g_Girls.HasTrait(girl, "Retarded"))					jobperformance -= 30;
	if (g_Girls.HasTrait(girl, "Flat Ass"))					jobperformance -= 5;
	if (g_Girls.HasTrait(girl, "Flat Chest"))				jobperformance -= 5;
	
	if (g_Girls.HasTrait(girl, "Alcoholic"))			jobperformance -= 5;
	if (g_Girls.HasTrait(girl, "Fairy Dust Addict"))	jobperformance -= 5;
	if (g_Girls.HasTrait(girl, "Shroud Addict"))		jobperformance -= 5;
	if (g_Girls.HasTrait(girl, "Viras Blood Addict"))	jobperformance -= 5;

	return jobperformance;
}