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

using namespace std;

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

public:
	bool m_WinGame;
	unsigned char m_Skills[NUM_SKILLS];
	unsigned char m_Stats[NUM_STATS];

	cPlayer();				// constructor
	void SetToZero();

	TiXmlElement* SavePlayerXML(TiXmlElement* pRoot);
	bool LoadPlayerXML(TiXmlHandle hPlayer);
	void LoadPlayerLegacy(ifstream& ifs);

	int disposition()		{ return m_Disposition; }
	int disposition(int n);
	int evil(int n)			{cConfig cfg;						// `J` add check for if harsher torture is set
		if (cfg.initial.torture_mod() < 0 && n > 0){ n += n; }	// `J` double evil if increasing it BUT NOT IF LOWERING IT
		return disposition(-1 * n); }
	int suspicion()			{ return m_Suspicion; }
	int suspicion(int n);
	int customerfear()		{ return m_CustomerFear; }
	int customerfear(int n);
};


#endif	/* __CPLAYER_H */