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

bool cJobManager::WorkBarSinger(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";
	string girlName = girl->m_Realname;

	if(Preprocessing(ACTION_WORKBAR, girl, brothel, DayNight, summary, message))	// they refuse to have work in the bar
		return true;

	// put that shit away, you'll scare off the customers!
	g_Girls.UnequipCombat(girl);

	int wages = 20;
	message += "She worked as a singer in the bar.";

	int roll = g_Dice%100;
	int jobperformance = (g_Girls.GetStat(girl, STAT_CONFIDENCE) + g_Girls.GetSkill(girl, SKILL_PERFORMANCE));

	//good traits
	if (g_Girls.HasTrait(girl, "Charismatic"))  //
		jobperformance += 15;
	if (g_Girls.HasTrait(girl, "Sexy Air"))  //
		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Cool Person"))  //people love to be around her
		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Cute"))  //
		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Charming"))  //people like charming people
		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Elegant"))  //
		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Quick Learner"))  //
		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Psychic"))  //knows what people want to hear
		jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Fearless"))
		jobperformance += 5;

	//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))  //needs others to do the job
		jobperformance -= 50;
	if (g_Girls.HasTrait(girl, "Aggressive"))  //gets mad easy and may attack people
		jobperformance -= 10;
	if (g_Girls.HasTrait(girl, "Nervous"))  //don't like to be around people
		jobperformance -= 30;
	if (g_Girls.HasTrait(girl, "Meek"))
		jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Broken Will"))
		jobperformance -= 50;
	if (g_Girls.HasTrait(girl, "Clumsy"))  //might trip on stage
		jobperformance -= 10;
	if (g_Girls.HasTrait(girl, "Slow Learner"))
		jobperformance -= 10;
	if (g_Girls.HasTrait(girl, "Construct"))  //voice would sound funny
		jobperformance -= 20;

	if(jobperformance >= 245)
		{
			message += " She must have the voice of an angel the customers go on and on about her and always come to listen to her when she works.\n\n";
			wages += 155;
		if (roll <= 50)
			{
				message += girlName + "'s voice brought many patron's to tears as she sang a song full of sadness.\n";
			}
			else
			{
				message += girlName + "'s soothing voice seems to glide over the noise and bustling of the bar.\n";
			}
		}
	else if(jobperformance >= 185)
		{
			message += " She's unbelievable at this and is always getting praised by the customers for her voice.\n\n";
			wages += 95;
		if (roll <= 50)
			{
				message += girlName + " begun to acquire her own following - a small crowd of people came in just to listen to her and buy drinks\n";
			}
			else
			{
				message += girlName + "'s soothing voice seems to glide over the noise and bustling of the bar.\n";
			}
		}
	else if(jobperformance >= 145)
		{
			message += " Her voice is really good and gets praised by the customers often.\n\n";
			wages += 55;
		if (roll <= 50)
			{
				message += "Her singing was pleasing, if bland.  Her voice was nice, if slightly untrained.\n";
			}
			else
			{
				message += "The slow song " + girlName + " sang at the end of shift really had her full emotion and heart.\n";
			}
		}
	else if(jobperformance >= 100)
		{
			message += " She hits a few right notes but she still has room to improve.\n\n";
			wages += 15;
		if (roll <= 50)
			{
				message += "While she won't win any contests, " + girlName + " isn't a terrible singer.\n";
			}
			else
			{
				message += "The slow song " + girlName + " sang at the end of shift really had her full emotion and heart.  A pity that she felt so bored and tired.\n";
			}
		}
	else if(jobperformance >= 70)
		{
			message += " She almost never hits a right note. Luck for you most of your customers are drunks.\n\n";
			wages -= 5;
		if (roll <= 50)
			{
				message += "Her singing is barely acceptable, but fortunately the bustling of the bar drowns " + girlName + " out for the most part.\n";
			}
			else
			{
				message += girlName + " voice is all over the place as she sings.\n";
			}
		}
	else
		{
			message += " Her voice sounds like nails on a chalk board.  She could be the worst singer ever.\n\n";
			wages -= 15;
		if (roll <= 50)
			{
				message += "Her audience seems paralyzed, as if they couldn't believe that a human body could produce those sounds, much less call them 'singing'.\n";
			}
			else
			{
				message += girlName + " bellowed out a melody that caused the bar to go into a panic clearing it quickly.\n";
			}
		}

	//try and add randomness here
	if (g_Girls.GetStat(girl, STAT_BEAUTY) >85)
		if((g_Dice%100) < 15)
		{
			message += " Stunned by her beauty a customer left her a great tip.\n";
			wages += 15;
		}

	if (g_Girls.HasTrait(girl, "Clumsy"))
		if((g_Dice%100) < 5)
		{
			message += " Her clumsy nature caused her to trip coming on stage causing the crowed to go wild with laughter. She went to the back and hide refusing to take the stage for her song set.\n";
			wages -= 15;
		}

	if (g_Girls.HasTrait(girl, "Pessimist"))
		if((g_Dice%100) < 5)
		{
			if(jobperformance < 125)
			{
			message += " Her pessimistic mood depressed the customers making them tip less.\n";
			wages -= 10;
			}
			else
			{
				message += girl->m_Realname + " was in a poor mood so the patrons gave her a bigger tip to try and cheer her up.\n";
				wages += 10;
			}
		}

	if (g_Girls.HasTrait(girl, "Optimist"))
		if((g_Dice%100) < 5)
		{
			if(jobperformance < 125)
			{
				message += girl->m_Realname + " was in a cheerful mood but the patrons thought she needed to work more on her on her sining.\n";
				wages -= 10;
			}
			else
			{
			message += " Her optimistic mood made patrons cheer up increasing the amount they tip.\n";
			wages += 10;
			}
		}

	if (g_Girls.HasTrait(girl, "Psychic"))
		if((g_Dice%101) < 20)
		{
			message += "She knew just what songs to sing to get better tips by using her Psychic powers.\n";
			wages += 15;
		}

	if (g_Girls.HasTrait(girl, "Assassin"))
		if((g_Dice%101) < 5)
		{
			if(jobperformance < 150)
			{
				message += " A patron booed her making her mad and using her Assassin skills she killed him before even thinking about it resulting in patrons storming out without paying.\n";
				wages -= 50;
			}
			else
			{
				message += " A patron booed her.  But was drunk and started crying a moment later so she ignored them.\n";
			}
		}

	if (g_Girls.HasTrait(girl, "Horrific Scars"))
		if((g_Dice%101) < 15)
		{
			if(jobperformance < 150)
			{
				message += " A patron gasped at her Horrific Scars making her uneasy.  But they didn't feel sorry for her.\n";
			}
			else
			{
				message += " A patron gasped at her Horrific Scars making her sad.  Feeling bad about it as she sang wonderful they left her a good tip.\n";
				wages += 15;
			}
		}

	if (g_Brothels.GetNumGirlsOnJob(0,JOB_PIANO,false) == 1)
		if((g_Dice%100) < 25)
		{
			if(jobperformance < 125)
			{
				message += girl->m_Realname + " she sang very out of tune with the paino player forcing people to leave.\n";
				wages -= 10;
			}
			else
			{
			message += girl->m_Realname + " the paino player took her sining to the next level causing the tips to flood in.\n";
			wages += 25;
			}
		}

		if(wages < 0)
			wages = 0;


	//enjoyed the work or not
	if(roll <= 5)
	{
		message += " \nSome of the patrons abused her during the shift.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKBAR, -1, true);
	}
	else if(roll <= 25) {
		message += " \nShe had a pleasant time working.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKBAR, +3, true);
		g_Girls.UpdateStat(girl, STAT_CONFIDENCE, 1);
	}
	else
	{
		message += " \nOtherwise, the shift passed uneventfully.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKBAR, +1, true);
	}

	girl->m_Events.AddMessage(message, IMGTYPE_SING, DayNight);
	int roll_max = (g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_CHARISMA));
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
	if(g_Dice%10)
		g_Girls.UpdateStat(girl, STAT_CONFIDENCE, 1);
	g_Girls.UpdateSkill(girl, SKILL_PERFORMANCE, skill);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	//gain traits
	g_Girls.PossiblyGainNewTrait(girl, "Charismatic", 70, ACTION_WORKBAR, "Singing on a daily basis has made " + girl->m_Realname + " more Charismatic.", DayNight != 0);

	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Nervous", 30, ACTION_WORKBAR, girl->m_Realname + " seems to finally be getting over her shyness. She's not always so Nervous anymore.", DayNight != 0);
	g_Girls.PossiblyLoseExistingTrait(girl, "Meek", 50, ACTION_WORKBAR, girl->m_Realname + "'s having to sing every day has forced her to get over her meekness.", DayNight != 0);

	return false;
}