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
#include "cScrollBar.h"
#include "utils/DirPath.h"
#include "interface/CGraphics.h"

//these static vars defined in the header file need to be specified here
Uint8 cScrollBar::m_NotchOffset=0;
    
cScrollBar::cScrollBar(cInterfaceWindow* parent, int ID, int x, int y, int width, int height, int visibleitems) :
    cUIWidget(ID, x, y, width, height, parent),
    m_RectBGTop(new SDL_Rect),
    m_RectBGBottom(new SDL_Rect),
    m_RectTop(new SDL_Rect),
    m_RectBottom(new SDL_Rect)
{
    auto make_path = [](const char* file){ return ImagePath("Scroll").str() + file; };

    m_ImgBarBG = GetGraphics().LoadImage(make_path("LongBackground.png"));
    m_ImgBarDisabled = GetGraphics().LoadImage(make_path("LongDisabled.png"), -1, -1, true);
    m_ImgBarOff = GetGraphics().LoadImage(make_path("LongOff.png"), -1, -1, true);
    m_ImgBarOn = GetGraphics().LoadImage(make_path("LongOn.png"), -1, -1, true);
    m_ImgNotches = GetGraphics().LoadImage(make_path("Notches.png"), -1, -1, true);
    m_ImgButtonUpDisabled = GetGraphics().LoadImage(make_path("UpDisabled.png"), -1, -1, true);
    m_ImgButtonUpOff = GetGraphics().LoadImage(make_path("UpOff.png"), -1, -1, true);
    m_ImgButtonUpOn = GetGraphics().LoadImage(make_path("UpOn.png"), -1, -1, true);
    m_ImgButtonDownDisabled = GetGraphics().LoadImage(make_path("DownDisabled.png"), -1, -1, true);
    m_ImgButtonDownOff = GetGraphics().LoadImage(make_path("DownOff.png"), -1, -1, true);
    m_ImgButtonDownOn = GetGraphics().LoadImage(make_path("DownOn.png"), -1, -1, true);

    m_NotchOffset = int(((double)m_ImgNotches.GetHeight() / 2));

    m_SectionHeight = height - 2*m_ImgButtonDownOff.GetHeight();
    m_BarHeight = m_SectionHeight;
    m_ItemsVisible = visibleitems;
    m_PageAmount = visibleitems - 1;

    // set up SDL_Rects indicating top and bottom halves of displayed background from source background images
    m_RectBGTop->x = m_RectBGTop->y = m_RectBGBottom->x = 0;
    m_RectBGTop->w = m_RectBGBottom->w = m_ImgBarBG.GetWidth();
    m_RectBGTop->h = (m_SectionHeight / 2);
    m_RectBGBottom->h = m_SectionHeight - m_RectBGTop->h;
    m_RectBGBottom->y = m_ImgBarBG.GetHeight() - m_RectBGBottom->h;

    // set up initial base data for SDL_Rects indicating top and bottom halves of displayed bar from source bar images
    m_RectTop->x = m_RectTop->y = m_RectBottom->x = 0;
    m_RectTop->w = m_RectBottom->w = m_ImgBarBG.GetWidth();

    // go ahead and prepare initial "disabled" state
    UpdateScrollBar();
}

cScrollBar::~cScrollBar() = default;


void cScrollBar::UpdateScrollBar()
{
    m_BarTop = m_ItemTop = 0;
    m_ItemsTotalLast = m_ItemsTotal;

    if (m_ItemsTotal <= m_ItemsVisible)
    {  // if all items are shown with no need to scroll, disable self
        m_BarHeight = m_SectionHeight;
        m_ImgBar = m_ImgBarDisabled;
        m_ImgButtonUp = m_ImgButtonUpDisabled;
        m_ImgButtonDown = m_ImgButtonDownDisabled;
        SetDisabled(true);
    }
    else
    {  // calculate height of dragbar based on total items vs. visible items
        m_BarHeight = (int) ( (double)m_SectionHeight * ((double)m_ItemsVisible / (double)m_ItemsTotal) );
        m_ImgBar = m_ImgBarOff;
        m_ImgButtonUp = m_ImgButtonUpOff;
        m_ImgButtonDown = m_ImgButtonDownOff;
        SetDisabled(false);
    }

    // bar height must be at least 15px tall; arbitrary limit, could be changed if desired
    if (m_BarHeight < 15)
        m_BarHeight = 15;

    // set size and Y offset for Rects for source bar surfaces
    m_RectTop->h = (m_BarHeight / 2);
    m_RectBottom->h = m_BarHeight - m_RectTop->h;
    m_RectBottom->y = m_ImgBarOff.GetHeight() - m_RectBottom->h;
}

void cScrollBar::SetTopValue(int itemnum)
{  // this function updates the vertical position of the scroll bar based on which item is the first visible one
    m_ItemTop = itemnum;

    if (m_ItemsTotal <= m_ItemsVisible)
        return;

    //if total # of items changed, update bar rects first
    if (m_ItemsTotal != m_ItemsTotalLast)
        UpdateScrollBar();

    // lastitem is the last possible "topmost" item; maxtop is the lowest Y value the bar top can be located
    int lastitem = m_ItemsTotal - m_ItemsVisible;
    int maxtop = m_SectionHeight - m_BarHeight;

    if (itemnum == 0)  // first item
        m_BarTop = 0;
    else if (itemnum >= lastitem)  // last item
        m_BarTop = maxtop;
    else  // anywhere else between, calculate the appropriate position
        m_BarTop = (int) ( (double)(maxtop) * ((double)itemnum / (double)lastitem) );
}

bool cScrollBar::IsOver(int x, int y) const
{
    if(m_IsBeingDragged) {
        return true;
    }
    return cUIWidget::IsOver(x, y);
}

