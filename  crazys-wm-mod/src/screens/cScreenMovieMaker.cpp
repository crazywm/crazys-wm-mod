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
#include "buildings/cMovieStudio.h"
#include "cScreenMovieMaker.h"
#include "cWindowManager.h"
#include "widgets/cListBox.h"
#include "widgets/cCheckBox.h"
#include "cTariff.h"
#include "cJobManager.h"

extern sGirl *g_selected_girl;
extern vector<int> cycle_girls;
extern int cycle_pos;

extern	bool	g_UpArrow;
extern	bool	g_DownArrow;
extern	bool	g_W_Key;
extern	bool	g_S_Key;

static cTariff tariff;
static stringstream ss;

static int ImageNum = -1;
static int selection = -1;
static bool Day0Night1 = SHIFT_DAY;	// 1 is night, 0 is day.
int cRow = -1;
int cRowM = -1;

cScreenMovieMaker::cScreenMovieMaker() : cInterfaceWindowXML("movie_maker_screen.xml")
{
}

void cScreenMovieMaker::set_ids()
{
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

	SetButtonNavigation(back_id, "<back>");

    SetButtonCallback(moveup_id, [this]() { movie_scene_up(); });
    SetButtonHotKey(moveup_id, SDLK_e);
    SetButtonCallback(movedown_id, [this]() { movie_scene_down(); });
    SetButtonHotKey(moveup_id, SDLK_d);
    SetButtonCallback(addscene_id, [this]() { movie_add_scene(); });
    SetButtonHotKey(addscene_id, SDLK_r);
    SetButtonCallback(removescene_id, [this]() { movie_remove_scene(); });
    SetButtonHotKey(removescene_id, SDLK_f);

    SetButtonCallback(releasemovie_id, [this]() {
        getStudio().ReleaseCurrentMovie();
        init(false);
	});
    SetButtonCallback(scrapscene_id, [this]() {
        selection = GetSelectedItemFromList(sceneslist_id);
        cRow = getStudio().DeleteScene(selection);
        init(false);
	});
    SetListBoxSelectionCallback(sceneslist_id, [this](int selection) { on_select_source_scene(selection); });
    SetListBoxHotKeys(sceneslist_id, SDLK_q, SDLK_a);
    SetListBoxSelectionCallback(makethismovie_id, [this](int selection) { on_select_movie_scene(selection); });
    SetListBoxHotKeys(makethismovie_id, SDLK_w, SDLK_s);

    SetCheckBoxCallback(autocreatemovies_id, [this](bool on) {
        cfg.initial.autocreatemovies() = on;
    });
}

void cScreenMovieMaker::init(bool back)
{
	Focused();

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

	auto& brothel = dynamic_cast<sMovieStudio&>(active_building());

	// Add scene to list
	for(int i = 0; i < brothel.GetNumScenes(); i++)
	{
		sMovieScene* scene = brothel.GetScene(i);
		unsigned int item_color = COLOR_BLUE;
		scene->m_Row = row;
		scene->m_RowM = -1;
		scene->OutputSceneRow(Data, columnNames);
		AddToListBox(sceneslist_id, row, Data, numColumns, item_color);
		row++;
	}
	for (int i = 0; i < brothel.GetNumMovieScenes(); i++)
	{
		sMovieScene* scene = brothel.GetMovieScene(i);
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
		movietext << brothel.BuildDirectorList() << "\n\n";
		movietext << brothel.BuildCastList() << "\n\n";
		movietext << brothel.BuildCrewList();
	}
	EditTextItem(movietext.str(), moviedetails_id);
	m_CheckBoxes[autocreatemovies_id]->SetState(cfg.initial.autocreatemovies());




	delete[] Data;
	delete[] DataM;
}

void cScreenMovieMaker::on_select_movie_scene(int selection)
{
    DisableButton(removescene_id, ::selection == -1);
    DisableButton(moveup_id, ::selection <= 0);
    DisableButton(movedown_id, ::selection == -1 || ::selection >= m_ListBoxes[makethismovie_id]->GetSize() - 1);
    stringstream scenedetails;
    if (::selection != -1)
    {

    }
    EditTextItem(scenedetails.str(), scenedetails_id);
}

void cScreenMovieMaker::on_select_source_scene(int selection)
{
    DisableButton(addscene_id, ::selection == -1);
    DisableButton(scrapscene_id, ::selection == -1);

    stringstream scenedetails;
    if (::selection != -1)
    {

    }
    EditTextItem(scenedetails.str(), scenedetails_id);
}

void cScreenMovieMaker::movie_scene_up()
{
    selection = GetSelectedItemFromList(makethismovie_id);
    cRowM     = getStudio().MovieSceneUp(selection);
    init(false);
}

void cScreenMovieMaker::movie_scene_down()
{
    selection = GetSelectedItemFromList(makethismovie_id);
    cRowM = getStudio().MovieSceneDown(selection);
    init(false);
}

void cScreenMovieMaker::movie_add_scene()
{
    selection = GetSelectedItemFromList(sceneslist_id);
    vector<int> a = getStudio().AddSceneToMovie(selection);
    cRow = a[0];
    cRowM = a[1];
    init(false);
}

void cScreenMovieMaker::movie_remove_scene(){
    selection = GetSelectedItemFromList(makethismovie_id);
    vector<int> a = getStudio().RemoveSceneFromMovie(selection);
    cRow = a[0];
    cRowM = a[1];
    init(false);
}


sMovieStudio& cScreenMovieMaker::getStudio() const
{ return dynamic_cast<sMovieStudio&>(active_building()); }

void cScreenMovieMaker::update_image()
{
	if ((g_selected_girl)) //&& !IsMultiSelected(girllist_id))
	{
		PrepareImage(girlimage_id, g_selected_girl, IMGTYPE_PROFILE, true, ImageNum);
		HideImage(girlimage_id, false);
	}
	else
	{
		HideImage(girlimage_id, true);
	}
}

