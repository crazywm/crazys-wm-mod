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
#include "interface/cSurface.h"
#include "interface/cInterfaceObject.h"

class cFont;

class cEditBox : public cUIWidget
{
public:
	cEditBox(cInterfaceWindow* parent, int ID, int x, int y, int width, int height, int BorderSize, int FontSize = 16);

    void ClearText();
	std::string GetText();

    void DrawWidget(const CGraphics& gfx) override;
    bool HandleSetFocus(bool focus) override;
    bool HandleClick(int x, int y, bool press) override;
    bool HandleKeyPress(SDL_keysym sym) override;

	std::unique_ptr<cFont> m_Text;

	cSurface m_FocusedBackground;
	cSurface m_Background;
	cSurface m_Border;
	int m_BorderSize;
};

#endif
