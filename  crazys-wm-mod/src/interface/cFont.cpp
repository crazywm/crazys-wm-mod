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
#include <SDL_ttf.h>
#include "CLog.h"
#include "interface/CGraphics.h"
#include "sConfig.h"
#include <vector>
#include "cColor.h"
using namespace std;

extern CGraphics g_Graphics;

float FontScale = 1.0f;

cFont::cFont()
{
	m_Font = nullptr;
	m_Text = "";
	m_NewText = true;
	m_IsMultiline = false;
	m_Height = m_Width = 0;
	m_Lineskip = 0;
	m_NumLines = 0;
	m_LeftOrRight = false;
}

void cFont::SetText(string text)
{
    if(text != m_Text) {
        m_NewText = true;
        m_Text    = std::move(text);
    }
}

// ok this works by separating strings lines and storing each line into a vector
// then it creates a surface capable of fitting all the lines with the correct width
// it then blits the lines of text to this surface so it is then ready to be drawn as normal
void cFont::RenderMultilineText()
{
	if (!m_NewText && m_MultilineMessage) return;
	if (m_Text.empty()) return;
	if(!m_Font) {
	    throw std::logic_error("No font loaded!");
	}
	std::string text = UpdateLineEndings(m_Text);

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
		n = temp.find(' ', p + 1);		// -- Find the next " "
		q = temp.find('\n', p + 1);		// -- Find the next "\n"
		if (q < n && q != -1)
		{
			strSub = temp.substr(0, q);
			GetSize(strSub, charwidth, charheight);
			if (charwidth >= width)
			{
				strSub = temp.substr(0, p);
				lines.push_back(strSub);	// -- Puts strSub into the lines vector
                temp = temp.substr(p + 1, string::npos);
				p = 0;
			}
			else
			{
				strSub = temp.substr(0, q);
				lines.push_back(strSub);
                temp = temp.substr(q + 1, string::npos);
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

	// create a surface to render all the text too
	m_MultilineMessage = g_Graphics.CreateSurface(width, height, sColor(0xff, 0, 0), true);
	//SDL_SetAlpha(m_MultilineMessage.RawSurface()->surface(), SDL_SRCALPHA, SDL_ALPHA_OPAQUE);

	string otext = m_Text;
	m_NumLines = lines.size();
    cConfig cfg;
	for (unsigned int i = 0; i < m_NumLines; i++)
	{
	    if(!lines[i].empty()) {
            auto line = g_Graphics.GetImageCache().CreateTextSurface(m_Font.get(), lines[i],
                                                                     sColor(m_TextColor.r, m_TextColor.g,
                                                                            m_TextColor.b),
                                                                     cfg.fonts.antialias());
            if(!line)
                continue;

            SDL_SetAlpha(line.RawSurface()->surface(), 0, SDL_ALPHA_OPAQUE);
            SDL_Rect dst = {0, static_cast<Sint16>(i * m_Lineskip),
                            static_cast<Uint16>(line.GetWidth()), static_cast<Uint16>(line.GetHeight())};
            m_MultilineMessage = m_MultilineMessage.BlitOther(line, nullptr, &dst);
            SDL_SetAlpha(line.RawSurface()->surface(), SDL_SRCALPHA, SDL_ALPHA_OPAQUE);
        }
	}
	m_Text = otext;
	m_Message = cSurface();
	m_NewText = false;
}

void cFont::RenderText(bool multi)
{
	cConfig cfg;
	if (!m_NewText && m_Message) return;
	if (m_Font == nullptr)
	{
		if (cfg.debug.log_fonts())
		{
			g_LogFile.ss() << "Error rendering font string: " << m_Text << endl;
			g_LogFile.ssend();
		}
		return;
	}

	m_Message = g_Graphics.GetImageCache().CreateTextSurface(m_Font.get(), m_Text,
	        sColor(m_TextColor.r, m_TextColor.g, m_TextColor.b), cfg.fonts.antialias());
	m_NewText = false;
}

int cFont::GetWidth()
{
	if (!m_IsMultiline)
	{
        RenderText(false);
		return m_Message.GetWidth();
	}
	else
	{
        RenderMultilineText();
		return m_MultilineMessage.GetWidth();
	}
}

int cFont::GetHeight()
{
	if (!m_IsMultiline)
	{
        RenderText(false);
		return m_Message.GetHeight();
	}
	else
	{
        RenderMultilineText();
		return m_MultilineMessage.GetHeight();
	}
}

bool cFont::DrawText(int x, int y, bool multi)
{
	if (m_Text.empty()) return true;
	if (!m_Font) return false;
    RenderText(multi);
	if (m_Message)
	{
		SDL_Rect offset;
		offset.x = x;
		offset.y = y;
		// Draw the source surface
		m_Message.DrawSurface(x, y);
	}
	return true;
}

bool cFont::DrawMultilineText(int x, int y, int linesToSkip, int offsetY, SDL_Surface* destination)
{
	if (m_Text.empty()) return true;
	if (!m_Font) return false;
    RenderMultilineText();
	if (m_MultilineMessage)
	{
		SDL_Rect offset;
		offset.x = x + 5;  // pad the sides a bit, it was otherwise slightly overflowing
		offset.y = y;
		offset.w = m_Width - 10;  // likewise
		offset.h = (m_MultilineMessage.GetHeight() < m_Height) ? m_MultilineMessage.GetHeight() : m_Height;

		SDL_Rect srcRect;
		srcRect.x = 0;
		srcRect.y = linesToSkip*m_Lineskip;
		srcRect.y += offsetY;
		srcRect.h = m_Height;
		srcRect.w = m_Width;

		// Draw the source surface onto the destination
		m_MultilineMessage.DrawSurface(x+5, y, &srcRect);
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
	m_Font = nullptr;
	if (cfg.debug.log_fonts()) std::cerr << "loading font: '" << font << "' at size " << size << endl;

	FontScale = (cfg.resolution.fixedscale() ? g_Graphics.GetScaleY() : 1.0f);

	int t = int((float)size * FontScale);
	if (FontScale < 1.0f) t += 1;
    m_Font.reset(TTF_OpenFont(font.c_str(), t));
	if (!m_Font)
	{
		g_LogFile.ss() << "Error in LoadFont for font file: '" << font << "': " << TTF_GetError();
		g_LogFile.ssend();
		throw(std::runtime_error(TTF_GetError()));
		return false;
	}
	return true;
}

void cFont::Free()
{
	m_Font = nullptr;
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

int cFont::GetFontHeight()
{
    return TTF_FontHeight(m_Font.get());
}

int cFont::GetFontLineSkip()
{
    return TTF_FontLineSkip(m_Font.get());
}

void cFont::SetFontBold(bool Bold)
{
    TTF_SetFontStyle(m_Font.get(), (Bold ? TTF_STYLE_BOLD : TTF_STYLE_NORMAL) );
}

int cFont::IsFontFixedWidth()
{
    return TTF_FontFaceIsFixedWidth(m_Font.get());
}

void cFont::GetSize(const std::string& text, int& width, int& height)
{
    TTF_SizeText(m_Font.get(), text.c_str(), &width, &height);
}

void FontDeleter::operator()(TTF_Font * font)
{
    if(font)
        TTF_CloseFont(font);
}
