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

extern unsigned long g_Year;
extern unsigned long g_Month;
extern unsigned long g_Day;

cRivalManager::cRivalManager()
{
	m_Rivals=0;
	m_NumRivals=0;
	m_Last=0;
	m_PlayerSafe = true;


	DirPath first_names = DirPath() << "Resources" << "Data" << "RivalGangFirstNames.txt";
	DirPath last_names = DirPath() << "Resources" << "Data" << "RivalGangLastNames.txt";
	names.load(first_names, last_names);
}

static inline int max(int a, int b) { return((a > b) ? a : b); }

void cRivalManager::rivals_plunder_pc_gold(cRival* rival, string& message)
{
/*
 *	no gold to sieze? nothing to do.
 */
	if(g_Gold.ival() <= 0) {
		return;
	}
/*
 *	work out how much they take.
 *	make a note of how much we have
 */
	long pc_gold = g_Gold.ival();
	long gold = g_Dice.random(min((long)2000, pc_gold));
/*
 *	make sure there's at least 45 gold taken
 *	unless the pc has less than that, in which case
 *	take the lot
 */
	if(gold < 45) gold = 45;
	if(pc_gold < gold) gold = pc_gold;
/*
 *	deduct the losses against rival raid losses
 *	add the aount to rival coffers
 */
	g_Gold.rival_raids(gold);
	rival->m_Gold += gold;
/*
 *	format a message and store it in the string 
 *	that was passed to us
 */
	stringstream ss;
	ss << gettext("They also take ") << gold << gettext(" gold.");
	message += ss.str();
}

