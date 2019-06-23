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

class cScreenPrison : public cInterfaceWindowXML
{
private:
	static bool	ids_set;	// --- interface/event IDs --- //
	int header_id;			// screen header
	int back_id;			// Back button
	int more_id;			// More button
	int release_id;			// Release button
	int prison_list_id;		// Prisoner list
	int girl_desc_id;		// Girl description
	int girlimage_id;		// Girl image

	int selection;
	int DetailLevel;
	void		set_ids();
public:
	cScreenPrison();
	~cScreenPrison();

	void init();
	void process();
	void more_button();
	void release_button();
	int PrisonReleaseCost(sGirl* girl);
	void update_details();
	sGirl* get_selected_girl();
	void selection_change();
	bool check_keys();
	void	update_image();

};
