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
#include "tinyxml.h"
#include "cInventory.h"
#include <iostream>
#include <fstream>
#include "cBrothel.h"
#include "cMessageBox.h"
#include "cCustomers.h"
#include "libintl.h"
using namespace std;

extern cGirls g_Girls;
extern cTraits g_Traits;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cBrothelManager g_Brothels;
extern cMessageQue g_MessageQue;
extern cRng g_Dice;

// ----- Misc

cInventory::~cInventory()
{
	Free();
}

void cInventory::Free()
{
	for(int i = 0; i < NUM_SHOPITEMS; i++) m_ShopItems[i] = 0;
}

const char *sEffect::girl_status_name(unsigned int id)
{
	if(id < sGirl::max_statuses) {
		return sGirl::status_names[id];
	}
	g_LogFile.os() << "[sEffect::girl_status_name] Error: girl status id " << id 
	     << " too large (max is "
	     << sGirl::max_statuses
	     << ")"
	     << endl;
	return "";
}

const char *sEffect::skill_name(unsigned int id)
{
	if(id < sGirl::max_skills) {
		return sGirl::skill_names[id];
	}
	g_LogFile.os() << "[sEffect::skill_name] Error: skill id " << id 
	     << " too large (max is "
	     << sGirl::max_skills
	     << ")"
	     << endl;
	return "";
}

const char *sEffect::stat_name(unsigned int id)
{
	if(id < sGirl::max_stats) {
		return sGirl::stat_names[id];
	}
	g_LogFile.os() << "[sEffect::stat_name] Error: stat id " << id 
	     << " too large (max is "
	     << sGirl::max_stats
	     << ")"
	     << endl;
	return "";
}


bool sEffect::set_skill(string s)
	{
		
		int nID	= sGirl::lookup_skill_code(s);

		if (nID == -1)		// ERROR
		{
			g_LogFile.os() << "[sEffect::set_skill] Error: unknown Skill: " << s << 
				". Skill ID: " << nID << endl;
			return false;
		}
		m_EffectID = nID;
		return true;
	}

	bool sEffect::set_girl_status(string s)
	{
		
		int nID	= sGirl::lookup_status_code(s);

		if (nID == -1)		// ERROR
		{
			g_LogFile.os() << "[sEffect::lookup_status_code] Error: unknown Status: " << s <<
				". Skill ID: " << nID << endl;
			return false;
		}
		m_EffectID = nID;
		return true;
	}
	bool sEffect::set_stat(string s)
	{
		
		int nID	= sGirl::lookup_stat_code(s);

		if (nID == -1)		// ERROR
		{
			g_LogFile.os() << "[sEffect::set_stat] Error: unknown Stat: " << s <<
				". Skill ID: " << nID << endl;
			return false;
		}
		m_EffectID = nID;
		return true;
	}


void cInventory::GivePlayerAllItems()
{
	sInventoryItem* item;

	for(u_int i = 0;  i < items.size(); i++) {
		item = items[i];
		int curI = g_Brothels.HasItem(item->m_Name, -1);

		bool loop = true;
		while(loop)
		{
			if(curI != -1)
			{
				if(g_Brothels.m_NumItem[curI] >= 254)
					curI = g_Brothels.HasItem(item->m_Name, curI+1);
				else
					loop = false;
			}
			else
				loop = false;
		}

		if(g_Brothels.m_NumInventory >= MAXNUM_INVENTORY && curI == -1) {
			g_LogFile.write("Adding all items cheat: inventory full");
			break;
		}

		if(curI != -1)
		{
			g_Brothels.m_NumItem[curI]++;
			continue;
		}

		for(int j=0; j<MAXNUM_INVENTORY; j++)
		{
			if(g_Brothels.m_Inventory[j] == 0)
			{
				g_Brothels.m_Inventory[j] = item;
				g_Brothels.m_EquipedItems[j] = 0;
				g_Brothels.m_NumInventory++;
				g_Brothels.m_NumItem[j]++;
				break;
			}
		}
	}
}

/*
 * should make this part of the cInventoryItem
 */
static void do_effects(TiXmlElement *parent, sInventoryItem *item)
{
	int ival;
	const char *pt;
	TiXmlElement *el;

	for(el = parent->FirstChildElement(); el; el = el->NextSiblingElement()) {
		sEffect *ept = new sEffect;

		if((pt = el->Attribute("What"))) 
			ept->set_what(pt);

		if((pt = el->Attribute("Name"))) {
			switch(ept->m_Affects) {
			case sEffect::Trait:
				ept->m_Trait = pt;
				break;
			case sEffect::Stat:
				if(ept->set_stat(pt) == false) {
					g_LogFile.os() << "effect type code == " << ept->m_Affects;
					g_LogFile.os() << " stat lookup failed for " << item->m_Name << endl;

				}
				break;
			case sEffect::GirlStatus:
				ept->set_girl_status(pt);
				break;
			case sEffect::Skill:
				ept->set_skill(pt);
				break;
			default:
				g_LogFile.os() << " can't handle effect type " << ept->m_Affects << endl;
			}
		}

		if((pt = el->Attribute("Amount", &ival))) 
			ept->m_Amount = ival;
		
		item->m_Effects.push_back(*ept);
	}

}

