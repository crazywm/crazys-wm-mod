/*
* Copyright 2009, 2010, The Pink Petal Development Team.
* The Pink Petal Devloment Team are defined as the game's coders
* who meet on http://pinkpetal.org
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

#include "InterfaceProcesses.h"
#include "sConfig.h"
#include "interface/CGraphics.h"
#include "Revision.h"
#include "Game.hpp"
#include "CLog.h"
#include "cRng.h"
#include "cNameList.h"
#include "interface/cWindowManager.h"

#include <sstream>
#include <SDL_events.h>

// Function Defs
void Shutdown();
bool Init(CGraphics& gfx);

std::string monthnames[13]
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

int main(int ac, char* av[])    // `J` Bookmark - #1 - Entering the game
{
    bool running = true;

    // Init the program
    // SDL Graphics interface
    CGraphics graphics;
    g_LogFile.disable_verbose();

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
                    window_manager().OnMouseClick(vent.button.x, vent.button.y, false);
            }
            else if (vent.type == SDL_MOUSEWHEEL) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                window_manager().OnMouseWheel(x, y, vent.wheel.y < 0);
            }
            else if (vent.type == SDL_MOUSEBUTTONDOWN)
            {
                if (vent.button.button == SDL_BUTTON_LEFT)
                {
                    //srand(SDL_GetTicks());
                    window_manager().OnMouseClick(vent.button.x, vent.button.y, true);
                }
                else {
                    // do nothing ...
                }
            }
            else if (vent.type == SDL_KEYUP)
            {
                window_manager().OnKeyEvent(vent.key.keysym, false);
            }
            else if (vent.type == SDL_KEYDOWN)
            {
                window_manager().OnKeyEvent(vent.key.keysym, true);
            }
            else if (vent.type == SDL_MOUSEMOTION)
            {
                // if dragging a scrollbar, send movements to it exclusively until mouseup
                window_manager().UpdateMouseMovement(vent.motion.x, vent.motion.y);
            }
            else if(vent.type == SDL_TEXTINPUT) {
                window_manager().OnTextInput(vent.text.text);
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

bool Init(CGraphics& gfx)        // `J` Bookmark    - Initializing the game
{
    g_LogFile.log(ELogLevel::INFO, "*** Initializing ***");
    /*
    *       build the caption string
    */
    std::stringstream ss;
    ss << "Whore Master v" << g_MajorVersion << "." << g_MinorVersionA << g_MinorVersionB << "." << g_StableVersion << " BETA" << " Svn: " << svn_revision;
    /*
    *       init the graphics, with the caption on the titlebar
    */

    if (!gfx.InitGraphics(ss.str(),
                          cfg.resolution.width(), cfg.resolution.height(),
                          cfg.resolution.width(), cfg.resolution.height(),
                          cfg.resolution.fullscreen()))
    {
        g_LogFile.log(ELogLevel::ERROR,"Initializing Graphics");
        return false;
    }

    g_LogFile.log(ELogLevel::NOTIFY,"Graphics Initialized");

    g_LogFile.log(ELogLevel::NOTIFY, "Loading Interface");
    InitInterface(&gfx, cfg.resolution.resolution());
    LoadInterface();        // Load the interface
    gfx.GetImageCache().PrintStats();

    g_LogFile.log(ELogLevel::NOTIFY, "Interface Loaded");

    return true;
}

