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
#ifndef __CEDITBOX_H
#define __CEDITBOX_H

#include <string>
#include <memory>
#include "CSurface.h"
#include "cInterfaceObject.h"

class cFont;

class cEditBox : public cUIWidget
{
public:
	cEditBox(int ID, int x, int y, int width, int height, int BorderSize, int FontSize = 16);
	~cEditBox();

	bool IsOver(int x, int y);
	bool OnClicked(int x, int y);
	void ClearText();
	std::string GetText();

	void UpdateText(char key, bool upper);

	void DrawWidget(const CGraphics& gfx) override;

	std::unique_ptr<cFont> m_Text;
	bool m_HasFocus = false;

	SDL_Surface* m_FocusedBackground = nullptr;
	SDL_Surface* m_Background = nullptr;
	SDL_Surface* m_Border = nullptr;
	int m_BorderSize;
};

#endif
