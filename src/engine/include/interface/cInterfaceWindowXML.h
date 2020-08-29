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
#ifndef __CINTERFACEWINDOWXML_H
#define __CINTERFACEWINDOWXML_H

#include "interface/cInterfaceWindow.h"

struct sXmlWidgetBase {
    std::string name;
    int x = -1;
    int y = -1;
    int w = -1;
    int h = -1;
    bool hide = false;
};

struct sXmlWidgetPart : public sXmlWidgetBase {
public:
    int         r = 0, g = 0, b = 0;
    int         fontsize=0, bordersize=0;
    bool        stat, alpha, scale, events, multi, leftorright;
    std::string on, off, disabled_img, type, text, file, seq;
    bool        force_scroll = false;
};

class cXmlWidget {
    std::vector<sXmlWidgetPart> list;
public:
    cXmlWidget() = default;
    int size() {    return int(list.size()); }
    sXmlWidgetPart& operator[](int i) {
        return list[i];
    }
    void add(sXmlWidgetPart &part)
    {
        list.push_back(part);
    }
};


class cInterfaceWindowXML : public cInterfaceWindow
{
protected:
    std::string m_ScreenName;
    std::map<std::string,int>        name_to_id;
    std::map<int,std::string>        id_to_name;

public:
    enum AttributeNecessity {
        Mandatory    = 0,
        Optional     = 1
    };

    explicit cInterfaceWindowXML(const char* base_file);
    ~cInterfaceWindowXML() override;

    void load(cWindowManager* root) final;

    /*
     *    populates the maps so we can get the IDs from strings
     */
    void register_id(int id, std::string name);
    /*
     *    XML reading stuff
     */
    using widget_map_t = std::map<std::string, std::unique_ptr<cXmlWidget>>;
    void read_text_item(tinyxml2::XMLElement& el);
    void read_window_definition(tinyxml2::XMLElement& el);
    void read_editbox_definition(tinyxml2::XMLElement& el);
    void read_button_definition(tinyxml2::XMLElement& el);
    void read_image_definition(tinyxml2::XMLElement& el);
    void read_listbox_definition(tinyxml2::XMLElement& el);
    void read_checkbox_definition(tinyxml2::XMLElement& el);
    void read_slider_definition(tinyxml2::XMLElement& el);

    void define_widget(tinyxml2::XMLElement& base_el, widget_map_t& widgets);
    void place_widget(tinyxml2::XMLElement& el, const widget_map_t& widgets);
    void widget_text_item(tinyxml2::XMLElement& el, sXmlWidgetPart& xw);
    void widget_button_item(tinyxml2::XMLElement& el, sXmlWidgetPart& xw);
    void widget_editbox_item(tinyxml2::XMLElement& el, sXmlWidgetPart& xw);
    void widget_listbox_item(tinyxml2::XMLElement& el, sXmlWidgetPart& xw);
    void widget_checkbox_item(tinyxml2::XMLElement& el, sXmlWidgetPart& xw);
    void widget_widget(tinyxml2::XMLElement& el, sXmlWidgetPart& xw);
    void widget_image_item(tinyxml2::XMLElement& el, sXmlWidgetPart& xw);
    void widget_slider_item(tinyxml2::XMLElement& el, sXmlWidgetPart& xw);

    int get_id(std::string a, std::string b = "", std::string c = "", std::string d = "");
    void add_widget(std::string widget_name,int x,int y,std::string seq, const widget_map_t& widgets);

private:
    virtual void set_ids() = 0;
};

#endif
