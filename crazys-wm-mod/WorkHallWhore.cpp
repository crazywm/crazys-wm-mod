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

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cPlayer* The_Player;

//SIN	// `J` this was only in brothel whore so I copied it to hall and bar whore
//SPICE = added a lot of spice (variety/trait/skill) to dialogues
//1 turns them ON, 0 turns them OFF (compiles with warnings, but not errors - worth it for the easy search)
#define SPICE 1;

// `J` Job Brothel - Hall
bool cJobManager::WorkHallWhore(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_SEX;
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

	stringstream fuckMessage;
	stringstream ss;
	string girlName = girl->m_Realname;
	int iNum = 0;
	int iOriginal = 0;
	int	AskPrice = girl->askprice();
	int pay = 0;					// pay from a single customer
	int tip = 0;					// tip from a single customer
	int wages = 0;					// pay from all customers
	int tips = 0;					// tips from all customers
	int LoopCount;
	bool group = false;				// Group sex flag
	bool knowwife = false;			// if the girl is your daughter and the customer knows this
	bool knowdaughter = false;		// if the girl is your wife and the customer knows this
	bool bCustCanPay = true;		// Customer has enough money to pay
	bool custout = false;	// if a male customer tries running and gets caught
	bool femalecustcaught = false;	// if a female customer tries running and gets caught
	bool acceptsGirl = false;		// Customer will sleep girl

	int oralcount = 0;		// how much oral she gave for use with AdjustTraitGroupGagReflex
	u_int SexType = 0;
	u_int job = (Day0Night1 ? girl->m_NightJob : girl->m_DayJob);

	// work out how many customers the girl can service

	// Changed the number of custmers stats add.. reasone was that old value,
	// it was only adding 1 customer per stat, unless stat was 100 for beauty and Charisma. Fame would add a max of 3. and only if was = 10
	// there would be NO point in doing this, if it defaults to NumCusts++ since it is basically the same effect.	-PP

	// Max number on customers the girl can fuck
	int b = girl->beauty(), c = girl->charisma(), f = girl->fame();
	int NumCusts = min(8, 2 + ((b + c + f + 1) / 50));
	int NumSleptWith = 0;		// Total num customers she fucks this session

	// Complications

	/*
	*	WD:	Rival Gang is incompleate
	*
	*	Chance of defeating gang is based on  combat / magic skill
	*	Added Damage and Tiredness
	*	ToDo Girl fightrivalgang() if its implemented
	*
	*/

	// WD: Set the limits on the Number of customers a girl can try and fuck
	LoopCount = max(NumCusts * 2, 5);

	// WD: limit to number of customers left
	if (LoopCount > g_Customers.GetNumCustomers()) LoopCount = g_Customers.GetNumCustomers();

	for (int i = 0; i < LoopCount; i++)	// Go through all customers
	{
		fuckMessage.str("");
		// WD:	Move exit test to top of loop
		// if she has already slept with the max she can attact then stop processing her fucking routine
		if (NumSleptWith >= NumCusts) break;
		// Stop if she has worked the bare minimum and tiredness is high enough to get a warning, pushing too hard is bad for the business too
		if ((girl->tiredness() > 80 || girl->health() < 20) && NumSleptWith >= 2) break;

		// WD:	Init Loop variables
		pay = AskPrice;
		SexType = 0;
		group = false;
		acceptsGirl = false;
		// WD:	Create Customer
		sCustomer Cust = g_Customers.GetCustomer(*brothel);

		// `J` check for disease
		if (g_Girls.detect_disease_in_customer(brothel, girl, &Cust)) continue;

		// filter out unwanted sex types (unless it is street work)
		if (!is_sex_type_allowed(Cust.m_SexPref, brothel) && !is_sex_type_allowed(Cust.m_SexPrefB, brothel))
		{
			brothel->m_RejectCustomersRestrict++;
			continue;	// `J` if both their sexprefs are banned then they leave
		}
		else if (!is_sex_type_allowed(Cust.m_SexPref, brothel)) // it their first sexpref is banned then switch to
		    // the second
		{
			SexType = Cust.m_SexPref = Cust.m_SexPrefB;
			Cust.m_Stats[STAT_HAPPINESS] = 32 + g_Dice % 9 + g_Dice % 9;	// `J` and they are less happy
		}
		else	// `J` otherwise they are happy with their first choice.
		{
			// WD:	Set the customers begining happiness/satisfaction
			Cust.m_Stats[STAT_HAPPINESS] = 42 + g_Dice % 10 + g_Dice % 10; // WD: average 51 range 42 to 60
			SexType = Cust.m_SexPref;
		}

		// WD:	Consolidate GROUP Sex Calcs here
		//		adjust price by number of parcitipants
		if (Cust.m_Amount > 1)
		{
			group = true;
			if (Cust.m_SexPref == SKILL_GROUP)	pay = (Cust.m_Amount * pay);					// full price for all
			        // of them
			if (Cust.m_SexPref == SKILL_STRIP)	pay += (int)((Cust.m_Amount - 1) * pay / 2);	// full price for the
			        // first then half price for the rest
			// WD: this is complicated total for 1.7 * pay * num of customers
			// pay += (int)((float)(pay*(Cust->m_Amount))*0.7f);
		}

		// WD: Has the customer have enough money
		bCustCanPay = Cust.m_Money >= pay;

		// WD:	TRACE Customer Money = {Cust->m_Money}, Pay = {pay}, Can Pay = {bCustCanPay}

		// WD:	If the customer doesn't have enough money, he will only sleep with her if he is stupid
		if (!bCustCanPay && !g_Dice.percent((50 - Cust.m_Stats[STAT_INTELLIGENCE]) / 5))
		{
			//continue;
			// WD: Hack to avoid many newcustomer() calls
			Cust.m_Money += pay;
			bCustCanPay = true;
		}

		// test for specific girls
		if (girl->has_trait("Skeleton"))
		{
			fuckMessage << "The customer sees that you are offering up a Skeleton for sex and is scared, if you allow that kind of thing in your brothels, what else do you allow? They left in a hurry, afraid of what might happen if they stay.\n \n";
			brothel->m_Fame -= 5;
			The_Player->customerfear(2);
			acceptsGirl = false;
			continue;
		}
		if (Cust.m_Fetish == FETISH_SPECIFICGIRL)
		{
			if (Cust.m_ParticularGirl == g_Brothels.GetGirlPos(brothel->m_id, girl))
			{
				fuckMessage << "This is the customer's favorite girl.\n \n";
				acceptsGirl = true;
			}
		}
		else if (girl->has_trait("Zombie") && Cust.m_Fetish == FETISH_FREAKYGIRLS && g_Dice.percent(10))
		{
			fuckMessage << "This customer is intrigued to fuck a Zombie girl.\n \n";
			acceptsGirl = true;
		}
		else
		{
			// 50% chance of getting something a little weirder during the night
			if (Day0Night1 && Cust.m_Fetish < NUM_FETISH - 2 && g_Dice.percent(50)) Cust.m_Fetish += 2;

			// Check for fetish match
			if (g_Girls.CheckGirlType(girl, Cust.m_Fetish))
			{
				fuckMessage << "The customer loves this type of girl.\n \n";
				acceptsGirl = true;
			}
		}

		// Other ways the customer will accept the girl
		if (acceptsGirl == false)
		{
			if (girl->has_trait("Zombie"))
			{
				fuckMessage << "The customer sees that you are offering up a Zombie girl and is scared, if you allow that kind of thing in your brothels, what else do you allow? They left in a hurry, afraid of what might happen if they stay.\n \n";
				brothel->m_Fame -= 10;
				The_Player->customerfear(5);
				acceptsGirl = false;
			}
			else if (girl->has_trait("Lesbian") && Cust.m_IsWoman && g_Dice.percent(50))
			{
				fuckMessage << "The female customer chooses her because she is a Lesbian.\n \n";
				acceptsGirl = true;
			}
			else if (girl->has_trait("Straight") && Cust.m_IsWoman && g_Dice.percent(10))
			{
				fuckMessage << girlName << " refuses to accept a female customer because she is Straight.\n \n";
				brothel->m_Fame -= 2;
				acceptsGirl = false;
			}
			else if (girl->has_trait("Lesbian") && !Cust.m_IsWoman && g_Dice.percent(10))
			{
				fuckMessage << girlName << " refuses to accept a male customer because she is a Lesbian.\n \n";
				brothel->m_Fame -= 5;
				acceptsGirl = false;
			}
			else if (girl->dignity() >= 70 && Cust.m_SexPref == SKILL_BEASTIALITY && g_Dice
                                                                                                                 .percent(20))	//
			{
				fuckMessage << girlName << " refuses to sleep with a beast because she has too much dignity for that.\n \n";
				brothel->m_Fame -= 5;
				acceptsGirl = false;
			}
			else if ((girl->has_trait("Queen") || girl->has_trait("Princess")) && Cust.m_SexPref == SKILL_BEASTIALITY
                     && g_Dice.percent(20))
			{
				fuckMessage << girlName << " refuses to sleep with a beast because one of Royal blood is above that.\n \n";
				brothel->m_Fame -= 5;
				acceptsGirl = false;
			}
			else if (girl->is_pregnant() && Cust.m_SexPref == SKILL_BEASTIALITY && g_Dice.percent(35))
			{
				fuckMessage << girlName << " refuses because she shouldn't fuck beasts in her condition.\n \n";
				brothel->m_Fame -= 5;
				acceptsGirl = false;
			}
			else if (girl->health() < 33 && g_Dice.percent(50))
			{
				fuckMessage << "The customer refuses because " << girlName << " looks sick and he doesn't want to catch anything.\n \n";
				brothel->m_Fame -= 10;
				acceptsGirl = false;
			}
			else if (girl->has_trait( "Your Daughter") && g_Dice.percent(20))
			{
				fuckMessage << "The customer chooses her because " << (Cust.m_IsWoman ? "she" : "he") << " wants to "
                                                                                             "fuck your daughter.\n \n";
				knowdaughter = true;
				acceptsGirl = true;
			}
			else if (girl->has_trait( "Your Wife") && g_Dice.percent(20))
			{
				fuckMessage << "The customer chooses her because " << (Cust.m_IsWoman ? "she" : "he") << " wants to "
                                                                                             "fuck your wife.\n \n";
				knowwife = true;
				acceptsGirl = true;
			}
			else if (girl->has_trait("Porn Star") && g_Dice.percent(15))
			{
				fuckMessage << "The customer chooses her because " << (Cust.m_IsWoman ? "she" : "he") << " has seen "
                                                                                             "her in porn.\n \n";
				acceptsGirl = true;
			}
			else if ((girl->has_trait("Queen") || girl->has_trait("Princess")) && g_Dice.percent(10))
			{
				fuckMessage << "The customer chooses her because she is former royalty.\n \n";
				acceptsGirl = true;
			}
			else if (girl->has_trait("Teacher") && g_Dice.percent(10))
			{
				fuckMessage << "The customer chooses her because " << (Cust.m_IsWoman ? "she" : "he") << " used to "
                                                                                             "daydream about this back when "
					<< (Cust.m_IsWoman ? "she" : "he") << " was in " << girlName << "'s class.\n \n";
				acceptsGirl = true;
			}
			else if (girl->has_trait("Old") && g_Dice.percent(20))
			{
				fuckMessage << "The customer chooses her because " << (Cust.m_IsWoman ? "she" : "he") << " likes "
                                                                                             "mature women.\n \n";
				acceptsGirl = true;
			}
			else if (girl->has_trait("Natural Pheromones") && g_Dice.percent(20))
			{
				fuckMessage << "The customer chooses her for reasons " << (Cust.m_IsWoman ? "she" : "he") << " can't "
                                                                                                 "explain. There's something about her.\n \n";
				acceptsGirl = true;
			}
			else if (g_Dice.percent(10) && girl->has_trait("Lolita"))
			{
				fuckMessage << "The customer chooses her because " << (Cust.m_IsWoman ? "she wants a young woman, "
                                                                            "uncorrupted by men.\n" : "he's hoping for a virgin, and she looks like one.\n") << "\n";
				acceptsGirl = true;
			}
			else if (g_Dice.percent(20) && girl->has_trait("Social Drinker"))
			{
				fuckMessage << "The customer chooses her because she's fun, flirty and half-cut.\n \n";
				acceptsGirl = true;
			}
			else if (g_Dice.percent(40) && girl->has_trait("Exhibitionist") && girl->beauty() >= 50)
			{
				fuckMessage << "The customer chooses her because she walks into the waiting room naked and the customer likes what "					<< (Cust.m_IsWoman ? "she sees.\n"  : "he sees.\n") << "\n";
				acceptsGirl = true;
			}
			else if (g_Dice.percent(5) && (girl->has_trait("Slut") || girl->dignity() >= 70))
			{
				fuckMessage << girlName << " gets bored of waiting for someone to step up and starts " << (Cust.m_IsWoman ? "fingering this lady" : "giving this guy a handjob")					<< " right there in the waiting room. The customer quickly chooses her.\n \n";
				acceptsGirl = true;
			}
			else if (Cust.m_Stats[STAT_LIBIDO] >= 80)
			{
				fuckMessage << "Customer chooses her because they are very horny.\n \n";
				acceptsGirl = true;
			}
			else if (((girl->beauty() + girl->charisma()) / 2) >= 90)	// if she is drop dead gorgeous
			{
				fuckMessage << "Customer chooses her because they are stunned by her beauty.\n \n";
				acceptsGirl = true;
			}
			else if (girl->fame() >= 80)	// if she is really famous
			{
				fuckMessage << "Customer chooses her because she is so famous.\n \n";
				acceptsGirl = true;
			}
			// WD:	Use Magic only as last resort
			else if (girl->magic() > 50 && girl->mana() >= 20)	// she can use magic to get him
			{
				fuckMessage << girlName << " uses magic to get the customer to choose her.\n \n";
				girl->mana(-20);
				acceptsGirl = true;
			}
		}

		if (!acceptsGirl)	// if the customer will not sleep with her
		{
			if (fuckMessage.str().length() > 0)	// if there is a reason, include it in her messages.
			{
				girl->m_Events.AddMessage(fuckMessage.str(), IMGTYPE_PROFILE, EVENT_WARNING);
			}
			continue;	// otherwise just move on
		}

		// Horizontal boogy
		string fm = "";
		g_Girls.GirlFucks(girl, Day0Night1, &Cust, group, fm, SexType);
		fuckMessage << fm;

		/* */if (SexType == SKILL_ORALSEX)		oralcount += 5;
		else if (SexType == SKILL_GROUP)		oralcount += 5;
		else if (SexType == SKILL_BEASTIALITY)	oralcount += g_Dice % 3;
		else if (SexType == SKILL_LESBIAN)		oralcount += g_Dice % 2;
		else if (SexType == SKILL_TITTYSEX)		oralcount += g_Dice % 2;
		else if (SexType == SKILL_HANDJOB)		oralcount += g_Dice % 2;

		NumSleptWith++;
		brothel->m_Filthiness++;

		// update how happy the customers are on average
		brothel->m_Happiness += Cust.m_Stats[STAT_HAPPINESS];

		// Time for the customer to fork over some cash

		// WD:	Customer can not pay
		sGang* guardgang = g_Gangs.GetRandomGangOnMission(MISS_GUARDING);
		if (!bCustCanPay)
		{
			if (g_Dice.percent(Cust.m_Stats[STAT_CONFIDENCE] - 25))	// Runner
			{
				fuckMessage << " The customer couldn't pay and ";
				if (guardgang)
				{
					if (g_Dice%Cust.m_Stats[STAT_AGILITY] > guardgang->m_Num + g_Dice%guardgang->agility())
					{
						fuckMessage << "managed to elude your guards.";
						pay = 0;
					}
					else
					{
						pay = (int)Cust.m_Money;	// WD: Take what customer has
						Cust.m_Money = 0;
						fuckMessage << "tried to run off.";
						if (Cust.m_IsWoman)	femalecustcaught = true;
						else
						{
							fuckMessage << " Your men caught him before he got out the door.";
							custout = true;
						}
					}
				}
				else
				{
					fuckMessage << "ran off. There were no guards!";
					pay = 0;
				}
			}
			else
			{
				// offers to pay the girl what he has
				fuckMessage << " The customer couldn't pay the full amount";
				if (g_Dice.percent(girl->intelligence()))	// she turns him over to the goons
				{
					fuckMessage << " so your girl turned them over to your men";
					if (Cust.m_IsWoman)	femalecustcaught = true;
					else/*            */	custout = true;
				}
				fuckMessage << ".";
				pay = Cust.m_Money;
				Cust.m_Money = 0;
			}
		}
		// WD:	Unhappy Customer tries not to pay and does a runner
		else if (g_Dice.percent((40 - Cust.m_Stats[STAT_HAPPINESS]) / 2) && g_Dice.percent(Cust.m_Stats[STAT_CONFIDENCE] - 25))
		{
			fuckMessage << " The customer refused to pay and ";
			if (guardgang)
			{
				if (g_Dice%Cust.m_Stats[STAT_AGILITY] > guardgang->m_Num + g_Dice%guardgang->agility())
				{
					fuckMessage << "managed to elude your guards.";
					pay = 0;
				}
				else if (Cust.m_Money > pay + 10)
				{
					Cust.m_Money -= pay; // WD: ??? not needed Cust record is not saved when this fn ends!  Leave for now just in case ???
					int extra = 10 + g_Dice%Cust.m_Money;
					fuckMessage << "tried to run off. Your men caught him before he got out the door and forced him to pay the full amount plus " << extra << " extra for not throwing them in the dungeon.";
					pay += extra;
				}
				else
				{
					pay = (int)Cust.m_Money;	// WD: Take what customer has
					Cust.m_Money = 0;
					fuckMessage << "tried to run off.";
					if (Cust.m_IsWoman)	femalecustcaught = true;
					else
					{
						fuckMessage << " Your men caught him before he got out the door.";
						custout = true;
					}
				}
			}
			else
			{
				fuckMessage << " ran off. There were no guards!";
				pay = 0;
			}
		}
		else if ((knowwife || knowdaughter) && g_Dice.percent(Cust.m_Stats[STAT_CONFIDENCE] / 5))
		{
			fuckMessage << " The customer wanted to screw you and your " << (knowwife ? "wife" : "daughter") << " so they made a break for it";
			if (guardgang)
			{
				if (g_Dice%Cust.m_Stats[STAT_AGILITY] > guardgang->m_Num + g_Dice%guardgang->agility())
				{
					fuckMessage << " and managed to elude your guards.";
					pay = 0;
				}
				else if (Cust.m_Money > pay + 10)
				{
					Cust.m_Money -= pay; // WD: ??? not needed Cust record is not saved when this fn ends!  Leave for now just in case ???
					int extra = 10 + g_Dice%Cust.m_Money;
					fuckMessage << " tried to run off. Your men caught him before he got out the door and forced him to pay the full amount plus " << extra << " extra for not throwing them in the dungeon.";
					pay += extra;
				}
				else
				{
					pay = (int)Cust.m_Money;	// WD: Take what customer has
					Cust.m_Money = 0;
					fuckMessage << " tried to run off.";
					if (Cust.m_IsWoman)	femalecustcaught = true;
					else
					{
						fuckMessage << " Your men caught him before he got out the door.";
						custout = true;
					}
				}
			}
			else
			{
				fuckMessage << ". They got away because there were no guards!";
				pay = 0;
			}
		}
		else  // Customer has enough money
		{
			Cust.m_Money -= (unsigned)pay; // WD: ??? not needed Cust record is not saved when this fn ends!  Leave for now just in case ??? // Yes this is necessary for TIP calculation.
			if (girl->has_trait( "Your Daughter") && knowdaughter && Cust.m_Money >= 20 && g_Dice.percent(50))
			{
				fuckMessage << "The customer tosses your daughter a bag of gold";
				switch (g_Dice % 3)
				{
				case 0:		fuckMessage << " saying no dad should do this to their daughter.";					break;
				case 1:		fuckMessage << ". They seem to enjoy the thought of fucking the boss's daughter.";	break;
				default:	fuckMessage << " with a wink and a smile.";											break;
				}
				Cust.m_Money -= 20;
				tip += 20;
			}
			else if (girl->has_trait( "Your Wife") && knowwife && Cust.m_Money >= 20 && g_Dice.percent(50))
			{
				fuckMessage << "The customer tosses your wife a bag of gold";
				switch (g_Dice % 3)
				{
				case 0:		fuckMessage << " and tells her she can do better.";		break;
				case 1:		fuckMessage << " and asks who is better in the sack.";	break;
				default:	fuckMessage << " with a wink and a smile.";				break;
				}
				Cust.m_Money -= 20;
				tip += 20;
			}
			else if (girl->has_trait( "Your Daughter") && Cust.m_Money >= 20 && g_Dice.percent(15))
			{
				if (g_Dice.percent(50))
				{
					fuckMessage << "Learning that she was your daughter the customer tosses some extra gold down saying no dad should do this to their daughter.";
				}
				else
				{
					fuckMessage << "A smile crossed the customer's face upon learning that she is your daughter and they threw some extra gold down. They seem to enjoy the thought of fucking the boss's daughter.";
				}
				Cust.m_Money -= 20;
				tip += 20;
			}
			else if (girl->has_trait( "Your Wife") && Cust.m_Money >= 20 && g_Dice.percent(15))
			{
				if (g_Dice.percent(50))
				{
					fuckMessage << "Learning that she was your wife the customer tosses some extra gold down saying no husband should do this to their wife.";
				}
				else
				{
					fuckMessage << "A smile crossed the customer's face upon learning that she is your wife and they threw some extra gold down. They seem to enjoy the thought of fucking the boss's wife.";
				}

				Cust.m_Money -= 20;
				tip += 20;
			}
			// if he is happy and has some extra gold he will give a tip
			if (Cust.m_Money >= 20 && Cust.m_Stats[STAT_HAPPINESS] > 90)
			{
				tip = (int)Cust.m_Money;
				if (tip > 20)
				{
					Cust.m_Money -= 20;
					tip = 20;
				}
				else Cust.m_Money = 0;

				fuckMessage << "\nShe received a tip of " << tip << " gold.";
				girl->m_Tips += tip;

				// If the customer is a government official
				if (Cust.m_Official == 1)
				{
					The_Player->suspicion(-5);
					fuckMessage << " It turns out that the customer was a government official, which lowers your suspicion.";
				}
			}
		}
		fuckMessage << "\n";

		if (custout)
		{
			SetGameFlag(FLAG_CUSTNOPAY);		// male customers use the old code
		}
		else if (femalecustcaught)
		{
			// try to find an item
			/*			 */int	itemnum = g_Brothels.HasItem("Brainwashing Oil", -1);
			if (itemnum == -1)	itemnum = g_Brothels.HasItem("Necklace of Control", -1);
			if (itemnum == -1)	itemnum = g_Brothels.HasItem("Slave Band", -1);
			if (itemnum >= 0 || g_Dice.percent(10))
			{
				sInventoryItem* item;
				string itemname = "";
				stringstream itemtext;
				itemtext << "Your gang " << (guardgang ? guardgang->m_Name : "");
				stringstream CGmsg;
				// `J` create the customer
				sGirl* custgirl = g_Girls.CreateRandomGirl(18 + (max(0, g_Dice % 40 - 10)), false, true, false, (g_Dice % 3 == 1));	// `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live
				int emprisontraittime = 1;
				custgirl->pclove(-(g_Dice % 50 + 50));
				custgirl->pcfear(g_Dice % 50 + 50);
				custgirl->pchate(g_Dice % 50 + 50);
				custgirl->m_Enjoyment[ACTION_COMBAT] -= (g_Dice % 50 + 20);
				custgirl->m_Enjoyment[ACTION_SEX] -= (g_Dice % 50 + 20);

				if (itemnum == -1)	// no item
				{
					itemtext << " beat up " << custgirl->m_Realname;
					fuckMessage << itemtext.str() << " and dragged her to the dungeon.";
					itemtext << " until she stops fighting back. They place your brand on her and bring your new slave to the dungeon.";
					emprisontraittime = 5 + g_Dice % 21;
					g_Girls.SetStat(custgirl, STAT_HEALTH, 5 + g_Dice % 70);
					g_Girls.SetStat(custgirl, STAT_HAPPINESS, g_Dice % 50);
					g_Girls.SetStat(custgirl, STAT_TIREDNESS, 50 + g_Dice % 51);
					girl->obedience(g_Dice % 10);
					girl->pchate(g_Dice % 10);
					girl->pcfear(g_Dice % 10);
					girl->pclove(-(g_Dice % 10));
					girl->spirit(-(g_Dice % 10));
				}
				else		// item was found
				{
					item = g_Brothels.m_Inventory[itemnum];
					itemname = item->m_Name;
					g_InvManager.Equip(custgirl, custgirl->add_inv(item), true);
					g_Brothels.RemoveItemFromInventoryByNumber(itemnum);
					if (itemname == "Brainwashing Oil")
					{
						emprisontraittime += g_Dice % 5;
						itemtext << " forced a bottle of Brainwashing Oil down her throat";
						fuckMessage << itemtext.str() << " and dragged her to the dungeon.";
						itemtext << ". After a few minutes of struggling,";
					}
					else if (itemname == "Necklace of Control")
					{
						emprisontraittime += g_Dice % 10;
						itemtext << " placed a Necklace of Control around her neck";
						fuckMessage << itemtext.str() << " and dragged her to the dungeon.";
						itemtext << ". After a few minutes of struggling, the magic in the necklace activated and";
					}
					else if (itemname == "Slave Band")
					{
						emprisontraittime += g_Dice % 15;
						itemtext << " placed a Slave Band on her arm";
						fuckMessage << itemtext.str() << " and dragged her to the dungeon.";
						itemtext << ". After a few minutes of struggling, the magic in the Slave Band activated and";
					}
					itemtext << " your new slave, " << custgirl->m_Realname << ", was taken to the dungeon.";

				}
				// `J` do all the messages
				CGmsg << custgirl->m_Realname << " was caught trying to run out without paying for services provided by " << girl->m_Realname << ".\n \n" << itemtext.str();
				custgirl->add_trait("Emprisoned Customer", emprisontraittime);	// add temp trait
				if (g_Dice.percent(75))	custgirl->lose_virginity();				// most of the time she will not be a virgin
				The_Player->suspicion(g_Dice % 10);
				The_Player->disposition(-(g_Dice % 10));
				The_Player->customerfear(g_Dice % 10);
				custgirl->m_Money = 0;

				custgirl->m_Events.AddMessage(CGmsg.str(), IMGTYPE_DEATH, EVENT_WARNING);
				// `J` add the customer to the dungeon
				g_Brothels.GetDungeon()->AddGirl(custgirl, DUNGEON_CUSTNOPAY);
			}
			else SetGameFlag(FLAG_CUSTNOPAY);		// if not taken as a slave, use the old code
		}
		else
		{
			// chance of customer beating or attempting to beat girl
			if (work_related_violence(girl, Day0Night1, false))
				pay = 0;		// WD TRACE WorkRelatedViloence {girl->m_Name} earns nothing
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
		else if (SexType == SKILL_FOOTJOB)		imageType = IMGTYPE_FOOT;
		else if (SexType == SKILL_STRIP)		imageType = IMGTYPE_STRIP;


		// WD:	Save gold earned
		wages += pay;
		tips += tip;
		girl->m_Events.AddMessage(fuckMessage.str(), imageType, Day0Night1);
	}

	// WD:	Reduce number of availabe customers for next whore
	iNum = g_Customers.GetNumCustomers();		// WD: Should not happen but lets make sure
	if (iNum < NumSleptWith)	g_Customers.AdjustNumCustomers(-iNum);
	else						g_Customers.AdjustNumCustomers(-NumSleptWith);

	// End of shift messages
	ss.str("");
	ss << girlName << " saw " << NumSleptWith << " customers this shift.";
	if (g_Customers.GetNumCustomers() == 0)	{ ss << "\n \nThere were no more customers left."; }
	else if (NumSleptWith < NumCusts)		{ ss << "\n \nShe ran out of customers who like her."; }
	summary += ss.str();

	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);
	girl->m_Events.AddMessage(summary, IMGTYPE_PROFILE, Day0Night1);

	//gain
	g_Girls.PossiblyGainNewTrait(girl, "Good Kisser", 50, actiontype, girlName + " has had a lot of practice kissing and as such as become a Good Kisser.", Day0Night1);
	g_Girls.PossiblyGainNewTrait(girl, "Slut", 60, actiontype, girlName + " has turned into quite a slut.", Day0Night1, EVENT_WARNING);
	g_Girls.PossiblyGainNewTrait(girl, "Nymphomaniac", 70, actiontype, girlName + " has been having so much sex she is now wanting sex all the time.", Day0Night1);

	//SIN: use a few of the new traits
	if (g_Dice.percent(1) && g_Dice.percent(girl->oralsex()) && (girl->has_trait( "Nymphomaniac")))
		g_Girls.PossiblyGainNewTrait(girl, "Cum Addict", 90, actiontype, girlName + " has tasted so much cum she now craves it at all times.", Day0Night1);

	if (girl->oralsex() > 30 && g_Dice.percent(oralcount))
		g_Girls.AdjustTraitGroupGagReflex(girl, +1, true, Day0Night1);

	return false;
}

double cJobManager::JP_HallWhore(sGirl* girl, bool estimate)// not used
{
	double jobperformance = 0.0;
	if (estimate)// for third detail string
	{
		jobperformance += g_Girls.GetAverageOfSexSkills(girl) + (girl->charisma() + girl->beauty()) / 2;
	}
	else// for the actual check
	{
	}
	return jobperformance;
}