void cInventory::AddItem(sInventoryItem* item)
{
	items.push_back(item);
}

void cInventory::remove_trait(sGirl* girl, int num, int index)
{
	u_int item_type = girl->m_Inventory[num]->m_Type;
	string trait_name = girl->m_Inventory[num]->m_Effects[index].m_Trait;

	/*
	 *	WD:	New logic for remembering traits 
	 *		moved to AddTrait() RemoveTrait() fn's
	 *
	 *		Don't think this is called any more.
	 *
	 */

	g_Girls.RemoveTrait(girl, trait_name,					// Remove Traits
		item_type != INVFOOD && item_type != INVMAKEUP);	// Remember if not consumable
}

bool cInventory::GirlBuyItem(sGirl* girl, int ShopItem, int MaxItems, bool AutoEquip)
{  
	// girl buys selected item if possible; returns true if bought
	sInventoryItem* item = GetShopItem(ShopItem);
	if(g_Girls.GetNumItemType(girl, item->m_Type) >= MaxItems)
	{  
		// if she has enough of this type, she won't buy more unless it's better than what she has
		int nicerThan = g_Girls.GetWorseItem(girl,(int)item->m_Type, item->m_Cost);
		if(nicerThan != -1)
		{  
			// found a worse item of the same type in her inventory
			g_Girls.SellInvItem(girl, nicerThan);
			girl->m_Money -= item->m_Cost;
			int temp = g_Girls.AddInv(girl, item);
			if (temp != -1 && AutoEquip)   // MYR: Check temp value
				Equip(girl, temp, false);
			return true;
		}
	}
	else
	{
		girl->m_Money -= item->m_Cost;
		int temp = g_Girls.AddInv(girl, item);
		if(temp != -1 && AutoEquip)   // MYR: Check temp value
			Equip(girl, temp, false);
		return true;
	}

	return false;
}

void cInventory::CalculateCost(sInventoryItem* newItem)
{
	for(u_int i=0; i<newItem->m_Effects.size(); i++)
	{
		// stats
		if(newItem->m_Effects[i].m_Affects == 1)
		{
			if(newItem->m_Effects[i].m_Amount >= 0)
			{
				// bad stats increases
				if(newItem->m_Effects[i].m_EffectID == STAT_TIREDNESS || newItem->m_Effects[i].m_EffectID == STAT_PCHATE)
					newItem->m_Cost += newItem->m_Effects[i].m_Amount*2;
				else	// good stats increases
				{
					newItem->m_Cost += newItem->m_Effects[i].m_Amount*10;
				}

				// make items that do more cost a little less (buy in bulk)
				newItem->m_Cost -= (newItem->m_Effects[i].m_Amount/5)*5;
			}
			else
			{
				// bad stats decreases
				if(newItem->m_Effects[i].m_EffectID == STAT_TIREDNESS || newItem->m_Effects[i].m_EffectID == STAT_PCHATE)
					newItem->m_Cost += newItem->m_Effects[i].m_Amount*10;
				else	// good stats decreases
				{
					newItem->m_Cost += newItem->m_Effects[i].m_Amount*2;
				}

				// make items that do heaps of stuff cost a little less
				newItem->m_Cost -= (newItem->m_Effects[i].m_Amount/5)*5;
			}
		}

		// skills
		if(newItem->m_Effects[i].m_Affects == 0)
		{
			if(newItem->m_Effects[i].m_Amount >= 0)	// increases skill
				newItem->m_Cost += newItem->m_Effects[i].m_Amount*15;
			else	// decreases skill
				newItem->m_Cost += newItem->m_Effects[i].m_Amount*2;

			// make items that do heaps of stuff cost a little less
			newItem->m_Cost -= (newItem->m_Effects[i].m_Amount/5)*5;
		}

		// status
		if(newItem->m_Effects[i].m_Affects == 3)
		{
			if(newItem->m_Effects[i].m_Amount == 1)	// adds status
			{
				if(newItem->m_Effects[i].m_EffectID == STATUS_POISONED)
					newItem->m_Cost -= 20;
				else if(newItem->m_Effects[i].m_EffectID == STATUS_BADLY_POISONED)
					newItem->m_Cost -= 50;
				else if(newItem->m_Effects[i].m_EffectID == STATUS_PREGNANT)
					newItem->m_Cost += 30;
				else if(newItem->m_Effects[i].m_EffectID == STATUS_PREGNANT_BY_PLAYER)
					newItem->m_Cost += 30;
				else if(newItem->m_Effects[i].m_EffectID == STATUS_SLAVE)
					newItem->m_Cost += 400;
				else if(newItem->m_Effects[i].m_EffectID == STATUS_HAS_DAUGHTER)
					newItem->m_Cost += 200;
				else if(newItem->m_Effects[i].m_EffectID == STATUS_HAS_SON)
					newItem->m_Cost += 200;
				else if(newItem->m_Effects[i].m_EffectID == STATUS_INSEMINATED)
					newItem->m_Cost += 30;
				else if(newItem->m_Effects[i].m_EffectID == STATUS_CONTROLLED)
					newItem->m_Cost += 900;
				else if(newItem->m_Effects[i].m_EffectID == STATUS_CATACOMBS)
					newItem->m_Cost += 90;
				else if(newItem->m_Effects[i].m_EffectID == STATUS_ARENA)
					newItem->m_Cost += 90;
			}
			else	// removes status
			{
				if(newItem->m_Effects[i].m_EffectID == STATUS_POISONED)
					newItem->m_Cost += 100;
				else if(newItem->m_Effects[i].m_EffectID == STATUS_BADLY_POISONED)
					newItem->m_Cost += 200;
				else if(newItem->m_Effects[i].m_EffectID == STATUS_PREGNANT)
					newItem->m_Cost += 10;
				else if(newItem->m_Effects[i].m_EffectID == STATUS_PREGNANT_BY_PLAYER)
					newItem->m_Cost += 10;
				else if(newItem->m_Effects[i].m_EffectID == STATUS_SLAVE)
					newItem->m_Cost += 70;
				else if(newItem->m_Effects[i].m_EffectID == STATUS_HAS_DAUGHTER)
					newItem->m_Cost += 20;
				else if(newItem->m_Effects[i].m_EffectID == STATUS_HAS_SON)
					newItem->m_Cost += 20;
				else if(newItem->m_Effects[i].m_EffectID == STATUS_INSEMINATED)
					newItem->m_Cost += 10;
				else if(newItem->m_Effects[i].m_EffectID == STATUS_CONTROLLED)
					newItem->m_Cost += 20;
				else if(newItem->m_Effects[i].m_EffectID == STATUS_CATACOMBS)
					newItem->m_Cost += 20;
				else if(newItem->m_Effects[i].m_EffectID == STATUS_ARENA)
					newItem->m_Cost += 20;
			}
		}

		// traits
		if(newItem->m_Effects[i].m_Affects == 4)
		{
			newItem->m_Cost += 500;
		}
	}

	if(newItem->m_Effects.size() > 0)
		newItem->m_Cost += newItem->m_Effects.size()*5;
	if(newItem->m_Rarity > 0)
		newItem->m_Cost += newItem->m_Rarity*5;
	if(newItem->m_Special == 1)
		newItem->m_Cost += 2000;
	else if(newItem->m_Special == 2)
		newItem->m_Cost += 100;

	if(newItem->m_Cost <= 10)
		newItem->m_Cost = 10;
}

