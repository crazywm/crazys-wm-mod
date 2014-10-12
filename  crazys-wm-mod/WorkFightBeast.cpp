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
#include "cArena.h"
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

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cArenaManager g_Arena;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;

// `J` Arena Job - Fighting
bool cJobManager::WorkFightBeast(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = ""; string girlName = girl->m_Realname;

	// ready armor and weapons!
	g_Girls.EquipCombat(girl);

	Uint8 fight_outcome = 0;

	int roll = g_Dice % 100;
	int wages = 175, work = 0;

	if (roll <= 10 && g_Girls.DisobeyCheck(girl, ACTION_COMBAT, brothel))
	{
		message = girlName + " refused to fight beasts today.\n";
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else if (roll <= 15)
	{ message += girlName + " didn't like fighting beasts today.\n\n"; work -= 3; }
	else if (roll >= 90)
	{ message += girlName + " loved fighting beasts today.\n\n"; work += 3; }
	else
	{ message += girlName + " had a pleasant time fighting beats today.\n\n"; work += 1; }


	if (g_Brothels.GetNumBeasts() == 0)
	{
		message = "There are no beasts to fight.";
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, DayNight);
	}
	else
	{
		// TODO need better dialog

		sGirl* tempgirl = g_Girls.CreateRandomGirl(18, false, false, false, true, false);
		if (tempgirl)		// `J` reworked incase there are no Non-Human Random Girls
		{
			fight_outcome = g_Girls.girl_fights_girl(girl, tempgirl);
		}
		else
		{
			g_LogFile.write("Error: You have no Non-Human Random Girls for your girls to fight\n");
			g_LogFile.write("Error: You need a Non-Human Random Girl to allow WorkFightBeast randomness");
			fight_outcome = 7;
		}
		if (fight_outcome == 7)
		{
			message = "The beasts were not cooperating and refused to fight.\n\n";
			message += "(Error: You need a Non-Human Random Girl to allow WorkFightBeast randomness)";
			girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, DayNight);
		}
		else if (fight_outcome == 1)	// she won
		{
			message = "She had fun fighting beasts today."; work += 3;
			girl->m_Events.AddMessage(message, IMGTYPE_COMBAT, DayNight);
			int roll_max = girl->fame() + girl->charisma();
			roll_max /= 4;
			wages += 10 + g_Dice%roll_max;
			girl->m_Pay = wages;
			g_Girls.UpdateStat(girl, STAT_FAME, 2);
		}
		else  // she lost or it was a draw
		{
			message = " She was unable to win the fight."; work -= 1;
			girl->m_Events.AddMessage(message, IMGTYPE_COMBAT, DayNight);
			g_Girls.UpdateStat(girl, STAT_FAME, -1);
		}

		int kills = g_Dice % 6 - 4;		 		// `J` how many beasts she kills 0-2
		if (g_Brothels.GetNumBeasts() < kills)	// or however many there are
			kills = g_Brothels.GetNumBeasts();
		if (kills < 0) kills = 0;				// can't gain any
		g_Brothels.add_to_beasts(kills);

		// Cleanup
		if (tempgirl)
			delete tempgirl;
		tempgirl = 0;
	}

	g_Girls.UpdateEnjoyment(girl, ACTION_COMBAT, work, true);
	// Improve girl
	int fightxp = 1;	if (fight_outcome == 1)	fightxp = 3;
	int xp = 3 * fightxp, libido = 2, skill = 1;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_COMBAT, g_Dice%fightxp + skill);
	g_Girls.UpdateSkill(girl, SKILL_MAGIC, g_Dice%fightxp + skill);
	g_Girls.UpdateStat(girl, STAT_AGILITY, g_Dice%fightxp + skill);
	g_Girls.UpdateStat(girl, STAT_CONSTITUTION, g_Dice%fightxp + skill);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);
	g_Girls.UpdateSkill(girl, SKILL_BEASTIALITY, g_Dice%fightxp * 2 + skill);

	g_Girls.PossiblyGainNewTrait(girl, "Tough", 20, ACTION_COMBAT, "She has become pretty Tough from all of the fights she's been in.", DayNight != 0);
	g_Girls.PossiblyGainNewTrait(girl, "Aggressive", 60, ACTION_COMBAT, "She is getting rather Aggressive from her enjoyment of combat.", DayNight != 0);
	g_Girls.PossiblyGainNewTrait(girl, "Fleet of Foot", 30, ACTION_COMBAT, "She is getting rather fast from all the fighting.", DayNight != 0);

	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Fragile", 75, ACTION_COMBAT, girl->m_Realname + " has had to heal from so many injuries you can't say she is fragile anymore.", DayNight != 0);

	return false;
}
