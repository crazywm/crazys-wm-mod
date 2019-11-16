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

#include "cRandomGirl.h"
#include "cGirls.h"
#include "CLog.h"
#include "src/Game.hpp"
#include "src/sGirl.hpp"
#include "cInventory.h"
#include <iomanip>
#include "sConfig.h"
#include "cTraits.h"

extern cConfig cfg;

sRandomGirl::sRandomGirl()
{
    m_Name = "";
    m_Desc = "-";

    m_Human = true;
    m_Catacomb = m_Arena = m_YourDaughter = m_IsDaughter = false;

    m_NumItems = m_NumItemNames = 0;
    for (int i = 0; i < MAXNUM_GIRL_INVENTORY; i++)
    {
        m_ItemChance[i] = 0;
        m_Inventory[i] = nullptr;
    }

    //assigning defaults
    for (int i = 0; i < NUM_STATS; i++)
    {
        // `J` When modifying Stats or Skills, search for "J-Change-Stats-Skills"  :  found in >> cGirls.h > sRandomGirl
        switch (i)
        {
        case STAT_HAPPINESS:
        case STAT_HEALTH:
            m_MinStats[i] = m_MaxStats[i] = 100;
            break;
        case STAT_TIREDNESS:
        case STAT_FAME:
        case STAT_LEVEL:
        case STAT_EXP:
        case STAT_PCFEAR:
        case STAT_PCLOVE:
        case STAT_PCHATE:
        case STAT_ASKPRICE:
        case STAT_HOUSE:
            m_MinStats[i] = m_MaxStats[i] = 0;
            break;
        case STAT_AGE:
            m_MinStats[i] = 17; m_MaxStats[i] = 25;
            break;
        case STAT_MORALITY:
        case STAT_REFINEMENT:
        case STAT_DIGNITY:
        case STAT_SANITY:
            m_MinStats[i] = -10; m_MaxStats[i] = 10;
            break;
        case STAT_LACTATION:
            m_MinStats[i] = -20; m_MaxStats[i] = 20;
            break;
        default:
            m_MinStats[i] = 30;	m_MaxStats[i] = 60;
            break;
        }
    }
    for (int i = 0; i < NUM_SKILLS; i++)// Changed from 10 to NUM_SKILLS so that it will always set the proper number of defaults --PP
    {
        m_MinSkills[i] = 0;				// Changed from 30 to 0, made no sense for all skills to be a default of 30.
        m_MaxSkills[i] = 30;
    }
    // now for a few overrides
    m_MinMoney = 0;
    m_MaxMoney = 10;
}

sRandomGirl::~sRandomGirl() = default;

void sRandomGirl::load_from_xml(TiXmlElement *el)
{
    const char *pt;
    m_NumItems = 0; m_NumItemNames = 0;
    // name and description are easy
    if (pt = el->Attribute("Name")) 		m_Name = pt;
    g_LogFile.os() << "Loading Rgirl : " << pt << endl;
    if (pt = el->Attribute("Desc"))			m_Desc = pt;

    // DQ - new random type ...
    if (pt = el->Attribute("Human"))			m_Human = strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0;
    if (pt = el->Attribute("Catacomb"))			m_Catacomb = strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0;
    if (pt = el->Attribute("Arena"))			m_Arena = strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0;
    if (pt = el->Attribute("Your Daughter"))	m_YourDaughter = strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0;
    if (pt = el->Attribute("Is Daughter"))		m_IsDaughter = strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0;

    // loop through children
    TiXmlElement *child;
    for (child = el->FirstChildElement(); child; child = child->NextSiblingElement())
    {
        /*
        *		now: what we do depends on the tag string
        *		which we can get from the ValueStr() method
        *
        *		Let's process each tag type in its own method.
        *		Keep things cleaner that way.
        */
        if (child->ValueStr() == "Gold")
        {
            process_cash_xml(child);
            continue;
        }
        // OK: is it a stat?
        if (child->ValueStr() == "Stat")
        {
            process_stat_xml(child);
            continue;
        }
        // How about a skill?
        if (child->ValueStr() == "Skill")
        {
            process_skill_xml(child);
            continue;
        }
        // surely a trait then?
        if (child->ValueStr() == "Trait")
        {
            process_trait_xml(child);
            continue;
        }
        // surely a item then?
        if (child->ValueStr() == "Item")
        {
            process_item_xml(child);
            continue;
        }
        // None of the above? Better ask for help then.
        g_LogFile.os() << "Unexpected tag: " << child->ValueStr() << endl;
        g_LogFile.os() << "	don't know what do to, ignoring" << endl;
    }
}

