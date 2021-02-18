/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Development Team are defined as the game's coders 
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

#include "interface/cAnimatedSurface.h"
#include "interface/cSurface.h"
#include "CLog.h"
#include "interface/cImageCache.h"
#include <chrono>

cAnimatedSurface::cAnimatedSurface(std::string file_name, std::vector<sAnimationFrame> frames) :
    m_FileName(std::move(file_name)),
    m_Frames(std::move(frames))
{
}

void cAnimatedSurface::DrawSurface(int x, int y, SDL_Rect * clip) const
{
    if(m_Frames.empty())
        return;

    m_Frames[m_CurrentFrame].surface.DrawSurface(x, y , clip);
}

void cAnimatedSurface::UpdateFrame()
{
    if (m_LastTime.time_since_epoch().count() == 0)
    {
        m_LastTime = std::chrono::high_resolution_clock::now();
    }
    // TODO improve time keeping, once i now more about when UpdateFrame will be called
    else if ((std::chrono::high_resolution_clock::now() - m_LastTime) >= std::chrono::milliseconds(m_Frames[m_CurrentFrame].delay)) {
        m_CurrentFrame++;

        if (m_CurrentFrame >= m_Frames.size()) {
            m_CurrentFrame = 0;
        }

        m_LastTime = std::chrono::high_resolution_clock::now();
    }
}

int cAnimatedSurface::GetWidth() const {
    return m_Frames.at(m_CurrentFrame).surface.GetWidth();
}

int cAnimatedSurface::GetHeight() const {
    return m_Frames.at(m_CurrentFrame).surface.GetHeight();
}
