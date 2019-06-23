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

#include <time.h>
#include <string>
#include <cctype>
#include "cInterfaceWindow.h"
#include "DirPath.h"
#include "CLog.h"
#include "tinyxml.h"
#include "XmlUtil.h"
#include "libintl.h" 
#include "Globals.h"

using namespace std;

typedef unsigned int u_int;

extern CLog g_LogFile;
extern CGraphics g_Graphics;
extern CResourceManager rmanager;
extern cInterfaceEventManager g_InterfaceEvents;

extern unsigned char g_WindowBorderR, g_WindowBorderG, g_WindowBorderB;
extern unsigned char g_WindowBackgroundR, g_WindowBackgroundG, g_WindowBackgroundB;

extern bool g_InitWin;

cInterfaceWindow::~cInterfaceWindow()
{
	Free();
}

void cInterfaceWindow::Free()
{
	for (unsigned int i = 0; i< m_Buttons.size(); i++)		delete m_Buttons[i];
	m_Buttons.clear();
	for (unsigned int i = 0; i< m_ScrollBars.size(); i++)	delete m_ScrollBars[i];
	m_ScrollBars.clear();
	for (unsigned int i = 0; i<m_EditBoxes.size(); i++)		delete m_EditBoxes[i];
	m_EditBoxes.clear();

	//there is a special case where a picture may have already been deleted by something prior
	//and it seems like way too much effort to include smart pointers
	//so just detect any pointers that have been deleted already
	for (unsigned int i = 0; i < m_Images.size(); i++)
	{
		if (m_Images[i])
		{
			// If this is NULL then the deconstructor has been called already.
			if (m_Images[i]->m_AnimatedImage != 0)
				delete m_Images[i];
		}
	}

	m_Images.clear();
	for (unsigned int i = 0; i<m_CheckBoxes.size(); i++)	delete m_CheckBoxes[i];
	m_CheckBoxes.clear();
	for (unsigned int i = 0; i<m_TextItems.size(); i++)		delete m_TextItems[i];
	m_TextItems.clear();
	for (unsigned int i = 0; i<m_ListBoxes.size(); i++)		delete m_ListBoxes[i];
	m_ListBoxes.clear();
	for (unsigned int i = 0; i<m_Sliders.size(); i++)		delete m_Sliders[i];
	m_Sliders.clear();
	if (m_Background)	SDL_FreeSurface(m_Background);
	m_Background = 0;
	if (m_Border)		SDL_FreeSurface(m_Border);
	m_Border = 0;
}

void cInterfaceWindow::UpdateWindow(int x, int y)
{
	// check buttons
	for (unsigned int i = 0; i< m_Buttons.size(); i++)		m_Buttons[i]->IsOver(x, y);
	// check listbox scroll bars
	for (unsigned int i = 0; i< m_ScrollBars.size(); i++)	m_ScrollBars[i]->IsOver(x, y);
	// check sliders
	for (unsigned int i = 0; i< m_Sliders.size(); i++)		m_Sliders[i]->IsOver(x, y);
}

void cInterfaceWindow::MouseDown(int x, int y)
{  // this method added to handle draggable objects
	// check listbox scroll bars
	for (unsigned int i = 0; i< m_ScrollBars.size(); i++)	m_ScrollBars[i]->MouseDown(x, y);
	// check sliders
	for (unsigned int i = 0; i< m_Sliders.size(); i++)		m_Sliders[i]->MouseDown(x, y);
}

void cInterfaceWindow::Click(int x, int y, bool mouseWheelDown, bool mouseWheelUp)
{
	// things which shouldn't respond to scroll wheel up/down as clicks
	if (!mouseWheelDown && !mouseWheelUp)
	{
		// check buttons
		for (unsigned int i = 0; i< m_Buttons.size(); i++)			m_Buttons[i]->ButtonClicked(x, y);
		// Check EditBoxes
		for (unsigned int i = 0; i<m_EditBoxes.size(); i++)			m_EditBoxes[i]->OnClicked(x, y);
		// check check boxes
		for (unsigned int i = 0; i<m_CheckBoxes.size(); i++)		m_CheckBoxes[i]->ButtonClicked(x, y);
	}
	// things which should only respond to mouse scroll wheel up/down
	else if (mouseWheelDown || mouseWheelUp)
	{
		for (unsigned int i = 0; i<m_TextItems.size(); i++)			m_TextItems[i]->MouseScrollWheel(x, y, mouseWheelDown);
	}
	// check listbox scroll bars
	for (unsigned int i = 0; i< m_ScrollBars.size(); i++)			m_ScrollBars[i]->ButtonClicked(x, y, mouseWheelDown, mouseWheelUp);
	// check sliders
	for (unsigned int i = 0; i<m_Sliders.size(); i++)				m_Sliders[i]->ButtonClicked(x, y, mouseWheelDown, mouseWheelUp);
	// Check list boxes
	for (unsigned int i = 0; i<m_ListBoxes.size(); i++)				m_ListBoxes[i]->OnClicked(x, y, mouseWheelDown, mouseWheelUp);
}

void cInterfaceWindow::Reset()
{
	for (unsigned int i = 0; i<m_Images.size(); i++)
	{
		if (m_Images[i]->m_loaded == false)	m_Images[i]->m_Image = 0;
	}
	for (unsigned int i = 0; i<m_ListBoxes.size(); i++)
	{
		m_ListBoxes[i]->ClearList();
	}
}

void cInterfaceWindow::Draw()
{
	if (m_Background && m_Border)
	{
		// Draw the window
		SDL_Rect offset;
		offset.x = m_XPos;
		offset.y = m_YPos;
		// blit to the screen
		SDL_BlitSurface(m_Border, 0, g_Graphics.GetScreen(), &offset);
		offset.x = m_XPos + m_BorderSize;
		offset.y = m_YPos + m_BorderSize;
		SDL_BlitSurface(m_Background, 0, g_Graphics.GetScreen(), &offset);
	}
	if (m_BackgroundSurface)
	{
		SDL_Rect clip;
		clip.x = m_XPos + m_BorderSize;
		clip.y = m_YPos + m_BorderSize;
		clip.w = m_Width - (m_BorderSize * 2);
		clip.h = m_Height - (m_BorderSize * 2);
		m_BackgroundSurface->DrawSurface(clip.x, clip.y, 0, &clip, true, false); // `J`
	}

	for (unsigned int i = 0; i< m_Images.size(); i++)		m_Images[i]->Draw();		// draw Images
	for (unsigned int i = 0; i<m_EditBoxes.size(); i++)		m_EditBoxes[i]->Draw();		// Draw Editboxes
	for (unsigned int i = 0; i<m_TextItems.size(); i++)		m_TextItems[i]->Draw();		// Draw Text item boxes
	for (unsigned int i = 0; i<m_ListBoxes.size(); i++)		m_ListBoxes[i]->Draw();		// Draw list boxes
	for (unsigned int i = 0; i<m_Sliders.size(); i++)		m_Sliders[i]->Draw();		// Draw sliders
	for (unsigned int i = 0; i<m_Buttons.size(); i++)		m_Buttons[i]->Draw();		// draw buttons
	for (unsigned int i = 0; i<m_ScrollBars.size(); i++)	m_ScrollBars[i]->Draw();	// draw listbox scroll bars
	for (unsigned int i = 0; i<m_CheckBoxes.size(); i++)	m_CheckBoxes[i]->Draw();	// draw check boxes
}

