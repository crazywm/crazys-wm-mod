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

extern string stringtolowerj(string name);

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
	if (id < sGirl::max_statuses) return sGirl::status_names[id];
	g_LogFile.os() << "[sEffect::girl_status_name] Error: girl status id " << id << " too large (max is " << sGirl::max_statuses << ")" << endl;
	return "";
}

const char *sEffect::skill_name(unsigned int id)
{
	if (id < sGirl::max_skills) return sGirl::skill_names[id];
	g_LogFile.os() << "[sEffect::skill_name] Error: skill id " << id << " too large (max is " << sGirl::max_skills << ")" << endl;
	return "";
}

const char *sEffect::stat_name(unsigned int id)
{
	if (id < sGirl::max_stats) return sGirl::stat_names[id];
	g_LogFile.os() << "[sEffect::stat_name] Error: stat id " << id << " too large (max is " << sGirl::max_stats << ")" << endl;
	return "";
}

bool sEffect::set_skill(string s)
{
	int nID = sGirl::lookup_skill_code(s);
	if (nID == -1)		// ERROR
	{
		g_LogFile.os() << "[sEffect::set_skill] Error: unknown Skill: " << s << ". Skill ID: " << nID << endl;
		return false;
	}
	m_EffectID = nID;
	return true;
}

bool sEffect::set_girl_status(string s)
{
	int nID = sGirl::lookup_status_code(s);
	if (nID == -1)		// ERROR
	{
		g_LogFile.os() << "[sEffect::lookup_status_code] Error: unknown Status: " << s << ". Skill ID: " << nID << endl;
		return false;
	}
	m_EffectID = nID;
	return true;
}

bool sEffect::set_stat(string s)
{
	int nID = sGirl::lookup_stat_code(s);
	if (nID == -1)		// ERROR
	{
		g_LogFile.os() << "[sEffect::set_stat] Error: unknown Stat: " << s << ". Skill ID: " << nID << endl;
		return false;
	}
	m_EffectID = nID;
	return true;
}

