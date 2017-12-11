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
#include "CGraphics.h"
#include "fstream"
#include "CLog.h"
#include "DirPath.h"
#include "sConfig.h"
#include "Globals.h"

using namespace std;

extern CLog g_LogFile;
extern cConfig cfg;

CGraphics::CGraphics()
{
	m_Screen = 0;
	m_CurrentTime = 0;
}

CGraphics::~CGraphics()
{
	Free();
}

void CGraphics::Free()
{
	TTF_Quit();
	SDL_Quit();
	m_Screen = 0;
}

void CGraphics::Begin()
{
	m_FPS.Start();
	m_CurrentTime = SDL_GetTicks();
	//Fill the screen white
	SDL_FillRect(m_Screen, &m_Screen->clip_rect, SDL_MapRGB(m_Screen->format, 0, 0, 0));
}

bool CGraphics::End()
{
	// flip the surface
	if(SDL_Flip(m_Screen) == -1)
	{
		g_LogFile.write(SDL_GetError());
		return false;
	}

	// Maintain framerate
	if((m_FPS.GetTicks() < 1000 / FRAMES_PER_SECOND))
	{
		//Sleep the remaining frame time
		SDL_Delay((1000/FRAMES_PER_SECOND)-m_FPS.GetTicks());
	}

	return true;
}

bool CGraphics::InitGraphics(string caption, int Width, int Height, int BPP)
{
	if ((Width == 0 || Height == 0) && cfg.resolution.configXML())
	{
		m_ScreenWidth = _G.g_ScreenWidth = cfg.resolution.width();
		m_ScreenHeight = _G.g_ScreenHeight = cfg.resolution.height();
		_G.g_ScreenScaleX = (float)m_ScreenWidth / (float)cfg.resolution.scalewidth();
		_G.g_ScreenScaleY = (float)m_ScreenHeight / (float)cfg.resolution.scaleheight();
		_G.g_Fullscreen = cfg.resolution.fullscreen();
	}
	else
	{
		g_LogFile.write("Skipping Screen Mode");
		_G.g_ScreenWidth = m_ScreenWidth = Width;
		_G.g_ScreenHeight = m_ScreenHeight = Height;
		_G.g_Fullscreen = false;
	}

	// init SDL
	g_LogFile.write("Initializing SDL");
	if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		g_LogFile.write("Poop. SDL_INIT_EVERYTHING failed.");
		g_LogFile.write(SDL_GetError());
		return false;
	}
	m_ScreenBPP = BPP;

	// set window icon
	g_LogFile.write("Setting Window Icon");
	SDL_Surface* loadIcon = IMG_Load(ImagePath("window_icon.png"));
	if(!loadIcon)
	{
		g_LogFile.write("Error setting window icon (window_icon.png)");
		g_LogFile.write(SDL_GetError());
		g_LogFile.write(IMG_GetError());
		return false;
	}
	else
		SDL_WM_SetIcon(loadIcon, NULL);

	// Setup the screen
	g_LogFile.write("Determining Fullscreen or Windowed Mode");
	if (_G.g_Fullscreen == false)
		m_Screen = SDL_SetVideoMode(m_ScreenWidth, m_ScreenHeight, m_ScreenBPP, SDL_SWSURFACE);
	else
		m_Screen = SDL_SetVideoMode(m_ScreenWidth, m_ScreenHeight, m_ScreenBPP, SDL_SWSURFACE|SDL_FULLSCREEN);
	if(!m_Screen)	// check for error
	{
		g_LogFile.write(SDL_GetError());
		return false;
	}

	// set window caption
	g_LogFile.write("Setting Window Caption");
	SDL_WM_SetCaption(caption.c_str(), 0);

	// Init TTF
	g_LogFile.write("Initializing TTF");
	if(TTF_Init() == -1)
	{
		g_LogFile.write("Error with TTF_Init()");
		g_LogFile.write(TTF_GetError());
		return false;
	}

	return true;
}
