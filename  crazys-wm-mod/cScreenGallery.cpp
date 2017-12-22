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
#include <iostream>
#include <locale>
#include <sstream>
#include "cBrothel.h"
#include "cClinic.h"
#include "cMovieStudio.h"
#include "cArena.h"
#include "cCentre.h"
#include "cFarm.h"
#include "cHouse.h"
#include "cScreenGallery.h"
#include "cWindowManager.h"
#include "cGold.h"
#include "sFacilityList.h"
#include "cGetStringScreenManager.h"
#include "InterfaceProcesses.h"
#include "cScriptManager.h"
#include "cGangs.h"
#include "libintl.h"
#include "FileList.h"

extern	bool					g_InitWin;
extern	int						g_CurrBrothel;
extern	int						g_CurrClinic;
extern	int						g_CurrStudio;
extern	int						g_CurrArena;
extern	int						g_CurrCentre;
extern	int						g_CurrFarm;
extern	int						g_CurrHouse;
extern	int						g_CurrentScreen;
extern	cGold					g_Gold;
extern	cBrothelManager			g_Brothels;
extern	cClinicManager			g_Clinic;
extern	cMovieStudioManager		g_Studios;
extern	cArenaManager			g_Arena;
extern	cCentreManager			g_Centre;
extern	cFarmManager			g_Farm;
extern	cHouseManager			g_House;
extern	cWindowManager			g_WinManager;
extern	cInterfaceEventManager	g_InterfaceEvents;
//extern	cInterfaceWindow		g_Gallery;
extern	bool					g_Cheats;
extern	bool					eventrunning;
extern	bool					g_AllTogle;
extern	sInterfaceIDs			g_interfaceid;

extern	string					pic_types[];
extern	string					galtxt[];
extern	void					GetString();
extern	cInterfaceWindow		g_GetString;
extern	sGirl*					selected_girl;


bool cScreenGallery::ids_set = false;

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
extern	string	numeric;

static int Mode = 0;
static int Img = 0;	// what image currently drawing
static int numimages[NUM_IMGTYPES][4];
static bool changeimage = false;

cScreenGallery::cScreenGallery()
{
	DirPath dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "gallery_screen.xml";
	m_filename = dp.c_str();
}
cScreenGallery::~cScreenGallery() {}

void cScreenGallery::set_ids()
{
	ids_set			/**/ = true;
	g_LogFile.write("set_ids in cScreenGallery");

	back_id			/**/ = get_id("BackButton", "Back");
	prev_id			/**/ = get_id("PrevButton","Prev");
	next_id			/**/ = get_id("NextButton","Next");
	image_id		/**/ = get_id("GirlImage");
	imagename_id	/**/ = get_id("ImageName");
	imagelist_id	/**/ = get_id("ImageList");

	string ILColumns[] = { "ILName", "ILTotal", "ILjpg", "ILAni", "ILGif" };
	SortColumns(imagelist_id, ILColumns, 5);
}

