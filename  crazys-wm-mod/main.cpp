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

#if 0	// `J` change this to 1 to enable Visual Leak Detector
#include <vld.h>
#endif	// if you don't have Visual Leak Detector, get it here - https://vld.codeplex.com/

#include "main.h"
#include "src/screens/cScreenMainMenu.h"
#include "src/screens/cScreenNewGame.h"
#include "src/screens/cScreenBrothelManagement.h"
#include "src/screens/cScreenPreparingGame.h"
#include "InterfaceGlobals.h"
#include "GameFlags.h"
#include "InterfaceProcesses.h"
#include "sConfig.h"
#include "CSurface.h"
#include "cJobManager.h"
#include "Revision.h"
#include "libintl.h"
#include <clocale>
#include "FileList.h"
#include "src/Game.hpp"

#ifndef LINUX
#ifdef _DEBUG
// to enable leak detection uncomment the below and the first comment in main()
/* #ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#endif
#include <stdlib.h>
#include <crtdbg.h>*/
#endif
#else
#endif
#include <sstream>

#include "src/widgets/cSlider.h"
#include "src/widgets/cScrollBar.h"
#include "cObjectiveManager.hpp"
#include "cInventory.h"


extern cScreenMainMenu* g_MainMenu;
extern cScreenNewGame* g_NewGame;
extern cScreenPreparingGame* g_Preparing;
extern cScreenBrothelManagement* g_BrothelManagement;

// Function Defs
void Shutdown();
bool Init();

bool eventrunning = false;

bool g_ShiftDown = false;	bool g_CTRLDown = false;

bool g_UpArrow = false;		bool g_DownArrow = false;
bool g_EnterKey = false;

bool g_S_Key = false;
bool g_W_Key = false;

string monthnames[13]
{
	"No Month", "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"
};

bool g_AltKeys = true;          // Toggles the alternate hotkeys --PP
bool playershopinventory = false;
extern bool g_AllTogle;

cScrollBar* g_DragScrollBar = nullptr;  // if a scrollbar is being dragged, this points to it
cSlider* g_DragSlider = nullptr;  // if a slider is being dragged, this points to it

// SDL Graphics interface
CGraphics g_Graphics;

// Resource Manager
CResourceManager rmanager;

// logfile
CLog g_LogFile(true);

// Trait list
cTraits g_Traits;

// Holds the currently running script

cConfig cfg;
cWindowManager g_WinManager;

cRng g_Dice;

// Game manager
Game g_Game;

cNameList	g_GirlNameList;
cNameList	g_BoysNameList;
cSurnameList g_SurnameList;

void handle_hotkeys(const SDL_Event& vent)
{
	switch (vent.key.keysym.sym)
	{
	case SDLK_RSHIFT:
	case SDLK_LSHIFT:
		g_ShiftDown = true;		// enable multi select
		break;
	case SDLK_RCTRL:
	case SDLK_LCTRL:
		g_CTRLDown = true;		// enable multi select
		break;

	case SDLK_RETURN:
	case SDLK_KP_ENTER:	g_EnterKey = true;	break;

	case SDLK_UP:		g_UpArrow = true;		break;
	case SDLK_DOWN:		g_DownArrow = true;		break;

	case SDLK_s:		g_S_Key = true;			break;
	case SDLK_w:		g_W_Key = true;			break;
	default:	break;
	}




	// Process the keys for every screen except MainMenu, LoadGame and NewGame - they have their own keys
	if (g_WinManager.GetWindow() != g_MainMenu && g_WinManager.GetWindow() != g_LoadGame && g_WinManager.GetWindow() != g_Preparing && g_WinManager.GetWindow() != g_NewGame)
	{
		int br_no = 0;
		string msg;

		switch (vent.key.keysym.sym)
		{
		case SDLK_RSHIFT:
		case SDLK_LSHIFT:
			g_ShiftDown = true;		// enable multi select
			break;
		case SDLK_RCTRL:
		case SDLK_LCTRL:
			g_CTRLDown = true;		// enable multi select
			break;

		case SDLK_UP:		g_UpArrow = true;		break;
		case SDLK_DOWN:		g_DownArrow = true;		break;
		case SDLK_s:		g_S_Key = true;		break;
		case SDLK_w:		g_W_Key = true;		break;
		default:	break;
		}
	}
}

