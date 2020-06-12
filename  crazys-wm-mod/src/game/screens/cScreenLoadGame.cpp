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
#include "cScreenLoadGame.hpp"
#include "CLog.h"
#include "FileList.h"
#include "sConfig.h"
#include <tinyxml2.h>
#include "widgets/cListBox.h"
#include "xml/getattr.h"
#include "xml/util.h"

extern cConfig cfg;
extern std::string g_ReturnText;
extern int g_ReturnInt;



cScreenLoadGame::cScreenLoadGame()
{
}

void cScreenLoadGame::load_game()
{
    int selection = GetLastSelectedItemFromList(LIST_LOADGSAVES);

    //	nothing selected means nothing more to do
    if (selection == -1)
    {
        return;
    }
    g_ReturnText = GetSelectedTextFromList(LIST_LOADGSAVES);;
    g_ReturnInt = 0;
    replace_window("Preparing Game");
}

void cScreenLoadGame::OnKeyPress(SDL_Keysym keysym)
{
    if(keysym.sym == SDLK_BACKSPACE) {
        pop_window();
    } else if (keysym.sym == SDLK_UP) {
        ArrowUpListBox(LIST_LOADGSAVES);
    } else if (keysym.sym == SDLK_DOWN) {
        ArrowDownListBox(LIST_LOADGSAVES);
    }else if (keysym.sym == SDLK_KP_ENTER) {
        load_game();
    }
}

void cScreenLoadGame::init(bool back)
{
    DirPath    location = DirPath(cfg.folders.saves().c_str());
    const char *pattern = "*.gam";
    FileList   fl       = FileList(location, pattern);

    Focused();
    ClearListBox(LIST_LOADGSAVES);	// clear the list box with the save games
    for (int i = 0; i < fl.size(); i++)						// loop through the files, adding them to the box
    {
        if (fl[i].leaf() != "autosave.gam")	AddToListBox(LIST_LOADGSAVES, i, fl[i].leaf());
    }
}

void cScreenLoadGame::load(cWindowManager* root)
{
    cInterfaceWindow::load(root);

    const int FONT_SIZE = 10;
    const int ROW_HEIGHT = 20;
    std::ifstream incol;

    auto dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "LoadMenu.xml";
    tinyxml2::XMLDocument docLoadMenu;
    if (docLoadMenu.LoadFile(dp.c_str()) == tinyxml2::XML_SUCCESS)
    {
        g_LogFile.log(ELogLevel::DEBUG, "Loading LoadMenu.xml");
        std::string m_filename = dp.c_str();
        auto* root_el = docLoadMenu.RootElement();
        for (auto& el : IterateChildElements(*root_el))
        {
            std::string name = GetStringAttribute(el, "Name");
            int x = GetIntAttribute(el, "XPos");
            int y = GetIntAttribute(el, "YPos");
            int w = GetIntAttribute(el, "Width");
            int h = GetIntAttribute(el, "Height");
            int e = el.IntAttribute("Border", 0);
            bool Transparency = el.BoolAttribute("Transparency", false);
            bool Scale = el.BoolAttribute("Scale", true);
            std::string image = GetDefaultedStringAttribute(el, "Image", "");
            std::string file = GetDefaultedStringAttribute(el, "File", "");
            std::string text = GetDefaultedStringAttribute(el, "Text", "");
            int fontsize = el.IntAttribute("FontSize", FONT_SIZE);
            int rowheight = el.IntAttribute("RowHeight", ROW_HEIGHT);
            int r = el.IntAttribute("Red", 0);
            int g = el.IntAttribute("Green", 0);
            int b = el.IntAttribute("Blue", 0);

            if (name == "LoadMenu")		CreateWindow(x, y, w, h, e);
            if (name == "WhoreMaster")	AddTextItem(STATIC_STATIC, x, y, w, h, text, fontsize, false, r, g, b);
            if (name == "FileName") {
                auto lb = AddListBox(x, y, w, h, e, true, false, false, true, true, fontsize, rowheight);
                LIST_LOADGSAVES = lb->get_id();
            }
            if (name == "LoadGame") AddButton(image, BUTTON_LOADGLOAD, x, y, w, h, Transparency, Scale);
            if (name == "BackButton") AddButton(image, BUTTON_LOADGBACK, x, y, w, h, Transparency, Scale);
            if (name == "Background")
            {
                DirPath dp = ImagePath(file);
                AddImage(IMAGE_BGIMAGE, dp, x, y, w, h);
            }
        }
    }
    else
    {
        // TODO get rid of this hardcoded default?
        g_LogFile.log(ELogLevel::INFO, "Loading Default LoadMenu");
        CreateWindow(224, 128, 344, 344, 1);
        AddTextItem(STATIC_STATIC, 0, 334 - 10, 344, 12, "Please read the readme.html", 10);
        auto lb = AddListBox(8, 8, 328, 288, 1, true, false,
                             false, true, true, FONT_SIZE, ROW_HEIGHT);
        LIST_LOADGSAVES = lb->get_id();
        AddButton("Load", BUTTON_LOADGLOAD, 8, 304, 160, 32, true, false);
        AddButton("Back", BUTTON_LOADGBACK, 176, 304, 160, 32, true, false);
    }

    SetButtonNavigation(BUTTON_LOADGBACK, "Main Menu");
    SetButtonCallback(BUTTON_LOADGLOAD, [this]() { load_game(); });
    SetListBoxDoubleClickCallback(LIST_LOADGSAVES, [this](int sel) { load_game(); });
}
