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
#include "cEditBox.h"

#include <memory>
#include "interface/CGraphics.h"
#include "sConfig.h"
#include "interface/cFont.h"
#include "interface/cInterfaceWindow.h"
#include "interface/cColor.h"

extern cConfig cfg;

extern sColor g_EditBoxBorderColor;
extern sColor g_EditBoxBackgroundColor;
extern sColor g_EditBoxSelectedColor;
extern sColor g_EditBoxTextColor;

void cEditBox::DrawWidget(const CGraphics& gfx)
{
    m_Border.DrawSurface(m_XPos, m_YPos);

    if(HasFocus())
        m_FocusedBackground.DrawSurface(m_XPos + m_BorderSize, m_YPos + m_BorderSize);
    else
        m_Background.DrawSurface(m_XPos + m_BorderSize, m_YPos + m_BorderSize);

	// draw the text
	m_Text->DrawText(m_XPos+m_BorderSize+1, m_YPos+m_BorderSize+1);
}

cEditBox::cEditBox(cInterfaceWindow* parent, int ID, int x, int y, int width, int height, int BorderSize, int FontSize):
    cUIWidget(ID, x, y, width, height, parent), m_BorderSize(BorderSize),
    m_Text(std::make_unique<cFont>())
{
    m_Border = GetGraphics().CreateSurface(width, height, g_EditBoxBorderColor);
    m_Background = GetGraphics().CreateSurface(width - (BorderSize*2), height - (BorderSize*2), g_EditBoxBackgroundColor);
    m_FocusedBackground = GetGraphics().CreateSurface(width - (BorderSize*2), height - (BorderSize*2), g_EditBoxSelectedColor);

	m_Text->LoadFont(cfg.fonts.normal(), FontSize);
	m_Text->SetText("");
	m_Text->SetColor(g_EditBoxTextColor.r, g_EditBoxTextColor.g, g_EditBoxTextColor.b);
}

void cEditBox::ClearText()
{
	if(m_Text)
		m_Text->SetText("");
}

std::string cEditBox::GetText()
{
    return m_Text->GetText();
}

bool cEditBox::HandleClick(int x, int y, bool press)
{
    GetParent()->SetFocusTo(this);
    return true;
}

bool cEditBox::HandleKeyPress(SDL_keysym sym)
{
    if(!HasFocus()) return false;

    if (sym.sym == SDLK_BACKSPACE) {
        string text = m_Text->GetText();
        if(text.length() > 0)
        {
            text.erase(text.length()-1);
            m_Text->SetText(text);
        }
        return true;
    }
    else if (sym.sym == SDLK_ESCAPE) {
        ClearText();
    }
    else
    {
        std::uint16_t unicode = sym.unicode;
        unsigned char ascii   = unicode & 0xffu;
        if(unicode && unicode == ascii && std::isprint(ascii)) {
            string text = m_Text->GetText();
            text += ascii;
            int w = 0, h = 0;
            m_Text->GetSize(text,w,h);
            if(w > m_Width)
                return true;
            m_Text->SetText(text);
            return true;
        }
        return false;
    }
}

bool cEditBox::HandleSetFocus(bool focus)
{
    return true;
}
