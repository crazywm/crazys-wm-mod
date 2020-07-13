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
#include <algorithm>
#include <SDL_timer.h>              // needed to detect double clicks
#include "utils/DirPath.h"
#include "widgets/cListBox.h"
#include "interface/CGraphics.h"
#include "interface/cColor.h"
#include "sConfig.h"
#include "cScrollBar.h"
#include "interface/cWindowManager.h"

extern cConfig cfg;

extern sColor g_ListBoxBorderColor;
extern sColor g_ListBoxBackgroundColor;
extern sColor g_ListBoxElementBackgroundColor[5];
extern sColor g_ListBoxSelectedElementColor[5];
extern sColor g_ListBoxElementBorderColor;
extern sColor g_ListBoxElementBorderHColor;
extern sColor g_ListBoxTextColor;
extern sColor g_ListBoxHeaderBackgroundColor;
extern sColor g_ListBoxHeaderBorderColor;
extern sColor g_ListBoxHeaderBorderHColor;
extern sColor g_ListBoxHeaderTextColor;

cListBox::cListBox(cInterfaceWindow* parent, int ID, int x, int y, int width, int height, int BorderSize, bool MultiSelect,
        bool ShowHeaders, bool HeaderDiv, bool HeaderSort, int fontsize, int rowheight):
    cUIWidget(ID, x, y, width, height, parent),
    m_ShowHeaders(ShowHeaders), m_HeaderDividers(HeaderDiv), m_HeaderClicksSort(HeaderSort), m_BorderSize(BorderSize), m_Font(&GetGraphics())
{
    m_FontSize = fontsize == 0 ? 10 : fontsize;
    m_Font.LoadFont(cfg.fonts.normal(), m_FontSize);
    m_Font.SetColor(g_ListBoxTextColor.r, g_ListBoxTextColor.g, g_ListBoxTextColor.b);

    m_LastSelected = m_Items.end();

    DefineColumns(std::vector<std::string>(1), std::vector<std::string>(1), std::vector<int>(1), std::vector<bool>(1));
    SDL_Rect dest_rect;

    m_RowHeight = (rowheight == 0 ? LISTBOX_ITEMHEIGHT : rowheight);

    m_Border = GetGraphics().CreateSurface(width, height, g_ListBoxBorderColor);
    m_Background = GetGraphics().CreateSurface(width - (BorderSize * 2) - 16, height - (BorderSize * 2), g_ListBoxBackgroundColor);

    m_NumDrawnElements = height / m_RowHeight;
    if (m_ShowHeaders) // Account for headers if shown
        m_NumDrawnElements--;
    m_eWidth = (width - (BorderSize * 2));
    m_eHeight = m_RowHeight;

    for(int i = 0; i < 5; ++i) {
        m_ElementBackgrounds.push_back(
                GetGraphics().CreateSurface(m_eWidth - 2 - 16, m_eHeight - 2, g_ListBoxElementBackgroundColor[i]));
        m_SelectedElementBackgrounds.push_back(
                GetGraphics().CreateSurface(m_eWidth - 2 - 16, m_eHeight - 2, g_ListBoxSelectedElementColor[i]) );
    }

    m_ElementBorder = GetGraphics().CreateSurface(m_eWidth - 16, m_eHeight, g_ListBoxElementBorderColor);

    // "beveled" looking border for bottom & right sides of list element
    dest_rect.x = m_BorderSize;
    dest_rect.y = m_BorderSize;
    dest_rect.h = m_eHeight - m_BorderSize;
    dest_rect.w = m_eWidth - m_BorderSize - 16;
    m_ElementBorder = m_ElementBorder.FillRect(dest_rect, g_ListBoxElementBorderHColor);

    if (ShowHeaders)
    {
        // background for optional column header box
        int scroll_space = HeaderSort ? 0 : 16;
        dest_rect.w = m_eWidth - m_BorderSize - scroll_space;
        m_HeaderBackground = GetGraphics().CreateSurface(m_eWidth - scroll_space, m_eHeight, g_ListBoxHeaderBorderColor);
        m_HeaderBackground = m_HeaderBackground.FillRect(dest_rect, g_ListBoxHeaderBorderHColor);

        dest_rect.h = m_eHeight - (m_BorderSize * 2);
        dest_rect.w = m_eWidth - (m_BorderSize * 2) - scroll_space;
        m_HeaderBackground = m_HeaderBackground.FillRect(dest_rect, g_ListBoxHeaderBackgroundColor);

        DirPath dp = ImagePath("ListboxSort");
        m_SortAscImage = dp.str() + "Asc.png";
        m_SortDescImage = dp.str() + "Desc.png";
        std::string None = dp.str() + "None.png";
        m_HeaderUnSort = GetGraphics().LoadImage(None, -1, -1, true);

        // draw the "un-sort" clickable header

        if (HeaderSort)
        {
            SDL_Rect area{m_eWidth - 17, 0, 2, m_eHeight};
            m_HeaderBackground = m_HeaderBackground.FillRect(area, g_ListBoxHeaderBorderHColor);
            area.x++;
            area.w = 1;
            area.h--;
            m_HeaderBackground = m_HeaderBackground.FillRect(area, g_ListBoxHeaderBorderColor);
            area.h++;

            dest_rect.x = m_eWidth - 15;
            dest_rect.y = m_BorderSize;
            dest_rect.h = m_eHeight - 2;
            dest_rect.w = 14;
            m_HeaderBackground = m_HeaderBackground.BlitOther(m_HeaderUnSort, nullptr, &dest_rect);
        }

    }

    m_MultiSelect = MultiSelect;

    m_Divider.h = m_eHeight - (m_BorderSize * 2) - 3;
    m_Divider.y = m_BorderSize + 1;
}

