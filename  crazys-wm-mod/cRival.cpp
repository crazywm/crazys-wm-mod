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
#include "cRival.h"
#include <stdlib.h>
#include <fstream>
#include <sstream>

#ifdef LINUX
#include "linux.h"
#endif

using namespace std;
#include "cBrothel.h"
#include "cMessageBox.h"
#include "cGold.h"
#include "GameFlags.h"
#include "DirPath.h"
#include "cGangs.h"
#include "libintl.h"
#include "cInventory.h"

extern cMessageQue g_MessageQue;
extern cBrothelManager g_Brothels;
extern CLog g_LogFile;
extern cRng g_Dice;
extern cGold g_Gold;
extern cGangManager g_Gangs;
extern cInventory g_InvManager;
extern cPlayer* The_Player;

extern unsigned long g_Year;
extern unsigned long g_Month;
extern unsigned long g_Day;

cRivalManager::cRivalManager()
{
	m_Rivals = 0;
	m_NumRivals = 0;
	m_Last = 0;
	m_PlayerSafe = true;

	DirPath first_names = DirPath() << "Resources" << "Data" << "RivalGangFirstNames.txt";
	DirPath last_names = DirPath() << "Resources" << "Data" << "RivalGangLastNames.txt";
	names.load(first_names, last_names);
}

static inline int max(int a, int b) { return((a > b) ? a : b); }

string cRivalManager::rivals_plunder_pc_gold(cRival* rival)
{
	if (g_Gold.ival() <= 0) return "";						// no gold to sieze? nothing to do.
	long pc_gold = g_Gold.ival();							// work out how much they take. make a note of how much we have

	long gold = g_Dice.random(min((long)2000, pc_gold));
	if (gold < 45) gold = 45;								// make sure there's at least 45 gold taken
	if (pc_gold < gold) gold = pc_gold;						// unless the pc has less than that, in which case take the lot
	g_Gold.rival_raids(gold);								// deduct the losses against rival raid losses
	rival->m_Gold += gold;									// add the aount to rival coffers

	stringstream ss;
	ss << "\nThey get away with " << gold << " gold.";	// format a message and store it in the string that was passed to us
	return ss.str();
}

