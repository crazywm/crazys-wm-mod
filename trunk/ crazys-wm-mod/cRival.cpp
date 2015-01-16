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

extern cMessageQue g_MessageQue;
extern cBrothelManager g_Brothels;
extern CLog g_LogFile;
extern cRng g_Dice;
extern cGold g_Gold;
extern cGangManager g_Gangs;
extern cInventory g_InvManager;


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
	ss << gettext("\nThey get away with ") << gold << gettext(" gold.");	// format a message and store it in the string that was passed to us
	return ss.str();
}

void cRivalManager::Update(int& NumPlayerBussiness)
{
	cRival* curr = m_Rivals;
	cConfig cfg;

	if (g_Year >= 1209 && g_Month > 3) m_PlayerSafe = false;

	while (curr)
	{
		int income = 0; int upkeep = 0;
		curr->m_Age++;	// `J` added - age rival

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

		// sell or use items
		if (curr->m_NumInventory > 0)
		{
			for (int i = 0; i < MAXNUM_RIVAL_INVENTORY; i++)
			{
				sInventoryItem* temp = curr->m_Inventory[i];
				if (temp)
				{
					switch (g_Dice % 4)
					{
					case 0:		// use
						RemoveInvByNumber(curr, i);
						break;
					case 1:		// sell
						curr->m_Gold += (temp->m_Cost / 2);
						income += (temp->m_Cost / 2);
						RemoveInvByNumber(curr, i);
						break;
					default:	// keep
						break;
					}
				}
			}
		}





		// check if a rival is in danger
		if (curr->m_Gold <= 0 || curr->m_NumBrothels <= 0 ||
			curr->m_NumGirls <= 0 || curr->m_NumGamblingHalls <= 0 || curr->m_NumBars <= 0)
		{
			// The AI is in danger so will stop extra spending
			curr->m_BribeRate = 0;
			curr->m_Influence = 0;

			// try to buy at least one of each to make up for losses
			if (curr->m_NumBrothels <= 0 && curr->m_Gold - 20000 >= 0)
			{
				curr->m_Gold -= 20000;
				curr->m_NumBrothels++;
			}
			if (curr->m_NumGirls <= 0 && curr->m_Gold - 550 >= 0)
			{
				curr->m_Gold -= 550;
				curr->m_NumGirls++;
			}
			if (curr->m_NumGamblingHalls <= 0 && curr->m_Gold - 10000 >= 0)
			{
				curr->m_NumGamblingHalls++;
				curr->m_Gold -= 10000;
			}
			if (curr->m_NumBars <= 0 && curr->m_Gold - 2500 >= 0)
			{
				curr->m_NumBars++;
				curr->m_Gold -= 2500;
			}
			// buy more girls if there is enough money left (save at least 1000 in reserve)
			if (curr->m_Gold >= 1550 && (curr->m_NumGirls < 5 || curr->m_NumGirls < curr->m_NumBrothels * 20))
			{
				int i = 0;
				while (curr->m_Gold >= 1550 && i < (g_Dice % 5) + 1)	// buy up to 5 girls if they can afford it.
				{
					curr->m_Gold -= 550;
					curr->m_NumGirls++;
					i++;
				}
			}

		}

		// Get their income and upkeep

		// from girls
		for (int i = 0; i < curr->m_NumGirls; i++)
		{
			// If a rival has more girls than their brothels can handle, the rest work on the streets
			double rapechance = (i > curr->m_NumBrothels * 20 ? cfg.prostitution.rape_brothel() : cfg.prostitution.rape_streets());
			int girlcost = g_Dice % 41 + 10;				// 10-50 gold per cust
			int Customers = g_Dice % 6;						// 0-5 cust per girl
			if (Customers > 0 && g_Dice.percent(rapechance))
			{
				Customers--;								// they killed the rapist
				upkeep += girlcost + 100;					// and paid off the girl and the officials to keep it quiet
			}
			income += (girlcost * Customers) + (g_Dice % (girlcost * 2) - girlcost);
		}
		// from halls
		for (int i = 0; i < curr->m_NumGamblingHalls; i++)
		{
			int Customers = ((g_Dice%curr->m_NumGirls) + curr->m_NumGirls + 1)*((g_Dice % 2) + 1);
			if (g_Dice.percent(5))
			{
				upkeep += ((g_Dice % 101) + 200);			// Big Winner
				Customers += g_Dice % 10;					// attracts more customers
			}
			if (g_Dice.percent(5))
			{
				income += ((g_Dice % 601) + 400);			// Big Loser
				Customers -= g_Dice % (Customers / 5);		// scares off some customers
			}
			// they will kick a customer out if they win too much so they can win up to 400 but only lose 50
			for (int j = 0; j < Customers; j++)
			{
				int winloss = (g_Dice % 451 - 50);
				if (winloss > 0) income += winloss;
				else /*       */ upkeep += winloss;
			}
		}
		// from bars
		for (int i = 0; i < curr->m_NumBars; i++)
		{
			int Customers = ((g_Dice%curr->m_NumGirls) + curr->m_NumGirls + 1)*((g_Dice % 2) + 1);
			if (g_Dice.percent(5))
			{
				upkeep += ((g_Dice % 250) + 1);				// bar fight - cost to repair
				Customers -= g_Dice % (Customers / 5);		// scare off some customers
			}
			if (g_Dice.percent(5))
			{
				income += ((g_Dice % 250) + 1);				// Big Spender
				Customers += g_Dice % 5;					// attracts more customers
			}
			for (int j = 0; j < Customers; j++)
			{
				income += (g_Dice % 38) + 3;				// customers spend 3-40 per visit
			}

		}
		// from businesses
		if (curr->m_BusinessesExtort > 0) income += (curr->m_BusinessesExtort * INCOME_BUSINESS);

		// Calc their upkeep
		upkeep += curr->m_BribeRate;
		upkeep += curr->m_NumGirls * 5;
		upkeep += curr->m_NumBars * 20;
		upkeep += curr->m_NumGamblingHalls * 80;
		upkeep += (curr->m_NumBars)*((g_Dice % 100) + 30);	// upkeep for buying barrels of booze
		upkeep += (curr->m_NumGangs * 90);

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
			upkeep += tax;
		}

		curr->m_Gold += income;
		curr->m_Gold -= upkeep;

		// Determine if their profits
		int profit = income - upkeep;

		bool danger = false;
		bool sellfail = false;
		// if they are loosing money and they will be bankrupt in 2 turns or less
		if (profit <= 0 && curr->m_Gold - (profit * 2) < 0)		// sell off some stuff
		{
			danger = true;						// this will make sure AI doesn't replace them this turn
			while (curr->m_Gold - (profit * 2) < 0 && !sellfail)
			{
				// sell extra stuff first - hall or bar
				if (curr->m_NumGamblingHalls > curr->m_NumBrothels)
				{
					curr->m_NumGamblingHalls--;
					curr->m_Gold += 5000;
				}
				else if (curr->m_NumBars > curr->m_NumBrothels)
				{
					curr->m_NumBars--;
					curr->m_Gold += 1250;
				}
				// if they have an empty brothel, sell it
				else if (curr->m_NumBrothels > 1 && (curr->m_NumBrothels - 1) * 20 > curr->m_NumGirls + 1)
				{
					curr->m_NumBrothels--;
					curr->m_Gold += 10000;
				}
				// sell extra girls
				else if (curr->m_NumGirls > curr->m_NumBrothels*20)
				{
					curr->m_NumGirls--;
					curr->m_Gold += g_Dice% 401 +300;	// variable price 300-700
				}
				// sell a hall or bar keeping at least 1 of each
				else if (curr->m_NumGamblingHalls > 1 && curr->m_NumBars <= curr->m_NumGamblingHalls)
				{
					curr->m_NumGamblingHalls--;
					curr->m_Gold += 5000;
				}
				else if (curr->m_NumBars > 1)
				{
					curr->m_NumBars--;
					curr->m_Gold += 1250;
				}
				// Finally - sell a girl
				else if (curr->m_NumGirls > 1)
				{
					curr->m_NumGirls--;
					curr->m_Gold += g_Dice % 401 + 300;	// variable price 300-700
				}
				else
				{
					sellfail = true;	// could not sell anything so break out of the while loop
				}
			}
		}

		if (!danger)
		{
			// buy a new brothel
			if (curr->m_Gold - 20000 > 0 && curr->m_NumGirls + 2 >= curr->m_NumBrothels * 20 && curr->m_NumBrothels < 6)
			{
				curr->m_NumBrothels++;
				curr->m_Gold -= 20000;
				upkeep += 20000;
			}
			// buy new girls
			int girlsavailable = (g_Dice % 6) + 1;
			while (curr->m_Gold - 550 >= 0 && girlsavailable > 0 && curr->m_NumGirls < curr->m_NumBrothels * 20)
			{
				curr->m_NumGirls++;
				girlsavailable--;
				curr->m_Gold -= 550;
				upkeep += 550;
			}
			// hire gangs
			int gangsavailable = (g_Dice % 4) + 1;
			while (curr->m_Gold - 90 >= 0 && gangsavailable > 0 && curr->m_NumGangs < 8)
			{
				curr->m_NumGangs++;
				curr->m_Gold -= 90;
				upkeep += 90;
			}
			// buy a gambling hall 
			if (g_Dice.percent(30) && curr->m_Gold - 10000 >= 0 && curr->m_NumGamblingHalls < curr->m_NumBrothels)
			{
				curr->m_NumGamblingHalls++;
				curr->m_Gold -= 10000;
				upkeep += 10000;
			}
			// buy a new bar
			if (g_Dice.percent(60) && curr->m_Gold - 2500 >= 0 && curr->m_NumBars < curr->m_NumBrothels)
			{
				curr->m_NumBars++;
				curr->m_Gold -= 2500;
				upkeep += 2500;
			}
		}

		// Work out gang missions
		int cGangs = curr->m_NumGangs;
		for (int i = 0; i < cGangs; i++)
		{
			sGang* cG1 = g_Gangs.GetTempGang(curr->m_Age);	// create a random gang for this rival
			int missionid = -1;
			int tries = 0;
			while (missionid == -1 && tries < 10)	// choose a mission
			{
				switch (g_Dice % 7)	// `J` zzzzzz - need to add checks into this
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
				case 6:		default:
					missionid = MISS_CATACOMBS;		// random but dangerous
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
						int extraIncome = n * 20;
						curr->m_Gold += extraIncome;
						income += extraIncome;
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
								ss << gettext("Your guards encounter ") << curr->m_Name << gettext(" going after some of your territory.");

								sGang* rGang = g_Gangs.GetTempGang(curr->m_Age);
								if (g_Gangs.GangBrawl(miss1, rGang))	// if you win
								{
									if (rGang->m_Num == 0) curr->m_NumGangs--;
									ss << gettext("\nBut you maintain control of the territory.");
									miss1->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);
								}
								else									// if you lose
								{
									if (miss1->m_Num == 0) g_Gangs.RemoveGang(miss1);
									ss << gettext("\nYou lose the territory.");
									NumPlayerBussiness--;
									curr->m_BusinessesExtort++;
									g_MessageQue.AddToQue(ss.str(), COLOR_RED);
								}
								delete rGang; rGang = 0;	// cleanup
							}
							else										// if you do not have a gang guarding
							{
								ss << gettext("Your rival ") << curr->m_Name << gettext(" has taken one of the undefended territories you control.");
								g_MessageQue.AddToQue(ss.str(), COLOR_RED);
								NumPlayerBussiness--;
								curr->m_BusinessesExtort++;
							}
						}
					}
					else	// attack another rival
					{
						ss << gettext("The ") << curr->m_Name << gettext(" attacked the territories of ");
						cRival* rival = GetRival(who);
						if (rival != curr && rival->m_BusinessesExtort > 0)
						{
							ss << rival->m_Name;
							if (rival->m_NumGangs > 0)
							{
								sGang* rG1 = g_Gangs.GetTempGang(rival->m_Age);
								if (g_Gangs.GangBrawl(cG1, rG1))
								{
									rival->m_NumGangs--;
									rival->m_BusinessesExtort--;
									curr->m_BusinessesExtort++;
									ss << gettext(" and won.");
								}
								else
								{
									curr->m_NumGangs--;
									ss << gettext(" and lost.");
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
					int extraIncome = g_Dice % 800 + 1;
					curr->m_Gold += extraIncome;
					income += extraIncome;
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
					int extraIncome = (g_Dice % 20 + 1) * 100;
					curr->m_Gold += extraIncome;
					income += extraIncome;
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
							ss << gettext("Your rival the ") << curr->m_Name << gettext(" attack your assets.");

							if (!g_Gangs.GangBrawl(miss1, cG1))
							{
								if (miss1->m_Num == 0) g_Gangs.RemoveGang(miss1);
								ss << gettext("\nYour men are defeated.");
								int num = (g_Dice % 2) + 1;
								damage = true;
							}
							else
							{
								if (cG1->m_Num == 0) curr->m_NumGangs--;
								ss << gettext(" But they fail.");
								miss1->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);
							}
						}
						else
						{
							ss << gettext("You have no guards so your rival ") << curr->m_Name << gettext(" attacks.");
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
						ss << gettext("The ") << curr->m_Name << gettext(" launched an assault on ");
						cRival* rival = GetRival(who);
						if (rival && rival != curr)
						{
							int num = 0;
							bool damage = false;
							ss << rival->m_Name;
							if (rival->m_NumGangs > 0)
							{
								sGang* rG1 = g_Gangs.GetTempGang(rival->m_Age);
								if (g_Gangs.GangBrawl(cG1, rG1))
								{
									rival->m_NumGangs--;
									ss << gettext(" and won.");
									damage = true;
									num = (g_Dice % 2) + 1;
								}
								else
								{
									ss << gettext(" and lost.");
									curr->m_NumGangs--;
								}
								delete rG1; rG1 = 0;	// cleanup
							}
							else
							{
								damage = true;
								num = (g_Dice % 3) + 1;
							}
							if (damage)
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
									curr->m_Gold += gold;
								}
								int buildinghit = g_Dice.d100();
								if (buildinghit < 10 && rival->m_NumBrothels > 0)			// 10% brothel
								{
									rival->m_NumBrothels--;
									ss << "\nThey destroyed one of their Brothels.";
								}
								else if (buildinghit < 30 && rival->m_NumGamblingHalls > 0)	// 20% hall
								{
									rival->m_NumGamblingHalls--;
									ss << "\nThey destroyed one of their Gambling Halls.";
								}
								else if (buildinghit < 60 && rival->m_NumBars > 0)			// 30% bar
								{
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
				if (g_Dice.percent(35))			// chance to find a girl
				{
					bool addgirl = false;
					sGirl* girl = g_Girls.GetRandomGirl();
					if (girl)
					{
						if (g_Dice.percent(cG1->m_Stats[STAT_CHARISMA]))	// convince her
						{
							addgirl = true;
						}
						else if (g_Brothels.FightsBack(girl))				// try to kidnap her
						{
							if (!g_Gangs.GangCombat(girl, cG1)) addgirl = true;
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
					curr->m_Gold += gold;
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
							AddInv(curr, temp);
							curr->m_NumInventory++;
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

		profit = income - upkeep;

//		// handicap super profits
//		if (income - upkeep > 10000)
//		{
//			income = 0;
//			curr->m_Gold -= 10000;
//		}

		// adjust their bribe rate		
		if (profit > 1000)		curr->m_BribeRate += (long)(50);	// if doing well financially then increase 
		else if (profit < 0)	curr->m_BribeRate -= (long)(50);	// if loosing money decrease
		if (curr->m_BribeRate < 0) curr->m_BribeRate = 0;			// check 0
		g_Brothels.UpdateBribeInfluence();							// update influence

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
		pRival->SetAttribute("BribeRate", current->m_BribeRate);
		pRival->SetAttribute("BusinessesExtort", current->m_BusinessesExtort);
		pRival->SetAttribute("Gold", current->m_Gold);
		pRival->SetAttribute("NumBars", current->m_NumBars);
		pRival->SetAttribute("NumBrothels", current->m_NumBrothels);
		pRival->SetAttribute("NumGamblingHalls", current->m_NumGamblingHalls);
		pRival->SetAttribute("NumGirls", current->m_NumGirls);
		pRival->SetAttribute("NumGangs", current->m_NumGangs);
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

			pRival->QueryValueAttribute<long>("BribeRate", &current->m_BribeRate);
			pRival->QueryIntAttribute("BusinessesExtort", &current->m_BusinessesExtort);
			pRival->QueryValueAttribute<long>("Gold", &current->m_Gold);
			pRival->QueryIntAttribute("NumBars", &current->m_NumBars);
			pRival->QueryIntAttribute("NumBrothels", &current->m_NumBrothels);
			pRival->QueryIntAttribute("NumGamblingHalls", &current->m_NumGamblingHalls);
			pRival->QueryIntAttribute("NumGirls", &current->m_NumGirls);
			pRival->QueryIntAttribute("NumGangs", &current->m_NumGangs);

			message = "loaded rival: ";
			message += current->m_Name;
			g_LogFile.write(message);

			AddRival(current);
		}
	}
	return true;
}

void cRivalManager::CreateRival(long bribeRate, int extort, long gold, int bars, int gambHalls, int Girls, int brothels, int gangs, int age)
{
	ifstream in;
	cRival* rival = new cRival();

	DirPath first_names = DirPath() << "Resources" << "Data" << "RivalGangFirstNames.txt";
	DirPath last_names = DirPath() << "Resources" << "Data" << "RivalGangLastNames.txt";

	rival->m_Age = age;						// `J` added
	rival->m_Gold = gold;
	rival->m_NumBrothels = brothels;
	rival->m_NumGirls = Girls;
	rival->m_NumGangs = gangs;
	rival->m_BribeRate = bribeRate;
	rival->m_BusinessesExtort = extort;
	rival->m_NumBars = bars;
	rival->m_NumGamblingHalls = gambHalls;

	for (;;)
	{
		rival->m_Name = names.random();
		if (!NameExists(rival->m_Name)) break;
	}
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

	rival->m_Age = 0;
	rival->m_Gold = (g_Dice % 20000) + 5000;
	rival->m_NumBrothels = (g_Dice % 3) + 1;
	rival->m_NumGirls = 0;
	while (rival->m_NumGirls == 0)
		rival->m_NumGirls = (g_Dice % ((rival->m_NumBrothels) * 20)) + 20;
	rival->m_NumGangs = g_Dice % 5+3;

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



int cRivalManager::AddInv(cRival* rival, sInventoryItem* item)
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

bool cRivalManager::RemoveInvByNumber(cRival* rival, int num)
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

void cRivalManager::SellInvItem(cRival* rival, int num)
{
	if (rival->m_Inventory[num] != 0)
	{
		rival->m_Gold += (int)((float)rival->m_Inventory[num]->m_Cost*0.5f);
		rival->m_NumInventory--;
		rival->m_Inventory[num] = 0;
	}
}

sInventoryItem* cRivalManager::GetItem(cRival* rival, int num)
{
	sInventoryItem *ipt;
	ipt = rival->m_Inventory[num];
	return ipt;
}

sInventoryItem* cRivalManager::GetRandomItem(cRival* rival)
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

int cRivalManager::GetRandomItemNum(cRival* rival)
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
