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

// `J` Job Farm - Staff
bool cJobManager::WorkFarmMarketer(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
#pragma region //	Job setup				//
	int actiontype = ACTION_WORKCUSTSERV;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();
	if (girl->disobey_check(actiontype, brothel))			// they refuse to work
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked as a marketer on the farm.";

	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	int wages = 20, tips = 0;
	int enjoy = 0;
	int imagetype = IMGTYPE_PROFILE;
	int msgtype = Day0Night1;

#pragma endregion
#pragma region	//	Create Items				//

	int ForSale_HandmadeGoods = max(0, g_Brothels.m_HandmadeGoods - g_Brothels.m_HandmadeGoodsReserves);
	int ForSale_Beasts = max(0, g_Brothels.m_Beasts - g_Brothels.m_BeastsReserves);
	int ForSale_Food = max(0, g_Brothels.m_Food - g_Brothels.m_FoodReserves);
	int ForSale_Drinks = max(0, g_Brothels.m_Drinks - g_Brothels.m_DrinksReserves);
	int ForSale_Alchemy = max(0, g_Brothels.m_Alchemy - g_Brothels.m_AlchemyReserves);

	double pricemultiplier = 1.0;
	double jobperformance = JP_FarmMarketer(girl, false);

	// `J` Farm Bookmark - adding in items that can be created in the farm
	if (ForSale_Food + (g_Brothels.m_FoodReserves / 2) >= 10000 && g_Dice.percent(5))
	{
		sGirl* ugirl = 0;
		int cost = 10000;
		if (ForSale_Food >= 15000 && g_Dice.percent(cfg.slavemarket.unique_market()))
		{
			cost = 15000;
			ugirl = g_Girls.GetRandomGirl();				// Unique girl type
		}
		if (ugirl == 0)		// if not unique or a unique girl can not be found
		{
			cost = 10000;
			ugirl = g_Girls.CreateRandomGirl(0, false);	// create a random girl
		}
		if (ugirl)
		{
			stringstream Umsg;
			// improve her a bit because she should be a good girl to be traded
			ugirl->tiredness(-100);		ugirl->happiness(100);		ugirl->health(100);
			ugirl->charisma(10 + g_Dice % 30);			ugirl->beauty(10 + g_Dice % 10);
			ugirl->constitution(30 - g_Dice % 40);		ugirl->intelligence(10 - g_Dice % 15);
			ugirl->confidence(10 - g_Dice % 30);		ugirl->agility(20 - g_Dice % 25);
			ugirl->strength(1 + g_Dice % 20);			ugirl->obedience(10 + g_Dice % 40);
			ugirl->spirit(50 - g_Dice % 100);			ugirl->morality(10 + g_Dice % 40);
			ugirl->refinement(30 - g_Dice % 40);		ugirl->dignity(20 - g_Dice % 40);
			ugirl->npclove(g_Dice.bell(-100, 100));		// she may have had a boyfriend before she got sold to you
			ugirl->service(10 + g_Dice % 30);			ugirl->performance(g_Dice % 5);
			ugirl->crafting(g_Dice % 10);				ugirl->herbalism(g_Dice % 5);
			ugirl->farming(g_Dice % 10);				ugirl->brewing(g_Dice % 5);
			ugirl->animalhandling(g_Dice % 10);			ugirl->cooking(g_Dice % 20);

			Umsg << ugirl->m_Realname << " was purchased by Farm Marketer " << girlName << " in exchange for " << cost << " units of food.\n";
			ugirl->m_Events.AddMessage(Umsg.str(), IMGTYPE_PROFILE, EVENT_DUNGEON);
			g_Brothels.GetDungeon()->AddGirl(ugirl, DUNGEON_NEWGIRL);	// Either type of girl goes to the dungeon

			ss << "\n \nA merchant from a far off village brought a girl from his village to trade for " << cost << " units of food.\n" << ugirl->m_Realname << " has been sent to your dungeon.\n";
			g_Brothels.add_to_food(-cost);
		}
	}

#pragma endregion
#pragma region //	Job Performance			//

	if (jobperformance >= 245)
	{
		ss << " She must be the perfect at this.\n \n";
		pricemultiplier += 1.0;
	}
	else if (jobperformance >= 185)
	{
		ss << " She's unbelievable at this.\n \n";
		pricemultiplier += 0.5;
	}
	else if (jobperformance >= 145)
	{
		ss << " She's good at this job.\n \n";
		pricemultiplier += 0.2;
	}
	else if (jobperformance >= 100)
	{
		ss << " She made a few mistakes but overall she is okay at this.\n \n";
	}
	else if (jobperformance >= 70)
	{
		ss << " She was nervous and made a few mistakes. She isn't that good at this.\n \n";
		pricemultiplier -= 0.2;
	}
	else
	{
		ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.\n \n";
		pricemultiplier -= 0.5;
	}

#pragma endregion
#pragma region	//	Enjoyment and Tiredness		//

	if (roll_a <= 10)
	{
		enjoy -= g_Dice % 3;
		pricemultiplier -= 0.1;
		girl->happiness(-(g_Dice % 11));
		ss << "She did not like selling things today.";
	}
	else if (roll_a >= 90)
	{
		pricemultiplier += 0.1;
		enjoy += g_Dice % 3;
		girl->happiness(g_Dice % 8);
		ss << "She had a great time selling today.";
	}
	else
	{
		enjoy += g_Dice % 2;
		ss << "The shift passed uneventfully.";
	}
	ss << "\n \n";

#pragma endregion
#pragma region	//	Money					//

	double gold = 0.0;
	// start with how many of each she can sell
	int Sell_HandmadeGoods	= min(ForSale_HandmadeGoods,	(int)(jobperformance / 2.0 * ForSale_HandmadeGoods));
	int Sell_Beasts			= min(ForSale_Beasts,			(int)(jobperformance / 2.5 * ForSale_Beasts));
	int Sell_Food			= min(ForSale_Food,				(int)(jobperformance / 2.0 * ForSale_Food));
	int Sell_Drinks			= min(ForSale_Drinks,			(int)(jobperformance / 2.0 * ForSale_Drinks));
	int Sell_Alchemy		= min(ForSale_Alchemy,			(int)(jobperformance / 3.0 * ForSale_Alchemy));
	// for how much
	gold += (pricemultiplier * 1 * Sell_HandmadeGoods);
	gold += (pricemultiplier * (g_Dice % 451 + 50) * Sell_Beasts);
	gold += (pricemultiplier * 0.1 * Sell_Food);
	gold += (pricemultiplier * 0.1 * Sell_Drinks);
	gold += (pricemultiplier * (g_Dice % 141 + 10) * Sell_Alchemy);
	// remove them from the count
	g_Brothels.add_to_goods(-Sell_HandmadeGoods);
	g_Brothels.add_to_beasts(-Sell_Beasts);
	g_Brothels.add_to_food(-Sell_Food);
	g_Brothels.add_to_drinks(-Sell_Drinks);
	g_Brothels.add_to_alchemy(-Sell_Alchemy);
	// tell the player
	if (gold > 0)	// something was sold
	{
		ss << girlName << " was able to sell:\n";

		if (Sell_HandmadeGoods > 0)
		{
			if (Sell_HandmadeGoods == ForSale_HandmadeGoods) ss << "All " << Sell_HandmadeGoods;
			else ss << Sell_HandmadeGoods << " of the " << ForSale_HandmadeGoods;
			ss << " Handmade Goods.\n";
		}
		if (Sell_Beasts > 0)
		{
			if (Sell_Beasts == ForSale_Beasts) ss << "All " << Sell_Beasts;
			else ss << Sell_Beasts << " of the " << ForSale_Beasts;
			ss << " Beasts.\n";

		}
		if (Sell_Food > 0)
		{
			if (Sell_Food == ForSale_Food) ss << "All " << Sell_Food;
			else ss << Sell_Food << " of the " << ForSale_Food;
			ss << " Food.\n";
		}
		if (Sell_Drinks > 0)
		{
			if (Sell_Drinks == ForSale_Drinks) ss << "All " << Sell_Drinks;
			else ss << Sell_Drinks << " of the " << ForSale_Drinks;
			ss << " Drinks.\n";
		}
		if (Sell_Alchemy > 0)
		{
			if (Sell_Alchemy == ForSale_Alchemy) ss << "All " << Sell_Alchemy;
			else ss << Sell_Alchemy << " of the " << ForSale_Alchemy;
			ss << " Alchemy Items.\n";
		}
		ss << "She made a total of " << (int)gold << " from it all.\nShe gets 1% of the sales: " << (int)(gold / 100)<<".\nThe rest goes directly into your coffers.\n \n";
		wages += (int)(gold / 100); // `J` Pay her based on how much she brought in
		gold -= (int)(gold / 100);
		enjoy += (int)(wages / 100);		// the more she gets paid, the more she likes selling
	}

#pragma endregion
#pragma region	//	Finish the shift			//


	// Money
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);
	g_Gold.farm_income(gold);

	// Improve stats
	int xp = 10, libido = 1, skill = 3;

	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }

	// EXP and Libido
	int I_xp = (g_Dice % xp) + 1;							girl->exp(I_xp);
	int I_libido = (g_Dice % libido) + 1;					girl->upd_temp_stat(STAT_LIBIDO, I_libido);

	// primary (+2 for single or +1 for multiple)
	int I_charisma		= max(0, (g_Dice % skill) + 1);		girl->charisma(I_charisma);
	int I_confidence	= max(0, (g_Dice % skill) + 1);		girl->confidence(I_confidence);
	// secondary (-1 for one then -2 for others)
	int I_intelligence	= max(0, (g_Dice % skill) - 1);		girl->intelligence(I_intelligence);
	int I_fame			= max(0, (g_Dice % skill) - 2);		girl->fame(I_fame);
	int I_farming		= max(0, (g_Dice % skill) - 2);		girl->farming(I_farming);

	girl->upd_Enjoyment(actiontype, enjoy);
	g_Girls.PossiblyGainNewTrait(girl,		"Charismatic",	30, actiontype, girlName + " has been selling long enough that she has learned to be more Charismatic.", Day0Night1);
	g_Girls.PossiblyLoseExistingTrait(girl, "Meek",			40, actiontype, girlName + "'s having to work with customers every day has forced her to get over her meekness.", Day0Night1);
	g_Girls.PossiblyLoseExistingTrait(girl, "Shy",			50, actiontype, girlName + " has been selling for so long now that her confidence is super high and she is no longer Shy.", Day0Night1);
	g_Girls.PossiblyLoseExistingTrait(girl, "Nervous",		70, actiontype, girlName + " seems to finally be getting over her shyness. She's not always so Nervous anymore.", Day0Night1);
	g_Girls.PossiblyGainNewTrait(girl,		"Psychic",		90, actiontype, girlName + " has learned to size up the buyers so well that you'd almost think she was Psychic.", Day0Night1);

	if (cfg.debug.log_show_numbers())
	{
		ss << "\n \nNumbers:"
			<< "\n Job Performance = " << (int)jobperformance
			<< "\n Wages = " << (int)wages
			<< "\n Tips = " << (int)tips
			<< "\n Xp = " << I_xp
			<< "\n Libido = " << I_libido
			<< "\n Charisma = " << I_charisma
			<< "\n Confidence = " << I_confidence
			<< "\n Intelligence = " << I_intelligence
			<< "\n Fame = " << I_fame
			<< "\n Farming = " << I_farming
			<< "\n Enjoy " << girl->enjoy_jobs[actiontype] << " = " << enjoy
			;
	}

	// Push out the turn report
	girl->m_Events.AddMessage(ss.str(), imagetype, msgtype);

