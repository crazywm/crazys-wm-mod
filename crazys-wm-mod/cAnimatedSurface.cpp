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
#include "cAnimatedSurface.h"
#include "CLog.h"

extern CLog g_LogFile;


cAnimatedSurface::cAnimatedSurface()
{
	m_Surface = 0;
	m_SpriteSurface = 0;
	m_SaveSurface = 0;
	m_AFrames = 0;
	m_Gif = false;
	m_FrameDone = false;
	m_PlayOnce = false;
	m_CurrentFrame = 0;
	m_Speed = 0;
	m_NumFrames = 0;
	m_LastTime = 0;
	m_Rows = 0;
	m_Colums = 0;
	m_CurrentRow = 0;
	m_CurrentColumn = 0;
}
cAnimatedSurface::~cAnimatedSurface()
{
	m_Surface = 0;
	if (m_SpriteSurface)
		SDL_FreeSurface(m_SpriteSurface);
	m_SpriteSurface = 0;
	m_SaveSurface = 0;
	m_AFrames = 0;

	m_Gif = false;
	m_FrameDone = false;
	m_PlayOnce = false;
	m_CurrentFrame = 0;
	m_Speed = 0;
	m_NumFrames = 0;
	m_LastTime = 0;
	m_Rows = 0;
	m_Colums = 0;
	m_CurrentRow = 0;
	m_CurrentColumn = 0;


}

// Updates animation according to speed, and then draws it on the screen
bool cAnimatedSurface::DrawFrame(int x, int y, int width, int height, unsigned int currentTime)
{
	SDL_Rect temp = m_Frames;

	if(m_LastTime == 0)
	{
		m_LastTime = currentTime;
		UpdateSprite(temp, width, height);
	}
	else if((currentTime-m_LastTime) >= (unsigned int)m_Speed)
	{
		m_LastTime = currentTime;
		m_CurrentFrame++;
		m_CurrentColumn++;

		if(m_CurrentFrame>=m_NumFrames)
		{
			m_CurrentFrame = 0;
			m_FrameDone = true;
			m_CurrentColumn = m_CurrentRow = 0;
		}
		else
		{
			if(m_CurrentColumn>=m_Colums)
			{
				m_CurrentRow++;
				m_CurrentColumn=0;
			}
		}
		temp.x = m_CurrentColumn*m_Frames.w;
		temp.y = m_CurrentRow*m_Frames.h;
		UpdateSprite(temp, width, height);
	}
	return m_Surface->DrawSprite(x,y);
}

// Updates animation according to speed, and then draws it on the screen
bool cAnimatedSurface::DrawGifFrame(int x, int y, int width, int height, unsigned int currentTime)
{
	SDL_Rect temp = m_Frames;
	int m_Speed = m_AFrames[m_CurrentFrame].delay;
	if (m_Speed < 0) m_Speed = m_AFrames[0].delay;	// to correct for frames without their own delay
	if (m_Speed < 0) m_Speed = 0;					// to correct bad delay values
	if (m_LastTime == 0)
	{
		m_LastTime = currentTime;
		UpdateSprite(temp, width, height,true);
	}
	else if ((currentTime - m_LastTime) >= (unsigned int)m_Speed)
	{
		m_LastTime = currentTime;
		m_CurrentFrame++;

		if (m_CurrentFrame >= m_NumFrames)
		{
			m_CurrentFrame = 0;
			m_FrameDone = true;
		}
		UpdateSprite(temp, width, height, true);
	}
	return m_Surface->DrawSprite(x, y);
}

void cAnimatedSurface::UpdateSprite(SDL_Rect& rect, int width, int height, bool gif)
{
	SDL_Rect temp;
	temp.x = temp.y = 0;
	temp.w = width;
	temp.h = height;
	if (gif)	m_Surface->DrawGifSurface(0, 0, getAFrames(), m_CurrentFrame, m_SpriteSurface, &rect,false,true);
	else		m_Surface->DrawSurface(0, 0, m_SpriteSurface, &rect);
	m_Surface->ResizeSprite(m_SpriteSurface,&temp,true);
}

void cAnimatedSurface::SetData(int xPos, int yPos, int numFrames, int speed, int width, int height, CSurface* surface, bool gif)
{
	m_CurrentColumn = m_CurrentRow = m_CurrentFrame = 0;
	m_LastTime = 0;
	m_Speed = speed;
	if (m_Speed < 0) m_Speed = 0;
	m_NumFrames = numFrames;
	if (gif)
	{
		m_Colums = m_Rows = 1;
	}
	else if(surface->GetSurface() != 0)
	{
		m_Colums = (**(surface->GetSurface())).w/width;
		m_Rows = (**(surface->GetSurface())).h/height;
	}
	else
	{
		g_LogFile.write("ERROR - ANIMATED IMAGE GIVEN NULL SURFACE - SetData()");
		return;
	}

	m_Frames.x = xPos;
	m_Frames.y = yPos;
	m_Frames.w = width;
	m_Frames.h = height;

	// prepare the sprite surface
	if(m_SpriteSurface)		SDL_FreeSurface(m_SpriteSurface);
	m_SpriteSurface = 0;
	m_SpriteSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, m_Frames.w, m_Frames.h, 32, 0,0,0,0);

	m_Surface = surface;
}

void cAnimatedSurface::SetGifData(int xPos, int yPos, int numFrames, AG_Frame* agf, CSurface* surface)
{
	m_Gif = true;
	m_LastTime = 0;
	m_NumFrames = numFrames;
	m_CurrentFrame = 0;					// set it to the end so it starts with 0 when run
	m_SaveSurface = agf[0].surface;
	m_AFrames = agf;
	m_Speed = 0;

	m_Frames.x = xPos;
	m_Frames.y = yPos;
	m_Frames.w = agf[0].surface->w;
	m_Frames.h = agf[0].surface->h;

	// prepare the sprite surface
	if (m_SpriteSurface)		SDL_FreeSurface(m_SpriteSurface);
	m_SpriteSurface = 0;
	m_SpriteSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, m_Frames.w, m_Frames.h, 32, 0, 0, 0, 0);

	m_Surface = surface;
}

CAnimatedSprite::~CAnimatedSprite()
{
	Free();
}

void CAnimatedSprite::Free()
{
	if(m_Animations)	delete [] m_Animations;
	m_Animations=0;
	if(m_Image)			delete m_Image;
	m_Image = 0;
	m_CurrAnimation=0;
}

bool CAnimatedSprite::Draw(int x, int y, int width, int height, unsigned int currentTime)
{
	return m_Animations[m_CurrAnimation].DrawFrame(x,y,width,height,currentTime);
}

