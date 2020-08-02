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
#include "cScreenPropertyManagement.h"
#include "interface/cWindowManager.h"
#include "cGold.h"
#include "cTariff.h"
#include "buildings/cBuildingManager.h"
#include "Game.hpp"
#include <sstream>


static std::stringstream ss;

cScreenPropertyManagement::cScreenPropertyManagement() : cGameWindow("property_management.xml")
{
}

void cScreenPropertyManagement::set_ids()
{
    back_id = get_id("BackButton", "Back");
    barstaff_id     = get_id("BarStaffCost");  // not used anymore?
    barhire_id      = get_id("BarHireButton");  // not used anymore?
    barfire_id      = get_id("BarFireButton");  // not used anymore?
    casinostaff_id  = get_id("CasinoStaffCost");  // not used anymore?
    casinohire_id   = get_id("CasinoHireButton");  // not used anymore?
    casinofire_id   = get_id("CasinoFireButton");  // not used anymore?
    roomcost_id     = get_id("RoomAddCost");
    restrict_id     = get_id("SexRestrictions");

    load_brothel_ui_ids("P_B0", BuildingType::BROTHEL, 0, p_b0);
    load_brothel_ui_ids("P_B1", BuildingType::BROTHEL, 1, p_b1);
    load_brothel_ui_ids("P_B2", BuildingType::BROTHEL, 2, p_b2);
    load_brothel_ui_ids("P_B3", BuildingType::BROTHEL, 3, p_b3);
    load_brothel_ui_ids("P_B4", BuildingType::BROTHEL, 4, p_b4);
    load_brothel_ui_ids("P_B5", BuildingType::BROTHEL, 5, p_b5);
    load_brothel_ui_ids("P_B6", BuildingType::BROTHEL, 6, p_b6);
    load_brothel_ui_ids("P_St", BuildingType::STUDIO, 0, p_st);
    load_brothel_ui_ids("P_Ar", BuildingType::ARENA, 0, p_ar);
    load_brothel_ui_ids("P_Ce", BuildingType::CENTRE, 0, p_ce);
    load_brothel_ui_ids("P_Cl", BuildingType::CLINIC, 0, p_cl);
    load_brothel_ui_ids("P_Ho", BuildingType::HOUSE, 0, p_ho);
    load_brothel_ui_ids("P_Fa", BuildingType::FARM, 0, p_fa);

    curbrothel_id    = get_id("CurrentBrothel");
    buyrooms_id        = get_id("BuildRoomsButton");
    potioncost_id    = get_id("PotionCost");
    potionavail_id    = get_id("AvailablePotions");
    potions10_id    = get_id("10PotionsButton");
    potions20_id    = get_id("20PotionsButton");
    autopotions_id    = get_id("AutoBuyPotionToggle");
    noanal_id        = get_id("ProhibitAnalToggleRes");
    nobdsm_id        = get_id("ProhibitBDSMToggleRes");
    nonormal_id        = get_id("ProhibitNormalToggleRes");
    nobeast_id        = get_id("ProhibitBeastToggleRes");
    nogroup_id        = get_id("ProhibitGroupToggleRes");
    nolesbian_id    = get_id("ProhibitLesbianToggleRes");
    nooral_id        = get_id("ProhibitOralToggleRes");
    notitty_id        = get_id("ProhibitTittyToggleRes");
    nohand_id        = get_id("ProhibitHandToggleRes");
    advertsli_id    = get_id("AdvertisingSlider");
    advertamt_id    = get_id("AdvertisingValue");

    SetButtonNavigation(back_id, "<back>");
}

namespace {
    struct STCB {
        int id;
        SKILLS skill;
    };

    auto sex_type_mapping(const BrothelUiIDs& target) {
        std::array<STCB, 11> stbc{
                STCB{target.noanal,  SKILL_ANAL},    {target.nobdsm,  SKILL_BDSM},
                {target.nobeast, SKILL_BEASTIALITY}, {target.nofoot,  SKILL_FOOTJOB},
                {target.nogroup, SKILL_GROUP},       {target.nohand,  SKILL_HANDJOB},
                {target.noles,   SKILL_LESBIAN},     {target.nonorm,  SKILL_NORMALSEX},
                {target.nooral,  SKILL_ORALSEX},     {target.nostrip, SKILL_STRIP},
                {target.notitty, SKILL_TITTYSEX}};
        return stbc;
    }
}

