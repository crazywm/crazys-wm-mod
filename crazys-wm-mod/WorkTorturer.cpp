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
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;

// `J` Job Brothel - General
bool cJobManager::WorkTorturer(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKTORTURER;
	if (Day0Night1) return false;		// Do this only once a day
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	if (girl->disobey_check(actiontype, brothel))
	{
		ss << " refused to torture anyone.";
		girl->morality(1);
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " is assigned to torture people in the dungeon.";
	girl->morality(-1);
	int wages = 0, tips = 0;

	g_Girls.EquipCombat(girl);	// ready armor and weapons!

	// Complications
	//SIN: bit more variety for the above
	int roll(g_Dice % 5);
	bool forFree = false;
	if (g_Dice.percent(10))
	{
		girl->upd_Enjoyment(actiontype, -3);
		if (girl->has_trait( "Sadistic") || girl->has_trait( "Merciless") || girl->morality() < 30)
			ss << girlName << (" hurt herself while torturing someone.\n");
		else
		{
			switch (roll)
			{
			case 0:		ss << girlName << (" felt bad torturing people as she could easily see herself in the victim.\n"); break;
			case 1:		ss << girlName << (" doesn't like this as she feels it is wrong to torture people.\n"); break;
			case 2:		ss << girlName << (" feels like a bitch after one of her torture victims wept the entire time and kept begging her to stop.\n"); break;
			case 3:		ss << girlName << (" feels awful after accidentally whipping someone in an excruciating place.\n"); break;
			case 4:		ss << girlName << (" didn't enjoy this as she felt sorry for the victim.\n"); break;
			default:	ss << girlName << (" didn't enjoy this for some illogical reason. [error]\n"); break; //shouldn't happen
			}
			//And a little randomness
			if (g_Dice.percent(40))
			{
				roll = g_Dice % 3;
				switch (roll)
				{
				case 0:
					ss << ("She hates you for making her do this today.\n");
					girl->pclove(-(g_Dice % 2));
					girl->pchate(g_Dice % 2);
					break;
				case 1:
					ss << girlName << (" is terrified that you treat people like this.\n");
					girl->pcfear(g_Dice % 6);
					girl->obedience(g_Dice % 2);
					break;
				case 2:
					ss << ("She learned a bit about medicine while trying to stop the pain.\n");
					girl->medicine(g_Dice % 10);
					break;
				default:
					ss << girlName << (" did something completely unexpected. [error]");
					break;
				}
			}
		}
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_DOM, Day0Night1);
	}
	else
	{
		girl->upd_Enjoyment(actiontype, +3);
		switch (roll)
		{
		case 0:		ss << girlName << (" enjoyed her job working in the dungeon.\n"); break;
		case 1:		ss << girlName << (" is turned on by the power of torturing people.\n"); break;
		case 2:		ss << girlName << (" enjoyed trying out different torture devices and watching the effects on the victim.\n"); break;
		case 3:		ss << girlName << (" spent her time in the dungeon whipping her victim in time to music to make amusing sound effects.\n"); break;
		case 4:		ss << girlName << (" uses the victim's cries and screams to to figure out the 'best' areas to torture.\n"); break;
		default:	ss << girlName << (" enjoyed this for some illogical reason. [error]\n"); break;
		}

		//And a little randomness
		if ((girl->morality() < 20 || girl->has_trait( "Sadistic")) && g_Dice.percent(20))
		{
			ss << girlName << (" loved this so much she wouldn't accept any money, as long as you promise she can do it again soon.\n");
			girl->upd_Enjoyment(actiontype, +3);
			forFree = true;
		}
		if (g_Dice.percent(20))
		{
			roll = g_Dice % 4;
			switch (roll)
			{
			case 0:
				ss << girlName << (" put so much energy into this it seems to have improved her fitness.\n");
				girl->constitution(g_Dice % 3);
				break;
			case 1:
				ss << girlName << (" went way too far, creating a hell of a mess. Still it looks like she had fun - she hasn't stopped smiling.\n");
				girl->happiness(g_Dice % 5);
				girl->upd_Enjoyment(actiontype, +1);
				brothel->m_Filthiness += 15;
				break;
			case 2:
				ss << girlName << (" over-exerted herself.");
				girl->health(-(g_Dice % 5));
				girl->tiredness(g_Dice % 5);
				break;
			case 3:
				ss << girlName << (" appreciates that you entrust her with this kind of work.");
				girl->pclove(g_Dice % 2);
				girl->pchate(-(g_Dice % 2));
				break;
			default:
				ss << girlName << (" did something completely unexpected. [error]");
				break;
			}
		}
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);
	}

	// Improve girl
	int xp = 15, libido = 5, skill = 1;

	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }

	if (!forFree)
	{
		wages += 65;
		//g_Gold.staff_wages(65);  // wages come from you
	}
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);

	girl->exp(xp);
	girl->morality(-2);
	girl->bdsm(skill);
	girl->upd_temp_stat(STAT_LIBIDO, libido);

	// WD: Update flag
	g_Brothels.TortureDone(true);

	// Check for new traits
	g_Girls.PossiblyGainNewTrait(girl, "Sadistic", 30, actiontype, girl->m_Realname + (" has come to enjoy her job so much that she has become rather Sadistic."), Day0Night1);
	g_Girls.PossiblyGainNewTrait(girl, "Merciless", 50, actiontype, girl->m_Realname + (" extensive experience with torture has made her absolutely Merciless."), Day0Night1);

	return false;
}

