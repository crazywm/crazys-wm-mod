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

// `J` Farm Job - Laborers
bool cJobManager::WorkBeastCapture(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_COMBAT;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	if (g_Girls.DisobeyCheck(girl, actiontype, brothel))			// they refuse to work 
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " equipped herself and went out to hunt for exotic beasts and animals.\n\n";

	g_Girls.EquipCombat(girl);	// ready armor and weapons!

	int gain = g_Dice % 2 + 2;
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
	if (fight_outcome == 7)
	{
		ss << "She came back with just one animal today.\n\n";
		ss << "(Error: You need a Non-Human Random Girl to allow WorkBeastCapture randomness)";
		gain = 1;
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);
	}
	else if (fight_outcome == 1)	// she won
	{
		g_Girls.UpdateEnjoyment(girl, actiontype, +3, true);
		ss << "She had fun hunting today and came back with ";
		if (gain <= 2)	{ ss << "two";  gain = 2; }
		else if (gain == 3)	{ ss << "three"; }
		else if (gain == 4)	{ ss << "four"; }
		else   { gain = 5;    ss << "five"; } // shouldn't happen but just in case
		ss << " new beasts for you.\n";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_COMBAT, Day0Night1);
	}
	else		// she lost or it was a draw
	{
		g_Girls.UpdateEnjoyment(girl, actiontype, -1, true);
		ss << " The animals were difficult to track today. " << girlName << " eventually returned worn out and frustrated, dragging one captured beast behind her.\n";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_COMBAT, Day0Night1);
		gain = 1;
	}
	g_Brothels.add_to_beasts(gain);

	// Cleanup
	if (tempgirl)
		delete tempgirl;
	tempgirl = 0;

	//SIN: A little randomness
	if (((g_Girls.GetSkill(girl, SKILL_ANIMALHANDLING) + g_Girls.GetSkill(girl, SKILL_ANIMALHANDLING)) > 125) && g_Dice.percent(30))
	{
		ss << girlName << " has a way with animals. Another beast freely follows her back.\n";
		gain++;
	}
	//SIN: most the rest rely on more than one cap so might as well skip the lot if less than this...
	if (gain > 1){
		if (g_Girls.HasTrait(girl, "Twisted") && g_Girls.HasTrait(girl, "Nymphomaniac") && (g_Girls.GetStat(girl, STAT_LIBIDO) >= 80))
		{
			ss << "Being a horny, twisted nymphomaniac, " << girlName << " had some fun with the beasts before she handed them over.\n";
			g_Girls.UpdateSkill(girl, SKILL_BEASTIALITY, g_Dice % gain);
			g_Girls.UpdateStat(girl, STAT_LIBIDO, -(g_Dice % gain));
		}
		if (g_Girls.HasTrait(girl, "Psychic") && (g_Girls.GetStat(girl, STAT_LIBIDO) >= 90) && g_Dice.percent(10))
		{
			ss << girlName << "'s Psychic sensitivity caused her mind be overwhelmed by the creatures' lusts";
			if (g_Girls.CheckVirginity(girl))
			{
				ss << " but, things were moving too fast and she regained control before they could take her virginity.\n";
				g_Girls.UpdateSkill(girl, SKILL_BEASTIALITY, 1);
				g_Girls.UpdateStat(girl, STAT_LIBIDO, 2); //no satisfaction!
			}
			else
			{
				ss << ". Many hours later she staggered in to present the creatures to you.\n";
				g_Girls.UpdateSkill(girl, SKILL_BEASTIALITY, g_Dice % gain);
				g_Girls.UpdateStat(girl, STAT_LIBIDO, -2 * (g_Dice % gain));
				g_Girls.UpdateStat(girl, STAT_TIREDNESS, gain);
			}
		}
		if (g_Girls.HasTrait(girl, "Assassin") && g_Dice.percent(5))
		{
			ss << " One of the captured creatures tried to escape on the way back. Trained assassin, " << girlName << ", instantly killed it as an example to the others.\n";
			g_Girls.UpdateSkill(girl, SKILL_COMBAT, 1);
			gain--;
		}
		if (g_Girls.GetStat(girl, STAT_TIREDNESS) > 50 && g_Dice.percent(5))
		{
			ss << girlName << " was so exhausted she couldn't concentrate. One of the creatures escaped.";
			gain--;
		}
	}



	// `J` Farm Bookmark - adding in items that can be gathered in the farm
#if 1

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
		else if (chooseitem < 98)
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













#endif







	// Improve girl
	int xp = 8, libido = 2, skill = 1;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	girl->m_Pay += 50 + (gain * 10);	// you catch more you get paid more
	g_Gold.staff_wages(50 + (gain * 10));	// wages come from you
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_COMBAT, g_Dice % gain + skill);
	g_Girls.UpdateSkill(girl, SKILL_MAGIC, g_Dice % gain + skill);
	g_Girls.UpdateStat(girl, STAT_AGILITY, g_Dice % gain + skill);
	g_Girls.UpdateStat(girl, STAT_CONSTITUTION, g_Dice % 2 + skill);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);
	g_Girls.UpdateSkill(girl, SKILL_BEASTIALITY, gain + skill);

	g_Girls.PossiblyGainNewTrait(girl, "Tough", 30, actiontype, "She has become pretty Tough from all of the fights she's been in.", Day0Night1);
	g_Girls.PossiblyGainNewTrait(girl, "Adventurer", 40, actiontype, "She has been in enough tough spots to consider herself Adventurer.", Day0Night1);
	g_Girls.PossiblyGainNewTrait(girl, "Aggressive", 60, actiontype, "She is getting rather Aggressive from her enjoyment of combat.", Day0Night1);

	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Fragile", 15, actiontype, girl->m_Realname + " has had to heal from so many injuries you can't say she is fragile anymore.", Day0Night1);

	return false;
}
double cJobManager::JP_BeastCapture(sGirl* girl, bool estimate)// not used
{
	double jobperformance = 0.0;
	if (estimate)// for third detail string
	{
	}
	else// for the actual check
	{
	}
	return jobperformance;
}
