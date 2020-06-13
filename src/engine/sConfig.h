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
#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace tinyxml2
{
    class XMLElement;
}

typedef unsigned int u_int;

struct SDL_Color;

struct sConfigData
{
    /*
    *    initialisation
    */
    struct {
        int            girl_meet;
        int            torture_mod;
    } initial;


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
        std::string resolution;
        int            width;
        int            height;
        int            scalewidth;
        int            scaleheight;
        bool        fullscreen;
        bool        fixedscale;
        bool        configXML;
        int            list_scroll;
        int            text_scroll;
        bool        next_turn_enter;
    } resolution;

    /*
    *    outgoings factors
    */
    struct OutFactors {
        double        training;
        double        actress_wages;
        double        movie_cost;
        double        goon_wages;
        double        matron_wages;
        double        staff_wages;        // `J` ?not used?
        double        girl_support;
        double        consumables;
        double        item_cost;
        double        slave_cost;
        double        brothel_cost;
        double        brothel_support;    // `J` ?not used?
        double        bribes;
        double        fines;
        double        advertising;
    } out_fact;

    struct ProstitutionData {
        double        rape_streets;
        double        rape_brothel;
    } prostitution;

    struct CatacombsData {
        int            unique_catacombs;
        int            girl_gets_girls;
        int            girl_gets_items;
        int            girl_gets_beast;
        int            gang_gets_girls;
        int            gang_gets_items;
        int            gang_gets_beast;
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

    std::string override_dir;

    sConfigData(const char *filename = "config.xml");

    using XMLElement = tinyxml2::XMLElement;

    void    set_defaults();

    void    get_expense_factors(XMLElement *el);
    void    get_pros_factors(XMLElement *el);
    void    get_catacombs_data(XMLElement *el);
    void    get_item_data(XMLElement *el);
    void    get_font_data(XMLElement *el);
    void    get_initial_values(XMLElement *el);
    void    get_folders_data(XMLElement *el);
    void    get_resolution_data(XMLElement *el);
    void    get_att(XMLElement *el, const char *name, int *data);
    void    get_att(XMLElement *el, const char *name, double        *data);
    void    get_att(XMLElement *el, const char *name, std::string &s);
    void    get_att(XMLElement *el, const char *name, bool        &bval);
    void    get_debug_flags(XMLElement *el);
};

class cConfig
{
    static    sConfigData    *data;
public:
    cConfig();

    /*
    *    outgoings factors
    */
    struct OutFactors {
        double        training()                { return data->out_fact.training; }
        double        goon_wages()            { return data->out_fact.goon_wages; }
        double        matron_wages()            { return data->out_fact.matron_wages; }
        double        consumables()            { return data->out_fact.consumables; }
        double        item_cost()                { return data->out_fact.item_cost; }
        double        slave_cost()            { return data->out_fact.slave_cost; }
        double        brothel_cost()            { return data->out_fact.brothel_cost; }
        double        brothel_support()        { return data->out_fact.brothel_support; }
        double        advertising()            { return data->out_fact.advertising; }
    } out_fact;

    struct ProstitutionData {
        double        rape_streets()            { return data->prostitution.rape_streets; }
        double        rape_brothel()            { return data->prostitution.rape_brothel; }
    } prostitution;

    struct CatacombsData {
        int&        unique_catacombs()        { return data->catacombs.unique_catacombs; }
        int&        girl_gets_girls()        { return data->catacombs.girl_gets_girls; }
        int&        girl_gets_items()        { return data->catacombs.girl_gets_items; }
        int&        girl_gets_beast()        { return data->catacombs.girl_gets_beast; }
        int&        gang_gets_girls()        { return data->catacombs.gang_gets_girls; }
        int&        gang_gets_items()        { return data->catacombs.gang_gets_items; }
        int&        gang_gets_beast()        { return data->catacombs.gang_gets_beast; }
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

    struct {
        int            girl_meet()                { return data->initial.girl_meet; }
        int            torture_mod()            { return data->initial.torture_mod; }
    } initial;

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
        int                scalewidth()            { return data->resolution.scalewidth; }
        int                scaleheight()            { return data->resolution.scaleheight; }
        bool            fixedscale()            { return data->resolution.fixedscale; }
        bool            fullscreen()            { return data->resolution.fullscreen; }
        bool            configXML()                { return data->resolution.configXML; }
        int                list_scroll()            { return data->resolution.list_scroll; }
        int                text_scroll()            { return data->resolution.text_scroll; }
        bool            next_turn_enter()        { return data->resolution.next_turn_enter; }
    } resolution;

    struct Debug{
        bool        log_extradetails()        { return data->debug.log_extra_details; }
        bool        log_show_numbers()        { return data->debug.log_show_numbers; }
    } debug;

};

