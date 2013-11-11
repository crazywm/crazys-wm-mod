/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders 
 * who meet on http://pinkpetal.co.cc
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
#ifndef __CINTERFACEOBJECT_H
#define __CINTERFACEOBJECT_H

class cInterfaceObject
{
public:
	void SetPosition(int x, int y, int width, int height) {m_XPos = x; m_YPos = y; m_Width = width; m_Height = height;}
	virtual void Draw() {};
	int GetXPos() {return m_XPos;}
	int GetYPos() {return m_YPos;}
	int GetWidth() {return m_Width;}
	int GetHeight() {return m_Height;}

protected:
	int m_XPos, m_YPos;
	int m_Width, m_Height;
};

#endif
