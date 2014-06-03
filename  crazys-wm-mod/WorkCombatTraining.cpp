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

bool cJobManager::WorkCombatTraining(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	cTariff tariff;

	string message = "";
	if(Preprocessing(ACTION_COMBAT, girl, brothel, DayNight, summary, message))
		return true;
	stringstream ss;
	ss.str(message);

	double roll_a = g_Dice % 100; //this is used to determine gain amount
	int skill = 0;
	     if (roll_a <= 10){ skill = 6; }
	else if (roll_a <= 20){ skill = 5; }
	else if (roll_a <= 35){ skill = 4; }
	else if (roll_a <= 60){ skill = 3; }
	else                  { skill = 2; }
	if (g_Girls.HasTrait(girl, "Quick Learner"))	{ skill += 1; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; }

	ss << gettext("She trains in combat for the day.\n\n");
		//message = "She trains in combat for the day.\n\n";
	int train = 0;
	int tcom = girl->m_Skills[SKILL_COMBAT];	// train = 1
	int tmag = girl->m_Skills[SKILL_MAGIC];		// train = 2
	int tagi = girl->m_Stats[STAT_AGILITY];		// train = 3
	int tcon = girl->m_Stats[STAT_CONSTITUTION];// train = 4
	double roll_b = g_Dice % 100;
	do{		// `J` New method of selecting what job to do
		     if (roll_b < 30 && tcom < 100)	train = 1;
		else if (roll_b < 60 && tmag < 100)	train = 2;
		else if (roll_b < 80 && tagi < 100)	train = 3;
		else if (tcon < 100)				train = 4;
		roll_b -= 30;
	} while (train == 0 && roll_b > 0);

	if (train == 0)	// no training today
	{
		ss << gettext("There is nothing more she can learn here so she takes the rest of the day off.");
		girl->m_NightJob = girl->m_DayJob = JOB_ARENAREST;
	}

	if (train == 1)
	{
		ss << gettext("She learns how to fight better with her weapons.\n");
		ss << gettext("She managed to gain ") << skill << gettext(" Combat.\n\n");
		g_Girls.UpdateSkill(girl, SKILL_COMBAT, skill);
	}
	if (train == 2)
	{
		ss << gettext("She learns how to cast better magic.\n");
		ss << gettext("She managed to gain ") << skill << gettext(" Magic.\n\n");
		g_Girls.UpdateSkill(girl, SKILL_MAGIC, skill);
	}
	if (train == 3)
	{
		ss << gettext("She worked her speed today.\n\n");
		ss << gettext("She managed to gain ") << skill << gettext(" Agility.\n\n");
		g_Girls.UpdateStat(girl, STAT_AGILITY, skill);
	}
	if (train == 4)
	{
		ss << gettext("She has gotten tougher from the training.\n\n");
		ss << gettext("She managed to gain ") << skill << gettext(" Constituion.\n\n");
		g_Girls.UpdateStat(girl, STAT_CONSTITUTION, skill);
	}




	/*
	else if (roll_b > 80)	// `J` Try to add a trait - 20% chance if all skills are maxed otherwise 5%
	{
		double roll_c = g_Dice % 100;
		if (roll_c < 10 && g_Girls.HasTrait(girl, "Fragile"))
		{
			g_Girls.RemoveTrait(girl, "Fragile");
			ss << "She has had to heal from so many injuries you can't say she is fragile anymore.";
		}
		else if (roll_c < 10 && !g_Girls.HasTrait(girl, "Tough"))
		{
			girl->add_trait("Tough", false);
			ss << "She has become pretty Tough from her training.";
		}
		else if (roll_c < 20 && !g_Girls.HasTrait(girl, "Adventurer"))
		{
			girl->add_trait("Adventurer", false);
			ss << "She has been in enough tough spots to consider herself an Adventurer.";
		}
		else if (roll_c < 20 && !g_Girls.HasTrait(girl, "Aggressive"))
		{
			girl->add_trait("Aggressive", false);
			ss << "She is getting rather Aggressive from her enjoyment of combat.";
		}
		else if (roll_c < 20 && !g_Girls.HasTrait(girl, "Strong"))
		{
			girl->add_trait("Strong", false);
			ss << "She is getting rather Strong from handling heavy weapons and armor.";
		}
		else if (roll_c < 20 && g_Girls.HasTrait(girl, "Nervous"))
		{
			g_Girls.RemoveTrait(girl, "Nervous");
			ss << "She seems to be getting over her Nervousness with her training.";
		}

		/*
		else if (roll_c < 20 && !g_Girls.HasTrait(girl, "ttttttttt"))
		{
			g_Girls.RemoveTrait(girl, "ttttttttt");
			girl->add_trait("ttttttttt", false);
			ss << "ttttttttt";
		}
		
		"Nervous", 20, ACTION_WORKDOCTOR, girl->m_Realname + " seems to finally be getting over her shyness. She's not always so Nervous anymore.", DayNight != 0);

		/*

	&& !g_Girls.HasTrait(girl, "tttttt"))
	{
		g_Girls.RemoveTrait(girl, "tttttt");
		girl->add_trait("tttttt", false);
		ss << "tttttttttttttttttt";
	}
		Assassin
		Fearless
		Fleet of Foot
		Great Figure
		Meek
		Merciless
		Psychic
		Sadistic
		Strong Magic
		
		
		
		*/
	
	
		
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_COMBAT, DayNight);
	girl->m_Pay = 25 + (skill*5); // `J` Pay her more if she learns more

	// Improve stats
	int xp = 5 + skill, libido = int(1 + skill/2);

	if (g_Girls.HasTrait(girl, "Quick Learner"))
	{
		xp += 2;
	}
	else if (g_Girls.HasTrait(girl, "Slow Learner"))
	{
		xp -= 2;
	}
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))
		libido += 2;

	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	return false;
}