void cInterfaceWindow::AddButton(const char *img_name, int & ID, int x, int y, int width, int height, bool transparency, bool scale, bool cached)
{
	DirPath dp = ButtonPath(img_name);
	string on = string(dp.c_str()) + "On.png";
	string off = string(dp.c_str()) + "Off.png";
	string disabled;
	disabled = string(dp.c_str()) + "Disabled.png";
	AddButton(off, disabled, on, ID, x, y, width, height, transparency, scale, cached);
}

void cInterfaceWindow::AddButtonND(const char *img_name, int & ID, int x, int y, int width, int height, bool transparency, bool scale, bool cached)
{
	DirPath dp = ButtonPath(img_name);
	string on = string(dp.c_str()) + "On.png";
	string off = string(dp.c_str()) + "Off.png";
	string disabled;
	AddButton(off, disabled, on, ID, x, y, width, height, transparency, scale, cached);
}

void cInterfaceWindow::AddButton(string image_name, int & ID, int x, int y, int width, int height, bool transparency, bool scale, bool cached)
{
	DirPath dp = ButtonPath(image_name);
	string on = string(dp.c_str()) + "On.png";
	string off = string(dp.c_str()) + "Off.png";
	string disabled = string(dp.c_str()) + "Disabled.png";
	AddButton(off, disabled, on, ID, x, y, width, height, transparency, scale, cached);
}

void cInterfaceWindow::AddButtonND(string image_name, int & ID, int x, int y, int width, int height, bool transparency, bool scale, bool cached)
{
	DirPath dp = ButtonPath(image_name);
	string on = string(dp.c_str()) + "On.png";
	string off = string(dp.c_str()) + "Off.png";
	string disabled = "";
	AddButton(off, disabled, on, ID, x, y, width, height, transparency, scale, cached);
}

void cInterfaceWindow::AddButton(string OffImage, string DisabledImage, string OnImage, int & ID, int x, int y, int width, int height, bool transparency, bool scale, bool cached)
{
	if (scale)
	{
		width = (int)((float)width*m_xRatio);
		height = (int)((float)height*m_yRatio);
		x = (int)((float)x*m_xRatio);
		y = (int)((float)y*m_yRatio);
	}
	ID = m_Buttons.size();
	// create button
	cButton* newButton = new cButton();
	newButton->CreateButton(OffImage, DisabledImage, OnImage, ID, x + m_XPos, y + m_YPos, width, height, transparency, cached);

	// Store button
	m_Buttons.push_back(newButton);
}

void cInterfaceWindow::AddScrollBar(int & ID, int x, int y, int width, int height, int visibleitems)
{
	ID = m_ScrollBars.size();
	// create scroll bar
	g_LogFile.write("initializing scrollbar");
	cScrollBar* newScrollBar = new cScrollBar();
	g_LogFile.write("creating scrollbar");
	newScrollBar->CreateScrollBar(ID, x + m_XPos, y + m_YPos, width, height, visibleitems);
	newScrollBar->m_ScrollAmount = cfg.resolution.list_scroll();

	// Store scroll bar
	g_LogFile.write("storing scrollbar");
	m_ScrollBars.push_back(newScrollBar);
}

void cInterfaceWindow::AddTextItemScrollBar(int id)
{	// adding scrollbar to a TextItem
	int x = m_TextItems[id]->GetXPos();
	int y = m_TextItems[id]->GetYPos();
	int width = m_TextItems[id]->GetWidth();
	int height = m_TextItems[id]->GetHeight();
	int newID = m_ScrollBars.size();
	AddScrollBar(newID, x + width - m_XPos - 15, y - m_YPos, 16, height, height);
	m_TextItems[id]->m_ScrollBar = m_ScrollBars[newID];  // give TextItem pointer to scrollbar
	m_ScrollBars[newID]->ParentPosition = &m_TextItems[id]->m_ScrollChange;  // give scrollbar pointer to value it should update
	m_ScrollBars[newID]->m_ScrollAmount = cfg.resolution.text_scroll() * m_TextItems[id]->m_Font.GetFontHeight();
	m_ScrollBars[newID]->m_PageAmount = m_ScrollBars[newID]->m_PageAmount - m_TextItems[id]->m_Font.GetFontHeight();
}

void cInterfaceWindow::HideImage(int id, bool hide)
{
	if (id == -1) return;
	(hide) ? m_Images[id]->hide() : m_Images[id]->unhide();

}

void cInterfaceWindow::HideButton(int id, bool hide)
{
	if (id == -1) return;
	(hide) ? m_Buttons[id]->hide() : m_Buttons[id]->unhide();
}

void cInterfaceWindow::AddImage(int & id, string filename, int x, int y, int width, int height, bool statImage, int R, int G, int B)
{
	width = (int)((float)width*m_xRatio);
	height = (int)((float)height*m_yRatio);
	x = (int)((float)x*m_xRatio);
	y = (int)((float)y*m_yRatio);

	// create image
	id = m_Images.size();
	cImageItem* newImage = new cImageItem();
	newImage->CreateImage(id, filename, x + m_XPos, y + m_YPos, width, height, statImage, R, G, B);

	// Store button
	m_Images.push_back(newImage);
}

void cInterfaceWindow::SetImage(int id, CSurface* image)
{
	m_Images[id]->m_Image = image;
	m_Images[id]->m_AnimatedImage = 0;
}

void cInterfaceWindow::SetImage(int id, cAnimatedSurface* image)
{
	m_Images[id]->m_AnimatedImage = image;
}

