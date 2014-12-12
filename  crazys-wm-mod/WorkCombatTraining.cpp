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
#include "cGold.h"
#include "cGangs.h"
#include "cMessageBox.h"
#include "libintl.h"

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
bool cJobManager::WorkCombatTraining(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{

	string message = "";
	if (Preprocessing(ACTION_COMBAT, girl, brothel, Day0Night1, summary, message)) return true;
	cConfig cfg;
	stringstream ss;
	ss.str(message);
	int wages = 0;
	int dirtyloss = 0;
	int roll_a, roll_b, roll_c;

	ss << gettext("She trains in combat for the day.\n\n");

	roll_a = g_Dice.d100(); //this is used to determine gain amount
	int skill = 0;
	/* */if (roll_a <= 10)	{ skill = 6; }
	else if (roll_a <= 20)	{ skill = 5; }
	else if (roll_a <= 35)	{ skill = 4; }
	else if (roll_a <= 60)	{ skill = 3; }
	else /*             */	{ skill = 2; }
	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; }

	dirtyloss = brothel->m_Filthiness / 100;
	skill -= dirtyloss;
	ss << "The Arena is ";
	if (dirtyloss <= 0) ss << "clean and tidy";
	if (dirtyloss == 1) ss << "dirty and the equipment has not been put back in its place";
	if (dirtyloss == 2) ss << "messy. The equipment is damaged and strewn about the building";
	if (dirtyloss == 3) ss << "filthy and some of the equipment is broken";
	if (dirtyloss >= 4) ss << "in complete disarray and the equipment barely usable";
	ss << ".\n\n";
	if (skill < 1) skill = 1;

	int train = 0;
	int tcom = girl->m_Skills[SKILL_COMBAT];		// train = 1
	int tmag = girl->m_Skills[SKILL_MAGIC];			// train = 2
	int tagi = girl->m_Stats[STAT_AGILITY];			// train = 3
	int tcon = girl->m_Stats[STAT_CONSTITUTION];	// train = 4

	if (tcom + tmag + tagi + tcon >= 400)
	{
		ss << gettext("There is nothing more she can learn here so she takes the rest of the day off.");
		girl->m_NightJob = girl->m_DayJob = JOB_ARENAREST;
		return false;
	}
	else
	{
		roll_b = g_Dice.d100();
		do{		// `J` New method of selecting what job to do
			/* */if (roll_b < 30 && tcom < 100)	train = 1;
			else if (roll_b < 60 && tmag < 100)	train = 2;
			else if (roll_b < 80 && tagi < 100)	train = 3;
			else if /*          */ (tcon < 100)	train = 4;
			roll_b -= 30;
		} while (train == 0 && roll_b > 0);
		if (train == 0) train = g_Dice % 4 + 1;
	}


	if (train == 1)
	{
		ss << gettext("She learns how to fight better with her weapons.\n");
		skill = min(skill, 100 - girl->m_Skills[SKILL_COMBAT]);
		if (skill > 0)
		{
			ss << gettext("She managed to gain ") << skill << gettext(" Combat.\n\n");
			g_Girls.UpdateSkill(girl, SKILL_COMBAT, skill);
		}
		else ss << "She was unable to learn anything new.\n\n";
	}
	if (train == 2)
	{
		ss << gettext("She learns how to cast better magic.\n");
		skill = min(skill, 100 - girl->m_Skills[SKILL_MAGIC]);
		if (skill > 0)
		{
			ss << gettext("She managed to gain ") << skill << gettext(" Magic.\n\n");
			g_Girls.UpdateSkill(girl, SKILL_MAGIC, skill);
		}
		else ss << "She was unable to learn anything new.\n\n";
	}
	if (train == 3)
	{
		ss << gettext("She worked her speed today.\n\n");
		skill = min(skill, 100 - girl->m_Skills[STAT_AGILITY]);
		if (skill > 0)
		{
			ss << gettext("She managed to gain ") << skill << gettext(" Agility.\n\n");
			g_Girls.UpdateStat(girl, STAT_AGILITY, skill);
		}
		else ss << "She was unable to learn anything new.\n\n";
	}
	if (train == 4)
	{
		ss << gettext("She has gotten tougher from the training.\n\n");
		skill = min(skill, 100 - girl->m_Skills[STAT_CONSTITUTION]);
		if (skill > 0)
		{
			ss << gettext("She managed to gain ") << skill << gettext(" Constituion.\n\n");
			g_Girls.UpdateStat(girl, STAT_CONSTITUTION, skill);
		}
		else ss << "She was unable to learn anything new.\n\n";
	}




	// `J` Try to add a trait 
	roll_c = g_Dice%50;		// 2% per trait group chance
	switch (roll_c)
	{
	case 0:
		if (g_Girls.HasTrait(girl, "Fragile"))
		{
			g_Girls.RemoveTrait(girl, "Fragile");
			ss << "She has had to heal from so many injuries you can't say she is fragile anymore.";
		}
		else if (!g_Girls.HasTrait(girl, "Tough"))
		{
			girl->add_trait("Tough", false);
			ss << "She has become pretty Tough from her training.";
		}
		break;
	case 1:
		if (!g_Girls.HasTrait(girl, "Adventurer"))
		{
			girl->add_trait("Adventurer", false);
			ss << "She has been in enough tough spots to consider herself an Adventurer.";
		}
		break;
	case 2:
		if (g_Girls.HasTrait(girl, "Nervous") || g_Girls.HasTrait(girl, "Meek") || g_Girls.HasTrait(girl, "Dependant"))
		{
			if (g_Girls.HasTrait(girl, "Nervous"))
			{
				g_Girls.RemoveTrait(girl, "Nervous");
				ss << "She seems to be getting over her Nervousness with her training.";
			}
			else if (g_Girls.HasTrait(girl, "Meek"))
			{
				g_Girls.RemoveTrait(girl, "Meek");
				ss << "She seems to be getting over her Meakness with her training.";
			}
			else if (g_Girls.HasTrait(girl, "Dependant"))
			{
				g_Girls.RemoveTrait(girl, "Dependant");
				ss << "She seems to be getting over her Dependancy with her training.";
			}
		}
		else
		{
			if (!g_Girls.HasTrait(girl, "Aggressive"))
			{
				girl->add_trait("Aggressive", false);
				ss << "She is getting rather Aggressive from her enjoyment of combat.";
			}
			else if (!g_Girls.HasTrait(girl, "Fearless"))
			{
				girl->add_trait("Fearless", false);
				ss << "She is getting rather Fearless from her enjoyment of combat.";
			}
			else if (!g_Girls.HasTrait(girl, "Audacity"))
			{
				girl->add_trait("Audacity", false);
				ss << "She is getting rather Audacious from her enjoyment of combat.";
			}
		}
		break;
	case 3:
		if (!g_Girls.HasTrait(girl, "Strong"))
		{
			girl->add_trait("Strong", false);
			ss << "She is getting rather Strong from handling heavy weapons and armor.";
		}
		break;
	case 4:
		break;
	case 5:

		break;
	case 6:

		break;

	default:	break;	// no trait gained
	}



	/*
	else if (roll_c < 20 && !g_Girls.HasTrait(girl, "ttttttttt"))
	{
	g_Girls.RemoveTrait(girl, "ttttttttt");
	girl->add_trait("ttttttttt", false);
	ss << "ttttttttt";
	}

	"Nervous", 20, ACTION_WORKDOCTOR, girl->m_Realname + " seems to finally be getting over her shyness. She's not always so Nervous anymore.", Day0Night1 == SHIFT_NIGHT);

	/*

	&& !g_Girls.HasTrait(girl, "tttttt"))
	{
	g_Girls.RemoveTrait(girl, "tttttt");
	girl->add_trait("tttttt", false);
	ss << "tttttttttttttttttt";
	}
	
Small Scars
Cool Scars
Horrific Scars
Bruises
Idol
Agile
Fleet of Foot
Clumsy
Strong
Merciless
Delicate
Brawler
Assassin
Masochist
Sadistic
Tsundere
Twisted
Yandere


Missing Nipple

Muggle
Weak Magic
Strong Magic
Powerful Magic

Broken Will
Iron Will

Eye Patch
One Eye

Shy
Missing Teeth


No Arms
One Arm
No Hands
One Hand
Missing Finger
Missing Fingers

No Feet
No Legs
One Foot
One Leg
Missing Toe
Missing Toes


Muscular
Plump
Great Figure



	*/

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_COMBAT, Day0Night1);

	brothel->m_Filthiness += 2;	// fighting is dirty

	wages = 50 + (skill * 5); // `J` Pay her more if she learns more
	if (girl->is_slave() && !cfg.initial.slave_pay_outofpocket()) wages = 0;	// You own her so you don't have to pay her.
	girl->m_Pay = wages;

	// Improve stats
	int xp = 5 + skill, libido = int(1 + skill / 2);

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ xp += 2; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ xp -= 2; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	return false;
}
