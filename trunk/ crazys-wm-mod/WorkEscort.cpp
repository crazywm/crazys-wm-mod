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
#include <algorithm>

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;

bool cJobManager::WorkEscort(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "", girlName = girl->m_Realname;
	int jobperformance = 0, wages = 0, tips = 0;

	if (Preprocessing(ACTION_WORKESCORT, girl, brothel, DayNight, summary, message))	// they refuse to have work in the bar
		return true;


	// put that shit away, you'll scare off the customers!
	g_Girls.UnequipCombat(girl);

	int agl = (g_Girls.GetStat(girl, STAT_AGILITY));
	int roll_a = g_Dice % 100;							// customer type
	int roll_b = g_Dice % 100;							// customer wealth
	int roll_c = g_Dice % 100 + agl;					// agility adjustment
	int roll_d = g_Dice % 100;

	int pass_a = false;
	int pass_b = false;

	//CRAZY
	/*Escort plans
		General ideal is to have her arrive to an appoiment with a client.  If she is to late they may turn her away 
		resulting in no money if she is on time it goes to the next part.  Next part is they look her over seeing if
		she looks good enough for them or not and looks at what she is wearing.  Fancy dresses help while something like 
		slave rags would hurt and may get her sent away again.  Then they are off to do something like go to dinner, a bar,
		maybe a fancy party what have u.  From there its can she hold his intrest and not embarres him.  Traits play the biggest
		part in this.  An elegant girl would do well for a rich person where a aggressive girl might not. On the other hand
		a deadbeat might be turned off by a elegant girl and prefer something more sleazy or such.  If they pass this part 
		it goes to a will they have sex roll.  Sometimes they might be okay with not having sex with the girl and be more then
		happy with just her company other times sex is a must.  Do a roll for the sex type while taking into account what
		types of sex are allowed.  Her skill will determine if the client is happy or not.  Sex would be an extra cost.  The 
		further a girl makes it and the happier the client the more she makes.  Deadbeat would be super easy to please where
		a king might be almost impossable to make fully happy without the perfect girl.  Sending a virgin should have different
		things happen if it comes to sex where the girl could accept or not.  Maybe have special things happen if the escort is 
		your daughter.*/ 


	// `J` The type of customer She Escorts
	/*default*/	int cust_type = 1;    string cust_type_text = "Commoner";
	/* */if (roll_a <= 1)	{ cust_type = 9; cust_type_text = "King"; }
	else if (roll_a <= 3)	{ cust_type = 8; cust_type_text = "Prince"; }
	else if (roll_a <= 6)	{ cust_type = 7; cust_type_text = "Noble"; }
	else if (roll_a <= 10)	{ cust_type = 6; cust_type_text = "Judge"; }
	else if (roll_a <= 15)	{ cust_type = 5; cust_type_text = "Mayor"; }
	else if (roll_a <= 21)	{ cust_type = 4; cust_type_text = "Sheriff"; }
	else if (roll_a <= 45)	{ cust_type = 3; cust_type_text = "Bureaucrat"; }
	else if (roll_a <= 65)	{ cust_type = 2; cust_type_text = "Regular"; }
	else if (roll_a >= 98)	{ cust_type = 0; cust_type_text = "Deadbeat"; }

	// `J` The wealth of customer She Escorts
	/*default*/	int cust_wealth = 2;	string cust_wealth_text = "";
	/* */if (roll_b <= 20)	{ cust_wealth = 3; cust_wealth_text = "rich "; }
	else if (roll_b <= 40)	{ cust_wealth = 1; cust_wealth_text = "poor "; }
	else if (roll_b >= 98)	{ cust_wealth = 0; cust_wealth_text = "broke "; }

	// `J` do job performance
	message = girlName;
	/* */if (roll_c >= 150)	{ jobperformance += 20;	message += " arrived early"; }
	else if (roll_c >= 100)	{ jobperformance += 10;	message += " was on time"; }
	else if (roll_c >= 80)	{ jobperformance += 0;	message += " was a few minutes late"; }
	else if (roll_c >= 50)	{ jobperformance -= 5;	message += " was late"; }
	else /*             */	{ jobperformance -= 10;	message += " was very late"; }
	message += " to her appointment with a " + cust_wealth_text + cust_type_text + ".\n";

	//// Where do they go?
	//*default*/	int loc_type = 1;    string loc_type_text = "Restaurant";
	///* */if (roll_d <= 1)	{ loc_type = 8; loc_type_text = "Vacation"; }
	//else if (roll_d <= 3)	{ loc_type = 7; loc_type_text = "Wedding"; }
	//else if (roll_d <= 6)	{ loc_type = 6; loc_type_text = "Party"; }
	//else if (roll_d <= 10)	{ loc_type = 5; loc_type_text = "Arena Match"; }
	//else if (roll_d <= 15)	{ loc_type = 4; loc_type_text = "Movie"; }
	//else if (roll_d <= 45)	{ loc_type = 3; loc_type_text = "Strip Club"; }
	//else if (roll_d <= 65)	{ loc_type = 2; loc_type_text = "Bar"; }
	//else if (roll_d >= 98)	{ loc_type = 0; loc_type_text = "Park"; }


	// `J` do wages and tips
	if (cust_type * cust_wealth <= 0 || g_Dice.percent(2))	// the customer can not or will not pay
	{
		wages = tips = 0;
		if (g_Dice.percent(25))	// Runner
		{
			if (g_Gangs.GetGangOnMission(MISS_GUARDING))
			{
				sGang* gang = g_Gangs.GetGangOnMission(MISS_GUARDING);
				if (g_Dice.percent(gang->m_Skills[STAT_AGILITY]))
				{
					message += " The customer tried to run off without paying. Your men caught him before he got away.";
					SetGameFlag(FLAG_CUSTNOPAY);
					wages = max(g_Dice%girl->askprice(), girl->askprice() * cust_type * cust_wealth);	// Take what customer has
				}
				else	message += " The customer couldn't pay and managed to elude your guards.";
			}
			else	message += " The customer couldn't pay and ran off. There were no guards!";
		}
		else
		{
			// offers to pay the girl what he has
			if (g_Dice.percent(g_Girls.GetStat(girl, STAT_INTELLIGENCE)))
			{
				// she turns him over to the goons
				message += " The customer couldn't pay the full amount, so your girl turned them over to your men.";
				SetGameFlag(FLAG_CUSTNOPAY);
			}
			else	message += " The customer couldn't pay the full amount.";
			wages = max(g_Dice%girl->askprice(), g_Dice%(girl->askprice() * cust_type * cust_wealth));	// Take what customer has
		}
	}
	else
	{
		wages = girl->askprice() * cust_type * cust_wealth;
		tips = (jobperformance > 0) ? (g_Dice%jobperformance) * cust_type * cust_wealth : 0;
	}


	// work out the pay between the house and the girl
	girl->m_Tips = max(tips, 0);
	girl->m_Pay = wages;
	
	// Improve stats
	int xp = 20, libido = 1, skill = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3;}
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateStat(girl, STAT_INTELLIGENCE, g_Dice%skill+1);
	g_Girls.UpdateStat(girl, STAT_CONFIDENCE, g_Dice%skill+1);
	g_Girls.UpdateStat(girl, STAT_FAME, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_PERFORMANCE, g_Dice%skill + 1);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	girl->m_Events.AddMessage(message, IMGTYPE_FORMAL, DayNight);

	//gain traits
	g_Girls.PossiblyGainNewTrait(girl, "Charismatic", 60, ACTION_WORKESCORT, "Dealing with customers and talking with them about their problems has made " + girl->m_Realname + " more Charismatic.", DayNight != 0);
	g_Girls.PossiblyGainNewTrait(girl, "Elegant", 40, ACTION_WORKESCORT, "Playing the doting girlfriend has given " + girl->m_Realname + " an Elegant nature.", DayNight != 0);

	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Nervous", 40, ACTION_WORKESCORT, girl->m_Realname + " seems to finally be getting over her shyness. She's not always so Nervous anymore.", DayNight != 0);
	g_Girls.PossiblyLoseExistingTrait(girl, "Aggressive", 70, ACTION_WORKESCORT, "Controlling her temper has greatly reduced " + girl->m_Realname + "'s Aggressive tendencies.", DayNight != 0);

	return false;
}