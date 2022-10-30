/*
* Copyright 2022, The Pink Petal Development Team.
* The Pink Petal Development Team are defined as the game's coders
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
#pragma once

#include "cGameWindow.h"

class cScreenInfoTraits : public cGameWindow
{
public:
    cScreenInfoTraits();

    void init(bool back) override;
    void process() override { };

private:
    void set_ids() override;

    void load_effects(int trait_id);
    void show_property_explanation(int property_id);

    int trait_list_id = -1;
    int effects_list_id = -1;
    int description_id = -1;
    int exclusion_list_id = -1;
    int property_list_id = -1;
    int property_expl_id = -1;
    int help_text_id = -1;


    std::vector<std::string> m_SortedTraits;
    std::vector<std::string> m_SortedProperties;
/*


int running_movies_id;
int predict_list_id;
int incticket_id;
int decticket_id;
int ad_cmp_btn_id;
int ad_points_txt_id;
int survey_btn_id;
int survey_txt_id;

void on_select_running_movie(int selection);

void run_ad_campaign();
void update_ad_button();
void run_survey();
void update_survey_button();
 */
};
