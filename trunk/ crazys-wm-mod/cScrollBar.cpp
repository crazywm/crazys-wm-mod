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
#include "CResourceManager.h"
#include "DirPath.h"
#include "main.h"
extern cInterfaceEventManager g_InterfaceEvents;
extern CResourceManager rmanager;

//these static vars defined in the header file need to be specified here
SDL_Surface* cScrollBar::m_ImgBarBG=0;
SDL_Surface* cScrollBar::m_ImgBarOn=0;
SDL_Surface* cScrollBar::m_ImgBarOff=0;
SDL_Surface* cScrollBar::m_ImgBarDisabled=0;
SDL_Surface* cScrollBar::m_ImgNotches=0;
Uint8 cScrollBar::m_NotchOffset=0;
SDL_Surface* cScrollBar::m_ImgButtonUpOn=0;
SDL_Surface* cScrollBar::m_ImgButtonUpOff=0;
SDL_Surface* cScrollBar::m_ImgButtonUpDisabled=0;
SDL_Surface* cScrollBar::m_ImgButtonDownOn=0;
SDL_Surface* cScrollBar::m_ImgButtonDownOff=0;
SDL_Surface* cScrollBar::m_ImgButtonDownDisabled=0;
	
cScrollBar::cScrollBar()
{
	m_BarHeight = m_SectionHeight = m_BarTop = m_ItemTop = 0;
	m_ItemsTotal = m_ItemsVisible = m_ItemsTotalLast = 0;
	ParentPosition = 0;
	m_ScrollAmount = 1;
	m_PageAmount = 3;
	m_UpdateSelf = true;
	m_Disabled = false;
	m_Hidden = false;
	m_RectBGTop = new SDL_Rect;
	m_RectBGBottom = new SDL_Rect;
	m_RectTop = new SDL_Rect;
	m_RectBottom = new SDL_Rect;
	m_ImgBar = m_ImgButtonUp = m_ImgButtonDown = 0;
}

cScrollBar::~cScrollBar()
{
	delete m_RectBGTop;
	delete m_RectBGBottom;
	delete m_RectTop;
	delete m_RectBottom;
	m_ImgBar = m_ImgButtonUp = m_ImgButtonDown = 0;
}

bool cScrollBar::CreateScrollBar(int ID, int x, int y, int width, int height, int visibleitems)
{
	// see if static class-wide default images are loaded; if not, do so
	if (!m_ImgNotches)
		LoadInitial();

	SetPosition(x, y, width, height);
	m_SectionHeight = height - m_ImgButtonDownOff->h - m_ImgButtonDownOff->h;
	m_BarHeight = m_SectionHeight;
	m_ItemsVisible = visibleitems;
	m_PageAmount = visibleitems - 1;

	// set up SDL_Rects indicating top and bottom halves of displayed background from source background images
	m_RectBGTop->x = m_RectBGTop->y = m_RectBGBottom->x = 0;
	m_RectBGTop->w = m_RectBGBottom->w = m_ImgBarBG->w;
	m_RectBGTop->h = (m_SectionHeight / 2);
	m_RectBGBottom->h = m_SectionHeight - m_RectBGTop->h;
	m_RectBGBottom->y = m_ImgBarBG->h - m_RectBGBottom->h;

	// set up initial base data for SDL_Rects indicating top and bottom halves of displayed bar from source bar images
	m_RectTop->x = m_RectTop->y = m_RectBottom->x = 0;
	m_RectTop->w = m_RectBottom->w = m_ImgBarBG->w;

	// go ahead and prepare initial "disabled" state
	UpdateScrollBar();

	m_ID = ID;

	return true;
}

