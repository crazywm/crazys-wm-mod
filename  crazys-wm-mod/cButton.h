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
#ifndef __CBUTTON_H
#define __CBUTTON_H

#include <string>
using namespace std;
#include "CSurface.h"
#include "cInterfaceObject.h"
#include "cInterfaceEvent.h"

class CSurface;

class cButton : public cInterfaceObject
{
	bool m_Hidden;
public:
	cButton() {
		m_CurrImage = m_OffImage = m_DisabledImage = m_OnImage = 0;
		m_Next=0;
		m_Disabled=false;
		m_Hidden=false;
	}
	~cButton();

	bool CreateButton(string OffImage, string DisabledImage, string OnImage, int ID, int x, int y, int width, int height, bool transparency = false,bool cached=false);
	bool IsOver(int x, int y);
	bool ButtonClicked(int x, int y);
	void SetDisabled(bool disable) 
	{
		m_Disabled = disable;
		if(disable)
			m_CurrImage = m_DisabledImage;
		else
			m_CurrImage = m_OffImage;
	}

	virtual void Draw();

	void hide()	{ m_Hidden = true; }
	void unhide()	{ m_Hidden = false; }
	void toggle()	{ m_Hidden = !m_Hidden; }

	CSurface* m_OffImage;
	CSurface* m_DisabledImage;
	CSurface* m_OnImage;
	CSurface* m_CurrImage;

	bool m_Disabled;

	int m_ID;
	
	cButton* m_Next;	// next button on the window
};


#endif