ostream& operator << (ostream& os, sEffect::What &w)
{
	switch(w) {
	case sEffect::Skill:		return os << "Skill";
	case sEffect::Stat:		return os << "Stat";
	case sEffect::Nothing:		return os << "Nothing";
	case sEffect::GirlStatus:	return os << "GirlStatus";
	case sEffect::Trait:		return os << "Trait";
	default:
		g_LogFile.os() << "error: unexpected 'what' value: " << int(w) << endl;
		return os << "Error(" << int(w) << ")";
	}
}

int cInventory::HappinessFromItem(sInventoryItem* item)
{
	// decrease value by 5% for each point of badness
	int Value = int((double)item->m_Cost * ((100 - ((double)item->m_Badness * 5)) / 100));
	if(Value > 300)  // then, 1 happiness per 300 gold value
		return (Value / 300);
	else
		return 1;
}

// ----- Shop

sInventoryItem* cInventory::BuyShopItem(int num)
{
	if(num >= NUM_SHOPITEMS)
		return 0;

	sInventoryItem* item = m_ShopItems[num];

	if(item->m_Infinite == 0)
	{
		m_ShopItems[num] = 0;
		m_NumShopItems--;
	}

	return item;
}

int cInventory::CheckShopItem(string name)
{
	int num = -1;
	for(int i=0; i<NUM_SHOPITEMS; i++)
	{
		if(m_ShopItems[i])
		{
			if(m_ShopItems[i]->m_Name == name)
				num = i;
		}
	}

	return num;
}

void cInventory::UpdateShop()
{
	for(int i=0; i<NUM_SHOPITEMS; i++)
	{
		sInventoryItem* item = GetRandomItem();
		while(item == 0)
			item = GetRandomItem();
		if(item->m_Infinite == 1 && CheckShopItem(item->m_Name) != -1)
		{
			i--;
			continue;
		}

		if(item == 0)
			break;
		if(item->m_Rarity == 4 || item->m_Rarity == 5)
		{
			while(item->m_Rarity == 4 || item->m_Rarity == 5)
			{
				item = GetRandomItem();
				while(item == 0)
					item = GetRandomItem();
			}
		}

		if(item->m_Rarity == 0)
		{
			m_ShopItems[i] = item;
			m_NumShopItems++;
			continue;
		}
		else if(item->m_Rarity == 1)
		{
			if(((g_Dice%100)+1) <= 50)
			{
				m_ShopItems[i] = item;
				m_NumShopItems++;
				continue;
			}
			else
			{
				i--;
				continue;
			}
		}
		else if(item->m_Rarity == 2)
		{
			if(((g_Dice%100)+1) <= 25)
			{
				m_ShopItems[i] = item;
				m_NumShopItems++;
				continue;
			}
			else
			{
				i--;
				continue;
			}
		}
		else if(item->m_Rarity == 3)
		{
			if(((g_Dice%100)+1) <= 5)
			{
				m_ShopItems[i] = item;
				m_NumShopItems++;
				continue;
			}
			else
			{
				i--;
				continue;
			}
		}
	}
}

