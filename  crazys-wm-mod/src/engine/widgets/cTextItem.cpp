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

#include "interface/cFont.h"
#include "sConfig.h"
#include "cScrollBar.h"
#include "cTextItem.h"
#include <utility>
#include "interface/cInterfaceWindow.h"

extern cConfig cfg;

cTextItem::cTextItem(cInterfaceWindow* parent, int ID, int x, int y, int width, int height, std::string text, int size,
                     bool force_scrollbar, int red , int green , int blue):
    cUIWidget(ID, x, y, width, height, parent), m_Font(std::make_unique<cFont>(&GetGraphics()))
{
    m_ScrollBar = nullptr;
    m_ForceScrollBar = false;
	m_ScrollChange = 0;
    m_FontHeight = size;

    ChangeFontSize(size, red, green, blue);
    SetText(std::move(text));

    m_ForceScrollBar = force_scrollbar;
}

cTextItem::~cTextItem() {

}

int cTextItem::HeightTotal() { return m_PreRenderedText.GetHeight(); }

bool cTextItem::HandleMouseWheel(bool down)
{
	if (m_ScrollBar && !m_ScrollBar->IsHidden())
	{
		int newpos = m_ScrollChange + (m_Font->GetFontHeight() * (down ? 1 : -1) * m_ScrollBar->m_ScrollAmount);
		if (newpos < 0) newpos = 0;
		else if (newpos > HeightTotal() - GetHeight())
			newpos = HeightTotal() - GetHeight();
		m_ScrollBar->SetTopValue(newpos);
		m_ScrollChange = newpos;
		return true;
	}
	return false;
}

bool cTextItem::IsOver(int x, int y) const { return (x > m_XPos && y > m_YPos && x < m_XPos + m_Width - 15 && y < m_YPos + m_Height); }

void cTextItem::ChangeFontSize(int FontSize, int red , int green , int blue )
{
	m_Font->LoadFont(cfg.fonts.normal(), FontSize);
	m_Font->SetColor(red, green, blue);
	SetText(std::move(m_Text));
}

void cTextItem::SetText(std::string text)
{
    m_Text = std::move(text);

    int effective_width = GetWidth();

    if (m_ScrollBar && !m_ScrollBar->IsHidden()) {
        effective_width -= 17;
        m_ScrollBar->SetTopValue(0);
    }

    m_PreRenderedText = m_Font->RenderMultilineText(m_Text, effective_width);


    if (GetHeight() == 0 || m_Text.empty())
    {
        if (m_ScrollBar)
        {  // has scrollbar but doesn't need one since there doesn't seem to be any text at the moment; hide scrollbar
            m_ScrollBar->SetTopValue(0);
            m_ScrollBar->hide();
        }
        return;
    }

    m_ScrollChange = 0;

    if(m_ScrollBar) {
        if (!m_ScrollBar->IsHidden() && HeightTotal() <= GetHeight() && !m_ForceScrollBar)  // hide scrollbar
        {
            m_ScrollBar->SetTopValue(0);
            m_ScrollBar->hide();
        } else if (m_ScrollBar->IsHidden() && HeightTotal() > GetHeight()) { // un-hide existing scrollbar
            m_ScrollBar->unhide();
        }
    } else if (GetHeight() > 47 && (HeightTotal() > GetHeight() || m_ForceScrollBar)) {// add scrollbar
        /// TODO what does this have to do with parent position?
        int  x      = GetXPos() + GetWidth() - 15 - GetParent()->GetXPos();
        int  y      = GetYPos() - GetParent()->GetYPos();
        int  height = GetHeight();
        auto bar    = GetParent()->AddScrollBar(x, y, 16, height, height);
        m_ScrollBar   = bar;  // give TextItem pointer to scrollbar
        bar->ParentPosition = &m_ScrollChange;  // give scrollbar pointer to value it should update
        bar->m_ScrollAmount = cfg.resolution.text_scroll() * m_Font->GetFontHeight();
        bar->m_PageAmount   = bar->m_PageAmount - m_Font->GetFontHeight();
    }

    // update scrollbar if it exists
    int updated_width = GetWidth();
    if (m_ScrollBar)
    {
        if (!m_ScrollBar->IsHidden())
        {  // also, re-render text in narrower space to accommodate scrollbar width
            updated_width -= 17;
            if (m_ScrollBar && !m_ScrollBar->IsHidden())
                m_ScrollBar->SetTopValue(0);
        }
        m_ScrollBar->m_ItemsTotal = HeightTotal();
    }

    if(updated_width != effective_width) {
        m_PreRenderedText = m_Font->RenderMultilineText(m_Text, updated_width);
    }
}

void cTextItem::DrawWidget(const CGraphics& gfx)
{
	if (m_Text.empty()) return;

#if 0	// draw visible box showing exact area covered by TextItem; for debug usage, disabled by default
	SDL_Rect dstRect;
	dstRect.x = m_XPos;
	dstRect.y = m_YPos;
	dstRect.h = m_Height;
	dstRect.w = m_Width;
	SDL_FillRect(gfx.GetScreen(), &dstRect, SDL_MapRGB(gfx.GetScreen()->format, 200, 255, 255));
#endif

	if(!m_Text.empty()) {
        SDL_Rect srcRect;
        srcRect.x = 0;
        srcRect.y = m_ScrollChange;
        srcRect.h = m_Height;
        srcRect.w = m_Width;
	    m_PreRenderedText.DrawSurface(m_XPos, m_YPos, &srcRect);
	}
}

void cTextItem::SetHidden(bool mode) {
    cUIWidget::SetHidden(mode);
    if (m_ScrollBar) m_ScrollBar->SetHidden(mode);
}