void cRivalManager::Update(int& NumPlayerBussiness)
{
	cRival* curr = m_Rivals;


	if (g_Year >= 1209 && g_Month > 3) m_PlayerSafe = false;

	while (curr)
	{
		// check if rival is killed
		if (curr->m_Gold <= 0 && curr->m_NumBrothels <= 0 && curr->m_NumGangs <= 0 &&
			curr->m_NumGirls <= 0 && curr->m_NumGamblingHalls <= 0 && curr->m_NumBars <= 0 &&
			curr->m_NumInventory <= 0)
		{
			cRival* tmp = curr->m_Next;
			RemoveRival(curr);
			curr = tmp;
			SetGameFlag(FLAG_RIVALLOSE);
			continue;
		}

		int income = 0; int upkeep = 0; int profit = 0;
		int totalincome = 0; int totalupkeep = 0;
		int startinggold = curr->m_Gold;

		// `J` added - rival power
		// `J` reworked to reduce the rival's power
		curr->m_Power =
			max(0, curr->m_NumBrothels * 5) +
			max(0, curr->m_NumGamblingHalls * 2) +
			max(0, curr->m_NumBars * 1);

		// check if a rival is in danger
		if (curr->m_Gold <= 0 || curr->m_NumBrothels <= 0 || curr->m_NumGirls <= 0 || curr->m_NumGamblingHalls <= 0 || curr->m_NumBars <= 0)
		{
			// The AI is in danger so will stop extra spending
			curr->m_BribeRate = 0;
			curr->m_Influence = 0;

			// first try to sell any items
			if (curr->m_NumInventory > 0)
			{
				for (int i = 0; i < MAXNUM_RIVAL_INVENTORY && curr->m_Gold + income + upkeep < 1000; i++)
				{
					sInventoryItem* temp = curr->m_Inventory[i];
					if (temp)
					{
						income += (temp->m_Cost / 2);
						RemoveRivalInvByNumber(curr, i);
					}
				}
			}

			// try to buy at least one of each to make up for losses
			if (curr->m_NumBrothels <= 0 && curr->m_Gold + income + upkeep - 20000 >= 0)
			{
				upkeep -= 20000;
				curr->m_NumBrothels++;
			}
			if (curr->m_NumGirls <= 0 && curr->m_Gold + income + upkeep - 550 >= 0)
			{
				upkeep -= 550;
				curr->m_NumGirls++;
			}
			if (curr->m_NumGamblingHalls <= 0 && curr->m_Gold + income + upkeep - 10000 >= 0)
			{
				curr->m_NumGamblingHalls++;
				upkeep -= 10000;
			}
			if (curr->m_NumBars <= 0 && curr->m_Gold + income + upkeep - 2500 >= 0)
			{
				curr->m_NumBars++;
				upkeep -= 2500;
			}
			// buy more girls if there is enough money left (save at least 1000 in reserve)
			if (curr->m_Gold + income + upkeep >= 1550 && (curr->m_NumGirls < 5 || curr->m_NumGirls < curr->m_NumBrothels * 20))
			{
				int i = 0;
				while (curr->m_Gold + income + upkeep >= 1550 && i < (g_Dice % 5) + 1)	// buy up to 5 girls if they can afford it.
				{
					upkeep -= 550;
					curr->m_NumGirls++;
					i++;
				}
			}
		}

		// process money
		totalincome += income; totalupkeep += upkeep; curr->m_Gold += income; curr->m_Gold += upkeep; profit = totalincome + totalupkeep;
		income = upkeep = 0;

		for (int i = 0; i < curr->m_NumGirls; i++)	// from girls
		{
			// If a rival has more girls than their brothels can handle, the rest work on the streets
			double rapechance = (i > curr->m_NumBrothels * 20 ? cfg.prostitution.rape_brothel() : cfg.prostitution.rape_streets());
			int Customers = g_Dice % 6;				// 0-5 cust per girl
			for (int i = 0; i < Customers;i++)
			{
				if (g_Dice.percent(rapechance))
				{
					upkeep -= 50;					// pay off the girl and the officials after killing the rapist
				}
				else
				{
					income += g_Dice % 38 + 2;		// 2-40 gold per cust
				}
			}
		}
		// from halls
		for (int i = 0; i < curr->m_NumGamblingHalls; i++)
		{
			int Customers = ((g_Dice%curr->m_NumGirls) + curr->m_NumGirls / 5);
			if (g_Dice.percent(5))
			{
				upkeep -= ((g_Dice % 101) + 200);			// Big Winner
				Customers += g_Dice % 10;					// attracts more customers
			}
			if (g_Dice.percent(5))
			{
				income += ((g_Dice % 601) + 400);			// Big Loser
				Customers -= g_Dice % (Customers / 5);		// scares off some customers
			}
			// they will kick a customer out if they win too much so they can win up to 100 but only lose 50
			for (int j = 0; j < Customers; j++)
			{
				int winloss = (g_Dice % 151 - 50);
				if (winloss > 0) income += winloss;
				else /*       */ upkeep += winloss;
			}
		}
		// from bars
		for (int i = 0; i < curr->m_NumBars; i++)
		{
			int Customers = ((g_Dice%curr->m_NumGirls) + curr->m_NumGirls/5);
			if (g_Dice.percent(5))
			{
				upkeep -= ((g_Dice % 250) + 1);				// bar fight - cost to repair
				Customers -= g_Dice % (Customers / 5);		// scare off some customers
			}
			if (g_Dice.percent(5))
			{
				income += ((g_Dice % 250) + 1);				// Big Spender
				Customers += g_Dice % 5;					// attracts more customers
			}
			for (int j = 0; j < Customers; j++)
			{
				income += (g_Dice % 9) + 1;				// customers spend 1-10 per visit
			}

		}
		// from businesses
		if (curr->m_BusinessesExtort > 0) income += (curr->m_BusinessesExtort * INCOME_BUSINESS);

		// Calc their upkeep
		upkeep -= curr->m_BribeRate;
		upkeep -= curr->m_NumGirls * 5;
		upkeep -= curr->m_NumBars * 20;
		upkeep -= curr->m_NumGamblingHalls * 80;
		upkeep -= (curr->m_NumBars)*((g_Dice % 50) + 30);	// upkeep for buying barrels of booze
		upkeep -= (curr->m_NumGangs * 90);

		float taxRate = 0.06f;	// normal tax rate is 6%
		if (curr->m_Influence > 0)	// can you influence it lower
		{
			int lowerBy = curr->m_Influence / 20;
			float amount = (float)(lowerBy / 100);
			taxRate -= amount;
			if (taxRate <= 0.01f) taxRate = 0.01f;
		}
		if (income > 0)
		{
			int tmp = income - (g_Dice % (int)(income*0.25f));	// launder up to 25% of gold
			int tax = (int)(tmp*taxRate);
			upkeep -= tax;
		}

		// process money
		totalincome += income; totalupkeep += upkeep; curr->m_Gold += income; curr->m_Gold += upkeep; profit = totalincome + totalupkeep;
		income = upkeep = 0;

		// Work out gang missions
		int cGangs = curr->m_NumGangs;
		for (int i = 0; i < cGangs; i++)
		{
			sGang* cG1 = g_Gangs.GetTempGang(curr->m_Power);	// create a random gang for this rival
			int missionid = -1;
			int tries = 0;
			while (missionid == -1 && tries < 10)	// choose a mission
			{
				switch (g_Dice % 9)	// `J` zzzzzz - need to add checks into this
				{
				case 0:
					missionid = MISS_EXTORTION;		// gain territory
					break;
				case 1:
					missionid = MISS_PETYTHEFT;		// small money but safer
					break;
				case 2:
					missionid = MISS_GRANDTHEFT;	// large money but difficult
					break;
				case 3:
					missionid = MISS_SABOTAGE;		// attack rivals
					break;
				case 4:
					break;	// not ready
					missionid = MISS_CAPTUREGIRL;	// take girls from rivals
					break;
				case 5:
					missionid = MISS_KIDNAPP;		// get new girls
					break;
				case 6:
					missionid = MISS_CATACOMBS;		// random but dangerous
					break;
				default:
					missionid = MISS_GUARDING;		// don't do anything but guard
					break;
				}
				tries++;
			}


			switch (missionid)
			{
			case MISS_EXTORTION:		// gain territory
			{
				int numB = GetNumBusinesses() + NumPlayerBussiness;
				if (numB < TOWN_NUMBUSINESSES)	// if there are uncontrolled businesses
				{
					int n = g_Dice % 5 - 2;
					if (n > 0)					// try to take some
					{
						if (numB + n > TOWN_NUMBUSINESSES)
							n = TOWN_NUMBUSINESSES - numB;

						curr->m_BusinessesExtort += n;
						income += n * 20;
					}
				}
				else			// if there are no uncontrolled businesses
				{
					stringstream ss;
					int who = (g_Dice % (m_NumRivals + 1));				// who to attack
					if (who == m_NumRivals)								// try to attack you
					{
						if (!player_safe() && NumPlayerBussiness > 0)	// but only if you are a valid target
						{
							sGang* miss1 = g_Gangs.GetGangOnMission(MISS_GUARDING);
							if (miss1)									// if you have a gang guarding
							{
								ss << "Your guards encounter " << curr->m_Name << (" going after some of your territory.");

								sGang* rGang = g_Gangs.GetTempGang(curr->m_Power);
								if (g_Gangs.GangBrawl(miss1, rGang))	// if you win
								{
									if (rGang->m_Num == 0) curr->m_NumGangs--;
									ss << ("\nBut you maintain control of the territory.");
									miss1->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);
								}
								else									// if you lose
								{
									if (miss1->m_Num == 0) g_Gangs.RemoveGang(miss1);
									ss << ("\nYou lose the territory.");
									NumPlayerBussiness--;
									curr->m_BusinessesExtort++;
									g_MessageQue.AddToQue(ss.str(), COLOR_RED);
								}
								delete rGang; rGang = 0;	// cleanup
							}
							else										// if you do not have a gang guarding
							{
								ss << ("Your rival ") << curr->m_Name << (" has taken one of the undefended territories you control.");
								g_MessageQue.AddToQue(ss.str(), COLOR_RED);
								NumPlayerBussiness--;
								curr->m_BusinessesExtort++;
							}
						}
					}
					else	// attack another rival
					{
						ss << ("The ") << curr->m_Name << (" attacked the territories of ");
						cRival* rival = GetRival(who);
						if (rival != curr && rival->m_BusinessesExtort > 0)
						{
							ss << rival->m_Name;
							if (rival->m_NumGangs > 0)
							{
								sGang* rG1 = g_Gangs.GetTempGang(rival->m_Power);
								if (g_Gangs.GangBrawl(cG1, rG1, true))
								{
									rival->m_NumGangs--;
									rival->m_BusinessesExtort--;
									curr->m_BusinessesExtort++;
									ss << (" and won.");
								}
								else
								{
									curr->m_NumGangs--;
									ss << (" and lost.");
								}
								delete rG1; rG1 = 0;	// cleanup
							}
							else
							{
								ss << " and took an unguarded territory.";
								rival->m_BusinessesExtort--;
								curr->m_BusinessesExtort++;
							}
							g_MessageQue.AddToQue(ss.str(), COLOR_BLUE);
						}
					}
				}
			}break;
			case MISS_PETYTHEFT:		// small money but safer
			{
				if (g_Dice.percent(70))
				{
					income += g_Dice % 400 + 1;
				}
				else if (g_Dice.percent(10))	// they may lose the gang
				{
					curr->m_NumGangs--;
				}
			}break;
			case MISS_GRANDTHEFT:		// large money but difficult
			{
				if (g_Dice.percent(30))
				{
					income += (g_Dice % 20 + 1) * 100;
				}
				else if (g_Dice.percent(30))	// they may lose the gang
				{
					curr->m_NumGangs--;
				}
			}break;
			case MISS_SABOTAGE:			// attack rivals
			{
				if (g_Dice.percent(min(90, cG1->intelligence())))	// chance they find a target
				{
					stringstream ss;
					int who = (g_Dice % (m_NumRivals + 1));
					if (who == m_NumRivals && !player_safe())	// if it is you and you are a valid target
					{
						int num = 0;
						bool damage = false;
						sGang* miss1 = g_Gangs.GetGangOnMission(MISS_GUARDING);
						if (miss1)
						{
							ss << ("Your rival the ") << curr->m_Name << (" attack your assets.");

							if (!g_Gangs.GangBrawl(miss1, cG1))
							{
								if (miss1->m_Num == 0) g_Gangs.RemoveGang(miss1);
								ss << ("\nYour men are defeated.");
								int num = (g_Dice % 2) + 1;
								damage = true;
							}
							else
							{
								if (cG1->m_Num == 0) curr->m_NumGangs--;
								ss << (" But they fail.");
								miss1->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);
							}
						}
						else
						{
							ss << ("You have no guards so your rival ") << curr->m_Name << (" attacks.");
							if (NumPlayerBussiness > 0 || g_Gold.ival() > 0)
							{
								num = (g_Dice % 3) + 1;
								damage = true;
							}
						}
						if (damage)
						{
							if (NumPlayerBussiness > 0)
							{
								ss << "\nThey destroy ";
								NumPlayerBussiness -= num;
								if (NumPlayerBussiness < 0)
								{
									ss << "all";
									NumPlayerBussiness = 0;
								}
								else if (num == 1)	ss << "one";
								else if (num == 2)	ss << "two";
								else /*         */	ss << num;
								ss << " of your territories.";
							}
							else ss << ".";

							ss << rivals_plunder_pc_gold(curr);
							g_MessageQue.AddToQue(ss.str(), COLOR_RED);
						}
					}
					else
					{
						ss << ("The ") << curr->m_Name << (" launched an assault on ");
						cRival* rival = GetRival(who);
						if (rival && rival != curr)
						{
							int num = 0;
							ss << rival->m_Name;
							if (rival->m_NumGangs > 0)
							{
								sGang* rG1 = g_Gangs.GetTempGang(rival->m_Power);
								if (g_Gangs.GangBrawl(cG1, rG1, true))
								{
									rival->m_NumGangs--;
									ss << (" and won.");
									num = (g_Dice % 2) + 1;
								}
								else
								{
									ss << (" and lost.");
									curr->m_NumGangs--;
								}
								delete rG1; rG1 = 0;	// cleanup
							}
							else
							{
								num = (g_Dice % 4) + 1;	// can do more damage if not fighting another gang
							}
							if (num > 0)
							{
								if (rival->m_BusinessesExtort > 0)
								{
									rival->m_BusinessesExtort -= num;
									if (rival->m_BusinessesExtort < 0)
										rival->m_BusinessesExtort = 0;
								}
								if (rival->m_Gold > 0)
								{
									long gold = (g_Dice % 2000) + 45;	// get a random ammount
									if ((rival->m_Gold - gold) > 0)		// and if they have more than that
									{
										rival->m_Gold -= gold;			// take it
									}
									else								// but if not
									{
										gold = rival->m_Gold;			// take all they have
										rival->m_Gold = 0;
									}
									income += gold;
								}
								int buildinghit = g_Dice.d100() - num;
								if (rival->m_NumBrothels > 0 && buildinghit < 10 + (rival->m_NumBrothels * 2))
								{		// 10% base + 2% per brothel
									rival->m_NumBrothels--;
									rival->m_Power--;
									ss << "\nThey destroyed one of their Brothels.";
								}
								else if (rival->m_NumGamblingHalls > 0 && buildinghit < 30 + (rival->m_NumGamblingHalls * 2))
								{		// 20% base + 2% per hall
									rival->m_NumGamblingHalls--;
									ss << "\nThey destroyed one of their Gambling Halls.";
								}
								else if (rival->m_NumBars > 0 && buildinghit < 60 + (rival->m_NumBars * 2))
								{		// 60% base + 2% per bar
									rival->m_NumBars--;
									ss << "\nThey destroyed one of their Bars.";
								}
							}
							g_MessageQue.AddToQue(ss.str(), 0);
						}
					}
				}
			}break;
			case MISS_CAPTUREGIRL:		// take girls from rivals
			{





			}break;
			case MISS_KIDNAPP:			// get new girls
			{
				if (g_Dice.percent(cG1->intelligence()))			// chance to find a girl
				{
					bool addgirl = false;
					sGirl* girl = g_Girls.GetRandomGirl();
					g_Girls.SetStat(girl, STAT_HEALTH, 100);		// make sure she is at full health
					if (girl)
					{
						if (g_Dice.percent(cG1->m_Stats[STAT_CHARISMA]))	// convince her
						{
							addgirl = true;
						}
						else if (g_Brothels.FightsBack(girl))				// try to kidnap her
						{
							if (!g_Gangs.GirlVsEnemyGang(girl, cG1)) addgirl = true;
							else if (cG1->m_Num <= 0) curr->m_NumGangs--;
						}
						else { addgirl = true; }							// she goes willingly
					}
					if (addgirl) curr->m_NumGirls++;
				}
			}break;
			case MISS_CATACOMBS:		// random but dangerous
			{
				int num = cG1->m_Num;
				for (int i = 0; i < num; i++)
				{
					if (!g_Dice.percent(cG1->combat())) cG1->m_Num--;
				}
				if (cG1->m_Num > 0)
				{
					// determine loot
					int gold = cG1->m_Num;
					gold += g_Dice % (cG1->m_Num * 100);
					income += gold;

					int items = 0;
					while (g_Dice.percent(60) && items <= (cG1->m_Num / 3) && curr->m_NumInventory < MAXNUM_RIVAL_INVENTORY)
					{
						bool quit = false; bool add = false;
						sInventoryItem* temp;
						do { temp = g_InvManager.GetRandomItem();
						} while (!temp || temp->m_Rarity < RARITYSHOP25 || temp->m_Rarity > RARITYCATACOMB01);

						switch (temp->m_Rarity)
						{
						case RARITYSHOP25:								add = true;		break;
						case RARITYSHOP05:		if (g_Dice.percent(25))	add = true;		break;
						case RARITYCATACOMB15:	if (g_Dice.percent(15))	add = true;		break;
						case RARITYCATACOMB05:	if (g_Dice.percent(5))	add = true;		break;
						case RARITYCATACOMB01:	if (g_Dice.percent(1))	add = true;		break;
							// adding these cases to shut the compiler up
						case RARITYCOMMON:	case RARITYSHOP50:	case RARITYSCRIPTONLY:	case RARITYSCRIPTORREWARD:
						default:
							break;
						}
						if (add)
						{
							AddRivalInv(curr, temp);
						}
					}

					int girls = 0;
					while (g_Dice.percent(40) && girls <= 4)	// up to 4 girls
					{
						girls++;
						curr->m_NumGirls++;
					}
				}
			}break;
			default:	break;			// No mission
			}	// end mission switch
			delete cG1; cG1 = 0;	// cleanup
		}	// end Gang Missions

		// process money
		totalincome += income; totalupkeep += upkeep; curr->m_Gold += income; curr->m_Gold += upkeep; profit = totalincome + totalupkeep;
		income = upkeep = 0;

		bool danger = false;
		bool sellfail = false;
		// if they are loosing money and they will be bankrupt in 2 turns or less
		if (profit <= 0 && curr->m_Gold - (profit * 2) < 0)		// sell off some stuff
		{
			danger = true;						// this will make sure AI doesn't replace them this turn
			while (curr->m_Gold + income + upkeep - (profit * 2) < 0 && !sellfail)
			{
				// first try to sell any items
				if (curr->m_NumInventory > 0)
				{
					for (int i = 0; i < MAXNUM_RIVAL_INVENTORY && curr->m_Gold + income + upkeep - (profit * 2) < 0; i++)
					{
						sInventoryItem* temp = curr->m_Inventory[i];
						if (temp)
						{
							income += (temp->m_Cost / 2);
							RemoveRivalInvByNumber(curr, i);
						}
					}
				}

				// sell extra stuff - hall or bar
				if (curr->m_NumGamblingHalls > curr->m_NumBrothels)
				{
					curr->m_NumGamblingHalls--;
					income += 5000;
				}
				else if (curr->m_NumBars > curr->m_NumBrothels)
				{
					curr->m_NumBars--;
					income += 1250;
				}
				// if they have an empty brothel, sell it
				else if (curr->m_NumBrothels > 1 && (curr->m_NumBrothels - 1) * 20 > curr->m_NumGirls + 1)
				{
					curr->m_NumBrothels--;
					income += 10000;
				}
				// sell extra girls
				else if (curr->m_NumGirls > curr->m_NumBrothels * 20)
				{
					curr->m_NumGirls--;
					income += g_Dice % 401 + 300;	// variable price 300-700
				}
				// sell a hall or bar keeping at least 1 of each
				else if (curr->m_NumGamblingHalls > 1 && curr->m_NumBars <= curr->m_NumGamblingHalls)
				{
					curr->m_NumGamblingHalls--;
					income += 5000;
				}
				else if (curr->m_NumBars > 1)
				{
					curr->m_NumBars--;
					income += 1250;
				}
				// Finally - sell a girl
				else if (curr->m_NumGirls > 1)
				{
					curr->m_NumGirls--;
					income += g_Dice % 401 + 300;	// variable price 300-700
				}
				else
				{
					sellfail = true;	// could not sell anything so break out of the while loop
				}
			}
		}

		// process money
		totalincome += income; totalupkeep += upkeep; curr->m_Gold += income; curr->m_Gold += upkeep; profit = totalincome + totalupkeep;
		income = upkeep = 0;

		if (!danger)
		{
			// use or sell items
			if (curr->m_NumInventory > 0)
			{
				for (int i = 0; i < MAXNUM_RIVAL_INVENTORY; i++)
				{
					sInventoryItem* temp = curr->m_Inventory[i];
					if (temp && g_Dice.percent(50))
					{
						if (g_Dice.percent(50)) income += (temp->m_Cost / 2);
						RemoveRivalInvByNumber(curr, i);
					}
				}
			}

			// buy a new brothel if they have enough money
			if (curr->m_Gold + income + upkeep - 20000 > 0 && curr->m_NumGirls + 2 >= curr->m_NumBrothels * 20 && curr->m_NumBrothels < 6)
			{
				curr->m_NumBrothels++;
				upkeep -= 20000;
			}
			// buy new girls
			int girlsavailable = (g_Dice % 6) + 1;
			while (curr->m_Gold + income + upkeep - 550 >= 0 && girlsavailable > 0 && curr->m_NumGirls < curr->m_NumBrothels * 20)
			{
				curr->m_NumGirls++;
				girlsavailable--;
				upkeep -= 550;
			}
			// hire gangs
			int gangsavailable = (max(0, (g_Dice % 5) - 2));
			while (curr->m_Gold + income + upkeep - 90 >= 0 && gangsavailable > 0 && curr->m_NumGangs < 8)
			{
				curr->m_NumGangs++;
				upkeep -= 90;
			}
			// buy a gambling hall
			if (g_Dice.percent(30) && curr->m_Gold + income + upkeep - 10000 >= 0 && curr->m_NumGamblingHalls < curr->m_NumBrothels)
			{
				curr->m_NumGamblingHalls++;
				upkeep -= 10000;
			}
			// buy a new bar
			if (g_Dice.percent(60) && curr->m_Gold + income + upkeep - 2500 >= 0 && curr->m_NumBars < curr->m_NumBrothels)
			{
				curr->m_NumBars++;
				upkeep -= 2500;
			}

			// buy items
			int rper[7] = { 90, 70, 50, 30, 10, 5, 1 };
			int i = 0;
			while (i < 6)
			{
				sInventoryItem* item = g_InvManager.GetRandomItem();
				if (item && item->m_Rarity <= RARITYCATACOMB01 && g_Dice.percent(rper[item->m_Rarity])
					&& curr->m_Gold + income + upkeep > item->m_Cost)
				{
					if (g_Dice.percent(50))
					{
						AddRivalInv(curr, item);	// buy 50%, use 50%
					}
					upkeep -= item->m_Cost;
				}
				i++;

			}
		}

		// process money
		totalincome += income; totalupkeep += upkeep; curr->m_Gold += income; curr->m_Gold += upkeep; profit = totalincome + totalupkeep;
		income = upkeep = 0;

		// adjust their bribe rate
		if (profit > 1000)		curr->m_BribeRate += (long)(50);	// if doing well financially then increase
		else if (profit < 0)	curr->m_BribeRate -= (long)(50);	// if loosing money decrease
		if (curr->m_BribeRate < 0) curr->m_BribeRate = 0;			// check 0
		g_Brothels.UpdateBribeInfluence();							// update influence


		// `J` bookmark - rival money at the end of their turn
		if (cfg.debug.log_debug())
		g_LogFile.os() << "Processing Rival: " << curr->m_Name
			<< " | Starting Gold: " << startinggold
			<< " | Income: " << totalincome
			<< " | Upkeep: " << totalupkeep
			<< " | Profit: " << totalincome + totalupkeep
			<< " | Ending Gold: " << curr->m_Gold <<"\n";

		curr = curr->m_Next;
	}
}

