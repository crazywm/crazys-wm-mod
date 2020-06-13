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
#include "Constants.h"

struct BrothelUiIDs {
    int name;
    int addroom;
    int pot_c;
    int pot_a;
    int pot10;
    int pot20;
    int pot_b;
    int noanal;
    int nobdsm;
    int nonorm;
    int nobeast;
    int nogroup;
    int noles;
    int nooral;
    int notitty;
    int nohand;
    int nofoot;
    int nostrip;
    int advslid;
    int advamnt;

    BuildingType type;
    int index;
};


class cScreenPropertyManagement : public cGameWindow
{
private:
    int back_id;            // Back button
    int barstaff_id;        // Bar Staff Cost text
    int barhire_id;            // Hire Bar Staff button
    int barfire_id;            // Fire Bar Staff button
    int casinostaff_id;        // Casino Staff Cost text
    int casinohire_id;        // Hire Casino Staff button
    int casinofire_id;        // Fire Casino Staff button
    int roomcost_id;        // Room Cost text
    int restrict_id;        // Sex Restrictions text

    int curbrothel_id;        // Current Brothel text
    int buyrooms_id;        // Add 5 Rooms button
    int potioncost_id;        // Potion Cost text
    int potionavail_id;        // Available Potions text
    int potions10_id;        // Buy 10 Potions button
    int potions20_id;        // Buy 20 Potions button
    int autopotions_id;        // Auto Buy Potions checkbox
    int noanal_id;            // Prohibit Anal checkbox
    int nobdsm_id;            // Prohibit BDSM checkbox
    int nonormal_id;        // Prohibit Normal Sex checkbox
    int nobeast_id;            // Prohibit Bestiality checkbox
    int nogroup_id;            // Prohibit Group Sex checkbox
    int nolesbian_id;        // Prohibit Lesbian checkbox
    int nooral_id;            // Prohibit Oral checkbox
    int notitty_id;            // Prohibit Titty Sex checkbox
    int nohand_id;            // Prohibit Hand Job checkbox
    int advertsli_id;        // Advertising Budget slider
    int advertamt_id;        // Advertising Budget value output text

    // `J` individual building ids
    BrothelUiIDs p_b0;
    BrothelUiIDs p_b1;
    BrothelUiIDs p_b2;
    BrothelUiIDs p_b3;
    BrothelUiIDs p_b4;
    BrothelUiIDs p_b5;
    BrothelUiIDs p_b6;
    BrothelUiIDs p_st;
    BrothelUiIDs p_ar;
    BrothelUiIDs p_ce;
    BrothelUiIDs p_cl;
    BrothelUiIDs p_ho;
    BrothelUiIDs p_fa;

    void set_ids() override;

    void load_brothel_ui_ids(const std::string& prefix, BuildingType type, int index,
                             BrothelUiIDs& target);
    void init_building_ui(const BrothelUiIDs& ui);
public:
    cScreenPropertyManagement();

    void init(bool back) override;
    void process() override { };
};