void cInterfaceWindow::AddEditBox(int & ID, int x, int y, int width, int height, int BorderSize, int FontSize)
{
	width = (int)((float)width*m_xRatio);
	height = (int)((float)height*m_yRatio);
	x = (int)((float)x*m_xRatio);
	y = (int)((float)y*m_yRatio);

	// create button
	ID = m_EditBoxes.size();
	cEditBox* newEditBox = new cEditBox();
	newEditBox->CreateEditBox(ID, x + m_XPos, y + m_YPos, width, height, BorderSize, FontSize);

	// Store button
	m_EditBoxes.push_back(newEditBox);
}

void cInterfaceWindow::HideEditBox(int id, bool hide)
{
	if (id == -1) return;
	(hide) ? m_EditBoxes[id]->hide() : m_EditBoxes[id]->unhide();
}

void cInterfaceWindow::DisableButton(int id, bool disable)
{
	if (id>-1)
		m_Buttons[id]->SetDisabled(disable);
}

void cInterfaceWindow::CreateWindow(int x, int y, int width, int height, int BorderSize)
{
	m_xRatio = 1.0f;	m_yRatio = 1.0f;
	// `J` fixed this to allow for nonscaled 800x600 screen sizes
	if (_G.g_ScreenWidth != cfg.resolution.scalewidth())	m_xRatio = ((float)_G.g_ScreenWidth / (float)cfg.resolution.scalewidth());
	if (_G.g_ScreenHeight != cfg.resolution.scaleheight())	m_yRatio = ((float)_G.g_ScreenHeight / (float)cfg.resolution.scaleheight());

	width = (int)((float)width*m_xRatio);
	height = (int)((float)height*m_yRatio);
	x = (int)((float)x*m_xRatio);
	y = (int)((float)y*m_yRatio);

	m_BorderSize = BorderSize;
	SetPosition(x, y, width, height);
	m_Border = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, 0, 0, 0, 0);
	SDL_FillRect(m_Border, 0, SDL_MapRGB(m_Border->format, g_WindowBorderR, g_WindowBorderG, g_WindowBorderB));

	m_Background = SDL_CreateRGBSurface(SDL_SWSURFACE, width - (BorderSize * 2), height - (BorderSize * 2), 32, 0, 0, 0, 0);
	SDL_FillRect(m_Background, 0, SDL_MapRGB(m_Background->format, g_WindowBackgroundR, g_WindowBackgroundG, g_WindowBackgroundB));
}

void cInterfaceWindow::SetBackgroundImage(string file)
{
	m_BackgroundSurface = new CSurface(file);
}

string cInterfaceWindow::GetEditBoxText(int ID)
{
	return m_EditBoxes[ID]->GetText();
}

void cInterfaceWindow::UpdateEditBoxes(char key, bool upper)
{
	for (unsigned int i = 0; i<m_EditBoxes.size(); i++)
		if (m_EditBoxes[i]->m_HasFocus)
			m_EditBoxes[i]->UpdateText(key, upper);
}

void cInterfaceWindow::Focused()
{
	// clear any events
	g_InterfaceEvents.ClearEvents();
	// clear edit boxes and set the first one as focused
	for (unsigned int i = 0; i < m_EditBoxes.size(); i++)		m_EditBoxes[i]->ClearText();
	if (!m_EditBoxes.empty())	m_EditBoxes[0]->m_HasFocus = true;
}

void cInterfaceWindow::SetCheckBox(int ID, bool on)
{
	m_CheckBoxes[ID]->SetState(on);
}

void cInterfaceWindow::DisableCheckBox(int ID, bool disable)
{
	m_CheckBoxes[ID]->m_Disabled = disable;
}

bool cInterfaceWindow::IsCheckboxOn(int ID)
{
	return m_CheckBoxes[ID]->m_StateOn;
}

void cInterfaceWindow::AddSlider(int & ID, int x, int y, int width, int min, int max, int increment, int value, bool live_update)
{
	width = (int)((float)width*m_xRatio);
	float height = m_yRatio;
	x = (int)((float)x*m_xRatio);
	y = (int)((float)y*m_yRatio);

	ID = m_Sliders.size();
	cSlider* newSlider = new cSlider();
	newSlider->CreateSlider(ID, x + m_XPos, y + m_YPos, width, min, max, increment, value, height);
	newSlider->LiveUpdate(live_update);

	m_Sliders.push_back(newSlider);
}

void cInterfaceWindow::DisableSlider(int ID, bool disable)
{
	if (ID == -1) return;
	m_Sliders[ID]->Disable(disable);
}

void cInterfaceWindow::HideSlider(int ID, bool hide)
{
	if (ID == -1) return;
	m_Sliders[ID]->Hide(hide);
}

void cInterfaceWindow::SliderLiveUpdate(int ID, bool live_update)
{
	if (ID == -1) return;
	m_Sliders[ID]->LiveUpdate(live_update);
}

int cInterfaceWindow::SliderRange(int ID, int min, int max, int value, int increment)
{
	if (ID == -1) return 0;
	return m_Sliders[ID]->SetRange(min, max, value, increment);
}

int cInterfaceWindow::SliderValue(int ID)
{
	if (ID == -1) return 0;
	return m_Sliders[ID]->Value();
}

int cInterfaceWindow::SliderValue(int ID, int value)
{
	if (ID == -1) return 0;
	return m_Sliders[ID]->Value(value);
}

void cInterfaceWindow::SliderMarker(int ID, int value)
{
	if (ID == -1) return;
	m_Sliders[ID]->SetMarker(value);
}

void cInterfaceWindow::SliderMarkerDisable(int ID)
{
	if (ID == -1) return;
	m_Sliders[ID]->RemoveMarker();
}

void cInterfaceWindow::AddCheckbox(int & ID, int x, int y, int width, int height, string text, int size, bool leftorright)
{
	width = (int)((float)width*m_xRatio);
	height = (int)((float)height*m_yRatio);
	x = (int)((float)x*m_xRatio);
	y = (int)((float)y*m_yRatio);

	// create checkbox item
	ID = m_CheckBoxes.size();
	cCheckBox* newCheckBox = new cCheckBox();
	newCheckBox->CreateCheckBox(ID, x + m_XPos, y + m_YPos, width, height, text, size, leftorright);

	// Store text item
	m_CheckBoxes.push_back(newCheckBox);
}

