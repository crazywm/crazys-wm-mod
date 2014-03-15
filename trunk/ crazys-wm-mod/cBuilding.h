/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders 
 * who meet on http://pinkpetal.co.cc
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
#include <ostream>
#include <fstream>
#include <string>
#include <vector>
#include "cTariff.h"
#include "sFacility.h"

using namespace std;

class cBuilding
{
	int	m_capacity;
	int	m_free;
	
	vector<sFacility*>	m_facilities;
	vector<sFacility*>	*m_reversion;

public:

	int capacity()		{ return m_capacity; }
	int free_space()	{ return m_free; }
	int used_space()	{ return m_capacity - m_free; }

	cBuilding() {
		m_capacity	= 20;
		m_free		= 20;
		m_reversion	= 0;
	}

	bool add(sFacility *fac)
	{
		int needed = fac->space_taken();
		if(needed > m_free) {
			return false;
		}
		m_free -= needed;
		m_facilities.push_back(fac);
		return true;
	}
	sFacility *remove(int i) {
		sFacility* fac = m_facilities[i];
		m_facilities.erase(m_facilities.begin()+i);
		return fac;
	}
	sFacility *item_at(int i) {
		return m_facilities[i];
	}
	sFacility *operator[](int i) {
		return item_at(i);
	}
	int	size() {
		return m_facilities.size();
	}
	void commit() {
		for(u_int i = 0; i < m_facilities.size(); i++) {
			m_facilities[i]->commit();
		}
	}
	void revert()
	{
		for(u_int i = 0; i < m_facilities.size(); i++) {
			delete m_facilities[i];
		}
		m_facilities.clear();
		if(!m_reversion) {
			return;
		}
		m_free = m_capacity;
		for(u_int i = 0; i < m_reversion->size(); i++) {
			sFacility *fpt = (*m_reversion)[i];
			m_facilities.push_back(fpt);
			m_free -= fpt->space_taken();
		}
		delete m_reversion;
		m_reversion = 0;
	}

	ofstream &save(ofstream &ofs, string building_name);
	ifstream &load(ifstream &ifs);
/*
 *	is the list free of changes that may need to be reverted?
 */
	bool list_is_clean();
/*
 *	copies the current list so we can revert all changes maed
 */
	void make_reversion_list();
	void clear_reversion_list();
};


