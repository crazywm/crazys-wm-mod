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

bool cJobManager::WorkMechanic(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";
	int tex = g_Dice % 4;

	if (Preprocessing(ACTION_WORKDOCTOR, girl, brothel, DayNight, summary, message))	// they refuse to have work in the bar
		return true;

	// put that shit away, you'll scare off the customers!
	g_Girls.UnequipCombat(girl);

	int wages = 15;
	message += "She worked as a mechanic.";

	int roll = g_Dice % 100;
	int jobperformance = (g_Girls.GetStat(girl, STAT_INTELLIGENCE) + g_Girls.GetSkill(girl, SKILL_SERVICE));

	//good traits
	if (g_Girls.HasTrait(girl, "Charismatic"))		jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Sexy Air"))			jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Cool Person"))		jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Cute"))				jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Charming"))			jobperformance += 10;
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


	if (jobperformance >= 245){ wages += 155;	message += " She must be the perfect mechanic patients go on and on about her and always come to see her when she works.\n\n"; }
	else if (jobperformance >= 185){ wages += 95;	message += " She's unbelievable at this and is always getting praised by the patients for her work.\n\n"; }
	else if (jobperformance >= 135){ wages += 55;	message += " She's good at this job and gets praised by the patients often.\n\n"; }
	else if (jobperformance >= 85){ wages += 15;	message += " She made a few mistakes but overall she is okay at this.\n\n"; }
	else if (jobperformance >= 65){ wages -= 5;		message += " She was nervous and made a few mistakes. She isn't that good at this.\n\n"; }
	else	{ wages -= 15;	message += " She was nervous and constantly making mistakes. She really isn't very good at this job.\n\n"; }


	//try and add randomness here
	if (g_Girls.GetStat(girl, STAT_BEAUTY) >85)
		if (g_Dice % 100 <= 20)	{ wages += 25;	message += " Stunned by her beauty a patient left her a great tip.\n\n"; }
	if (g_Girls.HasTrait(girl, "Construct"))
		if (g_Dice % 100 <= 15)	{ wages += 15;	message += " Seeing a kindred spirit, the mechanical patient left her a great tip.\n"; }
	if (g_Girls.HasTrait(girl, "Half-Construct"))
		if (g_Dice % 100 <= 15)	{ wages += 15;	message += " Seeing a kindred spirit, the mechanical patient left her a great tip.\n"; }
	if (g_Girls.HasTrait(girl, "Clumsy"))
		if (g_Dice % 100 <= 15)	{ wages -= 15;	message += " Her clumsy nature caused her to drop parts everywhere.\n"; }
	if (g_Girls.HasTrait(girl, "Pessimist"))
	{	if (g_Dice % 100 <= 5)	{
			if (jobperformance < 125){ wages -= 10;	message += " Her pessimistic mood depressed the patients making them tip less.\n"; }
			else {	wages += 10;	message += girl->m_Realname + " was in a poor mood so the patients gave her a bigger tip to try and cheer her up.\n"; }
	}	}
	if (g_Girls.HasTrait(girl, "Optimist"))
	{	if (g_Dice % 100 <= 5)	{
			if (jobperformance < 125){	wages -= 10;	message += girl->m_Realname + " was in a cheerful mood but the patients thought she needed to work more on her services.\n";}
			else {	wages += 10;	message += " Her optimistic mood made patients cheer up increasing the amount they tip.\n";}
	}	}
	
	if(wages < 0)	wages = 0;



	/*if (roll <= 50 && g_Girls.DisobeyCheck(girl, ACTION_WORKBAR, brothel))
	{
		message = girl->m_Realname + gettext(" refused to as a barmaid today.");
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else if(roll <= 15) {
		message += gettext(" \nSome of the patrons abused her during the shift.");
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKBAR, -1, true);
	}
	else if(roll >=90)
	{
		message += gettext(" \nShe had a pleasant time working.");
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKBAR, +3, true);
	}
	else
	{
		message += gettext(" \nOtherwise, the shift passed uneventfully.");
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKBAR, +1, true);
	}*/



	//enjoyed the work or not
	if(roll <= 5)
	{
		message += " \nSome of the patrons abused her during the shift.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKDOCTOR, -1, true);
	}
	else if(roll <= 25) {
		message += " \nShe had a pleasant time working.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKDOCTOR, +3, true);
	}
	else
	{
		message += " \nOtherwise, the shift passed uneventfully.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKDOCTOR, +1, true);
	}

	girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, DayNight);
	int roll_max = (g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetSkill(girl, SKILL_SERVICE));
	roll_max /= 4;
	wages += 10 + g_Dice%roll_max;
	girl->m_Pay = wages;
	

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
	if(g_Dice%2) 
		g_Girls.UpdateStat(girl, STAT_INTELLIGENCE, 1);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, skill);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	//gain traits
	g_Girls.PossiblyGainNewTrait(girl, "Charismatic", 60, ACTION_WORKDOCTOR, "Dealing with patients and talking with them about their problems has made " + girl->m_Realname + " more Charismatic.", DayNight != 0);
	g_Girls.PossiblyGainNewTrait(girl, "Strong", 60, ACTION_WORKDOCTOR, "Handling heavy parts and working with heavy tools has made " + girl->m_Realname + " much Stronger.", DayNight != 0);

	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Nervous", 20, ACTION_WORKDOCTOR, girl->m_Realname + " seems to finally be getting over her shyness. She's not always so Nervous anymore.", DayNight != 0);
	g_Girls.PossiblyLoseExistingTrait(girl, "Elegant", 20, ACTION_WORKDOCTOR, " Working with dirty, greasy equipment has damaged " + girl->m_Realname + "'s hair, skin and nails making her less Elegant.", DayNight != 0);
	return false;
}