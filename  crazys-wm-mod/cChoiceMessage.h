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
#pragma once

#include "CSurface.h"
#include <string>
#include <memory>
#include <vector>

class cFont;

typedef void(*menu_callback_type)(int);

class cChoice	// represents a list of text selections and the currently selected text
{
public:

	cChoice(int ID, int num_choices);
	~cChoice();

	int num_choices() const;

	std::string m_Question;						// `J` The question at the top of the choice box
	std::vector<std::string> m_Choices;			// array of choices available
	int m_CurrChoice;							// The choice selected at present
	int m_ID;									// the id for this particular box
	SDL_Surface* m_Background           = nullptr;
	SDL_Surface* m_Border               = nullptr;
	SDL_Surface* m_ElementBackground    = nullptr;		// the background and border for the list elements
	SDL_Surface* m_ElementSelectedBackground = nullptr;	// the background and border for the list elements
	SDL_Surface* m_HeaderBackground     = nullptr;
	int m_XPos, m_YPos, m_Width, m_Height, m_FontSize;

	int m_NumDrawnElements;
	int m_eWidth;
	int m_eHeight;
	int m_Position;

	bool m_ScrollDisabled;
};


class cChoiceManager
{
	menu_callback_type m_callback;
public:
	cChoiceManager();
	~cChoiceManager();

	void Free();
	void CreateChoiceBox(int x, int y, int width, int height, int ID, int numChoices, int itemHeight, int MaxStrLen = 0, int fontsize = 16);
	void CreateChoiceBoxResize(int ID, int numChoices);
	void BuildChoiceBox(int ID, int MaxStrLen);
	void Question(int ID, string text);
	void AddChoice(int ID, string text, int choiceID);
	void Draw();
	int GetChoice(int ID);
	void SetActive(int ID);
	bool IsActive();

	bool IsOver(int x, int y);
	bool ButtonClicked(int x, int y);

	void set_callback(menu_callback_type func) {
		m_callback = func;
	}
	bool find_active(int x, int y);

private:
    std::vector<std::unique_ptr<cChoice>> m_ChoiceBoxes;
    cChoice* m_ActiveChoice = nullptr;

	std::unique_ptr<cFont> m_Font;

	CSurface* m_UpOn = nullptr;
	CSurface* m_DownOn = nullptr;
	CSurface* m_UpOff = nullptr;
	CSurface* m_DownOff = nullptr;
	CSurface* m_CurrUp = nullptr;
	CSurface* m_CurrDown = nullptr;
};

