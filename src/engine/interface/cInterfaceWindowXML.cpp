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

#include "interface/cInterfaceWindowXML.h"
#include "utils/DirPath.h"
#include "CLog.h"
#include <tinyxml2.h>
#include "xml/util.h"
#include "xml/getattr.h"

#include "widgets/cListBox.h"
#include "interface/cWindowManager.h"
#include "include/interface/cTheme.h"
#include <boost/optional.hpp>

cInterfaceWindowXML::cInterfaceWindowXML(const char* base_file) :
        cInterfaceWindow(base_file),
        m_ScreenName(base_file) {
}

cInterfaceWindowXML::~cInterfaceWindowXML()
{
    g_LogFile.debug("interface", "~cInterfaceWindowXML: ", m_ScreenName);
}

void cInterfaceWindowXML::load(cWindowManager* wm)
{
    cInterfaceWindow::load(wm);
    DirPath xml_source = {};
    xml_source << "Resources" << "Interface" << wm->GetTheme().directory() << m_ScreenName;
    auto doc = LoadXMLDocument(xml_source.str());
    if(doc->RootElement()->Attribute("Extends")) {
        DirPath base_source;
        base_source << "Resources" << "Interface" << wm->GetTheme().directory() << doc->RootElement()->Attribute("Extends");
        read_definition(*LoadXMLDocument(base_source.str())->RootElement());
    }
    read_definition(*doc->RootElement());

    g_LogFile.debug("interface", "calling set_ids for window ", m_ScreenName);
    set_ids();
}

void cInterfaceWindowXML::read_definition(const tinyxml2::XMLElement& root) {
    for (auto& el : IterateChildElements(root)) {
        std::string tag = el.Value();
        try {
            if (tag == "Window") { read_window_definition(el);            continue; }
            if (tag == "EditBox") { read_editbox_definition(el);        continue; }
            if (tag == "Text") { read_text_item(el);                    continue; }
            if (tag == "Button") { read_button_definition(el);            continue; }
            if (tag == "Image") { read_image_definition(el);            continue; }
            if (tag == "ListBox") { read_listbox_definition(el);        continue; }
            if (tag == "Checkbox"){ read_checkbox_definition(el);        continue; }
            if (tag == "Slider") { read_slider_definition(el);            continue; }
        } catch(std::runtime_error& e) {
            g_LogFile.error("interface", "Could not create Widget ", tag, " from file '", m_ScreenName, "': ", e.what());
        }

        g_LogFile.log(ELogLevel::WARNING, "Unexpected tag in '", m_ScreenName, "': '", tag, '\'');
    }
}

int cInterfaceWindowXML::read_x_coordinate(const tinyxml2::XMLElement& element, const char* attribute) const {
    return window_manager().GetTheme().calc_x(GetIntAttribute(element, attribute));
}

int cInterfaceWindowXML::read_y_coordinate(const tinyxml2::XMLElement& element, const char* attribute) const {
    return window_manager().GetTheme().calc_y(GetIntAttribute(element, attribute));
}

int cInterfaceWindowXML::read_width(const tinyxml2::XMLElement& element, const char* attribute, boost::optional<int> fallback) const {
    if(!fallback) {
        return window_manager().GetTheme().calc_w(GetIntAttribute(element, attribute));
    } else {
        return window_manager().GetTheme().calc_w(element.IntAttribute(attribute, fallback.value()));
    }
}

int cInterfaceWindowXML::read_height(const tinyxml2::XMLElement& element, const char* attribute, boost::optional<int> fallback) const {
    if(!fallback) {
        return window_manager().GetTheme().calc_h(GetIntAttribute(element, attribute));
    } else {
        return window_manager().GetTheme().calc_h(element.IntAttribute(attribute, fallback.value()));
    }
}

