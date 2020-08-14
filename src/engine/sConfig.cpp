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
#include <ostream>
#include <cctype>
#include "sConfig.h"
#include "utils/DirPath.h"
#include "CLog.h"
#include "utils/FileList.h"
#include <tinyxml2.h>
#include "interface/cColor.h"
#include "Revision.h"
#include "SDL.h"
#include "xml/util.h"
#include "xml/getattr.h"
// TODO(refactor) get rid of this dependence!
#include "../game/Constants.h"

static cColor ColorConvert;
sConfigData *cConfig::data;

cConfig::cConfig()
{
    if (!data)
    {
        data = new sConfigData();
    }
}

/*
* changed this to take a filename so we can pass config files on the command line
* default is config.xml
*/
sConfigData::sConfigData(const char *a_filename)
    : fonts()
{

    DirPath dpdef = DirPath() << a_filename;    // `J` moved to root directory
    std::string filenamedef = dpdef.c_str();
    pXMLDocument doc;
    g_LogFile.info( "engine", "Attempting to load config file: ",filenamedef );
    try {
        doc = LoadXMLDocument(filenamedef);
    } catch( std::runtime_error& error ){
        g_LogFile.error("engine", "Could not load any config.xml files, using defaults.");
        g_LogFile.warning("engine", "*** Game will run with default pricing factors.\n*** This may seem a little easy. To fix this\n*** get a config.xml file from pinkpetal.org\n*** or make one with Whore Master Editor");
        set_defaults();
        return;
    }

    /*
    *    loop over the elements attached to the root
    */
    for (auto& el : IterateChildElements(*doc->RootElement()))
    {
        std::string tag = el.Value();        //    now, depending on the tag name...
        if (tag == "Folders")        { get_folders_data(&el);    continue; }
        if (tag == "Resolution")    { get_resolution_data(&el);    continue; }
        if (tag == "Catacombs")        { get_catacombs_data(&el);    continue; }
        if (tag == "Fonts")            { get_font_data(&el);        continue; }
        if (tag == "Debug")            { get_debug_flags(&el);        continue; }

        g_LogFile.warning("engine", "config.xml: tag: '", tag, "' unexpected");
    }
    ReadItemData();
    /// TODO add a way to specify this
    fonts.detailfontsize = 9;    // default to 9
}


void sConfigData::get_att(XMLElement *el, const char *name, bool &bval)
{
    const char *pt;

    pt = el->Attribute(name);
    /*
    *    if we didn't get a value, but it wasn't mandatory
    *    we can go
    */
    if (!pt) {
        g_LogFile.error("engine", "config.xml: No attribute named '", name, "' found.");
        return;
    }
    /*
    *    convert to a string, and then squash that to lower case
    */
    std::string s = pt;
    for(char& c : s) { c = std::tolower(c); }
    /*
    *    now we expect either "true" or "false"
    *    we can take "1" and "0" as well
    */
    if (s == "true" || s == "1")        { bval = true;    return; }
    if (s == "false" || s == "0")    { bval = false;    return; }

    g_LogFile.error("engine", "config.xml: : Binary attribute '", name, "': unexpected value '", s, "'");
}

void sConfigData::get_att(XMLElement *el, const char *name, int *ipt)
{
    int def_val = *ipt;
    if (el->QueryAttribute(name, ipt) == tinyxml2::XML_SUCCESS) { return; }
    g_LogFile.warning("engine", "config.xml: No '", name, "' attribute: defaulting to ", def_val);
    *ipt = def_val;
}

void sConfigData::get_folders_data(XMLElement *el)
{
    folders.backupsaves = false;
    folders.preferdefault = false;
    folders.characters        = (DirPath() << "Resources" << "Characters").c_str();
    folders.saves            = (DirPath() << "Saves").c_str();
    folders.items            = (DirPath() << "Resources" << "Items").c_str();
    folders.defaultimageloc    = (DirPath() << "Resources" << "DefaultImages").c_str();

    std::string testch = GetDefaultedStringAttribute(*el, "Characters", "");
    std::string testsa = GetDefaultedStringAttribute(*el, "Saves", "");
    std::string testil = GetDefaultedStringAttribute(*el, "Items", "");
    std::string testdi = GetDefaultedStringAttribute(*el, "DefaultImages", "");
    if (el->Attribute("BackupSaves"))        get_att(el, "BackupSaves", folders.backupsaves);
    if (el->Attribute("PreferDefault"))    get_att(el, "PreferDefault", folders.preferdefault);

    set_directory(folders.characters, testch, "Character");
    set_directory(folders.saves, testsa, "Saves");
    set_directory(folders.items, testil, "Items");
    set_directory(folders.defaultimageloc, testdi, "Default Images");

    // check if we can write to the save folder
    DirPath savegame_check = DirPath(folders.saves.c_str()) << ".Whore Master Save Games folder";
    FILE* fp = fopen(savegame_check.c_str(), "w");
    if (fp != nullptr) {
        fclose(fp);
    } else {
        g_LogFile.error("settings", "Cannot write to Saves directory");
        std::stringstream msg;
        msg << "Could not write to the save game directory '" << folders.saves << "'";
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Invalid Save Game Location",
                                     msg.str().c_str(),
                                     nullptr);

    }
}

