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
#include <algorithm>
#include "libintl.h"

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;

// `J` Brothel Job - Brothel
bool cJobManager::WorkWhore(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	// put that shit away, you'll scare off the customers!
	g_Girls.UnequipCombat(girl);


	/*
	*	WD:	Modified to fix customer service problems.. I hope :)
	*
	*	Change logic as original code is based on linked list of customers
	*	not random generation for each call to GetCustomer()
	*
	*	Pricing issues seem to be resolved with getting lots of money
	*	from customer that cant pay
	*
	*	The numbers I have added need to be tested
	*
	*	Limit number customers a girl can fuck to 10 max
	*
	*	Limit the number of customers a girl can see if they will
	*	fuck her from 5 to Max Customers * 2
	*
	*	Redid the code for deadbeat customers
	*
	*	% Chance of customers without any money getting service is
	*  percent(50 - INTELLIGENCE) / 5) where  20 < INTELLIGENCE < 100
	*	If caught will set deadbeat flag
	*
	*	GetCustomer() is generating a lot of poor customers changed
	*	code to add pay to customers funds instead of generating
	*	New customer.
	*
	*	% Chance of customer refusing to pay despite having funds is
	*	percent((40 - HAPPINESS) / 2) && percent(CONFIDENCE - 25)
	*	If caught by guards they will pay
	*
	*	Only decrement filthiness when service is performed in brothel
	*
	*	Street Work will not decrement number customers
	*  Street work will only service 66% number of customers
	*	Street Work will only generate 66% of brothel income
	*	Street Work Really needs its own NumCustomers() counter
	*
	*	Rival gangs can damage girls doing Street Work
	*  % Chance of destroying rival gang is depended on best of
	*	SKILL_COMBAT & SKILL_MAGIC / 5
	*
	*	Changed message for rival gangs attacking street girls to give
	*	EVENT_WARNING
	*
	*	GROUP sex code caculations now consolidated to one place
	*
	*  Fixed end of shift messages
	*
	*	Fame only to be done in GirlFucks()
	*
	*	Now Base Customer HAPPINESS = 60, code conslidated from 2 places in file
	*
	*/

	string fuckMessage = "";
	string message = "";
	int iNum = 0;
	int iOriginal = 0;
	int	AskPrice = g_Girls.GetStat(girl, STAT_ASKPRICE);
	int pay;
	int tip;
	int LoopCount;
	bool group = false;	// Group sex flag
	bool bCustCanPay;				// Customer has enough money to pay 
	bool acceptsGirl;				// Customer will sleep girl
	bool bStreetWork;				// Girl Doing StreetWork

	u_int SexType = 0;
	u_int job = (DayNight == 0) ? girl->m_DayJob : girl->m_NightJob;
	bStreetWork = (job == JOB_WHORESTREETS);
	stringstream ss;

	girl->m_Pay = 0;

	// work out how many customers the girl can service

	// Changed the number of custmers stats add.. reasone was that old value, 
	// it was only adding 1 customer per stat, unless stat was 100 for beauty and Charisma. Fame would add a max of 3. and only if was = 10
	// there would be NO point in doing this, if it defaults to NumCusts++ since it is basically the same effect.	-PP

	// Max number on customers the girl can fuck
	int b = g_Girls.GetStat(girl, STAT_BEAUTY), c = g_Girls.GetStat(girl, STAT_CHARISMA), f = g_Girls.GetStat(girl, STAT_FAME);
	int NumCusts = min(10, 3 + ((b + 1) / 50) + ((c + 1) / 50) + ((f + 1) / 25));

	int NumSleptWith = 0;		// Total num customers she fucks this session

	if (bStreetWork)
	{
		NumCusts = NumCusts * 2 / 3;
		AskPrice = AskPrice * 2 / 3;
	}

	NumCusts = min(NumCusts, 10);		// No more than 10 Customers per shift

	// Complications

	/*
	*	WD:	Rival Gang is incompleate
	*
	*	Chance of defeating gane is based on  combat / magic skill
	*	Added Damage and Tiredness
	*	ToDo Girl fightrivalgang() if its implemented
	*
	*/

	if (bStreetWork && g_Dice.percent(5))
	{
		cRival* rival = g_Brothels.GetRivalManager()->GetRandomRival();
		if (rival)
		{
			if (rival->m_NumGangs > 0)
			{
				ss.str("");
				summary += girl->m_Realname + " was attacked by enemy goons. \n";
				//message		+= "She ran into some enemy goons and was attacked.\n";
				ss << girl->m_Realname << " ran into some enemy goons and was attacked.\n";

				// WD: Health loss, Damage 0-15, 25% chance of 0 damage
				iNum = max(g_Dice % 20 - 5, 0);
				iOriginal = g_Girls.GetStat(girl, STAT_HEALTH);
				g_Girls.UpdateStat(girl, STAT_HEALTH, -iNum);
				iNum = iOriginal - g_Girls.GetStat(girl, STAT_HEALTH);

				if (iNum > 0)
				{
					//message += "She fought back and was hurt.\n";	
					ss << "She fought back and was hurt taking " << iNum << " points of damage.\n";
				}
				else
					ss << "She fought back taking no damage.\n";

				// WD:	Tiredness (5 + 2 * damage) points avg is (6 + Health Damage) is bell curve
				iNum = g_Dice % (iNum)+g_Dice % (iNum)+5;
				g_Girls.UpdateStat(girl, STAT_TIREDNESS, iNum);
				message = ss.str();

				// WD:	If girl used magic to defend herself she will use mana
				if (g_Girls.GetStat(girl, STAT_MANA)  > 20 && g_Girls.GetSkill(girl, SKILL_MAGIC) > g_Girls.GetSkill(girl, SKILL_COMBAT))
				{
					g_Girls.UpdateStat(girl, STAT_MANA, -20);
					iNum = g_Girls.GetSkill(girl, SKILL_MAGIC) / 5;		// WD: Chance to destroy rival gang
				}
				else
					iNum = g_Girls.GetSkill(girl, SKILL_COMBAT) / 5;	// WD: Chance to destroy rival gang

				// WD:	Destroy rival gang
				if (g_Dice.percent(iNum))
					rival->m_NumGangs--;


				girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_WARNING);
				// WD TRACE Enemy Goons {girl->m_Name} dmg= {iNum} msg= {message}
			}
		}
	}



	// WD: Set the limits on the Number of customers a girl can try and fuck
	LoopCount = max(NumCusts * 2, 5);

	// WD: limit to number of customers left
	if (!bStreetWork && LoopCount >g_Customers.GetNumCustomers())
		LoopCount = g_Customers.GetNumCustomers();



	for (int i = 0; i < LoopCount; i++)	// Go through all customers
	{
		// WD:	Move exit test to top of loop
		// if she has already slept with the max she can attact then stop processing her fucking routine
		if (NumSleptWith >= NumCusts) break;

		// WD:	Init Loop variables
		pay = AskPrice;
		SexType = 0;
		group = false;
		acceptsGirl = false;
		// WD:	Create Customer
		sCustomer Cust;
		g_Customers.GetCustomer(Cust, brothel);

		// filter out unwanted sex types (unless it is street work)
		if (!bStreetWork && !is_sex_type_allowed(Cust.m_SexPref, brothel) && !is_sex_type_allowed(Cust.m_SexPrefB, brothel))
		{
			brothel->m_RejectCustomers++;
			continue;	// `J` if both their sexprefs are banned then they leave
		}
		else if (!bStreetWork && !is_sex_type_allowed(Cust.m_SexPref, brothel)) // it their first sexpref is banned then switch to the second
		{
			Cust.m_SexPref = Cust.m_SexPrefB;
			Cust.m_Stats[STAT_HAPPINESS] = 32 + g_Dice % 9 + g_Dice % 9;	// `J` and they are less happy
		}
		else	// `J` otherwise they are happy with their first choice.
		{
			// WD:	Set the customers begining happiness/satisfaction
			Cust.m_Stats[STAT_HAPPINESS] = 42 + g_Dice % 10 + g_Dice % 10; // WD: average 51 range 42 to 60
		}

		// WD:	Consolidate GROUP Sex Calcs here
		//		adjust price by number of parcitipants
		if (Cust.m_Amount > 1)
		{
			group = true;
			pay *= (int)Cust.m_Amount;
			if (Cust.m_SexPref == SKILL_GROUP) pay = pay * 17 / 10;
			if (Cust.m_SexPref == SKILL_STRIP) pay = pay * 14 / 10;
			// WD: this is complicated total for 1.7 * pay * num of customers
			// pay += (int)((float)(pay*(Cust.m_Amount))*0.7f); 
		}

		// WD: Has the customer have enough money
		bCustCanPay = Cust.m_Money >= (unsigned)pay;

		// WD:	TRACE Customer Money = {Cust.m_Money}, Pay = {pay}, Can Pay = {bCustCanPay}

		// WD:	If the customer doesn't have enough money, he will only sleep with her if he is stupid
		if (!bCustCanPay && !g_Dice.percent((50 - Cust.m_Stats[STAT_INTELLIGENCE]) / 5))
		{
			//continue;
			// WD: Hack to avoid many newcustomer() calls
			Cust.m_Money += (unsigned)pay;
			bCustCanPay = true;
		}

		// test for specific girls
		if (Cust.m_Fetish == FETISH_SPECIFICGIRL)
		{
			if (Cust.m_ParticularGirl == g_Brothels.GetGirlPos(brothel->m_id, girl))
			{
				fuckMessage = "This is the customer's favorite girl.\n\n";
				acceptsGirl = true;
			}
		}
		else
		{
			// 50% chance of getting something a little weirder during the night
			if (DayNight == 1 && Cust.m_Fetish < NUM_FETISH - 2 && g_Dice.percent(50)) Cust.m_Fetish += 2;

			// Check for fetish match
			if (g_Girls.CheckGirlType(girl, Cust.m_Fetish))
			{
				fuckMessage = "The customer loves this type of girl.\n\n";
				acceptsGirl = true;
			}
		}

		// Other ways the customer will accept the girl
		if (acceptsGirl == false)
		{
			if (Cust.m_Stats[STAT_LIBIDO] >= 80)
			{
				fuckMessage = "Customer chooses her because they are very horny.\n\n";
				acceptsGirl = true;
			}
			else if (((g_Girls.GetStat(girl, STAT_BEAUTY) + g_Girls.GetStat(girl, STAT_CHARISMA)) / 2) >= 90)	// if she is drop dead gorgeous
			{
				fuckMessage = "Customer chooses her because they are stunned by her beauty.\n\n";
				acceptsGirl = true;
			}
			else if (g_Girls.GetStat(girl, STAT_FAME) >= 80)	// if she is really famous
			{
				fuckMessage = "Customer chooses her because she is so famous.\n\n";
				acceptsGirl = true;
			}
			// WD:	Use Magic only as last resort
			else if (g_Girls.GetSkill(girl, SKILL_MAGIC) > 50 && g_Girls.GetStat(girl, STAT_MANA))	// she can use magic to get him
			{
				fuckMessage = girl->m_Realname + " uses magic to get the customer to choose her.\n\n";
				g_Girls.UpdateStat(girl, STAT_MANA, -20);
				acceptsGirl = true;
			}
		}

		if (!acceptsGirl) continue;		// will the customer sleep with her?

		// Horizontal boogy
		g_Girls.GirlFucks(girl, DayNight, &Cust, group, fuckMessage, SexType);
		NumSleptWith++;
		if (!bStreetWork) brothel->m_Filthiness++;

		// update how happy the customers are on average
		brothel->m_Happiness += Cust.m_Stats[STAT_HAPPINESS];


		// Time for the customer to fork over some cash

		// WD:	Customer can not pay
		if (!bCustCanPay)
		{
			pay = 0;	// WD: maybe no money from this customer
			if (g_Dice.percent(Cust.m_Stats[STAT_CONFIDENCE] - 25))	// Runner
			{
				if (g_Gangs.GetGangOnMission(MISS_GUARDING))
				{
					if (g_Dice.percent(50))
						fuckMessage += " The customer couldn't pay and managed to elude your guards.";

					else
					{
						fuckMessage += " The customer couldn't pay and tried to run off. Your men caught him before he got out the door.";
						SetGameFlag(FLAG_CUSTNOPAY);
						pay = (int)Cust.m_Money;	// WD: Take what customer has
						Cust.m_Money = 0;	// WD: ??? not needed Cust record is not saved when this fn ends!  Leave for now just in case ???
					}
				}
				else
					fuckMessage += " The customer couldn't pay and ran off. There were no guards!";

			}
			else
			{
				// offers to pay the girl what he has
				if (g_Dice.percent(g_Girls.GetStat(girl, STAT_INTELLIGENCE)))
				{
					// she turns him over to the goons
					fuckMessage += " The customer couldn't pay the full amount, so your girl turned them over to your men.";
					SetGameFlag(FLAG_CUSTNOPAY);
				}
				else
					fuckMessage += " The customer couldn't pay the full amount.";

				pay = (int)Cust.m_Money;
				Cust.m_Money = 0;	// WD: ??? not needed Cust record is not saved when this fn ends!  Leave for now just in case ???
			}
		}


		// WD:	Unhappy Customer tries not to pay and does a runner
		else if (g_Dice.percent((40 - Cust.m_Stats[STAT_HAPPINESS]) / 2) && g_Dice.percent(Cust.m_Stats[STAT_CONFIDENCE] - 25))
		{
			if (g_Gangs.GetGangOnMission(MISS_GUARDING))
			{
				if (g_Dice.percent(50))
				{
					fuckMessage += " The customer refused to pay and managed to elude your guards.";
					pay = 0;
				}
				else
				{
					fuckMessage += " The customer refused to pay and tried to run off. Your men caught him before he got out the door and forced him to pay.";
					Cust.m_Money -= (unsigned)pay; // WD: ??? not needed Cust record is not saved when this fn ends!  Leave for now just in case ???
				}
			}
			else
			{
				fuckMessage += " The customer refused to pay and ran off. There were no guards!";
				pay = 0;
			}
		}

		else  // Customer has enough money
		{
			Cust.m_Money -= (unsigned)pay; // WD: ??? not needed Cust record is not saved when this fn ends!  Leave for now just in case ???

			// if he is happy and has some extra gold he will give a tip
			if ((int)Cust.m_Money >= 20 && Cust.m_Stats[STAT_HAPPINESS] > 90)
			{
				tip = (int)Cust.m_Money;
				if (tip > 20)
				{
					Cust.m_Money -= 20;	// WD: ??? not needed Cust record is not saved when this fn ends!  Leave for now just in case ???
					tip = 20;
				}
				else
					Cust.m_Money = 0;	// WD: ??? not needed Cust record is not saved when this fn ends!  Leave for now just in case ???

				fuckMessage += ("\nShe received a tip of " + intstring(tip) + " gold");

				girl->m_Tips += tip;

				fuckMessage += ".";

				// If the customer is a government official
				if (Cust.m_Official == 1)
				{
					g_Brothels.GetPlayer()->suspicion(-5);
					fuckMessage += " It turns out that the customer was a government official, which lowers your suspicion.";
				}
			}
		}

		// Match image type to the deed done
		int imageType = IMGTYPE_SEX;
		/* */if (SexType == SKILL_ANAL)			imageType = IMGTYPE_ANAL;
		else if (SexType == SKILL_BDSM)			imageType = IMGTYPE_BDSM;
		else if (SexType == SKILL_NORMALSEX)	imageType = IMGTYPE_SEX;
		else if (SexType == SKILL_BEASTIALITY)	imageType = IMGTYPE_BEAST;
		else if (SexType == SKILL_GROUP)		imageType = IMGTYPE_GROUP;
		else if (SexType == SKILL_LESBIAN)		imageType = IMGTYPE_LESBIAN;
		else if (SexType == SKILL_ORALSEX)		imageType = IMGTYPE_ORAL;
		else if (SexType == SKILL_TITTYSEX)		imageType = IMGTYPE_TITTY;
		else if (SexType == SKILL_HANDJOB)		imageType = IMGTYPE_HAND;
		else if (SexType == SKILL_STRIP)		imageType = IMGTYPE_STRIP;

		// chance of customer beating or attempting to beat girl
		if (work_related_violence(girl, DayNight, bStreetWork))
			pay = 0;		// WD TRACE WorkRelatedViloence {girl->m_Name} earns nothing

		// WD:	Save gold earned
		girl->m_Pay += pay;		// WD TRACE Save Pay {girl->m_Name} earns {pay} totaling {girl->m_Pay}
		girl->m_Events.AddMessage(fuckMessage, imageType, DayNight);
	}


	// WD:	Reduce number of availabe customers for next whore
	if (!bStreetWork)								// WD:	only brothel workers
	{
		iNum = g_Customers.GetNumCustomers();		// WD: Should not happen but lets make sure
		if (iNum < NumSleptWith)	g_Customers.AdjustNumCustomers(-iNum);
		else						g_Customers.AdjustNumCustomers(-NumSleptWith);
	}
	else brothel->m_MiscCustomers += NumSleptWith;	// WD:	 Count number of customers from Street Work

	// WD:	End of shift messages
	// doc: adding braces - gcc warns of ambiguous if nesting
	if (!bStreetWork)  
	{
		if (g_Customers.GetNumCustomers() == 0)	{ girl->m_Events.AddMessage("No more customers.", IMGTYPE_PROFILE, DayNight); }
		else if (NumSleptWith < NumCusts)		{ girl->m_Events.AddMessage(girl->m_Realname + " ran out of customers who like her.", IMGTYPE_PROFILE, DayNight); }
	}

	// WD:	Summary messages
	ss.str("");
	ss << girl->m_Realname << (bStreetWork ? " worked the streets and" : "") << " saw " << NumSleptWith << " customers this shift.";
	summary += ss.str();

	girl->m_Events.AddMessage(summary, IMGTYPE_PROFILE, DayNight);

	//gain
	g_Girls.PossiblyGainNewTrait(girl, "Good Kisser", 50, ACTION_SEX, girl->m_Realname + " has had a lot of practice kissing and as such as become a Good Kisser.", DayNight != 0);
	g_Girls.PossiblyGainNewTrait(girl, "Nymphomaniac", 70, ACTION_SEX, girl->m_Realname + " has been having so much sex she is now wanting sex all the time.", DayNight != 0);

	return false;
}
