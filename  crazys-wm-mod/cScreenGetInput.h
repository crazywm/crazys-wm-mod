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

enum INPUT_MODE {
	MODE_STRING,
	MODE_INT,
	MODE_CONFIRM,
	MODE_CONFIRM_EXIT,
};

class cScreenGetInput : public cInterfaceWindowXML
{
	bool ids_set = false;
	INPUT_MODE m_profile = MODE_INT; // GetInt, GetString or GetConfirm?
	int id_btn_ok;
	int id_btn_cancel;
	int id_label;
	int id_textfield;
public:
	cScreenGetInput();
	~cScreenGetInput();
	void set_ids();
	void init();
	void process();
	void check_events();
	bool check_keys();
	void Free()				{ cInterfaceWindowXML::Free(); }
	void ModeGetString()	{ m_profile = MODE_STRING; };
	void ModeGetInt()		{ m_profile = MODE_INT; };
	void ModeConfirm()		{ m_profile = MODE_CONFIRM; };
	void ModeConfirmExit()	{ m_profile = MODE_CONFIRM_EXIT; };
	void CheckEvents_ConfirmExit();
};
