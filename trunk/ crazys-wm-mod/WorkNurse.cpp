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

bool cJobManager::WorkNurse(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";

	if (Preprocessing(ACTION_WORKNURSE, girl, brothel, DayNight, summary, message))
		return true;

	// put that shit away, you'll scare off the customers!
	g_Girls.UnequipCombat(girl);

	int wages = 25;
	message += "She worked as a nurse.";

	int roll = g_Dice%100;
	int jobperformance = (g_Girls.GetStat(girl, STAT_INTELLIGENCE) + g_Girls.GetSkill(girl, SKILL_SERVICE));

	//good traits
	if (g_Girls.HasTrait(girl, "Charismatic"))  //
		jobperformance += 20;
	if (g_Girls.HasTrait(girl, "Sexy Air"))  //
		jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Cool Person"))  //people love to be around her
		jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Cute"))  //
		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Charming"))  //people like charming people
		jobperformance += 15;
	if (g_Girls.HasTrait(girl, "Nerd"))  //
		jobperformance += 15;
	if (g_Girls.HasTrait(girl, "Quick Learner"))  
		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Psychic"))  //knows what is going in peoples heads
		jobperformance += 10;

	//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))  //needs others to do the job
		jobperformance -= 50;
	if (g_Girls.HasTrait(girl, "Clumsy"))  //spills food and breaks things often
		jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Aggressive"))  //gets mad easy and may attack people
		jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Nervous"))  //don't like to be around people
		jobperformance -= 30;
	if (g_Girls.HasTrait(girl, "Meek"))
		jobperformance -= 20;

	
	if (jobperformance >= 245)
	{
		message += " She must be the perfect nurse patients go on and on about her and always come to see her when she works.\n\n";
		wages += 155;
	}
	else if (jobperformance >= 185)
	{
		message += " She's unbelievable at this and is always getting praised by the patients for her work.\n\n";
		wages += 95;
	}
	else if (jobperformance >= 135)
	{
		message += " She's good at this job and gets praised by the patients often.\n\n";
		wages += 55;
	}
	else if (jobperformance >= 85)
	{
		message += " She made a few mistakes but overall she is okay at this.\n\n";
		wages += 15;
	}
	else if (jobperformance >= 65)
	{
		message += " She was nervous and made a few mistakes. She isn't that good at this.\n\n";
		wages -= 5;
	}
	else
	{
		message += " She was nervous and constantly making mistakes. She really isn't very good at this job.\n\n";
		wages -= 15;
	}

	//try and add randomness here
	if (g_Girls.GetStat(girl, STAT_BEAUTY) > 85)
	{
		if (roll <= 20)
		{
			message += " Stunned by her beauty a patient left her a great tip.\n\n";
			wages += 25;
		}
	}
	if (g_Girls.HasTrait(girl, "Clumsy"))
	{
		if (roll <= 15)
		{
			message += " Her clumsy nature caused her to spill a medicine everywhere.\n";
			wages -= 15;
		}
	}
	if (g_Girls.HasTrait(girl, "Pessimist"))
	{
		if (roll <= 5)
		{
			if (jobperformance < 125)
			{
				message += " Her pessimistic mood depressed the patients making them tip less.\n";
				wages -= 10;
			}
			else
			{
				message += girl->m_Realname + " was in a poor mood so the patients gave her a bigger tip to try and cheer her up.\n";
				wages += 10;
			}
		}
	}
	if (g_Girls.HasTrait(girl, "Optimist"))
	{
		if (roll <= 5)
		{
			if (jobperformance < 125)
			{
				message += girl->m_Realname + " was in a cheerful mood but the patients thought she needed to work more on her services.\n";
				wages -= 10;
			}
			else
			{
				message += " Her optimistic mood made patients cheer up increasing the amount they tip.\n";
				wages += 10;
			}
		}
	}
	
	//enjoyed the work or not
	if(roll <= 5)
	{
		message += " \nSome of the patrons abused her during the shift.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKNURSE, -1, true);
	}
	else if(roll <= 25) {
		message += " \nShe had a pleasant time working.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKNURSE, +3, true);
	}
	else
	{
		message += " \nOtherwise, the shift passed uneventfully.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKNURSE, +1, true);
	}

	girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, DayNight);
	
	int roll_max = (g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetSkill(girl, SKILL_SERVICE));
	roll_max /= 4;
	wages += 10 + g_Dice%roll_max;

	int patients = g_Clinic.GetNumGirlsOnJob(0, JOB_GETHEALING, DayNight == 0) +
		g_Clinic.GetNumGirlsOnJob(0, JOB_GETABORT, DayNight==0) +
		g_Clinic.GetNumGirlsOnJob(0, JOB_PHYSICALSURGERY, DayNight == 0) +
		g_Clinic.GetNumGirlsOnJob(0, JOB_LIPO, DayNight == 0) +
		g_Clinic.GetNumGirlsOnJob(0, JOB_BREASTREDUCTION, DayNight == 0) +
		g_Clinic.GetNumGirlsOnJob(0, JOB_BOOBJOB, DayNight == 0) +
		g_Clinic.GetNumGirlsOnJob(0, JOB_VAGINAREJUV, DayNight == 0) +
		g_Clinic.GetNumGirlsOnJob(0, JOB_FACELIFT, DayNight == 0) +
		g_Clinic.GetNumGirlsOnJob(0, JOB_ASSJOB, DayNight == 0);


	wages += 5 * patients;		// `J` pay her 5 for each patient you send to her		

	if (wages < 0)	wages = 0;
	girl->m_Pay = wages;
	

	// Improve stats
	int xp = 5 + patients, libido = 1, skill = 2;

	if (g_Girls.HasTrait(girl, "Quick Learner"))
	{
		skill += 1;
		xp += 3;
	}
	else if (g_Girls.HasTrait(girl, "Slow Learner"))
	{
		skill -= 1;
		xp -= 3;
	}

	if (g_Girls.HasTrait(girl, "Nymphomaniac"))
		libido += 2;

	if (g_Girls.HasTrait(girl, "Lesbian"))
		libido += patients/2;

	g_Girls.UpdateStat(girl, STAT_FAME, 1);
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	if (g_Dice%2==1)
		g_Girls.UpdateStat(girl, STAT_INTELLIGENCE, skill);
	else
		g_Girls.UpdateStat(girl, STAT_CHARISMA, skill);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, skill);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	//gain traits
	g_Girls.PossiblyGainNewTrait(girl, "Charismatic", 60, ACTION_WORKNURSE, "Dealing with patients and talking with them about their problems has made " + girl->m_Realname + " more Charismatic.", DayNight != 0);

	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Nervous", 20, ACTION_WORKNURSE, girl->m_Realname + " seems to finally be getting over her shyness. She's not always so Nervous anymore.", DayNight != 0);
	
	return false;
}