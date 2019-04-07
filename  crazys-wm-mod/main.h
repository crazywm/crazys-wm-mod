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
#ifndef __MAIN_H
#define __MAIN_H

#include "GameSystem.h"
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

#include <stdio.h>
#include <time.h>
#include "main.h"
#include "Constants.h"
#include "cTraits.h"
#include "cGirls.h"
#include "cBrothel.h"
#include "cGangs.h"
#include "cEvents.h"
#include "cCustomers.h"
#include "math.h"
#include "cTriggers.h"
#include "cGameScript.h"
#include "cWindowManager.h"
#include "cRival.h"
#include "cGold.h"
#include "cScrollBar.h"
#include "cArena.h"
#include "cClinic.h"
#include "cMovieStudio.h"
#include "cCentre.h"
#include "cHouse.h"
#include "cFarm.h"
#include "SDL_anigif.h"
#include "cImageItem.h"

class cScrollBar;

// interface manager
extern cWindowManager g_WinManager;

// SDL Graphics interface
extern CGraphics g_Graphics;

// Resource Manager
extern CResourceManager rmanager;

// Events
extern SDL_Event vent;

// logfile
extern CLog g_LogFile;

// Trait list
extern cTraits g_Traits;

// Girl manager
extern cGirls g_Girls;

// Brothel Manager
extern cBrothelManager g_Brothels;

// Clinic Manager
extern cClinicManager g_Clinic;

// Movie Studio Manager
extern cMovieStudioManager g_Studios;

// Arena Manager
extern cArenaManager g_Arena;

// Centre Manager
extern cCentreManager g_Centre;

// House Manager
extern cHouseManager g_House;

// Farm Manager
extern cFarmManager g_Farm;

// Gang Manager
extern cGangManager g_Gangs;

// Customer Manager
extern cCustomers g_Customers;

// The global trigger manager
extern cTriggerList g_GlobalTriggers;

extern cNameList	g_GirlNameList;
extern cNameList	g_BoysNameList;
extern cSurnameList g_SurnameList;


// Keeping time in the game
extern unsigned long g_Year;
extern unsigned long g_Month;
extern unsigned long g_Day;

// the players gold
extern cGold g_Gold;

// Inventory manager
extern cInventory g_InvManager;

extern int g_CurrBrothel;
extern int g_Building;
extern int g_CurrStudio;
extern int g_CurrArena;
extern int g_CurrCentre;
extern int g_CurrHouse;
extern int g_CurrFarm;

// if a scrollbar is currently being dragged, this points to it
extern cScrollBar* g_DragScrollBar;

#endif
