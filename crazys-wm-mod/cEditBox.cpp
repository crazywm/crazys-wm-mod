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
#include "cEditBox.h"
#include "CGraphics.h"
#include "sConfig.h"

extern CGraphics g_Graphics;

extern cConfig cfg;

extern unsigned char g_EditBoxBorderR, g_EditBoxBorderG, g_EditBoxBorderB;
extern unsigned char g_EditBoxBackgroundR, g_EditBoxBackgroundG, g_EditBoxBackgroundB;
extern unsigned char g_EditBoxSelectedR, g_EditBoxSelectedG, g_EditBoxSelectedB;
extern unsigned char g_EditBoxTextR, g_EditBoxTextG, g_EditBoxTextB;

cEditBox::~cEditBox()
{
	if(m_Background)
		SDL_FreeSurface(m_Background);
	m_Background = 0;

	if(m_FocusedBackground)
		SDL_FreeSurface(m_FocusedBackground);
	m_FocusedBackground = 0;

	if(m_Border)
		SDL_FreeSurface(m_Border);
	m_Border = 0;

	if(m_Text)
		delete m_Text;
	m_Text= 0;

	if(m_Next)
		delete m_Next;
	m_Next = 0;
}

void cEditBox::Draw()
{
	if (m_Hidden) { return; }

	if(m_Background && m_Border)
	{
		// Draw the window
		SDL_Rect offset;
		offset.x = m_XPos;
		offset.y = m_YPos;

		// blit to the screen
		SDL_BlitSurface(m_Border, 0, g_Graphics.GetScreen(), &offset);

		offset.x = m_XPos+m_BorderSize;
		offset.y = m_YPos+m_BorderSize;

		if(m_HasFocus)
			SDL_BlitSurface(m_FocusedBackground, 0, g_Graphics.GetScreen(), &offset);
		else
			SDL_BlitSurface(m_Background, 0, g_Graphics.GetScreen(), &offset);
	}

	// draw the text
	m_Text->DrawText(m_XPos+m_BorderSize+1, m_YPos+m_BorderSize+1);
}

bool cEditBox::CreateEditBox(int ID, int x, int y, int width, int height, int BorderSize, int FontSize)
{
	m_BorderSize = BorderSize;
	SetPosition(x,y,width,height);
	m_Border = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, 0,0,0,0);
	SDL_FillRect(m_Border,0,SDL_MapRGB(m_Border->format,g_EditBoxBorderR,g_EditBoxBorderG,g_EditBoxBorderB));
		
	m_Background = SDL_CreateRGBSurface(SDL_SWSURFACE, width-(BorderSize*2), height-(BorderSize*2), 32, 0,0,0,0);
	SDL_FillRect(m_Background,0,SDL_MapRGB(m_Background->format,g_EditBoxBackgroundR,g_EditBoxBackgroundG,g_EditBoxBackgroundB));

	m_FocusedBackground = SDL_CreateRGBSurface(SDL_SWSURFACE, width-(BorderSize*2), height-(BorderSize*2), 32, 0,0,0,0);
	SDL_FillRect(m_FocusedBackground,0,SDL_MapRGB(m_Background->format,g_EditBoxSelectedR,g_EditBoxSelectedG,g_EditBoxSelectedB));

	m_Text = new cFont();
	m_Text->LoadFont(cfg.fonts.normal(), FontSize);
	m_Text->SetText("");
	m_Text->SetColor(g_EditBoxTextR,g_EditBoxTextG,g_EditBoxTextB);
	m_ID = ID;

	return true;
}

bool cEditBox::IsOver(int x, int y)
{
	bool over = false;
	if(x > m_XPos && y > m_YPos && x < m_XPos+m_Width && y < m_YPos+m_Height)
		over = true;

	return over;
}

bool cEditBox::OnClicked(int x, int y)
{
	if(IsOver(x,y))
	{
		m_HasFocus = true;
//		g_InterfaceEvents.AddEvent(EVENT_BUTTONCLICKED, m_ID);
		return true;
	}
	else
		m_HasFocus = false;

	return false;
}

void cEditBox::ClearText()
{
	if(m_Text)
		m_Text->SetText("");
}

void cEditBox::UpdateText(char key, bool upper)
{
	string text = m_Text->GetText();
	if(key == '-')
	{
		if(text.length() > 0)
		{
			text.erase(text.length()-1);
			m_Text->SetText(text);
		}
		return;
	}
	if(upper)
		key = (char)toupper((int)key);
	text += key;
	int w = 0, h = 0;
	m_Text->GetSize(text,w,h);
	if(w > m_Width)
		return;
	m_Text->SetText(text);
}
