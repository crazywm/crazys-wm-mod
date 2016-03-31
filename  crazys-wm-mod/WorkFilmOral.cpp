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
	int wages = 50;
	int enjoy = 0, bonus = 0;
	double jobperformance = JP_FilmOral(girl, false);

	/*
	//BSIN - not sure what's wrong but this crashes the game.
	sGang * TempGang = g_Gangs.GetGangOnMission(MISS_GUARDING);
	string gangName = "";
	int inGang = 0, members = 0;
	if (TempGang && (TempGang->m_Num > 0))
	{
		gangName = TempGang->m_Name;
		inGang = TempGang->m_Num;
		members = ((g_Dice%inGang) + 1);
	}
	delete TempGang;
	*/

	g_Girls.UnequipCombat(girl);	// not for actress (yet)

	ss << girlName << " worked as an actress filming oral scenes.\n\n";

	int roll = g_Dice.d100();
	if (roll <= 10 && g_Girls.DisobeyCheck(girl, ACTION_WORKMOVIE, brothel))
	{
		ss << "She refused to give head on film today.\n";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	
	if (jobperformance >= 350)
	{
	/*	as above - crashes game
		if (g_Dice.percent(50) && TempGang)
		{
			ss << gettext("When the 'actor' didn't turn up, ") << girlName << gettext(" expertly sucked off ");
			if (members > 1) ss << members << gettext(" lucky guys");
			else ss << gettext("one lucky guy");
			ss << gettext(" from your gang ") << gangName << gettext(".");
		}
		else */ss << gettext("The lucky guy came uncontrollably, damn near passing out in pleasure as ") << girlName << gettext(" expertly sucked him dry.");
		ss << gettext("\nShe savoured the cum in her mouth, rolling it around her lips and tongue before finally swallowing it down.");
		bonus = 12;
	}
	else if (jobperformance >= 245)
	{
		ss << girlName << gettext(" sucked off her man like a pro - not once breaking eye-contact - and was rewarded with ");
		if (g_Dice.percent(50) || g_Girls.HasTrait(girl,"Cum Addict")) ss << gettext("a mouthful of semen. She kept her lips clamped to his cock to the last, thirstily swallowing down every drop of hot cum.");
		else ss << gettext("a explosion of cum in her face. As she licked his penis clean, she rubbed cum around her skin and licked it off her fingers.");
		bonus = 6;
	}
	else if (jobperformance >= 185)
	{
		ss << girlName << gettext(" gave a passable blowjob, and ended up glazed in hot cum.");
		bonus = 4;
	}
	else if (jobperformance >= 145)
	{
		ss << gettext(" gave a fairly pleasant blowjob, but in the end the actor had to finish himself off, splatting cum on her face.");
		bonus = 2;
	}
	else if (jobperformance >= 100)
	{
		ss << gettext("It was a pretty awkward and uncomfortable scene, with the actor not getting any kind of pleasure from her clumsy, toothy attempts. ");
		ss << gettext("In the end he gave up and simply wanked in her face, but even then she dodged at the last moment, ruining that scene too.");
		bonus = 1;
	}
	else
	{
		ss << gettext("After the fourth time she 'snagged' the actor on her tooth, he cursed and stormed off set. Your gang, ") //<< gangName
			<< gettext(" saved the day by pinning her down wanking on her face one-by-one.\nOverall it's a terrible scene.");
	}
	ss << gettext("\n");

	//Enjoyed? If she performed well, she'd should have enjoyed it.
	if (jobperformance >= 200)
	{
		enjoy += (g_Dice % 3 + 1);
		ss << gettext("She really enjoyed giving head today.\n\n");
	}
	else if (jobperformance >= 100)
	{
		enjoy += g_Dice % 2;
		ss << gettext("She enjoyed this performance.\n\n");
	}
	else
	{
		enjoy -= (g_Dice % 3 + 2);
		ss << gettext("She's bad at this, and the whole experience was pretty humiliating.\n\n");
	}
	bonus = bonus + enjoy;


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
	g_Girls.UpdateSkill(girl, SKILL_ORALSEX, g_Dice%skill + 1);
	g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, libido);

	g_Girls.UpdateEnjoyment(girl, ACTION_SEX, enjoy);
	g_Girls.UpdateEnjoyment(girl, ACTION_WORKMOVIE, enjoy);
	g_Girls.PossiblyGainNewTrait(girl, "Porn Star", 80, ACTION_WORKMOVIE, "She has performed in enough sex scenes that she has become a well known Porn Star.", Day0Night1);
	if (g_Dice.percent(5) && (g_Girls.GetStat(girl, STAT_HAPPINESS) > 80) && (g_Girls.GetEnjoyment(girl, ACTION_WORKMOVIE) > 75))
		g_Girls.AdjustTraitGroupGagReflex(girl, 1, true, Day0Night1);
	return false;
}

double cJobManager::JP_FilmOral(sGirl* girl, bool estimate)// not used
{
	double jobperformance =
		(((g_Girls.GetStat(girl, STAT_CHARISMA) + g_Girls.GetStat(girl, STAT_BEAUTY)) / 2)
		+ g_Girls.GetSkill(girl, SKILL_ORALSEX));
	
	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}
	
	//Good
	if (g_Girls.HasTrait(girl, "Deep Throat"))					jobperformance += 60;	//better ability
	if (g_Girls.HasTrait(girl, "No Gag Reflex"))				jobperformance += 40;	//
	if (g_Girls.HasTrait(girl, "Good Kisser"))					jobperformance += 20;	//
	if (g_Girls.HasTrait(girl, "Nimble Tongue"))				jobperformance += 20;	//
	if (g_Girls.HasTrait(girl, "Pierced Tongue"))				jobperformance += 25;	//
	if (g_Girls.HasTrait(girl, "Dick-Sucking Lips"))			jobperformance += 35;	//
	if (g_Girls.HasTrait(girl, "Exotic"))						jobperformance += 5;	//
	if (g_Girls.HasTrait(girl, "Whore"))						jobperformance += 20;	//Knows how to work it
	if (g_Girls.HasTrait(girl, "Porn Star"))					jobperformance += 30;	//Knows how to work it on film
	if (g_Girls.HasTrait(girl, "No Teeth"))						jobperformance += 15;	//No chance of 'accident'
	if (g_Girls.HasTrait(girl, "Missing Teeth"))				jobperformance += 15;	//
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))					jobperformance += 25;	//had lots of practice
	if (g_Girls.HasTrait(girl, "Slut"))							jobperformance += 15;	//had practice
	if (g_Girls.HasTrait(girl, "Cum Addict"))					jobperformance += 30;	//eager to please
	if (g_Girls.HasTrait(girl, "Mind Fucked"))					jobperformance += 25;	//eager to please
	if (g_Girls.HasTrait(girl, "Sexy Air"))						jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Open Minded"))					jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Natural Pheromones"))			jobperformance += 5;


	//Bad
	if (g_Girls.HasTrait(girl, "Strong Gag Reflex"))			jobperformance -= 40;
	if (g_Girls.HasTrait(girl, "Gag Reflex"))					jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Clumsy"))						jobperformance -= 25;
	if (g_Girls.HasTrait(girl, "Nervous"))						jobperformance -= 15;

	return jobperformance;
}