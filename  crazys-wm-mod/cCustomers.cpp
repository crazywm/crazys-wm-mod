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
#include "cCustomers.h"
#include "cBrothel.h"
#include "libintl.h"
extern cRng g_Dice;

// constructors + destructors
cCustomers::cCustomers()
{
	//	m_Parent=0;
	m_NumCustomers = 0;
	ChangeCustomerBase();
	//	m_Last=0;
}
cCustomers::~cCustomers()
{
	Free();
}
sCustomer::sCustomer()
{
	m_IsWoman = false;
	m_Amount = 1;
	m_Class = 1;
	m_Official = false;
	m_Money = 100;
	m_HasAIDS = m_HasChlamydia = m_HasSyphilis = m_HasHerpes = false;
	for (int j = 0; j < NUM_STATS; j++)		m_Stats[j] = 50;
	for (int j = 0; j < NUM_SKILLS; j++)	m_Skills[j] = 50;
	m_GoalA = m_GoalB = m_GoalC = 0;
	m_Fetish = 0;
	m_SexPref = m_SexPrefB = 0;
	m_ParticularGirl = 0;
	m_Next = 0;
	m_Prev = 0;
}
sCustomer::~sCustomer()
{
	if (m_Next) delete m_Next;	m_Next = 0;
}
void cCustomers::Free()
{
	m_NumCustomers = 0;
	if(m_Parent)
		delete m_Parent;
	m_Last = m_Parent = 0;
}

/*int cCustomers::GetHappiness()
{
	sCustomer * curr=this->m_Parent;
	int num=0;
	while(curr)
	{
		num+=curr->m_Stats[STAT_HAPPINESS];
		curr=curr->m_Next;
	}
	return num;
}*/

sCustomer* cCustomers::CreateCustomer(sBrothel& brothel)
{
	return new sCustomer{GetCustomer(brothel)};
}

