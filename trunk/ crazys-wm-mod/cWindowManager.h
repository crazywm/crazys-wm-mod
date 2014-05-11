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
#ifndef __CWINDOWMANAGER_H
#define __CWINDOWMANAGER_H
#include<stack>
#include<vector>

#include "cInterfaceWindow.h"

typedef struct sWindow
{
	cInterfaceWindow* m_Interface;	// pointer to the interface
	void (*Function)();	// pointer to the update function
	void (*XmlFunction)(cInterfaceWindow*);
	bool xmlfunc;
	sWindow* m_Next;

	sWindow()
	{
		m_Next = 0;
		m_Interface = 0;
	}

	~sWindow()
	{
		if(m_Next)
			delete m_Next;
		m_Next = 0;
		m_Interface = 0;
	}
}sWindow;

typedef void (*process_func)();
typedef void (*process_funcxml)(cInterfaceWindow *);

class cWindowManager
{
	map<string,cInterfaceWindowXML *> windows;
public:
	cWindowManager()
	{
		m_Parent = 0;
	}

	~cWindowManager()
	{
		if(m_Parent)
			delete m_Parent;
		m_Parent = 0;
	}

	void add_window(string name, cInterfaceWindowXML *win) {
		windows[name] = win;
	}

/*
 *	add the screen to the stac
 *
 *	Studio seems to handle some Push methods in special ways
 *	(Alternatively, it's broken, depending on your POV)
 */
	void push(string window_name);
	void Push(process_func Process, cInterfaceWindow* Interface);
	void push(process_funcxml Process,cInterfaceWindow * Interface);

	// remove function from the stack
	void Pop()
	{
		if(m_Parent != 0)
		{
			sWindow *InterfacePtr = m_Parent;
			m_Parent = m_Parent->m_Next;
			InterfacePtr->m_Next = 0;
			delete InterfacePtr;
			InterfacePtr = 0;
		}
	}

	void PopToWindow(cInterfaceWindow* Interface)
	{
		if(m_Parent != 0)
		{
			while(m_Parent->m_Interface != Interface)
				Pop();
		}
	}

	void UpdateCurrent()
	{
		if(!m_Parent) {
			return;
		}
		if(m_Parent->xmlfunc)
			m_Parent->XmlFunction(m_Parent->m_Interface);
		else
			m_Parent->Function();
	}

	void UpdateMouseMovement(int x, int y)
	{
		if(m_Parent)
			m_Parent->m_Interface->UpdateWindow(x, y);
	}

	void UpdateMouseDown(int x, int y)
	{
		if(m_Parent)
			m_Parent->m_Interface->MouseDown(x, y);
	}

	void UpdateMouseClick(int x, int y, bool mouseWheelDown = false, bool mouseWheelUp = false)
	{
		if(m_Parent)
			m_Parent->m_Interface->Click(x, y, mouseWheelDown, mouseWheelUp);
	}

	void UpdateKeyInput(char key, bool upper = false)
	{
		if(m_Parent)
			m_Parent->m_Interface->UpdateEditBoxes(key, upper);
	}

	bool HasEditBox() {if(!m_Parent)return false; return m_Parent->m_Interface->HasEditBox();}

	cInterfaceWindow* GetWindow() {if(!m_Parent) return 0; return m_Parent->m_Interface;}

	void Draw() {if(m_Parent) m_Parent->m_Interface->Draw();}

private:
	sWindow* m_Parent;
};

#endif
