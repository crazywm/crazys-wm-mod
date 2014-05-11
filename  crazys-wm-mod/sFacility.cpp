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
#include "sFacility.h"
#include "XmlUtil.h"
#include "CLog.h"
#include "tinyxml.h"

void sFacility::load_from_xml(TiXmlElement *base_el)
{
	CLog l;
	XmlUtil u("Loading Facility Data from XML");

	new_flag = false;
	m_type_name = "Unknown";
	u.get_att(base_el, "Name", m_type_name);
	u.context(
		string("Loading Facility Data for ") +
		m_type_name +
		" from XML"
	);
	u.get_att(base_el,	"Instance",	m_instance_name, true);
	u.get_att(base_el,	"Space",	m_space_taken);
	u.get_att(base_el,	"Provides",	m_slots);
	u.get_att(base_el,	"Price",	m_base_price);


	TiXmlElement *el;
	for(	el = base_el->FirstChildElement();
		el ;
		el = el->NextSiblingElement()
	) {
		string tag = el->ValueStr();

		if(tag != "BoundedVar") {
			l.ss()	<< "Warning: Unexpected tag '"
				<< tag
				<< "': don't know what to do - ignoring."
			;
			l.ssend();
			continue;
		}
/*
 *		we have (at the time of writing) three BoundedVar variables
 *		to parse: "Secure", "Stealth" and "Glitz"
 *
 *		the struct knows how to parse itself,
 *		but we need to find out which one it is
 */
		string bvar_name;
		u.context(tag + " tag");
		u.get_att(el,	"Name",	bvar_name);
		l.ss() << "loading boundedvar " << bvar_name << endl;
		l.ssend();

		if(bvar_name == "Glitz") {
			m_glitz.from_xml(el);
		}
		else if(bvar_name == "Secure") {
			m_secure.from_xml(el);
		}
		else if(bvar_name == "Stealth") {
			m_stealth.from_xml(el);
		}
		else {
			l.ss()	<< "Warning: unknown name '"
				<< bvar_name
				<< "' for bounded var"
			;
			l.ssend();
		}
	}
	
}

TiXmlElement *sBoundedVar::to_xml(string name)
{
	TiXmlElement *el = new TiXmlElement("BoundedVar");  
	el->SetAttribute("Name", name);
	el->SetAttribute("Min", m_min);
	el->SetAttribute("Max", m_max);
	el->SetAttribute("Curr", m_curr);
	return el;
}

bool sBoundedVar::from_xml(TiXmlElement *el)
{
	CLog l;
	l.ss() << "sBoundedVar::from_xml called";
	l.ssend();
	XmlUtil u("Loading BoundedVar/Facility Data from XML");
/*
 *	The element we get passed should be a BoundedVar node
 *	so we just need to list the attributes
 */
	u.get_att(el, "Curr", m_curr);
	u.get_att(el, "Max", m_max);
	u.get_att(el, "Min", m_min);
	return true;
}

void sBoundedVar_Provides::init(sFacility *fac)
{
	m_min = fac->m_space_taken;
	m_max = 99;		// maybe set this in the XML
	m_curr = m_min;
	m_space = fac->m_space_taken;
/*
 *	we need to know how many slots per space
 */
	m_slots_per_space = 1.0 * m_curr / m_min;
}


/*

 *

 */
