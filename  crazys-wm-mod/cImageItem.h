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
#ifndef __CIMAGEITEM_H
#define __CIMAGEITEM_H

#include "CSurface.h"
#include "cAnimatedSurface.h"
#include "cInterfaceObject.h"
#include "sConfig.h"
#include "Constants.h"
#include "SDL_anigif.h"


class CSurface;


// Character image
class cImage
{
public:
	cImage();
	~cImage();
	cImage* m_Next;
	CSurface* m_Surface;
	cAnimatedSurface* m_AniSurface;
};

class cImageItem : public cInterfaceObject
{
	bool m_Hidden;
public:
	cImageItem();
	~cImageItem();


	bool CreateImage(int id, string filename, int x, int y, int width, int height, bool statImage = false, int R = 0, int G = 0, int B = 0);
	bool CreateAnimatedImage(int id, string filename, string dataFilename, int x, int y, int width, int height);

	void Draw();

	cAnimatedSurface* m_AnimatedImage;
	CSurface* m_Image;
	SDL_Surface* m_Surface;
	int m_ID;
	bool m_loaded;
	cImageItem* m_Next;	// next button on the window

	void hide()	{ m_Hidden = true; }
	void unhide()	{ m_Hidden = false; }
	void toggle()	{ m_Hidden = !m_Hidden; }
};

#endif
