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

#include "cFont.h"
#include "cInterfaceObject.h"
#include "sConfig.h"
#include "cScrollBar.h"
#include "cTextItem.h"

extern cConfig cfg;

cTextItem::cTextItem(int ID, int x, int y, int width, int height, string text, int size, bool auto_scrollbar,
                     bool force_scrollbar , bool leftorright , int red , int green , int blue):
    cUIWidget(ID, x, y, width, height)
{
    m_ScrollBar = nullptr;
	m_AutoScrollBar = true;
	m_ForceScrollBar = false;
	m_ScrollChange = 0;
    m_FontHeight = size;

    SetText(text);
    ChangeFontSize(size, red, green, blue);

    m_AutoScrollBar = auto_scrollbar;
    m_ForceScrollBar = force_scrollbar;
}

cTextItem::~cTextItem() = default;

void cTextItem::DisableAutoScroll(bool disable) { m_AutoScrollBar = !disable; }
void cTextItem::ForceScrollBar(bool force) { m_ForceScrollBar = force; }

// does scrollbar exist, but current text fits, and scrollbar isn't being forced?
bool cTextItem::NeedScrollBarHidden() { return (m_ScrollBar && !m_ScrollBar->IsHidden() && HeightTotal() <= GetHeight() && !m_ForceScrollBar); }
// does scrollbar exist but is hidden, and current text doesn't fit?
bool cTextItem::NeedScrollBarShown() { return (m_ScrollBar && m_ScrollBar->IsHidden() && HeightTotal() > GetHeight()); }
// does a scrollbar need to be added?
bool cTextItem::NeedScrollBar() { return (!m_ScrollBar && GetHeight() > 47 && (HeightTotal() > GetHeight() || m_ForceScrollBar)); }
int cTextItem::HeightTotal() { return m_Font.GetHeight(); }
void cTextItem::MouseScrollWheel(int x, int y, bool ScrollDown )
{
	if (m_ScrollBar && !m_ScrollBar->IsHidden() && IsOver(x, y))
	{
		int newpos = m_ScrollChange + ((m_Font.GetFontHeight() * (ScrollDown) ? 1 : -1) * m_ScrollBar->m_ScrollAmount);
		if (newpos < 0) newpos = 0;
		else if (newpos > HeightTotal() - GetHeight())
			newpos = HeightTotal() - GetHeight();
		m_ScrollBar->SetTopValue(newpos);
		m_ScrollChange = newpos;
	}
}

bool cTextItem::IsOver(int x, int y) { return (x > m_XPos && y > m_YPos && x < m_XPos + m_Width - 15 && y < m_YPos + m_Height); }

void cTextItem::ChangeFontSize(int FontSize, int red , int green , int blue )
{
	m_Font.LoadFont(cfg.fonts.normal(), FontSize);
	m_Font.SetText(m_Text);

	m_Font.SetColor(red, green, blue);
	m_Font.SetMultiline(true, m_Width, m_Height);
}

void cTextItem::SetText(string text)
{
	m_Text = text;
	m_Font.SetText(m_Text);
	if (m_ScrollBar && !m_ScrollBar->IsHidden())
		m_ScrollBar->SetTopValue(0);
}

void cTextItem::DrawWidget()
{
	if (m_Text.empty()) return;

#if 0	// draw visible box showing exact area covered by TextItem; for debug usage, disabled by default
	SDL_Rect dstRect;
	dstRect.x = m_XPos;
	dstRect.y = m_YPos;
	dstRect.h = m_Height;
	dstRect.w = m_Width;
	SDL_FillRect(g_Graphics.GetScreen(), &dstRect, SDL_MapRGB(g_Graphics.GetScreen()->format, 200, 255, 255));
#endif

	m_Font.DrawMultilineText(m_XPos, m_YPos, 0, m_ScrollChange);
}

void cTextItem::SetHidden(bool mode) {
    cUIWidget::SetHidden(mode);
    if (m_ScrollBar) m_ScrollBar->SetHidden(mode);
}