// Create 1 customer
sCustomer cCustomers::GetCustomer(sBrothel& brothel)
{
	sCustomer customer;
	customer.m_Next = customer.m_Prev = 0;	// we are just making 1 customer
	// It may be a group of people looking for group sex (5% chance)
	if (g_Dice.percent(5)) // changed to bring to documented 5%, consider rasing to 10 or 15, was 4. -PP
	{
		customer.m_IsWoman = 0;
		customer.m_Amount = (g_Dice % 3) + 2; // was +1 this allowed groups of 1 -PP
	}
	else	// Then it is just one customer
	{
		customer.m_Amount = 1;
		// 15% chance they are a woman since women don't come often
		customer.m_IsWoman = (g_Dice.percent(15)) ? true : false;
	}

	// get their stats generated
	for (int j = 0; j < NUM_STATS; j++)		customer.m_Stats[j] = (g_Dice % 91) + 10;
	for (u_int j = 0; j < NUM_SKILLS; j++)	customer.m_Skills[j] = (g_Dice % 91) + 10;

	SetGoals(&customer);

	// generate their fetish
	customer.m_Fetish = g_Dice%NUM_FETISH;
	if (customer.m_Fetish == FETISH_SPECIFICGIRL)
		customer.m_ParticularGirl = g_Dice%brothel.m_NumGirls;

	// generate their sex preference
	if (customer.m_IsWoman)
	{
		int b = g_Dice.d100();
		if (b < 20)
		{
			customer.m_SexPref = SKILL_BEASTIALITY;
			if (g_Dice.percent(80))	customer.m_SexPrefB = SKILL_LESBIAN;
			else					customer.m_SexPrefB = SKILL_BDSM;
		}
		else if (b < 40)
		{
			customer.m_SexPref = SKILL_BDSM;
			if (g_Dice.percent(80))	customer.m_SexPrefB = SKILL_LESBIAN;
			else					customer.m_SexPrefB = SKILL_BEASTIALITY;
		}
		else
		{
			customer.m_SexPref = SKILL_LESBIAN;
			if (g_Dice.percent(40))	customer.m_SexPrefB = SKILL_BEASTIALITY;
			else					customer.m_SexPrefB = SKILL_BDSM;
		}
	}
	else if (customer.m_Amount > 1)
	{
		int b = g_Dice.d100();
		if (b < 10)	// bachelor party
		{
			customer.m_SexPref = SKILL_STRIP;
			customer.m_SexPrefB = SKILL_GROUP;
		}
		else
		{
			customer.m_SexPref = SKILL_GROUP;
			customer.m_SexPrefB = SKILL_STRIP;
		}
	}
	else
	{
		int b = g_Dice.d100();
		/* */if (b < 20)	customer.m_SexPref = SKILL_NORMALSEX;		// 20%
		else if (b < 38)	customer.m_SexPref = SKILL_ANAL;			// 18%
		else if (b < 52)	customer.m_SexPref = SKILL_BDSM;			// 14%
		else if (b < 65)	customer.m_SexPref = SKILL_BEASTIALITY;		// 13%
		else if (b < 77)	customer.m_SexPref = SKILL_ORALSEX;			// 12%
		else if (b < 87)	customer.m_SexPref = SKILL_TITTYSEX;		// 10%
		else if (b < 94)	customer.m_SexPref = SKILL_HANDJOB;			// 7%
		else if (b < 99)	customer.m_SexPref = SKILL_FOOTJOB;			// 5%
		else /*       */	customer.m_SexPref = SKILL_STRIP;			// 1%

		b = g_Dice.d100();
		/* */if (b < 20)	customer.m_SexPrefB = SKILL_NORMALSEX;		// 20%
		else if (b < 38)	customer.m_SexPrefB = SKILL_ANAL;			// 18%
		else if (b < 52)	customer.m_SexPrefB = SKILL_BDSM;			// 14%
		else if (b < 65)	customer.m_SexPrefB = SKILL_BEASTIALITY;	// 13%
		else if (b < 77)	customer.m_SexPrefB = SKILL_ORALSEX;		// 12%
		else if (b < 87)	customer.m_SexPrefB = SKILL_TITTYSEX;		// 10%
		else if (b < 94)	customer.m_SexPrefB = SKILL_HANDJOB;		// 7%
		else if (b < 99)	customer.m_SexPrefB = SKILL_FOOTJOB;		// 5%
		else /*       */	customer.m_SexPrefB = SKILL_STRIP;			// 1%

		if (customer.m_SexPref == customer.m_SexPrefB)
		{
			customer.m_SexPrefB = (customer.m_SexPref == SKILL_NORMALSEX ? SKILL_ANAL : SKILL_NORMALSEX);
		}
	}

	customer.m_Official = g_Dice.percent(2);	// are they an official
	int level = g_Dice.d100();	// what working class are they
	/* */if (level < m_Rich)	{ customer.m_Class = 1; customer.m_Money = (g_Dice % 2000) + 600; }
	else if (level < m_Middle)	{ customer.m_Class = 2; customer.m_Money = (g_Dice % 200) + 60; }
	else /*                 */	{ customer.m_Class = 3; customer.m_Money = (g_Dice % 100) + 20; }

	customer.m_HasAIDS = customer.m_HasChlamydia = customer.m_HasSyphilis = customer.m_HasHerpes = false;
	if (g_Dice.percent(4 + customer.m_Amount))
	{
		int a = g_Dice.d100();
		/* */if (a < 5)		customer.m_HasAIDS = true;		// 5%
		else if (a < 15)	customer.m_HasChlamydia = true;	// 10%
		else if (a < 35)	customer.m_HasSyphilis = true;	// 20%
		else if (a < 75)	customer.m_HasHerpes = true;	// 40%
		else												// 25% chance for multiple (or none)
		{
			customer.m_HasAIDS = g_Dice.percent(20);
			customer.m_HasChlamydia = g_Dice.percent(40);
			customer.m_HasSyphilis = g_Dice.percent(60);
			customer.m_HasHerpes = g_Dice.percent(80);
		}
	}
	customer.m_Money *= customer.m_Amount;

	return customer;
}

