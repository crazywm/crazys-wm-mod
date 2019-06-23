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
	
	/**/ if (girl->has_item("Cow-Print Lingerie") != -1)		item = ("Cow-Print Lingerie"), bonus = 6;
	else if (girl->has_item("Designer Lingerie") != -1)		item = ("Designer Lingerie"), bonus = 10;
	else if (girl->has_item("Sequin Lingerie") != -1)		item = ("Sequin Lingerie"), bonus = 7;
	else if (girl->has_item("Sexy X-Mas Lingerie") != -1)	item = ("Sexy X-Mas Lingerie"), bonus = 7;
	else if (girl->has_item("Leather Lingerie") != -1)		item = ("Leather Lingerie"), bonus = 6;
	else if (girl->has_item("Leopard Lingerie") != -1)		item = ("Leopard Lingerie"), bonus = 5;
	else if (girl->has_item("Organic Lingerie") != -1)		item = ("Organic Lingerie"), bonus = 5;
	else if (girl->has_item("Sheer Lingerie") != -1)			item = ("Sheer Lingerie"), bonus = 5;
	else if (girl->has_item("Lace Lingerie") != -1)			item = ("Lace Lingerie"), bonus = 3;
	else if (girl->has_item("Silk Lingerie") != -1)			item = ("Silk Lingerie"), bonus = 2;
	else if (girl->has_item("Plain Lingerie") != -1)			item = ("Plain Lingerie"), bonus = 1;

	int roll = g_Dice.d100();
	ss << girlName;
	if (roll <= 10 && girl->disobey_check(ACTION_WORKMOVIE, brothel))
	{
		ss << (" refused to make a teaser clip today.\n");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else ss << " worked filming teaser scenes to arouse viewers.\n \n";

	if (bonus)
	{
		ss << "To improve the scene, " << girlName << " wore her " << item << ".\n";
		jobperformance += (2 * bonus);
	}

	if (jobperformance >= 350)
	{
		ss << ("She created an outstanding teaser scene. She's so stunning and flirtatious that it's somehow hotter that she never gets naked.");
		girl->fame(4);
		girl->charisma(2);
		bonus = 12;
	}
	else if (jobperformance >= 245)
	{
		ss << ("She created an amazing teaser scene. Somehow she makes the few little she has on look hotter than being naked.");
		girl->fame(2);
		girl->charisma(1);
		bonus = 6;
	}
	else if (jobperformance >= 185)
	{
		ss << ("She created a decent teaser scene. Not the best, but still arousing.");
		girl->fame(1);
		girl->charisma(1);
		bonus = 4;
	}
	else if (jobperformance >= 145)
	{
		ss << ("It wasn't a great teaser scene. By the end you wish she'd get naked and do something.");
		bonus = 2;
	}
	else if (jobperformance >= 100)
	{
		ss << ("It was a bad teaser scene. Could someone just fuck her already?");
		bonus = 1;
		ss << ("\nThe Studio Director advised ") << girlName << (" how to spice up the scene");
		if (g_Dice.percent(40))
		{
			ss << (" and improve her performance. The scene definitely got better after this.");
			bonus++;
		}
		else
		{
			ss << (", but she wouldn't listen.");
		}
	}
	else
	{
		ss << ("That was nothing 'teasing' about this awkward, charmless performance. Even the CameraMage seemed to lose interest.");
		if (g_Brothels.GetNumGirlsOnJob(0, JOB_DIRECTOR, Day0Night1) > 0)
		{
			ss << ("\nThe Studio Director stepped in and took control ");
			if (g_Dice.percent(50))
			{
				ss << ("significantly improving ") << girlName << ("'s performance in the scene.");
				bonus++;
			}
			else
			{
				ss << ("but wasn't able to save this trainwreck of a scene.");
			}
		}
	}
	ss << ("\n");

	//Enjoyed? If she performed well, she'd should have enjoyed it.
	if (jobperformance >= 200)
	{
		enjoy += (g_Dice % 3 + 1);
		ss << "She loved flirting with the camera today.\n \n";
	}
	else if (jobperformance >= 100)
	{
		enjoy += g_Dice % 2;
		ss << "She enjoyed this performance.\n \n";
	}
	else
	{
		enjoy -= (g_Dice % 3 + 2);
		ss << "She didn't really get what she was supposed to do, and did not enjoy making this scene.\n \n";
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

	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }

	girl->exp(xp);
	girl->performance(g_Dice%skill);
	girl->strip(g_Dice%skill + 1);
	girl->upd_temp_stat(STAT_LIBIDO, libido);

	girl->upd_Enjoyment(ACTION_WORKSTRIP, enjoy);
	girl->upd_Enjoyment(ACTION_WORKMOVIE, enjoy);

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

	girl->happiness(MrNiceGuy);
	girl->fame(MrNiceGuy);
	girl->pclove(MrNiceGuy);
	girl->pchate(-MrNiceGuy);
	girl->pcfear(-MrNiceGuy);
	The_Player->disposition(MrNiceGuy);

	//----------------------------------------------------------------------

	return false;
}

