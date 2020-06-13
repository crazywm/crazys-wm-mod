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
#include <vector>
#include "cImageItem.h"
#include "interface/CGraphics.h"
#include "FileList.h"
#include "interface/cInterfaceWindow.h"
#include "interface/cAnimatedSurface.h"
#include "interface/cSurface.h"

cImageItem::cImageItem(cInterfaceWindow* parent, int id, int x, int y, int width, int height) : cUIWidget(id, x, y, width, height, parent)
{
    m_loaded = false;
}
cImageItem::~cImageItem()
{
}

bool cImageItem::CreateImage(std::string filename, bool statImage, int R, int G, int B)
{
    if (!filename.empty())
    {
        m_loaded = true;
        m_Image = GetGraphics().LoadImage(filename, m_Width, m_Height, true);
    }
    else
        m_loaded = false;

    return true;
}

void cImageItem::DrawWidget(const CGraphics& gfx)
{
    if (m_AnimatedImage) {
        m_AnimatedImage.UpdateFrame();
        m_AnimatedImage.DrawSurface(m_XPos, m_YPos);
    } else if (m_Image)    {
        m_Image.DrawSurface(m_XPos, m_YPos);
    }
}

void cImageItem::Reset()
{
    if (!m_loaded) m_Image = cSurface();
}

void cImageItem::SetImage(cSurface image)
{
    m_Image = std::move(image);
    m_AnimatedImage = {};
}

void cImageItem::SetImage(cAnimatedSurface image)
{
    m_Image = {};
    m_AnimatedImage = std::move(image);
}
