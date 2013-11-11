/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Development Team are defined as the game's coders 
 * who meet on http://pinkpetal.co.cc
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

bool cAnimatedSurface::DrawFrame(int x, int y, int width, int height, unsigned int currentTime)	// Updates animation according to speed, and then draws it on the screen
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

		if(m_CurrentFrame==m_NumFrames)
		{
			if(!m_PlayOnce)
				m_CurrentFrame = m_NumFrames-1;
			else
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

void cAnimatedSurface::UpdateSprite(SDL_Rect& rect, int width, int height)
{
	SDL_Rect temp;
	temp.x = temp.y = 0;
	temp.w = width;
	temp.h = height;
	m_Surface->DrawSurface(0,0,m_SpriteSurface,&rect);
	m_Surface->ResizeSprite(m_SpriteSurface,&temp);
}

void cAnimatedSurface::SetData(int xPos, int yPos, int numFrames, int speed, int width, int height, CSurface* surface)
{
	m_CurrentColumn = m_CurrentRow = m_CurrentFrame = 0;
	m_Speed = speed;
	m_NumFrames = numFrames;

	if(surface->GetSurface() != 0)
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
	if(m_SpriteSurface)
		SDL_FreeSurface(m_SpriteSurface);
	m_SpriteSurface = 0;
	m_SpriteSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, m_Frames.w, m_Frames.h, 32, 0,0,0,0);

	m_Surface = surface;
}

CAnimatedSprite::~CAnimatedSprite()
{
	Free();
}

void CAnimatedSprite::Free()
{
	if(m_Animations)
		delete [] m_Animations;
	m_Animations=0;

	if(m_Image)
		delete m_Image;
	m_Image = 0;

	m_CurrAnimation=0;
}

bool CAnimatedSprite::Draw(int x, int y, int width, int height, unsigned int currentTime)
{
	return m_Animations[m_CurrAnimation].DrawFrame(x,y,width,height,currentTime);
}

bool CAnimatedSprite::LoadAnimations(string imgFilename, string animationData)
{
	int NumAnims;
	int numFrames, speed, xPos, yPos, width, height;
	ifstream ifile(animationData.c_str());

	if(ifile == 0)
		return false;

	m_Image = new CSurface(imgFilename);
	m_Image->SetColorKey(255,0,168);

	ifile>>NumAnims;

	m_Animations = new cAnimatedSurface[NumAnims];
	for(int i=0; i<NumAnims; i++)
	{
		ifile>>numFrames>>speed>>xPos>>yPos>>width>>height;
		m_Animations[i].SetData(xPos,yPos,numFrames,speed,width,height, m_Image);
	}

	ifile.close();

	return true;
}