cListBox::~cListBox()
{
}

void cListBox::ClearList()
{
    m_Items.clear();
    m_LastSelected = m_Items.end();
    m_Position = 0;
    m_NumElements = 0;
    m_ScrollBar->SetTopValue(0);
}

int cListBox::ArrowDownList()
{
    if(m_Items.empty())
        return -1;

    int selection = GetSelected();
    if (selection == -1)
    {
        SetSelected(m_Items.front().m_ID);
        return m_Items.front().m_ID;
    }

    auto current = std::find_if(begin(m_Items), end(m_Items),
            [selection](const cListItem& item) { return item.m_ID == selection; });

    if(current == m_Items.end())
        return -1;

    ++current;

    // If there is not next item, jump back to the top.
    if(current == m_Items.end()) {
        SetSelected(m_Items.front().m_ID);
        return m_Items.front().m_ID;
    }

    SetSelected(current->m_ID);
    return current->m_ID;
}

int cListBox::ArrowUpList()
{
    if(m_Items.empty())
        return -1;

    int selection = GetSelected();
    if (selection == -1)
    {
        SetSelected(m_Items.back().m_ID);
        return m_Items.back().m_ID;
    }

    auto current = std::find_if(begin(m_Items), end(m_Items),
                                [selection](const cListItem& item) { return item.m_ID == selection; });

    if(current == m_Items.end())
        return -1;

    if(current == m_Items.begin()) {
        SetSelected(m_Items.back().m_ID);
        return m_Items.back().m_ID;
    }

    --current;
    SetSelected(current->m_ID);
    return current->m_ID;
}

bool cListBox::HasMultiSelected()
{
    return m_HasMultiSelect;
}

bool cListBox::IsOver(int x, int y) const
{
    if(x > m_XPos && y > m_YPos && x < m_XPos + m_Width && y < m_YPos + m_Height) {
        return (m_ShowHeaders && y < m_YPos + m_BorderSize + m_RowHeight) || x < m_XPos + m_Width - 15;
    }
    return false;
}

bool cListBox::DoubleClicked()
{
    // make sure click time values are both set
    if (!m_CurrentClickTime || !m_LastClickTime)
        return false;

    // make sure little enough time has passed... 1000ms = 1 second
    // Windows default maximum for a double-click is 500ms, so let's use that
    if (m_CurrentClickTime > m_LastClickTime + 500)
        return false;

    // make sure click locations aren't spaced too far apart
    if (m_CurrentClickY > m_LastClickY + 2 || m_CurrentClickY < m_LastClickY - 2)
        return false;
    if (m_CurrentClickX > m_LastClickX + 2 || m_CurrentClickX < m_LastClickX - 2)
        return false;

    // all that having passed, looks like a double-click
    m_CurrentClickTime = 0;
    return true;
}

