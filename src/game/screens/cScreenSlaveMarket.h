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
#include "text/repo.h"

class cScreenSlaveMarket : public cGameWindow
{
private:
    int more_id;            // More Details button
    int buy_slave_id;        // Buy button
    int cur_brothel_id;        // Current Brothel text
    int slave_list_id;        // Slave listbox
    int trait_list_id;        // Trait listbox
    int trait_list_text_id;    // Trait list text
    int details_id;            // Girl Details text
    int girl_desc_id;        // Girl desc text
    int trait_id;            // Trait text
    int image_id;            // Image of selected slave
    int header_id;            // page header text ("Slave Market")
    int slave_market_id;    // "Slave Market" text
    int gold_id;            // Player's Gold

    int releaseto_id;        // Where to release girls to Text
    int roomsfree_id;        // how many rooms are available
    int dungeon_id;            // Dungeon's button

    int m_SelectedGirl;
    int DetailLevel;
    int ImageNum;
    int sel_pos;
    void set_ids() override;

    IBuilding* m_TargetBuilding;

    struct RelBtnData {
        int id;
        BuildingType type;
        int index;
    };

    std::vector<RelBtnData> m_ReleaseButtons;
    std::unique_ptr<ITextRepository> m_TextRepo;
public:
    cScreenSlaveMarket();

    void init(bool back) override;
    void process() override;

    bool buy_slaves();            // `J` used when buying slaves
    void update_release_text();
    void preparescreenitems(sGirl* girl);

    int &image_num() { return ImageNum; }
    int &selected_item() { return m_SelectedGirl; }
    bool change_selected_girl(int selected);
    std::string get_buy_slave_string(sGirl* girl);

    void affect_girl_by_disposition(sGirl& girl) const;
    void affect_dungeon_girl_by_disposition(sGirl& girl) const;

    void on_select_trait(int selection);
};
