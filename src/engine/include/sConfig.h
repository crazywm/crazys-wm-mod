/*
* Copyright 2009, 2010, The Pink Petal Development Team.
* The Pink Petal Development Team are defined as the game's coders
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
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "utils/cKeyValueStore.h"

namespace tinyxml2
{
    class XMLElement;
}

struct SDL_Color;

struct sConfigData : public cSimpleKeyValue
{
    struct item_data {
        SDL_Color*    rarity_color[9];
    } items;

    sConfigData(const char *filename = "config.xml");
    void    ReadItemData();
};

class cConfig
{
    static    sConfigData    *data;
public:
    cConfig();

    struct font_data {
        const std::string&    normal()             { return data->get_str("font.font"); }
    } fonts;

    struct item_data {
        SDL_Color* rarity_color(int num)    { return data->items.rarity_color[num]; }
    } items;

    struct Folders {
        const std::string&    characters()         { return data->get_str("folders.characters"); }
        const std::string&    saves()              { return data->get_str("folders.saves"); }
        const std::string&    items()              { return data->get_str("folders.items"); }
        const std::string&    defaultimageloc()    { return data->get_str("folders.default_images");  }
        bool                  backupsaves()        { return data->get_bool("folders.backup_saves"); }
        bool                  preferdefault()      { return data->get_bool("folders.prefer_defaults"); }
    } folders;

    struct Resolution{
        const std::string& resolution()            { return data->get_str("interface.theme"); }
        int                width()                 { return data->get_integer("interface.width"); }
        int                height()                { return data->get_integer("interface.height"); }
        bool               fullscreen()            { return data->get_bool("interface.fullscreen"); }
        int                list_scroll()           { return data->get_integer("interface.list_scroll"); }
        int                text_scroll()           { return data->get_integer("interface.text_scroll"); }
    } resolution;

    void set_value(const char* id, std::string value);
    void set_value(const char* id, bool value);

    void save();
};

