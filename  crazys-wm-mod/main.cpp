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
#include "main.h"
#include "cScreenMainMenu.h"
#include "cScreenBrothelManagement.h"
#include "InterfaceGlobals.h"
#include "GameFlags.h"
#include "InterfaceProcesses.h"
#include "sConfig.h"
#include "CSurface.h"
#include "cJobManager.h"
#include "Revision.h"
#include "libintl.h"
#include "locale.h"

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
#include "linux.h"
#endif
#include <signal.h>
#include <sstream>

#include "IconSurface.h"
#include "Globals.h"

Globals _G;

extern cScreenMainMenu g_MainMenu;
extern cScreenBrothelManagement g_BrothelManagement;
sInterfaceIDs g_interfaceid;

// Function Defs
void NextWeek();
void Shutdown();
bool Init();

bool quitAccepted = false;

int g_CurrBrothel = 0;
int g_Building = 0;
int g_CurrClinic = 0;
int g_CurrStudio = 0;
int g_CurrArena = 0;
int g_CurrCentre = 0;
int g_CurrHouse = 0;
int g_CurrFarm = 0;
unsigned int g_LastSeed = 0;            // for seeding the random number generater every 3 seconds (3000 ticks)

bool eventrunning = false;
bool newWeek = false;

bool g_ShiftDown = false;
bool g_CTRLDown = false;

bool g_LeftArrow = false;
bool g_RightArrow = false;
bool g_UpArrow = false;
bool g_DownArrow = false;
bool g_EnterKey = false;
// New Locally defined keys --PP
bool g_SpaceKey = false;
bool g_Q_Key = false;
bool g_W_Key = false;
bool g_E_Key = false;
bool g_A_Key = false;
bool g_S_Key = false;
bool g_D_Key = false;
bool g_Z_Key = false;
bool g_X_Key = false;
bool g_C_Key = false;

bool g_R_Key = false;
bool g_F_Key = false;
bool g_T_Key = false;
bool g_G_Key = false;
bool g_Y_Key = false;
bool g_H_Key = false;
bool g_U_Key = false;
bool g_J_Key = false;
bool g_I_Key = false;
bool g_K_Key = false;

/*                      // Alternate hotkeys (g is global keys, L is function defined locally) Ironically local keys must have global scope. --PP
g               Tab     cycle brothels
L               space   next in list.
g               0               Display hotkeys
g               Esc             Back
g               Home    AltKeys
g               End             NormalKeys

L               qwe
L               asd             Navigation
L               zxc
g               1-6 Brothels
g               F1      Girl Management
g               F2      Gang Management
g               F3      Brothel Management
g               F4      Dungeon

g               F5      Slave market
g               F6      Town
g               F7      Bank
g               F8      Mayors Office

g               F9      Turn Summary
g               F10     Shop screen
g               F11     Clinic
g               F12     Arena
*/
bool g_AltKeys = true;          // Toggles the alternate hotkeys --PP

cScrollBar* g_DragScrollBar = 0;  // if a scrollbar is being dragged, this points to it
cSlider* g_DragSlider = 0;  // if a slider is being dragged, this points to it

extern CSurface* g_BrothelImages[7];
extern bool g_InitWin;
extern sGirl* MarketSlaveGirls[12];
extern int MarketSlaveGirlsDel[12];
extern string g_ReturnText;

// SDL Graphics interface
CGraphics g_Graphics;

// Resource Manager
CResourceManager rmanager;

// Events
SDL_Event vent;

// logfile
CLog g_LogFile(true);

// Trait list
cTraits g_Traits;

// Girl manager
cGirls g_Girls;
cAbstractGirls *g_GirlsPtr = &g_Girls;

// Brothel Manager
cBrothelManager g_Brothels;

// Gang Manager
cGangManager g_Gangs;

// Customer Manager
cCustomers g_Customers;

// Clinic Manager
cClinicManager g_Clinic;

// Movie Studio Manager
cMovieStudioManager g_Studios;

// Arena Manager
cArenaManager g_Arena;

// Centre Manager
cCentreManager g_Centre;

// House Manager
cHouseManager g_House;

// Farm Manager
cFarmManager g_Farm;

// the background image
CSurface* g_BackgroundImage = 0;

// The global trigger manager
cTriggerList g_GlobalTriggers;

// Holds the currently running script

cWindowManager g_WinManager;

// Keeping time in the game
unsigned long g_Year;
unsigned long g_Month;
unsigned long g_Day;

// Keeping track of what screen is currently showing
int g_CurrentScreen = 0;

// the players gold
cGold g_Gold;

// Inventory manager
cInventory g_InvManager;

// TEmporary testing crap
int IDS = 0;
cRng g_Dice;

