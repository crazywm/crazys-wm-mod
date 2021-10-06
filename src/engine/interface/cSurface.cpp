/*
* Copyright 2009, 2010, The Pink Petal Development Team.
* The Pink Petal Devloment Team are defined as the game's coders
* who meet on http://pinkpetal.org
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
#include "interface/cSurface.h"
#include "CLog.h"
#include "interface/CGraphics.h"
#include "interface/sColor.h"
#include <SDL_image.h>
#include <algorithm>
#include "utils/DirPath.h"


cSurface::cSurface(std::shared_ptr<cCachedSurface> surface, CGraphics* gfx) :
        m_Surface(std::move(surface)),
        m_GFX(gfx)
{
}


void cSurface::DrawSurface(int x, int y, SDL_Rect * clip) const
{
    if(!m_Surface) return;
    SDL_Rect offset;
    offset.x = x;
    offset.y = y;
    SDL_BlitSurface(m_Surface->surface(), clip, m_GFX->GetScreen(), &offset);
}

int cSurface::GetWidth() const
{
    return m_Surface->width();
}

int cSurface::GetHeight() const
{
    return m_Surface->height();
}

const std::shared_ptr<cCachedSurface>& cSurface::RawSurface() const
{
    return m_Surface;
}

cSurface cSurface::BlitOther(const cSurface& source, SDL_Rect * src_rect, SDL_Rect * dst_rect) const
{
    return m_GFX->GetImageCache().BlitSurface(*this, dst_rect, source, src_rect);
}

cSurface cSurface::FillRect(SDL_Rect area, sColor color) const
{
    return m_GFX->GetImageCache().FillRect(*this, area, color);
}

const std::string& cSurface::GetFileName() const
{
    return m_Surface->name();
}

cImageCache* cSurface::GetCache()
{
    return &m_GFX->GetImageCache();
}

void cSurface::DrawScaled(int x, int y, int width, int height, SDL_Rect* clip) const {
    if(!m_Surface) return;
    SDL_Rect offset;
    offset.x = x;
    offset.y = y;
    offset.w = width;
    offset.h = height;
    SDL_BlitScaled(m_Surface->surface(), clip, m_GFX->GetScreen(), &offset);
}
