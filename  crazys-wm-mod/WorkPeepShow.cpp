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

#pragma endregion

// `J` Job Brothel - Brothel
bool cJobManager::WorkPeepShow(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
#pragma region //	Job setup				//
	int actiontype = ACTION_WORKSTRIP;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();
	if (girl->disobey_check(actiontype, brothel))
	{
		//SIN - More informative mssg to show *what* she refuses
		ss << " refused to be in your brothel's peep show " << (Day0Night1 ? "tonight." : "today.");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " let the customers watch her get naked.\n \n";

	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!


	int wages = girl->askprice() + g_Dice % 50;
	int tips = max((g_Dice % 50) - 10, 0);
	int enjoy = 0, fame = 0;
	u_int sextype = SKILL_STRIP;
	int imagetype = IMGTYPE_STRIP;
	int msgtype = Day0Night1;

#pragma endregion
#pragma region //	Job Performance			//

	double jobperformance = JP_PeepShow(girl, false);
	double mod = 0.0;

	if (jobperformance >= 245)
	{
		mod = 3.0;/*           */ ss << "She must be the perfect teaser she never goes to fast but never to slow, she knows just when to do things. The customers can't get enough of her.\n \n";
		/* */if (roll_b <= 20)	{ ss << "Knowing every trick in the book and not hesitating to use them, she makes a lot of money today.\n"; }
		else if (roll_b <= 40)	{ ss << "She is just a gold mine.\n"; }
		else if (roll_b <= 60)	{ ss << "She certainly knows how to sell the show. After hours of teasing she finally gets naked.\n"; }
		else if (roll_b <= 80)	{ ss << girlName << " was requested by name by a single customer. He paid double of her daily earnings for the privilege of getting an exclusive performance.\n"; }
		else /*             */	{ ss << girlName << " is performing today. Like always the cleaning crew have a lot of work after her shows.\n"; }
	}
	else if (jobperformance >= 185)
	{
		mod = 2.0;				ss << "She is unbelievable at this and is always finding new ways of pleasing the customers.\n \n";
		/* */if (roll_b <= 20) { ss << "She is really good at this, knowing how to tease customers into staying and paying for hours.\n"; }
		else if (roll_b <= 40) { ss << girlName << " swinging her boobs and ass in a very sexual way. As a result causing heavy breathing from the booths.\n"; }
		else if (roll_b <= 60) { ss << "Her body movement is flawless and very sensual, earning a nice sum today.\n"; }
		else if (roll_b <= 80) { ss << girlName << " gave a very good performance, making her clients very satisfied with the time they spent here.\n"; }
		else /*           */ { ss << girlName << " was requested by name by a group of customers. She gave them a special performance for a really high price.\n"; }
	}
	else if (jobperformance >= 145)
	{
		mod = 1.5;				ss << "She's good at this job and knows a few tricks to drive the customers wild.\n \n";
		/* */if (roll_b <= 20) { ss << "She has a good body and sense of rhythm, but she strips too fast, so she make less money from today performance.\n"; }
		else if (roll_b <= 40) { ss << "She makes the mistake of getting naked too fast, ending the show prematurely.\n"; }
		else if (roll_b <= 60) { ss << "She certainly has the skills for this job, but with more work she will get even better.\n"; }
		else if (roll_b <= 80) { ss << girlName << " comes on stage with some 'toys' that she uses to prolong her performance.\n"; }
		else /*           */ { ss << "Having confidence in her skills, " << girlName << " gives a very good performance.\n"; }
	}
	else if (jobperformance >= 100)
	{
		mod = 1.0;				ss << "She made a few mistakes but overall she is okay at this.\n \n";
		/* */if (roll_b <= 20) { ss << girlName << " can strip and people come here to see her do that but there is nothing extraordinary in her performance.\n"; }
		else if (roll_b <= 40) { ss << "Pretty boring show, But at least she gets naked all the way thru.\n"; }
		else if (roll_b <= 60) { ss << "Having stage fright, her movement seems mechanical, leaving paying customers unsatisfied.\n"; }
		else if (roll_b <= 80) { ss << "Not too confident in herself, " << girlName << "'s tense performance rubs off on the customers.\n"; }
		else /*           */ { ss << "She did good today. With improving her skills, " << girlName << " could become one of the best.\n"; }
	}
	else if (jobperformance >= 70)
	{
		mod = 0.9;				ss << "She was nervous and made a few mistakes. She isn't that good at this.\n \n";
		/* */if (roll_b <= 20) { ss << "When it was time for " << girlName << " to take off her panties, she collapsed on the floor covering her breasts and crying.\n"; }
		else if (roll_b <= 40) { ss << "Not being able to unbuckle her fancy bra, she struggled for minutes. Some of the customers left demanding their money back.\n"; }
		else if (roll_b <= 60) { ss << "She gets naked but in a boring and not exciting way.\n"; }
		else if (roll_b <= 80) { ss << "Trying a more advanced pose, she hurts herself, falling painfully on the floor.\n"; }
		else /*           */ { ss << girlName << " definitely tries to give a good performance, but having only good will isn't enough.\n"; }
	}
	else
	{
		mod = 0.8;				ss << "She was nervous and constantly making mistakes. She really isn't very good at this job.\n \n";
		/* */if (roll_b <= 20) { ss << "She is awful at this job. Her stage movement and striptease are lifeless.\n"; }
		else if (roll_b <= 40) { ss << "Her stripping ability could be compared to getting undressed before a shower.\n"; }
		else if (roll_b <= 60) { ss << "Her show was so bad, that you could swear you heard snoring from one booth.\n"; }
		else if (roll_b <= 80) { ss << "When trying to take off her t-shirt, " << girlName << "'s head got stuck for minutes.\n"; }
		else /*           */ { ss << "Trying to give her best, " << girlName << " gave a poor performance.\n"; }
	}


	//try and add randomness here
	if (girl->libido() > 80)
	{
		if (girl->has_trait( "Lesbian") && (girl->has_trait( "Nymphomaniac") || girl->has_trait( "Succubus")))
		{
			ss << "\nShe was horny and she loves sex so she brought in another girl and had sex with her while the customers watched.\n";
			sextype = SKILL_LESBIAN;
			/* `J` g_Girls.GirlFucks handles skill gain from sex
			girl->lesbian(1);
			//*/
		}
		else if (girl->has_trait( "Bisexual") && (girl->has_trait( "Nymphomaniac") || girl->has_trait( "Succubus")))
		{
			if (roll_c <= 50)
			{
				ss << "\nShe was horny and she loves sex so she brought in another girl and had sex with her while the customers watched.\n";
				sextype = SKILL_LESBIAN;
				/* `J` g_Girls.GirlFucks handles skill gain from sex
				girl->lesbian(1);
				//*/
			}
			else
			{
				ss << "\nShe was horny and she loves sex so she brought in one of the customers and had sex with him while the others watched.";
				sextype = SKILL_NORMALSEX;
			}
		}
		else if (girl->has_trait( "Straight") && (girl->has_trait( "Nymphomaniac") || girl->has_trait( "Succubus")))
		{
			ss << "\nShe was horny and she loves sex so she brought in one of the customers and had sex with him while the others watched.\n";
			sextype = SKILL_NORMALSEX;
		}
		// new stuff
		else if (girl->has_trait( "Futanari"))
		{
			//Addiction bypasses confidence check
			if (girl->has_trait( "Cum Addict"))
			{
				//Autofellatio, belly gets in the way if pregnant, requires extra flexibility
				if (girl->has_trait( "Flexible") && !(girl->is_pregnant()) && g_Dice.percent(50))
				{
					ss << "\nDuring her shift " << girlName << " couldn't resist the temptation of taking a load of hot, delicious cum in her mouth and began to suck her own cock. The customers enjoyed a lot such an unusual show.";
					girl->oralsex(1);
					girl->happiness(1);
					fame += 1;
					tips += 30;
				}
				else
				{
					//default Cum Addict
					ss << "\n" << girlName << " won't miss a chance to taste some yummy cum. She came up on the stage with a goblet, cummed in it and then drank the content to entertain the customers.";
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
					ss << "\n" << girlName << " was horny and decided to deliver a good show. She put her cock between her huge breasts and began to slowly massage it. The crowd went wild when she finally came on her massive tits.";
					girl->tittysex(1);
					fame += 1;
					tips += 30;
				}
				//cums over self
				else if (girl->dignity() < -40 && g_Dice.percent(25))
				{
					ss << "\nThe customers were really impressed when " << girlName << " finished her show by cumming all over herself";
					tips += 10;
				}
				//Regular futa masturbation
				else
				{
					ss << "\n" << girlName << "'s cock was hard all the time and she ended up cumming on stage. The customers enjoyed it but the cleaning crew won't be happy.";
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
				ss << "\nThere was a noticeable bulge in " << girlName << "'s panties but she didn't have enough confidence to masturbate in public.";
			}
		}
		else
		{
			//GirlFucks handles all the stuff for the other events but it isn't used here so everything has to be added manually
			//It's is the same text as the XXX entertainer masturbation event, so I'll just copy the rest
			ss << "\nShe was horny and ended up masturbating for the customers, making them very happy.";
			GetMiscCustomer(*brothel);
			brothel->m_Happiness += 100;
			girl->upd_temp_stat(STAT_LIBIDO, -30, true);
			// work out the pay between the house and the girl
			wages += girl->askprice() + 60;
			fame += 1;
			imagetype = IMGTYPE_MAST;
		}
	}
	else if (g_Dice.percent(5))  //glory hole event
	{
		ss << "A man managed to cut a hole out from his booth and made himself a glory hole, " << girlName
			<< " saw his cock sticking out and ";
		{
			if (girl->has_trait( "Meek") || girl->has_trait( "Shy"))
			{
				enjoy -= 5;
				ss << "meekly ran away from it.\n";
			}
			else if (girl->has_trait( "Lesbian"))
			{
				enjoy -= 2;
				girl->upd_temp_stat(STAT_LIBIDO, -10, true);
				ss << "she doesn't understand the appeal of them, which turned her off.\n";
			}
			else if (!brothel->m_RestrictNormal && !girl->check_virginity() && (girl->has_trait( "Nymphomaniac") || girl->has_trait( "Succubus")) && girl->libido() >= 80) //sex
			{
				sextype = SKILL_NORMALSEX;
				ss << "decided she needed to use it for her own entertainment.\n";
			}
			else if (!brothel->m_RestrictOral && (girl->has_trait( "Nymphomaniac") || girl->has_trait( "Succubus") || girl->has_trait( "Cum Addict")) && girl->libido() >= 60) //oral
			{
				sextype = SKILL_ORALSEX;
				ss << "decided she needed to taste it.\n";
			}
			else if (!brothel->m_RestrictFoot && (girl->has_trait( "Nymphomaniac") || girl->has_trait( "Succubus") || girl->dignity() < -30) && girl->libido() >= 40) //foot
			{
				sextype = SKILL_FOOTJOB;
				imagetype = IMGTYPE_FOOT;
				ss << "decided she would give him a foot job for being so brave.\n";
			}
			else if (!brothel->m_RestrictHand && (girl->has_trait( "Nymphomaniac") || girl->has_trait( "Succubus") || girl->dignity() < -30))	//hand job
			{
				sextype = SKILL_HANDJOB;
				ss << "decided she would give him a hand job for being so brave.\n";
			}
			else
			{
				ss << "pointed and laughed.\n";
			}

			/* `J` suggest adding bad stuff,
			else if (girl->has_trait( "Merciless") && girl->has_item("Dagger") != -1 && g_Dice.percent(10))
			{
			imagetype = IMGTYPE_COMBAT;
			ss << "decided she would teach this guy a lesson and cut his dick off.\n";
			}
			*
			* This would probably require some other stuff and would deffinatly reduce her pay.
			* It may even get the girl arrested if the players suspicion were too high
			*
			//*/
		}
	}

	// `J` calculate base pay and tips with mod before special pay and tips are added
	tips = max(0, int(tips * mod));
	wages = max(0, int(wages * mod));

	if (girl->beauty() > 85 && g_Dice.percent(20))
	{
		ss << "Stunned by her beauty, a customer left her a great tip.\n \n";
		tips += g_Dice % 50 + 10;
	}

	if (sextype != SKILL_STRIP)
	{
		// `J` get the customer and configure them to what is already known about them
		sCustomer Cust = GetMiscCustomer(*brothel);
		Cust.m_Amount = 1;										// always only 1
		Cust.m_SexPref = sextype;								// they don't get a say in this
		if (sextype == SKILL_LESBIAN) Cust.m_IsWoman = true;	// make sure it is a lesbian

		string message = ss.str();
		g_Girls.GirlFucks(girl, Day0Night1, &Cust, false, message, sextype);
		ss.str(""); ss << message;
		brothel->m_Happiness += Cust.m_Stats[STAT_HAPPINESS];

		/* `J` g_Girls.GirlFucks handles libido and customer happiness
		Cust.m_Stats[STAT_HAPPINESS] = max(100, Cust.m_Stats[STAT_HAPPINESS] + 50);
		girl->upd_temp_stat(STAT_LIBIDO, -20);
		//*/

		int sexwages = min(g_Dice % (Cust.m_Money / 4) + girl->askprice(), int(Cust.m_Money));
		Cust.m_Money -= sexwages;
		int sextips = max(0, int(g_Dice%Cust.m_Money - (Cust.m_Money / 2)));
		Cust.m_Money -= sextips;
		wages += sexwages;
		tips += sextips;

		ss << "The customer she had sex with gave her " << sexwages << " gold for her services";
		if (sextips > 0) ss << " and slipped her another " << sextips << " under the table.\n \n";
		else ss << ".\n \n";

		if (imagetype == IMGTYPE_STRIP)
		{
			/* */if (sextype == SKILL_ANAL)			imagetype = IMGTYPE_ANAL;
			else if (sextype == SKILL_BDSM)			imagetype = IMGTYPE_BDSM;
			else if (sextype == SKILL_NORMALSEX)	imagetype = IMGTYPE_SEX;
			else if (sextype == SKILL_BEASTIALITY)	imagetype = IMGTYPE_BEAST;
			else if (sextype == SKILL_GROUP)		imagetype = IMGTYPE_GROUP;
			else if (sextype == SKILL_LESBIAN)		imagetype = IMGTYPE_LESBIAN;
			else if (sextype == SKILL_ORALSEX)		imagetype = IMGTYPE_ORAL;
			else if (sextype == SKILL_TITTYSEX)		imagetype = IMGTYPE_TITTY;
			else if (sextype == SKILL_HANDJOB)		imagetype = IMGTYPE_HAND;
			else if (sextype == SKILL_FOOTJOB)		imagetype = IMGTYPE_FOOT;
		}
	}


#pragma endregion
#pragma region	//	Enjoyment and Tiredness		//

	//enjoyed the work or not
	/* */if (roll_a <= 5)	{ enjoy -= 1; ss << "\nSome of the patrons abused her during the shift."; }
	else if (roll_a <= 25)	{ enjoy += 3; ss << "\nShe had a pleasant time working."; }
	else /*           */	{ enjoy += 1; ss << "\nOtherwise, the shift passed uneventfully."; }

#pragma endregion
#pragma region	//	Money					//


#pragma endregion
#pragma region	//	Finish the shift			//




	girl->upd_Enjoyment(ACTION_WORKSTRIP, enjoy);

	// work out the pay between the house and the girl
	girl->m_Events.AddMessage(ss.str(), imagetype, Day0Night1);
	// Money
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);

	// Improve stats
	int xp = 15, libido = 1, skill = 3;

	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }
	if (girl->fame() < 10 && jobperformance >= 70)		{ fame += 1; }
	if (girl->fame() < 30 && jobperformance >= 100)		{ fame += 1; }
	if (girl->fame() < 60 && jobperformance >= 145)		{ fame += 1; }
	if (girl->fame() < 80 && jobperformance >= 185)		{ fame += 1; }

	girl->fame(fame);
	girl->exp(xp);
	girl->strip(g_Dice%skill + 1);
	girl->performance(g_Dice%skill + 1);
	girl->upd_temp_stat(STAT_LIBIDO, libido);

	//gain traits
		if (jobperformance >= 140 && g_Dice.percent(25))
	{
		g_Girls.PossiblyGainNewTrait(girl, "Sexy Air", 80, ACTION_WORKSTRIP, girlName + " has been having to be sexy for so long she now reeks  sexiness.", Day0Night1);
	}
	if (sextype != SKILL_STRIP && girl->dignity() < 0 && g_Dice.percent(25))
	{
		g_Girls.PossiblyGainNewTrait(girl, "Slut", 80, ACTION_SEX, girlName + " has turned into quite a slut.", Day0Night1, EVENT_WARNING);
	}

	//lose
	g_Girls.PossiblyLoseExistingTrait(girl, "Nervous", 30, ACTION_WORKSTRIP, girlName + " has had so many people see her naked she is no longer nervous about anything.", Day0Night1);

#pragma endregion
	return false;
}

