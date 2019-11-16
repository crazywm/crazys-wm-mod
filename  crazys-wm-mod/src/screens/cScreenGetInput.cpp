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
#include "cScreenGetInput.h"

cScreenGetInput::cScreenGetInput() : cInterfaceWindowXML("getInput.xml")
{
}

void cScreenGetInput::set_ids()
{
	id_btn_ok	  = get_id("Ok");
	id_btn_cancel = get_id("Cancel");
	id_label	  = get_id("Label");
	id_textfield  = get_id("TextField");

	// set callbacks
	SetButtonCallback(id_btn_ok, [this]() {
        std::string text = GetEditBoxText(id_textfield);

        pop_window();

	    switch(m_profile) {
	    case MODE_CONFIRM:
	        if(m_ConfirmCallback)
	            m_ConfirmCallback();
	    break;
        case MODE_INT:
	        if(m_IntegerCallback)
	            m_IntegerCallback(atol(text.c_str()));
	        break;
	    case MODE_STRING:
            if(m_StringCallback)
                m_StringCallback(text);
            break;
	    }
	});

    SetButtonCallback(id_btn_cancel, [this]() {
        pop_window();
    });
}

void cScreenGetInput::process()
{
    /// TODO handle ENTER key
}

void cScreenGetInput::init(bool back)
{
	Focused();

	switch (m_profile)
	{
	case MODE_STRING:
	{
        HideWidget(id_textfield, false);
		EditTextItem("Enter Text:", id_label);
		break;
	}
	case MODE_INT:
	{
        HideWidget(id_textfield, false);
		EditTextItem("Enter Value:", id_label);
		break;
	}
	case MODE_CONFIRM:
	{
        HideWidget(id_textfield, true);
        EditTextItem("Confirm?", id_label);
		break;
	}
	}
}

void cScreenGetInput::ModeGetString(std::function<void(const std::string&)> callback)
{
    m_profile = MODE_STRING;
    m_StringCallback = std::move(callback);
}

void cScreenGetInput::ModeGetInt(std::function<void(int)> callback)
{
    m_profile = MODE_INT;
    m_IntegerCallback = std::move(callback);
}

void cScreenGetInput::ModeConfirm(std::function<void()> callback)
{
    m_profile = MODE_CONFIRM;
    m_ConfirmCallback = std::move(callback);
}
