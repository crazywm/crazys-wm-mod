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
#include <vector>

using namespace std;


struct tEffect	// `J` copied from cInventory.cpp - .06.01.17
{
	// MOD docclox
	/*
	*	let's have an enum for possible values of m_Affects
	*/
	enum What {
		Skill = 0,
		Stat = 1,
		Nothing = 2,
		GirlStatus = 3,
		Enjoy = 4
	};
	What m_Affects;
	/*
	*	define an ostream operator so it will pretty print
	*	(more useful for debugging than game play
	*	but why take it out?)
	*/
	friend ostream& operator << (ostream& os, tEffect::What &w);
	/*
	*	and a function to go the other way
	*	we need this to turn the strings in the xml file
	*	into numbers
	*/
	void set_what(string s) {
		/* */if (s == "Skill")		m_Affects = Skill;
		else if (s == "Stat")		m_Affects = Stat;
		else if (s == "Nothing")	m_Affects = Nothing;
		else if (s == "GirlStatus")	m_Affects = GirlStatus;
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
	friend ostream& operator << (ostream& os, tEffect &eff)
	{
		os << "Effect: " << eff.m_Affects << " ";
		if (eff.m_Affects == Stat) { os << eff.stat_name(eff.m_EffectID); }
		if (eff.m_Affects == Skill) { os << eff.skill_name(eff.m_EffectID); }
		if (eff.m_Affects == Enjoy) { os << eff.enjoy_name(eff.m_EffectID); }
		if (eff.m_Affects == GirlStatus) { os << eff.girl_status_name(eff.m_EffectID); }
		os << (eff.m_Amount > 0 ? " +" : " ") << eff.m_Amount;
		return os << endl;
	}
	// end mod
};


// Represents a single trait
typedef struct sTrait
{
	char*	m_Name = 0;				// the name and unique ID of the trait
	char*	m_Desc = 0;				// a description of the trait
	char*	m_Type = 0;				// a description of the trait
	int		m_InheritChance = -1;	// chance of inheriting the trait
	int		m_RandomChance = -1;	// chance of a random girl to get the trait
	bool	m_Use_XML_Mods = false;	// `J` added to allow customized traits - .06.01.17
	vector<tEffect> m_Effects;

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
