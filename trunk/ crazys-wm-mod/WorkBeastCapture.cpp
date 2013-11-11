/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders 
 * who meet on http://pinkpetal.co.cc
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
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;

bool cJobManager::WorkBeastCapture(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";
	if(Preprocessing(ACTION_COMBAT, girl, brothel, DayNight, summary, message))
		return true;

	// ready armor and weapons!
	g_Girls.EquipCombat(girl);

	// TODO need better dialog
	sGirl* tempgirl = g_Girls.CreateRandomGirl(18, false, false, false, true, false);

	Uint8 fight_outcome = g_Girls.girl_fights_girl(girl, tempgirl);
	if(fight_outcome == 1)	// she won
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_COMBAT, +3, true);
		message = "She had fun hunting down animals today and came back with two of them.";
		girl->m_Events.AddMessage(message,IMGTYPE_COMBAT,DayNight);
		g_Brothels.add_to_beasts(2);
	}
	else  // she lost or it was a draw
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_COMBAT, -1, true);
		message = " The animals were difficult to track, but she did manage to capture one.";
		girl->m_Events.AddMessage(message,IMGTYPE_COMBAT,DayNight);
		g_Brothels.add_to_beasts(1);
	}

    // Cleanup
	if(tempgirl)
		delete tempgirl;
	tempgirl = 0;

	// Improve girl
	int xp = 8, libido = 2, skill = 1, BestSkill = g_Dice%4;

	if (g_Girls.HasTrait(girl, "Quick Learner"))
	{
		skill += 1;
		BestSkill += 1;
		xp += 3;
	}
	else if (g_Girls.HasTrait(girl, "Slow Learner"))
	{
		skill -= 1;
		xp -= 3;
		if (BestSkill > 0)
			BestSkill -= 1;
	}

	if (g_Girls.HasTrait(girl, "Nymphomaniac"))
		libido += 2;

	girl->m_Pay += 75;
	g_Gold.staff_wages(75);  // wages come from you
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_COMBAT, skill);
	g_Girls.UpdateSkill(girl, SKILL_MAGIC, skill);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);
	g_Girls.UpdateSkill(girl, SKILL_BEASTIALITY, BestSkill);

	g_Girls.PossiblyGainNewTrait(girl, "Tough", 15, ACTION_COMBAT, "She has become pretty Tough from all of the fights she's been in.", DayNight != 0);
	g_Girls.PossiblyGainNewTrait(girl, "Adventurer", 40, ACTION_COMBAT, "She has been in enough tough spots to consider herself Adventurer.", DayNight != 0);
	g_Girls.PossiblyGainNewTrait(girl, "Aggressive", 60, ACTION_COMBAT, "She is getting rather Aggressive from her enjoyment of combat.", DayNight != 0);

	return false;
}
