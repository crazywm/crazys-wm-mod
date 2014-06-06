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
#include "cCentre.h"
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
extern cCentreManager g_Centre;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;

bool cJobManager::WorkComunityService(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";

	if(Preprocessing(ACTION_WORKCENTRE, girl, brothel, DayNight, summary, message))	// they refuse to have work
		return true;

	// put that shit away, you'll scare off the customers!
	g_Girls.UnequipCombat(girl);

	int roll = g_Dice%100;
	int jobperformance = ((g_Girls.GetStat(girl, STAT_INTELLIGENCE) / 2) + (g_Girls.GetStat(girl, STAT_CHARISMA) / 2) + g_Girls.GetSkill(girl, SKILL_SERVICE));

	message += "She worked doing comunity services.";


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

	int dispo; // `J` merged slave/free messages and moved actual dispo change to after
	if (jobperformance >= 245)
	{
		message += " She must be perfect at this.\n\n";
		dispo = 12;
	}
	else if(jobperformance >= 185)
	{
		message += " She's unbelievable at this and is always getting praised by people for her work.\n\n";
		dispo = 10;
	}
	else if (jobperformance >= 145)
	{
		message += " She's good at this job and gets praised by people often.\n\n";
		dispo = 8;
	}
	else if (jobperformance >= 100)
	{
		message += " She made a few mistakes but overall she is okay at this.\n\n";
		dispo = 6;
	}
	else if (jobperformance >= 70)
	{
		message += " She was nervous and made a few mistakes. She isn't that good at this.\n\n";
		dispo = 4;
	}
	else
	{
		message += " She was nervous and constantly making mistakes. She really isn't very good at this job.\n\n";
		dispo = 2;
	}

	if (girl->m_States&(1 << STATUS_SLAVE))
	{
		message += " \nThe fact that she is your slave makes people think its less of a good deed on your part.";
		g_Brothels.GetPlayer()->disposition(dispo);
	}
	else
	{
		message += " \nThe fact that your paying this girl to do this helps people think your a better person.";
		girl->m_Pay += 100;
		g_Gold.staff_wages(100);  // wages come from you
		g_Brothels.GetPlayer()->disposition(int(dispo*1.5));
	}
	
	//enjoyed the work or not
	if(roll <= 5)
	{
		message += " \nSome of the patrons abused her during the shift.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKCENTRE, -1, true);
	}
	else if(roll <= 25) {
		message += " \nShe had a pleasant time working.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKCENTRE, +3, true);
	}
	else
	{
		message += " \nOtherwise, the shift passed uneventfully.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKCENTRE, +1, true);
	}

	girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, DayNight);

	// Improve stats
	int xp = 10, libido = 1, skill = 3;

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

	g_Girls.UpdateStat(girl, STAT_FAME, 1);
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	if(g_Dice%1)
		g_Girls.UpdateStat(girl, STAT_INTELLIGENCE, 1);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, skill);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	return false;
}