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

TraitEffect TraitEffect::from_xml(TiXmlElement* el)
{
	TraitEffect effect;
	const char* target = el->Attribute("name");
	if(!target) {
		throw std::runtime_error("No 'name' specified for TraitEffect!");
	}

	if(!el->Attribute("value", &effect.value))
		throw std::runtime_error("No 'value' specified for TraitEffect!");

	const char* type_p = el->Attribute("type");
    if(!type_p)
        throw std::runtime_error("No 'type' specified for TraitEffect!");
    std::string type = type_p;

	if(type == "stat") {
		effect.type = TraitEffect::STAT;
		effect.target = sGirl::lookup_stat_code(target);
	} else if (type == "skill") {
		effect.type = TraitEffect::SKILL;
        effect.target = sGirl::lookup_skill_code(target);
	} else if (type == "enjoyment") {
		effect.type = TraitEffect::ENJOYMENT;
        effect.target = sGirl::lookup_enjoy_code(target);
	} else {
		throw std::runtime_error("Invalid 'type' tag for TraitEffect!");
	}

	return effect;
}

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
    std::string type;
    int inherit = -1;
    int random = -1;
    if (pt = el->Attribute("Name"))				name = pt;
    if (pt = el->Attribute("Type"))				type = pt;
    if (pt = el->Attribute("InheritChance"))
    {
        pt = el->Attribute("InheritChance", &inherit);
    }
    if (pt = el->Attribute("RandomChance"))
    {
        pt = el->Attribute("RandomChance", &random);
    }

    sTrait trait(name, "", type, inherit, random);

    for (el = el->FirstChildElement(); el; el = el->NextSiblingElement())
    {
        if(el->ValueStr() == "description") {
            trait.m_Desc = el->GetText();
        }
        else if(el->ValueStr() == "effects") {

			for (TiXmlElement* effect_el = el->FirstChildElement(); effect_el; effect_el = effect_el->NextSiblingElement())
			{
				if(effect_el->Type() == TiXmlNode::COMMENT)
					continue;

				try {
					trait.add_effect(TraitEffect::from_xml(effect_el));
				} catch(const std::runtime_error& rte) {
					CLog log;
					log.ss() << "Error: Could not load effect for trait '" << trait.name() << "'\n";
					log.ss() << rte.what() << "\n";
					log.ssend();
				}
			}
        }
        else {
			CLog log;
			log.ss() << "Warning: Encountered unexpected XML Element '" << el->ValueStr() << "'\n";
			log.ssend();
        }
    }

    return trait;
}

std::string sTrait::display_name() const {
    return cTraits::GetTranslateName(name());
}

void sTrait::add_effect(TraitEffect effect)
{
	m_Effects.push_back(effect);
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


void sTrait::apply_effects(sGirl* target) const
{
	for(const auto& effect : m_Effects) {
		switch(effect.type) {
			case TraitEffect::STAT:
				cGirls::UpdateStatTr(target, effect.target, effect.value);
				break;
			case TraitEffect::SKILL:
				cGirls::UpdateSkillTr(target, effect.target, effect.value);
				break;
			case TraitEffect::ENJOYMENT:
				cGirls::UpdateEnjoymentTR(target, effect.target, effect.value);
				break;
		}
	}
}