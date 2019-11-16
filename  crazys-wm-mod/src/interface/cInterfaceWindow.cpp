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

#include "cInterfaceWindow.h"
#include "cWindowManager.h"
#include "DirPath.h"
#include "CLog.h"
#include "tinyxml.h"
#include "XmlUtil.h"

#include "src/widgets/cListBox.h"
#include "src/widgets/cButton.h"
#include "src/widgets/cCheckBox.h"
#include "src/widgets/cSlider.h"
#include "src/widgets/cEditBox.h"
#include "src/widgets/cTextItem.h"
#include "src/widgets/cImageItem.h"
#include "src/widgets/cScrollBar.h"
#include "src/Game.hpp"
#include "src/buildings/cBrothel.h"
#include "interface/CGraphics.h"
#include "cColor.h"
#include "cChoiceMessage.h"

using namespace std;

typedef unsigned int u_int;

extern CGraphics g_Graphics;

extern sColor g_WindowBorderColor;
extern sColor g_WindowBackgroundColor;

cInterfaceWindow::~cInterfaceWindow()
{
}

void cInterfaceWindow::load(cWindowManager* root) {
    assert(!m_WindowManager);
    m_WindowManager = root;
}

void cInterfaceWindow::UpdateWindow(int x, int y)
{
    for (auto & widget : m_Widgets) widget->OnMouseMove(x, y);
}

void cInterfaceWindow::MouseClick(int x, int y, bool down)
{
    // this method added to handle draggable objects
    try {
        for(auto& widget : m_Widgets)
            if(widget->OnMouseClick(x, y, down)) return;
            // nobody wanted to handle this click -> remove focus
            SetFocusTo(nullptr);
    } catch (std::exception& exception) {
        push_message(exception.what(), 1);
    }
}

void cInterfaceWindow::MouseWheel(int x, int y, bool down)
{
    try {
        for(auto& widget : m_Widgets)
            if(widget->OnMouseWheel(down, x, y, false)) return;

        // if nothing handles the mouse wheel, try the keyboard focused widget
        if(m_KeyboardFocusWidget && m_KeyboardFocusWidget->OnMouseWheel(down, x, y, true)) return;

    } catch (std::exception& exception) {
        push_message(exception.what(), 1);
    }
}

void cInterfaceWindow::Reset()
{
	for (auto& widget : m_Widgets) {
		widget->Reset();
	}
}

void cInterfaceWindow::Draw(const CGraphics& gfx)
{
	if (m_Background && m_Border)
	{
		// Draw the window
		SDL_Rect offset;
		offset.x = m_XPos;
		offset.y = m_YPos;
		// blit to the screen
		m_Border.DrawSurface(m_XPos, m_YPos);
		offset.x = m_XPos + m_BorderSize;
        offset.y = m_YPos + m_BorderSize;
        m_Background.DrawSurface(m_XPos + m_BorderSize, m_YPos + m_BorderSize);
	}

	for (auto& widget : m_Widgets) widget->Draw(gfx);
}

void cInterfaceWindow::AddButton(std::string image, int& ID, int x, int y, int width, int height, bool transparency,
                                 bool scale)
{
    std::string dp = ButtonPath(image);
    string on = dp + "On.png";
    string off = dp + "Off.png";
    string disabled;
    disabled = dp + "Disabled.png";
    ID = AddButton(off, disabled, on, x, y, width, height, transparency, scale);
}

int
cInterfaceWindow::AddButton(std::string OffImage, std::string DisabledImage, const std::string& OnImage, int x, int y,
                            int width,
                            int height, bool transparency, bool scale)
{
	if (scale)
	{
		width = (int)((float)width*m_xRatio);
		height = (int)((float)height*m_yRatio);
		x = (int)((float)x*m_xRatio);
		y = (int)((float)y*m_yRatio);
	}
	int ID = m_Widgets.size();
	// create button
	auto newButton = std::make_unique<cButton>(this, OffImage, DisabledImage, OnImage, ID, x + m_XPos, y + m_YPos,
	        width, height,transparency);

	// Store button
	m_Widgets.push_back(std::move(newButton));
	return ID;
}

cScrollBar* cInterfaceWindow::AddScrollBar(int x, int y, int width, int height, int visibleitems)
{
	int ID = m_Widgets.size();
	auto newScrollBar = std::make_unique<cScrollBar>(this, ID, x + m_XPos, y + m_YPos, width, height, visibleitems);
	newScrollBar->m_ScrollAmount = cfg.resolution.list_scroll();
	m_Widgets.push_back(std::move(newScrollBar));
	return (cScrollBar*)m_Widgets.back().get();
}

cButton* cInterfaceWindow::GetButton(int id)
{
    if (id == -1) return nullptr;
    return dynamic_cast<cButton*>(m_Widgets[id].get());
}

bool cInterfaceWindow::HasFocus(int id)
{
    if (id == -1) return false;
    return m_Widgets.at(id)->HasFocus();
}

void cInterfaceWindow::HideWidget(int id, bool hide)
{
	if (id == -1) return;
	m_Widgets.at(id)->SetHidden(hide);
}

void cInterfaceWindow::AddImage(int & id, string filename, int x, int y, int width, int height, bool statImage, int R, int G, int B)
{
	width = (int)((float)width*m_xRatio);
	height = (int)((float)height*m_yRatio);
	x = (int)((float)x*m_xRatio);
	y = (int)((float)y*m_yRatio);

	// create image
	id = m_Widgets.size();
	auto newImage = std::make_unique<cImageItem>(this, id, x + m_XPos, y + m_YPos, width, height);
    newImage->CreateImage(filename, statImage, R, G, B);

	// Store button
	m_Widgets.push_back(std::move(newImage));
}

