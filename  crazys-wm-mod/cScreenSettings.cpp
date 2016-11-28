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
#include "cScreenSettings.h"
#include "cGetStringScreenManager.h"
#include "cScriptManager.h"
#include "cWindowManager.h"
#include "FileList.h"

extern cWindowManager g_WinManager;
extern string g_ReturnText;
extern bool g_InitWin;
extern int g_CurrentScreen;

extern bool g_EscapeKey;

static bool revert = true;
bool cScreenSettings::ids_set = false;

void cScreenSettings::set_ids()
{
	ids_set = true;
	back_id				= get_id("BackButton");
	ok_id				= get_id("OkButton");
	revert_id			= get_id("RevertButton");
	characters_id		= get_id("CharactersFol");
	saves_id			= get_id("SavesFol");
	defaultimages_id	= get_id("DefaultImagesFol");
	items_id			= get_id("ItemsFol");
	backupsaves_id		= get_id("BackupSavesFol");
	preferdefault_id	= get_id("PreferDefaultFol");
}

cScreenSettings::cScreenSettings()
{
	cConfig cfg;
	DirPath dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "settings.xml";
	m_filename = dp.c_str();
	revert = true;
}

void cScreenSettings::init()
{
	if (g_CurrentScreen != SCREEN_SETTINGS) revert = true;
	g_CurrentScreen = SCREEN_SETTINGS;
	if (g_InitWin)
	{
		Focused();
		g_InitWin = false;
		if (revert)
		{
			m_EditBoxes[characters_id]->m_Text->SetText(cfg.folders.characters());
			m_EditBoxes[saves_id]->m_Text->SetText(cfg.folders.saves());
			m_EditBoxes[defaultimages_id]->m_Text->SetText(cfg.folders.defaultimageloc());
			m_EditBoxes[items_id]->m_Text->SetText(cfg.folders.items());
			m_CheckBoxes[backupsaves_id]->SetState(cfg.folders.backupsaves());
			m_CheckBoxes[preferdefault_id]->SetState(cfg.folders.preferdefault());

			revert = false;
		}
//	backupsaves_id
//	preferdefault_id
	}

}

void cScreenSettings::process()
{
	if (!ids_set) set_ids();	// we need to make sure the ID variables are set
	if (check_keys()) return;	// handle arrow keys
	init();						// set up the window if needed
	check_events();				// check to see if there's a button event needing handling
}

void cScreenSettings::check_events()
{
	if (g_InterfaceEvents.GetNumEvents() == 0) return;	// no events means we can go home
	if (g_InterfaceEvents.CheckButton(back_id))
	{
		g_WinManager.Pop();
		g_InitWin = true;
		return;
	}
	if (g_InterfaceEvents.CheckButton(revert_id))
	{
		revert = true;
		g_InitWin = true;
		return;
	}
	if (g_InterfaceEvents.CheckButton(ok_id))
	{
		cfg.folders.characters() = m_EditBoxes[characters_id]->m_Text->GetText();
		cfg.folders.saves() = m_EditBoxes[saves_id]->m_Text->GetText();
		cfg.folders.defaultimageloc() = m_EditBoxes[defaultimages_id]->m_Text->GetText();
		cfg.folders.items() = m_EditBoxes[items_id]->m_Text->GetText();
		cfg.folders.backupsaves() = m_CheckBoxes[backupsaves_id]->GetState();
		cfg.folders.preferdefault() = m_CheckBoxes[preferdefault_id]->GetState();


		revert = true;
		g_InitWin = true;
		return;
	}

}

bool cScreenSettings::check_keys()
{
	return false;
}