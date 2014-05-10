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
#pragma once

#include "DirPath.h"
#include "cInterfaceWindow.h"
#include "InterfaceGlobals.h"

class cBuilding;

class cScreenTown : public cInterfaceWindowXML 
{
public:

private:
	int BuyBrothel;
	int BuyClinic;
	int BuyArena;
	int BuyStudio;
	int BuyCentre;
	bool GetClinic;
	bool GetArena;
	bool GetStudio;
	bool GetCentre;
	bool GetName;

	static	bool		ids_set;
/*
 *	interface/event IDs
 */
	int		back_id;		// Back button
	int		walk_id;		// Walk Around Town button
	int		curbrothel_id;	// Current Brothel text
	int		slavemarket_id;	// Slave Market map button
	int		shop_id;		// Shop map button
	int		mayor_id;		// Mayor's Office map button
	int		bank_id;		// Bank map button
	int		house_id;		// Player's House map button
	int		clinic_id;		// Clinic's House map button
	int		studio_id;		// Studio's House map button
	int		arena_id;		// Arena's House map button
	int		centre_id;		// Centre's House map button
	int		prison_id;		// Town Prison map button
	int		brothel0_id;	// Brothel 0 map button
	int		brothel1_id;	// Brothel 1 map button
	int		brothel2_id;	// Brothel 2 map button
	int		brothel3_id;	// Brothel 3 map button
	int		brothel4_id;	// Brothel 4 map button
	int		brothel5_id;	// Brothel 5 map button
	int		brothel6_id;	// Brothel 6 map button

	bool m_first_walk;

	void set_ids();
	void check_brothel(int BrothelNum);
	void check_clinic(int ClinicNum);
	void check_arena(int ArenaNum);
	void check_studio(int StudioNum);
	void check_centre(int CentreNum);
public:
	cScreenTown()
	{
		cConfig cfg;
		DirPath dp = DirPath()
			<< "Resources"
			<< "Interface"
			<< cfg.resolution.resolution()
			<< "town_screen.xml"
		;
		m_filename = dp.c_str();
		BuyBrothel = -1;
		BuyClinic = -1;
		GetName = false;
		m_first_walk = true;
	}
	~cScreenTown() {}

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