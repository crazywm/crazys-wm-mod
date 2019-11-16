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
#include "interface/cInterfaceWindow.h"

class cScreenNewGame : public cInterfaceWindowXML
{
private:
	int ok_id;
	int cancel_id;
	int brothel_id;
	int pname_id;
	int psname_id;
	int pbm_id;
	int pbm1_id;
	int pbd_id;
	int pbd1_id;
	int phn_id;

	int currentbox = 0;

	void set_ids() override;
    void OnKeyPress(SDL_keysym keysym) override;
public:
	cScreenNewGame();

	void init(bool back) override;
	void process() override {}

    void update_ui();

    void start_game();

    void update_birthday();
};