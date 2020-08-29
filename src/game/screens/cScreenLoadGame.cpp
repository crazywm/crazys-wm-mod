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
#include "cScreenLoadGame.hpp"
#include "utils/FileList.h"
#include "widgets/cListBox.h"

extern std::string g_ReturnText;
extern int g_ReturnInt;



cScreenLoadGame::cScreenLoadGame(const std::string& save_folder) :
    cInterfaceWindowXML("LoadMenu.xml"),
    m_SavesPath(save_folder.c_str())
{
}

void cScreenLoadGame::load_game()
{
    int selection = GetLastSelectedItemFromList(id_saveslist);

    //    nothing selected means nothing more to do
    if (selection == -1)
    {
        return;
    }
    auto lp = m_SavesPath;
    lp << GetSelectedTextFromList(id_saveslist);
    g_ReturnText = lp.str();
    g_ReturnInt = 0;
    replace_window("Preparing Game");
}

void cScreenLoadGame::OnKeyPress(SDL_Keysym keysym)
{
    if(keysym.sym == SDLK_BACKSPACE) {
        pop_window();
    } else if (keysym.sym == SDLK_UP) {
        ArrowUpListBox(id_saveslist);
    } else if (keysym.sym == SDLK_DOWN) {
        ArrowDownListBox(id_saveslist);
    }else if (keysym.sym == SDLK_KP_ENTER) {
        load_game();
    }
}

void cScreenLoadGame::init(bool back)
{
    const char *pattern = "*.gam";
    FileList   fl       = FileList(m_SavesPath, pattern);

    Focused();
    ClearListBox(id_saveslist);    // clear the list box with the save games
    for (int i = 0; i < fl.size(); i++)                        // loop through the files, adding them to the box
    {
        if (fl[i].leaf() != "autosave.gam")    AddToListBox(id_saveslist, i, fl[i].leaf());
    }
}

void cScreenLoadGame::set_ids() {
    id_saveslist = get_id("FileName");
    int id_load = get_id("LoadGame");

    SetButtonCallback(id_load, [this]() { load_game(); });
    SetListBoxDoubleClickCallback(id_saveslist, [this](int sel) { load_game(); });
}