cImageItem* cInterfaceWindow::GetImage(int id)
{
    if (id == -1) return nullptr;
    return dynamic_cast<cImageItem*>(m_Widgets[id].get());
}


void cInterfaceWindow::SetImage(int id, string image)
{
    if(id < 0) return;
    cImageItem * item = GetImage(id);
    item->SetImage(GetGraphics().LoadImage(ImagePath(image), item->GetWidth(), item->GetHeight(), true));
}

void cInterfaceWindow::AddEditBox(int & ID, int x, int y, int width, int height, int BorderSize, int FontSize)
{
	width = (int)((float)width*m_xRatio);
	height = (int)((float)height*m_yRatio);
	x = (int)((float)x*m_xRatio);
	y = (int)((float)y*m_yRatio);

	// create button
	ID = m_Widgets.size();
	auto newEditBox = std::make_unique<cEditBox>(this, ID, x + m_XPos, y + m_YPos, width, height, BorderSize, FontSize);
	// Store button
	m_Widgets.push_back(std::move(newEditBox));
}

void cInterfaceWindow::DisableWidget(int id, bool disable)
{
	if (id>-1)
		m_Widgets.at(id)->SetDisabled(disable);
}

void cInterfaceWindow::SetButtonCallback(int id, std::function<void()> cb) {
    if (id>-1)
        GetButton(id)->SetCallback(std::move(cb));
}

void cInterfaceWindow::SetButtonNavigation(int id, std::string target, bool replace)
{
    if(target == "<back>") {
        SetButtonCallback(id, [this]() { pop_window(); });
    } else if(replace) {
        SetButtonCallback(id, [this, t = std::move(target)]() {
            replace_window(t);
        });
    } else {
        SetButtonCallback(id, [this, t = std::move(target)]() {
            push_window(t);
        });
    }
}

void cInterfaceWindow::CreateWindow(int x, int y, int width, int height, int BorderSize)
{
	m_xRatio = 1.0f;	m_yRatio = 1.0f;
	// `J` fixed this to allow for nonscaled 800x600 screen sizes
	if (g_Graphics.GetWidth() != cfg.resolution.scalewidth())	m_xRatio = (float)g_Graphics.GetWidth() / (float)cfg.resolution.scalewidth();
	if (g_Graphics.GetHeight() != cfg.resolution.scaleheight())	m_yRatio = (float)g_Graphics.GetHeight() / (float)cfg.resolution.scaleheight();

	width = (int)((float)width*m_xRatio);
	height = (int)((float)height*m_yRatio);
	x = (int)((float)x*m_xRatio);
	y = (int)((float)y*m_yRatio);

	m_BorderSize = BorderSize;
	SetPosition(x, y, width, height);
	m_Border = g_Graphics.CreateSurface(width, height, g_WindowBorderColor);
    m_Background = g_Graphics.CreateSurface(width - (BorderSize * 2), height - (BorderSize * 2), g_WindowBackgroundColor);
}

string cInterfaceWindow::GetEditBoxText(int ID)
{
	return GetEditBox(ID)->GetText();
}

void cInterfaceWindow::SetEditBoxText(int ID, std::string text)
{
    GetEditBox(ID)->m_Text->SetText(std::move(text));
}

cEditBox* cInterfaceWindow::GetEditBox(int id)
{
    if (id == -1) return nullptr;
    return dynamic_cast<cEditBox*>(m_Widgets[id].get());
}

void cInterfaceWindow::Focused()
{
	// clear edit boxes and set the first one as focused
	// for (auto & m_EditBoxe : m_EditBoxes)		m_EditBoxe->ClearText();
	TabFocus();
}

void cInterfaceWindow::SetFocusTo(cUIWidget* widget)
{
    if(widget != m_KeyboardFocusWidget) {
        if(m_KeyboardFocusWidget)
            m_KeyboardFocusWidget->SetFocus(false);
        if(widget)
            widget->SetFocus(true);
        m_KeyboardFocusWidget = widget;
    }
}

void cInterfaceWindow::SetCheckBox(int ID, bool on)
{
    if(ID == -1) return;
	GetCheckBox(ID)->SetState(on);
}

bool cInterfaceWindow::IsCheckboxOn(int ID)
{
	return GetCheckBox(ID)->GetState();
}

void cInterfaceWindow::AddSlider(int & ID, int x, int y, int width, int min, int max, int increment, int value, bool live_update)
{
	width = (int)((float)width*m_xRatio);
	float height = m_yRatio;
	x = (int)((float)x*m_xRatio);
	y = (int)((float)y*m_yRatio);

	ID = m_Widgets.size();
	auto newSlider = std::make_unique<cSlider>(this, ID, x + m_XPos, y + m_YPos, width, min, max, increment, value, height);
	newSlider->LiveUpdate(live_update);
	m_Widgets.push_back(std::move(newSlider));
}

int cInterfaceWindow::SliderRange(int ID, int min, int max, int value, int increment)
{
	if (ID == -1) return 0;
	return GetSlider(ID)->SetRange(min, max, value, increment);
}

int cInterfaceWindow::SliderValue(int ID)
{
	if (ID == -1) return 0;
	return GetSlider(ID)->Value();
}

int cInterfaceWindow::SliderValue(int ID, int value)
{
	if (ID == -1) return 0;
	return GetSlider(ID)->Value(value);
}

