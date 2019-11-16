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

#include "interface/cInterfaceWindow.h"

class cScreenGirlDetails : public cInterfaceWindowXML
{
private:
	int back_id;			// Back button
	int girlname_id;		// Girl Name text
	int girldesc_id;		// Girl Description text
	int girlimage_id;		// Girl image
	int more_id;			// More button
	int antipreg_id;		// Use Anti-preg checkbox
	int prev_id;			// Prev button
	int next_id;			// Next button
	int inventory_id;		// Inventory button
	int senddungeon_id;		// Send To Dungeon button
	int reldungeon_id;		// Release From Dungeon button
	int interact_id;		// Interact button
	int interactc_id;		// Interact Count
	int takegold_id;		// Take Gold button
	int accomup_id;			// Accom Up button
	int accomdown_id;		// Accom Down button
	int accom_id;			// Accom slider
	int accomval_id;		// Accom value text
	int houseperc_id;		// House Percentage slider
	int housepercval_id;	// House Percentage value text
	int gallery_id;			// Gallery button
	int jobtypehead_id;		// Job Types header text
	int jobtypelist_id;		// Job Types listbox
	int jobhead_id;			// Job header text
	int joblist_id;			// Job listbox
	int day_id;				// Day button
	int night_id;			// Night button
	int traithead_id;		// Trait header text
	int traitlist_id;		// Trait listbox
	int traitdesc_id;		// Trait Description text

	void set_ids() override;
	void RefreshJobList();
	void PrevGirl();
	void NextGirl();
	sGirl *get_prev_girl();
	sGirl *get_next_girl();
	bool do_take_gold(sGirl *girl, string &message);
	void take_gold(sGirl *girl);
    void set_shift(int shift);

	sGirl* m_SelectedGirl = nullptr;

    void OnKeyPress(SDL_keysym keysym) override;

    bool m_Refresh = false;
    int lastsexact = -1;
public:
    void set_image(int img);
	cScreenGirlDetails();

    void init(bool back) override;
	void process() override;

    void do_interaction();

    void send_to_dungeon();

    void release_from_dungeon();

    void update_accomodation(int accadj);

    void on_select_job(int selection, bool fulltime);

    void set_house_percentage(int value);
    void set_accomodation(int value);
};