cRival* cRivalManager::GetRandomRival()
{
	if (m_NumRivals == 0) return 0;
	if (m_NumRivals == 1) return m_Rivals;

	int number = g_Dice%m_NumRivals;
	cRival* current = m_Rivals;
	int tmp = 0;
	while (current)
	{
		if (tmp == number) break;
		tmp++;
		current = current->m_Next;
	}

	return current;
}

// check a random rival for gangs
cRival* cRivalManager::GetRandomRivalWithGangs()
{
	if (m_NumRivals == 0) return 0;
	if (m_NumRivals == 1) return m_Rivals;

	cRival* current = m_Rivals;
	int tries = m_NumRivals*5;
	while (tries > 0)
	{
		tries--;
		int number = g_Dice%m_NumRivals;
		current = m_Rivals;
		int tmp = 0;
		while (current)
		{
			if (tmp == number)
			{
				if (current->m_NumGangs > 0) return current;
				else break;
			}
			tmp++;
			current = current->m_Next;
		}
	}

	// do one last check of all rivals if the random check failed
	current = m_Rivals;
	while (current)
	{
		if (current->m_NumGangs > 0) return current;
		current = current->m_Next;
	}


	return 0;
}

// check a random rival for gangs
cRival* cRivalManager::GetRandomRivalWithBusinesses()
{
	if (m_NumRivals == 0) return 0;
	if (m_NumRivals == 1) return m_Rivals;

	cRival* current = m_Rivals;
	int tries = m_NumRivals * 5;
	while (tries > 0)
	{
		tries--;
		int number = g_Dice%m_NumRivals;
		current = m_Rivals;
		int tmp = 0;
		while (current)
		{
			if (tmp == number)
			{
				if (current->m_BusinessesExtort > 0) return current;
				else break;
			}
			tmp++;
			current = current->m_Next;
		}
	}

	// do one last check of all rivals if the random check failed
	current = m_Rivals;
	while (current)
	{
		if (current->m_BusinessesExtort > 0) return current;
		current = current->m_Next;
	}
	return 0;
}