void cRivalManager::Update(int& NumPlayerBussiness)
{
	int income = 0, upkeep = 0;
	cRival* curr = m_Rivals;

	if(g_Year >= 1209 && g_Month > 6)
		m_PlayerSafe = false;

	while(curr)
	{
		// check if rival is killed
		if(curr->m_NumBrothels == 0 || curr->m_NumGirls == 0 || curr->m_Gold == 0)
		{
			// set stuff that should be 0 to 0
			curr->m_NumGirls = 0;
			curr->m_NumBars = 0;
			curr->m_NumGamblingHalls = 0;

			// The AI is in danger so will stop extra spending
			curr->m_BribeRate = 0;
			curr->m_Influence = 0;
			
			// Attempt to purchace a new brothel
			if(curr->m_Gold-20000 > 0)
			{
				curr->m_Gold -= 20000;

				// attempt to buy new girls
				while(1)
				{
					if(curr->m_Gold-550 < 0) {
						break;
					}

					curr->m_NumGirls++;
					curr->m_Gold -= 550;

					if(curr->m_NumGirls < 20) {
						continue;
					}
					if(curr->m_Gold-2500 >= 0)
					{
						curr->m_NumBars++;
						curr->m_Gold -= 2500;
					}

					if(curr->m_Gold-15000 >= 0)
					{
						curr->m_NumGamblingHalls++;
						curr->m_Gold -= 15000;
					}
					break;
				}
			}
			
			if(curr->m_NumBrothels == 0 || curr->m_NumGirls == 0 || curr->m_Gold == 0)
			{
				// now this rival is truly dead
				cRival* tmp = curr->m_Next;
				RemoveRival(curr);
				curr = tmp;
				SetGameFlag(FLAG_RIVALLOSE);
				continue;
			}
		}

		// check numbers of girls, gambling halls, and bars are correct for the number of brothels
		int MaxNumGirls = (20*curr->m_NumBrothels);
		if(curr->m_NumGirls > MaxNumGirls)
			curr->m_NumGirls = MaxNumGirls;
		if(curr->m_NumBars > curr->m_NumBrothels)
			curr->m_NumBars = curr->m_NumBrothels;
		if(curr->m_NumGamblingHalls > curr->m_NumBrothels)
			curr->m_NumGamblingHalls = curr->m_NumBrothels;

		// Get their income
		income = 0;

		int MinGirls = MaxNumGirls;
		int MaxGirls = curr->m_NumGirls*80;
		income += ((g_Dice%(MaxGirls-MinGirls))+MinGirls)*((g_Dice%2)+1);

		int Customers = 0, MinGamb = 0, MaxGamb = 0, MinBar = 0, MaxBar = 0;

		Customers = ((g_Dice%curr->m_NumGirls)+curr->m_NumGirls)*((g_Dice%1)+1);
		MinGamb = Customers*5;
		MaxGamb = Customers*100;
		income += (curr->m_NumGamblingHalls)*(((g_Dice%(MaxGamb-MinGamb))+MinGamb));

		MinBar = Customers*10;
		MaxBar = Customers*20;
		income += (curr->m_NumBars)*(((g_Dice%(MaxBar-MinBar))+MinBar));

		if(curr->m_BusinessesExtort > 0)
			income += (curr->m_BusinessesExtort*INCOME_BUSINESS);

		curr->m_Gold += income;

		// Calc their upkeep
		upkeep = (MaxNumGirls)*1;	// girls rent
		upkeep += curr->m_NumGirls*5;
		upkeep += curr->m_BribeRate;
		upkeep += curr->m_NumBars*20;
		upkeep += curr->m_NumGamblingHalls*80;
		upkeep += (curr->m_NumGamblingHalls)*(((g_Dice%(MaxGamb-MinGamb))+MinGamb));
		upkeep += (curr->m_NumBars)*((g_Dice%100)+30);	// upkeep for buying barrels of booze
		upkeep += (curr->m_NumGangs*90);

		float taxRate = 0.06f;	// normal tax rate is 6%
		if(curr->m_Influence > 0)	// can you influence it lower
		{
			int lowerBy = curr->m_Influence/20;
			float amount = (float)(lowerBy/100);
			taxRate -= amount;
			if(taxRate <= 0.0f)
				taxRate = 0.01f;
		}
		if(income > 0)
		{
			int tmp = income-(g_Dice%(int)(income*0.25f));	// launder up to 25% of gold
			int tax = (int)(tmp*taxRate);
			upkeep += tax;
		}

		curr->m_Gold -= upkeep;

		// Determine if their upkeep is greater than their income
		bool danger = false;
		if(upkeep >= income)
		{
			// Danger of bankrupt cull some assets
			danger = true;	// this will make sure AI doesn't replace them this turn

			if(curr->m_NumGamblingHalls > 0)
				curr->m_NumGamblingHalls--;
			else if(curr->m_NumBars > 0)
				curr->m_NumBars--;
			else if(curr->m_NumBrothels != 1)
			{
				curr->m_NumBrothels--;
				curr->m_Gold += 1000;
			}
			else
			{
				curr->m_NumGangs -= (g_Dice%5)+6;
				if(curr->m_NumGangs < 0)
					curr->m_NumGangs = 0;
			}

			// check numbers of girls, gambling halls, and bars are correct for the number of brothels
			int MaxNumGirls = (20*curr->m_NumBrothels);
			if(curr->m_NumGirls > MaxNumGirls)
				curr->m_NumGirls = MaxNumGirls;
			if(curr->m_NumBars > curr->m_NumBrothels)
				curr->m_NumBars = curr->m_NumBrothels;
			if(curr->m_NumGamblingHalls > curr->m_NumBrothels)
				curr->m_NumGamblingHalls = curr->m_NumBrothels;
		}

		// buy new brothels
		if(curr->m_NumGirls == MaxNumGirls && curr->m_NumBrothels < 6 && !danger)
		{
			if(curr->m_Gold-20000 > 0)
			{
				curr->m_Gold -= 20000;
				curr->m_NumBrothels++;
				MaxNumGirls+=20;
				upkeep += 20000;
			}
		}

		// attempt to buy new girls
		int maxNum = (g_Dice%6)+1;
		int i=0;
		while(1)
		{
			if(curr->m_NumGirls < MaxNumGirls && i < maxNum)
			{
				i++;
				if(curr->m_Gold-550 >= 0)
				{
					curr->m_NumGirls++;
					curr->m_Gold -= 550;
					upkeep += 550;
				}
				else
					break;
			}
			else
				break;
		}

		// hire new gangs
		if(curr->m_NumGangs < 8 && !danger)
		{
			curr->m_NumGangs += (g_Dice%5)+1;
			if(curr->m_NumGangs > 8)
				curr->m_NumGangs = 8;
		}

		// buy new bars or gambling halls
		if(curr->m_NumBars < curr->m_NumBrothels && !danger)
		{
			if(curr->m_Gold-2500 >= 0)
			{
				curr->m_NumBars++;
				curr->m_Gold -= 2500;
				upkeep += 2500;
			}
		}
		else if(curr->m_NumGamblingHalls < curr->m_NumBrothels && !danger)
		{
			if(curr->m_Gold-15000 >= 0)
			{
				curr->m_NumGamblingHalls++;
				curr->m_Gold -= 15000;
				upkeep += 15000;
			}
		}

		// Work out goon missions (only if have 20 or more goons)
		if(curr->m_NumGangs > 0)
		{
			// extort mission
			int who = (g_Dice%(m_NumRivals+1));

			int numB = GetNumBusinesses()+NumPlayerBussiness;

			if(numB < TOWN_NUMBUSINESSES)
			{
				int n = g_Dice%5;

				if(numB+n > TOWN_NUMBUSINESSES)
					n = TOWN_NUMBUSINESSES-numB;

				if(n > 0)
				{
					curr->m_BusinessesExtort += n;
					int extraIncome = n*20;
					curr->m_Gold += extraIncome;
					income += extraIncome;
				}
			}
			else
			{
				if(who == m_NumRivals)
				{
					if(!player_safe())
					{
						if(NumPlayerBussiness > 0)
						{
							sGang* miss1 = g_Gangs.GetGangOnMission(MISS_GUARDING);
							if(miss1)
							{
								string message;
								message += gettext("Your guards encounter ");
								message += curr->m_Name;
								message += gettext(" going after some of your territory.");

								sGang* rGang = g_Gangs.GetTempGang();
								if(g_Gangs.GangBrawl(miss1, rGang))
								{
									if(rGang->m_Num == 0)
										curr->m_NumGangs--;
									message += gettext("\nBut you maintain control of the territory.");
									miss1->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_GANG);
								}
								else
								{
									if(miss1->m_Num == 0)
										g_Gangs.RemoveGang(miss1);
									message += gettext("\nYou lose the territory.");
									NumPlayerBussiness--;
									curr->m_BusinessesExtort++;
									g_MessageQue.AddToQue(message, COLOR_RED);
								}
								delete rGang;
								rGang = 0;
							}
							else
							{
								string message = gettext("Your rival ");
								message += curr->m_Name;
								message += gettext(" has taken one of the undefended territories you control.");
								g_MessageQue.AddToQue(message, COLOR_RED);
								NumPlayerBussiness--;
								curr->m_BusinessesExtort++;
							}
						}
					}
				}
				else
				{
					string message = gettext("The ");
					message += curr->m_Name;
					message += gettext(" attacked the territories of ");
					cRival* rival = GetRival(who);
					if(rival != curr)
					{
						message += rival->m_Name;
						if(rival->m_BusinessesExtort > 0)
						{
							if(rival->m_NumGangs > 0)
							{
								if(1+(g_Dice%100) <= 50)
								{
									curr->m_NumGangs--;
									message += gettext(" and lost.");
								}
								else
								{
									rival->m_NumGangs--;
									rival->m_BusinessesExtort--;
									curr->m_BusinessesExtort++;
									message += gettext(" and won.");
								}
								g_MessageQue.AddToQue(message, 0);
							}
							else
							{
								rival->m_BusinessesExtort--;
								curr->m_BusinessesExtort++;
							}
						}
					}
				}
			}

			// theft mission
			if(curr->m_NumGangs > 0)
			{
				int extraIncome = g_Dice%600;
				curr->m_Gold += extraIncome;
				income += extraIncome;
			}

			// attack mission
			if(curr->m_NumGangs > 0)
			{
				if((g_Dice%100) < 70)
				{
					if(who == m_NumRivals)
					{
						if(!player_safe())
						{
							sGang* miss1 = g_Gangs.GetGangOnMission(MISS_GUARDING);
							if(miss1)
							{
								string message = "";
								message += gettext("Your rival the ");
								message += curr->m_Name;
								message += gettext(" attack your assets.");

								sGang* rGang = g_Gangs.GetTempGang();

								if(!g_Gangs.GangBrawl(miss1, rGang))
								{
									if(miss1->m_Num == 0)
										g_Gangs.RemoveGang(miss1);
									message += gettext(" Your men are defeated and ");
									if(NumPlayerBussiness > 0)
									{
										int num = (g_Dice%3)+1;
										NumPlayerBussiness -= num;
										if(NumPlayerBussiness < 0)
											NumPlayerBussiness = 0;
										message += gettext("they destroy some of your territories.");
									}

									rivals_plunder_pc_gold(curr, message);
									g_MessageQue.AddToQue(message, COLOR_RED);

								}
								else
								{
									if(rGang->m_Num == 0)
										curr->m_NumGangs--;
									message += gettext("But they fail.");
									miss1->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_GANG);
								}
								delete rGang;
								rGang = 0;
							}
							else
							{
								string message = gettext("You have no guards so your rival ");
								message += curr->m_Name;
								message += gettext(" attacks and ");
								if(NumPlayerBussiness > 0 || g_Gold.ival() > 0)
								{
									if(NumPlayerBussiness > 0)
									{
										int num = (g_Dice%3)+1;
										NumPlayerBussiness -= num;
										if(NumPlayerBussiness < 0)
											NumPlayerBussiness = 0;
										message += gettext("they destroy some of your territories.");
									}

									rivals_plunder_pc_gold(curr, message);
									g_MessageQue.AddToQue(message, COLOR_RED);
								}
							}
						}
					}
					else
					{
						string message = gettext("The ");
						message += curr->m_Name;
						message += gettext(" launched an assault on ");
						cRival* rival = GetRival(who);
						if(rival != curr)
						{
							message += rival->m_Name;
							if(rival->m_NumGangs > 0)
							{
								if(1+(g_Dice%100) <= 50)
								{
									rival->m_NumGangs--;
									message += gettext(" and won.");
									if(rival->m_BusinessesExtort > 0)
									{
										int num = (g_Dice%3)+1;
										rival->m_BusinessesExtort -= num;
										if(rival->m_BusinessesExtort < 0)
											rival->m_BusinessesExtort = 0;
									}

									if(rival->m_Gold > 0)
									{
										long gold = (g_Dice%2000)+44;
										if((rival->m_Gold - gold) > 0)
											rival->m_Gold -= gold;
										else
										{
											gold = gold-rival->m_Gold;
											rival->m_Gold = 0;
										}
										curr->m_Gold += gold;
									}

									if(rival->m_NumBrothels > 0 && (g_Dice%100) < 10)
										rival->m_NumBrothels--;
								}
								else
								{
									message += gettext(" and lost.");
									curr->m_NumGangs--;
								}
							}
							else
							{
								if(rival->m_BusinessesExtort > 0)
								{
									int num = (g_Dice%3)+1;
									rival->m_BusinessesExtort -= num;
									if(rival->m_BusinessesExtort < 0)
										rival->m_BusinessesExtort = 0;
								}

								if(rival->m_Gold > 0)
								{
									long gold = (g_Dice%2000)+44;
									if((rival->m_Gold - gold) > 0)
										rival->m_Gold -= gold;
									else
									{
										gold = gold-rival->m_Gold;
										rival->m_Gold = 0;
									}
									curr->m_Gold += gold;
								}

								if(rival->m_NumBrothels > 0 && (g_Dice%100) < 10)
									rival->m_NumBrothels--;
							}
							g_MessageQue.AddToQue(message, 0);
						}
					}
				}
			}
		}

		// handicap super profits
		if(income-upkeep > 10000)
		{
			income = 0;
			curr->m_Gold -= 10000;
		}

		// if doing well financially then increase their bribe rate
		if(income-upkeep > 1000)
		{
			curr->m_BribeRate += (long)(50);
			g_Brothels.UpdateBribeInfluence();
		}
		else if(income-upkeep < 0 && curr->m_BribeRate > 0)
		{
			curr->m_BribeRate -= (long)(upkeep-income);
			if(curr->m_BribeRate < 0)
				curr->m_BribeRate = 0;
			g_Brothels.UpdateBribeInfluence();
		}

		curr = curr->m_Next;
	}
}