void sConfigData::get_resolution_data(XMLElement *el)
{
    resolution.configXML = false;
    std::string testa;
    if (el->Attribute("Resolution")) {
        testa = GetStringAttribute(*el, "Resolution");
    }
    if (!testa.empty())
    {
        DirPath location = DirPath() << "Resources" << "Interface" << testa;
        FileList test(location, "*.xml");
        if (test.size() > 0)
        {
            resolution.resolution = testa;
            g_LogFile.info("engine", "Success: config.xml: Loading Interface: ", location.c_str());
        }
        else
        {
            g_LogFile.warning("engine", "config.xml: 'Resolution' attribute points to an invalid interface folder:\n\tDefaulting to 'J_1024x768'");
        }
    }
    else
    {
        g_LogFile.warning("engine", "config.xml: No Resolution specified, using defaults.");
    }
    if (el->Attribute("Width"))            { get_att(el, "Width", &resolution.width);        resolution.configXML = true; }
    if (el->Attribute("Height"))            { get_att(el, "Height", &resolution.height);    resolution.configXML = true; }
    if (el->Attribute("FullScreen"))        { get_att(el, "FullScreen", resolution.fullscreen); }
    el->QueryIntAttribute("ListScrollAmount", &resolution.list_scroll);
    el->QueryIntAttribute("TextScrollAmount", &resolution.text_scroll);
}

void sConfigData::get_catacombs_data(XMLElement *el)
{
    // load them
    el->QueryIntAttribute("GirlGetsGirls", &catacombs.girl_gets_girls);
    el->QueryIntAttribute("GirlGetsItems", &catacombs.girl_gets_items);
    el->QueryIntAttribute("GirlGetsBeast", &catacombs.girl_gets_beast);
    // make them positive
    if (catacombs.girl_gets_girls < 0) catacombs.girl_gets_girls = -catacombs.girl_gets_girls;
    if (catacombs.girl_gets_items < 0) catacombs.girl_gets_items = -catacombs.girl_gets_items;
    if (catacombs.girl_gets_beast < 0) catacombs.girl_gets_beast = -catacombs.girl_gets_beast;
    // make them percents
    double checkggirl = catacombs.girl_gets_girls + catacombs.girl_gets_items + catacombs.girl_gets_beast;
    if (checkggirl == 0) catacombs.girl_gets_girls = catacombs.girl_gets_items = catacombs.girl_gets_beast = (100 / 3);
    else if (checkggirl != 100)
    {
        catacombs.girl_gets_girls = int((100.0 / checkggirl) * (double)catacombs.girl_gets_girls);
        catacombs.girl_gets_items = int((100.0 / checkggirl) * (double)catacombs.girl_gets_items);
        catacombs.girl_gets_beast = int(100.0 - (double)catacombs.girl_gets_girls - (double)catacombs.girl_gets_items);
    }
}

void sConfigData::get_font_data(XMLElement *el)
{
    fonts.normal = GetDefaultedStringAttribute(*el, "Normal", fonts.normal.c_str());
    fonts.fixed = GetDefaultedStringAttribute(*el, "Fixed", fonts.fixed.c_str());
    if (el->Attribute("Antialias"))        get_att(el, "Antialias", fonts.antialias);
    if (el->Attribute("ShowPercent"))        get_att(el, "ShowPercent", fonts.showpercent);
}

