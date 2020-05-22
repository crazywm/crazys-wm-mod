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
#include "cScreenNewGame.h"
#include "widgets/cEditBox.h"
#include "widgets/cSlider.h"
#include "cPlayer.h"
#include "Game.hpp"
#include "cGirls.h"

extern string monthnames[13];
extern string g_ReturnText;
extern int g_ReturnInt;

cScreenNewGame::cScreenNewGame() : cInterfaceWindowXML("NewGame.xml")
{
}

void cScreenNewGame::set_ids()
{
	ok_id			= get_id("Ok");
	cancel_id		= get_id("Cancel");
	brothel_id		= get_id("BrothelName");
	pname_id		= get_id("PlayerName");
	psname_id		= get_id("PlayerSurname");
	pbm_id			= get_id("PlayerBirthMonth");
	pbm1_id			= get_id("PlayerBirthMonthNum");
	pbd_id			= get_id("PlayerBirthDay");
	pbd1_id			= get_id("PlayerBirthDayNum");
	phn_id			= get_id("PlayerHoroscope");

	SetButtonNavigation(cancel_id, "Main Menu");
	SetButtonCallback(ok_id, [this]() { start_game(); });
	SetSliderCallback(pbm_id, [this](int) {update_birthday(); });
	SetSliderCallback(pbd_id, [this](int) {update_birthday(); });
}

void cScreenNewGame::init(bool back)
{
	Focused();
}

void cScreenNewGame::update_birthday()
{
    stringstream ss;
    g_Game.player().SetBirthDay(SliderValue(pbd_id));
    SliderValue(pbd_id, g_Game.player().BirthDay());
    g_Game.player().SetBirthMonth(SliderValue(pbm_id));
    SliderValue(pbm_id, g_Game.player().BirthMonth());
    ss << g_Game.player().BirthDay();
    EditTextItem(ss.str(), pbd1_id);
    ss.str("");
    ss << monthnames[g_Game.player().BirthMonth()];
    EditTextItem(ss.str(), pbm1_id);
    EditTextItem(cGirls::GetHoroscopeName(g_Game.player().BirthMonth(), g_Game.player().BirthDay()), phn_id);
}

void cScreenNewGame::start_game()
{
    string b = GetEditBoxText(brothel_id);
    string p = GetEditBoxText(pname_id);
    string s = GetEditBoxText(psname_id);
    if (b.empty() || p.empty() || s.empty())
    {
        push_message("You must enter a name in all 3 boxes.", COLOR_RED);
        return;
    }
    else	// ready to start the game now
    {
        g_ReturnInt = 1;
        g_ReturnText = b;
        g_Game.player().SetFirstName(p);
        g_Game.player().SetSurname(s);
        g_Game.player().SetRealName(p + " " + s);

        replace_window("Preparing Game");
        return;
    }
}

void cScreenNewGame::OnKeyPress(SDL_keysym keysym)
{
    if (keysym.sym == SDLK_ESCAPE && currentbox < m_EditBoxes.size())
    {
        m_EditBoxes[currentbox]->ClearText();
    }

    if (keysym.sym == SDLK_TAB || keysym.sym == SDLK_DOWN)
    {
        currentbox++;
        update_ui();
    }
    if ( keysym.sym == SDLK_UP)
    {
        currentbox--;
        update_ui();
    }

    if (keysym.sym == SDLK_PAGEDOWN || (currentbox == 3 && keysym.sym == SDLK_LEFT))
    {
        g_Game.player().BirthMonth(-1);
        update_ui();
    }
    if (keysym.sym == SDLK_PAGEUP || (currentbox == 3 && keysym.sym == SDLK_RIGHT))
    {
        g_Game.player().BirthMonth(1);
        update_ui();
    }
    if (keysym.sym == SDLK_END || (currentbox == 4 && keysym.sym == SDLK_LEFT))
    {
        g_Game.player().BirthDay(-1);
        update_ui();
    }
    if (keysym.sym == SDLK_HOME || (currentbox == 4 && keysym.sym == SDLK_RIGHT))
    {
        g_Game.player().BirthDay(1);
        update_ui();
    }

    if (currentbox == 4)
    {
        SDLKey numbers[] = {SDLK_0, SDLK_1, SDLK_2, SDLK_3, SDLK_4, SDLK_5,SDLK_6, SDLK_7, SDLK_8, SDLK_9};
        for(int num = 0; num <= 9; ++num) {
            if(keysym.sym == numbers[num]) {
                int cur = g_Game.player().BirthDay();
                int fin = 0;
                if (cur == 0)      fin = num;
                else if (cur > 9)  fin = ((cur % 10) * 10) + num;
                else if (cur < 10) fin = (cur * 10) + num;
                if (fin > 30)      fin = 30;

                g_Game.player().SetBirthDay(fin);
                update_ui();
            }
        }
    }

    SDLKey months[] = {SDLK_F1, SDLK_F2, SDLK_F3, SDLK_F4, SDLK_F5, SDLK_F6, SDLK_F7, SDLK_F8, SDLK_F9, SDLK_F10, SDLK_F11, SDLK_F12};
    for(int num = 0; num <= 12; ++num) {
        if (keysym.sym == months[num]) {
            g_Game.player().SetBirthMonth(num + 1);
            update_ui();
        }
    }
}

void cScreenNewGame::update_ui()
{
    stringstream ss;
    SliderValue(pbd_id, g_Game.player().BirthDay());
    ss << g_Game.player().BirthDay();
    EditTextItem(ss.str(), pbd1_id);
    ss.str("");
    SliderValue(pbm_id, g_Game.player().BirthMonth());
    ss << monthnames[g_Game.player().BirthMonth()];
    EditTextItem(ss.str(), pbm1_id);

    if (currentbox >= (int) m_EditBoxes.size() + (int) m_Sliders.size()) currentbox = 0;
    if (currentbox < 0) currentbox = (int) m_EditBoxes.size() + (int) m_Sliders.size();

    for (auto & editBox : m_EditBoxes) editBox->m_HasFocus = false;
    if (currentbox<3) m_EditBoxes[currentbox]->m_HasFocus = true;

    m_Sliders[0]->IsActive(currentbox == 3);
    m_Sliders[1]->IsActive(currentbox == 4);
    EditTextItem(cGirls::GetHoroscopeName(g_Game.player().BirthMonth(), g_Game.player().BirthDay()), phn_id);
}