// `J` added - hit whoever has the most brothels
cRival* cRivalManager::GetRandomRivalToSabotage()
{
	if (m_NumRivals == 0) return 0;
	if (m_NumRivals == 1) return m_Rivals;

	cRival* current = m_Rivals;
	cRival* temp = current;
	while (current)
	{
		if (temp->m_NumBrothels < current->m_NumBrothels) temp = current;
		current = current->m_Next;
	}

	return temp;
}

// how many businesses are controlled by rivals
int cRivalManager::GetNumBusinesses()
{
	int number = 0;

	cRival* current = m_Rivals;
	while (current)
	{
		number += current->m_BusinessesExtort;
		current = current->m_Next;
	}

	return number;
}

int cRivalManager::GetNumRivalGangs()
{
	int gangs = 0;
	cRival* current = m_Rivals;
	while (current)
	{
		gangs+= current->m_NumGangs;
		current = current->m_Next;
	}
	return gangs;
}

cRival* cRivalManager::GetRival(string name)
{
	cRival* current = m_Rivals;
	while (current)
	{
		if (current->m_Name == name) return current;
		current = current->m_Next;
	}
	return 0;
}

// this will return the most influential rival or null if there were no rivals with influence
cRival* cRivalManager::get_influential_rival()
{
	cRival* current;
	cRival* top = 0;
	for (current = m_Rivals; current; current = current->m_Next)
	{
		// if the rival has no influence, skip on
		if (current->m_Influence <= 0) continue;
		// If we don't have a candidate yet, anyone with influence will do.
		// And since we already weeded out the influence-less rivals at this point...
		if (top == 0) { top = current; continue; }
		// is the current rival more influential than the the one we have our eye on?
		if (current->m_Influence < top->m_Influence) continue;
		top = current;
	}
	return top;
}

