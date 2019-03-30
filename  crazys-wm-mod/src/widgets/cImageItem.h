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

#include "cInterfaceObject.h"
#include <string>


class CSurface;
class cAnimatedSurface;
class SDL_Surface;


// Character image
class cImage
{
public:
	cImage();
	~cImage();
	CSurface* m_Surface;
	cAnimatedSurface* m_AniSurface;
};

class cImageItem : public cUIWidget
{
public:
	cImageItem(int id, int x, int y, int width, int height);
	~cImageItem();


	bool CreateImage(std::string filename, bool statImage, int R, int G, int B);
	bool CreateAnimatedImage(std::string filename, std::string dataFilename);

	void DrawWidget(const CGraphics& gfx) override;

	cAnimatedSurface* m_AnimatedImage;
	CSurface* m_Image;
	SDL_Surface* m_Surface;
	int m_ID;
	bool m_loaded;
};

#endif
