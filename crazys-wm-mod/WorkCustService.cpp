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
extern cGold g_Gold;

// `J` Job Brothel - General
bool cJobManager::WorkCustService(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKCUSTSERV;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	if (girl->disobey_check(actiontype, brothel))
	{
		//SIN - More informative mssg to show *what* she refuses
		//ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		ss << " refused to provide Customer Service in your brothel " << (Day0Night1 ? "tonight." : "today.");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked as Customer Service.\n \n";

	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	// Note: Customer service needs to be done last, after all the whores have worked.

	int numCusts = 0; // The number of customers she can handle
	int serviced = 0;

	// Complications
	int roll = g_Dice%100;
	if (roll <= 5)
	{
		ss << "Some of the patrons abused her during the shift.";
		girl->upd_Enjoyment(actiontype, -1);
	}
#if 1
	if (roll <= 15)
	{
		ss << "A customer mistook her for a whore and was abusive when she wouldn't provide THAT service.";
		girl->upd_Enjoyment(actiontype, -1);
	}
#endif
	else if (roll >= 75) {
		ss << "She had a pleasant time working.";
		girl->upd_Enjoyment(actiontype, +3);
	}
	else
	{
		ss << "The shift passed uneventfully.";
	}
	// Decide how many customers the girl can handle
	if (girl->confidence() > 0)
		numCusts += girl->confidence() / 10; // 0-10 customers for confidence
	if (girl->spirit() > 0)
		numCusts += girl->spirit() / 20; // 0-5 customers for spirit
	if (girl->service() > 0)
		numCusts += girl->service() / 25; // 0-4 customers for service
	numCusts++;
	// A single girl working customer service can take care of 1-20 customers in a week.
	// So she can take care of lots of customers. It's not like she's fucking them.

	// Add a small amount of happiness to each serviced customer
	// First, let's find out what her happiness bonus is
	int bonus = 0;
	if (girl->charisma() > 0)
		bonus += girl->charisma() / 20;
	if (girl->beauty() > 0)
		bonus += girl->beauty() / 20;
	// Beauty and charisma will only take you so far, if you don't know how to do service.
	if (girl->performance() > 0)			// `J` added
		bonus += girl->performance() / 20;
	if (girl->service() > 0)
		bonus += girl->service() / 20;
	// So this means a maximum of 20 extra points of happiness to each
	// customer serviced by customer service, if a girl has 100 charisma,
	// beauty, performance and service.

	// Let's make customers angry if the girl sucks at customer service.
	if (bonus < 5)
	{
		bonus = -20;
		ss << "\n \nHer efforts only made the customers angrier.";
		//And she's REALLY not going to like this job if she's failing at it, so...
		girl->upd_Enjoyment(actiontype, -5);
	}

	// Now let's take care of our neglected customers.
	for (int i=0; i<numCusts; i++)
	{
		if (g_Customers.GetNumCustomers() > 0)
		{
            sCustomer Cust = g_Customers.GetCustomer(*brothel);
			// Let's find out how much happiness they started with.
			// They're not going to start out very happy. They're seeing customer service, after all.
			Cust.m_Stats[STAT_HAPPINESS] = 22 + g_Dice%10 + g_Dice%10; // average 31 range 22 to 40
			// Now apply her happiness bonus.
			Cust.m_Stats[STAT_HAPPINESS] += bonus;
			// update how happy the customers are on average
			brothel->m_Happiness += Cust.m_Stats[STAT_HAPPINESS];
			// And decrement the number of customers to be taken care of
			g_Customers.AdjustNumCustomers(-1);
			serviced++;
		}
		else
		{
			//If there aren't enough customers to take care of, time to quit.
			girl->m_Events.AddMessage(girl->m_Realname + " ran out of customers to take care of.", IMGTYPE_PROFILE, Day0Night1);
			break;
		}
	}
	// So in the end, customer service can take care of lots of customers, but won't do it
	// as well as good service from a whore. This is acceptable to me.
	ss << "\n \n" << girlName << " took care of " << serviced << " customers this shift.";

	/* Note that any customers that aren't handled by either customer service or a whore count as a 0 in the
	 * average for the brothel's customer happiness. So customer service leaving customers with 27-60 in their
	 * happiness stat is going to be a huge impact. Again, not as good as if the whores do their job, but better
	 * than nothing. */

	// Bad customer service reps will leave the customer with 2-20 happiness. Bad customer service is at least better than no customer service.
#if 0
	string debug = "";
	debug += ("There were " + intstring(g_Customers.GetNumCustomers()) + " customers.\n");
	debug += ("She could have handled " + intstring(numCusts) + " customers.\n");
	girl->m_Events.AddMessage(debug, IMGTYPE_PROFILE, EVENT_DEBUG);
#endif
	// Now pay the girl.

	girl->m_Pay += 50;
	g_Gold.staff_wages(50);  // wages come from you
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);

	// Raise skills
	int xp = 5 + (serviced / 5), libido = 1, skill = 2 + (serviced / 10);

	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }

	girl->fame(1);
	girl->exp(xp);
	int gain = g_Dice % skill;
	if (gain == 1)		girl->confidence(g_Dice%skill);
	else if(gain == 2)	girl->spirit(g_Dice%skill);
	else				girl->performance(g_Dice%skill);
	girl->service(g_Dice%skill+1);
	girl->upd_temp_stat(STAT_LIBIDO, libido);

	return false;
}

