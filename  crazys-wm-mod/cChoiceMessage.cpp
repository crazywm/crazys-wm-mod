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
#include "IconSurface.h"
#include "CGraphics.h"
#include "sConfig.h"
#include "XmlUtil.h"
#include "tinyxml.h"

extern CGraphics g_Graphics;

extern cConfig cfg;
extern	bool	g_UpArrow;
extern	bool	g_DownArrow;
extern	bool	g_EnterKey;

extern unsigned char g_ChoiceMessageTextR, g_ChoiceMessageTextG, g_ChoiceMessageTextB;
extern unsigned char g_ChoiceMessageBorderR, g_ChoiceMessageBorderG, g_ChoiceMessageBorderB;
extern unsigned char g_ChoiceMessageHeaderR, g_ChoiceMessageHeaderG, g_ChoiceMessageHeaderB;
extern unsigned char g_ChoiceMessageBackgroundR, g_ChoiceMessageBackgroundG, g_ChoiceMessageBackgroundB;
extern unsigned char g_ChoiceMessageSelectedR, g_ChoiceMessageSelectedG, g_ChoiceMessageSelectedB;

cChoice::cChoice() 
{
	m_Next = 0;
	m_Choices = 0;
	m_NumChoices = 0;
	m_CurrChoice = -1;
	m_Background = m_Border = 0;
	m_ElementSelectedBackground = m_ElementBackground = 0;
	m_Position = 0;
	m_HeaderBackground = 0;
	m_ScrollDisabled = false;
}
cChoice::~cChoice()
{
	if (m_Next)							delete m_Next;
	m_Next = 0;
	if (m_Choices)						delete[] m_Choices;
	m_Choices = 0;
	if (m_Background)					SDL_FreeSurface(m_Background);
	m_Background = 0;
	if (m_Border)						SDL_FreeSurface(m_Border);
	m_Border = 0;
	if (m_ElementBackground)			SDL_FreeSurface(m_ElementBackground);
	m_ElementBackground = 0;
	if (m_ElementSelectedBackground)	SDL_FreeSurface(m_ElementSelectedBackground);
	m_ElementSelectedBackground = 0;
	if (m_HeaderBackground)				SDL_FreeSurface(m_HeaderBackground);
	m_HeaderBackground = 0;
}

void cChoiceManager::Free()
{
	if (m_Parent)	delete m_Parent;
	m_Parent = 0;
	m_ActiveChoice = 0;
	m_CurrUp = m_CurrDown = 0;
	if (m_Font)
	{
		m_Font->Free();
		delete m_Font;
	}
	m_Font = 0;

	// I'm leaving these for the resource manager, which I think should collect them
	// If I'm wrong, they'll leak memory -- doc
	// if(m_UpOn) delete m_UpOn; m_UpOn = 0;
	// if(m_DownOn) delete m_DownOn; m_DownOn = 0;
	// if(m_UpOff) delete m_UpOff; m_UpOff = 0;
	// if(m_DownOff) delete m_DownOff; m_DownOff = 0;
}

void cChoiceManager::CreateChoiceBoxResize(int ID, int numChoices)
{
	cChoice* newChoice = new cChoice();
	newChoice->m_NumChoices = numChoices;
	newChoice->m_Choices = new string[numChoices];
	newChoice->m_ID = ID;

	if (m_Parent)
	{
		cChoice* current = m_Parent;
		while (current->m_Next)
			current = current->m_Next;
		current->m_Next = newChoice;
	}
	else
		m_Parent = newChoice;
}

