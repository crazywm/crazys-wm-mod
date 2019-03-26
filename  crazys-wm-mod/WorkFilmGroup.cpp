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

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cMovieStudioManager g_Studios;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;

// `J` Job Movie Studio - Actress
bool cJobManager::WorkFilmGroup(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
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
	int enjoy = 0;
	int jobperformance = 0;
	int bonus = 0;

	int roll = g_Dice.d100();
	if (roll <= 10 && girl->disobey_check(ACTION_WORKMOVIE, brothel))
	{
		ss << girlName << " refused to do an orgy on film today.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}

	g_Girls.UnequipCombat(girl);	// not for actress (yet)

	ss << girlName;

	int guys = g_Dice.d100();
	if (guys < 50)
	{
		guys = 2;
		jobperformance += 5;
		ss << " worked as an actress filming a three-way.\n \n";
	}
	else if (guys < 95)
	{
		jobperformance += 10;
		guys = g_Dice.d100();
		/* */if (guys < 40)		guys = 3;
		else if (guys < 60)		guys = 4;
		else if (guys < 75)		guys = 5;
		else if (guys < 85)		guys = 6;
		else if (guys < 90)		guys = 7;
		else if (guys < 94)		guys = 8;
		else if (guys < 98)		guys = 9;
		else /*          */		guys = 10;
		ss << " worked in a gang-bang scene with " << guys << " other people.\n \n";
	}
	else if (!girl->has_trait("Lesbian") && girl->has_trait("Nymphomaniac") && girl->has_trait("Porn Star"))
	{
		jobperformance += 50;
		guys = g_Dice % 240 + 120;
		ss << " worked in a scene where she tried to set a new record by taking part in a " << guys << " men gangbang.\n \n";
	}
	else
	{
		jobperformance += 20;
		guys = g_Dice % 40 + 11;
		ss << " worked in a orgy scene with " << guys << " other people.\n \n";
	}

	girl->tiredness(guys - 2);	// Extra tiredness per guy

	if (roll <= 10)
	{
		enjoy -= ((guys > 10 ? (guys / 10) : (guys / 2)) + 1);
		ss << "She found it unpleasant fucking that many people.\n \n";
	}
	else if (roll >= 90 || (girl->has_trait("Nymphomaniac") && roll >= 50))
	{
		enjoy += (guys > 10 ? (guys / 10) : (guys / 2)) + 1;
		ss << "She loved getting so much action, and wants more!\n \n";
	}
	else
	{
		enjoy += g_Dice % 2;
		ss << "She wasn't really into having so much sex today, but managed to get through.\n \n";
	}
	jobperformance = enjoy * 2;

	if (girl->has_trait("Lolita") && g_Dice.percent(50))
	{
		ss << "With such a young looking girl on the cover, this movie is simply sure to sell out.\n \n";
		jobperformance += 15;
	}
	if (girl->has_trait("Bimbo") && g_Dice.percent(50))
	{
		ss << "Her performance couldn't be considered acting. She just enjoyed a good dicking with a blissfully stupid smile on her face.\n \n";
		jobperformance += 5;
	}
	if (girl->has_trait("Cute") && g_Dice.percent(50))
	{
		ss << "Such a cute girl doing her best to handle the rough ponding her co-stars gave her will surely make a few hearts skip a beat.\n \n";
		jobperformance += 5;
	}
	if (girl->has_trait("Natural Pheromones") && g_Dice.percent(50))
	{
		ss << "Her scent drove the actors mad with lust. They went at it harder and wilder than the director had ordered.\n \n";
		jobperformance += 5;
	}
	if (girl->has_trait("Fast Orgasms") && g_Dice.percent(50))
	{
		ss << "She came the second she got two dicks inside her. That was the first of many orgasms. She barely remembered her own name by the end of the scene.\n \n";
		jobperformance += 5;
	}
	if (girl->has_trait("Great Arse") && g_Dice.percent(50))
	{
		ss << "Her ass jiggling with each thrust was a sight to behold. The camera mage made sure to capture its wiggly glory in slow motion.\n \n";
		jobperformance += 5;
	}
	if (girl->has_trait("Plump") && g_Dice.percent(50))
	{
		ss << "She sure offered a lot of meat to work with. The wet and fleshy sound she made with each thrust is sure to drive the audience wild.\n \n";
		jobperformance += 5;
	}
	if ((girl->has_trait("Fragile") || girl->has_trait("Delicate")) && g_Dice.percent(50))
	{
		ss << "This was somewhat rough for her, she was barely able to move by the end of the scene. Some people find that hot though.\n \n";
		jobperformance += 5;
		enjoy -= g_Dice % 5 - 2;
	}
	if (girl->breast_size() > 5 && g_Dice.percent(50))
	{
		ss << "Her gigantic breasts stole the spotlight towards the end of the scene when she pushed them together to collect her co-star's seed.\n \n";
		jobperformance += 5;
	}
	if (girl->has_trait("Cum Addict") && g_Dice.percent(50))
	{
		ss << "She insisted that her co-stars collect their semen in a cup for her to drink at the end of the scene.\n \n";
		jobperformance += 5;
	}
	if (girl->has_trait("Succubus") && g_Dice.percent(50))
	{
		ss << "After the camera stopped running, she grabed two actors and had another round with them.\n \n";
		jobperformance += 5;
	}

	if (girl->check_virginity())
	{
		girl->lose_virginity();	// `J` updated for trait/status
		jobperformance += 50;
		ss << "She is no longer a virgin.\n";
	}

	sCustomer Cust = g_Customers.GetCustomer(*brothel);
	Cust.m_Amount = guys;
	if (!girl->calc_group_pregnancy(Cust, false, 1.0))
	{
		g_MessageQue.AddToQue(girl->m_Realname + " has gotten pregnant", 0);
	}
	
	bonus = (int)(jobperformance / 10);
	// remaining modifiers are in the AddScene function --PP
	int finalqual = g_Studios.AddScene(girl, JOB_FILMGROUP, bonus);
	ss << "Her scene is valued at: " << finalqual << " gold.\n";

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_GROUP, Day0Night1);

	// work out the pay between the house and the girl
	if (girl->is_slave() && !cfg.initial.slave_pay_outofpocket())
	{
		wages = 0;	// You own her so you don't have to pay her.
	}
	else
	{
		wages += (guys * 10);	// Extra pay per guy
		wages += finalqual * 2;
	}
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);


	// Improve stats
	int xp = (((guys > 10 ? (guys / 10) : (guys / 2)) + 1) * 5);
	int skill = ((guys > 10 ? (guys / 10) : (guys / 2)) + 1);

	if (girl->has_trait("Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait("Slow Learner"))	{ skill -= 1; xp -= 3; }

	girl->exp(xp);
	girl->performance(g_Dice%skill);
	girl->group(g_Dice%skill + 1);

	girl->upd_Enjoyment(ACTION_SEX, enjoy);
	girl->upd_Enjoyment(ACTION_WORKMOVIE, enjoy);
	//gain
	g_Girls.PossiblyGainNewTrait(girl, "Fake Orgasm Expert", 50, ACTION_SEX, "She has become quite the faker.", Day0Night1);
	g_Girls.PossiblyGainNewTrait(girl, "Slut", 80, ACTION_SEX, girlName + " has turned into quite a slut.", Day0Night1, EVENT_WARNING);
	g_Girls.PossiblyGainNewTrait(girl, "Porn Star", 80, ACTION_WORKMOVIE, "She has performed in enough sex scenes that she has become a well known Porn Star.", Day0Night1);
	//lose

	return false;
}

double cJobManager::JP_FilmGroup(sGirl* girl, bool estimate)// not used
{
	return 0;
}