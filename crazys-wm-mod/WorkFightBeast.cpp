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
extern cJobManager m_JobManager;
extern cPlayer* The_Player;

// `J` Job Arena - Fighting
bool cJobManager::WorkFightBeast(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_COMBAT;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;

	if (g_Brothels.GetNumBeasts() < 1)
	{
		ss << " had no beasts to fight.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);
		return false;	// not refusing
	}
	int roll = g_Dice.d100();
	if (roll <= 10 && girl->disobey_check(actiontype, brothel))
	{
		ss << " refused to fight beasts today.\n";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}

	int found = 0;
	int Armor = -1, Weap1 = -1, Weap2 = -1;
	for (int i = 0; i < MAXNUM_GIRL_INVENTORY && found<girl->m_NumInventory; i++)
		if (girl->m_Inventory[i] != 0)
		{
			found++;
			if (girl->m_Inventory[i]->m_Type == INVWEAPON)
			{
				g_InvManager.Unequip(girl, i);
				if (Weap1 == -1) Weap1 = i;
				else if (Weap2 == -1) Weap2 = i;
				else if (girl->m_Inventory[i]->m_Cost > girl->m_Inventory[Weap1]->m_Cost)
				{
					Weap2 = Weap1;
					Weap1 = i;
				}
				else if (girl->m_Inventory[i]->m_Cost > girl->m_Inventory[Weap2]->m_Cost)
					Weap2 = i;
			}
			if (girl->m_Inventory[i]->m_Type == INVARMOR)
			{
				g_InvManager.Unequip(girl, i);
				if (Armor == -1) Armor = i;
				else if (girl->m_Inventory[i]->m_Cost > girl->m_Inventory[Armor]->m_Cost) Armor = i;
			}
		}

	if (Armor > -1)		g_InvManager.Equip(girl, Armor, false);
	if (Weap1 > -1)		g_InvManager.Equip(girl, Weap1, false);
	if (Weap2 > -1)		g_InvManager.Equip(girl, Weap2, false);

	if (Armor == -1)
	{
		ss << "The crowd can't belive you sent " << girlName << " out to fight without armor";
		if (Weap1 == -1 && Weap2 == -1)
		{
			ss << " or a weapon.";
		}
		else if (Weap1 > -1 || Weap2 > -1)
		{
			ss << ". But atleast she had a weapon.";
		}
		else
		{
			ss << ".";
		}

	}
	else
	{
		ss << girlName << " came out in armor";
		if (Weap1 == -1 && Weap2 == -1)
		{
			ss << " but didn't have a weapon.";
		}
		else
		{
			ss << " and with a weapon in hand. The crowd felt she was ready for battle.";
		}
	}
	ss << "\n \n";


	g_Girls.EquipCombat(girl);	// ready armor and weapons!
	Uint8 fight_outcome = 0;
	int wages = 175, tips = 0, enjoy = 0;
	double jobperformance = JP_FightBeast(girl, false);


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
		ss << "The beasts were not cooperating and refused to fight.\n \n";
		ss << "(Error: You need a Non-Human Random Girl to allow WorkFightBeast randomness)";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);
	}
	else if (fight_outcome == 1)	// she won
	{
		ss << "She won the fight againts beasts today.";//was confusing
		enjoy += 3;
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_COMBAT, Day0Night1);
		int roll_max = girl->fame() + girl->charisma();
		roll_max /= 4;
		wages += 10 + g_Dice%roll_max;
		girl->m_Tips = max(0, tips);
		girl->m_Pay = max(0, wages);
		girl->fame(2);
	}
	else  // she lost or it was a draw
	{
		ss << "She was unable to win the fight.";
		enjoy -= 1;
		//Crazy i feel there needs be more of a bad outcome for losses added this... Maybe could use some more
		if (m_JobManager.is_sex_type_allowed(SKILL_BEASTIALITY, brothel) && !girl->check_virginity())
		{
			ss << " So as punishment you allow the beast to have its way with her."; enjoy -= 1;
			girl->upd_temp_stat(STAT_LIBIDO, -50, true);
			girl->beastiality(2);
			girl->m_Events.AddMessage(ss.str(), IMGTYPE_BEAST, Day0Night1);
			if (!girl->calc_insemination(*g_Girls.GetBeast(), false, 1.0))
			{
				g_MessageQue.AddToQue(girl->m_Realname + " has gotten inseminated", 0);
			}
		}
		else
		{
			ss << " So you send your men in to cage the beast before it can harm her.";
			girl->m_Events.AddMessage(ss.str(), IMGTYPE_COMBAT, Day0Night1);
			girl->fame(-1);
		}
	}

	int kills = g_Dice % 6 - 4;		 		// `J` how many beasts she kills 0-2
	if (g_Brothels.GetNumBeasts() < kills)	// or however many there are
		kills = g_Brothels.GetNumBeasts();
	if (kills < 0) kills = 0;				// can't gain any
	g_Brothels.add_to_beasts(-kills);

	if (girl->is_pregnant())
	{
		if (girl->strength() >= 60)
		{
			ss << "\n \nAll that fighting proved to be quite exhausting for a pregnant girl, even for one as strong as " << girlName << " .\n";
		}
		else
		{
			ss << "\n \nAll that fighting proved to be quite exhausting for a pregnant girl like " << girlName << " .\n";
		}
		girl->tiredness(10 - girl->strength() / 20 );
	}



	if (roll <= 15)
	{
		ss << " didn't like fighting beasts today.";
		enjoy -= 3;
	}
	else if (roll >= 90)
	{
		ss << " loved fighting beasts today.";
		enjoy += 3;
	}
	else
	{
		ss << " had a pleasant time fighting beasts today.";
		enjoy += 1;
	}
	ss << "\n \n";

	// Cleanup
	if (tempgirl) delete tempgirl; tempgirl = 0;


	if ((girl->is_slave() && !cfg.initial.slave_pay_outofpocket()))
	{
		wages = 0;
	}

	int earned = 0;
	for (int i = 0; i < jobperformance; i++)
	{
		earned += g_Dice % 10 + 5; // 5-15 gold per customer  This may need tweaked to get it where it should be for the pay
	}
	brothel->m_Finance.arena_income(earned);
	ss.str("");
	ss << girlName << " drew in " << jobperformance << " people to watch her and you earned " << earned << " from it.";
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);

	girl->upd_Enjoyment(actiontype, enjoy);
	// Improve girl
	int fightxp = (fight_outcome == 1 ? 3 : 1);
	int xp = 3 * fightxp, libido = 2, skill = 1;

	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }

	girl->exp(xp);
	girl->combat(g_Dice%fightxp + skill);
	girl->magic(g_Dice%fightxp + skill);
	girl->agility(g_Dice%fightxp + skill);
	girl->constitution(g_Dice%fightxp + skill);
	girl->upd_temp_stat(STAT_LIBIDO, libido);
	girl->beastiality(g_Dice%fightxp * 2 + skill);

	g_Girls.PossiblyGainNewTrait(girl, "Tough", 20, actiontype, "She has become pretty Tough from all of the fights she's been in.", Day0Night1);
	g_Girls.PossiblyGainNewTrait(girl, "Aggressive", 60, actiontype, "She is getting rather Aggressive from her enjoyment of combat.", Day0Night1);
	g_Girls.PossiblyGainNewTrait(girl, "Fleet of Foot", 30, actiontype, "She is getting rather fast from all the fighting.", Day0Night1);
	if (g_Dice.percent(25) && girl->strength() >= 60 && girl->combat() > girl->magic())
	{
		g_Girls.PossiblyGainNewTrait(girl, "Strong", 60, ACTION_COMBAT, girlName + " has become pretty Strong from all of the fights she's been in.", Day0Night1);
	}

	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Fragile", 75, actiontype, girl->m_Realname + " has had to heal from so many injuries you can't say she is fragile anymore.", Day0Night1);

	return false;
}

double cJobManager::JP_FightBeast(sGirl* girl, bool estimate)// not used
{
	double jobperformance = 0.0;

	if (estimate)// for third detail string
	{
		jobperformance +=
			(girl->fame() / 2) +
			(girl->charisma() / 2) +
			(girl->combat() / 2) +
			(girl->magic() / 2) +
			(girl->level());
	}
	else// for the actual check
	{
		jobperformance += (girl->fame() + girl->charisma()) / 2;
		if (!estimate)
		{
			int t = girl->tiredness() - 80;
			if (t > 0)
				jobperformance -= (t + 2) * (t / 2);
		}

	}
	return jobperformance;
}
