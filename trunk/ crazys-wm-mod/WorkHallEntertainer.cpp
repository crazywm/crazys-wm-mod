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

// `J` Job Brothel - Hall
bool cJobManager::WorkHallEntertainer(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKHALL;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	if (g_Girls.DisobeyCheck(girl, actiontype, brothel))
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked as an entertainer in the gambling hall.\n\n";

	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	sGirl* dealeronduty = g_Brothels.GetRandomGirlOnJob(0, JOB_DEALER, Day0Night1);
	string dealername = (dealeronduty ? "Dealer " + dealeronduty->m_Realname + "" : "the Dealer");

	int roll = g_Dice.d100();
	int wages = 25, work = 0;

	double jobperformance = JP_HallEntertainer(girl, false);

	//SIN: A little pre-randomness
	if (g_Dice.percent(50))
	{
		if (g_Girls.GetStat(girl, STAT_TIREDNESS) > 75)
		{
			ss << "She was very tired, negatively affecting her performance.\n";
			jobperformance -= 10;
		}
		else if (g_Girls.GetStat(girl, STAT_HAPPINESS) > 90)
		{
			ss << "Her cheeriness improved her performance.\n";
			jobperformance += 5;
		}
		if (g_Dice.percent(10))
		{
			if (g_Girls.GetSkill(girl, SKILL_STRIP) > 60)
			{
				ss << "A born stripper, " << girlName << " wears her clothes just short of showing flesh, just the way the customers like it.\n";
				jobperformance += 15;
			}
			if (g_Girls.GetStat(girl, STAT_PCHATE) > g_Girls.GetStat(girl, STAT_PCFEAR))
			{
				ss << " " << girlName << " opened with some rather rude jokes about you. While this annoys you a little, ";
				if (g_Girls.HasTrait(girl, "Your Daughter"))
				{
					ss << "she is your daughter, and ";
				}
				ss << "it seems to get the audience on her side.\n";
				jobperformance += 15;
			}
		}
	}

	if (jobperformance >= 245)
	{
		ss << " She must be the perfect entertainer customers go on and on about her and always come to see her when she works.\n\n";
		wages += 155;
		if (roll <= 20)
		{
			if (g_Girls.HasTrait(girl, "Aggressive"))
			{
				ss << "The heat of her passion and strength of her fierce nature make " << girlName << "'s songs absolutely riveting to listen to.\n";
			}
			else
			{
				ss << "Every one of " << girlName << "'s jokes elicits a wave of laughter from the crowd of patrons, many of whom have no interest in gambling.\n";
			}
		}
		else if (roll <= 40)
		{
			if (g_Girls.HasTrait(girl, "Elegant"))
			{
				ss << girlName << "'s very appearance in the door of the gambling hall leads to applause and the sudden appearence of a clear way to the stage steps.\n";
			}
			else
			{
				ss << "Her voice is that of an angel, and her humor is as sharp and cutting as a blade.\n";
			}
		}
		else if (roll <= 60)
		{
			ss << "Each shift, " << girlName << " ends her show by blowing a kiss to her fans, producing waves of applause and cheers.\n";
		}
		else if (roll <= 80)
		{
			if (g_Girls.HasTrait(girl, "Charming") || g_Girls.HasTrait(girl, "Charismatic"))
			{
				ss << "There is no way to resist the raw charisma " << girlName << " practically exudes, drawing everybody around her towards her stage.\n";
			}
			else
			{
				ss << "She could probably excel in any club or entertainment center - fortunately, " << girlName << "'s in yours, drawing customers.\n";
			}
		}
		else
		{
			if (g_Girls.HasTrait(girl, "Your Daughter"))
			{
				ss << "You feel inexpressibly proud that " << girlName << ", your daughter, is such a skilled entertainer.\n";
			}
			else
			{
				ss << girlName << " is downright ideal for this job.  Her presentation and material are flawless\n";
			}
		}
	}
	else if (jobperformance >= 185)
	{
		ss << " She's unbelievable at this and is always getting praised by the customers for her work.\n\n";
		wages += 95;
		if (roll <= 20)
		{
			if (g_Girls.HasTrait(girl, "Your Daughter"))
			{
				ss << girlName << " isn't perfect, but she's your own flesh and blood, which is more then close enough.\n";
			}
			else
			{
				ss << girlName << " begun to acquire her own following - a small crowd of people came in just to listen to her and buy drinks.\n";
			}
		}
		else if (roll <= 40)
		{
			if (g_Girls.HasTrait(girl, "Aggressive"))
			{
				ss << "The savagery of " << girlName << "'s temper has honed her wit to a razor's edge, letting her jab away at the audience and still rake in the tips.\n";
			}
			else if (g_Girls.HasTrait(girl, "Elegant"))
			{
				ss << "She cooly paces the edge of the stage as she sings, each step deliberate and perfectly poised, as if " << girlName << " was some noble pacing her seat of power.\n";
			}
			else
			{
				ss << "Her jokes are almost always funny as " << girlName << "'s repertoire develops and expands.\n";
			}
		}
		else if (roll <= 60)
		{
			ss << girlName << "'s arrival on the stage at the beginning of her shift is greeted by scattered applause by the last few gamblers from the previous shift, and the new clients.\n";
		}
		else if (roll <= 80)
		{
			ss << girlName << "'s soothing voice seems to glide over the noise and bustling of the gamblers and dealers.\n";
		}
		else
		{
			if (g_Girls.HasTrait(girl, "Charming") || g_Girls.HasTrait(girl, "Charismatic"))
			{
				ss << "While her technical skills could use improvement, " << girlName << "'s delivery is beyond reproach.\n";
			}
			else
			{
				ss << " Each shift the tips " << girlName << " collects seem to grow in size!\n";
			}
		}
	}
	else if (jobperformance >= 145)
	{
		ss << " She's good at this job and gets praised by the customers often.\n\n";
		wages += 55;
		if (roll <= 20)
		{
			ss << girlName << " managed to provide amusement for anybody who happened to listen to her.\n";
		}
		else if (roll <= 40)
		{
			if (g_Girls.HasTrait(girl, "Aggressive"))
			{
				ss << "Her jokes are a bit more violent then they perhaps should be, but the clients seem to like " << girlName << ".\n";
			}
			else if (g_Girls.HasTrait(girl, "Elegant"))
			{
				ss << "The elegence " << girlName << " brings to all things enhances her already pleasant singing.\n";
			}
			else
			{
				ss << "Most of the jokes " << girlName << " told were amusing.\n";
			}
		}
		else if (roll <= 60)
		{
			if (g_Girls.HasTrait(girl, "Charming") || g_Girls.HasTrait(girl, "Charismatic"))
			{
				ss << "While " << girlName << "'s repertoire of jokes and other amusements could stand expansion, she's got the presentation down pat.\n";
			}
			else
			{
				ss << "Her singing was pleasing, if bland.  Her voice was nice, if slightly untrained.\n";
			}
		}
		else if (roll <= 80)
		{
			if (g_Girls.HasTrait(girl, "Your Daughter"))
			{
				ss << "Though you'll never admit it, you're happy that " << girlName << " has carved out a niche that doesn't involve renting her cunt to public servants.\n";
			}
			else
			{
				ss << girlName << " earned a decent tip today for her sincere, if not always effective, attempts to entertain the clients.\n";
			}
		}
		else
		{
			ss << "A couple of gamblers parted with some of their hard-earned winnings after swearing that her singing had brought them luck.\n";
		}
	}
	else if (jobperformance >= 100)
	{
		ss << " She made a few mistakes but overall she is okay at this.\n\n";
		wages += 15;
		if (roll <= 20)
		{
			ss << girlName << " managed to elicit a few chuckles, and got a few tips.\n";
		}
		else if (roll <= 40)
		{
			if (g_Girls.HasTrait(girl, "Charming") || g_Girls.HasTrait(girl, "Charismatic"))
			{
				ss << "The tips she recieved were far more than " << girlName << "'s skills had any reason to bring in.\n";
			}
			else
			{
				ss << "While she won't win any contests, " << girlName << " isn't a terrible singer.\n";
			}
		}
		else if (roll <= 60)
		{
			if (g_Girls.HasTrait(girl, "Aggressive"))
			{
				ss << "While " << girlName << "'s jokes tend to fall flat, nobody seems to have the bravery to tell her that.\n";
			}
			else
			{
				ss << girlName << " monologued the guests for an hour or so.  While nobody seemed particularly interested, nobody looked pissed either.\n";
			}
		}
		else if (roll <= 80)
		{
			if (g_Girls.HasTrait(girl, "Elegant"))
			{
				ss << "Her air of sophistication lets " << girlName << " get away with telling boring jokes.\n";
			}
			else
			{
				ss << "While most of the jokes " << girlName << " tells are old and worn out, a few were rather amusing.\n";
			}
		}
		else
		{
			if (g_Girls.HasTrait(girl, "Your Daughter"))
			{
				ss << girlName << " isn't impressive in any way, but she is your daughter, so you dutifully clap as she descends from the stage.\n";
			}
			else
			{
				ss << "The slow song " << girlName << " sang at the end of shift really had her full emotion and heart.  A pity that she felt so bored and tired.\n";
			}
		}
	}
	else if (jobperformance >= 70)
	{
		ss << " She was nervous and made a few mistakes. She isn't that good at this.\n\n";
		wages -= 5;
		if (roll <= 20)
		{
			ss << "While her jokes aren't actually driving anyone off, nobody seems to find " << girlName << " entertaining either.\n";
		}
		else if (roll <= 40)
		{
			if (g_Girls.HasTrait(girl, "Charming") || g_Girls.HasTrait(girl, "Charismatic"))
			{
				ss << "Even though her jokes were stale and boring, the audience couldn't seem to make themselves let " << girlName << " down, and laughed regardless.\n";
			}
			else
			{
				ss << "Her singing is barely acceptable, but fortunately the bustling of the gambling hall drowns " << girlName << " out for the most part.\n";
			}
		}
		else if (roll <= 60)
		{
			if (g_Girls.HasTrait(girl, "Aggressive"))
			{
				ss << girlName << " cursed the audience roundly for their dislike of her music.\n";
			}
			else
			{
				ss << girlName << " tells bland jokes and sings forgetable songs.\n";
			}
		}
		else if (roll <= 80)
		{
			if (g_Girls.HasTrait(girl, "Your Daughter"))
			{
				ss << "You wonder if your daughter, child of your loins, might do better on her back then on the stage.\n";
			}
			else
			{
				ss << girlName << " doesn't appear to be enjoying herself, an emotion she has in common with the clientele.\n";
			}
		}
		else
		{
			ss << "For all the reaction she's getting, you could probably replace " << girlName << " with a simple mannequin.\n";
		}
	}
	else
	{
		ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.\n\n";
		wages -= 15;
		if (roll <= 20)
		{
			if (g_Girls.HasTrait(girl, "Clumsy"))
			{
				ss << girlName << " trips and falls flat on her face when climbing up the stage stairs.  The crowd cheers loudly.\n";
			}
			else if (g_Girls.HasTrait(girl, "Big Boobs") || g_Girls.HasTrait(girl, "Abnormally Large Boobs"))
			{
				ss << "You have a feeling if she wasn't so well endowed no one would show up for her show.\n";
			}
			else
			{
				ss << "You stuck your head into the hall halfway through shift, and swore off the gin for the third time this week after hearing " << girlName << "'s horrible singing.\n";
			}
		}
		else if (roll <= 40)
		{
			if (g_Girls.HasTrait(girl, "Fearless"))
			{
				ss << "Despite the crowd's displeasure, " << girlName << " keeps herself cool and collected, apparently unaffected by their ire.\n";
			}
			else
			{
				ss << "She managed to aggravate several customers with her poor taste in jokes.\n";
			}
		}
		else if (roll <= 60)
		{
			if (g_Girls.HasTrait(girl, "Your Daughter"))
			{
				ss << "As " << girlName << " butchers another song, you feel bitterly ashamed by your relationship to your worthless child.\n";
			}
			else
			{
				ss << girlName << "'s as funny as a peasant uprising and about as appreciated by her audience.\n";
			}
		}
		else if (roll <= 80)
		{
			if (g_Girls.HasTrait(girl, "Aggressive"))
			{
				ss << "When the crowd booed her, " << girlName << " responded by throwing a shoe at one of the loudest and most insulting members.\n";
			}
			else
			{
				ss << "Her audience seems paralyzed, as if they couldn't believe that a human body could produce those sounds, much less call them \"singing\".\n";
			}
		}
		else
		{
			if (g_Girls.HasTrait(girl, "Charming") || g_Girls.HasTrait(girl, "Charismatic"))
			{
				ss << "The poor quality of her routine was offset by her natural charisma.\n";
			}
			else
			{
				ss << "For some reason, " << girlName << " attempted a dramatic monologue and feigned suicide on stage.  Needless to say, she didn't receive any tips.\n";
			}
		}
	}


	//try and add randomness here
	if (g_Girls.GetStat(girl, STAT_BEAUTY) > 85 && g_Dice.percent(20))
	{
		ss << "Stunned by her beauty a customer left her a great tip.\n\n";
		wages += 25;
	}

	if (g_Girls.HasTrait(girl, "Clumsy") && g_Dice.percent(15))
	{
		ss << "Her clumsy nature caused her to spill a drink on a customer resulting in them storming off without paying.\n";
		wages -= 15;
	}

	if (g_Girls.HasTrait(girl, "Pessimist") && g_Dice.percent(5))
	{
		if (jobperformance < 125)
		{
			ss << "Her pessimistic mood depressed the customers making them tip less.\n";
			wages -= 10;
		}
		else
		{
			ss << girlName << " was in a poor mood so the patrons gave her a bigger tip to try and cheer her up.\n";
			wages += 10;
		}
	}

	if (g_Girls.HasTrait(girl, "Optimist") && g_Dice.percent(5))
	{
		if (jobperformance < 125)
		{
			ss << girlName << " was in a cheerful mood but the patrons thought she needed to work more on her services.\n";
			wages -= 10;
		}
		else
		{
			ss << " Her optimistic mood made patrons cheer up increasing the amount they tip.\n";
			wages += 10;
		}
	}

	if (g_Dice.percent(5) && (g_Girls.HasTrait(girl, "Charming") || g_Girls.HasTrait(girl, "Charismatic")))
	{
		if (jobperformance < 125)
		{
			ss << "Despite her poor performance, people still applaud loudly for her.\n";
			wages -= 10;
		}
		else
		{
			ss << "People love to see her perform, and " << girlName << " collects a massive tip!.\n";
			wages += 100;
		}
	}

	if (g_Dice.percent(5) && (g_Girls.HasTrait(girl, "Princess") || g_Girls.HasTrait(girl, "Queen")))
	{
		if (jobperformance < 125)
		{
			ss << "Her royal bitchiness combined with the poor entertainment pisses the audience off.\n";
			wages -= 15;
		}
		else
		{
			ss << girlName << " owns the stage, imperious eyes flashing out across the audience.  Tips are very good tonight.\n";
			wages += 25;
		}
	}

	if (g_Brothels.GetNumGirlsOnJob(0, JOB_DEALER, false) >= 1 && g_Dice.percent(25))
	{
		if (jobperformance < 125)
		{
			ss << girlName << " tried to distract the patrons but due to her lack of skills she distracted " << dealername << " causing you to lose some money.\n";
			wages -= 10;
		}
		else
		{
			ss << girlName << " was able to perfectly distract some patrons while the " << dealername << " cheated to make some more money.\n";
			wages += 25;
		}
	}

	//enjoyed the work or not
	if (roll <= 5)
	{
		ss << "\nSeveral patrons heckled her and made her shift generally unpleasant.";
		work -= 1;
	}
	else if (roll <= 25)
	{
		ss << "\nShe had a great time working.";
		work += 3;
	}
	else
	{
		ss << "\nOtherwise, the shift passed uneventfully.";
		work += 1;
	}

	g_Girls.UpdateEnjoyment(girl, actiontype, work);
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_BUNNY, Day0Night1);


	wages += (g_Dice % ((int)(((g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_CHARISMA)) / 2)*0.5f))) + 10;
	if (wages < 0) wages = 0;
	girl->m_Pay = wages;


	// Improve girl
	int xp = 10, libido = 1, skill = 2;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_FAME, 1);
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateStat(girl, STAT_CONFIDENCE, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_PERFORMANCE, g_Dice%skill + 1);
	g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, libido);

	return false;
}