void cCustomers::GenerateCustomers(sBrothel& brothel, bool Day0Night1)
{
	Free();	// Free any existing customers
	if (brothel.m_NumGirls == 0) return;	// no girls, no customers

	stringstream ss;
	string daynighttime = (Day0Night1 ? "nighttime" : "daytime");
	/*
 *	base number of customers = number of girls times 1.5f
 *	(was set to time 5 - reverting it to agree with the comment for now
 *	--doc)
 *
 *	adding a .5 bonus to night time trade as well - should see more
 *	punters after dark it seems to me
 */
	int num = int(brothel.m_NumGirls * (Day0Night1 ? 2.0 : 1.5));
	ss << "The number of girls in this brothel attracted " << num << " initial " << daynighttime << " customers.\n \n";
/*
 *	the customers attracted by the places fame (for this shift)
 *	is the fame divided by 4 (so a max of 25 people)
 *	they may be culled by randomizing this value
 *	(halved the number -- doc)
 */
	int fame_customers = brothel.m_Fame / 4;
	ss << "This brothel's fame enticed " << fame_customers << " additional " << daynighttime << " customers to visit.\n \n";
	num += fame_customers;

	// each 100 gold of advertising adds 6 customers which is then randomized a little
	if (brothel.m_AdvertisingBudget > 0 || brothel.m_AdvertisingLevel > 1.0)
	{	// advertising value is actual gold budget multiplied by advertising level, set by girls working in advertising
		double advert = double(brothel.m_AdvertisingBudget);
		if (brothel.m_AdvertisingLevel > 1.0) advert += 50;		// a 50 gold gimme if you have girls working on advertising
		advert *= brothel.m_AdvertisingLevel;						// apply multiplier from girls working on advertising
		int custsFromAds = int(advert * 0.06);						// 6 customers per 100 gold or so
		custsFromAds = g_Dice%custsFromAds + (custsFromAds / 2);	// randomized from 50% to 150%
		ss << "You brought in " << custsFromAds << " more " << daynighttime << " customers through advertising.\n \n";
		num += custsFromAds;
	}

	// filthiness will take away customers
	int LostCustomers = max(0, int(brothel.m_Filthiness / 10));		// was /3, but that was overly harsh; changed to /10
	num -= LostCustomers;

	if (LostCustomers <= 0)	ss << "Your brothel was spotlessly clean, so you didn't lose any " << daynighttime << " customers due to filthiness.\n \n";
	else/*               */	ss << "You lost " << LostCustomers << " " << daynighttime << " customers due to the filthiness of your brothel.\n \n";

	// `J` Too much security will scare away customers
	int ScareCustomers = int(brothel.m_SecurityLevel / 500);	// this number will need to be tweaked a bit
	ScareCustomers -= 4;	// less security could attract more customers (for good or bad)
	if (ScareCustomers < 0) ScareCustomers = (g_Dice % 3) * -1;
	if (ScareCustomers > 10) ScareCustomers += g_Dice%ScareCustomers;
	num -= ScareCustomers;

	if (ScareCustomers < 0)
	{
		ss << "Your nonintrusive security attracted " << -ScareCustomers << " " << daynighttime << " customers. (for better or worse)";
	}
	else if (ScareCustomers == 0)
		ss << "Your brothel was safe and secure, so you didn't lose any " << daynighttime << " customers due to excessive security.";
	else if (ScareCustomers < 10)
		ss << "You lost " << ScareCustomers << " " << daynighttime << " customers due to the excessive security in your brothel.";
	else
		ss << "You lost " << ScareCustomers << " " << daynighttime << " customers due to the oppressive security in your brothel.";
	brothel.m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_BROTHEL);


	if (num < 0)	num = 0;  // negative number of customers doesn't make sense

	for (int i = 0; i < num; i++)
	{
		Add(CreateCustomer(brothel));

	}

}

/*sCustomer* cCustomers::GetParentCustomer()
{
	return m_Parent;
}*/

void cCustomers::ChangeCustomerBase()
{
	m_Rich = g_Dice % 31 + 10;				// 10-40% Rich
	m_Middle = g_Dice % 41 + 10 + m_Rich;	// 10-50% Middle
	// leaving 10-80% poor
}

void cCustomers::Add(sCustomer* cust)
{
	cust->m_Prev = cust->m_Next = 0;
	if (m_Parent)
	{
		cust->m_Prev = m_Last;
		m_Last->m_Next = cust;
		m_Last = cust;
	}
	else	m_Last = m_Parent = cust;
	m_NumCustomers++;
}

void cCustomers::Remove(sCustomer* cust)
{
	if (cust->m_Prev)
	{
		cust->m_Prev->m_Next = cust->m_Next;
		if (cust->m_Next) cust->m_Next->m_Prev = cust->m_Prev;
		cust->m_Next = cust->m_Prev = 0;
		delete cust;
		cust = 0;
	}
	else
	{
		m_Parent = cust->m_Next;
		cust->m_Next = 0;
		if (m_Parent) m_Parent->m_Prev = 0;
		delete cust;
		cust = 0;
	}
}

void cCustomers::SetGoals(sCustomer* customer)
{
	if (!customer) return;
	int a = g_Dice % NUM_GOALS, b = g_Dice % NUM_GOALS, c = g_Dice % NUM_GOALS;

	if ((g_Dice.percent(50) && a == GOAL_FIGHT) || (g_Dice.percent(80) && a == GOAL_RAPE))
	{
		customer->m_GoalA  = GOAL_SEX;
		customer->m_GoalB  = GOAL_FIGHT;
		customer->m_GoalC  = GOAL_RAPE;
		return;
	}


	// GOAL_UNDECIDED
	// GOAL_FIGHT
	// GOAL_RAPE
	// GOAL_SEX
	// GOAL_GETDRUNK
	// GOAL_GAMBLE
	// GOAL_ENTERTAINMENT
	// GOAL_XXXENTERTAINMENT
	// GOAL_MASSAGE
	// GOAL_STRIPSHOW
	// GOAL_FREAKSHOW
	// GOAL_CULTURALEXPLORER
	// GOAL_OTHER

	customer->m_GoalA = a;
	customer->m_GoalB = b;
	customer->m_GoalC = c;
}
