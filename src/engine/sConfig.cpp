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
    explicit sConfigData(const char *filename);
    void load(const DirPath& source);
};

cConfig::cConfig(const std::string& file_name) : data(std::make_unique<sConfigData>(file_name.c_str())) {
}
cConfig::~cConfig() = default;
cConfig::cConfig(cConfig&& other)  noexcept = default;
cConfig& cConfig::operator=(cConfig&& other) noexcept = default;

cConfig cConfig::load(const std::string& filename)
{
    return {filename};
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

    load(DirPath() << a_filename);
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

const std::string& cConfig::characters() const { return data->get_str("folders.characters"); }
const std::string& cConfig::saves() const { return data->get_str("folders.saves"); }
const std::string& cConfig::items() const { return data->get_str("folders.items"); }
const std::string& cConfig::defaultimageloc() const { return data->get_str("folders.default_images");  }
bool cConfig::preferdefault() const { return data->get_bool("folders.prefer_defaults"); }

const std::string& cConfig::theme() const { return data->get_str("interface.theme"); }
int cConfig::width() const { return data->get_integer("interface.width"); }
int cConfig::height() const { return data->get_integer("interface.height"); }
bool cConfig::fullscreen() const { return data->get_bool("interface.fullscreen"); }
