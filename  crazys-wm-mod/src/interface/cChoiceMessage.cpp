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
#include "cChoiceMessage.h"

#include <memory>
#include "cFont.h"
#include "CGraphics.h"
#include "sConfig.h"
#include "XmlUtil.h"
#include "tinyxml.h"
#include "DirPath.h"
#include "cColor.h"

extern CGraphics g_Graphics;
extern cConfig cfg;
extern sColor g_ChoiceMessageTextColor;
extern sColor g_ChoiceMessageBorderColor;
extern sColor g_ChoiceMessageHeaderColor;
extern sColor g_ChoiceMessageBackgroundColor;
extern sColor g_ChoiceMessageSelectedColor;

cChoice::cChoice(int x, int y, int width, int height, int ID, int num_choices, int item_height, int max_str_len, int fontsize, cInterfaceWindow* parent) :
    cUIWidget(ID, x, y, width, height, parent), m_Choices(num_choices), m_FontSize(fontsize)
{
    m_Font.LoadFont(cfg.fonts.normal(), fontsize);
    m_Font.SetText("");
    m_Font.SetColor(g_ChoiceMessageTextColor.r, g_ChoiceMessageTextColor.g, g_ChoiceMessageTextColor.b);

    if(max_str_len > 0) {
        int MaxWidth = 0, MaxHeight = 0;
        std::string temp = std::string('W', max_str_len);
        m_Font.GetSize(temp, MaxWidth, MaxHeight);
        int newHeight = (MaxHeight * num_choices) + 2;

        if (newHeight > g_Graphics.GetHeight())
            newHeight = g_Graphics.GetHeight() - 34;
        if (MaxWidth > g_Graphics.GetWidth())
            MaxWidth = g_Graphics.GetWidth() - 2;

        m_NumDrawnElements = newHeight / MaxHeight;
        if (m_NumDrawnElements >= num_choices) {
            m_ScrollDisabled = true;
            m_eWidth = (MaxWidth);
        } else
            m_eWidth = (MaxWidth - (18));
        m_eHeight = MaxHeight;

        m_CurrChoice = -1;
        m_Width = MaxWidth;
        m_Height = newHeight;
        m_FontSize = fontsize;

        m_XPos = ((g_Graphics.GetWidth() / 2) - (m_Width / 2));
        m_YPos = ((g_Graphics.GetHeight() / 2) - (m_Height / 2));
    } else {
        if ((height - 2) / item_height < num_choices)
            height = (num_choices*item_height) + 2;
        m_NumDrawnElements = (height - 2) / item_height;
        m_eWidth = (width - (18));
        m_eHeight = item_height;
    }

    m_Border = g_Graphics.CreateSurface(m_Width + (m_ScrollDisabled ? 2 : 20), m_Height, g_ChoiceMessageBorderColor);
    m_Background = g_Graphics.CreateSurface(m_Width - (m_ScrollDisabled ? 0 : 18), m_Height, g_ChoiceMessageBackgroundColor);
    m_ElementBackground = g_Graphics.CreateSurface(m_eWidth, m_eHeight,g_ChoiceMessageBackgroundColor);
    m_ElementSelectedBackground = g_Graphics.CreateSurface(m_eWidth, m_eHeight, g_ChoiceMessageSelectedColor);
    m_HeaderBackground = g_Graphics.CreateSurface(std::max(120, m_eWidth), 32, g_ChoiceMessageHeaderColor);

    m_UpOn = g_Graphics.LoadImage(ButtonPath("UpOn.png"), 16, 16, true);
    m_UpOff = g_Graphics.LoadImage(ButtonPath("UpOff.png"), 16, 16, true);
    m_CurrUp = m_UpOff;

    m_DownOn = g_Graphics.LoadImage(ButtonPath("DownOn.png"), 16, 16, true);
    m_DownOff = g_Graphics.LoadImage(ButtonPath("DownOff.png"), 16, 16, true);
    m_CurrDown = m_DownOff;
}

int cChoice::num_choices() const
{
    return m_Choices.size();
}

