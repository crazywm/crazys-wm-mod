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
#ifndef __CCHECKBOX_H
#define __CCHECKBOX_H

#include <memory>
#include "cInterfaceObject.h"
#include "cInterfaceEvent.h"
#include "cFont.h"

class CSurface;

class cCheckBox : public cUIWidget
{
public:
	cCheckBox(int id, int x, int y, int width, int height, string text, int fontsize = 12, bool leftorright = false);
	~cCheckBox();

	bool GetState() {return m_StateOn;}
	void ButtonClicked(int x, int y);
	void SetState(bool on) {m_StateOn = on;}

	void DrawWidget() override;

	std::unique_ptr<CSurface> m_Image;
	SDL_Surface* m_Surface = nullptr;
	SDL_Surface* m_Border = nullptr;
	bool m_StateOn = false;
	cFont m_Font;
	bool m_Disabled = false;
};

#endif