void cInterfaceWindow::AddTextItem(int & ID, int x, int y, int width, int height, string text, int size, bool auto_scrollbar,
	bool force_scrollbar, bool leftorright, int red, int green, int blue)
{
	width = (int)((float)width*m_xRatio);
	height = (int)((float)height*m_yRatio);
	x = (int)((float)x*m_xRatio);
	y = (int)((float)y*m_yRatio);

	// create text item
	ID = m_TextItems.size();
	cTextItem* newTextItem = new cTextItem();
	newTextItem->CreateTextItem(ID, x + m_XPos, y + m_YPos, width, height, text, size, auto_scrollbar, force_scrollbar, leftorright, red, green, blue);

	// Store text item
	m_TextItems.push_back(newTextItem);
}

void cInterfaceWindow::HideText(int id, bool hide)
{
	if (id == -1) return;
	(hide) ? m_TextItems[id]->hide() : m_TextItems[id]->unhide();
}

void cInterfaceWindow::EditTextItem(string text, int ID)
{
	if (ID == -1) return;

	// force width to full (no scrollbar) to properly detect if scrollbar is needed for new text
	m_TextItems[ID]->m_Font.SetMultiline(true, m_TextItems[ID]->GetWidth(), m_TextItems[ID]->GetHeight());
	m_TextItems[ID]->SetText(text);

	if (m_TextItems[ID]->GetHeight() == 0 || text == "")
	{
		if (m_TextItems[ID]->m_ScrollBar)
		{  // has scrollbar but doesn't need one since there doesn't seem to be any text at the moment; hide scrollbar
			m_TextItems[ID]->m_ScrollBar->SetTopValue(0);
			m_TextItems[ID]->m_ScrollBar->hide();
		}
		return;
	}

	m_TextItems[ID]->m_ScrollChange = 0;

	if (m_TextItems[ID]->NeedScrollBarHidden())  // hide scrollbar
	{
		m_TextItems[ID]->m_ScrollBar->SetTopValue(0);
		m_TextItems[ID]->m_ScrollBar->hide();
	}
	else if (m_TextItems[ID]->NeedScrollBarShown())  // un-hide existing scrollbar
		m_TextItems[ID]->m_ScrollBar->unhide();
	else if (m_TextItems[ID]->NeedScrollBar())  // add scrollbar
		AddTextItemScrollBar(ID);

	// update scrollbar if it exists
	if (m_TextItems[ID]->m_ScrollBar)
	{
		if (!m_TextItems[ID]->m_ScrollBar->m_Hidden)
		{  // also, re-render text in narrower space to accommodate scrollbar width
			m_TextItems[ID]->m_Font.SetMultiline(true, m_TextItems[ID]->GetWidth() - 17, m_TextItems[ID]->GetHeight());
			m_TextItems[ID]->SetText(text);
		}
		m_TextItems[ID]->m_ScrollBar->m_ItemsTotal = m_TextItems[ID]->HeightTotal();
	}
}

void cInterfaceWindow::AddListBox(int & ID, int x, int y, int width, int height, int BorderSize, bool enableEvents, bool MultiSelect, bool ShowHeaders, bool HeaderDiv, bool HeaderSort, int fontsize, int rowheight)
{
	width = (int)((float)width*m_xRatio);
	height = (int)((float)height*m_yRatio);
	x = (int)((float)x*m_xRatio);
	y = (int)((float)y*m_yRatio);

	g_LogFile.write("Adding listbox...");
	// create listbox item
	ID = m_ListBoxes.size();
	g_LogFile.write("initializing listbox");
	cListBox* newListBox = new cListBox();
	g_LogFile.write("creating listbox");
	newListBox->CreateListbox(ID, x + m_XPos, y + m_YPos, width, height, BorderSize, MultiSelect, ShowHeaders, HeaderDiv, HeaderSort, fontsize, rowheight);
	g_LogFile.write("enabling events");
	newListBox->m_EnableEvents = enableEvents;

	// Store listbox item
	DirPath up, down;
	up = ButtonPath("Up");
	down = ButtonPath("Down");
	string ups = up.c_str();
	string downs = down.c_str();

	// if showing headers and allowing header clicks to sort list, offset scrollbar and scroll up button
	int header_offset = (ShowHeaders && HeaderSort) ? 21 : 0;

	g_LogFile.write("getting scrollbar ID");
	newListBox->m_ScrollDragID = m_ScrollBars.size();
	g_LogFile.write("adding scrollbar");
	AddScrollBar(newListBox->m_ScrollDragID, x + width - 16, y + header_offset + 1, 16, height - header_offset - 2, newListBox->m_NumDrawnElements);
	newListBox->m_ScrollBar = m_ScrollBars[newListBox->m_ScrollDragID];
	m_ScrollBars[newListBox->m_ScrollDragID]->ParentPosition = &newListBox->m_ScrollChange;
	g_LogFile.write("pushing listbox onto stack");
	m_ListBoxes.push_back(newListBox);
}

int cInterfaceWindow::GetListBoxSize(int ID)
{
	return m_ListBoxes[ID]->GetSize();
}

void cInterfaceWindow::ScrollListBoxDown(int ID)
{
	m_ListBoxes[ID]->ScrollDown();
}

void cInterfaceWindow::ScrollListBoxUp(int ID)
{
	m_ListBoxes[ID]->ScrollUp();
}

void cInterfaceWindow::SetSelectedItemText(int listBoxID, int itemID, string data)
{
	m_ListBoxes[listBoxID]->SetElementText(itemID, data);
}

void cInterfaceWindow::AddToListBox(int listBoxID, int dataID, string data, int color)
{
	string datarray[] = { data };
	AddToListBox(listBoxID, dataID, datarray, 1, color);
}

void cInterfaceWindow::SetSelectedItemText(int listBoxID, int itemID, string data[], int columns)
{
	m_ListBoxes[listBoxID]->SetElementText(itemID, data, columns);
}

void cInterfaceWindow::SetSelectedItemColumnText(int listBoxID, int itemID, string data, int column)
{
	m_ListBoxes[listBoxID]->SetElementColumnText(itemID, data, column);
}

void cInterfaceWindow::SetSelectedItemTextColor(int listBoxID, int itemID, SDL_Color* text_color)
{
	m_ListBoxes[listBoxID]->SetElementTextColor(itemID, text_color);
}

void cInterfaceWindow::FillSortedIDList(int listBoxID, vector<int> *id_vec, int *vec_pos)
{
	m_ListBoxes[listBoxID]->GetSortedIDList(id_vec, vec_pos);
}

void cInterfaceWindow::AddToListBox(int listBoxID, int dataID, string data[], int columns, int color)
{
	m_ListBoxes[listBoxID]->AddElement(dataID, data, columns, color);
	// update "item total" reference for scroll bar
	m_ScrollBars[m_ListBoxes[listBoxID]->m_ScrollDragID]->m_ItemsTotal = m_ListBoxes[listBoxID]->m_NumElements;
}

