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

class cScreenSettings : public cInterfaceWindowXML
{
private:
	static bool ids_set;	// --- interface/event IDs --- //
	int back_id;
	int ok_id;
	int revert_id;

	// Folders
	int characters_id;
	int saves_id;
	int defaultimages_id;
	int items_id;
	int backupsaves_id;
	int preferdefault_id;

	// Catacombs
	int uniquecatacombs_id;
	int uniquecatacombslabel_id;
	int controlgirls_id;
	int controlgangs_id;
	int girlgetsgirls_id;
	int girlgetsitems_id;
	int girlgetsbeast_id;
	int girlsgirlspercslider_id;
	int girlsitemspercslider_id;
	int ganggetsgirls_id;
	int ganggetsitems_id;
	int ganggetsbeast_id;
	int gangsgirlspercslider_id;
	int gangsitemspercslider_id;






public:
	void set_ids();

	cScreenSettings();
	void init();
	void process();
	void check_events();
	bool check_keys();

	void Free() { cInterfaceWindow::Free(); }
};