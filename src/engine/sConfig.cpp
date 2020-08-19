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
#include "sConfig.h"
#include "utils/DirPath.h"
#include "CLog.h"
#include "utils/FileList.h"
#include <tinyxml2.h>
#include "interface/cColor.h"
#include "Revision.h"
#include "SDL.h"
#include "xml/util.h"
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

void cConfig::set_value(const char* id, std::string value) {
    data->set_value(id, std::move(value));
}

void cConfig::set_value(const char* id, bool value) {
    data->set_value(id, value);
}

void cConfig::save() {
    tinyxml2::XMLDocument doc;
    doc.LinkEndChild(doc.NewDeclaration());
    auto* pRoot = doc.NewElement("Config");
    doc.LinkEndChild(pRoot);
    data->save_xml(*pRoot);
    doc.SaveFile("config.xml");
}

/*
* changed this to take a filename so we can pass config files on the command line
* default is config.xml
*/
sConfigData::sConfigData(const char *a_filename)
{
    add_setting("folders.characters", "Character Folder", (DirPath() << "Resources" << "Characters").str());
    add_setting("folders.saves", "Save Folder", std::string("Saves"));
    add_setting("folders.items", "Items Folder", (DirPath() << "Resources" << "Items").str());
    add_setting("folders.default_images", "Items Folder", (DirPath() << "Resources" << "DefaultImages").str());
    add_setting("folders.backup_saves", "Backup Saves", false);
    add_setting("folders.prefer_defaults", "Prefer Defaults", false);

    add_setting("interface.theme", "Interface Theme", "J_1024x768");
    add_setting("interface.fullscreen", "Fullscreen", false);
    add_setting("interface.width", "Width", 1024);
    add_setting("interface.height", "Height", 768);
    add_setting("interface.list_scroll", "List Scroll", 3);
    add_setting("interface.text_scroll", "Text Scroll", 3);

    add_setting("font.font", "Font", "comic.ttf");

    DirPath dpdef = DirPath() << a_filename;    // `J` moved to root directory
    std::string filenamedef = dpdef.c_str();
    pXMLDocument doc;
    g_LogFile.info( "engine", "Attempting to load config file: ",filenamedef );
    try {
        doc = LoadXMLDocument(filenamedef);
        auto root = doc->RootElement();
        if(root)
            load_xml(*root);
    } catch( std::runtime_error& error ){
        g_LogFile.error("engine", "Could not load any config.xml files, using defaults.");
        return;
    }

    ReadItemData();
}

void sConfigData::ReadItemData()
{
    // check interface for colors
    DirPath dpi = DirPath() << "Resources" << "Interface" << get_str("interface.theme") << "InterfaceColors.xml";
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
