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
#include "cCentre.h"
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
extern cCentreManager g_Centre;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;
extern int g_Building;

// `J` Job Centre - General
bool cJobManager::WorkFeedPoor(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKCENTRE;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	if (g_Girls.DisobeyCheck(girl, ACTION_WORKCENTRE, brothel))			// they refuse to work 
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked feeding the poor.";

	g_Building = BUILDING_CENTRE;
	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	int roll = g_Dice.d100();
	bool blow = false, sex = false;
	int wages = 100, work = 0, feed = 0;

	double jobperformance = JP_FeedPoor(girl, false);


	//Adding cust here for use in scripts...
	sCustomer cust;
	GetMiscCustomer(brothel, cust);


	int dispo; // `J` merged slave/free messages and moved actual dispo change to after
	if (jobperformance >= 245)
	{
		ss << " She must be perfect at this.\n\n";
		dispo = 12;
		if (roll <= 20)
		{
			ss << "Today " << girlName << " was managing the kitchen giving orders to other cooks and checking the quality of their work.\n";
		}
		else if (roll <= 40)
		{
			ss << girlName << " was helping in the kitchen. Her task was to stir-fry vegetables. One word: Perfection. Food that she prepared was great!\n";
		}
		else if (roll <= 60)
		{
			ss << "Being done with the main dish earlier, " << girlName << " decided to bake cookies for desert!\n";
		}
		else if (roll <= 80)
		{
			ss << "Excellent dish! Some world class chefs should learn from " << girlName << "!\n";
		}
		else
		{
			ss << girlName << " knife skill is impressive. She's cutting precisely and really fast, almost like a machine.\n";
		}
	}
	else if (jobperformance >= 185)
	{
		ss << " She's unbelievable at this and is always getting praised by people for her work.\n\n";
		dispo = 10;
		if (roll <= 20)
		{
			ss << girlName << " is in charge of the cooking for several weeks now. You could swear that the population of rodents and small animals in the area went down.\n";
		}
		else if (roll <= 40)
		{
			ss << "While preparing for today's cooking, " << girlName << " noticed that one of the crucial ingredients is missing. She manage to change the menu and fully use available ingredients.\n";
		}
		else if (roll <= 60)
		{
			ss << "She speedily served all in line at the food counter. All the portions handed out were equal.\n";
		}
		else if (roll <= 80)
		{
			ss << "Preparing something new she mixed up the proportions from the recipe. The outcome tasted great!\n";
		}
		else
		{
			ss << girlName << " was helping in the kitchen. Her task was to prepare the souse for today's meatballs. The texture and flavor was top notch.\n";
		}
	}
	else if (jobperformance >= 145)
	{
		ss << " She's good at this job and gets praised by people often.\n\n";
		dispo = 8;
		if (roll <= 20)
		{
			ss << "While cooking she used everything that was in the kitchen. Nothing was wasted.\n";
		}
		else if (roll <= 40)
		{
			ss << "While cooking she accidently sneezed into the pot. Luckily nobody saw that.\n";
		}
		else if (roll <= 60)
		{
			ss << girlName << " was helping in the kitchen. Her task was to carve the meat. Smile on her face that appear while doing this, was somehow scary and disturbing.\n";
		}
		else if (roll <= 80)
		{
			ss << girlName << " was doing the dishes. Most of them \"survived\" and will be used next time.\n";
		}
		else
		{
			ss << "She spent her shift at the food counter without any trouble.\n";
		}
	}
	else if (jobperformance >= 100)
	{
		ss << " She made a few mistakes but overall she is okay at this.\n\n";
		dispo = 6;
		if (roll <= 20)
		{
			ss << "Her cooking isn't very good. But You probably would risk serving the prepared dish to a dog.\n";
		}
		else if (roll <= 40)
		{
			ss << "Today she was assign as the cook. Not being able to focus on the task at hand, she overcooked the pasta.\n";
		}
		else if (roll <= 60)
		{
			ss << "She served all in line at the food counter. Some portions were smaller than others.\n";
		}
		else if (roll <= 80)
		{
			ss << girlName << " was helping in the kitchen. Her task was to make a salad. She manage to do this much.\n";
		}
		else
		{
			ss << "When walking with a pile of clean dished she slipped and fall. All the plates got broken.\n";
		}
	}
	else if (jobperformance >= 70)
	{
		ss << " She was nervous and made a few mistakes. She isn't that good at this.\n\n";
		dispo = 4;
		if (roll <= 20)
		{
			ss << "Today she was assign as the cook. Meatballs that she prepared could be used as lethal projectiles.\n";
		}
		else if (roll <= 40)
		{
			ss << girlName << " was doing the dishes. Let's say it would be a lot quicker to throw them all right away, then wait for her to brake so many during this simple chore.\n";
		}
		else if (roll <= 60)
		{
			ss << "While cooking she burned two, brand new pots!\n";
		}
		else if (roll <= 80)
		{
			ss << girlName << " was helping in the kitchen. Her task was to dice the carrots. Surely the carrots were cut, but to called them diced would be an exaggeration.\n";
		}
		else
		{
			ss << "Just wanting to serve the food fast to end her shift. She did a sloppy job at the food counter.\n";
		}
	}
	else
	{
		ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.\n\n";
		dispo = 2;
		if (roll <= 20)
		{
			ss << "While preparing ingredients for the soup she almost cut off her hand!\n";
		}
		else if (roll <= 40)
		{
			ss << girlName << " was helping in the kitchen. Her task was to peel the potatoes. The peels were thick and had a lot of the vegetable left on them. What a waste!\n";
		}
		else if (roll <= 60)
		{
			ss << "Today she was assignd as the cook. The thing that she created hardly could be called food.\n";
		}
		else if (roll <= 80)
		{
			ss << "Today she was assignd to work at the food counter. While handing out food she served different portions to people in line.\n";
		}
		else
		{
			ss << "Being assign to the food counter, she putted a sign \"self serving\" and went out.\n";
		}
	}


	//try and add randomness here
	if (g_Girls.GetStat(girl, STAT_INTELLIGENCE) < 55 && g_Dice.percent(30))//didnt put a check on this one as we could use some randomness and its an intel check... guess we can if people keep bitching
	{
		blow = true;	ss << "An elderly fellow managed to convince " << girlName << " that he was full and didn't need anymore food but that she did. He told her his cock gave a special treat if she would suck on it long enough. Which she did man she isn't very smart.\n\n";
	}

	if (g_Girls.HasTrait(girl, "Nymphomaniac") && g_Dice.percent(30) && g_Girls.GetStat(girl, STAT_LIBIDO) > 75
		&& !g_Girls.HasTrait(girl, "Lesbian") && !g_Girls.HasTrait(girl, "Virgin")
		&& !brothel->m_RestrictNormal)
	{
		sex = true;
		ss << "Her Nymphomania got the better of her today and she decide to let them eat her pussy!  After a few minutes they started fucking her.\n";
	}



	if (girl->m_States&(1 << STATUS_SLAVE))
	{
		ss << "\nThe fact that she is your slave makes people think its less of a good deed on your part.";
		g_Brothels.GetPlayer()->disposition(dispo);
	}
	else
	{
		ss << "\nThe fact that your paying this girl to do this helps people think your a better person.";
		girl->m_Pay = wages;
		g_Gold.staff_wages(100);  // wages come from you
		g_Brothels.GetPlayer()->disposition(int(dispo*1.5));
	}



	//enjoyed the work or not
	if (roll <= 5)
	{
		ss << "\nSome of the patrons abused her during the shift."; work -= 1;
	}
	else if (roll <= 25)
	{
		ss << "\nShe had a pleasant time working."; work += 3;
	}
	else
	{
		ss << "\nOtherwise, the shift passed uneventfully."; work += 1;
	}

	g_Girls.UpdateEnjoyment(girl, ACTION_WORKCENTRE, work, true);

	if (sex)
	{
		if (roll <= 50 && !brothel->m_RestrictNormal)
		{
			girl->m_Events.AddMessage(ss.str(), IMGTYPE_SEX, Day0Night1);
			g_Girls.UpdateSkill(girl, SKILL_NORMALSEX, 2);
			if (g_Girls.CheckVirginity(girl))
			{
				g_Girls.LoseVirginity(girl);	// `J` updated for trait/status
				ss << "She is no longer a virgin.\n";
			}
			if (!girl->calc_pregnancy(&cust, false, 1.0))
			{
				g_MessageQue.AddToQue(girl->m_Realname + " has gotten pregnant", 0);
			}
		}
		else if (!brothel->m_RestrictAnal)
		{
			girl->m_Events.AddMessage(ss.str(), IMGTYPE_ANAL, Day0Night1);
			g_Girls.UpdateSkill(girl, SKILL_ANAL, 2);
		}
		brothel->m_Happiness += 100;
		g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, -20);
		g_Girls.UpdateEnjoyment(girl, ACTION_SEX, +3, true);
		dispo += 6;
	}
	else if (blow)
	{
		brothel->m_Happiness += (g_Dice % 70) + 60;
		dispo += 4;
		g_Girls.UpdateSkill(girl, SKILL_ORALSEX, 2);
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_ORAL, Day0Night1);
	}
	else
	{
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);
	}

	feed += (int)(jobperformance / 10);		//  1 feed per 10 point of performance

	int cost = 0;
	for (int i = 0; i < feed; i++)
	{
		cost += g_Dice % 3 + 2; // 2-5 gold per customer
	}
	brothel->m_Finance.centre_costs(cost);
	ss.str("");
	ss << girlName << " feed " << feed << " costing you " << cost << " gold.";
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);


	// Improve stats
	int xp = 10, libido = 1, skill = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }


	g_Girls.UpdateStat(girl, STAT_FAME, 1);
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	if (g_Dice % 2)
		g_Girls.UpdateStat(girl, STAT_INTELLIGENCE, 1);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, skill);
	g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, libido);

	return false;
}