cRival* cRivalManager::GetRival(int number)
{
	cRival* current = m_Rivals;
	int tmp = 0;
	while (current)
	{
		if (tmp == number) return current;
		tmp++;
		current = current->m_Next;
	}

	return 0;
}

TiXmlElement* cRivalManager::SaveRivalsXML(TiXmlElement* pRoot)
{
	TiXmlElement* pRivalManager = new TiXmlElement("Rival_Manager");
	pRoot->LinkEndChild(pRivalManager);
	TiXmlElement* pRivals = new TiXmlElement("Rivals");
	pRivalManager->LinkEndChild(pRivals);

	string message = "";
	cRival* current = m_Rivals;
	while (current)
	{
		message = "saving rival: ";
		message += current->m_Name;
		g_LogFile.write(message);

		TiXmlElement* pRival = new TiXmlElement("Rival");
		pRivals->LinkEndChild(pRival);
		pRival->SetAttribute("Name", current->m_Name);
		pRival->SetAttribute("Power", current->m_Power);
		pRival->SetAttribute("Gold", current->m_Gold);
		pRival->SetAttribute("NumGirls", current->m_NumGirls);
		pRival->SetAttribute("NumBrothels", current->m_NumBrothels);
		pRival->SetAttribute("NumGamblingHalls", current->m_NumGamblingHalls);
		pRival->SetAttribute("NumBars", current->m_NumBars);
		pRival->SetAttribute("NumGangs", current->m_NumGangs);
		pRival->SetAttribute("BribeRate", current->m_BribeRate);
		pRival->SetAttribute("BusinessesExtort", current->m_BusinessesExtort);
		current = current->m_Next;
	}
	return pRivalManager;
}

