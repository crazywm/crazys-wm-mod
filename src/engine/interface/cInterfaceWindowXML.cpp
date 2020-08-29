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
    xml_source << "Resources" << "Interface" << wm->GetTheme() << m_ScreenName;
    auto doc = LoadXMLDocument(xml_source.str());
    widget_map_t widgets;
    /*
    *    loop over the elements attached to the root
    */
    for (auto& el : IterateChildElements(*doc->RootElement()))
    {
        std::string tag = el.Value();
        try {
            if (tag == "Define") { define_widget(el, widgets);    continue; }
            if (tag == "Widget") { place_widget(el, widgets);        continue; }
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

    g_LogFile.debug("interface", "calling set_ids for window ", m_ScreenName);
    set_ids();
}

void cInterfaceWindowXML::read_text_item(tinyxml2::XMLElement& el)
{
    int id;
    sXmlWidgetPart wdg;
    widget_text_item(el, wdg);

    /*
    *    create the text item
    */
    AddTextItem(id, wdg.x, wdg.y, wdg.w, wdg.h, wdg.text, wdg.fontsize, wdg.force_scroll, wdg.r, wdg.g, wdg.b);
    /*
    *    make a note of the ID
    */
    register_id(id, wdg.name);
}

void cInterfaceWindowXML::define_widget(tinyxml2::XMLElement& base_el, widget_map_t& widgets)
{
    std::string widget_name = GetStringAttribute(base_el, "Widget");
    g_LogFile.debug("interface", "define widget: '", widget_name, "'");

    /*
    *    create a new widget for that name
    */
    auto widget = std::make_unique<cXmlWidget>();
    /*
    *    this is like reading the file again, in minature
    */
    for (auto& el : IterateChildElements(base_el))
    {
        std::string tag = el.Value();

        sXmlWidgetPart current_widget;
        if (tag == "Text") { widget_text_item(el, current_widget); }
        if (tag == "Button") { widget_button_item(el, current_widget); }
        if (tag == "Image") { widget_image_item(el, current_widget); }
        if (tag == "EditBox") { widget_editbox_item(el, current_widget); }
        if (tag == "ListBox") { widget_listbox_item(el, current_widget); }
        if (tag == "Checkbox") { widget_checkbox_item(el, current_widget); }
        if (tag == "Widget") { widget_widget(el, current_widget); }
        if (tag == "Slider") { widget_slider_item(el, current_widget); }
        if(!current_widget.name.empty()) {
            current_widget.type = tag;
            widget->add(current_widget);
            continue;
        }

        g_LogFile.log(ELogLevel::WARNING, "Unhandled widget tag: '", tag, "'");
    }

    widgets[widget_name] = std::move(widget);
}

void cInterfaceWindowXML::place_widget(tinyxml2::XMLElement& el, const widget_map_t& widgets)
{
    /*
    *    we need the base co-ords for the widget and the
    *    sequence number to generate new names
    */
    std::string name = GetStringAttribute(el, "Definition");
    int x = GetIntAttribute(el, "XPos");
    int y = GetIntAttribute(el, "YPos");
    std::string seq = GetStringAttribute(el, "Seq");
    g_LogFile.debug("interface", "Placing Widget '", name, "'");
    add_widget(name, x, y, seq, widgets);
}

void cInterfaceWindowXML::add_widget(std::string widget_name, int x, int y, std::string seq, const widget_map_t& widgets)
{
    int id;
    auto widget_it = widgets.find(widget_name);
    if (widget_it == widgets.end())
    {
        g_LogFile.error("interface", "Can't find definition for widget '", widget_name, "'");
         return;
    }

    auto widget = widget_it->second.get();

    /*
    *    now loop over the widget components
    */
    for (int i = 0; i < widget->size(); i++) {
        sXmlWidgetPart &xw = (*widget)[i];
        std::string tag = xw.type;
        std::string name = xw.name + seq;
        /*
        *        the OO way to do this is to subclass
        *        sXmlWidgetPart and have each class have its own
        *        add() routine.
        *
        *        but then I need to store a vector of pointers
        *        because the vector class creates a new, empty instance
        *        and copies in the data, meaning that subclass information
        *        gets discarded for instance vectors.
        *
        *        and storing pointers means needing to delete them
        *        but without getting them deleted on each copy ...
        *
        *        all told it's less fuss to use a big IF, really
        */
        g_LogFile.debug("interface", "add_widget: x = ", x, ", xw.x = ", xw.x, ", y = ", y, ", xw.y = ", xw.y);

        int full_x = x + xw.x;
        int full_y = y + xw.y;

        if (tag == "Button") {
            auto bp = [](const std::string& source) -> std::string { return source.empty() ? "" : ButtonPath(source); };
            id = AddButton(bp(xw.off), bp(xw.disabled_img), bp(xw.on), full_x, full_y, xw.w, xw.h,
                           xw.alpha);
            register_id(id, name);
            HideWidget(id, xw.hide);
        }
        else if (tag == "Image") {
            DirPath dp = ImagePath(xw.file);
            AddImage(id, dp, full_x, full_y, xw.w, xw.h, xw.stat, xw.r, xw.g, xw.b);
            register_id(id, name);
            HideWidget(id, xw.hide);
        }
        else if (tag == "Slider") {
            AddSlider(id, full_x, full_y, xw.w, xw.r, xw.g, xw.h, xw.b, xw.events);
            register_id(id, name);
            HideWidget(id, xw.hide);
            DisableWidget(id, xw.stat);
        }
        else if (tag == "Text") {
            AddTextItem(id, full_x, full_y, xw.w, xw.h, xw.text, xw.fontsize, xw.force_scroll, xw.r, xw.g, xw.b);
            register_id(id, name);
            HideWidget(id, xw.hide);
        }
        else if (tag == "EditBox") {
            AddEditBox(id, full_x, full_y, xw.w, xw.h, xw.bordersize, xw.fontsize);
            register_id(id, name);
        }
        else if (tag == "Checkbox") {
            AddCheckbox(id, full_x, full_y, xw.w, xw.h, xw.text, xw.fontsize, xw.leftorright);
            register_id(id, name);
        }
        else if (tag == "Widget") {
            g_LogFile.debug("interface", "Placing nested widget at ", full_x, ", ", full_y);
            add_widget(xw.name, full_x, full_y, xw.seq + seq, widgets);
        }
        else {
            g_LogFile.log(ELogLevel::WARNING, "Unexpected tag in widget '", tag, "'.");
        }
    }
}


void cInterfaceWindowXML::read_window_definition(tinyxml2::XMLElement& el)
{
    CreateWindow(GetIntAttribute(el, "XPos"), GetIntAttribute(el, "YPos"),
            GetIntAttribute(el, "Width"), GetIntAttribute(el, "Height"), GetIntAttribute(el, "Border"));
}

void cInterfaceWindowXML::read_editbox_definition(tinyxml2::XMLElement& el)
{
    int id;
    sXmlWidgetPart wdg;
    widget_editbox_item(el, wdg);

    AddEditBox(id, wdg.x, wdg.y, wdg.w, wdg.h, wdg.bordersize, wdg.fontsize);
    register_id(id, wdg.name);
}

void cInterfaceWindowXML::read_listbox_definition(tinyxml2::XMLElement& el)
{
    // TODO widget_listbox seems incomplete, leave like this for now
    std::string name = GetStringAttribute(el, "Name");
    int x = GetIntAttribute(el, "XPos");
    int y = GetIntAttribute(el, "YPos");
    int w = GetIntAttribute(el, "Width");
    int h = GetIntAttribute(el, "Height");
    int fontsize = GetFallbackIntAttribute(el, "FontSize", 10);
    int rowheight = GetFallbackIntAttribute(el, "RowHeight", 20);

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
            int offset = sub_el.IntAttribute("Offset", 0);
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
    if (!column_name.empty())    box->DefineColumns(column_name, column_header, column_offset, column_skip);
}


void read_generic(tinyxml2::XMLElement& el, sXmlWidgetBase& data)
{
    /*
    *    get the button name - we'll use this to match up
    *    interface IDs
    */
    data.name = GetStringAttribute(el, "Name");
    data.x = GetIntAttribute(el, "XPos");
    data.y = GetIntAttribute(el, "YPos");
    data.w = GetIntAttribute(el, "Width");
    if(data.h == -1) {
        data.h = GetIntAttribute(el, "Height");
    } else {
        data.h = el.IntAttribute("Height", data.h);
    }
    data.hide = el.BoolAttribute("Hidden", false);
}

void cInterfaceWindowXML::widget_editbox_item(tinyxml2::XMLElement& el, sXmlWidgetPart& xw)
{
    read_generic(el, xw);
    xw.fontsize = el.IntAttribute("FontSize", 16);
    xw.bordersize = el.IntAttribute("Border", xw.bordersize);
    xw.multi = el.BoolAttribute("Multi", xw.multi);
    xw.events = el.BoolAttribute("Events", xw.events);
}

void cInterfaceWindowXML::widget_listbox_item(tinyxml2::XMLElement& el, sXmlWidgetPart& xw)
{
    read_generic(el, xw);
    xw.bordersize = el.IntAttribute("Border", xw.bordersize);
    xw.multi = el.BoolAttribute("Multi", xw.multi);
    xw.events = el.BoolAttribute("Events", xw.events);
}

void cInterfaceWindowXML::widget_checkbox_item(tinyxml2::XMLElement& el, sXmlWidgetPart& xw)
{
    read_generic(el, xw);
    xw.text = GetStringAttribute(el, "Text");
    xw.fontsize = GetIntAttribute(el, "FontSize");
    xw.leftorright = GetFallbackBoolAttribute(el, "LeftOrRight", true);
}

void cInterfaceWindowXML::widget_widget(tinyxml2::XMLElement& el, sXmlWidgetPart& xw)
{
    xw.name = GetStringAttribute(el, "Definition");
    xw.x = GetIntAttribute(el, "XPos");
    xw.y = GetIntAttribute(el, "YPos");
    xw.seq = GetStringAttribute(el, "Seq");
}

void cInterfaceWindowXML::read_checkbox_definition(tinyxml2::XMLElement& el)
{
    int id;

    sXmlWidgetPart wdg;
    widget_checkbox_item(el, wdg);

    AddCheckbox(id, wdg.x, wdg.y, wdg.w, wdg.h, wdg.text, wdg.fontsize, wdg.leftorright);
    register_id(id, wdg.name);
}

void cInterfaceWindowXML::read_image_definition(tinyxml2::XMLElement& el)
{
    int id;
    sXmlWidgetPart wdg;
    widget_image_item(el, wdg);
    DirPath dp = ImagePath(wdg.file);

    AddImage(id, dp, wdg.x, wdg.y, wdg.w, wdg.h, wdg.stat, wdg.r, wdg.g, wdg.b);
    HideWidget(id, wdg.hide);
    register_id(id, wdg.name);
}

void cInterfaceWindowXML::read_button_definition(tinyxml2::XMLElement& el)
{
    int id;
    sXmlWidgetPart wdg;
    widget_button_item(el, wdg);
    DirPath dp = ImagePath(wdg.file);

    auto bp = [](const std::string& source) -> std::string { return source.empty() ? "" : ButtonPath(source); };
    id = AddButton(bp(wdg.off), bp(wdg.disabled_img), bp(wdg.on), wdg.x, wdg.y, wdg.w, wdg.h, wdg.alpha);
    register_id(id, wdg.name);
}

void cInterfaceWindowXML::read_slider_definition(tinyxml2::XMLElement& el)
{
    int id;
    sXmlWidgetPart wdg;
    widget_slider_item(el, wdg);

    AddSlider(id, wdg.x, wdg.y, wdg.w, wdg.r, wdg.g, wdg.h, wdg.b, wdg.events);
    DisableWidget(id, wdg.stat);
    HideWidget(id, wdg.hide);
    register_id(id, wdg.name);
}

void cInterfaceWindowXML::widget_slider_item(tinyxml2::XMLElement& el, sXmlWidgetPart& xw)
{
    xw.stat = false;
    xw.events = true;
    xw.r = 0; xw.g = 100; xw.b = 0; xw.h = 5;

    read_generic(el, xw);
    el.QueryIntAttribute("Increment", &xw.h);
    el.QueryIntAttribute("MinValue", &xw.r);
    el.QueryIntAttribute("MaxValue", &xw.g);
    el.QueryIntAttribute("Value", &xw.b);
    el.QueryBoolAttribute("Disabled", &xw.stat);
    el.QueryBoolAttribute("LiveUpdate", &xw.events);

    // for some reason, optional ints above are unfortunately being set to 0 when not present; correct for that
    if (xw.r == 0 && xw.g == 0)    xw.g = 100;
    if (xw.b < xw.r)            xw.b = xw.r;
    if (xw.b > xw.g)            xw.b = xw.g;
    if (xw.h == 0)                xw.h = 5;
}

void cInterfaceWindowXML::widget_text_item(tinyxml2::XMLElement& el, sXmlWidgetPart& xw)
{
    read_generic(el, xw);

    xw.text = GetStringAttribute(el, "Text");
    xw.fontsize = GetIntAttribute(el, "FontSize");
    el.QueryBoolAttribute("ForceScrollbar", &xw.force_scroll);
    el.QueryIntAttribute("Red", &xw.r);
    el.QueryIntAttribute("Green", &xw.g);
    el.QueryIntAttribute("Blue", &xw.b);
}

void cInterfaceWindowXML::widget_button_item(tinyxml2::XMLElement& el, sXmlWidgetPart& xw)
{
    read_generic(el, xw);

    const char* base_img = el.Attribute("Image");
    if (base_img) {
        std::string img_base = base_img;
        xw.on           = img_base + "On.png";
        xw.off          = img_base + "Off.png";
        xw.disabled_img = img_base + "Disabled.png";
    }

    if(const char* on = el.Attribute("On"))             { xw.on = on; }
    if(const char* off = el.Attribute("Off"))         { xw.off = off; }
    if(const char* dis = el.Attribute("Disabled"))     { xw.disabled_img = dis; }
    xw.scale = GetBoolAttribute(el, "Scale");
    xw.alpha = GetBoolAttribute(el, "Transparency");
}

void cInterfaceWindowXML::widget_image_item(tinyxml2::XMLElement& el, sXmlWidgetPart& xw)
{
    xw.stat = false;
    xw.r = xw.g = xw.b = 0;

    read_generic(el, xw);
    xw.file = GetStringAttribute(el, "File");
}

void cInterfaceWindowXML::register_id(int id, std::string name)
{
    g_LogFile.debug("interface", "registering ID ", id, " to name '", name, "'");
    name_to_id[name] = id;
    id_to_name[id] = name;
}

int cInterfaceWindowXML::get_id(std::string a, std::string b, std::string c, std::string d)
{
    if (name_to_id.find(a) != name_to_id.end())    return name_to_id[a];
    if (name_to_id.find(b) != name_to_id.end())    return name_to_id[b];
    if (name_to_id.find(c) != name_to_id.end())    return name_to_id[c];
    if (name_to_id.find(d) != name_to_id.end())    return name_to_id[d];

    bool un = false;
    if (a == "*Unused*" || a == "*Optional*")    { un=true; a=""; }
    if (b == "*Unused*" || b == "*Optional*")    { un=true; b=""; }
    if (c == "*Unused*" || c == "*Optional*")    { un=true; c=""; }
    if (d == "*Unused*" || d == "*Optional*")    { un=true; d=""; }

    if (!un) {
        g_LogFile.error("interface", "setting ids for interface ", (un ? " but it is not used or optional " : ""),
                        "- acceptable names for this item are : '", a, "', '", b, "', '", c, "', '", d, "'. Please check ", m_ScreenName); }
    return -1;
}