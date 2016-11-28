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
#include "Globals.h"

extern CGraphics g_Graphics;
extern cConfig cfg;

extern unsigned char g_MessageBoxBorderR, g_MessageBoxBorderG, g_MessageBoxBorderB;
extern unsigned char g_MessageBoxBackground0R, g_MessageBoxBackground0G, g_MessageBoxBackground0B;
extern unsigned char g_MessageBoxBackground1R, g_MessageBoxBackground1G, g_MessageBoxBackground1B;
extern unsigned char g_MessageBoxBackground2R, g_MessageBoxBackground2G, g_MessageBoxBackground2B;
extern unsigned char g_MessageBoxBackground3R, g_MessageBoxBackground3G, g_MessageBoxBackground3B;
extern unsigned char g_MessageBoxBackground4R, g_MessageBoxBackground4G, g_MessageBoxBackground4B;
extern unsigned char g_MessageBoxTextR, g_MessageBoxTextG, g_MessageBoxTextB;

bool loadfile = false;




cMessageBox::cMessageBox()
{
	m_Color = 0;
	m_TextAdvance = false;
	m_Font = 0;
	m_Text = "";
	for (int i = 0; i < NUM_MESSBOXCOLOR; i++)
		m_Background[i] = 0;
	m_Border = 0;
	m_Active = false;
	m_Advance = false;
	m_Position = 0;
}


cMessageBox::~cMessageBox()
{
	for(int i=0; i<NUM_MESSBOXCOLOR; i++)
	{
		if(m_Background[i])
			SDL_FreeSurface(m_Background[i]);
		m_Background[i] = 0;
	}

	if(m_Border)
		SDL_FreeSurface(m_Border);
	m_Border = 0;

	if(m_Font)
	{
		m_Font->Free();
		delete m_Font;
	}
	m_Font= 0;
}

void cMessageBox::CreateWindow(int x, int y, int width, int height, int BorderSize, int FontSize, bool scale)
{
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
		if (_G.g_ScreenWidth != cfg.resolution.width())		xScale = (float)((float)_G.g_ScreenWidth / (float)cfg.resolution.width());
		if (_G.g_ScreenHeight != cfg.resolution.height())	yScale = (float)((float)_G.g_ScreenHeight / (float)cfg.resolution.height());
	}

	x = (int)((float)x*xScale);
	y = (int)((float)y*yScale);
	width = (int)((float)width*xScale);
	height = (int)((float)height*yScale);
	FontSize = (int)((float)FontSize*yScale);

	m_BorderSize = BorderSize;
	m_XPos=x;
	m_YPos = y;
	m_Width = width;
	m_Height = height;
	m_FontHeight = FontSize;
	m_Border = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, 0,0,0,0);
	SDL_FillRect(m_Border,0,SDL_MapRGB(m_Border->format,g_MessageBoxBorderR,g_MessageBoxBorderG,g_MessageBoxBorderB));

	m_Background[0] = SDL_CreateRGBSurface(SDL_SWSURFACE, width-(BorderSize*2), height-(BorderSize*2), 32, 0,0,0,0);
	SDL_FillRect(m_Background[0],0,SDL_MapRGB(m_Background[0]->format,g_MessageBoxBackground0R,g_MessageBoxBackground0G,g_MessageBoxBackground0B));

	m_Background[1] = SDL_CreateRGBSurface(SDL_SWSURFACE, width-(BorderSize*2), height-(BorderSize*2), 32, 0,0,0,0);
	SDL_FillRect(m_Background[1],0,SDL_MapRGB(m_Background[1]->format,g_MessageBoxBackground1R,g_MessageBoxBackground1G,g_MessageBoxBackground1B));

	m_Background[2] = SDL_CreateRGBSurface(SDL_SWSURFACE, width-(BorderSize*2), height-(BorderSize*2), 32, 0,0,0,0);
	SDL_FillRect(m_Background[2],0,SDL_MapRGB(m_Background[2]->format,g_MessageBoxBackground2R,g_MessageBoxBackground2G,g_MessageBoxBackground2B));

	m_Background[3] = SDL_CreateRGBSurface(SDL_SWSURFACE, width-(BorderSize*2), height-(BorderSize*2), 32, 0,0,0,0);
	SDL_FillRect(m_Background[3],0,SDL_MapRGB(m_Background[3]->format,g_MessageBoxBackground3R,g_MessageBoxBackground3G,g_MessageBoxBackground3B));

	m_Background[4] = SDL_CreateRGBSurface(SDL_SWSURFACE, width - (BorderSize * 2), height - (BorderSize * 2), 32, 0, 0, 0, 0);
	SDL_FillRect(m_Background[4], 0, SDL_MapRGB(m_Background[4]->format, g_MessageBoxBackground4R, g_MessageBoxBackground4G, g_MessageBoxBackground4B));

	ChangeFontSize(FontSize);
}

void cMessageBox::Draw()
{
	if(!m_Active)
		return;

	if(m_Background[m_Color] && m_Border)
	{
		// Draw the window
		SDL_Rect offset;
		offset.x = m_XPos;
		offset.y = m_YPos;

		// blit to the screen
		SDL_BlitSurface(m_Border, 0, g_Graphics.GetScreen(), &offset);

		offset.x = m_XPos+m_BorderSize;
		offset.y = m_YPos+m_BorderSize;
		SDL_BlitSurface(m_Background[m_Color], 0, g_Graphics.GetScreen(), &offset);
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

	if(m_Position >= m_Font->GetTotalNumberOfLines())
		m_Active = false;
	
	m_Advance = true;
}

void cMessageBox::ChangeFontSize(int FontSize)
{
	if(m_Font)
	{
		m_Font->Free();
		delete m_Font;
	}
	m_Font = 0;
	m_Font = new cFont();
	m_Font->LoadFont(cfg.fonts.normal(), FontSize);
	m_Font->SetText("");
	m_Font->SetColor(g_MessageBoxTextR,g_MessageBoxTextG,g_MessageBoxTextB);
	m_Font->SetMultiline(true, m_Width, m_Height);
}
