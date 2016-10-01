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
#include "tinyxml.h"
#include "XmlMisc.h"
#include "cLog.h"

#ifdef LINUX
#include "linux.h"
#endif

cTraits::~cTraits()
{
	Free();
}

void cTraits::Free()
{
	if (m_ParentTrait) delete m_ParentTrait;
	m_ParentTrait = m_LastTrait = 0;
}

void cTraits::LoadTraits(string filename)
{
	ifstream in;
	in.open(filename.c_str());
	char *pt, buffer[500];
	sTrait* newTrait = 0;

	while (in.good())
	{
		newTrait = new sTrait;
		if (in.peek() == '\n') in.ignore(1, '\n');
		in.getline(buffer, sizeof(buffer), '\n');		// get the name
		if ((pt = strrchr(buffer, '\r'))) 
		{
			*pt = 0;
		}
		newTrait->m_Name = new char[strlen(buffer) + 1];
		strcpy(newTrait->m_Name, buffer);
		if (in.peek() == '\n') in.ignore(1, '\n');
		if (in.peek() == '\r') in.ignore(1, '\r');
		in.getline(buffer, sizeof(buffer), '\n');		// get the description
		if ((pt = strrchr(buffer, '\r')))			*pt = 0;
		if (strcmp(buffer, "na") != 0)
		{
			newTrait->m_Desc = new char[strlen(buffer) + 1];
			strcpy(newTrait->m_Desc, buffer);
		}
		else newTrait->m_Desc = 0;
		AddTrait(newTrait);
		newTrait = 0;
	}
	in.close();
}
static char *n_strdup(const char *s)
{
	return strcpy(new char[strlen(s) + 1], s);
}

void cTraits::LoadXMLTraits(string filename)
{
	CLog l;
	l.ss() << "loading " << filename; l.ssend();
	TiXmlDocument doc(filename);
	if (!doc.LoadFile())
	{
		l.ss() << "can't load XML girls " << filename << endl;
		l.ss() << "Error: line " << doc.ErrorRow() << ", col " << doc.ErrorCol() << ": " << doc.ErrorDesc() << endl;
		l.ssend();
		return;
	}

	const char *pt;
	sTrait* newTrait = 0;

	TiXmlElement *el, *root_el = doc.RootElement();
	// loop over the elements attached to the root
	for (el = root_el->FirstChildElement(); el; el = el->NextSiblingElement())
	{
		newTrait = new sTrait;
		if (pt = el->Attribute("Name"))				newTrait->m_Name = n_strdup(pt);
		if (pt = el->Attribute("Desc"))				newTrait->m_Desc = n_strdup(pt);
		if (pt = el->Attribute("Type"))				newTrait->m_Type = n_strdup(pt);
		if (pt = el->Attribute("InheritChance"))
		{
			int ival = -1;
			pt = el->Attribute("InheritChance", &ival);
			newTrait->m_InheritChance = ival;
		}
		if (pt = el->Attribute("RandomChance"))	
		{
			int ival = -1;
			pt = el->Attribute("RandomChance", &ival);
			newTrait->m_RandomChance = ival;
		}
		AddTrait(newTrait);
		newTrait = 0;
	}
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
	if (m_ParentTrait)	m_LastTrait->m_Next = trait;
	else				m_ParentTrait = trait;
	m_LastTrait = trait;
	m_NumTraits++;
}

