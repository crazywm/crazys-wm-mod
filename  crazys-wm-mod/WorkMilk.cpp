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
#include "cRng.h"
#include "CLog.h"
#include "cMessageBox.h"
#include "cGold.h"
#include "cBrothel.h"
#include "cFarm.h"

extern CLog g_LogFile;
extern cMessageQue g_MessageQue;
extern cRng g_Dice;
extern cGold g_Gold;
extern cBrothelManager g_Brothels;
extern cFarmManager g_Farm;
extern cInventory g_InvManager;
extern cJobManager m_JobManager;
extern cCustomers g_Customers;
extern cPlayer* The_Player;
//BSIN: function used to convert to ounces
double toOz(int ml)			{ return (0.0338 * ml); }



// `J` Job Farm - Laborers
bool cJobManager::WorkMilk(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKMILK;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;
	if (girl->disobey_check(actiontype, brothel))			// they refuse to work
	{
		ss << " refused to let her breasts be milked " << (Day0Night1 ? "tonight." : "today.");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << "'s breasts were milked.\n \n";



	g_Girls.UnequipCombat(girl);	// not needed

	sGirl* farmmanonduty = g_Brothels.GetRandomGirlOnJob(0, JOB_FARMMANGER, Day0Night1);
	string farmmanname = (farmmanonduty ? "Farm Manager " + farmmanonduty->m_Realname + "" : "the Farm Manager");

	int enjoy = 0;
	int wages = 0, tips = 0;
	int roll = g_Dice.d100();
	/*int roll_a = g_Dice.d100(), roll_b = g_Dice.d100(), roll_c = g_Dice.d100();*/


#if 1
	//\\BSIN - doesn't look like this has been touched in the last year - completing
	//\\On testing, decided to half the output volume on virgins and non-pregs without MILF trait.
	// `J` New job function - needs work - commenting out for now
	/*
	100 lactation + preg + notrait = volume should be 20-30oz per day, 140-210oz per week

	*Breast size*			*L*		*P*		*B*		*ml*	*oz*	*pt*			*L* = Lactation
	Flat Chest				100		2		1		200		 6.8	0.4				*P* = Pregnant
	Petite Breasts			100		2		2		400		13.5	0.8				*B* = Breast size multiplier
	Small Boobs				100		2		3		600		20.3	1.3				*ml* = milliliters
	(Normal)				100		2		4		800		27.1	1.7				*oz* = ounces
	Busty Boobs				100		2		5		1000	33.8	2.1				*Pt* = pints
	Big Boobs				100		2		6		1200	40.6	2.5
	Giant Juggs				100		2		7		1400	47.3	3.0				//// Lactation traits increase or decrease lactation up to 40%
	Massive Melons			100		2		8		1600	54.1	3.4
	Abnormally Large Boobs	100		2		9		1800	60.9	3.8
	Titanic Tits			100		2		10		2000	67.6	4.2

	//*/

	//BSIN
	//
	//MAGIC NUMBER COMING NEXT - milk seems to sell for 50G/oz, so trade/wholesale value of 10 seems reasonable and produces fair output.
	//Gives less than current for small breasted, non-pregs, and more for pregnant breasted girls.
	const int MILKWHOLESALE = 10;
	const int CATGIRLBONUS = 2;			//CG milk is four times as much in shop - trade boost of 2 seems right (shop pockets difference)
	//
	//
	//
	//INITIATE THINGS I NEED
	int volume = 0;						// in milliliters
	int lactation = girl->lactation();	// how much?
	bool isPregnant = (girl->m_States&(1 << STATUS_PREGNANT) || girl->m_States&(1 << STATUS_PREGNANT_BY_PLAYER) || girl->m_States&(1 << STATUS_INSEMINATED)) ? true : false;
	int pregMultiplier = ((isPregnant) ? 2 : 1);
	bool yours = (girl->m_States&(1 << STATUS_PREGNANT_BY_PLAYER)) ? true : false;
	int nips = 0;
	int HateLove = 0;
	HateLove = girl->pclove() - girl->pchate();
	//BSIN: Update: to generate an extra message in case of certain event
	stringstream ssextra;
	int extraimage = 0;
	bool extraEvent = false;
	bool noAnti = girl->m_UseAntiPreg = false;

	int breastsize = girl->breast_size();

	// Milk - a multiplier on base lactation
	if (girl->has_trait( "Dry Milk"))						{ lactation = (lactation * 4) / 10; }
	if (girl->has_trait( "Scarce Lactation"))				{ lactation = (lactation * 6) / 10; }
	if (girl->has_trait( "Abundant Lactation"))			{ lactation = (lactation * 14) / 10; }
	if (girl->has_trait( "Cow Tits"))						{ lactation = (lactation * 16) / 10; }



	//test code for auto preg
	if (girl->m_WeeksPreg < 0 && g_Brothels.GetNumGirlsOnJob(0, JOB_FARMMANGER, false) >= 1 && noAnti && !girl->check_virginity())
	{
		sCustomer Cust = g_Customers.GetCustomer(*brothel);
		ss << farmmanname <<" noticing that " << girlName << " wasn't pregnant decided to take it upon herself to make sure she got knocked up.\n";
		if (m_JobManager.is_sex_type_allowed(SKILL_BEASTIALITY, brothel) && g_Brothels.GetNumBeasts() >= 1 && g_Dice.percent(50))
		{
			ss << "She sends in one of your beasts to get the job done.";
			girl->beastiality(2);
			girl->m_Events.AddMessage(ss.str(), IMGTYPE_BEAST, Day0Night1);
			if (!girl->calc_insemination(*g_Girls.GetBeast(), false, 1.0))
			{
				g_MessageQue.AddToQue(girl->m_Realname + " has gotten inseminated", 0);
			}
		}
		else
		{
			ss << "She found a random man off the street and offered him the chance to have sex with " << girlName << " for free as long as he cummed inside her. He jumped at the chance for it.";
			girl->normalsex(2);
			girl->m_Events.AddMessage(ss.str(), IMGTYPE_SEX, Day0Night1);
			if (!girl->calc_pregnancy(Cust, false, 1.0))
			{
				g_MessageQue.AddToQue(girlName + " has gotten pregnant", 0);
			}
		}
	}


	//Lets get some scenario...
	int predisposition = girl->get_enjoyment(JOB_MILK);
	ss << girlName;
	/**/ if (predisposition <= -50)	ss << " froze when she saw the milking area. She had to be strapped in securely for milking.";
	else if (predisposition <= 10)	ss << " was led into the stall, strapped in and milked without incident.";
	else if (predisposition <= 50)	ss << " walked into the stall, sat down and got her breasts out. She massaged her breasts in preparation for being milked.";
	else							ss << " ran to the stall, stripped off all her clothes, and waited impatiently for the milker to get to her. She seems to like this.";
	ss << "\n \n";


	//Milking action - adapted from below
	if (breastsize <= 3)   // Using breastsize figure found above to reduce repeated trait calls
	{
		if (isPregnant)
		{
			ss << girlName << " only has little breasts, but her body still produces milk in anticipation of nursing " << ((yours) ? "your" : "her") << " child.";
		}
		else
		{
			ss << girlName << " has small breasts, which only produce a small yield.";
		}
	}
	else if (breastsize == 4)
	{
		if (isPregnant)
		{
			ss << girlName << " has average sized breasts, which yield a fair amount of milk " << ((yours) ? "now you've knocked her up." : "with the help of her pregnancy.");
		}
		else
		{
			ss << girlName << " has average sized breasts, perfect handfuls, which yield an okay amount of milk.";
		}
	}
	else if (breastsize <= 7)
	{
		if (isPregnant)
		{
			ss << girlName << "'s already sizable breasts have become engorged with milk in preparation for " << ((yours) ? "your" : "her") << " child.";
		}
		else
		{
			ss << girlName << " has large breasts, that yield a good amount of milk to the suction machine even without pregnancy.";
		}
	}
	else
	{
		if (isPregnant)
		{
			ss << girlName << " had ridiculously large breasts already. Now she's pregnant " << ((yours) ? "with your child " : "")
				<< "her tits are each larger than her head, and are dripping with milk near continuously.";
		}
		else
		{
			ss << girlName << "'s massive globes don't need pregnancy to yield a profitable quantity of milk!";
		}
	}
	ss << "\n \n";

	//Calculating base volume per above formula
	volume = lactation * pregMultiplier * breastsize;	//vol in ml
	//some randomization to prevent identical production numbers every time
	int randVol = volume / 10;			// rand vol = 10 percent of vol
	volume -= randVol;					// removing the 10 percent
	volume += 2 * (g_Dice % randVol);	// adding back between 0 and 20%


	//Testing and seems weird that virgins and never-pregs can produce so much, so halving this
		//This is every way I can find of asking if she's had a kid - MILF needed as this will register children prior to employment
	if (girl->check_virginity() || (!isPregnant && !girl->has_trait( "MILF") && girl->m_ChildrenCount[CHILD00_TOTAL_BIRTHS] < 1))
	{
		volume /= 2;											// never preg, so not producing much
		girl->lactation(g_Dice % 3);	//all this pumping etc induces lactation
	}

	///////////////////
	//let's see if there's a milker, and if so, influence this a little.
	bool milkerOnDuty = false;
	sGirl* milker = 0;
	string milkerName = "";

	vector<sGirl*> milkmaids = girls_on_job(brothel, JOB_MILKER, Day0Night1);
	if (milkmaids.size() > 0)
	{
		milkerOnDuty = true;
		int i = g_Dice % milkmaids.size();
		milker = milkmaids[i];
		milkerName = milker->m_Realname;
	}

	//O/P for info and debug
	ss << "She was milked by ";
	if (milkerOnDuty)
	{
		ss << milkerName;
		if (milker->has_trait("No Hands") || milker->has_trait("No Arms"))
		{
			ss << ", who really struggled. Why would you pick someone with no hands to be a milker?";
			volume /= 2;
		}
		else if (milker->has_trait("MILF") && g_Dice.percent(50))
		{
			ss << ", a mother, who has experience extracting milk effectively.";
			volume += (volume / 5);
		}
		else if (milker->has_trait("Sadistic") && g_Dice.percent(35))
		{
			ss << ", who seemed more interested in slapping " << girlName << "'s breasts and twisting her nipples than in actually trying to get milk out.";
			volume -= (volume / 5);
		}
		else if (milker->has_trait("Lesbian") && g_Dice.percent(40))
		{
			ss << ", who massaged " << girlName << "'s breasts thoroughly and was careful to thoroughly arouse the nipple with her tongue before attaching the cup. This helped with milking.";
			volume += (volume / 10);
			girl->upd_temp_stat(STAT_LIBIDO, 5, true);
		}
		else if (girl->has_trait( "Clumsy") && g_Dice.percent(40))
		{
			ss << ", who did a great job milking " << girlName << "'s breasts, but then tripped over the bucket, spilling quite a lot.";
			volume -= (volume / 4);
		}
		else if (milker->has_trait("Straight") && g_Dice.percent(40))
		{
			ss << ", who clearly didn't want to touch another woman's breasts. This made the milking akward and inefficient.";
			volume -= (volume / 10);
		}
		else if (milker->has_trait("Cum Addict") && g_Dice.percent(45))
		{
			ss << ", who kept compaining that she'd rather be 'milking' men.";
		}
		else ss << ".";
	}
	else ss << "some farmhand.";
	ss << "\n";

	//to ounces
	double milkProduced = toOz(volume);					//vol in oz

	//Creating new var to reflect the ease of milking her. Basing this on lactation, rather than dice roll, as seems more real
	//if they lactate a lot, the machines draining off the weight will be good
	//if not, the dry pulling on the nipples would hurt
	//
	int ease = volume;
	//
	// Nipples affects ease which is used to adjust the girls enjoyment and damage
	if (girl->has_trait( "Inverted Nipples"))				ease -= 20;
	if (girl->has_trait( "Puffy Nipples"))				ease += 40;
	if (girl->has_trait( "Perky Nipples"))				ease += 20;


	if (ease < 75)
	{
		enjoy -= 2 * (roll % 3 + 2);								// -8 to -4
		girl->health(-(roll % 6));			// 0 to 5 damage
		ss << "She's barely lactating, so this was a slow, painful process that left her with raw, ";
		if (girl->has_trait( "Missing Nipple") || girl->has_trait( "No Nipples")) ss << "aching breasts.";
		else ss << "bleeding nipples.";
	}
	else if (ease < 150)
	{
		enjoy -= 2 * (roll % 3 + 1);								// -6 to -2
		girl->health(-(roll % 3));			// 0 to 2 damage
		ss << "She's barely producing so all the squeezing, tugging and suction-cup just left her breasts raw and painful.";
	}
	else if (ease < 300)
	{
		enjoy -= roll % 3 + 1;										// -3 to -1
		ss << "It was unpleasant. She produced little milk and the suction-cup left her breasts aching.";
	}
	else if (ease < 600)
	{
		enjoy++;													// +1
		ss << "Being milked was okay for her.";
	}
	else if (ease < 1200)
	{
		enjoy += roll % 3 + 1;										// +1 to +3
		ss << "Being milked felt good today.";
	}
	else if (ease < 1600)
	{
		enjoy += 2 * (roll % 3 + 1);								// +2 to +6
		girl->happiness((roll % 3));		// 0 to 2 happiness
		ss << "Her breasts were uncomfortably full. Getting that weight off felt great.";
	}
	else
	{
		enjoy += 2 * (roll % 3 + 2);								// +4 to +8
		girl->happiness((roll % 6));		// 0 to 5 happiness
		ss << "Her breasts were so full milk was leaking through her clothes. Finally getting milked felt incredible.";
	}
	ss << "\n \n";

	//value calculations
	int milkValue = int(milkProduced * MILKWHOLESALE);		//Base value
	int traitBoost = milkValue;								// now basing the boost on base value, not on inflated CG value.

	if (girl->has_trait( "Cat Girl"))
	{
		ss << "Cat-Girl breast-milk has higher value.\n";
		milkValue *= CATGIRLBONUS;
	}

	//finally a little randomness
	if (volume > 0)   // no point mentioning this if she doesn't produce anything
	{
		if (g_Dice.percent(60) && (girl->has_trait( "Fallen Goddess") || girl->has_trait( "Goddess")))
		{
			ss << "Customers are willing to pay much more to sup from the breast of a Goddess.\n";
			milkValue += (2 * traitBoost);
		}
		else if (g_Dice.percent(40) && girl->has_trait( "Demon"))
		{
			ss << "Customers are thrilled at the chance to consume the milk of a Demon.\n";
			milkValue += (2 * traitBoost);
		}
		else if (g_Dice.percent(50) && girl->has_trait( "Queen"))
		{
			ss << "Customers are willing to pay more to enjoy the breast-milk of a Queen.\n";
			traitBoost *= (1 + g_Dice % 2);
			milkValue += traitBoost;
		}
		else if (g_Dice.percent(50) && girl->has_trait( "Princess"))
		{
			ss << "Customers are willing to pay more to enjoy the breast-milk of a Princess.\n";
			traitBoost *= (1 + g_Dice % 2);
			milkValue += traitBoost;
		}
		else if (g_Dice.percent(30) && (girl->has_trait( "Priestess")))
		{
			ss << "Customers pay more to drink the breast-milk of a religious holy-woman.\n";
			milkValue += traitBoost;
		}
		else if (g_Dice.percent(40) && (girl->fame() >= 95))
		{
			ss << "Your customers eagerly gulp down the breast-milk of such a famous and well-loved girl.\n";
			milkValue += traitBoost;
		}
		else if (girl->has_trait( "Vampire"))
		{
			ss << "Customers pay more to try the breast-milk of a Vampire. Perhaps they hope for eternal life.\n";
			milkValue += (g_Dice % 30 + 5);
		}

		if (g_Dice.percent(30) && (girl->has_trait( "Shroud Addict") || girl->has_trait( "Fairy Dust Addict") || girl->has_trait( "Viras Blood Addict")))
		{
			ss << "Her breast-milk has a strangely bitter flavour. However, customers find it quite addictive and end up paying extra for more.\n";
			milkValue += (g_Dice % 30 + 10);
		}

		if (g_Dice.percent(15) && (girl->has_trait( "Strong Magic") || girl->has_trait( "Powerful Magic")))
		{
			if ((girl->magic() > 75) && (girl->mana() > 50))
			{
				ss << "Her milk pulsates with magical healing energies. It can cure colds, heal injuries and 'improve performance.' Customers pay significantly more for it.\n";
				milkValue += traitBoost;
				girl->mana(-25); //Mana passes into milk
			}
			else
			{
				ss << "Her milk has mild magical healing properties and leaves customers feeling upbeat. Customers pay a little more for this.\n";
				milkValue += (g_Dice % 30 + 10);
			}
		}
		if (girl->has_trait( "Undead") || girl->has_trait( "Zombie"))
		{
			ss << "Customers are very reluctant to drink the milk of the undead. You can barely give the stuff away.\n";
			milkValue /= 10;
		}
	}
	//update to add options based on how good you are...
	if (g_Dice.percent(3) && (girl->has_trait( "Great Arse") || girl->has_trait( "Tight Butt") || girl->has_trait( "Phat Booty") || girl->has_trait( "Deluxe Derriere")))
	{
		extraEvent = true;
		ssextra << "\nAs you survey the milking area from the doorway, you can't help noticing " << girlName << "'s butt rising into the air from a milking stall";
		if (The_Player->disposition() < -30)  //more than a bit evil
		{
			ssextra << ". Looking closer you find her strapped to a milking bench, that butt pointed right at you while her breasts hang beneath, pumped by suction cups. "
				<< "It's too damn good an opportunity";
			if (roll % 2)
			{
				ssextra << ", you decide, as you squeeze your dick into her ass.\n";
				girl->anal(1);
				girl->bdsm(1);
				extraimage = IMGTYPE_ANAL;
			}
			else if (girl->check_virginity())
			{
				ssextra << " and you are about to enter her when you remember she is a virgin. Reluctantly, you switch and instead fuck her ass.\n";
				girl->anal(1);
				girl->bdsm(1);
				extraimage = IMGTYPE_ANAL;
			}
			else
			{
				ssextra << " so you clamp your hands around that booty and fuck her pussy hard. ";
				girl->normalsex(1);
				girl->bdsm(1);
				extraimage = IMGTYPE_SEX;

				if (!girl->calc_pregnancy(The_Player, false, 1.0))
				{
					stringstream ssm;
					ssm << girlName << " has gotten pregnant. This should help with her milk production.";
					ssextra << ssm.str();
					g_MessageQue.AddToQue(ssm.str(), 0);
				}
				ssextra << "\n";
			}
			if (HateLove <= -50)
			{
				ssextra << "She's pissed now, telling the other girls you raped her";
				if (girl->is_slave()) ssextra << ", almost like she's forgotten who owns her";
				ssextra << ".\n \n";
				girl->pchate(5);
				enjoy -= 5;
			}
			else if (HateLove <= 0)
			{
				ssextra << "She's upset you took advantage of her. She thought you were better than that.\n \n";
				girl->pchate(2);
				enjoy -= 2;
			}
			else if (HateLove <= 50)
			{
				ssextra << "She was surprised, but pleased you noticed her. She enjoyed it.\n \n";
				girl->pclove(1);
				enjoy += 2;
			}
			else
			{
				ssextra << "She loved it! It made milking much more enjoyable.\n \n";
				girl->pclove(4);
				enjoy += 4;
			}
		}
		else if (The_Player->disposition() < 40)   // not that good
		{
			ssextra << ". Looking closer, she really does have a great butt. You stop for a moment, but decide that doing anything more just wouldn't be right. "
				<< "You give her butt a gentle pat and walk away.\n \n";
			extraimage = IMGTYPE_COWGIRL;
		}
		else //you are good
		{
			ssextra << " and, right behind her, one of the delivery-boys from the market. He knows he shouldn't be in the production area. As you move closer, "
				<< "you are shocked to find him taunting and abusing " << girlName << " as he roughly shoves a finger in and out of her asshole. You watch horrified as he "
				<< "starts to take down his pants, clearly readying to rape her. She wriggles and squeals but is so tightly strapped in she is powerless to stop him.\nFurious you "
				<< "grab a heavy, wooden milk-bucket and smash it across his head, knocking him out cold. Your gang drag the unconscious wretch to your dungeon as you write a "
				<< "note to the market trader, explaining this.\n";
			switch (roll % 3)
			{
			case 0:
				ssextra << "The trader sends back 100 gold and a note apologizing, and promising this will never happen again. He says you can do what you like with the "
					<< "delivery boy, \"But if you ever release the fool, tell him he'll need a new job.\"\n";
				g_Brothels.GetDungeon()->AddCust(DUNGEON_CUSTBEATGIRL, 0, 0);
				wages += 100;
				break;
			case 1:
				ssextra << "The trader arrives soon after, begging for his son's release. Eventually you agree, adding that if his boy EVER enters your farm again "
					<< "his balls will be staying here. The trader thanks you repeatedly for your kindness and apologizes to " << girlName << ", giving her 200 extra gold for her... discomfort.\n";
				wages += 200;
				break;
			case 2:
				ssextra << "You never hear a word from the market trader.\n";
				g_Brothels.GetDungeon()->AddCust(DUNGEON_CUSTBEATGIRL, 0, 0);
				break;
			}
			if (girl->check_virginity()) ssextra << "Thanks to you, her virginity is intact so ";
			ssextra << girlName << " comes to your office after her shift";
			if ((HateLove <= 50) || girl->has_trait( "Nymphomaniac") || girl->has_trait( "Cum Addict") || girl->has_trait( "Slut"))
			{
				ssextra << ", pulls down your pants, and 'thanks'";
				extraimage = IMGTYPE_ORAL;
			}
			else
			{
				ssextra << " and earnestly thanks";
				extraimage = IMGTYPE_PROFILE;
			}
			ssextra << " you for your intervention.\n";
		}
		//Add whatever is in here onto the original message too.
		//ss << ssextra.str();
		//Ditched the above - too cumbersome.
		ss << "Something happened here today (see extra message).\n";
	}
	wages += milkValue;

	//Output
	//
	ss << "In the end, " << girlName << " produced ";
	if (milkValue <= 0)
	{
		ss << "no milk at all, earning nothing for her pains.";
	}
	else if ((int)milkProduced == 0)
	{
		ss << "a trickle of ";
		if (girl->has_trait( "Cat Girl"))	ss << "Cat-Girl ";
		ss << "breast-milk, earning just " << wages << " gold.";
	}
	else
	{
		ss << "just over " << (int)milkProduced << " ounces. This fine, freshly-squeezed ";
		if (girl->has_trait( "Cat Girl"))	ss << "Cat-Girl ";
		ss << "breast-milk earns " << wages << " gold.";
	}

#else	// `J` old job function
	int work = 0;
	int num_items = 0;
	void AddItem(sInventoryItem* item);
	sInventoryItem* GetItem(string name);

	wages += 15;

	if (girl->has_trait( "Small Boobs") || girl->has_trait( "Flat Chest") || girl->has_trait( "Petite Breasts"))
	{
		if (girl->m_States&(1 << STATUS_PREGNANT) || girl->m_States&(1 << STATUS_PREGNANT_BY_PLAYER))
		{
			ss << girlName << " has small breasts, but her body still gives plenty of milk in anticipation of nursing!.";
			wages += 125;
		}
		else
		{
			ss << girlName << " has small breasts, which only yield a small amount of milk.";
			wages += 25;
		}
	}
	else if (girl->has_trait( "Big Boobs") || girl->has_trait( "Busty Boobs") || girl->has_trait( "Giant Juggs"))
	{
		if (girl->m_States&(1 << STATUS_PREGNANT) || girl->m_States&(1 << STATUS_PREGNANT_BY_PLAYER))
		{
			ss << girlName << "'s already sizable breasts have become fat and swollen with milk in preparation for her child.";
			wages += 135;
		}
		else
		{
			ss << girlName << " has large breasts, that yield a good amount of milk to the suction machine even without pregnancy.";
			wages += 35;
		}
	}
	else if (girl->has_trait( "Abnormally Large Boobs") || girl->has_trait( "Massive Melons") || girl->has_trait( "Titanic Tits"))
	{
		if (girl->m_States&(1 << STATUS_PREGNANT) || girl->m_States&(1 << STATUS_PREGNANT_BY_PLAYER))
		{
			ss << girlName << " has ridiculously large breasts, even without a baby in development.  With a bun in the oven, her tits are each larger than her head, and leak milk near continuously.";
			wages += 140;
		}
		else
		{
			ss << girlName << "'s massive globes don't need pregnancy to yield a profitable quantity of milk!";
			wages += 40;
		}
	}
	else
	{
		if (girl->m_States&(1 << STATUS_PREGNANT) || girl->m_States&(1 << STATUS_PREGNANT_BY_PLAYER))
		{
			ss << girlName << " has average sized breasts, which yield a fair amount of milk with the help of pregnancy.";
			wages += 130;
		}
		else
		{
			ss << girlName << " has average sized breasts, perfect handfuls, which yield an okay amount of milk.";
			wages += 30;
		}
	}



	//enjoyed the work or not
	if (roll <= 5)
	{
		ss << "\nShe had a bad time letting her breasts be milked."; work -= 1;
	} //zzzzz FIXME this needs better text
	else if (roll <= 25)
	{
		ss << "\nShe had a pleasant time letting her breasts be milked."; work += 3;
	}
	else
	{
		ss << "\nOtherwise, the shift passed uneventfully."; work += 1;
	}
#endif

#if 0

	// `J` Farm Bookmark - adding in items that can be created in the farm















#endif


	girl->upd_Enjoyment(actiontype, enjoy);
	girl->m_Tips = max(0, tips);
	girl->m_Pay = max(0, wages);

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_MILK, Day0Night1);
#if 1
	//generate extra message
	if (extraEvent) girl->m_Events.AddMessage(ssextra.str(), extraimage, Day0Night1);
#endif
	// Improve stats
	int xp = 5, libido = 1, skill = 3;

	if (girl->has_trait( "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (girl->has_trait( "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (girl->has_trait( "Nymphomaniac"))			{ libido += 2; }

	girl->exp((g_Dice % xp) + 1);
	girl->upd_temp_stat(STAT_LIBIDO, libido);

	// primary
	girl->service((g_Dice % skill) + 1);

	return false;
}


double cJobManager::JP_Milk(sGirl* girl, bool estimate)// not used
{
	double jobperformance = 0.0;
	jobperformance += girl->lactation();
	/* */if (girl->has_trait( "Flat Chest"))				jobperformance += 5;
	else if (girl->has_trait( "Petite Breasts"))			jobperformance += 10;
	else if (girl->has_trait( "Small Boobs"))				jobperformance += 15;
	else if (girl->has_trait( "Busty Boobs"))				jobperformance += 25;
	else if (girl->has_trait( "Big Boobs"))				jobperformance += 30;
	else if (girl->has_trait( "Giant Juggs"))				jobperformance += 35;
	else if (girl->has_trait( "Massive Melons"))			jobperformance += 40;
	else if (girl->has_trait( "Abnormally Large Boobs"))	jobperformance += 45;
	else if (girl->has_trait( "Titanic Tits"))			jobperformance += 50;
	else /*                                               */	jobperformance += 20;

	/* */if (girl->has_trait( "Dry Milk"))				jobperformance /= 5;
	else if (girl->has_trait( "Scarce Lactation"))		jobperformance /= 2;
	else if (girl->has_trait( "Abundant Lactation"))		jobperformance *= 1.5;
	else if (girl->has_trait( "Cow Tits"))				jobperformance *= 2;

	if (girl->is_pregnant()) jobperformance *= 2;

	return jobperformance;
}
