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
#include "interface/fwd.hpp"

class cImageCache;
class cCachedSurface;

class cSurface {
public:
    cSurface() = default;
    cSurface(std::shared_ptr<cCachedSurface> surface, CGraphics* gfx);

    // Infos
    int GetWidth() const;
    int GetHeight() const;
    const std::string& GetFileName() const;
    const std::shared_ptr<cCachedSurface>& RawSurface() const;

    cImageCache* GetCache();

    explicit operator bool() const { return m_Surface.get(); }

    // Drawing functions
    void DrawSurface(int x, int y, SDL_Rect* clip = nullptr) const;
    void DrawScaled(int x, int y, int width, int height, SDL_Rect* clip = nullptr) const;

    cSurface FillRect(SDL_Rect area, sColor color) const;
    cSurface BlitOther(const cSurface& source, SDL_Rect* src_rect=nullptr, SDL_Rect* dst_rect=nullptr) const;
private:
    std::shared_ptr<cCachedSurface> m_Surface;
    CGraphics* m_GFX = nullptr;
};