void cInterfaceWindow::SliderMarker(int ID, int value)
{
	if (ID == -1) return;
    GetSlider(ID)->SetMarker(value);
}

cSlider* cInterfaceWindow::GetSlider(int id)
{
    if (id == -1) return nullptr;
    return dynamic_cast<cSlider*>(m_Widgets[id].get());
}


void cInterfaceWindow::AddCheckbox(int & ID, int x, int y, int width, int height, string text, int size, bool leftorright)
{
	width = (int)((float)width*m_xRatio);
	height = (int)((float)height*m_yRatio);
	x = (int)((float)x*m_xRatio);
	y = (int)((float)y*m_yRatio);

	// create checkbox item
	ID = m_Widgets.size();
	auto newCheckBox = std::make_unique<cCheckBox>(this, ID, x + m_XPos, y + m_YPos, width, height, text, size, leftorright);

	// Store text item
	m_Widgets.push_back(std::move(newCheckBox));
}

cCheckBox* cInterfaceWindow::GetCheckBox(int id) {
    if (id == -1) return nullptr;
    return dynamic_cast<cCheckBox*>(m_Widgets[id].get());
}

void cInterfaceWindow::AddTextItem(int & ID, int x, int y, int width, int height, string text, int size,
	bool force_scrollbar, int red, int green, int blue)
{
	width = (int)((float)width*m_xRatio);
	height = (int)((float)height*m_yRatio);
	x = (int)((float)x*m_xRatio);
	y = (int)((float)y*m_yRatio);

	// create text item
	ID = m_Widgets.size();
	auto newTextItem = std::make_unique<cTextItem>(this, ID, x + m_XPos, y + m_YPos, width, height, text, size,
	        force_scrollbar, red, green, blue);
	// Store text item
	m_Widgets.push_back(std::move(newTextItem));
}

void cInterfaceWindow::EditTextItem(string text, int ID)
{
	if (ID == -1) return;

    cTextItem* item = GetTextItem(ID);
	item->SetText(text);
}

cTextItem* cInterfaceWindow::GetTextItem(int id)
{
    if (id == -1) return nullptr;
    return dynamic_cast<cTextItem*>(m_Widgets[id].get());
}


cListBox* cInterfaceWindow::AddListBox(int x, int y, int width, int height, int BorderSize, bool enableEvents, bool MultiSelect, bool ShowHeaders, bool HeaderDiv, bool HeaderSort, int fontsize, int rowheight)
{
	width = (int)((float)width*m_xRatio);
	height = (int)((float)height*m_yRatio);
	x = (int)((float)x*m_xRatio);
	y = (int)((float)y*m_yRatio);

	g_LogFile.write("Adding listbox...");
	// create listbox item
	int ID = m_Widgets.size();
	g_LogFile.write("initializing listbox");
	auto newListBox = std::make_unique<cListBox>(this, ID, x + m_XPos, y + m_YPos, width, height, BorderSize, MultiSelect,
	        ShowHeaders, HeaderDiv, HeaderSort, fontsize, rowheight);
	g_LogFile.write("enabling events");
    cListBox* lb = newListBox.get();
	lb->m_EnableEvents = enableEvents;

	// Store listbox item
	DirPath up, down;
	up = ButtonPath("Up");
	down = ButtonPath("Down");
	string ups = up.c_str();
	string downs = down.c_str();

	// if showing headers and allowing header clicks to sort list, offset scrollbar and scroll up button
	int header_offset = (ShowHeaders && HeaderSort) ? 21 : 0;
    m_Widgets.push_back(std::move(newListBox));

	g_LogFile.write("adding scrollbar");
	auto bar = AddScrollBar( x + width - 16, y + header_offset + 1, 16, height - header_offset - 2, lb->m_NumDrawnElements);
    bar->ParentPosition = &lb->m_ScrollChange;
    lb->m_ScrollBar = bar;
	g_LogFile.write("pushing listbox onto stack");
	return lb;
}

int cInterfaceWindow::GetListBoxSize(int ID)
{
	return GetListBox(ID)->GetSize();
}

void cInterfaceWindow::SetSelectedItemText(int listBoxID, int itemID, string data)
{
	GetListBox(listBoxID)->SetElementText(itemID, data);
}

void cInterfaceWindow::AddToListBox(int listBoxID, int dataID, string data, int color)
{
	AddToListBox(listBoxID, dataID, std::vector<std::string>{std::move(data)}, color);
}

void cInterfaceWindow::SetSelectedItemText(int listBoxID, int itemID, string data[], int columns)
{
	GetListBox(listBoxID)->SetElementText(itemID, data, columns);
}

void cInterfaceWindow::SetSelectedItemColumnText(int listBoxID, int itemID, string data, const std::string& column)
{
    GetListBox(listBoxID)->SetElementColumnText(itemID, std::move(data), column);
}

void cInterfaceWindow::SetSelectedItemTextColor(int listBoxID, int itemID, const SDL_Color& text_color)
{
	GetListBox(listBoxID)->SetElementTextColor(itemID, text_color);
}

void cInterfaceWindow::FillSortedIDList(int listBoxID, vector<int>& id_vec, int& vec_pos)
{
	GetListBox(listBoxID)->GetSortedIDList(&id_vec, &vec_pos);
}

void cInterfaceWindow::AddToListBox(int listBoxID, int dataID, std::vector<std::string> data, int color)
{
    GetListBox(listBoxID)->AddElement(dataID, std::move(data), color);
}

void cInterfaceWindow::SortColumns(int listBoxID, const std::vector<std::string>& column_name)
{
	GetListBox(listBoxID)->SetColumnSort(column_name);
}