int main(int ac, char* av[])	// `J` Bookmark - #1 - Entering the game
{
#ifndef LINUX
#ifdef _DEBUG
	//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	//_CrtSetBreakAlloc(16477);
#endif
#endif

#if 0
	g_LogFile.ss() << "\n`J` DEBUG CODE - this section is used to debug a piece of code.\n"; g_LogFile.ssend();











	g_LogFile.ss() << "\n`J` DEBUG CODE - this section is used to debug a piece of code.\n"; g_LogFile.ssend();
#endif



	// get text
	setlocale(LC_ALL, "");
	DirPath base = DirPath() << "Resources" << "lang";
	bindtextdomain("whoremaster", base);
	textdomain("whoremaster");


	bool running = true;
	bool quitPending = false;
	bool mouseDown = false;

	g_LogFile.write("\n------------------------------------------------------------------------------------------------------------------------\nCalling Init");
	// INit the program
	if (!Init())
		return 1;

	g_WinManager.push("Main Menu");

	while (running)
	{
	    SDL_Event vent;
		while (SDL_PollEvent(&vent))
		{
			if (vent.type == SDL_QUIT)
			{
				running = false;
			}
			else if (vent.type == SDL_MOUSEBUTTONUP)
			{
				if (mouseDown)
				{
					if (g_DragScrollBar != nullptr)
					{
						g_DragScrollBar->SetTopValue(g_DragScrollBar->m_ItemTop);
						g_DragScrollBar = nullptr;
					}
					else if (g_DragSlider != nullptr)
					{
						g_DragSlider->EndDrag();
						g_DragSlider = nullptr;
					}
					else if (g_ChoiceManager.IsActive())
						g_ChoiceManager.ButtonClicked(vent.motion.x, vent.motion.y);
					else
						g_WinManager.UpdateMouseClick(vent.motion.x, vent.motion.y);
					mouseDown = false;
				}
			}
			else if (vent.type == SDL_MOUSEBUTTONDOWN)
			{
				if (vent.button.button == SDL_BUTTON_WHEELDOWN)
				{
					g_WinManager.UpdateMouseClick(vent.motion.x, vent.motion.y, true, false);
				}
				else if (vent.button.button == SDL_BUTTON_WHEELUP)
				{
					g_WinManager.UpdateMouseClick(vent.motion.x, vent.motion.y, false, true);
				}
				else if (vent.button.button == SDL_BUTTON_LEFT)
				{
					//srand(SDL_GetTicks());
					if (!mouseDown)
						mouseDown = true;
					g_WinManager.UpdateMouseDown(vent.motion.x, vent.motion.y);
				}
				/*
				*                               horizontal mouse scroll events happen here,
				*                               as do right and middle clicks.
				*/
				else {
					// do nothing ...
				}
			}
			else if (vent.type == SDL_KEYUP)
			{
				if (!g_ChoiceManager.IsActive())
				{
					switch (vent.key.keysym.sym)
					{
					case SDLK_RSHIFT:
					case SDLK_LSHIFT:	g_ShiftDown = false;	break;	// enable multi select
					case SDLK_RCTRL:
					case SDLK_LCTRL:	g_CTRLDown = false;		break;	// enable multi select
					case SDLK_RETURN:
					case SDLK_KP_ENTER:	g_EnterKey = false;		break;

					case SDLK_UP:		g_UpArrow = false;		break;
					case SDLK_DOWN:		g_DownArrow = false;	break;

					case SDLK_s:		g_S_Key = false;		break;
					case SDLK_w:		g_W_Key = false;		break;
					}
				}
			}
			else if (vent.type == SDL_KEYDOWN)
			{
			    g_WinManager.OnKeyPress(vent.key.keysym);
				if (!g_ChoiceManager.IsActive())
				{
					if (g_WinManager.HasEditBox())
					{
						if (vent.key.keysym.sym == SDLK_BACKSPACE)		g_WinManager.UpdateKeyInput('-');
						else if (vent.key.keysym.sym == SDLK_RETURN)	g_EnterKey = true;
						else if (vent.key.keysym.sym == SDLK_KP_ENTER)	g_EnterKey = true;
						else if (vent.key.keysym.sym == SDLK_PERIOD || vent.key.keysym.sym == SDLK_SLASH || vent.key.keysym.sym == SDLK_BACKSLASH)
						{
							g_WinManager.UpdateKeyInput((char)vent.key.keysym.sym);
						}
						else if ((vent.key.keysym.sym >= 97 && vent.key.keysym.sym <= 122) || vent.key.keysym.sym == 39 || vent.key.keysym.sym == 32 || (vent.key.keysym.sym >= 48 && vent.key.keysym.sym <= 57) || ((vent.key.keysym.sym >= 256 && vent.key.keysym.sym <= 265)))
						{
							if (vent.key.keysym.sym >= 256)
							{
								if (vent.key.keysym.sym == 256)			vent.key.keysym.sym = SDLK_0;
								else if (vent.key.keysym.sym == 257)	vent.key.keysym.sym = SDLK_1;
								else if (vent.key.keysym.sym == 258)	vent.key.keysym.sym = SDLK_2;
								else if (vent.key.keysym.sym == 259)	vent.key.keysym.sym = SDLK_3;
								else if (vent.key.keysym.sym == 260)	vent.key.keysym.sym = SDLK_4;
								else if (vent.key.keysym.sym == 261)	vent.key.keysym.sym = SDLK_5;
								else if (vent.key.keysym.sym == 262)	vent.key.keysym.sym = SDLK_6;
								else if (vent.key.keysym.sym == 263)	vent.key.keysym.sym = SDLK_7;
								else if (vent.key.keysym.sym == 264)	vent.key.keysym.sym = SDLK_8;
								else if (vent.key.keysym.sym == 265)	vent.key.keysym.sym = SDLK_9;
							}

							if (vent.key.keysym.mod & KMOD_LSHIFT || vent.key.keysym.mod & KMOD_RSHIFT || vent.key.keysym.mod & KMOD_CAPS)
								g_WinManager.UpdateKeyInput((char)vent.key.keysym.sym, true);
							else
								g_WinManager.UpdateKeyInput((char)vent.key.keysym.sym);
						}
					}
					else    // hotkeys
					{
						handle_hotkeys(vent);
					}
				}
				else if (vent.key.keysym.sym == SDLK_RETURN || vent.key.keysym.sym == SDLK_KP_ENTER)
				{
					if (g_ChoiceManager.IsActive())
					{
						g_EnterKey = true;
						g_ChoiceManager.ButtonClicked(0, 0);
					}
					g_EnterKey = false;
				}
				else if (g_ChoiceManager.IsActive())
				{
					if (vent.key.keysym.sym == SDLK_UP || vent.key.keysym.sym == SDLK_DOWN)
					{
						/* */if (vent.key.keysym.sym == SDLK_UP)		g_UpArrow = true;
						else if (vent.key.keysym.sym == SDLK_DOWN)		g_DownArrow = true;
						g_ChoiceManager.ButtonClicked(0, 0);
					}
				}
			}
			else if (vent.type == SDL_MOUSEMOTION)
			{
				if (!g_ChoiceManager.IsActive())
				{
					// if dragging a scrollbar, send movements to it exclusively until mouseup
					if (g_DragScrollBar != nullptr)
						g_DragScrollBar->DragMove(vent.motion.y);
					// if dragging a slider, send movements to it exclusively until mouseup
					else if (g_DragSlider != nullptr)
						g_DragSlider->DragMove(vent.motion.x);
					// update interface
					else
						g_WinManager.UpdateMouseMovement(vent.motion.x, vent.motion.y);
				}
				else
					g_ChoiceManager.IsOver(vent.motion.x, vent.motion.y);
			}
		}

		//              if(!sleeping)
		//              {
		// Clear the screen
		g_Graphics.Begin();

		// Draw the interface
		g_WinManager.Draw();

		if (eventrunning && !g_WinManager.HasActiveModal() && !g_ChoiceManager.IsActive())    // run any events that are being run
			GameEvents();

		// Run the interface
		if (!g_ChoiceManager.IsActive())
			g_WinManager.UpdateCurrent();
		else
			g_ChoiceManager.Draw();

		rmanager.CullOld(g_Graphics.GetTicks());

		g_Graphics.End();
	}

	Shutdown();
	return 0;
}