void handle_hotkeys()
{

	if (vent.key.keysym.sym == SDLK_RSHIFT || vent.key.keysym.sym == SDLK_LSHIFT)   // enable multi select
		g_ShiftDown = true;
	else if (vent.key.keysym.sym == SDLK_RCTRL || vent.key.keysym.sym == SDLK_LCTRL)        // enable multi select
		g_CTRLDown = true;

	if (vent.key.keysym.sym == SDLK_UP)			g_UpArrow = true;
	else if (vent.key.keysym.sym == SDLK_DOWN)  g_DownArrow = true;
	else if (vent.key.keysym.sym == SDLK_LEFT)  g_LeftArrow = true;
	else if (vent.key.keysym.sym == SDLK_RIGHT) g_RightArrow = true;
	else if (vent.key.keysym.sym == SDLK_SPACE) g_SpaceKey = true;
	else if (vent.key.keysym.sym == SDLK_q)     g_Q_Key = true;
	else if (vent.key.keysym.sym == SDLK_w)     g_W_Key = true;
	else if (vent.key.keysym.sym == SDLK_e)     g_E_Key = true;
	else if (vent.key.keysym.sym == SDLK_a)     g_A_Key = true;
	else if (vent.key.keysym.sym == SDLK_s)     g_S_Key = true;
	else if (vent.key.keysym.sym == SDLK_d)     g_D_Key = true;
	else if (vent.key.keysym.sym == SDLK_z)     g_Z_Key = true;
	else if (vent.key.keysym.sym == SDLK_x)     g_X_Key = true;
	else if (vent.key.keysym.sym == SDLK_c)     g_C_Key = true;

	else if (vent.key.keysym.sym == SDLK_r)     g_R_Key = true;
	else if (vent.key.keysym.sym == SDLK_f)     g_F_Key = true;
	else if (vent.key.keysym.sym == SDLK_t)     g_T_Key = true;
	else if (vent.key.keysym.sym == SDLK_g)     g_G_Key = true;
	else if (vent.key.keysym.sym == SDLK_y)     g_Y_Key = true;
	else if (vent.key.keysym.sym == SDLK_h)     g_H_Key = true;
	else if (vent.key.keysym.sym == SDLK_u)     g_U_Key = true;
	else if (vent.key.keysym.sym == SDLK_j)     g_J_Key = true;
	else if (vent.key.keysym.sym == SDLK_i)     g_I_Key = true;
	else if (vent.key.keysym.sym == SDLK_k)     g_K_Key = true;

	if (g_WinManager.GetWindow() != &g_MainMenu && g_WinManager.GetWindow() != &g_LoadGame)
	{
		int br_no = 0;
		string msg = "";

		switch (vent.key.keysym.sym) {                  // Select Brothel
		case SDLK_1: case SDLK_2: case SDLK_3:
		case SDLK_4: case SDLK_5: case SDLK_6:
		case SDLK_7:
			br_no = vent.key.keysym.sym - SDLK_1;
			if (g_Brothels.GetNumBrothels() > br_no) {
				g_CurrBrothel = br_no;
				g_InitWin = true;
			}
			break;

		case SDLK_TAB:  //cycle through brothles
			if (g_ShiftDown)
			{
				g_CurrBrothel--;
				if (g_CurrBrothel < 0)
					g_CurrBrothel = g_Brothels.GetNumBrothels() - 1;
			}
			else
			{
				g_CurrBrothel++;
				if (g_Brothels.GetNumBrothels() <= g_CurrBrothel)
					g_CurrBrothel = 0;
			}
			g_InitWin = true;
			break;

		case SDLK_ESCAPE:       // Go back to previous screen
			if (g_CurrentScreen == SCREEN_BROTHEL)
				break;
			g_Building = BUILDING_BROTHEL;
			g_InitWin = true;
			g_WinManager.Pop();
			break;

			// girl management screen
		case SDLK_g:    if (g_AltKeys)  break;
		case SDLK_F1:
			g_Building = BUILDING_BROTHEL;
			g_WinManager.PopToWindow(&g_BrothelManagement);
			g_CurrentScreen = SCREEN_GIRLMANAGEMENT;
			g_InitWin = true;
			g_WinManager.push("Girl Management");
			break;

			// staff management screen (gang management)
		case SDLK_t:    if (g_AltKeys)  break;
		case SDLK_F2:
			g_Building = BUILDING_BROTHEL;
			g_WinManager.PopToWindow(&g_BrothelManagement);
			g_CurrentScreen = SCREEN_GANGMANAGEMENT;
			g_InitWin = true;
			g_WinManager.push("Gangs");
			break;

			// Dungeon
		case SDLK_d:    if (g_AltKeys)  break;
		case SDLK_F3:
			g_Building = BUILDING_BROTHEL;
			g_WinManager.PopToWindow(&g_BrothelManagement);
			g_InitWin = true;
			g_CurrentScreen = SCREEN_DUNGEON;
			g_WinManager.push("Dungeon");
			break;

			// Slave market screen
		case SDLK_s:    if (g_AltKeys)  break;
		case SDLK_F4:
			g_Building = BUILDING_BROTHEL;
			g_WinManager.PopToWindow(&g_BrothelManagement);
			g_InitWin = true;
			g_CurrentScreen = SCREEN_SLAVEMARKET;
			g_WinManager.push("Town");
			g_WinManager.push("Slave Market");
			break;

		case SDLK_F5:   // Studio
			if (g_Studios.GetNumBrothels() == 0)    // Does player own the Studio yet?
			{
				msg = "You do not own a Studio";
				g_MessageQue.AddToQue(msg, 0);
				break;
			}
			// Yes!
			g_Building = BUILDING_STUDIO;
			g_WinManager.PopToWindow(&g_BrothelManagement);
			g_CurrentScreen = SCREEN_STUDIO;
			g_InitWin = true;
			g_WinManager.push("Town");
			g_WinManager.push("Movie Screen");
			g_WinManager.push("Studio");
			break;

			// Arena
		case SDLK_e:    if (g_AltKeys)  break;
		case SDLK_F6:
			if (g_Arena.GetNumBrothels() == 0)      // Does player own the Arena yet?
			{
				msg = "You do not own a Arena";
				g_MessageQue.AddToQue(msg, 0);
				break;
			}
			// Yes!
			g_Building = BUILDING_ARENA;
			g_WinManager.PopToWindow(&g_BrothelManagement);
			g_CurrentScreen = SCREEN_ARENA;
			g_InitWin = true;
			g_WinManager.push("Town");
			g_WinManager.push("Arena Screen");
			g_WinManager.push("Arena");
			break;

		case SDLK_F7:   // Centre
			if (g_Centre.GetNumBrothels() == 0)     // Does player own the Centre yet?
			{
				msg = "You do not own a Centre";
				g_MessageQue.AddToQue(msg, 0);
				break;
			}
			// Yes!
			g_Building = BUILDING_CENTRE;
			g_WinManager.PopToWindow(&g_BrothelManagement);
			g_CurrentScreen = SCREEN_CENTRE;
			g_InitWin = true;
			g_WinManager.push("Town");
			g_WinManager.push("Centre Screen");
			g_WinManager.push("Centre");
			break;

			// clinic
		case SDLK_c:    if (g_AltKeys)  break;
		case SDLK_F8:
			if (g_Clinic.GetNumBrothels() == 0)     // Does player own the clinic yet?
			{
				msg = "You do not own a Clinic";
				g_MessageQue.AddToQue(msg, 0);
				break;
			}
			// Yes!
			g_Building = BUILDING_CLINIC;
			g_WinManager.PopToWindow(&g_BrothelManagement);
			g_CurrentScreen = SCREEN_CLINIC;
			g_InitWin = true;
			g_WinManager.push("Town");
			g_WinManager.push("Clinic Screen");
			g_WinManager.push("Clinic");
			break;

			// farm
		case SDLK_F9:
			if (g_Farm.GetNumBrothels() == 0)       // Does player own the farm yet?
			{
				msg = "You do not own a Farm";
				g_MessageQue.AddToQue(msg, 0);
				break;
			}
			// Yes!
			g_Building = BUILDING_FARM;
			g_WinManager.PopToWindow(&g_BrothelManagement);
			g_CurrentScreen = SCREEN_FARM;
			g_InitWin = true;
			g_WinManager.push("Town");
			g_WinManager.push("Farm Screen");
			g_WinManager.push("Farm");
			break;
			// shop screen
		case SDLK_p:    if (vent.key.keysym.sym == SDLK_p && g_AltKeys)  break;
		case SDLK_i:
			if (vent.key.keysym.sym == SDLK_i && g_CurrentScreen == SCREEN_INVENTORY) break;
			g_Building = BUILDING_BROTHEL;
			g_WinManager.PopToWindow(&g_BrothelManagement);
			g_CurrentScreen = SCREEN_INVENTORY;
			g_InitWin = true;
			g_WinManager.push("Town");
			g_WinManager.push("Item Management");
			break;

			// town screen
		case SDLK_o:    if (g_AltKeys)  break;
		case SDLK_F10:
			g_Building = BUILDING_BROTHEL;
			g_WinManager.PopToWindow(&g_BrothelManagement);
			g_CurrentScreen = SCREEN_TOWN;
			g_InitWin = true;
			g_WinManager.push("Town");
			break;

			// turn summary screen
		case SDLK_END:
			if (g_CTRLDown)
			{
				g_CTRLDown = false;
				g_AltKeys = true;
				msg = "Alternate HotKeys Active\n";
				g_MessageQue.AddToQue(msg, 0);
				g_ChoiceManager.CreateChoiceBox(224, 825, 352, 600, 0, 1, 32, strlen(gettext("Close")));
				g_ChoiceManager.AddChoice(0, gettext("Close"), 0);
				g_ChoiceManager.SetActive(0);
				g_ChoiceManager.Free();
				break;
			}
		case SDLK_a:    if (g_AltKeys && vent.key.keysym.sym == SDLK_a)  break;
		case SDLK_F11:
			g_Building = BUILDING_BROTHEL;
			g_WinManager.PopToWindow(&g_BrothelManagement);
			g_CurrentScreen = SCREEN_TURNSUMMARY;
			g_InitWin = true;
			g_WinManager.Push(Turnsummary, &g_Turnsummary);
			break;

		case SDLK_HOME:
			if (g_CTRLDown)
			{
				g_CTRLDown = false;
				g_AltKeys = false;
				msg = "Default HotKeys Active\n";
				g_MessageQue.AddToQue(msg, 0);
				g_ChoiceManager.CreateChoiceBox(224, 825, 352, 600, 0, 1, 32, strlen(gettext("Close")));
				g_ChoiceManager.AddChoice(0, gettext("Close"), 0);
				g_ChoiceManager.SetActive(0);
				g_ChoiceManager.Free();
				break;
			}
		case SDLK_F12:  // House
			g_Building = BUILDING_HOUSE;
			g_WinManager.PopToWindow(&g_BrothelManagement);
			g_CurrentScreen = SCREEN_HOUSE;
			g_InitWin = true;
			g_WinManager.push("Town");
			g_WinManager.push("Player House");
			g_WinManager.push("House Management");
			break;

			// Non F-Key hotkeys (disabled by alt)
			// mayors office screen
		case SDLK_m:    if (g_AltKeys)  break;
			g_Building = BUILDING_BROTHEL;
			g_WinManager.PopToWindow(&g_BrothelManagement);
			g_CurrentScreen = SCREEN_BROTHEL;
			g_InitWin = true;
			/*
			*                       this will make "m" go to brothel management
			*                       you need "M" to go to the mayor screen now
			*                       which is far less used, I think, and easy to get
			*                       to from the town screen
			*
			*                       we should consider some customisable keyboard mapping
			*                       mechanism at some point
			*/
			if (g_ShiftDown) {
				g_CurrentScreen = SCREEN_MAYOR;
				g_WinManager.push("Town");
				g_WinManager.push("Mayor");
			}
			break;

			// bank screen
		case SDLK_b:    if (g_AltKeys)  break;
			g_Building = BUILDING_BROTHEL;
			g_WinManager.PopToWindow(&g_BrothelManagement);
			g_CurrentScreen = SCREEN_BANK;
			g_InitWin = true;
			g_WinManager.push("Town");
			g_WinManager.push("Bank");
			break;

			// upgrades management screen
		case SDLK_u:    if (g_AltKeys)  break;
			g_Building = BUILDING_BROTHEL;
			g_WinManager.PopToWindow(&g_BrothelManagement);
			g_CurrentScreen = SCREEN_BUILDINGMANAGEMENT;
			g_InitWin = true;
			if (g_ShiftDown)	g_WinManager.push("Building Management");
			else				g_WinManager.push("Building Setup");
			break;

		case SDLK_9:
			msg = "These are the active hot keys:\n";
			if (g_AltKeys)
			{
				switch (g_CurrentScreen)
				{
				case SCREEN_BROTHEL:
					msg += "Brothel Screen:\n";
					msg += "Right Arrown    Next Brothel\n";
					msg += "Left Arrow      Previous Brothel\n\n";
					break;
				case SCREEN_TURNSUMMARY:
					msg += "Turn Summary Screen:\n";
					msg += "Up Arrow     Previous Girl\n";
					msg += "Down Arrow   Next Girl\n";
					msg += "Left Arrow   Previous Event\n";
					msg += "Right Arrow  Next Event\n\n";
					msg += "For left handed control:\n";
					msg += "A     Previous Girl\n";
					msg += "D     Next Girl\n";
					msg += "W     Previous Event\n";
					msg += "S     Next Event\n";
					msg += "Q     Next Catagory\n";
					msg += "E     Previous Catagory\n";
					msg += "Space Change current picture\n\n";
					break;
				case SCREEN_GALLERY:
					msg += "Gallery:\n";
					msg += "Left Arrow     Previous Picture\n";
					msg += "Right Arrow    Next Picture\n";
					msg += "Up Arrow     Previous Gallery\n";
					msg += "Down Arrow     Next Gallery\n\n";
					msg += "For left handed control:\n";
					msg += "A     Previous Picture\n";
					msg += "D     Next Picture\n";
					msg += "W     Previous Gallery\n";
					msg += "S     Next Gallery\n\n";
					break;
				case SCREEN_TRANSFERGIRLS:
					msg += "Transfer Screen:\n";
					msg += "No special hotkeys\n";
					msg += "Yet...\n\n";
					break;
				case SCREEN_GIRLMANAGEMENT:
					msg += "Girl Management:\n";
					msg += "Up Arrow     Previous Girl\n";
					msg += "Down Arrow   Next Girl\n\n";
					msg += "For left handed control:\n";
					msg += "A     Previous Girl\n";
					msg += "D     Next Girl\n";
					msg += "W     Previous Work Area\n";
					msg += "S     Next Work Area\n";
					msg += "Q     Previous Job\n";
					msg += "E     Next Job\n";
					msg += "Z     Day Shift\n";
					msg += "C     Night Shift\n";
					msg += "Space   Goto Girl Details\n\n";
					break;
				case SCREEN_GIRLDETAILS:
					msg += "Girl Details:\n";
					msg += "Up Arrow    Previous Girl\n";
					msg += "Down Arrow  Next Girl\n\n";
					msg += "A    Previous Girl\n";
					msg += "D    Next Girl\n";
					msg += "S    More Details\n";
					msg += "Space    Gallery\n\n";
					msg += "J    House Percent Up\n";
					msg += "H    House Percent Down\n\n";
					break;
				case SCREEN_INVENTORY:
					msg += "Inventory Screen:\n";
					msg += "                                    Up         Down\n";
					msg += "Inventory type :        R           F\n";
					msg += "Owner list left :          T           G\n";
					msg += "Owner list right :        Y           H\n";
					msg += "Items list left :          U           J\n";
					msg += "Items list right :         I           K\n\n\n";
					msg += "There are no hotkeys for buy and sell buttons\n";
					msg += "or for equip or unequip buttons\n";
					msg += "to prevent accidental buying, selling or equiping of items\n";
					msg += "\n\n";
					break;
				case SCREEN_GANGMANAGEMENT:
					msg += "Gang Management:\n";
					msg += "Up Arrow    Previous Gang\n";
					msg += "Down Arrow  Next Gang\n\n";
					msg += "For left handed control:\n";
					msg += "A     Previous Gang\n";
					msg += "D     Next Gang\n";
					msg += "W     Previous Mission\n";
					msg += "S     Next Mission\n";
					msg += "Q     Previous Recruits\n";
					msg += "E     Next Recruits\n";
					msg += "Space    Hire Gang\n\n";
					break;
				case SCREEN_BROTHELMANAGEMENT:
					msg += "Brothel Management:\n";
					msg += "No special hotkeys\n";
					msg += "Yet...\n\n";
					break;
				case SCREEN_DUNGEON:
					msg += "Dungeon:\n";
					msg += "Up Arrow    Previous girl\n";
					msg += "Down Arrow  Next Girl\n\n";
					msg += "For left handed control:\n";
					msg += "A     Previous Girl\n";
					msg += "D     Next Girl\n\n";
					break;
				case SCREEN_TOWN:
					msg += "Town:\n";
					msg += "No special hotkeys\n";
					msg += "Yet...\n\n";
					break;
				case SCREEN_MAYOR:
					msg += "Mayor:\n";
					msg += "No special hotkeys\n";
					msg += "Yet...\n\n";
					break;
				case SCREEN_BANK:
					msg += "Bank:\n";
					msg += "No special hotkeys\n";
					msg += "Yet...\n\n";
					break;
				case SCREEN_JAIL:
					msg += "Jail:\n";
					msg += "Up Arrow    Previous Gang\n";
					msg += "Down Arrow  Next Gang\n\n";
					msg += "For left handed control:\n";
					msg += "A     Previous Girl\n";
					msg += "D     Next Girl\n\n";
					break;
				case SCREEN_HOUSE:
					msg += "House:\n";
					msg += "Space   Goto Girl Details\n\n";
					break;
				case SCREEN_CLINIC:
					msg += "Clinic:\n";
					msg += "Up Arrow    Previous Gang\n";
					msg += "Down Arrow  Next Gang\n\n";
					msg += "For left handed control:\n";
					msg += "A     Previous Girl\n";
					msg += "D     Next Girl\n\n";
					msg += "Space   Goto Girl Details\n\n";
					break;
				case SCREEN_ARENA:
					msg += "Arena:\n";
					msg += "Up Arrow    Previous Gang\n";
					msg += "Down Arrow  Next Gang\n\n";
					msg += "For left handed control:\n";
					msg += "A     Previous Girl\n";
					msg += "D     Next Girl\n\n";
					msg += "Space   Goto Girl Details\n\n";
					break;
				case SCREEN_TRYOUTS:
					msg += "Try Outs:\n";
					msg += "No special hotkeys\n";
					msg += "Yet...\n\n";
					break;
				case SCREEN_CENTRE:
					msg += "Centre:\n";
					msg += "Up Arrow    Previous Gang\n";
					msg += "Down Arrow  Next Gang\n\n";
					msg += "For left handed control:\n";
					msg += "A     Previous Girl\n";
					msg += "D     Next Girl\n\n";
					msg += "Space   Goto Girl Details\n\n";
					break;
				case SCREEN_STUDIO:
					msg += "Studio:\n";
					msg += "Up Arrow    Previous Gang\n";
					msg += "Down Arrow  Next Gang\n\n";
					msg += "For left handed control:\n";
					msg += "A     Previous Girl\n";
					msg += "D     Next Girl\n\n";
					msg += "Space   Goto Girl Details\n\n";
					break;
				case SCREEN_CREATEMOVIE:
					msg += "Create Movie:\n";
					msg += "No special hotkeys\n";
					msg += "Yet...\n\n";
					break;
				case SCREEN_BUILDINGMANAGEMENT:
					msg += "Building Management:\n";
					msg += "This screen is not implimented yet\n\n";
					break;
				case SCREEN_SLAVEMARKET:
					msg += "Slave Market:\n";
					msg += "Up Arrow    Previous girl\n";
					msg += "Down Arrow  Next Girl\n\n";
					msg += "For left handed control:\n";
					msg += "A     Previous Girl\n";
					msg += "D     Next Girl\n";
					msg += "S     More Details\n";
					msg += "Space   Purchase Girl\n\n";
					break;
				case SCREEN_GALLERY2:
					msg += "Second Gallery:\n";
					msg += "Left Arrow     Previous Picture\n";
					msg += "Right Arrow    Next Picture\n";
					msg += "Up Arrow     Previous Gallery\n";
					msg += "Down Arrow     Next Gallery\n\n";
					msg += "For left handed control:\n";
					msg += "A     Previous Picture\n";
					msg += "D     Next Picture\n";
					msg += "W     Previous Gallery\n";
					msg += "S     Next Gallery\n\n";
					break;
				}
			}
			else
			{
				switch (g_CurrentScreen)
				{
				case SCREEN_BROTHEL:
					msg += "Brothel Screen";
					msg += "Right Arrown    Next Brothel\n";
					msg += "Left Arrow      Previous Brothel\n\n";
					break;
				case SCREEN_TURNSUMMARY:
					msg += "Up Arrow     Previous Girl\n";
					msg += "Down Arrow   Next Girl\n";
					msg += "Left Arrow   Previous Event\n";
					msg += "Right Arrow  Next Event\n\n";
					break;
				case SCREEN_GALLERY:
					msg += "Gallery:\n";
					msg += "Left Arrow     Previous Picture\n";
					msg += "Right Arrow    Next Picture\n\n";
					break;
				case SCREEN_TRANSFERGIRLS:
					msg += "Transfer Screen:\n";
					msg += "No special hotkeys\n";
					msg += "Yet...\n\n";
					break;
				case SCREEN_GIRLMANAGEMENT:
					msg += "Girl Management:\n";
					msg += "Up Arrow     Previous Girl\n";
					msg += "Down Arrow   Next Girl\n\n";
					break;
				case SCREEN_GIRLDETAILS:
					msg += "Girl Details\n";
					msg += "Up Arrow    Previous Girl\n";
					msg += "Down Arrow  Next Girl\n\n";
					break;
				case SCREEN_INVENTORY:
					msg += "Inventory Screen:\n";
					msg += "No special hotkeys\n";
					msg += "Yet...\n\n";
					break;
				case SCREEN_GANGMANAGEMENT:
					msg += "Gang Management:\n";
					msg += "Up Arrow    Previous Gang\n";
					msg += "Down Arrow  Next Gang\n";
					msg += "Space     Hire gang\n\n";
					break;
				case SCREEN_BROTHELMANAGEMENT:
					msg += "Brothel Management:\n";
					msg += "No special hotkeys\n";
					msg += "Yet...\n\n";
					break;
				case SCREEN_DUNGEON:
					msg += "Dungeon:\n";
					msg += "Up Arrow    Previous girl\n";
					msg += "Down Arrow  Next Girl\n\n";
					break;
				case SCREEN_TOWN:
					msg += "Town:\n";
					msg += "No special hotkeys\n";
					msg += "Yet...\n\n";
					break;
				case SCREEN_MAYOR:
					msg += "Mayor:\n";
					msg += "No special hotkeys\n";
					msg += "Yet...\n\n";
					break;
				case SCREEN_BANK:
					msg += "Bank:\n";
					msg += "No special hotkeys\n";
					msg += "Yet...\n\n";
					break;
				case SCREEN_JAIL:
					msg += "Jail:\n";
					msg += "Up Arrow    Previous Gang\n";
					msg += "Down Arrow  Next Gang\n\n";
					break;
				case SCREEN_HOUSE:
					msg += "House:\n";
					msg += "No special hotkeys\n";
					msg += "Yet...\n\n";
					break;
				case SCREEN_CLINIC:
					msg += "Clinic:\n";
					msg += "Up Arrow    Previous Gang\n";
					msg += "Down Arrow  Next Gang\n\n";
					break;
				case SCREEN_ARENA:
					msg += "Arena:\n";
					msg += "Up Arrow    Previous Gang\n";
					msg += "Down Arrow  Next Gang\n\n";
					break;
				case SCREEN_TRYOUTS:
					msg += "Try Outs:\n";
					msg += "No special hotkeys\n";
					msg += "Yet...\n\n";
					break;
				case SCREEN_CENTRE:
					msg += "Centre:\n";
					msg += "Up Arrow    Previous Gang\n";
					msg += "Down Arrow  Next Gang\n\n";
					break;
				case SCREEN_STUDIO:
					msg += "Studio:\n";
					msg += "Up Arrow    Previous Gang\n";
					msg += "Down Arrow  Next Gang\n\n";
					break;
				case SCREEN_CREATEMOVIE:
					msg += "Create Movie:\n";
					msg += "No special hotkeys\n";
					msg += "Yet...\n\n";
					break;
				case SCREEN_BUILDINGMANAGEMENT:
					msg += "Building Management:\n";
					msg += "This screen is not implimented yet\n\n";
					break;
				case SCREEN_SLAVEMARKET:
					msg += "Slave Market:\n";
					msg += "Up Arrow    Previous girl\n";
					msg += "Down Arrow  Next Girl\n\n";
					break;
				}
			}
			g_MessageQue.AddToQue(msg, 1);
			break;

		case SDLK_0:
		{
			cConfig cfg;
			msg = "Global Hotkeys:\n";
			msg += "1-7         Brothels\n";
			msg += "Tab         Cycle Brothels\n";
			msg += "Shift-Tab   Reverse\n";
			msg += "Escape      Back one screen\n";
			msg += "F1          Girl Management\n";
			msg += "F2          Gang management\n";
			msg += "F3          Dungeon\n";
			msg += "F4          SlaveMarket\n";
			msg += "F5          Studio\n";
			msg += "F6          Arena\n";
			msg += "F7          Centre\n";
			msg += "F8          Clinic\n";
			msg += "F9          Farm\n";
			msg += "F10         Town Screen\n";
			msg += "F11         Turn Summary\n";
			msg += "F12         House\n";
			msg += "9           List Hotkeys for this screen.\n";
			msg += "0           List Global Hotkeys.\n";
			msg += "I           Shop Screen (Inventory)\n";
			msg += "\n\n";
			msg += "Space Key   Clears message boxes.\n";
			msg += "Ctrl + Home Default HotKeys\n";
			msg += "Ctrl + End  Alternate HotKeys\n";
			g_MessageQue.AddToQue(msg, 0);
			break;
		}

		case SDLK_SPACE:
			g_SpaceKey = true;
			break;

		default:
			// do nothing, but the "default" clause silences an irritating warning
			break;
		}
		if (g_AltKeys)
		{
			switch (vent.key.keysym.sym)
			{
			case SDLK_q:    g_Q_Key = true;         break;
			case SDLK_w:    g_W_Key = true;         break;
			case SDLK_e:    g_E_Key = true;         break;
			case SDLK_a:    g_A_Key = true;         break;
			case SDLK_s:    g_S_Key = true;         break;
			case SDLK_d:    g_D_Key = true;         break;
			case SDLK_z:    g_Z_Key = true;         break;
			case SDLK_x:    g_X_Key = true;         break;
			case SDLK_c:    g_C_Key = true;         break;

			case SDLK_r:    g_R_Key = true;         break;
			case SDLK_f:    g_F_Key = true;         break;
			case SDLK_t:    g_T_Key = true;         break;
			case SDLK_g:    g_G_Key = true;         break;
			case SDLK_y:    g_Y_Key = true;         break;
			case SDLK_h:    g_H_Key = true;         break;
			case SDLK_u:    g_U_Key = true;         break;
			case SDLK_j:    g_J_Key = true;         break;
			case SDLK_i:    g_I_Key = true;         break;
			case SDLK_k:    g_K_Key = true;         break;
			default:
				break;
			}
		}
	}
}