bool cListBox::HandleClick(int x, int y, bool press)
{
    if (press) return false;
    if (m_Items.empty())    // it doesn't matter if there are no items in the list
        return true;

    // if user clicked on "un-sort" header, do that
    if (m_ShowHeaders && y <= m_YPos + m_BorderSize + m_RowHeight)
    {
        if (m_HeaderClicksSort && x > m_XPos + m_eWidth - 16) {
            UnSortList();
            return true;
        }

        // See if a header was clicked
        m_HeaderClicked = "";
        for (auto& column : m_Columns) {
            if (column.skip) continue;

            int x_start = column.offset - 3;
            if (x >= m_XPos + x_start && x <= m_XPos + x_start + column.width) {
                HandleColumnHeaderClick(column);
                return true;
            }
        }
    }

    bool deselect = false;
    if (m_MultiSelect)
    {
        if (!window_manager().IsShiftHeld() && !window_manager().IsCtrlHeld())
        {
            m_HasMultiSelect = false;
            deselect = true;
        }
        else m_HasMultiSelect = true;
    }
    else deselect = true;

    // first unselect any currently selected items
    if (deselect)
    {
        for(auto& item : m_Items)
            item.m_Selected = false;
        m_LastSelected = m_Items.end();
    }

    bool singleSelect = true;
    if (m_MultiSelect)
    {
        if (window_manager().IsShiftHeld())
            singleSelect = false;
    }
    else singleSelect = true;

    if (singleSelect)    // find the first element displayed
    {
        auto clicked = FindItemAtPosition(x - m_XPos, y - m_YPos);
        if(clicked != m_Items.end()) {
            clicked->m_Selected = !clicked->m_Selected;
            m_LastSelected = clicked;

            // update info for tracking double-clicks
            m_LastClickTime = m_CurrentClickTime;
            m_LastClickX = m_CurrentClickX;
            m_LastClickY = m_CurrentClickY;
            m_CurrentClickTime = SDL_GetTicks();
            m_CurrentClickX = x;
            m_CurrentClickY = y;
        }
    }
    else
    {
        if (window_manager().IsShiftHeld())    // select from first to last
        {
            auto clicked = FindItemAtPosition(x - m_XPos, y - m_YPos);
            bool select = false;
            for(auto item = m_Items.begin(); item != m_Items.end(); ++item) {
                if(item == clicked || item == m_LastSelected) {
                    if(select) {
                        m_LastSelected = item;
                        item->m_Selected = true;
                        break;
                    }
                    select = true;
                }
                if(select) {
                    item->m_Selected = true;
                }
            }
        }
    }

    if (m_EnableEvents) {
        handle_selection_change();
    }

    return true;
}

void cListBox::HandleColumnHeaderClick(const sColumnData& column)
{
    // set it as clicked
    m_HeaderClicked = column.name;

    // should we re-sort list based on header?
    if (m_HeaderClicksSort)
    {
        if (m_SortedColumn == column.name)
            m_SortedDescending = !m_SortedDescending;
        else
        {
            m_SortedColumn     = column.name;
            m_SortedDescending = false;
        }
        ReSortList();
    }

    if (m_EnableEvents) {
        handle_selection_change();
    }
}

auto cListBox::FindItemAtPosition(int x, int y) -> item_list_t::iterator {
    int pos = 0;
    for (auto item = m_Items.begin(); item != m_Items.end(); ++item, ++pos) {
        if (pos < m_Position) continue;

        if ((pos - m_Position) >= m_NumDrawnElements)    // stop if running past the visible list
            break;

        int cX = m_BorderSize;
        int cY = m_BorderSize + m_RowHeight * (pos - m_Position);
        if (m_ShowHeaders) // Account for headers if shown
            cY += m_RowHeight;

        // Check if over the item
        if (x > cX && y > cY && x < cX + m_eWidth && y <= cY + m_eHeight) {
            // then select it
            return item;
        }
    }

    return m_Items.end();
}

