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

cTextItem::cTextItem()
{
	m_Next = 0;
	m_ScrollBar = 0;
	m_Hide = false;
	m_AutoScrollBar = true;
	m_ForceScrollBar = false;
	m_ScrollChange = 0;
}
cTextItem::~cTextItem()
{ 
	if (m_Next) delete m_Next;
	m_Next = 0;
}

void cTextItem::CreateTextItem(int ID, int x, int y, int width, int height, string text, int size, bool auto_scrollbar,
	bool force_scrollbar , bool leftorright , int red , int green , int blue )
{
	m_ID = ID;
	m_FontHeight = size;
	SetPosition(x, y, width, height);

	SetText(text);
	ChangeFontSize(size, red, green, blue);

	m_AutoScrollBar = auto_scrollbar;
	m_ForceScrollBar = force_scrollbar;
}

void cTextItem::DisableAutoScroll(bool disable) { m_AutoScrollBar = !disable; }
void cTextItem::ForceScrollBar(bool force) { m_ForceScrollBar = force; }

// does scrollbar exist, but current text fits, and scrollbar isn't being forced?
bool cTextItem::NeedScrollBarHidden() { return (m_ScrollBar && !m_ScrollBar->m_Hidden && HeightTotal() <= GetHeight() && !m_ForceScrollBar); }
// does scrollbar exist but is hidden, and current text doesn't fit?
bool cTextItem::NeedScrollBarShown() { return (m_ScrollBar && m_ScrollBar->m_Hidden && HeightTotal() > GetHeight()); }
// does a scrollbar need to be added?
bool cTextItem::NeedScrollBar() { return (!m_ScrollBar && GetHeight() > 47 && (HeightTotal() > GetHeight() || m_ForceScrollBar)); }
int cTextItem::HeightTotal() { return m_Font.GetHeight(); }
void cTextItem::MouseScrollWheel(int x, int y, bool ScrollDown )
{
	if (m_ScrollBar && !m_ScrollBar->m_Hidden && IsOver(x, y))
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
	if (m_ScrollBar && !m_ScrollBar->m_Hidden)
		m_ScrollBar->SetTopValue(0);
}

void cTextItem::Draw()
{
	if (m_Hide) return;
	if (m_Text == "") return;

#if 0	// draw visible box showing exact area covered by TextItem; for debug usage, disabled by default
	SDL_Rect dstRect;
	dstRect.x = m_XPos;
	dstRect.y = m_YPos;
	dstRect.h = m_Height;
	dstRect.w = m_Width;
	SDL_FillRect(g_Graphics.GetScreen(), &dstRect, SDL_MapRGB(g_Graphics.GetScreen()->format, 200, 255, 255));
#endif

	m_Font.DrawMultilineText(m_XPos, m_YPos, 0, m_ScrollChange);

	/*int position = 0;
	for(int i=0; i<m_LinesPerBox; i++)
	{
	char buffer[9000];
	int j;
	bool newline = false;
	bool end = false;
	int bufferPos = 0;

	// copy all characters accross
	for(j=0; j<m_CharsPerLine; j++)
	{
	if(position+j >= (signed int) m_Text.length())
	{
	buffer[bufferPos+j] = '\0';
	end = true;

	m_Font.SetText(buffer);
	m_Font.DrawText(m_XPos, m_YPos+(i*m_CharHeight)+1);

	break;
	}

	if(m_Text[position+j] == '\n')	// create new line when \n is encountered
	{
	position += j+1;

	buffer[bufferPos+j] = '\0';

	m_Font.SetText(buffer);
	m_Font.DrawText(m_XPos, m_YPos+(i*m_CharHeight)+1);

	newline = true;
	break;
	}

	buffer[bufferPos+j] = m_Text[position+j];
	}

	if(end)
	break;

	if(newline)
	continue;

	bufferPos = j;
	position += j;

	// check for any half done words
	if(position+1 < (signed int) m_Text.length())
	{
	if((m_Text[position] != '\n' || m_Text[position] != ' ') && (m_Text[position+1] != '\n' || m_Text[position+1] != ' '))
	{
	while(((position+1) && m_Text[position] != '\n' && m_Text[position] != ' ' ))
	{
	buffer[bufferPos] = '\0';
	bufferPos--;
	position--;
	}
	buffer[bufferPos] = '\0';
	}
	else
	buffer[bufferPos] = '\0';
	}
	else
	buffer[bufferPos] = '\0';

	m_Font.SetText(buffer);
	m_Font.DrawText(m_XPos, m_YPos+(i*m_CharHeight)+1);
	}*/
}

void cTextItem::hide() { m_Hide = true; if (m_ScrollBar && !m_ScrollBar->m_Hidden) m_ScrollBar->hide(); }
void cTextItem::unhide() { m_Hide = false; if (m_ScrollBar && m_ScrollBar->m_Hidden) m_ScrollBar->unhide(); }
