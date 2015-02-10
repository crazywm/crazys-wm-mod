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

static cPlayer* m_Player = g_Brothels.GetPlayer();	//SIN: a way to access player details

// `J` Brothel Job - Hall
bool cJobManager::WorkHallXXXEntertainer(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKSTRIP;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;

	if (g_Girls.DisobeyCheck(girl, actiontype, brothel))
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked as a sexual entertainer in the gambling hall.\n\n";

	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	int roll = g_Dice.d100();
	int wages = 25, work = 0;
	int imagetype = IMGTYPE_ECCHI;

	double jobperformance = JP_HallXXXEntertainer(girl, false);


	// SIN: A little pre-show randomness - temporary stats that may affect show
	if (g_Dice.percent(20))
	{
		if (g_Girls.GetStat(girl, STAT_TIREDNESS) > 75)
		{
			ss << girlName << " was very tired. This affected her performance. ";
			jobperformance -= 10;
		}
		else if (g_Girls.GetStat(girl, STAT_LIBIDO) > 30)
		{
			ss << girlName << "'s horniness improved her performance. ";
			jobperformance += 10;
		}

		if (g_Girls.HasTrait(girl, "Demon") || g_Girls.HasTrait(girl, "Shape Shifter") || g_Girls.HasTrait(girl, "Construct") ||
			g_Girls.HasTrait(girl, "Cat Girl") || g_Girls.HasTrait(girl, "Succubus") || g_Girls.HasTrait(girl, "Reptilian"))
		{
			ss << "Customers are surprised to see such an unusual girl giving sexual entertainment. ";
			ss << "Some are disgusted, some are turned on, but many can't help watching.\n";
			ss << "The dealers at the tables make a small fortune from distracted guests. ";
			wages += 30;
		}
		else if (girl->age() > 30 && g_Dice.percent(min(90, max((girl->age() - 30) * 3, 1))))
		{	//"Too old!" - chance of heckle: age<30y= 0%, then 4%/year (32y - 6%, 40y - 30%...) max 90%... (but only a 20% chance this bit even runs)
			//Note: demons are exempt as they age differently
			ss << "Some customers heckle " << girlName << " over her age.";
			ss << "\n\"Gross!\" \"Grandma is that you!?\"\n";
			ss << "This makes it harder for her to work this shift. ";
			jobperformance -= 20;
		}
		if ((g_Girls.HasTrait(girl, "Syphilis") || g_Girls.HasTrait(girl, "Herpes"))
			&& g_Dice.percent(100 - g_Girls.GetStat(girl, STAT_HEALTH)))
		{
			ss << "She's unwell. A man in the audience recognises " << girlName << "'s symptoms and heckles her about her ";
			if (g_Girls.HasTrait(girl, "Syphilis") && g_Girls.HasTrait(girl, "Herpes"))
			{
				ss << "diseases";
			}
			else if (g_Girls.HasTrait(girl, "Herpes"))
			{
				ss << "Herpes";
			}
			else if (g_Girls.HasTrait(girl, "Syphilis"))
			{
				ss << "Syphilis";
			}
			else
			{
				ss << "diseases";
			}
			ss << ". This digusts some in the audience and results in further heckling which disrupts ";
			ss << "her performance and makes her very uncomfortable. ";
			jobperformance -= 60;
			girl->happiness(-10);
		}
		ss << "\n";
	}


	if (jobperformance >= 245)
	{
		ss << " She must be the perfect entertainer customers go on and on about her and always come to see her when she works.\n\n";
		wages += 155;
		if (roll <= 20)
		{
			if (g_Girls.HasTrait(girl, "Nymphomaniac"))
			{
				ss << girlName << " could do this all day with a genuine smile on her face, pussy dripping onto the stained boards beneath her legs.\n";
			}
			else
			{
				ss << girlName << " has a large devoted group of fans, who come in every night to watch her.\n";
			}
		}
		else if (roll <= 40)
		{
			ss << "A simple smile towards the audience from " << girlName << " makes every gambler howl and cheer.\n";
		}
		else if (roll <= 60)
		{
			if (g_Girls.HasTrait(girl, "Your Daughter"))
			{
				ss << "You probably shouldn't be watching your own daughter doing this, but " << girlName << "'s so damn good it's impossible not to.\n";
			}
			else
			{
				ss << "It's amazing how many tricks " << girlName << " knows, to tease her audience but never quite push them over the point where they'd lose interest.\n";
			}
		}
		else if (roll <= 80)
		{
			ss << girlName << " stripped in a unique way tonight, dancing with a razor in each hand.  With those razors, she peeled her clothes away from her lovely body, without nicking herself a single time.\n";
		}
		else
		{
			if (g_Girls.HasTrait(girl, "Sexy Air"))
			{
				ss << girlName << " could be dressed in a sack and still make a fortune masturbating on stage.\n";
			}
			else
			{
				ss << girlName << "'s shows are always sold out, and her fame is spreading across the city.\n";
				g_Girls.UpdateStat(girl, STAT_FAME, 1);
			}
		}
	}
	else if (jobperformance >= 185)
	{
		ss << " She's unbelievable at this and is always getting praised by the customers for her work.\n\n";
		wages += 95;
		if (roll <= 20)
		{
			if (g_Girls.HasTrait(girl, "Nymphomaniac"))
			{
				ss << girlName << " loves her job. You can tell by the zeal she employs as she penetrates her mouth and pussy with dildoes on stage, moaning throughout.\n";
			}
			else
			{
				ss << "From start to finish, every move " << girlName << " makes practically sweats sexuallity.\n";
			}
		}
		else if (roll <= 40)
		{
			if (g_Girls.HasTrait(girl, "Exhibitionist"))
			{
				ss << girlName << " feels so comfortable naked in front of strangers. She loves having all these strangers staring at her naked body ";
				ss << " and rewards the audience with a XXX show they will never forget.\n";
			}
			else ss << girlName << " moved off the stage halfway through the act and walked amongst the audience, to their joy.\n";
		}
		else if (roll <= 60)
		{
			ss << "As clothing slowly fell from her body and her hand descended towards her core, " << girlName << " smiled seductively at the audience.\n";
		}
		else if (roll <= 80)
		{
			ss << "All male members of " << girlName << "'s audience had trouble standing after she fellated a dildo on stage.\n";
		}
		else
		{
			if (g_Girls.HasTrait(girl, "Sexy Air"))
			{
				ss << "The skill of " << girlName << "'s dancing coupled with her sexual air just pack the customers in.\n";
			}
			else
			{
				ss << "Amusingly, one of the female members of " << girlName << "'s audience threw her panties on the stage, where " << girlName << " used them as an impromptu prop.\n";
			}
		}
	}
	else if (jobperformance >= 145)
	{
		ss << " She's good at this job and gets praised by the customers often.\n\n";
		wages += 55;
		if (roll <= 20)
		{
			ss << "A lucky gambler almost fainted when " << girlName << " gave him her freshly removed skirt as a present.\n";
		}
		else if (roll <= 40)
		{
			if (g_Girls.HasTrait(girl, "Long Legs"))
			{
				ss << girlName << "'s erotic dances are accentuated by her perfect legs.\n";
			}
			else
			{
				ss << girlName << " has a small, but devoted fanbase, who are willing to pay gate fees just to watch her dance.\n";
			}
		}
		else if (roll <= 60)
		{
			ss << "After letting " << girlName << " choose the music that she danced to, her erotic dances have improved markedly.\n";
		}
		else if (roll <= 80)
		{
			ss << "You watched with amusement as members of her audience made a disproportionate number of trips to the bathroom after her performance.\n";
		}
		else
		{
			if (g_Girls.HasTrait(girl, "Sexy Air"))
			{
				ss << girlName << " is by no measure bad at her job, but she gets way more customers than could be expected.\n";
			}
			else
			{
				ss << "A man made an offer to buy " << girlName << " today, which you turned down.  She's popular with the patrons.\n";
			}
		}
	}
	else if (jobperformance >= 100)
	{
		ss << " She made a few mistakes but overall she is okay at this.\n\n";
		wages += 15;
		if (roll <= 20)
		{
			if (g_Girls.HasTrait(girl, "Sexy Air"))
			{
				ss << girlName << "'s dances are mechanical, but her aurora of sexuality make clients break out in sweats.\n";
			}
			else
			{
				ss << girlName << "'s striptease draws a decent crowd each night.\n";
			}
		}
		else if (roll <= 40)
		{
			ss << girlName << " is not the best erotic dancer you've ever seen, but the gamblers enjoy the eyecandy.\n";
		}
		else if (roll <= 60)
		{
			ss << "The cleaners always hate having to clean up after " << girlName << "'s audience.\n";
		}
		else if (roll <= 80)
		{
			ss << girlName << " got a round of applause when she bowed, and an even bigger one when her ";

			//SIN: new additional traits added.
			if (g_Girls.HasTrait(girl, "Abnormally Large Boobs") ||
				g_Girls.HasTrait(girl, "Titanic Tits") ||
				g_Girls.HasTrait(girl, "Massive Melons"))
			{
				ss << "enormous breasts \"accidentally\" burst out from her top.\n";
			}
			else if (g_Girls.HasTrait(girl, "Big Boobs") || g_Girls.HasTrait(girl, "Busty Boobs") || g_Girls.HasTrait(girl, "Giant Juggs"))
			{
				ss << "large breasts \"accidentally\" flopped out of her top.\n";
			}
			else if (g_Girls.HasTrait(girl, "Small Boobs") || g_Girls.HasTrait(girl, "Petite Breasts"))
			{
				ss << "shirt \"accidentally\" exposed her small and perky breasts.\n";
			}
			else if (g_Girls.HasTrait(girl, "Flat Chest"))
			{
				ss << "shirt \"accidentally\" exposed the nipples on her perfectly flat chest.\n";
			}
			else
			{
				ss << "breasts \"accidentally\" fell out of her top.\n";
			}
		}
		else
		{
			if (g_Girls.HasTrait(girl, "Long Legs"))
			{
				ss << "Although " << girlName << " is no great shake at this job, her legs lend her a significant popularity boost.\n";
			}
			else
			{
				ss << girlName << " has some talent at this job, but she needs to develop it.\n";
			}
		}
	}
	else if (jobperformance >= 70)
	{
		ss << " She was nervous and made a few mistakes. She isn't that good at this.\n\n";
		wages -= 5;
		if (roll <= 20)
		{
			if (g_Girls.HasTrait(girl, "Sexy Air"))
			{
				ss << "Even though a block of wood could dance better, + " << girlName << " still draws the eye.\n";
			}
			else
			{
				ss << girlName << " is still incredibly sexy, despite her lack of anything resembling an active customer appeal.\n";
			}
		}
		else if (roll <= 40)
		{
			ss << girlName << "'s distaste for the crowd was evident by her curled lip and perpetual scowl.\n";
		}
		else if (roll <= 60)
		{
			if (g_Girls.HasTrait(girl, "Slut"))
			{
				ss << "A total slut, " << girlName << " tried to liven up her show by getting one of the security team to fuck her on stage. He refused.";
			}
			else
			{
				ss << "The only person who seemed to find " << girlName << "'s striptease appetizing was a random drunk.\n";
			}
		}
		else if (roll <= 80)
		{
			if (g_Girls.HasTrait(girl, "Long Legs"))
			{
				ss << "The perfectly sculpted flesh of her leg drew attention away from the stupid look on " << girlName << "'s face.\n";
			}
			else
			{
				ss << girlName << "'s hips twitched in a decidedly half-assed way.\n";
			}
		}
		else
		{
			ss << girlName << " isn't qualified as a striper, which might explain why she couldn't successfully detach her bra on stage.\n";
		}
	}
	else
	{
		ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.\n\n";
		wages -= 15;
		if (roll <= 20)
		{
			if (g_Girls.HasTrait(girl, "Long Legs"))
			{
				ss << "Customers just can't tear their eyes away from " << girlName << "'s long, tasty legs, even as she tripped over her discarded top.\n";
			}
			else
			{
				ss << girlName << " slipped on a fresh smear of ejaculate halfway through her dance, and fell on her ass.\n";
			}
		}
		//SIN: added more dialogue options
		else if (roll <= 40)
		{
			if (g_Girls.HasTrait(girl, "Strong Gag Reflex") || g_Girls.HasTrait(girl, "Gag Reflex"))
			{
				ss << girlName << " was sucking off a dildo, when she made herself gag and threw up on stage.\n";
				brothel->m_Filthiness += 5;
			}
			else
			{
				ss << girlName << "'s hair got tangled up in her shirt when she tried to take it off, making her reel about the stage like a drunk.\n";
			}
		}
		else if (roll <= 60)
		{
			if (g_Girls.GetSkill(girl, SKILL_MAGIC) < 20)
			{
				ss << girlName << " tried using magic light to enhance her show, but ended up setting her pubic hair on fire.\n";
			}
			else
			{
				ss << "Seeing the large crowd waiting outside, " << girlName << "'s nerve broke and she ran back out of the gambling hall.\n";
			}
		}
		else if (roll <= 80)
		{
			ss << girlName << " was drunk when she stumbled out at the beginning of shift. ";
			if (g_Dice.percent(g_Girls.GetSkill(girl, SKILL_COMBAT)))
			{
				ss << "A customer insulted her, so she leapt off the stage butt-naked and beat him down. This turned out to be the sexiest part of an otherwise boring show.\n";
				brothel->m_Happiness += 5;
			}
			else
			{
				ss << " She threw up and fell asleep on the floor almost as soon as she reached the stage.\n";
				brothel->m_Filthiness += 5;
			}
		}
		else
		{
			if (g_Girls.HasTrait(girl, "Sexy Air"))
			{
				ss << "Even though a block of wood could dance better, + " << girlName << " still draws the eye.\n";
			}
			else
			{
				ss << "Somehow, " << girlName << " managed to be so unsexy that the audience barely seemed to notice her presence.\n";
			}
		}
	}


	//try and add randomness here
	if (g_Girls.GetStat(girl, STAT_BEAUTY) > 85 && g_Dice.percent(20))
	{
		ss << "Stunned by her beauty a customer left her a great tip.\n\n"; wages += 25;
	}

	if (g_Girls.HasTrait(girl, "Clumsy") && g_Dice.percent(15))
	{
		ss << "Her clumsy nature caused her to lose one of her \"toys\" up a hole. ";
		if (g_Dice.percent(20) || g_Girls.HasTrait(girl, "Psychic") || g_Girls.HasTrait(girl, "Exhibitionist") || g_Girls.GetSkill(girl, SKILL_MEDICINE) > 25)
		{
			ss << "She put on a damn sexy show of getting it back out.\n"; wages += 10;
		}
		else
		{
			ss << "She panicked and ran off stage to go find a doctor.\n"; wages -= 15;
		}
	}

	if (g_Girls.HasTrait(girl, "Pessimist") && g_Dice.percent(5))
	{
		if (jobperformance < 125)
		{
			ss << "Her pessimism made her unadventurous. The customers were unimpressed and the tips were lower.\n"; wages -= 10;
		}
		else
		{
			ss << girlName << " performed well despite her mood. The customers enjoy the darker, sultry nature of her sex shows.\n"; wages += 10;
		}
	}

	if (g_Girls.HasTrait(girl, "Optimist") && g_Dice.percent(5))
	{
		if (jobperformance < 125)
		{
			ss << girlName << " smiled far too much to look seductive.\n"; wages -= 10;
		}
		else
		{
			ss << girlName << " showed endless energy, agility and enthusiasm as she rucked around the stage.\n"; wages += 10; g_Girls.UpdateStat(girl, STAT_AGILITY, 1);
		}
	}

	if (g_Girls.HasTrait(girl, "Cat Girl") && g_Dice.percent(10))
	{
		ss << girlName << " is able to clean herself like a cat. Customers are amazed as this pussy-cat ";
		ss << "spreads her legs wide and licks her own cunt right there on stage.\n";
		g_Girls.UpdateStat(girl, STAT_FAME, 2);
		wages += 15;
	}

	if (g_Girls.HasTrait(girl, "Masochist") && g_Dice.percent(10))
	{
		ss << girlName << " invites a few customers to punish her on stage. They happily agree to spank and whip her.\n";
		wages += 15;
		g_Girls.UpdateStat(girl, STAT_HEALTH, -2);
		g_Girls.UpdateSkill(girl, SKILL_BDSM, 1);
	}
	if (g_Girls.HasTrait(girl, "Your Daughter") && g_Dice.percent(20))
	{
		ss << "Word got around that " << girlName << " is your daughter, so more customers than normal came to watch her perform.\n";
		wages += (wages / 5);
		if (m_Player->disposition() > 0)
		{
			ss << "This is about the nicest job you can give her. She's safe here and the customers can only look - ";
		}
		else
		{
			ss << "At the end of the day, she's another whore to manage, it's a job that needs doing and ";
		}
		if (jobperformance >= 120)
		{
			ss << " she shows obvious talent at this.\n";
			g_Girls.UpdateStat(girl, STAT_FAME, 5);
		}
		else
		{
			ss << " it's just a damn shame she sucks at it.\n";
		}
	}

	if (g_Girls.GetStat(girl, STAT_LIBIDO) > 90)
	{
		ss << "She was horny and ended up masturbating for the customers making them very happy.";
		sCustomer cust;
		GetMiscCustomer(brothel, cust);
		brothel->m_Happiness += 100;
		g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, -20);
		// work out the pay between the house and the girl
		wages += g_Girls.GetStat(girl, STAT_ASKPRICE) + 60;
		imagetype = IMGTYPE_MAST;
	}


	if (wages < 0) wages = 0;

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

	g_Girls.UpdateEnjoyment(girl, ACTION_WORKHALL, work, true);
	g_Girls.UpdateEnjoyment(girl, ACTION_WORKSTRIP, work, true);

	girl->m_Events.AddMessage(ss.str(), imagetype, Day0Night1);



	// work out the pay between the house and the girl
	wages += (g_Dice % ((int)(((g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_CHARISMA)) / 2)*0.5f))) + 10;
	girl->m_Pay = wages;


	// Improve girl
	int xp = 10, libido = 1, skill = 2;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_FAME, 1);
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateStat(girl, STAT_CONFIDENCE, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_STRIP, g_Dice%skill + 1);
	g_Girls.UpdateSkill(girl, SKILL_PERFORMANCE, g_Dice%skill + 1);
	g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, libido);

	//gain traits
	g_Girls.PossiblyGainNewTrait(girl, "Nymphomaniac", 75, ACTION_WORKSTRIP, "Having to perform sexual entertainment for patrons every day has made " + girlName + " quite the nympho.", Day0Night1);
	//SIN: new trait
	g_Girls.PossiblyGainNewTrait(girl, "Exhibitionist", 50, ACTION_WORKSTRIP, "Performing sexual entertainment for strangers every day has made " + girlName + " quite keen to show off her sexuality.", Day0Night1);

	return false;
}