void cChoiceManager::BuildChoiceBox(int ID, int MaxStrLen)
{
	cChoice* newChoice = m_Parent;
	while (newChoice)
	{
		if (newChoice->m_ID == ID)	break;
		newChoice = newChoice->m_Next;
	}

	if (newChoice)
	{
		if (m_Font == 0)
		{
			m_Font = new cFont();
			m_Font->LoadFont(cfg.fonts.normal(), (m_Parent->m_FontSize > 0 ? m_Parent->m_FontSize : 16));
			m_Font->SetText("");
			m_Font->SetColor(g_ChoiceMessageTextR, g_ChoiceMessageTextG, g_ChoiceMessageTextB);
		}

		int MaxWidth = 0, MaxHeight = 0;
		string temp = "";
		for (int i = 0; i<MaxStrLen - 1; i += 2)
			temp += "oW";
		m_Font->GetSize(temp, MaxWidth, MaxHeight);
		int newHeight = (MaxHeight*newChoice->m_NumChoices) + 2;

		if (newHeight > g_Graphics.GetHeight())
			newHeight = g_Graphics.GetHeight() - 34;
		if (MaxWidth > g_Graphics.GetWidth())
			MaxWidth = g_Graphics.GetWidth() - 2;

		newChoice->m_NumDrawnElements = newHeight / MaxHeight;
		if (newChoice->m_NumDrawnElements >= newChoice->m_NumChoices)
		{
			newChoice->m_ScrollDisabled = true;
			newChoice->m_eWidth = MaxWidth;
		}
		else
			newChoice->m_eWidth = (MaxWidth - (18));
		newChoice->m_eHeight = MaxHeight;

		newChoice->m_CurrChoice = -1;
		newChoice->m_Width = MaxWidth;
		newChoice->m_Height = newHeight;

		if (newChoice->m_ScrollDisabled)
			newChoice->m_Border = SDL_CreateRGBSurface(SDL_SWSURFACE, newChoice->m_Width + 2, newChoice->m_Height + 2, 32, 0, 0, 0, 0);
		else
			newChoice->m_Border = SDL_CreateRGBSurface(SDL_SWSURFACE, newChoice->m_Width + 20, newChoice->m_Height + 2, 32, 0, 0, 0, 0);
		SDL_FillRect(newChoice->m_Border, 0, SDL_MapRGB(newChoice->m_Border->format, g_ChoiceMessageBorderR, g_ChoiceMessageBorderG, g_ChoiceMessageBorderB));

		if (newChoice->m_ScrollDisabled)
			newChoice->m_Background = SDL_CreateRGBSurface(SDL_SWSURFACE, newChoice->m_Width, newChoice->m_Height, 32, 0, 0, 0, 0);
		else
			newChoice->m_Background = SDL_CreateRGBSurface(SDL_SWSURFACE, newChoice->m_Width - (18), newChoice->m_Height - 2, 32, 0, 0, 0, 0);
		SDL_FillRect(newChoice->m_Background, 0, SDL_MapRGB(newChoice->m_Background->format, g_ChoiceMessageBackgroundR, g_ChoiceMessageBackgroundG, g_ChoiceMessageBackgroundB));

		newChoice->m_ElementBackground = SDL_CreateRGBSurface(SDL_SWSURFACE, newChoice->m_eWidth, newChoice->m_eHeight, 32, 0, 0, 0, 0);
		SDL_FillRect(newChoice->m_ElementBackground, 0, SDL_MapRGB(newChoice->m_ElementBackground->format, g_ChoiceMessageBackgroundR, g_ChoiceMessageBackgroundG, g_ChoiceMessageBackgroundB));

		newChoice->m_ElementSelectedBackground = SDL_CreateRGBSurface(SDL_SWSURFACE, newChoice->m_eWidth, newChoice->m_eHeight, 32, 0, 0, 0, 0);
		SDL_FillRect(newChoice->m_ElementSelectedBackground, 0, SDL_MapRGB(newChoice->m_ElementSelectedBackground->format, g_ChoiceMessageSelectedR, g_ChoiceMessageSelectedG, g_ChoiceMessageSelectedB));

		if (newChoice->m_eWidth > 120)
			newChoice->m_HeaderBackground = SDL_CreateRGBSurface(SDL_SWSURFACE, newChoice->m_eWidth, 32, 32, 0, 0, 0, 0);
		else
			newChoice->m_HeaderBackground = SDL_CreateRGBSurface(SDL_SWSURFACE, 120, 32, 32, 0, 0, 0, 0);
		SDL_FillRect(newChoice->m_HeaderBackground, 0, SDL_MapRGB(newChoice->m_HeaderBackground->format, g_ChoiceMessageHeaderR, g_ChoiceMessageHeaderG, g_ChoiceMessageHeaderB));

		newChoice->m_XPos = ((g_Graphics.GetWidth() / 2) - (newChoice->m_Width / 2));
		newChoice->m_YPos = ((g_Graphics.GetHeight() / 2) - (newChoice->m_Height / 2));

		if (m_UpOn == 0) m_UpOn = new ButtonSurface("UpOn");
		if (m_UpOff == 0) m_UpOff = new ButtonSurface("UpOff");
		m_CurrUp = m_UpOff;

		if (m_DownOn == 0) m_DownOn = new ButtonSurface("DownOn");
		if (m_DownOff == 0) m_DownOff = new ButtonSurface("DownOff");
		m_CurrDown = m_DownOff;
	}
}