void cInventory::GivePlayerAllItems()
{
	sInventoryItem* item;
	for (u_int i = 0; i < items.size(); i++)
	{
		item = items[i];
		int curI = g_Brothels.HasItem(item->m_Name, -1);
		bool loop = true;
		while (loop)
		{
			if (curI != -1 && g_Brothels.m_NumItem[curI] >= 999) curI = g_Brothels.HasItem(item->m_Name, curI + 1);
			else loop = false;
		}
		if (g_Brothels.m_NumInventory >= MAXNUM_INVENTORY && curI == -1)
		{
			g_LogFile.write("Adding all items cheat: inventory full");
			break;
		}
		if (curI != -1)
		{
			g_Brothels.m_NumItem[curI]++;
			continue;
		}
		for (int j = 0; j < MAXNUM_INVENTORY; j++)
		{
			if (g_Brothels.m_Inventory[j] == 0)
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
	int ival; const char *pt; TiXmlElement *el;
	for (el = parent->FirstChildElement(); el; el = el->NextSiblingElement())
	{
		sEffect *ept = new sEffect;
		if (pt = el->Attribute("What")) ept->set_what(pt);
		if (pt = el->Attribute("Name"))
		{
			switch (ept->m_Affects)
			{
			case sEffect::Trait:
				ept->m_Trait = pt;
				break;
			case sEffect::Stat:
				if (ept->set_stat(pt) == false)
				{
					g_LogFile.os() << "effect type code == " << ept->m_Affects;
					g_LogFile.os() << " stat lookup failed for " << item->m_Name << endl;
				}
				break;
			case sEffect::GirlStatus:
				if (!ept->set_girl_status(pt)) g_LogFile.os() << " Error in item " << item->m_Name << endl;
				break;
			case sEffect::Skill:
				if (!ept->set_skill(pt)) g_LogFile.os() << " Error in item " << item->m_Name << endl;
				break;
			default:
				g_LogFile.os() << " can't handle effect type " << ept->m_Affects << endl;
				break;
			}
		}
		if (pt = el->Attribute("Amount", &ival)) ept->m_Amount = ival;
		
		if (pt = el->Attribute("Duration", &ival)) ept->m_Duration = ival;
		else ept->m_Duration = 20;

		item->m_Effects.push_back(*ept);
	}
}

static void do_tests(TiXmlElement *parent, sInventoryItem *item)	// `J` added
{// `J` This is to be for xml item tests for if the girl can auto use the item
	/*  `J` copied from do_effects as the base
	int ival;
	const char *pt;
	TiXmlElement *el;
	for (el = parent->FirstChildElement(); el; el = el->NextSiblingElement()) {
		sEffect *ept = new sEffect;

		if ((pt = el->Attribute("What")))
			ept->set_what(pt);

		if ((pt = el->Attribute("Name"))) {
			switch (ept->m_Affects) {
			case sEffect::Trait:
				ept->m_Trait = pt;
				break;
			case sEffect::Stat:
				if (ept->set_stat(pt) == false) {
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

		if ((pt = el->Attribute("Amount", &ival)))
			ept->m_Amount = ival;

		item->m_Effects.push_back(*ept);
	}
	*/
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
	if (trait_name == "Virgin") girl->m_Virgin = 0;

	g_Girls.RemoveTrait(girl, trait_name,					// Remove Traits
		item_type != INVFOOD && item_type != INVMAKEUP);	// Remember if not consumable
}

bool cInventory::GirlBuyItem(sGirl* girl, int ShopItem, int MaxItems, bool AutoEquip)
{
	// girl buys selected item if possible; returns true if bought
	sInventoryItem* item = GetShopItem(ShopItem);
	if (g_Girls.GetNumItemType(girl, item->m_Type) >= MaxItems)
	{
		// if she has enough of this type, she won't buy more unless it's better than what she has
		int nicerThan = g_Girls.GetWorseItem(girl, (int)item->m_Type, item->m_Cost);
		if (nicerThan != -1)
		{
			// found a worse item of the same type in her inventory
			g_Girls.SellInvItem(girl, nicerThan);
			girl->m_Money -= item->m_Cost;
			int temp = g_Girls.AddInv(girl, item);
			if (temp != -1 && AutoEquip) Equip(girl, temp, false); // MYR: Check temp value
			return true;
		}
	}
	else
	{
		girl->m_Money -= item->m_Cost;
		int temp = g_Girls.AddInv(girl, item);
		if (temp != -1 && AutoEquip) Equip(girl, temp, false);	// MYR: Check temp value
		return true;
	}
	return false;
}

void cInventory::CalculateCost(sInventoryItem* newItem)
{
	for (u_int i = 0; i < newItem->m_Effects.size(); i++)
	{
		if (newItem->m_Effects[i].m_Affects == 1)	// stats
		{
			if (newItem->m_Effects[i].m_Amount >= 0)
			{
				// bad stats increases
				if (newItem->m_Effects[i].m_EffectID == STAT_TIREDNESS || newItem->m_Effects[i].m_EffectID == STAT_PCHATE)
					newItem->m_Cost += newItem->m_Effects[i].m_Amount * 2;
				else	// good stats increases
				{
					newItem->m_Cost += newItem->m_Effects[i].m_Amount * 10;
				}
				// make items that do more cost a little less (buy in bulk)
				newItem->m_Cost -= (newItem->m_Effects[i].m_Amount / 5) * 5;
			}
			else
			{
				// bad stats decreases
				if (newItem->m_Effects[i].m_EffectID == STAT_TIREDNESS || newItem->m_Effects[i].m_EffectID == STAT_PCHATE)
					newItem->m_Cost += newItem->m_Effects[i].m_Amount * 10;
				else	// good stats decreases
				{
					newItem->m_Cost += newItem->m_Effects[i].m_Amount * 2;
				}
				// make items that do heaps of stuff cost a little less
				newItem->m_Cost -= (newItem->m_Effects[i].m_Amount / 5) * 5;
			}
		}
		if (newItem->m_Effects[i].m_Affects == 0)	// skills
		{
			if (newItem->m_Effects[i].m_Amount >= 0)	// increases skill
				newItem->m_Cost += newItem->m_Effects[i].m_Amount * 15;
			else	// decreases skill
				newItem->m_Cost += newItem->m_Effects[i].m_Amount * 2;

			// make items that do heaps of stuff cost a little less
			newItem->m_Cost -= (newItem->m_Effects[i].m_Amount / 5) * 5;
		}
		if (newItem->m_Effects[i].m_Affects == 3)	// status
		{
			if (newItem->m_Effects[i].m_Amount == 1)	// adds status
			{
				/* */if (newItem->m_Effects[i].m_EffectID == STATUS_POISONED)				newItem->m_Cost -= 20;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_BADLY_POISONED)			newItem->m_Cost -= 50;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_PREGNANT)				newItem->m_Cost += 30;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_PREGNANT_BY_PLAYER)		newItem->m_Cost += 30;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_SLAVE)					newItem->m_Cost += 400;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_HAS_DAUGHTER)			newItem->m_Cost += 200;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_HAS_SON)				newItem->m_Cost += 200;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_INSEMINATED)			newItem->m_Cost += 30;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_CONTROLLED)				newItem->m_Cost += 900;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_CATACOMBS)				newItem->m_Cost += 90;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_ARENA)					newItem->m_Cost += 90;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_YOURDAUGHTER)			newItem->m_Cost += 900;
			}
			else	// removes status
			{
				/* */if (newItem->m_Effects[i].m_EffectID == STATUS_POISONED)				newItem->m_Cost += 100;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_BADLY_POISONED)			newItem->m_Cost += 200;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_PREGNANT)				newItem->m_Cost += 10;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_PREGNANT_BY_PLAYER)		newItem->m_Cost += 10;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_SLAVE)					newItem->m_Cost += 70;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_HAS_DAUGHTER)			newItem->m_Cost += 20;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_HAS_SON)				newItem->m_Cost += 20;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_INSEMINATED)			newItem->m_Cost += 10;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_CONTROLLED)				newItem->m_Cost += 20;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_CATACOMBS)				newItem->m_Cost += 20;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_ARENA)					newItem->m_Cost += 20;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_YOURDAUGHTER)			newItem->m_Cost += 200;
			}
		}
		if (newItem->m_Effects[i].m_Affects == 4)	newItem->m_Cost += 500;	// traits
	}
	if (newItem->m_Effects.size() > 0)		newItem->m_Cost += newItem->m_Effects.size() * 5;
	if (newItem->m_Rarity > 0)				newItem->m_Cost += newItem->m_Rarity * 5;
	if (newItem->m_Special == 1)			newItem->m_Cost += 2000;
	else if (newItem->m_Special == 2)		newItem->m_Cost += 100;
	if (newItem->m_Cost <= 10)				newItem->m_Cost = 10;
}

