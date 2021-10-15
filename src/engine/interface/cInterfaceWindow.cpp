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

#include "interface/cInterfaceWindow.h"
#include "interface/cWindowManager.h"
#include "utils/DirPath.h"
#include "CLog.h"
#include <tinyxml2.h>
#include "xml/util.h"

#include "widgets/cListBox.h"
#include "widgets/cButton.h"
#include "widgets/cCheckBox.h"
#include "widgets/cSlider.h"
#include "widgets/cEditBox.h"
#include "widgets/cTextItem.h"
#include "widgets/cImageItem.h"
#include "widgets/cScrollBar.h"
#include "interface/CGraphics.h"
#include "interface/sColor.h"
#include <algorithm>
#include <cassert>
#include "interface/cTheme.h"
#include "widgets/theme_ids.h"
using namespace widgets_theme;


cInterfaceWindow::~cInterfaceWindow() = default;

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
    /// If we are in fuzz-testing mode, don't do the try/catch, because we
    /// want the debugger to trigger in the original callstack, so we can
    /// pinpoint the problem
    #ifndef FUZZ_TESTING
    try {
    #endif
        for(auto& widget : m_Widgets)
            if(widget->OnMouseClick(x, y, down)) return;
        // nobody wanted to handle this click -> remove focus
        SetFocusTo(nullptr);
    #ifndef FUZZ_TESTING
    } catch (std::exception& exception) {
        g_LogFile.error("interface", "Error when handling MouseClick event at (", x, ", ", y,
                "): ", exception.what());
        push_error(exception.what());
    }
    #endif
}

void cInterfaceWindow::MouseWheel(int x, int y, bool down)
{
    try {
        for(auto& widget : m_Widgets)
            if(widget->OnMouseWheel(down, x, y, false)) return;

        // if nothing handles the mouse wheel, try the keyboard focused widget
        if(m_KeyboardFocusWidget && m_KeyboardFocusWidget->OnMouseWheel(down, x, y, true)) return;

    } catch (std::exception& exception) {
        g_LogFile.error("interface", "Error when handling MouseWheel event at (", x, ", ", y,
                "): ", exception.what());
        push_error(exception.what());
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
        m_Border.DrawSurface(m_XPos, m_YPos);
        m_Background.DrawSurface(m_XPos + m_BorderSize, m_YPos + m_BorderSize);
    }

    for (auto& widget : m_Widgets) widget->Draw(gfx);
}

void cInterfaceWindow::AddButton(const std::string& image, int& ID, int x, int y, int width, int height)
{
    std::string on = image + "On.png";
    std::string off = image + "Off.png";
    std::string disabled = image + "Disabled.png";
    ID = AddButton(off, disabled, on, x, y, width, height);
}

int
cInterfaceWindow::AddButton(const std::string& OffImage, const std::string& DisabledImage, const std::string& OnImage, int x, int y,
                            int width, int height)
{
    int ID = m_Widgets.size();

    // create button
    auto newButton = std::make_unique<cButton>(this, OffImage, DisabledImage, OnImage, ID, x + m_XPos, y + m_YPos,
            width, height);

    // Store button
    m_Widgets.push_back(std::move(newButton));
    return ID;
}

cScrollBar* cInterfaceWindow::AddScrollBar(int x, int y, int width, int height, int visibleitems)
{
    int ID = m_Widgets.size();
    auto newScrollBar = std::make_unique<cScrollBar>(this, ID, x + m_XPos, y + m_YPos, width, height, visibleitems);
    newScrollBar->m_ScrollAmount = window_manager().GetTheme().list_scroll();
    m_Widgets.push_back(std::move(newScrollBar));
    return (cScrollBar*)m_Widgets.back().get();
}

cUIWidget* cInterfaceWindow::GetWidget(int id)
{
    if (id == -1 || id >= m_Widgets.size()) return nullptr;
    return m_Widgets[id].get();
}

