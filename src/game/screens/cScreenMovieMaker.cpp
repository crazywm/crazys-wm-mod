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
#include "cScreenMovieMaker.h"
#include "interface/cWindowManager.h"
#include "widgets/cListBox.h"
#include "cTariff.h"
#include "cJobManager.h"
#include <sstream>
#include "Game.hpp"
#include "movies/manager.h"

namespace settings {
    extern const char* USER_MOVIES_AUTO;
}

static std::stringstream ss;

static int ImageNum = -1;

cScreenMovieMaker::cScreenMovieMaker() : cGameWindow("movie_maker_screen.xml")
{
}

void cScreenMovieMaker::set_ids()
{
    autocreatemovies_id = get_id("AutoCreateMovies");
    sceneslist_id       = get_id("ScenesList");
    makethismovie_id    = get_id("MakeThisMovie");
    running_movies_id   = get_id("MoviesList");
    releasemovie_id     = get_id("ReleaseMovieButton");
    girlimage_id        = get_id("GirlImage");
    scrapscene_id       = get_id("ScrapScene");
    addscene_id         = get_id("AddScene");
    removescene_id      = get_id("RemoveScene");
    moviedetails_id     = get_id("MovieDetails");
    incticket_id        = get_id("IncreaseTicketPrice");
    decticket_id        = get_id("DecreaseTicketPrice");

    SetButtonCallback(addscene_id, [this]() { movie_add_scene(); });
    SetButtonHotKey(addscene_id, SDLK_r);
    SetButtonCallback(removescene_id, [this]() { movie_remove_scene(); });
    SetButtonHotKey(removescene_id, SDLK_f);

    SetButtonCallback(releasemovie_id, [this]() {
        std::vector<const MovieScene*> active_scenes;
        if(m_ScenesInMovie.empty())
            return;

        for(auto& index : m_ScenesInMovie) {
            active_scenes.push_back(&g_Game->movie_manager().get_scenes().at(index));
        }
        m_ScenesInMovie.clear();
        auto& new_movie = g_Game->movie_manager().create_movie(active_scenes,
                                                               g_Game->movie_manager().auto_create_name(active_scenes));
        init(false);
    });

    SetButtonCallback(scrapscene_id, [this]() {
        int selection = GetSelectedItemFromList(sceneslist_id);
        if(selection == -1) {
            DisableWidget(scrapscene_id);
            return;
        }

        g_Game->movie_manager().delete_scene(&g_Game->movie_manager().get_scenes().at(selection));
        // adjust indices
        std::set<int> updated;
        for(int index : m_ScenesInMovie) {
            if(index > selection) {
                updated.insert(index - 1);
            } else {
                updated.insert(index);
            }
        }
        m_ScenesInMovie = std::move(updated);
        init(false);
    });

    SetListBoxSelectionCallback(sceneslist_id, [this](int selection) { on_select_source_scene(selection); });
    SetListBoxHotKeys(sceneslist_id, SDLK_q, SDLK_a);
    SetListBoxSelectionCallback(makethismovie_id, [this](int selection) { on_select_movie_scene(selection); });
    SetListBoxHotKeys(makethismovie_id, SDLK_w, SDLK_s);
    SetListBoxSelectionCallback(running_movies_id, [this](int selection) { on_select_running_movie(selection); });

    SetCheckBoxCallback(autocreatemovies_id, [this](bool on) {
        g_Game->settings().set_value(settings::USER_MOVIES_AUTO, on);
    });

    SetButtonCallback(incticket_id, [this](){
        int sel = GetSelectedItemFromList(running_movies_id);
        if(sel == -1)  return;
        auto& movie = g_Game->movie_manager().get_movies().at(sel);
        if(movie.TicketPrice < 10) {
            g_Game->movie_manager().set_ticket_price(sel, movie.TicketPrice + 1);
        } else if(movie.TicketPrice < 20) {
            g_Game->movie_manager().set_ticket_price(sel, movie.TicketPrice + 2);
        } else{
            g_Game->movie_manager().set_ticket_price(sel, movie.TicketPrice + 5);
        }
        init(false);
    });
    SetButtonCallback(decticket_id, [this](){
        int sel = GetSelectedItemFromList(running_movies_id);
        if(sel == -1)  return;
        auto& movie = g_Game->movie_manager().get_movies().at(sel);
        if(movie.TicketPrice > 20) {
            g_Game->movie_manager().set_ticket_price(sel, movie.TicketPrice - 5);
        } else if(movie.TicketPrice > 10) {
            g_Game->movie_manager().set_ticket_price(sel, movie.TicketPrice - 2);
        } else if(movie.TicketPrice > 1) {
            g_Game->movie_manager().set_ticket_price(sel, movie.TicketPrice - 1);
        }
        init(false);
    });
}