void cChoiceManager::CreateChoiceBox(int x, int y, int width, int height, int ID, int numChoices, int itemHeight, int MaxStrLen, int fontsize)
{
	DirPath dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "ChoiceBox.xml";
	TiXmlDocument docChoiceBox(dp.c_str());
	int a, b, c, d, e, f;
	if (docChoiceBox.LoadFile())
	{
//<Window  Name = "ChoiceBox" XPos = "224" YPos = "127" Width = "352" Height = "160" RowHeight = "32" FontSize = "16" / >

		string m_filename = dp.c_str();
		TiXmlElement *el, *root_el = docChoiceBox.RootElement();
		for (el = root_el->FirstChildElement(); el; el = el->NextSiblingElement())
		{
			XmlUtil xu(m_filename);
			xu.get_att(el, "XPos", a); xu.get_att(el, "YPos", b); xu.get_att(el, "Width", c); xu.get_att(el, "Height", d);
			xu.get_att(el, "RowHeight", e); xu.get_att(el, "FontSize", f);
		}
		if (a > 0)	x = a;
		if (b > 0)	y = b;
		if (c > 0)	width = c;
		if (d > 0)	height = d;
		if (e > 0)	itemHeight = e;
		if (f > 0)	fontsize = f;
	}


	cChoice* newChoice = 0;
	if (m_Font == 0)
	{
		m_Font = new cFont();
		m_Font->LoadFont(cfg.fonts.normal(), fontsize);
		m_Font->SetText("");
		m_Font->SetColor(0, 0, 0);
	}

	if (MaxStrLen == 0)
	{
		newChoice = new cChoice();
		if ((height - 2) / itemHeight < numChoices)
			height = (numChoices*itemHeight) + 2;
		newChoice->m_NumDrawnElements = (height - 2) / itemHeight;
		newChoice->m_eWidth = (width - (18));
		newChoice->m_eHeight = itemHeight;
		newChoice->m_NumChoices = numChoices;

		newChoice->m_CurrChoice = -1;
		newChoice->m_XPos = x;
		newChoice->m_YPos = y;
		newChoice->m_Width = width;
		newChoice->m_Height = height;
		newChoice->m_FontSize = fontsize;
		newChoice->m_Choices = new string[numChoices];
		newChoice->m_ID = ID;

		newChoice->m_Border = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, 0, 0, 0, 0);
		SDL_FillRect(newChoice->m_Border, 0, SDL_MapRGB(newChoice->m_Border->format, 0, 0, 0));

		newChoice->m_Background = SDL_CreateRGBSurface(SDL_SWSURFACE, width - (18), height - 2, 32, 0, 0, 0, 0);
		SDL_FillRect(newChoice->m_Background, 0, SDL_MapRGB(newChoice->m_Background->format, 88, 163, 113));

		newChoice->m_ElementBackground = SDL_CreateRGBSurface(SDL_SWSURFACE, newChoice->m_eWidth, newChoice->m_eHeight, 32, 0, 0, 0, 0);
		SDL_FillRect(newChoice->m_ElementBackground, 0, SDL_MapRGB(newChoice->m_ElementBackground->format, 88, 163, 113));

		newChoice->m_ElementSelectedBackground = SDL_CreateRGBSurface(SDL_SWSURFACE, newChoice->m_eWidth, newChoice->m_eHeight, 32, 0, 0, 0, 0);
		SDL_FillRect(newChoice->m_ElementSelectedBackground, 0, SDL_MapRGB(newChoice->m_ElementSelectedBackground->format, 229, 227, 52));

		if (newChoice->m_eWidth > 120)
			newChoice->m_HeaderBackground = SDL_CreateRGBSurface(SDL_SWSURFACE, newChoice->m_eWidth, 32, 32, 0, 0, 0, 0);
		else
			newChoice->m_HeaderBackground = SDL_CreateRGBSurface(SDL_SWSURFACE, 120, 32, 32, 0, 0, 0, 0);
		SDL_FillRect(newChoice->m_HeaderBackground, 0, SDL_MapRGB(newChoice->m_HeaderBackground->format, 229, 227, 52));
	}
	else	// autosize and center according to the max string size, also shut off the scroll box if not needed
	{
		newChoice = new cChoice();

		int MaxWidth = 0, MaxHeight = 0;
		string temp = "W";
		for (int i = 0; i<MaxStrLen - 1; i++)
			temp += "W";
		m_Font->GetSize(temp, MaxWidth, MaxHeight);
		int newHeight = (MaxHeight*numChoices) + 2;

		if (newHeight > g_Graphics.GetHeight())
			newHeight = g_Graphics.GetHeight() - 34;
		if (MaxWidth > g_Graphics.GetWidth())
			MaxWidth = g_Graphics.GetWidth() - 2;

		newChoice->m_NumDrawnElements = newHeight / MaxHeight;
		if (newChoice->m_NumDrawnElements >= numChoices)
		{
			newChoice->m_ScrollDisabled = true;
			newChoice->m_eWidth = (MaxWidth);
		}
		else
			newChoice->m_eWidth = (MaxWidth - (18));
		newChoice->m_eHeight = MaxHeight;
		newChoice->m_NumChoices = numChoices;

		newChoice->m_CurrChoice = -1;
		newChoice->m_Width = MaxWidth;
		newChoice->m_Height = newHeight;
		newChoice->m_FontSize = fontsize;
		newChoice->m_Choices = new string[numChoices];
		newChoice->m_ID = ID;

		if (newChoice->m_ScrollDisabled)
			newChoice->m_Border = SDL_CreateRGBSurface(SDL_SWSURFACE, newChoice->m_Width + 2, newChoice->m_Height + 2, 32, 0, 0, 0, 0);
		else
			newChoice->m_Border = SDL_CreateRGBSurface(SDL_SWSURFACE, newChoice->m_Width + 20, newChoice->m_Height + 2, 32, 0, 0, 0, 0);
		SDL_FillRect(newChoice->m_Border, 0, SDL_MapRGB(newChoice->m_Border->format, 0, 0, 0));

		if (newChoice->m_ScrollDisabled)
			newChoice->m_Background = SDL_CreateRGBSurface(SDL_SWSURFACE, newChoice->m_Width, newChoice->m_Height, 32, 0, 0, 0, 0);
		else
			newChoice->m_Background = SDL_CreateRGBSurface(SDL_SWSURFACE, newChoice->m_Width - (18), newChoice->m_Height - 2, 32, 0, 0, 0, 0);
		SDL_FillRect(newChoice->m_Background, 0, SDL_MapRGB(newChoice->m_Background->format, 88, 163, 113));

		newChoice->m_ElementBackground = SDL_CreateRGBSurface(SDL_SWSURFACE, newChoice->m_eWidth, newChoice->m_eHeight, 32, 0, 0, 0, 0);
		SDL_FillRect(newChoice->m_ElementBackground, 0, SDL_MapRGB(newChoice->m_ElementBackground->format, 88, 163, 113));

		newChoice->m_ElementSelectedBackground = SDL_CreateRGBSurface(SDL_SWSURFACE, newChoice->m_eWidth, newChoice->m_eHeight, 32, 0, 0, 0, 0);
		SDL_FillRect(newChoice->m_ElementSelectedBackground, 0, SDL_MapRGB(newChoice->m_ElementSelectedBackground->format, 229, 227, 52));

		if (newChoice->m_eWidth > 120)
			newChoice->m_HeaderBackground = SDL_CreateRGBSurface(SDL_SWSURFACE, newChoice->m_eWidth, 32, 32, 0, 0, 0, 0);
		else
			newChoice->m_HeaderBackground = SDL_CreateRGBSurface(SDL_SWSURFACE, 120, 32, 32, 0, 0, 0, 0);
		SDL_FillRect(newChoice->m_HeaderBackground, 0, SDL_MapRGB(newChoice->m_HeaderBackground->format, 229, 227, 52));

		newChoice->m_XPos = ((g_Graphics.GetWidth() / 2) - (newChoice->m_Width / 2));
		newChoice->m_YPos = ((g_Graphics.GetHeight() / 2) - (newChoice->m_Height / 2));
	}

	if (m_UpOn == 0) m_UpOn = new ButtonSurface("UpOn");
	if (m_UpOff == 0) m_UpOff = new ButtonSurface("UpOff");
	m_CurrUp = m_UpOff;

	if (m_DownOn == 0) m_DownOn = new ButtonSurface("DownOn");
	if (m_DownOff == 0) m_DownOff = new ButtonSurface("DownOff");
	m_CurrDown = m_DownOff;

	if (!m_Parent) {
		m_Parent = newChoice;
		return;
	}
	cChoice* current = m_Parent;
	while (current->m_Next)
		current = current->m_Next;
	current->m_Next = newChoice;
}