cRival* cRivalManager::GetRandomRival()
{
	if(m_NumRivals == 0)
		return 0;

	if(m_NumRivals == 1)
		return m_Rivals;

	int number = g_Dice%m_NumRivals;
	cRival* current = m_Rivals;
	int tmp = 0;
	while(current)
	{
		if(tmp == number)
			break;
		tmp++;
		current = current->m_Next;
	}

	return current;
}

int cRivalManager::GetNumBusinesses()
{
	int number = 0;

	cRival* current = m_Rivals;
	while(current)
	{
		number += current->m_BusinessesExtort;
		current = current->m_Next;
	}

	return number;
}

cRival* cRivalManager::GetRival(string name)
{
	cRival* current = m_Rivals;
	while(current)
	{
		if(current->m_Name == name)
			return current;
		current = current->m_Next;
	}
	return 0;
}

cRival* cRivalManager::get_influential_rival()
{
	cRival* current;
	cRival* top = 0;
/*
 *	we're looking for the rival with the most influence
 */
	for(current = m_Rivals; current; current = current->m_Next) {
/*
 *		if the rival has no influence, skip on
 */
		if(current->m_Influence <= 0) {
			continue;
		}
/*
 *		If we don't have a candidate yet, 
 *		anyone with influence will do.
 *
 *		And since we already weeded out
 *		the influence-less rivals at this point...
 */
		if(top == 0) {
			top = current;
			continue;
		}
/*
 *		is the current rival more influential than the
 *		the one we have our eye on?
 */
 		if(current->m_Influence < top->m_Influence) {
			continue;
		}
		top = current;
	}
/*
 *	this will return the most influential rival
 *	or null if there were no rivals with influence
 */
	return top;
}