void Shutdown()
{
	g_LogFile.write("\n\n\t*** Shutting Down ***");
	g_LogFile.write("Releasing Graphics");
	g_Graphics.Free();

	g_LogFile.write("Releasing Girls");
    g_Game.girl_pool().Free();

	g_Traits.Free();
	g_Game.inventory_manager().Free();

	g_LogFile.write("Releasing Interface");
	FreeInterface();

	g_LogFile.write("Releasing Resource Manager");
	rmanager.Free();

	g_LogFile.write("Releasing Jobs");
#ifdef _DEBUG
	cJobManager::freeJobs();
#else
	cJobManager::free();
#endif
	g_LogFile.write("Shutdown Complete");
}

bool Init()		// `J` Bookmark	- Initializing the game
{
	g_LogFile.write("Initializing Graphics");
	/*
	*       build the caption string
	*/
	stringstream ss;
	ss << "Whore Master v" << g_MajorVersion << "." << g_MinorVersionA << g_MinorVersionB << "." << g_StableVersion << " BETA" << " Svn: " << svn_revision;
	/*
	*       init the graphics, with the caption on the titlebar
	*/
	if (!g_Graphics.InitGraphics(ss.str(), 0, 0, 32))
	{
		g_LogFile.write("ERROR - Initializing Graphics");
		return false;
	}

	g_LogFile.write("Graphics Initialized");

	LoadInterface();        // Load the interface
	g_LogFile.write("Interface Loaded");

	InitGameFlags();        // Init the game flags
	g_LogFile.write("Game Flags Initialized");

	return true;
}

