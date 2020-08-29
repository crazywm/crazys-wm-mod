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

#ifndef CRAZYS_WM_MOD_FWD_HPP
#define CRAZYS_WM_MOD_FWD_HPP

#include <memory>

extern "C" {
struct SDL_Surface;
struct SDL_Texture;
struct SDL_Rect;
struct AG_Frame;
typedef struct _TTF_Font TTF_Font;
}

// our own classes
class cFont;
class CGraphics;
class cInterfaceWindow;
class cAnimatedSurface;
class cWindowManager;
class cSurface;
class sColor;

// some useful global stuff
void InitInterface(CGraphics* g, std::string theme);
void ShutdownInterface();

cWindowManager& window_manager();

#endif //CRAZYS_WM_MOD_FWD_HPP
