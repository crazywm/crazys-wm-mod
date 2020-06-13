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
#pragma once

#include <string>
#include <memory>
#include <SDL_video.h>
#include "fwd.hpp"
#include "cSurface.h"

class cFont
{
public:
    explicit cFont(CGraphics* gfx);

    void SetColor(unsigned char r, unsigned char g, unsigned char b);
    bool LoadFont(const std::string& font, int size);
    void GetSize(const std::string& text, int &width, int &height) const;

    int GetFontHeight();    // returns the height in pixels of the font
    int GetFontLineSkip() const;    // returns the number of pixels you should have between lines
    void SetFontBold(bool Bold = true);

    cSurface RenderText(std::string text) const;
    cSurface RenderMultilineText(std::string text, int width) const;
private:
    ttf_font_ptr m_Font    = nullptr;
    SDL_Color m_TextColor;

    static std::string UpdateLineEndings(std::string text) ;  // added function to fix line endings ("/n"=>"/r/n") for Windows

    CGraphics* m_GFX = nullptr;
};

