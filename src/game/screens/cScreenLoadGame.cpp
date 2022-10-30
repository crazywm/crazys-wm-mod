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
#include "widgets/IListBox.h"
#include "SavesList.h"

extern std::string g_ReturnText;
extern int g_ReturnInt;



cScreenLoadGame::cScreenLoadGame(const std::string& save_folder) :
    cInterfaceWindowXML("LoadMenu.xml"),
    m_SavesPath(DirPath::expand_path(save_folder).c_str())
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
    }
}

void cScreenLoadGame::init(bool back)
{
    SavesList list;
    list.BuildSaveGameList(m_SavesPath);

    Focused();
    auto lb = GetListBox(id_saveslist);
    lb->Clear();
    int i = 0;
    for (auto& save : list.get_saves(m_SavesPath))                        // loop through the files, adding them to the box
    {
        lb->AddRow(i, [&](const std::string& query) -> FormattedCellData {
            if(query == "Name") {
                return mk_text(save.File);
            } else if (query == "Money") {
                return mk_num(save.Data.Money);
            } else if (query == "WeeksPlayed") {
                return mk_num(save.Data.WeeksPlayed);
            } else {
                return mk_error(query);
            }
        }, COLOR_NEUTRAL);
        ++i;
    }
}

void cScreenLoadGame::set_ids() {
    id_saveslist = get_id("FileName");
    int id_load = get_id("LoadGame");

    SetButtonCallback(id_load, [this]() { load_game(); });
    SetButtonHotKey(id_load, SDLK_KP_ENTER);
    SetListBoxDoubleClickCallback(id_saveslist, [this](int sel) { load_game(); });
    SetListBoxHotKeys(id_saveslist, SDLK_UP, SDLK_DOWN);
}
