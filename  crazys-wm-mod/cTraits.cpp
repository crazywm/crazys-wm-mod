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
#include <iostream>
#include "cTraits.h"
#include "stdio.h"
#include "libintl.h"

#ifdef LINUX
#include "linux.h"
#endif

cTraits::~cTraits()
{
	Free();
}

void cTraits::Free()
{
	if(m_ParentTrait)
		delete m_ParentTrait;
	m_ParentTrait = m_LastTrait = 0;
}

void cTraits::LoadTraits(string filename)
{
	ifstream in;
	in.open(filename.c_str());
	char *pt, buffer[500];
	sTrait* newTrait = 0;

	while(in.good())
	{
		newTrait = new sTrait;

		if (in.peek()=='\n') in.ignore(1,'\n');
		in.getline(buffer, sizeof(buffer), '\n');		// get the name
		if((pt = strrchr(buffer, '\r'))) {
			*pt = 0;
		}

		newTrait->m_Name = new char[strlen(buffer)+1];
		strcpy(newTrait->m_Name, buffer);

		if (in.peek()=='\n') in.ignore(1,'\n');
		if (in.peek()=='\r') in.ignore(1,'\r');
		in.getline(buffer, sizeof(buffer), '\n');		// get the description
		if((pt = strrchr(buffer, '\r'))) {
			*pt = 0;
		}

		if(strcmp(buffer, "na") != 0)
		{
			newTrait->m_Desc = new char[strlen(buffer)+1];
			strcpy(newTrait->m_Desc, buffer);
		}
		else
			newTrait->m_Desc = 0;

		AddTrait(newTrait);
		newTrait = 0;
	}

	in.close();
}

void cTraits::SaveTraits(string filename)
{
	ofstream out;
	sTrait* current = m_ParentTrait;

	out.open(filename.c_str());

	while(current != 0)
	{
		out<<current->m_Name<<"\n";
		out<<current->m_Desc<<"\n";
		current = current->m_Next;
	}
	out.flush();
	out.close();
}

void cTraits::AddTrait(sTrait* trait)
{
	trait->m_Next = 0;
	if(m_ParentTrait)
		m_LastTrait->m_Next = trait;
	else
		m_ParentTrait = trait;
	m_LastTrait = trait;
	m_NumTraits++;
}

void cTraits::RemoveTrait(string name)
{
	if(m_ParentTrait == 0)
		return;

	sTrait* current = m_ParentTrait;
	sTrait* last = 0;

	if(strcmp(current->m_Name, name.c_str()) == 0)
	{
		m_ParentTrait = current->m_Next;
		current->m_Next = 0;
		delete current;
	}
	else
	{
		last = current ;
		current = current->m_Next;
		while(strcmp(current->m_Name, name.c_str()) == 0)
		{
			last = current ;
			current = current->m_Next;
		}

		last->m_Next = current->m_Next;

		current->m_Next = 0;
		delete current;
	}

	m_NumTraits--;
	current = last = 0;
}

sTrait* cTraits::GetTrait(string name)
{
	if(m_ParentTrait == 0)
		return 0;

	sTrait* current = m_ParentTrait;
/*
 *	MOD docclox
 *	changed the way the loop works to make it easier to include
 *	debug print statements
 */
	// g_LogFile.os() << "Looking up trait '" << name << "'" << endl;
	for(current = m_ParentTrait; current; current = current->m_Next) {
		/*
		g_LogFile.os()	<< "	testing '"
			<< current->m_Name
			<<"'"
			<< endl;
		*/
		if(name == current->m_Name) {
			//g_LogFile.os() << "		gotit!" << endl;
			return current;
		}
		//g_LogFile.os() << "		nope!" << endl;
	}
	// END MOD
	return 0;
}

