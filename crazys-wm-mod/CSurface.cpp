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
#include "CSurface.h"
#include "CLog.h"
#include "CGraphics.h"
#include "CResourceManager.h"
#include <SDL_image.h>
#include <algorithm>
#include "DirPath.h"

extern CLog g_LogFile;
extern CGraphics g_Graphics;
extern CResourceManager rmanager;
//extern sCachedSurfaces cache;

CSurface::CSurface()
{
	m_Temp = 0;
	m_Surface = 0;
	m_Next = m_Prev = 0;
	m_UseKey = false;
	m_Cached = false;
	//m_CachedLocation=0;
	//m_UseAlpha = false;
	// mod - trying to fix the problem where PNGs occasionally
	// ignore their alpha channels.
	m_UseAlpha = true;
	loaded = false;
	m_SaveSurface = false;
	m_ColoredSurface = false;
	m_SpriteImage = 0;
	m_Message = "";
}
CSurface::CSurface(SDL_Surface* inputsurface)
{
	m_Temp = 0;
	m_Surface = 0;
	m_Next = m_Prev = 0;
	m_UseKey = false;
	m_UseAlpha = true;
	m_Cached = false;
	m_Registered = false;
	Register(false);
	LoadSurface(inputsurface);
	m_SaveSurface = false;
	m_ColoredSurface = false;
	m_SpriteImage = 0;
	m_Message = "";

}
CSurface::CSurface(string filename)
{
	m_Temp = 0;
	m_Surface = 0;
	m_SpriteImage = 0;
	m_Filename = filename;
	m_Next = m_Prev = 0;
	m_UseKey = false;
	m_UseAlpha = true;
	loaded = false;
	m_Cached = false;
	m_Registered = false;
	Register(false);
	m_SaveSurface = false;
	m_ColoredSurface = false;
	m_Message = filename;
}
CSurface::~CSurface()
{
	m_SaveSurface = false;
	Free();
	g_LogFile.ss() << "'" << m_Filename << "' Freed"; g_LogFile.ssend();
}

void CSurface::Free()
{
	if (m_SaveSurface) return;
	if (m_Temp) SDL_FreeSurface(m_Temp);
	m_Temp = 0;
	if (m_Surface) SDL_FreeSurface(m_Surface);
	m_Surface = 0;
	if (m_SpriteImage) SDL_FreeSurface(m_SpriteImage);
	m_SpriteImage = 0;
}
void CSurface::FreeResources()
{
	if (m_ColoredSurface) return;
	Free();
}

void CSurface::Register(bool loaded)
{
	if (loaded) rmanager.AddResource(this, IMAGE_RESOURCE);
}

bool CSurface::LoadImage(string filename, bool load)
{
	m_Filename = filename;
	if (m_Filename == ButtonPath("").c_str()) return true;	// ignore disabled buttons
	if (load == false)
	{
		Register(false);
		return true;
	}
	SDL_Surface* loadedImage = 0;
	loadedImage = IMG_Load(filename.c_str());		// Load image

	if (loadedImage)
	{
		m_Surface = m_UseAlpha?						// Convert image to screen BPP
			SDL_DisplayFormatAlpha(loadedImage) :	// either with alpha
			SDL_DisplayFormat(loadedImage);			// or without alpha
		SDL_FreeSurface(loadedImage);				// Free old surface
	}
	else
	{
		g_LogFile.write("CSurface LoadImage loadedImage error with filename: " + m_Filename);
		g_LogFile.write(SDL_GetError());
		g_LogFile.write(IMG_GetError());
		return false;
	}

	if (!m_Surface)
	{
		g_LogFile.write("CSurface LoadImage !m_Surface error with filename: " + m_Filename);
		g_LogFile.write(SDL_GetError());
		g_LogFile.write(IMG_GetError());
		return false;
	}

	if (m_UseKey && !m_UseAlpha)
		SDL_SetColorKey(m_Surface, SDL_SRCCOLORKEY, m_ColorKey);
	if (!m_Cached)
		Register(true);

	return true;
}

