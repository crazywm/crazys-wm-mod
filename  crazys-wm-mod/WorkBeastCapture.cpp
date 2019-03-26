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

// `J` Job Farm - Laborers - Combat_Job
bool cJobManager::WorkBeastCapture(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
#pragma region //	Job setup				//
	int actiontype = ACTION_COMBAT;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();
	if (girl->disobey_check(actiontype, brothel))			// they refuse to work
	{
		ss << " refused to capture beasts during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " equipped herself and went out to hunt for exotic beasts and animals.\n \n";

	g_Girls.EquipCombat(girl);	// ready armor and weapons!

	int wages = 40, tips = 0;
	int enjoy = 0;
	int imagetype = IMGTYPE_COMBAT;
	int msgtype = Day0Night1;

#pragma endregion
#pragma region //	The Fight to get the Beasts		//

	int tired = 0;
	int gainmax = (int)(JP_BeastCapture(girl, false) / 30) + 1;
	int gain = g_Dice % gainmax + 1;
	sGirl* tempgirl = g_Girls.CreateRandomGirl(18, false, false, false, true, false);
	Uint8 fight_outcome = 0;
	if (tempgirl)		// `J` reworked incase there are no Non-Human Random Girls
	{
		fight_outcome = g_Girls.girl_fights_girl(girl, tempgirl);
	}
	else
	{
		g_LogFile.write("Error: You have no Non-Human Random Girls for your girls to fight\n");
		g_LogFile.write("Error: You need a Non-Human Random Girl to allow WorkBeastCapture randomness");
		fight_outcome = 7;
	}
	if (tempgirl) delete tempgirl; tempgirl = 0;	// Cleanup
	if (fight_outcome == 7)
	{
		ss << "She came back with just one animal today.\n \n";
		ss << "(Error: You need a Non-Human Random Girl to allow WorkBeastCapture randomness)";
		gain = 1;
		msgtype = EVENT_WARNING;
		tired = 15;
	}
	else if (fight_outcome == 1)	// she won
	{
		if (gain <= 2)	gain = 2;
		if (gain >= gainmax)	gain = gainmax;
		ss << "She had fun hunting today and came back with " << gain << " new beasts.";;
		imagetype = IMGTYPE_COMBAT;
		tired = g_Dice % (3 * gain);
		enjoy += g_Dice % 4 + 2;
	}
	else		// she lost or it was a draw
	{
		gain = g_Dice.bell(-gainmax / 3, gainmax / 2);
		ss << " The animals were difficult to track today. " << girlName << " eventually returned worn out and frustrated, ";
		if (gain <= 0)
		{
			gain = 0;
			ss << "empty handed.";
		}
		else
		{
			ss << "dragging ";
			if (gain == 1)	ss << "one";
			if (gain > 1)	ss << gain;
			ss << " captured beast" << (gain > 1 ? "s" : "") << " behind her.";
		}
		imagetype = IMGTYPE_COMBAT;
		enjoy -= g_Dice % 3 + 1;
		tired = g_Dice % (10 * gain) + 20;
	}
	ss << "\n \n";

#pragma endregion
#pragma region	//	A Little Randomness			//

	//SIN: A little randomness
	if (((girl->animalhandling() + girl->charisma()) > 125) && g_Dice.percent(30))
	{
		ss << girlName << " has a way with animals, a" << (gain > 1 ? "nother" : "") << " beast freely follows her back.\n";
		gain++;
	}
	//SIN: most the rest rely on more than one cap so might as well skip the lot if less than this...
	if (gain > 1)
	{
		// `J` added a switch with a use next if check fails and changed percents to (gain * 5)
		switch (g_Dice % 10)
		{
		case 0:
			if (girl->has_trait( "Twisted") && girl->has_trait( "Nymphomaniac") && (girl->libido() >= 80))
			{
				ss << "Being a horny, twisted nymphomaniac, " << girlName << " had some fun with the beasts before she handed them over.\n";
				girl->beastiality(g_Dice % gain);
				girl->libido(-(g_Dice % gain));
				tired += gain;
				break;
			}
		case 1:
			if (girl->has_trait( "Psychic") && (girl->libido() >= 90) && g_Dice.percent(gain * 5))
			{
				ss << girlName << "'s Psychic sensitivity caused her mind be overwhelmed by the creatures' lusts";
				if (girl->check_virginity())
				{
					ss << " but, things were moving too fast and she regained control before they could take her virginity.\n";
					girl->beastiality(1);
					girl->libido(2); // no satisfaction!
				}
				else
				{
					ss << ". Many hours later she staggered in to present the creatures to you.\n";
					girl->beastiality(g_Dice % gain);
					girl->libido(-2 * (g_Dice % gain));
					girl->tiredness(gain);
					girl->calc_insemination(*g_Girls.GetBeast(), 1);
				}
				tired += gain;
				break;
			}
		case 2:
			if (girl->has_trait( "Assassin") && g_Dice.percent(gain * 5))
			{
				ss << " One of the captured creatures tried to escape on the way back. Trained assassin, " << girlName << ", instantly killed it as an example to the others.\n";
				girl->combat(1);
				gain--;
				break;
			}
		case 3:
			if (girl->tiredness() > 50 && g_Dice.percent(gain * 5))
			{
				ss << girlName << " was so exhausted she couldn't concentrate. One of the creatures escaped.\n";
				gain--;
				break;
			}
		default: break;

		}
	}

#pragma endregion
#pragma region	//	Collect Pets				//

	// `J` Farm Bookmark - adding in items that can be gathered in the farm
	if (g_Dice.percent(5))
	{
		string itemfound = ""; string itemfoundtext = "";
		int chooseitem = g_Dice.d100();

		if (chooseitem < 25)
		{
			itemfound = (g_Dice % 2) ? "Black Cat" : "Cat";
			itemfoundtext = "a stray cat and brought it back with her.";
		}
		else if (chooseitem < 50)
		{
			itemfound = "Guard Dog";
			itemfoundtext = "a tough looking stray dog and brought it back with her.";
		}
		else if (chooseitem < 60)
		{
			itemfound = "A Bunch of Baby Ducks";
			itemfoundtext = "a bunch of baby ducks without a mother, so she gathered them up in her shirt and brought them home.";
		}
		else if (chooseitem < 65)
		{
			itemfound = "Death Bear";
			itemfoundtext = "a large bear that seems to have had some training, so she brought it home.";
		}
		else if (chooseitem < 80)
		{
			itemfound = "Pet Spider";
			itemfoundtext = "a strange looking spider so she collected it in a bottle and brought it back with her.";
		}
		else if (chooseitem < 95)
		{
			itemfound = (g_Dice % 4 == 1) ? "Fox Stole" : "Fur Stole";
			itemfoundtext = "a dead animal that was not too badly damaged. She brought it home, skinned it, cleaned it up and made a lovely stole from it.";
		}
		else if (chooseitem < 96)
		{
			itemfound = "Echidna's Snake";
			itemfoundtext = "a rather obedient and psychic snake. It wrapped itself around her crotch and let her take it home.";
		}
		else
		{
			itemfound = "Tiger Shark Tooth";
			itemfoundtext = "a rather large shark tooth and brought it home.";
		}

		sInventoryItem* item = g_InvManager.GetItem(itemfound);
		if (item)
		{
			ss << girlName << " found " << itemfoundtext;
			g_Brothels.AddItemToInventory(item);
		}
	}

#pragma endregion
#pragma region	//	Money					//

	// slave girls not being paid for a job that normally you would pay directly for do less work
	if ((girl->is_slave() && !cfg.initial.slave_pay_outofpocket()))
	{
		wages = 0;
	}
	else
	{
		wages += gain * 10; // `J` Pay her based on how much she brings back
	}

#pragma endregion
#pragma region	//	Finish the shift			//

	g_Brothels.add_to_beasts(gain);

	// Money
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);

	// Improve girl
	// Base Improvement and trait modifiers
	int xp = 10, libido = 1, skill = 3;
	/* */if (girl->has_trait("Quick Learner"))	{ skill += 1; xp += 3; }
	else if (girl->has_trait("Slow Learner"))	{ skill -= 1; xp -= 3; }
	/* */if (girl->has_trait("Nymphomaniac"))	{ libido += 2; }
	// EXP and Libido
	int I_xp = (g_Dice % xp) + 1;							girl->exp(I_xp);
	int I_libido = (g_Dice % libido) + 1;					girl->upd_temp_stat(STAT_LIBIDO, I_libido);
	if (tired > 0) girl->tiredness(tired);

	// primary improvement (+2 for single or +1 for multiple)
	int I_combat		= (g_Dice % skill) + 1;				girl->combat(I_combat);
	int I_animalh		= (g_Dice % skill) + 1;				girl->animalhandling(I_animalh);
	int I_strength		= (g_Dice % skill) + 1;				girl->strength(I_strength);
	// secondary improvement (-1 for one then -2 for others)
	int I_constitution	= max(0, (g_Dice % skill) - 1);		girl->constitution(I_constitution);
	int I_beastiality	= max(0, (g_Dice % skill) - 2);		girl->beastiality(I_beastiality);
	int I_agility		= max(0, (g_Dice % skill) - 2);		girl->agility(I_agility);
	int I_magic			= max(0, (g_Dice % skill) - 2);		girl->magic(I_magic);

	// Update Enjoyment
	girl->upd_Enjoyment(actiontype, enjoy);

	// Gain Traits
	g_Girls.PossiblyGainNewTrait(girl, "Tough", 30, actiontype, "She has become pretty Tough from all of the fights she's been in.", Day0Night1);
	g_Girls.PossiblyGainNewTrait(girl, "Adventurer", 40, actiontype, "She has been in enough tough spots to consider herself Adventurer.", Day0Night1);
	g_Girls.PossiblyGainNewTrait(girl, "Aggressive", 60, actiontype, "She is getting rather Aggressive from her enjoyment of combat.", Day0Night1);
	if (g_Dice.percent(25) && girl->strength() >= 60 && girl->combat() > girl->magic())
	{
		g_Girls.PossiblyGainNewTrait(girl, "Strong", 60, ACTION_COMBAT, girlName + " has become pretty Strong from all of the fights she's been in.", Day0Night1);
	}

	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Fragile", 15, actiontype, girl->m_Realname + " has had to heal from so many injuries you can't say she is fragile anymore.", Day0Night1);

	if (cfg.debug.log_show_numbers())
	{
		ss << "\n \nNumbers:"
			<< "\n Wages = " << (int)wages
			<< "\n Tips = " << (int)tips
			<< "\n Xp = " << I_xp
			<< "\n Libido = " << I_libido
			<< "\n combat = " << I_combat
			<< "\n animalh = " << I_animalh
			<< "\n strength = " << I_strength
			<< "\n constitution = " << I_constitution
			<< "\n beastiality = " << I_beastiality
			<< "\n agility = " << I_agility
			<< "\n magic = " << I_magic
			<< "\n Tiredness = " << tired
			<< "\n Enjoy " << girl->enjoy_jobs[actiontype] << " = " << enjoy
			;

	}

	girl->m_Events.AddMessage(ss.str(), imagetype, msgtype);

#pragma endregion
	return false;
}

double cJobManager::JP_BeastCapture(sGirl* girl, bool estimate)// not used
{
	double jobperformance = 0.0;

	jobperformance +=
		// primary - first 100
		((girl->animalhandling() + girl->combat() + girl->strength()) / 3) +
		// secondary - second 100
		((girl->beastiality() + girl->constitution() + girl->agility() + girl->magic()) / 4) +
		// level bonus
		girl->level();
	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}

	if (girl->has_trait("Hunter"))				jobperformance += 40;
	if (girl->has_trait("Adventurer"))			jobperformance += 10;
	if (girl->has_trait("Angel"))				jobperformance += 5;
	if (girl->has_trait("Agile"))				jobperformance += 5;
	if (girl->has_trait("Aggressive"))			jobperformance += 5;

	return jobperformance;
}
