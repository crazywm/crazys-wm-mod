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
#include "include/interface/cTheme.h"
#include "utils/cKeyValueStore.h"
#include "utils/DirPath.h"
#include "CLog.h"
#include "utils/FileList.h"
#include <tinyxml2.h>
#include "xml/util.h"
#include "xml/getattr.h"

namespace {
    struct sThemeData : public cSimpleKeyValue
    {
        explicit sThemeData(std::string filename);
    };
}

sThemeData::sThemeData(std::string filename) : cSimpleKeyValue("Entry", "Key", "Value") {
    using namespace IntWithBoundsFactoryFunctions;
    add_setting("list_scroll", "List Scroll", positive(3));
    add_setting("text_scroll", "Text Scroll", positive(3));
    add_setting("font", "Font", "comic.ttf");
    add_setting("directory", "Directory", "Light-16x9");
    add_setting("images", "Image Directory", "Default");
    add_setting("color-scheme", "Color Scheme", "LightColors.xml");
    add_setting("width", "Width", positive(1920));
    add_setting("height", "Height", positive(1080));

    DirPath source;
    source << "Resources" << "Interface" << std::move(filename) + ".xml";
    std::string filenamedef = source.c_str();
    pXMLDocument doc;
    g_LogFile.info( "engine", "Attempting to load config file: ",filenamedef );
    try {
        doc = LoadXMLDocument(filenamedef);
        auto root = doc->RootElement();
        if(root)
            load_xml(*root);
    } catch( std::runtime_error& error ) {
        g_LogFile.error("engine", "Could not load theme file, using defaults.");
        return;
    }
}

const std::string& cTheme::directory() const {
    return m_Directory;
}

int cTheme::base_width() const {
    return m_BaseWidth;
}
int cTheme::base_height() const {
    return m_BaseHeight;
}

int cTheme::list_scroll() const {
    return m_ListScroll;
}

int cTheme::text_scroll() const {
    return m_TextScroll;
}

std::string cTheme::normal_font() const {
    return m_Font;
}

void cTheme::load(std::string source) {
    auto data = sThemeData(std::move(source));
    m_ListScroll = data.get_integer("list_scroll");
    m_TextScroll = data.get_integer("text_scroll");
    m_Directory = data.get_str("directory");
    m_BaseWidth = data.get_integer("width");
    m_BaseHeight = data.get_integer("height");
    m_Font = data.get_str("font");
    m_ImageDirectory = data.get_str("images");

    // load interface colors
    try {
        DirPath dp = DirPath() << "Resources" << "Interface" << "ColorSchemes" << data.get_str("color-scheme");
        auto docInterfaceColors = LoadXMLDocument(dp.c_str());
        g_LogFile.log(ELogLevel::NOTIFY, "Loading ", dp.str());
        const std::string& m_filename = dp.str();
        for (auto& el : IterateChildElements(*docInterfaceColors->RootElement())) {
            std::string tag = el.Value();
            if (tag == "Color") {
                try {
                    std::string name = GetStringAttribute(el, "Name");
                    sColor target{0, 0, 0};
                    GetColorAttribute(el, target);
                    m_Colors.emplace(name, target);
                } catch(std::runtime_error& error) {
                    g_LogFile.log(ELogLevel::ERROR, "Could not load color definition from ", m_filename,
                                  "(", el.GetLineNum(), "): ", error.what());
                    continue;
                }
                // ItemRarity is loaded in sConfig.cpp
            }
        }
    } catch(std::runtime_error& error) {
        g_LogFile.log(ELogLevel::ERROR, "Could not load interface colors: ", error.what());
        g_LogFile.log(ELogLevel::NOTIFY, "Keeping Default InterfaceColors");
    }
}

void cTheme::set_screen_size(int width, int height) {
    m_ScreenWidth = width;
    m_ScreenHeight = height;
}

sColor cTheme::get_color(const std::string& lookup, const sColor& fallback) const {
    auto found = m_Colors.find(lookup);
    if(found == m_Colors.end()) {
        return fallback;
    }
    return found->second;
}

int cTheme::calc_x(int relative_x) const {
    return (relative_x * m_ScreenWidth) / m_BaseWidth;
}

int cTheme::calc_y(int relative_x) const {
    return (relative_x * m_ScreenHeight) / m_BaseHeight;
}

int cTheme::calc_w(int relative_w) const {
    return (relative_w * m_ScreenWidth) / m_BaseWidth;
}

int cTheme::calc_h(int relative_h) const {
    return (relative_h * m_ScreenHeight) / m_BaseHeight;
}

const std::string& cTheme::image_dir() const {
    return m_ImageDirectory;
}

std::string cTheme::get_image(const std::string& dir, const std::string& filename) const {
    DirPath path;
    path << "Resources" << "Themes" << image_dir() << dir << filename;
    if(path.exists()) {
        return path;
    } else {
        DirPath fallback;
        fallback << "Resources" << "Themes" << "Default" << dir << filename;
        return fallback;
    }
}
