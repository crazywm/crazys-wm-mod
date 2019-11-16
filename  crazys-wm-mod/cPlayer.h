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

#pragma once

#ifndef __CPLAYER_H
#define __CPLAYER_H

#include <fstream>
#include "Constants.h"
#include "tinyxml.h"
#include "sConfig.h"
#include "src/Inventory.hpp"

class sGirl;


class cPlayer
{
private:
	int Limit100(int nStat);								// Limit stats to -100 to 100
	int	Scale200(int nStatMod, int nCurrentStatValue);		// Scale stat from 1 to nStatMod
/*
 *	the suspicion level of the authorities.
 *	-100 means they are on players side
 *	+100 means they will probably raid his brothels
 */
	int m_Suspicion;	
/*
 *	How good or evil the player is considered to be:
 *	-100 is evil while +100 is a saint
 */
	int m_Disposition;
/*
 *	how much the customers fear you:
 *	-100 is not at all while 100 means a lot
 */
	int m_CustomerFear;


	int m_BirthYear  = 1190;	// the game starts in year 1209 so default start age is 18
	int m_BirthMonth = 1;	    // there are 12 month in the year
	int m_BirthDay   = 1;	    // there are 30 days in every month

	std::string m_Title;		// basic title - need to add more titles with more power gained
	std::string m_FirstName;	// no first name
	std::string m_Surname;	// basic surname
	std::string m_RealName;	// m_FirstName + " " + m_Surname

	int m_PlayerGender;	// `J` added - not going to be changeable yet but adding it in for later

	Inventory m_Inventory;

public:
	
	std::string Title()		{ return m_Title; }
	std::string FirstName()	{ return m_FirstName; }
	std::string Surname()	{ return m_Surname; }
	std::string RealName()	{ return m_RealName; }

	std::string SetTitle(std::string title);
	std::string SetFirstName(std::string firstname);
	std::string SetSurname(std::string surname);
	std::string SetRealName(std::string realname);

	int Gender()		{ return m_PlayerGender; }
	void SetGender(int x);
	void AdjustGender(int male, int female);
	bool CanImpregnateFemale();
	bool CanCarryOwnBaby();
	bool CanCarryNormalBaby();
	bool HasPenis();
	bool HasVagina();
	bool HasTestes();
	bool HasOvaries();


	bool m_WinGame;
	int m_Skills[NUM_SKILLS];
	int m_Stats[NUM_STATS];

	cPlayer();				// constructor
	void SetToZero();

	TiXmlElement* SavePlayerXML(TiXmlElement* pRoot);
	bool LoadPlayerXML(TiXmlHandle hPlayer);

	int disposition()		{ return m_Disposition; }
	int disposition(int n);
	int evil(int n);
	int suspicion()			{ return m_Suspicion; }
	int suspicion(int n);
	int customerfear()		{ return m_CustomerFear; }
	int customerfear(int n);

	int BirthYear()		{ return m_BirthYear; }
	int BirthMonth()	{ return m_BirthMonth; }
	int BirthDay()		{ return m_BirthDay; }
	int BirthYear(int n);
	int SetBirthYear(int n);
	int SetBirthMonth(int n);
	int SetBirthDay(int n);

	// text helpers
    std::string disposition_text() const;
    std::string suss_text() const;

    bool Combat(sGirl* girl);	// returns true if the girl wins

    Inventory& inventory();
    /// automatically use items on the given girl
    void apply_items(sGirl& girl);
    bool AutomaticFoodItemUse(sGirl& girl, const char* item_name, string message);
    bool AutomaticItemUse(sGirl& girl, const char* item_name, string message);
    bool AutomaticSlotlessItemUse(sGirl& girl, const char* item_name, string message);
};


#endif	/* __CPLAYER_H */