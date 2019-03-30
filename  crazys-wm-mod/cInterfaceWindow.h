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
#ifndef __CINTERFACEWINDOW_H
#define __CINTERFACEWINDOW_H

#include "cInterfaceObject.h"
#include "cXmlWidget.h"
#include "Constants.h"
#include <vector>
#include <functional>
#include <map>
#include <SDL_keyboard.h>
#include <SDL_video.h>

class cImageItem;
class CSurface;
class cButton;
class cEditBox;
class cTextItem;
class cListBox;
class cCheckBox;
class cScrollBar;
class cSlider;
class cWindowManager;
class sGirl;
class IBuilding;

class TiXmlElement;

class cInterfaceWindow : public cInterfaceObject
{
public:
	cInterfaceWindow() {
	    m_Background = m_Border = nullptr;
	    m_BackgroundSurface = nullptr;
	}
	virtual ~cInterfaceWindow();

	virtual void Free();
	void CreateWindow(int x, int y, int width, int height, int BorderSize);	// and color options latter
	void UpdateWindow(int x, int y);
	void MouseDown(int x, int y);
	void Click(int x, int y, bool mouseWheelDown = false, bool mouseWheelUp = false);
	void Draw(const CGraphics& gfx) override;

	void Reset();

	// MOD: added a couple of overloads for the cases where we
	// can deduce the image names from the stem&
	void AddButton(const char *image_name, int & ID,  int x, int y, int width, int height, bool transparency = false, bool scale = true,bool cached=false);
	void AddButton(std::string image, int & ID, int x, int y, int width, int height, bool transparency = false, bool scale = true,bool cached=false);
	// END MOD
	void AddButton(std::string OffImage, std::string DisabledImage, const std::string& OnImage, int & ID, int x, int y, int width, int height, bool transparency = false, bool scale = true,bool cached=false);
	void AddScrollBar(int & ID, int x, int y, int width, int height, int visibleitems);

	void HideButton(int id, bool hide);
	void DisableButton(int id, bool disable);
	void DisableButton(int id){DisableButton(id,true);}
	void EnableButton(int id){DisableButton(id,false);}
	void SetButtonCallback(int id, std::function<void()>);
	void SetButtonNavigation(int id, std::string target, bool replace = true);
	void SetButtonHotKey(int id, SDLKey key);

	void HideImage(int id, bool hide);
    void AddImage(int & id, std::string filename, int x, int y, int width, int height, bool statImage = false, int R = 0, int G = 0, int B = 0);
	void SetImage(int id, CSurface* image);

    void PrepareImage(int girlimage_id, sGirl* selected_girl, int imagetype = IMGTYPE_PROFILE, bool rand = true, int ImageNum = -1, bool gallery = false, std::string ImageName = "");

	void AddEditBox(int & ID, int x, int y, int width, int height, int BorderSize, int FontSize = 16);
	void HideEditBox(int id, bool hide);

	void EditTextItem(std::string text, int ID);
	void HideText(int id, bool hide);
	void AddTextItem(int & ID, int x, int y, int width, int height, std::string text, int size = 16, bool auto_scrollbar = true, bool force_scrollbar = false, bool leftorright = false , int red = 0, int green = 0, int blue = 0);
	void AddTextItemScrollBar(int id);

	void AddSlider(int & ID, int x, int y, int width, int min = 0, int max = 100, int increment = 5, int value = 0, bool live_update = true);
	void DisableSlider(int ID, bool disable = true);
	void HideSlider(int ID, bool hide = true);
	int SliderRange(int ID, int min, int max, int value, int increment);  // set min and max values; returns slider value
	int SliderValue(int ID);  // get slider value
	int SliderValue(int ID, int value);  // set slider value, get result (might be different than requested due to out-of-bounds or whatever)
	void SliderMarker(int ID, int value); // set value for a visual "marker" to be shown at, to denote a default or target value or similar
    void SetSliderCallback(int id, std::function<void(int)>);

	void DisableCheckBox(int ID, bool disable);
	void AddCheckbox(int & ID, int x, int y, int width, int height, std::string text, int size = 16, bool leftorright = false);
	bool IsCheckboxOn(int ID);
	void SetCheckBox(int ID, bool on);
    void SetCheckBoxCallback(int id, std::function<void(bool)>);

	void AddListBox(int & ID, int x, int y, int width, int height, int BorderSize, bool enableEvents, bool MultiSelect = false, bool ShowHeaders = false, bool HeaderDiv = true, bool HeaderSort = true, int fontsize = 10, int rowheight = 20);
	int GetListBoxSize(int ID);

    void AddToListBox(int listBoxID, int dataID, std::string data, int color = COLOR_BLUE);
    int GetSelectedItemFromList(int listBoxID);
	std::string GetSelectedTextFromList(int listBoxID); // MYR: For new message summary display in InterfaceProcesses.cpp
	int GetLastSelectedItemFromList(int listBoxID);
	int GetNextSelectedItemFromList(int listBoxID, int from, int& pos);
	int GetAfterSelectedItemFromList(int listBoxID);
	void SetListTopPos(int listBoxID, int pos = 0);
	void SetSelectedItemInList(int listBoxID, int itemID, bool ev = true, bool DeselectOthers = true);
	void SetSelectedItemText(int listBoxID, int itemID, std::string data);
	void ClearListBox(int ID);
	int ArrowDownListBox(int ID);
	int ArrowUpListBox(int ID);
	bool IsMultiSelected(int ID);	// returns true if list has more than one item selected
	void AddToListBox(int listBoxID, int dataID, std::string data[], int columns, int color = COLOR_BLUE);
	void SetSelectedItemText(int listBoxID, int itemID, std::string data[], int columns);
	cListBox* GetListBox(int listBoxID);
	void SetListBoxSelectionCallback(int id, std::function<void(int)>);
	void SetListBoxDoubleClickCallback(int id, std::function<void(int)>);
	void SetListBoxHotKeys(int id, SDLKey up, SDLKey down);

