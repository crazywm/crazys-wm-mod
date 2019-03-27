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
#ifndef __CINVENTORY_H
#define __CINVENTORY_H

// includes
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

#include "Constants.h"

using namespace std;

struct sGirl;

struct sEffect
{
	// MOD docclox
/*
 *	let's have an enum for possible values of m_Affects
 */
	enum What {
		Skill		= 0,
		Stat		= 1,
		Nothing		= 2,
		GirlStatus	= 3,
		Trait		= 4,
		Enjoy		= 5
	};
	What m_Affects;
/*
 *	define an ostream operator so it will pretty print
 *	(more useful for debugging than game play
 *	but why take it out?)
 */
	friend ostream& operator << (ostream& os, sEffect::What &w);
/*
 *	and a function to go the other way
 *	we need this to turn the strings in the xml file
 *	into numbers
 */
	void set_what(string s) {
		if		(s == "Skill")		m_Affects = Skill;
		else if	(s == "Stat")		m_Affects = Stat;
		else if	(s == "Nothing")	m_Affects = Nothing;
		else if	(s == "GirlStatus")	m_Affects = GirlStatus;
		else if (s == "Trait")		m_Affects = Trait;
		else if (s == "Enjoy")		m_Affects = Enjoy;
		else {
			m_Affects = Nothing;
			cerr << "Error: Bad 'what' string for item effect: '" << s << "'" << endl;
		}
	}
/*
 *	can't make an enum for this since it can represent
 *	different quantites.
 *
 *	The OO approach would be to write some variant classes, I expect
 *	but really? Life's too short...
 */
	unsigned char m_EffectID;	// what stat, skill or status effect it affects
/*
 *	but I still need strings for the skills, states, traits and so forth
 *
 *	these should be (were until the merge) in sGirl. Will be again
 *	as soon as I sort the main mess out...
 */
	const char *girl_status_name(unsigned int id);
	const char *skill_name(unsigned int id);		// WD:	Use definition in sGirl::
	const char *stat_name(unsigned int id);			// WD:	Use definition in sGirl::
	const char *enjoy_name(unsigned int id);		// `J`	Use definition in sGirl::

/*
 *	and we need to go the other way,
 *	setting m_EffectID from the strings in the XML file
 *
 *	WD:	Change to use definition and code in sGirl::
 *		remove duplicated code
 */
	bool set_skill(string s);
	bool set_girl_status(string s);
	bool set_stat(string s);
	bool set_Enjoyment(string s);


/*
 *	magnitude of the effect. 
 *	-10 will subtract 10 from the target stat while equiped
 *	and add 10 when unequiped.
 *
 *	With status effects and traits 1 means add,
 *	0 means take away and 2 means disable
 */
	int m_Amount;
	
	int m_Duration;	// `J` added for temporary trait duration
/*
 *	name of the trait it adds
 */
	string m_Trait;
/*
 *	and a pretty printer for the class as a whole
 *	just a debug thing, really
 */
	friend ostream& operator << (ostream& os, sEffect &eff) 
	{
		os << "Effect: " << eff.m_Affects << " ";
		if (eff.m_Affects == Stat) { os << eff.stat_name(eff.m_EffectID); }
		if (eff.m_Affects == Skill) { os << eff.skill_name(eff.m_EffectID); }
		if (eff.m_Affects == Trait) { os << "'" << eff.m_Trait << "'"; }
		if (eff.m_Affects == GirlStatus) { os << eff.girl_status_name(eff.m_EffectID); }
		if (eff.m_Affects == Enjoy) { os << eff.enjoy_name(eff.m_EffectID); }
		os << (eff.m_Amount > 0 ? " +" : " ") << eff.m_Amount;
		return os << endl;
	}
	// end mod
};

