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

#include "cSurface.h"
#include "cFont.h"
#include "cInterfaceObject.h"

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <SDL_keyboard.h>

class cChoice : public cUIWidget	// represents a list of text selections and the currently selected text
{
public:
	cChoice(int x, int y, int width, int height, int ID, std::string question, std::vector<std::string> options, int fontsize, cInterfaceWindow* parent);

    int num_choices() const;

    void DrawWidget(const CGraphics& gfx) override;

    void Question(std::string text);
    void AddChoice(std::string text, int choiceID);
    void SetCallback(std::function<void(int)> callback);
private:
    void HandleMouseMove(bool over, int x, int y) override;
    bool HandleClick(int x, int y, bool press) override;
    bool HandleKeyPress(SDL_keysym key) override;
    int FindActive(int x, int y) const;

	std::string m_Question;						// `J` The question at the top of the choice box
	std::vector<std::string> m_Choices;			// array of choices available
	std::vector<cSurface> m_ChoicesSurface;     // array of pre-rendered choice text
	cSurface m_QuestionSurface;
	int m_CurrChoice = -1;						// The choice selected at present
	cSurface m_Background;
    cSurface m_Border;
    cSurface m_ElementBackground;		        // the background and border for the list elements
    cSurface m_ElementSelectedBackground;	    // the background and border for the list elements
    cSurface m_HeaderBackground;
    cFont m_Font;
	int m_FontSize;

	int m_NumDrawnElements;
	int m_eWidth;
	int m_eHeight;
	int m_Position = 0;

	bool m_ScrollDisabled = false;
	// the scroll button
    cSurface m_UpOn;
    cSurface m_DownOn;
    cSurface m_UpOff;
    cSurface m_DownOff;
    cSurface m_CurrUp;
    cSurface m_CurrDown;

    std::function<void(int)> m_Callback;
};
