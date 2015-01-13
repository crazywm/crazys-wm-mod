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
bool cJobManager::WorkMilk(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	stringstream ss; string girlName = girl->m_Realname;
	cConfig cfg;

	if (Preprocessing(ACTION_WORKMILK, girl, brothel, Day0Night1, summary, ss.str()))	// they refuse to have work
		return true;


	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	int enjoy = 0, work = 0;
	int wages = 0;
	int roll = g_Dice % 100;

	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();

	ss << "She let her breasts be milked.\n\n";




#if 0	// `J` New job function - needs work - commenting out for now
	/*
	100 lactation + preg + notrait = volume should be 20-30oz per day, 140-210oz per week

	*Breast size*			*L*		*P*		*B*		*ml*	*oz*	*pt*			*L* = Lactation
	Flat Chest				100		2		1		200		 6.8	0.4				*P* = Pregnant
	Petite Breasts			100		2		2		400		13.5	0.8				*B* = Breast size multiplier
	Small Boobs				100		2		3		600		20.3	1.3				*ml* = milliliters
	(Normal)				100		2		4		800		27.1	1.7				*oz* = ounces
	Busty Boobs				100		2		5		1000	33.8	2.1				*Pt* = pints
	Big Boobs				100		2		6		1200	40.6	2.5
	Giant Juggs				100		2		7		1400	47.3	3.0
	Massive Melons			100		2		8		1600	54.1	3.4
	Abnormally Large Boobs	100		2		9		1800	60.9	3.8
	Titanic Tits			100		2		10		2000	67.6	4.2

	//*/

	int volume = 0;		// in milliliters
	int breastsize = 4;
	/* */if (g_Girls.HasTrait(girl, "Flat Chest"))				breastsize = 1;
	else if (g_Girls.HasTrait(girl, "Petite Breasts"))			breastsize = 2;
	else if (g_Girls.HasTrait(girl, "Small Boobs"))				breastsize = 3;
	else if (g_Girls.HasTrait(girl, "Busty Boobs"))				breastsize = 5;
	else if (g_Girls.HasTrait(girl, "Big Boobs"))				breastsize = 6;
	else if (g_Girls.HasTrait(girl, "Giant Juggs"))				breastsize = 7;
	else if (g_Girls.HasTrait(girl, "Massive Melons"))			breastsize = 8;
	else if (g_Girls.HasTrait(girl, "Abnormally Large Boobs"))	breastsize = 9;
	else if (g_Girls.HasTrait(girl, "Titanic Tits"))			breastsize = 10;

	// Nipples affects roll_a which is used to adjust the girls enjoyment and damage
	if (g_Girls.HasTrait(girl, "Inverted Nipples"))				roll_a -= 5;
	if (g_Girls.HasTrait(girl, "Puffy Nipples"))				roll_a += 1;
	if (g_Girls.HasTrait(girl, "Perky Nipples"))				roll_a += 2;


	// Milk - not used here yet
	//	if (g_Girls.HasTrait(girl, "Dry Milk"))					
	//	if (g_Girls.HasTrait(girl, "Scarce Lactation"))			
	//	if (g_Girls.HasTrait(girl, "Abundant Lactation"))		
	//	if (g_Girls.HasTrait(girl, "Cow Tits"))					

	volume = girl->lactation()*breastsize;

	if (roll_a <= 10)
	{
		enjoy -= g_Dice % 3 + 1;

	}
	else if (roll_a >= 90)
	{
		enjoy += g_Dice % 3 + 1;
	}
	else
	{
		enjoy += g_Dice % 2;
		ss << "The shift passed uneventfully.";
	}
	ss << "\n\n";












#else	// `J` old job function

	int num_items = 0;
	void AddItem(sInventoryItem* item);
	sInventoryItem* GetItem(string name);

	girl->m_Pay += 15;

	if (g_Girls.HasTrait(girl, "Small Boobs") || g_Girls.HasTrait(girl, "Flat Chest") || g_Girls.HasTrait(girl, "Petite Breasts"))
	{
		if (girl->m_States&(1 << STATUS_PREGNANT) || girl->m_States&(1 << STATUS_PREGNANT_BY_PLAYER))
		{
			ss << girl->m_Realname + " has small breasts, but her body still gives plenty of milk in anticipation of nursing!.";
			girl->m_Pay += 125;
		}
		else
		{
			ss << girl->m_Realname + " has small breasts, which only yield a small amount of milk.";
			girl->m_Pay += 25;
		}
	}
	else if (g_Girls.HasTrait(girl, "Big Boobs") || g_Girls.HasTrait(girl, "Busty Boobs") || g_Girls.HasTrait(girl, "Giant Juggs"))
	{
		if (girl->m_States&(1 << STATUS_PREGNANT) || girl->m_States&(1 << STATUS_PREGNANT_BY_PLAYER))
		{
			ss << girl->m_Realname + "'s already sizable breasts have become fat and swollen with milk in preparation for her child.";
			girl->m_Pay += 135;
		}
		else
		{
			ss << girl->m_Realname + " has large breasts, that yield a good amount of milk to the suction machine even without pregnancy.";
			girl->m_Pay += 35;
		}
	}
	else if (g_Girls.HasTrait(girl, "Abnormally Large Boobs") || g_Girls.HasTrait(girl, "Massive Melons") || g_Girls.HasTrait(girl, "Titanic Tits"))
	{
		if (girl->m_States&(1 << STATUS_PREGNANT) || girl->m_States&(1 << STATUS_PREGNANT_BY_PLAYER))
		{
			ss << girl->m_Realname + " has ridiculously large breasts, even without a baby in development.  With a bun in the oven, her tits are each larger than her head, and leak milk near continuously.";
			girl->m_Pay += 140;
		}
		else
		{
			ss << girl->m_Realname + "'s massive globes don't need pregnancy to yield a profitable quantity of milk!";
			girl->m_Pay += 40;
		}
	}
	else
	{
		if (girl->m_States&(1 << STATUS_PREGNANT) || girl->m_States&(1 << STATUS_PREGNANT_BY_PLAYER))
		{
			ss << girl->m_Realname + " has average sized breasts, which yield a fair amount of milk with the help of pregnancy.";
			girl->m_Pay += 130;
		}
		else
		{
			ss << girl->m_Realname + " has average sized breasts, perfect handfuls, which yield an okay amount of milk.";
			girl->m_Pay += 30;
		}
	}
#endif


	//enjoyed the work or not
	if (roll <= 5)
	{ ss << "\nShe had a bad time letting her breasts be milked."; work -= 1; } //zzzzz FIXME this needs better text
	else if (roll <= 25)
	{ ss << "\nShe had a pleasant time letting her breasts be milked."; work += 3; }
	else
	{ ss << "\nOtherwise, the shift passed uneventfully."; work += 1; }

	g_Girls.UpdateEnjoyment(girl, ACTION_WORKMILK, work, true);

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_MILK, Day0Night1);

	// Improve stats
	int xp = 5, libido = 1, skill = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_FAME, 1);
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, skill);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	return false;
}