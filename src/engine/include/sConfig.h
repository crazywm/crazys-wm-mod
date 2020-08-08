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

namespace tinyxml2
{
    class XMLElement;
}

struct SDL_Color;

struct sConfigData
{
    /*
    *    Folders
    */
    struct Folders {
        std::string    characters;
        std::string saves;
        std::string items;
        bool        backupsaves;
        std::string defaultimageloc;
        bool        preferdefault;
    } folders;

    /*
    *    resolution
    */
    struct Resolution {
        std::string    resolution;
        int            width;
        int            height;
        bool           fullscreen;
        bool           configXML;
        int            list_scroll;
        int            text_scroll;
    } resolution;

    struct CatacombsData {
        int            girl_gets_girls;
        int            girl_gets_items;
        int            girl_gets_beast;
    } catacombs;

    struct item_data {
        SDL_Color*    rarity_color[9];
    } items;

    struct font_data {
        std::string normal;
        std::string fixed;
        bool        antialias;
        bool        showpercent;
        int            detailfontsize;
        font_data() : normal(""), fixed(""), antialias("") { }
    } fonts;

    struct DebugData {
        bool        log_all;
        bool        log_extra_details;
        bool        log_show_numbers;
    } debug;

    sConfigData(const char *filename = "config.xml");

    using XMLElement = tinyxml2::XMLElement;

    void    set_defaults();

    void    get_catacombs_data(XMLElement *el);
    void    ReadItemData();
    void    get_font_data(XMLElement *el);
    void    get_folders_data(XMLElement *el);
    void    get_resolution_data(XMLElement *el);
    void    get_att(XMLElement *el, const char *name, int *data);
    void    get_att(XMLElement *el, const char *name, bool        &bval);
    void    get_debug_flags(XMLElement *el);

    static void set_directory(std::string& target, const std::string& setting, const char* name);
};

class cConfig
{
    static    sConfigData    *data;
public:
    cConfig();

    struct CatacombsData {
        int&        girl_gets_girls()        { return data->catacombs.girl_gets_girls; }
        int&        girl_gets_items()        { return data->catacombs.girl_gets_items; }
        int&        girl_gets_beast()        { return data->catacombs.girl_gets_beast; }
    } catacombs;

    struct font_data {
        std::string&    normal()                { return data->fonts.normal; }
        std::string&    fixed()                    { return data->fonts.fixed; }
        bool&            antialias()                { return data->fonts.antialias; }
        int&            detailfontsize()        { return data->fonts.detailfontsize; }
    } fonts;

    struct item_data {
        SDL_Color* rarity_color(int num)    { return data->items.rarity_color[num]; }
    } items;

    struct Folders{
        std::string&    characters()            { return data->folders.characters; }
        std::string&    saves()                    { return data->folders.saves; }
        std::string&    items()                    { return data->folders.items; }
        bool&            backupsaves()            { return data->folders.backupsaves; }
        std::string&    defaultimageloc()        { return data->folders.defaultimageloc; }
        bool&            preferdefault()            { return data->folders.preferdefault; }

    }folders;

    struct Resolution{
        std::string&    resolution()            { return data->resolution.resolution; }
        int                width()                    { return data->resolution.width; }
        int                height()                { return data->resolution.height; }
        bool            fullscreen()            { return data->resolution.fullscreen; }
        bool            configXML()                { return data->resolution.configXML; }
        int                list_scroll()            { return data->resolution.list_scroll; }
        int                text_scroll()            { return data->resolution.text_scroll; }
    } resolution;

    struct Debug{
        bool        log_extradetails()        { return data->debug.log_extra_details; }
        bool        log_show_numbers()        { return data->debug.log_show_numbers; }
    } debug;

};