sInventoryItem* cInventory::GetShopItem(int num)
{
	if(m_NumShopItems == 0)
		UpdateShop();

	if(num >= NUM_SHOPITEMS)
		return 0;

	return m_ShopItems[num];
}

int cInventory::GetRandomShopItem()
{
	if(m_NumShopItems == 0)
		UpdateShop();

	int num = g_Dice%NUM_SHOPITEMS;

	while(m_ShopItems[num] == 0)
		num = g_Dice%NUM_SHOPITEMS;

	if(num > NUM_SHOPITEMS-1)  // shouldn't be necessary, but once I got 40 back causing OOB elsewhere
		num = NUM_SHOPITEMS-1;

	return num;
}

// ----- Get

sInventoryItem* cInventory::GetRandomItem()
{
	sInventoryItem *ipt;
	//g_LogFile.os() << "cInventory::GetRandomItem: "
	//     << "items.size == "
	//     << items.size()
	//     << endl
	//;
	if(items.size() == 0) {
		//g_LogFile.os() << "	returning null" << endl;
		return 0;
	}

	if(items.size() == 1) {
		ipt = items[0];
		//g_LogFile.os() << "	returning 0x" << hex << int(ipt) << endl;
		return items[0];
	}

	int index = g_Dice%(items.size()-1);	// fixed crash with going outside vector size - necro
	//g_LogFile.os() << "	returning item at index " << index << endl;
	ipt = items[index];
	//g_LogFile.os() << "	returning 0x" << hex << int(ipt) << dec << endl;

	return ipt;
}

sInventoryItem* cInventory::GetItem(string name)
{
	sInventoryItem* item;

	for(u_int i = 0;  i < items.size(); i++) {
		item = items[i];
		if(item->m_Name == name) {
			return item;
		}
	}

	return 0;
}

// ----- Equip unequip

