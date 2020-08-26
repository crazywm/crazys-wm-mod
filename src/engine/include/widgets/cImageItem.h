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

#include "interface/cInterfaceObject.h"
#include <string>
#include "interface/cSurface.h"
#include "interface/cAnimatedSurface.h"


class SDL_Surface;


class cImageItem : public cUIWidget
{
public:
    cImageItem(cInterfaceWindow* parent, int id, int x, int y, int width, int height);
    ~cImageItem();

    void SetImage(cSurface image);
    void SetImage(cAnimatedSurface image);

    bool CreateImage(std::string filename, bool statImage, int R, int G, int B);
    void DrawWidget(const CGraphics& gfx) override;
    void Reset() override;

    cAnimatedSurface m_AnimatedImage;
    cSurface m_Image;
    bool m_loaded;
};

#endif
