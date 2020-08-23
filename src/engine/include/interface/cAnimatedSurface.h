/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Development Team are defined as the game's coders 
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
#include <vector>
#include <chrono>
#include "interface/fwd.hpp"
#include "interface/cSurface.h"

struct sAnimationFrame {
    cSurface surface;
    int delay;
};

// Class to hold the data for a single animation within an image
class cAnimatedSurface
{
public:
    cAnimatedSurface() = default;
    cAnimatedSurface(std::string file_name, std::vector<sAnimationFrame>);

    explicit operator bool() const { return !m_Frames.empty(); }

    // Drawing functions
    void DrawSurface(int x, int y, SDL_Rect* clip = nullptr) const;
    void UpdateFrame();

    std::string GetFileName() const { return m_FileName; }
private:
    std::chrono::high_resolution_clock::time_point m_LastTime {};
    std::vector<sAnimationFrame> m_Frames;

    int m_CurrentFrame = 0;                // Current frame in a playing animation

    std::string m_FileName;
};