void cListBox::DrawWidget(const CGraphics& gfx)
{
    if (m_ScrollChange >= 0)
    {  // scrollbar has changed top position
        m_Position = m_ScrollChange;
        m_ScrollChange = -1;
        m_ScrollBar->SetTopValue(m_Position);
    }

    SDL_Rect offset;

    // draw the box
    m_Border.DrawSurface(m_XPos, m_YPos);
    m_Background.DrawSurface(m_XPos + m_BorderSize, m_YPos + m_BorderSize);

    // Show column header box if enabled
    if (m_ShowHeaders)
    {
        offset.x = m_XPos + m_BorderSize;
        offset.y = m_YPos + m_BorderSize;

        // blit to the screen
        m_HeaderBackground.DrawSurface(m_XPos + m_BorderSize, m_YPos + m_BorderSize);

        m_Font.SetColor(g_ListBoxHeaderTextColor.r, g_ListBoxHeaderTextColor.g, g_ListBoxHeaderTextColor.b);

        // draw the header text
        for (int i = 0; i < m_Columns.size(); i++)
        {
            if (m_Columns[i].skip) continue;
            if (!m_SortedColumn.empty() && m_SortedColumn == m_Columns[i].name  )
            {
                SDL_Rect sort_offset;
                sort_offset.x = offset.x + m_Columns[i].offset;
                sort_offset.y = offset.y + 1;
                if (i == 0) sort_offset.x += 2 + m_BorderSize;
                m_HeaderSortBack.DrawSurface(sort_offset.x, sort_offset.y);
            }
            m_Columns[i].header_gfx.DrawSurface(offset.x + 3 + m_Columns[i].offset, offset.y + 3);
        }

        m_Font.SetColor(g_ListBoxTextColor.r, g_ListBoxTextColor.g, g_ListBoxTextColor.b);
    }

    // draw the elements
    int pos = 0;
    for (auto item = m_Items.begin(); item != m_Items.end(); ++item, ++pos)
    {
        if ((pos - m_Position) < 0) continue;
        if ((pos - m_Position) >= m_NumDrawnElements) break;

        // Draw the window
        offset.x = m_XPos + m_BorderSize;
        offset.y = (m_YPos + m_BorderSize) + (m_RowHeight*(pos - m_Position));
        if (m_ShowHeaders) // Account for headers if shown
            offset.y += m_RowHeight;

        // blit to the screen
        m_ElementBorder.DrawSurface(offset.x, offset.y);

        offset.x = offset.x + 1;
        offset.y = offset.y + 1;
        if (item->m_Selected) {
            m_SelectedElementBackgrounds[item->m_Color].DrawSurface(offset.x, offset.y);
        } else {
            m_ElementBackgrounds[item->m_Color].DrawSurface(offset.x, offset.y);
        }

        // draw the text
        for (auto& column : m_Columns)
        {
            if (column.skip) continue;
            item->m_PreRendered.at(column.sort).DrawSurface(offset.x + 2 + column.offset, offset.y + 1);
        }
    }
}

void cListBox::ScrollDown(int amount, bool updatebar)
{
    if (m_NumDrawnElements >= m_Items.size()) return;
    if (amount <= 0) amount = m_ScrollBar->m_ScrollAmount;
    if (m_Position + m_NumDrawnElements + amount < m_Items.size()) m_Position += amount;
    else m_Position = m_Items.size() - m_NumDrawnElements;
    if (updatebar) m_ScrollBar->SetTopValue(m_Position);
}

void cListBox::ScrollUp(int amount, bool updatebar)
{
    if (m_NumDrawnElements >= m_Items.size()) return;
    if (amount <= 0) amount = m_ScrollBar->m_ScrollAmount;
    if (m_Position - amount >= 0) m_Position -= amount;
    else m_Position = 0;
    if (updatebar) m_ScrollBar->SetTopValue(m_Position);
}

int cListBox::GetNextSelected(int from, int& pos)
{
    if (!m_MultiSelect) return -1;

    auto start = begin(m_Items);
    std::advance(start, from);
    auto selected = FindSelected(start);

    pos = std::distance(begin(m_Items), selected);

    if (selected != m_Items.end()) return selected->m_ID;
    return -1;
}