void cInterfaceWindowXML::read_listbox_definition(const tinyxml2::XMLElement& el)
{
    // TODO widget_listbox seems incomplete, leave like this for now
    std::string name = GetStringAttribute(el, "Name");
    int x = read_x_coordinate(el, "XPos");
    int y = read_y_coordinate(el, "YPos");
    int w = read_width(el, "Width", boost::none);
    int h = read_height(el, "Height", boost::none);
    int fontsize = read_height(el, "FontSize", 10);
    int rowheight = read_height(el, "RowHeight", 20);

    int border_size = el.IntAttribute("Border", 1);
    bool events = el.BoolAttribute("Events", true);
    bool multi = el.BoolAttribute("Multi", false);
    bool show_headers = el.BoolAttribute("ShowHeaders", false);
    bool header_div = el.BoolAttribute("HeaderDiv", true);
    bool header_sort = el.BoolAttribute("HeaderClicksSort", true);

    auto box = AddListBox(x, y, w, h, border_size, events, multi, show_headers, header_div, header_sort, fontsize, rowheight);
    register_id(box->get_id(), name);

    // Check for column definitions
    std::vector<int> column_offset;
    std::vector<bool> column_skip;
    std::vector<std::string> column_name, column_header;
    for (auto& sub_el : IterateChildElements(el))
    {
        std::string tag = sub_el.Value();
        // XML definition can arrange columns in any order and even leave some columns out if desired
        // Listbox itself (via DefineColumns) keeps track of what order they're to be displayed in based on this
        if (tag == "Column")   // hey, we've got a column definition in this listbox
        {
            std::string name = GetStringAttribute(sub_el, "Name");
            int offset = read_width(sub_el, "Offset", 0);
            bool skip = sub_el.BoolAttribute("Skip", false);
            std::string header = GetDefaultedStringAttribute(sub_el, "Header", name.c_str());

            column_name.push_back(std::move(name));
            column_header.push_back(std::move(header));
            column_offset.push_back(offset);
            column_skip.push_back(skip);
        }
        else
        {
            g_LogFile.log(ELogLevel::WARNING, "Unhandled listbox element: '", tag, "'");
        }
    }
    // If we have columns defined, go ahead and give the listbox all the gory details
    if (!column_name.empty())
      box->DefineColumns(column_name, column_header,
                         column_offset, column_skip);
}

void cInterfaceWindowXML::read_text_item(const tinyxml2::XMLElement& el)
{
    int id;
    /*
    *    create the text item
    */
    AddTextItem(id,
                read_x_coordinate(el, "XPos"),
                read_y_coordinate(el, "YPos"),
                read_width(el, "Width", boost::none),
                read_height(el, "Height", boost::none),
                GetStringAttribute(el, "Text"),
                read_height(el, "FontSize", 11),
                el.BoolAttribute("ForceScrollbar", false),
                el.IntAttribute("Red", -1),
                el.IntAttribute("Green", -1),
                el.IntAttribute("Blue", -1));
    /*
    *    make a note of the ID
    */
    register_id(id, GetStringAttribute(el, "Name"));
}


void cInterfaceWindowXML::read_window_definition(const tinyxml2::XMLElement& el)
{
    CreateWindow(read_x_coordinate(el, "XPos"),
                 read_y_coordinate(el, "YPos"),
                 read_width(el, "Width", boost::none),
                 read_height(el, "Height", boost::none),
                 GetIntAttribute(el, "Border"));
}

void cInterfaceWindowXML::read_editbox_definition(const tinyxml2::XMLElement& el)
{
    int id;
    AddEditBox(id,
               read_x_coordinate(el, "XPos"),
               read_y_coordinate(el, "YPos"),
               read_width(el, "Width", boost::none),
               read_height(el, "Height", boost::none),
               el.IntAttribute("Border", 0),
               read_height(el, "FontSize", 16));
    register_id(id, GetStringAttribute(el, "Name"));
}

void cInterfaceWindowXML::read_checkbox_definition(const tinyxml2::XMLElement& el)
{
    int id;
    AddCheckbox(id,
                read_x_coordinate(el, "XPos"),
                read_y_coordinate(el, "YPos"),
                read_width(el, "Width", boost::none),
                read_height(el, "Height", boost::none),
                GetStringAttribute(el, "Text"),
                read_height(el, "FontSize", 11),
                el.BoolAttribute("LeftOrRight", true));
    register_id(id, GetStringAttribute(el, "Name"));
}