typedef struct sInventoryItem
{
	string m_Name;
	string m_Desc;
	/*
	 *	item type: let's make an enum
	 */
	enum Type {
		Ring = 1,			// worn on fingers (max 8)
		Dress = 2,			// Worn on body, (max 1)
		Shoes = 3,			// worn on feet, (max 1)
		Food = 4,			// Eaten, single use
		Necklace = 5,		// worn on neck, (max 1)
		Weapon = 6,			// equiped on body, (max 2)
		Makeup = 7,			// worn on face, single use
		Armor = 8,			// worn on body over dresses (max 1)
		Misc = 9,			// random stuff. may cause a constant effect without having to be equiped
		Armband = 10,		// (max 2), worn around arms
		SmWeapon = 11,		// (max 2), hidden on body
		Underwear = 12,		// (max 1) worn under dress
		Hat = 13,			// CRAZY added this - Noncombat worn on the head (max 1)
		Helmet = 14,		// CRAZY added this	- Combat worn on the head (max 1)
		Glasses = 15,		// CRAZY added this	- Glasses (max 1)
		Swimsuit = 16,		// CRAZY added this - Swimsuit (max 1 in use but can have as many as they want)
		Combatshoes = 17,	// `J`   added this - Combat Shoes (max 1) often unequipped outside of combat
		Shield = 18			// `J`   added this - Shields (max 1) often unequipped outside of combat
	};
	Type m_Type;
	/*
	 *	and another for special values
	 */
	enum Special {
		None = 0,
		AffectsAll = 1,
		Temporary = 2
	};
	Special m_Special;
	/*
	 *	if 1 then this item doesn't run out if stocked in shop inventory
	 */
	bool m_Infinite;
	/*
	 *	the number of effects this item has
	 */
	vector<sEffect> m_Effects;
	/*
	 *	how much the item is worth?
	 */
	long m_Cost;
	/*	0 is good, while badness > is evil.
	 *	Girls may fight back if badness > 0,
	 *	Girls won't normally buy items > 20 on their own
	 *      default formula is -5% chance to buy on their own per Badness point (5 Badness = 75% chance)
	 *	Girls with low obedience may take off something that is bad for them
	 */
	unsigned char m_Badness;
	unsigned char m_GirlBuyChance;  // chance that a girl on break will buy this item if she's looking at it in the shop
	/*
	 *	0 means common,
	 *	1 means 50% chance of appearing in shops,
	 *	2 means 25% chance,
	 *	3 means 5% chance and
	 *	4 means only 15% chance of being found in catacombs,
	 *	5 means ONLY given in scripts and
	 *	6 means same as 5 but also may be given as a reward for objective
	 7 means only 5% chance in catacombs (catacombs05)
	 8 means only 1% chance in catacombs (catacombs01)
	 */
	enum Rarity {
		Common = RARITYCOMMON,			// old 0
		Shop50 = RARITYSHOP50,			// old 1
		Shop25 = RARITYSHOP25,			// old 2
		Shop05 = RARITYSHOP05,			// old 3
		Catacomb15 = RARITYCATACOMB15,		// old 4
		Catacomb05 = RARITYCATACOMB05,		// old 7  // MYR: Added 05 and 01 for the really, really valuable things like invulnerability
		Catacomb01 = RARITYCATACOMB01,		// old 8
		ScriptOnly = RARITYSCRIPTONLY,		// old 5
		ScriptOrReward = RARITYSCRIPTORREWARD	// old 6
	};
	Rarity m_Rarity;

	void set_rarity(string s)
	{
		/* */if (s == "Common")			{ m_Rarity = Common; }
		else if (s == "Shop50")			{ m_Rarity = Shop50; }
		else if (s == "Shop25")			{ m_Rarity = Shop25; }
		else if (s == "Shop05")			{ m_Rarity = Shop05; }
		else if (s == "Catacomb15")		{ m_Rarity = Catacomb15; }
		else if (s == "Catacomb05")		{ m_Rarity = Catacomb05; }
		else if (s == "Catacomb01")		{ m_Rarity = Catacomb01; }
		else if (s == "ScriptOnly")		{ m_Rarity = ScriptOnly; }
		else if (s == "ScriptOrReward") { m_Rarity = ScriptOrReward; }
		else { cerr << "Error in set_rarity: unexpected value '" << s << "'" << endl; m_Rarity = Shop05; }	// what to do?
	}

	void set_special(string s)
	{
		/* */if (s == "None")			{ m_Special = None; }
		else if (s == "AffectsAll")		{ m_Special = AffectsAll; }
		else if (s == "Temporary")		{ m_Special = Temporary; }
		else { m_Special = None; cerr << "unexpected special string: '" << s << "'" << endl; }
	}

	// `J` Incomplete Craftable code - commenting out
#if 0
	enum Craftable {
		No = 0,
		Any = 1,
		Baker = JOB_BAKER,
		Blacksmith = JOB_BLACKSMITH,
		Brewer = JOB_BREWER,
		Butcher = JOB_BUTCHER,
		MakeItem = JOB_MAKEITEM,
		Milker = JOB_MILKER,
	};
	Craftable m_Craftable;	// Who can make it
	int m_CraftLevel;		// Girl Level needed to make it
	int m_CraftCraft;		// Craft skill needed to make it
	int m_CraftStrength;	// Strength needed to make it
	int m_CraftMagic;		// Magic skill needed to make it
	int m_CraftIntel;		// Intelligence needed to make it
	int m_CraftPoints;		// Craft points needed to make it

	void set_craftable(string s)
	{
		/* */if (s == "No" || s == "no" || s == "False" || s == "false")	{ m_Craftable = No; }
		else if (s == "Any")			{ m_Craftable = Any; }
		else if (s == "Baker")			{ m_Craftable = Baker; }
		else if (s == "Blacksmith")		{ m_Craftable = Blacksmith; }
		else if (s == "Brewer")			{ m_Craftable = Brewer; }
		else if (s == "Butcher")		{ m_Craftable = Butcher; }
		else if (s == "MakeItem")		{ m_Craftable = MakeItem; }
		else if (s == "Milker")			{ m_Craftable = Milker; }
		else { cerr << "Error in set_craftable: unexpected value '" << s << "'" << endl; m_Craftable = No; }	// what to do?
	}
#endif

	void set_type(string s)
	{
		/* */if (s == "Ring")							{ m_Type = Ring; }
		else if (s == "Dress")							{ m_Type = Dress; }
		else if (s == "Under Wear" || s == "Underwear")	{ m_Type = Underwear; }
		else if (s == "Shoes")							{ m_Type = Shoes; }
		else if (s == "Food")							{ m_Type = Food; }
		else if (s == "Necklace")						{ m_Type = Necklace; }
		else if (s == "Weapon")							{ m_Type = Weapon; }
		else if (s == "Small Weapon")					{ m_Type = SmWeapon; }
		else if (s == "Makeup")							{ m_Type = Makeup; }
		else if (s == "Armor")							{ m_Type = Armor; }
		else if (s == "Misc")							{ m_Type = Misc; }
		else if (s == "Armband")						{ m_Type = Armband; }
		else if (s == "Hat")							{ m_Type = Hat; }
		else if (s == "Glasses")						{ m_Type = Glasses; }
		else if (s == "Swimsuit")						{ m_Type = Swimsuit; }
		else if (s == "Helmet")							{ m_Type = Helmet; }
		else if (s == "Shield")							{ m_Type = Shield; }
		else if (s == "Combat Shoes")					{ m_Type = Combatshoes; }
		else if (s == "CombatShoes")					{ m_Type = Combatshoes; }
		else { m_Type = Misc; cerr << "Error: unexpected item type: " << s << endl; }
	}

	friend ostream& operator << (ostream& os, sInventoryItem::Special &spec) {
		switch(spec) {
		case None:				return os << "None";
		case AffectsAll:		return os << "AffectsAll";
		case Temporary:			return os << "Temporary";
		default:	cerr << "error: unexpected special value: " << int(spec) << endl;
			return os << "Error(" << int(spec) << ")";
		}
	}

	friend ostream& operator << (ostream& os, sInventoryItem::Rarity &r) {
		switch(r) {
		case Common:			return os << "Common";
		case Shop50:			return os << "Shops, 50%";
		case Shop25:			return os << "Shops, 25%";
		case Shop05:			return os << "Shops, 05%";
		case Catacomb15:		return os << "Catacombs, 15%";
		case Catacomb05:		return os << "Catacombs, 05%";
		case Catacomb01:		return os << "Catacombs, 01%";
		case ScriptOnly:		return os << "Scripted Only";
		case ScriptOrReward:	return os << "Scripts or Reward";
		default:	cerr << "error: unexpected rarity value: " << int(r) << endl;
			return os << "Error(" << int(r) << ")";
		}
	}

	friend ostream& operator << (ostream& os, sInventoryItem::Type &typ) {
		switch(typ) {
		case Ring:			return os << "Ring";
		case Dress:			return os << "Dress";
		case Underwear:		return os << "Underwear";
		case Shoes:			return os << "Shoes";
		case Food:			return os << "Food";
		case Necklace:		return os << "Necklace";
		case Weapon:		return os << "Weapon";
		case SmWeapon:		return os << "Small Weapon";
		case Makeup:		return os << "Makeup";
		case Armor:			return os << "Armor";
		case Misc:			return os << "Misc";
		case Armband:		return os << "Armband";
		case Hat:			return os << "Hat";
		case Helmet:		return os << "Helmet";
		case Glasses:		return os << "Glasses";
		case Swimsuit:		return os << "Swimsuit";
		case Combatshoes:	return os << "Combat Shoes";
		case Shield:		return os << "Shield";
		default:
			cerr << "Unexpected type value: " << int(typ) << endl;
			return os << "Error";
		}
		cerr << "How the hell did I get here?" << endl;
		return os;
	}

	friend ostream& operator << (ostream& os, sInventoryItem &it) {
		os << "Item: " << it.m_Name << endl;
		os << "Desc: " << it.m_Desc << endl;
		os << "Type: " << it.m_Type << endl;
		os << "Badness: " << int(it.m_Badness) << endl;
		os << "Special: " << it.m_Special << endl;
		os << "Cost: " << it.m_Cost << endl;
		os << "Rarity: " << it.m_Rarity << endl;
		os << "Infinite: " << (it.m_Infinite ? "True" : "False") << endl;
		for(unsigned int i = 0; i < it.m_Effects.size(); i++) {
			sEffect &eff = it.m_Effects[i];

			os << eff;
		}
		return os;
	}
} sInventoryItem;

