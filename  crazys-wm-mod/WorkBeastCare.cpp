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
#include <sstream>
#include "cJobManager.h"
#include "cRng.h"
#include "CLog.h"
#include "cMessageBox.h"
#include "cGold.h"
#include "cBrothel.h"
#include "cInventory.h"

extern cMessageQue g_MessageQue;
extern CLog g_LogFile;
extern cRng g_Dice;
extern cGold g_Gold;
extern cBrothelManager g_Brothels;
extern cInventory g_InvManager;

#pragma endregion

// `J` Job Brothel - General
bool cJobManager::WorkBeastCare(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
#pragma region //	Job setup				//
	int actiontype = ACTION_WORKCARING;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();
	if (roll_a < 50 && g_Girls.DisobeyCheck(girl, actiontype, brothel))
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked taking care of beasts.\n\n";

	if (g_Brothels.m_Beasts < 1)
	{
		ss << "There were no beasts in the brothel to take care of.\n\n";
	}

	g_Girls.UnequipCombat(girl);	// put that shit away

	double wages = 20, tips = 0;
	int enjoy = 0;
	int imagetype = IMGTYPE_FARM;
	int msgtype = Day0Night1;




#pragma endregion
#pragma region //	Job Performance			//

	double jobperformance = JP_BeastCare(girl, false);

	int numhandle = girl->animalhandling() * 2;	// `J` first we assume a girl can take care of 2 beasts per point of animalhandling
	int addbeasts = -1;

	// `J` if she has time to spare after taking care of the current beasts, she may try to get some new ones.
	if (numhandle / 2 > g_Brothels.m_Beasts && g_Dice.percent(50))	// `J` these need more options
	{
		if (girl->magic() > 70 && girl->mana() >= 30)
		{
			addbeasts = (g_Dice % ((girl->mana() / 30) + 1));
			ss << girlName;
			ss << (addbeasts > 0 ? " used" : " tried to use") << " her magic to summon ";
			if (addbeasts < 2) ss << "a beast";
			else ss << addbeasts << " beasts";
			ss << " for the brothel" << (addbeasts > 0 ? "." : " but failed.");
			g_Girls.UpdateSkill(girl, SKILL_MAGIC, addbeasts);
			g_Girls.UpdateStat(girl, STAT_MANA, -30 * max(1, addbeasts));
		}
		else if (girl->animalhandling() > 50 && girl->charisma() > 50)
		{
			addbeasts = 
				g_Dice.percent(girl->combat()) +
				g_Dice.percent(girl->charisma()) +
				g_Dice.percent(girl->animalhandling());
			ss << girlName;
			if (addbeasts <= 0)
			{
				addbeasts = 0;
				ss << " tried to lure in some beasts for the brothel but failed.";
			}
			else
			{
				ss << " lured in ";
				if (addbeasts == 1) ss << "a stray beast";
				else ss << addbeasts << " stray beasts";
				ss << " for the brothel.";
				g_Girls.UpdateStat(girl, STAT_CONFIDENCE, addbeasts);
			}
		}
		else if (girl->combat() > 50 && (g_Girls.HasTrait(girl, "Adventurer") || girl->confidence() > 70))
		{
			addbeasts = (g_Dice % 2);
			ss << girlName << " stood near the entrance to the catacombs, trying to lure out a beast by making noises of an injured animal.\n";
			if (addbeasts > 0) ss << "After some time, a beast came out of the catacombs. " << girlName << " threw a net over it and wrestled it into submission.\n";
			else ss << "After a few hours, she gave up.";
			g_Girls.UpdateSkill(girl, SKILL_COMBAT, addbeasts);
		}
	}
	if (addbeasts >= 0) ss << "\n\n";

#pragma endregion
#pragma region	//	Enjoyment and Tiredness		//
	if (roll_a <= 10)
	{
		enjoy -= g_Dice % 3 + 1;
		addbeasts--;
		ss << "The animals were restless and disobedient.";
	}
	else if (roll_a >= 90)
	{
		enjoy += g_Dice % 3 + 1;
		addbeasts++;
		ss << "She enjoyed her time working with the animals today.";
	}
	else
	{
		enjoy += g_Dice % 2;
		ss << (addbeasts>=0 ? "Otherwise, the" : "The") << " shift passed uneventfully.\n\n";
	}

#pragma endregion
#pragma region	//	Create Items				//

#pragma endregion
#pragma region	//	Money					//

	if (addbeasts < 0)	addbeasts = 0;
	// slave girls not being paid for a job that normally you would pay directly for do less work
	if ((girl->is_slave() && !cfg.initial.slave_pay_outofpocket()))
	{
		wages = 0;
	}
	else
	{
		wages += g_Brothels.m_Beasts/5;
		tips += addbeasts * 5;				// a little bonus for getting new beasts
	}

#pragma endregion
#pragma region	//	Finish the shift			//

	g_Brothels.add_to_beasts(addbeasts);
	girl->m_Events.AddMessage(ss.str(), imagetype, msgtype);

	// Money
	if (wages < 0)	wages = 0;	girl->m_Pay = (int)wages;
	if (tips < 0)	tips = 0;	girl->m_Tips = (int)tips;

	// Improve girl
	int xp = 5 + (g_Brothels.m_Beasts / 10), libido = 1, skill = 2 + (g_Brothels.m_Beasts / 20);

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, max(1, (g_Dice % skill) - 1));
	g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, libido);
	g_Girls.UpdateSkill(girl, SKILL_ANIMALHANDLING, max(1, (g_Dice % skill) + 1));

	g_Girls.UpdateEnjoyment(girl, actiontype, enjoy);
	g_Girls.PossiblyLoseExistingTrait(girl, "Elegant", 40, actiontype, " Working with dirty, smelly beasts has damaged " + girlName + "'s hair, skin and nails making her less Elegant.", Day0Night1);

#pragma endregion
	return false;
}

double cJobManager::JP_BeastCare(sGirl* girl, bool estimate)
{
	double jobperformance = 0.0;
	jobperformance = (g_Girls.GetSkill(girl, SKILL_ANIMALHANDLING) +
		g_Girls.GetStat(girl, STAT_INTELLIGENCE) / 3 +
		g_Girls.GetSkill(girl, SKILL_SERVICE) / 3 +
		g_Girls.GetSkill(girl, SKILL_MAGIC) / 3);

	return jobperformance;
}