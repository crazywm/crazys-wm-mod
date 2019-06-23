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
using namespace std;
#include "CSurface.h"
#include "cInterfaceObject.h"
#include "cInterfaceEvent.h"
#include "cFont.h"

class cEditBox : public cInterfaceObject
{
	bool m_Hidden = false;

public:
	cEditBox() {m_Next = 0;m_FocusedBackground=m_Border=m_Background=0;m_Text=0;m_HasFocus=false;}
	~cEditBox();

	bool CreateEditBox(int ID, int x, int y, int width, int height, int BorderSize, int FontSize = 16);
	bool IsOver(int x, int y);
	bool OnClicked(int x, int y);
	void ClearText();
	string GetText() {return m_Text->GetText();}

	void UpdateText(char key, bool upper);

	void Draw();

	void hide()	{ m_Hidden = true; }
	void unhide()	{ m_Hidden = false; }
	void toggle()	{ m_Hidden = !m_Hidden; }

	int m_ID;
	cFont* m_Text;
	bool m_HasFocus;

	SDL_Surface* m_FocusedBackground;
	SDL_Surface* m_Background;
	SDL_Surface* m_Border;
	int m_BorderSize;
	
	cEditBox* m_Next;	// next button on the window
};

#endif