void cInterfaceWindow::SortColumns(int listBoxID, string column_name[], int columns)
{
	m_ListBoxes[listBoxID]->SetColumnSort(column_name, columns);
}

void cInterfaceWindow::DefineColumns(int listBoxID, string name[], string header[], int offset[], bool skip[], int columns)
{
	m_ListBoxes[listBoxID]->DefineColumns(name, header, offset, skip, columns);
}

void cInterfaceWindow::SortListItems(int listBoxID, string column_name, bool Desc)
{
	m_ListBoxes[listBoxID]->SortByColumn(column_name, Desc);
}

string cInterfaceWindow::HeaderClicked(int listBoxID)
{
	string clicked = m_ListBoxes[listBoxID]->m_HeaderClicked;
	m_ListBoxes[listBoxID]->m_HeaderClicked = "";
	return clicked;
}

int cInterfaceWindow::GetNextSelectedItemFromList(int listBoxID, int from, int& pos)
{
	return m_ListBoxes[listBoxID]->GetNextSelected(from, pos);
}

int cInterfaceWindow::GetLastSelectedItemFromList(int listBoxID)
{
	return m_ListBoxes[listBoxID]->GetLastSelected();
}

int cInterfaceWindow::GetSelectedItemFromList(int listBoxID)
{
	return m_ListBoxes[listBoxID]->GetSelected();
}

string cInterfaceWindow::GetSelectedTextFromList(int listBoxID)
{
	return m_ListBoxes[listBoxID]->GetSelectedText();
}

int cInterfaceWindow::GetAfterSelectedItemFromList(int listBoxID)
{
	return m_ListBoxes[listBoxID]->GetAfterSelected();
}

bool cInterfaceWindow::ListDoubleClicked(int listBoxID)
{
	return m_ListBoxes[listBoxID]->DoubleClicked();
}

void cInterfaceWindow::SetListTopPos(int listBoxID, int pos)
{
	if (pos < 0)	return;
	m_ListBoxes[listBoxID]->m_Position = pos;
	m_ListBoxes[listBoxID]->m_ScrollBar->SetTopValue(pos);
}

void cInterfaceWindow::SetSelectedItemInList(int listBoxID, int itemID, bool ev, bool DeselectOthers)
{
	if (itemID == -1)	return;
	m_ListBoxes[listBoxID]->SetSelected(itemID, ev, DeselectOthers);
}

int cInterfaceWindow::ArrowDownListBox(int ID)
{
	if (m_ListBoxes.empty())		return -1;
	return m_ListBoxes[ID]->ArrowDownList();
}

bool cInterfaceWindow::IsMultiSelected(int ID)
{
	if (m_ListBoxes.empty())		return 0;
	return m_ListBoxes[ID]->HasMultiSelected();
}

int cInterfaceWindow::ArrowUpListBox(int ID)
{
	if (m_ListBoxes.empty())		return -1;
	return m_ListBoxes[ID]->ArrowUpList();
}

void cInterfaceWindow::ClearListBox(int ID)
{
	if (m_ListBoxes.empty())		return;
	m_ListBoxes[ID]->ClearList();
	// update "item total" reference for scroll bar
	m_ScrollBars[m_ListBoxes[ID]->m_ScrollDragID]->m_ItemsTotal = 0;
	m_ScrollBars[m_ListBoxes[ID]->m_ScrollDragID]->SetTopValue(0);
}

void cInterfaceWindow::SetListBoxPosition(int ID, int pos)
{
	if (m_ListBoxes.empty())		return;
	m_ListBoxes[ID]->m_Position = pos;
	m_ScrollBars[m_ListBoxes[ID]->m_ScrollDragID]->SetTopValue(pos);
}

cInterfaceWindowXML::cInterfaceWindowXML()
{
}

cInterfaceWindowXML::~cInterfaceWindowXML()
{
	g_LogFile.write(m_filename);
}

void cInterfaceWindowXML::load()
{
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
	/*
	*	loop over the elements attached to the root
	*/
	for (el = root_el->FirstChildElement(); el; el = el->NextSiblingElement())
	{
		string tag = el->ValueStr();
		if (tag == "Define") { define_widget(el);				continue; }
		if (tag == "Widget") { place_widget(el);				continue; }
		if (tag == "Window") { read_window_definition(el);		continue; }
		if (tag == "EditBox") { read_editbox_definition(el);	continue; }
		if (tag == "Text") { read_text_item(el);				continue; }
		if (tag == "Button") { read_button_definition(el);		continue; }
		if (tag == "Image") { read_image_definition(el);		continue; }
		if (tag == "ListBox") { read_listbox_definition(el);	continue; }
		if (tag == "Checkbox"){ read_checkbox_definition(el);	continue; }
		if (tag == "Slider") { read_slider_definition(el);		continue; }

		g_LogFile.ss() << "Error: unexpected tag in '" << m_filename << "': '" << tag << "' ...";
		g_LogFile.ssend();
	}
}

void cInterfaceWindowXML::read_text_item(TiXmlElement *el)
{
	string name, text;
	XmlUtil xu(m_filename);
	int id, x, y, w, h, fontsize, red = 0, green = 0, blue = 0;
	bool auto_scrollbar = true, force_scrollbar = false, leftorright = false;

	xu.get_att(el, "Name", name);
	xu.get_att(el, "Text", text);
	xu.get_att(el, "XPos", x);
	xu.get_att(el, "YPos", y);
	xu.get_att(el, "Width", w);
	xu.get_att(el, "Height", h);
	xu.get_att(el, "FontSize", fontsize); if (fontsize == 0) fontsize = 16;
	xu.get_att(el, "AutoScrollbar", auto_scrollbar);	// automatically use scrollbar if text doesn't fit; otherwise, don't
	xu.get_att(el, "ForceScrollbar", force_scrollbar);	// force scrollbar display even when not needed (shown disabled, grayed out)
	xu.get_att(el, "Red", red);
	xu.get_att(el, "Green", green);
	xu.get_att(el, "Blue", blue);
	xu.get_att(el, "LeftOrRight", leftorright);
	/*
	*	create the text item
	*/
	AddTextItem(id, x, y, w, h, text, fontsize, auto_scrollbar, force_scrollbar, leftorright, red, green, blue);
	/*
	*	make a note of the ID
	*/
	register_id(id, name);
}