void cChoiceManager::Question(int ID, string text)
{
	cChoice* current = m_Parent;
	while (current)
	{
		if (current->m_ID == ID)
		{
			current->m_Question = text;
			break;
		}
		current = current->m_Next;
	}
}

void cChoiceManager::AddChoice(int ID, string text, int choiceID)
{
	cChoice* current = m_Parent;
	while (current)
	{
		if (current->m_ID == ID)
		{
			current->m_Choices[choiceID] = text;
			break;
		}

		current = current->m_Next;
	}
}

void cChoiceManager::Draw()
{
	SDL_Rect offset;

	if (m_ActiveChoice)
	{
		if (m_ActiveChoice->m_Background && m_ActiveChoice->m_Border)
		{
			// Draw the window
			offset.x = m_ActiveChoice->m_XPos;
			offset.y = m_ActiveChoice->m_YPos;

			// blit to the screen
			SDL_BlitSurface(m_ActiveChoice->m_Border, 0, g_Graphics.GetScreen(), &offset);

			offset.x = m_ActiveChoice->m_XPos + 1;
			offset.y = m_ActiveChoice->m_YPos + 1;
			SDL_BlitSurface(m_ActiveChoice->m_Background, 0, g_Graphics.GetScreen(), &offset);
		}

		// Draw the heading text
		if (m_ActiveChoice->m_HeaderBackground)
		{

			string question = "Select Choice";
			if (m_ActiveChoice->m_Question.length() > 0)
				question = m_ActiveChoice->m_Question;
			m_Font->SetText(question);
			m_Font->SetMultiline(true, m_ActiveChoice->m_Width, m_ActiveChoice->m_Height);

			offset.x = m_ActiveChoice->m_XPos;
			offset.y = m_ActiveChoice->m_YPos - m_Font->GetHeight();
			offset.h = m_Font->GetHeight();
			offset.w = m_ActiveChoice->m_Border->w;
			m_ActiveChoice->m_HeaderBackground = SDL_CreateRGBSurface(SDL_SWSURFACE, offset.w, m_Font->GetHeight(), 32, 0, 0, 0, 0);
			SDL_FillRect(m_ActiveChoice->m_HeaderBackground, 0, SDL_MapRGB(m_ActiveChoice->m_HeaderBackground->format, g_ChoiceMessageHeaderR, g_ChoiceMessageHeaderG, g_ChoiceMessageHeaderB));

			//m_Font->DrawText(offset.x, offset.y);
			SDL_BlitSurface(m_ActiveChoice->m_HeaderBackground, 0, g_Graphics.GetScreen(), &offset);
			m_Font->DrawMultilineText(offset.x, offset.y);
		}

		for (int i = m_ActiveChoice->m_Position, j = 0; i<m_ActiveChoice->m_NumChoices && j<m_ActiveChoice->m_NumDrawnElements; i++, j++)
		{
			// Draw the window
			offset.x = m_ActiveChoice->m_XPos + 1;
			offset.y = (m_ActiveChoice->m_YPos + 1) + (m_ActiveChoice->m_eHeight*j);

			// blit to the screen
			if (i == m_ActiveChoice->m_CurrChoice)
				SDL_BlitSurface(m_ActiveChoice->m_ElementSelectedBackground, 0, g_Graphics.GetScreen(), &offset);
			else
				SDL_BlitSurface(m_ActiveChoice->m_ElementBackground, 0, g_Graphics.GetScreen(), &offset);

			// draw the text
			m_Font->SetText(m_ActiveChoice->m_Choices[i]);
			m_Font->DrawText(offset.x, offset.y);
		}


		// draw the up and down buttons
		if (!m_ActiveChoice->m_ScrollDisabled)
		{
			SDL_Rect rect;
			rect.y = rect.x = 0;
			rect.w = 16;
			rect.h = 16;

			m_CurrUp->DrawSurface(m_ActiveChoice->m_XPos + m_ActiveChoice->m_Width - 17, m_ActiveChoice->m_YPos + 1, 0, &rect);
			m_CurrDown->DrawSurface(m_ActiveChoice->m_XPos + m_ActiveChoice->m_Width - 17, m_ActiveChoice->m_YPos + m_ActiveChoice->m_Height - 17, 0, &rect);
		}
	}
}

