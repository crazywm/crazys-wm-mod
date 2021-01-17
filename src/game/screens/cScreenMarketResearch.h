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
#pragma once

#include "cGameWindow.h"
#include <set>

class sMovieStudio;

class cScreenMarketResearch : public cGameWindow
{
private:
    int targetgroup_list_id;

    int running_movies_id;
    int predict_list_id;
    int incticket_id;
    int decticket_id;
    int ad_cmp_btn_id;
    int ad_points_txt_id;
    int survey_btn_id;
    int survey_txt_id;

    void set_ids() override;
    void on_select_running_movie(int selection);

    void run_ad_campaign();
    void update_ad_button();
    void run_survey();
    void update_survey_button();
public:
    cScreenMarketResearch();

    void init(bool back) override;
    void process() override { };
};
