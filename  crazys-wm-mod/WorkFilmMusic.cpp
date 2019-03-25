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
bool cJobManager::WorkFilmMusic(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
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
	double jobperformance = JP_FilmMusic(girl, false);
	int bonus = 0;

	g_Girls.UnequipCombat(girl);	// not for actress (yet)

	//Refusal
	ss << girlName;
	int roll = g_Dice.d100();
	if (roll <= 10 && girl->disobey_check(ACTION_WORKMOVIE, brothel))
	{
		ss << " refused to shoot a music video scene today.\n";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else if (girl->tiredness() > 75)
	{
		ss << " was too exhausted to make a music video scene.\n \n";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else ss << " worked on a music video showcasing her singing and dancing talent.\n \n";


	//What's she best at?
	enum { PERFORMANCE, LOOKS };
	int TopSkill = (girl->performance() > (girl->beastiality() + girl->charisma() / 2)) ? PERFORMANCE : LOOKS;

	//What's she carrying?
	/**/ if (girl->has_item("Liquid Dress") != -1)			item = "Liquid Dress", bonus += 10;
	else if (girl->has_item("Dark Liquid Dress") != -1)		item = "Dark Liquid Dress", bonus += 10;
	else if (girl->has_item("Gemstone Dress") != -1)			item = "Gemstone Dress", bonus += 10;
	else if (girl->has_item("Hime Dress") != -1)				item = "Hime Dress", bonus += 12;
	else if (girl->has_item("Enchanted Dress") != -1)		item = "Enchanted Dress", bonus += 10;
	else if (girl->has_item("Leopard Lingerie") != -1)		item = "Leopard Lingerie", bonus += 10;
	else if (girl->has_item("Designer Lingerie") != -1)		item = "Designer Lingerie", bonus += 10;
	else if (girl->has_item("Sheer Lingerie") != -1)			item = "Sheer Lingerie", bonus += 6;
	else if (girl->has_item("Oiran Dress") != -1)			item = "Oiran Dress", bonus += 5;
	else if (girl->has_item("Silk Lingerie") != -1)			item = "Bunny Outfit", bonus += 3;
	else if (girl->has_item("Empress' New Clothes") != -1)	item = "Empress' New Clothes", bonus += 10;
	else if (girl->has_item("Faerie Gown") != -1)			item = "Faerie Gown", bonus += 10;
	else if (girl->has_item("Royal Gown") != -1)				item = "Royal Gown", bonus += 10;
	else if (girl->has_item("Classy Underwear") != -1)		item = "Classy Underwear", bonus += 10;

	if (bonus)
	{
		ss << "To improve the scene, " << girlName << " wore her " << item << ".\n";
	}
	jobperformance += (5 * bonus);

	ss << girlName;
	if (jobperformance >= 350)
	{
		ss << " created a legendary music video. ";
		if (TopSkill == PERFORMANCE) ss << "Her singing and dancing was outstanding, and she herself was truly breathtaking.";
		else ss << "She was stunning and gave a fantastic performance.";
		bonus = 12;
	}
	else if (jobperformance >= 245)
	{
		ss << " created a superb music video. ";
		if (TopSkill == PERFORMANCE) ss << "Her singing and dancing were top-notch and she looked amazing on camera.";
		else ss << "She was beautiful and she gave a very touching performance.";
		bonus = 6;
	}
	else if (jobperformance >= 185)
	{
		ss << " created a very good music video. ";
		if (TopSkill == PERFORMANCE) ss << "Her singing and dancing were excellent and she looked okay.";
		else ss << "She looked fantastic and gave a nice performance.";
		bonus = 4;
	}
	else if (jobperformance >= 145)
	{
		ss << " created an okay music video. ";
		if (TopSkill == PERFORMANCE) ss << "Her singing and dancing were decent.";
		else ss << "She looked pretty good.";
		bonus = 2;
	}
	else if (jobperformance >= 100)
	{
		ss << " created a weak music video. ";
		if (TopSkill == PERFORMANCE) ss << "Her singing and dancing were decent but she didn't look all that.";
		else ss << "She looked pretty good but the performance let her down.";
		bonus = 1;
		ss << "\nThe Studio Director advised " << girlName << " how to spice up the performance";
		if (g_Dice.percent(40))
		{
			ss << ". The scene definitely got better after this.";
			bonus++;
		}
		else
		{
			ss << ", but she wouldn't listen.";
		}
	}
	else if(jobperformance >= 50)
	{
		ss << " created a bad music video. ";
		if (TopSkill == PERFORMANCE) ss << "Her singing and dancing weren't great and she had zero charisma on camera.";
		else ss << "She was just about likeable on camera, but her performance was painful.";
	}
	else
	{
		ss << " created a terrible music video. You even considered playing it in the dungeon as a kind of torture. But no. That would be inhumane.";
		if (The_Player->disposition() < -20) ss << ".. Even for you.";
	}

	ss << "\n";

	//Enjoyed? If she performed well, she'd should have enjoyed it.
	if (jobperformance >= 200)
	{
		enjoy += (g_Dice % 3 + 1);
		ss << "She loved singing and performing today.\n \n";
	}
	else if (jobperformance >= 100)
	{
		enjoy += g_Dice % 2;
		ss << "She enjoyed this performance.\n \n";
	}
	else
	{
		enjoy -= (g_Dice % 3 + 2);
		ss << "She isn't much of a performer and did not enjoy making this scene.\n \n";
	}
	bonus = bonus + enjoy;


	int finalqual = g_Studios.AddScene(girl, JOB_FILMMUSIC, bonus);
	ss << "Her scene is valued at: " << finalqual << " gold.\n";

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_SING, Day0Night1);

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

	/* */if (girl->has_trait( "Quick Learner"))/*       */{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))/*        */{ skill -= 1; xp -= 3; }
	/* */if (girl->has_trait( "Nymphomaniac"))/*        */{ libido += 2; }

    girl->exp(xp);
	girl->performance(g_Dice%skill);
	girl->strip(g_Dice%skill + 1);
	girl->upd_temp_stat(STAT_LIBIDO, libido);

	girl->upd_Enjoyment(ACTION_WORKMUSIC, enjoy);
	girl->upd_Enjoyment(ACTION_WORKMOVIE, enjoy);

	//gain traits
	/* */if (jobperformance >= 100 && g_Dice.percent(25)) g_Girls.PossiblyGainNewTrait(girl, "Charming", 80, ACTION_WORKMOVIE, "Singing and dancing on film has made her more Charming.", Day0Night1);
	else if (jobperformance >= 140 && g_Dice.percent(25))
	{
		g_Girls.PossiblyGainNewTrait(girl, "Sexy Air", 80, ACTION_WORKSTRIP, girlName + " has been having to be sexy for so long she now reeks sexiness.", Day0Night1);
	}
	/* */if (girl->has_trait( "Singer"))
	{
		if (jobperformance >= 245 && g_Dice.percent(30))
		{
			g_Girls.PossiblyGainNewTrait(girl, "Idol", 80, ACTION_WORKMOVIE, "Her talented and charismatic performances have got a large number of fans Idolizing her.", Day0Night1);
		}
	}
	else if (jobperformance >= 200 && g_Dice.percent(30)) g_Girls.PossiblyGainNewTrait(girl, "Singer", 80, ACTION_WORKMOVIE, "Her singing has become quite excellent.", Day0Night1);

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

double cJobManager::JP_FilmMusic(sGirl* girl, bool estimate)
{
    double jobperformance =
        ((girl->charisma()+
          girl->beauty()+
          girl->confidence())/3+
          girl->performance());

	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}


	//good traits
	if (girl->has_trait( "Singer"))/*                   */jobperformance += 50;	//can Sing
	if (girl->has_trait( "Angel"))/*                    */jobperformance += 40;	//like a choir of angels
	if (girl->has_trait( "Sexy Air"))/*                 */jobperformance += 25;	// nice video
	if (girl->has_trait( "Agile"))/*                    */jobperformance += 20;	//
	if (girl->has_trait( "Cool Person"))/*              */jobperformance += 20;	//
	if (girl->has_trait( "Charismatic"))/*              */jobperformance += 20;	//
	if (girl->has_trait( "Charming"))/*                 */jobperformance += 10;	//
	if (girl->has_trait( "Audacity"))/*                 */jobperformance += 10;	//
	if (girl->has_trait( "Cute"))/*                     */jobperformance += 15;	//Attractive to some
	if (girl->has_trait( "Exotic"))/*                   */jobperformance += 15;	//
	if (girl->has_trait( "Idol"))/*                     */jobperformance += 15;	//
	if (girl->has_trait( "Lolita"))/*                   */jobperformance += 10;	//
	if (girl->has_trait( "Elegant"))/*                  */jobperformance += 10;	//
	if (girl->has_trait( "Great Figure"))/*             */jobperformance += 10;	//
	if (girl->has_trait( "Beauty Mark"))/*              */jobperformance += 5;	//
	if (girl->has_trait( "Porn Star"))/*                */jobperformance += 10;	//plays to camera well
	if (girl->has_trait( "Exhibitionist"))/*            */jobperformance += 10;	//
	if (girl->has_trait( "Social Drinker"))/*           */jobperformance += 5;	//relaxed

	//bad traits
	if (girl->has_trait( "Tone Deaf"))/*                */jobperformance -= 60;	//weakens performance
	if (girl->has_trait( "Mind Fucked"))/*              */jobperformance -= 40;	//too messed up
	if (girl->has_trait( "Nervous"))/*                  */jobperformance -= 25;	//weakens performance
	if (girl->has_trait( "Meek"))/*                     */jobperformance -= 25;
	if (girl->has_trait( "Shy"))/*                      */jobperformance -= 20;
	if (girl->has_trait( "Retarded"))/*                 */jobperformance -= 15;

	if (girl->has_trait( "Alcoholic"))/*                */jobperformance -= 5;
	if (girl->has_trait( "Fairy Dust Addict"))/*        */jobperformance -= 5;
	if (girl->has_trait( "Shroud Addict"))/*            */jobperformance -= 5;
	if (girl->has_trait( "Viras Blood Addict"))/*       */jobperformance -= 5;

	return jobperformance;
}