bool cRivalManager::LoadRivalsXML(TiXmlHandle hRivalManager)
{
	Free();		// everything should be init even if we failed to load an XML element
	TiXmlElement* pRivalManager = hRivalManager.ToElement();
	if (pRivalManager == 0) return false;

	string message = "";
	m_NumRivals = 0;
	TiXmlElement* pRivals = pRivalManager->FirstChildElement("Rivals");
	if (pRivals)
	{
		for (TiXmlElement* pRival = pRivals->FirstChildElement("Rival"); pRival != 0; pRival = pRival->NextSiblingElement("Rival"))
		{
			cRival* current = new cRival();

			if (pRival->Attribute("Name"))
			{
				current->m_Name = pRival->Attribute("Name");
			}
			pRival->QueryIntAttribute("Power", &current->m_Power);
			pRival->QueryValueAttribute<long>("BribeRate", &current->m_BribeRate);
			pRival->QueryIntAttribute("BusinessesExtort", &current->m_BusinessesExtort);
			pRival->QueryValueAttribute<long>("Gold", &current->m_Gold);
			pRival->QueryIntAttribute("NumBars", &current->m_NumBars);
			pRival->QueryIntAttribute("NumBrothels", &current->m_NumBrothels);
			pRival->QueryIntAttribute("NumGamblingHalls", &current->m_NumGamblingHalls);
			pRival->QueryIntAttribute("NumGirls", &current->m_NumGirls);
			pRival->QueryIntAttribute("NumGangs", &current->m_NumGangs);

			// `J` cleanup rival power for .06.01.17
			if (current->m_Power > 50) current->m_Power = max(0, current->m_NumBrothels * 5) + max(0, current->m_NumGamblingHalls * 2) + max(0, current->m_NumBars * 1);

                                                    //jim: re-initializing rival inventory to zero (hopefully fixes Linux segfaults)
                                                    current->m_NumInventory = 0;
                                                    for(int i = 0; i <MAXNUM_RIVAL_INVENTORY; i++)
                                                    {
                                                                      current->m_Inventory[i] = 0;
                                                    }
                        
			message = "loaded rival: ";
			message += current->m_Name;
			g_LogFile.write(message);

			AddRival(current);
		}
	}
	return true;
}

void cRivalManager::CreateRival(long bribeRate, int extort, long gold, int bars, int gambHalls, int Girls, int brothels, int gangs, int power)
{
	ifstream in;


	cRival* rival = new cRival();

	DirPath first_names = DirPath() << "Resources" << "Data" << "RivalGangFirstNames.txt";
	DirPath last_names = DirPath() << "Resources" << "Data" << "RivalGangLastNames.txt";

	rival->m_Gold = gold;
	rival->m_NumBrothels = brothels;
	rival->m_NumGirls = Girls;
	rival->m_NumGangs = gangs;
	rival->m_BribeRate = bribeRate;
	rival->m_BusinessesExtort = extort;
	rival->m_NumBars = bars;
	rival->m_NumGamblingHalls = gambHalls;

	// `J` added - rival power
	// `J` reworked to reduce the rival's power
	rival->m_Power = max(power,
		max(0, rival->m_NumBrothels * 5) +
		max(0, rival->m_NumGamblingHalls * 2) +
		max(0, rival->m_NumBars * 1));
        
                //jim: initializing rival inventory to zero (hopefully fixes Linux segfaults)
                  rival->m_NumInventory = 0;
                  for(int i = 0; i <MAXNUM_RIVAL_INVENTORY; i++)
                  {
                                    rival->m_Inventory[i] = 0;
                  }



	for (;;)
	{
		rival->m_Name = names.random();
		if (!NameExists(rival->m_Name)) break;
	}
	if (cfg.debug.log_debug())
	g_LogFile.os() << "Creating New Rival: " << rival->m_Name
		<< "     | Power: " << rival->m_Power
		<< "     | Gold : " << rival->m_Gold
		<< "     | Brthl: " << rival->m_NumBrothels
		<< "     | Girls: " << rival->m_NumGirls
		<< "     | Gangs: " << rival->m_NumGangs
		<< "     | Bribe: " << rival->m_BribeRate
		<< "     | Busns: " << rival->m_BusinessesExtort
		<< "     | Bars : " << rival->m_NumBars
		<< "     | Halls: " << rival->m_NumGamblingHalls
		<< "\n";
	AddRival(rival);
}

bool cRivalManager::NameExists(string name)
{
	cRival* current = m_Rivals;
	while (current)
	{
		if (current->m_Name == name) return true;
		current = current->m_Next;
	}
	return false;
}

void cRivalManager::CreateRandomRival()
{
	ifstream in;
	cRival* rival = new cRival();

	DirPath first_names = DirPath() << "Resources" << "Data" << "RivalGangFirstNames.txt";
	DirPath last_names = DirPath() << "Resources" << "Data" << "RivalGangLastNames.txt";

	rival->m_Gold = (g_Dice % 20000) + 5000;
	rival->m_NumBrothels = (g_Dice % 3) + 1;
	rival->m_Power = rival->m_NumBrothels * 4;	// starts out a little less powerful

	rival->m_NumGirls = 0;
	while (rival->m_NumGirls == 0)
		rival->m_NumGirls = (g_Dice % ((rival->m_NumBrothels) * 20)) + 20;
	rival->m_NumGangs = g_Dice % 5+3;
        
                   //jim: initializing rival inventory to zero (hopefully fixes Linux segfaults)
                  rival->m_NumInventory = 0;
                  for(int i = 0; i <MAXNUM_RIVAL_INVENTORY; i++)
                  {
                                    rival->m_Inventory[i] = 0;
                  }

	for (;;) {
		rival->m_Name = names.random();
		if (!NameExists(rival->m_Name)) break;
	}
	AddRival(rival);
}

void cRivalManager::AddRival(cRival* rival)
{
	if (m_Last)
	{
		m_Last->m_Next = rival;
		rival->m_Prev = m_Last;
		m_Last = rival;
	}
	else
		m_Rivals = m_Last = rival;
	m_NumRivals++;
}

void cRivalManager::RemoveRival(cRival* rival)
{
	if (rival->m_Next)		rival->m_Next->m_Prev = rival->m_Prev;
	if (rival->m_Prev)		rival->m_Prev->m_Next = rival->m_Next;
	if (rival == m_Rivals)	m_Rivals = rival->m_Next;
	if (rival == m_Last)	m_Last = rival->m_Prev;
	rival->m_Prev = rival->m_Next = 0;
	delete rival;
	rival = 0;
	m_NumRivals--;
}

