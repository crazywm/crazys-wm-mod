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
#ifndef __INTERFACEGLOBALS_H
#define __INTERFACEGLOBALS_H

#include "cInterfaceWindow.h"
#include "cMessageBox.h"
#include "cChoiceMessage.h"

#ifdef LINUX
#include "linux.h"
#endif

// Globals
extern cInterfaceWindow* g_LoadGame;
extern cChoiceManager g_ChoiceManager;

void LoadInterface();	// Function for loading the interface
void FreeInterface();
void ResetInterface();

#endif