bool CSurface::LoadSurface(SDL_Surface* inputsurface)
{
	SDL_Surface* loadedImage = inputsurface;

	m_Surface = m_UseAlpha ?					// Convert image to screen BPP
		SDL_DisplayFormatAlpha(loadedImage) :	// either with alpha
		SDL_DisplayFormat(loadedImage);			// or without alpha
	if (!m_Surface)
	{
		g_LogFile.write("CSurface LoadSurface !m_Surface error with filename: " + m_Filename);
		g_LogFile.write(SDL_GetError());
		g_LogFile.write(IMG_GetError());
		return false;
	}
	if (m_UseKey && !m_UseAlpha) SDL_SetColorKey(m_Surface, SDL_SRCCOLORKEY, m_ColorKey);
	if (!m_Cached) Register(true);
	loaded = true;
	return true;
}

void CSurface::SetColorKey(unsigned char r, unsigned char g, unsigned char b)
{
	m_ColorKey = SDL_MapRGB(g_Graphics.GetScreen()->format, r, g, b);
}

void CSurface::SetAlpha(bool UseAlpha)
{
	m_UseAlpha = UseAlpha;
}

bool CSurface::ResizeSprite(SDL_Surface* image, SDL_Rect* clip, bool maintainRatio)
{
	double scaleX, scaleY;
	if (m_SpriteImage) SDL_FreeSurface(m_SpriteImage);		// free old image
	m_SpriteImage = 0;
	

	if (maintainRatio == true)
	{
		if (clip->w - image->w <= clip->h - image->h)
			scaleX = scaleY = ((double)clip->w / (double)image->w);
		else	// assume the height is larger
			scaleX = scaleY = ((double)clip->h / (double)image->h);
		m_SpriteImage = zoomSurface(image, scaleX, scaleY, 1);
	}
	else
	{
		if (clip->w != image->w || clip->h != image->h)
		{
			scaleX = ((double)clip->w / (double)image->w);
			scaleY = ((double)clip->h / (double)image->h);
			m_SpriteImage = zoomSurface(image, scaleX, scaleY, 1);
		}
	}

	return true;
}

bool CSurface::DrawSprite(int x, int y)
{
	if (m_SpriteImage == 0)
	{
		g_LogFile.ss() << "ERROR - Draw Sprite, null surface"; g_LogFile.ssend();
		return false;
	}

	SDL_Rect offset;
	offset.x = x;
	offset.y = y;

	SDL_BlitSurface(m_SpriteImage, 0, g_Graphics.GetScreen(), &offset);

	return true;
}

