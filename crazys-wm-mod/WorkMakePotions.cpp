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

// `J` Job Farm - Producers - updated 1/29/15
bool cJobManager::WorkMakePotions(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
#pragma region //	Job setup				//
	int actiontype = ACTION_WORKMAKEPOTIONS;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();
	if (girl->disobey_check(actiontype, brothel))			// they refuse to work
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked as a potions maker on the farm.\n \n";

	g_Girls.UnequipCombat(girl);	// weapons and armor can get in the way

	int wages = 20, tips = 0;
	int enjoy = 0;
	int imagetype = IMGTYPE_CRAFT;
	int msgtype = Day0Night1;

#pragma endregion
#pragma region //	Job Performance			//


	double jobperformance = JP_MakePotions(girl, false);

	if (jobperformance >= 245)
	{
		ss << " She must be the perfect at this.";
		wages += 155;
		roll_a -= 20;
	}
	else if (jobperformance >= 185)
	{
		ss << " She's unbelievable at this.";
		wages += 95;
		roll_a -= 10;
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
		roll_a += 5;
	}
	else if (jobperformance >= 70)
	{
		ss << " She was nervous and made a few mistakes. She isn't that good at this.";
		wages -= 5;
		roll_a += 10;
	}
	else
	{
		ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.";
		wages -= 15;
		roll_a += 20;
	}
	ss << "\n \n";

#pragma endregion
#pragma region	//	Enjoyment and Tiredness		//


	//enjoyed the work or not
	if (roll_a <= 10)
	{
		enjoy += g_Dice % 3 + 1;
		ss << "She had a great time making potions today.";
	}
	else if (roll_a >= 90)
	{
		enjoy -= (g_Dice % 5 + 1);
		ss << "Some potions blew up in her face today.";
		girl->health(-(g_Dice % 10));
		girl->happiness(-(g_Dice % 20));
		girl->beauty(-(g_Dice % 3));

	}
	else if (roll_a >= 80)
	{
		enjoy -= (g_Dice % 3 + 1);
		ss << "She did not like making potions today.";
	}
	else
	{
		enjoy += g_Dice % 2;
		ss << "The shift passed uneventfully.";
	}
	ss << "\n \n";


#pragma endregion
#pragma region	//	Create Items				//


	// `J` Farm Bookmark - adding in potions that can be created in the farm

	stringstream ssitem;
	int numitemsmade = 1;	// counts down
	int totalitemsmade = 0;	// counts up
	int choosequality = (g_Dice % ((int)jobperformance / 2) + ((int)jobperformance / 2));
	// more girls working can help out a bit, but too many can hurt so limit it to 10
	choosequality += min(10, (g_Farm.GetNumGirlsOnJob(0, JOB_MAKEPOTIONS, Day0Night1) - 1));

	while (numitemsmade > 0 && choosequality >= 70)
	{
		numitemsmade--;
		sInventoryItem* item = NULL;
		string itemmade = "";
		if (choosequality < 70)	{}	// she made nothing
		else if (choosequality < 100)	// Common items
		{
			int chooseitem = g_Dice % 46;
			/* */if (chooseitem < 6)	itemmade = "Healing Salve (T)";
			else if (chooseitem < 10)	itemmade = "Healing Salve (S)";
			else if (chooseitem < 16)	itemmade = "Incense of Serenity (T)";
			else if (chooseitem < 20)	itemmade = "Incense of Serenity (S)";
			else if (chooseitem < 23)	itemmade = "Cheap Perfume";
			else if (chooseitem < 26)	itemmade = "Basic Makeup";
			else if (chooseitem < 29)	itemmade = "Mana Potion";
			else if (chooseitem < 32)	itemmade = "Booty Lube";
			else if (chooseitem < 34)	itemmade = "NO MORE SLEEP! Brand Sleeping Pills";
			else if (chooseitem < 36)	itemmade = "Dar-E-Pills (Red)";
			else if (chooseitem < 38)	itemmade = "Dar-E-Pills (Orange)";
			else if (chooseitem < 40)	itemmade = "Dar-E-Pills (Yellow)";
			else if (chooseitem < 42)	itemmade = "Dar-E-Pills (Green)";
			else if (chooseitem < 44)	itemmade = "Dar-E-Pills (Blue)";
			else /*                */	itemmade = "Stim pack";
		}
		else if (choosequality < 145)	// Uncommon items
		{
			int chooseitem = g_Dice % 40;
			/* */if (chooseitem < 5)	itemmade = "Incense of Serenity (M)";
			else if (chooseitem < 10)	itemmade = "Healing Salve (M)";
			else if (chooseitem < 15)	itemmade = "Quality Perfume";
			else if (chooseitem < 20)	itemmade = "Quality Makeup";
			else if (chooseitem < 22)	{ switch (g_Dice % 9) { case 1: itemmade = "Philtre of Abortion ,"; case 2: itemmade = "Philtre of Abortion ."; case 3: itemmade = "Philtre of Abortion, "; case 4: itemmade = "Philtre of Abortion,,"; case 5: itemmade = "Philtre of Abortion. "; case 6: itemmade = "Philtre of Abortion.."; default: itemmade = "Philtre of Abortion  "; } }
			else if (chooseitem < 24)	{ switch (g_Dice % 4) { case 0: itemmade = "Anti-age cream,"; case 1: itemmade = "Anti-age cream."; default: itemmade = "Anti-age cream "; } }
			else if (chooseitem < 26)	itemmade = "Magic Makeup";
			else if (chooseitem < 28)	itemmade = "Dark Magic Makeup";
			else if (chooseitem < 30)	itemmade = "Deluxe Booty Lube";
			else if (chooseitem < 31)	itemmade = "Stoneskin Elixir";
			else if (chooseitem < 32)	itemmade = "Enchanting Perfume";
			else if (chooseitem < 33)	itemmade = "Oil of Lesser Breast Growth";
			else if (chooseitem < 34)	itemmade = "Oil of Lesser Scar Removing";
			else if (chooseitem < 35)	itemmade = "Breast Reduction Pill A";
			else if (chooseitem < 36)	itemmade = "Breast Reduction Pill B";
			else if (chooseitem < 37)	itemmade = "Humanity Elixir";
			else if (chooseitem < 38)	itemmade = "Hybrid Injection";
			else if (chooseitem < 39)	itemmade = "Potion of Broken Ascendance";
			else /*                */	itemmade = "BrainMax";
		}
		else if (choosequality < 185)	// rare items
		{
			int chooseitem = g_Dice % 50;
			/* */if (chooseitem < 5)	itemmade = "Incense of Serenity (L)";
			else if (chooseitem < 10)	itemmade = "Healing Salve (L)";
			else if (chooseitem < 13)	itemmade = "Cure for Alcoholic";
			else if (chooseitem < 16)	itemmade = "Cure for Fairy Dust Addiction";
			else if (chooseitem < 19)	itemmade = "Cure for Shroud Addiction";
			else if (chooseitem < 22)	itemmade = "Cure for Viras Blood Addiction";
			else if (chooseitem < 25)	itemmade = "Perfect Philtre of Abortion";
			else if (chooseitem < 26)	itemmade = "Super Stim pack";
			else if (chooseitem < 27)	itemmade = "Seductive Perfume";
			else if (chooseitem < 28)	itemmade = "Pills of Regression";
			else if (chooseitem < 29)	itemmade = "Potion of Tightness";
			else if (chooseitem < 30)	itemmade = "Love Potion";
			else if (chooseitem < 31)	itemmade = "Elixir of Conviction";
			else if (chooseitem < 32)	itemmade = "Elixir of Youth";
			else if (chooseitem < 33)	itemmade = "Hearts and Hooves Potion";
			else /*                */	{ numitemsmade += 2; choosequality = 140; }		// make 2 items of lesser quality

		}
		else if (choosequality < 245)	// Very rare items
		{
			int chooseitem = g_Dice % 40;
			/* */if (chooseitem < 3)	itemmade = "Herpes Cure";
			else if (chooseitem < 6)	itemmade = "Syphilis Cure";
			else if (chooseitem < 9)	itemmade = "Chlamydia Cure";
			else if (chooseitem < 12)	itemmade = "AIDS Cure";
			else if (chooseitem < 13)	itemmade = "Brainwashing Oil";
			else if (chooseitem < 14)	itemmade = "Potion of Androgyny";
			else if (chooseitem < 15)	itemmade = "Oil of Greater Breast Growth";
			else if (chooseitem < 16)	itemmade = "Oil of Greater Scar Removing";
			else if (chooseitem < 17)	itemmade = "Starfish Elixir";
			else if (chooseitem < 18) { itemmade = "Better Zed than Dead"; switch (g_Dice % 6) { case 0: itemmade += " ."; break; case 1: itemmade += ", "; break; case 2: itemmade += ". "; break; case 3: itemmade += ".."; break; default: break; } }
			else if (chooseitem < 19)	itemmade = "Oil of Extreme Breast Growth";
			else if (chooseitem < 20)	itemmade = "Oil of Redemption";
			else if (chooseitem < 21)	itemmade = "Unguent of Intense Desire";
			else if (chooseitem < 22)	itemmade = "Attitude Reajustor";
			else { numitemsmade += 2; choosequality = 180; }		// make 2 items of lesser quality
		}
		else							// unique items
		{
			int chooseitem = g_Dice % 100;
			/* */if (chooseitem < 5)	itemmade = "Potion of Oppai Loli";
			else if (chooseitem < 10)	itemmade = "Refined Mandragora Extract";
			else if (chooseitem < 15)	itemmade = "Elixir of Ultimate Regeneration";
			else if (chooseitem < 16)	itemmade = "Reset Potion MK i";
			else if (chooseitem < 17)	itemmade = "Reset Potion MK ii";
			else { numitemsmade += 2; choosequality = 240; }		// make 2 items of lesser quality
		}

		if (itemmade != "")
		{
			item = g_InvManager.GetItem(itemmade);
			if (item)
			{
				if (totalitemsmade > 0) ssitem << ", ";
				ssitem << itemmade;
				totalitemsmade++;
				g_Brothels.AddItemToInventory(item);
			}
		}
	}

	if (totalitemsmade > 0)
	{
		ss << "\n \n" << girlName << " was able to make ";
		if (totalitemsmade == 1) ss << "one ";
		else ss << totalitemsmade << " items: ";
		ss << ssitem.str();
		msgtype = EVENT_GOODNEWS;
	}



#pragma endregion
#pragma region	//	Money					//


#pragma endregion
#pragma region	//	Finish the shift			//



	girl->upd_Enjoyment(actiontype, enjoy);
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_CRAFT, msgtype);

	// Money
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);

	// Improve stats
	int xp = 5, libido = 1, skill = 3;

	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }

	girl->exp((g_Dice % xp) + 1);
	girl->upd_temp_stat(STAT_LIBIDO, libido);

	// primary (+2 for single or +1 for multiple)
	girl->brewing((g_Dice % skill) + 1);
	girl->herbalism((g_Dice % skill) + 1);
	// secondary (-1 for one then -2 for others)
	girl->intelligence(max(0, (g_Dice % skill) - 1));
	girl->cooking(max(0, (g_Dice % skill) - 2));
	girl->magic(max(0, (g_Dice % skill) - 2));

#pragma endregion
	return false;
}

double cJobManager::JP_MakePotions(sGirl* girl, bool estimate)// not used
{
	double jobperformance =
		// primary - first 100
		((girl->brewing() + girl->herbalism()) / 2) +
		// secondary - second 100
		((girl->intelligence() + girl->cooking() + girl->magic()) / 3) +
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
	if (girl->has_trait( "Nerd"))			  jobperformance += 10;

	//bad traits
	if (girl->has_trait( "Dependant"))	jobperformance -= 50; // needs others to do the job
	if (girl->has_trait( "Clumsy")) 		jobperformance -= 20; //spills food and breaks things often
	if (girl->has_trait( "Aggressive")) 	jobperformance -= 20; //gets mad easy
	if (girl->has_trait( "Nervous"))		jobperformance -= 30; //don't like to be around people
	if (girl->has_trait( "Meek"))			jobperformance -= 20;
	return jobperformance;
}
