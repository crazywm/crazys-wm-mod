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

// `J` Brothel Job - Sleazy Bar
bool cJobManager::WorkSleazyBarmaid(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	string message = ""; string girlName = girl->m_Realname;
	int tex = g_Dice % 4;

	if (Preprocessing(ACTION_WORKCLUB, girl, brothel, Day0Night1, summary, message))	// they refuse to have work in the bar
		return true;

	// put that shit away, you'll scare off the customers!
	g_Girls.UnequipCombat(girl);

	int HateLove = 0;
	HateLove = g_Girls.GetStat(girl, STAT_PCLOVE) - g_Girls.GetStat(girl, STAT_PCHATE);
	int wages = 15, work = 0;
	int roll = g_Dice % 100;
	int imagetype = IMGTYPE_ECCHI;
	int jobperformance = ((g_Girls.GetStat(girl, STAT_CHARISMA) +
		g_Girls.GetStat(girl, STAT_BEAUTY) +
		g_Girls.GetSkill(girl, SKILL_PERFORMANCE)) / 3 +
		g_Girls.GetSkill(girl, SKILL_SERVICE));


	message += "She worked as a bartender in the strip club.";


	//good traits
	if (g_Girls.HasTrait(girl, "Charismatic"))		jobperformance += 20;
	if (g_Girls.HasTrait(girl, "Sexy Air"))			jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Cool Person"))		jobperformance += 10; //people love to be around her	
	if (g_Girls.HasTrait(girl, "Cute"))				jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Charming"))			jobperformance += 15;  //people like charming people	
	if (g_Girls.HasTrait(girl, "Great Figure"))		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Great Arse"))		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Quick Learner"))	jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Psychic"))			jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Mixologist"))		jobperformance += 25;

	//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))	jobperformance -= 50; //needs others to do the job	
	if (g_Girls.HasTrait(girl, "Clumsy"))		jobperformance -= 20; //spills food and breaks things often	
	if (g_Girls.HasTrait(girl, "Aggressive"))	jobperformance -= 20; //gets mad easy and may attack people
	if (g_Girls.HasTrait(girl, "Nervous"))		jobperformance -= 30; //don't like to be around people
	if (g_Girls.HasTrait(girl, "Meek"))			jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Slow Learner"))	jobperformance -= 10;
	if (g_Girls.HasTrait(girl, "Alcoholic"))	jobperformance -= 40;  //bad idea let an alcoholic near booze


	if (jobperformance >= 245)
	{
		message += " She must be the perfect bar tender customers go on and on about her and always come to see her when she works.\n\n";
		wages += 155;
	}
	else if (jobperformance >= 185)
	{
		message += " She's unbelievable at this and is always getting praised by the customers for her work.\n\n";
		wages += 95;
	}
	else if (jobperformance >= 145)
	{
		message += " She's good at this job and gets praised by the customers often.\n\n";
		wages += 55;
	}
	else if (jobperformance >= 100)
	{
		message += " She made a few mistakes but overall she is okay at this.\n\n";
		wages += 15;
	}
	else if (jobperformance >= 70)
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
	if (g_Girls.GetStat(girl, STAT_BEAUTY) > 85 && g_Dice.percent(20))
	{
		message += "Stunned by her beauty a customer left her a great tip.\n\n"; wages += 25;
	}

	if (g_Girls.GetStat(girl, STAT_BEAUTY) > 99 && g_Dice.percent(5))
	{
		message += girlName + " looked absolutely stunning during her shift and was unable to hide it. Instead of her ass or tits, the patrons couldn't glue their eyes off her face, and spent a lot more than usual on tipping her.\n"; wages += 50;
	}

	if (g_Girls.GetStat(girl, STAT_CHARISMA) > 85 && g_Dice.percent(20))
	{
		message += girlName + " surprised a couple of gentlemen discussing some complicated issue by her insightful comments when she was taking her order. They decided her words were worth a heavy tip.\n"; wages += 35;
	}

	if (g_Girls.HasTrait(girl, "Clumsy") && g_Dice.percent(15))
	{
		message += "Her clumsy nature caused her to spill a drink on a customer resulting in them storming off without paying.\n"; wages -= 15;
	}

	if (g_Girls.HasTrait(girl, "Pessimist") && g_Dice.percent(5))
	{
		if (jobperformance < 125)
		{
			message += "Her pessimistic mood depressed the customers making them tip less.\n"; wages -= 10;
		}
		else
		{
			message += girlName + " was in a poor mood so the patrons gave her a bigger tip to try and cheer her up.\n"; wages += 10;
		}
	}

	if (g_Girls.HasTrait(girl, "Optimist") && g_Dice.percent(5))
	{
		if (jobperformance < 125)
		{
			message += girlName + " was in a cheerful mood but the patrons thought she needed to work more on her services.\n"; wages -= 10;
		}
		else
		{
			message += "Her optimistic mood made patrons cheer up increasing the amount they tip.\n"; wages += 10;
		}
	}
	// `J` slightly higher percent compared to regular barmaid, I would think sleazy barmaid's uniform is more revealing
	if ((g_Dice.percent(5) && g_Girls.HasTrait(girl, "Busty Boobs")) ||
		(g_Dice.percent(10) && g_Girls.HasTrait(girl, "Big Boobs"))				||
		(g_Dice.percent(15) && g_Girls.HasTrait(girl, "Giant Juggs"))			||
		(g_Dice.percent(20) && g_Girls.HasTrait(girl, "Massive Melons"))		||
		(g_Dice.percent(25) && g_Girls.HasTrait(girl, "Abnormally Large Boobs"))||
		(g_Dice.percent(30) && g_Girls.HasTrait(girl, "Titanic Tits")))
	{
		if (jobperformance < 150)
		{ message += "A patron was staring obviously at her large breasts. But she had no idea how to take advantage of it.\n"; }
		else 
		{ message += "A patron was staring obviously at her large breasts. So she over charged them for drinks while they drooled not paying any mind to the price.\n"; wages += 15; }
	}

	if (g_Girls.HasTrait(girl, "Meek") && g_Dice.percent(5) && jobperformance < 125)
	{ 
		message += girlName + " spilled a drink all over a mans lap. He told her she had to lick it up and forced her to clean him up which she Meekly accepted and went about licking his cock clean.\n";
		imagetype = IMGTYPE_ORAL; work -= 3;
	}

	if (g_Dice.percent(5)) //may get moved to waitress
	{
		message += "A drunken patron decide to grab " + girlName + "'s boob. ";
		if (g_Girls.HasTrait(girl, "Meek"))
		{ message += "She was shocked and didn't react. The man molested her for a few minutes!\n"; }
		else if (g_Girls.HasTrait(girl, "Tough"))
		{ message += "She knocked him out! You could swear that she knocked a couple of his teeth out too!\n"; }
		else if (g_Girls.HasTrait(girl, "Your Daughter"))
		{ message += "She screamed do you know who my dad is? He will have your head for this!\n"; }
		else if (HateLove >= 80) //loves you
		{ message += "She screamed do you know who my love is? He will have your head for this!\n"; }
		else
		{ message += "She screamed and shook his hand off.\n"; }
	}

	if (g_Girls.HasTrait(girl, "Nymphomaniac") && g_Girls.GetStat(girl, STAT_LIBIDO) > 80 && g_Dice.percent(20) &&  !g_Girls.HasTrait(girl, "Virgin") && !g_Girls.HasTrait(girl, "Lesbian"))
	{
		if (roll <= 50)
		{ message += girlName + " dragged one of the less sober clients outside into a back alley and ordered him to fuck her senseless against a wall. After the deed, the poor fellow passed out and " + girlName + " returned to her job as if nothing happened.\n"; }
		else
		{ message += girlName + " spotted a relatively good-looking guy walking into the bathroom alone. She followed him inside, and as he tried to exit the bathroom stall, he got pushed back in by her. " + girlName + " didn't waste any time and in a matter of seconds was vigorously fucking the client. After the deed, the client made sure " + girlName + " had a pretty hefty wad of money stuck behind her skirt.\n"; wages += 50; }
		imagetype = IMGTYPE_SEX; g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -20);
	}

	if (wages < 0)
		wages = 0;


	//enjoyed the work or not
	if (roll <= 5)
	{ message += "\nSome of the patrons abused her during the shift."; work -= 1; }
	else if (roll <= 25) 
	{ message += "\nShe had a pleasant time working."; work += 3; }
	else
	{ message += "\nOtherwise, the shift passed uneventfully."; work += 1; }


	g_Girls.UpdateEnjoyment(girl, ACTION_WORKCLUB, work , true);
	girl->m_Events.AddMessage(message, imagetype, Day0Night1);


	int roll_max = (g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_CHARISMA));
	roll_max /= 4;
	wages += 10 + g_Dice%roll_max;
	girl->m_Pay = wages;

	// Improve stats
	int xp = 15, libido = 1, skill = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_FAME, 1);
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_PERFORMANCE, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, g_Dice%skill+1);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	//gained
	g_Girls.PossiblyGainNewTrait(girl, "Charismatic", 60, ACTION_WORKBAR, "Dealing with customers at the bar and talking with them about their problems has made " + girlName + " more Charismatic.", Day0Night1 == SHIFT_NIGHT);
	if (jobperformance < 100 && roll <= 2) { g_Girls.PossiblyGainNewTrait(girl, "Assassin", 10, ACTION_WORKBAR, girlName + "'s lack of skill at mixing drinks has been killing people left and right making her into quite the Assassin.", Day0Night1 == SHIFT_NIGHT); }

	//lose
	g_Girls.PossiblyLoseExistingTrait(girl, "Nervous", 20, ACTION_WORKBAR, girlName + " seems to finally be getting over her shyness. She's not always so Nervous anymore.", Day0Night1 == SHIFT_NIGHT);

	return false;
}