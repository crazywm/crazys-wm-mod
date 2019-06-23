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

class cScreenTurnSummary : public cInterfaceWindowXML
{
private:
	static bool ids_set;	// --- interface/event IDs --- //
	int brothel_id;
	int gold_id;			// Player Gold
	int category_id;
	int labelitem_id;
	int item_id;
	int event_id;
	int labeldesc_id;
	int goto_id;
	int nextweek_id;
	int back_id;
	int prev_id;
	int next_id;
	int image_id;
	int imagename_id;

	void set_ids();
public:
	cScreenTurnSummary();
	~cScreenTurnSummary();

	void init();
	void process();
	bool check_events();

	void Fill_Items_GIRLS();
	void Fill_Items_GANGS();
	void Fill_Items_BROTHELS();
	void Fill_Items_DUNGEON();
	void Fill_Items_STUDIO();
	void Fill_Items_ARENA();
	void Fill_Items_CENTRE();
	void Fill_Items_CLINIC();
	void Fill_Items_FARM();
	void Fill_Items_HOUSE();

	void Fill_Events(sGirl* girl);
	void Fill_Events_GANGS();
	void Fill_Events_BROTHELS();

};