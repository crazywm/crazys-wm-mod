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
extern cJobManager m_JobManager;

#pragma endregion

// `J` Job Brothel - Brothel
bool cJobManager::WorkBrothelMasseuse(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
#pragma region //	Job setup				//
	int actiontype = ACTION_WORKMASSEUSE;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();
	if (girl->disobey_check(actiontype, brothel))
	{
		//SIN - More informative mssg to show *what* she refuses
		ss << " refused to massage customers in your brothel " << (Day0Night1 ? "tonight." : "today.");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked massaging customers.\n \n";

	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	int wages = girl->askprice() + 40;
	int tips = 0;
	int work = 0, fame = 0;
	int imageType = IMGTYPE_PROFILE;
	int msgtype = Day0Night1;

#pragma endregion
#pragma region //	Job Performance			//

	double jobperformance = JP_BrothelMasseuse(girl, false);

	bool bannedCustomer = false; //ANON: in case she bans cust as per msg



	if (jobperformance >= 245)
	{
		ss << " She must be the perfect masseuse she never goes too hard but never too soft. She knows just what to do and the customers can't get enough of her.\n \n";
		wages += 155;

		if (roll_b <= 20)
		{
			ss << girlName << " hands are the best thing that could touch a man. Customers left big tips after getting a massage from her.\n";
			tips += 20;
		}
		else if (roll_b <= 40)
		{
			ss << "She is well known and has a steady base of faithful customers.\n";
			brothel->m_Fame += 10;
		}
		else if (roll_b <= 60)
		{
			ss << "A client wanted " << girlName << " to focus on a tight region. Doing as he asked, she left him with a boner.\n";
			brothel->m_Happiness += 5;
		}
		else if (roll_b <= 80)
		{
			ss << "Every man left " << girlName << "'s room with a wide smile.\n";
			brothel->m_Fame += 5;
		}
		else
		{
			ss << "Being asked to perfume more sensual massage, " << girlName << " massage the customer with her breasts for some extra coin.\n";
			tips += 20;
		}
	}
	else if (jobperformance >= 185)
	{
		ss << " She's unbelievable at this and is always finding new ways to please the customer.\n \n";
		wages += 95;

		if (roll_b <= 20)
		{
			ss << "Knowing most of masseur's techniques, she is one of the best.\n";
		}
		else if (roll_b <= 40)
		{
			ss << "Her customer wanted something more than the usual service. She declined and banned him from her customers list.\n";
			bannedCustomer = true;
		}
		else if (roll_b <= 60)
		{
			ss << "Trying a new technique, " << girlName << " performed a massage with her feet. The customer really liked it and suggested she should put this on the regular menu.\n";
		}
		else if (roll_b <= 80)
		{
			ss << "She certainly knows how and where to press on a man's body. Today, customer moans were heard in the corridor.\n";
			brothel->m_Happiness += 5;
		}
		else
		{
			ss << "Today she used some new oils. The customers liked them and paid some extra for the service.\n";
			wages += 15;
		}
	}
	else if (jobperformance >= 145)
	{
		ss << " She's good at this job and knows a few tricks to drive the customers wild.\n \n";
		wages += 55;

		if (roll_b <= 20)
		{
			ss << girlName << " helped a customer with a really painful back pain. The man bowed several times, thanking her for the treatment.\n";
			brothel->m_Happiness += 5;
			brothel->m_Fame += 5;
		}
		else if (roll_b <= 40)
		{
			ss << "She's making less mistakes with every day and earning good money.\n";
		}
		else if (roll_b <= 60)
		{
			ss << "When massaging a customer, she had a pleasant conversation with him, leaving a very good impression.\n";
			brothel->m_Happiness += 5;
		}
		else if (roll_b <= 80)
		{
			ss << "She certainly knows how to do her job and is getting better at it every day.\n";
		}
		else
		{
			ss << "A female customer requested a face massage. " << girlName << " managed to meet her expectations.\n";
			brothel->m_Happiness += 5;
		}
	}
	else if (jobperformance >= 100)
	{
		ss << " She made a few mistakes but overall she is okay at this.\n \n";
		wages += 15;

		if (roll_b <= 20)
		{
			ss << "Maybe she isn't the best, but at least she won't kill anyone.\n";
		}
		else if (roll_b <= 40)
		{
			ss << "She got the job done. No complaints were noted.\n";
		}
		else if (roll_b <= 60)
		{
			ss << "She was good. A few of " << girlName << "'s customers praised her.\n";
		}
		else if (roll_b <= 80)
		{
			ss << "She made few mistakes, but managed not to hurt anyone today.\n";
		}
		else
		{
			ss << "Only a few customers came today. She didn't earn much.\n";
		}
	}
	else if (jobperformance >= 70)
	{
		ss << " She was nervous and made a few mistakes. She isn't that good at this.\n \n";
		wages -= 5;

		if (roll_b <= 20)
		{
			ss << girlName << " almost killed a customer today!\n";
		}
		else if (roll_b <= 40)
		{
			ss << "Her performance was bad. The customer left a complaint and didn't pay for the service.\n";
			brothel->m_Happiness -= 5;
		}
		else if (roll_b <= 60)
		{
			ss << "Thing that she did today could be called a backrub, not a massage.\n";
		}
		else if (roll_b <= 80)
		{
			ss << "She argued with a client and kicked him out on the street wearing only a towel around his waist.\n";
			brothel->m_Happiness -= 5;
		}
		else
		{
			ss << "Not knowing what to do, she tried her best and failed.\n";
		}
	}
	else
	{
		ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.\n \n";
		wages -= 15;

		if (roll_b <= 20)
		{
			ss << girlName << " massaged a client. He could barely walk out after her service.\n";
			brothel->m_Fame -= 5;
		}
		else if (roll_b <= 40)
		{
			ss << "Getting kicked by a horse or getting a massage from " << girlName << ". Equally deadly but the first you could get for free.\n";
		}
		else if (roll_b <= 60)
		{
			ss << "A loud scream was heard in your building shortly after " << girlName << " started her shift.\n";
			brothel->m_Happiness -= 5;
		}
		else if (roll_b <= 80)
		{
			ss << "Not having any experience in this kind of work she fails miserably.\n";
		}
		else
		{
			ss << "She tries to massage a client in the best way she knows. After ten minutes he leaves without paying and frustrated.\n";
			brothel->m_Happiness -= 5;
		}
	}


	//base tips, aprox 5-30% of base wages
	tips += int(((5.0 + jobperformance / 8.0) * wages) / 100.0);

	if ((girl->libido() > 90) && !bannedCustomer)
		//ANON: sanity check: not gonna give 'perks' to the cust she just banned for wanting perks!
	{
		u_int n;
		ss << "Because she was quite horny, she ended up ";
		sCustomer Cust = g_Customers.GetCustomer(*brothel);
		brothel->m_Happiness += 100;
		if (Cust.m_IsWoman && m_JobManager.is_sex_type_allowed(SKILL_LESBIAN, brothel))
		{
			n = SKILL_LESBIAN, ss << "intensely licking the female customer's clit until she got off, making the lady very happy.\n";
		}
		else
		{
			switch (g_Dice % 10)
			{
			case 0:        n = SKILL_ORALSEX;   ss << "massaging the customer's cock with her tongue";					break;
			case 1:        n = SKILL_TITTYSEX;  ss << "using her tits to get the customer off";							break;
			case 2:        n = SKILL_HANDJOB;   ss << "giving him a cock-rub as well";									break;
			case 3:        n = SKILL_ANAL;      ss << "oiling the customer's cock and massaging it with her asshole.";	break;
			case 4:        n = SKILL_FOOTJOB;   ss << "using her feet to get the customer off";							break;
			default:	   n = SKILL_NORMALSEX; ss << "covered in massage oil and riding the customer's cock";			break;
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
		if (n == SKILL_NORMALSEX)
		{
			if (girl->check_virginity())
			{
				girl->lose_virginity();
				ss << "\nShe is no longer a virgin.\n";
			}
			if (!girl->calc_pregnancy(Cust, false, 1.0))
			{
				g_MessageQue.AddToQue(girlName + " has gotten pregnant", 0);
			}
		}
		girl->upd_skill(n, 2);
		girl->upd_temp_stat(STAT_LIBIDO, -25, true);
		wages += 225;
		tips += 30 + girl->get_skill(n) / 5;
		girl->upd_Enjoyment( ACTION_SEX, +1);
		fame += 1;
		//girl->m_Events.AddMessage(ss.str(), imageType, Day0Night1);
	}
	//SIN - bit more spice - roll_c doesn't seem to be used anywhere else so ok here
	else if (girl->has_trait( "Doctor") && roll_c > 95)
	{
		ss << "Due to " << girlName << "'s training as a Doctor, she was able to discover an undetected medical condition in her client during the massage. ";
		if (girl->charisma() < 50)
		{
			ss << "The customer was devastated to get such news from a massage and numbly accepted the referral for treatment.\n";
		}
		else
		{
			ss << "The customer was shocked to get such news, but was calmed by " << girlName << "'s kind explanations, and happily accepted the referral for treatment.\n";
			brothel->m_Happiness += 20;
		}
	}
	else
	{
		brothel->m_Happiness += (g_Dice % 70) + 30;
		brothel->m_MiscCustomers++;
		//girl->m_Events.AddMessage(ss.str(), imageType, Day0Night1);
	}


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
	}
	else
	{
		ss << "\nOtherwise, the shift passed uneventfully."; work += 1;
	}

#pragma endregion
#pragma region	//	Money					//


#pragma endregion
#pragma region	//	Finish the shift			//

	// Money
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);

	girl->upd_Enjoyment(actiontype, work);

	girl->m_Events.AddMessage(ss.str(), imageType, Day0Night1);


	// Improve stats
	int xp = 15, libido = 1, skill = 3;

	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }
	if (girl->fame() < 10 && jobperformance >= 70)		{ fame += 1; }
	if (girl->fame() < 20 && jobperformance >= 100)		{ fame += 1; }
	if (girl->fame() < 40 && jobperformance >= 145)		{ fame += 1; }
	if (girl->fame() < 60 && jobperformance >= 185)		{ fame += 1; }

	girl->fame(fame);
	girl->exp(xp);
	girl->medicine(g_Dice%skill);
	girl->service(g_Dice%skill + 1);
	girl->upd_temp_stat(STAT_LIBIDO, libido);


#pragma endregion
	return false;
}

