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
extern cInventory g_InvManager;

// `J` Job Farm - Producers
bool cJobManager::WorkButcher(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKCOOKING;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	if (g_Girls.DisobeyCheck(girl, actiontype, brothel))			// they refuse to work 
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked as a butcher on the farm.\n\n";

	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	int enjoy = 0;
	int wages = 25;
	int tips = 0;
	int imagetype = IMGTYPE_COOK;
	int msgtype = Day0Night1;


	double jobperformance = JP_Butcher(girl, false);
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

	int roll = g_Dice.d100();
#if 1
	//enjoyed the work or not
	if (roll <= 5)
	{
		ss << "\nSome of the patrons abused her during the shift.";
		enjoy -= 1;
	}
	else if (roll <= 25)
	{
		ss << "\nShe had a pleasant time working.";
		enjoy += 3;
	}
	else
	{
		ss << "\nOtherwise, the shift passed uneventfully.";
		enjoy += 1;
	}
#else
	if (roll_a <= 10)
	{
		enjoyC -= g_Dice % 3; enjoyF -= g_Dice % 3;
		CleanAmt = int(CleanAmt * 0.8);
		/* */if (roll_b < 30)	ss << "She spilled a bucket of something unpleasant all over herself.";
		else if (roll_b < 60)	ss << "She stepped in something unpleasant.";
		else /*            */	ss << "She did not like working on the farm today.";
	}
	else if (roll_a >= 90)
	{
		enjoyC += g_Dice % 3; enjoyF += g_Dice % 3;
		CleanAmt = int(CleanAmt * 1.1);
		/* */if (roll_b < 50)	ss << "She cleaned the building while humming a pleasant tune.";
		else /*            */	ss << "She had a great time working today.";
	}
	else
	{
		enjoyC += g_Dice % 2; enjoyF += g_Dice % 2;
		ss << "The shift passed uneventfully.";
	}
	ss << "\n\n";
#endif


	// `J` Farm Bookmark - adding in items that can be created in the farm
#if 1

	string itemmade = "";
	sInventoryItem* item = NULL;
	int totalitemsmade = 1;
	if (g_Dice.percent(min(90.0, jobperformance / 2)))
	{
		int chooseitem = g_Dice % (girl->magic() < 50 ? 50 : 60);	// limit some of the more magical items

		/* */if (chooseitem < 10)	itemmade = "C.G. Burger";
		else if (chooseitem < 20)	itemmade = "Premium Hot Dogs";
		else if (chooseitem < 28)	itemmade = "C.G. Cheese Burger";
		else if (chooseitem < 36)	itemmade = "C.G. Bacon Cheese Burger";
		else if (chooseitem < 43)	itemmade = "Turducken";
		else if (chooseitem < 50)	itemmade = "Cold Turkey";
		else if (chooseitem < 52)	itemmade = "Cold Turkey ";
		else if (chooseitem < 54)	itemmade = "Cold Turkey  ";
		else if (chooseitem < 56)	itemmade = "Cold  Turkey";
		else if (chooseitem < 58)	itemmade = "Cold  Turkey ";
		else /*                */	itemmade = "Aoshima BEEF!!";

		if (chooseitem < 40) totalitemsmade += g_Dice % 3;
		item = g_InvManager.GetItem(itemmade);
	}
	if (item)
	{
		msgtype = EVENT_GOODNEWS;
		ss << "\n\n" << girlName << " made ";
		if (totalitemsmade == 1) ss << "one ";
		else ss << totalitemsmade << " ";
		ss << itemmade << " for you.";
		for (int i = 0; i < totalitemsmade; i++)
			g_Brothels.AddItemToInventory(item);
	}

#endif


	g_Girls.UpdateEnjoyment(girl, actiontype, enjoy);
	girl->m_Events.AddMessage(ss.str(), imagetype, msgtype);


	int roll_max = (g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_CHARISMA));
	roll_max /= 4;
	wages += 10 + g_Dice%roll_max;
	if (wages < 0) wages = 0;
	girl->m_Pay = wages;


	// Improve stats
	int xp = 5, libido = 1, skill = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_EXP, (g_Dice % xp) + 1);
	g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, libido);

	// primary (+2 for single or +1 for multiple)
	g_Girls.UpdateSkill(girl, SKILL_ANIMALHANDLING, (g_Dice % skill) + 1);
	g_Girls.UpdateStat(girl, STAT_STRENGTH, (g_Dice % skill) + 1);
	// secondary (-1 for one then -2 for others)
	g_Girls.UpdateSkill(girl, SKILL_MEDICINE, max(0, (g_Dice % skill) - 1));
	g_Girls.UpdateStat(girl, STAT_INTELLIGENCE, max(0, (g_Dice % skill) - 2));
	g_Girls.UpdateSkill(girl, SKILL_COOKING, max(0, (g_Dice % skill) - 2));

	return false;
}

double cJobManager::JP_Butcher(sGirl* girl, bool estimate)// not used
{
	double jobperformance =
		// primary - first 100
		((girl->animalhandling() + girl->strength()) / 2) +
		// secondary - second 100
		((girl->medicine() + girl->intelligence() + girl->cooking()) / 3) +
		// level bonus
		girl->level();

	//good traits
	if (g_Girls.HasTrait(girl, "Quick Learner"))  jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Psychic"))		  jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Farmers Daughter"))	jobperformance += 20;
	if (g_Girls.HasTrait(girl, "Country Gal"))		jobperformance += 10;

	//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))	jobperformance -= 50; //needs others to do the job
	if (g_Girls.HasTrait(girl, "Clumsy")) 		jobperformance -= 20; //spills food and breaks things often
	if (g_Girls.HasTrait(girl, "Aggressive")) 	jobperformance -= 20; //gets mad easy
	if (g_Girls.HasTrait(girl, "Nervous"))		jobperformance -= 30; //don't like to be around people	
	if (g_Girls.HasTrait(girl, "Meek"))			jobperformance -= 20;

	if (g_Girls.HasTrait(girl, "One Arm"))		jobperformance -= 40;
	if (g_Girls.HasTrait(girl, "One Foot"))		jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "One Hand"))		jobperformance -= 30; 
	if (g_Girls.HasTrait(girl, "One Leg"))		jobperformance -= 35;
	if (g_Girls.HasTrait(girl, "No Arms"))		jobperformance -= 125;
	if (g_Girls.HasTrait(girl, "No Feet"))		jobperformance -= 40;
	if (g_Girls.HasTrait(girl, "No Hands"))		jobperformance -= 50;
	if (g_Girls.HasTrait(girl, "No Legs"))		jobperformance -= 75;
	if (g_Girls.HasTrait(girl, "Blind"))		jobperformance -= 30;
	if (g_Girls.HasTrait(girl, "Retarded"))		jobperformance -= 60;
	if (g_Girls.HasTrait(girl, "Smoker"))		jobperformance -= 10;	//would need smoke breaks

	if (g_Girls.HasTrait(girl, "Alcoholic"))			jobperformance -= 25;
	if (g_Girls.HasTrait(girl, "Fairy Dust Addict"))	jobperformance -= 25;
	if (g_Girls.HasTrait(girl, "Shroud Addict"))		jobperformance -= 25;
	if (g_Girls.HasTrait(girl, "Viras Blood Addict"))	jobperformance -= 25;

	return jobperformance;
}