namespace {
    void OutputSceneRow(const MovieScene& scene, std::vector<std::string>& Data, const std::vector<std::string>& columnNames)
    {
        Data.resize(columnNames.size());
        for (unsigned int x = 0; x < columnNames.size(); ++x)
        {
            const std::string& name = columnNames[x];
            if(name == "Actress") {
                Data[x] = scene.Actress;
            } else if(name == "Director") {
                Data[x] = scene.Director;
            } else if(name == "Camera Mage") {
                Data[x] = scene.CameraMage;
            } else if(name == "Crystal Purifier") {
                Data[x] = scene.CrystalPurifier;
            } else if(name == "Type") {
                Data[x] = get_name(scene.Type);
            } else if(name == "ContentQuality") {
                Data[x] = std::to_string(scene.ContentQuality);
            } else if(name == "TechnicalQuality") {
                Data[x] = std::to_string(scene.TechnicalQuality);
            }
        }
    }
    void OutputSceneRow(const Movie& movie, std::vector<std::string>& Data, const std::vector<std::string>& columnNames)
    {
        Data.resize(columnNames.size());
        for (unsigned int x = 0; x < columnNames.size(); ++x)
        {
            const std::string& name = columnNames[x];
            if(name == "Name") {
                Data[x] = movie.Name;
            } else if(name == "Price") {
                Data[x] = std::to_string(movie.TicketPrice);
            } else if(name == "Age") {
                Data[x] = std::to_string(movie.Age);
            } else if(name == "EarningsLastWeek") {
                Data[x] = std::to_string(movie.EarningsLastWeek);
            } else if(name == "TotalEarnings") {
                Data[x] = std::to_string(movie.TotalEarnings);
            }else if(name == "Cost") {
                Data[x] = std::to_string(movie.TotalCost);
            }
        }
    }

}

void cScreenMovieMaker::init(bool back)
{
    Focused();

    ClearListBox(sceneslist_id);    // clear the lists
    ClearListBox(makethismovie_id);
    ClearListBox(running_movies_id);

    //get a list of all the column names, so we can find which data goes in that column
    auto& columns = GetListBox(sceneslist_id)->GetColumnData();
    std::vector<std::string> scene_col_names(columns.size());
    std::vector<std::string> scene_col_data(columns.size());
    for(int i = 0; i < columns.size(); ++i) {
        scene_col_names[i] = columns[i].name;
    }

    auto& columnsm = GetListBox(makethismovie_id)->GetColumnData();
    std::vector<std::string> movie_col_names(columnsm.size());
    std::vector<std::string> movie_col_data(columnsm.size());
    for(int i = 0; i < columnsm.size(); ++i) {
        movie_col_names[i] = columnsm[i].name;
    }

    auto& scenes = g_Game->movie_manager().get_scenes();

    // Add scene to list
    int row = 0;
    for(auto& scene: scenes) {
        unsigned int item_color = COLOR_BLUE;
        if(m_ScenesInMovie.count(row) > 0) {
            OutputSceneRow(scene, movie_col_data, movie_col_names);
            AddToListBox(makethismovie_id, row, std::move(movie_col_data), item_color);
        } else {
            OutputSceneRow(scene, scene_col_data, scene_col_names);
            AddToListBox(sceneslist_id, row, std::move(scene_col_data), item_color);
        }
        row++;
    }

    DisableWidget(scrapscene_id, GetSelectedItemFromList(sceneslist_id) == -1);
    DisableWidget(addscene_id, GetSelectedItemFromList(sceneslist_id) == -1 || m_ScenesInMovie.size() >= 5);
    DisableWidget(removescene_id, GetSelectedItemFromList(makethismovie_id) == -1);
    std::stringstream movietext;
    if (GetListBox(makethismovie_id)->GetSize() > 0)
    {
        std::set<std::string> directors;
        std::set<std::string> cast;
        std::set<std::string> crew;
        for(auto& index : m_ScenesInMovie) {
            directors.insert( g_Game->movie_manager().get_scenes().at(index).Director );
            cast.insert( g_Game->movie_manager().get_scenes().at(index).Actress );
            crew.insert( g_Game->movie_manager().get_scenes().at(index).CrystalPurifier );
            crew.insert( g_Game->movie_manager().get_scenes().at(index).CameraMage );
        }
        movietext << "Director" << (directors.size() == 1 ? ": \t" : "s: \t");
        for(auto& d : directors) {
            movietext << d << ", ";
        }
        movietext << "\n";
        movietext << "Cast" << (cast.size() == 1 ? ": \t" : "s: \t");
        for(auto& d : cast) {
            movietext << d << ", ";
        }
        movietext << "\n";
        movietext << "Crew: \t";
        for(auto& d : crew) {
            movietext << d << ", ";
        }
        movietext << "\n";

        // a test screening
        Movie fake_movie;
        for(auto& index : m_ScenesInMovie) {
            fake_movie.Scenes.push_back(g_Game->movie_manager().get_scenes().at(index));
        }
        float sum_score = 0;
        float max_score = 0;
        float sum_aud = 0;
        for(auto& aud : g_Game->movie_manager().get_audience()) {
            auto rating = g_Game->movie_manager().rate_movie_for_audience(aud, fake_movie);
            sum_score += rating.Score * aud.Amount * rating.NoTurnOff;
            sum_aud += aud.Amount;
            if(rating.NoTurnOff > 0.5f) {
                max_score = std::max(max_score, rating.Score);
            }
        }
        float avg_score = sum_score / sum_aud;
        movietext << "Avg. Score: \t" << int(avg_score) << "\n";
        movietext << "Best Score: \t" << int(max_score) << "\n";
        int ticket =  g_Game->movie_manager().auto_detect_ticket_price(fake_movie);
        movietext << "Ticket: \t" << ticket << "\n";
        fake_movie.TicketPrice = ticket;
        movietext << "Revenue: \t" << g_Game->movie_manager().estimate_revenue(fake_movie) << "\n";
    }
    EditTextItem(movietext.str(), moviedetails_id, true);
    SetCheckBox(autocreatemovies_id, g_Game->settings().get_bool(settings::USER_MOVIES_AUTO));

    // Add movies to list
    auto& columns_movie = GetListBox(running_movies_id)->GetColumnData();
    std::vector<std::string> rmovie_col_names(columns_movie.size());
    std::vector<std::string> rmovie_col_data(columns_movie.size());
    for(int i = 0; i < columns_movie.size(); ++i) {
        rmovie_col_names[i] = columns_movie[i].name;
    }

    row = 0;
    for(auto& movie: g_Game->movie_manager().get_movies()) {
        unsigned int item_color = COLOR_BLUE;
        OutputSceneRow(movie, rmovie_col_data, rmovie_col_names);
        AddToListBox(running_movies_id, row, std::move(rmovie_col_data), item_color);
        ++row;
    }
}

