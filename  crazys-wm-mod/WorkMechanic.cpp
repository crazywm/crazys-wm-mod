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
#include "cClinic.h"
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
extern cClinicManager g_Clinic;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;

// `J` Job Clinic - Staff
bool cJobManager::WorkMechanic(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKMECHANIC;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	if (g_Girls.DisobeyCheck(girl, actiontype, brothel))			// they refuse to work 
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked as a mechanic.\n\n";

	
	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!
	int wages = 15, work = 0;
	int roll = g_Dice.d100();
	double jobperformance = JP_Mechanic(girl, false);


	if (jobperformance >= 245)
	{
		wages += 155;
		ss << "She must be the perfect mechanic patients go on and on about her and always come to see her when she works.\n\n";
	}
	else if (jobperformance >= 185)
	{
		wages += 95;
		ss << "She's unbelievable at this and is always getting praised by the patients for her work.\n\n";
	}
	else if (jobperformance >= 135)
	{
		wages += 55;
		ss << "She's good at this job and gets praised by the patients often.\n\n";
	}
	else if (jobperformance >= 85)
	{
		wages += 15;
		ss << "She made a few mistakes but overall she is okay at this.\n\n";
	}
	else if (jobperformance >= 65)
	{
		wages -= 5;
		ss << "She was nervous and made a few mistakes. She isn't that good at this.\n\n";
	}
	else
	{
		wages -= 15;
		ss << "She was nervous and constantly making mistakes. She really isn't very good at this job.\n\n";
	}


	//try and add randomness here
	if (g_Girls.GetStat(girl, STAT_BEAUTY) > 85 && g_Dice.percent(20))
	{
		wages += 25;
		ss << " Stunned by her beauty a patient left her a great tip.\n\n";
	}
	if (g_Girls.HasTrait(girl, "Construct") && g_Dice.percent(15))
	{
		wages += 15;
		ss << " Seeing a kindred spirit, the mechanical patient left her a great tip.\n";
	}
	if (g_Girls.HasTrait(girl, "Half-Construct") && g_Dice.percent(15))
	{
		wages += 15;
		ss << " Seeing a kindred spirit, the mechanical patient left her a great tip.\n";
	}
	if (g_Girls.HasTrait(girl, "Clumsy") && g_Dice.percent(15))
	{
		wages -= 15;
		ss << " Her clumsy nature caused her to drop parts everywhere.\n";
	}
	if (g_Girls.HasTrait(girl, "Pessimist") && g_Dice.percent(5))
	{
		if (jobperformance < 125)
		{
			wages -= 10;
			ss << " Her pessimistic mood depressed the patients making them tip less.\n";
		}
		else
		{
			wages += 10;
			ss << girlName << " was in a poor mood so the patients gave her a bigger tip to try and cheer her up.\n";
		}
	}
	if (g_Girls.HasTrait(girl, "Optimist") && g_Dice.percent(5))
	{
		if (jobperformance < 125)
		{
			wages -= 10;
			ss << girlName << " was in a cheerful mood but the patients thought she needed to work more on her services.\n";
		}
		else
		{
			wages += 10;
			ss << " Her optimistic mood made patients cheer up increasing the amount they tip.\n";
		}
	}


	//enjoyed the work or not
	if (roll <= 5)
	{
		ss << "\nSome of the patrons abused her during the shift.";
		work -= 1;
	}
	else if (roll <= 25)
	{
		ss << "\nShe had a pleasant time working.";
		work += 3;
	}
	else
	{
		ss << "\nOtherwise, the shift passed uneventfully.";
		work += 1;
	}

	g_Girls.UpdateEnjoyment(girl, actiontype, work);

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);
	int roll_max = (g_Girls.GetStat(girl, STAT_INTELLIGENCE) + g_Girls.GetSkill(girl, SKILL_SERVICE));
	roll_max /= 4;
	wages += 10 + g_Dice%roll_max;
	wages += 5 * g_Clinic.GetNumGirlsOnJob(0, JOB_GETREPAIRS, Day0Night1);	// `J` pay her 5 for each patient you send to her		
	if (wages < 0)	wages = 0;
	girl->m_Pay = wages;


	// Improve stats
	int xp = 10, libido = 1, skill = 2;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_FAME, 1);
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateStat(girl, STAT_INTELLIGENCE, g_Dice%skill + 1);
	g_Girls.UpdateSkill(girl, SKILL_MEDICINE, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, g_Dice%skill);
	g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, libido);

	//gain traits
	g_Girls.PossiblyGainNewTrait(girl, "Charismatic", 60, actiontype, "Dealing with patients and talking with them about their problems has made " + girl->m_Realname + " more Charismatic.", Day0Night1);
	g_Girls.PossiblyGainNewTrait(girl, "Strong", 60, actiontype, "Handling heavy parts and working with heavy tools has made " + girl->m_Realname + " much Stronger.", Day0Night1);

	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Nervous", 20, actiontype, girl->m_Realname + " seems to finally be getting over her shyness. She's not always so Nervous anymore.", Day0Night1);
	g_Girls.PossiblyLoseExistingTrait(girl, "Elegant", 40, actiontype, " Working with dirty, greasy equipment has damaged " + girl->m_Realname + "'s hair, skin and nails making her less Elegant.", Day0Night1);
	return false;
}

double cJobManager::JP_Mechanic(sGirl* girl, bool estimate)// not used
{
	double jobperformance
		= (g_Girls.GetStat(girl, STAT_INTELLIGENCE) +
		g_Girls.GetSkill(girl, SKILL_MEDICINE) / 2 +
		g_Girls.GetSkill(girl, SKILL_SERVICE) / 2);

	//good traits
	if (g_Girls.HasTrait(girl, "Charismatic"))		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Sexy Air"))			jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Cool Person"))		jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Charming"))			jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Nerd"))				jobperformance += 15;
	if (g_Girls.HasTrait(girl, "Strong"))			jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Tough"))			jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Construct"))		jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Half-Construct"))	jobperformance += 5;

	//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))		jobperformance -= 40;
	if (g_Girls.HasTrait(girl, "Clumsy"))			jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Aggressive"))		jobperformance -= 10;
	if (g_Girls.HasTrait(girl, "Nervous"))			jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Meek"))				jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Elegant"))			jobperformance -= 5;	// Don't break a nail
	if (g_Girls.HasTrait(girl, "Queen"))			jobperformance -= 20;	// Manual labor is beneth her
	if (g_Girls.HasTrait(girl, "Princess"))			jobperformance -= 10;	// Manual labor is beneth her
	return jobperformance;
}