double cJobManager::JP_CustService(sGirl* girl, bool estimate)
{
#if 1	//SIN - standardizing job performance per J's instructs
	double jobperformance =
		//main stats - first 100
		((girl->service() + girl->charisma()) / 2) +
		//secondary stats - second 100
		((girl->confidence() + girl->spirit() + girl->performance() + girl->beauty()) / 4) +
		//add level
		girl->level();
#else
	double jobperformance = 0.0;
	if (estimate)	// for third detail string
	{
		jobperformance += (girl->confidence() + girl->spirit() + girl->beauty() +
			girl->charisma() + girl->performance() + girl->service()) / 3;
	}
	else			// for the actual check			// not used
	{

	}
#endif
	//tiredness penalty
	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}

	////////SIN - and traits
	//Good traits
	if (girl->has_trait( "Charismatic"))					jobperformance += 20;	//good with people
	if (girl->has_trait( "Psychic"))						jobperformance += 15;	//knows what will make them happy
	if (girl->has_trait( "Porn Star"))					jobperformance += 15;	//famous for related reasons
	if (girl->has_trait( "Succubus"))						jobperformance += 15;	//likes to sexually entertain
	if (girl->has_trait( "Titanic Tits"))					jobperformance += 10;	//huge distraction for customer not getting laid
	if (girl->has_trait( "Abnormally Large Boobs"))		jobperformance += 10;	//huge distraction for customer not getting laid
	if (girl->has_trait( "Natural Pheromones"))			jobperformance += 10;   // natural turnon
	if (girl->has_trait( "Charming"))						jobperformance += 10;	//good with people
	if (girl->has_trait( "Sexy Air"))						jobperformance += 10;	// natural turnon
	if (girl->has_trait( "Idol"))							jobperformance += 10;	//famous
	if (girl->has_trait( "Actress"))						jobperformance += 10;	//can act the part
	if (girl->has_trait( "Fallen Goddess"))				jobperformance += 10;	//like exotic but better - not too judgemental
	if (girl->has_trait( "Massive Melons"))				jobperformance += 5;	//big distraction for customer not getting laid
	if (girl->has_trait( "Giant Juggs"))					jobperformance += 5;	//big distraction for customer not getting laid
	if (girl->has_trait( "Cool Person"))					jobperformance += 5;	//good with people
	if (girl->has_trait( "Social Drinker"))				jobperformance += 5;	//gets customers merry
	if (girl->has_trait( "Exotic"))						jobperformance += 5;	//holds their interest
	if (girl->has_trait( "Cat Girl"))						jobperformance += 5;	//holds their interest
	if (girl->has_trait( "Strong Magic"))					jobperformance += 5;	//a saucier sorceror
	if (girl->has_trait( "Big Boobs"))					jobperformance += 2;	//distraction for customer not getting laid
	if (girl->has_trait( "Busty Boobs"))					jobperformance += 2;	//distraction for customer not getting laid
	if (girl->has_trait( "Cute"))							jobperformance += 2;	//cheer up customer
	if (girl->has_trait( "Optimist"))						jobperformance += 2;	//cheer up customer


	//Bad traits
	if (girl->has_trait( "Zombie"))						jobperformance -= 50;	//Guuhhh! Customehh serv... Huh?! Mr run run?!
	if (girl->has_trait( "Undead"))						jobperformance -= 50;	//Stop! I'm already dead...
	if (girl->has_trait( "Mind Fucked"))					jobperformance -= 50;	//Hello. I like flowers. Do you like vaginas? ... This!? It's a knife, silly; can I cut you? ... For FUN!
	if (girl->has_trait( "Mute"))							jobperformance -= 30;	// (-_-) ...
	if (girl->has_trait( "Fairy Dust Addict"))			jobperformance -= 25;	//Focussed on own needs
	if (girl->has_trait( "Shroud Addict"))				jobperformance -= 25;	//Focussed on own needs
	if (girl->has_trait( "Viras Blood Addict"))			jobperformance -= 25;	//Focussed on own needs
	if (girl->has_trait( "Queen"))						jobperformance -= 20;	//Service?! Oh Jeeves! Jeeves!? Where is he?! Y'know you can't get the help these days.
	if (girl->has_trait( "Blind"))						jobperformance -= 20;	//Waiting customers?! Where?!
	if (girl->has_trait( "Broken Will"))					jobperformance -= 15;	// (-_-) ...
	if (girl->has_trait( "Vampire"))						jobperformance -= 15;	//I vant to help vith your... hey come back!?
	if (girl->has_trait( "Princess"))						jobperformance -= 15;	//Me serve you?! Get away, commoner.
	if (girl->has_trait( "Aggressive"))					jobperformance -= 10;	//"Customer service. Impotent whiner says what?"
	if (girl->has_trait( "Cow Girl"))						jobperformance -= 10;	// *blinks*  ...  *chews a little* ... *blinks*
	if (girl->has_trait( "Nervous"))						jobperformance -= 10;	//C-c-customer service. Can I, like... How can I... I mean...
	if (girl->has_trait( "Goddess"))						jobperformance -= 10;	//Preachy
	if (girl->has_trait( "Angel"))						jobperformance -= 10;	//Preachy
	if (girl->has_trait( "Deaf"))							jobperformance -= 5;	//What's the problem sir? What?! WHAT!?!
	if (girl->has_trait( "Pessimist"))					jobperformance -= 5;	//I can help you, but you probably won't like it very much.
	if (girl->has_trait( "Flat Chest"))					jobperformance -= 5;	// no natural distractions


	return jobperformance;
}
