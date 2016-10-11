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

class cScreenGirlManagement : public cInterfaceWindowXML 
{
public:

private:
	char	buffer[256];

	static bool ids_set;
/*
 *	interface/event IDs
 */
	int back_id;		// Back button
	int curbrothel_id;	// Current Brothel text
	int girllist_id;	// Girls listbox
	int girllistbig_id;	// Girls listbox (big)
	int girlimage_id;	// Girl image
	int girlimagesmall_id;	// Girl image (small)
	int expandimage_id; // Button to expand image size
	int shrinkimage_id; // Button to shrink image size
	int girldesc_id;	// Girl Description text
	int transfer_id;	// Transfer Girl button
	int firegirl_id;	// Fire Girl button
	int freeslave_id;	// Free Slave Button
	int viewdetails_id;	// View Details button
	int sellslave_id;	// Sell Slave button
	int jobtypehead_id;	// Job Types header text
	int jobtypelist_id;	// Job Types listbox
	int jobtypedesc_id;	// Job Types Description text
	int jobhead_id;		// Job header text
	int joblist_id;		// Job listbox
	int jobdesc_id;		// Job Description text
	int day_id;			// Day button
	int night_id;		// Night button
	int prev_id;
	int next_id;

	void set_ids();
	bool GirlDead(sGirl *dgirl, bool sendmessage = true);
	void RefreshSelectedJobType();
	void RefreshJobList();
	void GetSelectedGirls(vector<int> *girl_array);
	void ViewSelectedGirl();

public:

	cScreenGirlManagement()
	{
		DirPath dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "girl_management_screen.xml";
		m_filename = dp.c_str();
	}

	~cScreenGirlManagement() {}

	void init();
	void process();
	void check_events();
	bool check_keys();
	void update_image();
};