void cScrollBar::LoadInitial()
{  // load static class-wide shared base images into memory; only called once by first scrollbar created
	DirPath dp = ImagePath("Scroll");
	string disabled = string(dp.c_str()) + "LongDisabled.png";
	string off = string(dp.c_str()) + "LongOff.png";
	string on = string(dp.c_str()) + "LongOn.png";
	string bg = string(dp.c_str()) + "LongBackground.png";
	string notches = string(dp.c_str()) + "Notches.png";
	m_ImgBarBG = IMG_Load(bg.c_str());
	SDL_Surface* TmpImg;
	TmpImg = IMG_Load(disabled.c_str());
	m_ImgBarDisabled = SDL_DisplayFormatAlpha(TmpImg);
	SDL_FreeSurface(TmpImg);
	TmpImg = IMG_Load(off.c_str());
	m_ImgBarOff = SDL_DisplayFormatAlpha(TmpImg);
	SDL_FreeSurface(TmpImg);
	TmpImg = IMG_Load(on.c_str());
	m_ImgBarOn = SDL_DisplayFormatAlpha(TmpImg);
	SDL_FreeSurface(TmpImg);
	TmpImg = IMG_Load(notches.c_str());
	m_ImgNotches = SDL_DisplayFormatAlpha(TmpImg);
	SDL_FreeSurface(TmpImg);
	m_NotchOffset = int(((double)m_ImgNotches->h / 2));
	string updisabled = string(dp.c_str()) + "UpDisabled.png";
	string upoff = string(dp.c_str()) + "UpOff.png";
	string upon = string(dp.c_str()) + "UpOn.png";
	string downdisabled = string(dp.c_str()) + "DownDisabled.png";
	string downoff = string(dp.c_str()) + "DownOff.png";
	string downon = string(dp.c_str()) + "DownOn.png";
	TmpImg = IMG_Load(updisabled.c_str());
	m_ImgButtonUpDisabled = SDL_DisplayFormatAlpha(TmpImg);
	SDL_FreeSurface(TmpImg);
	TmpImg = IMG_Load(upoff.c_str());
	m_ImgButtonUpOff = SDL_DisplayFormatAlpha(TmpImg);
	SDL_FreeSurface(TmpImg);
	TmpImg = IMG_Load(upon.c_str());
	m_ImgButtonUpOn = SDL_DisplayFormatAlpha(TmpImg);
	SDL_FreeSurface(TmpImg);
	TmpImg = IMG_Load(downdisabled.c_str());
	m_ImgButtonDownDisabled = SDL_DisplayFormatAlpha(TmpImg);
	SDL_FreeSurface(TmpImg);
	TmpImg = IMG_Load(downoff.c_str());
	m_ImgButtonDownOff = SDL_DisplayFormatAlpha(TmpImg);
	SDL_FreeSurface(TmpImg);
	TmpImg = IMG_Load(downon.c_str());
	m_ImgButtonDownOn = SDL_DisplayFormatAlpha(TmpImg);
	SDL_FreeSurface(TmpImg);
}

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
		m_Disabled = true;
	}
	else
	{  // calculate height of dragbar based on total items vs. visible items
		m_BarHeight = (int) ( (double)m_SectionHeight * ((double)m_ItemsVisible / (double)m_ItemsTotal) );
		m_ImgBar = m_ImgBarOff;
		m_ImgButtonUp = m_ImgButtonUpOff;
		m_ImgButtonDown = m_ImgButtonDownOff;
		m_Disabled = false;
	}

	// bar height must be at least 15px tall; arbitrary limit, could be changed if desired
	if (m_BarHeight < 15)
		m_BarHeight = 15;

	// set size and Y offset for Rects for source bar surfaces
	m_RectTop->h = (m_BarHeight / 2);
	m_RectBottom->h = m_BarHeight - m_RectTop->h;
	m_RectBottom->y = m_ImgBarOff->h - m_RectBottom->h;
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

bool cScrollBar::IsOver(int x, int y)
{
	if(m_Disabled || m_Hidden)
		return false;

	bool over = false;
	m_ImgBar = m_ImgBarOff;
	m_ImgButtonUp = m_ImgButtonUpOff;
	m_ImgButtonDown = m_ImgButtonDownOff;

	if(x > m_XPos && y > m_YPos && x < m_XPos+m_Width && y < m_YPos+m_Height)
	{
		over = true;
		if(y > m_YPos+m_ImgButtonUp->h && y < m_YPos+m_Height-m_ImgButtonDown->h)
			m_ImgBar = m_ImgBarOn;  // over scroll section
		else if(y <= m_YPos+m_ImgButtonUp->h)
			m_ImgButtonUp = m_ImgButtonUpOn;  // over up button
		else if(y >= m_YPos+m_Height-m_ImgButtonDown->h)
			m_ImgButtonDown = m_ImgButtonDownOn;  // over down button
	}

	return over;
}