void cInterfaceWindow::SortListItems(int listBoxID, string column_name, bool Desc)
{
	GetListBox(listBoxID)->SortByColumn(column_name, Desc);
}

string cInterfaceWindow::HeaderClicked(int listBoxID)
{
	string clicked = GetListBox(listBoxID)->m_HeaderClicked;
	GetListBox(listBoxID)->m_HeaderClicked = "";
	return clicked;
}

int cInterfaceWindow::GetNextSelectedItemFromList(int listBoxID, int from, int& pos)
{
	return GetListBox(listBoxID)->GetNextSelected(from, pos);
}

int cInterfaceWindow::GetLastSelectedItemFromList(int listBoxID)
{
	return GetListBox(listBoxID)->GetLastSelected();
}

int cInterfaceWindow::GetSelectedItemFromList(int listBoxID)
{
	return GetListBox(listBoxID)->GetSelected();
}

string cInterfaceWindow::GetSelectedTextFromList(int listBoxID)
{
	return GetListBox(listBoxID)->GetSelectedText();
}

int cInterfaceWindow::GetAfterSelectedItemFromList(int listBoxID)
{
	return GetListBox(listBoxID)->GetAfterSelected();
}

bool cInterfaceWindow::ListDoubleClicked(int listBoxID)
{
	return GetListBox(listBoxID)->DoubleClicked();
}

void cInterfaceWindow::SetListTopPos(int listBoxID, int pos)
{
	if (pos < 0)	return;
	GetListBox(listBoxID)->m_Position = pos;
	GetListBox(listBoxID)->m_ScrollBar->SetTopValue(pos);
}

void cInterfaceWindow::SetSelectedItemInList(int listBoxID, int itemID, bool ev, bool DeselectOthers)
{
	if (itemID == -1)	return;
	GetListBox(listBoxID)->SetSelected(itemID, ev, DeselectOthers);
}

int cInterfaceWindow::ArrowDownListBox(int ID)
{
	if (ID == -1)		return -1;
	return GetListBox(ID)->ArrowDownList();
}

bool cInterfaceWindow::IsMultiSelected(int ID)
{
	if (ID == -1)		return false;
	return GetListBox(ID)->HasMultiSelected();
}

int cInterfaceWindow::ArrowUpListBox(int ID)
{
	if (ID == -1)		return -1;
	return GetListBox(ID)->ArrowUpList();
}

void cInterfaceWindow::ClearListBox(int ID)
{
	GetListBox(ID)->ClearList();
	// update "item total" reference for scroll bar
	GetListBox(ID)->m_ScrollBar->m_ItemsTotal = 0;
	GetListBox(ID)->m_ScrollBar->SetTopValue(0);
}

cListBox* cInterfaceWindow::GetListBox(int id)
{
    if (id == -1) return nullptr;
    return dynamic_cast<cListBox*>(m_Widgets[id].get());
}

cInterfaceWindowXML::cInterfaceWindowXML(const char* base_file) :
    m_filename( (DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << base_file).str() ) {

}

cInterfaceWindowXML::~cInterfaceWindowXML()
{
	g_LogFile.write(m_filename);
}

void cInterfaceWindowXML::load(cWindowManager* wm)
{
    cInterfaceWindow::load(wm);
	TiXmlDocument doc(m_filename);
	if (!doc.LoadFile()) {
		g_LogFile.ss() << "cInterfaceWindowXML: " << "Can't load screen definition from '" << m_filename << "'" << endl;
		g_LogFile.ss() << "Error: line " << doc.ErrorRow() << ", col " << doc.ErrorCol() << ": " << doc.ErrorDesc() << endl;
		g_LogFile.ssend();
		return;
	}
	/*
	*	get the docuement root
	*/
	TiXmlElement *el, *root_el = doc.RootElement();
    widget_map_t widgets;
	/*
	*	loop over the elements attached to the root
	*/
	for (el = root_el->FirstChildElement(); el; el = el->NextSiblingElement())
	{
        string tag = el->ValueStr();
        try {
            if (tag == "Define") { define_widget(el, widgets);		continue; }
            if (tag == "Widget") { place_widget(el, "", widgets);	continue; }
            if (tag == "Window") { read_window_definition(el);		    continue; }
            if (tag == "EditBox") { read_editbox_definition(el);	    continue; }
            if (tag == "Text") { read_text_item(el);				    continue; }
            if (tag == "Button") { read_button_definition(el);		    continue; }
            if (tag == "Image") { read_image_definition(el);		    continue; }
            if (tag == "ListBox") { read_listbox_definition(el);	    continue; }
            if (tag == "Checkbox"){ read_checkbox_definition(el);	    continue; }
            if (tag == "Slider") { read_slider_definition(el);		    continue; }
		} catch(std::runtime_error& e) {
            g_LogFile.ss() << "Error: Could not create Widget " << tag << " from file '" << m_filename << "': " << e.what();
            g_LogFile.ssend();
	    }

		g_LogFile.ss() << "Error: unexpected tag in '" << m_filename << "': '" << tag << "' ...";
		g_LogFile.ssend();
	}

	set_ids();
}

void cInterfaceWindowXML::read_text_item(TiXmlElement *el)
{
    int id;
    sXmlWidgetPart wdg;
    widget_text_item(el, wdg);

	/*
	*	create the text item
	*/
	AddTextItem(id, wdg.x, wdg.y, wdg.w, wdg.h, wdg.text, wdg.fontsize, wdg.force_scroll, wdg.r, wdg.g, wdg.b);
	/*
	*	make a note of the ID
	*/
	register_id(id, wdg.name);
}