void cInterfaceWindowXML::define_widget(TiXmlElement *base_el)
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
	cXmlWidget *widget = new_widget(widget_name);
	/*
	*	this is like reading the file again, in minature
	*/
	for (el = base_el->FirstChildElement(); el; el = el->NextSiblingElement())
	{
		string tag = el->ValueStr();
		l.ss() << "define widget: '" << tag << "'";
		l.ssend();

		if (tag == "Text") { widget_text_item(el, *widget);			continue; }
		if (tag == "Button") { widget_button_item(el, *widget);		continue; }
		if (tag == "Image") { widget_image_item(el, *widget);		continue; }
		if (tag == "EditBox") { widget_editbox_item(el, *widget);	continue; }
		if (tag == "ListBox") { widget_listbox_item(el, *widget);	continue; }
		if (tag == "Checkbox") { widget_checkbox_item(el, *widget);	continue; }
		if (tag == "Widget") { widget_widget(el, *widget);			continue; }
		if (tag == "Slider") { widget_slider_item(el, *widget);		continue; }

		l.ss() << "Warning: Unhandled widget tag: '" << tag << "'";
		l.ssend();
	}
}

void cInterfaceWindowXML::place_widget(TiXmlElement *el, string suffix)
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
	add_widget(name, x, y, seq);
}

void cInterfaceWindowXML::add_widget(string widget_name, int x, int y, string seq)
{
	int id;
	CLog l;
	cXmlWidget *widget = find_widget(widget_name);
	if (widget == 0)
	{
		l.ss() << "Error: can't find definition for widget '" << widget_name << "'";
		l.ssend();
		return;
	}
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
			AddButton(ButtonPath(xw.off), ButtonPath(xw.disabled), ButtonPath(xw.on), id, full_x, full_y, xw.w, xw.h, xw.alpha, xw.scale, xw.cache);
			register_id(id, name);
			HideButton(id, xw.hide);
		}
		else if (tag == "Image") {
			cerr << "adding image: " << xw.file << endl;
			DirPath dp = ImagePath(xw.file);
			AddImage(id, dp, full_x, full_y, xw.w, xw.h, xw.stat, xw.r, xw.g, xw.b);
			register_id(id, name);
			HideImage(id, xw.hide);
		}
		else if (tag == "Slider") {
			AddSlider(id, full_x, full_y, xw.w, xw.r, xw.g, xw.h, xw.b, xw.events);
			register_id(id, name);
			HideSlider(id, xw.hide);
			DisableSlider(id, xw.stat);
		}
		else if (tag == "Text") {
			AddTextItem(id, full_x, full_y, xw.w, xw.h, xw.text, xw.fontsize, xw.alpha, xw.hide, xw.leftorright, xw.r, xw.g, xw.b);
			register_id(id, name);
			HideText(id, xw.hide);
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
			add_widget(xw.name, full_x, full_y, xw.seq + seq);
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
	XmlUtil xu(m_filename);
	string name;
	int id, x, y, w, h, border_size, fontsize = 16;

	xu.get_att(el, "Name", name);
	xu.get_att(el, "XPos", x);
	xu.get_att(el, "YPos", y);
	xu.get_att(el, "Width", w);
	xu.get_att(el, "Height", h);
	xu.get_att(el, "Border", border_size);
	xu.get_att(el, "FontSize", fontsize); if (fontsize == 0) fontsize = 16;

	AddEditBox(id, x, y, w, h, border_size, fontsize);
	register_id(id, name);
}

void cInterfaceWindowXML::read_listbox_definition(TiXmlElement *el)
{
	string name;
	XmlUtil xu(m_filename);
	bool events = true, multi = false, show_headers = false, header_div = true, header_sort = true;
	int id, x, y, w, h, border_size = 1, fontsize = 10, rowheight = 20;

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

	AddListBox(id, x, y, w, h, border_size, events, multi, show_headers, header_div, header_sort, fontsize, rowheight);
	register_id(id, name);

	// Check for column definitions
	CLog l;
	TiXmlElement *sub_el;
	int column_count = 0, column_offset[LISTBOX_COLUMNS];
	string column_name[LISTBOX_COLUMNS], column_header[LISTBOX_COLUMNS];
	bool column_skip[LISTBOX_COLUMNS];
	for (sub_el = el->FirstChildElement(); sub_el; sub_el = sub_el->NextSiblingElement())
	{
		string tag = sub_el->ValueStr();
		l.ss() << "define listbox element: '" << tag << "'";
		l.ssend();
		// XML definition can arrange columns in any order and even leave some columns out if desired
		// Listbox itself (via DefineColumns) keeps track of what order they're to be displayed in based on this
		if (tag == "Column")   // hey, we've got a column definition in this listbox
		{
			column_offset[column_count] = 0;
			column_skip[column_count] = false;
			xu.get_att(sub_el, "Name", column_name[column_count]);  // Careful, Name is how we ID columns
			xu.get_att(sub_el, "Header", column_header[column_count], Optional);  // Text header of column
			xu.get_att(sub_el, "Offset", column_offset[column_count], Optional);  // X Offset for column position
			xu.get_att(sub_el, "Skip", column_skip[column_count], Optional);  // hide column? not really used
			column_count++;
		}
		else
		{
			l.ss() << "Warning: Unhandled listbox element: '" << tag << "'";
			l.ssend();
		}
	}
	// If we have columns defined, go ahead and give the listbox all the gory details
	if (column_count > 0)	DefineColumns(id, column_name, column_header, column_offset, column_skip, column_count);
}


void cInterfaceWindowXML::widget_editbox_item(TiXmlElement *el, cXmlWidget &wid)
{
	sXmlWidgetPart xw;
	XmlUtil xu(m_filename);

	xw.type = "EditBox";
	xu.get_att(el, "Name", xw.name);
	xu.get_att(el, "XPos", xw.x);
	xu.get_att(el, "YPos", xw.y);
	xu.get_att(el, "Width", xw.w);
	xu.get_att(el, "Height", xw.h);
	xu.get_att(el, "FontSize", xw.fontsize); if (xw.fontsize == 0) xw.fontsize = 16;
	xu.get_att(el, "Border", xw.bordersize, Optional);
	xu.get_att(el, "Multi", xw.multi, Optional);
	xu.get_att(el, "Events", xw.events, Optional);
	wid.add(xw);
}