void cScrollBar::DragMove(int y)
{  // ScrollBar is being dragged, so update bar position and send new position to scrollbar's parent
    // this function should be called only by mousemove detection after dragging is initiated
    int NPos = y - m_YPos;

    // set current bar position based on movement
    if (NPos <= m_DragInitYPos)
        m_BarTop = 0;
    else if (NPos - m_DragInitYPos + m_BarHeight >= m_SectionHeight)
        m_BarTop = m_SectionHeight - m_BarHeight;
    else
        m_BarTop = NPos - m_DragInitYPos;

    int lastitem = m_ItemsTotal - m_ItemsVisible;
    int maxtop = m_SectionHeight - m_BarHeight;
    int listpos = 0;

    // extrapolate first visible item based on bar position, set it
    if (m_BarTop == 0)  // first item
        listpos = 0;
    else if (m_BarTop >= maxtop)  // last item
        listpos = lastitem;
    else  // anywhere else between; the 0.5 added is to make it effectively "round" instead of "floor"
        listpos = (int) ( (double)m_BarTop * ((double)lastitem / (double)maxtop) + 0.5 );

    if(ParentPosition)  // send updated position back to parent control
    {
        *ParentPosition = listpos;
        if(m_UpdateSelf)
            SetTopValue(listpos);
    }
}

void cScrollBar::DrawWidget(const CGraphics& gfx)
{
    //if total # of list items changed, update bar rects
    if (m_ItemsTotal != m_ItemsTotalLast)
        UpdateScrollBar();

    // draw buttons
    m_ImgButtonUp.DrawSurface(m_XPos, m_YPos);
    m_ImgButtonDown.DrawSurface(m_XPos,  m_YPos + m_Height - m_ImgButtonDown.GetHeight());

    // draw background
    m_ImgBarBG.DrawSurface(m_XPos, m_YPos + m_ImgButtonUp.GetHeight(), m_RectBGTop.get());
    m_ImgBarBG.DrawSurface(m_XPos, m_YPos + m_ImgButtonUp.GetHeight() + m_RectBGTop->h, m_RectBGBottom.get());

    // draw bar
    m_ImgBar.DrawSurface(m_XPos, m_YPos + m_ImgButtonUp.GetHeight() + m_BarTop, m_RectTop.get());
    m_ImgBar.DrawSurface(m_XPos, m_YPos + m_ImgButtonUp.GetHeight() + m_BarTop + m_RectTop->h, m_RectBottom.get());

    if(!IsDisabled())
    {
        // draw notches in the center
        m_ImgNotches.DrawSurface(m_XPos, m_YPos + m_ImgButtonUp.GetHeight() + m_BarTop + m_RectTop->h - m_NotchOffset);
    }
}

void cScrollBar::SetDisabled(bool disable) {
    cUIWidget::SetDisabled(disable);
    m_ImgBar = disable ? m_ImgBarDisabled : m_ImgBarOff;
}

bool cScrollBar::HandleClick(int x, int y, bool press)
{
    if(!press && m_IsBeingDragged) {
        SetTopValue(m_ItemTop);
        m_IsBeingDragged = false;
        return true;
    }

    // clicked on "up" button; scroll up small amount
    if (y <= m_YPos + m_ImgButtonUp.GetHeight()) {
        ScrollBy(-m_ScrollAmount);
        return true;
        // clicked on "down" button; scroll down small amount
    } else if (y >= m_YPos + m_Height - m_ImgButtonUp.GetHeight()) {
        ScrollBy(m_ScrollAmount);
        return true;
    }

    m_DragInitYPos = y - m_YPos;  // mouse Y position within entire bar section

    // make sure they clicked within the scroll bar itself, not just anywhere in the overall scrollbar section
    if( IsOver(x,y) && (m_DragInitYPos > m_BarTop + m_ImgButtonUp.GetHeight()) && (m_DragInitYPos < m_BarTop + m_ImgButtonUp.GetHeight() + m_BarHeight) )
    {
        m_IsBeingDragged = true;  // g_DragScrollBar is used in main.cpp to reference whichever bar is being dragged (if any)
        m_DragInitYPos -= m_BarTop;  // update to mouse Y position, within scroll bar itself
    }
    return true;
}

void cScrollBar::ScrollBy(int amount)
{
    int newpos = m_ItemTop + amount;
    if(newpos != m_ItemTop)
    {
        if(newpos > m_ItemsTotal - m_ItemsVisible)
            newpos = m_ItemsTotal - m_ItemsVisible;
        else if(newpos < 0)
            newpos = 0;

        if(ParentPosition)
            *ParentPosition = newpos;

        if(m_UpdateSelf)
            SetTopValue(newpos);
    }
}

bool cScrollBar::HandleMouseWheel(bool down)
{
    ScrollBy(down ? m_ScrollAmount : -m_ScrollAmount);
    return true;
}

void cScrollBar::HandleMouseMove(bool over, int x, int y)
{
    m_ImgBar = m_ImgBarOff;
    m_ImgButtonUp = m_ImgButtonUpOff;
    m_ImgButtonDown = m_ImgButtonDownOff;

    if(m_IsBeingDragged) {
        DragMove(y);
    }

    if(cUIWidget::IsOver(x, y))
    {
        if(y > m_YPos+m_ImgButtonUp.GetHeight() && y < m_YPos+m_Height-m_ImgButtonDown.GetHeight())
            m_ImgBar = m_ImgBarOn;  // over scroll section
        else if(y <= m_YPos+m_ImgButtonUp.GetHeight())
            m_ImgButtonUp = m_ImgButtonUpOn;  // over up button
        else if(y >= m_YPos+m_Height-m_ImgButtonDown.GetHeight())
            m_ImgButtonDown = m_ImgButtonDownOn;  // over down button
        return;
    }
}
