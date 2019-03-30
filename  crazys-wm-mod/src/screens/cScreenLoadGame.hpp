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

#ifndef CRAZYS_WM_MOD_CSCREENLOADGAME_HPP
#define CRAZYS_WM_MOD_CSCREENLOADGAME_HPP

#include "cInterfaceWindow.h"

class cScreenLoadGame : public cInterfaceWindow
{
private:
    // UI IDs
    int STATIC_STATIC;

    int IMAGE_BGIMAGE;
    int LIST_LOADGSAVES;
    int BUTTON_LOADGLOAD;
    int BUTTON_LOADGBACK;
    void load_game();

    void OnKeyPress(SDL_keysym keysym) override;
public:
    cScreenLoadGame();
    void init(bool back) override;
    void process() override {};

    // fake load
    void load() {};
};


#endif //CRAZYS_WM_MOD_CSCREENLOADGAME_HPP
