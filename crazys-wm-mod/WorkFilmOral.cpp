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

// `J` Job Movie Studio - Actress
bool cJobManager::WorkFilmOral(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
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
	int wages = 50, tips = 0;
	int enjoy = 0, bonus = 0;
	double jobperformance = JP_FilmOral(girl, false);

	//BSIN - not sure what's wrong but this crashes the game.
	sGang* Gang = g_Gangs.GetGangOnMission(MISS_GUARDING);//g_Gangs.GetGangOnMission(MISS_GUARDING);

	g_Girls.UnequipCombat(girl);	// not for actress (yet)

	ss << girlName << " worked as an actress filming oral scenes.\n \n";

	int roll = g_Dice.d100();
	if (roll <= 10 && girl->disobey_check(ACTION_WORKMOVIE, brothel))
	{
		ss << "She refused to give head on film today.\n";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}

	if (jobperformance >= 350)
	{
		if (Gang && Gang->m_Num > 0 && g_Dice.percent(50))
		{
			int members = min(g_Dice.bell(0, Gang->m_Num * 2), Gang->m_Num);
			ss << "When the 'actor' didn't turn up, " << girlName << " expertly sucked off ";
			if (members >= Gang->m_Num)	ss << "everyone";
			else if (members > 1)/*  */	ss << members << " lucky guys";
			else/*                   */	ss << "one lucky guy";
			ss << " from your gang " << Gang->m_Name << ".";
		}
		else ss << "The lucky guy came uncontrollably, damn near passing out in pleasure as " << girlName 
			<< " expertly sucked him dry.\nShe savoured the cum in her mouth, rolling it around her lips and tongue before finally swallowing it down.";
		bonus = 12;
	}
	else if (jobperformance >= 245)
	{
		ss << girlName << " sucked off her man like a pro - not once breaking eye-contact - and was rewarded with ";
		if (g_Dice.percent(50) || girl->has_trait("Cum Addict")) ss << "a mouthful of semen. She kept her lips clamped to his cock to the last, thirstily swallowing down every drop of hot cum.";
		else ss << "a explosion of cum in her face. As she licked his penis clean, she rubbed cum around her skin and licked it off her fingers.";
		bonus = 6;
	}
	else if (jobperformance >= 185)
	{
		ss << girlName << " gave a fairly pleasant blowjob, and ended up glazed in hot cum.";
		bonus = 4;
	}
	else if (jobperformance >= 145)
	{
		ss << girlName << " gave a passable blowjob, but in the end the actor had to finish himself off, splatting cum on her face.";
		bonus = 2;
	}
	else if (jobperformance >= 100)
	{
		ss << "It was a pretty awkward and uncomfortable scene, with the actor not getting any kind of pleasure from her clumsy, toothy attempts. ";
		ss << "In the end he gave up and simply wanked in her face, but even then she dodged at the last moment, ruining that scene too.";
		bonus = 1;
	}
	else
	{
		ss << "After the fourth time she 'snagged' the actor on her tooth, he cursed and stormed off set. Your gang, " << (Gang ? Gang->m_Name : "") << ", saved the day by pinning her down wanking on her face one-by-one.\nOverall it's a terrible scene.";
	}
	ss << "\n";

	//Enjoyed? If she performed well, she'd should have enjoyed it.
	if (girl->has_trait("Cum Addict")) enjoy += 2;
	if (jobperformance >= 200)
	{
		enjoy += (g_Dice % 3 + 1);
		ss << "She really enjoyed giving head today.\n \n";
	}
	else if (jobperformance >= 100)
	{
		enjoy += g_Dice % 2;
		ss << "She enjoyed this performance.\n \n";
	}
	else
	{
		enjoy -= (g_Dice % 3 + 2);
		ss << "She's bad at this, and the whole experience was pretty humiliating.\n \n";
	}
	bonus += enjoy;

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
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);

	// Improve stats
	int xp = 10, skill = 3, libido = 1;

	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }

	girl->exp(xp);
	girl->performance(g_Dice%skill);
	girl->oralsex(g_Dice%skill + 1);
	girl->upd_temp_stat(STAT_LIBIDO, libido);

	girl->upd_Enjoyment(ACTION_SEX, enjoy);
	girl->upd_Enjoyment(ACTION_WORKMOVIE, enjoy);
	g_Girls.PossiblyGainNewTrait(girl, "Porn Star", 80, ACTION_WORKMOVIE, "She has performed in enough sex scenes that she has become a well known Porn Star.", Day0Night1);
	if (g_Dice.percent(5) && (girl->happiness() > 80) && (girl->get_enjoyment(ACTION_WORKMOVIE) > 75))
		g_Girls.AdjustTraitGroupGagReflex(girl, 1, true, Day0Night1);
	return false;
}

double cJobManager::JP_FilmOral(sGirl* girl, bool estimate)// not used
{
	double jobperformance =
		(((girl->charisma() + girl->beauty()) / 2)
		+ girl->oralsex());

	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}

	//Good
	if (girl->has_trait( "Deep Throat"))					jobperformance += 60;	//better ability
	if (girl->has_trait( "No Gag Reflex"))				jobperformance += 40;	//
	if (girl->has_trait( "Good Kisser"))					jobperformance += 20;	//
	if (girl->has_trait( "Nimble Tongue"))				jobperformance += 20;	//
	if (girl->has_trait( "Pierced Tongue"))				jobperformance += 25;	//
	if (girl->has_trait( "Dick-Sucking Lips"))			jobperformance += 35;	//
	if (girl->has_trait( "Exotic"))						jobperformance += 5;	//
	if (girl->has_trait( "Whore"))						jobperformance += 20;	//Knows how to work it
	if (girl->has_trait( "Porn Star"))					jobperformance += 30;	//Knows how to work it on film
	if (girl->has_trait( "No Teeth"))						jobperformance += 15;	// no chance of 'accident'
	if (girl->has_trait( "Missing Teeth"))				jobperformance += 15;	//
	if (girl->has_trait( "Nymphomaniac"))					jobperformance += 25;	//had lots of practice
	if (girl->has_trait( "Slut"))							jobperformance += 15;	//had practice
	if (girl->has_trait( "Cum Addict"))					jobperformance += 30;	//eager to please
	if (girl->has_trait( "Mind Fucked"))					jobperformance += 25;	//eager to please
	if (girl->has_trait( "Sexy Air"))						jobperformance += 10;
	if (girl->has_trait( "Open Minded"))					jobperformance += 10;
	if (girl->has_trait( "Natural Pheromones"))			jobperformance += 5;


	//Bad
	if (girl->has_trait( "Strong Gag Reflex"))			jobperformance -= 40;
	if (girl->has_trait( "Gag Reflex"))					jobperformance -= 20;
	if (girl->has_trait( "Clumsy"))						jobperformance -= 25;
	if (girl->has_trait( "Nervous"))						jobperformance -= 15;

	return jobperformance;
}