void cInventory::Equip(sGirl* girl, int num, bool force)
{
	// dead girls shouldn't be able to equip or use anything
	if(girl->health() <= 0)
		return;

	// if already equiped do nothing
	if(girl->m_EquipedItems[num] == 1) 
		return;

	if(girl->m_Inventory[num]->m_Special == sInventoryItem::AffectsAll) 
		return;

	if(ok_2_equip(girl, num, force) == false) 
		return;

	// apply the effects
	for(u_int i=0; i < girl->m_Inventory[num]->m_Effects.size(); i++)
	{
		int eff_id = girl->m_Inventory[num]->m_Effects[i].m_EffectID;
		int amount = girl->m_Inventory[num]->m_Effects[i].m_Amount;

		if(girl->m_Inventory[num]->m_Special == sInventoryItem::Temporary)
		{
			switch(girl->m_Inventory[num]->m_Effects[i].m_Affects)
			{
			case sEffect::Nothing:
			case sEffect::GirlStatus:
/*
 *				I can understand ignoring "Nothing"
 *				Ought "GirlStatus" to have an effect?
 *				Seem to remember this cropping up on the forum
 */
				break;

			case sEffect::Skill:			// affects skill
				girl->upd_temp_skill(eff_id, amount);
				break;

			case sEffect::Stat:			// affects skill
				girl->upd_temp_stat(eff_id, amount);
				break;

			case sEffect::Trait:			// affects skill
#if 1
					/*
					 *	WD:	New logic for remembering traits 
					 *		moved to AddTrait() RemoveTrait() fn's
					 *
					 *		EQUIP Temporary Item
					 */

				if(amount == 0)													// remove trait temporarily from equiping an item	
					g_Girls.RemoveTrait(girl, girl->m_Inventory[num]->m_Effects[i].m_Trait,	true);		// addrememberlist = true Temporary Item trait removal

				else if(girl->m_Inventory[num]->m_Effects[i].m_Amount == 1)		// add temporary trait
					 g_Girls.AddTrait(girl, girl->m_Inventory[num]->m_Effects[i].m_Trait, true, true);	// Temp = true Temporary Item, removeitem = true for Temporary Item trait addition

#else			//	WD:	Original Code.
				if(amount == 0)						// remove temp trait
					remove_trait(girl, num, i);

				else if(girl->m_Inventory[num]->m_Effects[i].m_Amount == 1)	// add trait
				{
					// if the item is consumable
					if(girl->m_Inventory[num]->m_Type != INVFOOD && girl->m_Inventory[num]->m_Type != INVMAKEUP)
					{
						// if they don't already have the trait and it is not being remembered
						if(!g_Girls.HasTrait(girl, girl->m_Inventory[num]->m_Effects[i].m_Trait) && !g_Girls.HasRememberedTrait(girl, girl->m_Inventory[num]->m_Effects[i].m_Trait))
							// then just add the trait
							g_Girls.AddTrait(girl, girl->m_Inventory[num]->m_Effects[i].m_Trait, true);
						else
							// ok just add the trait to the trait memory
							g_Girls.AddRememberedTrait(girl, girl->m_Inventory[num]->m_Effects[i].m_Trait);
						}
					else // just add the trait
						g_Girls.AddTrait(girl, girl->m_Inventory[num]->m_Effects[i].m_Trait, true);
				}
#endif
			}
		}
		else	// m_Special == sInventoryItem::None
		{
			if(girl->m_Inventory[num]->m_Effects[i].m_Affects == 0)	// affects skill
				g_Girls.UpdateSkill(girl,girl->m_Inventory[num]->m_Effects[i].m_EffectID, girl->m_Inventory[num]->m_Effects[i].m_Amount);
			else if(girl->m_Inventory[num]->m_Effects[i].m_Affects == 1)	// affects stat
				g_Girls.UpdateStat(girl,girl->m_Inventory[num]->m_Effects[i].m_EffectID, girl->m_Inventory[num]->m_Effects[i].m_Amount);
			else if(girl->m_Inventory[num]->m_Effects[i].m_Affects == 3)	// adds/removes status
			{
				if(girl->m_Inventory[num]->m_Effects[i].m_Amount >= 1)	// add status
				{
					if(girl->m_Inventory[num]->m_Effects[i].m_EffectID == STATUS_PREGNANT || girl->m_Inventory[num]->m_Effects[i].m_EffectID == STATUS_PREGNANT_BY_PLAYER || girl->m_Inventory[num]->m_Effects[i].m_EffectID == STATUS_INSEMINATED)
					{  // does this item impregnate/inseminate? If so, m_Amount is used to determine chance... 1 is normal (8% default), 10 would give 80% chance (default config)
						if(!g_Girls.HasTrait(girl, "Sterile"))
						{
							cPlayer *player = g_Brothels.GetPlayer();
							bool AntiPreg = girl->m_UseAntiPreg;
							girl->m_UseAntiPreg = false;  // we don't want contraceptives automatically used, eh?
							if(girl->m_Inventory[num]->m_Effects[i].m_EffectID == STATUS_PREGNANT_BY_PLAYER)
							{
								bool preg = !girl->calc_pregnancy(player, false, (double)girl->m_Inventory[num]->m_Effects[i].m_Amount);
								if(preg)
									g_MessageQue.AddToQue(girl->m_Realname + gettext(": ") + girl->m_Inventory[num]->m_Name + gettext(": The use of this item has impregnated her with your seed."), 1);
								else
									g_MessageQue.AddToQue(girl->m_Realname + gettext(": ") + girl->m_Inventory[num]->m_Name + gettext(": The use of this item has failed to impregnate her with your seed. Better luck next time."), 0);
							}
							if(girl->m_Inventory[num]->m_Effects[i].m_EffectID == STATUS_PREGNANT)
							{
								sBrothel* brothel = g_Brothels.GetBrothel(0);
								sCustomer Cust;
								g_Customers.GetCustomer(Cust, brothel);
								bool preg = !girl->calc_pregnancy(&Cust, false, (double)girl->m_Inventory[num]->m_Effects[i].m_Amount);
								if(preg)
									g_MessageQue.AddToQue(girl->m_Realname + gettext(": ") + girl->m_Inventory[num]->m_Name + gettext(": The use of this item has impregnated her."), 1);
								else
									g_MessageQue.AddToQue(girl->m_Realname + gettext(": ") + girl->m_Inventory[num]->m_Name + gettext(": The use of this item has failed to impregnate her. Better luck next time."), 0);
							}
							if(girl->m_Inventory[num]->m_Effects[i].m_EffectID == STATUS_INSEMINATED)
							{
								bool preg = !girl->calc_insemination(player, false, (double)girl->m_Inventory[num]->m_Effects[i].m_Amount);
								if(preg)
									g_MessageQue.AddToQue(girl->m_Realname + gettext(": ") + girl->m_Inventory[num]->m_Name + gettext(": The use of this item has inseminated her with the seed of a beast."), 1);
								else
									g_MessageQue.AddToQue(girl->m_Realname + gettext(": ") + girl->m_Inventory[num]->m_Name + gettext(": The use of this item has failed to inseminate her. Better luck next time."), 0);
							}
							girl->m_UseAntiPreg = AntiPreg;
						}
					}
					else
						girl->m_States |= (1<<girl->m_Inventory[num]->m_Effects[i].m_EffectID);
				}
				else if(girl->m_Inventory[num]->m_Effects[i].m_Amount == 0)	// remove status
				{
					if(girl->m_States&(1<<girl->m_Inventory[num]->m_Effects[i].m_EffectID))
						girl->m_States&=~(1<<girl->m_Inventory[num]->m_Effects[i].m_EffectID);
					if(girl->m_Inventory[num]->m_Effects[i].m_EffectID == STATUS_PREGNANT || girl->m_Inventory[num]->m_Effects[i].m_EffectID == STATUS_PREGNANT_BY_PLAYER || girl->m_Inventory[num]->m_Effects[i].m_EffectID == STATUS_INSEMINATED)
					{
						if(!g_Girls.HasTrait(girl, "Sterile"))
						{
							if(g_Dice%100 == 0)
							{
								g_MessageQue.AddToQue(girl->m_Realname + gettext(": ") + girl->m_Inventory[num]->m_Name + gettext(": The use of this item has made her sterile, she can no longer have children."), 1);
								g_Girls.AddTrait(girl, "Sterile");
							}
						}
					}
				}
			}
			else if(girl->m_Inventory[num]->m_Effects[i].m_Affects == 4)	// trait
			{
			/*
			 *	WD:	New logic for remembering traits 
			 *		moved to AddTrait() RemoveTrait() fn's
			 *
			 *		EQUIP Normal Item
			 */

			if(girl->m_Inventory[num]->m_Effects[i].m_Amount == 0)				// remove trait from equiping an item
					g_Girls.RemoveTrait(girl, girl->m_Inventory[num]->m_Effects[i].m_Trait,								
						girl->m_Inventory[num]->m_Type != INVFOOD && girl->m_Inventory[num]->m_Type != INVMAKEUP);		// addrememberlist = true only if not consumable

			else if(girl->m_Inventory[num]->m_Effects[i].m_Amount == 1)			// add normal trait	from equiping an item
					g_Girls.AddTrait(girl, girl->m_Inventory[num]->m_Effects[i].m_Trait, false,							
						girl->m_Inventory[num]->m_Type != INVFOOD && girl->m_Inventory[num]->m_Type != INVMAKEUP);		// Temp = false Normal Item, removeitem = true only if not consumable
			}
		}
	}

	if(girl->m_Inventory[num]->m_Type == INVFOOD || girl->m_Inventory[num]->m_Type == INVMAKEUP)	// if consumable then remove from inventory
	{
		girl->m_Inventory[num] = 0;
		girl->m_EquipedItems[num] = 0;
		girl->m_NumInventory--;
	}
	else	// set it as equiped
		girl->m_EquipedItems[num] = 1;

	g_Girls.CalculateGirlType(girl);
}

