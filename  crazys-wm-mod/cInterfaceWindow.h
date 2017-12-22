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

#include "cButton.h"
#include "GameSystem.h"
#include "CSurface.h"
#include "cInterfaceObject.h"
#include "cEditBox.h"
#include "cImageItem.h"
#include "cTextItem.h"
#include "cListBox.h"
#include "cScrollBar.h"
#include "cCheckBox.h"
#include "cSlider.h"
#include "cXmlWidget.h"
#include "DirPath.h"
#include "cGirls.h"
#include <vector>
#include <map>

class cImageItem;
class CSurface;
class cButton;
class cEditBox;
class cTextItem;
class cListBox;
class cCheckBox;
class cScrollBar;
class cSlider;

class TiXmlElement;

class cInterfaceWindow : public cInterfaceObject
{
public:
	cInterfaceWindow() {m_Background=m_Border=0;m_BackgroundSurface=0;}
	virtual ~cInterfaceWindow();

	virtual void Free();
	void CreateWindow(int x, int y, int width, int height, int BorderSize);	// and color options latter
	void UpdateWindow(int x, int y);
	void MouseDown(int x, int y);
	void Click(int x, int y, bool mouseWheelDown = false, bool mouseWheelUp = false);
	void Draw();

	void Reset();

	// MOD: added a couple of overloads for the cases where we
	// can deduce the image names from the stem&
	void AddButton(const char *image_name, int & ID,  int x, int y, int width, int height, bool transparency = false, bool scale = true,bool cached=false);
	void AddButton(string image, int & ID, int x, int y, int width, int height, bool transparency = false, bool scale = true,bool cached=false);
/*
 *	the ND version stands for "No Disabled" meaning that it supplies
 *	an empty string in place a disabled icon
 */
	void AddButtonND(string image, int & ID, int x, int y, int width, int height, bool transparency = false, bool scale = true,bool cached=false);
	void AddButtonND(const char *image, int & ID, int x, int y, int width, int height, bool transparency = false, bool scale = true,bool cached=false);
	// END MOD
	void AddButton(string OffImage, string DisabledImage, string OnImage, int & ID, int x, int y, int width, int height, bool transparency = false, bool scale = true,bool cached=false);
	void AddScrollBar(int & ID, int x, int y, int width, int height, int visibleitems);

	void HideButton(int id, bool hide);
	void HideButton(int id) { HideButton(id, true); }
	void UnhideButton(int id) { HideButton(id, false); }
	void DisableButton(int id, bool disable);
	void DisableButton(int id){DisableButton(id,true);}
	void EnableButton(int id){DisableButton(id,false);}
	
	void HideImage(int id, bool hide);
	void HideImage(int id) { HideImage(id, true); }
	void UnhideImage(int id) { HideImage(id, false); }
	void AddImage(int & id, string filename, int x, int y, int width, int height, bool statImage = false, int R = 0, int G = 0, int B = 0);
	void SetImage(int id, CSurface* image);
	void SetImage(int id, cAnimatedSurface* image);
	
	void PrepareImage(int girlimage_id, sGirl* selected_girl, int imagetype = IMGTYPE_PROFILE, bool rand = true, int ImageNum = -1, bool gallery = false, string ImageName = "");

	void AddEditBox(int & ID, int x, int y, int width, int height, int BorderSize, int FontSize = 16);
	void HideEditBox(int id, bool hide);

	void EditTextItem(string text, int ID);
	void HideText(int id, bool hide);
	void HideText(int id) { HideText(id, true); }
	void UnhideText(int id) { HideText(id, false); }
	void AddTextItem(int & ID, int x, int y, int width, int height, string text, int size = 16, bool auto_scrollbar = true, bool force_scrollbar = false, bool leftorright = false , int red = 0, int green = 0, int blue = 0);
	void AddTextItemScrollBar(int id);

	void AddSlider(int & ID, int x, int y, int width, int min = 0, int max = 100, int increment = 5, int value = 0, bool live_update = true);
	void DisableSlider(int ID, bool disable = true);
	void HideSlider(int ID, bool hide = true);
	void SliderLiveUpdate(int ID, bool live_update = true);
	int SliderRange(int ID, int min, int max, int value, int increment);  // set min and max values; returns slider value
	int SliderValue(int ID);  // get slider value
	int SliderValue(int ID, int value);  // set slider value, get result (might be different than requested due to out-of-bounds or whatever)
	void SliderMarker(int ID, int value); // set value for a visual "marker" to be shown at, to denote a default or target value or similar
	void SliderMarkerDisable(int ID); // stop displaying marker

	void DisableCheckBox(int ID, bool disable);
	void AddCheckbox(int & ID, int x, int y, int width, int height, string text, int size = 16, bool leftorright = false);
	bool IsCheckboxOn(int ID);
	void SetCheckBox(int ID, bool on);

