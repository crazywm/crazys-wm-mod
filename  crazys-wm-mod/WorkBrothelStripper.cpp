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
extern cJobManager m_JobManager;

// `J` Job Brothel - Brothel
bool cJobManager::WorkBrothelStripper(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKSTRIP;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	if (g_Girls.DisobeyCheck(girl, actiontype, brothel))
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " was stripping in the brothel.\n";

	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	int roll = g_Dice.d100();
	double jobperformance = JP_BrothelStripper(girl, false);


	int lapdance = (g_Girls.GetStat(girl, STAT_INTELLIGENCE) / 2 +
		g_Girls.GetSkill(girl, SKILL_PERFORMANCE) / 2 +
		g_Girls.GetSkill(girl, SKILL_STRIP)) / 2;
	int mast = false, sex = false;
	int wages = 45, work = 0, imageType = IMGTYPE_STRIP;
	bool contraception = false;

	if (girl->beauty() > 90)
	{
		ss << "\nShe is so hot, customers were waving money to attract her to dance for them.";
		wages += 20;
	}
	if (girl->intelligence() > 75)
	{
		ss << "\nShe was smart enough to boost her pay by playing two customers against one another.";
		wages += 25;
	}
	if (girl->beauty() <= 90 && girl->intelligence() <= 75)
	{
		ss << "\nShe stripped for a customer.";
	}
	ss << "\n\n";


	//Adding cust here for use in scripts...
	sCustomer cust;
	GetMiscCustomer(brothel, cust);

	//A little more randomness
	if (cust.m_IsWoman && (g_Girls.HasTrait(girl, "Lesbian") || g_Girls.GetSkill(girl, SKILL_LESBIAN) > 60))
	{
		ss << girlName << " was overjoyed to perform for a woman, and gave a much more sensual, personal performance.\n";
		jobperformance += 25;
	}

	if (g_Girls.GetStat(girl, STAT_TIREDNESS) > 70)
	{
		ss << girlName << " was too exhausted to give her best tonight";
		if (jobperformance >= 120)
		{
			ss << ", but she did a fairly good job of hiding her exhaustion.\n";
			jobperformance -= 10;
		}
		else
		{
			ss << ". Unable to even mask her tiredness, she moved clumsily and openly yawned around customers.\n";
			jobperformance -= 30;
		}
	}

	if (jobperformance >= 245)
	{
		ss << "She must be the perfect stripper - customers go on and on about her and always come to see her when she works.";
		wages += 155;
	}
	else if (jobperformance >= 185)
	{
		ss << "She's unbelievable at this and is always getting praised by the customers for her work.";
		wages += 95;
	}
	else if (jobperformance >= 145)
	{
		ss << "She's good at this job and gets praised by the customers often.";
		wages += 55;
	}
	else if (jobperformance >= 100)
	{
		ss << "She made a few mistakes but overall she is okay at this.";
		wages += 15;
	}
	else if (jobperformance >= 70)
	{
		ss << "She was nervous and made a few mistakes. She isn't that good at this.";
		wages -= 5;
	}
	else
	{
		ss << "She was nervous and constantly making mistakes. She really isn't very good at this job.";
		wages -= 15;
	}
	ss << "\n\n";

	// lap dance code.. just test stuff for now
	if (lapdance >= 90)
	{
		ss << girlName << " doesn't have to try to sell private dances the patrons beg her to buy one off her.\n";
		if (roll < 5)
		{
			ss << "She sold a champagne dance.";
			wages += 250;
		}
		else if (roll < 20)
		{
			ss << "She sold a shower dance.\n";
			wages += 125;
			if (g_Girls.GetStat(girl, STAT_LIBIDO) > 70)
			{
				ss << "She was in the mood so she put on quite a show, taking herself to orgasm right in front of the customer.";
				g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, -20);
				wages += 50;
				mast = true;
			}
		}
		else if (roll < 40)
		{
			ss << "She was able to sell a few VIP dances.\n";
			wages += 160;
			if (g_Dice.percent(20)) sex = true;
		}
		else if (roll < 60)
		{
			ss << "She sold a VIP dance.\n";
			wages += 75;
			if (g_Dice.percent(15)) sex = true;
		}
		else
		{
			ss << "She sold several lap dances.";
			wages += 85;
		}
	}
	else if (lapdance >= 65)
	{
		ss << girlName << "'s skill at selling private dances is impressive.\n";
		if (roll < 10)
		{
			ss << "She convinced a patron to buy a shower dance.\n";
			wages += 75;
			if (g_Girls.GetStat(girl, STAT_LIBIDO) > 70)
			{
				ss << "She was in the mood so she put on quite a show, taking herself to orgasm right in front of the customer.";
				g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, -20);
				wages += 50;
				mast = true;
			}
		}
		if (roll < 40)
		{
			ss << "Sold a VIP dance to a patron.\n";
			wages += 75;
			if (g_Dice.percent(20))
			{
				sex = true;
			}
		}
		else
		{
			ss << "Sold a few lap dances.";
			wages += 65;
		}
	}
	else if (lapdance >= 40)
	{
		ss << girlName << " tried to sell private dances and ";
		if (roll < 5)
		{
			ss << "was able to sell a vip dance againts all odds.\n";
			wages += 75;
			if (g_Dice.percent(10))
			{
				sex = true;
			}
		}
		if (roll < 20)
		{
			ss << "was able to sell a lap dance.";
			wages += 25;
		}
		else
		{
			ss << "wasn't able to sell any.";
		}
	}
	else
	{
		ss << girlName << "'s doesn't seem to understand the real money in stripping is selling private dances.\n";
	}


	//try and add randomness here
	if (g_Girls.GetStat(girl, STAT_BEAUTY) > 85 && g_Dice.percent(20))
	{
		ss << "Stunned by her beauty a customer left her a great tip.\n\n"; wages += 25;
	}

	if (g_Girls.HasTrait(girl, "Clumsy") && g_Dice.percent(5))
	{
		ss << " Her clumsy nature caused her to slide off the pole causing her to have to stop stripping for a few hours.\n"; wages -= 15;
	}

	if (g_Girls.HasTrait(girl, "Pessimist") && g_Dice.percent(5))
	{
		if (jobperformance < 125)
		{
			ss << " Her pessimistic mood depressed the customers making them tip less.\n"; wages -= 10;
		}
		else
		{
			ss << girlName << " was in a poor mood so the patrons gave her a bigger tip to try and cheer her up.\n"; wages += 10;
		}
	}

	if (g_Girls.HasTrait(girl, "Optimist") && g_Dice.percent(5))
	{
		if (jobperformance < 125)
		{
			ss << girlName << " was in a cheerful mood but the patrons thought she needed to work more on her stripping.\n"; wages -= 10;
		}
		else
		{
			ss << " Her optimistic mood made patrons cheer up increasing the amount they tip.\n"; wages += 10;
		}
	}

	if (g_Girls.HasTrait(girl, "Great Figure") && g_Dice.percent(20))
	{
		if (jobperformance < 125)
		{
			ss << girlName << " has a great figure so she draws a few extra patrons even if she needed to work more on her stripping.\n"; wages += 5;
		}
		else
		{
			ss << girlName << "'s great figure draws a large crowed to the stage and her skill at stripping makes them pay up to see the show up close.\n"; wages += 15;
		}
	}

	//if (g_Dice.percent(10))//ruffe event
	//{
	//	ss << "A patron keep buying her drinks \n";
	//	if (g_Girls.GetStat(girl, SKILL_HERBALISM) > 35)
	//	{
	//		ss << "but she noticed an extra taste that she knew was a drug to make her pass out. She reported him to secuirty and he was escorted out. Good news is she made a good amount of money off him before this.\n"; wages += 25;
	//	}
	//	else
	//	{
	//		//guy gets to have his way with her
	//	}
	//}

	if (girl->is_addict(true) && !sex && !mast && g_Dice.percent(60)) //not going to get money or drugs any other way
	{
		string warning = "Noticing her addiction, a customer offered her drugs for a blowjob. She accepted, taking him out of sight of security and sucking him off for no money.\n";
		ss << "\n" << warning << "\n";
		if (g_Girls.HasTrait(girl, "Shroud Addict"))		g_Girls.AddInv(girl, g_InvManager.GetItem("Shroud Mushroom"));
		if (g_Girls.HasTrait(girl, "Fairy Dust Addict"))	g_Girls.AddInv(girl, g_InvManager.GetItem("Fairy Dust"));
		if (g_Girls.HasTrait(girl, "Viras Blood Addict"))	g_Girls.AddInv(girl, g_InvManager.GetItem("Vira Blood"));
		girl->m_Events.AddMessage(warning, IMGTYPE_ORAL, EVENT_WARNING);
	}

	if (sex)
	{
		u_int n;
		ss << "In one of the private shows, she ended up ";
		brothel->m_Happiness += 100;
		//int imageType = IMGTYPE_SEX;
		if (cust.m_IsWoman && m_JobManager.is_sex_type_allowed(SKILL_LESBIAN, brothel))
		{
			n = SKILL_LESBIAN;
			ss << "licking the customer's clit until she screamed out in pleasure, making her very happy.";
		}
		else
		{
			switch (g_Dice % 10)
			{
			case 0:        n = SKILL_ORALSEX;   ss << "sucking the customer off";					break;
			case 1:        n = SKILL_TITTYSEX;  ss << "using her tits to get the customer off";		break;
			case 2:        n = SKILL_HANDJOB;   ss << "using her hand to get the customer off";     break;
			case 3:        n = SKILL_ANAL;      ss << "letting the customer use her ass";			break;
			case 4:        n = SKILL_FOOTJOB;   ss << "using her feet to get the customer off";     break;
			default:	   n = SKILL_NORMALSEX; ss << "fucking the customer as well";				break;
			}
			ss << ", making him very happy.\n";
		}
		/* */if (n == SKILL_LESBIAN)	imageType = IMGTYPE_LESBIAN;
		else if (n == SKILL_ORALSEX)	imageType = IMGTYPE_ORAL;
		else if (n == SKILL_TITTYSEX)	imageType = IMGTYPE_TITTY;
		else if (n == SKILL_HANDJOB)	imageType = IMGTYPE_HAND;
		else if (n == SKILL_FOOTJOB)	imageType = IMGTYPE_FOOT;
		else if (n == SKILL_ANAL)		imageType = IMGTYPE_ANAL;
		else if (n == SKILL_NORMALSEX)	imageType = IMGTYPE_SEX;
		if (n == SKILL_NORMALSEX && !girl->calc_pregnancy(&cust, false, 1.0))
		{
			g_MessageQue.AddToQue(girlName + " has gotten pregnant", 0);
		}
		g_Girls.UpdateSkill(girl, n, 2);
		g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, -25);
		g_Girls.UpdateEnjoyment(girl, ACTION_SEX, +1);
		// work out the pay between the house and the girl
		wages += g_Girls.GetStat(girl, STAT_ASKPRICE);
		int roll_max = (g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_CHARISMA));
		roll_max /= 4;
		wages += 50 + g_Dice%roll_max;
		//girl->m_Events.AddMessage(ss.str(), imageType, Day0Night1);
	}
	else if (mast)
	{
		brothel->m_Happiness += (g_Dice % 70) + 60;
		// work out the pay between the house and the girl
		int roll_max = (g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_CHARISMA));
		roll_max /= 4;
		wages += 50 + g_Dice%roll_max;
		imageType = IMGTYPE_MAST;
		//girl->m_Events.AddMessage(ss.str(), IMGTYPE_MAST, Day0Night1);
	}
	else
	{
		brothel->m_Happiness += (g_Dice % 70) + 30;
		// work out the pay between the house and the girl
		int roll_max = (g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_CHARISMA));
		roll_max /= 4;
		wages += 10 + g_Dice%roll_max;
		//girl->m_Events.AddMessage(ss.str(), IMGTYPE_STRIP, Day0Night1);
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

	g_Girls.UpdateEnjoyment(girl, actiontype, work);
	girl->m_Events.AddMessage(ss.str(), imageType, Day0Night1);

	if (wages < 0) wages = 0;
	girl->m_Pay = wages;

	// Improve stats
	int xp = 15, libido = 1, skill = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_FAME, 1);
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_PERFORMANCE, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_STRIP, g_Dice%skill + 2);
	g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, libido);

	//gained
	g_Girls.PossiblyGainNewTrait(girl, "Sexy Air", 80, actiontype, girlName + " has been stripping and having to be sexy for so long she now reeks of sexyness.", Day0Night1);

	//lose
	g_Girls.PossiblyLoseExistingTrait(girl, "Nervous", 30, actiontype, girlName + " has had so many people see her naked she is no longer nervous about anything.", Day0Night1);
	if (jobperformance > 150 && g_Girls.GetStat(girl, STAT_CONFIDENCE) > 65) { g_Girls.PossiblyLoseExistingTrait(girl, "Shy", 60, actiontype, girlName + " has been stripping for so long now that her confidence is super high and she is no longer Shy.", Day0Night1); }
	return false;
}