void cScreenGallery::process()
{
	if (selected_girl == 0)
	{
		g_InitWin = true;
		g_MessageQue.AddToQue("ERROR: No girl selected", 1);
		g_WinManager.Pop();
		return;
	}
	if (!ids_set) set_ids();		// we need to make sure the ID variables are set
	init();							// set up the window if needed
	check_events();					// check to see if there's a button event needing handling

	// Draw the image
	if (selected_girl && changeimage)
	{
		changeimage = false;
		PrepareImage(image_id, selected_girl, Mode, false, Img, true);
		string t = m_Images[image_id]->m_Image->m_Message;
		if (t == "") t = m_Images[image_id]->m_Image->GetFilename();
		m_TextItems[imagename_id]->SetText(t);
		SetSelectedItemInList(imagelist_id, Mode, false);
	}
}
void cScreenGallery::init()
{
	g_CurrentScreen = SCREEN_GALLERY;
	if (!g_InitWin) return;
	Focused();
	g_InitWin = false;
	ClearListBox(imagelist_id);

	int usefolder = 0;	// 0=none, 1=cfg, 2=original
	int startmode = -1;
	// start with what the config has set
	DirPath imagedir = DirPath(cfg.folders.characters().c_str()) << selected_girl->m_Name;
	FileList testall(imagedir, "*.*");
	if (testall.size() > 0)		usefolder = 1;
	else		// if config is not found, check for images in the original folder
	{
		imagedir = DirPath() << "Resources" << "Characters" << selected_girl->m_Name;
		FileList testall(imagedir, "*.*");
		if (testall.size() > 0) usefolder = 2;
	}
	if (usefolder == 0)
	{
		g_InitWin = true;
		g_MessageQue.AddToQue("ERROR: " + selected_girl->m_Realname + " ( " + selected_girl->m_Name + " ) has no images.", 1);
		g_WinManager.Pop();
		return;
	}
	FileList readall(imagedir, "*.*");
	int totalnum = readall.size();

	for (int i = 0; i < NUM_IMGTYPES; i++)
	{
		if (i == IMGTYPE_PREGNANT)
		{
			string ext[3] = { "*g", "ani", "gif" };
			for (u_int e = 0; e < 3; e++)
			{
				string t = ("preg." + ext[e]);
				FileList testmode(imagedir, t.c_str());
				for (u_int j = 0; j < numeric.size(); j++)
				{
					t = ("preg" + numeric.substr(j, 1) + "*." + ext[e]);
					testmode.add(t.c_str());
				}
				numimages[i][e + 1] = testmode.size();
			}
		}
		else
		{
			FileList testmode1(imagedir, (pic_types[i] + "*g").c_str());	numimages[i][1] = testmode1.size();
			FileList testmode2(imagedir, (pic_types[i] + "ani").c_str());	numimages[i][2] = testmode2.size();
			FileList testmode3(imagedir, (pic_types[i] + "gif").c_str());	numimages[i][3] = testmode3.size();
		}
		numimages[i][0] = numimages[i][1] + numimages[i][2] + numimages[i][3];
		if (numimages[i][0] > 0)
		{
			if (startmode == -1) startmode = i;
			stringstream num0;	num0 << numimages[i][0];
			stringstream num1;	num1 << numimages[i][1];
			stringstream num2;	num2 << numimages[i][2];
			stringstream num3;	num3 << numimages[i][3];

			string dataP[] = { galtxt[i], num0.str(), num1.str(), num2.str(), num3.str() };
			AddToListBox(imagelist_id, i, dataP, 5);
		}
	}
	Mode = startmode;
	Img = 0;
	changeimage = true;
}

void cScreenGallery::check_events()
{
	if (g_LeftArrow || g_A_Key || g_RightArrow || g_D_Key || g_W_Key || g_UpArrow || g_S_Key || g_DownArrow){}
	else if (g_InterfaceEvents.GetNumEvents() == 0)	return;		// no events means we can go home
	// otherwise, compare event IDs 

	if (g_InterfaceEvents.CheckButton(back_id))		// if it's the back button, pop the window off the stack and we're done
	{
		g_InitWin = true;
		g_WinManager.Pop();
		return;
	}
	if (g_InterfaceEvents.CheckButton(prev_id) || g_LeftArrow || g_A_Key)
	{
		g_A_Key = g_LeftArrow = false;
		Img--;
		if (Img < 0) Img = numimages[Mode][0] - 1;
		changeimage = true;
		return;
	}
	if (g_InterfaceEvents.CheckButton(next_id) || g_RightArrow || g_D_Key)
	{
		g_D_Key = g_RightArrow = false;
		Img++;
		if (Img == numimages[Mode][0]) Img = 0;
		changeimage = true;
		return;
	}
	if (g_W_Key || g_UpArrow || g_S_Key || g_DownArrow)
	{
		bool up = (g_W_Key || g_UpArrow);
		g_UpArrow = g_W_Key = g_DownArrow = g_S_Key = false;
		while (true)
		{
			Mode += (up ? -1 : 1);
			if (Mode < 0) Mode = NUM_IMGTYPES - 1;
			if (Mode >= NUM_IMGTYPES) Mode = 0;
			Img = 0;
			if (numimages[Mode][0] > 0)
			{
				changeimage = true;
				return;
			}
		}
	}
	if (Mode == NUM_IMGTYPES)
	{
		//we've gone through all categories and could not find a single image!
		return;
	}
	if (g_InterfaceEvents.CheckListbox(imagelist_id))
	{
		Mode = GetSelectedItemFromList(imagelist_id);
		if (Img > numimages[Mode][0]) Img = 0;
		changeimage = true;
		return;
	}
}