/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders 
 * who meet on http://pinkpetal.co.cc
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

#ifndef __CCLINIC_H
#define __CCLINIC_H

#include "cBrothel.h"
#include "cGold.h"

using namespace std;

// defines a single clinic
typedef struct sClinic : sBrothel
{
	sClinic();							// constructor
	~sClinic();						// destructor
	unsigned short	m_var;	// customers used for temp purposes but must still be taken into account
	cGold			m_Finance;
	
	TiXmlElement* SaveClinicXML(TiXmlElement* pRoot);
	bool LoadClinicXML(TiXmlHandle hBrothel);

}sClinic;


/*
 * manages the clinic
 *
 * extend cBrothelManager
 */
class cClinicManager : public cBrothelManager
{
public:
	cClinicManager();					// constructor
	~cClinicManager();					// destructor

	void AddGirl(int brothelID, sGirl* girl);
	void RemoveGirl(int brothelID, sGirl* girl, bool deleteGirl = false);	// Removes a girl from the list (only used with editor where all girls are available)
	void UpdateClinic();
	void UpdateGirls(sBrothel* brothel, int DayNight);
	//void	AddBrothel(sBrothel* newBroth);
	TiXmlElement* SaveDataXML(TiXmlElement* pRoot);
	bool LoadDataXML(TiXmlHandle hBrothelManager);
	void Free();
	int m_NumClinics;
	cJobManager m_JobManager;
	//Aika Edit
	//sGirl* GetGirl(int brothelID, int num);
	//sClinic* m_Parent;
};


#endif  /* __CCLINIC_H */
