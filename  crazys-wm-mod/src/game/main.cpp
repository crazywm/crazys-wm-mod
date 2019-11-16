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
#include "InterfaceProcesses.h"
#include "sConfig.h"
#include "interface/CGraphics.h"
#include "cJobManager.h"
#include "Revision.h"
#include "Game.hpp"

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
#include <SDL_events.h>

// Function Defs
void Shutdown();
bool Init(CGraphics& gfx);

bool g_ShiftDown = false;	bool g_CTRLDown = false;

string monthnames[13]
{
	"No Month", "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"
};

bool g_AltKeys = true;          // Toggles the alternate hotkeys --PP
bool playershopinventory = false;

// logfile
CLog g_LogFile;

cConfig cfg;

cRng g_Dice;

// Game manager
std::unique_ptr<Game> g_Game;

cNameList g_GirlNameList;
cNameList g_BoysNameList;
cNameList g_SurnameList;

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
	default:	break;
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
	// get text
	setlocale(LC_ALL, "");

	bool running = true;
	bool mouseDown = false;

	// Init the program
    // SDL Graphics interface
    CGraphics graphics;

	if (!Init(graphics))
		return 1;

	window_manager().push("Main Menu");
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
                if (vent.button.button == SDL_BUTTON_LEFT)
                    window_manager().OnMouseClick(vent.motion.x, vent.motion.y, false);
			}
			else if (vent.type == SDL_MOUSEBUTTONDOWN)
			{
				if (vent.button.button == SDL_BUTTON_WHEELDOWN)
				{
                    window_manager().OnMouseWheel(vent.motion.x, vent.motion.y, true);
				}
				else if (vent.button.button == SDL_BUTTON_WHEELUP)
				{
                    window_manager().OnMouseWheel(vent.motion.x, vent.motion.y, false);
				}
				else if (vent.button.button == SDL_BUTTON_LEFT)
				{
					//srand(SDL_GetTicks());
					if (!mouseDown)
						mouseDown = true;
                    window_manager().OnMouseClick(vent.motion.x, vent.motion.y, true);
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
                switch (vent.key.keysym.sym)
                {
                case SDLK_RSHIFT:
                case SDLK_LSHIFT:	g_ShiftDown = false;	break;	// enable multi select
                case SDLK_RCTRL:
                case SDLK_LCTRL: {
                    g_CTRLDown = false;
                }
                break;	// enable multi select
                }
			}
			else if (vent.type == SDL_KEYDOWN)
			{
			    window_manager().OnKeyPress(vent.key.keysym);
			    handle_hotkeys(vent);
			}
			else if (vent.type == SDL_MOUSEMOTION)
			{
                // if dragging a scrollbar, send movements to it exclusively until mouseup
                window_manager().UpdateMouseMovement(vent.motion.x, vent.motion.y);
			}
		}

		// Clear the screen
		graphics.Begin();

		// Draw the interface
		window_manager().Draw();

		// Run the interface
		window_manager().UpdateCurrent();

		graphics.End();
	}

	Shutdown();
	return 0;
}

void Shutdown()
{
    g_LogFile.log(ELogLevel::INFO, "*** Shutting Down ***");

    ShutdownInterface();

    g_LogFile.log(ELogLevel::INFO,"Shutdown Complete");
}

bool Init(CGraphics& gfx)		// `J` Bookmark	- Initializing the game
{
    g_LogFile.log(ELogLevel::INFO, "*** Initializing ***");
	/*
	*       build the caption string
	*/
	stringstream ss;
	ss << "Whore Master v" << g_MajorVersion << "." << g_MinorVersionA << g_MinorVersionB << "." << g_StableVersion << " BETA" << " Svn: " << svn_revision;
	/*
	*       init the graphics, with the caption on the titlebar
	*/

	if (!gfx.InitGraphics(ss.str(), cfg.resolution.width(), cfg.resolution.height(), 32))
	{
        g_LogFile.log(ELogLevel::ERROR,"Initializing Graphics");
		return false;
	}

	// enable automatic unicode translation
    SDL_EnableUNICODE(1);

    g_LogFile.log(ELogLevel::INFO,"Graphics Initialized");

    g_LogFile.log(ELogLevel::INFO, "Loading Interface");
	InitInterface(&gfx);
	LoadInterface();        // Load the interface
	gfx.GetImageCache().PrintStats();

    g_LogFile.log(ELogLevel::INFO,"Interface Loaded");

	return true;
}