auto cListBox::FindSelected(const std::list<cListItem, std::allocator<cListItem>>::iterator& start) -> item_list_t::iterator
{
    auto selected = find_if(start, end(m_Items),
                            [](const cListItem& item) { return item.m_Selected; });
    return selected;
}

int cListBox::GetLastSelected()
{
    if (m_LastSelected != m_Items.end()) return m_LastSelected->m_ID;
    return -1;
}

int cListBox::GetSelected()
{
    if (m_LastSelected == m_Items.end()) return -1;
    //else return m_LastSelected->m_ID;

    auto selected = FindSelected(begin(m_Items));
    if(selected == m_Items.end())
        return -1;

    return selected->m_ID;
}

const std::string& cListBox::GetSelectedText()
{
    static std::string empty;
    if (m_LastSelected == m_Items.end())
        return empty;
    else
        return m_LastSelected->m_Data.front();
}

bool cListBox::IsSelected()
{
    return FindSelected(begin(m_Items)) != end(m_Items);
}

void cListBox::GetSortedIDList(std::vector<int> *id_vec, int *vec_pos)
{
    id_vec->clear();
    id_vec->reserve(m_Items.size());
    for(auto& item : m_Items) {
        id_vec->push_back(item.m_ID);
    }

    *vec_pos = std::distance(begin(m_Items), m_LastSelected);
}

void cListBox::SetElementText(int ID, std::string data)
{
    std::string datarray[] = { data };
    SetElementText(ID, datarray, 1);
}

void cListBox::SetElementText(int ID, std::string data[], int columns)
{
    for(auto& item : m_Items) {
        if (item.m_ID == ID)
        {
            for (int i = 0; i < columns; i++) {
                item.m_Data[i] = data[i];
                if(item.m_TextColor) {
                    m_Font.SetColor(item.m_TextColor->r, item.m_TextColor->g, item.m_TextColor->b);
                } else {
                    m_Font.SetColor(g_ListBoxTextColor.r, g_ListBoxTextColor.g, g_ListBoxTextColor.b);
                }
                item.m_PreRendered[i] = m_Font.RenderText(item.m_Data[i]);
            }
            break;
        }
    }
    ReSortList();
}

void cListBox::SetElementColumnText(int ID, std::string data, const std::string& column)
{
    int column_id = -1;
    for (int i = 0; i < m_Columns.size(); i++)
    {
        if (m_Columns[i].name == column) {
            column_id = i;
            break;
        }
    }

    if(column_id == -1) return;

    for(auto& item : m_Items) {
        if (item.m_ID == ID)
        {
            if(item.m_TextColor) {
                m_Font.SetColor(item.m_TextColor->r, item.m_TextColor->g, item.m_TextColor->b);
            } else {
                m_Font.SetColor(g_ListBoxTextColor.r, g_ListBoxTextColor.g, g_ListBoxTextColor.b);
            }
            item.m_PreRendered[column_id] = m_Font.RenderText(data);

            item.m_Data[column_id] = std::move(data);
            break;
        }
    }
    ReSortList();
}

void cListBox::SetElementTextColor(int ID, SDL_Color text_color)
{
    for(auto& item : m_Items) {
        if (item.m_ID == ID)
        {
            item.m_TextColor = std::make_unique<SDL_Color>(text_color);
            m_Font.SetColor(text_color.r, text_color.g, text_color.b);
            for(unsigned i = 0; i < item.m_Data.size(); ++i) {
                item.m_PreRendered[i] = m_Font.RenderText(item.m_Data[i]);
            }
            break;
        }
    }
}
void cListBox::AddElement(int ID, std::vector<std::string> data, int color)
{
    m_Items.emplace_back();
    auto& newItem = m_Items.back();
    if(data.size() != m_Columns.size())
        throw std::logic_error("Column count mismatch when adding new element to list box");
    newItem.m_Data = std::move(data);
    newItem.m_PreRendered.reserve(newItem.m_Data.size());
    for(std::size_t i = 0; i < newItem.m_Data.size(); ++i) {
        /// TODO add ability to use reference to existing font with new text
        m_Font.SetColor(g_ListBoxTextColor.r, g_ListBoxTextColor.g, g_ListBoxTextColor.b);
        auto gfx = m_Font.RenderText(newItem.m_Data[i]);
        newItem.m_PreRendered.push_back(std::move(gfx));
    }

    newItem.m_ID = ID;
    newItem.m_Color = color;
    newItem.m_InsertionOrder = m_NumElements;
    m_NumElements++;
    ReSortList();

    // update "item total" reference for scroll bar
    if(m_ScrollBar)
        m_ScrollBar->m_ItemsTotal = m_NumElements;
}

