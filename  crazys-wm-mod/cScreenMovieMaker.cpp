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
extern sGirl *selected_girl;
extern vector<int> cycle_girls;
extern int cycle_pos;

extern	bool	g_LeftArrow;
extern	bool	g_RightArrow;
extern	bool	g_UpArrow;
extern	bool	g_DownArrow;
extern	int		g_CurrentScreen;
extern	bool	g_Q_Key;
extern	bool	g_A_Key;
extern	bool	g_W_Key;
extern	bool	g_S_Key;
extern	bool	g_E_Key;
extern	bool	g_D_Key;
extern	bool	g_R_Key;
extern	bool	g_F_Key;

static cTariff tariff;
static stringstream ss;

static int lastNum = -1;
static int ImageNum = -1;
static bool FireGirl = false;
static bool FreeGirl = false;
static bool SellGirl = false;
static int selection = -1;
static bool Day0Night1 = SHIFT_DAY;	// 1 is night, 0 is day.
static bool SetJob = false;
int cRow = -1;
int cRowM = -1;

bool cScreenMovieMaker::ids_set = false;
cScreenMovieMaker::cScreenMovieMaker()
{
	DirPath dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "movie_maker_screen.xml";
	m_filename = dp.c_str();
}
cScreenMovieMaker::~cScreenMovieMaker() {}

void cScreenMovieMaker::set_ids()
{
	ids_set			/**/ = true;
	g_LogFile.write("set_ids in cScreenMovieMaker");

	curstudio_id		/**/ = get_id("Studio");
	back_id				/**/ = get_id("BackButton");
	autocreatemovies_id	/**/ = get_id("AutoCreateMovies");
	sceneslist_id		/**/ = get_id("ScenesList");
	makethismovie_id	/**/ = get_id("MakeThisMovie");
	releasemovie_id		/**/ = get_id("ReleaseMovieButton");
	girlimage_id		/**/ = get_id("GirlImage");
	scrapscene_id		/**/ = get_id("ScrapScene");
	moveup_id			/**/ = get_id("MoveUp");
	movedown_id			/**/ = get_id("MoveDown");
	addscene_id			/**/ = get_id("AddScene");
	removescene_id		/**/ = get_id("RemoveScene");
	scenedetails_id		/**/ = get_id("SceneDetails");
	moviedetails_id		/**/ = get_id("MovieDetails");

	SortColumns(sceneslist_id, m_ListBoxes[sceneslist_id]->m_ColumnName, m_ListBoxes[sceneslist_id]->m_ColumnCount);
	SortColumns(makethismovie_id, m_ListBoxes[makethismovie_id]->m_ColumnName, m_ListBoxes[makethismovie_id]->m_ColumnCount);

}

