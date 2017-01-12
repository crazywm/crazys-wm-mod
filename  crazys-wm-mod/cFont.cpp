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
#include <iostream>
#include "cFont.h"
#include "CLog.h"
#include "CGraphics.h"
#include "sConfig.h"
#include "Globals.h"
#include <vector>
using namespace std;

extern CLog g_LogFile;
extern CGraphics g_Graphics;

float FontScale = 1.0f;

cFont::cFont()
{
	m_Font = 0;
	m_MultilineMessage = m_Message = 0;
	m_Text = "";
	m_NewText = true;
	m_IsMultiline = false;
	m_Height = m_Width = 0;
	m_Lineskip = 0;
	m_NumLines = 0;
	m_LeftOrRight = false;
}

cFont::~cFont()
{
	Free();
}

void cFont::SetText(string text)
{
	m_NewText = true;
	m_Text = text;
}

// ok this works by separating strings lines and storing each line into a vector
// then it creates a surface capable of fitting all the lines with the correct width
// it then blits the lines of text to this surface so it is then ready to be drawn as normal
void cFont::RenderMultilineText(string text)
{
	if (m_NewText == false && m_MultilineMessage != 0) return;
	if (text == "" && m_Text == "") return;
	if (text == "") text = m_Text;
	text = UpdateLineEndings(text);

	// first separate into lines according to width
	vector<string> lines;
	string temp(text);	// current line of text
	temp += " ";	// makes sure that all the text will be displayed
	int n = 0;	// current index into the string
	int q = 0;	// the next \n int the string
	int p = 0;	// holds the previous index into the string
	int charwidth, charheight;
	int width = m_Width - 10;  // pad the sides a bit, it was otherwise slightly overflowing

	// -- Get until either ' ' or '\n' 
	while (n != -1)
	{
		string strSub;
		n = temp.find(" ", p + 1);		// -- Find the next " "
		q = temp.find("\n", p + 1);		// -- Find the next "\n"
		if (q < n && q != -1)
		{
			strSub = temp.substr(0, q);
			GetSize(strSub, charwidth, charheight);
			if (charwidth >= width || q == -1)
			{
				strSub = temp.substr(0, p);
				lines.push_back(strSub);	// -- Puts strSub into the lines vector
				if (q != -1) temp = temp.substr(p + 1, string::npos);
				p = 0;
			}
			else
			{
				strSub = temp.substr(0, q);
				lines.push_back(strSub);
				if (q != -1) temp = temp.substr(q + 1, string::npos);
				p = 0;
			}
		}
		else
		{
			strSub = temp.substr(0, n);
			GetSize(strSub, charwidth, charheight);
			if (charwidth >= width || n == -1)
			{
				strSub = temp.substr(0, p);
				lines.push_back(strSub);	// -- Puts strSub into the lines vector
				if (n != -1) temp = temp.substr(p + 1, string::npos);
				p = 0;
			}
			else
				p = n;
		}
	}

	m_Lineskip = GetFontLineSkip();
	int height = lines.size()*m_Lineskip;

	if (m_MultilineMessage) SDL_FreeSurface(m_MultilineMessage);
	m_MultilineMessage = 0;

	// create a surface to render all the text too
	m_MultilineMessage = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
	SDL_SetAlpha(m_MultilineMessage, SDL_SRCALPHA, SDL_ALPHA_TRANSPARENT);

	string otext = m_Text;
	m_NumLines = lines.size();
	for (unsigned int i = 0; i < m_NumLines; i++)
	{
		SetText(lines[i].c_str());
		DrawText(0, i*m_Lineskip, m_MultilineMessage, true);
	}
	m_Text = otext;

	if (m_Message) SDL_FreeSurface(m_Message);
	m_Message = 0;
	m_NewText = false;
}

void cFont::RenderText(string text, bool multi)
{
	cConfig cfg;
	if (m_NewText == false && m_Message != 0) return;
	if (m_Font == 0)
	{
		if (cfg.debug.log_fonts())
		{
			g_LogFile.ss() << "Error rendering font string: " << text << endl;
			g_LogFile.ssend();
		}
		return;
	}
	if (m_Message) SDL_FreeSurface(m_Message);
	m_Message = 0;
	if ((m_Message != 0) && (text == "")) return;
	if (text == "") text = m_Text;
	m_Message = (cfg.fonts.antialias())
		? TTF_RenderText_Blended(m_Font, text.c_str(), m_TextColor)
		: m_Message = TTF_RenderText_Solid(m_Font, text.c_str(), m_TextColor);

	if (m_Message == 0) // `J` this log is useless - commenting it out
	{
		// g_LogFile.write("Error in RenderText m_Message. Text which was to be rendered: " + text);
		// g_LogFile.write(TTF_GetError());
		return;
	}
	else if (multi) SDL_SetAlpha(m_Message, 0, 0xFF);
	m_NewText = false;
}