void cScreenMovieMaker::on_select_movie_scene(int selection)
{
    DisableWidget(removescene_id, selection == -1);
}

void cScreenMovieMaker::on_select_source_scene(int selection)
{
    DisableWidget(addscene_id, selection == -1 || m_ScenesInMovie.size() >= 5);
    DisableWidget(scrapscene_id, selection == -1);
}

void cScreenMovieMaker::on_select_running_movie(int selection) {
    DisableWidget(incticket_id, selection == -1);
    DisableWidget(decticket_id, selection == -1);
    if(selection != -1) {
        const auto& movie = g_Game->movie_manager().get_movies().at(selection);
        std::stringstream movietext;
        int total_expected = 0;
        for(auto& a : g_Game->movie_manager().get_audience()) {
            auto rating = g_Game->movie_manager().rate_movie_for_audience(a, movie);
            float factor = rating.QualityFactor * rating.PriceFactor * rating.HypeFactor * rating.NoTurnOff;
            int viewers = static_cast<int>((a.Amount - a.Saturation) * factor);
            movietext << a.Name << " [" << a.Amount << ", " << int(100 * a.Saturation / a.Amount) << "%]:\t " << (int)rating.Score << " / " << (int)a.RequiredScore;
            movietext << "  \t" << int(rating.NoTurnOff * rating.HypeFactor / 0.05f * 100) << "% "<< int(rating.PriceFactor * 100) << "% " << int(rating.QualityFactor * 100) << "% \t-> ";
            movietext << viewers << " (" << viewers * movie.TicketPrice << ")";
            if(viewers > 0) {
                movietext << "\t -- " << int(100 * rating.Satisfaction) << "\n";
            } else {
                movietext << "\t\n";
            }
            total_expected += viewers * movie.TicketPrice;
        }
        movietext << " => " << total_expected << "\n";
        movietext << int(100 * movie.Hype) << "\n";
        EditTextItem(movietext.str(), moviedetails_id, true);
    }
}

void cScreenMovieMaker::movie_add_scene() {
    ForAllSelectedItems(sceneslist_id, [this](int selection) {
        m_ScenesInMovie.insert(selection);
    });
    init(false);
}

void cScreenMovieMaker::movie_remove_scene() {
    ForAllSelectedItems(sceneslist_id, [this](int selection) {
        m_ScenesInMovie.erase(selection);
    });
    init(false);
}

void cScreenMovieMaker::update_image()
{
    if (selected_girl()/*&& !IsMultiSelected(girllist_id)*/)
    {
        PrepareImage(girlimage_id, selected_girl().get(), IMGTYPE_PROFILE, true, ImageNum);
        HideWidget(girlimage_id, false);
    }
    else
    {
        HideWidget(girlimage_id, true);
    }
}