double cJobManager::JP_PeepShow(sGirl* girl, bool estimate)// not used
{
#if 1  //SIN - standardizing job performance calc per J's instructs
	double jobperformance =
		//basing this on payout logic from code above
		//main stats - first 100 - charisma and beauty are used above to calc typical payout
		(girl->charisma() + girl->beauty() / 2) +
		//secondary stats - second 100 - these set her lapdance chance (so chance of higher payout)
		((girl->performance() + girl->strip()) / 2) +
		//add level
		girl->level();

	// next up tiredness penalty
#else
	double jobperformance =
		(girl->charisma() / 2 +
		girl->beauty() / 2 +
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
	if (girl->has_trait( "Charismatic"))              jobperformance += 15;
	if (girl->has_trait( "Sexy Air"))                 jobperformance += 10;
	if (girl->has_trait( "Cool Person"))              jobperformance += 10;  //people love to be around her
	if (girl->has_trait( "Cute"))                     jobperformance += 5;
	if (girl->has_trait( "Charming"))                 jobperformance += 10;  //people like charming people
	if (girl->has_trait( "Great Figure"))             jobperformance += 5;
	if (girl->has_trait( "Great Arse"))               jobperformance += 5;
	if (girl->has_trait( "Quick Learner"))            jobperformance += 5;
	if (girl->has_trait( "Psychic"))                  jobperformance += 10;  //knows what people want to see
	if (girl->has_trait( "Fearless"))                 jobperformance += 10;
	if (girl->has_trait( "Dick-Sucking Lips"))		jobperformance += 5;
	if (girl->has_trait( "Flexible"))					jobperformance += 10;
	if (girl->has_trait( "Exhibitionist"))			jobperformance += 10; //likes showing off her body

	//bad traits
	if (girl->has_trait( "Dependant"))                jobperformance -= 50;  // needs others to do the job
	if (girl->has_trait( "Clumsy"))                   jobperformance -= 20;  //spills food and breaks things often
	if (girl->has_trait( "Aggressive"))               jobperformance -= 20;  //gets mad easy and may attack people
	if (girl->has_trait( "Nervous"))                  jobperformance -= 30;  //don't like to be around people
	if (girl->has_trait( "Meek"))                     jobperformance -= 20;
	if (girl->has_trait( "Shy"))                      jobperformance -= 20;
	if (girl->has_trait( "Slow Learner"))             jobperformance -= 10;
	if (girl->has_trait( "Horrific Scars"))           jobperformance -= 20;
	if (girl->has_trait( "Small Scars"))              jobperformance -= 5;
	if (girl->is_pregnant())								jobperformance -= 5;
	if (girl->has_trait( "Flat Ass"))					jobperformance -= 15;
	if (girl->has_trait( "Flat Chest"))				jobperformance -= 15;

	if (girl->has_trait( "One Arm"))		jobperformance -= 40;
	if (girl->has_trait( "One Foot"))		jobperformance -= 40;
	if (girl->has_trait( "One Hand"))		jobperformance -= 30;
	if (girl->has_trait( "One Leg"))		jobperformance -= 60;
	if (girl->has_trait( "No Arms"))		jobperformance -= 125;
	if (girl->has_trait( "No Feet"))		jobperformance -= 60;
	if (girl->has_trait( "No Hands"))		jobperformance -= 50;
	if (girl->has_trait( "No Legs"))		jobperformance -= 150;
	if (girl->has_trait( "Blind"))		jobperformance -= 15;
	if (girl->has_trait( "Deaf"))			jobperformance -= 15;
	if (girl->has_trait( "Retarded"))		jobperformance -= 60;
	if (girl->has_trait( "Smoker"))		jobperformance -= 10;	//would need smoke breaks

	if (girl->has_trait( "Alcoholic"))			jobperformance -= 25;
	if (girl->has_trait( "Fairy Dust Addict"))	jobperformance -= 25;
	if (girl->has_trait( "Shroud Addict"))		jobperformance -= 25;
	if (girl->has_trait( "Viras Blood Addict"))	jobperformance -= 25;

	return jobperformance;
}