bool CSurface::DrawSurface(int x, int y, SDL_Surface* destination, SDL_Rect* clip, bool resize, bool maintainRatio)
{
	double scaleX = 0;	double scaleY = 0; double deltaX = 0; double deltaY = 0;

	if (m_Surface == 0)
	{
		if (m_ColoredSurface == true)
		{
			g_LogFile.ss() << "ERROR - Colored surface null: '" << m_Filename << "'"; g_LogFile.ssend();
			return false;
		}

		if (m_Temp) SDL_FreeSurface(m_Temp);
		m_Temp = 0;
		if (!loaded)
		{
			if (m_Filename == ButtonPath("").c_str() || m_Filename == "")	// fix disabled buttons
				m_Filename = ImagePath("blank.png").c_str();
			if (!LoadImage(m_Filename))
			{
				g_LogFile.ss() << "ERROR - Loading Image '" << m_Filename << "'"; g_LogFile.ssend();
				return false;
			}
			if (m_Cached) loaded = true;
		}
	}

	m_TimeUsed = g_Graphics.GetTicks();

	if (clip && resize)
	{
		if (m_Temp == 0)
		{
			if (clip->w != m_Surface->w && clip->h != m_Surface->h)
			{
				scaleX = ((double)clip->w / (double)m_Surface->w);
				scaleY = ((double)clip->h / (double)m_Surface->h);
				if (maintainRatio == true)
				{
					// Use the most restrictive scale
					if (scaleX < scaleY) { scaleY = scaleX; }
					else { scaleX = scaleY; }
				}
				m_Temp = zoomSurface(m_Surface, scaleX, scaleY, 1);
			}
		}
		else
		{
			if (m_Temp->w != clip->w && m_Temp->h != clip->h)
			{
				if (m_Temp) SDL_FreeSurface(m_Temp);	// free old image
				m_Temp = 0;
				scaleX = ((double)clip->w / (double)m_Surface->w);
				scaleY = ((double)clip->h / (double)m_Surface->h);
				if (maintainRatio == true)
				{
					// Use the most restrictive scale
					if (scaleX < scaleY) { scaleY = scaleX; }
					else { scaleX = scaleY; }
				}
				m_Temp = zoomSurface(m_Surface, scaleX, scaleY, 1);
			}
		}
	}

	// if a m_Temp doesnt exist the image fits perfectly
	if (m_Temp)
	{
		deltaX = (clip->w - m_Temp->w) / 2;
		deltaY = (clip->h - m_Temp->h) / 2;
	}

	// create and setup a SDL offset rectangle
	SDL_Rect offset;
	offset.x = x + (int) deltaX;
	offset.y = y + (int) deltaY;

	// Draw the source surface onto the destination
	int error = 0;
	if (destination)
	{
		error = m_Temp ?
			SDL_BlitSurface(m_Temp, clip, destination, &offset) :
			SDL_BlitSurface(m_Surface, clip, destination, &offset);
	}
	else	// blit to the screen
	{
		error = m_Temp ?
			SDL_BlitSurface(m_Temp, clip, g_Graphics.GetScreen(), &offset) :
			SDL_BlitSurface(m_Surface, clip, g_Graphics.GetScreen(), &offset);
	}

	if (error == -1)
	{
		g_LogFile.ss() << "Error Blitting surface (" << m_Filename << ") - " << SDL_GetError(); g_LogFile.ssend();
		return false;
	}

	return true;
}

bool CSurface::DrawGifSurface(int x, int y, AG_Frame* agframes, int currentframe, SDL_Surface* destination, SDL_Rect* clip, bool resize, bool maintainRatio)
{
	double scaleX = 0;	double scaleY = 0;

	agframes->delay = agframes[currentframe].delay;

	SDL_Rect u = { 
		clip->x + agframes[currentframe].x,
		clip->y + agframes[currentframe].y,
		agframes[currentframe].surface->w, 
		agframes[currentframe].surface->h };
	SDL_Rect f = u;

	// If GIF disposal mode requires background paint or if frame zero, do it.
	if (agframes->disposal >= AG_DISPOSE_RESTORE_BACKGROUND || currentframe == 0)
	{
		SDL_Rect l = { clip->x + agframes[currentframe].x, clip->y + agframes[currentframe].y, agframes[currentframe].surface->w, agframes[currentframe].surface->h };
		if (agframes->disposal == AG_DISPOSE_RESTORE_PREVIOUS && agframes[0].surface)
			SDL_BlitSurface(agframes[0].surface, &l, destination, &l);
		else
			SDL_BlitSurface(m_Surface, &l, destination, &l);

		// Set update rectangle to smallest rectangle that bounds both last and frame rectangles.

		clip->x = min(l.x, f.x);
		clip->w = max(l.x + l.w, f.x + f.w) - clip->x;
		clip->y = min(l.y, f.y);
		clip->h = max(l.y + l.h, f.y + f.h) - clip->y;
	}
	// Save current background in frame rectangle if GIF disposal mode 3.
	if (agframes[currentframe].disposal == AG_DISPOSE_RESTORE_PREVIOUS)
		SDL_BlitSurface(destination, &f, agframes[0].surface, &f);

	// Draw the current frame using the frame rectangle.
	SDL_BlitSurface(agframes[currentframe].surface, NULL, destination, &f);

	// Update the display using the update rectangle.
	SDL_UpdateRects(destination, 1, &u);

	return true;
}

void CSurface::MakeColoredSurface(int width, int height, int red, int green, int blue, int BPP)
{
	m_ColoredSurface = true;
	m_Surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, BPP, 0, 0, 0, 0);
	SDL_FillRect(m_Surface, 0, SDL_MapRGB(m_Surface->format, red, green, blue));
}