double cJobManager::JP_FilmTease(sGirl* girl, bool estimate)
{
	double jobperformance =
		((girl->charisma() + girl->beauty() + girl->confidence()) / 3
		+ (girl->performance() + girl->strip() / 2));

	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}


	//good traits
	if (girl->has_trait( "Charismatic"))		jobperformance += 25;	//can tease
	if (girl->has_trait( "Sexy Air"))			jobperformance += 25;	//can tease
	if (girl->has_trait( "Porn Star"))		jobperformance += 25;	//plays to camera well
	if (girl->has_trait( "Cool Person"))		jobperformance += 10;	//people love to be around her
	if (girl->has_trait( "Charming"))			jobperformance += 10;	//
	if (girl->has_trait( "Social Drinker"))	jobperformance += 5;	//relaxed
	if (girl->has_trait( "Exhibitionist"))	jobperformance += 30;	//Loves showing off body
	if (girl->has_trait( "Cute"))				jobperformance += 15;	//Attractive to some
	if (girl->has_trait( "Lolita"))			jobperformance += 10;	//
	if (girl->has_trait( "Elegant"))			jobperformance += 15;	//
	if (girl->has_trait( "Exotic"))			jobperformance += 10;	//
	if (girl->has_trait( "Beauty Mark"))		jobperformance += 5;	//
	if (girl->has_trait( "Great Figure"))		jobperformance += 10;
	if (girl->has_trait( "Great Arse"))		jobperformance += 10;
	if (girl->has_trait( "Agile"))			jobperformance += 15;	//moves well


	//bad traits
	if (girl->has_trait( "Nervous"))					jobperformance -= 30;	//weakens performance
	if (girl->has_trait( "Meek"))						jobperformance -= 20;
	if (girl->has_trait( "Shy"))						jobperformance -= 20;
	if (girl->has_trait( "Broken Will"))				jobperformance -= 80;	//too messed up
	if (girl->has_trait( "Mind Fucked"))				jobperformance -= 80;	//too messed up
	if (girl->has_trait( "Branded on the Ass"))		jobperformance -= 20;
	if (girl->has_trait( "Branded on the Forehead"))	jobperformance -= 20;
	if (girl->has_trait( "One Arm"))					jobperformance -= 20;
	if (girl->has_trait( "One Foot"))					jobperformance -= 10;
	if (girl->has_trait( "One Hand"))					jobperformance -= 10;
	if (girl->has_trait( "One Leg"))					jobperformance -= 20;
	if (girl->has_trait( "No Arms"))					jobperformance -= 50;	//hard to strip
	if (girl->has_trait( "No Feet"))					jobperformance -= 60;	//hard to strip or move
	if (girl->has_trait( "No Hands"))					jobperformance -= 50;
	if (girl->has_trait( "No Legs"))					jobperformance -= 75;
	if (girl->has_trait( "Retarded"))					jobperformance -= 30;
	if (girl->has_trait( "Flat Ass"))					jobperformance -= 5;
	if (girl->has_trait( "Flat Chest"))				jobperformance -= 5;

	if (girl->has_trait( "Alcoholic"))			jobperformance -= 5;
	if (girl->has_trait( "Fairy Dust Addict"))	jobperformance -= 5;
	if (girl->has_trait( "Shroud Addict"))		jobperformance -= 5;
	if (girl->has_trait( "Viras Blood Addict"))	jobperformance -= 5;

	return jobperformance;
}
