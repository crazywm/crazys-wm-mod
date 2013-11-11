/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders 
 * who meet on http://pinkpetal.co.cc
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

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;

bool cJobManager::WorkHallWhore(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
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

	string fuckMessage	= "";
	string message		= "";
	sCustomer Cust;
	int NumCusts		= 0;		// Max number on customers the girl can fuck
	int NumSleptWith	= 0;		// Total num customers she fucks this session
	int iNum			= 0;
	int iOriginal		= 0;
	int	AskPrice		= g_Girls.GetStat(girl, STAT_ASKPRICE);
	int pay;
	int tip;
	int LoopCount;
	bool group			= false;	// Group sex flag
	bool bCustCanPay;				// Customer has enough money to pay 
	bool acceptsGirl;				// Customer will sleep girl
	
	u_int SexType = 0;
	u_int job = 0;		

	if(DayNight == 0)
		job				= girl->m_DayJob;	
	else
		job				= girl->m_NightJob;
	stringstream ss;

	girl->m_Pay = 0;

	// work out how many customers the girl can service

	// Changed the number of custmers stats add.. reasone was that old value, 
	// it was only adding 1 customer per stat, unless stat was 100 for beauty and Charisma. Fame would add a max of 3. and only if was = 10
	// there would be NO point in doing this, if it defaults to NumCusts++ since it is basically the same effect.	-PP

	if(g_Girls.GetStat(girl, STAT_BEAUTY) > 0)
		NumCusts = g_Girls.GetStat(girl, STAT_BEAUTY) / 50 + 1;	// was 50
	else
		NumCusts++;

	if(g_Girls.GetStat(girl, STAT_CHARISMA) > 0)
		NumCusts += g_Girls.GetStat(girl, STAT_CHARISMA) / 50 + 1;	// was 50
	else
		NumCusts++;

	if(g_Girls.GetStat(girl, STAT_FAME) > 0)
		NumCusts += g_Girls.GetStat(girl, STAT_FAME) / 25;	// was 25
	else
		NumCusts++;

	/*
	 *	WD:	Reduce price and number of customers that 
	 *		can be serviced for street work.
	 *
	 *		Done as there is no limit on number of customers
	 *		for street work
	 */

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

	// WD: Set the limits on the Number of customers a girl can try and fuck
	LoopCount = max(NumCusts * 2, 5);

	// WD: limit to number of customers left
	if(LoopCount >g_Customers.GetNumCustomers())		
		LoopCount = g_Customers.GetNumCustomers();



	for(int i=0; i < LoopCount ; i++)	// Go through all customers
	{
		// WD:	Move exit test to top of loop
		// if she has already slept with the max she can attact then stop processing her fucking routine
		if(NumSleptWith >= NumCusts)		
			break;

		// WD:	Init Loop variables
		pay = AskPrice;
		SexType = 0;
		acceptsGirl = false;
		// WD:	Create Customer
		g_Customers.GetCustomer(Cust, brothel);



#if 0	// WD:	Why is this here we have code lower down for customers that can't / wont pay
		// if the customer doesn't have enough money, he will only sleep with her if he is stupid
		if(Cust.m_Money < (unsigned)pay)
		{
			brothel->m_Happiness+=100;					
			g_Customers.AdjustNumCustomers(-1);
			continue;
		}
		else
			pay *= Cust.m_Amount;

		// WD:	Rejecting this shold not change happiness or number of customers
		// filter out unwanted sex types (unless it is street work)
		if(!bStreetWork)
		{
			if(!is_sex_type_allowed(Cust.m_SexPref, brothel))
			{
				brothel->m_Happiness+=100;
				g_Customers.AdjustNumCustomers(-1);
				continue;
			}
#endif

		// filter out unwanted sex types (unless it is street work)
		if(!is_sex_type_allowed(Cust.m_SexPref, brothel))
			continue; 

		// WD:	Consolidate GROUP Sex Calcs here
		//		adjust price by number of parcitipants
		if(Cust.m_Amount > 1 && is_sex_type_allowed(SKILL_GROUP, brothel))
		{
			group = true;
			pay *= (int) Cust.m_Amount;	
			if (Cust.m_SexPref == SKILL_GROUP)
				pay = pay * 17 / 10;
				// WD: this is complicated total for 1.7 * pay * num of customers
				// pay += (int)((float)(pay*(Cust.m_Amount))*0.7f); 
		}

		// WD: Has the customer have enough money
		bCustCanPay = Cust.m_Money >= (unsigned)pay;	

		// WD:	TRACE Customer Money = {Cust.m_Money}, Pay = {pay}, Can Pay = {bCustCanPay}

		// WD:	If the customer doesn't have enough money, he will only sleep with her if he is stupid
		if(!bCustCanPay && !g_Dice.percent((50 - Cust.m_Stats[STAT_INTELLIGENCE]) / 5))
		{
			//continue;
			// WD: Hack to avoid many newcustomer() calls
			Cust.m_Money += (unsigned)pay;
			bCustCanPay = true;
		}

		// test for specific girls
		if(Cust.m_Fetish == FETISH_SPECIFICGIRL)
		{
			if(Cust.m_ParticularGirl == g_Brothels.GetGirlPos(brothel->m_id, girl))
			{
				fuckMessage = "This is the customer's favorite girl.\n\n";
				acceptsGirl = true;
			}
		}
		else
		{
			if(DayNight == 1)	// 50% chance of getting something a little weirder during the night
			{
				if(Cust.m_Fetish < NUM_FETISH-2)
				{
					if(g_Dice.percent(50))
						Cust.m_Fetish += 2;
				}
			}

			// Check for fetish match
			if(g_Girls.CheckGirlType(girl, Cust.m_Fetish))	
			{
				fuckMessage = "The customer loves this type of girl.\n\n";
			//	fuckMessage += ("Fetish type: " + intstring(Cust.m_Fetish) + "\n");
				acceptsGirl = true;
			}
		}

		// Other ways the customer will accept the girl
		if(acceptsGirl == false)	
		{
			if(Cust.m_Stats[STAT_LIBIDO] >= 80)
			{
				fuckMessage = "Customer chooses her because they are very horny.\n\n";
				acceptsGirl = true;
			}
			else if(((g_Girls.GetStat(girl, STAT_BEAUTY)+g_Girls.GetStat(girl, STAT_CHARISMA))/2) >= 90)	// if she is drop dead gorgeous
			{
				fuckMessage = "Customer chooses her because they are stunned by her beauty.\n\n";
				acceptsGirl = true;
			}
			else if(g_Girls.GetStat(girl, STAT_FAME) >= 80)	// if she is really famous
			{
				fuckMessage = "Customer chooses her because she is so famous.\n\n";
				acceptsGirl = true;
			}
			// WD:	Use Magic only as last resort
			else if(g_Girls.GetSkill(girl, SKILL_MAGIC) > 50 && g_Girls.GetStat(girl, STAT_MANA))	// she can use magic to get him
			{
				fuckMessage = girl->m_Realname + " uses magic to get the customer to choose her.";
				fuckMessage += "\n\n";
				g_Girls.UpdateStat(girl, STAT_MANA, -20);
				acceptsGirl = true;
			}

		}

		// will the customer sleep with her?
		if(!acceptsGirl)
			continue;

#if 0	// WD:	Consolidate diffrent HAPPINESS Calcs
		// set the customers begining happiness/satisfaction
		Cust.m_Stats[STAT_HAPPINESS] = 40;
		// he found someone he is attracted to so gets a small bonus
		Cust.m_Stats[STAT_HAPPINESS] += 20;	
		if(Cust.m_Stats[STAT_HAPPINESS] > 100)
			Cust.m_Stats[STAT_HAPPINESS] = 100;
#else
		// WD:	Set the customers begining happiness/satisfaction
		Cust.m_Stats[STAT_HAPPINESS] = 42 + g_Dice%10 + g_Dice%10; // WD: average 51 range 42 to 60
#endif

		// Horizontal boogy
		g_Girls.GirlFucks(girl, DayNight, &Cust, group, fuckMessage, SexType);
		g_Girls.UpdateTempStat(girl, STAT_LIBIDO, -4);
		NumSleptWith++;
		brothel->m_Filthiness++;

		// update how happy the customers are on average
		brothel->m_Happiness += Cust.m_Stats[STAT_HAPPINESS];


		// Time for the customer to fork over some cash

		// WD:	Customer can not pay
		if(!bCustCanPay)
		{
			pay = 0;	// WD: maybe no money from this customer
			if(g_Dice.percent(Cust.m_Stats[STAT_CONFIDENCE] - 25))	// Runner
			{
				if(g_Gangs.GetGangOnMission(MISS_GUARDING))
				{
					if(g_Dice.percent(50))
						fuckMessage += " The customer couldn't pay and managed to elude your guards.";

					else
					{
						fuckMessage += " The customer couldn't pay and tried to run off. Your men caught him before he got out the door.";
						SetGameFlag(FLAG_CUSTNOPAY);
						pay = (int) Cust.m_Money;	// WD: Take what customer has
						Cust.m_Money = 0;	// WD: ??? not needed Cust record is not saved when this fn ends!  Leave for now just in case ???
					}
				}
				else
					fuckMessage += " The customer couldn't pay and ran off. There were no guards!";
				
			}
			else
			{
				// offers to pay the girl what he has
				if(g_Dice.percent(g_Girls.GetStat(girl, STAT_INTELLIGENCE)))
				{
					// she turns him over to the goons
					fuckMessage += " The customer couldn't pay the full amount, so your girl turned them over to your men.";
					SetGameFlag(FLAG_CUSTNOPAY);
				}
				else
					fuckMessage += " The customer couldn't pay the full amount.";

				pay = (int) Cust.m_Money;
				Cust.m_Money = 0;	// WD: ??? not needed Cust record is not saved when this fn ends!  Leave for now just in case ???
			}
		}


		// WD:	Unhappy Customer tries not to pay and does a runner
		else if(g_Dice.percent((40 - Cust.m_Stats[STAT_HAPPINESS]) / 2) && g_Dice.percent(Cust.m_Stats[STAT_CONFIDENCE] - 25))
		{
			if(g_Gangs.GetGangOnMission(MISS_GUARDING))
			{
				if(g_Dice.percent(50))						
				{
					fuckMessage += " The customer refused to pay and managed to elude your guards.";
					pay = 0;
				}
				else
				{
					fuckMessage += " The customer refused to pay and tried to run off. Your men caught him before he got out the door and forced him to pay.";
					Cust.m_Money -= (unsigned) pay; // WD: ??? not needed Cust record is not saved when this fn ends!  Leave for now just in case ???
				}
			}
			else
			{
				fuckMessage += " The customer refused to pay and ran off. There were no guards!";
				pay = 0;
			}		
#if 0			// Customer refusing to pay old code 
		else
			{
				// offers to pay the girl what he has
				if(g_Dice.percent(g_Girls.GetStat(girl, STAT_INTELLIGENCE)))
				{
					// she turns him over to the goons
					fuckMessage += " The customer couldn't pay so your girl turned them over to your men.";
					SetGameFlag(FLAG_CUSTNOPAY);
//					pay	+= (int)Cust.m_Money; // WD: Borked Cust.m_Money could have any amount and is +=
					if (!bCustCanPay)
					{
						pay = (int) Cust.m_Money;
						
						Cust.m_Money = 0;	// WD: ??? not needed Cust record is not saved when this fn ends!  Leave for now just in case ???
					}
					else
						Cust.m_Money -= (unsigned) pay;	// WD: ??? not needed Cust record is not saved when this fn ends!  Leave for now just in case ???
				}
				else
					// she just accepts less then the agreed upon price
//					pay	+= (int)Cust.m_Money; // WD: Borked Cust.m_Money could have any amount and is +=
					if (!bCustCanPay)
					{
						pay = (int) Cust.m_Money;
						Cust.m_Money = 0;	// WD: ??? not needed Cust record is not saved when this fn ends!  Leave for now just in case ???
					}
					else
						Cust.m_Money -= (unsigned) pay;	// WD: ??? not needed Cust record is not saved when this fn ends!  Leave for now just in case ???


			}
#endif
		}

		else  // Customer has enough money
		{
			Cust.m_Money -= (unsigned) pay; // WD: ??? not needed Cust record is not saved when this fn ends!  Leave for now just in case ???

			// if he is happy and has some extra gold he will give a tip
			if((int)Cust.m_Money >= 20 && Cust.m_Stats[STAT_HAPPINESS] > 90)	
			{
				tip = (int) Cust.m_Money;
				if(tip > 20)
				{
					Cust.m_Money -= 20;	// WD: ??? not needed Cust record is not saved when this fn ends!  Leave for now just in case ???
					tip = 20;
				}
				else
					Cust.m_Money = 0;	// WD: ??? not needed Cust record is not saved when this fn ends!  Leave for now just in case ???

				fuckMessage += ("\nShe received a tip of " + intstring(tip) + " gold");

				pay += tip;

				// Slaves hand over all money to master
				if(girl->is_slave())
				{
					fuckMessage += ", which you claim";
					brothel->m_Finance.brothel_work(tip);
				}
				else
					girl->m_Pay += tip;

				fuckMessage += ".";

				// If the customer is a government official
				if(Cust.m_Official == 1)
				{
					g_Brothels.GetPlayer()->suspicion(-5);
					fuckMessage += " It turns out that the customer was a government official, which lowers your suspicion.";
				}
			}
		}

		// Match image type to the deed done
		int imageType = IMGTYPE_SEX;
		if(SexType == SKILL_ANAL)
			imageType = IMGTYPE_ANAL;
		else if(SexType == SKILL_BDSM)
			imageType = IMGTYPE_BDSM;
		else if(SexType == SKILL_NORMALSEX)
			imageType = IMGTYPE_SEX;
		else if(SexType == SKILL_BEASTIALITY)
			imageType = IMGTYPE_BEAST;
		else if(SexType == SKILL_GROUP)
			imageType = IMGTYPE_GROUP;
		else if(SexType == SKILL_LESBIAN)
			imageType = IMGTYPE_LESBIAN;
		else if(SexType == SKILL_ORALSEX)
			imageType = IMGTYPE_ORAL;
		else if(SexType == SKILL_TITTYSEX)
			imageType = IMGTYPE_TITTY;

		// chance of customer beating or attempting to beat girl
		if(work_related_violence(girl, DayNight, false)) 
			pay = 0;		// WD TRACE WorkRelatedViloence {girl->m_Name} earns nothing

		// if the customer is max happy then give girl some fame
		// WD:	Note fame is also added in girlfucks
		//if(Cust.m_Stats[STAT_HAPPINESS] >= 100)
		//	g_Girls.UpdateStat(girl, STAT_FAME, 2);

		// WD:	Is this being used??
		if(job == JOB_WHOREGAMBHALL)
			pay	+= 30;
		else if(job == JOB_BARWHORE)
			pay	+= 30;


		// WD:	Save gold earned
		girl->m_Pay	+= pay;		// WD TRACE Save Pay {girl->m_Name} earns {pay} totaling {girl->m_Pay}
		girl->m_Events.AddMessage(fuckMessage, imageType, DayNight);
	}


	// WD:	Reduce number of availabe customers for next whore
	iNum = g_Customers.GetNumCustomers();		// WD: Should not happen but lets make sure
	if(iNum < NumSleptWith)
		g_Customers.AdjustNumCustomers(-iNum);
	else
		g_Customers.AdjustNumCustomers(-NumSleptWith);
	
	// WD:	End of shift messages
	// doc: adding braces - gcc warns of ambiguous if nesting
	if (g_Customers.GetNumCustomers() == 0) 
	{
		girl->m_Events.AddMessage("No more customers.", IMGTYPE_PROFILE, DayNight);
	}
	else if(NumSleptWith < NumCusts) 
		girl->m_Events.AddMessage(girl->m_Realname + " ran out of customers who like her.", IMGTYPE_PROFILE, DayNight);
	
	

	// WD:	Summary messages
	ss.str("");
	ss << girl->m_Realname << " saw " << NumSleptWith << " customers this shift.";

	summary += ss.str();

	girl->m_Events.AddMessage(summary, IMGTYPE_PROFILE, DayNight);

	g_Girls.PossiblyGainNewTrait(girl, "Nymphomaniac", 60, ACTION_SEX, girl->m_Realname + " has been having so much sex she is now wanting sex all the time.", DayNight != 0);

	return false;
}
