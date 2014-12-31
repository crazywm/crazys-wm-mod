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




// `J` Farm Job - Laborers
bool cJobManager::WorkBeastCapture(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	string message = "";
	string girlName = girl->m_Realname; //for use in dialogue
	if (Preprocessing(ACTION_COMBAT, girl, brothel, Day0Night1, summary, message))
		return true;

	// ready armor and weapons!
	g_Girls.EquipCombat(girl);

	// TODO need better dialog - SIN: DONE
	message += girlName + " equipped herself and went out to hunt for exotic beasts and animals.\n";

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
		message += "She came back with just one animal today.\n\n";
		message += "(Error: You need a Non-Human Random Girl to allow WorkBeastCapture randomness)";
		gain = 1;
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, Day0Night1);
	}
	else if (fight_outcome == 1)	// she won
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_COMBAT, +3, true);
		message += "She had fun hunting today and came back with ";
		if (gain <= 2)	{ message += "two";  gain = 2; }
		else if (gain == 3)	{ message += "three"; }
		else if (gain == 4)	{ message += "four"; }
		else   { gain = 5;    message += "five"; } // shouldn't happen but just in case
		message += " new beasts for you.\n";
		girl->m_Events.AddMessage(message, IMGTYPE_COMBAT, Day0Night1);
	}
	else		// she lost or it was a draw
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_COMBAT, -1, true);
		message += " The animals were difficult to track today. " + girlName + " eventually returned worn out and frustrated, dragging one captured beast behind her.\n";
		girl->m_Events.AddMessage(message, IMGTYPE_COMBAT, Day0Night1);
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
		message += girlName + " has a way with animals. Another beast freely follows her back.\n";
		gain++;
	}
	//SIN: most the rest rely on more than one cap so might as well skip the lot if less than this...
	if (gain > 1){
		if (g_Girls.HasTrait(girl, "Twisted") && g_Girls.HasTrait(girl, "Nymphomaniac") && (g_Girls.GetStat(girl, STAT_LIBIDO) >= 80))
		{
			message += "Being a horny, twisted nymphomaniac, " + girlName + " had some fun with the beasts before she handed them over.\n";
			g_Girls.UpdateSkill(girl, SKILL_BEASTIALITY, g_Dice % gain);
			g_Girls.UpdateStat(girl, STAT_LIBIDO, -(g_Dice % gain));
		}
		if (g_Girls.HasTrait(girl, "Psychic") && (g_Girls.GetStat(girl, STAT_LIBIDO) >= 90) && g_Dice.percent(10))
		{
			message += girlName + "'s Psychic sensitivity caused her mind be overwhelmed by the creatures' lusts";
			if (g_Girls.CheckVirginity(girl))
			{
				message += " but, things were moving too fast and she regained control before they could take her virginity.\n";
				g_Girls.UpdateSkill(girl, SKILL_BEASTIALITY, 1);
				g_Girls.UpdateStat(girl, STAT_LIBIDO, 2); //no satisfaction!
			}
			else
			{
				message += ". Many hours later she staggered in to present the creatures to you.\n";
				g_Girls.UpdateSkill(girl, SKILL_BEASTIALITY, g_Dice % gain);
				g_Girls.UpdateStat(girl, STAT_LIBIDO, -2 * (g_Dice % gain));
				g_Girls.UpdateStat(girl, STAT_TIREDNESS, gain);
			}
		}
		if (g_Girls.HasTrait(girl, "Assassin") && g_Dice.percent(5))
		{
			message += " One of the captured creatures tried to escape on the way back. Trained assassin, " +
				girlName + ", instantly killed it as an example to the others.\n";
			g_Girls.UpdateSkill(girl, SKILL_COMBAT, 1);
			gain--;
		}
		if (g_Girls.GetStat(girl, STAT_TIREDNESS) > 50 && g_Dice.percent(5))
		{
			message += girlName + " was so exhausted she couldn't concentrate. One of the creatures escaped.";
			gain--;
		}
	}
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

	g_Girls.PossiblyGainNewTrait(girl, "Tough", 30, ACTION_COMBAT, "She has become pretty Tough from all of the fights she's been in.", Day0Night1 == SHIFT_NIGHT);
	g_Girls.PossiblyGainNewTrait(girl, "Adventurer", 40, ACTION_COMBAT, "She has been in enough tough spots to consider herself Adventurer.", Day0Night1 == SHIFT_NIGHT);
	g_Girls.PossiblyGainNewTrait(girl, "Aggressive", 60, ACTION_COMBAT, "She is getting rather Aggressive from her enjoyment of combat.", Day0Night1 == SHIFT_NIGHT);

	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Fragile", 15, ACTION_COMBAT, girl->m_Realname + " has had to heal from so many injuries you can't say she is fragile anymore.", Day0Night1 == SHIFT_NIGHT);

	return false;
}