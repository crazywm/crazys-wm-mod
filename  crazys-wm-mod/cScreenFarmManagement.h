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

class cScreenFarmManagement : public cInterfaceWindowXML
{
private:
	static bool ids_set;	// --- interface/event IDs --- //
	int curfarm_id;			// Current Brothel text
	int gold_id;			// Player Gold
	int girldesc_id;		// Girl Description text
	int girlimage_id;		// Girl image
	int girllist_id;		// Girls listbox
	int back_id;			// Back button
	int day_id;				// Day button
	int night_id;			// Night button
	int jobdesc_id;			// Job Description text
	int jobhead_id;			// Job header text
	int joblist_id;			// Job listbox
	int jobtypedesc_id;		// Job Types Description text
	int jobtypehead_id;		// Job Types header text
	int jobtypelist_id;		// Job Types listbox
	int firegirl_id;		// Fire Girl button
	int freeslave_id;		// Free Slave Button
	int sellslave_id;		// Sell Slave button
	int viewdetails_id;		// View Details button
	int transfer_id;		// Transfer Girl button

	void set_ids();
	void GetSelectedGirls(vector<int> *girl_array);
	void ViewSelectedGirl();
	void RefreshJobList();
	void RefreshSelectedJobType();

public:
	cScreenFarmManagement();
	~cScreenFarmManagement();

	void init();
	void process();
	void check_events();
	bool check_keys();
	void update_image();
};
