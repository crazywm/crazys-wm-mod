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

extern string stringtolowerj(string name);

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

cTraits::~cTraits()
{
}

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

cTraits::trait_list_t::iterator cTraits::find_trait_by_name(const std::string& name)
{
	return std::find_if(begin(m_CoreTraits), end(m_CoreTraits),
		[&](const std::unique_ptr<TraitSpec>& trait) { return stringtolowerj(trait->name()) == stringtolowerj(name); });
}

TraitSpec* cTraits::GetTrait(const string& name)
{
	auto found = find_trait_by_name(name);
	if (found == m_CoreTraits.end())
		return nullptr;

	return found->get();
}

void TraitSpec::apply_effects(sGirl* target) const
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