double cJobManager::JP_FeedPoor(sGirl* girl, bool estimate)// not used
{
	double jobperformance =
		((g_Girls.GetStat(girl, STAT_INTELLIGENCE) / 2) + 
		(g_Girls.GetStat(girl, STAT_CHARISMA) / 2) + 
		g_Girls.GetSkill(girl, SKILL_SERVICE));

	//good traits
	if (g_Girls.HasTrait(girl, "Charismatic"))  jobperformance += 20;
	if (g_Girls.HasTrait(girl, "Sexy Air"))		jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Cool Person"))  jobperformance += 10;  //people love to be around her
	if (g_Girls.HasTrait(girl, "Cute"))			jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Charming"))		jobperformance += 15;  //people like charming people
	if (g_Girls.HasTrait(girl, "Optimist"))		jobperformance += 10;


	//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))    jobperformance -= 50; //needs others to do the job
	if (g_Girls.HasTrait(girl, "Clumsy"))		jobperformance -= 20; //spills food and breaks things often
	if (g_Girls.HasTrait(girl, "Aggressive"))   jobperformance -= 20; //gets mad easy and may attack people
	if (g_Girls.HasTrait(girl, "Nervous"))		jobperformance -= 30; //don't like to be around people
	if (g_Girls.HasTrait(girl, "Meek"))			jobperformance -= 20;

	return jobperformance;
}
