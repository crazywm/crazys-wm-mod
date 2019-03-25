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
#include <sstream>
#include "cJobManager.h"
#include "cBrothel.h"

#include "cCustomers.h"
#include "cRng.h"
#include "cInventory.h"
#include "sConfig.h"
#include "cRival.h"
#include "CLog.h"
#include "cTrainable.h"
#include "cTariff.h"
#include "cGold.h"
#include "cBrothel.h"
#include "cGangs.h"
#include "cMessageBox.h"
#include "libintl.h"

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;

extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;

// `J` Job Brothel - General - Matron_Job - Full_Time_Job
bool cJobManager::WorkMatron(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKMATRON;
	girl->m_DayJob = girl->m_NightJob = JOB_MATRON;	// it is a full time job
	if (Day0Night1) return false;	// and is only checked once

	stringstream ss; string girlName = girl->m_Realname; ss << "Matron  " << girlName << "  ";

	// `J` zzzzzz - this needs to be updated for building flow
	if (girl->disobey_check(actiontype, brothel))
	{
		ss << "refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}



	int numgirls = brothel->m_NumGirls;
	int wages = 0, tips = 0;
	int enjoy = 0;
	int conf = 0;
	int happy = 0;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100();
	int imagetype = IMGTYPE_PROFILE;

	// Complications
	int check = g_Dice.d100();
	if (check < 10 && numgirls >(girl->service() + girl->confidence()) * 3)
	{
		enjoy -= (g_Dice % 6 + 5);
		conf -= 5; happy -= 10;
		ss << "was overwhelmed by the number of girls she was required to manage and broke down crying.";
	}
	else if (check < 10)
	{
		enjoy -= (g_Dice % 3 + 1);
		conf -= -1; happy -= -3;
		ss << "had trouble dealing with some of the girls.";
	}
	else if (check > 90)
	{
		enjoy += (g_Dice % 3 + 1);
		conf += 1; happy += 3;
		ss << "enjoyed helping the girls with their lives.";
	}
	else
	{
		enjoy += (g_Dice % 3 - 1);
		ss << "went about her day as usual.";
	}


	//Events

	if (g_Gold.ival() > 1000 &&								// `J` first you need to have enough money to steal
		g_Dice.percent(10 - girl->morality()) &&			// positive morality will rarely steal
		!g_Dice.percent(girl->pclove() + 20) &&				// Love will make her not want to steal
		!g_Dice.percent(girl->obedience()) &&				// if she fails an obedience check
		!g_Dice.percent(girl->pcfear()))					// Fear may keep her from stealing
	{
		int steal = g_Gold.ival() / 1000;
		if (steal > 1000) steal = 1000;		if (steal < 10) steal = 10;

		if (roll_b < brothel->m_SecurityLevel)
		{
			stringstream warning;
			warning << "Your security spotted " << girlName << " trying to take " << steal << " gold from the Brothel for herself.\n";
			ss << "\n" << warning.str() << "\n";
			girl->m_Events.AddMessage(warning.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		}
		else
		{
			g_Gold.misc_debit(steal);
			girl->m_Money += steal;		// goes directly into her pocket
		}
	}

	if (girl->is_addict(true) && g_Dice.percent(girl->m_Withdrawals * 20))
	{
		int cost = 0;
		int method = 0;	// 1 = out of pocket, 2 = brothel money, 3 = sex, 4 = bj
		// 'Mute' Added so if the cost of the item changes then the gold amout will be correct
		sInventoryItem* item = 0;
		string itemName = "";
		/* */if (girl->has_trait("Viras Blood Addict"))	{ itemName = "Vira Blood"; }		// cost += 150;	girl->add_inv(g_InvManager.GetItem("Vira Blood")); }
		else if (girl->has_trait("Shroud Addict"))		{ itemName = "Shroud Mushroom"; }	// cost += 100;	girl->add_inv(g_InvManager.GetItem("Shroud Mushroom")); }
		else if (girl->has_trait("Fairy Dust Addict"))	{ itemName = "Fairy Dust"; }		// cost += 50;	girl->add_inv(g_InvManager.GetItem("Fairy Dust")); }
		if (itemName != "")		item = g_InvManager.GetItem(itemName);
		if (item)
		{
			cost += item->m_Cost;
			girl->add_inv(item);
		}
		// 'Mute' End  Change
		if (girl->has_trait("Cum Addict"))
			method = 4;
		else if (girl->has_trait("Nymphomaniac") && girl->libido() > 50)
			method = 3;
		else if (cost < girl->m_Money && g_Dice.percent(girl->morality()))		// pay out of pocket
			method = 1;
		else if (cost < g_Gold.ival() / 10 && g_Dice.percent(30 - girl->morality()) && !g_Dice.percent(girl->obedience() / 2) && !g_Dice.percent(girl->pcfear() / 2))
			method = 2;
		else method = g_Dice % 4 + 3;

		stringstream warning;
		int warningimage = IMGTYPE_PROFILE;
		switch (method)
		{
		case 1:
			if (!g_Dice.percent(girl->agility()))	// you only get to know about it if she fails an agility check
			{
				warning << girlName << " bought some drugs with her money.\n";
			}
			girl->m_Money -= cost;
			break;
		case 2:
			if (g_Dice.percent(cost / 2))		// chance that you notice the missing money
			{
				warning << girlName << " bought some drugs with some of the brothel's money.\n";
			}
			g_Gold.misc_debit(cost);
			break;
		case 3:
			if (!g_Dice.percent(girl->agility()))	// you only get to know about it if she fails an agility check
			{
				warning << girlName << " saw a customer with drugs and offered to fuck him for some. He accepted, so she took him out of sight of security and banged him.\n";
				warningimage = IMGTYPE_SEX;
			}
			girl->normalsex(1);
			break;
		default:
			if (!g_Dice.percent(girl->agility()))	// you only get to know about it if she fails an agility check
			{
				warning << girlName << " saw a customer with drugs and offered to give him a blowjob for some. He accepted, so she took him out of sight of security and sucked him off.\n";
				warningimage = IMGTYPE_ORAL;
			}
			girl->oralsex(1);
			break;
		}
		if (warning.str().length() > 0)
		{
			ss << "\n" << warning.str() << "\n";
			girl->m_Events.AddMessage(warning.str(), warningimage, EVENT_WARNING);
		}
	}

	if (girl->has_trait("Exhibitionist"))
	{
		ss << "\n \nShe hung out in the brothel wearing barely anything.";
		if (g_Dice.percent(50) && girl->has_trait("Horrific Scars"))
		{
			ss << " The customers were disgusted by her horrific scars.";
			brothel->m_Happiness -= 15;
		}
		else if (g_Dice.percent(50) && girl->has_trait("Small Scars"))
		{
			ss << " Some customers were disgusted by her scars.";
			brothel->m_Happiness -= 5;
		}
		else if (g_Dice.percent(50) && girl->has_trait("Bruises"))
		{
			ss << " The customers were disgusted by her bruises.";
			brothel->m_Happiness -= 5;
		}

		if (g_Dice.percent(50) && girl->has_trait("Futanari"))
		{
			ss << " The girls and some customers couldn't stop looking at her big cock.";
			brothel->m_Happiness += 2;
		}

		if (g_Dice.percent(50) && (girl->has_trait("Massive Melons") || girl->has_trait("Abnormally Large Boobs") || girl->has_trait("Titanic Tits")))
		{
			ss << " Her enormous, heaving breasts drew a lot of attention from the customers.";
			brothel->m_Happiness += 15;
		}
		else if (g_Dice.percent(50) && (girl->has_trait("Big Boobs") || girl->has_trait("Busty Boobs") || girl->has_trait("Giant Juggs")))
		{
			ss << " Her big, round breasts drew a lot of attention from the customers.";
			brothel->m_Happiness += 10;
		}
		if (g_Dice.percent(50) && (girl->has_trait("Deluxe Derriere") || girl->has_trait("Great Arse")))
		{
			ss << " The customers were hypnotized by the movements of her well shaped butt.";
			brothel->m_Happiness += 15;
		}
		if (g_Dice.percent(50) && (girl->has_trait("Great Figure") || girl->has_trait("Hourglass Figure")))
		{
			ss << " She has such a great figure that the customers couldn't stop looking at her.";
			brothel->m_Happiness += 15;
		}
		if (g_Dice.percent(50) && girl->has_trait("Sexy Air"))
		{
			ss << " She's so sexy that the customers couldn't stop looking at her.";
			brothel->m_Happiness += 10;
		}
		if (g_Dice.percent(50) && (girl->has_trait("Pierced Nipples") || girl->has_trait("Pierced Navel") || girl->has_trait("Pierced Nose")))
		{
			ss << " Her piercings catch the eye of some customers.";
			brothel->m_Happiness += 5;
		}
		imagetype = IMGTYPE_ECCHI;
	}

	if (girl->has_trait("Optimistic") && roll_b < girl->happiness() / 2) // 50% chance at best
	{
		ss << "\n \nWorking with someone as cheerful as " << girlName << " makes everybody a bit happier.";
		g_Brothels.UpdateAllGirlsStat(brothel, STAT_HAPPINESS, 1);
	}

	if (girl->has_trait("Pessimistic") && roll_b > 50 + girl->happiness() / 2) // 50% chance at worst
	{
		ss << "\n \nWorking with someone as pessimistic as " << girlName << " makes everybody a little bit sadder.";
		g_Brothels.UpdateAllGirlsStat(brothel, STAT_HAPPINESS, -1);
	}


	girl->m_Events.AddMessage(ss.str(), imagetype, Day0Night1);

	// Improve girl
	int xp = numgirls / 10, libido = 1, skill = 3;

	if (girl->has_trait("Quick Learner"))		{ skill += 1; xp += 5; }
	else if (girl->has_trait("Slow Learner"))	{ skill -= 1; xp -= 5; }
	if (girl->has_trait("Nymphomaniac"))			libido += 2;
	if (girl->has_trait("Lesbian"))				libido += numgirls / 10;
	else  if (!girl->has_trait("Straight"))		libido += numgirls / 20;
	wages += int(float(100.0 + (((girl->get_skill(SKILL_SERVICE) + girl->get_stat(STAT_CHARISMA) + girl->get_stat(STAT_INTELLIGENCE) + girl->get_stat(STAT_CONFIDENCE) + girl->get_skill(SKILL_MEDICINE) + 50) / 50)*numgirls) * cfg.out_fact.matron_wages()));
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);

	if (conf>-1) conf += g_Dice%skill;
	girl->confidence(conf);
	girl->happiness(happy);

	girl->exp(g_Dice%xp + 5);
	girl->medicine(g_Dice%skill);
	girl->service(g_Dice%skill + 2);
	girl->upd_temp_stat(STAT_LIBIDO, g_Dice%libido);

	girl->upd_Enjoyment(actiontype, enjoy);
	g_Girls.PossiblyGainNewTrait(girl, "Charismatic", 30, actiontype, "She has worked as a matron long enough that she has learned to be more Charismatic.", Day0Night1);
	g_Girls.PossiblyGainNewTrait(girl, "Psychic", 60, actiontype, "She has learned to handle the girls so well that you'd almost think she was Psychic.", Day0Night1);

	return false;
}

double cJobManager::JP_Matron(sGirl* girl, bool estimate)	// not used
{
	double jobperformance = 0.0;
	if (estimate)	// for third detail string
	{
		// `J` estimate - needs work
		jobperformance = ((girl->charisma() + girl->confidence() + girl->spirit()) / 3) +
			((girl->service() + girl->intelligence() + girl->medicine()) / 3) +
			girl->level(); // maybe add obedience?

		if (girl->is_slave()) jobperformance -= 1000;

		//traits, commented for now

		//good traits
		//if (girl->has_trait("Charismatic")) jobperformance += 20;
		//if (girl->has_trait("Cool Person")) jobperformance += 5;
		//if (girl->has_trait("Psychic")) jobperformance += 10;
		//if (girl->has_trait("Teacher")) jobperformance += 10;

		//bad traits
		//if (girl->has_trait("Dependant")) jobperformance -= 50;
		//if (girl->has_trait("Mind Fucked")) jobperformance -= 50;
		//if (girl->has_trait("Retarded")) jobperformance -= 60;
		//if (girl->has_trait("Bimbo")) jobperformance -= 10;
		//if (girl->has_trait("Smoker")) jobperformance -= 10;
		//if (girl->has_trait("Alcoholic")) jobperformance -= 25;
		//if (girl->has_trait("Fairy Dust Addict")) jobperformance -= 50;
		//if (girl->has_trait("Shroud Addict")) jobperformance -= 50;
		//if (girl->has_trait("Viras Blood Addict")) jobperformance -= 50;
	}
	else			// for the actual check		// not used
	{

	}

	return jobperformance;
}
