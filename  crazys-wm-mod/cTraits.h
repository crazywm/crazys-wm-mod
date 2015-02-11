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
// Note: should implement hash table latter
#ifndef __CTRAITS_H
#define __CTRAITS_H

#include "Constants.h"
#include <string>
#include <fstream>
using namespace std;

// Represents a single trait
typedef struct sTrait
{
	char*	m_Name = 0;				// the name and unique ID of the trait
	char*	m_Desc = 0;				// a description of the trait
	char*	m_Type = 0;				// a description of the trait
	int		m_InheritChance = -1;	// chance of inheriting the trait
	int		m_RandomChance = -1;	// chance of a random girl to get the trait
	sTrait* m_Next;		// the next trait in the list

	sTrait()
	{
		m_Name = m_Desc = m_Type = 0;
		m_InheritChance = -1;
		m_RandomChance = -1;
		m_Next = 0;
	}

	~sTrait()
	{
		if (m_Name) delete[] m_Name;
		m_Name = 0;
		if (m_Desc) delete[] m_Desc;
		m_Desc = 0;
		if (m_Type) delete[] m_Type;
		m_Type = 0;
		if (m_Next) delete m_Next;
		m_Next = 0;
		m_InheritChance = 0;
		m_RandomChance = 0;

	}
}sTrait;

// Manages and loads the traits file
class cTraits
{
public:
	cTraits(){m_ParentTrait=0;m_LastTrait=0;m_NumTraits=0;}
	~cTraits();

	void Free();	// Delete all the loaded data

	void LoadTraits(string filename);		// Loads the traits from a file (adding them to the existing traits)
	void LoadXMLTraits(string filename);	// Loads the traits from an XML file (adding them to the existing traits)
	void SaveTraits(string filename);	// Saves the traits to a file

	void AddTrait(sTrait* trait);
	void RemoveTrait(string name);
	sTrait* GetTrait(string name);
	sTrait* GetTraitNum(int num);
	int GetNumTraits() {return m_NumTraits;}

	int GetInheritChance(sTrait* trait)	{ return trait->m_InheritChance; }
	int GetRandomChance(sTrait* trait)	{ return trait->m_RandomChance; }

	string GetTranslateName(string name);

private:
	int		m_NumTraits;
	sTrait* m_ParentTrait;				// the first trait in the list
	sTrait* m_LastTrait;				// the last trait in the list
};

#endif
