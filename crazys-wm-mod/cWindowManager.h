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
	void(*Function)();	// pointer to the update function
	void(*XmlFunction)(cInterfaceWindow*);
	bool xmlfunc;
	sWindow* m_Next;

	sWindow();
	~sWindow();
}sWindow;

typedef void(*process_func)();
typedef void(*process_funcxml)(cInterfaceWindow *);

class cWindowManager
{
	map<string, cInterfaceWindowXML *> windows;
public:
	cWindowManager();
	~cWindowManager();

	void add_window(string name, cInterfaceWindowXML *win) { windows[name] = win; }
	void push(string window_name);
	void Push(process_func Process, cInterfaceWindow* Interface);
	void push(process_funcxml Process, cInterfaceWindow * Interface);

	// remove function from the stack
	void Pop();
	void PopToWindow(cInterfaceWindow* Interface);
	void UpdateCurrent();
	void UpdateMouseMovement(int x, int y);
	void UpdateMouseDown(int x, int y);
	void UpdateMouseClick(int x, int y, bool mouseWheelDown = false, bool mouseWheelUp = false);
	void UpdateKeyInput(char key, bool upper = false);
	bool HasEditBox();
	cInterfaceWindow* GetWindow();
	void Draw();

private:
	sWindow* m_Parent;
};

#endif