void cInterfaceWindowXML::widget_listbox_item(TiXmlElement *el, cXmlWidget &wid)
{
	sXmlWidgetPart xw;
	XmlUtil xu(m_filename);

	xw.type = "Text";
	xu.get_att(el, "Name", xw.name);
	xu.get_att(el, "XPos", xw.x);
	xu.get_att(el, "YPos", xw.y);
	xu.get_att(el, "Width", xw.w);
	xu.get_att(el, "Height", xw.h);
	xu.get_att(el, "Border", xw.bordersize, Optional);
	xu.get_att(el, "Multi", xw.multi, Optional);
	xu.get_att(el, "Events", xw.events, Optional);
	wid.add(xw);
}

void cInterfaceWindowXML::read_checkbox_definition(TiXmlElement *el)
{
	string name, text;
	XmlUtil xu(m_filename);
	int id, x, y, w, h, fontsize;
	bool leftorright;

	xu.get_att(el, "Name", name);
	xu.get_att(el, "Text", text);
	xu.get_att(el, "XPos", x);
	xu.get_att(el, "YPos", y);
	xu.get_att(el, "Width", w);
	xu.get_att(el, "Height", h);
	xu.get_att(el, "FontSize", fontsize); if (fontsize == 0) fontsize = 16;
	xu.get_att(el, "LeftOrRight", leftorright);

	AddCheckbox(id, x, y, w, h, text, fontsize, leftorright);

	register_id(id, name);
}

void cInterfaceWindowXML::widget_checkbox_item(TiXmlElement *el, cXmlWidget &wid)
{
	sXmlWidgetPart xw;
	xw.type = "Checkbox";
	xw.hide = false;
	XmlUtil xu(m_filename);

	xu.get_att(el, "Name", xw.name);
	xu.get_att(el, "Text", xw.text);
	xu.get_att(el, "XPos", xw.x);
	xu.get_att(el, "YPos", xw.y);
	xu.get_att(el, "Width", xw.w);
	xu.get_att(el, "Height", xw.h);
	xu.get_att(el, "FontSize", xw.fontsize);
	xu.get_att(el, "LeftOrRight", xw.leftorright);
	wid.add(xw);
}

void cInterfaceWindowXML::widget_widget(TiXmlElement *el, cXmlWidget &wid)
{
	sXmlWidgetPart xw;
	XmlUtil xu(m_filename);

	xw.type = "Widget";
	xu.get_att(el, "Definition", xw.name);
	xu.get_att(el, "XPos", xw.x);
	xu.get_att(el, "YPos", xw.y);
	xu.get_att(el, "Seq", xw.seq);
	xu.get_att(el, "cache", xw.cache, true);
	wid.add(xw);
}

void cInterfaceWindowXML::read_image_definition(TiXmlElement *el)
{
	XmlUtil xu(m_filename);
	bool stat = false, hide = false;
	int id, x, y, w, h;
	int r = 0, g = 0, b = 0;
	string name, file = "blank.png";

	xu.get_att(el, "Name", name);
	xu.get_att(el, "File", file, Optional);
	xu.get_att(el, "XPos", x);
	xu.get_att(el, "YPos", y);
	xu.get_att(el, "Width", w);
	xu.get_att(el, "Height", h);
	xu.get_att(el, "Stat", stat, Optional);
	xu.get_att(el, "Red", r, Optional);
	xu.get_att(el, "Green", g, Optional);
	xu.get_att(el, "Blue", b, Optional);
	xu.get_att(el, "Hidden", hide, Optional);

	DirPath dp = ImagePath(file);

	AddImage(id, dp, x, y, w, h, stat, r, g, b);
	HideImage(id, hide);
	register_id(id, name);
}

void cInterfaceWindowXML::read_button_definition(TiXmlElement *el)
{
	int id, x, y, w, h;
	XmlUtil xu(m_filename);
	string scale, alpha, name, on, off, disabled, base = "";
	/*
	*	get the button name - we'll use this to match up
	*	interface IDs
	*/
	xu.get_att(el, "Name", name);
	/*
	*	"Image" is the base string for the button.
	*	If we have "BackOn.png" "BackOff.png" and
	*	"BackDisabled.png"
	*
	*	then the base string would be "Back"
	*/
	xu.get_att(el, "Image", base, Optional);
	/*
	*	if we have a base value, use it to construct the
	*	three image names
	*/
	if (base != "") {
		on = base + "On.png";
		off = base + "Off.png";
		disabled = base + "Disabled.png";
	}
	/*
	*	On, Off and Disabled override the base derived
	*	value. You don't need base at all - just specify
	*	all three buttons directly.
	*/
	xu.get_att(el, "On", on, Optional);
	xu.get_att(el, "Off", off, Optional);
	xu.get_att(el, "Disabled", disabled, Optional);
	/*
	*	xywh
	*/
	xu.get_att(el, "XPos", x);
	xu.get_att(el, "YPos", y);
	xu.get_att(el, "Width", w);
	xu.get_att(el, "Height", h);
	/*
	*	I assume these are bools for "use alpha channel"
	*	and "scale with window"
	*/
	xu.get_att(el, "Transparency", alpha);
	xu.get_att(el, "Scale", scale);
	/*
	*	and finally...
	*/
	AddButton(ButtonPath(off), ButtonPath(disabled), ButtonPath(on), id, x, y, w, h, alpha == "true", scale == "true");
	register_id(id, name);
}

void cInterfaceWindowXML::read_slider_definition(TiXmlElement *el)
{
	XmlUtil xu(m_filename);
	bool disable = false, hide = false, liveUpdate = true;
	int id, x, y, w;
	int min = 0, max = 100, value = 0, increment = 5;
	string name;

	xu.get_att(el, "Name", name);
	xu.get_att(el, "XPos", x);
	xu.get_att(el, "YPos", y);
	xu.get_att(el, "Width", w);
	xu.get_att(el, "Increment", increment, Optional);
	xu.get_att(el, "MinValue", min, Optional);
	xu.get_att(el, "MaxValue", max, Optional);
	xu.get_att(el, "Value", value, Optional);
	xu.get_att(el, "Hidden", hide, Optional);
	xu.get_att(el, "Disabled", disable, Optional);
	xu.get_att(el, "LiveUpdate", liveUpdate, Optional);

	// for some reason, optional ints above are unfortunately being set to 0 when not present
	if (min == 0 && max == 0)	max = 100;
	if (value < min)			value = min;
	if (value > max)			value = max;
	if (increment == 0)			increment = 5;

	AddSlider(id, x, y, w, min, max, increment, value, liveUpdate);
	DisableSlider(id, disable);
	HideSlider(id, hide);
	register_id(id, name);
}

