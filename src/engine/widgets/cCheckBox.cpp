/*
* Copyright 2009, 2010, The Pink Petal Development Team.
* The Pink Petal Devloment Team are defined as the game's coders
* who meet on http://pinkpetal.org
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
#include "cCheckBox.h"
#include "utils/DirPath.h"
#include "interface/CGraphics.h"
#include "interface/cSurface.h"
#include "sConfig.h"
#include <utility>
#include "interface/sColor.h"
#include "interface/cTheme.h"
#include "theme_ids.h"
using namespace widgets_theme;

void cCheckBox::DrawWidget(const CGraphics& gfx)
{
    if (IsDisabled()) return;

    int off = m_LeftOrRight ? m_Label.GetWidth() + 4 : 0;
    // Draw the window
    m_Border.DrawSurface(m_XPos + off, m_YPos);
    m_Surface.DrawSurface(m_XPos + off + 1, m_YPos + 1);

    if (m_StateOn)
    {
        m_Image.DrawSurface(m_XPos + off, m_YPos);
    }
    m_Label.DrawSurface(m_XPos + (m_LeftOrRight ? 0 : m_Border.GetWidth() + 4), m_YPos);
}

cCheckBox::cCheckBox(cInterfaceWindow* parent, int id, int x, int y, int width, int height, std::string text, int fontsize, bool leftorright):
    cUIWidget(id, x, y, width, height, parent),
    m_Font(GetGraphics().LoadFont(GetTheme().normal_font(), fontsize))
{
    m_Image = GetGraphics().LoadImage(ImagePath("CheckBoxCheck.png").str(), m_Width, m_Height, true);
    m_Border = GetGraphics().CreateSurface(width, height,
                                           GetTheme().get_color(CheckBoxBorderColor, {0, 0, 0}));
    m_Surface = GetGraphics().CreateSurface(width - 2, height - 2,
                                            GetTheme().get_color(CheckBoxBackgroundColor, {180, 180, 180}));

    m_Font.SetColor(GetTheme().get_color(CheckBoxTextColor, {0, 0, 0}));
    m_Label = m_Font.RenderText(std::move(text));
    m_LeftOrRight = leftorright;

    // adjust width to account for label
    m_Width += m_Label.GetWidth() + 4;
}

void cCheckBox::SetCallback(std::function<void(bool)> cb)
{
    m_Callback = std::move(cb);
}

bool cCheckBox::HandleClick(int x, int y, bool press)
{
    if (!press) return false;

    int off = (m_LeftOrRight ? m_Label.GetWidth() : m_Width) + 4;
    bool over = false;
    if (m_LeftOrRight)
    {
        if (x > m_XPos + off && y > m_YPos && x < m_XPos + off + m_Width && y < m_YPos + m_Height)
            over = true;
    }
    else
    {
        if (x > m_XPos && y > m_YPos && x < m_XPos + m_Width && y < m_YPos + m_Height)
            over = true;
    }

    if (over)
    {
        m_StateOn = !m_StateOn;
        if(m_Callback)
            m_Callback(m_StateOn);
        return true;
    }
    return false;
}
