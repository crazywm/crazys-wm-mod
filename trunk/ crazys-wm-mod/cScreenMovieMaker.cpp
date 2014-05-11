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
#include <algorithm>
#include "cMovieStudio.h"
#include "cScreenMovieMaker.h"
#include "cWindowManager.h"
#include "cGold.h"
#include "cTariff.h"
#include "cJobManager.h"
#include "InterfaceProcesses.h"
#include "libintl.h"

extern bool g_InitWin;
extern int g_CurrStudio;
extern cGold g_Gold;
extern cMovieStudioManager g_Studios;
extern cWindowManager g_WinManager;

extern	bool	g_LeftArrow;
extern	bool	g_RightArrow;
extern	bool	g_UpArrow;
extern	bool	g_DownArrow;
extern	int		g_CurrentScreen;

static cTariff tariff;
static stringstream ss;

static int lastNum = -1;
static int ImageNum = -1;
static bool FireGirl = false;
static bool FreeGirl = false;
static bool SellGirl = false;
static int selection = -1;
static int DayNight = 0;	// 1 is night, 0 is day.
static bool SetJob = false;

extern sGirl *selected_girl;
extern vector<int> cycle_girls;
extern int cycle_pos;


bool cScreenMovieMaker::ids_set = false;

void cScreenMovieMaker::set_ids()
{
	ids_set = true;
	curstudio_id = get_id("Studio");
	back_id = get_id("BackButton");
	sceneslist_id = get_id("ScenesList");
	releasemovie_id = get_id("ReleaseMovieButton");
}

void cScreenMovieMaker::init()
{
	g_CurrentScreen = SCREEN_CREATEMOVIE;
	if(!g_InitWin)
		return;

	Focused();
	g_InitWin = false;
	
	// clear the lists
	ClearListBox(sceneslist_id);

	//get a list of all the column names, so we can find which data goes in that column
	vector<string> columnNames;
	m_ListBoxes[sceneslist_id]->GetColumnNames(columnNames);
	int numColumns = columnNames.size();
	string* Data = new string[numColumns];

	// Add scene to list
	for(int i=0; i<g_Studios.GetNumScenes(); i++)
	{
		sMovieScene* scene = g_Studios.GetScene(i);
		//if (selected_girl == gir)
		//	selection = i;
		unsigned int item_color = LISTBOX_BLUE;

		scene->OutputSceneRow(Data, columnNames);
		AddToListBox(sceneslist_id, i, Data, numColumns, item_color);
	}
	delete [] Data;
	
	lastNum = -1;
	g_InitWin = false;
}

void cScreenMovieMaker::process()
{
	// we need to make sure the ID variables are set
	if(!ids_set)
		set_ids();

	// handle arrow keys
 	if(check_keys())
		return;

	// set up the window if needed
	init();

	// check to see if there's a button event needing handling
	check_events();
}


bool cScreenMovieMaker::check_keys()
{
	return false;
}


void cScreenMovieMaker::check_events()
{
	// no events means we can go home
	if(g_InterfaceEvents.GetNumEvents() == 0)
		return;
	// if it's the back button, pop the window off the stack and we're done
	if(g_InterfaceEvents.CheckButton(back_id)) {
		g_InitWin = true;
		g_WinManager.Pop();
		return;
	}
	// if it's the back button, pop the window off the stack and we're done
	if(g_InterfaceEvents.CheckButton(releasemovie_id)) {
		g_InitWin = true;
		g_WinManager.Pop();
		g_Studios.ReleaseCurrentMovie();
		return;
	}
}



