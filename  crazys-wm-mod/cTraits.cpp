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
#include <algorithm>
#include "cTraits.h"
#include "stdio.h"
#include "tinyxml.h"
#include "XmlMisc.h"
#include "CLog.h"
#include "cGirls.h"

#ifdef LINUX
#include "linux.h"
#include <libintl.h>
#else
#include "libintl.h"
#endif

sTrait::sTrait(std::string name, std::string description, std::string type, int inherit_chance, int random_chance) :
		m_Name(std::move(name)),
		m_Desc(std::move(description)),
		m_Type(std::move(type)),
		m_InheritChance(inherit_chance),
		m_RandomChance(random_chance)
{
}

sTrait sTrait::from_xml(TiXmlElement* el) {
    const char *pt;
    std::string name;
    std::string desc;
    std::string type;
    int inherit = -1;
    int random = -1;
    if (pt = el->Attribute("Name"))				name = pt;
    if (pt = el->Attribute("Desc"))				desc = pt;
    if (pt = el->Attribute("Type"))				type = pt;
    if (pt = el->Attribute("InheritChance"))
    {
        pt = el->Attribute("InheritChance", &inherit);
    }
    if (pt = el->Attribute("RandomChance"))
    {
        pt = el->Attribute("RandomChance", &random);
    }
    return sTrait(name, desc, type, inherit, random);
}

std::string sTrait::display_name() const {
    ;
}

cTraits::~cTraits()
{
	Free();
}

void cTraits::Free()
{
	for(auto trait : m_Traits)
		delete trait;
	m_Traits.clear();
}

void cTraits::LoadXMLTraits(const string& filename)
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

	TiXmlElement *el, *root_el = doc.RootElement();
	// loop over the elements attached to the root
	for (el = root_el->FirstChildElement(); el; el = el->NextSiblingElement())
	{
		AddTrait(sTrait::from_xml(el));
	}
}

void cTraits::AddTrait(sTrait trait)
{
	m_Traits.push_back(new sTrait(std::move(trait)));
}

void cTraits::RemoveTrait(const string& name)
{
	auto trait_iter = find_trait_by_name(name);
	if(trait_iter != m_Traits.end()) {
		delete *trait_iter;
		m_Traits.erase(trait_iter);
	}
}

cTraits::trait_list_t::iterator cTraits::find_trait_by_name(const std::string& name)
{
	return std::find_if(begin(m_Traits), end(m_Traits),
			[&](const sTrait* trait) { return trait->name() == name; });
}

sTrait* cTraits::GetTrait(const string& name)
{
	auto found = find_trait_by_name(name);
	if(found == m_Traits.end())
		return nullptr;

	return *found;
}

string traittolower(string name)
{
	for (char& c : name) {
        c = tolower(c);
	}
	return name;
}


