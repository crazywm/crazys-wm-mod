/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders 
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
#include "cImageItem.h"
#include "CGraphics.h"
#include "libintl.h"

extern CGraphics g_Graphics;

cImageItem::~cImageItem()
{
	if(m_Next)
		delete m_Next;
	m_Next = 0;
	
	if(m_Image && m_loaded)
		delete m_Image;
	m_Image = 0;

	if(m_Surface)
		SDL_FreeSurface(m_Surface);
	m_Surface = 0;

	if(m_AnimatedImage && m_loaded)
		delete m_AnimatedImage;
	m_AnimatedImage = 0;
}

bool cImageItem::CreateImage(int id, string filename, int x, int y, int width, int height, bool statImage, int R, int G, int B)
{
	m_ID = id;
	if(statImage)
	{
		m_Surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, 0,0,0,0);
		SDL_FillRect(m_Surface,0,SDL_MapRGB(m_Surface->format,R,G,B));
	}
	SetPosition(x,y,width,height);

	if(filename != "")
	{
		m_loaded = true;
		m_Image = new CSurface(filename);
		m_Image->SetAlpha(true);
	}
	else
		m_loaded = false;

	m_Next = 0;

	return true;
}

bool cImageItem::CreateAnimatedImage(int id, string filename, string dataFilename, int x, int y, int width, int height)
{
	m_ID = id;
	SetPosition(x,y,width,height);

	if(filename != "")
	{
		m_loaded = true;
		m_Image = new CSurface(filename);
		m_Image->SetAlpha(true);

		// load the animation
		m_AnimatedImage = new cAnimatedSurface();
		int numFrames, speed, aniwidth, aniheight;
		ifstream input;
		input.open(dataFilename.c_str());
		if(!input)
		{
			CLog log;
			log.ss() << "Incorrect data file given for animation - " << dataFilename;
			log.ssend();
			return false;
		}
		else
			input>>numFrames>>speed>>aniwidth>>aniheight;
		m_AnimatedImage->SetData(0,0,numFrames, speed, aniwidth, aniheight, m_Image);
		input.close();
	}
	else
	{
		CLog log;
		log.ss() << "Incorrect image file given for animation";
		log.ssend();
		m_loaded = false;
		return false;
	}

	m_Next = 0;

	return true;
}

void cImageItem::Draw()
{
	if(m_Hidden)
		return;

	if(m_AnimatedImage)
	{
		SDL_Rect rect;
		rect.y = rect.x = 0;
		rect.w = m_Width;
		rect.h = m_Height;

		m_AnimatedImage->DrawFrame(m_XPos,m_YPos, m_Width, m_Height, g_Graphics.GetTicks());
	}
	else if(m_Image)
	{
		SDL_Rect rect;
		rect.y = rect.x = 0;
		rect.w = m_Width;
		rect.h = m_Height;

		m_Image->DrawSurface(m_XPos,m_YPos, 0, &rect, true);
	}
	else if(m_Surface)
	{
		// Draw the window
		SDL_Rect offset;
		offset.x = m_XPos;
		offset.y = m_YPos;

		// blit to the screen
		SDL_BlitSurface(m_Surface, 0, g_Graphics.GetScreen(), &offset);
	}
}
