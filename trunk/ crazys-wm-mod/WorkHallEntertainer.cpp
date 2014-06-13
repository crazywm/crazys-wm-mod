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

bool cJobManager::WorkHallEntertainer(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	string message = "";
	string girlName = girl->m_Realname;
	if(Preprocessing(ACTION_WORKHALL, girl, brothel, DayNight, summary, message))	// they refuse to have work in the hall
		return true;

	// put that shit away, you'll scare off the customers!
	g_Girls.UnequipCombat(girl);

	int roll = g_Dice%100;
	int jobperformance = ( (g_Girls.GetStat(girl, STAT_CHARISMA) +
							g_Girls.GetStat(girl, STAT_BEAUTY)  +
							g_Girls.GetStat(girl, STAT_CONFIDENCE))/3 +
							g_Girls.GetSkill(girl, SKILL_PERFORMANCE));
	int wages = 25;

	message += "She worked as an entertainer in the gambling hall.";

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
		jobperformance += 15;
	if (g_Girls.HasTrait(girl, "Great Figure"))  //
		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Great Arse"))  //
		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Quick Learner"))  //
		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Psychic"))
		jobperformance += 15;
	if (g_Girls.HasTrait(girl, "Fearless"))
		jobperformance += 5;

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
	if (g_Girls.HasTrait(girl, "Broken Will"))
		jobperformance -= 50;
	if (g_Girls.HasTrait(girl, "Slow Learner"))
		jobperformance -= 15;


	 if (jobperformance >= 245)
		{
			message += " She must be the perfect entertainer customers go on and on about her and always come to see her when she works.\n\n";
			wages += 155;
		if (roll <= 20)
			{
				if (g_Girls.HasTrait(girl, "Aggressive"))
					{
					message +=  "The heat of her passion and strength of her fierce nature make " + girlName + "'s songs absolutely riveting to listen to.\n";
					}
				else
					{
				message +=  "Every one of " + girlName + "'s jokes elicits a wave of laughter from the crowd of patrons, many of whom have no interest in gambling.\n";
				}
			}
		else if (roll <= 40)
			{
				if (g_Girls.HasTrait(girl, "Elegant"))
					{
					message += girlName + "'s very appearance in the door of the gambling hall leads to applause and the sudden appearence of a clear way to the stage steps.\n";
					}
				else
					{
				message += "Her voice is that of an angel, and her humor is as sharp and cutting as a blade.\n";
				}
			}
		else if (roll <= 60)
			{
				message +=  "Each shift, " + girlName + " ends her show by blowing a kiss to her fans, producing waves of applause and cheers.\n";
			}
		else if (roll <= 80)
			{
				if (g_Girls.HasTrait(girl, "Charming") || g_Girls.HasTrait(girl, "Charismatic"))
					{
					message += "There is no way to resist the raw charisma " + girlName + " practically exudes, drawing everybody around her towards her stage.\n";
					}
				else
					{
				message +=  "She could probably excel in any club or entertainment center - fortunately, " + girlName + "'s in yours, drawing customers.\n";
				}
			}
		else
			{
				if (g_Girls.HasTrait(girl, "Your Daughter"))
					{
					message +=  "You feel inexpressibly proud that " + girlName + ", your daughter, is such a skilled entertainer.\n";
					}
				else
					{
				message += girlName + " is downright ideal for this job.  Her presentation and material are flawless\n";
				}
			}
		}
 else if (jobperformance >= 185)
		{
			message += " She's unbelievable at this and is always getting praised by the customers for her work.\n\n";
			wages += 95;
		if (roll <= 20)
			{
				if (g_Girls.HasTrait(girl, "Your Daughter"))
					{
					message += girlName + " isn't perfect, but she's your own flesh and blood, which is more then close enough.\n";
					}
				else
					{
				message += girlName + " begun to acquire her own following - a small crowd of people came in just to listen to her and buy drinks.\n";
				}
			}
		else if (roll <= 40)
			{
				if (g_Girls.HasTrait(girl, "Aggressive"))
					{
					message +=  "The savagry of " + girlName + "'s temper has honed her wit to a razor's edge, letting her jab away at the audience and still rake in the tips.\n";
					}
				else if (g_Girls.HasTrait(girl, "Elegant"))
					{
					message +=  "She cooly paces the edge of the stage as she sings, each step deliberate and perfectly poised, as if " + girlName + " was some noble pacing her seat of power.\n";
					}
				else
					{
				message += "Her jokes are almost always funny as " + girlName + "'s repertoire develops and expands.\n";
				}
			}
		else if (roll <= 60)
			{
				message += girlName + "'s arrival on the stage at the beginning of her shift is greeted by scattered applause by the last few gamblers from the previous shift, and the new clients.\n";
			}
		else if (roll <= 80)
			{
				message += girlName + "'s soothing voice seems to glide over the noise and bustling of the gamblers and dealers.\n";
			}
		else
			{
				if (g_Girls.HasTrait(girl, "Charming") || g_Girls.HasTrait(girl, "Charismatic"))
					{
					message += "While her technical skills could use improvement, " + girlName + "'s delivery is beyond reproach.\n";
					}
				else
					{
				message += " Each shift the tips " + girlName + " collects seem to grow in size!\n";
				}
			}
		}
	else if (jobperformance >= 145)
		{
			message += " She's good at this job and gets praised by the customers often.\n\n";
			wages += 55;
		if (roll <= 20)
			{
				message += girlName + " managed to provide amusement for anybody who happened to listen to her.\n";
			}
		else if (roll <= 40)
			{
				if (g_Girls.HasTrait(girl, "Aggressive"))
					{
					message +=  "Her jokes are a bit more violent then they perhaps should be, but the clients seem to like " + girlName + ".\n";
					}
				else if (g_Girls.HasTrait(girl, "Elegant"))
					{
					message +=  "The elegence " + girlName + " brings to all things enhances her already pleasant singing.\n";
					}
				else
					{
				message += "Most of the jokes " + girlName + " told were amusing.\n";
				}
			}
		else if (roll <= 60)
			{
				if (g_Girls.HasTrait(girl, "Charming") || g_Girls.HasTrait(girl, "Charismatic"))
					{
					message += "While " + girlName + "'s repertoire of jokes and other amusements could stand expansion, she's got the presentation down pat.\n";
					}
				else
					{
				message += "Her singing was pleasing, if bland.  Her voice was nice, if slightly untrained.\n";
				}
			}
		else if (roll <= 80)
			{
				if (g_Girls.HasTrait(girl, "Your Daughter"))
					{
					message +=  "Though you'll never admit it, you're happy that " + girlName + " has carved out a niche that doesn't involve renting her cunt to public servants.\n";
					}
				else
					{
				message += girlName + " earned a decent tip today for her sincere, if not always effective, attempts to entertain the clients.\n";
				}
			}
		else
			{
				message += "A couple of gamblers parted with some of their hard-earned winnings after swearing that her singing had brought them luck.\n";
			}
		}
 	else if (jobperformance >= 100)
		{
			message += " She made a few mistakes but overall she is okay at this.\n\n";
			wages += 15;
		if (roll <= 20)
			{
				message += girlName + " managed to elicit a few chuckles, and got a few tips.\n";
			}
		else if (roll <= 40)
			{
				if (g_Girls.HasTrait(girl, "Charming") || g_Girls.HasTrait(girl, "Charismatic"))
					{
					message += "The tips she recieved were far more then " + girlName + "'s skills had any reason to bring in.\n";
					}
				else
					{
				message += "While she won't win any contests, " + girlName + " isn't a terrible singer.\n";
				}
			}
		else if (roll <= 60)
			{
				if (g_Girls.HasTrait(girl, "Aggressive"))
					{
					message +=  "While " + girlName + "'s jokes tend to fall flat, nobody seems to have the bravery to tell her that.\n";
					}
				else
					{
				message += girlName + " monologued the guests for an hour or so.  While nobody seemed particularly interested, nobody looked pissed either.\n";
				}
			}
		else if (roll <= 80)
			{
				if (g_Girls.HasTrait(girl, "Elegant"))
					{
					message +=  "Her air of sophistication lets " + girlName + " get away with telling boring jokes.\n";
					}
				else
					{
				message +=  "While most of the jokes " + girlName + " tells are old and worn out, a few were rather amusing.\n";
				}
			}
		else
			{
				if (g_Girls.HasTrait(girl, "Your Daughter"))
					{
					message += girlName + " isn't impressive in any way, but she is your daughter, so you dutifully clap as she descends from the stage.\n";
					}
				else
					{
				message += "The slow song " + girlName + " sang at the end of shift really had her full emotion and heart.  A pity that she felt so bored and tired.\n";
				}
			}
		}
	else if (jobperformance >= 70)
		{
			message += " She was nervous and made a few mistakes. She isn't that good at this.\n\n";
			wages -= 5;
		if (roll <= 20)
			{
				message += "While her jokes aren't actually driving anyone off, nobody seems to find " + girlName + " entertaining either.\n";
			}
		else if (roll <= 40)
			{
				if (g_Girls.HasTrait(girl, "Charming") || g_Girls.HasTrait(girl, "Charismatic"))
					{
					message += "Even though her jokes were stale and boring, the audience couldn't seem to make themselves let " + girlName + " down, and laughed regardless.\n";
					}
				else
					{
				message += "Her singing is barely acceptable, but fortunately the bustling of the gambling hall drowns " + girlName + " out for the most part.\n";
				}
			}
		else if (roll <= 60)
			{
				if (g_Girls.HasTrait(girl, "Aggressive"))
					{
					message += girlName + " cursed the audience roundly for their dislike of her music.\n";
					}
				else
					{
				message += girlName + " tells bland jokes and sings forgetable songs.\n";
				}
			}
		else if (roll <= 80)
			{
				if (g_Girls.HasTrait(girl, "Your Daughter"))
					{
					message += "You wonder if your daughter, child of your loins, might do better on her back then on the stage.\n";
					}
				else
					{
				message += girlName + " doesn't appear to be enjoying herself, an emotion she has in common with the clientele.\n";
				}
			}
		else
			{
				message += "For all the reaction she's getting, you could probably replace " + girlName + " with a simple mannequin.\n";
			}
		}
	else
		{
			message += " She was nervous and constantly making mistakes. She really isn't very good at this job.\n\n";
			wages -= 15;
		if (roll <= 20)
			{
				if (g_Girls.HasTrait(girl, "Clumsy"))
					{
					message += girlName + " trips and falls flat on her face when climbing up the stage stairs.  The crowd cheers loudly.\n";
					}
				else if (g_Girls.HasTrait(girl, "Big Boobs") || g_Girls.HasTrait(girl, "Abnormally Large Boobs"))
					{
					message += "You have a feeling if she wasn't so well endowed no one would show up for her show.\n";
					}
				else
					{
				message += "You stuck your head into the hall halfway through shift, and swore off the gin for the third time this week after hearing " + girlName + "'s horrible singing.\n";
				}
			}
		else if (roll <= 40)
			{
				if (g_Girls.HasTrait(girl, "Fearless"))
					{
					message += "Despite the crowd's displeasure, " + girlName + " keeps herself cool and collected, apparently unaffected by their ire.\n";
					}
				else
					{
				message += "She managed to aggravate several customers with her poor taste in jokes.\n";
				}
			}
		else if (roll <= 60)
			{
				if (g_Girls.HasTrait(girl, "Your Daughter"))
					{
					message += "As " + girlName + " butchers another song, you feel bitterly ashamed by your relationship to your worthless child.\n";
					}
				else
					{
				message += girlName + "'s as funny as a peasant uprising and about as appreciated by her audience.\n";
				}
			}
		else if (roll <= 80)
			{
				if (g_Girls.HasTrait(girl, "Aggressive"))
					{
					message +=  "When the crowd boos her, " + girlName + " responded by throwing a shoe at one of the loudest and most insulting members.\n";
					}
				else
					{
				message += "Her audience seems paralyzed, as if they couldn't believe that a human body could produce those sounds, much less call them \"singing\".\n";
				}
			}
		else
			{
				if (g_Girls.HasTrait(girl, "Charming") || g_Girls.HasTrait(girl, "Charismatic"))
					{
					message += "The poor quality of her routine was offset by her natural charisma.\n";
					}
				else
					{
				message += "For some reason, " + girlName + " attempted a dramatic monologue and feigned suicide on stage.  Needless to say, she didn't recieve any tips.\n";
				}
			}
		}


	//try and add randomness here
	if (g_Girls.GetStat(girl, STAT_BEAUTY) >85)
		if((g_Dice%100) < 20)
		{
			message += " Stunned by her beauty a customer left her a great tip.\n";
			wages += 25;
		}

	if (g_Girls.HasTrait(girl, "Clumsy"))
		if((g_Dice%100) < 15)
		{
			message += " Her clumsy nature caused her to spill a drink on a custmoer resulting in them storming off without paying.\n";
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
				message += girl->m_Realname + " was in a cheerful mood but the patrons thought she needed to work more on her services.\n";
				wages -= 10;
			}
			else
			{
			message += " Her optimistic mood made patrons cheer up increasing the amount they tip.\n";
			wages += 10;
			}
		}

	if (g_Girls.HasTrait(girl, "Charming") || g_Girls.HasTrait(girl, "Charismatic"))
		if((g_Dice%100) < 5)
		{
			if(jobperformance < 125)
			{
				message += "Despite her poor singing, people still applaud loudly for her.\n";
				wages -= 10;
			}
			else
			{
			message += "People love to see her perform, and " + girlName + " collects a massive tip!.\n";
			wages += 100;
			}
		}

	if (g_Girls.HasTrait(girl, "Princess") || g_Girls.HasTrait(girl, "Queen"))
		if((g_Dice%100) < 5)
		{
			if(jobperformance < 125)
			{
				message += "Her royal bitchiness combined with the poor entertainment pisses the audience off.\n";
				wages -= 15;
			}
			else
			{
			message += girlName + " owns the stage, imperious eyes flashing out across the audience.  Tips are very good tonight.\n";
			wages += 15;
			}
		}

	if (g_Brothels.GetNumGirlsOnJob(0,JOB_DEALER,false) == 1)
		if(roll <= 25)
		{
			if(jobperformance < 125)
			{
				message += girl->m_Realname + " tried to distract the patrons but due to her lack of skills she distracted your dealer causeing you to lose some money.\n";
				wages -= 10;
			}
			else
			{
			message += girl->m_Realname + " was able to perfectly distract some patrons while the dealer cheated to make some more money.\n";
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
	girl->m_Events.AddMessage(message, IMGTYPE_BUNNY, DayNight);

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
	g_Girls.UpdateStat(girl, STAT_CONFIDENCE, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_PERFORMANCE, g_Dice%skill+1);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	return false;
}
