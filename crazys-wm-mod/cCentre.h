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

#ifndef __CCENTRE_H
#define __CCENTRE_H

#include "cBrothel.h"
#include "cGold.h"

using namespace std;

// defines a single centre
typedef struct sCentre : sBrothel
{
	sCentre();							// constructor
	~sCentre();						// destructor
	unsigned short	m_var;	// customers used for temp purposes but must still be taken into account
	cGold			m_Finance;
	
	TiXmlElement* SaveCentreXML(TiXmlElement* pRoot);
	bool LoadCentreXML(TiXmlHandle hBrothel);

}sCentre;


/*
 * manages the centre
 *
 * extend cBrothelManager
 */
class cCentreManager : public cBrothelManager
{
public:
	cCentreManager();					// constructor
	~cCentreManager();					// destructor

	int m_Rehab_Patient_Time = 0;		// `J` basically how many Rehab patients the counselors at the centre can take care of

	void AddGirl(int brothelID, sGirl* girl, bool keepjob = false);
	void RemoveGirl(int brothelID, sGirl* girl, bool deleteGirl = false);	// Removes a girl from the list (only used with editor where all girls are available)
	void UpdateCentre();
	void UpdateGirls(sBrothel* brothel, bool Day0Night1);
	//void	AddBrothel(sBrothel* newBroth);
	TiXmlElement* SaveDataXML(TiXmlElement* pRoot);
	bool LoadDataXML(TiXmlHandle hBrothelManager);
	void Free();
	int m_NumCentres;
	cJobManager m_JobManager;
	int GetNumberPatients(bool Day0Night1 = SHIFT_DAY);

};


#endif  /* __CCENTRE_H */
