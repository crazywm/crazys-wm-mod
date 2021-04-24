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
#include "IGame.h"

#include <sstream>
#include <SDL_events.h>
#include "interface/cInterfaceWindow.h"
#include "CLog.h"
#include "cRng.h"
#include "cNameList.h"
#include "interface/cWindowManager.h"
#include <iostream>

#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
// on mingw, doctest includes windows.h which pollutes with this macro
#ifdef ERROR
#undef ERROR
#endif



// Function Defs
void Shutdown();
bool Init(CGraphics& gfx);

std::string monthnames[13]
        {
                "No Month", "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"
        };

bool g_AltKeys = true;          // Toggles the alternate hotkeys --PP
bool playershopinventory = false;

cConfig cfg;

class FuzzyMouse {
public:
    FuzzyMouse(bool replay){
        m_Recording = std::fstream("/tmp/track.txt", replay ? std::fstream::in : std::fstream::out);
        m_Replay = replay;
    }
    void make_event() {
        if(m_Replay) {
            std::string action;
            int x, y, type;
            m_Recording >> action >> type >> x >> y;
            SDL_Event mouse_event;

            mouse_event.type = type;
            mouse_event.button.button = SDL_BUTTON_LEFT;
            mouse_event.button.x = x;
            mouse_event.button.y = y;
            SDL_PushEvent(&mouse_event);
            return;
        }

        if(rand() % 1 != 0)  {
            m_Recording << "-" << std::endl;
            return;
        }
        SDL_Event mouse_event;

        mouse_event.type = rand() % 2 ? SDL_MOUSEBUTTONUP : SDL_MOUSEBUTTONDOWN;
        mouse_event.button.button = SDL_BUTTON_LEFT;
        mouse_event.button.x = rand() % cfg.resolution.width();
        mouse_event.button.y = rand() % cfg.resolution.height();
        for(int i = 0; i < 5; ++i) {
            cInterfaceWindow* pWindow = window_manager().GetWindow();
            size_t num_widgets = pWindow->NumWidgets();
            if(num_widgets == 0)
                break;

            auto wdg = pWindow->GetWidget(rand() % num_widgets);
            if (wdg) {
                int xmin = wdg->GetXPos();
                int ymin = wdg->GetYPos();
                int xmax = wdg->GetWidth();
                int ymax = wdg->GetHeight();
                mouse_event.button.x = rand() % (xmax - xmin) + xmin;
                mouse_event.button.y = rand() % (ymax - ymin) + ymin;
                break;
            }
        }

        SDL_PushEvent(&mouse_event);
        ++m_ClickCount;

        m_Recording << "mouse " << mouse_event.type << " " << mouse_event.button.x << " " << mouse_event.button.y << std::endl;

        if(m_ClickCount % 10000 == 0) {
            std::cout << "TESTED " << m_ClickCount << "\n";
            std::cout << window_manager().GetWindow()->name() << "\n";
        }
    }
private:
    int m_ClickCount = 0;
    std::fstream m_Recording;
    bool m_Replay;

};

// logfile
CLog g_LogFile;


cRng g_Dice;

// Game manager
std::unique_ptr<IGame> g_Game;

cNameList g_GirlNameList;
cNameList g_BoysNameList;
cNameList g_SurnameList;

int main(int ac, char* av[])    // `J` Bookmark - #1 - Entering the game
{

    // Init the program
    // SDL Graphics interface
    CGraphics graphics;
    g_LogFile.disable_verbose();

    if (!Init(graphics))
        return 1;

    while(true) {
        bool running = true;
        window_manager().push("Main Menu");
        FuzzyMouse mouse(false);
        while (running) {
            mouse.make_event();
            SDL_Event vent;
            while (SDL_PollEvent(&vent)) {
                if (vent.type == SDL_QUIT) {
                    running = false;
                } else if (vent.type == SDL_MOUSEBUTTONUP) {
                    if (vent.button.button == SDL_BUTTON_LEFT)
                        window_manager().OnMouseClick(vent.button.x, vent.button.y, false);
                } else if (vent.type == SDL_MOUSEWHEEL) {
                    int x, y;
                    SDL_GetMouseState(&x, &y);
                    window_manager().OnMouseWheel(x, y, vent.wheel.y < 0);
                } else if (vent.type == SDL_MOUSEBUTTONDOWN) {
                    if (vent.button.button == SDL_BUTTON_LEFT) {
                        //srand(SDL_GetTicks());
                        window_manager().OnMouseClick(vent.button.x, vent.button.y, true);
                    } else {
                        // do nothing ...
                    }
                } else if (vent.type == SDL_KEYUP) {
                    window_manager().OnKeyEvent(vent.key.keysym, false);
                } else if (vent.type == SDL_KEYDOWN) {
                    window_manager().OnKeyEvent(vent.key.keysym, true);
                } else if (vent.type == SDL_MOUSEMOTION) {
                    // if dragging a scrollbar, send movements to it exclusively until mouseup
                    window_manager().UpdateMouseMovement(vent.motion.x, vent.motion.y);
                } else if (vent.type == SDL_TEXTINPUT) {
                    window_manager().OnTextInput(vent.text.text);
                }
            }

            // Clear the screen
            // graphics.Begin();

            // Draw the interface
            // window_manager().Draw();

            // Run the interface
            window_manager().UpdateCurrent();

            // graphics.End();
        }
    }

    Shutdown();
    return 0;
}

void Shutdown()
{
    g_LogFile.log(ELogLevel::NOTIFY, "*** Shutting Down ***");

    ShutdownInterface();

    g_LogFile.log(ELogLevel::NOTIFY, "Shutdown Complete");
}

bool Init(CGraphics& gfx)        // `J` Bookmark    - Initializing the game
{
    g_LogFile.log(ELogLevel::NOTIFY, "*** Initializing ***");
    /*
    *       init the graphics, with the caption on the titlebar
    */

    if (!gfx.InitGraphics("", cfg.resolution.width(), cfg.resolution.height(), cfg.resolution.width(), cfg.resolution.height(), false))
    {
        g_LogFile.log(ELogLevel::ERROR,"Initializing Graphics");
        return false;
    }

    g_LogFile.log(ELogLevel::NOTIFY, "Graphics Initialized");

    g_LogFile.log(ELogLevel::NOTIFY, "Loading Interface");
    InitInterface(&gfx, "J_1366x768");
    LoadInterface();        // Load the interface
    gfx.GetImageCache().PrintStats();

    g_LogFile.log(ELogLevel::NOTIFY, "Interface Loaded");

    return true;
}

