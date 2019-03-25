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

// BSIN: Job Movie Studio - Actress - Chef
bool cJobManager::WorkFilmChef(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
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
	double jobperformance = JP_FilmChef(girl, false);
	int bonus = 0;

	ss << girlName;
	int roll = g_Dice.d100();
	if (roll <= 10 && girl->disobey_check(ACTION_WORKMOVIE, brothel))
	{
		ss << " refused to make a kitchen show today.\n";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else ss << " worked filming sexy chef scenes to educate and entertain viewers.\n \n";

	//What's she best at?
	enum { COOKING, LOOKS };
	int TopSkill = (girl->cooking() > ((girl->beauty() + girl->charisma()) / 2)) ? COOKING : LOOKS;

	ss << girlName;
	if (jobperformance >= 350)
	{
		ss << " created an outstanding show. She's so competent and flirtatious that the show is a hit with everyone.";
		bonus = 12;
		girl->fame(3);
	}
	else if (jobperformance >= 245)
	{
		ss << " created a great show. Women are impressed with her competence. Men are impressed with her... competence. It doesn't hurt that she's hot and flirtatious.";
		bonus = 6;
		girl->fame(2);
	}
	else if (jobperformance >= 185)
	{
		ss << " created a good show. ";
		if (TopSkill == COOKING) ss << "Her skills in the kitchen are enough to carry the show.";
		else ss << "She's sexy enough that her fumbles in the kitchen go mostly unnoticed.";
		bonus = 4;
		girl->fame(1);
	}
	else if (jobperformance >= 145)
	{
		/*                      */ss << " created an okay cooking show. ";
		if (TopSkill == COOKING)  ss << "She was skilled enough in the kitchen to mostly entertain.";
		else/*                 */ ss << "She's cute, so it doesn't really matter that she can't cook.";
		bonus = 2;
	}
	else if (jobperformance >= 100)
	{
		/*                      */ss << " created a bad cooking show. ";
		if (TopSkill == COOKING)  ss << "The little skill she had in the kitchen wasn't nearly enough to save it.";
		else /*                 */ss << "Perhaps if she'd quit breaking all the utensils and had instead taken her clothes off it'd would've been worth watching.\nPerhaps.";
		bonus = 1;
	}
	else
	{
		ss << " created a really bad cooking show. ";
		ss << "She had no charisma and burned or broke basically everything she touched. It was almost funny.\nAlmost.";
	}
	ss << "\n";

	//Enjoyed? If she performed well, she'd should have enjoyed it.
	if (jobperformance >= 200)
	{
		enjoy += (g_Dice % 3 + 1);
		ss << "She loved cooking and flirting on camera.\n \n";
	}
	else if (jobperformance >= 100)
	{
		enjoy += g_Dice % 2;
		ss << "She enjoyed making this show.\n \n";
	}
	else
	{
		enjoy -= (g_Dice % 3 + 2);
		ss << "She was awful at this and did not enjoy making it.\n \n";
	}
	bonus = bonus + enjoy;


	int finalqual = g_Studios.AddScene(girl, JOB_FILMCHEF, bonus);
	ss << "Her scene is valued at: " << finalqual << " gold.\n";

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_COOK, Day0Night1);

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
	girl->cooking(g_Dice%skill + 1);

	girl->upd_Enjoyment(ACTION_WORKCOOKING, enjoy);
	girl->upd_Enjoyment(ACTION_WORKMOVIE, enjoy);

	//gain traits
	g_Girls.PossiblyGainNewTrait(girl, "Charming", 80, ACTION_WORKMOVIE, "She has become quite Charming.", Day0Night1);
	g_Girls.PossiblyGainNewTrait(girl, "Actress", 80, ACTION_WORKMOVIE, "Working on camera has improved her acting skills.", Day0Night1);
	if (jobperformance >= 220 && g_Dice.percent(30))
	{
		g_Girls.PossiblyGainNewTrait(girl, "Chef", 80, ACTION_WORKCOOKING, girlName + " has become a competent Chef.", Day0Night1);
	}
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

double cJobManager::JP_FilmChef(sGirl* girl, bool estimate)
{
	double jobperformance =
		((girl->charisma() + girl->beauty() + girl->confidence()) / 3
		+ girl->cooking());

	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}


	//good traits
	if (girl->has_trait( "Chef"))				jobperformance += 40;	//Good at cooking
	if (girl->has_trait( "Mixologist"))		jobperformance += 20;	//Good with measures
	if (girl->has_trait( "Charismatic"))		jobperformance += 25;	//Charisma on film
	if (girl->has_trait( "Sexy Air"))			jobperformance += 25;	//
	if (girl->has_trait( "Charming"))			jobperformance += 20;	//
	if (girl->has_trait( "Cool Person"))		jobperformance += 10;	//
	if (girl->has_trait( "Social Drinker"))	jobperformance += 5;	//Drinks too
	if (girl->has_trait( "Exhibitionist"))	jobperformance += 30;	//Shows off body
	if (girl->has_trait( "MILF"))				jobperformance += 15;	//Attractive to some
	if (girl->has_trait( "Cute"))				jobperformance += 10;	//
	if (girl->has_trait( "Lolita"))			jobperformance += 10;	//
	if (girl->has_trait( "Elegant"))			jobperformance += 10;	//
	if (girl->has_trait( "Exotic"))			jobperformance += 5;	//
	if (girl->has_trait( "Beauty Mark"))		jobperformance += 5;	//
	if (girl->has_trait( "Fat"))				jobperformance += 10;	//Likes food
	if (girl->has_trait( "Plump"))			jobperformance += 5;	//
	if (girl->has_trait( "Wide Bottom"))		jobperformance += 5;	//
	if (girl->has_trait( "Plump Tush"))		jobperformance += 5;	//


	//bad traits
	if (girl->has_trait( "Nervous"))					jobperformance -= 30;	//weakens performance
	if (girl->has_trait( "Meek"))						jobperformance -= 20;
	if (girl->has_trait( "Shy"))						jobperformance -= 20;
	if (girl->has_trait( "One Arm"))					jobperformance -= 20;
	if (girl->has_trait( "One Hand"))					jobperformance -= 10;
	if (girl->has_trait( "No Arms"))					jobperformance -= 50;	//hard to strip
	if (girl->has_trait( "No Hands"))					jobperformance -= 50;
	if (girl->has_trait( "Alcoholic"))				jobperformance -= 5;

	return jobperformance;
}