int itemcompless(const sInventoryItem * ,const sInventoryItem *);
int part(int first,int last,sInventoryItem * curr[]);
void qswap(int,int,sInventoryItem * curr[]);
void qu_sort(int,int,sInventoryItem * curr[]);

class cInventory
{
public:
	cInventory() {
		for(int i = 0; i < NUM_SHOPITEMS; i++) {
			m_ShopItems[i] = 0;
		}
		m_NumShopItems = 0;
	}
	~cInventory();

	void Free();

	void LoadItems(string filename);
	bool LoadItemsXML(string filename);
	void UpdateShop();	// re-randomizes the shops inventory
	sInventoryItem* GetItem(string name);
	sInventoryItem* GetShopItem(int num);
	int GetRandomShopItem();
	sInventoryItem* GetRandomItem();
	sInventoryItem* GetRandomCatacombItem();
	sInventoryItem* GetRandomCraftableItem(sGirl*girl, int job, int points);
	string CraftItem(sGirl*girl, int job, int points);

	int CheckShopItem(string name);	// checks if a item is in shop inventory, returns -1 if not and the id if it is
	sInventoryItem* BuyShopItem(int num);	// removes and returns the item from the shop
	bool GirlBuyItem(sGirl* girl, int ShopItem, int MaxItems, bool AutoEquip);  // girl buys selected item if possible; returns true if bought