void cInterfaceWindowXML::define_widget(TiXmlElement * base_el, widget_map_t& widgets)
{
	CLog l;
	TiXmlElement *el;
	string widget_name;
	XmlUtil xu(m_filename);
	/*
	*	first get the widget name
	*/
	if (!xu.get_att(base_el, "Widget", widget_name))
	{
		l.ss() << "Error in " << m_filename << ": " << "'Define' tag with no 'Widget' attribute";
		l.ssend();
		return;
	}
	/*
	*	then create a new widget for that name
	*/
	auto widget = std::make_unique<cXmlWidget>();
	/*
	*	this is like reading the file again, in minature
	*/
	for (el = base_el->FirstChildElement(); el; el = el->NextSiblingElement())
	{
		string tag = el->ValueStr();
		l.ss() << "define widget: '" << tag << "'";
		l.ssend();

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

		l.ss() << "Warning: Unhandled widget tag: '" << tag << "'";
		l.ssend();
	}

	widgets[widget_name] = std::move(widget);
}

void cInterfaceWindowXML::place_widget(TiXmlElement * el, string suffix, const widget_map_t& widgets)
{
	CLog l;
	int x, y;
	stringstream ss;
	string seq, name;
	bool cache;
	XmlUtil xu(m_filename);
	/*
	*	we need the base co-ords for the widget and the
	*	sequence number to generate new names
	*/
	if (xu.get_att(el, "Definition", name) == 0) {
		l.ss() << "Error in " << m_filename << ": " << "'Widget' tag found with no 'Definition' attribute";
		l.ssend();
		return;
	}
	l.ss() << "Placing Widget '" << name << "'";
	l.ssend();

	xu.get_att(el, "XPos", x);
	xu.get_att(el, "YPos", y);
	xu.get_att(el, "Seq", seq);
	xu.get_att(el, "Cache", cache, true);
	add_widget(name, x, y, seq, widgets);
}

void cInterfaceWindowXML::add_widget(string widget_name, int x, int y, string seq, const widget_map_t& widgets)
{
	int id;
	CLog l;
	auto widget_it = widgets.find(widget_name);
	if (widget_it == widgets.end())
	{
		l.ss() << "Error: can't find definition for widget '" << widget_name << "'";
		l.ssend();
		return;
	}

	auto widget = widget_it->second.get();

	/*
	*	now loop over the widget components
	*/
	for (int i = 0; i < widget->size(); i++) {
		sXmlWidgetPart &xw = (*widget)[i];
		string tag = xw.type;
		string name = xw.name + seq;
		/*
		*		the OO way to do this is to subclass
		*		sXmlWidgetPart and have each class have its own
		*		add() routine.
		*
		*		but then I need to store a vector of pointers
		*		because the vector class creates a new, empty instance
		*		and copies in the data, meaning that subclass information
		*		gets discarded for instance vectors.
		*
		*		and storing pointers means needing to delete them
		*		but without getting them deleted on each copy ...
		*
		*		all told it's less fuss to use a big IF, really
		*/
		l.ss() << "add_widget: x = " << x << ", xw.x = " << xw.x << ", y = " << y << ", xw.y = " << xw.y;
		l.ssend();

		int full_x = x + xw.x;
		int full_y = y + xw.y;

		if (tag == "Button") {
			cerr << "adding button: " << xw.off << endl;
			auto bp = [](const std::string& source) -> std::string { return source.empty() ? "" : ButtonPath(source); };
            id = AddButton(bp(xw.off), bp(xw.disabled_img), bp(xw.on), full_x, full_y, xw.w, xw.h,
                           xw.alpha, xw.scale);
			register_id(id, name);
            HideWidget(id, xw.hide);
		}
		else if (tag == "Image") {
			cerr << "adding image: " << xw.file << endl;
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
			l.ss() << "Placing nested widget at " << full_x << ", " << full_y;
			l.ssend();
			add_widget(xw.name, full_x, full_y, xw.seq + seq, widgets);
		}
		else {
			g_LogFile.ss() << "Error: unepected tag in widget '" << tag << ": " << "'.";
			g_LogFile.ssend();
		}
	}
}


void cInterfaceWindowXML::read_window_definition(TiXmlElement *el)
{
	XmlUtil xu(m_filename);
	int x, y, w, h, border_size;
	xu.get_att(el, "XPos", x);
	xu.get_att(el, "YPos", y);
	xu.get_att(el, "Width", w);
	xu.get_att(el, "Height", h);
	xu.get_att(el, "Border", border_size);
	CreateWindow(x, y, w, h, border_size);
}

void cInterfaceWindowXML::read_editbox_definition(TiXmlElement *el)
{
    int id;
    sXmlWidgetPart wdg;
    widget_editbox_item(el, wdg);

	AddEditBox(id, wdg.x, wdg.y, wdg.w, wdg.h, wdg.bordersize, wdg.fontsize);
	register_id(id, wdg.name);
}

