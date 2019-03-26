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
#pragma region //	Includes and Externs			//
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

extern cPlayer* The_Player;

#pragma endregion

// `J` Job Brothel - Hall
bool cJobManager::WorkHallXXXEntertainer(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
#pragma region //	Job setup				//
	int actiontype = ACTION_WORKSTRIP;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();
	if (girl->disobey_check(actiontype, brothel))
	{
		//SIN - More informative mssg to show *what* she refuses
		//ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		ss << " refused to provide sexual entertainment in the gambling hall " << (Day0Night1 ? "tonight." : "today.");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked as a sexual entertainer in the gambling hall.\n \n";

	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	int wages = 25, tips = 0;
	int work = 0, fame = 0;
	int imagetype = IMGTYPE_ECCHI;
	int msgtype = Day0Night1;

#pragma endregion
#pragma region //	Job Performance			//

	double jobperformance = JP_HallXXXEntertainer(girl, false);


	// SIN: A little pre-show randomness - temporary stats that may affect show
	if (g_Dice.percent(20))
	{
		if (girl->tiredness() > 75)
		{
			ss << girlName << " was very tired. This affected her performance. ";
			jobperformance -= 10;
		}
		else if (girl->libido() > 30)
		{
			ss << girlName << "'s horniness improved her performance. ";
			jobperformance += 10;
		}

		if (girl->has_trait( "Demon") || girl->has_trait( "Shape Shifter") || girl->has_trait( "Construct") ||
			girl->has_trait( "Cat Girl") || girl->has_trait( "Succubus") || girl->has_trait( "Reptilian"))
		{
			ss << "Customers are surprised to see such an unusual girl giving sexual entertainment. ";
			ss << "Some are disgusted, some are turned on, but many can't help watching.\n";
			ss << "The dealers at the tables make a small fortune from distracted guests. ";
			wages += 30;
			fame += 1;
		}
		else if (girl->age() > 30 && g_Dice.percent(min(90, max((girl->age() - 30) * 3, 1))) && girl->beauty() < 30)
		{	//"Too old!" - chance of heckle: age<30y= 0%, then 4%/year (32y - 6%, 40y - 30%...) max 90%... (but only a 20% chance this bit even runs)
			// note: demons are exempt as they age differently
			ss << "Some customers heckle " << girlName << " over her age.";
			ss << "\n\"Gross!\" \"Grandma is that you!?\"\n";
			ss << "This makes it harder for her to work this shift. ";
			jobperformance -= 20;
		}
		else if (girl->has_trait( "Exotic"))
		{
			ss << "The customers were pleasantly surprised to see such an exotic girl giving sexual entertainment.";
			wages += 15;
			fame += 1;
		}
		if ((girl->has_trait( "Syphilis") || girl->has_trait( "Herpes"))
			&& g_Dice.percent(100 - girl->health()))
		{
			ss << "She's unwell. A man in the audience recognises " << girlName << "'s symptoms and heckles her about her ";
			if (girl->has_trait( "Syphilis") && girl->has_trait( "Herpes"))
			{
				ss << "diseases";
			}
			else if (girl->has_trait( "Herpes"))
			{
				ss << "Herpes";
			}
			else if (girl->has_trait( "Syphilis"))
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
			fame -= 3;
		}
		ss << "\n";
	}


	if (jobperformance >= 245)
	{
		ss << " She must be the perfect entertainer customers go on and on about her and always come to see her when she works.\n \n";
		wages += 155;
		if (roll_b <= 20)
		{
			if (girl->has_trait( "Nymphomaniac"))
			{
				ss << girlName << " could do this all day with a genuine smile on her face, pussy dripping onto the stained boards beneath her legs.\n";
			}
			else
			{
				ss << girlName << " has a large devoted group of fans, who come in every night to watch her.\n";
			}
		}
		else if (roll_b <= 40)
		{
			ss << "A simple smile towards the audience from " << girlName << " makes every gambler howl and cheer.\n";
		}
		else if (roll_b <= 60)
		{
			if (girl->has_trait( "Your Daughter"))
			{
				ss << "You probably shouldn't be watching your own daughter doing this, but " << girlName << "'s so damn good it's impossible not to.\n";
			}
			else
			{
				ss << "It's amazing how many tricks " << girlName << " knows, to tease her audience but never quite push them over the point where they'd lose interest.\n";
			}
		}
		else if (roll_b <= 80)
		{
			ss << girlName << " stripped in a unique way tonight, dancing with a razor in each hand.  With those razors, she peeled her clothes away from her lovely body, without nicking herself a single time.\n";
		}
		else
		{
			if (girl->has_trait( "Sexy Air"))
			{
				ss << girlName << " could be dressed in a sack and still make a fortune masturbating on stage.\n";
			}
			else
			{
				ss << girlName << "'s shows are always sold out, and her fame is spreading across the city.\n";
				fame += 1;
			}
		}
	}
	else if (jobperformance >= 185)
	{
		ss << " She's unbelievable at this and is always getting praised by the customers for her work.\n \n";
		wages += 95;
		if (roll_b <= 20)
		{
			if (girl->has_trait( "Nymphomaniac"))
			{
				ss << girlName << " loves her job. You can tell by the zeal she employs as she penetrates her mouth and pussy with dildoes on stage, moaning throughout.\n";
			}
			else
			{
				ss << "From start to finish, every move " << girlName << " makes practically sweats sexuality.\n";
			}
		}
		else if (roll_b <= 40)
		{
			if (girl->has_trait( "Exhibitionist"))
			{
				ss << girlName << " feels so comfortable naked in front of strangers. She loves having all these strangers staring at her naked body ";
				ss << " and rewards the audience with a XXX show they will never forget.\n";
			}
			else ss << girlName << " moved off the stage halfway through the act and walked amongst the audience, to their joy.\n";
		}
		else if (roll_b <= 60)
		{
			ss << "As clothing slowly fell from her body and her hand descended towards her core, " << girlName << " smiled seductively at the audience.\n";
		}
		else if (roll_b <= 80)
		{
			ss << "All male members of " << girlName << "'s audience had trouble standing after she fellated a dildo on stage.\n";
		}
		else
		{
			if (girl->has_trait( "Sexy Air"))
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
		ss << " She's good at this job and gets praised by the customers often.\n \n";
		wages += 55;
		if (roll_b <= 20)
		{
			ss << "A lucky gambler almost fainted when " << girlName << " gave him her freshly removed skirt as a present.\n";
		}
		else if (roll_b <= 40)
		{
			if (girl->has_trait( "Long Legs"))
			{
				ss << girlName << "'s erotic dances are accentuated by her perfect legs.\n";
			}
			else
			{
				ss << girlName << " has a small, but devoted fanbase, who are willing to pay gate fees just to watch her dance.\n";
			}
		}
		else if (roll_b <= 60)
		{
			ss << "After letting " << girlName << " choose the music that she danced to, her erotic dances have improved markedly.\n";
		}
		else if (roll_b <= 80)
		{
			ss << "You watched with amusement as members of her audience made a disproportionate number of trips to the bathroom after her performance.\n";
		}
		else
		{
			if (girl->has_trait( "Sexy Air"))
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
		ss << " She made a few mistakes but overall she is okay at this.\n \n";
		wages += 15;
		if (roll_b <= 20)
		{
			if (girl->has_trait( "Sexy Air"))
			{
				ss << girlName << "'s dances are mechanical, but her aurora of sexuality make clients break out in sweats.\n";
			}
			else
			{
				ss << girlName << "'s striptease draws a decent crowd each night.\n";
			}
		}
		else if (roll_b <= 40)
		{
			ss << girlName << " is not the best erotic dancer you've ever seen, but the gamblers enjoy the eyecandy.\n";
		}
		else if (roll_b <= 60)
		{
			ss << "The cleaners always hate having to clean up after " << girlName << "'s audience.\n";
		}
		else if (roll_b <= 80)
		{
			ss << girlName << " got a round of applause when she bowed, and an even bigger one when her ";

			//SIN: new additional traits added.
			if (girl->has_trait( "Abnormally Large Boobs") ||
				girl->has_trait( "Titanic Tits") ||
				girl->has_trait( "Massive Melons"))
			{
				ss << "enormous breasts \"accidentally\" burst out from her top.\n";
			}
			else if (girl->has_trait( "Big Boobs") || girl->has_trait( "Busty Boobs") || girl->has_trait( "Giant Juggs"))
			{
				ss << "large breasts \"accidentally\" flopped out of her top.\n";
			}
			else if (girl->has_trait( "Small Boobs") || girl->has_trait( "Petite Breasts"))
			{
				ss << "shirt \"accidentally\" exposed her small and perky breasts.\n";
			}
			else if (girl->has_trait( "Flat Chest"))
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
			if (girl->has_trait( "Long Legs"))
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
		ss << " She was nervous and made a few mistakes. She isn't that good at this.\n \n";
		wages -= 5;
		if (roll_b <= 20)
		{
			if (girl->has_trait( "Sexy Air"))
			{
				ss << "Even though a block of wood could dance better, + " << girlName << " still draws the eye.\n";
			}
			else
			{
				ss << girlName << " is still incredibly sexy, despite her lack of anything resembling an active customer appeal.\n";
			}
		}
		else if (roll_b <= 40)
		{
			ss << girlName << "'s distaste for the crowd was evident by her curled lip and perpetual scowl.\n";
		}
		else if (roll_b <= 60)
		{
			if (girl->has_trait( "Slut"))
			{
				ss << "A total slut, " << girlName << " tried to liven up her show by getting one of the security team to fuck her on stage. He refused.";
			}
			else
			{
				ss << "The only person who seemed to find " << girlName << "'s striptease appetizing was a random drunk.\n";
			}
		}
		else if (roll_b <= 80)
		{
			if (girl->has_trait( "Long Legs"))
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
		ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.\n \n";
		wages -= 15;
		if (roll_b <= 20)
		{
			if (girl->has_trait( "Long Legs"))
			{
				ss << "Customers just can't tear their eyes away from " << girlName << "'s long, tasty legs, even as she tripped over her discarded top.\n";
			}
			else
			{
				ss << girlName << " slipped on a fresh smear of ejaculate halfway through her dance, and fell on her ass.\n";
			}
		}
		//SIN: added more dialogue options
		else if (roll_b <= 40)
		{
			if (girl->has_trait( "Strong Gag Reflex") || girl->has_trait( "Gag Reflex"))
			{
				ss << girlName << " was sucking off a dildo, when she made herself gag and threw up on stage.\n";
				brothel->m_Filthiness += 5;
			}
			else
			{
				ss << girlName << "'s hair got tangled up in her shirt when she tried to take it off, making her reel about the stage like a drunk.\n";
			}
		}
		else if (roll_b <= 60)
		{
			if (girl->magic() < 20)
			{
				ss << girlName << " tried using magic light to enhance her show, but ended up setting her pubic hair on fire.\n";
			}
			else
			{
				ss << "Seeing the large crowd waiting outside, " << girlName << "'s nerve broke and she ran back out of the gambling hall.\n";
			}
		}
		else if (roll_b <= 80)
		{
			ss << girlName << " was drunk when she stumbled out at the beginning of shift. ";
			if (g_Dice.percent(girl->combat()))
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
			if (girl->has_trait( "Sexy Air"))
			{
				ss << "Even though a block of wood could dance better, + " << girlName << " still draws the eye.\n";
			}
			else
			{
				ss << "Somehow, " << girlName << " managed to be so unsexy that the audience barely seemed to notice her presence.\n";
			}
		}
	}


	//base tips, aprox 5-40% of base wages
	tips += (int)(((5 + jobperformance / 6) * wages) / 100);

	//try and add randomness here
	if (girl->beauty() > 85 && g_Dice.percent(20))
	{
		ss << "Stunned by her beauty a customer left her a great tip.\n \n"; tips += 25;
	}

	if (girl->has_trait( "Clumsy") && g_Dice.percent(15))
	{
		ss << "Her clumsy nature caused her to lose one of her \"toys\" up a hole. ";
		if (g_Dice.percent(20) || girl->has_trait( "Psychic") || girl->has_trait( "Exhibitionist") || girl->medicine() > 25)
		{
			ss << "She put on a damn sexy show of getting it back out.\n"; tips += 10;
			fame += 1;
		}
		else
		{
			ss << "She panicked and ran off stage to go find a doctor.\n"; tips -= 15;
			fame -= 1;
		}
	}

	if (girl->has_trait( "Pessimist") && g_Dice.percent(5))
	{
		if (jobperformance < 125)
		{
			ss << "Her pessimism made her unadventurous. The customers were unimpressed and the tips were lower.\n"; tips -= 10;
		}
		else
		{
			ss << girlName << " performed well despite her mood. The customers enjoy the darker, sultry nature of her sex shows.\n"; tips += 10;
		}
	}

	if (girl->has_trait( "Optimist") && g_Dice.percent(5))
	{
		if (jobperformance < 125)
		{
			ss << girlName << " smiled far too much to look seductive.\n"; tips -= 10;
		}
		else
		{
			ss << girlName << " showed endless energy, agility and enthusiasm as she rucked around the stage.\n"; tips += 10; girl->agility(1);
		}
	}

	if (girl->has_trait( "Cat Girl") && g_Dice.percent(10))
	{
		ss << girlName << " is able to clean herself like a cat. Customers are amazed as this pussy-cat ";
		ss << "spreads her legs wide and licks her own cunt right there on stage.\n";
		fame += 2;
		tips += 15;
	}

	if (girl->has_trait( "Masochist") && g_Dice.percent(10))
	{
		ss << girlName << " invites a few customers to punish her on stage. They happily agree to spank and whip her.\n";
		tips += 15;
		girl->health(-2);
		girl->bdsm(1);
	}
	if (girl->has_trait( "Your Daughter") && g_Dice.percent(20))
	{
		ss << "Word got around that " << girlName << " is your daughter, so more customers than normal came to watch her perform.\n";
		wages += (wages / 5);
		if (The_Player->disposition() > 0)
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
			fame += 5;
		}
		else
		{
			ss << " it's just a damn shame she sucks at it.\n";
		}
	}

	if (girl->libido() > 90)
	{
		if (girl->has_trait( "Futanari"))
				{
						//Addiction bypasses confidence check
						if (girl->has_trait( "Cum Addict"))
						{
								//Autofellatio, belly gets in the way if pregnant, requires extra flexibility
								if ( girl->has_trait( "Flexible") && !girl->is_pregnant() && g_Dice.percent(50))
								{
										ss << "During her shift " << girlName << " couldn't resist the temptation of taking a load of hot, delicious cum in her mouth and began to suck her own cock. The customers enjoyed a lot such an unusual show.";
										girl->oralsex(1);
										girl->happiness(1);
										fame += 1;
										tips += 30;
								}
								else
								{
										//default Cum Addict
										ss << girlName << " won't miss a chance to taste some yummy cum. She came up on the stage with a goblet, cummed in it and then drank the content to entertain the customers.";
										girl->happiness(1);
										tips += 10;
								}
								GetMiscCustomer(*brothel);
								brothel->m_Happiness += 100;
								girl->upd_temp_stat(STAT_LIBIDO, -30, true);
								// work out the pay between the house and the girl
								wages += girl->askprice() + 60;
								fame += 1;
								imagetype = IMGTYPE_MAST;
						}
						//Let's see if she has what it takes to do it: Confidence > 65 or Exhibitionist trait, maybe shy girls should be excluded
						else if (!girl->has_trait( "Cum Addict") && girl->has_trait( "Exhibitionist") || !girl->has_trait( "Cum Addict") && girl->confidence() > 65)
						{
								//Some variety
								//Autopaizuri, requires very big breasts
								if (g_Dice.percent(25) && girl->has_trait( "Abnormally Large Boobs") || g_Dice.percent(25) && (girl->has_trait( "Titanic Tits")))
								{
										ss << girlName << " was horny and decided to deliver a good show. She put her cock between her huge breasts and began to slowly massage it. The crowd went wild when she finally came on her massive tits.";
										girl->tittysex(1);
										fame += 1;
										tips += 30;
								}
								//cums over self
								else if (girl->dignity() < -40 && g_Dice.percent(25))
								{
										ss << "The customers were really impressed when " << girlName << " finished her show by cumming all over herself";
										tips += 10;
								}
								//Regular futa masturbation
								else
								{
										ss << girlName << "'s cock was hard all the time and she ended up cumming on stage. The customers enjoyed it but the cleaning crew won't be happy.";
										brothel->m_Filthiness += 1;
								}
								GetMiscCustomer(*brothel);
								brothel->m_Happiness += 100;
								girl->upd_temp_stat(STAT_LIBIDO, -30, true);
								// work out the pay between the house and the girl
								wages += girl->askprice() + 60;
								fame += 1;
								imagetype = IMGTYPE_MAST;
						}
						else
						{
								ss << "There was a noticeable bulge in " << girlName << "'s panties but she didn't have enough confidence to masturbate in public.";
						}
				}
				//regular masturbation code by Crazy tweaked to exclude futas and keep the original Libido > 90 requirement
				else if (!girl->has_trait( "Futanari") && girl->libido() > 90)
				{
						ss << "She was horny and ended up masturbating for the customers making them very happy.";
						GetMiscCustomer(*brothel);
						brothel->m_Happiness += 100;
						girl->upd_temp_stat(STAT_LIBIDO, -20, true);
						// work out the pay between the house and the girl
						wages += girl->askprice() + 60;
						fame += 1;
						imagetype = IMGTYPE_MAST;
				}
	}



	if (wages < 0) wages = 0;
	if (tips < 0) tips = 0;

#pragma endregion
#pragma region	//	Enjoyment and Tiredness		//

	//enjoyed the work or not
	if (roll_a <= 5)
	{
		ss << "\nSome of the patrons abused her during the shift."; work -= 1;
	}
	else if (roll_a <= 25)
	{
		ss << "\nShe had a pleasant time working."; work += 3;
		if (girl->has_trait( "Exhibitionist"))	{ work += 1; }
	}
	else
	{
		ss << "\nOtherwise, the shift passed uneventfully."; work += 1;
		if (girl->has_trait( "Exhibitionist"))	{ work += 1; }
	}

#pragma endregion
#pragma region	//	Money					//


#pragma endregion
#pragma region	//	Finish the shift			//


	girl->upd_Enjoyment(ACTION_WORKHALL, work);
	girl->upd_Enjoyment(ACTION_WORKSTRIP, work);

	girl->m_Events.AddMessage(ss.str(), imagetype, Day0Night1);



	// work out the pay between the house and the girl
	wages += (g_Dice % ((int)(((girl->beauty() + girl->charisma()) / 2)*0.5f))) + 10;
	// Money
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);


	// Improve girl
	int xp = 10, libido = 1, skill = 2;

	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }
	if (girl->fame() < 10 && jobperformance >= 70)		{ fame += 1; }
	if (girl->fame() < 30 && jobperformance >= 100)		{ fame += 1; }
	if (girl->fame() < 60 && jobperformance >= 145)		{ fame += 1; }
	if (girl->fame() < 80 && jobperformance >= 185)		{ fame += 1; }

	girl->fame(fame);
	girl->exp(xp);
	girl->confidence(g_Dice%skill);
	girl->strip(g_Dice%skill + 1);
	girl->performance(g_Dice%skill + 1);
	girl->upd_temp_stat(STAT_LIBIDO, libido);

	//gain traits
	g_Girls.PossiblyGainNewTrait(girl, "Nymphomaniac", 75, ACTION_WORKSTRIP, "Having to perform sexual entertainment for patrons every day has made " + girlName + " quite the nympho.", Day0Night1);
	//SIN: new trait
	g_Girls.PossiblyGainNewTrait(girl, "Exhibitionist", 50, ACTION_WORKSTRIP, "Performing sexual entertainment for strangers every day has made " + girlName + " quite keen to show off her sexuality.", Day0Night1);

	if (jobperformance >= 140 && g_Dice.percent(25))
	{
		g_Girls.PossiblyGainNewTrait(girl, "Sexy Air", 80, ACTION_WORKSTRIP, girlName + " has been having to be sexy for so long she now reeks  sexiness.", Day0Night1);
	}

#pragma endregion
	return false;
}

double cJobManager::JP_HallXXXEntertainer(sGirl* girl, bool estimate)// not used
{
#if 1	//SIN - standardizing job performance per J's instructs
	double jobperformance =
		//Core stats - performance & beauty: can she put on a show, and will anyone want to see?
		((girl->performance() + girl->beauty()) / 2) +
		//secondary stats - personality, strip skills and how into it she is
		((girl->charisma() + girl->confidence() + girl->strip() + girl->libido()) / 4) +
		//add level
		girl->level();

	// next up tiredness penalty...
#else
	double jobperformance =
		((girl->charisma() +
		girl->beauty() +
		girl->confidence()) / 3 +
		girl->strip() / 2 +
		girl->performance() / 2);
#endif
	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}


	//good traits
	if (girl->has_trait( "Charismatic"))  jobperformance += 10;
	if (girl->has_trait( "Sexy Air"))		jobperformance += 10;
	if (girl->has_trait( "Cool Person"))  jobperformance += 5; //people love to be around her
	if (girl->has_trait( "Cute"))			jobperformance += 5;
	if (girl->has_trait( "Charming"))		jobperformance += 5; //people like charming people
	if (girl->has_trait( "Great Figure")) jobperformance += 10;
	if (girl->has_trait( "Great Arse"))   jobperformance += 5;
	if (girl->has_trait( "Quick Learner"))jobperformance += 5;
	if (girl->has_trait( "Psychic"))		jobperformance += 10;
	if (girl->has_trait( "Long Legs"))	jobperformance += 5;
	if (girl->has_trait( "Exhibitionist"))jobperformance += 10;
	if (girl->has_trait( "Dick-Sucking Lips"))jobperformance += 5;
	if (girl->has_trait( "Fearless"))		jobperformance += 5;
	if (girl->has_trait( "Natural Pheromones"))jobperformance += 10; //SIN - exudes sex
	if (girl->has_trait( "Agile"))		jobperformance += 5;  //good movement
	if (girl->has_trait( "Flexible"))		jobperformance += 10; //positions
	if (girl->has_trait( "Porn Star"))	jobperformance += 10; //this is her day job
	if (girl->has_trait( "Social Drinker"))jobperformance += 5; //uninhibited


	//bad traits
	if (girl->has_trait( "Dependant"))	jobperformance -= 50; // needs others to do the job
	if (girl->has_trait( "Clumsy"))		jobperformance -= 10; //spills food and breaks things often
	if (girl->has_trait( "Aggressive"))	jobperformance -= 20; //gets mad easy and may attack people
	if (girl->has_trait( "Nervous"))		jobperformance -= 30; //don't like to be around people
	if (girl->has_trait( "Meek"))			jobperformance -= 20;
	if (girl->has_trait( "Shy"))			jobperformance -= 20;
	if (girl->has_trait( "Slow Learner"))	jobperformance -= 10;
	if (girl->has_trait( "Horrific Scars")) jobperformance -= 15;
	if (girl->has_trait( "Small Scars"))	jobperformance -= 5;
	if (girl->is_pregnant())					jobperformance -= 5; //can't move so well
	if (girl->has_trait( "Flat Ass"))		jobperformance -= 10;
	if (girl->has_trait( "Flat Chest"))	jobperformance -= 10;

	if (girl->has_trait( "One Arm"))		jobperformance -= 40;
	if (girl->has_trait( "One Foot"))		jobperformance -= 40;
	if (girl->has_trait( "One Hand"))		jobperformance -= 30;
	if (girl->has_trait( "One Leg"))		jobperformance -= 60;
	if (girl->has_trait( "No Arms"))		jobperformance -= 125;
	if (girl->has_trait( "No Feet"))		jobperformance -= 60;
	if (girl->has_trait( "No Hands"))		jobperformance -= 50;
	if (girl->has_trait( "No Legs"))		jobperformance -= 150;
	if (girl->has_trait( "Blind"))		jobperformance -= 30;
	if (girl->has_trait( "Deaf"))			jobperformance -= 15;
	if (girl->has_trait( "Retarded"))		jobperformance -= 60;
	if (girl->has_trait( "Smoker"))		jobperformance -= 10;	//would need smoke breaks

	if (girl->has_trait( "Alcoholic"))			jobperformance -= 25;
	if (girl->has_trait( "Fairy Dust Addict"))	jobperformance -= 25;
	if (girl->has_trait( "Shroud Addict"))		jobperformance -= 25;
	if (girl->has_trait( "Viras Blood Addict"))	jobperformance -= 25;


	return jobperformance;
}
