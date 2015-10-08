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

	stringstream ss; string girlName = girl->m_Realname; ss << "Matron " << girlName;

	// `J` zzzzzz - this needs to be updated for building flow
	if (g_Girls.DisobeyCheck(girl, actiontype, brothel))
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}

	

	int numgirls = brothel->m_NumGirls;
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
		ss << " was overwhelmed by the number of girls she was required to manage and broke down crying.";
	}
	else if (check < 10)
	{
		enjoy -= (g_Dice % 3 + 1);
		conf -= -1; happy -= -3;
		ss << " had trouble dealing with some of the girls.";
	}
	else if (check > 90)
	{
		enjoy += (g_Dice % 3 + 1);
		conf += 1; happy += 3;
		ss << " enjoyed helping the girls with their lives.";
	}
	else
	{
		enjoy += (g_Dice % 3 - 1);
		ss << " went about her day as usual.";
	}


	//Events

	if (g_Girls.GetStat(girl, STAT_MORALITY) < -20 && roll_a > g_Girls.GetStat(girl, STAT_OBEDIENCE) && g_Dice.percent(50 + g_Girls.GetStat(girl, STAT_PCFEAR) / 2))
	{
		if (roll_b < brothel->m_SecurityLevel)
		{
			string warning = "Your security spotted her taking some of the brothel's money for herself.\n";
			ss << "\n" << warning << "\n";
			girl->m_Pay = 400 - g_Girls.GetStat(girl, STAT_MORALITY); //transfer gold from brothel to girl, hopefully this will do the trick. Is there a better way?
			girl->m_Events.AddMessage(warning, IMGTYPE_PROFILE, EVENT_WARNING);
		}
		else
		{
			girl->m_Pay = 400 - g_Girls.GetStat(girl, STAT_MORALITY); //transfer gold from brothel to girl, hopefully this will do the trick. Is there a better way?
		}
	}

	if (girl->is_addict(true) && girl->m_Withdrawals >= 3 && g_Dice.percent(60))
	{
		if (g_Girls.GetStat(girl, STAT_MORALITY) < -20 && !g_Girls.HasTrait(girl, "Cum Addict"))
		{
			string warning = "She bought some drugs with some of the brothel's money.\n";
			ss << "\n" << warning << "\n";
			if (g_Girls.HasTrait(girl, "Shroud Addict"))		g_Girls.AddInv(girl, g_InvManager.GetItem("Shroud Mushroom"));
			if (g_Girls.HasTrait(girl, "Fairy Dust Addict"))	g_Girls.AddInv(girl, g_InvManager.GetItem("Fairy Dust"));
			if (g_Girls.HasTrait(girl, "Viras Blood Addict"))	g_Girls.AddInv(girl, g_InvManager.GetItem("Vira Blood"));
			//Other items for other addictions?
			g_Gold.staff_wages(200); // hopefully this will do the trick. Is there a better way?
			girl->m_Events.AddMessage(warning, IMGTYPE_PROFILE, EVENT_WARNING);
		}
		else
		{
			string warning = "She saw a customer with drugs and offered to give him a blowjob for some. He accepted, so she took him out of sight of security and sucked him off.\n";
			ss << "\n" << warning << "\n";
			if (g_Girls.HasTrait(girl, "Shroud Addict"))		g_Girls.AddInv(girl, g_InvManager.GetItem("Shroud Mushroom"));
			if (g_Girls.HasTrait(girl, "Fairy Dust Addict"))	g_Girls.AddInv(girl, g_InvManager.GetItem("Fairy Dust"));
			if (g_Girls.HasTrait(girl, "Viras Blood Addict"))	g_Girls.AddInv(girl, g_InvManager.GetItem("Vira Blood"));
			g_Girls.UpdateSkill(girl, SKILL_ORALSEX, 1);
			//Other items for other addictions?
			girl->m_Events.AddMessage(warning, IMGTYPE_ORAL, EVENT_WARNING);
		}
	}
	
	if (g_Girls.HasTrait(girl, "Exhibitionist"))
	{
		ss << "\n\nShe hanged out in the brothel wearing barely anything.";
		if (g_Girls.HasTrait(girl, "Horrific Scars"))
		{
			ss << " The customers were disgusted by her horrific scars.";
			brothel->m_Happiness -= 15;
		}
		else if (g_Girls.HasTrait(girl, "Small Scars"))
		{
			ss << " Some customers were disgusted by her scars.";
			brothel->m_Happiness -= 5;
		}
		else if (g_Girls.HasTrait(girl, "Bruises"))
		{
			ss << " The customers were disgusted by her bruises.";
			brothel->m_Happiness -= 5;
		}
		else if (g_Girls.HasTrait(girl, "Futanari"))
		{
			ss << " The girls and some customers couldn't stop looking at her big cock.";
			brothel->m_Happiness += 2;
		}
		else if (g_Girls.HasTrait(girl, "Massive Melons") || g_Girls.HasTrait(girl, "Abnormally Large Boobs") || g_Girls.HasTrait(girl, "Titanic Tits"))
		{
			ss << " Her enormous, heaving breasts drew a lot of attention from the customers.";
			brothel->m_Happiness += 15;
		}
		else if (g_Girls.HasTrait(girl, "Deluxe Derriere") || g_Girls.HasTrait(girl, "Great Arse"))
		{
			ss << " The customers were hypnotized by the movements of her well shaped butt.";
			brothel->m_Happiness += 15;
		}
		else if (g_Girls.HasTrait(girl, "Great Figure") || g_Girls.HasTrait(girl, "Hourglass Figure"))
		{
			ss << " She has such a great figure that the customers couldn't stop looking at her.";
			brothel->m_Happiness += 15;
		}
		else if (g_Girls.HasTrait(girl, "Big Boobs") || g_Girls.HasTrait(girl, "Busty Boobs") || g_Girls.HasTrait(girl, "Giant Juggs"))
		{
			ss << " Her big, round breasts drew a lot of attention from the customers.";
			brothel->m_Happiness += 10;
		}
		else if (g_Girls.HasTrait(girl, "Sexy Air"))
		{
			ss << " She's so sexy that the customers couldn't stop looking at her.";
			brothel->m_Happiness += 10;
		}
		else if (g_Girls.HasTrait(girl, "Pierced Nipples") || g_Girls.HasTrait(girl, "Pierced Navel") || g_Girls.HasTrait(girl, "Pierced Nose"))
		{
			ss << " Her piercings catch the eye of some customers.";
			brothel->m_Happiness += 5;
		}
		imagetype = IMGTYPE_ECCHI;
	}
	
	if (g_Girls.HasTrait(girl, "Optimistic") && roll_b < g_Girls.GetStat(girl, STAT_HAPPINESS) / 2) // 50% chance at best
	{
		ss << "\n\nWorking with someone as cheerful as " << girlName << " makes everybody a bit happier.";
		UpdateAllGirlsStat(current, STAT_HAPPINESS, 1);
	}
	
	if (g_Girls.HasTrait(girl, "Pessimistic") && roll_b > 50 + g_Girls.GetStat(girl, STAT_HAPPINESS) / 2) // 50% chance at worst
	{
		ss << "\n\nWorking with someone as pessimistic as " << girlName << " makes everybody a little bit sadder.";
		UpdateAllGirlsStat(current, STAT_HAPPINESS, -1);
	}
	

	girl->m_Events.AddMessage(ss.str(), imagetype, Day0Night1);

	// Improve girl
	int xp = numgirls / 10, libido = 1, skill = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 5; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 5; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			libido += 2;
	if (!g_Girls.HasTrait(girl, "Straight"))			libido += numgirls / 20;

	girl->m_Pay = int(float(100.0 + (((girl->get_skill(SKILL_SERVICE) + girl->get_stat(STAT_CHARISMA) + girl->get_stat(STAT_INTELLIGENCE) + girl->get_stat(STAT_CONFIDENCE) + girl->get_skill(SKILL_MEDICINE) + 50) / 50)*numgirls) * cfg.out_fact.matron_wages()));

	if (conf>-1) conf += g_Dice%skill;
	g_Girls.UpdateStat(girl, STAT_CONFIDENCE, conf);
	g_Girls.UpdateStat(girl, STAT_HAPPINESS, happy);

	g_Girls.UpdateStat(girl, STAT_EXP, g_Dice%xp + 5);
	g_Girls.UpdateSkill(girl, SKILL_MEDICINE, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, g_Dice%skill + 2);
	g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, g_Dice%libido);

	g_Girls.UpdateEnjoyment(girl, actiontype, enjoy);
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