// `J` moved from cBrothel
void cRivalManager::check_rivals()
{
	int num_rivals = GetNumRivals();
	static bool peace = false;
	if (num_rivals > 5) return;					// a full set of rivals = nothing to do
	if (num_rivals == 0 && !peace)				// if there are no rivals, and we were not at peace last turn, peace has broken out
	{
		peace = true;
		peace_breaks_out();
	}
	// we only create new rivals after the game has been won
	// `J` added a chance for a new rival before the game is won
	if (The_Player->m_WinGame == false && g_Dice.percent(100 - num_rivals)) return;
	if (g_Dice.percent(70)) return;				// create new random rival or not!
	peace = false;								// flag the war as on again, (should be a field somewhere)
	CreateRandomRival();				// create a new rival and tell the player the good news
	g_MessageQue.AddToQue(new_rival_text(), COLOR_RED);
}

// `J` moved from cBrothel
string cRivalManager::new_rival_text()
{
	stringstream ss;

	enum {
		Slaver = 0,
		Gladiator = 1,
		Goon = 2,
		Slave = 3,
		Mage = 4,
		Demon = 5,
		Priest = 6,
		Noble = 7,
		Technologist = 8,
		Patriarch = 9,	// or Matriarch
		MaxChallengers = 10
	};

	bool male = g_Dice.percent(75);
	/*
	*	let's put the gender specific terms in
	*	variables. Might make the code cleaner
	*/
	string man, boy, He, he, him, his, sorcerer, gladiator, fellow, patriarch;
	if (male)
	{
		He = "He";
		he = "he";
		him = "him";
		his = "his";
		man = "man";
		boy = "boy";
		sorcerer = "sorcerer";
		gladiator = "gladiator";
		fellow = "fellow";
		patriarch = "patriarch ";
	}
	else
	{
		He = "She";
		he = "she";
		him = "her";
		his = "her";
		man = "woman";
		boy = "girl";
		sorcerer = "sorceress";
		gladiator = "gladiatrix";
		fellow = "wench";			// not sure what the feminine of "fellow" is I did wonder about "fellatrix"...
		patriarch = "matriarch ";
	}

	switch (g_Dice.random(MaxChallengers))
	{
	case Slaver:
		ss << "A lieutenant reports that one of the professional slavers, finding customers be scarce, has taken to whoring out " << his << " slavegirls to make ends meet. Your men arranged a meet with " << him << " in order to explain your position on the subject, but the discussion did not go well, ending with bared steel and threats of blood.\n \nIt would seem you have a challenger.";
		break;
	case Gladiator:
		ss << "Ask any Crossgate sports fan who rules the Arenas of the city. Almost always, the answer will be the same. For five long years one " << gladiator << " has stood " << his << " ground on the bloody sands and defied all who came before " << him << ".\n \nLast week, the " << gladiator << " bought " << his << " freedom from the arena, and chose to celebrate the occasion at one of your brothels. Sadly, an overindulgence in wine led to harsh words and a rash vow to show you how a whorehouse SHOULD be run.\n \nWith anyone else, the matter would have ended when the morning brought sobriety. But this is a " << man << " who has never turned " << his << " back on any sort of challenge. With wealthy admirers supplying premises and finance, and with a handful of arena veterans to provide the core of " << his << " enforcers, this is a challenger you would be foolish to ignore.";
		break;
	case Goon:
		ss << "The " << boy << " was just skin and bones; a dull eyed waif from gutters of Sleaze Street, a dozen like " << him << " on any street corner. But put a knife in " << his << " hands and the " << boy << " became an artist, painting effortless masterpieces in blood and greased lightning.\n \nQuickly recruited into one of the goon squads, it soon became apparent that behind that flat unblinking stare, there lurked a mind almost as keen as " << his << " blades. The " << boy << " rose quickly, coming to head " << his << " own squad before becoming one of your trusted lieutenants. If only " << his << " ambition had stopped there...\n \n" << "" << ((male) ? "His" : "Her") << " challenge was almost over before it began; for you that is. That you still live says more about the skill of your healers than any talent you might lay claim to. Your newest rival is not only a deadly fighter and a clever strategist, but one who knows your operation, inside and out.\n \nThis will not be easy.";
		break;
	case Slave:
		ss << "There are ways to beat a slaver tattoo. It wouldn't do were that to become widely known, of course. Nevertheless there are ways around it. One such is to find an area of unstable spacetime. Do it right, and you can overload the tracking spell, and the enchantment just falls apart. This is, of course wildly dangerous, but many escapees nevertheless head straight for the Crossgate sewers, which on a bad day can give the catacombs a run for their money.\n \nOver time, a community of ecapees has grown up in the sewers, survivor types, grown hardy in the most hostile environment. And as long as they stay down there, no one much minds. If nothing else they keep the monster population down. But now they seem to be organising a crusade. Against slavery. Against exploitation. Against you.\n \nRumour has it that their leader is one of your offspring, conceived of rape, born into slavery. True or not, this new factions seems determined to bring about your downfall.\n \nThis time, as the bards would say, it is personal.";
		break;
	case Mage:
		ss << "The " << sorcerer << " blew into town with a travelling entertainer show, promising exotic pleasures and the taste of forbidden fruit. But behind the showman's patter and the coloured smoke, the pleasures on offer were of a distinctly carnal nature, and no more exotic than those you yourself could offer.\n \nFor a travelling show, this need not be a problem. For a week, or even two, you can stand to see a little competition. However, the newcomer has been here a month now and shows no sign of moving on. On the contrary, he appears to be shopping for permanent premises.\n \nWith this in mind, you send some men to explain the situation. To everyone's surprise, it turns out that behind the glib charlatanry, there lies genuine magecraft, most likely tantric in nature.\n \nIn your organisation you have no shortage of mages. Any fighting force in Crossgate needs a battle mage or two. This newcomer however operates on a level far beyond what you are used to. And he seems determined to stay, and challenge you for control of the city.";
		break;
	case Priest:
		break;
	case Noble:
		ss << "They say " << he << " is a noble, an exile from " << his << " native land. Certainly, " << he << " has the manners of a courtier and the amused weariness of the jaded dilettante.\n \nAnd yet it seems there is steel behind the foppery, as many a Crossgate duelist has learned. And a wit to match the blade as well. An admirable " << fellow << " this, one you would be pleased to call 'friend', if only ...\n \nEarlier this week, your men were explaining to a handful of freelance scrubbers how prostitution worked in this city. If only " << he << " had not chosen to take the women's side against your men. If only " << his << " rash defiance had not caught the imagination of the city's duellists.\n \nAlas, such was not to be.\n \nEn Garde!";
		break;
	case Technologist:
		ss << "From the distant city of Abby's Crossing comes a new rival to challenge for your throne, wielding some strange non-magic " << he << " calls 'technology', an alien art of smoke and steam and noise and lighting; one they say functions strangely in Mundiga, when it chooses to work at all.\n \nBut the hollow metal men that make up " << his << " enforcers would seem to work with deadly efficicency and the strange collapsible maze " << he << " calls a 'tesseract' seems to share many properties with the catacombs under your headquarters. Then there are rumours of strange procedures that can break a slavegirl's will, far faster than the most skilled of Crossgate's torturers.\n \nIn short, far from unreliable, " << his << " arts seem deadly efficient to you. You have no idea what other surprises this otherworldly artisan may have up " << his << " sleeve, but one thing is for certain: this challenge may not go unanswered.";
		break;
	case Patriarch:
		ss << "Outside the walls of Crossgate, there is a shanty-town maze of tumbledown hovels, teeming with the poorest and most desperate of the City's inhabitants. Polygamy and incest are rife here, and extended families can run into the hundreds\n \nOne such family is ruled by the iron will of a dreadful old " << patriarch << " with a well earned reputation for utter ruthlessness. For years " << he << " has sent " << his << " progeny to the city markets, to trade, to steal, to bring back money for the clan in any way they can.\n \nNow it seems they are expanding their operation to include organised prostitution. Bad move.\n \nSomething about the " << patriarch << "'s operation disturbs you. There is a coldness in the way " << he << " sends sons and grandsons out to die for " << him << "; the way " << he << " casually rapes and enslaves " << his << " own daughters and granddaughters before sending them off to whore for " << him << ". This " << man << " holds up a mirror to what you are - or perhaps to what you could easily become. The image it presents is far from flattering.\n \nPersonal feelings aside, this is a situation that can only get worse. The time to end this, is now.";
		break;
	case Demon:
		ss << "Somewhere in Crossgate, a hand trembled inscribing a pentagram; a tongue stumbled over the nine syllables of the charm of binding. A magical being slipped his arcane bonds and slaughtered those mages foolish enough to dream they might command it.\n \nA demon lord now stalks the streets of the city.\n \nWhich, in itself, is not so big a deal. It is not of unheard that the aristocracy of Hell should find themselves stumbling dazed and confused through Crossgate market. They just tend to recover quickly and promptly open a portal home.\n \nBut not this one. This one chooses to briefly linger, to partake of Crossgate society and seek such amusements as the city can offer. Unfortunately, it seems the demon finds amusement trafficking in human misery and human sex. As do you, in the eyes of many.\n \nFor a demon, 'briefly' may be anything from a day to a thousand years. You cannot afford to wait until it grows bored. A demon lord is a formidable opponent, but to ignore this challenge will send entirely the wrong signal to the other would be whore-masters in the city.\n \nLike it or not, this means war.";
		break;
	}
	return ss.str();
}