    void SetSelectedItemColumnText(int listBoxID, int itemID, std::string data, const std::string& column);
	void SortColumns(int listBoxID, std::string column_name[], int columns);
	void DefineColumns(int listBoxID, std::string name[], std::string header[], int offset[], bool skip[], int columns);
	void SortListItems(int listBoxID, std::string column_name, bool Desc = false);
	std::string HeaderClicked(int listBoxID);
	bool ListDoubleClicked(int listBoxID);
	void SetSelectedItemTextColor(int listBoxID, int itemID, SDL_Color* text_color);
	void FillSortedIDList(int listBoxID, std::vector<int> *id_vec, int *vec_pos);

	void Focused();

    void UpdateEditBoxes(char key, bool upper = false);

	std::string GetEditBoxText(int ID);

	bool HasEditBox() {return !m_EditBoxes.empty();}

	void update();
    virtual void OnKeyPress(SDL_keysym keysym);
    virtual void process() = 0;
    virtual void init(bool back) {};

    void push_message(std::string text, int color);

protected:
	std::vector<cButton*> m_Buttons;	// buttons

	std::vector<cImageItem*> m_Images; // Images
	std::vector<cListBox*> m_ListBoxes;	// List boxes
	std::vector<cScrollBar*> m_ScrollBars;	// Scroll bars
	// check boxes
	std::vector<cEditBox*> m_EditBoxes; // Edit boxes;
	std::vector<cTextItem*> m_TextItems; // Text Items
	std::vector<cCheckBox* >m_CheckBoxes;	// check boxes
	std::vector<cSlider*>m_Sliders;	// Sliders

	// the windows properties
	CSurface* m_BackgroundSurface;
	SDL_Surface* m_Background;
	SDL_Surface* m_Border;
	int m_BorderSize;

	float m_xRatio, m_yRatio;	// for storing the scaling ratio of the interface


    // For interacting with the window manager
    cWindowManager& window_manager() const;
    void push_window(const std::string& name) const;
    void replace_window(const std::string& name) const;
    void pop_window() const;
    void pop_to_window(const std::string& target) const;

    // input handling
    void input_integer(std::function<void(int)> callback);
    void input_confirm(std::function<void()> callback);
    void input_string(std::function<void(const std::string&)> callback);

    // active building
    IBuilding& active_building() const;
    void set_active_building(IBuilding* building);
    IBuilding& cycle_building(int direction);

    // active girls
    sGirl& active_girl() const;
    sGirl* selected_girl() const;
    void set_active_girl(sGirl* girl);
};

class cInterfaceWindowXML : public cInterfaceWindow
{
protected:
	std::string	m_filename;
	std::map<std::string,int>		name_to_id;
	std::map<int,std::string>		id_to_name;

	std::map<std::string,cXmlWidget*>	widgets;

public:
	enum AttributeNecessity {
		Mandatory	= 0,
		Optional 	= 1
	};

	explicit cInterfaceWindowXML(const char* base_file);
	~cInterfaceWindowXML() override;

	void load();


    /*
     *	populates the maps so we can get the IDs from strings
     */
	void register_id(int id, std::string name);
    /*
     *	XML reading stuff
     */
	void read_text_item(TiXmlElement *el);
	void read_window_definition(TiXmlElement *el);
	void read_editbox_definition(TiXmlElement *el);
	void read_button_definition(TiXmlElement *el);
	void read_image_definition(TiXmlElement *el);
	void read_listbox_definition(TiXmlElement *el);
	void read_checkbox_definition(TiXmlElement *el);
	void read_slider_definition(TiXmlElement *el);
	void define_widget(TiXmlElement *el);
	void place_widget(TiXmlElement *el, std::string suffix="");
	void widget_text_item(TiXmlElement *el, cXmlWidget &vw);
	void widget_button_item(TiXmlElement *el, cXmlWidget &vw);
	void widget_editbox_item(TiXmlElement *el, cXmlWidget &vw);
	void widget_listbox_item(TiXmlElement *el, cXmlWidget &vw);
	void widget_checkbox_item(TiXmlElement *el, cXmlWidget &vw);
	void widget_widget(TiXmlElement *el, cXmlWidget &vw);
	void widget_image_item(TiXmlElement *el, cXmlWidget &vw);
	void widget_slider_item(TiXmlElement *el, cXmlWidget &vw);

	int get_id(std::string a, std::string b = "", std::string c = "", std::string d = "", std::string e = "");

	cXmlWidget* new_widget(std::string name);
	cXmlWidget* find_widget(std::string name);
	void add_widget(std::string widget_name,int x,int y,std::string seq);

private:
    virtual void set_ids() = 0;
};


#endif
