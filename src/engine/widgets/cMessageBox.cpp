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
#include <cassert>
#include "cMessageBox.h"
#include "interface/CGraphics.h"
#include "CLog.h"
#include "interface/cFont.h"
#include "interface/sColor.h"
#include "interface/cTheme.h"
#include "theme_ids.h"
using namespace widgets_theme;

cMessageBox::~cMessageBox() = default;

namespace {
    constexpr const char* Backgrounds[NUM_MESSBOXCOLOR] =
            {MessageBoxBackgroundColor1, MessageBoxBackgroundColor2, MessageBoxBackgroundColor3,
             MessageBoxBackgroundColor4, MessageBoxBackgroundColor5};
    constexpr sColor BackgroundColors[NUM_MESSBOXCOLOR] = {
            {100, 100, 150}, {200, 100, 150}, {100, 200, 150},
            {100, 100, 200}, {190, 190, 0}};
}


cMessageBox::cMessageBox(cInterfaceWindow* parent, int x, int y, int width, int height, int BorderSize, int FontSize) :
    cUIWidget(0, x, y, width, height, parent)
{
    m_Text = "";
    m_Position = 0;

    m_BorderSize = BorderSize;
    m_Border = GetGraphics().CreateSurface(width, height, GetTheme().get_color(MessageBoxBorderColor, {255, 255, 255}));
    for(int i = 0; i < NUM_MESSBOXCOLOR; ++i) {
        m_Background[i] = GetGraphics().CreateSurface(width-(BorderSize*2), height-(BorderSize*2),
                                                      GetTheme().get_color(Backgrounds[i], BackgroundColors[i]));
    }
    ChangeFontSize(FontSize);
}

void cMessageBox::DrawWidget(const CGraphics& gfx)
{
    if(m_Background.at(m_Color) && m_Border) {
        // Draw the window
        m_Border.DrawSurface(m_XPos, m_YPos);
        m_Background[m_Color].DrawSurface(m_XPos + m_BorderSize, m_YPos + m_BorderSize);
    }

    int offset = m_Position * m_Font->GetFontLineSkip();
    SDL_Rect srcRect;
    srcRect.x = 0;
    srcRect.y = offset;
    srcRect.h = std::min(m_Height, m_PreRendered.GetHeight() - offset);
    srcRect.w = std::min(m_Width, m_PreRendered.GetWidth());

    m_PreRendered.DrawSurface(m_XPos + 2, m_YPos, &srcRect);
}

void cMessageBox::Advance()
{
    m_Position += m_Height / m_Font->GetFontLineSkip();

    assert(m_PreRendered);
    if(m_Position >= m_PreRendered.GetHeight() / m_Font->GetFontLineSkip()) {
        m_Messages.pop_front();
        if(!m_Messages.empty()) {
            UpdateMessageText();
        } else {
            SetDisabled(true);
            if(m_Callback)
                m_Callback();

        }
    }
}

void cMessageBox::UpdateMessageText()
{
    if(!m_Messages.empty()) {
        m_Text        = m_Messages.front().m_Text;
        m_Position    = 0;
        m_Color       = m_Messages.front().m_Color;
        if(m_Color < 0 || m_Color >= m_Background.size()) {
            g_LogFile.error("interface", "Invalid color ", m_Color, " for MessageBox (", m_Text, ')');
        }

        m_PreRendered = m_Font->RenderMultilineText(m_Text, m_Width - 4);
        assert(m_PreRendered);
    }
}

void cMessageBox::ChangeFontSize(int FontSize)
{
    m_Font = std::make_unique<cFont>(GetGraphics().LoadFont(GetTheme().normal_font(), FontSize));
    m_Font->SetColor(GetTheme().get_color(MessageBoxTextColor, {0, 0, 0}));
    UpdateMessageText();
}

void cMessageBox::PushMessage(std::string text, int color)
{
    bool empty = m_Messages.empty();
    if(!text.empty())
    {
        m_Messages.emplace_back(sMessage{std::move(text), color});
        if(empty)
            UpdateMessageText();
    }
}

bool cMessageBox::HandleKeyPress(SDL_Keysym key) {
    Advance();
    return true;
}

bool cMessageBox::HandleClick(int x, int y, bool press) {
    if(!press) Advance();
    return true;
}

bool cMessageBox::IsOver(int x, int y) const {
    return true;
}

bool cMessageBox::HandleMouseWheel(bool down) {
    Advance();
    return true;
}
