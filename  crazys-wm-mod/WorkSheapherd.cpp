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

#if 0
#include "cJobManager.h"
#include "cRng.h"
#include "CLog.h"
#include "cMessageBox.h"
#include "cGold.h"
#include "cBrothel.h"
#include "cFarm.h"


extern CLog g_LogFile;
extern cMessageQue g_MessageQue;
extern cRng g_Dice;
extern cGold g_Gold;
extern cBrothelManager g_Brothels;
extern cFarmManager g_Farm;

// `J` Job Farm - Laborers
bool cJobManager::WorkSheapherd(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKFARM;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	if (girl->disobey_check(actiontype, brothel))
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked as a sheapherd in the farm.\n\n";

	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	int wages = 15, work = 0;

	double jobperformance = JP_Sheapherd(girl, false);
	if (jobperformance >= 245)
	{
		ss << " She must be the perfect at this.";
		wages += 155;
	}
	else if (jobperformance >= 185)
	{
		ss << " She's unbelievable at this.";
		wages += 95;
	}
	else if (jobperformance >= 145)
	{
		ss << " She's good at this job.";
		wages += 55;
	}
	else if (jobperformance >= 100)
	{
		ss << " She made a few mistakes but overall she is okay at this.";
		wages += 15;
	}
	else if (jobperformance >= 70)
	{
		ss << " She was nervous and made a few mistakes. She isn't that good at this.";
		wages -= 5;
	}
	else
	{
		ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.";
		wages -= 15;
	}
	ss << "\n\n";


	//enjoyed the work or not
	int roll = g_Dice.d100();
	if (roll <= 5)
	{
		ss << "\nSome of the patrons abused her during the shift.";
		work -= 1;
	}
	else if (roll <= 25)
	{
		ss << "\nShe had a pleasant time working.";
		work += 3;
	}
	else
	{
		ss << "\nOtherwise, the shift passed uneventfully.";
		work += 1;
	}


#if 0


	// `J` Farm Bookmark - adding in items that can be created in the farm














#endif


	girl->upd_Enjoyment(actiontype, work);
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_HERD, Day0Night1);


	int roll_max = (girl->beauty() + girl->charisma());
	roll_max /= 4;
	wages += 10 + g_Dice%roll_max;
	if (wages < 0) wages = 0;
	girl->m_Pay = wages;


	// Improve stats
	int xp = 5, libido = 1, skill = 3;

	if (girl->has_trait("Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait("Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait("Nymphomaniac"))			{ libido += 2; }

	girl->exp((g_Dice%xp) + 1);
	girl->upd_temp_stat(STAT_LIBIDO, libido);
	
	// primary (+2 for single or +1 for multiple)
	girl->animalhandling((g_Dice % skill) + 2);
	// secondary (-1 for one then -2 for others)
	girl->charisma(max(0, (g_Dice % skill) - 1));
	girl->intelligence(max(0, (g_Dice % skill) - 2));
	girl->confidence(max(0, (g_Dice % skill) - 2));

	return false;
}

double cJobManager::JP_Sheapherd(sGirl* girl, bool estimate)// not used
{
	double jobperformance =
		// primary - first 100
		girl->animalhandling() +
		// secondary - second 100
		((girl->charisma() + girl->intelligence() + girl->confidence()) / 3) +
		// level bonus
		girl->level();


	//good traits
	if (girl->has_trait("Quick Learner"))	jobperformance += 5;
	if (girl->has_trait("Psychic"))			jobperformance += 10;
	if (girl->has_trait("Farmers Daughter"))	jobperformance += 30;
	if (girl->has_trait("Country Gal"))		jobperformance += 10;

	//bad traits
	if (girl->has_trait("Dependant"))		jobperformance -= 50;	//needs others to do the job
	if (girl->has_trait("Clumsy"))			jobperformance -= 20;	//spills food and breaks things often
	if (girl->has_trait("Aggressive"))		jobperformance -= 20;	//gets mad easy
	if (girl->has_trait("Nervous"))			jobperformance -= 30;	//don't like to be around people
	if (girl->has_trait("Meek"))				jobperformance -= 20;

	if (girl->has_trait("One Arm"))		jobperformance -= 40;
	if (girl->has_trait("One Foot"))		jobperformance -= 40;
	if (girl->has_trait("One Hand"))		jobperformance -= 30;
	if (girl->has_trait("One Leg"))		jobperformance -= 60;
	if (girl->has_trait("No Arms"))		jobperformance -= 125;
	if (girl->has_trait("No Feet"))		jobperformance -= 60;
	if (girl->has_trait("No Hands"))		jobperformance -= 50;
	if (girl->has_trait("No Legs"))		jobperformance -= 150;
	if (girl->has_trait("Blind"))		jobperformance -= 30;
	if (girl->has_trait("Deaf"))			jobperformance -= 15;
	if (girl->has_trait("Retarded"))		jobperformance -= 60;
	if (girl->has_trait("Smoker"))		jobperformance -= 10;	//would need smoke breaks

	if (girl->has_trait("Alcoholic"))			jobperformance -= 25;
	if (girl->has_trait("Fairy Dust Addict"))	jobperformance -= 25;
	if (girl->has_trait("Shroud Addict"))		jobperformance -= 25;
	if (girl->has_trait("Viras Blood Addict"))	jobperformance -= 25;


	return jobperformance;
}
#endif

