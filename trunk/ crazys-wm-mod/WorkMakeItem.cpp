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

// `J` Farm Job - Producers
bool cJobManager::WorkMakeItem(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKMAKEITEMS;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	if (g_Girls.DisobeyCheck(girl, actiontype, brothel))			// they refuse to work 
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " was assigned to make items at the farm.\n\n";

	double jobperformance = JP_MakeItem(girl, false);

	int enjoy = 0;
	int wages = 25;
	int tips = 0;
	int imagetype = IMGTYPE_CRAFT;
	int msgtype = Day0Night1;

#if 1
	// TODO need better dialog
	if (g_Dice % 100 <= 10)
	{
		g_Girls.UpdateEnjoyment(girl, actiontype, -1, true);
		ss << " She wasn't able to make anything.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_CRAFT, Day0Night1);
	}
	else
	{
		g_Girls.UpdateEnjoyment(girl, actiontype, +3, true);
		ss << " She enjoyed her time working and made two items.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_CRAFT, Day0Night1);
		g_Brothels.add_to_goods(2);
	}
#else
	if (roll_a <= 10)
	{
		enjoyC -= g_Dice % 3; enjoyF -= g_Dice % 3;
		CleanAmt = int(CleanAmt * 0.8);
		/* */if (roll_b < 30)	ss << "She spilled a bucket of something unpleasant all over herself.";
		else if (roll_b < 60)	ss << "She stepped in something unpleasant.";
		else /*            */	ss << "She did not like working on the farm today.";
	}
	else if (roll_a >= 90)
	{
		enjoyC += g_Dice % 3; enjoyF += g_Dice % 3;
		CleanAmt = int(CleanAmt * 1.1);
		/* */if (roll_b < 50)	ss << "She cleaned the building while humming a pleasant tune.";
		else /*            */	ss << "She had a great time working today.";
	}
	else
	{
		enjoyC += g_Dice % 2; enjoyF += g_Dice % 2;
		ss << "The shift passed uneventfully.";
	}
	ss << "\n\n";
#endif



#if 0

	// `J` Farm Bookmark - adding in items that can be created in the farm















#endif






	// Improve girl
	int xp = 10, libido = 1, skill = 2;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	girl->m_Pay += 25;
	g_Gold.staff_wages(25);  // wages come from you
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_CRAFTING, g_Dice%skill + 1);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, g_Dice%skill);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	return false;
}

double cJobManager::JP_MakeItem(sGirl* girl, bool estimate)// not used
{
	double jobperformance =
		(g_Girls.GetSkill(girl, SKILL_CRAFTING) +
		g_Girls.GetSkill(girl, SKILL_SERVICE));

	return jobperformance;
}
