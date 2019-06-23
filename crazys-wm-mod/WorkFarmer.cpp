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

// `J` Job Farm - Laborers
bool cJobManager::WorkFarmer(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
#pragma region //	Job setup				//
	int actiontype = ACTION_WORKFARM;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();
	if (girl->disobey_check(actiontype, brothel))			// they refuse to work
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked tending crops on the farm.\n \n";

	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	int wages = 20, tips = 0;
	int enjoy = 0;
	int imagetype = IMGTYPE_FARM;
	int msgtype = Day0Night1;

#pragma endregion
#pragma region //	Job Performance			//

	double jobperformance = (int)JP_Farmer(girl, false);
	double foodproduced = jobperformance;
	int alchemyproduced = 0;
	int goodsproduced = 0;

	if (jobperformance >= 245)
	{
		ss << "Her basket practically fills itself as she walks down the rows of crops.";
		foodproduced *= 5; roll_a += 10; roll_b += 25;
	}
	else if (jobperformance >= 185)
	{
		ss << "Her hands moved like lightning as she picked only the best crops.";
		foodproduced *= 4; roll_a += 5; roll_b += 18;
	}
	else if (jobperformance >= 145)
	{
		ss << "She knows exactly when the crops are ready to be picked and how to best collect them.";
		foodproduced *= 3; roll_a += 2; roll_b += 10;
	}
	else if (jobperformance >= 100)
	{
		ss << "She can pick the crops fairly well without too many culls.";
		foodproduced *= 2;
	}
	else if (jobperformance >= 70)
	{
		ss << "She isn't very good at knowing which plants are ripe and which should have been left a little longer.";
		roll_a -= 2; roll_b -= 5;
	}
	else
	{
		ss << "She seems to take more of the unuseable parts of the plants than she takes the edible parts.";
		wages -= 10; foodproduced *= 0.8; roll_a -= 5; roll_b -= 10;
	}
	ss << "\n \n";

#pragma endregion
#pragma region	//	Enjoyment and Tiredness		//

	int tired = (300 - (int)jobperformance);	// this gets divided in roll_a by (8, 10 or 12) so it will end up around 0-40 tired
	if (roll_a <= 10)
	{
		tired /= 8;
		enjoy -= g_Dice % 3;
		if (roll_b < 20)	// injury
		{
			girl->health(-(1 + g_Dice % 5));
			foodproduced *= 0.8;
			if (g_Dice.percent(girl->magic() / 2))
			{
				girl->mana(-10 - (g_Dice % 10));
				ss << "While trying to use magic to do her work for her, the magic rebounded on her";
			}
			else ss << "She cut herself while working";
			if (girl->is_dead())
			{
				ss << " killing her.";
				g_MessageQue.AddToQue(girlName + " was killed in an accident at the Farm.", COLOR_RED);
				return false;	// not refusing, she is dead
			}
			else ss << ".";
		}
		else	// unhappy
		{
			foodproduced *= 0.9;
			ss << "She did not like working in the fields today.";
			girl->happiness(-(g_Dice % 11));
		}
	}
	else if (roll_a >= 90)
	{
		tired /= 12;
		foodproduced *= 1.1;
		enjoy += g_Dice % 3;
		/* */if (roll_b < 50)	ss << "She kept a steady pace by humming a pleasant tune.";
		else /*            */	ss << "She had a great time working today.";
	}
	else
	{
		tired /= 10;
		enjoy += g_Dice % 2;
		ss << "The shift passed uneventfully.";
	}
	ss << "\n \n";

#pragma endregion
#pragma region	//	Create Items				//

	if (g_Dice.percent((girl->farming() + girl->magic()) / 10) && g_Dice.percent(jobperformance / 10))
	{
		string itemname = ""; int itemnumber = 1;
		/* */if (roll_c > 30)	{ itemname = "Nut of Knowledge";		itemnumber = (roll_c > 90 ? g_Dice % 3 + 2 : 1); }
		else if (roll_c > 10)	{ itemname = "Mango of Knowledge";		itemnumber = (roll_c > 28 ? 2 : 1); }
		else/*            */	{ itemname = "Watermelon of Knowledge"; itemnumber = (roll_c == 9 ? 2 : 1); }

		sInventoryItem* item = g_InvManager.GetItem(itemname);
		if (item)
		{
			for (int i = 0; i < itemnumber; i++) g_Brothels.AddItemToInventory(item);
			ss << "While picking crops, " << girlName << " sensed a magical aura and found ";
			if (itemnumber == 1) ss << "a"; else ss << itemnumber;
			ss << " " << itemname << ".\n";
			foodproduced -= itemnumber;
		}
	}
	if (g_Dice.percent(girl->herbalism() / 2) && g_Dice.percent(jobperformance / 10))
	{
		alchemyproduced = 1 + g_Dice % (girl->herbalism() / 10);
		ss << "While sorting the day's haul, " << girlName << " came across ";
		if (alchemyproduced == 1) ss << "a specimen";
		else ss << alchemyproduced << " specimens";
		ss << " that would work well in potions.\n";
		foodproduced -= alchemyproduced;
	}
	if (g_Dice.percent(girl->crafting() / 2) && g_Dice.percent(jobperformance / 10))
	{
		goodsproduced = 1 + g_Dice % (girl->crafting() / 10);
		ss << girlName << " created ";
		if (goodsproduced == 1) ss << "a little toy";
		else ss << goodsproduced << " little toys";
		ss << " from the unuseable parts of her crops.\n";
	}
	if (foodproduced <= 0) foodproduced = 0;

#pragma endregion
#pragma region	//	Money					//

	// slave girls not being paid for a job that normally you would pay directly for do less work
	if ((girl->is_slave() && !cfg.initial.slave_pay_outofpocket()))
	{
		foodproduced *= 0.9;
		wages = 0;
	}
	else
	{
		wages += (int)foodproduced / 100; // `J` Pay her based on how much she brought in
	}

#pragma endregion
#pragma region	//	Finish the shift			//

	if (foodproduced > 0)
	{
		g_Brothels.add_to_food((int)foodproduced);
		ss << "\n" << girlName << " brought in " << (int)foodproduced << " units of food.";
	}
	if (alchemyproduced > 0)	g_Brothels.add_to_alchemy((int)alchemyproduced);
	if (goodsproduced > 0)		g_Brothels.add_to_goods((int)goodsproduced);

	// Money
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);

	// Base Improvement and trait modifiers
	int xp = 5, libido = 1, skill = 3;
	/* */if (girl->has_trait("Quick Learner"))	{ skill += 1; xp += 3; }
	else if (girl->has_trait("Slow Learner"))	{ skill -= 1; xp -= 3; }
	/* */if (girl->has_trait("Nymphomaniac"))	{ libido += 2; }
	// EXP and Libido
	int I_xp = (g_Dice % xp) + 1;							girl->exp(I_xp);
	int I_libido = (g_Dice % libido) + 1;					girl->upd_temp_stat(STAT_LIBIDO, I_libido);
	if (tired > 0) girl->tiredness(tired);

	// primary improvement (+2 for single or +1 for multiple)
	int I_farming = (g_Dice % skill) + 2;					girl->farming(I_farming);
	// secondary improvement (-1 for one then -2 for others)
	int I_strength = max(0, (g_Dice % skill) - 1);			girl->strength(I_strength);
	int I_constitution = max(0, (g_Dice % skill) - 2);		girl->constitution(I_constitution);
	int I_intelligence = max(0, (g_Dice % skill) - 2);		girl->intelligence(I_intelligence);

	// Update Enjoyment
	if (jobperformance < 50) enjoy -= 1; if (jobperformance < 0) enjoy -= 1;	// if she doesn't do well at the job, she enjoys it less
	if (jobperformance > 200) enjoy *= 2;		// if she is really good at the job, her enjoyment (positive or negative) is doubled
	girl->upd_Enjoyment(actiontype, enjoy);

	// Gain Traits
	g_Girls.PossiblyGainNewTrait(girl, "Tough", 50, actiontype, "Working in the heat of the sun has made " + girlName + " rather Tough.", Day0Night1);

	if (cfg.debug.log_show_numbers())
	{
		ss << "\n \nNumbers:"
			<< "\n Job Performance = " << (int)jobperformance
			<< "\n Wages = " << (int)wages
			<< "\n Tips = " << (int)tips
			<< "\n Xp = " << I_xp
			<< "\n Libido = " << I_libido
			<< "\n Farming = " << I_farming
			<< "\n Strength = " << I_strength
			<< "\n Constitution = " << I_constitution
			<< "\n Intelligence = " << I_intelligence
			<< "\n Tiredness = " << tired
			<< "\n Enjoy " << girl->enjoy_jobs[actiontype] << " = " << enjoy
			;
	}

	// Push out the turn report
	girl->m_Events.AddMessage(ss.str(), imagetype, msgtype);