void cInventory::Unequip(sGirl* girl, int num)
{
	// if already unequiped do nothing
	if(girl->m_EquipedItems[num] == 0)
		return;

	// unapply the effects
	for(u_int i=0; i<girl->m_Inventory[num]->m_Effects.size(); i++)
	{
		if(girl->m_Inventory[num]->m_Effects[i].m_Affects == 0)	// affects skill
			g_Girls.UpdateSkill(girl,girl->m_Inventory[num]->m_Effects[i].m_EffectID, -(girl->m_Inventory[num]->m_Effects[i].m_Amount));
		else if(girl->m_Inventory[num]->m_Effects[i].m_Affects == 1)	// affects stat
			g_Girls.UpdateStat(girl,girl->m_Inventory[num]->m_Effects[i].m_EffectID, -(girl->m_Inventory[num]->m_Effects[i].m_Amount));
		else if(girl->m_Inventory[num]->m_Effects[i].m_Affects == 3)	// adds/removes status
		{
			if(girl->m_Inventory[num]->m_Effects[i].m_Amount == 1)	// add status
				girl->m_States &= ~(1<<girl->m_Inventory[num]->m_Effects[i].m_EffectID);
			else if(girl->m_Inventory[num]->m_Effects[i].m_Amount == 0)	// remove status
				girl->m_States |= (1<<girl->m_Inventory[num]->m_Effects[i].m_EffectID);
		}
		else if(girl->m_Inventory[num]->m_Effects[i].m_Affects == 4)	// trait
#if 1
		{

			/*
			 *	WD:	New logic for remembering traits 
			 *		moved to AddTrait() RemoveTrait() fn's
			 *
			 *		UNEQUIP
			 */

			if(girl->m_Inventory[num]->m_Effects[i].m_Amount == 0)					// possibly add remembered trait from unequiping an item
				 g_Girls.AddTrait(girl, girl->m_Inventory[num]->m_Effects[i].m_Trait, false, false, true);	// inrememberlist = true Add trait only if it is in the rememebered list

			else if(girl->m_Inventory[num]->m_Effects[i].m_Amount == 1)				// remove item trait from unequiping an item
				g_Girls.RemoveTrait(girl, girl->m_Inventory[num]->m_Effects[i].m_Trait);
		}
	}

#else
		{
			if(girl->m_Inventory[num]->m_Effects[i].m_Amount == 0)	// add trait effect back
			{
				if(g_Girls.HasRememberedTrait(girl, girl->m_Inventory[num]->m_Effects[i].m_Trait))
				{
					// ok, they had the trait to begin with so add it back if it is only remembered once (if more then once then something else has removed it as well)
					g_Girls.RemoveRememberedTrait(girl, girl->m_Inventory[num]->m_Effects[i].m_Trait);
					if(!g_Girls.HasRememberedTrait(girl, girl->m_Inventory[num]->m_Effects[i].m_Trait))
						g_Girls.AddTrait(girl, girl->m_Inventory[num]->m_Effects[i].m_Trait);
				}
				// if they didn't have the trait to begin with cannot add it back
			}
			else if(girl->m_Inventory[num]->m_Effects[i].m_Amount == 1)	// remove trait effect
			{
				// if it is remembered then the trait should remain but removed from memory
				if(g_Girls.HasRememberedTrait(girl, girl->m_Inventory[num]->m_Effects[i].m_Trait))
					g_Girls.RemoveRememberedTrait(girl, girl->m_Inventory[num]->m_Effects[i].m_Trait);
				else
					g_Girls.RemoveTrait(girl, girl->m_Inventory[num]->m_Effects[i].m_Trait);
			}
		}
	}
