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
#include "cScreenMainMenu.h"
#include "utils/FileList.h"
#include "Revision.h"
#include <SDL.h>
#include "IGame.h"

extern std::string g_ReturnText;
extern int g_ReturnInt;

void cScreenMainMenu::set_ids()
{
    continue_id   = get_id("Continue");
    load_id       = get_id("Load Game");
    version_id    = get_id("Version");
    int new_id        = get_id("New Game");
    int settings_id   = get_id("Settings");
    int quit_id       = get_id("Quit Game");

    SetButtonHotKey(new_id, SDLK_n);
    SetButtonHotKey(load_id, SDLK_l);
    SetButtonHotKey(settings_id, SDLK_s);
    SetButtonCallback(continue_id, [this]()
    {
        g_ReturnInt = 0;
        g_ReturnText = (DirPath(m_SaveGamesPath.c_str()) << "autosave.gam").str();
        push_window("Preparing Game");
    });
    SetButtonHotKey(continue_id, SDLK_c);

    SetButtonCallback(quit_id, [this]()
    {
        SDL_Event evn;
        evn.type = SDL_QUIT;
        SDL_PushEvent(&evn);
    });
    SetButtonHotKey(quit_id, SDLK_q);

}

cScreenMainMenu::cScreenMainMenu(std::string saves_path) :
    cInterfaceWindowXML("main_menu.xml"),
    m_SaveGamesPath(DirPath::expand_path(std::move(saves_path)))
{
}

void cScreenMainMenu::init(bool back)
{
    Focused();

    DirPath location = DirPath(m_SaveGamesPath.c_str());
    FileList fl(location, "autosave.gam");
    FileList fla(location, "*.gam");
    bool d_continue = fl.size() < 1;
    bool d_load = (fla.size() < 1 || (fla.size() == 1 && !d_continue));
    DisableWidget(continue_id, d_continue);    // `J` disable continue button if autosave.gam is not found
    DisableWidget(load_id, d_load);            // `J` disable load game button if there are no save games found
    if (version_id >= 0) EditTextItem(VERSION_STRING, version_id);

    g_Game = nullptr;
}
