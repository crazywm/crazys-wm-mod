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
#include "cMovieStudio.h"
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
extern cMovieStudioManager g_Studios;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;

// `J` Movie Studio Job - Actress
bool cJobManager::WorkFilmGroup(sGirl* girl, sBrothel* brothel, int Day0Night1, string& summary)
{
	// No film crew.. then go home	// `J` this will be taken care of in building flow, leaving it in for now
	if (g_Studios.GetNumGirlsOnJob(0, JOB_CAMERAMAGE, SHIFT_NIGHT) == 0 || g_Studios.GetNumGirlsOnJob(0, JOB_CRYSTALPURIFIER, SHIFT_NIGHT) == 0)
	{
		girl->m_Events.AddMessage("There was no crew to film the scene, so she took the day off", IMGTYPE_PROFILE, EVENT_NOWORK);
		return false;
	}
	
	cConfig cfg;
	stringstream ss;
	string girlName = girl->m_Realname;
	int wages = 50;
	int enjoy = 0;
	int jobperformance = 0;

	g_Girls.UnequipCombat(girl);	// not for actress (yet)

	ss << girlName;

	int guys = g_Dice.d100();
	if (guys < 50)
	{
		guys = 2;
		jobperformance += 5;
		ss << " worked as an actress filming a three-way.\n\n";
	}
	else if (guys < 95)
	{
		jobperformance += 10;
		guys = g_Dice.d100();
		/* */if (guys < 40)		guys = 3;
		else if (guys < 60)		guys = 4;
		else if (guys < 75)		guys = 5;
		else if (guys < 85)		guys = 6;
		else if (guys < 90)		guys = 7;
		else if (guys < 94)		guys = 8;
		else if (guys < 98)		guys = 9;
		else /*          */		guys = 10;
		ss << " worked in a gang-bang scene with " << guys << " other people.\n\n";
	}
	else
	{
		jobperformance += 20;
		guys = g_Dice % 40 + 11;
		ss << " worked in a orgy scene with " << guys << " other people.\n\n";
	}

	girl->tiredness(guys - 2);	// Extra tiredness per guy

	int roll = g_Dice.d100();
	if (roll <= 10 && g_Girls.DisobeyCheck(girl, ACTION_WORKMOVIE, brothel))
	{
		ss << "She refused to do an orgy on film today.\n";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	else if (roll <= 10) 
	{ 
		enjoy -= ((guys > 10 ? (guys / 10) : (guys / 2)) + 1);
		ss << "She found it unpleasant fucking that many people.\n\n";
	}
	else if (roll >= 90) 
	{ 
		enjoy += (guys > 10 ? (guys / 10) : (guys / 2)) + 1;
		ss << "She loved getting so much action, and wants more!\n\n";
	}
	else 
	{ 
		enjoy += max(0, g_Dice % 3 - 1);	
		ss << "She wasn't really into having so much sex today, but managed to get through.\n\n"; 
	}
	jobperformance = enjoy * 2;

	if (g_Girls.CheckVirginity(girl))
	{
		g_Girls.LoseVirginity(girl);	// `J` updated for trait/status
		jobperformance += 50;
		ss << "She is no longer a virgin.\n";
	}

	if (!girl->calc_group_pregnancy(g_Brothels.GetPlayer(), false, 1.0))
	{
		g_MessageQue.AddToQue(girl->m_Realname + " has gotten pregnant", 0);
	}

	// remaining modifiers are in the AddScene function --PP
	int finalqual = g_Studios.AddScene(girl, SKILL_GROUP, jobperformance);
	ss << "Her scene is valued at: " << finalqual << " gold.\n";

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_GROUP, Day0Night1);

	// work out the pay between the house and the girl
	if (girl->is_slave() && !cfg.initial.slave_pay_outofpocket())
	{
		wages = 0;	// You own her so you don't have to pay her.
	}
	else
	{
		wages += (guys * 10);	// Extra pay per guy
		wages += finalqual*2;
	}
	girl->m_Pay += wages;
	

	// Improve stats
	int xp = (((guys > 10 ? (guys / 10) : (guys / 2)) + 1)*5);
	int skill = ((guys > 10 ? (guys / 10) : (guys / 2)) + 1);

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }

	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_PERFORMANCE, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_GROUP, g_Dice%skill + 1);

	g_Girls.UpdateEnjoyment(girl, ACTION_SEX, enjoy, true);
	g_Girls.UpdateEnjoyment(girl, ACTION_WORKMOVIE, enjoy, true);
	g_Girls.PossiblyGainNewTrait(girl, "Fake Orgasm Expert", 50, ACTION_SEX, "She has become quite the faker.", Day0Night1 == SHIFT_NIGHT);
	g_Girls.PossiblyGainNewTrait(girl, "Porn Star", 80, ACTION_WORKMOVIE, "She has performed in enough sex scenes that she has become a well known Porn Star.", Day0Night1 == SHIFT_NIGHT);

	return false;
}