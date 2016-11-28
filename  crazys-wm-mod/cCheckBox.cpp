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
#include "cCheckBox.h"
#include "DirPath.h"
#include "CGraphics.h"
#include "sConfig.h"

extern unsigned char g_CheckBoxBorderR, g_CheckBoxBorderG, g_CheckBoxBorderB;
extern unsigned char g_CheckBoxBackgroundR, g_CheckBoxBackgroundG, g_CheckBoxBackgroundB;
extern CGraphics g_Graphics;
extern cInterfaceEventManager g_InterfaceEvents;

extern cConfig cfg;

cCheckBox::~cCheckBox()
{
	if (m_Surface) SDL_FreeSurface(m_Surface);
	m_Surface = 0;
	if (m_Border) SDL_FreeSurface(m_Border);
	m_Border = 0;
	if (m_Image) { m_Image->Free(); delete m_Image; }
	m_Image = 0;
	if (m_Next) delete m_Next;
	m_Next = 0;
}

bool cCheckBox::CreateCheckBox(int id, int x, int y, int width, int height, string text, int fontsize, bool leftorright)
{
	m_ID = id;

	m_Border = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, 0, 0, 0, 0);
	SDL_FillRect(m_Border, 0, SDL_MapRGB(m_Border->format, g_CheckBoxBorderR, g_CheckBoxBorderG, g_CheckBoxBorderB));

	m_Surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width - 2, height - 2, 32, 0, 0, 0, 0);
	SDL_FillRect(m_Surface, 0, SDL_MapRGB(m_Surface->format, g_CheckBoxBackgroundR, g_CheckBoxBackgroundG, g_CheckBoxBackgroundB));

	SetPosition(x, y, width, height);

	m_Image = new CSurface(ImagePath("CheckBoxCheck.png"));
	m_Image->SetAlpha(true);

	m_Font.LoadFont(cfg.fonts.normal(), fontsize);
	m_Font.SetText(text);
	m_Font.SetColor(0, 0, 0);
	m_Font.LeftOrRight(leftorright);

	m_Next = 0;
	return true;
}

void cCheckBox::Draw()
{
	if (m_Disabled) return;

	int off = (m_Font.LeftOrRight() ? m_Font.GetWidth() : m_Width) + 4;
	// Draw the window
	SDL_Rect offset;
	offset.x = m_XPos + (m_Font.LeftOrRight() ? off : 0);
	offset.y = m_YPos;
	SDL_BlitSurface(m_Border, 0, g_Graphics.GetScreen(), &offset);

	offset.x = m_XPos + (m_Font.LeftOrRight() ? off + 1 : 1);
	offset.y = m_YPos + 1;
	SDL_BlitSurface(m_Surface, 0, g_Graphics.GetScreen(), &offset);

	if (m_StateOn == true)
	{
		SDL_Rect rect;
		rect.y = rect.x = 0;
		rect.w = m_Width;
		rect.h = m_Height;
		m_Image->DrawSurface(m_XPos + (m_Font.LeftOrRight() ? off : 0), m_YPos, 0, &rect, true);
	}
	m_Font.DrawText(m_XPos + (m_Font.LeftOrRight() ? 0 : off), m_YPos);
}

void cCheckBox::ButtonClicked(int x, int y)
{
	if (m_Disabled) return;

	int off = (m_Font.LeftOrRight() ? m_Font.GetWidth() : m_Width) + 4;
	bool over = false;
	if (m_Font.LeftOrRight())
	{
		if (x > m_XPos + off && y > m_YPos && x < m_XPos + off + m_Width && y < m_YPos + m_Height)
			over = true;
	}
	else
	{
		if (x > m_XPos && y > m_YPos && x < m_XPos + m_Width && y < m_YPos + m_Height)
			over = true;
	}

	if (over)
	{
		m_StateOn = !m_StateOn;
		g_InterfaceEvents.AddEvent(EVENT_CHECKBOXCLICKED, m_ID);
	}
}