	void Equip(sGirl* girl, int num, bool force);
	void Equip(sGirl* girl, sInventoryItem* item, bool force);
	void Unequip(sGirl* girl, int num);

	void AddItem(sInventoryItem* item);
	void CalculateCost(sInventoryItem* newItem);

	int HappinessFromItem(sInventoryItem* item);  // determines how happy a girl will be to receive an item (or how unhappy to lose it)

	void GivePlayerAllItems();

	bool IsItemEquipable(sInventoryItem* item)
	{
		if(item->m_Type != INVMISC)
			return true;
		return false;
	}
	void sort();

	bool	equip_limited_item_ok(sGirl*, int, bool, int);
	bool	equip_pair_ok(sGirl*, int, bool);
	bool	equip_ring_ok(sGirl*, int, bool);
	bool	equip_singleton_ok(sGirl*, int, bool);
	bool	ok_2_equip(sGirl*, int, bool);
	void	remove_trait(sGirl*, int, int);

	/*bool has_dildo()
	{
		return	girl->has_item_j( "Compelling Dildo") != -1) ||
			girl->has_item_j( "Dildo") != -1) ||
			girl->has_item_j( "Studded Dildo") != -1) ||
			girl->has_item_j( "Double Dildo") != -1) ||
			girl->has_item_j( "Dreidel Dildo") != -1);
	}*/


private:
	vector<sInventoryItem *> items;  // Master list of items?
	int m_NumShopItems;	// number of items in the shop
	sInventoryItem* m_ShopItems[NUM_SHOPITEMS];	// pointers to all items, the shop can only hold 30 random items
};

#endif
