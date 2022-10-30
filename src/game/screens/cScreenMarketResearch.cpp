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

#include "cScreenMarketResearch.h"
#include "interface/cWindowManager.h"
#include "widgets/IListBox.h"
#include "cTariff.h"
#include "cJobManager.h"
#include <sstream>
#include "IGame.h"
#include "buildings/studio/manager.h"

// defined in cScreenMovieMaker
FormattedCellData get_prediction_detail(const sTargetGroup& group, const cMovieManager::sRevenueEstimate& rating, const std::string& name);

namespace {
    FormattedCellData get_movie_detail(const Movie& movie, const std::string& name)
    {
        if(name == "Name") {
            return mk_text(movie.Name);
        } else if(name == "Price") {
            return mk_num(movie.TicketPrice);
        } else if(name == "Age") {
            return mk_num(movie.Age);
        } else if(name == "EarningsLastWeek") {
            return mk_num(movie.EarningsLastWeek);
        }  else if(name == "Estimate") {
            return mk_num(g_Game->movie_manager().estimate_revenue(movie));
        } else if(name == "TotalEarnings") {
            return mk_num(movie.TotalEarnings);
        } else if(name == "Cost") {
            return mk_num(movie.TotalCost);
        }else if(name == "Hype") {
            return mk_num(100*movie.Hype);
        }
        assert(false);
    }
}

cScreenMarketResearch::cScreenMarketResearch() : cGameWindow("movie_market_screen.xml")
{
}

void cScreenMarketResearch::set_ids()
{
    targetgroup_list_id = get_id("GroupsList");
    running_movies_id   = get_id("MoviesList");
    incticket_id        = get_id("IncreaseTicketPrice");
    decticket_id        = get_id("DecreaseTicketPrice");
    predict_list_id     = get_id("PredictionList");
    ad_cmp_btn_id       = get_id("AdCampaign");
    ad_points_txt_id    = get_id("Promo_txt");
    survey_btn_id       = get_id("Survey");
    survey_txt_id       = get_id("Survey_txt");

    SetListBoxSelectionCallback(running_movies_id, [this](int selection) { on_select_running_movie(selection); });

    SetButtonCallback(incticket_id, [this](){
        int sel = GetSelectedItemFromList(running_movies_id);
        if(sel == -1)  return;
        auto& movie = g_Game->movie_manager().get_movies().at(sel);
        if(movie.TicketPrice < 15) {
            g_Game->movie_manager().set_ticket_price(sel, movie.TicketPrice + 1);
        } else if(movie.TicketPrice < 30) {
            g_Game->movie_manager().set_ticket_price(sel, movie.TicketPrice + 2);
        } else{
            g_Game->movie_manager().set_ticket_price(sel, movie.TicketPrice + 5);
        }
        SetSelectedItemColumnText(running_movies_id, sel, std::to_string(movie.TicketPrice), "Price");
        SetSelectedItemColumnText(running_movies_id, sel, std::to_string(g_Game->movie_manager().estimate_revenue(movie)), "Estimate");
        on_select_running_movie(sel);
    });

    SetButtonCallback(ad_cmp_btn_id, [this](){
        run_ad_campaign();
    });

    SetButtonCallback(survey_btn_id, [this](){
        run_survey();
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
        SetSelectedItemColumnText(running_movies_id, sel, std::to_string(movie.TicketPrice), "Price");
        SetSelectedItemColumnText(running_movies_id, sel, std::to_string(g_Game->movie_manager().estimate_revenue(movie)), "Estimate");
        on_select_running_movie(sel);
    });

    SetListBoxSelectionCallback(targetgroup_list_id, [&](int sel) {
        update_survey_button();
    });
}

void cScreenMarketResearch::init(bool back)
{
    Focused();

    ClearListBox(targetgroup_list_id);
    ClearListBox(running_movies_id);

    auto& audiences = g_Game->movie_manager().get_audience();

    auto group_col_names = GetListBox(targetgroup_list_id)->GetColumnNames();
    std::vector<std::string> group_col_data(group_col_names.size());

    // Add scene to list
    int row = 0;
    for(auto& group: audiences) {
        unsigned int item_color = COLOR_NEUTRAL;
        GetListBox(targetgroup_list_id)->AddRow(row, [&](const std::string& col){ return group.get_formatted(col, false); }, item_color);
        row++;
    }

    // Add movies to list
    row = 0;
    for(auto& movie: g_Game->movie_manager().get_movies()) {
        unsigned int item_color = COLOR_NEUTRAL;
        GetListBox(running_movies_id)->AddRow(row, [&](const std::string& col){ return get_movie_detail(movie, col); }, item_color);
        ++row;
    }

    DisableWidget(incticket_id, GetSelectedItemFromList(running_movies_id) == -1);
    DisableWidget(decticket_id, GetSelectedItemFromList(running_movies_id) == -1);
    update_ad_button();
    update_survey_button();
}


void cScreenMarketResearch::on_select_running_movie(int selection) {
    DisableWidget(incticket_id, selection == -1);
    DisableWidget(decticket_id, selection == -1);
    ClearListBox(predict_list_id);
    if(selection != -1) {
        const auto& movie = g_Game->movie_manager().get_movies().at(selection);
        for(auto& aud : g_Game->movie_manager().get_audience()) {
            auto rating = g_Game->movie_manager().estimate_revenue(aud, movie);
            GetListBox(predict_list_id)->AddRow(0, [&](const std::string& col){ return get_prediction_detail(aud, rating, col); }, 0);
        }
    }

    update_ad_button();
}

void cScreenMarketResearch::run_ad_campaign() {
    if(g_Game->movie_manager().promotion_points() > 0) {
        auto mid = GetSelectedItemFromList(running_movies_id);
        if(mid >= 0) {
            g_Game->movie_manager().run_ad_campaign(mid);
            init(false);
        }
    }
}

void cScreenMarketResearch::update_ad_button() {
    EditTextItem("Points: " + std::to_string(g_Game->movie_manager().promotion_points()), ad_points_txt_id);
    if(GetSelectedItemFromList(running_movies_id) != -1) {
        DisableWidget(ad_cmp_btn_id, g_Game->movie_manager().promotion_points() <= 0);
    } else {
        DisableWidget(ad_cmp_btn_id, true);
    }
}

void cScreenMarketResearch::update_survey_button() {
    EditTextItem("Points: " + std::to_string(g_Game->movie_manager().market_research_points()), survey_txt_id);
    if(GetSelectedItemFromList(targetgroup_list_id) != -1) {
        DisableWidget(survey_btn_id, g_Game->movie_manager().market_research_points() <= 0);
    } else {
        DisableWidget(survey_btn_id, true);
    }
}

void cScreenMarketResearch::run_survey() {
    if(g_Game->movie_manager().market_research_points() > 0) {
        auto mid = GetSelectedItemFromList(targetgroup_list_id);
        if(mid >= 0) {
            g_Game->movie_manager().make_survey(mid);
            init(false);
        }
    }
}

