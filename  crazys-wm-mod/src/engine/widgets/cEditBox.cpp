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
	m_TextGFX.DrawSurface(m_XPos + m_BorderSize + 1, m_YPos + m_BorderSize + 1);
}

cEditBox::cEditBox(cInterfaceWindow* parent, int ID, int x, int y, int width, int height, int BorderSize, int FontSize):
        cUIWidget(ID, x, y, width, height, parent), m_BorderSize(BorderSize),
        m_Font(std::make_unique<cFont>(&GetGraphics()))
{
    m_Border = GetGraphics().CreateSurface(width, height, g_EditBoxBorderColor);
    m_Background = GetGraphics().CreateSurface(width - (BorderSize*2), height - (BorderSize*2), g_EditBoxBackgroundColor);
    m_FocusedBackground = GetGraphics().CreateSurface(width - (BorderSize*2), height - (BorderSize*2), g_EditBoxSelectedColor);

	m_Font->LoadFont(cfg.fonts.normal(), FontSize);
	m_Font->SetColor(g_EditBoxTextColor.r, g_EditBoxTextColor.g, g_EditBoxTextColor.b);

    UpdateText();
}

void cEditBox::ClearText()
{
    if(!m_Text.empty()) {
        m_Text.clear();
        UpdateText();
    }
}

const std::string& cEditBox::GetText() const
{
    return m_Text;
}

bool cEditBox::HandleClick(int x, int y, bool press)
{
    GetParent()->SetFocusTo(this);
    return true;
}

bool cEditBox::HandleKeyPress(SDL_Keysym sym)
{
    if(!HasFocus()) return false;

    if (sym.sym == SDLK_BACKSPACE) {

        if(m_Text.length() > 0)
        {
            m_Text.erase(m_Text.length()-1);
            UpdateText();
        }
    }
    else if (sym.sym == SDLK_ESCAPE) {
        ClearText();
    }
    return true;
}

void cEditBox::HandleTextInput(const char* t) {
    m_Text += t;
    int w = 0, h = 0;
    m_Font->GetSize(m_Text, w, h);
    if(w > m_Width) {
        // too long, remove that character again
        m_Text.erase(m_Text.length()-1);
        return;
    }
    UpdateText();
}

bool cEditBox::HandleSetFocus(bool focus)
{
    if(focus) {
        SDL_StartTextInput();
    } else {
        SDL_StopTextInput();
    }
    return true;
}

void cEditBox::UpdateText()
{
    m_TextGFX = m_Font->RenderText(m_Text);
}

void cEditBox::SetText(std::string text)
{
    m_Text = std::move(text);
    UpdateText();
}
