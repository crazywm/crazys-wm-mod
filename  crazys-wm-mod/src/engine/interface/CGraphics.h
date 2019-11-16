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
#ifndef __CGRAPHICS_H
#define __CGRAPHICS_H

#include <string>
#include <memory>
#include <vector>
#include <map>
#include "cImageCache.hpp"

class SDL_Surface;
class cSurface;
class sColor;
class cTimer;

class CGraphics
{
public:
	CGraphics();
	~CGraphics();

	bool InitGraphics(std::string caption, int Width = 0, int Height = 0, int BPP = 32);
	bool End();		// End Drawing Stuff
	void Begin();	// begins drawing stuff

    // pass through to image cache
    cImageCache& GetImageCache() { return m_ImageCache; }
    cSurface CreateSurface(int width, int height, sColor color, bool transparent=false);
    cSurface LoadImage(std::string filename, int width=-1, int height=-1, bool transparency=false, bool keep_ratio=true);

	// Accessors
	SDL_Surface* GetScreen() const {return m_Screen;}

	// absolute screen size
	int GetWidth() const {return m_ScreenWidth;}
	int GetHeight() const {return m_ScreenHeight;}

	float GetScaleX() const { return m_ScreenScaleX;}
	float GetScaleY() const { return m_ScreenScaleY;}

	cFont LoadFont(const std::string& font, int size);

private:
    // scaling
    bool m_Fullscreen = false;
    float m_ScreenScaleX = 1.0f;
    float m_ScreenScaleY = 1.0f;

	// Screen
	SDL_Surface* m_Screen = nullptr;

	// Images
    cSurface m_BackgroundImage;
    cImageCache m_ImageCache;

	// screen attributes
	int m_ScreenWidth;
	int m_ScreenHeight;
	int m_ScreenBPP;

	// Time
	unsigned int m_CurrentTime;
	std::unique_ptr<cTimer> m_FPS;
};

#endif
