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
#include "widgets/cImageItem.h"
#include "interface/CGraphics.h"
#include "utils/FileList.h"
#include "interface/cInterfaceWindow.h"
#include "interface/cAnimatedSurface.h"
#include "interface/cSurface.h"

cImageItem::cImageItem(cInterfaceWindow* parent, int id, int x, int y, int width, int height,
                       int mw, int mh) : cUIWidget(id, x, y, width, height, parent), m_MinWidth(mw), m_MinHeight(mh)
{
    m_loaded = false;
}
cImageItem::~cImageItem()
{
}

bool cImageItem::CreateImage(std::string filename, bool transparent)
{
    if (!filename.empty())
    {
        m_loaded = true;
        sLoadImageParams params;
        params.KeepRatio = true;
        params.Transparency = transparent;
        params.MinWidth = m_MinWidth;
        params.MinHeight = m_MinHeight;
        params.MaxWidth = m_Width;
        params.MaxHeight = m_Height;
        m_Image = GetGraphics().LoadImage(std::move(filename), params);
        m_AnimatedImage = {};
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
        m_Image.DrawSurface(m_XPos + (m_Width - m_Image.GetWidth()) / 2,
                            m_YPos + (m_Height - m_Image.GetHeight()) / 2);
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