#endif

	// set it as unequiped
	girl->m_EquipedItems[num] = 0;

	g_Girls.CalculateGirlType(girl);
}

void cInventory::Equip(sGirl* girl, sInventoryItem* item, bool force)
{
	// this function is only used for global effects sInventoryItem::AffectsAll = 1
	if(item->m_Special != sInventoryItem::AffectsAll)	
		return;

	for(u_int i = 0; i < item->m_Effects.size(); i++) 
	{
		if(item->m_Effects[i].m_Affects == 0)	// affects skill
			g_Girls.UpdateTempSkill(girl,item->m_Effects[i].m_EffectID, item->m_Effects[i].m_Amount);
		else if(item->m_Effects[i].m_Affects == 1)	// affects stat
			g_Girls.UpdateTempStat(girl,item->m_Effects[i].m_EffectID, item->m_Effects[i].m_Amount);
		else if(item->m_Effects[i].m_Affects == 4)	// trait
		{

		/*
		 *	WD:	New logic for remembering traits 
		 *		moved to AddTrait() RemoveTrait() fn's
		 *
		 *		EQUIP AffectAll Item
		 */

			if(item->m_Effects[i].m_Amount == 0)			// remove trait temporarily from equiping an item							
				g_Girls.RemoveTrait(girl, item->m_Effects[i].m_Trait, true);	// addrememberlist = true AffectAll trait removal
						

			else if(item->m_Effects[i].m_Amount == 1)		// add temporary trait 					
				g_Girls.AddTrait(girl, item->m_Effects[i].m_Trait, true, true); // Temp = true AffectAll Item, removeitem = true for AffectAll trait

		}
	}
}

bool cInventory::equip_ring_ok(sGirl* girl, int num, bool force)
{
	return equip_limited_item_ok(girl, num, force, 8);
}

bool cInventory::equip_singleton_ok(sGirl* girl, int num, bool force)
{
	return equip_limited_item_ok(girl, num, force, 1);
}

bool cInventory::equip_pair_ok(sGirl* girl, int num, bool force)
{
	return equip_limited_item_ok(girl, num, force, 2);
}

bool cInventory::equip_limited_item_ok(sGirl* girl, int num, bool force, int limit)
{
	int count = 0;
	int target_type = girl->m_Inventory[num]->m_Type;
/*
 *	there's a limited number of items that can be 
 *	equipped for any given type
 *
 *	so we're going to loop through the inventory
 *	counting the item in question
 */
	for(int i = 0; i < 40 && count < limit; i++)
	{
		// Don't double count the item we're checking
		//if (i == num)
		//	continue;
/*
 *		if there's nothing in the slot, skip it
 */
		if(girl->m_Inventory[i] == 0) 
			continue;
		
/*
 *		ok - there's something in the slot
 *		if it's not the correct type, skip it anyway
 */
		if(girl->m_Inventory[i]->m_Type != target_type) 
			continue;
		
/*
 *		so it is the target type: she might not have it equipped
 *		if not, skip it.
 */
		if(girl->m_EquipedItems[i] != 1) 
			continue;
		
/*
 *		All right, all right. it's the right type, and it's equipped
 *		add it to the count.
 */
		count++;
/*
 *		now if we're not forcing her to wear this
 *		then at this point we can skip on to the next item,
 *
 *		for that matter, if we are forcing her, but the count is
 *		less than the limit, we might still find a free slot
 *		so we skip on then, too
 */
 		if(!force || count < limit) 
			continue;
		
/*
 *		If we get here, we have no free slots,
 *		and we're forcing her to wear it (Muh-ha-ha-ha-ha!)
 *
 *		so we need to remove this last ring,
 *		decrement the count and exit the loop
 */
		count--;
		Unequip(girl, i);
		break;
	}

	if(count == limit) 
		return false;

	return true;
}

bool cInventory::ok_2_equip(sGirl *girl, int num, bool force)
{
	// first check if can equip it
	switch(girl->m_Inventory[num]->m_Type)
	{
	case sInventoryItem::Ring:
		if(equip_ring_ok(girl, num, force) == false) 
			return false;
		break;
	case sInventoryItem::Dress:
	case sInventoryItem::Shoes:
	case sInventoryItem::Necklace:
	case sInventoryItem::Armor:
	case sInventoryItem::UnderWear:
		if(equip_singleton_ok(girl,  num, force) == false) 
			return false;
		break;
	case sInventoryItem::SmWeapon:	// doc: added missing enum
	case sInventoryItem::Weapon:
	case sInventoryItem::Armband:
		if(equip_pair_ok(girl,  num, force) == false) 
			return false;
		break;
	case sInventoryItem::Food:
	case sInventoryItem::Makeup:
	case sInventoryItem::Misc:
		return true;
	}
	return true;
}

// ----- Load