double cJobManager::JP_HallXXXEntertainer(sGirl* girl, bool estimate)// not used
{
	double jobperformance =
		((g_Girls.GetStat(girl, STAT_CHARISMA) +
		g_Girls.GetStat(girl, STAT_BEAUTY) +
		g_Girls.GetStat(girl, STAT_CONFIDENCE)) / 3 +
		g_Girls.GetSkill(girl, SKILL_STRIP) / 2 +
		g_Girls.GetSkill(girl, SKILL_PERFORMANCE) / 2);


	//good traits
	if (g_Girls.HasTrait(girl, "Charismatic"))  jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Sexy Air"))		jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Cool Person"))  jobperformance += 5; //people love to be around her
	if (g_Girls.HasTrait(girl, "Cute"))			jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Charming"))		jobperformance += 5; //people like charming people
	if (g_Girls.HasTrait(girl, "Great Figure")) jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Great Arse"))   jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Quick Learner"))jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Psychic"))		jobperformance += 10;

	//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))	jobperformance -= 50; //needs others to do the job	
	if (g_Girls.HasTrait(girl, "Clumsy"))		jobperformance -= 10; //spills food and breaks things often	
	if (g_Girls.HasTrait(girl, "Aggressive"))	jobperformance -= 20; //gets mad easy and may attack people
	if (g_Girls.HasTrait(girl, "Nervous"))		jobperformance -= 30; //don't like to be around people
	if (g_Girls.HasTrait(girl, "Meek"))			jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Shy"))			jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Slow Learner"))	jobperformance -= 10;
	return jobperformance;
}
