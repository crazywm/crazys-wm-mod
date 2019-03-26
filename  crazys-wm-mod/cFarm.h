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

#ifndef __CFARM_H
#define __CFARM_H

#include "cBrothel.h"
#include "cGold.h"

using namespace std;

// defines a single farm
typedef struct sFarm : sBrothel
{
	sFarm();							// constructor
	~sFarm();						// destructor
	unsigned short	m_var;	// customers used for temp purposes but must still be taken into account
	cGold			m_Finance;
	
	TiXmlElement* SaveFarmXML(TiXmlElement* pRoot);
	bool LoadFarmXML(TiXmlHandle hBrothel);

}sFarm;


/*
 * manages the farm
 *
 * extend cBrothelManager
 */
class cFarmManager : public cBrothelManager
{
public:
	cFarmManager();					// constructor
	~cFarmManager();					// destructor

	void AddGirl(int brothelID, sGirl* girl, bool keepjob = false);
	void RemoveGirl(int brothelID, sGirl* girl, bool deleteGirl = false);	// Removes a girl from the list (only used with editor where all girls are available)
	void UpdateFarm();
	void UpdateGirls(sBrothel* brothel, bool Day0Night1);
	//void	AddBrothel(sBrothel* newBroth);
	TiXmlElement* SaveDataXML(TiXmlElement* pRoot);
	bool LoadDataXML(TiXmlHandle hBrothelManager);
	void Free();
	int m_NumFarms;
	cJobManager m_JobManager;

};


#endif  /* __CFARM_H */
