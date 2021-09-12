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
#include "Revision.h"
#include "xml/util.h"


struct sConfigData : public cSimpleKeyValue
{
    sConfigData(const char *filename = "config.xml");
    void load(const DirPath& source);
};

std::unique_ptr<sConfigData> cConfig::data;

cConfig::cConfig()
{
    if (!data)
    {
        data = std::make_unique<sConfigData>();
    }
}

void cConfig::reload(std::string const& filename)
{
   data = std::make_unique<sConfigData>(filename.c_str());
}

void cConfig::set_value(const char* id, std::string value) {
    data->set_value(id, std::move(value));
}

template void cConfig::set_value(const char* id, int value);
template void cConfig::set_value(const char* id, bool value);
template <typename T>
void cConfig::set_value(const char* id, T value) {
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
sConfigData::sConfigData(const char *a_filename) : cSimpleKeyValue("Entry", "Key", "Value")
{
    using namespace IntWithBoundsFactoryFunctions;

    add_setting("folders.characters", "Character Folder", (DirPath() << "Resources" << "Characters").str());
    add_setting("folders.saves", "Save Folder", std::string("Saves"));
    add_setting("folders.items", "Items Folder", (DirPath() << "Resources" << "Items").str());
    add_setting("folders.default_images", "Items Folder", (DirPath() << "Resources" << "DefaultImages").str());
    add_setting("folders.prefer_defaults", "Prefer Defaults", false);

    add_setting("interface.theme", "Interface Theme", "J_1024x768");
    add_setting("interface.fullscreen", "Fullscreen", false);
    add_setting("interface.width", "Width", positive(1024));
    add_setting("interface.height", "Height", positive(768));
    add_setting("interface.list_scroll", "List Scroll", positive(3));
    add_setting("interface.text_scroll", "Text Scroll", positive(3));

    add_setting("font.font", "Font", "comic.ttf");

    load(DirPath() << a_filename);

    // next, load the theme -- this overrides width and height
    load(DirPath() << "Resources" << "Interface" << get_str("interface.theme") << "config.xml");
}

void sConfigData::load(const DirPath& source) {
    std::string filenamedef = source.c_str();
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
}

const std::string& cConfig::font_data::normal() { return data->get_str("font.font"); }

const std::string& cConfig::Folders::characters() { return data->get_str("folders.characters"); }
const std::string& cConfig::Folders::saves() { return data->get_str("folders.saves"); }
const std::string& cConfig::Folders::items() { return data->get_str("folders.items"); }
const std::string& cConfig::Folders::defaultimageloc() { return data->get_str("folders.default_images");  }
bool cConfig::Folders::preferdefault() { return data->get_bool("folders.prefer_defaults"); }

const std::string& cConfig::Resolution::resolution() { return data->get_str("interface.theme"); }
int cConfig::Resolution::width() { return data->get_integer("interface.width"); }
int cConfig::Resolution::height() { return data->get_integer("interface.height"); }
bool cConfig::Resolution::fullscreen() { return data->get_bool("interface.fullscreen"); }
int cConfig::Resolution::list_scroll() { return data->get_integer("interface.list_scroll"); }
int cConfig::Resolution::text_scroll() { return data->get_integer("interface.text_scroll"); }