cButton* cInterfaceWindow::GetButton(int id)
{
    if (id == -1) return nullptr;
    return dynamic_cast<cButton*>(m_Widgets.at(id).get());
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

int cInterfaceWindow::AddImage(const std::string& dir, const std::string& filename, int x, int y, int width, int height, int min_width, int min_height)
{
    // create image
    int id = m_Widgets.size();
    auto newImage = std::make_unique<cImageItem>(this, id, x + m_XPos, y + m_YPos, width, height,
                                                 min_width, min_height);
    newImage->SetThemeImage(dir, filename);

    // Store button
    m_Widgets.push_back(std::move(newImage));
    return id;
}

cImageItem* cInterfaceWindow::GetImage(int id)
{
    if (id == -1) return nullptr;
    return dynamic_cast<cImageItem*>(m_Widgets[id].get());
}


void cInterfaceWindow::SetImage(int id, std::string image)
{
    if(id < 0) return;
    cImageItem * item = GetImage(id);
    item->SetImage(std::move(image));
}

void cInterfaceWindow::SetImage(int id, const std::string& dir, const std::string& image) {
    if(id < 0) return;
    cImageItem * item = GetImage(id);
    item->SetThemeImage(dir, image);
}


void cInterfaceWindow::AddEditBox(int & ID, int x, int y, int width, int height, int BorderSize, int FontSize)
{
    width = (int)((float)width);
    height = (int)((float)height);
    x = (int)((float)x);
    y = (int)((float)y);

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
    if (id > -1)
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
    m_BorderSize = BorderSize;
    SetPosition(x, y, width, height);
    m_Border = GetGraphics().CreateSurface(width, height,
                                           window_manager().GetTheme().get_color(WindowBorderColor, {0, 0, 0}));
    m_Background = GetGraphics().CreateSurface(width - (BorderSize * 2), height - (BorderSize * 2),
                                               window_manager().GetTheme().get_color(WindowBackgroundColor, {140, 191, 228}));
}

std::string cInterfaceWindow::GetEditBoxText(int ID)
{
    return GetEditBox(ID)->GetText();
}

void cInterfaceWindow::SetEditBoxText(int ID, std::string text)
{
    GetEditBox(ID)->SetText(std::move(text));
}

cEditBox* cInterfaceWindow::GetEditBox(int id)
{
    if (id == -1) return nullptr;
    return dynamic_cast<cEditBox*>(m_Widgets[id].get());
}

void cInterfaceWindow::Focused()
{
    // clear edit boxes and set the first one as focused
    // for (auto & m_EditBoxe : m_EditBoxes)        m_EditBoxe->ClearText();
    TabFocus();
}

bool cInterfaceWindow::SetFocusTo(cUIWidget* widget)
{
    // three scenarios: 1) widget is nullptr -- we reset the focus
    // 2) widget cannot be focused
    // 3) widget can be focused
    if(widget != m_KeyboardFocusWidget) {
        if(widget)
            if(!widget->SetFocus(true)) {
                // cannot be focused. In that case, nothing happens
                return false;
            }

        if(m_KeyboardFocusWidget)
            m_KeyboardFocusWidget->SetFocus(false);

        m_KeyboardFocusWidget = widget;
        return true;
    }
    return true;
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
    width = (int)((float)width);
    float height = 1.f;
    x = (int)((float)x);
    y = (int)((float)y);

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


void cInterfaceWindow::AddCheckbox(int & ID, int x, int y, int width, int height, std::string text, int size, bool leftorright)
{
    width = (int)((float)width);
    height = (int)((float)height);
    x = (int)((float)x);
    y = (int)((float)y);

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

void cInterfaceWindow::AddTextItem(int & ID, int x, int y, int width, int height, std::string text, int size,
    bool force_scrollbar, int red, int green, int blue)
{
    width = (int)((float)width);
    height = (int)((float)height);
    x = (int)((float)x);
    y = (int)((float)y);

    // create text item
    ID = m_Widgets.size();
    auto newTextItem = std::make_unique<cTextItem>(this, ID, x + m_XPos, y + m_YPos, width, height, text, size,
            force_scrollbar, red, green, blue);
    // Store text item
    m_Widgets.push_back(std::move(newTextItem));
}

void cInterfaceWindow::EditTextItem(std::string text, int ID, bool as_table)
{
    if (ID == -1) return;

    cTextItem* item = GetTextItem(ID);
    item->SetText(std::move(text), as_table);
}

cTextItem* cInterfaceWindow::GetTextItem(int id)
{
    if (id == -1) return nullptr;
    return dynamic_cast<cTextItem*>(m_Widgets[id].get());
}


cListBox* cInterfaceWindow::AddListBox(int x, int y, int width, int height, int BorderSize, bool enableEvents, bool MultiSelect, bool ShowHeaders, bool HeaderDiv, bool HeaderSort, int fontsize, int rowheight)
{
    g_LogFile.debug("interface", "Adding listbox...");
    // create listbox item
    int ID = m_Widgets.size();
    // need to ensure correct ordering of the list box and the scroll bar
    m_Widgets.push_back(nullptr);
    g_LogFile.debug("interface", "initializing listbox");
    auto newListBox = std::make_unique<cListBox>(this, ID, x + m_XPos, y + m_YPos, width, height, BorderSize, MultiSelect,
            enableEvents, ShowHeaders, HeaderDiv, HeaderSort, fontsize, rowheight);
    cListBox* lb = newListBox.get();
    m_Widgets[ID] = std::move(newListBox);
    return lb;
}

int cInterfaceWindow::GetListBoxSize(int ID)
{
    return GetListBox(ID)->NumItems();
}

void cInterfaceWindow::SetSelectedItemText(int listBoxID, int itemID, std::string data)
{
    GetCListBox(listBoxID)->SetElementText(itemID, data);
}

void cInterfaceWindow::AddToListBox(int listBoxID, int dataID, std::string text, int color)
{
    CellData value = text;
    AddToListBox(listBoxID, dataID,
                 std::vector<FormattedCellData>{{std::move(value), std::move(text)}},
                 color);
}

void cInterfaceWindow::AddToListBox(int listBoxID, int dataID, CellData value, std::string formatted, int color)
{
    AddToListBox(listBoxID, dataID, std::vector<FormattedCellData>{{std::move(value), std::move(formatted)}}, color);
}

void cInterfaceWindow::SetSelectedItemColumnText(int listBoxID, int itemID, std::string data, const std::string& column)
{
    GetCListBox(listBoxID)->SetElementColumnText(itemID, std::move(data), column);
}

void cInterfaceWindow::SetSelectedItemTextColor(int listBoxID, int itemID, const sColor& color)
{
    GetCListBox(listBoxID)->SetElementTextColor(itemID, color);
}

void cInterfaceWindow::AddToListBox(int listBoxID, int dataID, std::vector<FormattedCellData> data, int color)
{
    if(listBoxID < 0) {
        g_LogFile.error("interface", "Trying to access invalid ListBox");
        return;
    }
    GetCListBox(listBoxID)->AddElement(dataID, std::move(data), color);
}

void cInterfaceWindow::SortColumns(int listBoxID, const std::vector<std::string>& column_name)
{
    GetCListBox(listBoxID)->SetColumnSort(column_name);
}

void cInterfaceWindow::SortListItems(int listBoxID, std::string column_name, bool Desc)
{
    GetCListBox(listBoxID)->SortByColumn(column_name, Desc);
}

int cInterfaceWindow::GetLastSelectedItemFromList(int listBoxID)
{
    return GetCListBox(listBoxID)->GetLastSelected();
}

int cInterfaceWindow::GetSelectedItemFromList(int listBoxID)
{
    return GetListBox(listBoxID)->GetSelectedID();
}

std::string cInterfaceWindow::GetSelectedTextFromList(int listBoxID)
{
    return GetCListBox(listBoxID)->GetSelectedText();
}

void cInterfaceWindow::SetSelectedItemInList(int listBoxID, int itemID, bool ev, bool DeselectOthers)
{
    if (itemID == -1 || listBoxID == -1)    return;
    GetCListBox(listBoxID)->SetSelectedID(itemID, ev, DeselectOthers);
}

int cInterfaceWindow::ArrowDownListBox(int ID)
{
    if (ID == -1)        return -1;
    return GetCListBox(ID)->ArrowDownList();
}

bool cInterfaceWindow::IsMultiSelected(int ID)
{
    if (ID == -1)        return false;
    return GetCListBox(ID)->HasMultiSelected();
}

int cInterfaceWindow::ArrowUpListBox(int ID)
{
    if (ID == -1)        return -1;
    return GetCListBox(ID)->ArrowUpList();
}

void cInterfaceWindow::ClearListBox(int ID)
{
    GetListBox(ID)->Clear();
}

IListBox* cInterfaceWindow::GetListBox(int id)
{
    if (id == -1) return nullptr;
    auto* elem = m_Widgets[id].get();
    return dynamic_cast<IListBox*>(elem);
}

cListBox* cInterfaceWindow::GetCListBox(int id)
{
    if (id == -1) return nullptr;
    auto* elem = m_Widgets[id].get();
    return dynamic_cast<cListBox*>(elem);
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

void cInterfaceWindow::pop_window() const
{
    window_manager().Pop();
}

void cInterfaceWindow::update()
{
    process();
}

void cInterfaceWindow::OnKeyPress(SDL_Keysym keysym)
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
        g_LogFile.error("interface", "Error when handling KeyPress event of key ", keysym.sym,
                ": ", exception.what());
        push_error(exception.what());
    }
}

void cInterfaceWindow::TextInput(const char* ip) {
    if(m_KeyboardFocusWidget) {
        m_KeyboardFocusWidget->OnTextInput(ip);
    }
}

void cInterfaceWindow::pop_to_window(const std::string& target) const
{
    window_manager().PopToWindow(target);
}

void cInterfaceWindow::input_integer(std::function<void(int)> callback, int def_value)
{
    window_manager().InputInteger(std::move(callback), def_value);
}

void cInterfaceWindow::input_confirm(std::function<void()> callback)
{
    window_manager().InputConfirm(std::move(callback));
}

void cInterfaceWindow::input_string(std::function<void(const std::string&)> callback, std::string def_value, std::string prompt)
{
    window_manager().InputString(std::move(callback), std::move(def_value), std::move(prompt));
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

void cInterfaceWindow::SetButtonHotKey(int id, SDL_Keycode key)
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

void cInterfaceWindow::SetSliderHotKeys(int id, SDL_Keycode increase, SDL_Keycode decrease) {
    if(id > -1) {
        GetSlider(id)->SetHotKeys(increase, decrease);
    }
}

void cInterfaceWindow::SetListBoxHotKeys(int id, SDL_Keycode up, SDL_Keycode down)
{
    if(id > -1) {
        GetListBox(id)->SetArrowHotKeys(up, down);
    }
}

void cInterfaceWindow::push_message(std::string text, int color)
{
    window_manager().PushMessage(std::move(text), color);
}

void cInterfaceWindow::push_error(std::string text) {
    window_manager().PushError(std::move(text));
}

void cInterfaceWindow::TabFocus()
{
    if(m_Widgets.empty())
        return;

    // find focused element
    bool found  = false;
    for(auto& widget : m_Widgets) {
        if(found) {
            if( SetFocusTo(widget.get())) {
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
        if (SetFocusTo(widget.get())) {
            return;
        }
    }
    // There really is none!
}

void cInterfaceWindow::AddKeyCallback(SDL_Keycode key, std::function<void()> callback)
{
    m_KeyCallbacks.emplace(key, std::move(callback));
}

void cInterfaceWindow::AddWidget(std::unique_ptr<cUIWidget> wdg) {
    m_Widgets.push_back(std::move(wdg));
}

CGraphics& cInterfaceWindow::GetGraphics() {
    return window_manager().GetGraphics();
}

cInterfaceWindow::cInterfaceWindow(std::string name) : m_Name(name) {

}

bool cInterfaceWindow::is_ctrl_held() const {
    return window_manager().IsCtrlHeld();
}

std::size_t cInterfaceWindow::NumWidgets() const {
    return m_Widgets.size();
}

void cInterfaceWindow::ForAllSelectedItems(int id, std::function<void(int)> handler) {
    GetListBox(id)->HandleSelectedIndices(std::move(handler));
}

void cModalWindow::process() {
    if(!m_Widgets.empty()) {
        if(m_Widgets.front()->IsDisabled()) {
            m_Widgets.erase(m_Widgets.begin());
        }
    } else {
        pop_window();
    }
}

void cModalWindow::Draw(const CGraphics& gfx)
{
    if(!m_Widgets.empty())
        m_Widgets.front()->Draw(gfx);
}

cModalWindow::cModalWindow() : cInterfaceWindow("ModalWindow") {

}
