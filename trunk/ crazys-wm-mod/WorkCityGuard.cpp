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
#include "cArena.h"
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
extern cMovieStudioManager g_Studios;
extern cArenaManager g_Arena;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;

bool cJobManager::WorkCityGuard(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";
	string girlName = girl->m_Realname;
	if (Preprocessing(ACTION_WORKSECURITY, girl, brothel, DayNight, summary, message))
		return true;

	// ready armor and weapons!
	g_Girls.EquipCombat(girl);
	int roll = g_Dice%100;
	int wages = 150;
	int trouble = false;
	int agl = (g_Girls.GetStat(girl, STAT_AGILITY) / 2 + g_Dice%(g_Girls.GetStat(girl, SKILL_COMBAT) / 2));

	message += "She helps guard the city.\n";

	sGirl* tempgirl = g_Girls.CreateRandomGirl(18, false, false, false, false, false, true);

	if (roll >= 50)
	{
		message += girl->m_Realname + " didn't find any trouble today.";
		g_Brothels.GetPlayer()->suspicion(-5);
	}
	else if (roll >= 25)
	{
		message += girl->m_Realname + " spotted a theif and ";
		if (agl >= 90)
		{
			message += "was on them before they could blink.  Putting a stop to the theft.";
			g_Brothels.GetPlayer()->suspicion(-20);
			g_Girls.UpdateEnjoyment(girl, ACTION_WORKSECURITY, +3, true);
		}
		else if (agl >= 75)
		{
			message += "was on them before they could get away.  She is quick.";
			g_Brothels.GetPlayer()->suspicion(-15);
			g_Girls.UpdateEnjoyment(girl, ACTION_WORKSECURITY, +1, true);
		}
		else if (agl >= 50)
		{
			message += "was able to keep up, ";
			if (roll >= 50)
			{
				message += "but they ended up giving her the slip.";
				g_Brothels.GetPlayer()->suspicion(+10);
			}
			else
			{
				message += "and was able to catch them.";
				g_Brothels.GetPlayer()->suspicion(-10);
			}
		}
		else
		{
			message += "was left eating dust. Damn is she slow.";
			g_Brothels.GetPlayer()->suspicion(+10);
			g_Girls.UpdateEnjoyment(girl, ACTION_WORKSECURITY, -2, true);
		}
	}
	else
	{
		Uint8 fight_outcome = 0;
		if (tempgirl)		// `J` reworked incase there are no Non-Human Random Girls
		{
			fight_outcome = g_Girls.girl_fights_girl(girl, tempgirl);
		}
		else
		{
			g_LogFile.write("Error: You have no Random Girls for your girls to fight\n");
			g_LogFile.write("Error: You need a Random Girl to allow WorkCityGuard randomness");
			fight_outcome = 7;
		}
		if (fight_outcome == 7)
		{
			message = "There were no criminals around for her to fight.\n\n";
			message += "(Error: You need a Random Girl to allow WorkCityGuard randomness)";
		}
		else if (fight_outcome == 1)	// she won
		{
			g_Girls.UpdateEnjoyment(girl, ACTION_WORKSECURITY, +3, true);
			g_Girls.UpdateEnjoyment(girl, ACTION_COMBAT, +3, true);
			message = girl->m_Realname + " ran into some trouble and ended up in a fight. She was able to win.";
			g_Brothels.GetPlayer()->suspicion(-20);
			trouble = true;
		}
		else  // she lost or it was a draw
		{
			g_Girls.UpdateEnjoyment(girl, ACTION_WORKSECURITY, -1, true);
			g_Girls.UpdateEnjoyment(girl, ACTION_COMBAT, -1, true);
			message = girl->m_Realname + " ran into some trouble and ended up in a fight. She was unable to win the fight.";
			g_Brothels.GetPlayer()->suspicion(+20);
			trouble = true;
		}
	}

	 // Cleanup
	if(tempgirl)
		delete tempgirl;
	tempgirl = 0;

	if (trouble) 
	{
		girl->m_Events.AddMessage(message,IMGTYPE_COMBAT,DayNight);
	}
	else
	{
		girl->m_Events.AddMessage(message,IMGTYPE_PROFILE,DayNight);
	}

	if(girl->m_States&(1<<STATUS_SLAVE))
	{
		girl->m_Pay += 0;
	}
	else
	{
		g_Gold.girl_support(wages);  // matron wages come from you
		girl->m_Pay += wages;
	}

	// Improve girl
	int fightxp = 1;	if (trouble == 1)	fightxp = 3;
	int xp = 4 * fightxp, libido = 2, skill = 1;

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

	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_COMBAT, g_Dice % fightxp + skill);
	g_Girls.UpdateSkill(girl, SKILL_MAGIC, g_Dice % fightxp + skill);
	g_Girls.UpdateStat(girl, STAT_AGILITY, g_Dice % fightxp + skill);
	g_Girls.UpdateStat(girl, STAT_CONSTITUTION, g_Dice % fightxp + skill);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	g_Girls.PossiblyGainNewTrait(girl, "Tough", 20, ACTION_WORKSECURITY, "She has become pretty Tough from all of the fights she's been in.", DayNight != 0);
	g_Girls.PossiblyGainNewTrait(girl, "Aggressive", 60, ACTION_WORKSECURITY, "She is getting rather Aggressive from her enjoyment of combat.", DayNight != 0);
	g_Girls.PossiblyGainNewTrait(girl, "Fleet of Foot", 30, ACTION_WORKSECURITY, "She is getting rather fast from all the fighting.", DayNight != 0);


	return false;
}