cRival* cRivalManager::GetRival(int number)
{
	cRival* current = m_Rivals;
	int tmp = 0;
	while(current)
	{
		if(tmp == number)
			return current;
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
	while(current)
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
	Free();//everything should be init even if we failed to load an XML element
	TiXmlElement* pRivalManager = hRivalManager.ToElement();
	if (pRivalManager == 0)
	{
		return false;
	}

	string message = "";
	m_NumRivals = 0;
	TiXmlElement* pRivals = pRivalManager->FirstChildElement("Rivals");
	if (pRivals)
	{
		for(TiXmlElement* pRival = pRivals->FirstChildElement("Rival");
			pRival != 0;
			pRival = pRival->NextSiblingElement("Rival"))
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

void cRivalManager::CreateRival(long bribeRate, int extort, long gold, int bars, int gambHalls, int Girls, int brothels, int goons)
{
	ifstream in;
	cRival* rival = new cRival();

	DirPath first_names = DirPath()
		<< "Resources"
		<< "Data"
		<< "RivalGangFirstNames.txt"
		;
	DirPath last_names = DirPath()
		<< "Resources"
		<< "Data"
		"RivalGangLastNames.txt"
		;

	rival->m_Gold = gold;
	rival->m_NumBrothels = brothels;
	rival->m_NumGirls = Girls;
	rival->m_NumGangs = goons;
	rival->m_BribeRate = bribeRate;
	rival->m_BusinessesExtort = extort;
	rival->m_NumBars = bars;
	rival->m_NumGamblingHalls = gambHalls;

	for(;;) {
		rival->m_Name = names.random();
		if(!NameExists(rival->m_Name)) {
			break;
		}
	}
	AddRival(rival);
}

bool cRivalManager::NameExists(string name)
{
	cRival* current = m_Rivals;
	while(current)
	{
		if(current->m_Name == name)
			return true;
		current = current->m_Next;
	}
	return false;
}

void cRivalManager::CreateRandomRival()
{
	ifstream in;
	cRival* rival = new cRival();

	DirPath first_names = DirPath()
		<< "Resources"
		<< "Data"
		<< "RivalGangFirstNames.txt"
		;
	DirPath last_names = DirPath()
		<< "Resources"
		<< "Data"
		"RivalGangLastNames.txt"
		;

	rival->m_Gold = (g_Dice%20000)+5000;
	rival->m_NumBrothels = (g_Dice%3)+1;
	rival->m_NumGirls = 0;
	while(rival->m_NumGirls == 0)
		rival->m_NumGirls = (g_Dice%((rival->m_NumBrothels)*20)) + 20;
	rival->m_NumGangs = 10+g_Dice%5;
	
	for(;;) {
		rival->m_Name = names.random();
		if(!NameExists(rival->m_Name)) {
			break;
		}
	}
	AddRival(rival);
}

void cRivalManager::AddRival(cRival* rival)
{
	if(m_Last)
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
	if(rival->m_Next)
		rival->m_Next->m_Prev = rival->m_Prev;
	if(rival->m_Prev)
		rival->m_Prev->m_Next = rival->m_Next;
	if(rival == m_Rivals)
		m_Rivals = rival->m_Next;
	if(rival == m_Last)
		m_Last = rival->m_Prev;
	rival->m_Prev = rival->m_Next = 0;
	delete rival;
	rival = 0;
	m_NumRivals--;
}