#pragma endregion
	return false;
}

double cJobManager::JP_Farmer(sGirl* girl, bool estimate)// not used
{
	double jobperformance =
		// primary - first 100
		girl->farming() +
		// secondary - second 100
		((girl->intelligence() + girl->constitution() + girl->strength()) / 3) +
		// level bonus
		girl->level();
	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}

	//good traits
	if (girl->has_trait( "Farmer"))				jobperformance += 30;
	if (girl->has_trait( "Farmers Daughter"))		jobperformance += 20;
	if (girl->has_trait( "Country Gal"))			jobperformance += 10;
	if (girl->has_trait( "Psychic"))				jobperformance += 5;
	if (girl->has_trait( "Quick Learner"))		jobperformance += 5;

	//bad traits
	if (girl->has_trait( "Dependant"))			jobperformance -= 50; // needs others to do the job
	if (girl->has_trait( "Nervous"))				jobperformance -= 20; //don't like to be around people
	if (girl->has_trait( "City Girl"))			jobperformance -= 20;
	if (girl->has_trait( "Clumsy")) 				jobperformance -= 20; //spills food and breaks things often
	if (girl->has_trait( "Aggressive")) 			jobperformance -= 20; //gets mad easy
	if (girl->has_trait( "Meek"))					jobperformance -= 20;

	if (girl->has_trait( "One Arm"))				jobperformance -= 40;
	if (girl->has_trait( "One Foot"))				jobperformance -= 40;
	if (girl->has_trait( "One Hand"))				jobperformance -= 30;
	if (girl->has_trait( "One Leg"))				jobperformance -= 60;
	if (girl->has_trait( "No Arms"))				jobperformance -= 125;
	if (girl->has_trait( "No Feet"))				jobperformance -= 60;
	if (girl->has_trait( "No Hands"))				jobperformance -= 50;
	if (girl->has_trait( "No Legs"))				jobperformance -= 150;
	if (girl->has_trait( "Blind"))				jobperformance -= 30;
	if (girl->has_trait( "Retarded"))				jobperformance -= 30;
	if (girl->has_trait( "Smoker"))				jobperformance -= 10;	//would need smoke breaks

	if (girl->has_trait( "Alcoholic"))			jobperformance -= 25;
	if (girl->has_trait( "Fairy Dust Addict"))	jobperformance -= 25;
	if (girl->has_trait( "Shroud Addict"))		jobperformance -= 25;
	if (girl->has_trait( "Viras Blood Addict"))	jobperformance -= 25;

	return jobperformance;
}
