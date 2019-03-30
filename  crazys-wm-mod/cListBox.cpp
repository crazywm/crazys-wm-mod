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
#include <algorithm>
#include <sstream>
#include <SDL_rotozoom.h>
#include "cListBox.h"
#include "CGraphics.h"
#include "cInterfaceEvent.h"
#include "strnatcmp.h"
#include "sConfig.h"
#include "Globals.h"

SDL_Surface* cListBox::m_HeaderSortAsc = 0;
SDL_Surface* cListBox::m_HeaderSortDesc = 0;
SDL_Surface* cListBox::m_HeaderUnSort = 0;

extern CGraphics g_Graphics;
extern cInterfaceEventManager g_InterfaceEvents;

extern cConfig cfg;

extern bool g_ShiftDown;
extern bool g_CTRLDown;

extern unsigned char g_ListBoxBorderR, g_ListBoxBorderG, g_ListBoxBorderB;
extern unsigned char g_ListBoxBackgroundR, g_ListBoxBackgroundG, g_ListBoxBackgroundB;
extern unsigned char g_ListBoxElementBackgroundR, g_ListBoxElementBackgroundG, g_ListBoxElementBackgroundB;
extern unsigned char g_ListBoxS1ElementBackgroundR, g_ListBoxS1ElementBackgroundG, g_ListBoxS1ElementBackgroundB;
extern unsigned char g_ListBoxS2ElementBackgroundR, g_ListBoxS2ElementBackgroundG, g_ListBoxS2ElementBackgroundB;
extern unsigned char g_ListBoxS3ElementBackgroundR, g_ListBoxS3ElementBackgroundG, g_ListBoxS3ElementBackgroundB;
extern unsigned char g_ListBoxS4ElementBackgroundR, g_ListBoxS4ElementBackgroundG, g_ListBoxS4ElementBackgroundB;
extern unsigned char g_ListBoxSelectedElementR, g_ListBoxSelectedElementG, g_ListBoxSelectedElementB;
extern unsigned char g_ListBoxSelectedS1ElementR, g_ListBoxSelectedS1ElementG, g_ListBoxSelectedS1ElementB;
extern unsigned char g_ListBoxSelectedS2ElementR, g_ListBoxSelectedS2ElementG, g_ListBoxSelectedS2ElementB;
extern unsigned char g_ListBoxSelectedS3ElementR, g_ListBoxSelectedS3ElementG, g_ListBoxSelectedS3ElementB;
extern unsigned char g_ListBoxSelectedS4ElementR, g_ListBoxSelectedS4ElementG, g_ListBoxSelectedS4ElementB;
extern unsigned char g_ListBoxElementBorderR, g_ListBoxElementBorderG, g_ListBoxElementBorderB;
extern unsigned char g_ListBoxElementBorderHR, g_ListBoxElementBorderHG, g_ListBoxElementBorderHB;
extern unsigned char g_ListBoxTextR, g_ListBoxTextG, g_ListBoxTextB;
extern unsigned char g_ListBoxHeaderBackgroundR, g_ListBoxHeaderBackgroundG, g_ListBoxHeaderBackgroundB;
extern unsigned char g_ListBoxHeaderBorderR, g_ListBoxHeaderBorderG, g_ListBoxHeaderBorderB;
extern unsigned char g_ListBoxHeaderBorderHR, g_ListBoxHeaderBorderHG, g_ListBoxHeaderBorderHB;
extern unsigned char g_ListBoxHeaderTextR, g_ListBoxHeaderTextG, g_ListBoxHeaderTextB;

