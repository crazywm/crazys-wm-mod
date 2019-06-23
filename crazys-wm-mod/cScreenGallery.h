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

class cScreenGallery : public cInterfaceWindowXML
{
private:
	static bool	ids_set;	// --- interface/event IDs --- //
	int		back_id;		// Back button
	int		next_id;		// next button
	int		prev_id;		// prev button
	int		image_id;		// the image
	int		imagename_id;	// image name
	int		imagelist_id;	// image type list

	int BuyBrothel;
	bool GetName;
	void set_ids();
public:
	cScreenGallery();
	~cScreenGallery();

	void init();
	void process();
	void check_events();
	bool check_keys();
	void more_button();
	void release_button();
	void update_details();
	sGirl* get_selected_girl();
	void selection_change();
	void update_image();

};