string cTraits::GetTranslateName(string name){
	// TODO replace string by trait constants 
	// This is a copy/paste in cGirls.cpp ApplyTraits for testing trait
	// and translate with gettext
	if(name.compare("Abnormally Large Boobs") == 0)
	{
		return gettext("Abnormally Large Boobs");
	}
	else if(name.compare("Adventurer") == 0)
	{
		return gettext("Adventurer");
	}
	else if(name.compare("Aggressive") == 0)
	{
		return gettext("Aggressive");
	}
	else if (name.compare("AIDS") == 0)
	{
		return gettext("AIDS");
	}
	else if (name.compare("Angel") == 0)	// `J` added
	{
		return gettext("Angel");
	}
	else if (name.compare("Assassin") == 0)
	{
		return gettext("Assassin");
	}
	else if(name.compare("Big Boobs") == 0)
	{
		return gettext("Big Boobs");
	}
	else if(name.compare("Broken Will") == 0)
	{
		return gettext("Broken Will");
	}
	else if(name.compare("Cat Girl") == 0)
	{
		return gettext("Cat Girl");
	}
	else if(name.compare("Charismatic") == 0)
	{
		return gettext("Charismatic");
	}
	else if(name.compare("Charming") == 0)
	{
		return gettext("Charming");
	}
	else if(name.compare("Chlamydia") == 0)
	{
		return gettext("Chlamydia");
	}
	else if(name.compare("Clumsy") == 0)
	{
		return gettext("Clumsy");
	}
	else if(name.compare("Construct") == 0)
	{
		return gettext("Construct");
	}
	else if(name.compare("Cool Person") == 0)
	{
		return gettext("Cool Person");
	}
	else if(name.compare("Cool Scars") == 0)
	{
		return gettext("Cool Scars");
	}
	else if(name.compare("Cute") == 0)
	{
		return gettext("Cute");
	}
	else if(name.compare("Deep Throat") == 0)
	{
		return gettext("Deep Throat");
	}
	else if(name.compare("Demon") == 0)
	{
		return gettext("Demon");
	}
	else if(name.compare("Dependant") == 0)
	{
		return gettext("Dependant");
	}
	else if(name.compare("Different Colored Eyes") == 0)
	{
		return gettext("Different Colored Eyes");
	}
	else if(name.compare("Elegant") == 0)
	{
		return gettext("Elegant");
	}
	else if(name.compare("Eye Patch") == 0)
	{
		return gettext("Eye Patch");
	}
	else if(name.compare("Fairy Dust Addict") == 0)
	{
		return gettext("Fairy Dust Addict");
	}
	else if(name.compare("Fake orgasm expert") == 0 || name.compare("Fake Orgasm Expert") == 0)
	{
		return gettext("Fake Orgasm Expert");
	}
	else if(name.compare("Fast orgasms") == 0 || name.compare("Fast Orgasms") == 0)
	{
		return gettext("Fast Orgasms");
	}
	else if(name.compare("Fearless") == 0)
	{
		return gettext("Fearless");
	}
	else if(name.compare("Fleet of Foot") == 0 || name.compare("Fleet Of Foot") == 0)
	{
		return gettext("Fleet of Foot");
	}
	else if(name.compare("Fragile") == 0)
	{
		return gettext("Fragile");
	}
	else if(name.compare("Futanari") == 0)
	{
		return gettext("Futanari");
	}
	else if(name.compare("Gag Reflex") == 0)
	{
		return gettext("Gag Reflex");
	}
	else if(name.compare("Good Kisser") == 0)
	{
		return gettext("Good Kisser");
	}
	else if(name.compare("Great Arse") == 0)
	{
		return gettext("Great Arse");
	}
	else if(name.compare("Great Figure") == 0)
	{
		return gettext("Great Figure");
	}
	else if(name.compare("Half-Construct") == 0)
	{
		return gettext("Half-Construct");
	}
	else if(name.compare("Horrific Scars") == 0)
	{
		return gettext("Horrific Scars");
	}
	else if(name.compare("Incest") == 0)
	{
		return gettext("Incest");
	}
	else if(name.compare("Incorporeal") == 0 || name.compare("Incorporial") == 0)
	{
		return gettext("Incorporeal");
	}
	else if(name.compare("Iron Will") == 0)
	{
		return gettext("Iron Will");
	}
	else if(name.compare("Lesbian") == 0)
	{
		return gettext("Lesbian");
	}
	else if(name.compare("Lolita") == 0)
	{
		return gettext("Lolita");
	}
	else if(name.compare("Long Legs") == 0)
	{
		return gettext("Long Legs");
	}
	else if(name.compare("Malformed") == 0)
	{
		return gettext("Malformed");
	}
	else if(name.compare("Manly") == 0)
	{
		return gettext("Manly");
	}
	else if(name.compare("Masochist") == 0)
	{
		return gettext("Masochist");
	}
	else if(name.compare("Meek") == 0)
	{
		return gettext("Meek");
	}
	else if(name.compare("Merciless") == 0)
	{
		return gettext("Merciless");
	}
	else if(name.compare("MILF") == 0)
	{
		return gettext("MILF");
	}
	else if(name.compare("Mind Fucked") == 0)
	{
		return gettext("Mind Fucked");
	}
	else if(name.compare("Nerd") == 0)
	{
		return gettext("Nerd");
	}
	else if(name.compare("Nervous") == 0)
	{
		return gettext("Nervous");
	}
	else if(name.compare("No Gag Reflex") == 0)
	{
		return gettext("No Gag Reflex");
	}
	else if(name.compare("Not Human") == 0)
	{
		return gettext("Not Human");
	}
	else if(name.compare("Nymphomaniac") == 0)
	{
		return gettext("Nymphomaniac");
	}
	else if(name.compare("One Eye") == 0)
	{
		return gettext("One Eye");
	}
	else if(name.compare("Optimist") == 0)
	{
		return gettext("Optimist");
	}
	else if(name.compare("Perky Nipples") == 0)
	{
		return gettext("Perky Nipples");
	}
	else if(name.compare("Pessimist") == 0)
	{
		return gettext("Pessimist");
	}
	else if(name.compare("Pierced Clit") == 0)
	{
		return gettext("Pierced Clit");
	}
	else if(name.compare("Pierced Nipples") == 0)
	{
		return gettext("Pierced Nipples");
	}
	else if(name.compare("Pierced Tongue") == 0)
	{
		return gettext("Pierced Tongue");
	}
	else if(name.compare("Princess") == 0)
	{
		return gettext("Princess");
	}
	else if(name.compare("Psychic") == 0)
	{
		return gettext("Psychic");
	}
	else if(name.compare("Puffy Nipples") == 0)
	{
		return gettext("Puffy Nipples");
	}
	else if(name.compare("Queen") == 0)
	{
		return gettext("Queen");
	}
	else if(name.compare("Quick Learner") == 0)
	{
		return gettext("Quick Learner");
	}
	else if(name.compare("Retarded") == 0)
	{
		return gettext("Retarded");
	}
	else if(name.compare("Sadistic") == 0)
	{
		return gettext("Sadistic");
	}
	else if(name.compare("Sexy Air") == 0)
	{
		return gettext("Sexy Air");
	}
	else if(name.compare("Shape Shifter") == 0)
	{
		return gettext("Shape Shifter");
	}
	else if(name.compare("Shroud Addict") == 0)
	{
		return gettext("Shroud Addict");
	}
	else if(name.compare("Slow Learner") == 0)
	{
		return gettext("Slow Learner");
	}
	else if(name.compare("Slow orgasms") == 0 || name.compare("Slow Orgasms") == 0)
	{
		return gettext("Slow Orgasms");
	}
	else if(name.compare("Small Boobs") == 0)
	{
		return gettext("Small Boobs");
	}
	else if(name.compare("Small Scars") == 0)
	{
		return gettext("Small Scars");
	}
	else if(name.compare("Sterile") == 0)
	{
		return gettext("Sterile");
	}
	else if(name.compare("Strange Eyes") == 0)
	{
		return gettext("Strange Eyes");
	}
	else if(name.compare("Strong Magic") == 0)
	{
		return gettext("Strong Magic");
	}
	else if(name.compare("Strong") == 0)
	{
		return gettext("Strong");
	}
	else if(name.compare("Syphilis") == 0)
	{
		return gettext("Syphilis");
	}
	else if(name.compare("Tough") == 0)
	{
		return gettext("Tough");
	}
	else if(name.compare("Tsundere") == 0)
	{
		return gettext("Tsundere");
	}
	else if(name.compare("Twisted") == 0)
	{
		return gettext("Twisted");
	}
	else if(name.compare("Viras Blood Addict") == 0)
	{
		return gettext("Viras Blood Addict");
	}
	else if(name.compare("Yandere") == 0)
	{
		return gettext("Yandere");
	}
	else if(name.compare("Your Daughter") == 0)
	{
		return gettext("Your Daughter");
	}
	return name;
}

sTrait* cTraits::GetTraitNum(int num)
{
	int count = 0;
	if(m_ParentTrait == 0)
		return 0;

	sTrait* current = m_ParentTrait;

	while(current)
	{
		if(count == num)
			break;
		count++;

		current = current->m_Next;
		if(current == 0)
			break;
	}

	return current;
}
