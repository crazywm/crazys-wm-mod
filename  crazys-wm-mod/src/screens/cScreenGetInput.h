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

#include <functional>
#include "cInterfaceWindow.h"

enum INPUT_MODE {
	MODE_STRING,
	MODE_INT,
	MODE_CONFIRM,
};

class cScreenGetInput : public cInterfaceWindowXML
{
	INPUT_MODE m_profile = MODE_INT; // GetInt, GetString or GetConfirm?
	int id_btn_ok;
	int id_btn_cancel;
	int id_label;
	int id_textfield;
public:
	cScreenGetInput();
	void set_ids() override;
	void init(bool back) override;
	void process() override;

    void ModeGetString(std::function<void(const std::string&)> callback);
	void ModeGetInt(std::function<void(int)> callback);
	void ModeConfirm(std::function<void()> callback);

    std::function<void()> m_ConfirmCallback;
	std::function<void(int)> m_IntegerCallback;
	std::function<void(const std::string&)> m_StringCallback;
};
