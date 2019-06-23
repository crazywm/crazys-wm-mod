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

class cBuildingManagement : public cInterfaceWindowXML
{
public:
	struct IDBlock {
		int	unit;
		int	shade;
		int	desc;
		int	type;
		int	rename;
		int	space;
		int	cost;
		int	bevel_t, bevel_b, bevel_l, bevel_r;
		int	newsplash;
		int	delete_btn;

		int	glitz_cap, glitz_level, glitz_down, glitz_up;
		int	provides_cap, provides_level, provides_down, provides_up;
		int	secure_cap, secure_level, secure_down, secure_up;
		int	stealth_cap, stealth_level, stealth_down, stealth_up;

		void	hide(cBuildingManagement *mgr);
		void	display(cBuildingManagement *m, int n, cBuilding *b);
	};
private:
	sBrothel*	brothel;
	cBuilding*	building;
	int		selection;
	int		screen;

	static	bool		ids_set;
	/*
	*	interface/event IDs
	*/
	int		header_id;	// screen header - changes with brothel
	int		back_id;	// back button
	int		capacity_id;	// capacity/used/free line
	int		listbox_id;	// big listbox
	int		narrative_id;	// narrative text at bottom
	int		new_id;		// new button
	int		gold_id;	// gold display
	int		total_cost_id;	// total cost
	int		buy_button_id;
	int		revert_button_id;
	IDBlock		blocks[6];

	int		total_cost;

	void		set_ids();
public:
	cBuildingManagement()
	{

		DirPath dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "upgrade_screen.xml";
		m_filename = dp.c_str();
		selection = 0;
		screen = 0;
	}
	~cBuildingManagement() {}

	void init();
	void process();
	void buy_button();
	void revert_button();
	bool check_keys();
	void new_facility();

	void rename_button(int facility_idx);
	void delete_button(int facility_idx);
	void glitz_up(int facility_idx);
	void glitz_down(int facility_idx);
	void secure_down(int facility_idx);
	void secure_up(int facility_idx);
	void stealth_down(int facility_idx);
	void stealth_up(int facility_idx);
};
