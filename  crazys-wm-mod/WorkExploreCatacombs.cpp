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
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;
extern cJobManager m_JobManager;

// `J` Brothel Job - General
bool cJobManager::WorkExploreCatacombs(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	stringstream ss;
	if (Preprocessing(ACTION_COMBAT, girl, brothel, Day0Night1, summary, ss.str())) return true;
	// determine if they fight any monsters
	if (!g_Dice.percent(max(girl->combat(), girl->magic())))	// WD:	Allow best of Combat or Magic skill 
	{
		girl->m_Events.AddMessage("Nobody wants to play with her today in the catacombs :(", IMGTYPE_COMBAT, Day0Night1);
		return false;
	}

	int num_monsters = max(1,(g_Dice % 6) - 1);
	int type_monster_girls = 0;
	int type_unique_monster_girls = 0;
	int type_beasts = 0;
	int num_items = 0;
	long gold = 0;
	bool raped = false;

	g_Girls.EquipCombat(girl);	// ready armor and weapons!

	// fight/capture monsters here
	for (int i = num_monsters; i > 0; i--)
	{
		bool getagirl = g_Dice.percent(55);	// 0 is beast, 1 is girl (human or non-human)

		sGirl* tempgirl = g_Girls.CreateRandomGirl(18, false, false, false, true, false, false);
		Uint8 fight_outcome = 0;
		if (tempgirl)		// `J` reworked incase there are no Non-Human Random Girls
		{
			fight_outcome = g_Girls.girl_fights_girl(girl, tempgirl);
		}
		else // `J` this should have been corrected with the addition of the default random girl but leaving it in just in case.
		{
			g_LogFile.write("Error: You have no Non-Human Random Girls for your girls to fight\n");
			g_LogFile.write("Error: You need a Non-Human Random Girl to allow WorkExploreCatacombs randomness");
			fight_outcome = 7;
		}
		if (fight_outcome == 7)
		{
			if (g_Dice%girl->get_skill(SKILL_COMBAT) < 5) raped = true;
			else
			{
				ss << "She came back with one animal today.\n\n";
				ss << "(Error: You need a Non-Human Random Girl to allow WorkExploreCatacombs randomness)";
				girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);
				g_Brothels.add_to_beasts(1);
				type_beasts++;
			}
		}
		else if (fight_outcome == 1)  // If she won
		{
			if (getagirl)  // Catacombs girl type
			{				// NOTE that defeating tempgirl and capturing unrelated girl is
				// the only way to ever capture rare girls like those with incoporeal trait.
				// Some rationilization could be done, but is probably not necessary. DustyDan
				sGirl* ugirl = 0;
				bool unique = g_Dice.percent(50);	// chance of getting unique girl

				if (unique)  // Unique monster girl type
				{
					ugirl = g_Girls.GetRandomGirl(false, true);
					if (ugirl == 0) unique = false;
				}
				if (g_Brothels.GetObjective())
				{
					if (g_Brothels.GetObjective()->m_Objective == OBJECTIVE_CAPTUREXCATACOMBGIRLS)
						g_Brothels.GetObjective()->m_SoFar++;
				}

				// Either type of girl goes to the dungeon
				if (unique)
				{
					ugirl->m_States &= ~(1 << STATUS_CATACOMBS);
					g_Brothels.GetDungeon()->AddGirl(ugirl, DUNGEON_GIRLCAPTURED);
					type_unique_monster_girls++;
				}
				else
				{
					ugirl = g_Girls.CreateRandomGirl(0, false, false, true, true);
					if (ugirl != 0)  // make sure a girl was returned
					{
						g_Brothels.GetDungeon()->AddGirl(ugirl, DUNGEON_GIRLCAPTURED);
						type_monster_girls++;
					}
				}
			}
			else  // Beast type
			{
				g_Brothels.add_to_beasts(1);
				type_beasts++;
			}
		}
		else if (fight_outcome == 2)  // she lost
		{
			raped = true;
		}
		else if (fight_outcome == 0)  // it was a draw
		{
			// hmm, guess we'll just ignore draws for now
		}

		if (tempgirl) delete tempgirl; tempgirl = 0;
		if (raped) break;
	} // # of monsters to fight loop

	if (raped)
	{
		ss.str("");
		int NumMon = g_Dice % 6 + 1;
		ss << girl->m_Realname << " was defeated then" << ((NumMon <= 3) ? "" : " gang") << " raped and abused by " << NumMon << " monsters.";
		int health = -NumMon, happy = -NumMon * 5, spirit = -NumMon, sex = -NumMon * 2, combat = -NumMon * 2, injury = 9 + NumMon;

		if (g_Girls.CheckVirginity(girl))
		{
			ss << " That's a hell of a way to lose your virginity; naturally, she's rather distressed by this fact.";
			g_Girls.LoseVirginity(girl);					// Virginity both attrib & trait now, 04/15/13
			health -= 1, happy -= 10, spirit -= 2, sex -= 2, combat -= 2, injury += 2;
		}
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_DEATH, EVENT_DANGER);

		if (!girl->calc_insemination(g_Brothels.GetPlayer(), false, 1.0 + (NumMon * 0.5)))
		{
			g_MessageQue.AddToQue(girl->m_Realname + " has gotten inseminated", 0);
			health -= 1, happy -= 10, spirit -= 4, sex -= 4, combat -= 2, injury += 2;
		}

		g_Girls.UpdateStat(girl, STAT_HEALTH, health);
		g_Girls.UpdateStat(girl, STAT_HAPPINESS, happy);
		g_Girls.UpdateStat(girl, STAT_SPIRIT, spirit);
		g_Girls.GirlInjured(girl, injury);
		g_Girls.UpdateEnjoyment(girl, ACTION_SEX, sex, true);
		g_Girls.UpdateEnjoyment(girl, ACTION_COMBAT, combat, true);

		return false;
	}

	int ItemPlace = 0;  // Place in 0..299

	string item_list = "";
	for (int i = num_monsters; i > 0; i--)
	{
		gold += g_Dice % 150;

		// get any items
		while (g_Dice.percent(40))
		{
			sInventoryItem* TempItem = g_InvManager.GetRandomItem();

			// 1. If the item type already exists in the brothel, and there isn't already 999 of it,
			// add one to it
			ItemPlace = g_Brothels.HasItem(TempItem->m_Name, -1);
			if ((ItemPlace != -1) && (g_Brothels.m_NumItem[ItemPlace] <= 999))
			{
				item_list += ((item_list == "") ? "" : ",\n") + TempItem->m_Name;
				g_Brothels.m_NumItem[ItemPlace]++;
				num_items++;
			}
			// 2. If there are less than MAXNUM_INVENTORY different things in the inventory and the item type
			// isn't in the inventory already, add the item
			if (g_Brothels.m_NumInventory < MAXNUM_INVENTORY && ItemPlace == -1)
			{
				for (int j = 0; j < MAXNUM_INVENTORY; j++)
				{
					if (g_Brothels.m_Inventory[j] == 0) // Empty slot
					{
						item_list += ((item_list == "") ? "" : ",\n") + TempItem->m_Name;
						g_Brothels.m_Inventory[j] = TempItem;
						g_Brothels.m_EquipedItems[j] = 0;
						g_Brothels.m_NumInventory++;
						g_Brothels.m_NumItem[j]++;  // = 1 instead?

						num_items++;
						break;
					}
				}
			}
		}
	}

	ss << girl->m_Realname << " adventured in the catacombs.\n\n";
	if (num_monsters > 0)
	{
		ss << "She encountered " << num_monsters << " monster" << (num_monsters > 1 ? "s" : "") << " and captured:\n";
		if (type_monster_girls > 0)
		{
			ss << type_monster_girls << " catacomb girl" << (type_monster_girls > 1 ? "s" : "");
			if (type_unique_monster_girls + type_beasts == 0)
				ss << ".";
			else if (type_unique_monster_girls > 0 && type_beasts > 0)
			ss << ",\n";
			else
				ss << " and\n";
		}
		if (type_unique_monster_girls > 0)
		{
			ss << type_unique_monster_girls << " unique girl" << (type_unique_monster_girls > 1 ? "s" : "");
			if (type_beasts == 0)
				ss << ".";
			else
				ss << " and\n";
		}
		if (type_beasts > 0) 
			ss << type_beasts << " beast" << (type_beasts > 1 ? "s." : ".");
		ss << "\n\n";
	}
	if (num_items > 0)
	{
		ss << (num_monsters > 0 ? "Further, she" : "She") << " came out with ";
		if (num_items == 1) ss << "one ";
		else	ss << num_items << " items:\n";
		ss << item_list << ".\n\n";
	}
	if (gold > 0) ss << "She " << (num_monsters + num_items > 0 ? "also " : "") << "came out with " << gold << " gold.";

	if (num_monsters + num_items + gold < 1) ss << "She came out empty handed.";

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_COMBAT, Day0Night1);

	ss.str("");
	if (girl->get_stat(STAT_LIBIDO) > 90 && type_monster_girls + type_unique_monster_girls > 0 && m_JobManager.is_sex_type_allowed(SKILL_LESBIAN, brothel))
	{
		ss << girl->m_Realname << " was real horny so she had a little fun with the girl";
		if (type_monster_girls + type_unique_monster_girls > 1) ss << "s";
		ss << " she captured.";
		g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -50);
		g_Girls.UpdateSkill(girl, SKILL_LESBIAN, type_monster_girls + type_unique_monster_girls);
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_LESBIAN, Day0Night1);
	}
	else if (girl->get_stat(STAT_LIBIDO) > 90 && type_beasts > 0 && m_JobManager.is_sex_type_allowed(SKILL_BEASTIALITY, brothel))
	{
		ss << girl->m_Realname << " was real horny so she had a little fun with the beast";
		if (type_beasts > 1) ss << "s";
		ss << " she captured.";
		g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -50);
		g_Girls.UpdateSkill(girl, SKILL_BEASTIALITY, type_beasts);
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_BEAST, Day0Night1);
		if (!girl->calc_insemination(g_Brothels.GetPlayer(), false, 1.0))
		{
			g_MessageQue.AddToQue(girl->m_Realname + " has gotten inseminated", 0);
		}
	}

	girl->m_Pay += gold;

	// Improve girl
	int num = type_monster_girls + type_unique_monster_girls + type_beasts + 1;
	int xp = 5 * num, libido = 5, skill = num;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 5; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 5; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			libido += 2;
	if (g_Girls.HasTrait(girl, "Lesbian"))				libido += type_monster_girls + type_unique_monster_girls;

	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_COMBAT, (g_Dice % skill)+1);
	g_Girls.UpdateSkill(girl, SKILL_MAGIC,  (g_Dice % skill)+1);
	g_Girls.UpdateStat(girl, STAT_AGILITY, g_Dice % skill);
	g_Girls.UpdateStat(girl, STAT_CONSTITUTION, g_Dice % skill);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);
	g_Girls.UpdateEnjoyment(girl, ACTION_COMBAT, (g_Dice % skill)+2, true);

	// Myr: Turned trait gains into functions
	g_Girls.PossiblyGainNewTrait(girl, "Tough", 30, ACTION_COMBAT, "She has become pretty Tough from all of the fights she's been in.", Day0Night1 == SHIFT_NIGHT);
	g_Girls.PossiblyGainNewTrait(girl, "Adventurer", 40, ACTION_COMBAT, "She has been in enough tough spots to consider herself an Adventurer.", Day0Night1 == SHIFT_NIGHT);
	g_Girls.PossiblyGainNewTrait(girl, "Aggressive", 60, ACTION_COMBAT, "She is getting rather Aggressive from her enjoyment of combat.", Day0Night1 == SHIFT_NIGHT);

	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Fragile", 75, ACTION_COMBAT, girl->m_Realname + " has had to heal from so many injuries you can't say she is fragile anymore.", Day0Night1 == SHIFT_NIGHT);

	return false;
}