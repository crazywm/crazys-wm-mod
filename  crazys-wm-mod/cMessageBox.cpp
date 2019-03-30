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
#include "CGraphics.h"
#include "sConfig.h"
#include "CLog.h"
#include "cFont.h"
#include "tinyxml.h"
#include "DirPath.h"
#include "XmlUtil.h"

extern CGraphics g_Graphics;
extern cConfig cfg;

extern unsigned char g_MessageBoxBorderR, g_MessageBoxBorderG, g_MessageBoxBorderB;
extern unsigned char g_MessageBoxBackground0R, g_MessageBoxBackground0G, g_MessageBoxBackground0B;
extern unsigned char g_MessageBoxBackground1R, g_MessageBoxBackground1G, g_MessageBoxBackground1B;
extern unsigned char g_MessageBoxBackground2R, g_MessageBoxBackground2G, g_MessageBoxBackground2B;
extern unsigned char g_MessageBoxBackground3R, g_MessageBoxBackground3G, g_MessageBoxBackground3B;
extern unsigned char g_MessageBoxTextR, g_MessageBoxTextG, g_MessageBoxTextB;

bool loadfile = false;




cMessageBox::cMessageBox(int x, int y, int width, int height, int BorderSize, int FontSize, bool scale)
{
	m_Font = nullptr;
	m_Text = "";
	for (auto & i : m_Background)
		i = nullptr;
	m_Border = nullptr;
	m_Advance = false;
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
        if (g_Graphics.GetWidth() != cfg.resolution.width())		xScale = (float)g_Graphics.GetWidth() / (float)cfg.resolution.width();
        if (g_Graphics.GetHeight() != cfg.resolution.height())	yScale = (float)g_Graphics.GetHeight() / (float)cfg.resolution.height();
    }

    x = (int)((float)x*xScale);
    y = (int)((float)y*yScale);
    width = (int)((float)width*xScale);
    height = (int)((float)height*yScale);
    FontSize = (int)((float)FontSize*yScale);

    m_BorderSize = BorderSize;
    m_XPos = x;
    m_YPos = y;
    m_Width = width;
    m_Height = height;
    m_FontHeight = FontSize;
    m_Border = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, 0,0,0,0);
    SDL_FillRect(m_Border,nullptr,SDL_MapRGB(m_Border->format,g_MessageBoxBorderR,g_MessageBoxBorderG,g_MessageBoxBorderB));

    m_Background[0] = SDL_CreateRGBSurface(SDL_SWSURFACE, width-(BorderSize*2), height-(BorderSize*2), 32, 0,0,0,0);
    SDL_FillRect(m_Background[0],nullptr,SDL_MapRGB(m_Background[0]->format,g_MessageBoxBackground0R,g_MessageBoxBackground0G,g_MessageBoxBackground0B));

    m_Background[1] = SDL_CreateRGBSurface(SDL_SWSURFACE, width-(BorderSize*2), height-(BorderSize*2), 32, 0,0,0,0);
    SDL_FillRect(m_Background[1],nullptr,SDL_MapRGB(m_Background[1]->format,g_MessageBoxBackground1R,g_MessageBoxBackground1G,g_MessageBoxBackground1B));

    m_Background[2] = SDL_CreateRGBSurface(SDL_SWSURFACE, width-(BorderSize*2), height-(BorderSize*2), 32, 0,0,0,0);
    SDL_FillRect(m_Background[2],nullptr,SDL_MapRGB(m_Background[2]->format,g_MessageBoxBackground2R,g_MessageBoxBackground2G,g_MessageBoxBackground2B));

    m_Background[3] = SDL_CreateRGBSurface(SDL_SWSURFACE, width-(BorderSize*2), height-(BorderSize*2), 32, 0,0,0,0);
    SDL_FillRect(m_Background[3],nullptr,SDL_MapRGB(m_Background[3]->format,g_MessageBoxBackground3R,g_MessageBoxBackground3G,g_MessageBoxBackground3B));

    ChangeFontSize(FontSize);
}


cMessageBox::~cMessageBox()
{
	for(auto & i : m_Background)
	{
		if(i)
			SDL_FreeSurface(i);
		i = nullptr;
	}

	if(m_Border)
		SDL_FreeSurface(m_Border);
	m_Border = nullptr;

	if(m_Font)
	{
		m_Font->Free();
		delete m_Font;
	}
	m_Font= nullptr;
}


void cMessageBox::Draw()
{
	if(!IsActive())
		return;

	if(m_Background[m_Color] && m_Border)
	{
		// Draw the window
		SDL_Rect offset;
		offset.x = m_XPos;
		offset.y = m_YPos;

		// blit to the screen
		SDL_BlitSurface(m_Border, nullptr, g_Graphics.GetScreen(), &offset);

		offset.x = m_XPos+m_BorderSize;
		offset.y = m_YPos+m_BorderSize;
		SDL_BlitSurface(m_Background[m_Color], nullptr, g_Graphics.GetScreen(), &offset);
	}

	if(m_Font)	// draw the text
	{
		if(m_Text != m_Font->GetText())
			m_Font->SetText(m_Text);
		m_Font->DrawMultilineText(m_XPos, m_YPos, m_Position);
	}
}

void cMessageBox::Advance()
{
	m_Position += m_Font->GetLinesPerBox();
	m_TextAdvance = false;

	if(m_Position >= m_Font->GetTotalNumberOfLines()) {
	    m_Messages.pop_front();
	    if(!m_Messages.empty()) {
            UpdateMessageText();
        }
    }
	
	m_Advance = true;
}

void cMessageBox::UpdateMessageText()
{
    if(!m_Messages.empty()) {
        m_Text        = m_Messages.front().m_Text;
        m_Position    = 0;
        m_TextAdvance = false;
        m_Color       = m_Messages.front().m_Color;
    }
}

void cMessageBox::ChangeFontSize(int FontSize)
{
	if(m_Font)
	{
		m_Font->Free();
		delete m_Font;
	}
	m_Font = nullptr;
	m_Font = new cFont();
	m_Font->LoadFont(cfg.fonts.normal(), FontSize);
	m_Font->SetText("");
	m_Font->SetColor(g_MessageBoxTextR,g_MessageBoxTextG,g_MessageBoxTextB);
	m_Font->SetMultiline(true, m_Width, m_Height);
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