void cListBox::DefineColumns(std::vector<std::string> name, std::vector<std::string> header, std::vector<int> offset, std::vector<bool> skip)
{
    m_Columns.clear();

    m_Font.SetColor(g_ListBoxHeaderTextColor.r, g_ListBoxHeaderTextColor.g, g_ListBoxHeaderTextColor.b);

    m_Font.SetFontBold(true);
    for (int i = 0; i < name.size(); i++)
    {
        int left = offset[i];
        int right = i == name.size() - 1 ? m_eWidth : offset[i + 1];
        auto gfx = m_Font.RenderText(header[i]);
        m_Columns.emplace_back(sColumnData{std::move(name[i]), std::move(header[i]), left, right - left, i, skip[i], gfx});
    }
    m_Font.SetFontBold(false);

    if (!m_HeaderDividers) return;

    // while we're here, let's pre-draw the header dividers on the stored header background image
    for (int i = 1; i < name.size(); i++)
    {
        if (m_Columns[i].skip) continue;
        m_Divider.x = m_Columns[i].offset - 4;
        m_Divider.w = 2;
        m_HeaderBackground = m_HeaderBackground.FillRect(m_Divider, g_ListBoxHeaderBorderHColor);
        m_Divider.x++;
        m_Divider.w = 1;
        m_Divider.y++;
        m_Divider.h--;
        m_HeaderBackground = m_HeaderBackground.FillRect(m_Divider,g_ListBoxHeaderBorderColor);
        m_Divider.y--;
        m_Divider.h++;
    }
}

void cListBox::SetColumnSort(const std::vector<std::string>& column_name)
{
    for (int i = 0; i < column_name.size(); i++)
    {
        for (auto & column : m_Columns)
        {
            if (column.name == column_name[i]) {
                column.sort = i;
                break;
            }
        }
    }
}

void cListBox::SetSelected(int ID, bool ev, bool deselect_others)
{

    m_LastSelected = m_Items.end();
    m_HeaderClicked = "";

    if (m_Items.empty()) return;

    // special case if ID "-2" is sent, select first actual list item (not based on ID)
    if (ID == -2)
    {
        m_Items.front().m_Selected = true;
        m_LastSelected = m_Items.begin();
        m_Position = 0;
        m_ScrollBar->SetTopValue(m_Position);
        if (ev) handle_selection_change();
        return;
    }

    int count = 0; int posit = 0;
    for(auto current = m_Items.begin(); current != m_Items.end(); ++current) {
        if (current->m_ID == ID)
        {
            current->m_Selected = true;
            m_LastSelected = current;
            posit = count;
            if (!deselect_others) break;
        }
        else
        {
            if (deselect_others) current->m_Selected = false;
        }
        count++;
    }

    if (ev) handle_selection_change();

    if (count <= m_NumDrawnElements)
    {
        m_Position = 0;
    }
    else
    {
        if (m_Position >= posit)    // if the item is above the top of the list
        {
            m_Position = posit - 1;    // shift the list up
        }
        else if (m_Position < posit && posit < m_Position + m_NumDrawnElements - 1)
        {
            // don't change m_Position
        }
        else if (m_Position + m_NumDrawnElements - 1 <= posit)
            m_Position = posit - m_NumDrawnElements + 2;
    }

    if (m_Position > count - m_NumDrawnElements) m_Position = count - m_NumDrawnElements;
    if (m_Position < 0) m_Position = 0;

    m_ScrollBar->SetTopValue(m_Position);
}

