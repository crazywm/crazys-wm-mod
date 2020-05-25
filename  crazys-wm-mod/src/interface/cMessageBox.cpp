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
#include "cMessageBox.h"
#include "interface/CGraphics.h"
#include "sConfig.h"
#include "CLog.h"
#include "cFont.h"
#include "tinyxml.h"
#include "DirPath.h"
#include "XmlUtil.h"
#include "cColor.h"

extern CGraphics g_Graphics;
extern cConfig cfg;

extern sColor g_MessageBoxBorderColor;
extern sColor g_MessageBoxBackgroundColor[];
extern sColor g_MessageBoxTextColor;

cMessageBox::~cMessageBox() = default;


cMessageBox::cMessageBox(int x, int y, int width, int height, int BorderSize, int FontSize, bool scale)
{
	m_Text = "";
	m_Position = 0;

    DirPath dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "popup_message.xml";
    string m_filename = dp.c_str();
    TiXmlDocument doc(m_filename);
    if (!doc.LoadFile())
    {
        g_LogFile.ss() << "cInterfaceWindowXML: " << "Can't load screen definition from '" << m_filename << "'" << endl;
        g_LogFile.ss() << "Error: line " << doc.ErrorRow() << ", col " << doc.ErrorCol() << ": " << doc.ErrorDesc() << endl;
        g_LogFile.ssend();
    }
    else
    {
        const char *pt;
        TiXmlElement *el, *root_el = doc.RootElement();
        for (el = root_el->FirstChildElement(); el; el = el->NextSiblingElement())
        {
            if (el->ValueStr() == "Window")
            {
                XmlUtil xu(m_filename);
                if (pt = el->Attribute("XPos"))		xu.get_att(el, "XPos", x);
                if (pt = el->Attribute("YPos"))		xu.get_att(el, "YPos", y);
                if (pt = el->Attribute("Width"))	xu.get_att(el, "Width", width);
                if (pt = el->Attribute("Height"))	xu.get_att(el, "Height", height);
                if (pt = el->Attribute("FontSize"))	xu.get_att(el, "FontSize", FontSize);
                if (pt = el->Attribute("Border"))	xu.get_att(el, "Border", BorderSize);
                if (pt = el->Attribute("Scale"))	xu.get_att(el, "Scale", scale);
            }
        }
    }

    float xScale = 1.0f, yScale = 1.0f;
    if (scale)
    {
        if (g_Graphics.GetWidth() != cfg.resolution.width())	xScale = (float)g_Graphics.GetWidth() / (float)cfg.resolution.width();
        if (g_Graphics.GetHeight() != cfg.resolution.height())	yScale = (float)g_Graphics.GetHeight() / (float)cfg.resolution.height();
    }

    x = (int)((float)x*xScale);
    y = (int)((float)y*yScale);
    width = (int)((float)width*xScale);
    height = (int)((float)height*yScale);
    FontSize = (int)((float)FontSize*yScale);

    m_BorderSize = BorderSize;
    SetPosition(x, y, width, height);
    m_FontHeight = FontSize;
    m_Border = g_Graphics.CreateSurface(width, height, g_MessageBoxBorderColor);
    for(int i = 0; i < NUM_MESSBOXCOLOR; ++i) {
        m_Background[i] = g_Graphics.CreateSurface(width-(BorderSize*2), height-(BorderSize*2), g_MessageBoxBackgroundColor[i]);
    }
    ChangeFontSize(FontSize);
}

void cMessageBox::Draw(const CGraphics& gfx)
{
	if(!IsActive())
		return;

	if(m_Background[m_Color] && m_Border) {
        // Draw the window
        m_Border.DrawSurface(m_XPos, m_YPos);
        m_Background[m_Color].DrawSurface(m_XPos + m_BorderSize, m_YPos + m_BorderSize);
    }

    SDL_Rect srcRect;
    srcRect.x = 0;
    srcRect.y = m_Position * m_Font->GetFontLineSkip();
    srcRect.h = m_Height;
    srcRect.w = m_Width;

    m_PreRendered.DrawSurface(m_XPos, m_YPos, &srcRect);
}

void cMessageBox::Advance()
{
    m_Position += m_Height / m_Font->GetFontLineSkip();

    if(m_Position >= m_PreRendered.GetHeight() / m_Font->GetFontLineSkip()) {
        m_Messages.pop_front();
        if(!m_Messages.empty()) {
            UpdateMessageText();
        }
    }
}

void cMessageBox::UpdateMessageText()
{
    if(!m_Messages.empty()) {
        m_Text        = m_Messages.front().m_Text;
        m_Position    = 0;
        m_Color       = m_Messages.front().m_Color;

        m_PreRendered = m_Font->RenderMultilineText(m_Text, m_Width);
    }
}

void cMessageBox::ChangeFontSize(int FontSize)
{
    m_Font = std::make_unique<cFont>(g_Graphics.LoadFont(cfg.fonts.normal(), FontSize));
    m_Font->SetColor(g_MessageBoxTextColor.r, g_MessageBoxTextColor.g, g_MessageBoxTextColor.b);
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

bool cMessageBox::IsActive() const
{
    return !m_Messages.empty();
}