void sConfigData::ReadItemData()
{
    // check interface for colors
    DirPath dpi = DirPath() << "Resources" << "Interface" << resolution.resolution << "InterfaceColors.xml";
    tinyxml2::XMLDocument doci;
    for (int i = 0; i < NUM_ITEM_RARITY; i++)
    {
        items.rarity_color[i] = new SDL_Color();
    }
    if (doci.LoadFile(dpi.c_str()) == tinyxml2::XML_SUCCESS)
    {
        for (auto& el : IterateChildElements(*doci.RootElement()))
        {
            std::string tag = el.Value();
            /// TODO move this to all the other interface stuff
            if (tag == "Color")
            {
                int r, g, b;
                if(el.QueryAttribute("R", &r) == tinyxml2::XML_SUCCESS && el.QueryAttribute("G", &g) == tinyxml2::XML_SUCCESS &&
                    el.QueryAttribute("B", &b) == tinyxml2::XML_SUCCESS) {
                    std::string name = el.Attribute("Name");
                    /* */if (name == "ItemRarity0") ColorConvert.RGBToSDLColor(items.rarity_color[0], r, g, b);
                    else if (name == "ItemRarity1") ColorConvert.RGBToSDLColor(items.rarity_color[1], r, g, b);
                    else if (name == "ItemRarity2") ColorConvert.RGBToSDLColor(items.rarity_color[2], r, g, b);
                    else if (name == "ItemRarity3") ColorConvert.RGBToSDLColor(items.rarity_color[3], r, g, b);
                    else if (name == "ItemRarity4") ColorConvert.RGBToSDLColor(items.rarity_color[4], r, g, b);
                    else if (name == "ItemRarity5") ColorConvert.RGBToSDLColor(items.rarity_color[5], r, g, b);
                    else if (name == "ItemRarity6") ColorConvert.RGBToSDLColor(items.rarity_color[6], r, g, b);
                    else if (name == "ItemRarity7") ColorConvert.RGBToSDLColor(items.rarity_color[7], r, g, b);
                    else if (name == "ItemRarity8") ColorConvert.RGBToSDLColor(items.rarity_color[8], r, g, b);
                } else {
                    g_LogFile.error("engine", "Error reading Color definition from '", dpi.c_str(), "': ",  el.GetLineNum());
                }

            }
        }
    }
}

void sConfigData::get_debug_flags(XMLElement *el)
{
    const char *pt;
    if (pt = el->Attribute("LogAll"))                get_att(el, "LogAll", debug.log_all);

    if (debug.log_all) debug.log_extra_details = debug.log_show_numbers = debug.log_all;
    else
    {
        if (pt = el->Attribute("LogExtraDetails"))    get_att(el, "LogExtraDetails", debug.log_extra_details);
        if (pt = el->Attribute("LogShowNumbers"))    get_att(el, "LogShowNumbers", debug.log_show_numbers);
    }
}

/*
* these just make sure the game has a playable
* set of factors in the event that the config
* file can't be found
*/
void sConfigData::set_defaults()
{
    folders.characters = "";                // `J` where the characters folder is located
    folders.saves = "";                        // `J` where the saves folder is located
    folders.items = "";                        // `J` where the items folder is located
    folders.backupsaves = false;            // `J` backup saves in the version folder incase moving to the next version breaks the save
    folders.defaultimageloc = "";            // `J` where the default images folder is located
    folders.preferdefault = false;            // `J` default images will be preferred over the alttype tree

    resolution.resolution = "J_1024x768";    // `J` I set this to my interface because that is the one I edit myself
    resolution.width = 1024;                // `J` added - Will be moved to interfaces
    resolution.height = 768;                // `J` added - Will be moved to interfaces
    resolution.fullscreen = false;            // `J` added - Will be moved to interfaces
    resolution.configXML = false;            // `J` added - Will be changed to interfaces
    resolution.list_scroll = 3;                // `Dagoth` added
    resolution.text_scroll = 3;                // `Dagoth` added

    catacombs.girl_gets_girls = 34;
    catacombs.girl_gets_items = 33;
    catacombs.girl_gets_beast = 33;

    ReadItemData();

    /*
    *    not hugely sensible values
    *    but I want something I'm not using so I can test this
    */
    fonts.normal = "comic.ttf";
    fonts.fixed = "comic.ttf";
    fonts.antialias = true;
    fonts.showpercent = false;
    fonts.detailfontsize = 9;

    debug.log_all = false;
    debug.log_extra_details = false;
    debug.log_show_numbers = false;
}

void sConfigData::set_directory(std::string& target, const std::string& setting, const char* name) {
    if (!setting.empty())
    {
        DirPath abs_ch = DirPath(setting.c_str());
        DirPath rel_ch = DirPath() << setting;
        if (abs_ch.is_directory())
        {
            target = abs_ch.c_str();
            g_LogFile.info("engine", "Success: config.xml: Setting ", name, " folder to absolute location: ", target);
        }
        else if (rel_ch.is_directory())
        {
            target = rel_ch.c_str();
            g_LogFile.info("engine", "Success: config.xml: Setting ", name, " folder to relative location: ", target);
        }
        else
        {
            g_LogFile.warning("engine", "config.xml: ", name, " folder '", setting, "' does not exist.\n\tDefaulting to ", target);
        }
    }
}
