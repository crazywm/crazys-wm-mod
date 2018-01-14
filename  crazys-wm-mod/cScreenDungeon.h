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

class cScreenDungeon : public cInterfaceWindowXML
{
private:
	static bool ids_set;	// --- interface/event IDs --- //
	int back_id;			// Back button
	int girllist_id;		// Girls listbox
	int girlimage_id;		// Girl image
	int header_id;			// Dungeon header text
	int gold_id;			// Player Gold
	int brandslave_id;		// Brand Slave button
	int release_id;			// Release button
	int allowfood_id;		// Allow Food button
	int torture_id;			// Torture button
	int stopfood_id;		// Stop Feeding button
	int interact_id;		// Interact With button
	int interactc_id;		// Interact Count
	int releaseall_id;		// Release All Girls button
	int releasecust_id;		// Release All Customers button
	int viewdetails_id;		// Girl Details button
	int sellslave_id;		// Sell Slave button

	int releaseto_id;		// Where to release girls to Text
	int roomsfree_id;		// how many rooms are available 
	int house_id;			// Player's House button
	int clinic_id;			// Clinic's House button
	int studio_id;			// Studio's House button
	int arena_id;			// Arena's House button
	int centre_id;			// Centre's House button
	int farm_id;			// Farm's button
	int brothel0_id;		// Brothel 0 button
	int brothel1_id;		// Brothel 1 button
	int brothel2_id;		// Brothel 2 button
	int brothel3_id;		// Brothel 3 button
	int brothel4_id;		// Brothel 4 button
	int brothel5_id;		// Brothel 5 button
	int brothel6_id;		// Brothel 6 button

	int		sel_pos;
	int		selection;
	bool	no_guards;
	enum	{
		Return,
		Continue
	};

	void set_ids();
public:
	cScreenDungeon();
	~cScreenDungeon();

	enum {
		GirlLoses = 0,
		GirlWins = 1
	};
	bool	torture_possible();
	int		enslave();
	int		enslave_customer(int girls_removed, int custs_removed);
	int		process_events();
	int		view_girl();
	void	init();
	void	process();
	void	selection_change();
	void	set_slave_stats(sGirl *girl);
	void	release_all_customers();
	void	release_all_girls();
	void	sell_slaves();
	void	stop_feeding();
	void	start_feeding();
	void	torture_customer(int girls_removed);
	void	torture();
	void	change_release(string towhere);
	void	release();
	void	talk();
	void	get_selected_girls(vector<int> *girl_array);
	void	store_selected_girls();
	void	check_gang_for_survivors(sGirl *girl, sGang *gang, string &message, int start_count);
	void	update_image();

	// WD	Replaced by Doclox's cGirlTorture and cGirlGangFight code.
	//	int		girl_fights_gang(sGirl *girl, sGang *&gang);					
	//	bool	girl_fight_torture(sGirl *girl, string &message, bool &fight);


	// XML screen replacement for cSelector, for multi-select listboxes
	int multi_first() {
		sel_pos = 0;
		return GetNextSelectedItemFromList(girllist_id, 0, sel_pos);
	}
	int multi_next() {
		return GetNextSelectedItemFromList(girllist_id, sel_pos + 1, sel_pos);
	}
};