void cInterfaceWindowXML::read_listbox_definition(TiXmlElement *el)
{
    // TODO widget_listbox seems incomplete, leave like this for now
	string name;
	XmlUtil xu(m_filename);
	bool events = true, multi = false, show_headers = false, header_div = true, header_sort = true;
	int x, y, w, h, border_size = 1, fontsize = 10, rowheight = 20;

	xu.get_att(el, "Name", name);
	xu.get_att(el, "XPos", x);
	xu.get_att(el, "YPos", y);
	xu.get_att(el, "Width", w);
	xu.get_att(el, "Height", h);
	xu.get_att(el, "FontSize", fontsize); if (fontsize == 0) fontsize = 10;
	xu.get_att(el, "RowHeight", rowheight); if (rowheight == 0) rowheight = 20;

	xu.get_att(el, "Border", border_size, Optional);
	xu.get_att(el, "Events", events, Optional);
	xu.get_att(el, "Multi", multi, Optional);					// Multi-select
	xu.get_att(el, "ShowHeaders", show_headers, Optional);		// Show Headers, for multi-column listboxes
	xu.get_att(el, "HeaderDiv", header_div, Optional);			// Show dividers between headers
	xu.get_att(el, "HeaderClicksSort", header_sort, Optional);	// Sort list when user clicks on column header?

	auto box = AddListBox(x, y, w, h, border_size, events, multi, show_headers, header_div, header_sort, fontsize, rowheight);
	register_id(box->get_id(), name);

	// Check for column definitions
	CLog l;
	TiXmlElement *sub_el;
	std::vector<int> column_offset;
	std::vector<bool> column_skip;
	std::vector<std::string> column_name, column_header;
	for (sub_el = el->FirstChildElement(); sub_el; sub_el = sub_el->NextSiblingElement())
	{
		string tag = sub_el->ValueStr();
		l.ss() << "define listbox element: '" << tag << "'";
		l.ssend();
		// XML definition can arrange columns in any order and even leave some columns out if desired
		// Listbox itself (via DefineColumns) keeps track of what order they're to be displayed in based on this
		if (tag == "Column")   // hey, we've got a column definition in this listbox
		{
			std::string name;
			int offset = 0;
			bool skip = false;
			xu.get_att(sub_el, "Name", name);  // Careful, Name is how we ID columns
			column_name.push_back(std::move(name));
			xu.get_att(sub_el, "Header", name, Optional);  // Text header of column
            column_header.push_back(std::move(name));
			xu.get_att(sub_el, "Offset", offset, Optional);  // X Offset for column position
			column_offset.push_back(offset);
			xu.get_att(sub_el, "Skip", skip, Optional);  // hide column? not really used
			column_skip.push_back(skip);
		}
		else
		{
			l.ss() << "Warning: Unhandled listbox element: '" << tag << "'";
			l.ssend();
		}
	}
	// If we have columns defined, go ahead and give the listbox all the gory details
	if (column_name.size() > 0)	box->DefineColumns(column_name, column_header, column_offset, column_skip);
}


void read_generic(XmlUtil& xu, TiXmlElement* el, sXmlWidgetBase& data) {
    /*
	*	get the button name - we'll use this to match up
	*	interface IDs
	*/
    xu.get_att(el, "Name", data.name);

    /*
	*	xywh
	*/
    xu.get_att(el, "XPos", data.x);
    xu.get_att(el, "YPos", data.y);
    xu.get_att(el, "Width", data.w);
    xu.get_att(el, "Height", data.h, data.h != -1);

    xu.get_att(el, "Hidden", data.hide, true);
}

void cInterfaceWindowXML::widget_editbox_item(TiXmlElement *el, sXmlWidgetPart& xw)
{
	XmlUtil xu(m_filename);

    read_generic(xu, el, xw);
	xu.get_att(el, "FontSize", xw.fontsize); if (xw.fontsize == 0) xw.fontsize = 16;
	xu.get_att(el, "Border", xw.bordersize, Optional);
	xu.get_att(el, "Multi", xw.multi, Optional);
	xu.get_att(el, "Events", xw.events, Optional);
}

void cInterfaceWindowXML::widget_listbox_item(TiXmlElement *el, sXmlWidgetPart& xw)
{
	XmlUtil xu(m_filename);

    read_generic(xu, el, xw);
	xu.get_att(el, "Border", xw.bordersize, Optional);
	xu.get_att(el, "Multi", xw.multi, Optional);
	xu.get_att(el, "Events", xw.events, Optional);
}

void cInterfaceWindowXML::widget_checkbox_item(TiXmlElement *el, sXmlWidgetPart& xw)
{
	XmlUtil xu(m_filename);

    read_generic(xu, el, xw);
	xu.get_att(el, "Text", xw.text);
	xu.get_att(el, "FontSize", xw.fontsize);
	xu.get_att(el, "LeftOrRight", xw.leftorright);
}

void cInterfaceWindowXML::widget_widget(TiXmlElement *el, sXmlWidgetPart& xw)
{
	XmlUtil xu(m_filename);

	xu.get_att(el, "Definition", xw.name);
	xu.get_att(el, "XPos", xw.x);
	xu.get_att(el, "YPos", xw.y);
	xu.get_att(el, "Seq", xw.seq);
	xu.get_att(el, "cache", xw.cache, true);
}

void cInterfaceWindowXML::read_checkbox_definition(TiXmlElement *el)
{
    int id;

    sXmlWidgetPart wdg;
    widget_checkbox_item(el, wdg);

    AddCheckbox(id, wdg.x, wdg.y, wdg.w, wdg.h, wdg.text, wdg.fontsize, wdg.leftorright);
    register_id(id, wdg.name);
}

void cInterfaceWindowXML::read_image_definition(TiXmlElement *el)
{
	int id;
    sXmlWidgetPart wdg;
    widget_image_item(el, wdg);
    DirPath dp = ImagePath(wdg.file);

	AddImage(id, dp, wdg.x, wdg.y, wdg.w, wdg.h, wdg.stat, wdg.r, wdg.g, wdg.b);
    HideWidget(id, wdg.hide);
	register_id(id, wdg.name);
}

