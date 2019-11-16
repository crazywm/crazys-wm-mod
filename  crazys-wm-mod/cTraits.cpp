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
#include <cstdio>
#include "tinyxml.h"
#include "XmlMisc.h"
#include "CLog.h"
#include "cGirls.h"
#include "src/sGirl.hpp"

#ifdef LINUX
#include "linux.h"
#include <libintl.h>
#else
#include "libintl.h"
#endif

TraitEffect TraitEffect::from_xml(TiXmlElement* el)
{
	TraitEffect effect;
	if(!el->Attribute("value", &effect.value))
		throw std::runtime_error("No 'value' specified for TraitEffect!");

	const char* type_p = el->Attribute("type");
    if(!type_p)
        throw std::runtime_error("No 'type' specified for TraitEffect!");
    std::string type = type_p;

    const char* target = el->Attribute("name");
    if(!target && type != "sex") {
        throw std::runtime_error("No 'name' specified for TraitEffect!");
    }


    if(type == "stat") {
		effect.type = TraitEffect::STAT;
		effect.target = get_stat_id(target);
	} else if (type == "skill") {
		effect.type = TraitEffect::SKILL;
        effect.target = get_skill_id(target);
	} else if (type == "enjoyment") {
		effect.type = TraitEffect::ENJOYMENT;
        effect.target = get_action_id(target);
	} else if (type == "fetish") {
        effect.type = TraitEffect::FETISH;
        effect.target = get_fetish_id(target);;
    } else if (type == "sex") {
        effect.type = TraitEffect::SEX_QUALITY;
        const char* fetish_s = el->Attribute("fetish");
        effect.condition = FETISH_TRYANYTHING;
        if(fetish_s) {
            effect.condition = (Fetishs)get_fetish_id(fetish_s);
        }
    } else if (type == "modifier") {
        effect.type = TraitEffect::MODIFIER;
        effect.modifier = target;
    } else {
		throw std::runtime_error("Invalid 'type' tag for TraitEffect!");
	}

    // for all types that have a target, check its validity
    if(type != "sex" && type != "modifier") {
        if(effect.target == -1) {
            throw std::runtime_error("Invalid 'target' for TraitEffect!");
        }
    }

	return effect;
}

TraitSpec::TraitSpec(std::string name, std::string description, std::string type, int inherit_chance, int random_chance) :
		m_Name(std::move(name)),
		m_Desc(std::move(description)),
		m_Type(std::move(type)),
		m_InheritChance(inherit_chance),
		m_RandomChance(random_chance)
{
}

TraitSpec TraitSpec::from_xml(TiXmlElement* el) {
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

    TraitSpec trait(name, "", type, inherit, random);

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

std::string TraitSpec::display_name() const {
    return name();
}

void TraitSpec::add_effect(TraitEffect effect)
{
	m_Effects.push_back(effect);
}

cTraits::~cTraits() = default;

void cTraits::Free()
{
	m_CoreTraits.clear();
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
		AddTrait(TraitSpec::from_xml(el));
	}
}


void cTraits::LoadTraitsModifications(const std::string& filename)
{
    CLog l;
    l.ss() << "loading " << filename; l.ssend();
    TiXmlDocument doc(filename);
    if (!doc.LoadFile())
    {
        l.ss() << "can't load XML traits mod " << filename << endl;
        l.ss() << "Error: line " << doc.ErrorRow() << ", col " << doc.ErrorCol() << ": " << doc.ErrorDesc() << endl;
        l.ssend();
        return;
    }

    TiXmlElement *el, *root_el = doc.RootElement();
    // loop over the elements attached to the root
    for (el = root_el->FirstChildElement("Modifier"); el; el = el->NextSiblingElement("Modifier"))
    {
        std::string name = el->Attribute("Name");
        for(auto mod = el->FirstChildElement("Trait"); mod; mod = mod->NextSiblingElement("Trait")) {
            std::string trait_name = mod->Attribute("Name");
            int value;
            if(mod->QueryIntAttribute("Value", &value) != TIXML_SUCCESS) {
                l.ss() << "Invalid value for trait '" << trait_name << "' in modifier '" << name << "'\n";
                l.ssend();
                continue;
            }
            for(auto& trait : m_CoreTraits) {
                if(trait->name() == trait_name) {
                    trait->add_effect(TraitEffect{TraitEffect::MODIFIER, 0, name, value});
                }
            }
        }
    }
}

