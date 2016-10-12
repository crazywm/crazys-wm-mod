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
	if (roll <= 10 && g_Girls.DisobeyCheck(girl, ACTION_WORKMOVIE, brothel))
	{
		ss << gettext(" refused to shoot a music video scene today.\n");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else if (g_Girls.GetStat(girl, STAT_TIREDNESS) > 75)
	{
		ss << gettext(" was too exhausted to make a music video scene.\n\n");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else ss << gettext(" worked on a music video showcasing her singing and dancing talent.\n\n");


	//What's she best at?
	enum { PERFORMANCE, LOOKS };
	int TopSkill = (g_Girls.GetSkill(girl, SKILL_PERFORMANCE) > ((g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_CHARISMA)) / 2)) ? PERFORMANCE : LOOKS;

	//What's she carrying?
	/**/ if (g_Girls.HasItem(girl, "Liquid Dress") != -1)			item = gettext("Liquid Dress"), bonus += 10;
	else if (g_Girls.HasItem(girl, "Dark Liquid Dress") != -1)		item = gettext("Dark Liquid Dress"), bonus += 10;
	else if (g_Girls.HasItem(girl, "Gemstone Dress") != -1)			item = gettext("Gemstone Dress"), bonus += 10;
	else if (g_Girls.HasItem(girl, "Hime Dress") != -1)				item = gettext("Hime Dress"), bonus += 12;
	else if (g_Girls.HasItem(girl, "Enchanted Dress") != -1)		item = gettext("Enchanted Dress"), bonus += 10;
	else if (g_Girls.HasItem(girl, "Leopard Lingerie") != -1)		item = gettext("Leopard Lingerie"), bonus += 10;
	else if (g_Girls.HasItem(girl, "Designer Lingerie") != -1)		item = gettext("Designer Lingerie"), bonus += 10;
	else if (g_Girls.HasItem(girl, "Sheer Lingerie") != -1)			item = gettext("Sheer Lingerie"), bonus += 6;
	else if (g_Girls.HasItem(girl, "Oiran Dress") != -1)			item = gettext("Oiran Dress"), bonus += 5;
	else if (g_Girls.HasItem(girl, "Silk Lingerie") != -1)			item = gettext("Bunny Outfit"), bonus += 3;
	else if (g_Girls.HasItem(girl, "Empress' New Clothes") != -1)	item = gettext("Empress' New Clothes"), bonus += 10;
	else if (g_Girls.HasItem(girl, "Faerie Gown") != -1)			item = gettext("Faerie Gown"), bonus += 10;
	else if (g_Girls.HasItem(girl, "Royal Gown") != -1)				item = gettext("Royal Gown"), bonus += 10;
	else if (g_Girls.HasItem(girl, "Classy Underwear") != -1)		item = gettext("Classy Underwear"), bonus += 10;

	if (bonus)
	{
		ss << gettext("To improve the scene, ") << girlName << gettext(" wore her ") << item << gettext(".\n");
	}
	jobperformance += (5 * bonus);

	ss << girlName;
	if (jobperformance >= 350)
	{
		ss << gettext(" created a legendary music video. ");
		if (TopSkill == PERFORMANCE) ss << gettext("Her singing and dancing was outstanding, and she herself was truly breathtaking.");
		else ss << gettext("She was stunning and gave a fantastic performance.");
		bonus = 12;
	}
	else if (jobperformance >= 245)
	{
		ss << gettext(" created a superb music video. ");
		if (TopSkill == PERFORMANCE) ss << gettext("Her singing and dancing were top-notch and she looked amazing on camera.");
		else ss << gettext("She was beautiful and she gave a very touching performance.");
		bonus = 6;
	}
	else if (jobperformance >= 185)
	{
		ss << gettext(" created a very good music video. ");
		if (TopSkill == PERFORMANCE) ss << gettext("Her singing and dancing were excellent and she looked okay.");
		else ss << gettext("She looked fantastic and gave a nice performance.");
		bonus = 4;
	}
	else if (jobperformance >= 145)
	{
		ss << gettext(" created an okay music video. ");
		if (TopSkill == PERFORMANCE) ss << gettext("Her singing and dancing were decent.");
		else ss << gettext("She looked pretty good.");
		bonus = 2;
	}
	else if (jobperformance >= 100)
	{
		ss << gettext(" created a weak music video. ");
		if (TopSkill == PERFORMANCE) ss << gettext("Her singing and dancing were decent but she didn't look all that.");
		else ss << gettext("She looked pretty good but the performance let her down.");
		bonus = 1;
		ss << gettext("\nThe Studio Director advised ") << girlName << gettext(" how to spice up the performance");
		if (g_Dice.percent(40))
		{
			ss << gettext(". The scene definitely got better after this.");
			bonus++;
		}
		else
		{
			ss << gettext(", but she wouldn't listen.");
		}
	}
	else if(jobperformance >= 50)
	{
		ss << gettext(" created a bad music video. ");
		if (TopSkill == PERFORMANCE) ss << gettext("Her singing and dancing weren't great and she had zero charisma on camera.");
		else ss << gettext("She was just about likeable on camera, but her performance was painful.");
	}
	else
	{
		ss << gettext(" created a terrible music video. You even considered playing it in the dungeon as a kind of torture. But no. That would be inhumane.");
		if (The_Player->disposition() < -20) ss << gettext(".. Even for you.");
	}

	ss << gettext("\n");

	//Enjoyed? If she performed well, she'd should have enjoyed it.
	if (jobperformance >= 200)
	{
		enjoy += (g_Dice % 3 + 1);
		ss << gettext("She loved singing and performing today.\n\n");
	}
	else if (jobperformance >= 100)
	{
		enjoy += g_Dice % 2;
		ss << gettext("She enjoyed this performance.\n\n");
	}
	else
	{
		enjoy -= (g_Dice % 3 + 2);
		ss << gettext("She isn't much of a performer and did not enjoy making this scene.\n\n");
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

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_PERFORMANCE, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_STRIP, g_Dice%skill + 1);
	g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, libido);

	g_Girls.UpdateEnjoyment(girl, ACTION_WORKMUSIC, enjoy);
	g_Girls.UpdateEnjoyment(girl, ACTION_WORKMOVIE, enjoy);
	
	//gain traits
	if (jobperformance >= 100 && g_Dice.percent(25)) g_Girls.PossiblyGainNewTrait(girl, "Charming", 80, ACTION_WORKMOVIE, "Singing and dancing on film has made her more Charming.", Day0Night1);
	else if (jobperformance >= 140 && g_Dice.percent(25))
	{
		g_Girls.PossiblyGainNewTrait(girl, "Sexy Air", 80, ACTION_WORKSTRIP, girlName + " has been having to be sexy for so long she now reeks sexiness.", Day0Night1);
	}
	if (g_Girls.HasTrait(girl, "Singer"))
	{
		if (jobperformance >= 245 && g_Dice.percent(30))
		{
			g_Girls.PossiblyGainNewTrait(girl, "Idol", 80, ACTION_WORKMOVIE, "Her talented and charismatic performances have got a large number of fans Idolizing her.", Day0Night1);
		}
	}
	else if (jobperformance >= 200 && g_Dice.percent(30)) g_Girls.PossiblyGainNewTrait(girl, "Singer", 80, ACTION_WORKMOVIE, "Her singing has become quite excellent.", Day0Night1);

	//Nice job bonus-------------------------------------------------------
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

