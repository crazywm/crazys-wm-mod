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
#include "libintl.h"

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;

bool cJobManager::WorkFreetime(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	//brothel->m_Filthiness++;
	g_Girls.UpdateStat(girl, STAT_TIREDNESS, -20);
	g_Girls.UpdateStat(girl, STAT_HAPPINESS, 15);
	g_Girls.UpdateStat(girl, STAT_HEALTH, 10);
	g_Girls.UpdateStat(girl, STAT_MANA, 10);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, 5);
	g_Girls.UpdateStat(girl, STAT_EXP, 1);   // Just because!

	string message = gettext("She rested and recovered some energy.");

	if(g_Dice%2 != 1)	// add inventory items since she is going shopping :D
		return false;

	int v[2] = {-1,-1};
	girl->m_Triggers.CheckForScript(TRIGGER_SHOPPING, true, v);	// check for and trigger shopping scripts

	if(girl->m_Money == 0 || girl->m_NumInventory == 40)
	{
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, DayNight);
		return false;
	}

	// 1. if she is addicted she will first attempt to purchase drugs until she has no money
	if(g_Girls.HasTrait(girl, "Viras Blood Addict"))
      AddictBuysDrugs("Viras Blood Addict", "Viras Blood", girl, brothel, DayNight);

	if(g_Girls.HasTrait(girl, "Fairy Dust Addict"))
      AddictBuysDrugs("Fairy Dust Addict", "Fairy Dust", girl, brothel, DayNight);

	if(g_Girls.HasTrait(girl, "Shroud Addict"))
      AddictBuysDrugs("Shroud Addict", "Shroud Mushroom", girl, brothel, DayNight);

	if(girl->m_Money == 0 || girl->m_NumInventory == 40)
	{
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, DayNight);
		return false;
	}

	// 2. buy any items that catch her fancy
	int numberToBuy = g_Dice%5;	// buy up to 10 things  MYR: Reduced to 5 to cut down on inventory clutter
	int itemsBought = 0;
	string buyList = "";

	for(int i=0; i < numberToBuy && girl->m_NumInventory < 40; i++)
	{
		int item = g_InvManager.GetRandomShopItem();
		int cost = g_InvManager.GetShopItem(item)->m_Cost;
		string itemName = g_InvManager.GetShopItem(item)->m_Name;

		if(g_Girls.HasItem(girl, itemName) > -1)
			continue;
		if(girl->m_Money - cost < 0)
			continue;

		if(g_Dice.percent(g_InvManager.GetShopItem(item)->m_GirlBuyChance))
		{
			int chance = (g_Dice%100+1);
			switch((int)g_InvManager.GetShopItem(item)->m_Type)
			{
				case INVRING:
				{
					// Third argument to GirlBuyITem is the max # of the item type (ring, dress, etc...) they 
					// should have (Max. 8 rings, max 1 dress, etc...)
					if(g_InvManager.GirlBuyItem(girl, item, 8, true))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;

				case INVDRESS:
				{
					if(g_InvManager.GirlBuyItem(girl, item, 1, true))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;

				case INVUNDERWEAR:
				{
					if(g_InvManager.GirlBuyItem(girl, item, 1, true))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;

				case INVSHOES:
				{
					if(g_InvManager.GirlBuyItem(girl, item, 1, true))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;

				case INVFOOD:
				{
					if(g_InvManager.GirlBuyItem(girl, item, 15, false))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;

				case INVNECKLACE:
				{
					if(g_InvManager.GirlBuyItem(girl, item, 1, true))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;

				case INVWEAPON:
				{
					// if she isn't a warrior type she probably won't buy it
					if(!g_Girls.HasTrait(girl, "Adventurer") && !g_Girls.HasTrait(girl, "Assassin") && (girl->combat() < chance))
						break;

					if(g_InvManager.GirlBuyItem(girl, item, 1, true))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;

				case INVSMWEAPON:
				{
					if(g_InvManager.GirlBuyItem(girl, item, 2, true))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;

				case INVMAKEUP:
				{
					if(g_InvManager.GirlBuyItem(girl, item, 15, false))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;

				case INVARMOR:
				{
					// if she isn't a warrior type she probably won't buy it
					if(!g_Girls.HasTrait(girl, "Adventurer") && !g_Girls.HasTrait(girl, "Assassin") && (girl->combat() < chance))
						break;

					if(g_InvManager.GirlBuyItem(girl, item, 1, true))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;

				case INVMISC:
				{
					if(g_InvManager.GirlBuyItem(girl, item, 15, false))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;

				case INVARMBAND:
				{
					if(g_InvManager.GirlBuyItem(girl, item, 2, true))
					{
						buyList += ((buyList == "") ? "" : ", ") + itemName;
						itemsBought++;
					}
				}
				break;
			} // Switch
		}     // if buy
	}         // for # buy chances

	if(itemsBought > 0)
	{
		g_Girls.UpdateStat(girl, STAT_HAPPINESS, 5 + (itemsBought * 2));
		message += gettext(" She did some shopping, and bought: ") + buyList + ".";
	}

	girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, DayNight);
	return false;
}

bool cJobManager::AddictBuysDrugs(string Addiction, string Drug, sGirl* girl, sBrothel* brothel, int DayNight)
{
	int id = g_InvManager.CheckShopItem(Drug);
	if(id == -1)
		return false;

	int cost = g_InvManager.GetItem(Drug)->m_Cost;

	if(!g_Girls.CheckInvSpace(girl))	// quit if inventory is full
		return false;

	if((girl->m_Money - cost) < 0)	// if they can afford it, they will buy it
		return false;

	girl->m_Money -= cost;

	// If a matron is on shift, she may catch the girl buying drugs
	if((g_Brothels.GetNumGirlsOnJob(brothel->m_id, JOB_MATRON, true) >= 1 || g_Brothels.GetNumGirlsOnJob(brothel->m_id, JOB_MATRON, false) >= 1)
		&& (g_Dice%100)+1 < 70)
	{
		girl->m_Events.AddMessage("Matron confiscates drugs", IMGTYPE_PROFILE, EVENT_WARNING);
		return false;
	}
	else
	{
		g_Girls.AddInv(girl, g_InvManager.BuyShopItem(id));
		return true;
	}

}
