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

float FontScale = 1.0f;

cFont::cFont(CGraphics* gfx) : m_GFX(gfx)
{
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
bool cFont::LoadFont(const std::string& font, int size)
{
    cConfig cfg;
    m_Font = nullptr;
    if (cfg.debug.log_fonts()) std::cerr << "loading font: '" << font << "' at size " << size << std::endl;

    FontScale = (cfg.resolution.fixedscale() ? m_GFX->GetScaleY() : 1.0f);

    int t = int((float)size * FontScale);
    if (FontScale < 1.0f) t += 1;
    m_Font.reset(TTF_OpenFont(font.c_str(), t));
    if (!m_Font)
    {
        g_LogFile.write("Error in LoadFont for font file: " + font);
        g_LogFile.write(TTF_GetError());
        return false;
    }
    return true;
}

std::string cFont::UpdateLineEndings(std::string text)
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
    return std::move(text);
}

int cFont::GetFontHeight()
{
    return TTF_FontHeight(m_Font.get());
}

int cFont::GetFontLineSkip() const
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

void cFont::GetSize(const std::string& text, int& width, int& height) const
{
    TTF_SizeText(m_Font.get(), text.c_str(), &width, &height);
}

cSurface cFont::RenderText(std::string text) const
{
    cConfig cfg;
    return m_GFX->GetImageCache().CreateTextSurface(m_Font.get(), std::move(text),
                                                    sColor(m_TextColor.r, m_TextColor.g, m_TextColor.b), cfg.fonts.antialias());

}

cSurface cFont::RenderMultilineText(std::string text, int width) const
{
    text = UpdateLineEndings(std::move(text));

    // first separate into lines according to width
    std::vector<std::string> lines;
    std::string temp(text);	// current line of text
    temp += " ";	// makes sure that all the text will be displayed
    int n = 0;	// current index into the string
    int q = 0;	// the next \n int the string
    int p = 0;	// holds the previous index into the string
    int charwidth, charheight;
    width = width - 10;  // pad the sides a bit, it was otherwise slightly overflowing

    // -- Get until either ' ' or '\n'
    while (n != -1)
    {
        std::string strSub;
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
                temp = temp.substr(p + 1, std::string::npos);
                p = 0;
            }
            else
            {
                strSub = temp.substr(0, q);
                lines.push_back(strSub);
                temp = temp.substr(q + 1, std::string::npos);
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
                if (n != -1) temp = temp.substr(p + 1, std::string::npos);
                p = 0;
            }
            else
                p = n;
        }
    }

    int lineskip = GetFontLineSkip();
    int height = lines.size()*lineskip;

    // create a surface to render all the text too
    auto message = m_GFX->CreateSurface(width, height, sColor(0xff, 0, 0), true);
    //SDL_SetAlpha(m_MultilineMessage.RawSurface()->surface(), SDL_SRCALPHA, SDL_ALPHA_OPAQUE);

    cConfig cfg;
    for (unsigned int i = 0; i < lines.size(); i++)
    {
        if(!lines[i].empty()) {
            auto line = m_GFX->GetImageCache().CreateTextSurface(m_Font.get(), lines[i],
                                                                 sColor(m_TextColor.r, m_TextColor.g,
                                                                        m_TextColor.b),
                                                                 cfg.fonts.antialias());
            // TODO if the text was empty (only control characters), skip rendering
            if(!line)
                continue;
            SDL_SetAlpha(line.RawSurface()->surface(), 0, SDL_ALPHA_OPAQUE);
            SDL_Rect dst = {0, static_cast<Sint16>(i * lineskip),
                            static_cast<Uint16>(line.GetWidth()), static_cast<Uint16>(line.GetHeight())};
            message = message.BlitOther(line, nullptr, &dst);
            SDL_SetAlpha(line.RawSurface()->surface(), SDL_SRCALPHA, SDL_ALPHA_OPAQUE);
        }
    }
    return std::move(message);
}


void FontDeleter::operator()(TTF_Font * font)
{
    if(font)
        TTF_CloseFont(font);
}