int cListBox::GetAfterSelected()
{
    if (m_LastSelected != m_Items.end())
    {
        auto copy = m_LastSelected;
        ++copy;
        if (copy != m_Items.end())
            return copy->m_ID;
    }

    return -1;
}


void cListBox::ReSortList()
{
    if (m_SortedColumn.empty() || m_NumElements <= 0)
        return;
    SortByColumn(m_SortedColumn, m_SortedDescending);
}

void cListBox::UnSortList()
{
    if (m_NumElements <= 0)
        return;
    m_SortedColumn = "";
    // sort by insertion order
    m_Items.sort([](const cListItem& a, const cListItem& b) {
        return a.m_InsertionOrder < b.m_InsertionOrder;
    });
    UpdatePositionsAfterSort();
}

void cListBox::SortByColumn(std::string ColumnName, bool Descending)
{
    if (m_Items.empty())  // any items in list?
        return;

    int col_ref = -1;
    // Find the column id from the supplied column name
    int col_id = -1;
    for (int j = 0; j < m_Columns.size(); j++) {
        if (m_Columns[j].name == ColumnName) {
            col_id  = m_Columns[j].sort;
            col_ref = j;
            break;
        }
    }
    if (col_id == -1)  // match found for column name?
        return;

    // Sort the list
    m_Items.sort([col_id, Descending](const cListItem& a, const cListItem& b) {
        // TODO use a comparison function adequate for the column type
        bool cmp = a.m_Data[col_id].compare(b.m_Data[col_id]) > 0;
        return Descending == cmp;
    });

    UpdatePositionsAfterSort();

    if (m_ShowHeaders)
    {
        // Prepare Ascending/Descending image indicator for column header
        int dwidth;
        if (col_ref < m_Columns.size() - 1)
            dwidth = m_Columns[col_ref].width - 6;
        else
            dwidth = m_Columns[col_ref].width - 19;
        if (col_ref == 0)
            dwidth -= 2 + m_BorderSize;
        m_HeaderSortBack = GetGraphics().LoadImage(Descending ? m_SortDescImage : m_SortAscImage, dwidth, -1, true);
    }

    m_ScrollBar->SetTopValue(m_Position);
}

void cListBox::UpdatePositionsAfterSort()
{// Update m_Position
    int count = 0;
    for(auto & m_Item : m_Items)
{
        if (m_Item.m_Selected)
        {
            if (count < m_Position)
                m_Position = count;
            else if (count >(m_Position + m_NumDrawnElements - 1))
            {
                while (count > (m_Position + m_NumDrawnElements))
                    m_Position++;
                m_Position++;
            }
        }
        count++;
    }
}

void cListBox::handle_selection_change()
{
    if(DoubleClicked()) {
        if (m_DoubleClickCallback)
            m_DoubleClickCallback(GetSelected());
    } else {
        if (m_SelectionCallback)
            m_SelectionCallback(GetSelected());
    }
}

void cListBox::SetSelectionCallback(std::function<void(int)> cb)
{
    m_SelectionCallback = std::move(cb);
}

void cListBox::SetDoubleClickCallback(std::function<void(int)> cb)
{
    m_DoubleClickCallback = std::move(cb);
}

bool cListBox::HandleKeyPress(SDL_Keysym key){
    if(m_UpArrowHotKey != SDLK_UNKNOWN && key.sym == m_UpArrowHotKey) {
        ArrowUpList();
        return true;
    } else if(m_DownArrowHotKey != SDLK_UNKNOWN && key.sym == m_DownArrowHotKey) {
        ArrowDownList();
        return true;
    }
    return false;
}

void cListBox::SetArrowHotKeys(SDL_Keycode up, SDL_Keycode down)
{
    m_UpArrowHotKey = up;
    m_DownArrowHotKey = down;
}

void cListBox::Reset()
{
    ClearList();
}

bool cListBox::HandleMouseWheel(bool down)
{
    if(down) {
        ScrollDown();
    } else {
        ScrollUp();
    }
    return true;
}

std::vector<std::string> cListBox::GetColumnNames() const
{
    std::vector<std::string> result(m_Columns.size());
    std::transform(begin(m_Columns), end(m_Columns), begin(result), [](const sColumnData& d){ return d.name; });
    return std::move(result);
}
