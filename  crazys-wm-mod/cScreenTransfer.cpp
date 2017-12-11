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
#pragma region //	Includes and Externs			//
#include <iostream>
#include <locale>
#include <sstream>
#include "cScreenTransfer.h"

#include "cBrothel.h"
#include "cMovieStudio.h"
#include "cArena.h"
#include "cCentre.h"
#include "cClinic.h"
#include "cFarm.h"
#include "cHouse.h"

#include "cGangs.h"
#include "cScreenGirlDetails.h"
#include "InterfaceProcesses.h"
#include "cScriptManager.h"
#include "cGetStringScreenManager.h"
#include "cGold.h"
#include "cScreenBrothelManagement.h"

#include "cWindowManager.h"
#include "sFacilityList.h"
#include "FileList.h"
#include "cInterfaceWindow.h"

#include "InterfaceIDs.h"
#include "main.h"



extern	bool					g_InitWin;
extern	int						g_CurrBrothel;
extern	int						g_CurrClinic;
extern	int						g_CurrStudio;
extern	int						g_CurrArena;
extern	int						g_CurrCentre;
extern	int						g_CurrFarm;
extern	int						g_CurrHouse;
extern	int						g_CurrentScreen;
extern	cBrothelManager			g_Brothels;
extern	cClinicManager			g_Clinic;
extern	cMovieStudioManager		g_Studios;
extern	cArenaManager			g_Arena;
extern	cCentreManager			g_Centre;
extern	cFarmManager			g_Farm;
extern	cHouseManager			g_House;
extern	int						g_Building;

extern	cGangManager			g_Gangs;
extern	cGold					g_Gold;
extern cScreenBrothelManagement g_BrothelManagement;
extern	cWindowManager			g_WinManager;
extern	cInterfaceEventManager	g_InterfaceEvents;
extern	cScreenGirlDetails		g_GirlDetails;
extern	bool					g_Cheats;
extern	bool					eventrunning;
extern	bool					g_AllTogle;

extern	CSurface*				g_BrothelImages[];
extern	string					pic_types[];

extern	void					GetString();
extern	cInterfaceWindow		g_GetString;
extern	sGirl*					selected_girl;

extern	bool	g_LeftArrow;
extern	bool	g_RightArrow;
extern	bool	g_UpArrow;
extern	bool	g_DownArrow;
extern	bool	g_AltKeys;	// New hotkeys --PP
extern	bool	g_EnterKey;
extern	bool	g_SpaceKey;
extern	bool	g_Q_Key;
extern	bool	g_W_Key;
extern	bool	g_E_Key;
extern	bool	g_A_Key;
extern	bool	g_S_Key;
extern	bool	g_D_Key;
extern	bool	g_Z_Key;
extern	bool	g_X_Key;
extern	bool	g_C_Key;
extern	bool	g_O_Key;
extern	bool	g_CTRLDown;
extern  bool    g_EnterKey; // More Hotkeys

#pragma endregion
#pragma region //	Local Variables			//
bool cScreenTransfer::ids_set = false;
enum{
	Summary_GIRLS,
	Summary_GANGS,
	Summary_BROTHELS,
	Summary_DUNGEON,
	Summary_STUDIO,
	Summary_ARENA,
	Summary_CENTRE,
	Summary_CLINIC,
	Summary_FARM,
	Summary_HOUSE
};

static int	Category = -1;
static int	Category_Last = -1;
static bool	Category_Change = false;
static int	Item = -1;
static int	Item_Last = -1;
static bool	Item_Change = false;
static int	Event = -1;
static int	Event_Last = -1;
static bool	Event_Change = false;
static int	Image = -1;
static int	Image_Last = -1;
static int	Image_Type = -1;
static bool	Image_Change = false;
static int summarysortorder = 0;	// the order girls get sorted in the summary lists
#pragma endregion
static int leftBrothel = 0;
static int rightBrothel = 0;

cScreenTransfer::cScreenTransfer()
{
	DirPath dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "TransferGirls.xml";
	m_filename = dp.c_str();
}
cScreenTransfer::~cScreenTransfer() {}