double cJobManager::JP_HallEntertainer(sGirl* girl, bool estimate)// not used
{
	double jobperformance =
		((g_Girls.GetStat(girl, STAT_CHARISMA) +
		g_Girls.GetStat(girl, STAT_BEAUTY) +
		g_Girls.GetStat(girl, STAT_CONFIDENCE)) / 3 +
		g_Girls.GetSkill(girl, SKILL_PERFORMANCE));



	//good traits
	if (g_Girls.HasTrait(girl, "Charismatic"))   jobperformance += 15;
	if (g_Girls.HasTrait(girl, "Sexy Air"))		 jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Cool Person"))   jobperformance += 5; //people love to be around her
	if (g_Girls.HasTrait(girl, "Cute"))			 jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Charming"))		 jobperformance += 15; //people like charming people
	if (g_Girls.HasTrait(girl, "Great Figure"))  jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Great Arse"))	 jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Quick Learner")) jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Psychic"))		 jobperformance += 15;
	if (g_Girls.HasTrait(girl, "Fearless"))		 jobperformance += 5;

	//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))	jobperformance -= 50; //needs others to do the job	
	if (g_Girls.HasTrait(girl, "Clumsy"))		jobperformance -= 10; //spills food and breaks things often	
	if (g_Girls.HasTrait(girl, "Aggressive"))	jobperformance -= 20; //gets mad easy and may attack people
	if (g_Girls.HasTrait(girl, "Nervous"))		jobperformance -= 30; //don't like to be around people
	if (g_Girls.HasTrait(girl, "Meek"))			jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Shy"))			jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Broken Will"))	jobperformance -= 50;
	if (g_Girls.HasTrait(girl, "Slow Learner"))	jobperformance -= 15;
	if (girl->is_pregnant())					jobperformance -= 5; //can't move so well
	if (g_Girls.HasTrait(girl, "Flat Ass"))		jobperformance -= 5;
	if (g_Girls.HasTrait(girl, "Flat Chest"))	jobperformance -= 5;

	if (g_Girls.HasTrait(girl, "One Arm"))		jobperformance -= 40;
	if (g_Girls.HasTrait(girl, "One Foot"))		jobperformance -= 40;
	if (g_Girls.HasTrait(girl, "One Hand"))		jobperformance -= 30; 
	if (g_Girls.HasTrait(girl, "One Leg"))		jobperformance -= 60;
	if (g_Girls.HasTrait(girl, "No Arms"))		jobperformance -= 125;
	if (g_Girls.HasTrait(girl, "No Feet"))		jobperformance -= 60;
	if (g_Girls.HasTrait(girl, "No Hands"))		jobperformance -= 50;
	if (g_Girls.HasTrait(girl, "No Legs"))		jobperformance -= 150;
	if (g_Girls.HasTrait(girl, "Blind"))		jobperformance -= 30;
	if (g_Girls.HasTrait(girl, "Deaf"))			jobperformance -= 15;
	if (g_Girls.HasTrait(girl, "Retarded"))		jobperformance -= 60;
	if (g_Girls.HasTrait(girl, "Smoker"))		jobperformance -= 10;	//would need smoke breaks

	if (g_Girls.HasTrait(girl, "Alcoholic"))			jobperformance -= 25;
	if (g_Girls.HasTrait(girl, "Fairy Dust Addict"))	jobperformance -= 25;
	if (g_Girls.HasTrait(girl, "Shroud Addict"))		jobperformance -= 25;
	if (g_Girls.HasTrait(girl, "Viras Blood Addict"))	jobperformance -= 25;

	return jobperformance;
}
