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
#pragma once

#include "DirPath.h"
#include "cInterfaceWindow.h"
#include "InterfaceGlobals.h"

class cBuilding;

class cCentreScreen : public cInterfaceWindowXML 
{
public:

private:
	int BuyBrothel;
	bool GetName;

	static	bool		ids_set;
/*
 *	interface/event IDs
 */
	int		back_id;		// Back button
	int		walk_id;		// Walk Around Town button
	int		girls_id;		// girl managment map button
	int		staff_id;		// gang map button
	int		turns_id;		// turn summary map button
	int		weeks_id;		// next week map button
	int		dungeon_id;		// dungeon map button
	int		setup_id;		// setup map button
	int		curbrothel_id;	// Current Brothel text
	int		centre_id;		// Movie's House map button
	int		nextbrothel_id;	// next brothel button
	int		prevbrothel_id;		// prev brothel button
	int     centredetails_id;    // centre description text

	bool m_first_walk;

	void set_ids();
	void check_brothel(int BrothelNum);
	void check_centre(int CentreNum);
public:
	cCentreScreen()
	{
		DirPath dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "centre_screen.xml";
		m_filename = dp.c_str();
		GetName = false;
		m_first_walk = true;
	}
	~cCentreScreen() = default;

	void init();
	void process();
	void more_button();
	void release_button();
	void update_details();
	sGirl* get_selected_girl();
	void selection_change();
	bool check_keys();
	void do_walk();
	string walk_no_luck();

};