void cScreenTransfer::set_ids()
{
	ids_set = true;
	labelleft_id	= get_id("LabelLeft");
	labelright_id	= get_id("LabelRight");
	brothelleft_id	= get_id("BrothelLeft"); //0
	listleft_id		= get_id("ListLeft");
	listright_id	= get_id("ListRight");
	brothelright_id	= get_id("BrothelRight"); //3
	shiftright_id	= get_id("ShiftRight");
	shiftleft_id	= get_id("ShiftLeft");
	back_id			= get_id("Back");
}

void cScreenTransfer::process()
{
	if (!ids_set) set_ids();		// we need to make sure the ID variables are set
	if (g_InitWin) init();			// set up the window if needed
	check_events();

}
void cScreenTransfer::init()
{
	g_CurrentScreen = SCREEN_TRANSFERGIRLS;
	Focused();

	leftBrothel = rightBrothel = 0;
	// clear list boxes
	ClearListBox(brothelleft_id);
	ClearListBox(brothelright_id);
	ClearListBox(listleft_id);
	ClearListBox(listright_id);

	// list all the brothels
	sBrothel* current = g_Brothels.GetBrothel(0);
	int i = 6;
	while (current)
	{
		AddToListBox(brothelleft_id, i, current->m_Name);
		AddToListBox(brothelright_id, i, current->m_Name);
		i++;
		current = current->m_Next;
	}

	SetSelectedItemInList(brothelleft_id, 6);
	SetSelectedItemInList(brothelright_id, 6);

	if (g_Clinic.GetNumBrothels() > 0)		// add the clinic
	{
		sClinic* currentClinic = (sClinic*)g_Clinic.GetBrothel(0);
		while (currentClinic)
		{
			AddToListBox(brothelleft_id, 3, currentClinic->m_Name);
			AddToListBox(brothelright_id, 3, currentClinic->m_Name);
			currentClinic = (sClinic*)currentClinic->m_Next;
		}
	}
	if (g_Studios.GetNumBrothels() > 0)		// add the movie studio studio
	{
		sMovieStudio* currentStudio = (sMovieStudio*)g_Studios.GetBrothel(0);
		while (currentStudio)
		{
			AddToListBox(brothelleft_id, 0, currentStudio->m_Name);
			AddToListBox(brothelright_id, 0, currentStudio->m_Name);
			currentStudio = (sMovieStudio*)currentStudio->m_Next;
		}
	}
	if (g_Arena.GetNumBrothels() > 0)		// add the arena
	{
		sArena* currentArena = (sArena*)g_Arena.GetBrothel(0);
		while (currentArena)
		{
			AddToListBox(brothelleft_id, 1, currentArena->m_Name);
			AddToListBox(brothelright_id, 1, currentArena->m_Name);
			currentArena = (sArena*)currentArena->m_Next;
		}
	}
	if (g_Centre.GetNumBrothels() > 0)		// add the centre
	{
		sCentre* currentCentre = (sCentre*)g_Centre.GetBrothel(0);
		while (currentCentre)
		{
			AddToListBox(brothelleft_id, 2, currentCentre->m_Name);
			AddToListBox(brothelright_id, 2, currentCentre->m_Name);
			currentCentre = (sCentre*)currentCentre->m_Next;
		}
	}
	if (g_Farm.GetNumBrothels() > 0)	// add the farm
	{
		sFarm* currentFarm = (sFarm*)g_Farm.GetBrothel(0);
		while (currentFarm)
		{
			AddToListBox(brothelleft_id, 4, currentFarm->m_Name);
			AddToListBox(brothelright_id, 4, currentFarm->m_Name);
			currentFarm = (sFarm*)currentFarm->m_Next;
		}
	}
	// add the house
	sHouse* currentHouse = (sHouse*)g_House.GetBrothel(0);
	while (currentHouse)
	{
		AddToListBox(brothelleft_id, 5, currentHouse->m_Name);
		AddToListBox(brothelright_id, 5, currentHouse->m_Name);
		currentHouse = (sHouse*)currentHouse->m_Next;
	}

	g_InitWin = false;



}
void cScreenTransfer::check_events()
{
	if (g_InterfaceEvents.GetNumEvents() == 0)	return;							// no events means we can go home

	if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, back_id))
	{
		g_InitWin = true;
		g_WinManager.Pop();
		return;
	}
	else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, shiftright_id))
	{
		if ((rightBrothel != -1 && leftBrothel != -1))
		{
			TransferGirlsRightToLeft(rightBrothel, leftBrothel);
		}
	}
	else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, 
		shiftleft_id))
	{
		if ((rightBrothel != -1 && leftBrothel != -1))
		{
			TransferGirlsLeftToRight(rightBrothel, leftBrothel);
		}
	}
	else if (g_InterfaceEvents.CheckEvent(EVENT_SELECTIONCHANGE, brothelleft_id))
	{
		int color = COLOR_BLUE;
		ClearListBox(listleft_id);
		leftBrothel = GetSelectedItemFromList(brothelleft_id);
		if (leftBrothel != -1)
		{
			if (leftBrothel > 5){
				// add the girls to the list
				sGirl* temp = g_Brothels.GetGirl(leftBrothel - 6, 0);
				int i = 0;
				while (temp)
				{
					if (temp->m_DayJob == JOB_MATRON || temp->m_NightJob == JOB_MATRON)
						color = COLOR_RED;
					else if (temp->m_DayJob == JOB_RESTING && temp->m_NightJob == JOB_RESTING)
						color = COLOR_GREEN;
					else color = COLOR_BLUE;

					AddToListBox(listleft_id, i, temp->m_Realname, color);
					i++;
					temp = temp->m_Next;
				}
			}
			else if (leftBrothel == 4)
			{
				// add the girls to the list
				sGirl* temp = g_Farm.GetGirl(0, 0);
				int i = 0;
				while (temp)
				{
					if (temp->m_DayJob == JOB_FARMMANGER || temp->m_NightJob == JOB_FARMMANGER)
						color = COLOR_RED;
					else if (temp->m_DayJob == JOB_FARMREST && temp->m_NightJob == JOB_FARMREST)
						color = COLOR_GREEN;
					else color = COLOR_BLUE;

					AddToListBox(listleft_id, i, temp->m_Realname, color);
					i++;
					temp = temp->m_Next;
				}
			}
			else if (leftBrothel == 5)
			{
				// add the girls to the list
				sGirl* temp = g_House.GetGirl(0, 0);
				int i = 0;
				while (temp)
				{
					if (temp->m_DayJob == JOB_HEADGIRL || temp->m_NightJob == JOB_HEADGIRL)
						color = COLOR_RED;
					else if (temp->m_DayJob == JOB_HOUSEREST && temp->m_NightJob == JOB_HOUSEREST)
						color = COLOR_GREEN;
					else if (temp->m_DayJob == JOB_SO_STRAIGHT || temp->m_NightJob == JOB_SO_STRAIGHT ||
						temp->m_DayJob == JOB_SO_BISEXUAL || temp->m_NightJob == JOB_SO_BISEXUAL ||
						temp->m_DayJob == JOB_SO_LESBIAN || temp->m_NightJob == JOB_SO_LESBIAN ||
						temp->m_DayJob == JOB_FAKEORGASM || temp->m_NightJob == JOB_FAKEORGASM)
					{
						if (temp->m_WorkingDay == 0) color = COLOR_DARKBLUE;
						else color = COLOR_RED;
					}
					else color = COLOR_BLUE;

					AddToListBox(listleft_id, i, temp->m_Realname, color);
					i++;
					temp = temp->m_Next;
				}
			}
			else if (leftBrothel == 2)
			{
				// add the girls to the list
				sGirl* temp = g_Centre.GetGirl(0, 0);
				int i = 0;
				while (temp)
				{
					if (temp->m_DayJob == JOB_CENTREMANAGER || temp->m_NightJob == JOB_CENTREMANAGER)
						color = COLOR_RED;
					else if (temp->m_DayJob == JOB_COUNSELOR || temp->m_NightJob == JOB_COUNSELOR)
					{
						if (g_Centre.GetNumGirlsOnJob(0, JOB_REHAB, SHIFT_NIGHT) < 1) color = COLOR_DARKBLUE;
						else color = COLOR_RED;
					}
					else if (temp->m_DayJob == JOB_CENTREREST && temp->m_NightJob == JOB_CENTREREST)
						color = COLOR_GREEN;
					else if (temp->m_DayJob == JOB_REHAB || temp->m_NightJob == JOB_REHAB)
					{
						if (temp->m_WorkingDay == 0) color = COLOR_DARKBLUE;
						else color = COLOR_RED;
					}
					else color = COLOR_BLUE;

					AddToListBox(listleft_id, i, temp->m_Realname, color);
					i++;
					temp = temp->m_Next;
				}
			}
			else if (leftBrothel == 1)
			{
				// add the girls to the list
				sGirl* temp = g_Arena.GetGirl(0, 0);
				int i = 0;
				while (temp)
				{
					if (temp->m_DayJob == JOB_DOCTORE || temp->m_NightJob == JOB_DOCTORE)
						color = COLOR_RED;
					else if (temp->m_DayJob == JOB_ARENAREST && temp->m_NightJob == JOB_ARENAREST)
						color = COLOR_GREEN;
					else color = COLOR_BLUE;
					AddToListBox(listleft_id, i, temp->m_Realname, color);
					i++;
					temp = temp->m_Next;
				}
			}
			else if (leftBrothel == 3)
			{
				// add the girls to the list
				sGirl* temp = g_Clinic.GetGirl(0, 0);
				int i = 0;
				while (temp)
				{
					if (temp->m_DayJob == JOB_CHAIRMAN || temp->m_NightJob == JOB_CHAIRMAN)
						color = COLOR_RED;
					else if (temp->m_DayJob == JOB_CLINICREST && temp->m_NightJob == JOB_CLINICREST)
						color = COLOR_GREEN;
					else if (temp->m_DayJob == JOB_DOCTOR || temp->m_NightJob == JOB_DOCTOR)
					{
						if (g_Clinic.GetNumberPatients(0) < 1 && g_Clinic.GetNumberPatients(1) < 1)
							color = COLOR_DARKBLUE;
						else color = COLOR_RED;
					}
					else if (temp->m_DayJob == JOB_MECHANIC || temp->m_NightJob == JOB_MECHANIC)
					{
						if (g_Clinic.GetNumGirlsOnJob(0, JOB_GETREPAIRS, 0) < 1 && g_Clinic.GetNumGirlsOnJob(0, JOB_GETREPAIRS, 1) < 1)
							color = COLOR_DARKBLUE;
						else color = COLOR_RED;
					}
					else if (temp->m_DayJob == JOB_GETHEALING || temp->m_NightJob == JOB_GETHEALING
						|| temp->m_DayJob == JOB_GETREPAIRS || temp->m_NightJob == JOB_GETREPAIRS)
					{
						if (temp->health() > 70 && temp->tiredness() < 30) color = COLOR_DARKBLUE;
						else color = COLOR_RED;
					}
					else if (temp->m_DayJob == JOB_GETABORT || temp->m_NightJob == JOB_GETABORT
						|| temp->m_DayJob == JOB_COSMETICSURGERY || temp->m_NightJob == JOB_COSMETICSURGERY
						|| temp->m_DayJob == JOB_LIPO || temp->m_NightJob == JOB_LIPO
						|| temp->m_DayJob == JOB_BREASTREDUCTION || temp->m_NightJob == JOB_BREASTREDUCTION
						|| temp->m_DayJob == JOB_BOOBJOB || temp->m_NightJob == JOB_BOOBJOB
						|| temp->m_DayJob == JOB_VAGINAREJUV || temp->m_NightJob == JOB_VAGINAREJUV
						|| temp->m_DayJob == JOB_FACELIFT || temp->m_NightJob == JOB_FACELIFT
						|| temp->m_DayJob == JOB_ASSJOB || temp->m_NightJob == JOB_ASSJOB
						|| temp->m_DayJob == JOB_TUBESTIED || temp->m_NightJob == JOB_TUBESTIED
						|| temp->m_DayJob == JOB_FERTILITY || temp->m_NightJob == JOB_FERTILITY)
					{
						if (temp->m_WorkingDay == 0) color = COLOR_DARKBLUE;
						else color = COLOR_RED;
					}
					else color = COLOR_BLUE;

					AddToListBox(listleft_id, i, temp->m_Realname, color);
					i++;
					temp = temp->m_Next;
				}
			}
			else if (leftBrothel == 0)
			{
				// add the girls to the list
				sGirl* temp = g_Studios.GetGirl(0, 0);
				int i = 0;
				while (temp)
				{
					if (temp->m_NightJob == JOB_DIRECTOR)					color = COLOR_RED;
					else if (temp->m_NightJob == JOB_FILMFREETIME)			color = COLOR_GREEN;
					else if (temp->m_NightJob == JOB_CAMERAMAGE)
					{
						if (g_Studios.GetNumGirlsOnJob(0, JOB_CAMERAMAGE, SHIFT_NIGHT) > 1) color = COLOR_DARKBLUE;
						else color = COLOR_RED;
					}
					else if (temp->m_NightJob == JOB_CRYSTALPURIFIER)
					{
						if (g_Studios.GetNumGirlsOnJob(0, JOB_CRYSTALPURIFIER, SHIFT_NIGHT) > 1) color = COLOR_DARKBLUE;
						else color = COLOR_RED;
					}
					else color = COLOR_BLUE;

					AddToListBox(listleft_id, i, temp->m_Realname, color);
					i++;
					temp = temp->m_Next;
				}
			}
		}
	}
	else if (g_InterfaceEvents.CheckEvent(EVENT_SELECTIONCHANGE, brothelright_id))
	{
		int color = COLOR_BLUE;
		ClearListBox(listright_id);
		rightBrothel = GetSelectedItemFromList(brothelright_id);
		if (rightBrothel != -1)
		{
			if (rightBrothel > 5){
				// add the girls to the list
				sGirl* temp = g_Brothels.GetGirl(rightBrothel - 6, 0);
				int i = 0;
				while (temp)
				{
					if (temp->m_DayJob == JOB_MATRON || temp->m_NightJob == JOB_MATRON)			color = COLOR_RED;
					else if (temp->m_DayJob == JOB_RESTING && temp->m_NightJob == JOB_RESTING)	color = COLOR_GREEN;
					else color = COLOR_BLUE;
					AddToListBox(listright_id, i, temp->m_Realname, color);
					i++;
					temp = temp->m_Next;
				}
			}
			else if (rightBrothel == 4)
			{
				// add the girls to the list
				sGirl* temp = g_Farm.GetGirl(0, 0);
				int i = 0;
				while (temp)
				{
					if (temp->m_DayJob == JOB_FARMMANGER || temp->m_NightJob == JOB_FARMMANGER)
						color = COLOR_RED;
					else if (temp->m_DayJob == JOB_FARMREST && temp->m_NightJob == JOB_FARMREST)
						color = COLOR_GREEN;
					else color = COLOR_BLUE;

					AddToListBox(listright_id, i, temp->m_Realname, color);
					i++;
					temp = temp->m_Next;
				}
			}
			else if (rightBrothel == 5)
			{
				// add the girls to the list
				sGirl* temp = g_House.GetGirl(0, 0);
				int i = 0;
				while (temp)
				{
					if (temp->m_DayJob == JOB_HEADGIRL || temp->m_NightJob == JOB_HEADGIRL)
						color = COLOR_RED;
					else if (temp->m_DayJob == JOB_HOUSEREST && temp->m_NightJob == JOB_HOUSEREST)
						color = COLOR_GREEN;
					else color = COLOR_BLUE;

					AddToListBox(listright_id, i, temp->m_Realname, color);
					i++;
					temp = temp->m_Next;
				}
			}
			else if (rightBrothel == 2)
			{
				// add the girls to the list
				sGirl* temp = g_Centre.GetGirl(0, 0);
				int i = 0;
				while (temp)
				{
					if (temp->m_DayJob == JOB_CENTREMANAGER || temp->m_NightJob == JOB_CENTREMANAGER)
						color = COLOR_RED;
					else if (temp->m_DayJob == JOB_COUNSELOR || temp->m_NightJob == JOB_COUNSELOR)
					{
						if (g_Centre.GetNumGirlsOnJob(0, JOB_REHAB, SHIFT_NIGHT) < 1) color = COLOR_DARKBLUE;
						else color = COLOR_RED;
					}
					else if (temp->m_DayJob == JOB_CENTREREST && temp->m_NightJob == JOB_CENTREREST)
						color = COLOR_GREEN;
					else if (temp->m_DayJob == JOB_REHAB || temp->m_NightJob == JOB_REHAB)
					{
						if (temp->m_WorkingDay == 0) color = COLOR_DARKBLUE;
						else color = COLOR_RED;
					}
					else color = COLOR_BLUE;

					AddToListBox(listright_id, i, temp->m_Realname, color);
					i++;
					temp = temp->m_Next;
				}
			}
			else if (rightBrothel == 1)
			{
				// add the girls to the list
				sGirl* temp = g_Arena.GetGirl(0, 0);
				int i = 0;
				while (temp)
				{
					if (temp->m_DayJob == JOB_DOCTORE || temp->m_NightJob == JOB_DOCTORE)
						color = COLOR_RED;
					else if (temp->m_DayJob == JOB_ARENAREST && temp->m_NightJob == JOB_ARENAREST)
						color = COLOR_GREEN;
					else color = COLOR_BLUE;
					AddToListBox(listright_id, i, temp->m_Realname, color);
					i++;
					temp = temp->m_Next;
				}
			}
			else if (rightBrothel == 3)
			{
				// add the girls to the list
				sGirl* temp = g_Clinic.GetGirl(0, 0);
				int i = 0;
				while (temp)
				{
					if (temp->m_DayJob == JOB_CHAIRMAN || temp->m_NightJob == JOB_CHAIRMAN)
						color = COLOR_RED;
					else if (temp->m_DayJob == JOB_CLINICREST && temp->m_NightJob == JOB_CLINICREST)
						color = COLOR_GREEN;
					else if (temp->m_DayJob == JOB_DOCTOR || temp->m_NightJob == JOB_DOCTOR)
					{
						if (g_Clinic.GetNumberPatients(0) < 1 && g_Clinic.GetNumberPatients(1) < 1)
							color = COLOR_DARKBLUE;
						else color = COLOR_RED;
					}
					else if (temp->m_DayJob == JOB_MECHANIC || temp->m_NightJob == JOB_MECHANIC)
					{
						if (g_Clinic.GetNumGirlsOnJob(0, JOB_GETREPAIRS, 0) < 1 && g_Clinic.GetNumGirlsOnJob(0, JOB_GETREPAIRS, 1) < 1)
							color = COLOR_DARKBLUE;
						else color = COLOR_RED;
					}
					else if (temp->m_DayJob == JOB_GETHEALING || temp->m_NightJob == JOB_GETHEALING
						|| temp->m_DayJob == JOB_GETREPAIRS || temp->m_NightJob == JOB_GETREPAIRS)
					{
						if (temp->health() > 70 && temp->tiredness() < 30) color = COLOR_DARKBLUE;
						else color = COLOR_RED;
					}
					else if (temp->m_DayJob == JOB_GETABORT || temp->m_NightJob == JOB_GETABORT
						|| temp->m_DayJob == JOB_COSMETICSURGERY || temp->m_NightJob == JOB_COSMETICSURGERY
						|| temp->m_DayJob == JOB_LIPO || temp->m_NightJob == JOB_LIPO
						|| temp->m_DayJob == JOB_BREASTREDUCTION || temp->m_NightJob == JOB_BREASTREDUCTION
						|| temp->m_DayJob == JOB_BOOBJOB || temp->m_NightJob == JOB_BOOBJOB
						|| temp->m_DayJob == JOB_VAGINAREJUV || temp->m_NightJob == JOB_VAGINAREJUV
						|| temp->m_DayJob == JOB_FACELIFT || temp->m_NightJob == JOB_FACELIFT
						|| temp->m_DayJob == JOB_ASSJOB || temp->m_NightJob == JOB_ASSJOB
						|| temp->m_DayJob == JOB_TUBESTIED || temp->m_NightJob == JOB_TUBESTIED
						|| temp->m_DayJob == JOB_FERTILITY || temp->m_NightJob == JOB_FERTILITY)
					{
						if (temp->m_WorkingDay == 0) color = COLOR_DARKBLUE;
						else color = COLOR_RED;
					}
					else color = COLOR_BLUE;

					AddToListBox(listright_id, i, temp->m_Realname, color);
					i++;
					temp = temp->m_Next;
				}
			}
			else if (rightBrothel == 0)
			{
				// add the girls to the list
				sGirl* temp = g_Studios.GetGirl(0, 0);
				int i = 0;
				while (temp)
				{

					if (temp->m_NightJob == JOB_DIRECTOR)					color = COLOR_RED;
					else if (temp->m_NightJob == JOB_FILMFREETIME)			color = COLOR_GREEN;
					else if (temp->m_NightJob == JOB_CAMERAMAGE)
					{
						if (g_Studios.GetNumGirlsOnJob(0, JOB_CAMERAMAGE, SHIFT_NIGHT) > 1) color = COLOR_DARKBLUE;
						else color = COLOR_RED;
					}
					else if (temp->m_NightJob == JOB_CRYSTALPURIFIER)
					{
						if (g_Studios.GetNumGirlsOnJob(0, JOB_CRYSTALPURIFIER, SHIFT_NIGHT) > 1) color = COLOR_DARKBLUE;
						else color = COLOR_RED;
					}
					else color = COLOR_BLUE;

					AddToListBox(listright_id, i, temp->m_Realname, color);
					i++;
					temp = temp->m_Next;
				}
			}
		}
	}
	return;
}
void cScreenTransfer::TransferGirlsLeftToRight(int rightBrothel, int leftBrothel)
{
	sBrothel* brothel = nullptr;
	/* */if (leftBrothel > 5)	{ brothel = g_Brothels.GetBrothel(leftBrothel - 6); }
	else if (leftBrothel == 5)	{ brothel = g_House.GetBrothel(0); }
	else if (leftBrothel == 4)	{ brothel = g_Farm.GetBrothel(0); }
	else if (leftBrothel == 3)	{ brothel = g_Clinic.GetBrothel(0); }
	else if (leftBrothel == 2)	{ brothel = g_Centre.GetBrothel(0); }
	else if (leftBrothel == 1)	{ brothel = g_Arena.GetBrothel(0); }
	else if (leftBrothel == 0)	{ brothel = g_Studios.GetBrothel(0); }

	if (brothel->m_NumGirls == brothel->m_NumRooms)
	{
		g_MessageQue.AddToQue("Left side building is full", 1);
	}
	else
	{
		int pos = 0;
		int NumRemoved = 0;
		int girlSelection = GetNextSelectedItemFromList(listright_id, 0, pos);
		while (girlSelection != -1)
		{
			sGirl* temp = nullptr;
			// get the girl
			/* */if (rightBrothel > 5)	{ temp = g_Brothels.GetGirl(rightBrothel - 6, girlSelection - NumRemoved); }
			else if (rightBrothel == 5)	{ temp = g_House.GetGirl(0, girlSelection - NumRemoved); }
			else if (rightBrothel == 4)	{ temp = g_Farm.GetGirl(0, girlSelection - NumRemoved); }
			else if (rightBrothel == 3)	{ temp = g_Clinic.GetGirl(0, girlSelection - NumRemoved); }
			else if (rightBrothel == 2)	{ temp = g_Centre.GetGirl(0, girlSelection - NumRemoved); }
			else if (rightBrothel == 1)	{ temp = g_Arena.GetGirl(0, girlSelection - NumRemoved); }
			else if (rightBrothel == 0)	{ temp = g_Studios.GetGirl(0, girlSelection - NumRemoved); }

			// check there is still room
			if (brothel->m_NumGirls + 1 > brothel->m_NumRooms)
			{
				g_MessageQue.AddToQue("Left side building is full", 1);
				break;
			}

			// remove girl from right side
			NumRemoved++;
			/* */if (rightBrothel > 5)	{ g_Brothels.RemoveGirl(rightBrothel - 6, temp, false); }
			else if (rightBrothel == 5)	{ g_House.RemoveGirl(0, temp, false); }
			else if (rightBrothel == 4)	{ g_Farm.RemoveGirl(0, temp, false); }
			else if (rightBrothel == 3)	{ g_Clinic.RemoveGirl(0, temp, false); }
			else if (rightBrothel == 2)	{ g_Centre.RemoveGirl(0, temp, false); }
			else if (rightBrothel == 1)	{ g_Arena.RemoveGirl(0, temp, false); }
			else if (rightBrothel == 0)	{ g_Studios.RemoveGirl(0, temp, false); }

			// add to left side
			/* */if (leftBrothel > 5)	{ g_Brothels.AddGirl(leftBrothel - 6, temp); }
			else if (leftBrothel == 5)	{ g_House.AddGirl(0, temp); }
			else if (leftBrothel == 4)	{ g_Farm.AddGirl(0, temp); }
			else if (leftBrothel == 3)	{ g_Clinic.AddGirl(0, temp); }
			else if (leftBrothel == 2)	{ g_Centre.AddGirl(0, temp); }
			else if (leftBrothel == 1)	{ g_Arena.AddGirl(0, temp); }
			else if (leftBrothel == 0)	{ g_Studios.AddGirl(0, temp); }

			// get next girl
			girlSelection = 
				
				GetNextSelectedItemFromList(listright_id, pos + 1, pos);
		}

		// update the girl lists
		SetSelectedItemInList(brothelleft_id, leftBrothel);
		SetSelectedItemInList(brothelright_id, rightBrothel);
	}
}
void cScreenTransfer::TransferGirlsRightToLeft(int rightBrothel, int leftBrothel)
{
	sBrothel* brothel = nullptr;
	/* */if (rightBrothel > 5)	{ brothel = g_Brothels.GetBrothel(rightBrothel - 6); }
	else if (rightBrothel == 5)	{ brothel = g_House.GetBrothel(0); }
	else if (rightBrothel == 4)	{ brothel = g_Farm.GetBrothel(0); }
	else if (rightBrothel == 3)	{ brothel = g_Clinic.GetBrothel(0); }
	else if (rightBrothel == 2)	{ brothel = g_Centre.GetBrothel(0); }
	else if (rightBrothel == 1)	{ brothel = g_Arena.GetBrothel(0); }
	else if (rightBrothel == 0)	{ brothel = g_Studios.GetBrothel(0); }

	if (brothel->m_NumGirls == brothel->m_NumRooms)
	{
		g_MessageQue.AddToQue("Right side building is full", 1);
	}
	else
	{
		int pos = 0;
		int NumRemoved = 0;
		int girlSelection = GetNextSelectedItemFromList(listleft_id, 0, pos);
		while (girlSelection != -1)
		{
			sGirl* temp = nullptr;
			/* */if (leftBrothel > 5)	{ temp = g_Brothels.GetGirl(leftBrothel - 6, girlSelection - NumRemoved); }
			else if (leftBrothel == 5)	{ temp = g_House.GetGirl(0, girlSelection - NumRemoved); }
			else if (leftBrothel == 4)	{ temp = g_Farm.GetGirl(0, girlSelection - NumRemoved); }
			else if (leftBrothel == 3)	{ temp = g_Clinic.GetGirl(0, girlSelection - NumRemoved); }
			else if (leftBrothel == 2)	{ temp = g_Centre.GetGirl(0, girlSelection - NumRemoved); }
			else if (leftBrothel == 1)	{ temp = g_Arena.GetGirl(0, girlSelection - NumRemoved); }
			else if (leftBrothel == 0)	{ temp = g_Studios.GetGirl(0, girlSelection - NumRemoved); }

			// check there is still room
			if (brothel->m_NumGirls + 1 > brothel->m_NumRooms)
			{
				g_MessageQue.AddToQue("Right side building is full", 1);
				break;
			}

			// remove girl from left side
			NumRemoved++;
			/* */if (leftBrothel > 5)	{ g_Brothels.RemoveGirl(leftBrothel - 6, temp, false); }
			else if (leftBrothel == 5)	{ g_House.RemoveGirl(0, temp, false); }
			else if (leftBrothel == 4)	{ g_Farm.RemoveGirl(0, temp, false); }
			else if (leftBrothel == 3)	{ g_Clinic.RemoveGirl(0, temp, false); }
			else if (leftBrothel == 2)	{ g_Centre.RemoveGirl(0, temp, false); }
			else if (leftBrothel == 1)	{ g_Arena.RemoveGirl(0, temp, false); }
			else if (leftBrothel == 0)	{ g_Studios.RemoveGirl(0, temp, false); }

			// add to right side
			/* */if (rightBrothel > 5)	{ g_Brothels.AddGirl(rightBrothel - 6, temp); }
			else if (rightBrothel == 5)	{ g_House.AddGirl(0, temp); }
			else if (rightBrothel == 4)	{ g_Farm.AddGirl(0, temp); }
			else if (rightBrothel == 3)	{ g_Clinic.AddGirl(0, temp); }
			else if (rightBrothel == 2)	{ g_Centre.AddGirl(0, temp); }
			else if (rightBrothel == 1)	{ g_Arena.AddGirl(0, temp); }
			else if (rightBrothel == 0)	{ g_Studios.AddGirl(0, temp); }

			// get next girl
			girlSelection = GetNextSelectedItemFromList(listleft_id, pos + 1, pos);
		}

		// update the girl lists
		SetSelectedItemInList(brothelleft_id, leftBrothel);
		SetSelectedItemInList(brothelright_id, rightBrothel);
	}
}