void cScreenPropertyManagement::load_brothel_ui_ids(const std::string& prefix, BuildingType type, int index,
                                                    BrothelUiIDs& target)
{
    target.type    = type;
    target.index   = index;
    target.name    = get_id(prefix + "_Name");
    target.addroom = get_id(prefix + "_AddRoom");
    target.pot_c   = get_id(prefix + "_Pot_C");
    target.pot_a   = get_id(prefix + "_Pot_A");
    target.pot10   = get_id(prefix + "_Pot10");
    target.pot20   = get_id(prefix + "_Pot20");
    target.pot_b   = get_id(prefix + "_Pot_B");
    target.noanal  = get_id(prefix + "_NoAnal");
    target.nobdsm  = get_id(prefix + "_NoBDSM");
    target.nonorm  = get_id(prefix + "_NoNorm");
    target.nobeast = get_id(prefix + "_NoBeast");
    target.noles    = get_id(prefix + "_NoLes");
    target.nooral   = get_id(prefix + "_NoOral");
    target.notitty  = get_id(prefix + "_NoTitty");
    target.nohand   = get_id(prefix + "_NoHandJob");
    target.nofoot   = get_id(prefix + "_NoFoot");
    target.nostrip  = get_id(prefix + "_NoStrip");
    target.nogroup  = get_id(prefix + "_NoGroup");
    target.advslid  = get_id(prefix + "_AdvSlid");
    target.advamnt  = get_id(prefix + "_AdvAmnt");

    SetCheckBoxCallback(target.pot_b, [this, type, index](bool on) {
        g_Game->buildings().building_with_type(type, index)->m_KeepPotionsStocked = on;
    });

    for(const auto& data : sex_type_mapping(target)) {
        SetCheckBoxCallback(data.id, [this, skill=data.skill, type, index](bool on){
            g_Game->buildings().building_with_type(type, index)->set_sex_type_allowed(skill, !on);
        });
    }

    SetSliderCallback(target.advslid, [this, advamnt=target.advamnt, type, index](int value) {
        g_Game->buildings().building_with_type(type, index)->m_AdvertisingBudget = value * 50;
        ss.str(""); ss << ("Advertising Budget: ") << (value * 50) << (" gold / week");
        EditTextItem(ss.str(), advamnt);
    });
}

void cScreenPropertyManagement::init_building_ui(const BrothelUiIDs& ui)
{
    auto building = g_Game->buildings().building_with_type(ui.type, ui.index);
    for(const auto& data : sex_type_mapping(ui)) {
        HideWidget(data.id, !building);
        if(building) {
            SetCheckBox(data.id, !building->is_sex_type_allowed(data.skill));
        }
    }

    HideWidget(ui.advslid, !building);
    HideWidget(ui.advamnt, !building);
    HideWidget(ui.pot_b, !building);
    HideWidget(ui.pot_a, !building);

    if(!building) {
        EditTextItem("", ui.name);
        return;
    }
    EditTextItem(building->name(), ui.name);

    int advert = building->m_AdvertisingBudget / 50;
    advert = SliderRange(ui.advslid, 0, (2000 / 50), advert, 4);  // set slider min/max range
    ss.str(""); ss << "Promotion Budget: " << (advert * 50) << (" gold / week");
    EditTextItem(ss.str(), ui.advamnt);

    SetCheckBox(ui.pot_b, building->m_KeepPotionsStocked);
    ss.str("");
    int number = building->m_AntiPregPotions;
    ss << ("You have: ") << number;
    EditTextItem(ss.str(), ui.pot_a);
    DisableWidget(ui.pot_b, number < 1);
}

void cScreenPropertyManagement::init(bool back)
{
    Focused();

    ss.str(""); ss << "Add Rooms: " << g_Game->tariff().add_room_cost(5) << " gold";
    EditTextItem(ss.str(), roomcost_id);

    ss.str(""); ss << "Anti-Preg Potions: " << g_Game->tariff().anti_preg_price(1) << " gold each";
    EditTextItem(ss.str(), potioncost_id);

    BrothelUiIDs* uiids[] = {&p_b0, &p_b1, &p_b2, &p_b3, &p_b4, &p_b5, &p_b6, &p_ar, &p_fa, &p_cl, &p_ce, &p_st, &p_ho};
    for(auto & uiid : uiids) {
        init_building_ui(*uiid);
    }
}