double cJobManager::JP_Torturer(sGirl* girl, bool estimate)		// not used
{
#if 1	//SIN - this is a special case.
	//AFAIK the torturer ID/skills not used at all in the job processing (apart from names in strings)
	//Who does the currently has ZERO affect on outcome.
	//So this stat just shows how much THIS girl (i.e. the torturer) will 'enjoy' job.
	//standardized per J's instructs

	double jobperformance =
		//main stat - how evil?
		(100 - girl->morality()) +
		//secondary stats - obedience, effectiveness and understanding of anatomy
		((girl->obedience() + girl->combat() + girl->strength() + girl->medicine()) / 4) +
		//add level
		girl->level();

	//and finally traits
	//"good"
	if (girl->has_trait( "Sadistic"))					jobperformance += 30;	//how do you like... THIS!
	if (girl->has_trait( "Powerful Magic"))			    jobperformance += 25;	//magical flame
	if (girl->has_trait( "Strong Magic"))				jobperformance += 20;	//magical flame
	if (girl->has_trait( "Dominatrix"))				    jobperformance += 20;	//you will learn to obey me
	if (girl->has_trait( "Merciless"))				    jobperformance += 20;	//"Stop"? <shrug> I don't know that word.
	if (girl->has_trait( "Demon"))					    jobperformance += 20;	//satan taught me this move
	if (girl->has_trait( "Aggressive"))				    jobperformance += 15;	//WHAT did you say?
	if (girl->has_trait( "Assassin"))					jobperformance += 15;	//skills
	if (girl->has_trait( "Doctor"))					    jobperformance += 15;	//they call me doctor pain...
	if (girl->has_trait( "Iron Will"))				    jobperformance += 15;	//I *WILL* break you
	if (girl->has_trait( "Alchoholic"))				    jobperformance += 10;	//she's a mean drunk
	if (girl->has_trait( "Twisted"))					jobperformance += 10;	//twisted biatch
	if (girl->has_trait( "Broken Will"))				jobperformance += 5;	//just following orders
	if (girl->has_trait( "Mind Fucked"))				jobperformance += 5;	//let's play together

	//either
	if (girl->has_trait( "Psychic"))												//I feel your pain... such suffering...
	{
		if (girl->has_trait( "Masochist"))			    jobperformance += 30;	//... [smiles] and I like it!
		else											jobperformance -= 30;
	}

	//"bad"
	if (girl->has_trait( "Goddess"))					jobperformance -= 50;	//Wouldn't harm a soul
	if (girl->has_trait( "Angel"))					    jobperformance -= 40;	//Wouldn't harm a soul
	if (girl->has_trait( "Battery Operated"))			jobperformance -= 20;	//"What is this 'pain' you feel?"
	if (girl->has_trait( "Construct"))				    jobperformance -= 20;	//"What is this 'pain' you feel?"
	if (girl->has_trait( "Clumsy"))					    jobperformance += 20;	//OW! I just whipped myself.

#else
	double jobperformance = 0.0;
	if (estimate)	// for third detail string
	{

	}
	else			// for the actual security check
	{

	}
#endif
	return jobperformance;
}