string cTraits::GetTranslateName(const string& name)
{
	// `J` When adding new traits, search for "J-Add-New-Traits"  :  found in >> GetTranslateName
	string trait = traittolower(name);
	string first = trait.substr(0, 1);
	if (first == "a")
	{
		/* */if (trait.compare("aids") == 0)					return ("AIDS");
		else if (trait.compare("abnormally large boobs") == 0)	return ("Abnormally Large Boobs");
		else if (trait.compare("abundant lactation") == 0)		return ("Abundant Lactation");
		else if (trait.compare("actress") == 0)					return ("Actress");
		else if (trait.compare("adventurer") == 0)				return ("Adventurer");
		else if (trait.compare("aggressive") == 0)				return ("Aggressive");
		else if (trait.compare("agile") == 0)					return ("Agile");
		else if (trait.compare("alcoholic") == 0)				return ("Alcoholic");
		else if (trait.compare("angel") == 0)					return ("Angel");
		else if (trait.compare("assassin") == 0)				return ("Assassin");
		else if (trait.compare("audacity") == 0)				return ("Audacity");
	}
	else if (first == "b")
	{
		/* */if (trait.compare("battery operated") == 0)		return ("Battery Operated");
		else if (trait.compare("bad eyesight") == 0)			return ("Bad Eyesight");
		else if (trait.compare("beauty mark") == 0)				return ("Beauty Mark");
		else if (trait.compare("big boobs") == 0)				return ("Big Boobs");
		else if (trait.compare("bimbo") == 0)					return ("Bimbo");
		else if (trait.compare("bisexual") == 0)				return ("Bisexual");
		else if (trait.compare("blind") == 0)					return ("Blind");
		else if (trait.compare("botox treatment") == 0)			return ("Botox Treatment");
		else if (trait.compare("branded on the ass") == 0)		return ("Branded on the Ass");
		else if (trait.compare("branded on the forehead") == 0)	return ("Branded on the Forehead");
		else if (trait.compare("brawler") == 0)					return ("Brawler");
		else if (trait.compare("broken will") == 0)				return ("Broken Will");
		else if (trait.compare("broodmother") == 0)				return ("Broodmother");
		else if (trait.compare("bruises") == 0)					return ("Bruises");
		else if (trait.compare("busty boobs") == 0)				return ("Busty Boobs");
	}
	else if (first == "c")
	{
		/* */if (trait.compare("canine") == 0)					return ("Canine");
		else if (trait.compare("cat girl") == 0)				return ("Cat Girl");
		else if (trait.compare("chaste") == 0)					return ("Chaste");
		else if (trait.compare("charismatic") == 0)				return ("Charismatic");
		else if (trait.compare("charming") == 0)				return ("Charming");
		else if (trait.compare("chef") == 0)					return ("Chef");
		else if (trait.compare("chlamydia") == 0)				return ("Chlamydia");
		else if (trait.compare("city girl") == 0)				return ("City Girl");
		else if (trait.compare("clipped tendons") == 0)			return ("Clipped Tendons");
		else if (trait.compare("clumsy") == 0)					return ("Clumsy");
		else if (trait.compare("construct") == 0)				return ("Construct");
		else if (trait.compare("cool person") == 0)				return ("Cool Person");
		else if (trait.compare("cool scars") == 0)				return ("Cool Scars");
		else if (trait.compare("country gal") == 0)				return ("Country Gal");
		else if (trait.compare("cow girl") == 0)				return ("Cow Girl");
		else if (trait.compare("cow tits") == 0)				return ("Cow Tits");
		else if (trait.compare("cum addict") == 0)				return ("Cum Addict");
		else if (trait.compare("cute") == 0)					return ("Cute");
		else if (trait.compare("cyclops") == 0)					return ("Cyclops");
	}
	else if (first == "d")
	{
		/* */if (trait.compare("deaf") == 0)					return ("Deaf");
		else if (trait.compare("deep throat") == 0)				return ("Deep Throat");
		else if (trait.compare("delicate") == 0)				return ("Delicate");
		else if (trait.compare("deluxe derriere") == 0)			return ("Deluxe Derriere");
		else if (trait.compare("demon") == 0)					return ("Demon");
		else if (trait.compare("demon possessed") == 0)			return ("Demon Possessed");
		else if (trait.compare("dependant") == 0)				return ("Dependant");
		else if (trait.compare("dick-sucking lips") == 0)		return ("Dick-Sucking Lips");
		else if (trait.compare("different colored eyes") == 0)	return ("Different Colored Eyes");
		else if (trait.compare("director") == 0)				return ("Director");
		else if (trait.compare("doctor") == 0)					return ("Doctor");
		else if (trait.compare("dojikko") == 0)					return ("Dojikko");
		else if (trait.compare("dominatrix") == 0)				return ("Dominatrix");
		else if (trait.compare("dry milk") == 0)				return ("Dry Milk");
		else if (trait.compare("dryad") == 0)					return ("Dryad");
		else if (trait.compare("dwarf") == 0)					return ("Dwarf");
	}
	else if (first == "e")
	{
		/* */if (trait.compare("egg layer") == 0)				return ("Egg Layer");
		else if (trait.compare("elegant") == 0)					return ("Elegant");
		else if (trait.compare("elf") == 0)						return ("Elf");
		else if (trait.compare("equine") == 0)					return ("Equine");
		else if (trait.compare("emprisoned customer") == 0)		return ("Emprisoned Customer");
		else if (trait.compare("exhibitionist") == 0)			return ("Exhibitionist");
		else if (trait.compare("exotic") == 0)					return ("Exotic");
		else if (trait.compare("eye patch") == 0)				return ("Eye Patch");
	}
	else if (first == "f")
	{
		/* */if (trait.compare("fairy dust addict") == 0)		return ("Fairy Dust Addict");
		else if (trait.compare("fake orgasm expert") == 0)		return ("Fake Orgasm Expert");
		else if (trait.compare("fallen goddess") == 0)			return ("Fallen Goddess");
		else if (trait.compare("farmer") == 0)					return ("Farmer");
		else if (trait.compare("farmers daughter") == 0)		return ("Farmers Daughter");
		else if (trait.compare("fast orgasms") == 0)			return ("Fast Orgasms");
		else if (trait.compare("fat") == 0)						return ("Fat");
		else if (trait.compare("fearless") == 0)				return ("Fearless");
		else if (trait.compare("fertile") == 0)					return ("Fertile");
		else if (trait.compare("flat ass") == 0)				return ("Flat Ass");
		else if (trait.compare("flat chest") == 0)				return ("Flat Chest");
		else if (trait.compare("fleet of foot") == 0)			return ("Fleet of Foot");
		else if (trait.compare("flexible") == 0)				return ("Flexible");
		else if (trait.compare("flight") == 0)					return ("Flight");
		else if (trait.compare("former addict") == 0)			return ("Former Addict");
		else if (trait.compare("former official") == 0)			return ("Former Official");
		else if (trait.compare("fragile") == 0)					return ("Fragile");
		else if (trait.compare("furry") == 0)					return ("Furry");
		else if (trait.compare("futanari") == 0)				return ("Futanari");
	}
	else if (first == "g")
	{
		/* */if (trait.compare("gag reflex") == 0)				return ("Gag Reflex");
		else if (trait.compare("giant juggs") == 0)				return ("Giant Juggs");
		else if (trait.compare("giant") == 0)					return ("Giant");
		else if (trait.compare("goddess") == 0)					return ("Goddess");
		else if (trait.compare("good kisser") == 0)				return ("Good Kisser");
		else if (trait.compare("great arse") == 0)				return ("Great Arse");
		else if (trait.compare("great figure") == 0)			return ("Great Figure");
	}
	else if (first == "h")
	{
		/* */if (trait.compare("half-breed") == 0)				return ("Half-Breed");
		else if (trait.compare("half-construct") == 0)			return ("Half-Construct");
		else if (trait.compare("handyman") == 0)				return ("Handyman");
		else if (trait.compare("has boy friend") == 0)			return ("Has Boy Friend");
		else if (trait.compare("has girl friend") == 0)			return ("Has Girl Friend");
		else if (trait.compare("heavily tattooed") == 0)		return ("Heavily Tattooed");
		else if (trait.compare("heroine") == 0)					return ("Heroine");
		else if (trait.compare("herpes") == 0)					return ("Herpes");
		else if (trait.compare("homeless") == 0)				return ("Homeless");
		else if (trait.compare("horrific scars") == 0)			return ("Horrific Scars");
		else if (trait.compare("hourglass figure") == 0)		return ("Hourglass Figure");
		else if (trait.compare("hunter") == 0)					return ("Hunter");
	}
	else if (first == "i")
	{
		/* */if (trait.compare("idiot savant") == 0)			return ("Idiot Savant");
		else if (trait.compare("idol") == 0)					return ("Idol");
		else if (trait.compare("incest") == 0)					return ("Incest");
		else if (trait.compare("incorporeal") == 0)				return ("Incorporeal");
		else if (trait.compare("incorporial") == 0)				return ("Incorporeal");		// put this back in for those too lazy to cGirls::Update their (r)girlsx files
		else if (trait.compare("inverted nipples") == 0)		return ("Inverted Nipples");
		else if (trait.compare("iron will") == 0)				return ("Iron Will");
	}
	else if (first == "j")
	{
		/* */
	}
	else if (first == "k")
	{
		/* */if (trait.compare("kidnapped") == 0)				return ("Kidnapped");
	}
	else if (first == "l")
	{
		/* */if (trait.compare("large hips") == 0)				return ("Large Hips");
		else if (trait.compare("lesbian") == 0)					return ("Lesbian");
		else if (trait.compare("lolita") == 0)					return ("Lolita");
		else if (trait.compare("long legs") == 0)				return ("Long Legs");
	}
	else if (first == "m")
	{
		/* */if (trait.compare("milf") == 0)					return ("MILF");
		else if (trait.compare("maid") == 0)					return ("Maid");
		else if (trait.compare("malformed") == 0)				return ("Malformed");
		else if (trait.compare("manly") == 0)					return ("Manly");
		else if (trait.compare("masochist") == 0)				return ("Masochist");
		else if (trait.compare("massive melons") == 0)			return ("Massive Melons");
		else if (trait.compare("mature body") == 0)				return ("Mature Body");
		else if (trait.compare("meek") == 0)					return ("Meek");
		else if (trait.compare("merciless") == 0)				return ("Merciless");
		else if (trait.compare("middle aged") == 0)				return ("Middle Aged");
		else if (trait.compare("mind fucked") == 0)				return ("Mind Fucked");
		else if (trait.compare("missing finger") == 0)			return ("Missing Finger");
		else if (trait.compare("missing fingers") == 0)			return ("Missing Fingers");
		else if (trait.compare("missing nipple") == 0)			return ("Missing Nipple");
		else if (trait.compare("missing teeth") == 0)			return ("Missing Teeth");
		else if (trait.compare("missing toe") == 0)				return ("Missing Toe");
		else if (trait.compare("missing toes") == 0)			return ("Missing Toes");
		else if (trait.compare("mixologist") == 0)				return ("Mixologist");
		else if (trait.compare("muggle") == 0)					return ("Muggle");
		else if (trait.compare("muscular") == 0)				return ("Muscular");
		else if (trait.compare("mute") == 0)					return ("Mute");
	}
	else if (first == "n")
	{
		/* */if (trait.compare("natural pheromones") == 0)		return ("Natural Pheromones");
		else if (trait.compare("nerd") == 0)					return ("Nerd");
		else if (trait.compare("nervous") == 0)					return ("Nervous");
		else if (trait.compare("nimble tongue") == 0)			return ("Nimble Tongue");
		else if (trait.compare("no arms") == 0)					return ("No Arms");
		else if (trait.compare("no clit") == 0)					return ("No Clit");
		else if (trait.compare("no feet") == 0)					return ("No Feet");
		else if (trait.compare("no gag reflex") == 0)			return ("No Gag Reflex");
		else if (trait.compare("no hands") == 0)				return ("No Hands");
		else if (trait.compare("no legs") == 0)					return ("No Legs");
		else if (trait.compare("no nipples") == 0)				return ("No Nipples");
		else if (trait.compare("no teeth") == 0)				return ("No Teeth");
		else if (trait.compare("no tongue") == 0)				return ("No Tongue");
		else if (trait.compare("noble") == 0)					return ("Noble");
		else if (trait.compare("not human") == 0)				return ("Not Human");
		else if (trait.compare("nymphomaniac") == 0)			return ("Nymphomaniac");
	}
	else if (first == "o")
	{
		/* */if (trait.compare("old") == 0)						return ("Old");
		else if (trait.compare("one arm") == 0)					return ("One Arm");
		else if (trait.compare("one eye") == 0)					return ("One Eye");
		else if (trait.compare("one foot") == 0)				return ("One Foot");
		else if (trait.compare("one hand") == 0)				return ("One Hand");
		else if (trait.compare("one leg") == 0)					return ("One Leg");
		else if (trait.compare("open minded") == 0)				return ("Open Minded");
		else if (trait.compare("optimist") == 0)				return ("Optimist");
		else if (trait.compare("out-patient surgery") == 0)		return ("Out-Patient Surgery");
	}
	else if (first == "p")
	{
		/* */if (trait.compare("perky nipples") == 0)			return ("Perky Nipples");
		else if (trait.compare("pessimist") == 0)				return ("Pessimist");
		else if (trait.compare("petite breasts") == 0)			return ("Petite Breasts");
		else if (trait.compare("phat booty") == 0)				return ("Phat Booty");
		else if (trait.compare("pierced clit") == 0)			return ("Pierced Clit");
		else if (trait.compare("pierced navel") == 0)			return ("Pierced Navel");
		else if (trait.compare("pierced nipples") == 0)			return ("Pierced Nipples");
		else if (trait.compare("pierced nose") == 0)			return ("Pierced Nose");
		else if (trait.compare("pierced tongue") == 0)			return ("Pierced Tongue");
		else if (trait.compare("playful tail") == 0)			return ("Playful Tail");
		else if (trait.compare("plump tush") == 0)				return ("Plump Tush");
		else if (trait.compare("plump") == 0)					return ("Plump");
		else if (trait.compare("porn star") == 0)				return ("Porn Star");
		else if (trait.compare("powerful magic") == 0)			return ("Powerful Magic");
		else if (trait.compare("prehensile tail") == 0)			return ("Prehensile Tail");
		else if (trait.compare("priestess") == 0)				return ("Priestess");
		else if (trait.compare("princess") == 0)				return ("Princess");
		else if (trait.compare("psychic") == 0)					return ("Psychic");
		else if (trait.compare("puffy nipples") == 0)			return ("Puffy Nipples");
	}
	else if (first == "q")
	{
		/* */if (trait.compare("queen") == 0)					return ("Queen");
		else if (trait.compare("quick learner") == 0)			return ("Quick Learner");
	}
	else if (first == "r")
	{
		/* */if (trait.compare("reptilian") == 0)				return ("Reptilian");
		else if (trait.compare("retarded") == 0)				return ("Retarded");
	}
	else if (first == "s")
	{
		/* */if (trait.compare("sadistic") == 0)				return ("Sadistic");
		else if (trait.compare("scarce lactation") == 0)		return ("Scarce Lactation");
		else if (trait.compare("sexy air") == 0)				return ("Sexy Air");
		else if (trait.compare("shape shifter") == 0)			return ("Shape Shifter");
		else if (trait.compare("sharp-eyed") == 0)				return ("Sharp-Eyed");
		else if (trait.compare("short") == 0)					return ("Short");
		else if (trait.compare("shroud addict") == 0)			return ("Shroud Addict");
		else if (trait.compare("shy") == 0)						return ("Shy");
		else if (trait.compare("singer") == 0)					return ("Singer");
		else if (trait.compare("skeleton") == 0)				return ("Skeleton");
		else if (trait.compare("slitherer") == 0)				return ("Slitherer");
		else if (trait.compare("slow learner") == 0)			return ("Slow Learner");
		else if (trait.compare("slow orgasms") == 0)			return ("Slow Orgasms");
		else if (trait.compare("slut") == 0)					return ("Slut");
		else if (trait.compare("small boobs") == 0)				return ("Small Boobs");
		else if (trait.compare("small scars") == 0)				return ("Small Scars");
		else if (trait.compare("small tattoos") == 0)			return ("Small Tattoos");
		else if (trait.compare("smoker") == 0)					return ("Smoker");
		else if (trait.compare("social drinker") == 0)			return ("Social Drinker");
		else if (trait.compare("solar powered") == 0)			return ("Solar Powered");
		else if (trait.compare("spirit possessed") == 0)		return ("Spirit Possessed");
		else if (trait.compare("sterile") == 0)					return ("Sterile");
		else if (trait.compare("strange eyes") == 0)			return ("Strange Eyes");
		else if (trait.compare("straight") == 0)				return ("Straight");
		else if (trait.compare("strong gag reflex") == 0)		return ("Strong Gag Reflex");
		else if (trait.compare("strong magic") == 0)			return ("Strong Magic");
		else if (trait.compare("strong") == 0)					return ("Strong");
		else if (trait.compare("succubus") == 0)				return ("Succubus");
		else if (trait.compare("syphilis") == 0)				return ("Syphilis");
	}
	else if (first == "t")
	{
		/* */if (trait.compare("tall") == 0)					return ("Tall");
		else if (trait.compare("tattooed") == 0)				return ("Tattooed");
		else if (trait.compare("teacher") == 0)					return ("Teacher");
		else if (trait.compare("tight butt") == 0)				return ("Tight Butt");
		else if (trait.compare("titanic tits") == 0)			return ("Titanic Tits");
		else if (trait.compare("tomboy") == 0)					return ("Tomboy");
		else if (trait.compare("tone deaf") == 0)				return ("Tone Deaf");
		else if (trait.compare("tough") == 0)					return ("Tough");
		else if (trait.compare("tsundere") == 0)				return ("Tsundere");
		else if (trait.compare("twisted") == 0)					return ("Twisted");
	}
	else if (first == "u")
	{
		/* */if (trait.compare("undead") == 0)					return ("Undead");
	}
	else if (first == "v")
	{
		/* */if (trait.compare("vampire") == 0)					return ("Vampire");
		else if (trait.compare("viras blood addict") == 0)		return ("Viras Blood Addict");
		else if (trait.compare("virgin") == 0)					return ("Virgin");
	}
	else if (first == "w")
	{
		/* */if (trait.compare("waitress") == 0)				return ("Waitress");
		else if (trait.compare("weak magic") == 0)				return ("Weak Magic");
		else if (trait.compare("whore") == 0)					return ("Whore");
		else if (trait.compare("wide bottom") == 0)				return ("Wide Bottom");
		else if (trait.compare("wings") == 0)					return ("Wings");
	}
	else if (first == "y")
	{
		/* */if (trait.compare("yandere") == 0)					return ("Yandere");
		else if (trait.compare("your daughter") == 0)			return ("Your Daughter");
		else if (trait.compare("your wife") == 0)				return ("Your Wife");
	}
	else if (first == "z")
	{
		/* */if (trait.compare("zombie") == 0)					return ("Zombie");
	}
	return name;
}