// `J` moved from cBrothel
void cRivalManager::peace_breaks_out()
{
	stringstream ss;
	// if the PC already won, this is just an minor outbreak of peace in the day-to-day feuding in crossgate
	if (The_Player->m_WinGame)
	{
		ss << "The last of your challengers has been overthrown. Your domination of Crossgate is absolute.\n \nUntil the next time that is...";
		g_MessageQue.AddToQue(ss.str(), COLOR_GREEN);
		return;
	}
	// otherwise, the player has just won flag it as such
	The_Player->m_WinGame = true;
	// let's have a bit of chat to mark the event
	ss.str("");
	ss << "The last of your father's killers has been brought before you for judgement. None remain who would dare to oppose you. For all intents and purposes, the city is yours.\n \nWhether or not your father will rest easier for your efforts, you cannot say, but now, with the city at your feet, you feel sure he would be proud of you at this moment.\n \nBut pride comes before a fall, and in Crossgate, complacency kills. The city's slums and slave markets and the fighting pits are full of hungry young bloods burning to make their mark on the world, and any one of them could rise to challenge you at any time.\n \nYou may have seized the city, but holding on to it is never going to be easy.";
	g_MessageQue.AddToQue(ss.str(), COLOR_GREEN);
	return;
}

int cRivalManager::AddRivalInv(cRival* rival, sInventoryItem* item)
{
	int i;
	for (i = 0; i < MAXNUM_RIVAL_INVENTORY; i++)
	{
		if (rival->m_Inventory[i] == 0)
		{
			rival->m_Inventory[i] = item;
			rival->m_NumInventory++;
			return i;  // MYR: return i for success, -1 for failure
		}
	}
	return -1;
}

bool cRivalManager::RemoveRivalInvByNumber(cRival* rival, int num)
{
	// rivals inventories don't stack items
	if (rival->m_Inventory[num] != 0)
	{
		rival->m_Inventory[num] = 0;
		rival->m_NumInventory--;
		return true;
	}
	return false;
}

void cRivalManager::SellRivalInvItem(cRival* rival, int num)
{
	if (rival->m_Inventory[num] != 0)
	{
		rival->m_Gold += (int)((float)rival->m_Inventory[num]->m_Cost*0.5f);
		rival->m_NumInventory--;
		rival->m_Inventory[num] = 0;
	}
}

sInventoryItem* cRivalManager::GetRivalItem(cRival* rival, int num)
{
	sInventoryItem *ipt;
	ipt = rival->m_Inventory[num];
	return ipt;
}

sInventoryItem* cRivalManager::GetRandomRivalItem(cRival* rival)
{
	sInventoryItem *ipt;
	if (rival->m_NumInventory <= 0) return 0;
	int start = g_Dice%MAXNUM_RIVAL_INVENTORY;

	for (int i = 0; i < MAXNUM_RIVAL_INVENTORY; i++)
	{
		ipt = rival->m_Inventory[i];
		if (!ipt)
		{
			start++;
			if (start>MAXNUM_RIVAL_INVENTORY) start = 0;
		}
		else
		{
			return ipt;
		}
	}
	return 0;
}

int cRivalManager::GetRandomRivalItemNum(cRival* rival)
{
	sInventoryItem *ipt;
	if (rival->m_NumInventory <= 0) return -1;
	int start = g_Dice%MAXNUM_RIVAL_INVENTORY;

	for (int i = 0; i < MAXNUM_RIVAL_INVENTORY; i++)
	{
		ipt = rival->m_Inventory[i];
		if (!ipt)
		{
			start++;
			if (start>MAXNUM_RIVAL_INVENTORY) start = 0;
		}
		else
		{
			return i;
		}
	}
	return -1;
}
