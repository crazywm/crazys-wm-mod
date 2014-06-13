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
extern cGold g_Gold;

bool cJobManager::WorkHallDealer(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";
	string girlName = girl->m_Realname;
	if(Preprocessing(ACTION_WORKHALL, girl, brothel, DayNight, summary, message))	// they refuse to have work in the hall
		return true;

	// put that shit away, you'll scare off the customers!
	g_Girls.UnequipCombat(girl);

	int roll = g_Dice%100;
	int jobperformance = (	g_Girls.GetStat(girl, STAT_INTELLIGENCE)/2 + 	// intel makes her smart enough to know when to cheat
							g_Girls.GetStat(girl, STAT_AGILITY)/2 +			// agility makes her fast enough to cheat
							g_Girls.GetSkill(girl, SKILL_PERFORMANCE)/2 +	// performance helps her get away with it
							g_Girls.GetSkill(girl, SKILL_SERVICE)/2);
	int wages = 25;

	message += "She worked as a dealer in the gambling hall.";


	//good traits
	if (g_Girls.HasTrait(girl, "Charismatic"))  //
		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Sexy Air"))  //
		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Cool Person"))  //people love to be around her
		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Cute"))  //
		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Charming"))  //people like charming people
		jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Quick Learner"))  //
		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Psychic"))
		jobperformance += 15;

	//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))  //needs others to do the job
		jobperformance -= 50;
	if (g_Girls.HasTrait(girl, "Clumsy"))  //spills food and breaks things often
		jobperformance -= 10;
	if (g_Girls.HasTrait(girl, "Aggressive"))  //gets mad easy and may attack people
		jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Nervous"))  //don't like to be around people
		jobperformance -= 30;
	if (g_Girls.HasTrait(girl, "Meek"))
		jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Slow Learner"))
		jobperformance -= 15;


	if (jobperformance >= 245)
		{
			message += " She must be the perfect dealer she never losses and customers hate to see her at the tables cause they know they won't win.\n\n";
			wages += 155;
		if (roll <= 33)
			{
				if (g_Girls.HasTrait(girl, "Big Boobs") || g_Girls.HasTrait(girl, "Abnormally Large Boobs"))
					{
					message += "Between her exceptional card skills and her massive tits, " + girl->m_Realname + " raked the money in this shift.\n";
					}
				else if (g_Girls.HasTrait(girl, "Lolita"))
					{
					message += "Behind her small frame and innocent face lurks a true card-shark.\n";
					}
				else
					{
					message += girlName + " is as near to perfect as any being could get.  She made a pile of money today.\n";
				}
			}
		else if (roll <= 66)
			{
				if (g_Girls.HasTrait(girl, "Sexy Air"))
					{
					message += "Her sexy body draws gamblers to her table like flies to a pitcher plant.\n";
					}
				else
					{
				message += girlName + " managed to win every game she played in today.\n";
				}
			}
		else 
			{
				message += "A master of card-counting, the other players had no chance when " + girlName + " joined them this shift.\n";
			}
		}
else if (jobperformance >= 185)
		{
			message += " She's unbelievable at this and is always finding new ways to beat the customer.\n\n";
			wages += 95;
		if (roll <= 20)
			{
				message += girlName + "'s a skilled card dealer, and turned a substantial profit today.\n";
			}
		else if (roll <= 40)
			{
				if (g_Girls.HasTrait(girl, "Sexy Air"))
					{
					message += girlName + "'s sex appeal is paying off in a different way, as the profits from her table tumble in.\n";
					}
				else
					{
				message += "She won all of her games bar one or two today!.\n";
				}
			}
		else if (roll <= 60)
			{
				if (g_Girls.HasTrait(girl, "Quick Learner"))
					{
					message += "After a good deal of practical education, " + girlName + " is a formidable card dealer.\n";
					}
				else
					{
				message += girlName + " could find a place in any gambling institution with her skills with cards.\n";
				}
			}
		else if (roll <= 80)
			{
				message += "A fat merchant nearly had a heart attack after losing this month's profits to " + girlName + ".\n";
			}
		else
			{
				if (g_Girls.HasTrait(girl, "Lolita"))
					{
					message += "Lured into a false sense of security, the gamblers were shocked to lose to such a child-like woman!\n";
					}
				else if (g_Girls.HasTrait(girl, "Big Boobs") || g_Girls.HasTrait(girl, "Abnormally Large Boobs"))
					{
					message += "Distracted by " + girlName + "'s breasts, players didn't even seem to notice their money vanishing.\n";
					}
				else
				{
				message +=  "You flash a congratulatory smile at " + girlName + " on her way out the door at the end of shift.\n";
				}
			}
		}
else if (jobperformance >= 145)
		{
			message += " She's good at this job and knows a few tricks to win.\n\n";
			wages += 55;
		if (roll <= 20)
			{
				message += girlName + "'s a fairly good card dealer, and turned a profit today.\n";
			}
		else if (roll <= 40)
			{
				if (g_Girls.HasTrait(girl, "Lolita"))
					{
					message += "Nobody expected such a sweet little girl to win anything!\n";
					}
				else if (g_Girls.HasTrait(girl, "Big Boobs") || g_Girls.HasTrait(girl, "Abnormally Large Boobs"))
					{
					message += "While she's a good card dealer, " + girlName + "'s big tits helped weigh the odds in her favor.\n";
					}
				else
					{
					message += "Her professional smile and pleasing form reinforced her acceptable skill level.\n";
				}
			}
		else if (roll <= 60)
			{
				if (g_Girls.HasTrait(girl, "Quick Learner"))
					{
					message += "Using tricks learned before from her past, " + girlName + " had a productive shift.\n";
					}
				else
					{
				message += " Lady Luck seems to be smiling on " + girlName + " today - she won more games then she lost.\n";
				}
			}
		else if (roll <= 80)
			{
				message += "Most of the patrons that sat down at " + girlName + "'s table today rose just a bit lighter.\n";
			}
		else
			{
				if (g_Girls.HasTrait(girl, "Sexy Air"))
					{
					message += "The gamblers always seem surprised that such a lovely piece of ass can beat them at their chosen game.\n";
					}
				else
					{
				message += girlName + " shows real promise as a dealer.\n";
				}
			}
		}
else if (jobperformance >= 100)
		{
			message += " She made a few mistakes but overall she is okay at this.\n\n";
			wages += 15;
		if (roll <= 20)
			{
				if (g_Girls.HasTrait(girl, "Nervous") || g_Girls.HasTrait(girl, "Meek"))
					{
					message += "Despite her uncertain nature, " + girlName + " is holding her own at the card-table.\n";
					}
				else
					{
				message += "She's no cardsharp, but " + girlName + " can hold her own against the patrons.\n";
				}
			}
		else if (roll <= 40)
			{
				if (g_Girls.HasTrait(girl, "Quick Learner"))
					{
					message += "She could be a good dealer, but " + girlName + " has a lot to learn still.\n";
					}
				else
					{
				message += girlName + "broke even today, thank the Lady.\n";
				}
			}
		else if (roll <= 60)
			{
				if (g_Girls.HasTrait(girl, "Sexy Air"))
					{
					message += girlName + " isn't a terrible card dealer, but she's much more eye-candy then gambling queen.\n";
					}
				else
					{
				message += "Pasteboard isn't her friend as the cards seemed to taunt her.\n";
				}
			}
		else if (roll <= 80)
			{
				message += "Almost all the patrons managed to preserve most of their initial stake.\n";
			}
		else
			{
				if (g_Girls.HasTrait(girl, "Big Boobs") || g_Girls.HasTrait(girl, "Abnormally Large Boobs"))
					{
					message += "She turned a slight profit, with the help of her not inconsiderable breasts' distraction factor.\n";
					}
				else
				{
				message += "She's clocked in and clocked out, but nothing spectacular has happened in between.\n";
				}
			}
		}
else if (jobperformance >= 70)
		{
			message += " She was nervous and made a few mistakes. She isn't that good at this.\n\n";
			wages -= 5;
		if (roll <= 20)
			{
				 if (g_Girls.HasTrait(girl, "Quick Learner"))
					{
					message += "She's got a clue, but still has a long way to go to reach competancy.\n";
					}
				else
					{
				message += girlName + " struggles valiently against the forces of chance, and wins! A. Single. Game.\n";
				}
			}
		else if (roll <= 40)
			{
				if (g_Girls.HasTrait(girl, "Nervous") || g_Girls.HasTrait(girl, "Meek"))
					{
					message += girlName + "'s weak personality made it easy for clients to bully her out of money.\n";
					}
				else
					{
				message += "Despite her feeble protests, gamblers walked all over " + girlName + ".\n";
				}
			}
		else if (roll <= 60)
			{
				if (g_Girls.GetStat(girl, STAT_INTELLIGENCE) >80)
					{
					message += girlName + " is smart enough to understand the game.  But seems not to have the luck to win.\n";
					}
				else
				{
				message += "As you watch " + girlName + "fold like a house of cards on a royal flush, you idly wonder if she could be replaced with a shaved ape.\n";
				}
			}
		else if (roll <= 80)
			{
				if (g_Girls.HasTrait(girl, "Big Boobs") || g_Girls.HasTrait(girl, "Abnormally Large Boobs"))
					{
					message += "While players were distracted by " + girlName + "'s breasts for a few turns, she still lost more then she won.\n";
					}
				else
				{
				message += "The cards are not in her favor today - the highest hand you saw her with was two pair.\n";
				}
			}
		else
			{
				if (g_Girls.HasTrait(girl, "Sexy Air"))
					{
					message += girlName + " could make a corpse stand up and beg for a blow-job, but she can't play cards worth a damn.\n";
					}
				else
					{
				message += "As " + girlName + "'s shift ends, you struggle mightily against the urge to sigh in relief.\n";
				}
			}
		}
else
		{
			message += " She was nervous and constantly making mistakes. She really isn't very good at this job.\n\n";
			wages -= 15;
		if (roll <= 20)
			{
				if (g_Girls.HasTrait(girl, "Sexy Air"))
					{
					message += "It's almost a pity how attractive" + girlName + " is.  If she wasn't so desireable, fewer vultures would alight on her table.\n";
					}
				else
					{
				message += girlName + " dropped the deck on the floor, spraying cards everywhere.\n";
				}
			}
		else if (roll <= 40)
			{
				message += girlName + " managed, against all probability, to lose every single game.\n";
			}
		else if (roll <= 60)
			{
				if (g_Girls.HasTrait(girl, "Big Boobs") || g_Girls.HasTrait(girl, "Abnormally Large Boobs"))
					{
					message += girlName + "'s large breasts pleased the clients as they won over and over again.\n";
					}
				else
				{
				message += girlName + " shrugged with a degree of embarrasment as a chortling patron walked away with a fat moneybag.\n";
				}
			}
		else if (roll <= 80)
			{
				if (g_Girls.HasTrait(girl, "Nervous") || g_Girls.HasTrait(girl, "Meek"))
					{
					message += girlName + "'s weak personality made it easy for clients to bully her out of money.\n";
					}
				else
					{
				message += girlName + " is really, really, bad at this job.\n";
				}
			}
		else
			{
				if (g_Girls.HasTrait(girl, "Quick Learner"))
					{
					message += "After a terrible shift, you can only hope that she learned something from it.\n";
					}
				else
					{
				message += "You can almost see the profits slipping away as " + girlName + "loses yet another hand of poker.\n";
				}
			}
		}


	//try and add randomness here
	if (g_Girls.GetStat(girl, STAT_BEAUTY) >85)
		if((g_Dice%100) < 20)
		{
			message += " Stunned by her beauty a customer was to distracted to notice he was losing.\n";
			wages += 25;
		}

	if (g_Girls.HasTrait(girl, "Big Boobs") || g_Girls.HasTrait(girl, "Abnormally Large Boobs"))
		if((g_Dice%100) < 15)
		{
			if(jobperformance < 150)
			{
			message += " A patron was staring obviously at her large breasts. But she had no ideal how to take advantage of it.\n";
			}
			else
			{
			message += " A patron was staring obviously at her large breasts. So she used the chance to cheat him out of all his gold.\n";
			wages += 15;
			}
		}

	if (g_Girls.HasTrait(girl, "Lolita"))
		if((g_Dice%100) < 15)
		{
			if(jobperformance < 125)
			{
			message += "Furious at being outplayed by such a young girl, a couple of gamblers stormed out, and didn't give" + girlName + " any tips.\n";
			}
			else
			{
			message += "One of the gamblers was amused at being outplayed by such a young girl, and gave her an extra-large tip!\n";
			wages += 15;
			}
		}

	if (g_Girls.HasTrait(girl, "Elegent") || g_Girls.HasTrait(girl, "Princess") || g_Girls.HasTrait(girl, "Queen"))
		if((g_Dice%100) < 15)
		{
			if(jobperformance < 150)
			{
			message += "Surly at her apparently stuck-up attitude, several gamblers refused to tip " + girlName + ".\n";
			}
			else
			{
			message += "Impressed by her elegent demeanor and graceful compartment, several gamblers gave " + girlName + " larger tips then usual.\n";
			wages += 15;
			}
		}

	if (g_Girls.HasTrait(girl, "Assassin"))
		if((g_Dice%101) < 5)
		{
			if(jobperformance < 150)
			{
				message += "She decided a patron was cheating so she killed him causing a paninc of people running out with your money.\n";
				wages -= 50;
			}
			else
			{
				message += "She thought a patron was cheating but decided it was a luckey strike that she would end with her card skills.\n";
			}
		}

	if (g_Girls.HasTrait(girl, "Psychic"))
		if((g_Dice%100) < 20)
		{
			message += "She used her Psychic skills to know excatally what cards was coming up and won a big hand.\n";
			wages += 15;
		}

	if (g_Brothels.GetNumGirlsOnJob(0,JOB_ENTERTAINMENT,false) == 1)
		if(roll <= 25)
		{
			if(jobperformance < 125)
			{
				message += girl->m_Realname + " wasn't good enough at her job to use the entertainment's distraction to make more money.\n";
			}
			else
			{
			message += girl->m_Realname + " used the enterainment's distraction to make you some extra money.\n";
			wages += 25;
			}
		}

		if(wages < 0)
			wages = 0;


	//enjoyed the work or not
	if(roll <= 5)
	{
		message += " \nSome of the patrons abused her during the shift.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKHALL, -1, true);
	}
	else if(roll <= 25) {
		message += " \nShe had a pleasant time working.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKHALL, +3, true);
	}
	else
	{
		message += " \nOtherwise, the shift passed uneventfully.";
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKHALL, +1, true);
	}

	girl->m_Events.AddMessage(message, IMGTYPE_CARD, DayNight);

	// work out the pay between the house and the girl
	wages += (g_Dice%((int)(((g_Girls.GetStat(girl, STAT_BEAUTY)+g_Girls.GetStat(girl, STAT_CHARISMA))/2)*0.5f)))+10;
	girl->m_Pay = wages;
	string pay = "";



	// Improve girl
	int xp = 10, libido = 1, skill = 2;

	if (g_Girls.HasTrait(girl, "Quick Learner"))
	{
		skill += 1;
		xp += 2;
	}
	else if (g_Girls.HasTrait(girl, "Slow Learner"))
	{
		skill -= 1;
		xp -= 2;
	}

	if (g_Girls.HasTrait(girl, "Nymphomaniac"))
		libido += 2;

	g_Girls.UpdateStat(girl, STAT_FAME, 1);
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	int gain = g_Dice%3;
	if (gain == 0)
		g_Girls.UpdateStat(girl, STAT_INTELLIGENCE, g_Dice%skill);
	else if (gain == 1)
		g_Girls.UpdateStat(girl, STAT_AGILITY, g_Dice%skill);
	else
		g_Girls.UpdateSkill(girl, SKILL_PERFORMANCE, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, g_Dice%skill+1);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	return false;
}
