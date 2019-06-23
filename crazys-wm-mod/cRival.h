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
#ifndef __CRIVAL_H
#define __CRIVAL_H

#include <string>
#include "Constants.h"
#include "cInventory.h"

#include "cNameList.h"
#include "tinyxml.h"
using namespace std;

class cRival
{
public:
	cRival()
	{
		m_Next = m_Prev = 0;
		m_Name = "";
		m_Power = 0;					// `J` added
		m_Influence = 0;
		m_BribeRate = 0;
		m_Gold = 5000;
		m_NumBrothels = 1;
		m_NumGangs = 3;
		m_NumGirls = 8;
		m_NumBars = 0;
		m_NumGamblingHalls = 0;
		m_BusinessesExtort = 0;
		m_Inventory[MAXNUM_RIVAL_INVENTORY];
	}
	~cRival()
	{
		if (m_Next) delete m_Next;
		m_Next = 0;
		m_Prev = 0;
	}

	// variables
	string m_Name;
	int m_Power;						// `J` added
	int m_NumGangs;
	int m_NumBrothels;
	int m_NumGirls;
	int m_NumBars;
	int m_NumGamblingHalls;
	long m_Gold;
	int m_BusinessesExtort;
	long m_BribeRate;
	int m_Influence;	// based on the bribe rate this is the percentage of influence you have
	int m_NumInventory;										// current amount of inventory the brothel has
	sInventoryItem* m_Inventory[MAXNUM_RIVAL_INVENTORY];	// List of inventory items they have (40 max)


	cRival* m_Next;
	cRival* m_Prev;
};

class cRivalManager
{
public:
	cRivalManager();
	~cRivalManager()
	{
		Free();
	}
	void Free()
	{
		if (m_Rivals) delete m_Rivals;
		m_Rivals = 0;
		m_Last = 0;
		m_NumRivals = 0;
	}
	void Update(int& NumPlayerBussiness);
	cRival* GetRandomRival();
	cRival* GetRandomRivalWithGangs();
	cRival* GetRandomRivalWithBusinesses();
	cRival* GetRandomRivalToSabotage();
	cRival* GetRivals() { return m_Rivals; }
	cRival* GetRival(string name);
	cRival* GetRival(int number);
	TiXmlElement* SaveRivalsXML(TiXmlElement* pRoot);
	bool LoadRivalsXML(TiXmlHandle hRivalManager);
	void CreateRival(long bribeRate, int extort, long gold, int bars, int gambHalls, int Girls, int brothels, int gangs, int age);
	void AddRival(cRival* rival);
	void RemoveRival(cRival* rival);
	void CreateRandomRival();
	void check_rivals();		// `J` moved from cBrothel
	string new_rival_text();	// `J` moved from cBrothel
	void peace_breaks_out();	// `J` moved from cBrothel


	// `J` New - rival inventory
	int AddRivalInv(cRival* rival, sInventoryItem* item);	// add item
	bool RemoveRivalInvByNumber(cRival* rival, int num);	// remove item
	void SellRivalInvItem(cRival* rival, int num);		// sell item
	sInventoryItem* GetRandomRivalItem(cRival* rival);
	sInventoryItem* GetRivalItem(cRival* rival, int num);
	int GetRandomRivalItemNum(cRival* rival);


	int GetNumBusinesses();
	int GetNumRivals()				{ return m_NumRivals; }
	int GetNumRivalGangs();
	bool NameExists(string name);
	bool player_safe()				{ return m_PlayerSafe; }
	cRival* get_influential_rival();
	string rivals_plunder_pc_gold(cRival* rival);
private:
	int m_NumRivals;
	cRival* m_Rivals;
	cRival* m_Last;
	bool m_PlayerSafe;
	cDoubleNameList names;
};

#endif