ostream& operator << (ostream& os, sEffect::What &w)
{
	switch (w) {
	case sEffect::Skill:		return os << "Skill";
	case sEffect::Stat:			return os << "Stat";
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
	// then, 1 happiness per 300 gold value
	return (Value > 300) ? (Value / 300) : 1;
}

// ----- Shop

sInventoryItem* cInventory::BuyShopItem(int num)
{
	if (num >= NUM_SHOPITEMS) return 0;
	sInventoryItem* item = m_ShopItems[num];
	if (item->m_Infinite == 0)
	{
		m_ShopItems[num] = 0;
		m_NumShopItems--;
	}
	return item;
}

int cInventory::CheckShopItem(string name)
{
	int num = -1;
	for (int i = 0; i < NUM_SHOPITEMS; i++)
	{
		if (m_ShopItems[i])
		{
			if (m_ShopItems[i]->m_Name == name) num = i;
		}
	}
	return num;
}

void cInventory::UpdateShop()
{
	for (int i = 0; i < NUM_SHOPITEMS; i++)
	{
		sInventoryItem* item = GetRandomItem();
		while (item == 0) item = GetRandomItem();
		if (item == 0) break;	// should never happen but left in anyway

		int chance = g_Dice.d100();
		if ((item->m_Rarity == RARITYCOMMON ||
			(item->m_Rarity == RARITYSHOP50 && chance <= 50) ||
			(item->m_Rarity == RARITYSHOP25 && chance <= 25) ||
			(item->m_Rarity == RARITYSHOP05 && chance <= 5)) && 
			(item->m_Infinite == 0 || (item->m_Infinite == 1 && CheckShopItem(item->m_Name) == -1)))
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

sInventoryItem* cInventory::GetShopItem(int num)
{
	if (m_NumShopItems == 0) UpdateShop();
	if (num >= NUM_SHOPITEMS) return 0;
	return m_ShopItems[num];
}

int cInventory::GetRandomShopItem()
{
	if (m_NumShopItems == 0) UpdateShop();
	int num = g_Dice%NUM_SHOPITEMS;
	while (m_ShopItems[num] == 0) num = g_Dice%NUM_SHOPITEMS;
	if (num > NUM_SHOPITEMS - 1) num = NUM_SHOPITEMS - 1;   // shouldn't be necessary, but once I got 40 back causing OOB elsewhere
	return num;
}

// ----- Get

sInventoryItem* cInventory::GetRandomItem()
{
	sInventoryItem *ipt;
	cConfig cfg; int log = 0; if (cfg.debug.log_items()) log = (cfg.debug.log_extradetails()) ? 2 : 1;
	if (log > 0) g_LogFile.os() << "cInventory::GetRandomItem: " << "items.size == " << items.size() << endl;
	if (items.size() == 0)
	{
		if (log > 1) g_LogFile.os() << "	returning null" << endl;
		return 0;
	}
	if (items.size() == 1)
	{
		ipt = items[0];
		if (log > 1) g_LogFile.os() << "	returning 0x" << hex << int(ipt) << endl;
		return items[0];
	}
	int index = g_Dice % (items.size() - 1);	// fixed crash with going outside vector size - necro
	if (log > 1) g_LogFile.os() << "	returning item at index " << index << endl;
	ipt = items[index];
	if (log > 1) g_LogFile.os() << "	returning 0x" << hex << int(ipt) << dec << endl;
	return ipt;
}

sInventoryItem* cInventory::GetRandomCatacombItem()
{
	sInventoryItem *temp;
	int tries = 200;	// 200 tries to get an item
	while (tries > 0)
	{
		temp = GetRandomItem();
		switch (temp->m_Rarity) {
		case RARITYSHOP25:									return temp;	break;
		case RARITYSHOP05:		if (g_Dice.percent(25))		return temp;	break;
		case RARITYCATACOMB15:	if (g_Dice.percent(15))		return temp;	break;
		case RARITYCATACOMB05:	if (g_Dice.percent(5))		return temp;	break;
		case RARITYCATACOMB01:	if (g_Dice.percent(1))		return temp;	break;
		case RARITYSCRIPTONLY:	
		case RARITYSCRIPTORREWARD:	
			temp = 0;
			break;	// if at the end it is a script item, no item is returned
		case RARITYCOMMON:	
		case RARITYSHOP50: 
		default: 
			break;	// if at the end it is a common item, that item is returned
		}
		tries--;
	} 
	if (!temp) return 0;
	return temp;
}

sInventoryItem* cInventory::GetItem(string name)
{
	sInventoryItem* item;
	for (u_int i = 0; i < items.size(); i++)
	{
		item = items[i];
		if (item->m_Name == name) return item;
	}
	return 0;
}

// ----- Equip unequip

void cInventory::Equip(sGirl* girl, int num, bool force)
{
	// Allow certain Items to recover the dead
	if (girl->health() <= 0 && (
		stringtolowerj(girl->m_Inventory[num]->m_Name) == stringtolowerj("Better Zed than Dead") ||
		stringtolowerj(girl->m_Inventory[num]->m_Name) == stringtolowerj("Elixir of Ultimate Regeneration")
		)){}
	else if (girl->health() <= 0 ||					// dead girls shouldn't be able to equip or use anything
		girl->m_EquipedItems[num] == 1 ||		// if already equiped do nothing
		girl->m_Inventory[num]->m_Special == sInventoryItem::AffectsAll || // no "AffectsAll" item should be equipable
		ok_2_equip(girl, num, force) == false)	// of if it is not ok to equip it
		return;

	// apply the effects
	for (u_int i = 0; i < girl->m_Inventory[num]->m_Effects.size(); i++)
	{
		int affects = girl->m_Inventory[num]->m_Effects[i].m_Affects;
		int eff_id = girl->m_Inventory[num]->m_Effects[i].m_EffectID;
		int amount = girl->m_Inventory[num]->m_Effects[i].m_Amount;
		int duration = girl->m_Inventory[num]->m_Effects[i].m_Duration;

		if (girl->m_Inventory[num]->m_Special == sInventoryItem::Temporary)
		{
			switch (affects)
			{
			case sEffect::Nothing:
				break;
			case sEffect::GirlStatus:
				/*
				 *				I can understand ignoring "Nothing"
				 *				Ought "GirlStatus" to have an effect?
				 *				Seem to remember this cropping up on the forum
	"None",		"Poisoned",		"Badly Poisoned",	"Pregnant",		"Pregnant By Player",
	"Slave",		"Has Daughter",	"Has Son",		"Inseminated",		"Controlled",
	"Catacombs",	"Arena",		"Your Daughter"
				 */



				break;

			case sEffect::Skill:			// affects skill
				girl->upd_temp_skill(eff_id, amount);
				break;

			case sEffect::Stat:			// affects skill
				girl->upd_temp_stat(eff_id, amount);
				break;

			case sEffect::Trait:			// affects skill
				/*
				 *	WD:	New logic for remembering traits
				 *		moved to AddTrait() RemoveTrait() fn's
				 *
				 *		EQUIP Temporary Item
				 */
				if (amount == 0)						// remove trait temporarily from equiping an item	
					g_Girls.RemoveTrait(girl, girl->m_Inventory[num]->m_Effects[i].m_Trait, true);		// addrememberlist = true Temporary Item trait removal

				else if (amount == 1)		// add temporary trait
					g_Girls.AddTrait(girl, girl->m_Inventory[num]->m_Effects[i].m_Trait, duration, true);	// Temp = true Temporary Item, removeitem = true for Temporary Item trait addition

				if (girl->m_Inventory[num]->m_Effects[i].m_Trait == "Virgin")
				{
					girl->m_Virgin = (amount == 1);
				}
			}
		}
		else	// m_Special == sInventoryItem::None
		{
			/* */if (affects == sEffect::Skill)
			{
				// `J` food and makeup are single use items, so if permanent, make them affect the base skill
				if (girl->m_Inventory[num]->m_Type == INVFOOD || girl->m_Inventory[num]->m_Type == INVMAKEUP)
					g_Girls.UpdateSkill(girl, eff_id, amount);
				// `J` all other items can be removed so use skill mod
				else g_Girls.UpdateSkillMod(girl, eff_id, amount);
			}
			else if (affects == sEffect::Stat)
			{
				// `J` food and makeup are single use items, so if permanent, make them affect the base skill
				if (girl->m_Inventory[num]->m_Type == INVFOOD || girl->m_Inventory[num]->m_Type == INVMAKEUP)
					g_Girls.UpdateStat(girl, eff_id, amount);
				// `J` all other items can be removed so use skill mod
				else g_Girls.UpdateStatMod(girl, eff_id, amount);
			}
			else if (affects == sEffect::GirlStatus)	// adds/removes status
			{
				if (amount >= 1)	// add status
				{
					if (eff_id == STATUS_PREGNANT || eff_id == STATUS_PREGNANT_BY_PLAYER || eff_id == STATUS_INSEMINATED)
					{
						/* does this item impregnate/inseminate?
						 * If so, m_Amount is used to determine chance...
						 * 1 is normal (8% default),
						 * 10 would give 80% chance (default config)
						 */
						if (!g_Girls.HasTrait(girl, "Sterile") && !girl->is_pregnant())
						{
							cPlayer *player = g_Brothels.GetPlayer();
							bool AntiPreg = girl->m_UseAntiPreg;
							girl->m_UseAntiPreg = false;  // we don't want contraceptives automatically used, eh?
							if (eff_id == STATUS_PREGNANT_BY_PLAYER)
							{
								bool preg = !girl->calc_pregnancy(player, false, (double)amount);
								if (preg)
									g_MessageQue.AddToQue(girl->m_Realname + gettext(": ") +
									girl->m_Inventory[num]->m_Name +
									gettext(": The use of this item has impregnated her with your seed."), COLOR_BLUE);
								else
									g_MessageQue.AddToQue(girl->m_Realname + gettext(": ") +
									girl->m_Inventory[num]->m_Name +
									gettext(": The use of this item has failed to impregnate her with your seed. Better luck next time."), COLOR_BLUE);
							}
							if (eff_id == STATUS_PREGNANT)
							{
								sBrothel* brothel = g_Brothels.GetBrothel(0);
								sCustomer Cust;
								g_Customers.GetCustomer(Cust, brothel);
								bool preg = !girl->calc_pregnancy(&Cust, false, (double)amount);
								if (preg)
									g_MessageQue.AddToQue(girl->m_Realname + gettext(": ") +
									girl->m_Inventory[num]->m_Name +
									gettext(": The use of this item has impregnated her."), COLOR_BLUE);
								else
									g_MessageQue.AddToQue(girl->m_Realname + gettext(": ") +
									girl->m_Inventory[num]->m_Name +
									gettext(": The use of this item has failed to impregnate her. Better luck next time."), COLOR_BLUE);
							}
							if (eff_id == STATUS_INSEMINATED)
							{
								bool preg = !girl->calc_insemination(player, false, (double)amount);
								if (preg)
									g_MessageQue.AddToQue(girl->m_Realname + gettext(": ") +
									girl->m_Inventory[num]->m_Name +
									gettext(": The use of this item has inseminated her with the seed of a beast."), COLOR_BLUE);
								else
									g_MessageQue.AddToQue(girl->m_Realname + gettext(": ") +
									girl->m_Inventory[num]->m_Name +
									gettext(": The use of this item has failed to inseminate her. Better luck next time."), COLOR_BLUE);
							}
							girl->m_UseAntiPreg = AntiPreg;
						}
					}
					else girl->m_States |= (1 << eff_id);
				}
				else if (amount == 0)	// remove status
				{
					bool preg = girl->is_pregnant();
					if (girl->m_States&(1 << eff_id))
						girl->m_States &= ~(1 << eff_id);
					if (eff_id == STATUS_PREGNANT ||
						eff_id == STATUS_PREGNANT_BY_PLAYER ||
						eff_id == STATUS_INSEMINATED)
					{
						if (preg)
						{
							girl->m_ChildrenCount[CHILD09_ABORTIONS]++;
							g_MessageQue.AddToQue(girl->m_Realname + gettext(": ") + girl->m_Inventory[num]->m_Name +
								gettext(": The use of this item has ended her pregnancy."), COLOR_RED);
						}
						else
						{
							g_MessageQue.AddToQue(girl->m_Realname + gettext(": ") + girl->m_Inventory[num]->m_Name +
								gettext(": She was not pregnant before the use of this item."), COLOR_RED);
						}
						if (!g_Girls.HasTrait(girl, "Sterile"))
						{
							if (g_Dice.percent(preg ? 1 : 3))
							{
								g_MessageQue.AddToQue(girl->m_Realname + gettext(": ") + girl->m_Inventory[num]->m_Name +
									gettext(": The use of this item has made her sterile, she can no longer have children."), COLOR_RED);
								g_Girls.AddTrait(girl, "Sterile");
							}
						}
					}
				}
			}
			else if (affects == sEffect::Trait)	// trait
			{
				/*
				 *	WD:	New logic for remembering traits
				 *		moved to AddTrait() RemoveTrait() fn's
				 *
				 *		EQUIP Normal Item
				 */
				string trait = girl->m_Inventory[num]->m_Effects[i].m_Trait; // avoids repeat calls

				if (amount == 0)				// remove trait from equiping an item
				{
					//SIN: I know it's kinda player's fault anyway, but given cost of medicines and difficulty to
					//source player should get some kindness rep for this. Prob cheaper and easier to let them die
					//and replace, but if we get here, player is looking out for staff.
					if (trait == "AIDS" || trait == "Syphilis" || trait == "Herpes" || trait == "Chlamydia")
					{
						cPlayer * player = g_Brothels.GetPlayer();
						if (girl->is_slave())
						{		//SIN: just protecting investment in property
							if (trait == "AIDS") player->evil(-4);
							if (trait == "Syphilis") player->evil(-3);
							if (trait == "Herpes") player->evil(-2);
							if (trait == "Chlamydia") player->evil(-1);
						}
						else   //SIN: a genuinely kind act to support staff
						{
							if (trait == "AIDS") player->evil(-8);
							if (trait == "Syphilis") player->evil(-6);
							if (trait == "Herpes") player->evil(-4);
							if (trait == "Chlamydia") player->evil(-2);
						}
					}
					g_Girls.RemoveTrait(girl, trait, girl->m_Inventory[num]->m_Type != INVFOOD && girl->m_Inventory[num]->m_Type != INVMAKEUP);		// addrememberlist = true only if not consumable
				}
				else if (amount == 1)			// add normal trait	from equiping an item
				{
					g_Girls.AddTrait(girl, trait, false, girl->m_Inventory[num]->m_Type != INVFOOD && girl->m_Inventory[num]->m_Type != INVMAKEUP);		// Temp = false Normal Item, removeitem = true only if not consumable
				}
				if (trait == "Virgin")
				{
					girl->m_Virgin = (amount == 1);
				}
			}
		}
	}

	// if consumable then remove from inventory
	if (girl->m_Inventory[num]->m_Type == INVFOOD || girl->m_Inventory[num]->m_Type == INVMAKEUP)
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
	if (girl->m_EquipedItems[num] == 0) return;	// if already unequiped do nothing
	// unapply the effects
	for (u_int i = 0; i < girl->m_Inventory[num]->m_Effects.size(); i++)
	{
		int eff_id = girl->m_Inventory[num]->m_Effects[i].m_EffectID;
		int affects = girl->m_Inventory[num]->m_Effects[i].m_Affects;
		int amount = girl->m_Inventory[num]->m_Effects[i].m_Amount;

		/* */if (affects == sEffect::Skill)	g_Girls.UpdateSkillMod(girl, eff_id, -amount);
		else if (affects == sEffect::Stat)	g_Girls.UpdateStatMod(girl, eff_id, -amount);
		else if (affects == sEffect::GirlStatus)	// adds/removes status
		{
			if (amount == 1) girl->m_States &= ~(1 << eff_id);		// add status
			else if (amount == 0) girl->m_States |= (1 << eff_id);	// remove status
		}
		else if (affects == sEffect::Trait)	// trait
		{
			/*
			 *	WD:	New logic for remembering traits
			 *		moved to AddTrait() RemoveTrait() fn's
			 *
			 *		UNEQUIP
			 */
			if (amount == 0 && girl->m_Inventory[num]->m_Effects[i].m_Trait == "Virgin")
			{
				girl->m_Virgin = 0; // `J` unequiping an item will not make her a virgin again
			}
			else if (amount == 0)					// possibly add remembered trait from unequiping an item
				g_Girls.AddTrait(girl, girl->m_Inventory[num]->m_Effects[i].m_Trait, false, false, true);	// inrememberlist = true Add trait only if it is in the rememebered list

			else if (amount == 1)				// remove item trait from unequiping an item
				g_Girls.RemoveTrait(girl, girl->m_Inventory[num]->m_Effects[i].m_Trait);
		}
	}
	// set it as unequiped
	girl->m_EquipedItems[num] = 0;

	g_Girls.CalculateGirlType(girl);
}

void cInventory::Equip(sGirl* girl, sInventoryItem* item, bool force)
{
	// this function is only used for global effects sInventoryItem::AffectsAll = 1
	if (item->m_Special != sInventoryItem::AffectsAll) return;
	for (u_int i = 0; i < item->m_Effects.size(); i++)
	{
		int eff_id = item->m_Effects[i].m_EffectID;
		int affects = item->m_Effects[i].m_Affects;
		int amount = item->m_Effects[i].m_Amount;

		/* */if (affects == sEffect::Skill)	g_Girls.UpdateSkillMod(girl, eff_id, amount);
		else if (affects == sEffect::Stat)	g_Girls.UpdateStatMod(girl, eff_id, amount);

		else if (affects == sEffect::Trait)	// trait
		{
			if (amount == 0)			// remove trait temporarily from equiping an item							
				g_Girls.RemoveTrait(girl, item->m_Effects[i].m_Trait, true);	// addrememberlist = true AffectAll trait removal

			else if (amount == 1)		// add temporary trait 					
				g_Girls.AddTrait(girl, item->m_Effects[i].m_Trait, 20, true); // Temp = true AffectAll Item, removeitem = true for AffectAll trait

			if (item->m_Effects[i].m_Trait == "Virgin")
			{
				girl->m_Virgin = (amount == 1);
			}
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
	 *	there's a limited number of items that can be equipped for any given type
	 *	so we're going to loop through the inventory counting the item in question
	 */
	for (int i = 0; i < 40 && count < limit; i++)
	{
		if (girl->m_Inventory[i] == 0 ||					// if there's nothing in the slot or
			girl->m_Inventory[i]->m_Type != target_type ||	// if there's something in the slot but it's not the correct type or
			girl->m_EquipedItems[i] != 1)					// if it is the target type but she does not have it equipped,
			continue;										// Skip it

		count++;			// All right, all right. it's the right type, and it's equipped add it to the count.
		/*
		 *		now if we're not forcing her to wear this then at this point we can skip on to the next item,
		 *
		 *		for that matter, if we are forcing her, but the count is less than the limit, we might still find a free slot
		 *		so we skip on then, too
		 */
		if (!force || count < limit) continue;
		/*
		 *		If we get here, we have no free slots, and we're forcing her to wear it (Muh-ha-ha-ha-ha!)
		 *		so we need to remove this last ring, decrement the count and exit the loop
		 */
		count--;
		Unequip(girl, i);
		break;
	}
	if (count == limit) return false;
	return true;
}

bool cInventory::ok_2_equip(sGirl *girl, int num, bool force)
{
	// first check if can equip it
	switch (girl->m_Inventory[num]->m_Type)
	{
	case sInventoryItem::Ring:
		if (equip_ring_ok(girl, num, force) == false) return false;
		break;
	case sInventoryItem::Dress:
	case sInventoryItem::Shoes:
	case sInventoryItem::Necklace:
	case sInventoryItem::Armor:
	case sInventoryItem::Underwear:
	case sInventoryItem::Hat:
	case sInventoryItem::Helmet:
	case sInventoryItem::Glasses:
	case sInventoryItem::Swimsuit:
	case sInventoryItem::Combatshoes:
	case sInventoryItem::Shield:
		if (equip_singleton_ok(girl, num, force) == false) return false;
		break;
	case sInventoryItem::SmWeapon:	// doc: added missing enum
	case sInventoryItem::Weapon:
	case sInventoryItem::Armband:
		if (equip_pair_ok(girl, num, force) == false) return false;
		break;
	case sInventoryItem::Food:
	case sInventoryItem::Makeup:
	case sInventoryItem::Misc:
	default:
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
	if (pt = el->Attribute("Name"))						item->m_Name = pt;
	if (pt = el->Attribute("Desc"))						item->m_Desc = pt; else cout << "no desc attribute found" << endl;
	if (pt = el->Attribute("Type"))						item->set_type(pt);
	if (pt = el->Attribute("Badness", &ival))			item->m_Badness = ival;
	else 												item->m_Badness = 0;
	if (pt = el->Attribute("GirlBuyChance", &ival))		item->m_GirlBuyChance = ival;
	else  // if not specified, set chance based on badness
		item->m_GirlBuyChance = (item->m_Badness < 20) ? (100 - (item->m_Badness * 5)) : 0;
	if (pt = el->Attribute("Special"))					item->set_special(pt);
	if (pt = el->Attribute("Cost", &ival))				item->m_Cost = ival;
	if (pt = el->Attribute("Rarity"))					item->set_rarity(pt);
	if (pt = el->Attribute("Infinite"))					item->m_Infinite = ((string(pt) == "true") || (string(pt) == "True"));
	else												item->m_Infinite = false;
	do_effects(el, item);
	//	do_tests(el, item);		//	`J` will be added in the future (hopefully)
	return item;
}

bool cInventory::LoadItemsXML(string filename)
{
	TiXmlDocument doc(filename);
	if (!doc.LoadFile())
	{
		g_LogFile.os() << "can't load item file " << filename << endl;
		g_LogFile.os() << "Error: line " << doc.ErrorRow() << ", col " << doc.ErrorCol() << ": " << doc.ErrorDesc() << endl;
		return false;
	}
	TiXmlElement *el;
	TiXmlElement *root_el = doc.RootElement();
	cConfig cfg;
	bool log_flag = cfg.debug.log_items();
	for (el = root_el->FirstChildElement(); el; el = el->NextSiblingElement())
	{
		sInventoryItem* item = handle_element(el);
		if (log_flag) g_LogFile.os() << *item << endl;
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