bool cScrollBar::MouseDown(int x, int y)
{  // this function is needed to initiate dragging of the bar
	if(m_Disabled || m_Hidden)
		return true;
	
	m_DragInitYPos = y - m_YPos;  // mouse Y position within entire bar section
	
	// make sure they clicked within the scroll bar itself, not just anywhere in the overall scrollbar section
	if( IsOver(x,y) && (m_DragInitYPos > m_BarTop + m_ImgButtonUp->h) && (m_DragInitYPos < m_BarTop + m_ImgButtonUp->h + m_BarHeight) )
	{
		g_DragScrollBar = this;  // g_DragScrollBar is used in main.cpp to reference whichever bar is being dragged (if any)
		m_DragInitYPos -= m_BarTop;  // update to mouse Y position, within scroll bar itself
	}
	return true;
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

bool cScrollBar::ButtonClicked(int x, int y, bool mouseWheelDown, bool mouseWheelUp)
{
	if(m_Disabled || m_Hidden || !ParentPosition)
		return false;

	if(IsOver(x,y))
	{
		int newpos = m_ItemTop;
		if (mouseWheelUp)
			newpos -= m_ScrollAmount;
		else if (mouseWheelDown)
			newpos += m_ScrollAmount;
		// clicked on "up" button; scroll up small amount
		else if (y <= m_YPos + m_ImgButtonUp->h)
			newpos -= m_ScrollAmount;
		// clicked on "down" button; scroll down small amount
		else if (y >= m_YPos + m_Height - m_ImgButtonUp->h)
			newpos += m_ScrollAmount;
		// clicked in bar space above actual scroll bar; scroll up large amount
		else if (y < m_YPos + m_BarTop + m_ImgButtonUp->h)
			newpos -= m_PageAmount;
		// clicked in bar space below actual scroll bar; scroll down large amount
		else if (y > m_YPos + m_BarTop + m_BarHeight + m_ImgButtonUp->h)
			newpos += m_PageAmount;

		if(newpos != m_ItemTop)
		{
			if(newpos > m_ItemsTotal - m_ItemsVisible)
				newpos = m_ItemsTotal - m_ItemsVisible;
			else if(newpos < 0)
				newpos = 0;

			*ParentPosition = newpos;

			if(m_UpdateSelf)
				SetTopValue(newpos);
		}

		return true;
	}
	return false;
}

void cScrollBar::Draw()
{
	if(m_Hidden || !m_ImgBar)
		return;
	
	//if total # of list items changed, update bar rects
	if (m_ItemsTotal != m_ItemsTotalLast)
		UpdateScrollBar();

	SDL_Rect dstRect;
	dstRect.x = m_XPos;

	int error = 0;

	// draw "up" button
	dstRect.y = m_YPos;
	error = SDL_BlitSurface(m_ImgButtonUp, 0, g_Graphics.GetScreen(), &dstRect);
	if(error == -1)
	{
		LogScrollBarError("Error blitting scrollbar up button");
		return;
	}
	// draw "down" button
	dstRect.y = m_YPos + m_Height - m_ImgButtonDown->h;
	error = SDL_BlitSurface(m_ImgButtonDown, 0, g_Graphics.GetScreen(), &dstRect);
	if(error == -1)
	{
		LogScrollBarError("Error blitting scrollbar down button");
		return;
	}
	// draw top half of background
	dstRect.y = m_YPos + m_ImgButtonUp->h;
	error = SDL_BlitSurface(m_ImgBarBG, m_RectBGTop, g_Graphics.GetScreen(), &dstRect);
	if(error == -1)
	{
		LogScrollBarError("Error blitting scrollbar background (top half)");
		return;
	}
	// draw bottom half of background
	dstRect.y += m_RectBGTop->h;
	error = SDL_BlitSurface(m_ImgBarBG, m_RectBGBottom, g_Graphics.GetScreen(), &dstRect);
	if(error == -1)
	{
		LogScrollBarError("Error blitting scrollbar background (bottom half)");
		return;
	}

	// draw top half of bar
	dstRect.y = m_YPos + m_ImgButtonUp->h + m_BarTop;
	error = SDL_BlitSurface(m_ImgBar, m_RectTop, g_Graphics.GetScreen(), &dstRect);
	if(error == -1)
	{
		LogScrollBarError("Error blitting scrollbar bar (top half)");
		return;
	}
	// draw bottom half of bar
	dstRect.y += m_RectTop->h;
	error = SDL_BlitSurface(m_ImgBar, m_RectBottom, g_Graphics.GetScreen(), &dstRect);
	if(error == -1)
	{
		LogScrollBarError("Error blitting scrollbar bar (bottom half)");
		return;
	}

	if(!m_Disabled)
	{  // draw notches in the center
		dstRect.y -= m_NotchOffset;
		error = SDL_BlitSurface(m_ImgNotches, 0, g_Graphics.GetScreen(), &dstRect);
		if(error == -1)
		{
			LogScrollBarError("Error blitting scrollbar notches");
			return;
		}
	}
}

void cScrollBar::LogScrollBarError(string description)
{
	CLog l;
	l.ss() << description << " - " << SDL_GetError();
	l.ssend();
}