ostream& operator<<(ostream &os, sRandomGirl &g)
{
    os << g.m_Name << endl;
    os << g.m_Desc << endl;
    os << ("Human? ") << (g.m_Human ? "Yes" : "No") << endl;
    os << ("Catacomb Dweller? ") << (g.m_Catacomb ? "Yes" : "No") << endl;
    os << ("Arena Girl? ") << (g.m_Arena ? "Yes" : "No") << endl;
    os << ("Your Daughter? ") << (g.m_YourDaughter ? "Yes": "No") << endl;
    os << ("Is Daughter? ") << (g.m_IsDaughter ? "Yes" : "No") << endl;
    os << ("Money: Min = ") << g.m_MinMoney << ". Max = " << g.m_MaxMoney << endl;
    /*
    *	loop through stats
    *	setw sets a field width for the next operation,
    *	left forces left alignment. Makes the columns line up.
    */
    for (unsigned int i = 0; i < NUM_STATS; i++)
    {
        os << setw(14) << left << get_stat_name((STATS)i) << ": Min = " << (g.m_MinStats[i]) << endl;
        os << setw(14) << "" << ": Max = " << (g.m_MaxStats[i]) << endl;
    }
    /*
    *	loop through skills
    */
    for (unsigned int i = 0; i < NUM_SKILLS; i++)
    {
        os << setw(14) << left << get_skill_name((SKILLS)i) << ": Min = " << int(g.m_MinSkills[i]) << endl;
        os << setw(14) << "" << ": Max = " << int(g.m_MaxSkills[i]) << endl;
    }
    /*
    *	loop through traits
    */
    for (int i = 0; i < g.m_TraitNames.size(); i++)
    {
        string name = g.m_TraitNames[i];
        int percent = int(g.m_TraitChance[i]);
        os << "Trait: " << setw(14) << left << name << ": " << percent << "%" << endl;
    }
    /*
    *	important to return the stream, so the next
    *	thing in the << chain has something on which to operate
    */
    return os;
}

sRandomGirl
cRandomGirls::CreateRandomGirl(bool Human0Monster1, bool arena, bool daughter, const std::string& findbyname)
{
    // If we do not have any girls to choose from, return a hardcoded "Error Girl"
    if (m_RandomGirls.empty()) {
        sRandomGirl hard_coded;
        hard_coded.m_Desc = "Hard Coded Random Girl\n(The game did not find a valid .rgirlsx file)";
        hard_coded.m_Name = "Default";
        hard_coded.m_Human = (Human0Monster1 == 0);
        hard_coded.m_Arena = arena;
        hard_coded.m_YourDaughter = daughter;

        return hard_coded;
    }

    std::cout << "\a\aCREATE RANDOM GIRL\n";
    // 1. The most direct check is to try to find a girl by name.
    if (!findbyname.empty())
    {
        auto candidate = find_random_girl_by_name(findbyname);
        if(candidate)
            return *candidate;
    }

    // 2. Next we see if you are looking for your own daughter
    if (daughter &&	m_NumRandomYourDaughterGirls > 0)
    {
        bool monstergirl = Human0Monster1;
        if (m_NumNonHumanRandomYourDaughterGirls < 1)
            monstergirl = false;
        // if there are no monster girls we will accept a human

        int offset = g_Dice % m_RandomGirls.size();	// pick a random stating point
        for(unsigned i = 0; i < m_RandomGirls.size(); ++i) {
            auto& candidate = m_RandomGirls.at((i + offset) % m_RandomGirls.size());
            if (!candidate.m_YourDaughter)
                continue;

            if ((bool)candidate.m_Human == !monstergirl) {
                return candidate;
            }
        }
    }

    int offset = g_Dice % m_RandomGirls.size();	// pick a random stating point
    for(unsigned i = 0; i < m_RandomGirls.size(); ++i) {
        auto& candidate = m_RandomGirls.at((i + offset) % m_RandomGirls.size());
        if (Human0Monster1 == (candidate.m_Human == 0))	{			// test for humanity
            return candidate;
        }
    }

    // if we couldn't find a girl that fits the specs, so we just take a random one and set the flags as we want
    // them. We make a copy here since we modify data.
    auto candidate = m_RandomGirls.at( g_Dice % m_RandomGirls.size() );
    candidate.m_Human = (Human0Monster1 == 0);
    candidate.m_Arena = arena;
    candidate.m_YourDaughter = daughter;
    return candidate;

}