#pragma endregion
	return false;
}

double cJobManager::JP_FarmMarketer(sGirl* girl, bool estimate)// not used
{
	double jobperformance =
		// primary - first 100
		((girl->charisma() + girl->confidence()) / 2) +
		// secondary - second 100
		((girl->intelligence() + girl->fame() + girl->farming()) / 3) +
		// level bonus
		girl->level();
	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 5);
	}

	if (girl->morality() > 50)						jobperformance -= 5;	// too honest to cheat the customer
	else if (girl->morality() < -50)				jobperformance -= 5;	// too crooked to be trusted with an honest price

	//good traits
	if (girl->has_trait( "Charismatic"))		jobperformance += 15;
	if (girl->has_trait( "Sexy Air"))			jobperformance += 5;
	if (girl->has_trait( "Cool Person"))		jobperformance += 10;  //people love to be around her
	if (girl->has_trait( "Cute"))				jobperformance += 5;
	if (girl->has_trait( "Charming"))			jobperformance += 15;  //people like charming people
	if (girl->has_trait( "Quick Learner"))	jobperformance += 5;
	if (girl->has_trait( "Psychic"))			jobperformance += 10;



	//bad traits
	if (girl->has_trait( "Dependant"))		jobperformance -= 50; // needs others to do the job
	if (girl->has_trait( "Clumsy")) 			jobperformance -= 20; //spills food and breaks things often
	if (girl->has_trait( "Aggressive")) 		jobperformance -= 20; //gets mad easy
	if (girl->has_trait( "Nervous"))			jobperformance -= 30; //don't like to be around people
	if (girl->has_trait( "Meek"))				jobperformance -= 20;

	return jobperformance;
}