void cInterfaceWindowXML::widget_slider_item(TiXmlElement *el, cXmlWidget &wid)
{
	sXmlWidgetPart xw;
	XmlUtil xu(m_filename);
	xw.type = "Slider";
	xw.hide = xw.stat = false;
	xw.events = true;
	xw.r = 0; xw.g = 100; xw.b = 0; xw.h = 5;

	xu.get_att(el, "Name", xw.name);
	xu.get_att(el, "XPos", xw.x);
	xu.get_att(el, "YPos", xw.y);
	xu.get_att(el, "Width", xw.w);
	xu.get_att(el, "Increment", xw.h, Optional);
	xu.get_att(el, "MinValue", xw.r, Optional);
	xu.get_att(el, "MaxValue", xw.g, Optional);
	xu.get_att(el, "Value", xw.b, Optional);
	xu.get_att(el, "Hidden", xw.hide, Optional);
	xu.get_att(el, "Disabled", xw.stat, Optional);
	xu.get_att(el, "LiveUpdate", xw.events, Optional);

	// for some reason, optional ints above are unfortunately being set to 0 when not present; correct for that
	if (xw.r == 0 && xw.g == 0)	xw.g = 100;
	if (xw.b < xw.r)			xw.b = xw.r;
	if (xw.b > xw.g)			xw.b = xw.g;
	if (xw.h == 0)				xw.h = 5;
	wid.add(xw);
}

void cInterfaceWindowXML::widget_text_item(TiXmlElement *el, cXmlWidget &wid)
{
	sXmlWidgetPart xw;
	xw.type = "Text";
	xw.hide = false;
	XmlUtil xu(m_filename);

	xu.get_att(el, "Name", xw.name);
	xu.get_att(el, "Text", xw.text);
	xu.get_att(el, "XPos", xw.x);
	xu.get_att(el, "YPos", xw.y);
	xu.get_att(el, "Width", xw.w);
	xu.get_att(el, "Height", xw.h);
	xu.get_att(el, "FontSize", xw.fontsize);
	xu.get_att(el, "Hidden", xw.hide, Optional);
	xu.get_att(el, "AutoScrollbar", xw.alpha, Optional);
	xu.get_att(el, "ForceScrollbar", xw.hide, Optional);
	xu.get_att(el, "Red", xw.r, Optional);
	xu.get_att(el, "Green", xw.g, Optional);
	xu.get_att(el, "Blue", xw.b, Optional);
	xu.get_att(el, "LeftOrRight", xw.leftorright, Optional);


	wid.add(xw);
}

void cInterfaceWindowXML::widget_button_item(TiXmlElement *el, cXmlWidget &wid)
{
	sXmlWidgetPart xw;
	XmlUtil xu(m_filename);

	xw.type = "Button";
	xw.hide = false;
	xw.base = "";

	xu.get_att(el, "Name", xw.name);
	xu.get_att(el, "Image", xw.base, Optional);
	if (xw.base != "") {
		xw.on = xw.base + "On.png";
		xw.off = xw.base + "Off.png";
		xw.disabled = xw.base + "Disabled.png";
	}
	xu.get_att(el, "On", xw.on, Optional);
	xu.get_att(el, "Off", xw.off, Optional);
	xu.get_att(el, "Disabled", xw.disabled, Optional);
	xu.get_att(el, "XPos", xw.x);
	xu.get_att(el, "YPos", xw.y);
	xu.get_att(el, "Width", xw.w);
	xu.get_att(el, "Height", xw.h);
	xu.get_att(el, "Transparency", xw.alpha);
	xu.get_att(el, "Scale", xw.scale);
	xu.get_att(el, "Hidden", xw.hide, Optional);
	xu.get_att(el, "cache", xw.cache, Optional);
	wid.add(xw);
}

void cInterfaceWindowXML::widget_image_item(TiXmlElement *el, cXmlWidget &wid)
{
	sXmlWidgetPart xw;
	XmlUtil xu(m_filename);

	xw.type = "Image";
	xw.hide = false;
	xw.stat = false;
	xw.r = xw.g = xw.b = 0;

	xu.get_att(el, "Name", xw.name);
	xu.get_att(el, "File", xw.file);
	xu.get_att(el, "XPos", xw.x);
	xu.get_att(el, "YPos", xw.y);
	xu.get_att(el, "Width", xw.w);
	xu.get_att(el, "Height", xw.h);
	xu.get_att(el, "Red", xw.r, Optional);
	xu.get_att(el, "Green", xw.g, Optional);
	xu.get_att(el, "Blue", xw.b, Optional);
	xu.get_att(el, "Stat", xw.stat, Optional);
	xu.get_att(el, "Hidden", xw.hide, Optional);
	wid.add(xw);
}

void cInterfaceWindowXML::register_id(int id, string name)
{
	CLog l;
	l.ss() << "registering ID " << id << " to name '" << name << "'";
	l.ssend();
	name_to_id[name] = id;
	id_to_name[id] = name;
}

int cInterfaceWindowXML::get_id(string a, string b, string c, string d, string e)
{
	if (name_to_id.find(a) != name_to_id.end())	return name_to_id[a];
	if (name_to_id.find(b) != name_to_id.end())	return name_to_id[b];
	if (name_to_id.find(c) != name_to_id.end())	return name_to_id[c];
	if (name_to_id.find(d) != name_to_id.end())	return name_to_id[d];
	if (name_to_id.find(e) != name_to_id.end())	return name_to_id[e];

	bool un = false;
	if (a == "*Unused*" || a == "*Optional*")	{ un=true; a=""; }
	if (b == "*Unused*" || b == "*Optional*")	{ un=true; b=""; }
	if (c == "*Unused*" || c == "*Optional*")	{ un=true; c=""; }
	if (d == "*Unused*" || d == "*Optional*")	{ un=true; d=""; }
	if (e == "*Unused*" || e == "*Optional*")	{ un=true; e=""; }

	if (!un || cfg.debug.log_debug()) { g_LogFile.ss() << "Error setting ids for interface " << (un ? " but it is not used or optional " : "") << "- acceptable names for this item are : '" << a << "', '" << b << "', '" << c << "', '" << d << "', '" << e << "'.";	g_LogFile.ssend(); }
	return -1;
}

cXmlWidget* cInterfaceWindowXML::new_widget(string name)
{
	cXmlWidget *wid = new cXmlWidget();
	widgets[name] = wid;
	return wid;
}

cXmlWidget* cInterfaceWindowXML::find_widget(string name)
{
	map<string, cXmlWidget*>::iterator it;
	it = widgets.find(name);
	if (it == widgets.end())	return 0;
	return it->second;
}