double cJobManager::JP_BrothelStripper(sGirl* girl, bool estimate)// not used
{
	double jobperformance =
		(g_Girls.GetStat(girl, STAT_CHARISMA) / 4 +
		g_Girls.GetStat(girl, STAT_BEAUTY) / 4 +
		g_Girls.GetSkill(girl, SKILL_PERFORMANCE) / 2 +
		g_Girls.GetSkill(girl, SKILL_STRIP));

	//good traits
	if (g_Girls.HasTrait(girl, "Charismatic"))		jobperformance += 15;
	if (g_Girls.GetStat(girl, STAT_FAME) > 85)		jobperformance += 10; //more people willing to see her
	if (g_Girls.HasTrait(girl, "Sexy Air"))			jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Cool Person"))		jobperformance += 10; //people love to be around her
	if (g_Girls.HasTrait(girl, "Charming"))			jobperformance += 10; //people like charming people
	if (g_Girls.HasTrait(girl, "Psychic"))			jobperformance += 10; //knows what people want
	if (g_Girls.HasTrait(girl, "Long Legs"))		jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Exhibitionist"))	jobperformance += 10; //SIN - likes showing off her body
	if (g_Girls.HasTrait(girl, "Cute"))				jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Great Figure"))		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Great Arse"))		jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Quick Learner"))	jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Dick-Sucking Lips"))jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Fearless"))			jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Flexible"))			jobperformance += 10;


	//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))		jobperformance -= 50; //needs others to do the job
	if (g_Girls.HasTrait(girl, "Nervous"))			jobperformance -= 30; //don't like to be around people
	if (g_Girls.HasTrait(girl, "Clumsy"))			jobperformance -= 20; //spills food and breaks things often
	if (g_Girls.HasTrait(girl, "Aggressive"))		jobperformance -= 20; //gets mad easy and may attack people
	if (g_Girls.HasTrait(girl, "Meek"))				jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Shy"))				jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Horrific Scars"))	jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Slow Learner"))		jobperformance -= 10;
	if (girl->is_pregnant())						jobperformance -= 10; //SIN - can't move so well
	if (g_Girls.HasTrait(girl, "Small Scars"))		jobperformance -= 5;

	if (g_Girls.HasTrait(girl, "Flat Ass"))			jobperformance -= 15;
	if (g_Girls.HasTrait(girl, "Flat Chest"))		jobperformance -= 15;

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