int cChoiceManager::GetChoice(int ID)
{
	cChoice* current = m_Parent;
	while (current)
	{
		if (current->m_ID == ID)
			return current->m_CurrChoice;

		current = current->m_Next;
	}

	return -1;
}

void cChoiceManager::SetActive(int ID)
{
	cChoice* current = m_Parent;
	while (current)
	{
		if (current->m_ID == ID)
		{
			m_ActiveChoice = current;
			m_ActiveChoice->m_CurrChoice = -1;
			m_ActiveChoice->m_Position = 0;
			return;
		}

		current = current->m_Next;
	}

	m_ActiveChoice = 0;
}

bool cChoiceManager::IsActive()
{
	if (m_ActiveChoice)
		return true;
	return false;
}

bool cChoiceManager::IsOver(int x, int y)
{
	if (!IsActive())
		return false;

	bool over = false;
	if (x > m_ActiveChoice->m_XPos && y > m_ActiveChoice->m_YPos && x < m_ActiveChoice->m_XPos + m_ActiveChoice->m_Width && y < m_ActiveChoice->m_YPos + m_ActiveChoice->m_Height)
	{
		// check if over the buttons
		if (!m_ActiveChoice->m_ScrollDisabled)
		{
			if (x < m_ActiveChoice->m_XPos + m_ActiveChoice->m_Width - 1 && y < m_ActiveChoice->m_YPos + 17 && x > m_ActiveChoice->m_XPos - 16 + m_ActiveChoice->m_Width && y > m_ActiveChoice->m_YPos + 1)
			{
				m_CurrUp = m_UpOn;
			}
			else
			{
				m_CurrUp = m_UpOff;
			}

			if (x < m_ActiveChoice->m_XPos + m_ActiveChoice->m_Width - 1 && y < m_ActiveChoice->m_YPos + m_ActiveChoice->m_Height - 1 && x > m_ActiveChoice->m_XPos - 16 + m_ActiveChoice->m_Width && y > m_ActiveChoice->m_YPos + m_ActiveChoice->m_Height - 17)
			{
				m_CurrDown = m_DownOn;
			}
			else
			{
				m_CurrDown = m_DownOff;
			}
		}

		over = true;
	}
	else
	{
		m_CurrUp = m_UpOff;
		m_CurrDown = m_DownOff;
	}

	// now highlight the choice the mouse is over
	if (over)
	{
		int i, j;
		for (i = m_ActiveChoice->m_Position, j = 0; i<m_ActiveChoice->m_NumChoices && j<m_ActiveChoice->m_NumDrawnElements; i++, j++)
		{
			int cX = m_ActiveChoice->m_XPos + 1;
			int cY = (m_ActiveChoice->m_YPos + 1) + (m_ActiveChoice->m_eHeight*j);

			// Check if over the item
			if (x > cX && y > cY && x < cX + m_ActiveChoice->m_eWidth && y < cY + m_ActiveChoice->m_eHeight)
			{
				// then select it and deactivate the choice box;
				m_ActiveChoice->m_CurrChoice = i;
			}
		}
	}

	return over;
}