void cScreenMovieMaker::init()
{
	g_CurrentScreen = SCREEN_CREATEMOVIE;
	if(!g_InitWin) return;
	Focused();
	g_InitWin = false;
	
	ClearListBox(sceneslist_id);	// clear the lists
	ClearListBox(makethismovie_id);

	//get a list of all the column names, so we can find which data goes in that column
	vector<string> columnNames;
	m_ListBoxes[sceneslist_id]->GetColumnNames(columnNames);
	int numColumns = columnNames.size();
	string* Data = new string[numColumns];
	int row = 0;

	vector<string> columnNamesM;
	m_ListBoxes[makethismovie_id]->GetColumnNames(columnNamesM);
	int numColumnsM = columnNamesM.size();
	string* DataM = new string[numColumns];
	int rowm = 0;


	// Add scene to list
	for(int i = 0; i < g_Studios.GetNumScenes(); i++)
	{
		sMovieScene* scene = g_Studios.GetScene(i);
		unsigned int item_color = COLOR_BLUE;
		scene->m_Row = row;
		scene->m_RowM = -1;
		scene->OutputSceneRow(Data, columnNames);
		AddToListBox(sceneslist_id, row, Data, numColumns, item_color);
		row++;
	}
	for (int i = 0; i < g_Studios.GetNumMovieScenes(); i++)
	{
		sMovieScene* scene = g_Studios.GetMovieScene(i);
		unsigned int item_color = COLOR_BLUE;
		scene->m_Row = -1;
		scene->m_RowM = rowm;
		scene->OutputSceneRow(DataM, columnNamesM);
		AddToListBox(makethismovie_id, rowm, DataM, numColumnsM, item_color);
		rowm++;
	}

	if (cRow < 0 && m_ListBoxes[sceneslist_id]->GetSize()>0) { cRow = 0; }
	else if (cRow  > m_ListBoxes[sceneslist_id]->GetSize()) { cRow = m_ListBoxes[sceneslist_id]->GetSize(); }
	if (cRowM < 0 && m_ListBoxes[makethismovie_id]->GetSize()>0) { cRowM = 0; }
	else if (cRowM > m_ListBoxes[makethismovie_id]->GetSize()) { cRowM = m_ListBoxes[makethismovie_id]->GetSize(); }
	SetSelectedItemInList(sceneslist_id, cRow);
	SetSelectedItemInList(makethismovie_id, cRowM);
	DisableButton(addscene_id, cRow == -1);
	DisableButton(removescene_id, cRowM == -1);
	DisableButton(scrapscene_id, cRow == -1);
	DisableButton(moveup_id, cRowM <= 0);
	DisableButton(movedown_id, (cRowM == -1 || cRowM >= m_ListBoxes[makethismovie_id]->GetSize() - 1));
	DisableButton(releasemovie_id, m_ListBoxes[makethismovie_id]->GetSize() < 1);

	stringstream movietext;
	if (m_ListBoxes[makethismovie_id]->GetSize() > 0)
	{
		movietext << g_Studios.BuildDirectorList() << "\n\n";
		movietext << g_Studios.BuildCastList() << "\n\n";
		movietext << g_Studios.BuildCrewList();
	}
	EditTextItem(movietext.str(), moviedetails_id);
	m_CheckBoxes[autocreatemovies_id]->SetState(cfg.initial.autocreatemovies());




	delete[] Data;
	delete[] DataM;

	lastNum = -1;
	g_InitWin = false;
}

void cScreenMovieMaker::process()
{
	if(!ids_set) set_ids();		// we need to make sure the ID variables are set
 	if(check_keys()) return;	// handle arrow keys
	init();						// set up the window if needed
	check_events();				// check to see if there's a button event needing handling
}

bool cScreenMovieMaker::check_keys()
{
	/*
					msg += "Create Movie:\n";
					msg += "Q     Available Scene Select Up\n";
					msg += "A     Available Scene Select Down\n";
					msg += "W     Current Movie Select Up\n";
					msg += "S     Current Movie Select Down\n";
					msg += "E     Current Movie Move Up \n";
					msg += "D     Current Movie Move Down\n";
					msg += "R     Add Scene\n";
					msg += "F     Remove Scene\n";
					msg += "\n";

	*/
	if (g_Q_Key)
	{
		cRow = ArrowUpListBox(sceneslist_id);
		g_Q_Key = false;
		return true;
	}
	if (g_A_Key)
	{
		cRow = ArrowDownListBox(sceneslist_id);
		g_A_Key = false;
		return true;
	}
	if (g_W_Key)
	{
		cRowM = ArrowUpListBox(makethismovie_id);
		g_W_Key = false;
		return true;
	}
	if (g_S_Key)
	{
		cRowM = ArrowDownListBox(makethismovie_id);
		g_S_Key = false;
		return true;
	}
	if (g_E_Key)
	{
		selection = GetSelectedItemFromList(makethismovie_id);
		cRowM = g_Studios.MovieSceneUp(selection);
		g_InitWin = true;
		g_E_Key = false;
		return true;
	}
	if (g_D_Key)
	{
		selection = GetSelectedItemFromList(makethismovie_id);
		cRowM = g_Studios.MovieSceneDown(selection);
		g_InitWin = true;
		g_D_Key = false;
		return true;
	}
	if (g_R_Key)
	{
		selection = GetSelectedItemFromList(sceneslist_id);
		vector<int> a = g_Studios.AddSceneToMovie(selection);
		cRow = a[0];
		cRowM = a[1];
		g_InitWin = true;
		g_R_Key = false;
		return true;
	}
	if (g_F_Key)
	{
		selection = GetSelectedItemFromList(makethismovie_id);
		vector<int> a = g_Studios.RemoveSceneFromMovie(selection);
		cRow = a[0];
		cRowM = a[1];
		g_InitWin = true;
		g_F_Key = false;
		return true;
	}
	return false;
}

