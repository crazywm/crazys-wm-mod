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
#include "cButton.h"
#include "CResourceManager.h"
extern cInterfaceEventManager g_InterfaceEvents;
extern CResourceManager rmanager;

cButton::~cButton()
{
	if (m_Next) delete m_Next;
	m_Next = 0;
	m_CurrImage = 0;
	if (m_OffImage) delete m_OffImage;
	m_OffImage = 0;
	if (m_DisabledImage) delete m_DisabledImage;
	m_DisabledImage = 0;
	if (m_OnImage) delete m_OnImage;
	m_OnImage = 0;
}

bool cButton::CreateButton(string OffImage, string DisabledImage, string OnImage, int ID, int x, int y, int width, int height, bool transparency, bool cached)
{
	if (OffImage != "")
	{
		m_OffImage = new CSurface(OffImage);
		m_OffImage->m_Cached = cached;
	}
	else m_OffImage = 0;

	if (DisabledImage != "")
	{
		m_DisabledImage = new CSurface(DisabledImage);
		m_DisabledImage->m_Cached = cached;
	}
	else m_DisabledImage = 0;

	if (OnImage != "")
	{
		m_OnImage = new CSurface(OnImage);
		m_OnImage->m_Cached = cached;
	}
	else m_OnImage = 0;

	m_CurrImage = m_OffImage;
	SetPosition(x, y, width, height);

	if (transparency)
	{
		if (m_OffImage)			m_OffImage->SetAlpha(true);
		if (m_DisabledImage)	m_DisabledImage->SetAlpha(true);
		if (m_OnImage)			m_OnImage->SetAlpha(true);
	}

	m_Next = 0;
	m_ID = ID;

	return true;
}

bool cButton::IsOver(int x, int y)
{
	if (m_Disabled || m_Hidden) return false;
	bool over = false;
	if (x > m_XPos && y > m_YPos && x < m_XPos + m_Width && y < m_YPos + m_Height)
	{
		m_CurrImage = m_OnImage;
		over = true;
	}
	else m_CurrImage = m_OffImage;
	return over;
}

bool cButton::ButtonClicked(int x, int y)
{
	if (m_Disabled || m_Hidden) return false;
	if (IsOver(x, y))
	{
		g_InterfaceEvents.AddEvent(EVENT_BUTTONCLICKED, m_ID);
		return true;
	}
	return false;
}

void cButton::Draw()
{
	if (m_Hidden) return;
	if (m_CurrImage)
	{
		SDL_Rect rect;
		rect.y = rect.x = 0;
		rect.w = m_Width;
		rect.h = m_Height;
		m_CurrImage->DrawSurface(m_XPos, m_YPos, 0, &rect, true);
	}
}