int cFont::GetWidth()
{
	if (!m_IsMultiline)
	{
		RenderText();
		return m_Message->w;
	}
	else
	{
		RenderMultilineText("");
		return m_MultilineMessage->w;
	}
}

int cFont::GetHeight()
{
	if (!m_IsMultiline)
	{
		RenderText();
		return m_Message->h;
	}
	else
	{
		RenderMultilineText("");
		return m_MultilineMessage->h;
	}
}

bool cFont::DrawText(int x, int y, SDL_Surface* destination, bool multi)
{
	if (m_Text == "") return true;
	if (!m_Font) return false;
	RenderText("", multi);
	if (m_Message)
	{
		SDL_Rect offset;
		offset.x = x;
		offset.y = y;
		// Draw the source surface onto the destination
		int ret = 0;
		ret = (destination)
			? SDL_BlitSurface(m_Message, 0, destination, &offset)
			: SDL_BlitSurface(m_Message, 0, g_Graphics.GetScreen(), &offset);
		if (ret == -1)
		{
			g_LogFile.ss() << "Error bliting string" << endl;
			g_LogFile.ssend();
			return false;
		}
	}
	return true;
}

bool cFont::DrawMultilineText(int x, int y, int linesToSkip, int offsetY, SDL_Surface* destination)
{
	if (m_Text == "") return true;
	if (!m_Font) return false;
	RenderMultilineText("");
	if (m_MultilineMessage)
	{
		SDL_Rect offset;
		offset.x = x + 5;  // pad the sides a bit, it was otherwise slightly overflowing
		offset.y = y;
		offset.w = m_Width - 10;  // likewise
		offset.h = (m_MultilineMessage->h < m_Height) ? m_MultilineMessage->h : m_Height;

		SDL_Rect srcRect;
		srcRect.x = 0;
		srcRect.y = linesToSkip*m_Lineskip;
		srcRect.y += offsetY;
		srcRect.h = m_Height;
		srcRect.w = m_Width;

		// Draw the source surface onto the destination
		int ret = 0;
		ret = (destination)
			? SDL_BlitSurface(m_MultilineMessage, &srcRect, destination, &offset)
			: SDL_BlitSurface(m_MultilineMessage, &srcRect, g_Graphics.GetScreen(), &offset);
		if (ret == -1)
		{
			g_LogFile.ss() << "Error bliting string" << endl;
			g_LogFile.ssend();
			return false;
		}
	}
	return true;
}

void cFont::SetColor(unsigned char r, unsigned char g, unsigned char b)
{
	m_TextColor.r = r;
	m_TextColor.g = g;
	m_TextColor.b = b;
}

/*
 * old version for reference (and in case I balls it up
 */
bool cFont::LoadFont(string font, int size)
{
	cConfig cfg;
	if (m_Font) TTF_CloseFont(m_Font);
	m_Font = 0;
	if (cfg.debug.log_fonts()) std::cerr << "loading font: '" << font << "' at size " << size << endl;

	FontScale = (cfg.resolution.fixedscale() ? _G.g_ScreenScaleY : 1.0f);

	int t = int((float)size * FontScale);
	if (FontScale < 1.0f) t += 1;

	if ((m_Font = TTF_OpenFont(font.c_str(), t)) == 0)
	{
		g_LogFile.write("Error in LoadFont for font file: " + font);
		g_LogFile.write(TTF_GetError());
		return false;
	}
	return true;
}

void cFont::Free()
{
	if (m_Message) SDL_FreeSurface(m_Message);
	m_Message = 0;
	if (m_MultilineMessage) SDL_FreeSurface(m_MultilineMessage);
	m_MultilineMessage = 0;
	m_Font = 0;
}

string cFont::UpdateLineEndings(string text)
{
#ifndef LINUX
	// for Windows, double "\n \n" newline characters were showing up as one newline and a boxy (bad) character...
	// so, here's a cheap-ass workaround to add a "\r" carriage return in front of each "\n" for Windows
	int pos = text.find("\n", 0);
	while (pos != string::npos)
	{
		text.insert(pos, "\r");
		pos = text.find("\n", pos + 2);
	}
#endif
	return text;
}
