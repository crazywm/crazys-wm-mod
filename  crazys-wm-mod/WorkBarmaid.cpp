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
#include "libintl.h"

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;

bool cJobManager::WorkBarmaid(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";
	string girlName = girl->m_Realname;

	if(Preprocessing(ACTION_WORKBAR, girl, brothel, DayNight, summary, message))	// they refuse to have work in the bar
		return true;

	// put that shit away, you'll scare off the customers!
	g_Girls.UnequipCombat(girl);

	int wages = 15;
	message += "She worked as a barmaid.";

	int roll = g_Dice%100;
	int jobperformance = (	g_Girls.GetStat(girl, STAT_INTELLIGENCE)/2 + 
							g_Girls.GetSkill(girl, SKILL_PERFORMANCE)/2 + 
							g_Girls.GetSkill(girl, SKILL_SERVICE));

	//good traits
	if (g_Girls.HasTrait(girl, "Charismatic"))  //
		jobperformance += 15;
	if (g_Girls.HasTrait(girl, "Sexy Air"))  //
		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Cool Person"))  //people love to be around her
		jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Cute"))  //
		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Charming"))  //people like charming people
		jobperformance += 15;
	if (g_Girls.HasTrait(girl, "Quick Learner"))  
		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Psychic"))
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
	if (g_Girls.HasTrait(girl, "Slow Learner"))
		jobperformance -= 10;


	if (jobperformance >= 245)
		{
			message += " She must be the perfect bar tender customers go on and on about her and always come to see her when she works.\n\n";
			wages += 155;
		if (roll <= 14)
			{
				message += girlName + " was sliding drinks all over the bar without spilling a drop she put on quite a show for the patrons.\n";
			}
		else if (roll <= 28)
			{
				message +=  "She agree to play a bar game with a client. Knowing every type of bar game there is, " + girlName + " easily wins. The customer pays double for his drinks and leaves the bar saying that he will win next time.\n";
			}
		else if (roll <= 42)
			{
				message += girlName + " made an 11 layer drink like it was nothing. The amazed customer leaved her a big tip!\n";
			}
		else if (roll <= 56)
			{
				message += "She pours eleven 100ml shots from a single, one liter bottle. Now there’s a good barmaid!\n";
			}
		else if (roll <= 70)
			{
				message += girlName + " noticed that a client was upset about something. After a pleasant conversation she managed to cheer him up. The client left full of will power, leaving a generous tip behind.\n";
			}
		else if (roll <= 84)
			{
				message +=  "Bottles fly high under the ceiling when " + girlName + " is pouring drinks for the customers. The amazed crowd loudly applauses every caught bottle and leave big tips for the girl.\n";
			}
		else
			{
				message += girlName + " mixed up what some patrons called the perfect drink.  It got them drunk faster then anything they had before.\n";
			}
		}
 else if (jobperformance >= 185)
		{
			message += " She's unbelievable at this and is always getting praised by the customers for her work.\n\n";
			wages += 95;
		if (roll <= 14)
			{
				message += girlName + " had the bar filled with happy drunks.  She didn't miss a beat all shift.\n";
			}
		else if (roll <= 28)
			{
				message +=  "Today wasn’t really exciting for " + girlName + ". From boredom she spent some time trying to make more complicated drinks from the menu.\n";
			}
		else if (roll <= 42)
			{
				message += girlName + " propose to a client to play a drinking game with her. If she loses she will serve nude to the end of her shift, but if she wins he will be paying double. Some other patrons join the wager on the same terms. After a few hours the last of them drops drunk and " + girlName + " cleaned up on money. \n";
			}
		else if (roll <= 56)
			{
				message += "When taking orders from customers, " + girlName + " talked them into buying more expensive drinks, that let you make a solid profit today.\n";
			}
		else if (roll <= 70)
			{
				message += girlName + " is great at this job. At happy hour she was irreplaceable getting all the orders right. Later on she even prevented a fight between customers.\n";
			}
		else if (roll <= 84)
			{
				message +=  "Her shift was slow and hardly anyone was buying. " + girlName + " took the initiative, announcing a special promotion. Every third shot ordered by a client could be drunk from a shot-glass between her breasts. The promotion was such a success that you almost run out of booze.\n";
			}
		else
			{
				message += "People love seeing " + girl->m_Realname + " work and they pour into the bar during her shift.  She mixes wonderful drinks and doesn't mess orders up so they couldn't be happier.\n";
			}
		}
 else if (jobperformance >= 145)
		{
			message += " She's good at this job and gets praised by the customers often.\n\n";
			wages += 55;
		if (roll <= 14)
			{
				message += girlName + " didn't mix up any orders and kept the patrons drunk and happy.\n";
			}
		else if (roll <= 28)
			{
				message +=  girlName + " certainly knows what she is doing behind the bar counter. She spends her shift without making any mistakes and earning a lot from tips.\n";
			}
		else if (roll <= 42)
			{
				message += girlName + " didn’t make any mistakes today. She even earned some tips from happy customers.\n";
			}
		else if (roll <= 56)
			{
				message += "When mixing one of more complicated cocktails, " + girlName + " noticed that she made a mistake and remakes the order. She wasted some alcohol, but the customer has happy with his drink.\n";
			}
		else if (roll <= 70)
			{
				message += girlName + " spend more time talking with customers then pouring into their glasses. She didn’t earn much today.\n";
			}
		else if (roll <= 84)
			{
				message +=  "Her shift as a barmaid goes smooth. " + girlName + " feels more confident in her skills after today.\n";
			}
		else
			{
				message += girlName + " had some regualers come in.  She knows just how to keep them happy and spending gold.\n";
			}
		}
 else if (jobperformance >= 100)
		{
			message += " She made a few mistakes but overall she is okay at this.\n\n";
			wages += 15;
		if (roll <= 14)
			{
				message += girlName + " mixed up a few drink orders..  But they order the same drink so it didn't matter to much.\n";
			}
		else if (roll <= 28)
			{
				message +=  girlName + " made few mistakes but none of them were lethal.\n";
			}
		else if (roll <= 42)
			{
				message += "Trying her best she accidently knocks down a bottle containing one of the bars most expensive liquors.\n";
			}
		else if (roll <= 56)
			{
				message += "Maybe she isn’t the best and fastest, but at least every drop of a drink that she pours stays in the glass.\n";
			}
		else if (roll <= 70)
			{
				message +=  "She agreed to play a bar game with a client. " + girlName + " loses and the client spends the rest of his stay drinking on the house.\n";
			}
		else if (roll <= 84)
			{
				message +=  girlName + " focused all her attention on taking orders and making drinks. Her attitude wasn’t too appealing to clients. Some customers left feeling mistreated and unhappy.\n";
			}
		else
			{
				message += girlName + " wasted a few drinks by forgetting to ice them but wasn't anything major.\n";
			}
		}
 else if (jobperformance >= 70)
		{
			message += " She was nervous and made a few mistakes. She isn't that good at this.\n\n";
			wages -= 5;
		if (roll <= 14)
			{
				message += girlName + " mixed up peoples drink orders..  When she only had four patrons drinking.\n";
			}
		else if (roll <= 28)
			{
				message +=  girlName + " is having a bad day and she isn’t trying to hide it.  Her bad attitude shows and rubs off on the customers, leaving a negative impression on them.\n";
			}
		else if (roll <= 42)
			{
				message += "Not being very good at this, she makes few mistakes. " + girlName + " feels that she didn’t improve today.\n";
			}
		else if (roll <= 56)
			{
				message += "Wanting to impress a client, she throws a bottle of an expensive liquor into the air. Trying to catch it behind her back, " + girlName + " fails.\n";
			}
		else if (roll <= 70)
			{
				message +=  "Client looking for some encouragement and understanding words from the barmaid unfortunately approached " + girlName + ". After short conversation she made him cry.\n";
			}
		else if (roll <= 84)
			{
				message +=  girlName + " tried to uncork a new barrel of beer. In result of her attempt, she spends the rest of her shift mopping the floor.\n";
			}
		else
			{
				message += girlName + " gave someone a drink she mixed that made them sick.  It was nothing but coke and ice so who knows how she did it.\n";
			}
		}
 else
		{
			message += " She was nervous and constantly making mistakes. She really isn't very good at this job.\n\n";
			wages -= 15;
		if (roll <= 14)
			{
				message += girlName + " was giving orders to the wrong patrons and letting a lot people walk out without paying there tab.\n";
			}
		else if (roll <= 28)
			{
				message += "She mixed the ordered cocktails in the wrong proportions, making the clients throw up from the intoxication after just one shot! Besides the swearing at her and yelling that they will never come here again, they left without paying.\n";
			}
		else if (roll <= 42)
			{
				message += "You can surely tell, that standing behind the bar isn’t her happy place. Being tense she made a lot of mistakes today.\n";
			}
		else if (roll <= 56)
			{
				message += "Not having any experience at this kind of job, " + girlName + " tries her best.. Regrettably without results.\n";
			}
		else if (roll <= 70)
			{
				message += "She gets in a drinking game with a customer. Being a total lightweight she gets drunk quickly and pasts out on the job.\n";
			}
		else if (roll <= 84)
			{
				message += "She spends more of her shift flirting with a client not paying any attention to others. What’s worse, the guy she was flirting with, skips without paying the bill!\n";
			}
		else
			{
				message += girlName + " spilled drinks all over the place and mixed the wrong stuff when trying to make drinks for people.\n";
			}
		}


	//try and add randomness here
	if (g_Girls.GetStat(girl, STAT_BEAUTY) >85)
		if((g_Dice%101) < 20)
		{
			message += " Stunned by her beauty a customer left her a great tip.\n\n";
			wages += 25;
		}

	if (g_Girls.HasTrait(girl, "Clumsy"))
		if((g_Dice%101) < 15)
		{
			message += " Her clumsy nature caused her to spill a drink on a custmoer resulting in them storming off without paying.\n";
			wages -= 15;
		}

	if (g_Girls.HasTrait(girl, "Pessimist"))
		if((g_Dice%101) < 5)
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
		if((g_Dice%101) < 5)
		{
			if(jobperformance < 125)
			{
				message += girl->m_Realname + " was in a cheerful mood but the patrons thought she needed to work more on her services.\n";
				wages -= 10;
			}
			else
			{
			message += " Her optimistic mood made patrons cheer up increasing the amount they tip.\n";
			wages += 10;
			}
		}

	if (g_Girls.HasTrait(girl, "Big Boobs") || g_Girls.HasTrait(girl, "Abnormally Large Boobs"))
		if((g_Dice%101) < 15)
		{
			if(jobperformance < 150)
			{
				message += " A patron was staring obviously at her large breasts. But she had no ideal how to take advantage of it.\n";
			}
			else
			{
				message += " A patron was staring obviously at her large breasts. So she over charged them for drinks while they drooled not paying any mind to the price.\n";
				wages += 15;
			}
		}

	if (g_Girls.HasTrait(girl, "Psychic"))
		if((g_Dice%101) < 20)
		{
			message += "She used her Psychic skills to know excatally what the patrons wanted to order and when to refill there mugs keeping them happy and increasing tips.\n";
			wages += 15;
		}

	if (g_Girls.HasTrait(girl, "Assassin"))
		if((g_Dice%101) < 5)
		{
			if(jobperformance < 150)
			{
				message += " A patron pissed her off and using her Assassin skills she killed him before even thinking about it resulting in patrons storming out without paying.\n";
				wages -= 50;
			}
			else
			{
				message += " A patron pissed her off but she was able to keep her cool as she is getting use to this kinda thing.\n";
			}
		}

	if (g_Girls.HasTrait(girl, "Horrific Scars"))
		if((g_Dice%101) < 15)
		{
			if(jobperformance < 150)
			{
				message += " A patron gasped at her Horrific Scars making her sad.  But they didn't feel sorry for her.\n";
			}
			else
			{
				message += " A patron gasped at her Horrific Scars making her sad.  Feeling bad about it as she did a wonderful job they left a good tip.\n";
				wages += 15;
			}
		}

		if(wages < 0)
			wages = 0;



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
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKBAR, -1, true);
	}
	else if(roll <= 25) {
		message += " \nShe had a pleasant time working.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKBAR, +3, true);
	}
	else
	{
		message += " \nOtherwise, the shift passed uneventfully.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKBAR, +1, true);
	}

	girl->m_Events.AddMessage(message, IMGTYPE_WAIT, DayNight);
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
	if (g_Dice%2 == 1)	
		g_Girls.UpdateStat(girl, STAT_INTELLIGENCE, g_Dice%skill);
	else				
		g_Girls.UpdateSkill(girl, SKILL_PERFORMANCE, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, g_Dice%skill + 1);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	//gain traits
	g_Girls.PossiblyGainNewTrait(girl, "Charismatic", 60, ACTION_WORKBAR, "Dealing with customers at the bar and talking with them about their problems has made " + girl->m_Realname + " more Charismatic.", DayNight != 0);

	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Nervous", 40, ACTION_WORKBAR, girl->m_Realname + " seems to finally be getting over her shyness. She's not always so Nervous anymore.", DayNight != 0);
	return false;
}