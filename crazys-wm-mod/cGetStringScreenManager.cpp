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
#include "cGetStringScreenManager.h"
#include "InterfaceGlobals.h"
#include "main.h"
#include "libintl.h"

cGetStringScreenManager_Inner *cGetStringScreenManager::instance = 0;

extern sInterfaceIDs g_interfaceid;
extern bool g_InitWin;
extern string g_ReturnText;

void cGetStringScreenManager_Inner::process()
{
	if (g_InitWin)
	{
		g_GetString.Focused();
		g_InitWin = false;
	}
	if (g_EnterKey || g_InterfaceEvents.CheckButton(g_interfaceid.BUTTON_OK))	submit();
	if (g_InterfaceEvents.CheckButton(g_interfaceid.BUTTON_CANCEL))
	{
		g_ReturnText = "";
		g_InitWin = true;
		g_WinManager.Pop();
	}
}

void cGetStringScreenManager_Inner::submit()
{
	g_EnterKey = false;

	g_ReturnText = g_GetString.GetEditBoxText(g_interfaceid.EDITBOX_NAME);

	if (!m_empty_ok && g_ReturnText == "")
	{
		g_MessageQue.AddToQue("You must enter a name", COLOR_RED);
		return;
	}
	m_empty_ok = true;
	g_InitWin = true;
	g_WinManager.Pop();

	if (trans)
	{
		trans->assign(g_ReturnText);
		delete trans;
		trans = 0;
	}
}