void sTrait::apply_effect(sGirl* girl) const
{
	stringstream ss;
	ss << name();
	string Name = ss.str();
	string first = "";
	first = tolower(Name[0]);


	if (first == "a")
	{
		/* */if (Name == "Abnormally Large Boobs")
		{
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, 10);
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, 10);
			cGirls::UpdateStatTr(girl, STAT_AGILITY, -10);
			cGirls::UpdateSkillTr(girl, SKILL_TITTYSEX, 20);
		}
		else if (Name == "Actress")
		{
			cGirls::UpdateSkillTr(girl, SKILL_PERFORMANCE, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKMOVIE, 10);
		}
		else if (Name == "Adventurer")
		{
			cGirls::UpdateSkillTr(girl, SKILL_COMBAT, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_COMBAT, 10);
		}
		else if (Name == "Aggressive")
		{
			cGirls::UpdateSkillTr(girl, SKILL_COMBAT, 10);
			cGirls::UpdateStatTr(girl, STAT_SPIRIT, 10);
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, 5);
			cGirls::UpdateEnjoymentTR(girl, ACTION_COMBAT, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKSECURITY, 20);
		}
		else if (Name == "Agile")
		{
			cGirls::UpdateStatTr(girl, STAT_AGILITY, 20);
		}
		else if (Name == "Alcoholic")
		{
			cGirls::UpdateStatTr(girl, STAT_SANITY, -10);
		}
		else if (Name == "Angel")
		{
			cGirls::UpdateStatTr(girl, STAT_MORALITY, 20);
			cGirls::UpdateStatTr(girl, STAT_REFINEMENT, 10);
			cGirls::UpdateStatTr(girl, STAT_DIGNITY, 10);
		}
		else if (Name == "Assassin")
		{
			cGirls::UpdateStatTr(girl, STAT_MORALITY, -20);
			cGirls::UpdateSkillTr(girl, SKILL_COMBAT, 15);
			cGirls::UpdateEnjoymentTR(girl, ACTION_COMBAT, 15);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKSECURITY, 20);
		}
		else if (Name == "Audacity")
		{
			cGirls::UpdateStatTr(girl, STAT_PCFEAR, -30);
			cGirls::UpdateStatTr(girl, STAT_SPIRIT, 30);
			cGirls::UpdateStatTr(girl, STAT_OBEDIENCE, -40);
		}
	}
	else if (first == "b")
	{
		/* */if (Name == "Bad Eyesight")
		{
			cGirls::UpdateSkillTr(girl, SKILL_CRAFTING, -5);
			cGirls::UpdateSkillTr(girl, SKILL_COMBAT, -5);
		}
		else if (Name == "Battery Operated")
		{
			//
		}
		else if (Name == "Beauty Mark")
		{
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, 5);
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, 5);
		}
		else if (Name == "Big Boobs")
		{
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, 5);
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, 10);
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, 5);
			cGirls::UpdateStatTr(girl, STAT_AGILITY, -5);
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, 2);
			cGirls::UpdateSkillTr(girl, SKILL_TITTYSEX, 15);
		}
		else if (Name == "Bimbo")
		{
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, 8);
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, -10);
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, 10);
			cGirls::UpdateStatTr(girl, STAT_INTELLIGENCE, -10);
			cGirls::UpdateSkillTr(girl, SKILL_MEDICINE, -10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_SEX, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKTRAINING, -10);
		}
		else if (Name == "Blind")
		{
			//
		}
		else if (Name == "Branded on the Ass")
		{
			//
		}
		else if (Name == "Branded on the Forehead")
		{
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, -10);
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, -10);
		}
		else if (Name == "Brawler")
		{
			cGirls::UpdateSkillTr(girl, SKILL_COMBAT, 10);
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, 5);
			cGirls::UpdateEnjoymentTR(girl, ACTION_COMBAT, 10);
		}
		else if (Name == "Broken Will")
		{
			cGirls::UpdateStatTr(girl, STAT_SPIRIT, -100);
			cGirls::UpdateStatTr(girl, STAT_DIGNITY, -100);
			cGirls::UpdateStatTr(girl, STAT_OBEDIENCE, 100);
		}
		else if (Name == "Broodmother")
		{
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_SEX, 10);
		}
		else if (Name == "Bruises")
		{
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, -5);
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, -5);
			cGirls::UpdateEnjoymentTR(girl, ACTION_COMBAT, -5);
		}
		else if (Name == "Busty Boobs")
		{
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, 8);
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, 5);
			cGirls::UpdateStatTr(girl, STAT_AGILITY, -5);
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, 2);
			cGirls::UpdateSkillTr(girl, SKILL_TITTYSEX, 10);
		}
	}
	else if (first == "c")
	{
		/* */if (Name == "Canine")
		{
			//
		}
		else if (Name == "Cat Girl")
		{
			cGirls::UpdateStatTr(girl, STAT_AGILITY, 10);
		}
		else if (Name == "Chaste")
		{
			cGirls::UpdateStatTr(girl, STAT_LIBIDO, -20);
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, -5);
			cGirls::UpdateStatTr(girl, STAT_REFINEMENT, 5);
			cGirls::UpdateStatTr(girl, STAT_DIGNITY, 5);
			cGirls::UpdateStatTr(girl, STAT_SPIRIT, 5);
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, 5);

			cGirls::UpdateSkillTr(girl, SKILL_ANAL, -10);
			cGirls::UpdateSkillTr(girl, SKILL_BDSM, -10);
			cGirls::UpdateSkillTr(girl, SKILL_NORMALSEX, -10);
			cGirls::UpdateSkillTr(girl, SKILL_BEASTIALITY, -10);
			cGirls::UpdateSkillTr(girl, SKILL_GROUP, -10);
			cGirls::UpdateSkillTr(girl, SKILL_LESBIAN, -10);
			cGirls::UpdateSkillTr(girl, SKILL_STRIP, -10);
			cGirls::UpdateSkillTr(girl, SKILL_ORALSEX, -10);
			cGirls::UpdateSkillTr(girl, SKILL_TITTYSEX, -10);
			cGirls::UpdateSkillTr(girl, SKILL_HANDJOB, -10);
			cGirls::UpdateSkillTr(girl, SKILL_FOOTJOB, -10);

			cGirls::UpdateEnjoymentTR(girl, ACTION_SEX, -20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKHOUSEPET, -20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKHAREM, -20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKESCORT, -15);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKSTRIP, -10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKMILK, -5);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKMASSEUSE, -5);

			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKSECURITY, 20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKADVERTISING, 20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKTORTURER, 20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKDOCTOR, 20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKCENTRE, 20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKNURSE, 20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKMECHANIC, 20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKCOUNSELOR, 20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKFARM, 20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKMAKEPOTIONS, 20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKMAKEITEMS, 20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKCOOKING, 20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKTHERAPY, 20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKCLEANING, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKMATRON, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKCARING, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKMUSIC, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKCUSTSERV, 5);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKRECRUIT, 5);
		}
		else if (Name == "Charismatic")
		{
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, 50);
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKBAR, 20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKHALL, 20);
		}
		else if (Name == "Charming")
		{
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, 30);
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, 15);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKBAR, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKHALL, 10);
		}
		else if (Name == "City Girl")
		{
			cGirls::UpdateSkillTr(girl, SKILL_FARMING, -30);
			cGirls::UpdateSkillTr(girl, SKILL_HERBALISM, -20);
			cGirls::UpdateSkillTr(girl, SKILL_ANIMALHANDLING, -10);
			cGirls::UpdateSkillTr(girl, SKILL_COOKING, -10);
			cGirls::UpdateSkillTr(girl, SKILL_BEASTIALITY, -10);
			cGirls::UpdateStatTr(girl, STAT_REFINEMENT, 10);
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, -5);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKFARM, -20);
		}
		else if (Name == "Chef")
		{
			cGirls::UpdateSkillTr(girl, SKILL_COOKING, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKCOOKING, 20);
		}
		else if (Name == "Clumsy")
		{
			cGirls::UpdateSkillTr(girl, SKILL_ANAL, -10);
			cGirls::UpdateSkillTr(girl, SKILL_BDSM, -10);
			cGirls::UpdateSkillTr(girl, SKILL_NORMALSEX, -10);
			cGirls::UpdateSkillTr(girl, SKILL_BEASTIALITY, -10);
			cGirls::UpdateSkillTr(girl, SKILL_ORALSEX, -10);
			cGirls::UpdateSkillTr(girl, SKILL_GROUP, -10);
			cGirls::UpdateSkillTr(girl, SKILL_LESBIAN, -10);
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, -10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKCLEANING, -20);
		}
		else if (Name == "Cool Person")
		{
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, 5);
			cGirls::UpdateStatTr(girl, STAT_SPIRIT, 10);
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, 10);
		}
		else if (Name == "Cool Scars")
		{
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, 2);
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, 5);
			cGirls::UpdateStatTr(girl, STAT_SPIRIT, 5);
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, 2);
		}
		else if (Name == "Country Gal")
		{
			cGirls::UpdateSkillTr(girl, SKILL_FARMING, 10);
			cGirls::UpdateSkillTr(girl, SKILL_ANIMALHANDLING, 5);
		}
		else if (Name == "Cow Girl")
		{
			cGirls::UpdateStatTr(girl, STAT_REFINEMENT, -5);
			cGirls::UpdateStatTr(girl, STAT_SPIRIT, -10);
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, 5);
			cGirls::UpdateStatTr(girl, STAT_OBEDIENCE, 5);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKMILK, 20);
		}
		else if (Name == "Cow Tits")
		{
			//
		}
		else if (Name == "Cum Addict")
		{
			cGirls::UpdateSkillTr(girl, SKILL_ORALSEX, 30);
			cGirls::UpdateSkillTr(girl, SKILL_GROUP, 10);
			cGirls::UpdateStatTr(girl, STAT_OBEDIENCE, 20);
			cGirls::UpdateStatTr(girl, STAT_SPIRIT, -10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_SEX, 10);
		}
		else if (Name == "Cute")
		{
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, 10);
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, 5);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKCLUB, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKBAR, 20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKHALL, 15);
		}
		else if (Name == "Cyclops")
		{
			//
		}
	}
	else if (first == "d")
	{
		/* */if (Name == "Deaf")
		{
			//
		}
		else if (Name == "Deep Throat")
		{
			cGirls::UpdateSkillTr(girl, SKILL_ORALSEX, 50);
		}
		else if (Name == "Delicate")
		{
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, -15);
		}
		else if (Name == "Deluxe Derriere")
		{
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, 5);
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, 5);
			cGirls::UpdateSkillTr(girl, SKILL_ANAL, 10);
		}
		else if (Name == "Demon Possessed")
		{
			cGirls::UpdateStatTr(girl, STAT_MORALITY, -25);
			cGirls::UpdateStatTr(girl, STAT_SANITY, -30);
		}
		else if (Name == "Demon")
		{
			cGirls::UpdateStatTr(girl, STAT_MORALITY, -40);
		}
		else if (Name == "Dependant")
		{
			cGirls::UpdateStatTr(girl, STAT_SPIRIT, -30);
			cGirls::UpdateStatTr(girl, STAT_OBEDIENCE, 50);
			cGirls::UpdateSkillTr(girl, SKILL_PERFORMANCE, -10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_COMBAT, -20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKHALL, -20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKBAR, -20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKCLUB, -20);
		}
		else if (Name == "Dick-Sucking Lips")
		{
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, 5);
			cGirls::UpdateSkillTr(girl, SKILL_ORALSEX, 5);
		}
		else if (Name == "Director")
		{
			cGirls::UpdateStatTr(girl, STAT_INTELLIGENCE, 5);
			cGirls::UpdateSkillTr(girl, SKILL_PERFORMANCE, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKMOVIE, 20);
		}
		else if (Name == "Doctor")
		{
			cGirls::UpdateStatTr(girl, STAT_INTELLIGENCE, 15);
			cGirls::UpdateSkillTr(girl, SKILL_MEDICINE, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKDOCTOR, 20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKNURSE, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKTRAINING, 10);
		}
		else if (Name == "Dojikko")
		{
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, 5);
			cGirls::UpdateStatTr(girl, STAT_AGILITY, -5);
		}
		else if (Name == "Dominatrix")
		{
			cGirls::UpdateSkillTr(girl, SKILL_BDSM, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKTORTURER, 20);
		}
		else if (Name == "Dry Milk")
		{
			//
		}
		else if (Name == "Dryad")
		{
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKFARM, 10);
		}
	}
	else if (first == "e")
	{
		/* */if (Name == "Egg Layer")
		{
			//
		}
		else if (Name == "Elegant")
		{
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, 10);
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, 5);
			cGirls::UpdateStatTr(girl, STAT_REFINEMENT, 10);
			cGirls::UpdateSkillTr(girl, SKILL_PERFORMANCE, 5);
			cGirls::UpdateEnjoymentTR(girl, ACTION_SEX, -20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKMATRON, 20);
		}
		else if (Name == "Elf")
		{
			cGirls::UpdateStatTr(girl, STAT_AGILITY, 10);
			cGirls::UpdateStatTr(girl, STAT_INTELLIGENCE, 10);
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, -5);
			cGirls::UpdateStatTr(girl, STAT_REFINEMENT, 5);
		}
		else if (Name == "Equine")
		{
			cGirls::UpdateStatTr(girl, STAT_STRENGTH, 20);
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKFARM, 10);
		}
		else if (Name == "Exhibitionist")
		{
			cGirls::UpdateSkillTr(girl, SKILL_STRIP, 20);
			cGirls::UpdateSkillTr(girl, SKILL_PERFORMANCE, 10);
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, 20);
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, 10);
			cGirls::UpdateStatTr(girl, STAT_MORALITY, -10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKMUSIC, 20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKSTRIP, 30);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKMOVIE, 30);
			cGirls::UpdateEnjoymentTR(girl, ACTION_SEX, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKCLUB, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKHALL, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKBAR, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKESCORT, 10);
		}
		else if (Name == "Exotic")
		{
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, 5);
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, 10);
		}
	}
	else if (first == "f")
	{
		/* */if (Name == "Eye Patch")
		{
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, -5);
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, 5);
		}
		else if (Name == "Fairy Dust Addict")
		{
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, -10);
			cGirls::UpdateStatTr(girl, STAT_OBEDIENCE, 5);
		}
		else if (Name == "Fake Orgasm Expert")
		{
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, 10);
			cGirls::UpdateSkillTr(girl, SKILL_PERFORMANCE, 20);
			cGirls::UpdateSkillTr(girl, SKILL_ANAL, 2);
			cGirls::UpdateSkillTr(girl, SKILL_BDSM, 2);
			cGirls::UpdateSkillTr(girl, SKILL_NORMALSEX, 2);
			cGirls::UpdateSkillTr(girl, SKILL_BEASTIALITY, 2);
			cGirls::UpdateSkillTr(girl, SKILL_ORALSEX, 2);
			cGirls::UpdateSkillTr(girl, SKILL_GROUP, 2);
			cGirls::UpdateSkillTr(girl, SKILL_LESBIAN, 2);
			cGirls::UpdateEnjoymentTR(girl, ACTION_SEX, 10);
		}
		else if (Name == "Fallen Goddess")
		{
			cGirls::UpdateStatTr(girl, STAT_SANITY, -10);
		}
		else if (Name == "Farmer")
		{
			cGirls::UpdateSkillTr(girl, SKILL_FARMING, 40);
			cGirls::UpdateSkillTr(girl, SKILL_HERBALISM, 20);
			cGirls::UpdateSkillTr(girl, SKILL_ANIMALHANDLING, 20);
			cGirls::UpdateSkillTr(girl, SKILL_CRAFTING, 10);
			cGirls::UpdateSkillTr(girl, SKILL_COOKING, 10);
			cGirls::UpdateSkillTr(girl, SKILL_BEASTIALITY, 10);
			cGirls::UpdateSkillTr(girl, SKILL_MEDICINE, 5);
			cGirls::UpdateStatTr(girl, STAT_REFINEMENT, -20);
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, 20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKFARM, 50);
		}
		else if (Name == "Farmers Daughter")
		{
			cGirls::UpdateSkillTr(girl, SKILL_FARMING, 15);
			cGirls::UpdateSkillTr(girl, SKILL_ANIMALHANDLING, 10);
			cGirls::UpdateSkillTr(girl, SKILL_BEASTIALITY, 5);
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, 5);
			cGirls::UpdateStatTr(girl, STAT_REFINEMENT, -10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKFARM, 20);
		}
		else if (Name == "Fast Orgasms")
		{
			cGirls::UpdateStatTr(girl, STAT_LIBIDO, 10);
			cGirls::UpdateSkillTr(girl, SKILL_ANAL, 10);
			cGirls::UpdateSkillTr(girl, SKILL_BDSM, 10);
			cGirls::UpdateSkillTr(girl, SKILL_NORMALSEX, 10);
			cGirls::UpdateSkillTr(girl, SKILL_ORALSEX, 10);
			cGirls::UpdateSkillTr(girl, SKILL_BEASTIALITY, 10);
			cGirls::UpdateSkillTr(girl, SKILL_GROUP, 10);
			cGirls::UpdateSkillTr(girl, SKILL_LESBIAN, 10);
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_SEX, 10);
		}
		else if (Name == "Fearless")
		{
			cGirls::UpdateStatTr(girl, STAT_PCFEAR, -200);
			cGirls::UpdateStatTr(girl, STAT_SPIRIT, 30);
			cGirls::UpdateSkillTr(girl, SKILL_PERFORMANCE, 5);
			cGirls::UpdateEnjoymentTR(girl, ACTION_COMBAT, 20);
			cGirls::UpdateStatTr(girl, STAT_SANITY, 10);
		}
		else if (Name == "Fertile")
		{
			//
		}
		else if (Name == "Flat Ass")
		{
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, -5);
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, -5);
			cGirls::UpdateSkillTr(girl, SKILL_ANAL, -10);
		}
		else if (Name == "Flat Chest")
		{
			cGirls::UpdateStatTr(girl, STAT_AGILITY, 5);
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, 5);
			cGirls::UpdateSkillTr(girl, SKILL_TITTYSEX, -50);
		}
		else if (Name == "Fleet of Foot")
		{
			cGirls::UpdateStatTr(girl, STAT_AGILITY, 50);
		}
		else if (Name == "Flexible")
		{
			cGirls::UpdateStatTr(girl, STAT_AGILITY, 5);
		}
		else if (Name == "Flight")
		{
			//
		}
		else if (Name == "Former Addict")
		{
			//
		}
		else if (Name == "Former Official")
		{
			cGirls::UpdateStatTr(girl, STAT_FAME, 5);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKMATRON, 20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKCUSTSERV, 5);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKCENTRE, 5);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKRECRUIT, 5);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKCOUNSELOR, 5);

		}
		else if (Name == "Furry")
		{
			//
		}
	}
	else if (first == "g")
	{
		/* */if (Name == "Gag Reflex")
		{
			cGirls::UpdateSkillTr(girl, SKILL_ORALSEX, -30);
		}
		else if (Name == "Giant Juggs")
		{
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, 10);
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, 8);
			cGirls::UpdateStatTr(girl, STAT_AGILITY, -5);
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, 2);
			cGirls::UpdateSkillTr(girl, SKILL_TITTYSEX, 15);
		}
		else if (Name == "Giant")
		{
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, 20);
			cGirls::UpdateStatTr(girl, STAT_STRENGTH, 20);
			cGirls::UpdateStatTr(girl, STAT_AGILITY, -10);

		}
		else if (Name == "Goddess")
		{
			//
		}
		else if (Name == "Good Kisser")
		{
			cGirls::UpdateStatTr(girl, STAT_LIBIDO, 2);
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, 2);
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, 2);
			cGirls::UpdateSkillTr(girl, SKILL_SERVICE, 5);
			cGirls::UpdateEnjoymentTR(girl, ACTION_SEX, 5);
		}
		else if (Name == "Great Arse")
		{
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, 5);
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, 5);
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, 5);
			cGirls::UpdateSkillTr(girl, SKILL_ANAL, 10);
		}
		else if (Name == "Great Figure")
		{
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, 10);
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, 20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKCLUB, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKBAR, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKHALL, 10);
		}
	}
	else if (first == "h")
	{
		/* */if (Name == "Half-Breed")
		{
			//
		}
		else if (Name == "Half-Construct")
		{
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, 20);
			cGirls::UpdateSkillTr(girl, SKILL_COMBAT, 20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKMECHANIC, 10);
		}
		else if (Name == "Handyman")
		{
			cGirls::UpdateSkillTr(girl, SKILL_SERVICE, 10);
			cGirls::UpdateSkillTr(girl, SKILL_CRAFTING, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKMECHANIC, 10);
		}
		else if (Name == "Heavily Tattooed")
		{
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, -15);
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, -15);
		}
		else if (Name == "Heroine")
		{
			cGirls::UpdateSkillTr(girl, SKILL_COMBAT, 10);
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, 10);
			cGirls::UpdateStatTr(girl, STAT_STRENGTH, 10);
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, 5);
			cGirls::UpdateStatTr(girl, STAT_MORALITY, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_COMBAT, 10);
		}
		else if (Name == "Homeless")
		{
			cGirls::UpdateStatTr(girl, STAT_REFINEMENT, -10);
			cGirls::UpdateStatTr(girl, STAT_DIGNITY, -5);
		}
		else if (Name == "Horrific Scars")
		{
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, -5);
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, 10);
			cGirls::UpdateStatTr(girl, STAT_SPIRIT, -10);
		}
		else if (Name == "Hourglass Figure")
		{
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, 5);
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKCLUB, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKBAR, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKHALL, 10);
		}
		else if (Name == "Hunter")
		{
			cGirls::UpdateSkillTr(girl, SKILL_ANIMALHANDLING, 30);
			cGirls::UpdateSkillTr(girl, SKILL_BEASTIALITY, 20);
			cGirls::UpdateSkillTr(girl, SKILL_COMBAT, 20);
			cGirls::UpdateSkillTr(girl, SKILL_COOKING, 10);
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, 5);
			cGirls::UpdateStatTr(girl, STAT_REFINEMENT, -5);
			cGirls::UpdateEnjoymentTR(girl, ACTION_COMBAT, 10);
		}
	}
	else if (first == "i")
	{
		/* */if (Name == "Idiot Savant")
		{
			cGirls::UpdateStatTr(girl, STAT_INTELLIGENCE, -15);
		}
		else if (Name == "Idol")
		{
			cGirls::UpdateStatTr(girl, STAT_FAME, 25);
			cGirls::UpdateSkillTr(girl, SKILL_PERFORMANCE, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKMOVIE, 20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKMUSIC, 10);
		}
		else if (Name == "Incorporeal")
		{
			cGirls::UpdateEnjoymentTR(girl, ACTION_COMBAT, 20);
		}
		else if (Name == "Iron Will")
		{
			cGirls::UpdateStatTr(girl, STAT_PCFEAR, -60);
			cGirls::UpdateStatTr(girl, STAT_SPIRIT, 60);
			cGirls::UpdateStatTr(girl, STAT_OBEDIENCE, -90);
		}
	}
	else if (first == "j")
	{
		/* */
	}
	else if (first == "k")
	{
		/* */
	}
	else if (first == "l")
	{
		/* */ if (Name == "Large Hips")
		{
			cGirls::UpdateStatTr(girl, STAT_AGILITY, 5);
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, 5);
			cGirls::UpdateSkillTr(girl, SKILL_ANAL, 5);
		}
			//starts with M
		else if (Name == "Lesbian")
		{
			cGirls::UpdateSkillTr(girl, SKILL_LESBIAN, 40);
			cGirls::UpdateSkillTr(girl, SKILL_NORMALSEX, -10);
		}
		else if (Name == "Lolita")
		{
			cGirls::UpdateStatTr(girl, STAT_OBEDIENCE, 5);
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, 10);
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, 20);
		}
		else if (Name == "Long Legs")
		{
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, 10);
		}
	}
	else if (first == "m")
	{
		/* */if (Name == "Maid")
		{
			cGirls::UpdateSkillTr(girl, SKILL_SERVICE, 20);
			cGirls::UpdateSkillTr(girl, SKILL_COOKING, 5);
			cGirls::UpdateStatTr(girl, STAT_REFINEMENT, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKCLEANING, 20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKCOOKING, 5);
		}
		else if (Name == "Malformed")
		{
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, -20);
			cGirls::UpdateStatTr(girl, STAT_SPIRIT, -20);
			cGirls::UpdateStatTr(girl, STAT_INTELLIGENCE, -10);
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, -20);
		}
		else if (Name == "Manly")
		{
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, 10);
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, 10);
			cGirls::UpdateStatTr(girl, STAT_OBEDIENCE, -10);
			cGirls::UpdateStatTr(girl, STAT_SPIRIT, 10);
		}
		else if (Name == "Masochist")
		{
			cGirls::UpdateSkillTr(girl, SKILL_BDSM, 50);
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, 50);
			cGirls::UpdateStatTr(girl, STAT_OBEDIENCE, 30);
			cGirls::UpdateStatTr(girl, STAT_DIGNITY, -20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKTORTURER, 5);
		}
		else if (Name == "Massive Melons")
		{
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, 10);
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, 10);
			cGirls::UpdateStatTr(girl, STAT_AGILITY, -10);
			cGirls::UpdateSkillTr(girl, SKILL_TITTYSEX, 18);
		}
		else if (Name == "Meek")
		{
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, -20);
			cGirls::UpdateStatTr(girl, STAT_OBEDIENCE, 20);
			cGirls::UpdateStatTr(girl, STAT_SPIRIT, -20);
			cGirls::UpdateSkillTr(girl, SKILL_PERFORMANCE, -10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKMATRON, -30);
			cGirls::UpdateEnjoymentTR(girl, ACTION_COMBAT, -20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_SEX, -20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKCUSTSERV, -20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKBAR, -10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKCLUB, -10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKHALL, -10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKCOUNSELOR, -10);
		}
		else if (Name == "Merciless")
		{
			cGirls::UpdateSkillTr(girl, SKILL_COMBAT, 20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_COMBAT, 20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKTORTURER, 10);
		}
		else if (Name == "Middle Aged")
		{
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, -5);
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, -5);
		}
		else if (Name == "Mind Fucked")
		{
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, -50);
			cGirls::UpdateStatTr(girl, STAT_OBEDIENCE, 100);
			cGirls::UpdateStatTr(girl, STAT_SPIRIT, -50);
			cGirls::UpdateStatTr(girl, STAT_DIGNITY, -35);
			cGirls::UpdateStatTr(girl, STAT_SANITY, -40);
		}
		else if (Name == "Mixologist")
		{
			cGirls::UpdateSkillTr(girl, SKILL_SERVICE, 5);
			cGirls::UpdateSkillTr(girl, SKILL_BREWING, 15);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKBAR, 25);
		}
		else if (Name == "Muggle")
		{
			cGirls::UpdateSkillTr(girl, SKILL_MAGIC, -30);
			cGirls::UpdateStatTr(girl, STAT_MANA, -30);
		}
		else if (Name == "Muscular")
		{
			cGirls::UpdateSkillTr(girl, SKILL_COMBAT, 5);
			cGirls::UpdateStatTr(girl, STAT_AGILITY, 10);
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, 10);
			cGirls::UpdateStatTr(girl, STAT_STRENGTH, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_COMBAT, 10);
		}
		else if (Name == "Mute")
		{
			cGirls::UpdateSkillTr(girl, SKILL_MAGIC, -10);
			cGirls::UpdateSkillTr(girl, SKILL_PERFORMANCE, -10);
			cGirls::UpdateSkillTr(girl, SKILL_ANIMALHANDLING, -5);
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, -5);
		}
	}
	else if (first == "n")
	{
		/* */if (Name == "Natural Pheromones")
		{
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, 10);
		}
		else if (Name == "Nerd")
		{
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, -10);
			cGirls::UpdateStatTr(girl, STAT_INTELLIGENCE, 10);
			cGirls::UpdateSkillTr(girl, SKILL_COMBAT, -10);
			cGirls::UpdateSkillTr(girl, SKILL_MEDICINE, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKTRAINING, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKMAKEITEMS, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_COMBAT, -10);
		}
		else if (Name == "Nervous")
		{
			cGirls::UpdateStatTr(girl, STAT_OBEDIENCE, 10);
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, -10);
			cGirls::UpdateSkillTr(girl, SKILL_PERFORMANCE, -20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKCLUB, -10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_SEX, -20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKBAR, -5);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKHALL, -5);
		}
		else if (Name == "Nimble Tongue")
		{
			cGirls::UpdateStatTr(girl, STAT_LIBIDO, 2);
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, 2);
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, 2);
			cGirls::UpdateEnjoymentTR(girl, ACTION_SEX, 5);
			cGirls::UpdateSkillTr(girl, SKILL_ORALSEX, 5);
		}
		else if (Name == "No Arms")
		{
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, -30);
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, -30);
			cGirls::UpdateSkillTr(girl, SKILL_HANDJOB, -100);
			cGirls::UpdateSkillTr(girl, SKILL_COMBAT, -50);
			cGirls::UpdateSkillTr(girl, SKILL_GROUP, -20);
			cGirls::UpdateSkillTr(girl, SKILL_FOOTJOB, 30);
		}
		else if (Name == "No Clit")
		{
			cGirls::UpdateEnjoymentTR(girl, ACTION_SEX, -10);
		}
		else if (Name == "No Feet")
		{
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, -10);
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, -10);
			cGirls::UpdateSkillTr(girl, SKILL_FOOTJOB, -100);
			cGirls::UpdateSkillTr(girl, SKILL_COMBAT, -50);
			cGirls::UpdateSkillTr(girl, SKILL_GROUP, -20);
			cGirls::UpdateStatTr(girl, STAT_AGILITY, -30);
		}
		else if (Name == "No Gag Reflex")
		{
			cGirls::UpdateSkillTr(girl, SKILL_ORALSEX, 25);
		}
		else if (Name == "No Hands")
		{
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, -10);
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, -10);
			cGirls::UpdateSkillTr(girl, SKILL_HANDJOB, -100);
			cGirls::UpdateSkillTr(girl, SKILL_COMBAT, -20);
			cGirls::UpdateSkillTr(girl, SKILL_GROUP, -10);
			cGirls::UpdateSkillTr(girl, SKILL_FOOTJOB, 20);
		}
		else if (Name == "No Legs")
		{
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, -40);
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, -40);
			cGirls::UpdateSkillTr(girl, SKILL_FOOTJOB, -100);
			cGirls::UpdateSkillTr(girl, SKILL_COMBAT, -75);
			cGirls::UpdateSkillTr(girl, SKILL_GROUP, -20);
			cGirls::UpdateStatTr(girl, STAT_AGILITY, -70);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKFARM, -10);
		}
		else if (Name == "No Nipples")
		{
			cGirls::UpdateSkillTr(girl, SKILL_TITTYSEX, -5);
			cGirls::UpdateStatTr(girl, STAT_LACTATION, -200);
		}
		else if (Name == "No Teeth")
		{
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, -10);
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, -10);
			cGirls::UpdateSkillTr(girl, SKILL_ORALSEX, 10);
		}
		else if (Name == "No Tongue")
		{
			cGirls::UpdateSkillTr(girl, SKILL_ORALSEX, -5);
			cGirls::UpdateSkillTr(girl, SKILL_MAGIC, -10);
			cGirls::UpdateSkillTr(girl, SKILL_PERFORMANCE, -10);
			cGirls::UpdateSkillTr(girl, SKILL_ANIMALHANDLING, -5);
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, -5);
		}
		else if (Name == "Noble")
		{
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, 5);
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, 5);
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, 5);
			cGirls::UpdateStatTr(girl, STAT_OBEDIENCE, -5);
			cGirls::UpdateStatTr(girl, STAT_REFINEMENT, 10);
			cGirls::UpdateStatTr(girl, STAT_DIGNITY, 10);
			cGirls::UpdateStatTr(girl, STAT_FAME, 5);
			cGirls::UpdateEnjoymentTR(girl, ACTION_SEX, -10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKCLEANING, -20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKFARM, -20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKMAKEITEMS, -5);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKCOOKING, -5);
		}
		else if (Name == "Nymphomaniac")
		{
			cGirls::UpdateStatTr(girl, STAT_LIBIDO, 20);
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, 5);
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, 5);
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, 5);
			cGirls::UpdateEnjoymentTR(girl, ACTION_SEX, 30);
		}
	}
	else if (first == "o")
	{
		/* */if (Name == "Old")
		{
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, -15);
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, -15);
		}
		else if (Name == "One Arm")
		{
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, -15);
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, -15);
		}
		else if (Name == "One Eye")
		{
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, -5);
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, -5);
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, 5);
			cGirls::UpdateStatTr(girl, STAT_SPIRIT, -10);
		}
		else if (Name == "One Foot")
		{
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, -10);
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, -10);
			cGirls::UpdateSkillTr(girl, SKILL_FOOTJOB, -15);
		}
		else if (Name == "One Hand")
		{
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, -10);
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, -10);
		}
		else if (Name == "One Leg")
		{
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, -15);
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, -15);
			cGirls::UpdateSkillTr(girl, SKILL_FOOTJOB, -20);
		}
		else if (Name == "Open Minded")
		{
			//
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, 15);
			cGirls::UpdateEnjoymentTR(girl, ACTION_SEX, 10);
		}
		else if (Name == "Optimist")
		{
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKADVERTISING, 10);
		}
	}
	else if (first == "p")
	{
		/* */if (Name == "Perky Nipples")
		{
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, 5);
			cGirls::UpdateSkillTr(girl, SKILL_TITTYSEX, 5);
		}
		else if (Name == "Pessimist")
		{
			cGirls::UpdateStatTr(girl, STAT_SPIRIT, -60);
		}
		else if (Name == "Petite Breasts")
		{
			cGirls::UpdateStatTr(girl, STAT_AGILITY, 5);
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, 5);
			cGirls::UpdateSkillTr(girl, SKILL_TITTYSEX, -20);
		}
		else if (Name == "Phat Booty")
		{
			cGirls::UpdateStatTr(girl, STAT_AGILITY, -5);
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, 5);
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, 5);
			cGirls::UpdateSkillTr(girl, SKILL_ANAL, 10);
		}
		else if (Name == "Pierced Clit")
		{
			cGirls::UpdateStatTr(girl, STAT_LIBIDO, 20);
			cGirls::UpdateSkillTr(girl, SKILL_NORMALSEX, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_SEX, 10);
		}
		else if (Name == "Pierced Nipples")
		{
			cGirls::UpdateStatTr(girl, STAT_LIBIDO, 10);
			cGirls::UpdateStatTr(girl, STAT_LACTATION, -10);
			cGirls::UpdateSkillTr(girl, SKILL_TITTYSEX, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKMILK, -10);
		}
		else if (Name == "Pierced Navel")
		{
			//
		}
		else if (Name == "Pierced Nose")
		{
			//
		}
		else if (Name == "Pierced Tongue")
		{
			cGirls::UpdateSkillTr(girl, SKILL_ORALSEX, 10);
		}
		else if (Name == "Playful Tail")
		{
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, 5);
		}
		else if (Name == "Plump Tush")
		{
			cGirls::UpdateStatTr(girl, STAT_AGILITY, -10);
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, 5);
			cGirls::UpdateSkillTr(girl, SKILL_ANAL, 10);
		}
		else if (Name == "Plump")
		{
			cGirls::UpdateStatTr(girl, STAT_AGILITY, -10);
		}
		else if (Name == "Porn Star")
		{
			cGirls::UpdateStatTr(girl, STAT_FAME, 20);
			cGirls::UpdateSkillTr(girl, SKILL_PERFORMANCE, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKMOVIE, 30);
			cGirls::UpdateEnjoymentTR(girl, ACTION_SEX, 20);
		}
		else if (Name == "Powerful Magic")
		{
			cGirls::UpdateSkillTr(girl, SKILL_MAGIC, 30);
			cGirls::UpdateStatTr(girl, STAT_MANA, 30);
		}
		else if (Name == "Prehensile Tail")
		{
			//
		}
		else if (Name == "Priestess")
		{
			cGirls::UpdateStatTr(girl, STAT_REFINEMENT, 20);
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKCOUNSELOR, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKADVERTISING, 10);
		}
		else if (Name == "Princess")
		{
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, 10);
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, 10);
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, 10);
			cGirls::UpdateStatTr(girl, STAT_OBEDIENCE, -5);
			cGirls::UpdateStatTr(girl, STAT_REFINEMENT, 20);
			cGirls::UpdateStatTr(girl, STAT_DIGNITY, 20);
			cGirls::UpdateStatTr(girl, STAT_FAME, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_SEX, -10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKCLEANING, -30);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKFARM, -30);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKMAKEITEMS, -10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKCOOKING, -10);
		}
		else if (Name == "Puffy Nipples")
		{
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, 5);
			cGirls::UpdateSkillTr(girl, SKILL_TITTYSEX, 5);
		}
	}
	else if (first == "q")
	{
		/* */if (Name == "Queen")
		{
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, 20);
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, 20);
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, 10);
			cGirls::UpdateStatTr(girl, STAT_OBEDIENCE, -15);
			cGirls::UpdateStatTr(girl, STAT_REFINEMENT, 30);
			cGirls::UpdateStatTr(girl, STAT_DIGNITY, 30);
			cGirls::UpdateStatTr(girl, STAT_FAME, 15);
			cGirls::UpdateEnjoymentTR(girl, ACTION_SEX, -10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKCLEANING, -40);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKFARM, -40);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKMAKEITEMS, -20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKCOOKING, -20);
		}
	}
	else if (first == "r")
	{
		/* */if (Name == "Retarded")
		{
			cGirls::UpdateStatTr(girl, STAT_SPIRIT, -20);
			cGirls::UpdateStatTr(girl, STAT_INTELLIGENCE, -50);
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, -60);
		}
	}
	else if (first == "s")
	{
		if (Name == "Sadistic")
		{
			cGirls::UpdateSkillTr(girl, SKILL_BDSM, 20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKTORTURER, 25);
		}
		else if (Name == "Scarce Lactation")
		{
			cGirls::UpdateStatTr(girl, STAT_LACTATION, -50);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKMILK, -10);
		}
		else if (Name == "Sexy Air")
		{
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, 5);
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, 10);
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, 2);
		}
		else if (Name == "Shape Shifter")
		{
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, 20);
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, 20);
			cGirls::UpdateStatTr(girl, STAT_STRENGTH, 10);
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, 10);
			cGirls::UpdateSkillTr(girl, SKILL_PERFORMANCE, 30);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKMOVIE, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKADVERTISING, 10);
		}
		else if (Name == "Sharp-Eyed")
		{
			//
		}
		else if (Name == "Shroud Addict")
		{
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, -5);
			cGirls::UpdateStatTr(girl, STAT_OBEDIENCE, 5);
		}
		else if (Name == "Shy")
		{
			cGirls::UpdateSkillTr(girl, SKILL_PERFORMANCE, -20);
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, -20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKADVERTISING, -20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKRECRUIT, -20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKCUSTSERV, -10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKCLUB, -10);
		}
		else if (Name == "Singer")
		{
			cGirls::UpdateSkillTr(girl, SKILL_PERFORMANCE, 30);
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, 30);
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, 5);
		}
		else if (Name == "Skeleton")
		{
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, -100);		//
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, 50);		//
			cGirls::UpdateStatTr(girl, STAT_AGILITY, -20);			//
			cGirls::UpdateStatTr(girl, STAT_ASKPRICE, -1000);		//
			cGirls::UpdateStatTr(girl, STAT_HOUSE, 100);			// skeletons don't need money
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, -50);			// She may have great bone structure but thats all she has
			cGirls::UpdateStatTr(girl, STAT_TIREDNESS, -100);		// skeletons don't get tired
			cGirls::UpdateStatTr(girl, STAT_LACTATION, -1000);		//
			cGirls::UpdateSkillTr(girl, SKILL_BEASTIALITY, -50);	// animals are afraid of her
			cGirls::UpdateSkillTr(girl, SKILL_ANIMALHANDLING, -50);	// animals are afraid of her
			cGirls::UpdateSkillTr(girl, SKILL_SERVICE, -10);		//
			cGirls::UpdateSkillTr(girl, SKILL_ANAL, -1000);			//
			cGirls::UpdateSkillTr(girl, SKILL_BDSM, -1000);			//
			cGirls::UpdateSkillTr(girl, SKILL_NORMALSEX, -1000);	//
			cGirls::UpdateSkillTr(girl, SKILL_BEASTIALITY, -1000);	//
			cGirls::UpdateSkillTr(girl, SKILL_GROUP, -1000);		//
			cGirls::UpdateSkillTr(girl, SKILL_LESBIAN, -1000);		//
			cGirls::UpdateSkillTr(girl, SKILL_STRIP, -1000);		// what exactly is she covering up?
			cGirls::UpdateSkillTr(girl, SKILL_ORALSEX, -1000);		//
			cGirls::UpdateSkillTr(girl, SKILL_TITTYSEX, -1000);		//
			cGirls::UpdateSkillTr(girl, SKILL_HANDJOB, -1000);		//
			cGirls::UpdateSkillTr(girl, SKILL_FOOTJOB, -1000);		//
		}
		else if (Name == "Slitherer")
		{
			cGirls::UpdateSkillTr(girl, SKILL_FOOTJOB, -100);		//
		}
		else if (Name == "Slow Orgasms")
		{
			cGirls::UpdateSkillTr(girl, SKILL_ANAL, -2);
			cGirls::UpdateSkillTr(girl, SKILL_BDSM, -2);
			cGirls::UpdateSkillTr(girl, SKILL_NORMALSEX, -2);
			cGirls::UpdateSkillTr(girl, SKILL_BEASTIALITY, -2);
			cGirls::UpdateSkillTr(girl, SKILL_ORALSEX, -2);
			cGirls::UpdateSkillTr(girl, SKILL_GROUP, -2);
			cGirls::UpdateSkillTr(girl, SKILL_LESBIAN, -2);
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, -2);
			cGirls::UpdateEnjoymentTR(girl, ACTION_SEX, -10);
		}
		else if (Name == "Slut")
		{
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, -10);
			cGirls::UpdateStatTr(girl, STAT_DIGNITY, -5);
			cGirls::UpdateEnjoymentTR(girl, ACTION_SEX, 10);
		}
		else if (Name == "Small Boobs")
		{
			cGirls::UpdateStatTr(girl, STAT_AGILITY, 5);
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, 5);
			cGirls::UpdateSkillTr(girl, SKILL_TITTYSEX, -15);
		}
		else if (Name == "Small Scars")
		{
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, -2);
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, 2);
			cGirls::UpdateStatTr(girl, STAT_SPIRIT, -2);
		}
		else if (Name == "Small Tattoos")
		{
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, -5);
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, -5);
		}
		else if (Name == "Smoker")
		{
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, -5);
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, -5);
			cGirls::UpdateStatTr(girl, STAT_AGILITY, -10);
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, -5);
		}
		else if (Name == "Social Drinker")
		{
			//
		}
		else if (Name == "Solar Powered")
		{
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKFARM, 10);	// works outdoors
		}
		else if (Name == "Spirit Possessed")
		{
			//
		}
		else if (Name == "Straight")
		{
			cGirls::UpdateSkillTr(girl, SKILL_NORMALSEX, 10);
			cGirls::UpdateSkillTr(girl, SKILL_LESBIAN, -15);
		}
		else if (Name == "Strong Gag Reflex")
		{
			cGirls::UpdateSkillTr(girl, SKILL_ORALSEX, -50);
		}
		else if (Name == "Strong Magic")
		{
			cGirls::UpdateSkillTr(girl, SKILL_MAGIC, 20);
			cGirls::UpdateStatTr(girl, STAT_MANA, 20);
		}
		else if (Name == "Strong")
		{
			cGirls::UpdateSkillTr(girl, SKILL_COMBAT, 10);
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, 10);
			cGirls::UpdateStatTr(girl, STAT_STRENGTH, 10);
		}
		else if (Name == "Succubus")
		{
			cGirls::UpdateStatTr(girl, STAT_LIBIDO, 35);
			cGirls::UpdateEnjoymentTR(girl, ACTION_SEX, 40);
		}
	}
	else if (first == "t")
	{
		/* */if (Name == "Tattooed")
		{
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, -10);
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, -10);
		}
		else if (Name == "Teacher")
		{
			cGirls::UpdateStatTr(girl, STAT_INTELLIGENCE, 20);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKCOUNSELOR, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKMATRON, 10);
		}
		else if (Name == "Tight Butt")
		{
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, 5);
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, 5);
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, 5);
			cGirls::UpdateSkillTr(girl, SKILL_ANAL, 8);
		}
		else if (Name == "Titanic Tits")
		{
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, 10);
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, 10);
			cGirls::UpdateStatTr(girl, STAT_AGILITY, -15);
			cGirls::UpdateSkillTr(girl, SKILL_TITTYSEX, 25);
		}
		else if (Name == "Tomboy")
		{
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, 10);
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, -10);
		}
		else if (Name == "Tone Deaf")
		{
			cGirls::UpdateSkillTr(girl, SKILL_PERFORMANCE, -30);
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, -30);
		}
		else if (Name == "Tsundere")
		{
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, 20);
			cGirls::UpdateStatTr(girl, STAT_OBEDIENCE, -20);
		}
		else if (Name == "Twisted")
		{
			cGirls::UpdateStatTr(girl, STAT_SANITY, -20);
		}
	}
	else if (first == "u")
	{
		/* */if (Name == "Undead")
		{
			/* */
		}

	}
	else if (first == "v")
	{
		/* */if (Name == "Vampire")
		{
			// `J` zzzzzz - more needs to be added for this
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, 10);
			cGirls::UpdateStatTr(girl, STAT_OBEDIENCE, -5);
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, 10);
		}
		else if (Name == "Viras Blood Addict")
		{
			cGirls::UpdateStatTr(girl, STAT_CONFIDENCE, -15);
			cGirls::UpdateStatTr(girl, STAT_OBEDIENCE, 20);
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, -10);
		}
	}
	else if (first == "w")
	{
		/* */if (Name == "Waitress")
		{
			cGirls::UpdateSkillTr(girl, SKILL_SERVICE, 30);
			cGirls::UpdateSkillTr(girl, SKILL_COOKING, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKBAR, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKHALL, 10);
			cGirls::UpdateEnjoymentTR(girl, ACTION_WORKCOOKING, 5);
		}
		else if (Name == "Weak Magic")
		{
			cGirls::UpdateSkillTr(girl, SKILL_MAGIC, -20);
			cGirls::UpdateStatTr(girl, STAT_MANA, -20);
		}
		else if (Name == "Whore")
		{
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, -5);
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, -10);
			cGirls::UpdateStatTr(girl, STAT_CHARISMA, -10);
			cGirls::UpdateSkillTr(girl, SKILL_ANAL, 20);
			cGirls::UpdateSkillTr(girl, SKILL_BDSM, 10);
			cGirls::UpdateSkillTr(girl, SKILL_NORMALSEX, 30);
			cGirls::UpdateSkillTr(girl, SKILL_BEASTIALITY, 10);
			cGirls::UpdateSkillTr(girl, SKILL_GROUP, 30);
			cGirls::UpdateSkillTr(girl, SKILL_LESBIAN, 5);
			cGirls::UpdateSkillTr(girl, SKILL_STRIP, 5);
			cGirls::UpdateSkillTr(girl, SKILL_ORALSEX, 20);
			cGirls::UpdateSkillTr(girl, SKILL_TITTYSEX, 10);
			cGirls::UpdateSkillTr(girl, SKILL_HANDJOB, 30);
			cGirls::UpdateSkillTr(girl, SKILL_ANIMALHANDLING, 5);
			cGirls::UpdateSkillTr(girl, SKILL_FOOTJOB, 5);
			cGirls::UpdateEnjoymentTR(girl, ACTION_SEX, 30);
		}
		else if (Name == "Wide Bottom")
		{
			cGirls::UpdateStatTr(girl, STAT_CONSTITUTION, 5);
			cGirls::UpdateStatTr(girl, STAT_BEAUTY, 5);
			cGirls::UpdateSkillTr(girl, SKILL_ANAL, 5);
		}
		else if (Name == "Wings")
		{
			//
		}
	}
	else if (first == "x")
	{
		/* */
	}
	else if (first == "y")
	{
		/* */if (Name == "Yandere")
		{
			cGirls::UpdateStatTr(girl, STAT_SPIRIT, 20);
		}
	}
	else if (first == "z")
	{
		/* */if (Name == "Zombie")
		{
			cGirls::UpdateStatTr(girl,  STAT_CHARISMA		, -50);		//
			cGirls::UpdateStatTr(girl, 	STAT_CONSTITUTION	, 20);		//
			cGirls::UpdateStatTr(girl, 	STAT_INTELLIGENCE	, -50);		//
			cGirls::UpdateStatTr(girl, 	STAT_AGILITY		, -20);		//
			cGirls::UpdateStatTr(girl, 	STAT_ASKPRICE		, -1000);	//
			cGirls::UpdateStatTr(girl, 	STAT_BEAUTY			, -20);		//
			cGirls::UpdateStatTr(girl, 	STAT_TIREDNESS		, -100);	// zombies don't get tired
			cGirls::UpdateStatTr(girl, 	STAT_MORALITY		, -50);		// zombies eat people
			cGirls::UpdateStatTr(girl, 	STAT_REFINEMENT		, -50);		//
			cGirls::UpdateStatTr(girl, 	STAT_LACTATION		, -1000);	//
			cGirls::UpdateStatTr(girl, 	STAT_STRENGTH		, 20);		//
			cGirls::UpdateSkillTr(girl, SKILL_BEASTIALITY	, -50);		// animals are afraid of her
			cGirls::UpdateSkillTr(girl, SKILL_ANIMALHANDLING, -50);		// animals are afraid of her
			cGirls::UpdateSkillTr(girl, SKILL_SERVICE		, -20);		//
			cGirls::UpdateSkillTr(girl, SKILL_STRIP			, -50);		// zombies don't care about clothes
			cGirls::UpdateSkillTr(girl, SKILL_ORALSEX		, -50);		// You want to put what where?
			cGirls::UpdateSkillTr(girl, SKILL_MEDICINE		, -50);		//
			cGirls::UpdateSkillTr(girl, SKILL_PERFORMANCE	, -50);		//
			cGirls::UpdateSkillTr(girl, SKILL_HANDJOB		, -50);		// less control over hands and feet
			cGirls::UpdateSkillTr(girl, SKILL_CRAFTING		, -50);		//
			cGirls::UpdateSkillTr(girl, SKILL_HERBALISM		, -50);		//
			cGirls::UpdateSkillTr(girl, SKILL_FARMING		, -10);		//
			cGirls::UpdateSkillTr(girl, SKILL_BREWING		, -50);		//
			cGirls::UpdateSkillTr(girl, SKILL_FOOTJOB		, -50);		// less control over hands and feet
			cGirls::UpdateSkillTr(girl, SKILL_COOKING		, -50);		//
		}
	}
}