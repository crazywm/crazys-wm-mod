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
#include "cWindowManager.h"
#include "CLog.h"

extern CLog g_LogFile;

sWindow::sWindow()
{
	m_Next = 0;
	m_Interface = 0;
}
sWindow::~sWindow()
{
	if (m_Next) delete m_Next;
	m_Next = 0;
	m_Interface = 0;
}

cWindowManager::cWindowManager()
{
	m_Parent = 0;
}
cWindowManager::~cWindowManager()
{
	if (m_Parent) delete m_Parent;
	m_Parent = 0;
}

void cWindowManager::push(string window_name)
{
	if (windows.find(window_name) == windows.end())						//check the screen exists
	{
		g_LogFile.ss() << "cWindowManager::Push: can't find window named '" << window_name << "'" ; g_LogFile.ssend();
		return;
	}
	cInterfaceWindowXML *wpt = windows[window_name];					//look up the manager object
	push(process_funcxml(wpt->handler_func), (cInterfaceWindow*)wpt);	//push the static handler and the window pointer onto the stack
}

void cInterfaceWindowXML::handler_func(cInterfaceWindowXML *wpt)
{
	wpt->process();
}

void cWindowManager::Push(process_func Process, cInterfaceWindow* Interface)
{
	if (Process == 0) return;					// Don't push a NULL value
	sWindow *InterfacePtr = new sWindow();		// Allocate a new process and push it on stack
	InterfacePtr->m_Next = m_Parent;
	m_Parent = InterfacePtr;
	//if(xmlf)
	//	InterfacePtr->XmlFunction=void (_cdelc *)(cInterfaceWindow *)Process;
	//else
	InterfacePtr->Function = Process;
	InterfacePtr->m_Interface = Interface;
	InterfacePtr->xmlfunc = false;
}

void cWindowManager::push(process_funcxml Process, cInterfaceWindow* Interface)
{
	if (Process == 0) return;					// Don't push a NULL value
	sWindow *InterfacePtr = new sWindow();		// Allocate a new process and push it on stack
	InterfacePtr->m_Next = m_Parent;
	m_Parent = InterfacePtr;
	//if(xmlf)
	//	InterfacePtr->XmlFunction=void (_cdelc *)(cInterfaceWindow *)Process;
	//else
	InterfacePtr->XmlFunction = Process;
	InterfacePtr->m_Interface = Interface;
	InterfacePtr->xmlfunc = true;
}

// remove function from the stack
void cWindowManager::Pop()
{
	if (m_Parent != 0)
	{
		sWindow *InterfacePtr = m_Parent;
		m_Parent = m_Parent->m_Next;
		InterfacePtr->m_Next = 0;
		delete InterfacePtr;
		InterfacePtr = 0;
	}
}

void cWindowManager::PopToWindow(cInterfaceWindow* Interface)
{
	if (m_Parent != 0)
	{
		while (m_Parent->m_Interface != Interface) Pop();
	}
}

void cWindowManager::UpdateCurrent()
{
	if (!m_Parent) return;
	if (m_Parent->xmlfunc) m_Parent->XmlFunction(m_Parent->m_Interface);
	else m_Parent->Function();
}

void cWindowManager::UpdateMouseMovement(int x, int y)
{
	if (m_Parent) m_Parent->m_Interface->UpdateWindow(x, y);
}

void cWindowManager::UpdateMouseDown(int x, int y)
{
	if (m_Parent) m_Parent->m_Interface->MouseDown(x, y);
}

void cWindowManager::UpdateMouseClick(int x, int y, bool mouseWheelDown, bool mouseWheelUp)
{
	if (m_Parent) m_Parent->m_Interface->Click(x, y, mouseWheelDown, mouseWheelUp);
}

void cWindowManager::UpdateKeyInput(char key, bool upper)
{
	if (m_Parent) m_Parent->m_Interface->UpdateEditBoxes(key, upper);
}

bool cWindowManager::HasEditBox()
{
	if (!m_Parent) return false;
	return m_Parent->m_Interface->HasEditBox();
}

cInterfaceWindow* cWindowManager::GetWindow()
{
	if (!m_Parent) return 0;
	return m_Parent->m_Interface;
}

void cWindowManager::Draw()
{
	if (m_Parent) m_Parent->m_Interface->Draw();
}

/*

make: *** [cWindowManager.o] Error 1

*/
