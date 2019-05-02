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
#include "cBuilding.h"
#include "CLog.h"
#include "tinyxml.h"
#include "XmlUtil.h"

ofstream &cBuilding::save(ofstream &ofs, string name)
{
	CLog log;
	string s;
	stringstream ss;
/*
 *	we're going to build an XML structure in memory
 *	and then splurt it down the profferred ostream.
 *	(hostream? I been working on the program too long...)
 */
	TiXmlDocument doc;
/*
 *	I think that the decl is the <?xml ...> twaddle at the top
 *
 *	not sure how well this will play embedded in mid file.
 */
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );  
	doc.LinkEndChild( decl ); 
/*
 *	document root node. In this case we'll use a <building> tag
 */
	TiXmlElement * root = new TiXmlElement("Building");  
/*
 *	use an attribute to store the capacity
 *	we can re-create the used and free counts, but we
 *	need to save the capacity
 */
	root->SetAttribute("Capacity", m_capacity);
	doc.LinkEndChild( root );  
/*
 *	let's add in a comment
 */
    root->SetAttribute("Name", name);
	doc.LinkEndChild( root );
	//TiXmlComment *comment = new TiXmlComment();
	//ss << "Building configuration for '" << name << "'";
	//comment->SetValue(ss.str().c_str());  
	//root->LinkEndChild( comment );  
/*
 *	Now we loop over all the rooms in the building
 */
	/*u_int lim = m_facilities.size();
 	for(u_int i = 0; i < lim; i++) {
		sFacility *fac = m_facilities[i];*/
/*
 *		OK: for each building, we need an element
 *		to store the data
 */
		/*TiXmlElement *el = new TiXmlElement("Facility");  
		el->SetAttribute("Capacity",	m_capacity);
		el->SetAttribute("Name",	fac->m_type_name);
		el->SetAttribute("Instance",	fac->m_instance_name);
		el->SetAttribute("Space",	fac->m_space_taken);
		el->SetAttribute("Provides",	fac->m_slots);
		el->SetAttribute("Price",	fac->m_base_price);
		el->LinkEndChild(fac->m_glitz.to_xml("Glitz"));
		el->LinkEndChild(fac->m_secure.to_xml("Secure"));
		el->LinkEndChild(fac->m_stealth.to_xml("Stealth"));
		root->LinkEndChild( el );  
	}*/

	ss.str("");
	ss << doc;
	s = ss.str();
	log.write(s);
	ofs << s;
	return ofs;
}

ifstream &cBuilding::load(ifstream &ifs)
{
	CLog log;
	TiXmlDocument doc;
	XmlUtil u("Loading building data from XML");
/*
 *	read the document
 */
	ifs >> doc;
	if(doc.Error()) {
		log.ss() << "error loading building data from XML: " << endl;
		log.ss()<< "Error: line " << doc.ErrorRow() << ", col " << doc.ErrorCol() << ": " << doc.ErrorDesc();
		log.ssend();
		return ifs;
	}
	log.ss() << "Loaded XML string: " << doc ;
	log.ssend();
/*
 *	get the docuement root
 */
	TiXmlElement /**el,*/ *root_el = doc.RootElement();
/*
 *	check the root for the capacity elemeent
 */
 	u.get_att(root_el, "Capacity", m_capacity);
	//u.get_att(root_el, "Name", ...);
/*
 *	loop over the elements attached to the root
 */
	/*m_free = m_capacity;
	for(	el = root_el->FirstChildElement();
		el ;
		el = el->NextSiblingElement()
	) {
		sFacility *fac = new sFacility();

		fac->load_from_xml(el);
		m_facilities.push_back(fac);
		m_free -= fac->space_taken();
	} */
	return ifs;
}

bool cBuilding::list_is_clean()
{
	u_int lim = m_facilities.size();
 	for(u_int i = 0; i < lim; i++) {
		sFacility *fac = m_facilities[i];

		if(fac->new_flag) {
			return false;
		}
	}
	return true;
}

void cBuilding::make_reversion_list()
{
/*
 *	clear out the old list if any
 *	(should be cleared by the buy or revert buttons)
 */
	if(m_reversion) {
		clear_reversion_list();
	}
	m_reversion = new vector<sFacility*>();

	u_int lim = m_facilities.size();
 	for(u_int i = 0; i < lim; i++) {
		sFacility *fac = m_facilities[i];

		m_reversion->push_back(
			new sFacility(*fac)
		);
	}
}

void cBuilding::clear_reversion_list()
{
	if(!m_reversion) {
		return;
	}
	vector<sFacility*> &list = (*m_reversion);
	u_int lim = list.size();
 	for(u_int i = 0; i < lim; i++) {
		delete list[i];
		list[i] = nullptr;
	}
	delete m_reversion;
	m_reversion = nullptr;
}