void cChoice::DrawWidget(const CGraphics &gfx) {
    if (m_Background && m_Border)
    {
        // blit to the screen
        m_Border.DrawSurface(m_XPos, m_YPos);
        m_Background.DrawSurface(m_XPos + 1, m_YPos + 1);
    }

    // Draw the heading text
    if (m_HeaderBackground)
    {

        string question = "Select Choice";
        if (m_Question.length() > 0)
            question = m_Question;
        m_Font.SetText(question);
        m_Font.SetMultiline(true, m_Width, m_Height);

        int y = m_YPos - m_Font.GetHeight();
        // TODO this does not belong into the loop! why is this here?
        m_HeaderBackground = g_Graphics.CreateSurface(m_Border.GetWidth(), m_Font.GetHeight(), g_ChoiceMessageHeaderColor);
        m_HeaderBackground.DrawSurface(m_XPos, y);
        m_Font.DrawMultilineText(m_XPos, y);
    }

    for (int i = m_Position, j = 0; i < num_choices() && j < m_NumDrawnElements; i++, j++)
    {
        // Draw the window
        int y = (m_YPos + 1) + (m_eHeight*j);

        // blit to the screen
        if (i == m_CurrChoice)
            m_ElementSelectedBackground.DrawSurface(m_XPos + 1, y);
        else
            m_ElementBackground.DrawSurface(m_XPos + 1, y);

        // draw the text
        m_Font.SetText(m_Choices[i]);
        m_Font.DrawText(m_XPos, y);
    }


    // draw the up and down buttons
    if (!m_ScrollDisabled)
    {
        m_CurrUp.DrawSurface(m_XPos + m_Width - 17, m_YPos + 1);
        m_CurrDown.DrawSurface(m_XPos + m_Width - 17, m_YPos + m_Height - 17);
    }
}

void cChoice::Question(std::string text) {
    m_Question = std::move(text);
}

void cChoice::AddChoice(std::string text, int choiceID) {
    m_Choices.at(choiceID) = std::move(text);
}

void cChoice::HandleMouseMove(bool over, int x, int y) {
    if(!over) {
        m_CurrUp = m_UpOff;
        m_CurrDown = m_DownOff;
        return;
    }
    // check if over the buttons
    if (!m_ScrollDisabled)
    {
        if (x < m_XPos + m_Width - 1 && y < m_YPos + 17 && x > m_XPos - 16 + m_Width && y > m_YPos + 1)
        {
            m_CurrUp = m_UpOn;
        }
        else
        {
            m_CurrUp = m_UpOff;
        }

        if (x < m_XPos + m_Width - 1 && y < m_YPos + m_Height - 1 && x > m_XPos - 16 + m_Width && y > m_YPos + m_Height - 17)
        {
            m_CurrDown = m_DownOn;
        }
        else
        {
            m_CurrDown = m_DownOff;
        }
    }

    // now highlight the choice the mouse is over
    m_CurrChoice = FindActive(x, y);
}

int cChoice::FindActive(int x, int y) const {
    for (int i = m_Position, j = 0; i < num_choices() && j < m_NumDrawnElements; i++, j++)
    {
        int cX = m_XPos + 1;
        int cY = (m_YPos + 1) + (m_eHeight*j);

        // Check if over the item
        if (x > cX && y > cY && x < cX + m_eWidth && y < cY + m_eHeight)
        {
            // then select it and deactivate the choice box;
            return i;
        }
    }
    return -1;
}

bool cChoice::HandleClick(int x, int y, bool press) {
    if(press)
        return false;

    // check for scroll buttons
    if(!m_ScrollDisabled) {
        // TODO these conditions need to be fixed
        if (m_CurrUp.RawSurface() == m_UpOn.RawSurface()) {
            if (m_Position - 1 >= 0)
                m_Position--;
            return true;
        }

        if (m_CurrDown.RawSurface() == m_DownOn.RawSurface()) {
            if (m_Position + m_NumDrawnElements < num_choices())
                m_Position++;
            return true;
        }
    }

    m_CurrChoice = FindActive(x, y);
    // then select it and deactivate the choice box;
    if(m_CurrChoice == -1)
        return false;

    if (m_Callback) {
        m_Callback(m_CurrChoice);
    }
    SetDisabled(true);
    return true;
}

bool cChoice::HandleKeyPress(SDL_keysym key) {
    if (key.sym == SDLK_RETURN || key.sym == SDLK_KP_ENTER)
    {
        if (m_Callback) {
            m_Callback(m_CurrChoice);
        }
        SetDisabled(true);
    }
    if (key.sym == SDLK_UP)
    {
        m_CurrChoice--;
        if (m_CurrChoice < 0)
            m_CurrChoice = num_choices() - 1;
    } else if (key.sym == SDLK_DOWN)
    {
        m_CurrChoice++;
        if (m_CurrChoice > num_choices() - 1)
            m_CurrChoice = 0;
    }
    return false;
}

void cChoice::SetCallback(std::function<void(int)> callback) {
    m_Callback = std::move(callback);
}
