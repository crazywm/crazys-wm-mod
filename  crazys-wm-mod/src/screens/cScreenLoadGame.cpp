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
#include "XmlUtil.h"
#include "FileList.h"
#include "InterfaceGlobals.h"
#include "sConfig.h"
#include "tinyxml.h"

extern cConfig cfg;
extern bool g_Cheats;
extern string g_ReturnText;
extern int g_ReturnInt;



cScreenLoadGame::cScreenLoadGame()
{
    int r = 0, g = 0, b = 0, x = 0, y = 0, w = 0, h = 0, e = 0, fontsize = 10, rowheight = 20;
    string image; string text; string file;
    bool Transparency = false, Scale = true;
    ifstream incol;

    auto dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "LoadMenu.xml";
    TiXmlDocument docLoadMenu(dp.c_str());
    if (docLoadMenu.LoadFile())
    {
        g_LogFile.write("Loading LoadMenu.xml");
        string m_filename = dp.c_str();
        TiXmlElement *el, *root_el = docLoadMenu.RootElement();
        for (el = root_el->FirstChildElement(); el; el = el->NextSiblingElement())
        {
            XmlUtil xu(m_filename); string name;
            xu.get_att(el, "Name", name);
            xu.get_att(el, "XPos", x); xu.get_att(el, "YPos", y); xu.get_att(el, "Width", w); xu.get_att(el, "Height", h); xu.get_att(el, "Border", e, true);
            xu.get_att(el, "Image", image, true); xu.get_att(el, "Transparency", Transparency, true); xu.get_att(el, "Scale", Scale, true);
            xu.get_att(el, "File", file, true); xu.get_att(el, "Text", text, true);
            xu.get_att(el, "FontSize", fontsize); if (fontsize == 0) fontsize = 10;
            xu.get_att(el, "RowHeight", rowheight); if (rowheight == 0) rowheight = 20;
            xu.get_att(el, "Red", r, true); xu.get_att(el, "Green", g, true); xu.get_att(el, "Blue", b, true);

            if (name == "LoadMenu")		CreateWindow(x, y, w, h, e);
            if (name == "WhoreMaster")	AddTextItem(STATIC_STATIC, x, y, w, h, text, fontsize, false, false, false, r, g, b);
            if (name == "FileName")		AddListBox(LIST_LOADGSAVES, x, y, w, h, e, true, false, false, true, true, fontsize, rowheight);
            if (name == "LoadGame")		AddButton(image, BUTTON_LOADGLOAD, x, y, w, h, Transparency, Scale);
            if (name == "BackButton")	AddButton(image, BUTTON_LOADGBACK, x, y, w, h, Transparency, Scale);
            if (name == "Background")
            {
                DirPath dp = ImagePath(file);
                AddImage(IMAGE_BGIMAGE, dp, x, y, w, h);
            }
        }
    }
    else
    {
        g_LogFile.write("Loading Default LoadMenu");
        CreateWindow(224, 128, 344, 344, 1);
        AddTextItem(STATIC_STATIC, 0, 334 - 10, 344, 12, "Please read the readme.html", 10);
        AddListBox(LIST_LOADGSAVES, 8, 8, 328, 288, 1, true, false, false, true, true, fontsize, rowheight);
        AddButton("Load", BUTTON_LOADGLOAD, 8, 304, 160, 32, true);
        AddButton("Back", BUTTON_LOADGBACK, 176, 304, 160, 32, true);
    }

    SetButtonNavigation(BUTTON_LOADGBACK, "Main Menu");
    SetButtonCallback(BUTTON_LOADGLOAD, [this]() { load_game(); });
    SetListBoxDoubleClickCallback(LIST_LOADGSAVES, [this](int sel) { load_game(); });
}

void cScreenLoadGame::load_game()
{
    int selection = GetLastSelectedItemFromList(LIST_LOADGSAVES);

    //	nothing selected means nothing more to do
    if (selection == -1)
    {
        return;
    }
    string temp = GetSelectedTextFromList(LIST_LOADGSAVES);
    g_ReturnText = temp;

    //	enable cheat mode for a cheat brothel
    g_Cheats = (temp == "Cheat.gam");

    g_ReturnInt = 0;
    replace_window("Preparing Game");
}

void cScreenLoadGame::OnKeyPress(SDL_keysym keysym)
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
