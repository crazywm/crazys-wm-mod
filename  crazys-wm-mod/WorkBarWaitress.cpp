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

// `J` Brothel Job - Bar
bool cJobManager::WorkBarWaitress(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	string message = ""; string girlName = girl->m_Realname;

	if(Preprocessing(ACTION_WORKBAR, girl, brothel, Day0Night1, summary, message)) 		return true;

	
	g_Girls.UnequipCombat(girl);  // put that shit away, you'll scare off the customers!

	sGirl* barmaidonduty = NULL;
	string barmaidname = "the Barmaid";	// Who?
	vector<sGirl *> barmaid = g_Brothels.GirlsOnJob(0, JOB_BARMAID, Day0Night1);
	if (barmaid.size() > 0) barmaidonduty = barmaid[g_Dice%barmaid.size()];
	if (barmaidonduty)	barmaidname = "Barmaid " + barmaidonduty->m_Realname + "";
	else barmaidname = "";	// no barmaid

	int wages = 15, work = 0;
	message += "She worked as a waitress in the bar.\n";

	int roll = g_Dice%100;
	int jobperformance = (	g_Girls.GetStat(girl, STAT_INTELLIGENCE)/2 + 
							g_Girls.GetStat(girl, STAT_AGILITY)/2 +
							g_Girls.GetSkill(girl, SKILL_SERVICE));

	//good traits
	if (g_Girls.HasTrait(girl, "Charismatic"))  jobperformance += 15;
	if (g_Girls.HasTrait(girl, "Sexy Air"))		jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Cool Person"))  jobperformance += 10; //people love to be around her	
	if (g_Girls.HasTrait(girl, "Cute"))			jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Charming"))		jobperformance += 20; //people like charming people
	if (g_Girls.HasTrait(girl, "Quick Learner"))jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Psychic"))		jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Fleet of Foot")) jobperformance += 5;//faster at taking orders and droping them off
	if (g_Girls.HasTrait(girl, "Waitress"))		jobperformance += 40;
		

	//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))			  jobperformance -= 50; //needs others to do the job	
	if (g_Girls.HasTrait(girl, "Clumsy")) 				  jobperformance -= 20; //spills food and breaks things often
	if (g_Girls.HasTrait(girl, "Aggressive"))			  jobperformance -= 20; //gets mad easy and may attack people	
	if (g_Girls.HasTrait(girl, "Nervous"))				  jobperformance -= 30; //don't like to be around people	
	if (g_Girls.HasTrait(girl, "Abnormally Large Boobs")) jobperformance -= 20; //boobs are to big and get in the way	
	if (g_Girls.HasTrait(girl, "Meek"))					  jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Slow Learner"))			  jobperformance -= 10;
	if (g_Girls.HasTrait(girl, "One Eye"))				  jobperformance -= 10;


	 if(jobperformance >= 245)
		{
			message += " She's the perfect waitress. Customers go on about her and many seem to come more for her than for the drinks or entertainment.\n\n";
			wages += 155;

		if (roll <= 14)
			{
				message += girlName + " danced around the bar dropping orders off as if she didn't even have to think about it.\n";
			}
		else if (roll <= 28)
			{
				message +=  "Knowing how to speak with customers, " + girlName + " always gets the clients to order something extra or more pricy then they wanted at the beginning, making you a tidy profit.\n";
				wages += 10;
			}
		else if (roll <= 42)
			{
				message += "Being a very popular waitress made " + girlName + " some fans, that come here only to be served by her. On the other hand they leave generous tips behind.\n";
				brothel->m_Fame += 10;
				wages += 10;
			}
		else if (roll <= 56)
			{
				message += "Her shift past smoothly, earning her some really juicy tips.\n";
				wages += 15;
			}
		else if (roll <= 70)
			{
				message += "Memorizing the whole menu and prices, taking out three or four trays at once, not making a single mistake for days. " + girlName + " sure doesn't seem human.\n";
			}
		else if (roll <= 84)
			{
				message +=  "Today she saved your place from a major disturbance. When greeting clients, " + girlName + " noticed that the newly arrived group was part of a gang that was at war with another group of men which were already inside your place. She politely apologized and explained that your place was full and couldn't take such a large group. The men left unhappy but without giving her any trouble.\n";
			}
		else
			{
				message += "People came in from everywhere to see " + girlName + " work.  She bounces all around the bar laughing and keeping the patrons happy without messing anything up.\n";
				brothel->m_Happiness += 10;
			}
 }
 else if(jobperformance >= 185)
		{
			message += " She's unbelievable at this and is always getting praised by the customers for her work.\n\n";
			wages += 95;

		if (roll <= 16)
			{
				message += girlName + " bounced from table to table taking orders and recommending items to help you make more money.\n";
				wages += 10;
			}
		else if (roll <= 32)
			{
				message +=  "She's very good at this. You saw her several times today carrying out two orders at a time.\n";
			}
		else if (roll <= 48)
			{
				message += "When placing drink on the table she got slapped on the ass by one of the customers. " + girlName + " scolded them in a flirty way, saying that this kind of behavior could get them kicked out by the security and that she didn't want to lose her favorite customers. The client apologies, assuring her that he didn't mean any harm.\n";
			}
		else if (roll <= 64)
			{
				message += "Knowing the menu paid off for " + girlName + ". After hearing the order she advised a more expensive option. Customers went for it and enjoyed their stay. Happy with the good advice they left her a great tip.\n";
				wages += 10;
			}
		else if (roll <= 83)
			{
				message += "Memorizing the menu and prices has paid off for " + girlName + ".\n";
			}
		else
			{
				message += girlName + " is a town favourite and knows most of the patrons by name and what they order.\n";
				brothel->m_Happiness += 5;
			}
		}
 else if(jobperformance >= 145)
		{
			message += " She's good at this job and gets praised by the customers often.\n\n";
			wages += 55;

		if (roll <= 14)
			{   //SIN - implement code for new boob traits
			if (g_Girls.HasTrait(girl, "Big Boobs") || g_Girls.HasTrait(girl, "Giant Juggs") || g_Girls.HasTrait(girl, "Massive Melons"))
				{
					message += "The patrons love being served by " + girlName + ".  Due to her skill at this job and the chance to stare at her Big Boobs.\n";
				}
			else if (g_Girls.HasTrait(girl, "Abnormally Large Boobs") || g_Girls.HasTrait(girl, "Titanic Tits"))
					{
						message += "The patrons love being served by " + girlName + ".  Due to her skill at this job and the chance to stare at her legendary boobs.\n";
					}
				else
					{
						message += girlName + " didn't mess up any order this shift.  Keeping the patrons happy.\n";
						brothel->m_Happiness += 5;
					}
			}
		else if (roll <= 28)
			{
				message +=  "One mishap today. A customers get away without paying!\n";
				wages -= 10;
			}
		else if (roll <= 42)
			{
				message += "She spends her shift greeting customers and showing them to their tables.\n";
			}
		else if (roll <= 56)
			{
				message += "Being confident in her skill, " + girlName + " didn't make a single mistake today. She also earned some tips from happy customers.\n";
				wages += 10;
			}
		else if (roll <= 70)
			{
				message += "Having a slow shift, she mostly gossip with other staff members.\n";
			}
		else if (roll <= 84)
			{
				message +=  "Ensuring that every table was served, tired " + girlName + " took a five minute breather.\n";
			}
		else
			{
				message += girlName + " had some regulars come in. She knows there order by heart and put it in as soon as she seen them walk in making them happy.\n";
				brothel->m_Happiness += 5;
			}
		}
 else if(jobperformance >= 100)
		{
			message += " She made a few mistakes but overall she is okay at this.\n\n";
			wages += 15;

		if (roll <= 14)
			{
				message += girlName + " forgot to take an order to a table for over an hour.  But they were in a forgiving mood and stuck around.\n";
			}
		else if (roll <= 28)
			{
				message +=  "Trying her best, " + girlName + " made few mistakes but otherwise she had a pleasant day.\n";
				work += 1;
			}
		else if (roll <= 42)
			{
				message += "She tripped over her own shoelaces when carrying out an expensive order!\n";
				brothel->m_Filthiness += 5;
			}
		else if (roll <= 56)
			{
				message += "Taking orders without mistakes and getting drinks to the tables not spilling a single drop from them. Today was a good day for " + girlName + ".\n";
				work += 1;
			}
		else if (roll <= 70)
			{
				message += "When placing drink on the table she got slapped on the ass by one of the customers. " + girlName + " didn't ignore that and called him names. Her behavior left the clients stunned for a moment.\n";
			}
		else if (roll <= 84)
			{
				message += girlName + " spent her shift on cleaning duty. Mopping the floor, wiping tables and ensuring that the bathrooms were accessible. She did a decent job.\n";
				brothel->m_Filthiness -= 5;
			}
		else
			{
				message += girlName + " sneezed in an order she had just sat down on a table.  Needless to say the patron was mad and left.\n";
				brothel->m_Happiness -= 5;
			}
		}
 else if(jobperformance >= 70)
		{
			message += " She was nervous and made a few mistakes. She isn't that good at this.\n\n";
			wages -= 5;
		if (roll <= 14)
			{
				message += girlName + " wrote down the wrong orders for a few patrons resulting in them leaving.\n";
				brothel->m_Happiness -= 5;
			}
		else if (roll <= 28)
			{
				message +=  "After being asked for the fourth time to repeat his order, the irritated customer left your facility. " + girlName + " clearly isn't very good at this job.\n";
				brothel->m_Happiness -= 5;
			}
		else if (roll <= 42)
			{
				message += "Giving back change to a customer, " + girlName + " made an error calculating the amount in favor of the client. \n";
				wages -= 5;
			}
		else if (roll <= 56)
			{
				message += "Holding the tray firmly in her hands, " + girlName + " successfully delivered the order to designated table. She was so nervous and focused on not failing this time, that she jumped scared when the customer thanked her.\n";
			}
		else if (roll <= 70)
			{
				message += girlName + " really hates this job. She used every opportunity to take a break.\n";
			}
		else if (roll <= 84)
			{
				message += "Still learning to do her job, " + girlName + " gets some orders wrong making a lot of people really angry.\n";
				brothel->m_Happiness -= 5;
			}
		else
			{
				message += girlName + " sneezed in an order she had just sat down on a table.  Needless to say the patron was mad and left.\n";
				brothel->m_Happiness -= 5;
			}
		}
 else
		{
			message += " She was nervous and constantly making mistakes. She really isn't very good at this job.\n\n";
			wages -= 15;
		if (roll <= 14)
			{
				message += girlName + " was taking orders to the wrong tables and letting a lot of people walk out without paying their tab.\n";
			}
		else if (roll <= 28)
			{
				message += girlName + " was presenting the ordered dish when she sneezed in it. The outraged customer demanded a new serving, that he got on the house.\n";
				brothel->m_Happiness -= 5;
				wages -= 15;
			}
		else if (roll <= 42)
			{
				message += "The tray slipped from " + girlName + "'s hand right in front of the patron. Causing her to get yelled at for being bad at her job.\n";
				brothel->m_Filthiness += 5;
			}
		else if (roll <= 56)
			{
				message += "Trying her best, " + girlName + " focused on not screwing up today. Surprisingly she managed not to fail at one of her appointed tasks today.\n";
			}
		else if (roll <= 70)
			{
				message += "After picking up a tray full of drinks from the bar, " + girlName + " tried to bring it to the table. Her attempt failed when she slipped on wet floor that she mopped a minute ago.\n";
				brothel->m_Filthiness += 5;
			}
		else if (roll <= 84)
			{
				message += girlName + " was slacking on the job spending most of her shift chatting with other staff members.\n";
			}
		else
			{
				message += girlName + " spilled food all over the place and mixed orders up constantly.\n";
				brothel->m_Filthiness += 5;
			}
		}
		

	//try and add randomness here
	if (g_Girls.GetStat(girl, STAT_BEAUTY) > 85 && g_Dice.percent(20))
	{ message += "Stunned by her beauty a customer left her a great tip.\n\n"; wages += 25; }

	if (g_Girls.HasTrait(girl, "Clumsy") && g_Dice.percent(15))
		{ message += "Her clumsy nature cause her to spill food on a customer resulting in them storming off without paying.\n"; wages -= 25; }

	if (g_Girls.HasTrait(girl, "Pessimist") && g_Dice.percent(5))
	{
		if (jobperformance < 125)
			{ message += "Her pessimistic mood depressed the customers making them tip less.\n"; wages -= 10; }
		else
			{ message += girlName + " was in a poor mood so the patrons gave her a bigger tip to try and cheer her up.\n"; wages += 10; }
	}

	if (g_Girls.HasTrait(girl, "Optimist") && g_Dice.percent(5))
	{
		if (jobperformance < 125)
			{ message += girlName + " was in a cheerful mood but the patrons thought she needed to work more on her services.\n"; wages -= 10; }
		else
			{ message += "Her optimistic mood made patrons cheer up increasing the amount they tip.\n"; wages += 10; }
	}

	if (g_Girls.HasTrait(girl, "Psychic") && g_Dice.percent(20))
		{ message += "She used her Psychic skills to know exactly what the patrons wanted to order making them happy and increasing her tips.\n"; wages += 15; }

	if (g_Girls.HasTrait(girl, "Great Arse") && g_Dice.percent(15))
	{
		if (jobperformance >= 185) //great
			{ message += "A patron reached out to grab her ass. But she skillfully avoided it with a laugh and told him that her ass wasn't on the menu.  He laughed so hard he increased her tip\n"; wages += 15; }
		else if (jobperformance >= 135) //decent or good
			{ message += "A patron reached out and grabed her ass. She's use to this and skilled enough so she didn't drop anything\n"; }
		else if (jobperformance >= 85) //bad
			{ message += "A patron reached out and grabed her ass. She was startled and ended up dropping half an order.\n"; wages -= 10; }
		else  //very bad
			{ message += "A patron reached out and grabed her ass. She was startled and ended up dropping a whole order\n"; wages -= 15; }
	}

	if (g_Girls.HasTrait(girl, "Assassin") && g_Dice.percent(5))
	{
		if(jobperformance < 150)
			{ message += "A patron pissed her off and using her Assassin skills she killed him before even thinking about it resulting in patrons storming out without paying.\n"; wages -= 50; }
		else
			{ message += "A patron pissed her off but she just gave them a death stare and walked away.\n"; }
	}

	if (g_Girls.HasTrait(girl, "Horrific Scars") && g_Dice.percent(15))
	{
		if (jobperformance < 150)
			{ message += "A patron gasped at her Horrific Scars making her uneasy. But they didn't feel sorry for her.\n"; }
		else
			{ message += "A patron gasped at her Horrific Scars making her sad. Feeling bad about it as she did a wonderful job they left a good tip.\n"; wages += 25; }
	}

	if (g_Girls.HasTrait(girl, "Nymphomaniac") && g_Girls.GetStat(girl, STAT_LIBIDO) > 90)
	{
		message += "During her shift, " + girlName + " couldn't help but instinctively and excessively rub her ass against the crotches of the clients whenever she got the chance. Her slutty behavior earned her some extra tips, as a couple of patrons noticed her intentional butt grinding.\n";
		wages += 30;
	}

	if (g_Dice.percent(5))
	{
		/*if (g_Girls.GetSkill(girl, SKILL_MEDICINE) >= 90)
		{ message += "She used her Psychic skills to know excatally what the patrons wanted to order making them happy and increasing her tips.\n"; wages += 15; }
		else if (g_Girls.GetSkill(girl, SKILL_MEDICINE) >= 60)
		{ message += "She used her Psychic skills to know excatally what the patrons wanted to order making them happy and increasing her tips.\n"; wages += 15; }
		else*/ if (g_Girls.GetSkill(girl, SKILL_MEDICINE) >= 30)
		{ message += "A customer started chocking on his food so " + girlName + " performed the heimlich maneuver on him. Grateful the man left her a better tip.\n"; wages += 15; }
		else
		{ message += "A customer started chocking on his food so " + girlName + " not knowing what to do started screaming for help.\n"; }
	}

	if (g_Girls.GetSkill(girl, SKILL_HERBALISM) >= 40 && g_Dice.percent(5))
	{ message += "Added a litte something extra to the patrons order to spice it up. They enjoyed it greatly and she recieved some nice tips.\n\n"; wages += 25; }

	if (g_Brothels.GetNumGirlsOnJob(0,JOB_BARMAID,false) >= 1 && g_Dice.percent(25))
	{
		if (jobperformance < 125)
			{ message += girlName + " wasn't good enough at her job to use " + barmaidname + " to her advantage.\n"; }
		else
			{ message += girlName + " used " + barmaidname + " to great effect speeding up her work and increasing her tips.\n"; wages += 25; }
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

	g_Girls.UpdateEnjoyment(girl, ACTION_WORKBAR, work, true);
	girl->m_Events.AddMessage(message, IMGTYPE_WAIT, Day0Night1);

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
		g_Girls.UpdateStat(girl, STAT_INTELLIGENCE, 1);
	else 
		g_Girls.UpdateStat(girl, STAT_AGILITY, 1);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, g_Dice%skill+1);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);
	
	//gain traits
	g_Girls.PossiblyGainNewTrait(girl, "Charming", 70, ACTION_WORKBAR, girlName + " has been flirting with customers to try to get better tips. Enough practice at it has made her quite Charming.", Day0Night1 == SHIFT_NIGHT);
	if (jobperformance > 150 && g_Girls.GetStat(girl, STAT_CONSTITUTION) > 65) { g_Girls.PossiblyGainNewTrait(girl, "Fleet of Foot", 60, ACTION_WORKBAR, girlName + " has been doding bewteen tables and avoiding running into customers for so long she has become Fleet of Foot.", Day0Night1 == SHIFT_NIGHT); }

	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Clumsy", 30, ACTION_WORKBAR, "It took her breaking hundreds of dishes, and just as many reprimands, but " + girlName + " has finally stopped being so Clumsy.", Day0Night1 == SHIFT_NIGHT);
	return false;
}