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

// `J` Brothel Job - Brothel
bool cJobManager::WorkBrothelMasseuse(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	stringstream ss; string girlName = girl->m_Realname;
	if(Preprocessing(ACTION_WORKMASSUSSE, girl, brothel, Day0Night1, summary, ss.str()))
		return true;

	// put that shit away, you'll scare off the customers!
	g_Girls.UnequipCombat(girl);

	int roll = g_Dice%100;
	int jobperformance = (	g_Girls.GetStat(girl, STAT_CHARISMA) / 2 + 
							g_Girls.GetStat(girl, STAT_BEAUTY) / 2  +
							g_Girls.GetSkill(girl, SKILL_MEDICINE) / 2 +
							g_Girls.GetSkill(girl, SKILL_SERVICE) / 2);
	int wages = g_Girls.GetStat(girl, STAT_ASKPRICE)+40, work = 0;
	int imageType = IMGTYPE_PROFILE;
	bool bannedCustomer = false; //ANON: in case she bans cust as per msg

	ss << "She massaged a customer.";


	//good traits
	if (g_Girls.HasTrait(girl, "Charismatic"))   jobperformance += 15;
	if (g_Girls.HasTrait(girl, "Sexy Air"))		 jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Cool Person"))	 jobperformance += 10; //people love to be around her
	if (g_Girls.HasTrait(girl, "Cute"))			 jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Charming"))		 jobperformance += 10; //people like charming people	
	if (g_Girls.HasTrait(girl, "Great Figure"))  jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Great Arse"))    jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Quick Learner")) jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Psychic"))		 jobperformance += 10; //knows what people want to hear	

	//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))	jobperformance -= 50; //needs others to do the job
	if (g_Girls.HasTrait(girl, "Clumsy"))		jobperformance -= 20; //spills food and breaks things often
	if (g_Girls.HasTrait(girl, "Aggressive"))	jobperformance -= 20; //gets mad easy and may attack people
	if (g_Girls.HasTrait(girl, "Nervous"))		jobperformance -= 30; //don't like to be around people
	if (g_Girls.HasTrait(girl, "Meek"))			jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Slow Learner"))	jobperformance -= 10;


	if (jobperformance >= 245)
		{
			ss << " She must be the perfect massusse she never goes too hard but never too soft. She knows just what to do and the customers can't get enough of her.\n\n";
			wages += 155;

			if (roll <= 20)
			{
				ss << girlName + " hands are the best thing that could touch a man. Customers left big tips after getting a massage from her.\n";
				wages += 20;
			}
		else if (roll <= 40)
			{
				ss << "She is well known and has a steady base of faithful customers.\n";
				brothel->m_Fame += 10;
			}
		else if (roll <= 60)
			{
				ss << "A client wanted " + girlName + " to focus on a tight region. Doing as he asked, she left him with a boner.\n";
				brothel->m_Happiness += 5;
			}
		else if (roll <= 80)
			{
				ss <<  "Every man left " + girlName + "'s room with a wide smile.\n";
				brothel->m_Fame += 5;
			}
		else
			{
				ss << "Being asked to perfume more sensual massage, " + girlName + " massage the customer with her breasts for some extra coin.\n";
				wages += 20;
			}
		}
	else if (jobperformance >= 185)
		{
			ss << " She's unbelievable at this and is always finding new ways to please the customer.\n\n";
			wages += 95;

		if (roll <= 20)
			{
				ss << "Knowing most of masseur's techniques, she is one of the best.\n";
			}
		else if (roll <= 40)
			{
				ss << "Her customer wanted something more than the usual service. She declined and banned him from her customers list.\n";
				bannedCustomer = true;
			}
		else if (roll <= 60)
			{
				ss << "Trying a new technique, " + girlName + " performed a massage with her feet. The customer really liked it and suggested she should put this on the regular menu.\n";
			}
		else if (roll <= 80)
			{
				ss <<  "She certainly knows how and where to press on a man's body. Today, customer moans were heard in the corridor.\n";
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
			ss << " She's good at this job and knows a few tricks to drive the customers wild.\n\n";
			wages += 55;

		if (roll <= 20)
			{
				ss << girlName + " helped a customer with a really painful back pain. The man bowed several times, thanking her for the treatment.\n";
				brothel->m_Happiness += 5;
				brothel->m_Fame += 5;
			}
		else if (roll <= 40)
			{
				ss << "She's making less mistakes with every day and earning good money.\n";
			}
		else if (roll <= 60)
			{
				ss << "When massaging a customer, she had a pleasant conversation with him, leaving a very good impression.\n";
				brothel->m_Happiness += 5;
			}
		else if (roll <= 80)
			{
				ss <<  "She certainly knows how to do her job and is getting better at it every day.\n";
			}
		else
			{
				ss << "A female customer requested a face massage. " + girlName + " managed to meet her expectations.\n";
				brothel->m_Happiness += 5;
			}
		}
	else if (jobperformance >= 100)
		{
			ss << " She made a few mistakes but overall she is okay at this.\n\n";
			wages += 15;

		if (roll <= 20)
			{
				ss << "Maybe she isn't the best, but at least she won't kill anyone.\n";
			}
		else if (roll <= 40)
			{
				ss << "She got the job done. No complaints were noted.\n";
			}
		else if (roll <= 60)
			{
				ss << "She was good. A few of " + girlName + "'s customers praised her.\n";
			}
		else if (roll <= 80)
			{
				ss <<  "She made few mistakes, but manged not to hurt anyone today.\n";
			}
		else
			{
				ss << "Only a few customers came today. She didn't earn much.\n";
			}
		}
	else if (jobperformance >= 70)
		{
			ss << " She was nervous and made a few mistakes. She isn't that good at this.\n\n";
			wages -= 5;

		if (roll <= 20)
			{
				ss << girlName + " almost killed a customer today!\n";
			}
		else if (roll <= 40)
			{
				ss << "Her performance was bad. The customer left a complaint and didn't pay for the service.\n";
				brothel->m_Happiness -= 5;
			}
		else if (roll <= 60)
			{
				ss << "Thing that she did today could be called a backrub, not a massage.\n";
			}
		else if (roll <= 80)
			{
				ss <<  "She argued with a client and kicked him out on the street wearing only a towel around his waist.\n";
				brothel->m_Happiness -= 5;
			}
		else
			{
				ss << "Not knowing what to do, she tried her best and failed.\n";
			}
		}
	else
		{
			ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.\n\n";
			wages -= 15;

		if (roll <= 20)
			{
				ss << girlName + " massaged a client. He could barely walk out after her service.\n";
				brothel->m_Fame -= 5;
			}
		else if (roll <= 40)
			{
				ss << "Getting kicked by a horse or getting a massage from " + girlName + ". Equally deadly but the first you could get for free.\n";
			}
		else if (roll <= 60)
			{
				ss << "A loud scream was heard in your building shortly after " + girlName + " started her shift.\n";
				brothel->m_Happiness -= 5;
			}
		else if (roll <= 80)
			{
				ss <<  "Not having any experience in this kind of work she fails miserably.\n";
			}
		else
			{
				ss << "She tries to massage a client in the best way she knows. After ten minutes he leaves without paying and frustrated.\n";
				brothel->m_Happiness -= 5;
			}
		}

	if ((g_Girls.GetStat(girl, STAT_LIBIDO) > 90) && !bannedCustomer)
		//ANON: sanity check: not gonna give 'perks' to the cust she just banned for wanting perks!
	{
		u_int n;
		ss << "Because she was quite horny, she ended up ";
		sCustomer cust;
		brothel->m_Happiness += 100;
		GetMiscCustomer(brothel, cust);
		if (cust.m_IsWoman && m_JobManager.is_sex_type_allowed(SKILL_LESBIAN, brothel)) 
		{ n = SKILL_LESBIAN, ss << "intensely licking the female customer's clit until she got off, making the lady very happy.\n"; }
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
		if (n == SKILL_NORMALSEX && !girl->calc_pregnancy(&cust,  false, 1.0))
		{
			g_MessageQue.AddToQue(girl->m_Realname + " has gotten pregnant", 0);
		}
		g_Girls.UpdateSkill(girl, n, 2);
		g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -25);
		// work out the pay between the house and the girl
		wages += 225;
		girl->m_Pay = wages;
		g_Girls.UpdateEnjoyment(girl, ACTION_SEX, +1, true);
		//girl->m_Events.AddMessage(ss.str(), imageType, Day0Night1);
	}
	else
	{
		brothel->m_Happiness += (g_Dice%70)+30;
		brothel->m_MiscCustomers++;
		// work out the pay between the house and the girl
		girl->m_Pay = wages;
		//girl->m_Events.AddMessage(ss.str(), imageType, Day0Night1);
	}

	//enjoyed the work or not
	if (roll <= 5)
	{ ss << "\nSome of the patrons abused her during the shift."; work -= 1; }
	else if (roll <= 25) 
	{ ss << "\nShe had a pleasant time working."; work += 3; }
	else
	{ ss << "\nOtherwise, the shift passed uneventfully."; work += 1; }

	g_Girls.UpdateEnjoyment(girl, ACTION_WORKMASSUSSE, work, true);

	girl->m_Events.AddMessage(ss.str(), imageType, Day0Night1);


		// Improve stats
	int xp = 15, libido = 1, skill = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_FAME, 1);
	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_MEDICINE, g_Dice%skill);
	g_Girls.UpdateSkill(girl, SKILL_SERVICE, g_Dice%skill+1);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);

	return false;
}