void cScreenMovieMaker::check_events()
{
	if(g_InterfaceEvents.GetNumEvents() == 0)	return;	// no events means we can go home
	if(g_InterfaceEvents.CheckButton(back_id))			// if it's the back button, pop the window off the stack and we're done
	{
		g_InitWin = true;
		g_WinManager.Pop();
		return;
	}
	if (g_InterfaceEvents.CheckButton(releasemovie_id))	// if it's the back button, pop the window off the stack and we're done
	{
		g_InitWin = true;
		g_Studios.ReleaseCurrentMovie();
		return;
	}
	else if (g_InterfaceEvents.CheckCheckbox(autocreatemovies_id))
	{
		cfg.initial.autocreatemovies() = IsCheckboxOn(autocreatemovies_id);
			
			
	}
	else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, scrapscene_id))
	{
		selection = GetSelectedItemFromList(sceneslist_id);
		cRow = g_Studios.DeleteScene(selection);
		g_InitWin = true;
	}
	else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, moveup_id))
	{
		selection = GetSelectedItemFromList(makethismovie_id);
		cRowM = g_Studios.MovieSceneUp(selection);
		g_InitWin = true;
	}
	else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, movedown_id))
	{
		selection = GetSelectedItemFromList(makethismovie_id);
		cRowM = g_Studios.MovieSceneDown(selection);
		g_InitWin = true;
	}
	else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, addscene_id))
	{
		selection = GetSelectedItemFromList(sceneslist_id);
		vector<int> a = g_Studios.AddSceneToMovie(selection);
		cRow = a[0];
		cRowM = a[1];
		g_InitWin = true;
	}
	else if (g_InterfaceEvents.CheckEvent(EVENT_BUTTONCLICKED, removescene_id))
	{
		selection = GetSelectedItemFromList(makethismovie_id);
		vector<int> a = g_Studios.RemoveSceneFromMovie(selection);
		cRow = a[0];
		cRowM = a[1];
		g_InitWin = true;
	}
	if (g_InterfaceEvents.CheckListbox(sceneslist_id))
	{
		selection = GetSelectedItemFromList(sceneslist_id);
		DisableButton(addscene_id, selection == -1);
		DisableButton(scrapscene_id, selection == -1);

		stringstream scenedetails;
		if (selection != -1)
		{

		}
		EditTextItem(scenedetails.str(), scenedetails_id);
	}
	if (g_InterfaceEvents.CheckListbox(makethismovie_id))
	{
		selection = GetSelectedItemFromList(makethismovie_id);
		DisableButton(removescene_id, selection == -1);
		DisableButton(moveup_id, selection <= 0);
		DisableButton(movedown_id, selection == -1 || selection >= m_ListBoxes[makethismovie_id]->GetSize() - 1);
		stringstream scenedetails;
		if (selection != -1)
		{

		}
		EditTextItem(scenedetails.str(), scenedetails_id);
	}

}

void cScreenMovieMaker::update_image()
{
	if ((selected_girl)) //&& !IsMultiSelected(girllist_id))
	{
		PrepareImage(girlimage_id, selected_girl, IMGTYPE_PROFILE, true, ImageNum);
		HideImage(girlimage_id, false);
	}
	else
	{
		HideImage(girlimage_id, true);
	}
}