cListBox::cListBox(int ID, int x, int y, int width, int height, int BorderSize, bool MultiSelect,
        bool ShowHeaders, bool HeaderDiv, bool HeaderSort, int fontsize, int rowheight):
    cUIWidget(ID, x, y, width, height)
{
    m_NumDrawnElements = m_NumElements = m_Position = 0;
    m_LastSelected = m_Items.end();
    m_ScrollChange = -1;

    m_ColumnCount = 1;
    m_ShowHeaders = false;
    m_HeaderDividers = true;
    for (unsigned int i = 0; i<LISTBOX_COLUMNS; i++)
    {
        m_ColumnOffset[i] = 0;
        m_ColumnSort[i] = i;
        m_Header[i] = "";
        m_ColumnName[i] = "";
        m_SkipColumn[i] = false;
    }

    m_HeaderBackground = 0;

    m_HeaderClicksSort = true;
    m_SortedColumn = "";
    m_SortedDescending = false;

    m_LastClickX = 0;
    m_LastClickY = 0;
    m_LastClickTime = 0;
    m_CurrentClickX = 0;
    m_CurrentClickY = 0;
    m_CurrentClickTime = 0;

    m_Background = 0;
    m_Border = 0;

    m_DarkBlueBackground = m_RedBackground = m_GreenBackground = m_ElementBackground = m_YellowBackground = 0;
    m_SelectedRedBackground = m_SelectedDarkBlueBackground = m_SelectedGreenBackground = m_SelectedYellowBackground = m_ElementSelectedBackground = 0;
    m_ElementBorder = 0;

    m_EnableEvents = false;
    m_MultiSelect = false;
    m_HasMultiSelect = false;

    SDL_Rect dest_rect;

    m_ShowHeaders = ShowHeaders;
    m_HeaderDividers = HeaderDiv;
    m_HeaderClicksSort = HeaderSort;

    m_BorderSize = BorderSize;
    m_RowHeight = (rowheight == 0 ? LISTBOX_ITEMHEIGHT : rowheight);
    SetPosition(x, y, width, height);
    m_Border = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, 0, 0, 0, 0);
    SDL_FillRect(m_Border, 0, SDL_MapRGB(m_Border->format, g_ListBoxBorderR, g_ListBoxBorderG, g_ListBoxBorderB));

    m_Background = SDL_CreateRGBSurface(SDL_SWSURFACE, width - (BorderSize * 2) - 16, height - (BorderSize * 2), 32, 0, 0, 0, 0);
    SDL_FillRect(m_Background, 0, SDL_MapRGB(m_Background->format, g_ListBoxBackgroundR, g_ListBoxBackgroundG, g_ListBoxBackgroundB));

    m_NumDrawnElements = height / m_RowHeight;
    if (m_ShowHeaders) // Account for headers if shown
        m_NumDrawnElements--;
    m_eWidth = (width - (BorderSize * 2));
    m_eHeight = m_RowHeight;

    m_RedBackground = SDL_CreateRGBSurface(SDL_SWSURFACE, m_eWidth - 2 - 16, m_eHeight - 2, 32, 0, 0, 0, 0);
    SDL_FillRect(m_RedBackground, 0, SDL_MapRGB(m_RedBackground->format, g_ListBoxS1ElementBackgroundR, g_ListBoxS1ElementBackgroundG, g_ListBoxS1ElementBackgroundB));

    m_DarkBlueBackground = SDL_CreateRGBSurface(SDL_SWSURFACE, m_eWidth - 2 - 16, m_eHeight - 2, 32, 0, 0, 0, 0);
    SDL_FillRect(m_DarkBlueBackground, 0, SDL_MapRGB(m_DarkBlueBackground->format, g_ListBoxS2ElementBackgroundR, g_ListBoxS2ElementBackgroundG, g_ListBoxS2ElementBackgroundB));

    m_GreenBackground = SDL_CreateRGBSurface(SDL_SWSURFACE, m_eWidth - 2 - 16, m_eHeight - 2, 32, 0, 0, 0, 0);
    SDL_FillRect(m_GreenBackground, 0, SDL_MapRGB(m_GreenBackground->format, g_ListBoxS3ElementBackgroundR, g_ListBoxS3ElementBackgroundG, g_ListBoxS3ElementBackgroundB));

    m_YellowBackground = SDL_CreateRGBSurface(SDL_SWSURFACE, m_eWidth - 2 - 16, m_eHeight - 2, 32, 0, 0, 0, 0);
    SDL_FillRect(m_YellowBackground, 0, SDL_MapRGB(m_YellowBackground->format, g_ListBoxS4ElementBackgroundR, g_ListBoxS4ElementBackgroundG, g_ListBoxS4ElementBackgroundB));

    m_ElementBackground = SDL_CreateRGBSurface(SDL_SWSURFACE, m_eWidth - 2 - 16, m_eHeight - 2, 32, 0, 0, 0, 0);
    SDL_FillRect(m_ElementBackground, 0, SDL_MapRGB(m_ElementBackground->format, g_ListBoxElementBackgroundR, g_ListBoxElementBackgroundG, g_ListBoxElementBackgroundB));

    m_ElementSelectedBackground = SDL_CreateRGBSurface(SDL_SWSURFACE, m_eWidth - 2 - 16, m_eHeight - 2, 32, 0, 0, 0, 0);
    SDL_FillRect(m_ElementSelectedBackground, 0, SDL_MapRGB(m_ElementSelectedBackground->format, g_ListBoxSelectedElementR, g_ListBoxSelectedElementG, g_ListBoxSelectedElementB));

    m_SelectedRedBackground = SDL_CreateRGBSurface(SDL_SWSURFACE, m_eWidth - 2 - 16, m_eHeight - 2, 32, 0, 0, 0, 0);
    SDL_FillRect(m_SelectedRedBackground, 0, SDL_MapRGB(m_SelectedRedBackground->format, g_ListBoxSelectedS1ElementR, g_ListBoxSelectedS1ElementG, g_ListBoxSelectedS1ElementB));

    m_SelectedDarkBlueBackground = SDL_CreateRGBSurface(SDL_SWSURFACE, m_eWidth - 2 - 16, m_eHeight - 2, 32, 0, 0, 0, 0);
    SDL_FillRect(m_SelectedDarkBlueBackground, 0, SDL_MapRGB(m_SelectedDarkBlueBackground->format, g_ListBoxSelectedS2ElementR, g_ListBoxSelectedS2ElementG, g_ListBoxSelectedS2ElementB));

    m_SelectedGreenBackground = SDL_CreateRGBSurface(SDL_SWSURFACE, m_eWidth - 2 - 16, m_eHeight - 2, 32, 0, 0, 0, 0);
    SDL_FillRect(m_SelectedGreenBackground, 0, SDL_MapRGB(m_SelectedGreenBackground->format, g_ListBoxSelectedS3ElementR, g_ListBoxSelectedS3ElementG, g_ListBoxSelectedS3ElementB));

    m_SelectedYellowBackground = SDL_CreateRGBSurface(SDL_SWSURFACE, m_eWidth - 2 - 16, m_eHeight - 2, 32, 0, 0, 0, 0);
    SDL_FillRect(m_SelectedYellowBackground, 0, SDL_MapRGB(m_SelectedYellowBackground->format, g_ListBoxSelectedS3ElementR, g_ListBoxSelectedS3ElementG, g_ListBoxSelectedS3ElementB));

    m_ElementBorder = SDL_CreateRGBSurface(SDL_SWSURFACE, m_eWidth - 16, m_eHeight, 32, 0, 0, 0, 0);
    SDL_FillRect(m_ElementBorder, 0, SDL_MapRGB(m_ElementBorder->format, g_ListBoxElementBorderR, g_ListBoxElementBorderG, g_ListBoxElementBorderB));

    // "beveled" looking border for bottom & right sides of list element
    dest_rect.x = m_BorderSize;
    dest_rect.y = m_BorderSize;
    dest_rect.h = m_eHeight - m_BorderSize;
    dest_rect.w = m_eWidth - m_BorderSize - 16;
    SDL_FillRect(m_ElementBorder, &dest_rect, SDL_MapRGB(m_ElementBorder->format, g_ListBoxElementBorderHR, g_ListBoxElementBorderHG, g_ListBoxElementBorderHB));

    if (ShowHeaders)
    {
        // background for optional column header box
        int scroll_space = (HeaderSort) ? 0 : 16;
        dest_rect.w = m_eWidth - m_BorderSize - scroll_space;
        m_HeaderBackground = SDL_CreateRGBSurface(SDL_SWSURFACE, m_eWidth - scroll_space, m_eHeight, 32, 0, 0, 0, 0);
        SDL_FillRect(m_HeaderBackground, 0, SDL_MapRGB(m_HeaderBackground->format, g_ListBoxHeaderBorderR, g_ListBoxHeaderBorderG, g_ListBoxHeaderBorderB));
        SDL_FillRect(m_HeaderBackground, &dest_rect, SDL_MapRGB(m_HeaderBackground->format, g_ListBoxHeaderBorderHR, g_ListBoxHeaderBorderHG, g_ListBoxHeaderBorderHB));
        dest_rect.h = m_eHeight - (m_BorderSize * 2);
        dest_rect.w = m_eWidth - (m_BorderSize * 2) - scroll_space;
        SDL_FillRect(m_HeaderBackground, &dest_rect, SDL_MapRGB(m_HeaderBackground->format, g_ListBoxHeaderBackgroundR, g_ListBoxHeaderBackgroundG, g_ListBoxHeaderBackgroundB));

        DirPath dp = ImagePath("ListboxSort");
        string Asc = string(dp.c_str()) + "Asc.png";
        string Desc = string(dp.c_str()) + "Desc.png";
        string None = string(dp.c_str()) + "None.png";
        if (m_HeaderSortAsc == 0)	m_HeaderSortAsc = IMG_Load(Asc.c_str());
        if (m_HeaderSortDesc == 0)	m_HeaderSortDesc = IMG_Load(Desc.c_str());
        if (m_HeaderUnSort == 0)	m_HeaderUnSort = IMG_Load(None.c_str());

        // draw the "un-sort" clickable header
        if (HeaderSort)
        {
            m_Divider.x = m_eWidth - 17;
            m_Divider.w = 2;
            SDL_FillRect(
                    m_HeaderBackground,
                    &m_Divider,
                    SDL_MapRGB(
                            m_HeaderBackground->format,
                            g_ListBoxHeaderBorderHR,
                            g_ListBoxHeaderBorderHG,
                            g_ListBoxHeaderBorderHB
                    )
            );
            m_Divider.x++;
            m_Divider.w = 1;
            m_Divider.h--;
            SDL_FillRect(m_HeaderBackground, &m_Divider, SDL_MapRGB(m_HeaderBackground->format, g_ListBoxHeaderBorderR, g_ListBoxHeaderBorderG, g_ListBoxHeaderBorderB));
            m_Divider.h++;

            dest_rect.x = m_eWidth - 15;
            dest_rect.y = m_BorderSize;
            dest_rect.h = m_eHeight - 2;
            dest_rect.w = 14;
            SDL_BlitSurface(m_HeaderUnSort, 0, m_HeaderBackground, &dest_rect);
        }
    }

    m_Font.LoadFont(cfg.fonts.normal(), (fontsize == 0 ? 10 : fontsize));
    m_Font.SetText("");
    m_Font.SetColor(g_ListBoxTextR, g_ListBoxTextG, g_ListBoxTextB);

    m_MultiSelect = MultiSelect;

    m_Divider.h = m_eHeight - (m_BorderSize * 2) - 3;
    m_Divider.y = m_BorderSize + 1;
}

