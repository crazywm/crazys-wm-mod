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
#include "cButton.h"
#include "interface/cSurface.h"
#include "interface/CGraphics.h"

cButton::~cButton() = default;

void cButton::DrawWidget(const CGraphics& gfx)
{
	if (m_CurrImage)
	{
		m_CurrImage->DrawSurface(m_XPos, m_YPos);
	}
}

cButton::cButton(cInterfaceWindow* parent, const std::string& OffImage, const std::string& DisabledImage,
        const std::string& OnImage, int ID, int x, int y, int width, int height, bool transparency):
    cUIWidget(ID, x, y, width, height, parent)
{
    if (!OffImage.empty())
    {
        m_OffImage = GetGraphics().LoadImage(OffImage, m_Width, m_Height, transparency);
    }

    if (!DisabledImage.empty())
    {
        m_DisabledImage = GetGraphics().LoadImage(DisabledImage, m_Width, m_Height, transparency);
    }

    if (!OnImage.empty())
    {
        m_OnImage = GetGraphics().LoadImage(OnImage, m_Width, m_Height, transparency);
    }

    m_CurrImage = &m_OffImage;
}

void cButton::SetDisabled(bool disable)
{
    cUIWidget::SetDisabled(disable);
    m_CurrImage = disable ? &m_DisabledImage : &m_OffImage;
}

void cButton::SetCallback(std::function<void()> cb)
{
    m_Callback = std::move(cb);
}

bool cButton::HandleKeyPress(SDL_keysym key)
{
    if(key.sym != m_HotKey) return false;
    if(m_Callback)
        m_Callback();
    return true;
}

void cButton::SetHotKey(SDLKey key)
{
    m_HotKey = key;
}

bool cButton::HandleClick(int x, int y, bool press)
{
    if(press)        return false;
    if(m_Callback)   m_Callback();
    return true;
}

void cButton::HandleMouseMove(bool over, int x, int y)
{
    m_CurrImage = over ? &m_OnImage : &m_OffImage;
}