static sInventoryItem* handle_element(TiXmlElement *el)
{
	int ival;
	const char *pt;
	sInventoryItem* item = new sInventoryItem();
	if ((pt = el->Attribute("Name"))) 
		item->m_Name = pt;
	
	if ((pt = el->Attribute("Desc"))) 
		item->m_Desc = pt;
	else 
		cout << "no desc attribute found" << endl;
	
	if((pt = el->Attribute("Type"))) 
		item->set_type(pt);
	
	if((pt = el->Attribute("Badness", &ival))) 
		item->m_Badness = ival;
	else
		item->m_Badness = 0;
	
	if((pt = el->Attribute("GirlBuyChance", &ival))) 
		item->m_GirlBuyChance = ival;
	else  // if not specified, set chance based on badness
		item->m_GirlBuyChance = (item->m_Badness < 20) ? (100 - (item->m_Badness * 5)) : 0;
	
	if((pt = el->Attribute("Special"))) 
		item->set_special(pt);
	
	if((pt = el->Attribute("Cost", &ival))) 
		item->m_Cost = ival;
	
	if((pt = el->Attribute("Rarity"))) 
		item->set_rarity(pt);
	
	if((pt = el->Attribute("Infinite"))) 
		item->m_Infinite = (string(pt) == "true");
	else 
		item->m_Infinite = false;
	
	do_effects(el, item);
	return item;
}

bool cInventory::LoadItemsXML(string filename)
{
	cConfig cfg;

	TiXmlDocument doc(filename);
	if(!doc.LoadFile()) {
		g_LogFile.os()	<< "can't load item file " << filename << endl;
        g_LogFile.os()    << "Error: line " << doc.ErrorRow() << ", col " << doc.ErrorCol()
			<< ": " << doc.ErrorDesc() << endl;
		return false;
	}

	TiXmlElement *el;
	TiXmlElement *root_el = doc.RootElement();
	bool log_flag = cfg.debug.log_items();

	for(el = root_el->FirstChildElement(); el; el = el->NextSiblingElement()) {
		sInventoryItem* item = handle_element(el);
		if(log_flag) 
			g_LogFile.os() << *item << endl;
		items.push_back(item);
	}
	return true;
}

void cInventory::LoadItems(string filename)
{
	string msg = "Loading items from ";
	msg += filename;
	g_LogFile.write(msg);
	g_LogFile.os() << "loading items from '" << filename << "'" << endl;
	ifstream in;
	in.open(filename.c_str());
	if(!in.good()) 
		g_LogFile.os() << "LoadItems: stream not good after open" << endl;

	char buffer[1000];
	sInventoryItem* newItem = 0;
	long tempData;

	while(in.good())
	{
		newItem = new sInventoryItem();

		if (in.peek()=='\n') in.ignore(1,'\n');
		in.getline(buffer, sizeof(buffer), '\n');		// get the name
		g_LogFile.os() << "LoadItems: " << buffer << endl;
		if(buffer[0] == 0) 
			break;
		
		newItem->m_Name = buffer;

		if (in.peek()=='\n') in.ignore(1,'\n');
		in.getline(buffer, sizeof(buffer), '\n');		// get the description
		newItem->m_Desc = buffer;

		if (in.peek()=='\n') in.ignore(1,'\n');
		in>>tempData;
		newItem->m_Type = sInventoryItem::Type(tempData);
		in>>tempData;
		newItem->m_Badness = (unsigned char)tempData;
		in>>tempData;
		newItem->m_Special = sInventoryItem::Special(tempData);
		in>>tempData;
		newItem->m_Cost = tempData;
		in>>tempData;
		newItem->m_Rarity = sInventoryItem::Rarity(tempData);
		in>>tempData;
		newItem->m_Infinite = ((unsigned char)tempData !=0);

		// get items attributes
		if (in.peek()=='\n') in.ignore(1,'\n');
		in>>tempData;
		newItem->m_Effects.resize(tempData);
		for(u_int i = 0; i < newItem->m_Effects.size(); i++)
		{
			if (in.peek()=='\n') in.ignore(1,'\n');
			in>>tempData;

			newItem->m_Effects[i].m_Affects = sEffect::What(tempData);

			if(newItem->m_Effects[i].m_Affects == 4)
			{
				if (in.peek()=='\n') in.ignore(1,'\n');
				in.getline(buffer, sizeof(buffer), '\n');		// get the trait name
				newItem->m_Effects[i].m_Trait = buffer;
				if (in.peek()=='\n') in.ignore(1,'\n');	// get weather to add or remove the trait
				in>>tempData;
				newItem->m_Effects[i].m_Amount = tempData;
			}
			else if(newItem->m_Effects[i].m_Affects != 4)
			{
				in>>tempData;
				newItem->m_Effects[i].m_EffectID = (unsigned char)tempData;
				in>>tempData;
				newItem->m_Effects[i].m_Amount = tempData;
				newItem->m_Effects[i].m_Trait = "";
			}
		}

		//CalculateCost(newItem);
		AddItem(newItem);
	}

	in.close();
}


/*

g++  -D LINUX -I /usr/include/SDL -g -Wall  -c -o cInventory.o cInventory.cpp

*/
