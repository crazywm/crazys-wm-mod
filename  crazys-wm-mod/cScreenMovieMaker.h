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

class cScreenMovieMaker : public cInterfaceWindowXML 
{
public:

private:
	char	buffer[256];

	static bool ids_set;
/*
 *	interface/event IDs
 */
	int curstudio_id;	// Current Brothel text
	int back_id;		// Back button
	int sceneslist_id;	// Scenes listbox
	int releasemovie_id;	// Release movie

	void set_ids();
public:
	cScreenMovieMaker()
	{
		cConfig cfg;
		DirPath dp = DirPath()
			<< "Resources"
			<< "Interface"
			<< cfg.resolution.resolution()
			<< "movie_maker_screen.xml"
		;
		m_filename = dp.c_str();
	}
	~cScreenMovieMaker() {}

	void init();
	void process();
	void check_events();
	bool check_keys();
};
