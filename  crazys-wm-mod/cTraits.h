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
#include <list>
#include <memory>

struct sGirl;
class TiXmlElement;


struct TraitEffect
{
    enum Type {
        STAT, SKILL, ENJOYMENT
    } type;
    unsigned target;
    int value;

    static TraitEffect from_xml(TiXmlElement* el);
};


// Represents a single trait
class TraitSpec
{
public:
	TraitSpec(std::string name, std::string description, std::string type,
	        int inherit_chance=-1, int random_chance=-1);

	static TraitSpec from_xml(TiXmlElement* el);

	const std::string& name() const { return m_Name; }
	std::string display_name() const;
	const std::string& desc() const { return m_Desc; }
	int random_chance() const { return m_RandomChance; }
	int inherit_chance() const { return m_InheritChance; }

	void apply_effects(sGirl* target) const;
private:

    std::string m_Name;				// the name and unique ID of the trait
    std::string m_Desc;				// a description of the trait
    std::string m_Type ;				// a description of the trait
    int		m_InheritChance = -1;	// chance of inheriting the trait
    int		m_RandomChance = -1;	// chance of a random girl to get the trait

    void add_effect(TraitEffect);
    std::vector<TraitEffect> m_Effects;
};

// Manages and loads the traits file
class cTraits
{
    using trait_list_t = std::list<std::unique_ptr<TraitSpec>>;
public:
	cTraits() = default;

    ~cTraits();

	void Free();	// Delete all the loaded data

	void LoadXMLTraits(const std::string& filename);	// Loads the traits from an XML file (adding them to the existing traits)

	void AddTrait(TraitSpec trait);
	void RemoveTrait(const std::string& name);
	TraitSpec* GetTrait(const std::string& name);

	const trait_list_t& all_traits() const { return m_CoreTraits; }

private:

    trait_list_t::iterator find_trait_by_name(const std::string& name);
    trait_list_t m_CoreTraits;
};

#endif
