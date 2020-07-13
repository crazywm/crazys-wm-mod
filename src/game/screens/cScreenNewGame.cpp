/*
* Copyright 2009, 2010, The Pink Petal Development Team.
* The Pink Petal Devloment Team are defined as the game's coders
* who meet on http://pinkpetal.org
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
#include "character/cPlayer.h"
#include "Game.hpp"
#include "cGirls.h"
#include "cNameList.h"
#include "InterfaceProcesses.h"
#include "cShop.h"
#include <sstream>

extern std::string monthnames[13];
extern std::string g_ReturnText;
extern int g_ReturnInt;
extern cNameList g_BoysNameList;;
extern cNameList g_SurnameList;

cScreenNewGame::cScreenNewGame() : cInterfaceWindowXML("NewGame.xml")
{
}

void cScreenNewGame::set_ids()
{
    ok_id            = get_id("Ok");
    config_id        = get_id("Config");
    cancel_id        = get_id("Cancel");
    brothel_id        = get_id("BrothelName");
    pname_id        = get_id("PlayerName");
    psname_id        = get_id("PlayerSurname");
    pbm_id            = get_id("PlayerBirthMonth");
    pbm1_id            = get_id("PlayerBirthMonthNum");
    pbd_id            = get_id("PlayerBirthDay");
    pbd1_id            = get_id("PlayerBirthDayNum");
    phn_id            = get_id("PlayerHoroscope");

    SetButtonNavigation(cancel_id, "Main Menu");
    SetButtonNavigation(config_id, "GameSetup", false);
    SetButtonCallback(ok_id, [this]() { start_game(); });
    SetSliderCallback(pbm_id, [this](int) {update_birthday(); });
    SetSliderCallback(pbd_id, [this](int) {update_birthday(); });
    SetSliderHotKeys(pbm_id, SDLK_PAGEUP, SDLK_PAGEDOWN);
    SetSliderHotKeys(pbd_id, SDLK_HOME, SDLK_END);
}

void cScreenNewGame::init(bool back)
{
    Focused();
    // randomize the player
    LoadNames();
    if(!back) {
        g_Game = std::make_unique<Game>();
        g_Game->player().SetBirthDay(g_Dice.in_range(1, 30));
        g_Game->player().SetBirthMonth(g_Dice.in_range(1, 12));
        g_Game->player().SetName(g_BoysNameList.random(), " ", g_SurnameList.random());
    }
    update_ui();
}

void cScreenNewGame::update_birthday()
{
    std::stringstream ss;
    g_Game->player().SetBirthDay(SliderValue(pbd_id));
    SliderValue(pbd_id, g_Game->player().BirthDay());
    g_Game->player().SetBirthMonth(SliderValue(pbm_id));
    SliderValue(pbm_id, g_Game->player().BirthMonth());
    ss << g_Game->player().BirthDay();
    EditTextItem(ss.str(), pbd1_id);
    ss.str("");
    ss << monthnames[g_Game->player().BirthMonth()];
    EditTextItem(ss.str(), pbm1_id);
    EditTextItem(cGirls::GetHoroscopeName(g_Game->player().BirthMonth(), g_Game->player().BirthDay()), phn_id);
}

void cScreenNewGame::start_game()
{
    std::string b = GetEditBoxText(brothel_id);
    std::string p = GetEditBoxText(pname_id);
    std::string s = GetEditBoxText(psname_id);
    if (b.empty() || p.empty() || s.empty())
    {
        push_message("You must enter a name in all 3 boxes.", COLOR_RED);
        return;
    }
    else    // ready to start the game now
    {
        g_ReturnInt = 1;
        g_ReturnText = b;
        g_Game->player().SetName(p, " ", s);

        replace_window("Preparing Game");
        return;
    }
}

// for some reason, when mingw compiles the following
// function with optimization it causes segfaults.
// therefore we tune down the optimization in that case
#ifdef __MINGW32__
#pragma GCC push_options
#pragma GCC optimize("Og")
#endif
void cScreenNewGame::OnKeyPress(SDL_Keysym keysym)
{
    if (keysym.sym == SDLK_TAB)
    {
        TabFocus();
        update_ui();
        return;
    }

    if(HasFocus(pbd_id)) {
        SDL_Keycode numbers[] = {SDLK_0, SDLK_1, SDLK_2, SDLK_3, SDLK_4, SDLK_5, SDLK_6, SDLK_7, SDLK_8, SDLK_9};
        for (int num = 0; num <= 9; ++num) {
            if (keysym.sym == numbers[num]) {
                int cur = g_Game->player().BirthDay();
                int fin = 0;
                if (cur == 0) fin = num;
                else if (cur > 9) fin = ((cur % 10) * 10) + num;
                else if (cur < 10) fin = (cur * 10) + num;
                if (fin > 30) fin      = 30;

                g_Game->player().SetBirthDay(fin);
                update_ui();
                return;     // don't let the key handling propagate here -- so any text field will not add the number
            }
        }
    }


    SDL_Keycode months[] = {SDLK_F1, SDLK_F2, SDLK_F3, SDLK_F4, SDLK_F5, SDLK_F6, SDLK_F7, SDLK_F8, SDLK_F9, SDLK_F10, SDLK_F11, SDLK_F12};
    for(int num = 0; num < 12; ++num) {
        if (keysym.sym == months[num]) {
            g_Game->player().SetBirthMonth(num + 1);
            update_ui();
        }
    }

    cInterfaceWindow::OnKeyPress(keysym);
}
#ifdef __MINGW32__
#pragma GCC pop_options
#endif

void cScreenNewGame::update_ui()
{
    SetEditBoxText(pname_id, g_Game->player().FirstName());
    SetEditBoxText(psname_id, g_Game->player().Surname());

    SliderValue(pbd_id, g_Game->player().BirthDay());
    EditTextItem(std::to_string(g_Game->player().BirthDay()), pbd1_id);
    SliderValue(pbm_id, g_Game->player().BirthMonth());
    EditTextItem(monthnames[g_Game->player().BirthMonth()], pbm1_id);
    EditTextItem(cGirls::GetHoroscopeName(g_Game->player().BirthMonth(), g_Game->player().BirthDay()), phn_id);
}
