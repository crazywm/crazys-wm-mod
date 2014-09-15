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

bool cJobManager::WorkBarPiano(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";
	string girlName = girl->m_Realname;

	if(Preprocessing(ACTION_WORKMUSIC, girl, brothel, DayNight, summary, message))	// they refuse to have work in the bar
		return true;

	// put that shit away, you'll scare off the customers!
	g_Girls.UnequipCombat(girl);

	int wages = 20;
	message += "She played the piano in the bar.";

	int roll = g_Dice%100;
	int jobperformance = (	g_Girls.GetStat(girl, STAT_CONFIDENCE)/2 + 
							g_Girls.GetStat(girl, STAT_INTELLIGENCE)/2 +
							g_Girls.GetSkill(girl, SKILL_PERFORMANCE));

	//good traits
	if (g_Girls.HasTrait(girl, "Charismatic"))    jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Sexy Air"))		  jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Cool Person"))    jobperformance += 5;   //people love to be around her	
	if (g_Girls.HasTrait(girl, "Cute"))			  jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Charming"))		  jobperformance += 5;   //people like charming people
	if (g_Girls.HasTrait(girl, "Elegant"))		  jobperformance += 15;  //Elegant people usally know how to play the piano lol
	if (g_Girls.HasTrait(girl, "Quick Learner"))  jobperformance += 5;   //people like charming people
	if (g_Girls.HasTrait(girl, "Psychic"))		  jobperformance += 10;  //knows what people want to hear
		

	//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))	jobperformance -= 50;  //needs others to do the job	
	if (g_Girls.HasTrait(girl, "Aggressive"))	jobperformance -= 20;  //gets mad easy and may attack people
	if (g_Girls.HasTrait(girl, "Nervous"))		jobperformance -= 30;  //don't like to be around people
	if (g_Girls.HasTrait(girl, "Meek"))			jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Broken Will"))  jobperformance -= 50;
	if (g_Girls.HasTrait(girl, "Slow Learner")) jobperformance -= 10;


	 if (jobperformance >= 245)
		{
			message += " She plays with the grace of an angel the customers go on and on about her and always come to listen to her when she works.\n\n";
			wages += 155;
		if (roll <= 20)
			{
				message += girlName + "'s playing brought many patron's to tears as she played a song full of sadness.\n";
				brothel->m_Happiness += 5;
			}
		else if (roll <= 40)
			{
				message += "Nice melody fills the room when " + girlName + " is behind the piano.\n";
				brothel->m_Happiness += 10;
			}
		else if (roll <= 60)
			{
				message += "Knowing that she is good, " + girlName + " played all the tunes blindfolded.\n";
				brothel->m_Fame += 10;
			}
		else if (roll <= 80)
			{
				message += girlName + " being confident in her skill, " + girlName + " played today using only one hand.\n";
				brothel->m_Fame += 10;
			}
		else
			{
				message += girlName + "'s soothing playing seems to glide over the noise and bustling of the bar.\n";
				brothel->m_Happiness += 10;
			}
		}
 else if (jobperformance >= 185)
		{
			message += " She's unbelievable at this and is always getting praised by the customers for her playing skills.\n\n";
			wages += 95;
		if (roll <= 20)
			{
				message += girlName + " begun to acquire her own following - a small crowd of people came in just to listen to her and buy drinks\n";
				brothel->m_Fame += 5;
				wages += 10;
			}
		else if (roll <= 40)
			{
				message += "Her playing fills the room. Some customers hum the melody under their noses.\n";
				brothel->m_Happiness += 5;
			}
		else if (roll <= 60)
			{
				message += "After making a mistake she improvised a passage to the next song.\n";
			}
		else if (roll <= 80)
			{
				message += "She plays without music sheets having all the songs memorized.\n";
				brothel->m_Fame += 5;
			}
		else
			{
				message += girlName + "'s soothing playing seems to glide over the noise and bustling of the bar.\n";
			}
		}
 else if (jobperformance >= 145)
		{
			message += " Her playing is really good and gets praised by the customers often.\n\n";
			wages += 55;
		if (roll <= 20)
			{
				message += "Her playing was pleasing, if bland.  Her rythem was nice, if slightly untrained.\n";
			}
		else if (roll <= 40)
			{
				message += girlName + " don't have any trouble playing the piano.\n";
			}
		else if (roll <= 60)
			{
				message += "Give " + girlName + " any kind of music sheet and she will play it. She is really good at this.\n";
				brothel->m_Happiness += 5;
			}
		else if (roll <= 80)
			{
				message += "When asked to play one of the more complicated tunes she gave her all.\n";
			}
		else
			{
				message += "The slow song " + girlName + " played at the end of shift really had her full emotion and heart.\n";
			}
		}
 else if (jobperformance >= 100)
		{
			message += " She hits a few right notes but she still has room to improve.\n\n";
			wages += 15;
		if (roll <= 20)
			{
				message += "While she won't win any contests, " + girlName + " isn't a terrible pianist.\n";
			}
		else if (roll <= 40)
			{
				message += girlName + "'s performance today was good. She seems to you as a promising pianist.\n";
			}
		else if (roll <= 60)
			{
				message += "She gets the key order right most of the time.\n";
			}
		else if (roll <= 80)
			{
				message += "You could tell that there was something like a melody, but " + girlName + " still need a lot of practice.\n";
			}
		else
			{
				message += "The slow song " + girlName + " played at the end of shift really had her full emotion and heart.  A pity that she felt so bored and tired.\n";
			}
		}
 else if (jobperformance >= 70)
		{
			message += " She almost never hits a right note. Luck for you most of your customers are drunks.\n\n";
			wages -= 5;
		if (roll <= 20)
			{
				message += "Her playing is barely acceptable, but fortunately the bustling of the bar drowns " + girlName + " out for the most part.\n";
			}
		else if (roll <= 40)
			{
				message += "She is terrible at this. Some customers left after she started to play.\n";
				brothel->m_Happiness -= 5;
			}
		else if (roll <= 60)
			{
				message += "You could count on the fingers of one hand the part in her play that was clean.\n";
			}
		else if (roll <= 80)
			{
				message += "She is bad at playing the piano.\n";
			}
		else
			{
				message += girlName + " knows a note.  To bad its the only one she knows and plays it over and over.\n";
			}
		}
 else
		{
			message += " She didn't play the piano so much as banged on it.\n\n";
			wages -= 15;
		if (roll <= 20)
			{
				message += "Her audience seems paralyzed, as if they couldn't believe that a piano was capable of making such noise.\n";
				brothel->m_Happiness -= 10;
			}
		else if (roll <= 40)
			{
				message += "After ten seconds you wanted to grab an axe and ends the instruments misery being tortured by " + girlName + " attempt to play.\n";
				brothel->m_Happiness -= 5;
			}
		else if (roll <= 60)
			{
				message += "Hitting keys at random nobody wouldn't call playing, but " + girlName + " thinks otherwise.\n";
			}
		else if (roll <= 80)
			{
				message += "You could swear that a rat running inside the piano would bring better sounds then " + girlName + ".\n";
			}
		else
			{
				message += girlName + " banged on the piano clearly having no clue what a note was.\n";
			}
		}


	//try and add randomness here
	if (g_Girls.GetStat(girl, STAT_BEAUTY) >85)
		if((g_Dice%100) < 15)
		{
			message += " Stunned by her beauty a customer left her a great tip.\n\n";
			wages += 15;
		}

	if (g_Girls.HasTrait(girl, "Clumsy"))
		if((g_Dice%100) < 5)
		{
			message += " Her clumsy nature caused her to close the lid on her fingers making her have to stop playing for a few hours.\n\n";
			wages -= 15;
		}

	if (g_Girls.HasTrait(girl, "Pessimist"))
		if((g_Dice%100) < 5)
		{
			if(jobperformance < 125)
			{
			message += " Her pessimistic mood depressed the customers making them tip less.\n\n";
			wages -= 10;
			}
			else
			{
				message += girl->m_Realname + " was in a poor mood so the patrons gave her a bigger tip to try and cheer her up.\n\n";
				wages += 10;
			}
		}

	if (g_Girls.HasTrait(girl, "Optimist"))
		if((g_Dice%100) < 5)
		{
			if(jobperformance < 125)
			{
				message += girl->m_Realname + " was in a cheerful mood but the patrons thought she needed to work more on her on her playing.\n\n";
				wages -= 10;
			}
			else
			{
			message += " Her optimistic mood made patrons cheer up increasing the amount they tip.\n\n";
			wages += 10;
			}
		}

	if (g_Girls.HasTrait(girl, "Psychic"))
		if((g_Dice%101) < 20)
		{
			message += "She used her Psychic skills to know excatally what the patrons wanted to hear her play.\n";
			wages += 15;
		}

	if (g_Girls.HasTrait(girl, "Assassin"))
		if((g_Dice%101) < 5)
		{
			if(jobperformance < 150)
			{
				message += " A patron bumped into the piano causing her to miss a note.  This pissed her off and using her Assassin skills she killed him before even thinking about it resulting in patrons storming out without paying.\n";
				wages -= 50;
			}
			else
			{
				message += " A patron bumped into the piano.  But with her skill she didn't miss a note.  The patron was luck to leave with his life.\n";
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
				message += " A patron gasped at her Horrific Scars making her sad.  Feeling bad about it as she did a wonderful job they left a good tip.\n";
				wages += 15;
			}
		}

	if (g_Brothels.GetNumGirlsOnJob(0,JOB_SINGER,false) == 1)
		if((g_Dice%100) < 25)
		{
			if(jobperformance < 125)
			{
				message += girl->m_Realname + " played poorly with the singer making people leave.\n\n";
				wages -= 10;
			}
			else
			{
			message += girl->m_Realname + " played well with the singer increasing tips.\n\n";
			wages += 25;
			}
		}


	//enjoyed the work or not
	if(roll <= 5)
	{
		message += " \nSome of the patrons abused her during the shift.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKMUSIC, -1, true);
	}
	else if(roll <= 25) {
		message += " \nShe had a pleasant time working.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKMUSIC, +3, true);
		g_Girls.UpdateStat(girl, STAT_CONFIDENCE, 1);
	}
	else
	{
		message += " \nOtherwise, the shift passed uneventfully.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKMUSIC, +1, true);
	}

	
	girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, DayNight);
	int roll_max = (g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_CHARISMA));
	roll_max /= 4;
	wages += 10 + g_Dice%roll_max;
	girl->m_Pay = wages;

	// Improve stats
	int xp = 10, libido = 1, skill = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_FAME, 1);
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	if(g_Dice%2==1)
		g_Girls.UpdateStat(girl, STAT_CONFIDENCE, g_Dice%skill);
	else
		g_Girls.UpdateStat(girl, STAT_INTELLIGENCE, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_PERFORMANCE, g_Dice%skill + 1);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	//gain traits
	g_Girls.PossiblyGainNewTrait(girl, "Elegant", 75, ACTION_WORKMUSIC, "Playing the piano has given " + girl->m_Realname + " an Elegant nature.", DayNight != 0);

	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Nervous", 30, ACTION_WORKMUSIC, girl->m_Realname + " seems to finally be getting over her shyness. She's not always so Nervous anymore.", DayNight != 0);

	return false;
}