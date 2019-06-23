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
#pragma region //	Includes and Externs			//
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

#pragma endregion

// `J` Job Farm - Producers
bool cJobManager::WorkBaker(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
#pragma region //	Job setup				//
	int actiontype = ACTION_WORKCOOKING;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();
	if (girl->disobey_check(actiontype, brothel))			// they refuse to work
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked as a baker on the farm.\n \n";

	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	int wages = 20, tips = 0;
	int enjoy = 0;
	int imagetype = IMGTYPE_COOK;
	int msgtype = Day0Night1;

#pragma endregion
#pragma region //	Job Performance			//

	double jobperformance = JP_Baker(girl, false);
	double craftpoints = jobperformance;

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
	ss << "\n \n";

#pragma endregion
#pragma region	//	Enjoyment and Tiredness		//


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
		enjoy -= g_Dice % 3;
		/* */if (roll_b < 30)	ss << "She spilled a bucket of something unpleasant all over herself.";
		else if (roll_b < 60)	ss << "She stepped in something unpleasant.";
		else /*            */	ss << "She did not like working on the farm today.";
	}
	else if (roll_a >= 90)
	{
		enjoy += g_Dice % 3;
		/* */if (roll_b < 50)	ss << "She cleaned the building while humming a pleasant tune.";
		else /*            */	ss << "She had a great time working today.";
	}
	else
	{
		enjoy += g_Dice % 2;
		ss << "The shift passed uneventfully.";
	}
	ss << "\n \n";
#endif



#pragma endregion
#pragma region	//	Create Items				//


	// `J` Farm Bookmark - adding in items that can be created in the farm

	string itemmade = "";
	bool a0an1 = 0;
	sInventoryItem* item = NULL;
	if (g_Dice.percent(min(90.0, jobperformance / 2)))
	{
		int chooseitem = g_Dice % (girl->magic() < 80 ? 96 : 100);	// limit some of the more magical items

		/* */if (chooseitem < 10) { itemmade = "Apple Tart";			a0an1 = 1; }
		else if (chooseitem < 12) { itemmade = "Chocolate Cake"; }
		else if (chooseitem < 20) { itemmade = "Chocolate Cake "; }
		else if (chooseitem < 30) { itemmade = "Fancy Breath Mints"; }
		else if (chooseitem < 35) { itemmade = "Fruitcake"; }
		else if (chooseitem < 40) { itemmade = "Grilled Cheese Deluxe"; }
		else if (chooseitem < 45) { itemmade = "Pizza Cake"; }
		else if (chooseitem < 50) { itemmade = "Whole Wheat Donut"; }
		else if (chooseitem < 55) { itemmade = "Double Glazed Donut"; }
		else if (chooseitem < 66) { itemmade = "Box of Cookies"; }
		else if (chooseitem < 72) { itemmade = "Box of Sugar Cookies"; }
		else if (chooseitem < 77) { itemmade = "Box of Chocolate Chip Cookies"; }
		else if (chooseitem < 82) { itemmade = "Eggscellent Challenge";	a0an1 = 1; }
		else if (chooseitem < 87) { itemmade = "Yummi Gummi Lingere"; }
		else if (chooseitem < 92) { itemmade = "Yummy Cookie Bra"; }
		else if (chooseitem < 94) { itemmade = "Death Sandwich"; }
		else if (chooseitem < 96) { itemmade = "Death Sandwich "; }
		else if (chooseitem < 97) { itemmade = "Eldritch Cookie";		a0an1 = 1; }
		else if (chooseitem < 98) { itemmade = "Honeypuff Scones"; }
		else if (chooseitem < 99) { itemmade = "Leprechaun Biscuit"; }
		else /*                */ { itemmade = "Heaven-and-Earth Cake"; }

		item = g_InvManager.GetItem(itemmade);
	}
	if (item)
	{
		msgtype = EVENT_GOODNEWS;
		ss << "\n \n" << girlName << " made " << (a0an1 ? "an " : "a ") << itemmade << " for you.";
		g_Brothels.AddItemToInventory(item);
	}




#pragma endregion
#pragma region	//	Money					//


#pragma endregion
#pragma region	//	Finish the shift			//



	// `J` - Finish the shift - Baker

	// Push out the turn report
	girl->m_Events.AddMessage(ss.str(), imagetype, msgtype);

	// Money
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);

	// Base Improvement and trait modifiers
	int xp = 5, libido = 1, skill = 3;
	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }
	// EXP and Libido
	girl->exp((g_Dice%xp)+1);
	girl->upd_temp_stat(STAT_LIBIDO, libido);

	// primary improvement (+2 for single or +1 for multiple)
	girl->cooking((g_Dice % skill) + 2);
	// secondary improvement (-1 for one then -2 for others)
	girl->service(max(0,(g_Dice%skill)-1));
	girl->intelligence(max(0,(g_Dice%skill)-2));
	girl->herbalism(max(0,(g_Dice%skill)-2));

	// Update Enjoyment
	girl->upd_Enjoyment(actiontype, enjoy);
	// Gain Traits
	g_Girls.PossiblyGainNewTrait(girl, "Chef", 70, actiontype, girlName + " has prepared enough food to qualify as a Chef.", Day0Night1);

#pragma endregion
	return false;
}

double cJobManager::JP_Baker(sGirl* girl, bool estimate)// 'Mute' used inside of cJobManager::WorkBaker
{
	double jobperformance =
		// primary - first 100
		girl->cooking() +
		// secondary - second 100
		((girl->service() + girl->intelligence() + girl->herbalism()) / 3) +
		// level bonus
		girl->level();

	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}

	//good traits
	if (girl->has_trait( "Quick Learner"))  jobperformance += 5;
	if (girl->has_trait( "Psychic"))		  jobperformance += 10;
	if (girl->has_trait( "Mixologist"))	  jobperformance += 20;	//Good with measures
	if (girl->has_trait( "Chef"))			  jobperformance += 30;


	//bad traits
	if (girl->has_trait( "Dependant"))	jobperformance -= 50; // needs others to do the job
	if (girl->has_trait( "Clumsy")) 		jobperformance -= 20; //spills food and breaks things often
	if (girl->has_trait( "Aggressive")) 	jobperformance -= 20; //gets mad easy
	if (girl->has_trait( "Nervous"))		jobperformance -= 30; //don't like to be around people
	if (girl->has_trait( "Meek"))			jobperformance -= 20;


	return jobperformance;
}