void cInterfaceWindowXML::read_image_definition(const tinyxml2::XMLElement& el)
{
    std::string file = GetStringAttribute(el, "File");
    std::string type;
    if(file.empty()) {
        type = {};
    } else {
        type = GetStringAttribute(el, "Dir");
    }
    int width = read_width(el, "Width", boost::none);
    int height = read_height(el, "Height", boost::none);
    int min_width = width;
    int min_height = height;
    if(el.Attribute("MinWidth")) {
        min_width = read_width(el, "MinWidth", boost::none);
    }
    if(el.Attribute("MinHeight")) {
        min_height = read_height(el, "MinHeight", boost::none);
    }
    int id = AddImage(type, file,
                      read_x_coordinate(el, "XPos"),
                      read_y_coordinate(el, "YPos"),
                      width, height,
                      min_width, min_height);
    HideWidget(id, el.BoolAttribute("Hidden", false));
    register_id(id, GetStringAttribute(el, "Name"));
}

void cInterfaceWindowXML::read_button_definition(const tinyxml2::XMLElement& el)
{
    int id;
    const char* base_img = el.Attribute("Image");
    std::string on, off, disabled;
    if (base_img) {
        std::string img_base = base_img;
        on       = img_base + "On.png";
        off      = img_base + "Off.png";
        disabled = img_base + "Disabled.png";
    }

    if(const char* on_ = el.Attribute("On"))           { on = on_; }
    if(const char* off_ = el.Attribute("Off"))         { off = off_; }
    if(const char* dis_ = el.Attribute("Disabled"))    { disabled = dis_; }



    id = AddButton(off, disabled, on,
                   read_x_coordinate(el, "XPos"),
                   read_y_coordinate(el, "YPos"),
                   read_width(el, "Width", boost::none),
                   read_height(el, "Height", boost::none)
                   );
    if(const char* pw = el.Attribute("PushWindow")) {
        SetButtonNavigation(id, pw, false);
    }
    if(const char* pw = el.Attribute("ReplaceWindow")) {
        SetButtonNavigation(id, pw, true);
    }
    register_id(id, GetStringAttribute(el, "Name"));
}

void cInterfaceWindowXML::read_slider_definition(const tinyxml2::XMLElement& el)
{
    int id;
    AddSlider(id,
              read_x_coordinate(el, "XPos"),
              read_y_coordinate(el, "YPos"),
              read_width(el, "Width", boost::none),
              el.IntAttribute("MinValue", 0),
              el.IntAttribute("MaxValue", 100),
              el.IntAttribute("Increment", 5),
              el.IntAttribute("Value", 0),
              el.BoolAttribute("LiveUpdate", true));
    DisableWidget(id, el.BoolAttribute("Disabled", false));
    HideWidget(id, el.BoolAttribute("Hidden", false));
    register_id(id, GetStringAttribute(el, "Name"));
}

void cInterfaceWindowXML::register_id(int id, std::string name)
{
    g_LogFile.debug("interface", "registering ID ", id, " to name '", name, "'");
    name_to_id[name] = id;
}

int cInterfaceWindowXML::get_id(std::string a, std::string b, std::string c)
{
    if (name_to_id.find(a) != name_to_id.end())    return name_to_id[a];
    if (name_to_id.find(b) != name_to_id.end())    return name_to_id[b];
    if (name_to_id.find(c) != name_to_id.end())    return name_to_id[c];

    bool un = false;
    if (a == "*Unused*" || a == "*Optional*")    { un=true; a=""; }
    if (b == "*Unused*" || b == "*Optional*")    { un=true; b=""; }
    if (c == "*Unused*" || c == "*Optional*")    { un=true; c=""; }

    if (!un) {
        g_LogFile.error("interface", "setting ids for interface ", (un ? " but it is not used or optional " : ""),
                        "- acceptable names for this item are : '", a, "', '", b, "', '", c, "'. Please check ", m_ScreenName); }
    return -1;
}

int cInterfaceWindowXML::get_id_optional(const std::string& name) const {
    auto found = name_to_id.find(name);
    if (found != name_to_id.end())    return found->second;

    return -1;
}