void cInterfaceWindowXML::read_button_definition(TiXmlElement *el)
{
    int id;
    sXmlWidgetPart wdg;
    widget_button_item(el, wdg);
    DirPath dp = ImagePath(wdg.file);

    auto bp = [](const std::string& source) -> std::string { return source.empty() ? "" : ButtonPath(source); };
    id = AddButton(bp(wdg.off), bp(wdg.disabled_img), bp(wdg.on), wdg.x, wdg.y, wdg.w, wdg.h, wdg.alpha, wdg.scale);
	register_id(id, wdg.name);
}

void cInterfaceWindowXML::read_slider_definition(TiXmlElement *el)
{
    int id;
    sXmlWidgetPart wdg;
    widget_slider_item(el, wdg);

	AddSlider(id, wdg.x, wdg.y, wdg.w, wdg.r, wdg.g, wdg.h, wdg.b, wdg.events);
    DisableWidget(id, wdg.stat);
    HideWidget(id, wdg.hide);
	register_id(id, wdg.name);
}

void cInterfaceWindowXML::widget_slider_item(TiXmlElement *el, sXmlWidgetPart& xw)
{
	XmlUtil xu(m_filename);
	xw.stat = false;
	xw.events = true;
	xw.r = 0; xw.g = 100; xw.b = 0; xw.h = 5;

    read_generic(xu, el, xw);
	xu.get_att(el, "Increment", xw.h, Optional);
	xu.get_att(el, "MinValue", xw.r, Optional);
	xu.get_att(el, "MaxValue", xw.g, Optional);
	xu.get_att(el, "Value", xw.b, Optional);
	xu.get_att(el, "Disabled", xw.stat, Optional);
	xu.get_att(el, "LiveUpdate", xw.events, Optional);

	// for some reason, optional ints above are unfortunately being set to 0 when not present; correct for that
	if (xw.r == 0 && xw.g == 0)	xw.g = 100;
	if (xw.b < xw.r)			xw.b = xw.r;
	if (xw.b > xw.g)			xw.b = xw.g;
	if (xw.h == 0)				xw.h = 5;
}

void cInterfaceWindowXML::widget_text_item(TiXmlElement *el, sXmlWidgetPart& xw)
{
	XmlUtil xu(m_filename);
    read_generic(xu, el, xw);

	xu.get_att(el, "Text", xw.text);
	xu.get_att(el, "FontSize", xw.fontsize);
	xu.get_att(el, "ForceScrollbar", xw.force_scroll, Optional);
	xu.get_att(el, "Red", xw.r, Optional);
	xu.get_att(el, "Green", xw.g, Optional);
	xu.get_att(el, "Blue", xw.b, Optional);
}

void cInterfaceWindowXML::widget_button_item(TiXmlElement *el, sXmlWidgetPart& xw)
{
	XmlUtil xu(m_filename);

	std::string img_base = "";

    read_generic(xu, el, xw);

	xu.get_att(el, "Image", img_base, Optional);
	if (!img_base.empty()) {
		xw.on           = img_base + "On.png";
		xw.off          = img_base + "Off.png";
		xw.disabled_img = img_base + "Disabled.png";
	}
	xu.get_att(el, "On", xw.on, Optional);
	xu.get_att(el, "Off", xw.off, Optional);
	xu.get_att(el, "Disabled", xw.disabled_img, Optional);
	xu.get_att(el, "Transparency", xw.alpha);
	xu.get_att(el, "Scale", xw.scale);
	xu.get_att(el, "cache", xw.cache, Optional);
}

void cInterfaceWindowXML::widget_image_item(TiXmlElement *el, sXmlWidgetPart& xw)
{
	XmlUtil xu(m_filename);

	xw.stat = false;
	xw.r = xw.g = xw.b = 0;

    read_generic(xu, el, xw);
	xu.get_att(el, "File", xw.file);
	xu.get_att(el, "Red", xw.r, Optional);
	xu.get_att(el, "Green", xw.g, Optional);
	xu.get_att(el, "Blue", xw.b, Optional);
	xu.get_att(el, "Stat", xw.stat, Optional);
}

void cInterfaceWindowXML::register_id(int id, string name)
{
	CLog l;
	l.ss() << "registering ID " << id << " to name '" << name << "'";
	l.ssend();
	name_to_id[name] = id;
	id_to_name[id] = name;
}

int cInterfaceWindowXML::get_id(string a, string b, string c, string d)
{
	if (name_to_id.find(a) != name_to_id.end())	return name_to_id[a];
	if (name_to_id.find(b) != name_to_id.end())	return name_to_id[b];
	if (name_to_id.find(c) != name_to_id.end())	return name_to_id[c];
	if (name_to_id.find(d) != name_to_id.end())	return name_to_id[d];

	bool un = false;
	if (a == "*Unused*" || a == "*Optional*")	{ un=true; a=""; }
	if (b == "*Unused*" || b == "*Optional*")	{ un=true; b=""; }
	if (c == "*Unused*" || c == "*Optional*")	{ un=true; c=""; }
	if (d == "*Unused*" || d == "*Optional*")	{ un=true; d=""; }

	if (!un || cfg.debug.log_debug()) { g_LogFile.ss() << "Error setting ids for interface " << (un ? " but it is not used or optional " : "") << "- acceptable names for this item are : '" << a << "', '" << b << "', '" << c << "', '" << d << "'.";	g_LogFile.ssend(); }
	return -1;
}