double cJobManager::JP_BrothelMasseuse(sGirl* girl, bool estimate)// not used
{
#if 1	//SIN - standardizing job performance per J's instructs
	double jobperformance =
		//Core stats - first 100: how well she serves customers and understanding of body
		((girl->service() + girl->medicine()) / 2) +
		//secondary stats - second 100 - strength to do this all night, and attractiveness
		((girl->strength() + girl->beauty() + girl->charisma()) / 3) +
		//add level
		girl->level();

	// next up tiredness penalty...
#else
	double jobperformance =
		(girl->charisma() / 2 +
		girl->beauty() / 2 +
		girl->medicine() / 2 +
		girl->service() / 2);
#endif
	if (!estimate)
	{
		int t = girl->tiredness() - 80;
		if (t > 0)
			jobperformance -= (t + 2) * (t / 3);
	}

	//good traits
	if (girl->has_trait( "Charismatic"))   jobperformance += 15;
	if (girl->has_trait( "Sexy Air"))		 jobperformance += 10;
	if (girl->has_trait( "Cool Person"))	 jobperformance += 10; //people love to be around her
	if (girl->has_trait( "Cute"))			 jobperformance += 5;
	if (girl->has_trait( "Charming"))		 jobperformance += 10; //people like charming people
	if (girl->has_trait( "Great Figure"))  jobperformance += 5;
	if (girl->has_trait( "Great Arse"))    jobperformance += 5;
	if (girl->has_trait( "Quick Learner")) jobperformance += 5;
	if (girl->has_trait( "Psychic"))		 jobperformance += 10; //knows what people want to hear
	if (girl->has_trait( "Strong"))		 jobperformance += 15;
	if (girl->has_trait( "Blind"))		 jobperformance += 15; //SIN: heightened sense of touch
	if (girl->has_trait( "Doctor"))		 jobperformance += 10; //understands the body


	//bad traits
	if (girl->has_trait( "Dependant"))	jobperformance -= 50; // needs others to do the job
	if (girl->has_trait( "Clumsy"))		jobperformance -= 20; //spills food and breaks things often
	if (girl->has_trait( "Aggressive"))	jobperformance -= 20; //gets mad easy and may attack people
	if (girl->has_trait( "Nervous"))		jobperformance -= 30; //don't like to be around people
	if (girl->has_trait( "Meek"))			jobperformance -= 20;
	if (girl->has_trait( "Slow Learner"))	jobperformance -= 10;


	if (girl->has_trait( "One Arm"))		jobperformance -= 60;
	if (girl->has_trait( "One Foot"))		jobperformance -= 20;
	if (girl->has_trait( "One Hand"))		jobperformance -= 40;
	if (girl->has_trait( "One Leg"))		jobperformance -= 30;
	if (girl->has_trait( "No Arms"))		jobperformance -= 200;
	if (girl->has_trait( "No Feet"))		jobperformance -= 40;
	if (girl->has_trait( "No Hands"))		jobperformance -= 75;
	if (girl->has_trait( "No Legs"))		jobperformance -= 60;
	//if (girl->has_trait( "Blind"))		jobperformance -= 15; //SIN: Why? Moved this to positive
	if (girl->has_trait( "Deaf"))			jobperformance -= 10;
	if (girl->has_trait( "Retarded"))		jobperformance -= 60;
	if (girl->has_trait( "Smoker"))		jobperformance -= 10;	//would need smoke breaks

	if (girl->has_trait( "Alcoholic"))			jobperformance -= 25;
	if (girl->has_trait( "Fairy Dust Addict"))	jobperformance -= 25;
	if (girl->has_trait( "Shroud Addict"))		jobperformance -= 25;
	if (girl->has_trait( "Viras Blood Addict"))	jobperformance -= 25;


	return jobperformance;
}