int main(int ac, char* av[])
{
#ifndef LINUX
#ifdef _DEBUG
	//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	//_CrtSetBreakAlloc(16477);
#endif
#endif

	// get text
	setlocale(LC_ALL, "");
	DirPath base = DirPath() << "Resources" << "lang";
	bindtextdomain("whoremaster", base);
	textdomain("whoremaster");

	cConfig cfg;
	bool running = true;
	bool quitPending = false;
	bool mouseDown = false;

	g_LogFile.ss()
		<< "Startup: checking for variant config: argc = " << ac << ".\n";
	if (ac > 1) {
		g_LogFile.ss() << "     attempting to load '" << av[1] << "'\n";
		cfg.reload(av[1]);
	}

	CLog log = log;
	log.write("calling init");
	// INit the program
	if (!Init())
		return 1;

	g_WinManager.push("Main Menu");

	while (running)
	{
		while (SDL_PollEvent(&vent))
		{
			if (vent.type == SDL_QUIT)
			{
				running = false;
			}
			else if (vent.type == SDL_MOUSEBUTTONUP)
			{
				if (mouseDown == true)
				{
					if (g_DragScrollBar != 0)
					{
						g_DragScrollBar->SetTopValue(g_DragScrollBar->m_ItemTop);
						g_DragScrollBar = 0;
					}
					else if (g_DragSlider != 0)
					{
						g_DragSlider->EndDrag();
						g_DragSlider = 0;
					}
					else if (g_MessageBox.IsActive())
						g_MessageBox.Advance();
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
					if (mouseDown == false)
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
				if (!g_MessageBox.IsActive() && !g_ChoiceManager.IsActive())
				{
					if (vent.key.keysym.sym == SDLK_RSHIFT || vent.key.keysym.sym == SDLK_LSHIFT)   // enable multi select
						g_ShiftDown = false;
					else if (vent.key.keysym.sym == SDLK_RCTRL || vent.key.keysym.sym == SDLK_LCTRL)        // enable multi select
						g_CTRLDown = false;

					if (vent.key.keysym.sym == SDLK_UP)				g_UpArrow = false;
					else if (vent.key.keysym.sym == SDLK_DOWN)		g_DownArrow = false;
					else if (vent.key.keysym.sym == SDLK_LEFT)		g_LeftArrow = false;
					else if (vent.key.keysym.sym == SDLK_RIGHT)		g_RightArrow = false;
					else if (vent.key.keysym.sym == SDLK_SPACE)		g_SpaceKey = false;
					else if (vent.key.keysym.sym == SDLK_q)			g_Q_Key = false;
					else if (vent.key.keysym.sym == SDLK_w)			g_W_Key = false;
					else if (vent.key.keysym.sym == SDLK_e)			g_E_Key = false;
					else if (vent.key.keysym.sym == SDLK_a)			g_A_Key = false;
					else if (vent.key.keysym.sym == SDLK_s)			g_S_Key = false;
					else if (vent.key.keysym.sym == SDLK_d)			g_D_Key = false;
					else if (vent.key.keysym.sym == SDLK_z)			g_Z_Key = false;
					else if (vent.key.keysym.sym == SDLK_x)			g_X_Key = false;
					else if (vent.key.keysym.sym == SDLK_c)			g_C_Key = false;

					else if (vent.key.keysym.sym == SDLK_r)			g_R_Key = false;
					else if (vent.key.keysym.sym == SDLK_f)			g_F_Key = false;
					else if (vent.key.keysym.sym == SDLK_t)			g_T_Key = false;
					else if (vent.key.keysym.sym == SDLK_g)			g_G_Key = false;
					else if (vent.key.keysym.sym == SDLK_y)			g_Y_Key = false;
					else if (vent.key.keysym.sym == SDLK_h)			g_H_Key = false;
					else if (vent.key.keysym.sym == SDLK_u)			g_U_Key = false;
					else if (vent.key.keysym.sym == SDLK_j)			g_J_Key = false;
					else if (vent.key.keysym.sym == SDLK_i)			g_I_Key = false;
					else if (vent.key.keysym.sym == SDLK_k)			g_K_Key = false;
				}
			}
			else if (vent.type == SDL_KEYDOWN)
			{
				if (!g_MessageBox.IsActive() && !g_ChoiceManager.IsActive())
				{
					if (g_WinManager.HasEditBox())
					{
						if (vent.key.keysym.sym == SDLK_BACKSPACE)		g_WinManager.UpdateKeyInput('-');
						else if (vent.key.keysym.sym == SDLK_RETURN)	g_EnterKey = true;
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
						handle_hotkeys();
					}
				}
				else if (vent.key.keysym.sym == SDLK_SPACE)
				{
					if (g_MessageBox.IsActive())
						g_MessageBox.Advance();
					g_SpaceKey = false;
				}
			}
			else if (vent.type == SDL_MOUSEMOTION)
			{
				if (!g_MessageBox.IsActive() && !g_ChoiceManager.IsActive())
				{
					// if dragging a scrollbar, send movements to it exclusively until mouseup
					if (g_DragScrollBar != 0)
						g_DragScrollBar->DragMove(vent.motion.y);
					// if dragging a slider, send movements to it exclusively until mouseup
					else if (g_DragSlider != 0)
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

		// draw the background image
		SDL_Rect clip;
		clip.x = 0;
		clip.y = 0;
		clip.w = _G.g_ScreenWidth;
		clip.h = _G.g_ScreenHeight;
		g_BackgroundImage->DrawSurface(clip.x, clip.y, 0, &clip, true);

		// Draw the interface
		g_WinManager.Draw();

		if (!g_MessageBox.IsActive() && g_MessageQue.HasNext())
			g_MessageQue.ActivateNext();

		if (eventrunning && !g_MessageBox.IsActive() && !g_ChoiceManager.IsActive())    // run any events that are being run
			GameEvents();

		// Run the interface
		if (!g_MessageBox.IsActive() && !g_ChoiceManager.IsActive())
			g_WinManager.UpdateCurrent();

		// Draw Any message boxes
		if (g_MessageBox.IsActive())
		{
			g_MessageBox.Draw();
		}
		else if (g_ChoiceManager.IsActive() && !g_MessageQue.HasNext())
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
	g_LogFile.write("Releasing Images");
	delete g_BackgroundImage;

	for (int i = 0; i<7; i++)
	{
		if (g_BrothelImages[i])
		{
			delete g_BrothelImages[i];
			g_BrothelImages[i] = 0;
		}
	}

	g_LogFile.write("Releasing Girls");
	for (int i = 0; i<8; i++)
	{
		if (MarketSlaveGirls[i] && MarketSlaveGirlsDel[i] == -1)
			delete MarketSlaveGirls[i];
		MarketSlaveGirls[i] = 0;
	}

	g_Girls.Free();

	g_LogFile.write("Releasing Customers");
	g_Customers.Free();

	g_Traits.Free();
	g_InvManager.Free();

	g_LogFile.write("Releasing Buildings");
	g_Brothels.Free();
	g_Clinic.Free();
	g_Studios.Free();
	g_Arena.Free();
	g_Centre.Free();
	g_House.Free();
	g_Farm.Free();

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

bool Init()
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
	// Load the universal background image
	g_BackgroundImage = new ImageSurface("background", "");
	g_LogFile.write("Background Image Set");

	LoadInterface();        // Load the interface
	g_LogFile.write("Interface Loaded");

	InitGameFlags();        // Init the game flags
	g_LogFile.write("Game Flags Initialized");

	// Load the brothel images
	for (int i = 0; i<7; i++)
	{
		if (g_BrothelImages[i])
		{
			delete g_BrothelImages[i];
			g_BrothelImages[i] = 0;
		}
		/*
		*               I think this should work - kept the old line below
		*               reference
		*/
		char buffer[32];
		_itoa(i, buffer, 10);
		g_BrothelImages[i] = new ImageSurface("Brothel", buffer);
		//g_BrothelImages[i]->LoadImage(file,false);
	}
	g_LogFile.write("Brothel Images Set");

	return true;
}