	void AddListBox(int & ID, int x, int y, int width, int height, int BorderSize, bool enableEvents, bool MultiSelect = false, bool ShowHeaders = false, bool HeaderDiv = true, bool HeaderSort = true, int fontsize = 10, int rowheight = 20);
	void ScrollListBoxDown(int ID);
	int GetListBoxSize(int ID);
	void ScrollListBoxUp(int ID);
	void AddToListBox(int listBoxID, int dataID, string data, int color = COLOR_BLUE);
//	void RemoveFromListBox(int listBoxID, int dataID);
	int GetSelectedItemFromList(int listBoxID);
	string GetSelectedTextFromList(int listBoxID); // MYR: For new message summary display in InterfaceProcesses.cpp
	int GetLastSelectedItemFromList(int listBoxID);
	int GetNextSelectedItemFromList(int listBoxID, int from, int& pos);
	int GetAfterSelectedItemFromList(int listBoxID);
	void SetListTopPos(int listBoxID, int pos = 0);
	void SetSelectedItemInList(int listBoxID, int itemID, bool ev = true, bool DeselectOthers = true);
	void SetSelectedItemText(int listBoxID, int itemID, string data);
	void ClearListBox(int ID);
	void SetListBoxPosition(int ID, int pos = 0);
	int ArrowDownListBox(int ID);
	int ArrowUpListBox(int ID);
	bool IsMultiSelected(int ID);	// returns true if list has more than one item selected
	void AddToListBox(int listBoxID, int dataID, string data[], int columns, int color = COLOR_BLUE);
	void SetSelectedItemText(int listBoxID, int itemID, string data[], int columns);
	void SetSelectedItemColumnText(int listBoxID, int itemID, string data, int column);
	void SortColumns(int listBoxID, string column_name[], int columns);
	void DefineColumns(int listBoxID, string name[], string header[], int offset[], bool skip[], int columns);
	void SortListItems(int listBoxID, string column_name, bool Desc = false);
	string HeaderClicked(int listBoxID);
	bool ListDoubleClicked(int listBoxID);
	void SetSelectedItemTextColor(int listBoxID, int itemID, SDL_Color* text_color);
	void FillSortedIDList(int listBoxID, vector<int> *id_vec, int *vec_pos);

	void Focused();

	void SetBackgroundImage(string file);
	void UpdateEditBoxes(char key, bool upper = false);

	string GetEditBoxText(int ID);

	bool HasEditBox() {return !m_EditBoxes.empty();}

protected:
	vector<cButton*> m_Buttons;	// buttons
	vector<cImageItem*> m_Images; // Images
	vector<cListBox*> m_ListBoxes;	// List boxes
	vector<cScrollBar*> m_ScrollBars;	// Scroll bars
	// check boxes
	vector<cEditBox*> m_EditBoxes; // Edit boxes;
	vector<cTextItem*> m_TextItems; // Text Items
	vector<cCheckBox* >m_CheckBoxes;	// check boxes
	vector<cSlider*>m_Sliders;	// Sliders

	// the windows properties
	CSurface* m_BackgroundSurface;
	SDL_Surface* m_Background;
	SDL_Surface* m_Border;
	int m_BorderSize;

	float m_xRatio, m_yRatio;	// for storing the scaling ratio of the interface
};

class cInterfaceWindowXML : public cInterfaceWindow
{
protected:
	string	m_filename;
	map<string,int>		name_to_id;
	map<int,string>		id_to_name;

	map<string,cXmlWidget*>	widgets;
public:
	enum AttributeNecessity {
		Mandatory	= 0,
		Optional 	= 1
	};

	cInterfaceWindowXML();
	virtual ~cInterfaceWindowXML();

	void load();
/* 
 *	this has static linkage so we can pass it
 *	to the window manager's Push method
 */
static	void handler_func(cInterfaceWindowXML *wpt);
/*
 * 	the handler func just calls the virtual process
 *	method, which can process calls as it likes
 */
virtual	void process()=0;
/*
 *	populates the maps so we can get the IDs from strings
 */
	void register_id(int id, string name);
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
	void place_widget(TiXmlElement *el, string suffix="");
	void widget_text_item(TiXmlElement *el, cXmlWidget &vw);
	void widget_button_item(TiXmlElement *el, cXmlWidget &vw);
	void widget_editbox_item(TiXmlElement *el, cXmlWidget &vw);
	void widget_listbox_item(TiXmlElement *el, cXmlWidget &vw);
	void widget_checkbox_item(TiXmlElement *el, cXmlWidget &vw);
	void widget_widget(TiXmlElement *el, cXmlWidget &vw);
	void widget_image_item(TiXmlElement *el, cXmlWidget &vw);
	void widget_slider_item(TiXmlElement *el, cXmlWidget &vw);
	int get_id(string a, string b = "", string c = "", string d = "", string e = "");

	cXmlWidget* new_widget(string name);
	cXmlWidget* find_widget(string name);
	void add_widget(string widget_name,int x,int y,string seq);
};

class cSelector
{
	int id;			// list ID
	int pos;		// position variable
	cInterfaceWindow &win;	// window reference
public:
	cSelector(cInterfaceWindow &a_win, int a_id)
	: win(a_win)
	{
		id = a_id;
	}

	cSelector& operator=(const cSelector& other)
	{
		if (this != &other)
		{
			id = other.id;
			pos = other.pos;
			win = other.win;
		}
		return *this;
	}

	int first() {
		pos = 0;
		return win.GetNextSelectedItemFromList(id, 0, pos);
	}

	int next() {
		return win.GetNextSelectedItemFromList(id, pos+1, pos);
	}
};

#endif
