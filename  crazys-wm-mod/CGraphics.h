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

#include "CTimer.h"
#include <string>
#include <memory>
#include <vector>
#include <map>

class SDL_Surface;
class CSurface;

class CGraphics
{
public:
	CGraphics();
	~CGraphics();

	bool InitGraphics(std::string caption, int Width = 0, int Height = 0, int BPP = 32);
	void Free();
	bool End();		// End Drawing Stuff
	void Begin();	// begins drawing stuff

	// Drawing helpers
	int BlitSurface(SDL_Surface *src, SDL_Rect *srcrect, SDL_Rect *dstrect) const;

	// Accessors
	SDL_Surface* GetScreen() const {return m_Screen;}
	unsigned int GetTicks() const {return m_CurrentTime;}

	// absolute screen size
	int GetWidth() const {return m_ScreenWidth;}
	int GetHeight() const {return m_ScreenHeight;}

	float GetScaleX() const { return m_ScreenScaleX;}
	float GetScaleY() const { return m_ScreenScaleY;}

	// Gets a pointer to a brothel image
	CSurface* LoadBrothelImage(const std::string& name);

private:
    // scaling
    bool m_Fullscreen = false;
    float m_ScreenScaleX = 1.0f;
    float m_ScreenScaleY = 1.0f;

	// Screen
	SDL_Surface* m_Screen = nullptr;

	// Images
    std::unique_ptr<CSurface> m_BackgroundImage;
    std::map<std::string, std::unique_ptr<CSurface>> m_BrothelImages;

	// screen attributes
	int m_ScreenWidth;
	int m_ScreenHeight;
	int m_ScreenBPP;

	// Time
	unsigned int m_CurrentTime;
	CTimer m_FPS;
};

#endif