cListBox::~cListBox()
{
	if (m_Background)					SDL_FreeSurface(m_Background);					m_Background = 0;
	if (m_Border)						SDL_FreeSurface(m_Border);						m_Border = 0;
	if (m_RedBackground)				SDL_FreeSurface(m_RedBackground);				m_RedBackground = 0;
	if (m_DarkBlueBackground)			SDL_FreeSurface(m_DarkBlueBackground);			m_DarkBlueBackground = 0;
	if (m_GreenBackground)				SDL_FreeSurface(m_GreenBackground);				m_GreenBackground = 0;
	if (m_YellowBackground)				SDL_FreeSurface(m_YellowBackground);			m_YellowBackground = 0;
	if (m_HeaderBackground)				SDL_FreeSurface(m_HeaderBackground);			m_HeaderBackground = 0;
	m_HeaderSortBack = 0;
	if (m_SelectedRedBackground)		SDL_FreeSurface(m_SelectedRedBackground);		m_SelectedRedBackground = 0;
	if (m_SelectedDarkBlueBackground)	SDL_FreeSurface(m_SelectedDarkBlueBackground);	m_SelectedDarkBlueBackground = 0;
	if (m_SelectedGreenBackground)		SDL_FreeSurface(m_SelectedGreenBackground);		m_SelectedGreenBackground = 0;
	if (m_SelectedYellowBackground)		SDL_FreeSurface(m_SelectedYellowBackground);	m_SelectedYellowBackground = 0;
	if (m_ElementBackground)			SDL_FreeSurface(m_ElementBackground);			m_ElementBackground = 0;
	if (m_ElementSelectedBackground)	SDL_FreeSurface(m_ElementSelectedBackground);	m_ElementSelectedBackground = 0;
	if (m_ElementBorder)				SDL_FreeSurface(m_ElementBorder);				m_ElementBorder = 0;
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

bool cListBox::IsOver(int x, int y)
{
	bool over = false;
	if (x > m_XPos && y > m_YPos && x < m_XPos + m_Width - 15 && y < m_YPos + m_Height)
		over = true;

	return over;
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

void cListBox::OnClicked(int x, int y, bool mouseWheelDown, bool mouseWheelUp)
{
	cListItem* current = 0;
	if (m_NumElements == 0)	// it doesn't matter if there are no items in the list
		return;

	// if user clicked on "un-sort" header, do that
	if (m_ShowHeaders && m_HeaderClicksSort
		&& x > m_XPos + m_eWidth - 16
		&& x <= m_XPos + m_eWidth
		&& y > m_YPos + m_BorderSize
		&& y <= m_YPos + m_BorderSize + m_RowHeight
		)
	{
		UnSortList();
		return;
	}

	if (IsOver(x, y))
	{
		if (mouseWheelDown)
		{
			ScrollDown();
			return;
		}

		if (mouseWheelUp)
		{
			ScrollUp();
			return;
		}

		if (m_EnableEvents)
			g_InterfaceEvents.AddEvent(EVENT_SELECTIONCHANGE, m_ID);

		// See if a header was clicked
		m_HeaderClicked = "";
		if (m_ShowHeaders && y > m_YPos + m_BorderSize && y <= m_YPos + m_BorderSize + m_RowHeight)
		{
			int x_start = 0, x_end = 0;
			for (int i = 0; i < m_ColumnCount; i++)
			{
				if (m_SkipColumn[i]) continue;

				x_start = m_ColumnOffset[i] - 3;

				if (i < m_ColumnCount - 1)
					x_end = m_ColumnOffset[i + 1] - 3;
				else
					x_end = m_eWidth;

				if (x >= m_XPos + x_start && x <= m_XPos + x_end)
				{
					// then set it as clicked
					m_HeaderClicked = m_ColumnName[i];

					// should we re-sort list based on header?
					if (m_HeaderClicksSort)
					{
						if (m_SortedColumn == m_ColumnName[i])
							m_SortedDescending = !m_SortedDescending;
						else
						{
							m_SortedColumn = m_ColumnName[i];
							m_SortedDescending = false;
						}
						ReSortList();
					}
					return;
				}
			}
		}

		bool deselect = false;
		if (m_MultiSelect)
		{
			if (!g_ShiftDown && !g_CTRLDown)
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
			if (g_ShiftDown)
				singleSelect = false;
		}
		else singleSelect = true;

		if (singleSelect)	// find the first element displayed
		{
            auto clicked = FindItemAtPosition(x, y);
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
			if (g_ShiftDown)	// select from first to last
			{
				auto clicked = FindItemAtPosition(x, y);
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
	}
}

auto cListBox::FindItemAtPosition(int x, int y) -> item_list_t::iterator {
    int pos = 0;
    for (auto item = m_Items.begin(); item != m_Items.end(); ++item, ++pos) {
        if (pos < m_Position) continue;

        if ((pos - m_Position) >= m_NumDrawnElements)    // stop if running past the visible list
            break;

        int cX = m_XPos + m_BorderSize;
        int cY = (m_YPos + m_BorderSize) + (m_RowHeight * (pos - m_Position));
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

void cListBox::DrawWidget()
{
	if (m_ScrollChange >= 0)
	{  // scrollbar has changed top position
		m_Position = m_ScrollChange;
		m_ScrollChange = -1;
		m_ScrollBar->SetTopValue(m_Position);
	}

	int count = 0;
	SDL_Rect offset;

	// draw the box
	if (m_Background && m_Border)
	{
		// Draw the window
		offset.x = m_XPos;
		offset.y = m_YPos;

		// blit to the screen
		SDL_BlitSurface(m_Border, 0, g_Graphics.GetScreen(), &offset);

		offset.x = m_XPos + m_BorderSize;
		offset.y = m_YPos + m_BorderSize;
		SDL_BlitSurface(m_Background, 0, g_Graphics.GetScreen(), &offset);
	}

	// Show column header box if enabled
	if (m_ShowHeaders)
	{
		offset.x = m_XPos + m_BorderSize;
		offset.y = m_YPos + m_BorderSize;

		// blit to the screen
		SDL_BlitSurface(m_HeaderBackground, 0, g_Graphics.GetScreen(), &offset);

		m_Font.SetColor(g_ListBoxHeaderTextR, g_ListBoxHeaderTextG, g_ListBoxHeaderTextB);

		// draw the header text
		for (int i = 0; i<m_ColumnCount; i++)
		{
			if (m_SkipColumn[i]) continue;
			if (m_SortedColumn != "" && m_SortedColumn == m_ColumnName[i])
			{
				SDL_Rect sort_offset;
				sort_offset.x = offset.x + m_ColumnOffset[i];
				sort_offset.y = offset.y + 1;
				if (i == 0) sort_offset.x += 2 + m_BorderSize;
				SDL_BlitSurface(m_HeaderSortBack, 0, g_Graphics.GetScreen(), &sort_offset);
			}
			m_Font.SetText(m_Header[i]);
			//m_Font.SetFontBold(true);
			m_Font.DrawText(offset.x + 3 + m_ColumnOffset[i], offset.y + 3);
		}

		//m_Font.SetFontBold(false);
		m_Font.SetColor(g_ListBoxTextR, g_ListBoxTextG, g_ListBoxTextB);
	}

	// draw the elements
    int pos = 0;
    for (auto item = m_Items.begin(); item != m_Items.end(); ++item, ++pos)
    {
		if ((pos - m_Position) >= m_NumDrawnElements) break;

		// Draw the window
		offset.x = m_XPos + m_BorderSize;
		offset.y = (m_YPos + m_BorderSize) + (m_RowHeight*(pos - m_Position));
		if (m_ShowHeaders) // Account for headers if shown
			offset.y += m_RowHeight;

		// blit to the screen
		SDL_BlitSurface(m_ElementBorder, nullptr, g_Graphics.GetScreen(), &offset);

		offset.x = offset.x + 1;
		offset.y = offset.y + 1;
		if (item->m_Selected)
		{
			switch (item->m_Color)
			{
			case 1:		SDL_BlitSurface(m_SelectedRedBackground, 0, g_Graphics.GetScreen(), &offset);		break;
			case 2:		SDL_BlitSurface(m_SelectedDarkBlueBackground, 0, g_Graphics.GetScreen(), &offset);	break;
			case 3:		SDL_BlitSurface(m_SelectedGreenBackground, 0, g_Graphics.GetScreen(), &offset);		break;	// `J` added green
			case 4:		SDL_BlitSurface(m_SelectedYellowBackground, 0, g_Graphics.GetScreen(), &offset);	break;	// `J` added yellow
			default:	SDL_BlitSurface(m_ElementSelectedBackground, 0, g_Graphics.GetScreen(), &offset);	break;
			}
		}
		else
		{
			switch (item->m_Color)
			{
			case 1:		SDL_BlitSurface(m_RedBackground, 0, g_Graphics.GetScreen(), &offset);				break;
			case 2:		SDL_BlitSurface(m_DarkBlueBackground, 0, g_Graphics.GetScreen(), &offset);			break;
			case 3:		SDL_BlitSurface(m_GreenBackground, 0, g_Graphics.GetScreen(), &offset);				break;	// `J` added green
			case 4:		SDL_BlitSurface(m_YellowBackground, 0, g_Graphics.GetScreen(), &offset);			break;	// `J` added yellow
			default:	SDL_BlitSurface(m_ElementBackground, 0, g_Graphics.GetScreen(), &offset);			break;
			}
		}

		// if we have a custom text color specified, use it
		if (item->m_TextColor != 0)
			m_Font.SetColor(item->m_TextColor->r, item->m_TextColor->g, item->m_TextColor->b);

		// draw the text
		for (int i = 0; i<m_ColumnCount; i++)
		{
			if (m_SkipColumn[i]) continue;
			m_Font.SetText(item->m_Data[m_ColumnSort[i]]);
			m_Font.DrawText(offset.x + 2 + m_ColumnOffset[i], offset.y + 1);
		}

		m_Font.SetColor(0, 0, 0);
	}
}

void cListBox::ScrollDown(int amount, bool updatebar)
{
	if (m_NumDrawnElements >= m_NumElements) return;
	if (amount <= 0) amount = m_ScrollBar->m_ScrollAmount;
	if (m_Position + m_NumDrawnElements + amount < m_NumElements) m_Position += amount;
	else m_Position = m_NumElements - m_NumDrawnElements;
	if (updatebar) m_ScrollBar->SetTopValue(m_Position);
}

void cListBox::ScrollUp(int amount, bool updatebar)
{
	if (m_NumDrawnElements >= m_NumElements) return;
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

auto cListBox::FindSelected(const list<cListItem, std::allocator<cListItem>>::iterator& start) -> item_list_t::iterator
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

string cListBox::GetSelectedText()
{
	if (m_LastSelected == m_Items.end())
	    return "";
	else
	    return m_LastSelected->m_Data->c_str();
}

bool cListBox::IsSelected()
{
    return FindSelected(begin(m_Items)) != end(m_Items);
}

void cListBox::GetSortedIDList(vector<int> *id_vec, int *vec_pos)
{
	id_vec->clear();
	id_vec->reserve(m_Items.size());
	for(auto& item : m_Items) {
	    id_vec->push_back(item.m_ID);
	}

	*vec_pos = std::distance(begin(m_Items), m_LastSelected);
}

void cListBox::SetElementText(int ID, string data)
{
	string datarray[] = { data };
	SetElementText(ID, datarray, 1);
}

void cListBox::SetElementText(int ID, string data[], int columns)
{
	for(auto& item : m_Items) {
	    if (item.m_ID == ID)
		{
			for (int i = 0; i < columns; i++)
				item.m_Data[i] = data[i];
			break;
		}
	}
	ReSortList();
}

void cListBox::SetElementColumnText(int ID, string data, const string& column)
{
    int column_id = -1;
    for (int i = 0; i < m_ColumnCount; i++)
    {
        if (m_ColumnName[i] == column) {
            column_id = i;
            break;
        }
    }

    if(column_id == -1) return;

    for(auto& item : m_Items) {
        if (item.m_ID == ID)
        {
			item.m_Data[column_id] = std::move(data);
			break;
		}
    }
	ReSortList();
}

void cListBox::SetElementTextColor(int ID, SDL_Color* text_color)
{
    for(auto& item : m_Items) {
        if (item.m_ID == ID)
        {
			item.m_TextColor = text_color;
			break;
		}
	}
}
void cListBox::AddElement(int ID, string data[], int columns, int color)
{
    m_Items.emplace_back();
    auto& newItem = m_Items.back();
    for (int i = 0; i<columns; i++) {
        newItem.m_Data[i] = data[i];
    }
    newItem.m_ID = ID;
    newItem.m_Color = color;

	// for the purposes of allowing restoration of the default sort, store the count of this list item
	stringstream ss;
	ss << m_NumElements;
	newItem.m_Data[LISTBOX_COLUMNS] = ss.str();

	m_NumElements++;
	ReSortList();
}

void cListBox::DefineColumns(string name[], string header[], int offset[], bool skip[], int columns)
{
	m_ColumnCount = columns;
	for (int i = 0; i<m_ColumnCount; i++)
	{
		m_ColumnName[i] = name[i];
		m_Header[i] = header[i];
		m_ColumnOffset[i] = int((float)offset[i] * _G.g_ScreenScaleX);
		m_SkipColumn[i] = skip[i];
	}

	if (!m_HeaderDividers) return;

	// while we're here, let's pre-draw the header dividers on the stored header background image
	for (int i = 1; i<m_ColumnCount; i++)
	{
		if (m_SkipColumn[i]) continue;
		m_Divider.x = m_ColumnOffset[i] - 4;
		m_Divider.w = 2;
		SDL_FillRect(m_HeaderBackground, &m_Divider, SDL_MapRGB(m_HeaderBackground->format, g_ListBoxHeaderBorderHR, g_ListBoxHeaderBorderHG, g_ListBoxHeaderBorderHB));
		m_Divider.x++;
		m_Divider.w = 1;
		m_Divider.y++;
		m_Divider.h--;
		SDL_FillRect(m_HeaderBackground, &m_Divider, SDL_MapRGB(m_HeaderBackground->format, g_ListBoxHeaderBorderR, g_ListBoxHeaderBorderG, g_ListBoxHeaderBorderB));
		m_Divider.y--;
		m_Divider.h++;
	}
}

void cListBox::SetColumnSort(string column_name[], int columns)
{
	for (int i = 0; i<columns; i++)
	{
		for (int j = 0; j<m_ColumnCount; j++)
		{
			if (m_ColumnName[j] == column_name[i])
			{
				m_ColumnSort[j] = i;
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
		if (ev) g_InterfaceEvents.AddEvent(EVENT_SELECTIONCHANGE, m_ID);
		return;
	}

	int count = 0; int posit = 0;
	for(auto current = m_Items.begin(); current != m_Items.end(); ++current) {
		if (current->m_ID == ID)
		{
			if (ev) g_InterfaceEvents.AddEvent(EVENT_SELECTIONCHANGE, m_ID);
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

	if (count <= m_NumDrawnElements)
	{
		m_Position = 0;
	}
	else
	{
		if (m_Position >= posit)	// if the item is above the top of the list
		{
			m_Position = posit - 1;	// shift the list up
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
	m_SortedColumn = "<[=-UNSORT_THIS_BITCH-=]>";
	m_SortedDescending = false;
	SortByColumn(m_SortedColumn, m_SortedDescending);
}

void cListBox::SortByColumn(string ColumnName, bool Descending)
{
	if (m_Items.empty())  // any items in list?
		return;

	// Find the column id from the supplied column name
	int col_id = -1, col_ref = -1;
	if (ColumnName == "<[=-UNSORT_THIS_BITCH-=]>")
	{  // we're un-sorting, so reference the extra "unsorted order" column
		col_id = LISTBOX_COLUMNS;
		col_ref = -1;
	}
	else
	{
		for (int j = 0; j<m_ColumnCount; j++)
		{
			if (m_ColumnName[j] == ColumnName)
			{
				col_id = m_ColumnSort[j];
				col_ref = j;
				break;
			}
		}
		if (col_id == -1)  // match found for column name?
			return;
	}

	// Sort the list
	m_Items.sort([col_id, Descending](const cListItem& a, const cListItem& b){
        bool cmp = strnatcasecmp(a.m_Data[col_id].c_str(), b.m_Data[col_id].c_str()) > 0;
        return Descending == cmp;
	});

	// Update m_Position
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

	if (m_ShowHeaders && col_ref > -1)
	{
		// Prepare Ascending/Descending image indicator for column header
		int dwidth;
		if (col_ref < m_ColumnCount - 1)
			dwidth = m_ColumnOffset[col_ref + 1] - 6 - m_ColumnOffset[col_ref];
		else
			dwidth = m_eWidth - 19 - m_ColumnOffset[col_ref];
		if (col_ref == 0)
			dwidth -= 2 + m_BorderSize;
		double stretch = (double)dwidth / (double)m_HeaderSortDesc->w;
		if (Descending)
			m_HeaderSortBack = zoomSurface(m_HeaderSortDesc, stretch, 1, 1);
		else
			m_HeaderSortBack = zoomSurface(m_HeaderSortAsc, stretch, 1, 1);
	}

	m_ScrollBar->SetTopValue(m_Position);
}

void cListBox::GetColumnNames(vector<string>& columnNames)
{
	columnNames.clear();
	for (int x = 0; x < m_ColumnCount; ++x)
	{
		columnNames.push_back(m_ColumnName[x]);
	}
}

int cListBox::DayJobColumn()
{
	for (int i = 0; i < m_ColumnCount; i++)
	{
		if (m_ColumnName[i] == "DayJob") return i;
	}
	return -1;
}

int cListBox::NightJobColumn()
{
	for (int i = 0; i < m_ColumnCount; i++)
	{
		if (m_ColumnName[i] == "NightJob") return i;
	}
	return -1;
}