void cRandomGirls::LoadRandomGirlXML(const std::string& filename)
{
    TiXmlDocument doc(filename);
    if (!doc.LoadFile())
    {
        g_LogFile.os() << "can't load random XML girls " << filename << endl;
        g_LogFile.os() << "Error: line " << doc.ErrorRow() << ", col " << doc.ErrorCol() << ": " << doc.ErrorDesc() << endl;
        return;
    }
    g_LogFile.os() << endl << "Loading File ::: " << filename << endl;
    TiXmlElement *el, *root_el = doc.RootElement();	// get the docuement root

    for (el = root_el->FirstChildElement(); el; el = el->NextSiblingElement())
    {
        m_RandomGirls.emplace_back();
        auto& girl = m_RandomGirls.back();
        girl.load_from_xml(el);					// uses sRandomGirl::load_from_xml

        if (girl.m_YourDaughter)
        {
            m_NumRandomYourDaughterGirls++;
            if (girl.m_Human)		m_NumHumanRandomYourDaughterGirls++;
            if (!girl.m_Human)		m_NumNonHumanRandomYourDaughterGirls++;
        }
        else
        {
            if (girl.m_Human)		m_NumHumanRandomGirls++;
            if (!girl.m_Human)		m_NumNonHumanRandomGirls++;
        }


    }
}


void sRandomGirl::process_trait_xml(TiXmlElement *el)
{
    int ival; const char *pt;
    if (!(pt = el->Attribute("Name"))) // `J` if there is no name why continue?
        return;

    m_TraitNames.emplace_back(pt);

    if (el->Attribute("Percent", &ival))							// get the percentage chance
    {
        m_TraitChance.emplace_back(ival);
    }
}

void sRandomGirl::process_item_xml(TiXmlElement *el)
{
    int ival; const char *pt;
    sInventoryItem *item = nullptr;
    if ((pt = el->Attribute("Name")))
    {
        string finditem = pt;
        item = g_Game->inventory_manager().GetItem(finditem);
        if (!item)
        {
            g_LogFile.os() << "Error: Can't find Item: '" << finditem << "' - skipping it." << endl;
            return;		// do as much as we can without crashing
        }
        m_ItemNames[m_NumItemNames] = item->m_Name;
    }
    if (m_NumItemNames<MAXNUM_INVENTORY) m_Inventory[m_NumItems] = item;
    if (el->Attribute("Percent", &ival))
    {
        if (m_NumItemNames<MAXNUM_INVENTORY)	m_ItemChance[m_NumItems] = ival;
        m_ItemChanceB[m_NumItemNames] = ival;
    }
    if (m_NumItemNames<MAXNUM_INVENTORY) m_NumItems++;
    m_NumItemNames++;
}

void sRandomGirl::process_stat_xml(TiXmlElement *el)
{
    int ival, index; const char *pt;
    if ((pt = el->Attribute("Name"))) index = get_stat_id(pt);
    else
    {
        g_LogFile.os() << "can't find 'Name' attribute - can't process stat" << endl;
        return;		// do as much as we can without crashing
    }
    if (el->Attribute("Min", &ival)) m_MinStats[index] = ival;
    if (el->Attribute("Max", &ival)) m_MaxStats[index] = ival;
}

void sRandomGirl::process_skill_xml(TiXmlElement *el)
{
    int ival, index;
    const char *pt;
    /*
    *	Strictly, I should use something that lets me
    *	test for absence. This won't catch typos in the
    *	XML file
    */
    if ((pt = el->Attribute("Name"))) index = get_skill_id(pt);
    else
    {
        g_LogFile.os() << "can't find 'Name' attribute - can't process skill" << endl;
        return;		// do as much as we can without crashing
    }
    if (el->Attribute("Min", &ival)) m_MinSkills[index] = ival;
    if (el->Attribute("Max", &ival)) m_MaxSkills[index] = ival;
}

void sRandomGirl::process_cash_xml(TiXmlElement *el)
{
    int ival;
    if (el->Attribute("Min", &ival))
    {
        if (cfg.debug.log_girls()) g_LogFile.os() << " min money = " << ival << endl;
        m_MinMoney = ival;
    }

    if (el->Attribute("Max", &ival))
    {
        if (cfg.debug.log_girls()) g_LogFile.os() << " max money = " << ival << endl;
        m_MaxMoney = ival;
    }
}

sRandomGirl* cRandomGirls::find_random_girl_by_name(const std::string& name)
{
    for(auto& rg : m_RandomGirls) {
        if(rg.m_Name == name) {
            return &rg;
        }
    }
    return nullptr;
}