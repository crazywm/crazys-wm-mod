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

#include "CSurface.h"
#include "cInterfaceObject.h"
#include "cInterfaceEvent.h"
#include "cFont.h"

class CSurface;

class cCheckBox : public cInterfaceObject
{
public:
	cCheckBox() {m_Image = 0;m_Next=0;m_Border=m_Surface=0;m_StateOn=false;m_Disabled=false;}
	~cCheckBox();

	bool CreateCheckBox(int id, int x, int y, int width, int height, string text, int fontsize = 12, bool leftorright = false);
	bool GetState() {return m_StateOn;}
	void ButtonClicked(int x, int y);
	void SetState(bool on) {m_StateOn = on;}

	void Draw();

	CSurface* m_Image;
	SDL_Surface* m_Surface;
	SDL_Surface* m_Border;
	int m_ID;
	bool m_StateOn;
	cCheckBox* m_Next;	// next button on the window
	cFont m_Font;
	bool m_Disabled;
};

#endif