void cTraits::AddTrait(TraitSpec trait)
{
	m_CoreTraits.emplace_back(new TraitSpec(std::move(trait)));
}

void cTraits::RemoveTrait(const string& name)
{
	auto trait_iter = find_trait_by_name(name);
	if (trait_iter != m_CoreTraits.end()) {
		m_CoreTraits.erase(trait_iter);
	}
}

// case insensitive string compare
bool iequals(const string& a, const string& b)
{
    return std::equal(a.begin(), a.end(),
                      b.begin(),
                      [](char a, char b) {
                          return tolower(a) == tolower(b);
                      });
}

cTraits::trait_list_t::iterator cTraits::find_trait_by_name(const std::string& name)
{
	return std::find_if(begin(m_CoreTraits), end(m_CoreTraits),
			[&](const std::unique_ptr<TraitSpec>& trait) { return iequals(trait->name(), name); });
}

TraitSpec* cTraits::GetTrait(const string& name)
{
	auto found = find_trait_by_name(name);
	if(found == m_CoreTraits.end()) {
        CLog l;
        l.ss() << "Error: Trying to get unknown trait '"<< name << endl;
        l.ssend();
        return nullptr;
    }

	return found->get();
}

bool cTraits::LoadTraitsXML(TiXmlHandle hTraits, unsigned char& numTraits, TraitSpec* traits[], int tempTraits[])
{
    numTraits = 0;
    TiXmlElement* pTraits = hTraits.ToElement();
    if (pTraits == nullptr) return false;

    for(const auto& pTrait : all_traits())
    {
        TiXmlElement* pTraitElement = pTraits->FirstChildElement(XMLifyString(pTrait->name()));
        if (pTraitElement)
        {
            int tempInt = 0;
            traits[numTraits] = pTrait.get();
            if (tempTraits)
            {
                pTraitElement->QueryIntAttribute("Temp", &tempInt); tempTraits[numTraits] = tempInt; tempInt = 0;
            }
            ++numTraits;
        }
    }
    return true;
}

void TraitSpec::apply_effects(sGirl* target) const
{
	for(const auto& effect : m_Effects) {
		switch(effect.type) {
			case TraitEffect::STAT:
                {
                    auto stat = (STATS)effect.target;
                    // it does not make any sense to change these stats with a trait.
                    if (stat == STAT_HEALTH || stat == STAT_HAPPINESS || stat == STAT_TIREDNESS || stat == STAT_EXP ||
                        stat == STAT_LEVEL || stat == STAT_HOUSE || stat == STAT_ASKPRICE) {
                        CLog l;
                        l.ss() << "Error: Trait tried to change stat '"<< stat << endl;
                        l.ssend();
                        continue;
                    }
                    target->m_StatTr[stat] += effect.value;
                }
                break;
			case TraitEffect::SKILL:
                target->m_SkillTr[effect.target] += effect.value;
				break;
			case TraitEffect::ENJOYMENT:
                target->m_EnjoymentTR[effect.target] += effect.value;
				break;
        case TraitEffect::FETISH:
        case TraitEffect::SEX_QUALITY:
                // fetishes are handled separately when calculating a girls type
                break;
        }
	}
}

void TraitSpec::get_fetish_rating(std::array<int, NUM_FETISH>& rating) const
{
    for(const auto& effect : m_Effects) {
        if(effect.type == TraitEffect::FETISH) {
            rating[effect.target] += effect.value;
        }
    }
}

int TraitSpec::get_sex_mod(Fetishs fetish) const {
    int mod = 0;
    for(const auto& effect : m_Effects) {
        if(effect.type == TraitEffect::SEX_QUALITY) {
            if(fetish == effect.condition || effect.condition == FETISH_TRYANYTHING) {
                mod += effect.value;
            }
        }
    }
    return mod;
}

int TraitSpec::get_modifier(const std::string& mod) const
{
    int total = 0;
    for(auto& effect : m_Effects) {
        if(effect.type == TraitEffect::MODIFIER && effect.modifier == mod) {
            total += effect.value;
        }
    }
    return total;
}