cWindowManager& cInterfaceWindow::window_manager() const
{
    return *m_WindowManager;
}

void cInterfaceWindow::push_window(const std::string& name) const {
    window_manager().push(name);
}

void cInterfaceWindow::replace_window(const std::string& name) const {
    window_manager().replace(name);
}

void cInterfaceWindow::update()
{
    process();
}

void cInterfaceWindow::OnKeyPress(SDL_keysym keysym)
{
    try {
        if(m_KeyCallbacks.count(keysym.sym) != 0) {
            m_KeyCallbacks[keysym.sym]();
            return;
        }

        if(m_KeyboardFocusWidget) {
            if(m_KeyboardFocusWidget->OnKeyPress(keysym)) return;
        }
        for(auto& wdg : m_Widgets) {
            if(wdg->OnKeyPress(keysym)) return;
        }
    } catch (std::exception& exception) {
        push_message(exception.what(), 1);
    }
}

void cInterfaceWindow::pop_window() const
{
    window_manager().Pop();
}

void cInterfaceWindow::pop_to_window(const std::string& target) const
{
    window_manager().PopToWindow(target);
}

IBuilding& cInterfaceWindow::active_building() const {
    auto bld = window_manager().GetActiveBuilding();
    if(bld)
        return *bld;
    throw std::logic_error("No active building");
}

void cInterfaceWindow::set_active_building(IBuilding* building)
{
    window_manager().SetActiveBuilding(building);
}

IBuilding& cInterfaceWindow::cycle_building(int direction)
{
    auto& buildings = g_Game->buildings();
    auto next = (buildings.find(&active_building()) + direction) % buildings.num_buildings();
    auto& active = buildings.get_building(next);
    set_active_building(&active);
    return active;
}

void cInterfaceWindow::input_integer(std::function<void(int)> callback)
{
    window_manager().InputInteger(std::move(callback));
}

void cInterfaceWindow::input_confirm(std::function<void()> callback)
{
    window_manager().InputConfirm(std::move(callback));
}

void cInterfaceWindow::input_string(std::function<void(const std::string&)> callback)
{
    window_manager().InputString(std::move(callback));
}

void cInterfaceWindow::input_choice(std::string question, std::vector<std::string> options, std::function<void(int)> callback)
{
    window_manager().InputChoice(std::move(question), std::move(options), std::move(callback));
}

void cInterfaceWindow::SetListBoxSelectionCallback(int id, std::function<void(int)> cb)
{
    if (id>-1)
        GetListBox(id)->SetSelectionCallback(std::move(cb));
}

void cInterfaceWindow::SetListBoxDoubleClickCallback(int id, std::function<void(int)> cb)
{
    if (id>-1)
        GetListBox(id)->SetDoubleClickCallback(std::move(cb));
}

void cInterfaceWindow::SetButtonHotKey(int id, SDLKey key)
{
    if(id > -1) {
        GetButton(id)->SetHotKey(key);
    }
}

void cInterfaceWindow::SetCheckBoxCallback(int id, std::function<void(bool)> cb)
{
    if(id > -1) {
        GetCheckBox(id)->SetCallback(std::move(cb));
    }
}

void cInterfaceWindow::SetSliderCallback(int id, std::function<void(int)> cb)
{
    if(id > -1) {
        GetSlider(id)->SetCallback(std::move(cb));
    }
}

void cInterfaceWindow::SetSliderHotKeys(int id, SDLKey increase, SDLKey decrease) {
    if(id > -1) {
        GetSlider(id)->SetHotKeys(increase, decrease);
    }
}

void cInterfaceWindow::SetListBoxHotKeys(int id, SDLKey up, SDLKey down)
{
    if(id > -1) {
        GetListBox(id)->SetArrowHotKeys(up, down);
    }
}

void cInterfaceWindow::push_message(std::string text, int color)
{
    window_manager().PushMessage(std::move(text), color);
}

sGirl& cInterfaceWindow::active_girl() const
{
    auto girl = window_manager().GetActiveGirl();
    if(girl)
        return *girl;
    throw std::logic_error("No girl selected");
}

void cInterfaceWindow::set_active_girl(sGirl * girl)
{
    window_manager().SetActiveGirl(girl);
}

sGirl* cInterfaceWindow::selected_girl() const
{
    return window_manager().GetActiveGirl();
}

void cInterfaceWindow::TabFocus()
{
    if(m_Widgets.empty())
        return;

    // find focused element
    bool found  = false;
    for(auto& widget : m_Widgets) {
        if(found) {
            if(widget->SetFocus(true)) {
                SetFocusTo(widget.get());
                return;
            }
        }
        // looking for the originally focused element
        else if (m_KeyboardFocusWidget == nullptr || widget.get() == m_KeyboardFocusWidget) {
            found = true;
        }
    }

    // Did not find a focusable widget. Try again from the start.
    for(auto& widget : m_Widgets) {
        if (widget->SetFocus(true)) {
            SetFocusTo(widget.get());
            return;
        }
    }
    // There really is none!
}

void cInterfaceWindow::AddKeyCallback(SDLKey key, std::function<void()> callback)
{
    m_KeyCallbacks.emplace(key, std::move(callback));
}

void cInterfaceWindow::AddWidget(std::unique_ptr<cUIWidget> wdg) {
    m_Widgets.push_back(std::move(wdg));
}

void cModalWindow::process() {
    if(!m_Widgets.empty()) {
        if(m_Widgets.back()->IsDisabled())
            pop_window();
    } else {
        pop_window();
    }
}