double cJobManager::JP_FilmMusic(sGirl* girl, bool estimate)
{
	double jobperformance =
		((g_Girls.GetStat(girl, STAT_CHARISMA) + g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_CONFIDENCE)) / 3
		+ (g_Girls.GetSkill(girl, SKILL_PERFORMANCE)));

	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}


	//good traits
	if (g_Girls.HasTrait(girl, "Singer"))			jobperformance += 50;	//can Sing
	if (g_Girls.HasTrait(girl, "Angel"))			jobperformance += 40;	//like a choir of angels
	if (g_Girls.HasTrait(girl, "Sexy Air"))			jobperformance += 25;	//nice video
	if (g_Girls.HasTrait(girl, "Agile"))			jobperformance += 20;	//
	if (g_Girls.HasTrait(girl, "Cool Person"))		jobperformance += 20;	//
	if (g_Girls.HasTrait(girl, "Charismatic"))		jobperformance += 20;	//
	if (g_Girls.HasTrait(girl, "Charming"))			jobperformance += 10;	//
	if (g_Girls.HasTrait(girl, "Audacity"))			jobperformance += 10;	//
	if (g_Girls.HasTrait(girl, "Cute"))				jobperformance += 15;	//Attractive to some
	if (g_Girls.HasTrait(girl, "Exotic"))			jobperformance += 15;	//
	if (g_Girls.HasTrait(girl, "Idol"))				jobperformance += 15;	//
	if (g_Girls.HasTrait(girl, "Lolita"))			jobperformance += 10;	//
	if (g_Girls.HasTrait(girl, "Elegant"))			jobperformance += 10;	//
	if (g_Girls.HasTrait(girl, "Great Figure"))		jobperformance += 10;	//
	if (g_Girls.HasTrait(girl, "Beauty Mark"))		jobperformance += 5;	//
	if (g_Girls.HasTrait(girl, "Porn Star"))		jobperformance += 10;	//plays to camera well
	if (g_Girls.HasTrait(girl, "Exhibitionist"))	jobperformance += 10;	//
	if (g_Girls.HasTrait(girl, "Social Drinker"))	jobperformance += 5;	//relaxed
	
	//bad traits
	if (g_Girls.HasTrait(girl, "Tone Deaf"))				jobperformance -= 60;	//weakens performance
	if (g_Girls.HasTrait(girl, "Mind Fucked"))				jobperformance -= 40;	//too messed up
	if (g_Girls.HasTrait(girl, "Nervous"))					jobperformance -= 25;	//weakens performance
	if (g_Girls.HasTrait(girl, "Meek"))						jobperformance -= 25;
	if (g_Girls.HasTrait(girl, "Shy"))						jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Retarded"))					jobperformance -= 15;

	if (g_Girls.HasTrait(girl, "Alcoholic"))			jobperformance -= 5;
	if (g_Girls.HasTrait(girl, "Fairy Dust Addict"))	jobperformance -= 5;
	if (g_Girls.HasTrait(girl, "Shroud Addict"))		jobperformance -= 5;
	if (g_Girls.HasTrait(girl, "Viras Blood Addict"))	jobperformance -= 5;

	return jobperformance;
}