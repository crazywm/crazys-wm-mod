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

#ifndef __CCENTRE_H
#define __CCENTRE_H

#include "cBrothel.h"
#include "cGirls.h"
#include "cGold.h"

using namespace std;

// defines a single Centre
typedef struct sCentre : sBrothel
{
	sCentre();							// constructor
	~sCentre();						// destructor
	unsigned short	m_var;	// customers used for temp purposes but must still be taken into account
	cGold			m_Finance;

	sGirl*		m_Girls;				// A list of all the girls this place has
	sGirl*		m_LastGirl;
	sCentre*	m_Next;
	sCentre* m_Parent;
	
	TiXmlElement* SaveCentreXML(TiXmlElement* pRoot);
	bool LoadCentreXML(TiXmlHandle hBrothel);

}sCentre;

// Keeps track of girls in the dungeon
typedef struct sCentreGirl
{
	sCentreGirl();						// constructor
	~sCentreGirl();					// destructor

	
	// customer data
	sGirl*			m_Girl;
	sCentreGirl*	m_Next;
	sCentreGirl*	m_Prev;
	void OutputGirlDetailString(string& Data, const string& detailName);
}sCentreGirl;


/*
 * manages the Centre
 *
 * extend cBrothelManager
 */
class cCentreManager : public cBrothelManager
{
public:
	cCentreManager();					// constructor
	~cCentreManager();					// destructor

	void AddGirl(int brothelID, sGirl* girl);
	void RemoveGirl(int brothelID, sGirl* girl, bool deleteGirl = false);	// Removes a girl from the list (only used with editor where all girls are available)
	void UpdateCentre();
	void UpdateGirls(sBrothel* brothel, int DayNight);
	//void	AddBrothel(sBrothel* newBroth);
	TiXmlElement* SaveDataXML(TiXmlElement* pRoot);
	bool LoadDataXML(TiXmlHandle hBrothelManager);
	void Free();
	int m_NumCentres;
	sGirl* GetGirl(int centreID, int num);
	cJobManager m_JobManager;
};


#endif  /* __CCENTRE_H */