bool cChoiceManager::ButtonClicked(int x, int y)
{
	if (!IsActive())
	{
		return false;
	}
	if (!IsOver(x, y) && !g_UpArrow && !g_DownArrow && !g_EnterKey)
	{
		return false;
	}

	if (g_EnterKey)
	{
		if (m_callback) {
			m_callback(m_ActiveChoice->m_CurrChoice);
			m_callback = 0;
		}
		m_ActiveChoice = 0;
		return true;
	}

	if (g_UpArrow)
	{
		g_UpArrow = false;
		m_ActiveChoice->m_CurrChoice--;
		if (m_ActiveChoice->m_CurrChoice < 0)
			m_ActiveChoice->m_CurrChoice = m_ActiveChoice->m_NumChoices - 1;
		return true;
	}
	if (g_DownArrow)
	{
		g_DownArrow = false;
		m_ActiveChoice->m_CurrChoice++;
		if (m_ActiveChoice->m_CurrChoice > m_ActiveChoice->m_NumChoices - 1)
			m_ActiveChoice->m_CurrChoice = 0;
		return true;
	}

	if (m_ActiveChoice->m_ScrollDisabled) {
		bool bv = find_active(x, y);
		return bv;
	}

	if (m_CurrUp == m_UpOn)
	{
		if (m_ActiveChoice->m_Position - 1 >= 0)
			m_ActiveChoice->m_Position--;
		return true;
	}

	if (m_CurrDown == m_DownOn)
	{
		if (m_ActiveChoice->m_Position + m_ActiveChoice->m_NumDrawnElements < m_ActiveChoice->m_NumChoices)
			m_ActiveChoice->m_Position++;
		return true;
	}

	return find_active(x, y);
}

bool cChoiceManager::find_active(int x, int y)
{
	int i, j;
	for (i = m_ActiveChoice->m_Position, j = 0; i<m_ActiveChoice->m_NumChoices && j<m_ActiveChoice->m_NumDrawnElements; i++, j++) {
		int cX = m_ActiveChoice->m_XPos + 1;
		int cY = (m_ActiveChoice->m_YPos + 1) + (m_ActiveChoice->m_eHeight*j);

		// Check if over the item
		if (x > cX && y > cY && x < cX + m_ActiveChoice->m_eWidth && y < cY + m_ActiveChoice->m_eHeight) {
			// then select it and deactivate the choice box;
			m_ActiveChoice->m_CurrChoice = i;
			if (m_callback) {
				m_callback(i);
				m_callback = 0;
			}
			m_ActiveChoice = 0;

			return true;
		}
	}
	return false;
}