void cTraits::RemoveTrait(string name)
{
	if (m_ParentTrait == 0)		return;
	sTrait* current = m_ParentTrait;
	sTrait* last = 0;
	if (strcmp(current->m_Name, name.c_str()) == 0)
	{
		m_ParentTrait = current->m_Next;
		current->m_Next = 0;
		delete current;
	}
	else
	{
		last = current;
		current = current->m_Next;
		while (strcmp(current->m_Name, name.c_str()) == 0)
		{
			last = current;
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
	if (m_ParentTrait == 0)	return 0;

	sTrait* current = m_ParentTrait;
	/*
	*	MOD docclox
	*	changed the way the loop works to make it easier to include
	*	debug print statements
	*/
	// g_LogFile.os() << "Looking up trait '" << name << "'" << endl;
	for (current = m_ParentTrait; current; current = current->m_Next) 
	{
//		g_LogFile.os() << "	testing '" << current->m_Name << "'" << endl;
		if (name == current->m_Name)
		{
			//g_LogFile.os() << "		gotit!" << endl;
			return current;
		}
		//g_LogFile.os() << "		nope!" << endl;
	}
	// END MOD
	return 0;
}

string traittolower(string name)
{
	string s = name;
	for (u_int i = 0; i < name.length(); i++)
	{
		s[i] = tolower(name[i]);
	}
	return s;
}

string cTraits::GetTranslateName(string name)
{
	// `J` When adding new traits, search for "J-Add-New-Traits"  :  found in >> GetTranslateName
	string trait = traittolower(name);
	string first = trait.substr(0, 1);
	if (first == "a")
	{
		/* */if (trait.compare("aids") == 0)					return gettext("AIDS");
		else if (trait.compare("abnormally large boobs") == 0)	return gettext("Abnormally Large Boobs");
		else if (trait.compare("abundant lactation") == 0)		return gettext("Abundant Lactation");
		else if (trait.compare("actress") == 0)					return gettext("Actress");
		else if (trait.compare("adventurer") == 0)				return gettext("Adventurer");
		else if (trait.compare("aggressive") == 0)				return gettext("Aggressive");
		else if (trait.compare("agile") == 0)					return gettext("Agile");
		else if (trait.compare("alcoholic") == 0)				return gettext("Alcoholic");
		else if (trait.compare("angel") == 0)					return gettext("Angel");
		else if (trait.compare("assassin") == 0)				return gettext("Assassin");
		else if (trait.compare("audacity") == 0)				return gettext("Audacity");
	}
	else if (first == "b")
	{
		/* */if (trait.compare("battery operated") == 0)		return gettext("Battery Operated");
		else if (trait.compare("bad eyesight") == 0)			return gettext("Bad Eyesight");
		else if (trait.compare("beauty mark") == 0)				return gettext("Beauty Mark");
		else if (trait.compare("big boobs") == 0)				return gettext("Big Boobs");
		else if (trait.compare("bimbo") == 0)					return gettext("Bimbo");
		else if (trait.compare("bisexual") == 0)				return gettext("Bisexual");
		else if (trait.compare("blind") == 0)					return gettext("Blind");
		else if (trait.compare("botox treatment") == 0)			return gettext("Botox Treatment");
		else if (trait.compare("branded on the ass") == 0)		return gettext("Branded on the Ass");
		else if (trait.compare("branded on the forehead") == 0)	return gettext("Branded on the Forehead");
		else if (trait.compare("brawler") == 0)					return gettext("Brawler");
		else if (trait.compare("broken will") == 0)				return gettext("Broken Will");
		else if (trait.compare("broodmother") == 0)				return gettext("Broodmother");
		else if (trait.compare("bruises") == 0)					return gettext("Bruises");
		else if (trait.compare("busty boobs") == 0)				return gettext("Busty Boobs");
	}
	else if (first == "c")
	{
		/* */if (trait.compare("canine") == 0)					return gettext("Canine");
		else if (trait.compare("cat girl") == 0)				return gettext("Cat Girl");
		else if (trait.compare("charismatic") == 0)				return gettext("Charismatic");
		else if (trait.compare("charming") == 0)				return gettext("Charming");
		else if (trait.compare("chef") == 0)					return gettext("Chef");
		else if (trait.compare("chlamydia") == 0)				return gettext("Chlamydia");
		else if (trait.compare("city girl") == 0)				return gettext("City Girl");
		else if (trait.compare("clumsy") == 0)					return gettext("Clumsy");
		else if (trait.compare("construct") == 0)				return gettext("Construct");
		else if (trait.compare("cool person") == 0)				return gettext("Cool Person");
		else if (trait.compare("cool scars") == 0)				return gettext("Cool Scars");
		else if (trait.compare("country gal") == 0)				return gettext("Country Gal");
		else if (trait.compare("cow girl") == 0)				return gettext("Cow Girl");
		else if (trait.compare("cow tits") == 0)				return gettext("Cow Tits");
		else if (trait.compare("cum addict") == 0)				return gettext("Cum Addict");
		else if (trait.compare("cute") == 0)					return gettext("Cute");
		else if (trait.compare("cyclops") == 0)					return gettext("Cyclops");
	}
	else if (first == "d")
	{
		/* */if (trait.compare("deaf") == 0)					return gettext("Deaf");
		else if (trait.compare("deep throat") == 0)				return gettext("Deep Throat");
		else if (trait.compare("delicate") == 0)				return gettext("Delicate");
		else if (trait.compare("deluxe derriere") == 0)			return gettext("Deluxe Derriere");
		else if (trait.compare("demon") == 0)					return gettext("Demon");
		else if (trait.compare("demon possessed") == 0)			return gettext("Demon Possessed");
		else if (trait.compare("dependant") == 0)				return gettext("Dependant");
		else if (trait.compare("dick-sucking lips") == 0)		return gettext("Dick-Sucking Lips");
		else if (trait.compare("different colored eyes") == 0)	return gettext("Different Colored Eyes");
		else if (trait.compare("director") == 0)				return gettext("Director");
		else if (trait.compare("doctor") == 0)					return gettext("Doctor");
		else if (trait.compare("dojikko") == 0)					return gettext("Dojikko");
		else if (trait.compare("dominatrix") == 0)				return gettext("Dominatrix");
		else if (trait.compare("dry milk") == 0)				return gettext("Dry Milk");
		else if (trait.compare("dryad") == 0)					return gettext("Dryad");
		else if (trait.compare("dwarf") == 0)					return gettext("Dwarf");
	}
	else if (first == "e")
	{
		/* */if (trait.compare("egg layer") == 0)				return gettext("Egg Layer");
		else if (trait.compare("elegant") == 0)					return gettext("Elegant");
		else if (trait.compare("elf") == 0)						return gettext("Elf");
		else if (trait.compare("equine") == 0)					return gettext("Equine");
		else if (trait.compare("emprisoned customer") == 0)		return gettext("Emprisoned Customer");
		else if (trait.compare("exhibitionist") == 0)			return gettext("Exhibitionist");
		else if (trait.compare("exotic") == 0)					return gettext("Exotic");
		else if (trait.compare("eye patch") == 0)				return gettext("Eye Patch");
	}
	else if (first == "f")
	{
		/* */if (trait.compare("fairy dust addict") == 0)		return gettext("Fairy Dust Addict");
		else if (trait.compare("fake orgasm expert") == 0)		return gettext("Fake Orgasm Expert");
		else if (trait.compare("fallen goddess") == 0)			return gettext("Fallen Goddess");
		else if (trait.compare("farmer") == 0)					return gettext("Farmer");
		else if (trait.compare("farmers daughter") == 0)		return gettext("Farmers Daughter");
		else if (trait.compare("fast orgasms") == 0)			return gettext("Fast Orgasms");
		else if (trait.compare("fat") == 0)						return gettext("Fat");
		else if (trait.compare("fearless") == 0)				return gettext("Fearless");
		else if (trait.compare("fertile") == 0)					return gettext("Fertile");
		else if (trait.compare("flat ass") == 0)				return gettext("Flat Ass");
		else if (trait.compare("flat chest") == 0)				return gettext("Flat Chest");
		else if (trait.compare("fleet of foot") == 0)			return gettext("Fleet of Foot");
		else if (trait.compare("flexible") == 0)				return gettext("Flexible");
		else if (trait.compare("flight") == 0)					return gettext("Flight");
		else if (trait.compare("former addict") == 0)			return gettext("Former Addict");
		else if (trait.compare("former official") == 0)			return gettext("Former Official");
		else if (trait.compare("fragile") == 0)					return gettext("Fragile");
		else if (trait.compare("furry") == 0)					return gettext("Furry");
		else if (trait.compare("futanari") == 0)				return gettext("Futanari");
	}
	else if (first == "g")
	{
		/* */if (trait.compare("gag reflex") == 0)				return gettext("Gag Reflex");
		else if (trait.compare("giant juggs") == 0)				return gettext("Giant Juggs");
		else if (trait.compare("giant") == 0)					return gettext("Giant");
		else if (trait.compare("goddess") == 0)					return gettext("Goddess");
		else if (trait.compare("good kisser") == 0)				return gettext("Good Kisser");
		else if (trait.compare("great arse") == 0)				return gettext("Great Arse");
		else if (trait.compare("great figure") == 0)			return gettext("Great Figure");
	}
	else if (first == "h")
	{
		/* */if (trait.compare("half-breed") == 0)				return gettext("Half-Breed");
		else if (trait.compare("half-construct") == 0)			return gettext("Half-Construct");
		else if (trait.compare("handyman") == 0)				return gettext("Handyman");
		else if (trait.compare("has boy friend") == 0)			return gettext("Has Boy Friend");
		else if (trait.compare("has girl friend") == 0)			return gettext("Has Girl Friend");
		else if (trait.compare("heavily tattooed") == 0)		return gettext("Heavily Tattooed");
		else if (trait.compare("heroine") == 0)					return gettext("Heroine");
		else if (trait.compare("herpes") == 0)					return gettext("Herpes");
		else if (trait.compare("homeless") == 0)				return gettext("Homeless");
		else if (trait.compare("horrific scars") == 0)			return gettext("Horrific Scars");
		else if (trait.compare("hourglass figure") == 0)		return gettext("Hourglass Figure");
		else if (trait.compare("hunter") == 0)					return gettext("Hunter");
	}
	else if (first == "i")
	{
		/* */if (trait.compare("idiot savant") == 0)			return gettext("Idiot Savant");
		else if (trait.compare("idol") == 0)					return gettext("Idol");
		else if (trait.compare("incest") == 0)					return gettext("Incest");
		else if (trait.compare("incorporeal") == 0)				return gettext("Incorporeal");
		else if (trait.compare("incorporial") == 0)				return gettext("Incorporeal");		// put this back in for those too lazy to update their (r)girlsx files
		else if (trait.compare("inverted nipples") == 0)		return gettext("Inverted Nipples");
		else if (trait.compare("iron will") == 0)				return gettext("Iron Will");
	}
	else if (first == "j")
	{
		/* */
	}
	else if (first == "k")
	{
		/* */if (trait.compare("kidnapped") == 0)				return gettext("Kidnapped");
	}
	else if (first == "l")
	{
		/* */if (trait.compare("large hips") == 0)				return gettext("Large Hips");
		else if (trait.compare("lesbian") == 0)					return gettext("Lesbian");
		else if (trait.compare("lolita") == 0)					return gettext("Lolita");
		else if (trait.compare("long legs") == 0)				return gettext("Long Legs");
	}
	else if (first == "m")
	{
		/* */if (trait.compare("milf") == 0)					return gettext("MILF");
		else if (trait.compare("maid") == 0)					return gettext("Maid");
		else if (trait.compare("malformed") == 0)				return gettext("Malformed");
		else if (trait.compare("manly") == 0)					return gettext("Manly");
		else if (trait.compare("masochist") == 0)				return gettext("Masochist");
		else if (trait.compare("massive melons") == 0)			return gettext("Massive Melons");
		else if (trait.compare("mature body") == 0)				return gettext("Mature Body");
		else if (trait.compare("meek") == 0)					return gettext("Meek");
		else if (trait.compare("merciless") == 0)				return gettext("Merciless");
		else if (trait.compare("middle aged") == 0)				return gettext("Middle Aged");
		else if (trait.compare("mind fucked") == 0)				return gettext("Mind Fucked");
		else if (trait.compare("missing finger") == 0)			return gettext("Missing Finger");
		else if (trait.compare("missing fingers") == 0)			return gettext("Missing Fingers");
		else if (trait.compare("missing nipple") == 0)			return gettext("Missing Nipple");
		else if (trait.compare("missing teeth") == 0)			return gettext("Missing Teeth");
		else if (trait.compare("missing toe") == 0)				return gettext("Missing Toe");
		else if (trait.compare("missing toes") == 0)			return gettext("Missing Toes");
		else if (trait.compare("mixologist") == 0)				return gettext("Mixologist");
		else if (trait.compare("muggle") == 0)					return gettext("Muggle");
		else if (trait.compare("muscular") == 0)				return gettext("Muscular");
		else if (trait.compare("mute") == 0)					return gettext("Mute");
	}
	else if (first == "n")
	{
		/* */if (trait.compare("natural pheromones") == 0)		return gettext("Natural Pheromones");
		else if (trait.compare("nerd") == 0)					return gettext("Nerd");
		else if (trait.compare("nervous") == 0)					return gettext("Nervous");
		else if (trait.compare("nimble tongue") == 0)			return gettext("Nimble Tongue");
		else if (trait.compare("no arms") == 0)					return gettext("No Arms");
		else if (trait.compare("no clit") == 0)					return gettext("No Clit");
		else if (trait.compare("no feet") == 0)					return gettext("No Feet");
		else if (trait.compare("no gag reflex") == 0)			return gettext("No Gag Reflex");
		else if (trait.compare("no hands") == 0)				return gettext("No Hands");
		else if (trait.compare("no legs") == 0)					return gettext("No Legs");
		else if (trait.compare("no nipples") == 0)				return gettext("No Nipples");
		else if (trait.compare("no teeth") == 0)				return gettext("No Teeth");
		else if (trait.compare("noble") == 0)					return gettext("Noble");
		else if (trait.compare("not human") == 0)				return gettext("Not Human");
		else if (trait.compare("nymphomaniac") == 0)			return gettext("Nymphomaniac");
	}
	else if (first == "o")
	{
		/* */if (trait.compare("old") == 0)						return gettext("Old");
		else if (trait.compare("one arm") == 0)					return gettext("One Arm");
		else if (trait.compare("one eye") == 0)					return gettext("One Eye");
		else if (trait.compare("one foot") == 0)				return gettext("One Foot");
		else if (trait.compare("one hand") == 0)				return gettext("One Hand");
		else if (trait.compare("one leg") == 0)					return gettext("One Leg");
		else if (trait.compare("open minded") == 0)				return gettext("Open Minded");
		else if (trait.compare("optimist") == 0)				return gettext("Optimist");
		else if (trait.compare("out-patient surgery") == 0)		return gettext("Out-Patient Surgery");
	}
	else if (first == "p")
	{
		/* */if (trait.compare("perky nipples") == 0)			return gettext("Perky Nipples");
		else if (trait.compare("pessimist") == 0)				return gettext("Pessimist");
		else if (trait.compare("petite breasts") == 0)			return gettext("Petite Breasts");
		else if (trait.compare("phat booty") == 0)				return gettext("Phat Booty");
		else if (trait.compare("pierced clit") == 0)			return gettext("Pierced Clit");
		else if (trait.compare("pierced navel") == 0)			return gettext("Pierced Navel");
		else if (trait.compare("pierced nipples") == 0)			return gettext("Pierced Nipples");
		else if (trait.compare("pierced nose") == 0)			return gettext("Pierced Nose");
		else if (trait.compare("pierced tongue") == 0)			return gettext("Pierced Tongue");
		else if (trait.compare("playful tail") == 0)			return gettext("Playful Tail");
		else if (trait.compare("plump tush") == 0)				return gettext("Plump Tush");
		else if (trait.compare("plump") == 0)					return gettext("Plump");
		else if (trait.compare("porn star") == 0)				return gettext("Porn Star");
		else if (trait.compare("powerful magic") == 0)			return gettext("Powerful Magic");
		else if (trait.compare("prehensile tail") == 0)			return gettext("Prehensile Tail");
		else if (trait.compare("priestess") == 0)				return gettext("Priestess");
		else if (trait.compare("princess") == 0)				return gettext("Princess");
		else if (trait.compare("psychic") == 0)					return gettext("Psychic");
		else if (trait.compare("puffy nipples") == 0)			return gettext("Puffy Nipples");
	}
	else if (first == "q")
	{
		/* */if (trait.compare("queen") == 0)					return gettext("Queen");
		else if (trait.compare("quick learner") == 0)			return gettext("Quick Learner");
	}
	else if (first == "r")
	{
		/* */if (trait.compare("reptilian") == 0)				return gettext("Reptilian");
		else if (trait.compare("retarded") == 0)				return gettext("Retarded");
	}
	else if (first == "s")
	{
		/* */if (trait.compare("sadistic") == 0)				return gettext("Sadistic");
		else if (trait.compare("scarce lactation") == 0)		return gettext("Scarce Lactation");
		else if (trait.compare("sexy air") == 0)				return gettext("Sexy Air");
		else if (trait.compare("shape shifter") == 0)			return gettext("Shape Shifter");
		else if (trait.compare("sharp-eyed") == 0)				return gettext("Sharp-Eyed");
		else if (trait.compare("short") == 0)					return gettext("Short");
		else if (trait.compare("shroud addict") == 0)			return gettext("Shroud Addict");
		else if (trait.compare("shy") == 0)						return gettext("Shy");
		else if (trait.compare("singer") == 0)					return gettext("Singer");
		else if (trait.compare("skeleton") == 0)				return gettext("Skeleton");
		else if (trait.compare("slitherer") == 0)				return gettext("Slitherer");
		else if (trait.compare("slow learner") == 0)			return gettext("Slow Learner");
		else if (trait.compare("slow orgasms") == 0)			return gettext("Slow Orgasms");
		else if (trait.compare("slut") == 0)					return gettext("Slut");
		else if (trait.compare("small boobs") == 0)				return gettext("Small Boobs");
		else if (trait.compare("small scars") == 0)				return gettext("Small Scars");
		else if (trait.compare("small tattoos") == 0)			return gettext("Small Tattoos");
		else if (trait.compare("smoker") == 0)					return gettext("Smoker");
		else if (trait.compare("social drinker") == 0)			return gettext("Social Drinker");
		else if (trait.compare("solar powered") == 0)			return gettext("Solar Powered");
		else if (trait.compare("spirit possessed") == 0)		return gettext("Spirit Possessed");
		else if (trait.compare("sterile") == 0)					return gettext("Sterile");
		else if (trait.compare("strange eyes") == 0)			return gettext("Strange Eyes");
		else if (trait.compare("straight") == 0)				return gettext("Straight");
		else if (trait.compare("strong gag reflex") == 0)		return gettext("Strong Gag Reflex");
		else if (trait.compare("strong magic") == 0)			return gettext("Strong Magic");
		else if (trait.compare("strong") == 0)					return gettext("Strong");
		else if (trait.compare("succubus") == 0)				return gettext("Succubus");
		else if (trait.compare("syphilis") == 0)				return gettext("Syphilis");
	}
	else if (first == "t")
	{
		/* */if (trait.compare("tall") == 0)					return gettext("Tall");
		else if (trait.compare("tattooed") == 0)				return gettext("Tattooed");
		else if (trait.compare("teacher") == 0)					return gettext("Teacher");
		else if (trait.compare("tight butt") == 0)				return gettext("Tight Butt");
		else if (trait.compare("titanic tits") == 0)			return gettext("Titanic Tits");
		else if (trait.compare("tomboy") == 0)					return gettext("Tomboy");
		else if (trait.compare("tone deaf") == 0)				return gettext("Tone Deaf");
		else if (trait.compare("tough") == 0)					return gettext("Tough");
		else if (trait.compare("tsundere") == 0)				return gettext("Tsundere");
		else if (trait.compare("twisted") == 0)					return gettext("Twisted");
	}
	else if (first == "u")
	{
		/* */if (trait.compare("undead") == 0)					return gettext("Undead");
	}
	else if (first == "v")
	{
		/* */if (trait.compare("vampire") == 0)					return gettext("Vampire");
		else if (trait.compare("viras blood addict") == 0)		return gettext("Viras Blood Addict");
		else if (trait.compare("virgin") == 0)					return gettext("Virgin");
	}
	else if (first == "w")
	{
		/* */if (trait.compare("waitress") == 0)				return gettext("Waitress");
		else if (trait.compare("weak magic") == 0)				return gettext("Weak Magic");
		else if (trait.compare("whore") == 0)					return gettext("Whore");
		else if (trait.compare("wide bottom") == 0)				return gettext("Wide Bottom");
		else if (trait.compare("wings") == 0)					return gettext("Wings");
	}
	else if (first == "y")
	{
		/* */if (trait.compare("yandere") == 0)					return gettext("Yandere");
		else if (trait.compare("your daughter") == 0)			return gettext("Your Daughter");
		else if (trait.compare("your wife") == 0)				return gettext("Your Wife");
	}
	else if (first == "z")
	{
		/* */if (trait.compare("zombie") == 0)					return gettext("Zombie");
	}
	return name;
}

sTrait* cTraits::GetTraitNum(int num)
{
	int count = 0;
	if (m_ParentTrait == 0) return 0;
	sTrait* current = m_ParentTrait;
	while (current)
	{
		if (count == num) break;
		count++;
		current = current->m_Next;
		if (current == 0) break;
	}
	return current;
}
