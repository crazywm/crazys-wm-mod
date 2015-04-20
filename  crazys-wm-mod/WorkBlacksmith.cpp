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


// `J` Job Arena - Staff
bool cJobManager::WorkBlacksmith(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKMAKEITEMS;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();
	if (g_Girls.DisobeyCheck(girl, actiontype, brothel))			// they refuse to work 
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked as a blacksmith at the arena.\n\n";

	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	int enjoy = 0;
	int wages = 0;
	int tips = 0;
	int imagetype = IMGTYPE_CRAFT;
	int msgtype = Day0Night1;

	double jobperformance = JP_Blacksmith(girl, false);
	double craftpoints = jobperformance;
	
	int dirtyloss = brothel->m_Filthiness / 10;		// craftpoints lost due to repairing equipment
	if (dirtyloss > 0)
	{
		craftpoints -= dirtyloss * 2;
		brothel->m_Filthiness -= dirtyloss;
		ss << "She spent some of her time repairing the Arena's equipment instead of making new stuff.\n";
	}

	if (jobperformance >= 245)
	{
		ss << " She must be the perfect at this.";
		craftpoints *= 1.2;
	}
	else if (jobperformance >= 185)
	{
		ss << " She's unbelievable at this.";
		craftpoints *= 1.1;
	}
	else if (jobperformance >= 145)
	{
		ss << " She's good at this job.";
	}
	else if (jobperformance >= 100)
	{
		ss << " She made a few mistakes but overall she is okay at this.";
		craftpoints *= 0.8;
	}
	else if (jobperformance >= 70)
	{
		ss << " She was nervous and made a few mistakes. She isn't that good at this.";
		craftpoints *= 0.6;
	}
	else
	{
		ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.";
		wages -= 15;
		craftpoints *= 0.4;
	}
	ss << "\n\n";

	int tired = (300 - (int)jobperformance);	// this gets divided in roll_a by (8, 10 or 12) so it will end up around 0-40 tired
	if (roll_a <= 10)
	{
		tired /= 8;
		enjoy -= g_Dice % 3;
		if (roll_b < 30)	// injury
		{
			girl->health(-1 - g_Dice % 5);
			craftpoints *= 0.8;
			if (girl->magic() > 50 && girl->mana() > 20)
			{
				girl->mana(-10 - (g_Dice % 10));
				ss << "While trying to enchant an item, the magic rebounded on her";
			}
			else
				ss << "She burnt herself in the heat of the forge";
			if (girl->health() <= 0)
			{
				ss << " killing her.";
				g_MessageQue.AddToQue(girlName + " was killed in an accident while working as a Blacksmith at the Arena.", COLOR_RED);
				return false;	// not refusing, she is dead
			}
			else ss << ".";
		}
		else if (roll_b < 60)	// fire
		{
			int fire = max(0, g_Dice.bell(-2, 10));
			brothel->m_Filthiness += fire * 2;
			craftpoints -= (craftpoints * (fire * 0.1));
			if (girl->pcfear() > 20) girl->pcfear(fire / 2);	// she is afraid you will get mad at her
			ss << "She accidently started a fire";
			/* */if (fire < 3)	ss << " but it was quickly put out.";
			else if (fire < 6)	ss << " that destroyed several racks of equipment.";
			else if (fire < 10)	ss << " that destroyed most of the equipment she had made.";
			else /*          */	ss << " destroying everything she had made.";

			if (fire > 5) g_MessageQue.AddToQue(girlName + " accidently started a large fire while working as a Blacksmith at the Arena.", COLOR_RED);
		}
		else	// unhappy
		{
			ss << "She did not like working in the arena today.";
			girl->happiness(-(g_Dice % 11));
		}
	}
	else if (roll_a >= 90)
	{
		tired /= 12;
		craftpoints *= 1.1;
		enjoy += g_Dice % 3;
		/* */if (roll_b < 50)	ss << "She kept a steady pace of hammer blows by humming a pleasant tune.";
		else /*            */	ss << "She had a great time working today.";
	}
	else
	{
		tired /= 10;
		enjoy += g_Dice % 2;
		ss << "The shift passed uneventfully.";
	}
	ss << "\n\n";

	// slave girls not being paid for a job that normally you would pay directly for do less work
	if ((girl->is_slave() && !cfg.initial.slave_pay_outofpocket()))
	{
		craftpoints *= 0.9;
		wages = 0;
	}
	else
	{
		wages = int(craftpoints); // `J` Pay her based on how much she made
	}




	// `J` Arena Bookmark - adding in items that can be created in the Arena
#if 1
	int craftpointsuseable = int(craftpoints);
	if (craftpointsuseable > 0)
	{
		int total_made = 0;
		int made_Weapon = 0;
		int made_SmWeapon = 0;
		int made_Armor = 0;
		int made_Misc = 0;
		int made_Armband = 0;
		int made_Helmet = 0;
		int made_Combatshoes = 0;
		int made_Shield = 0;

		string itemmade = "";
		sInventoryItem* item = NULL;
		int nummade = 1;
		int tries = girl->constitution() / 2;

		int magicroll = g_Dice % girl->magic();

		while (craftpointsuseable > 0 && tries > 0)
		{
			tries--;
			item = g_InvManager.GetRandomCraftableItem(girl, JOB_BLACKSMITH, craftpointsuseable);
			if (item)
			{
				if (g_Brothels.AddItemToInventory(item))
				{
					total_made++;
					craftpointsuseable -= item->m_CraftPoints;
					switch (item->m_Type)
					{
					case sInventoryItem::Weapon:		made_Weapon++;		break;
					case sInventoryItem::SmWeapon:		made_SmWeapon++;	break;
					case sInventoryItem::Armor:			made_Armor++;		break;
					case sInventoryItem::Armband:		made_Armband++;		break;
					case sInventoryItem::Helmet:		made_Helmet++;		break;
					case sInventoryItem::Combatshoes:	made_Combatshoes++;	break;
					case sInventoryItem::Shield:		made_Shield++;		break;
					case sInventoryItem::Misc: default:	made_Misc++;		break;
					}
				}
			}
			else craftpointsuseable--;
		}
		if (total_made > 0)
		{
			msgtype = EVENT_GOODNEWS;
			ss << "\n\n" << girlName << " made\n";
			if (made_Weapon > 0)		ss << made_Weapon << " Weapon" << (made_Weapon > 1 ? "s" : "") << "\n";
			if (made_SmWeapon > 0)		ss << made_SmWeapon << " Small Weapon" << (made_SmWeapon > 1 ? "s" : "") << "\n";
			if (made_Armor > 0)			ss << made_Armor << " suit" << (made_Armor > 1 ? "s" : "") << " of Armor\n";
			if (made_Armband > 0)		ss << made_Armband << " Armband" << (made_Armband > 1 ? "s" : "") << "\n";
			if (made_Helmet > 0)		ss << made_Helmet << " Helmet" << (made_Helmet > 1 ? "s" : "") << "\n";
			if (made_Combatshoes > 0)	ss << made_Combatshoes << " Combatshoes\n";
			if (made_Shield > 0)		ss << made_Shield << " Shield" << (made_Shield > 1 ? "s" : "") << "\n";
			if (made_Misc > 0)			ss << made_Misc << " other item" << (made_Misc > 1 ? "s" : "") << "\n";
		}
	}

#endif



	// `J` - Finish the shift - Baker

	// Push out the turn report
	girl->m_Events.AddMessage(ss.str(), imagetype, msgtype);
	if (tired > 0) girl->tiredness(tired);

	// Money
	if (wages < 0)	wages = 0;	girl->m_Pay = wages;
	if (tips < 0)	tips = 0;	girl->m_Tips = tips;

	// Base Improvement and trait modifiers
	int xp = 5, libido = 1, skill = 3;
	/* */if (girl->has_trait("Quick Learner"))	{ skill += 1; xp += 3; }
	else if (girl->has_trait("Slow Learner"))	{ skill -= 1; xp -= 3; }
	/* */if (girl->has_trait("Nymphomaniac"))	{ libido += 2; }
	// EXP and Libido
	g_Girls.UpdateStat(girl, STAT_EXP, (g_Dice % xp) + 1);
	g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, libido);

	// primary improvement (+2 for single or +1 for multiple)
	g_Girls.UpdateSkill(girl, SKILL_CRAFTING,		(g_Dice % skill) + 1);
	g_Girls.UpdateStat(girl, STAT_STRENGTH,			(g_Dice % skill) + 1);
	// secondary improvement (-1 for one then -2 for others)
	g_Girls.UpdateStat(girl, STAT_CONSTITUTION,	max(0, (g_Dice % skill) - 1));
	g_Girls.UpdateSkill(girl, SKILL_COMBAT,		max(0, (g_Dice % skill) - 2));

	// Update Enjoyment
	g_Girls.UpdateEnjoyment(girl, actiontype, enjoy);
	// Gain Traits
	g_Girls.PossiblyGainNewTrait(girl, "Tough", 50, actiontype, "Working in the heat of the forge has made " + girlName + " rather Tough.", Day0Night1);

	return false;
}
double cJobManager::JP_Blacksmith(sGirl* girl, bool estimate)// not used
{
	double jobperformance =
		// primary - first 100
		((girl->crafting() + girl->strength()) / 2) +
		// secondary - second 100
		((girl->constitution() + girl->intelligence() + girl->magic() + girl->combat()) / 4) +
		// level bonus
		girl->level();


	//good traits


	return jobperformance;
}
