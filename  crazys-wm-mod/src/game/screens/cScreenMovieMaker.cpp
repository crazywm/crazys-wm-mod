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
#include <algorithm>
#include "buildings/cMovieStudio.h"
#include "cScreenMovieMaker.h"
#include "interface/cWindowManager.h"
#include "widgets/cListBox.h"
#include "cTariff.h"
#include "cJobManager.h"
#include <sstream>
#include "Game.hpp"

namespace settings {
    extern const char* USER_MOVIES_AUTO;
}

extern vector<int> cycle_girls;
extern int cycle_pos;

static stringstream ss;

static int ImageNum = -1;
static int selection = -1;
static bool Day0Night1 = SHIFT_DAY;	// 1 is night, 0 is day.
int cRow = -1;
int cRowM = -1;

cScreenMovieMaker::cScreenMovieMaker() : cGameWindow("movie_maker_screen.xml")
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
        g_Game->settings().set_value(settings::USER_MOVIES_AUTO, on);
    });
}

void cScreenMovieMaker::init(bool back)
{
	Focused();

	ClearListBox(sceneslist_id);	// clear the lists
	ClearListBox(makethismovie_id);

	//get a list of all the column names, so we can find which data goes in that column
	auto& columns = GetListBox(sceneslist_id)->GetColumnData();
	std::vector<std::string> scene_col_names(columns.size());
	std::vector<std::string> scene_col_data(columns.size());
	for(int i = 0; i < columns.size(); ++i) {
        scene_col_names[i] = columns[i].name;
	}
	int row = 0;

    auto& columnsm = GetListBox(sceneslist_id)->GetColumnData();
    std::vector<std::string> movie_col_names(columns.size());
    std::vector<std::string> movie_col_data(columns.size());
    for(int i = 0; i < columnsm.size(); ++i) {
        movie_col_names[i] = columnsm[i].name;
    }
	int rowm = 0;

	auto& brothel = dynamic_cast<sMovieStudio&>(active_building());

	// Add scene to list
	for(int i = 0; i < brothel.GetNumScenes(); i++)
	{
		sMovieScene* scene = brothel.GetScene(i);
		unsigned int item_color = COLOR_BLUE;
		scene->m_Row = row;
		scene->m_RowM = -1;
		scene->OutputSceneRow(scene_col_data, scene_col_names);
		AddToListBox(sceneslist_id, row, std::move(scene_col_data), item_color);
		row++;
	}
	for (int i = 0; i < brothel.GetNumMovieScenes(); i++)
	{
		sMovieScene* scene = brothel.GetMovieScene(i);
		unsigned int item_color = COLOR_BLUE;
		scene->m_Row = -1;
		scene->m_RowM = rowm;
		scene->OutputSceneRow(movie_col_data, movie_col_names);
		AddToListBox(makethismovie_id, rowm, std::move(movie_col_data), item_color);
		rowm++;
	}

	if (cRow < 0 && GetListBox(sceneslist_id)->GetSize()>0) { cRow = 0; }
	else if (cRow  > GetListBox(sceneslist_id)->GetSize()) { cRow = GetListBox(sceneslist_id)->GetSize(); }
	if (cRowM < 0 && GetListBox(makethismovie_id)->GetSize()>0) { cRowM = 0; }
	else if (cRowM > GetListBox(makethismovie_id)->GetSize()) { cRowM = GetListBox(makethismovie_id)->GetSize(); }
	SetSelectedItemInList(sceneslist_id, cRow);
	SetSelectedItemInList(makethismovie_id, cRowM);
    DisableWidget(addscene_id, cRow == -1);
    DisableWidget(removescene_id, cRowM == -1);
    DisableWidget(scrapscene_id, cRow == -1);
    DisableWidget(moveup_id, cRowM <= 0);
    DisableWidget(movedown_id, (cRowM == -1 || cRowM >= GetListBox(makethismovie_id)->GetSize() - 1));
    DisableWidget(releasemovie_id, GetListBox(makethismovie_id)->GetSize() < 1);

	stringstream movietext;
	if (GetListBox(makethismovie_id)->GetSize() > 0)
	{
		movietext << brothel.BuildDirectorList() << "\n\n";
		movietext << brothel.BuildCastList() << "\n\n";
		movietext << brothel.BuildCrewList();
	}
	EditTextItem(movietext.str(), moviedetails_id);
	SetCheckBox(autocreatemovies_id, g_Game->settings().get_bool(settings::USER_MOVIES_AUTO));
}

void cScreenMovieMaker::on_select_movie_scene(int selection)
{
    DisableWidget(removescene_id, ::selection == -1);
    DisableWidget(moveup_id, ::selection <= 0);
    DisableWidget(movedown_id, ::selection == -1 || ::selection >= GetListBox(makethismovie_id)->GetSize() - 1);
    stringstream scenedetails;
    if (::selection != -1)
    {

    }
    EditTextItem(scenedetails.str(), scenedetails_id);
}

void cScreenMovieMaker::on_select_source_scene(int selection)
{
    DisableWidget(addscene_id, ::selection == -1);
    DisableWidget(scrapscene_id, ::selection == -1);

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
	if (selected_girl()/*&& !IsMultiSelected(girllist_id)*/)
	{
		PrepareImage(girlimage_id, selected_girl(), IMGTYPE_PROFILE, true, ImageNum);
        HideWidget(girlimage_id, false);
	}
	else
	{
        HideWidget(girlimage_id, true);
	}
}

