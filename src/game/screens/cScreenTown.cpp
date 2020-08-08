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
#include "buildings/cBuildingManager.h"
#include "cScreenTown.h"
#include "interface/cWindowManager.h"
#include "scripting/cScriptManager.h"
#include <iostream>
#include "cGangs.h"
#include "utils/FileList.h"
#include "cObjectiveManager.hpp"
#include "Game.hpp"
#include "sConfig.h"
#include <sstream>
#include "cGirls.h"

extern bool                        g_WalkAround;
extern bool                        g_AllTogle;

namespace settings {
    extern const char* WORLD_ENCOUNTER_CHANCE;
}


static int ImageNum = -1;

struct static_brothel_data {
    int    price;
    int business;
    BrothelCreationData data;
};

// stats of each brothel: price to buy, starting rooms, maximum rooms, required # of businesses owned
static static_brothel_data brothel_data[] = {
        { 0, 0, 20, 200, "Brothel1.jpg", BuildingType::BROTHEL},
        { 10000, 30, 25, 250, "Brothel2.jpg", BuildingType::BROTHEL},
        { 30000, 70, 30, 300, "Brothel3.jpg", BuildingType::BROTHEL },
        { 70000, 100, 35, 350, "Brothel4.jpg", BuildingType::BROTHEL },
        { 150000, 140, 40, 400, "Brothel5.jpg", BuildingType::BROTHEL },
        { 300000, 170, 50, 500, "Brothel6.jpg", BuildingType::BROTHEL },
        { 1000000, 220, 80, 600, "Brothel7.jpg", BuildingType::BROTHEL },
        { 5000, 5, 20, 200, "Centre.jpg", BuildingType::CENTRE },
        { 10000, 10, 20, 200, "Farm.png", BuildingType::FARM },
        { 15000, 15, 20, 200, "Arena.png", BuildingType::ARENA },
        { 20000, 20, 20, 200, "Movies.jpg", BuildingType::STUDIO },
        { 25000, 25, 20, 200, "Clinic.png", BuildingType::CLINIC }
};

cScreenTown::cScreenTown() : cGameWindow("town_screen.xml")
{
    m_first_walk = true;
}


void cScreenTown::set_ids()
{
    back_id            /**/ = get_id("BackButton", "Back");
    gold_id            /**/ = get_id("Gold", "*Unused*");//
    walk_id            /**/ = get_id("WalkButton");
    girlimage_id    /**/    = get_id("GirlImage");
    curbrothel_id    /**/   = get_id("CurrentBrothel");
    slavemarket_id    /**/  = get_id("SlaveMarket");
    shop_id            /**/ = get_id("Shop");
    mayor_id        /**/    = get_id("MayorsOffice");
    bank_id            /**/ = get_id("Bank");
    house_id        /**/    = get_id("House");
    clinic_id        /**/   = get_id("Clinic");
    studio_id        /**/   = get_id("Studio");
    arena_id        /**/    = get_id("Arena");
    centre_id        /**/   = get_id("Centre");
    farm_id            /**/ = get_id("Farm");
    prison_id        /**/   = get_id("Prison");
    brothel0_id        /**/ = get_id("Brothel0");
    brothel1_id        /**/ = get_id("Brothel1");
    brothel2_id        /**/ = get_id("Brothel2");
    brothel3_id        /**/ = get_id("Brothel3");
    brothel4_id        /**/ = get_id("Brothel4");
    brothel5_id        /**/ = get_id("Brothel5");
    brothel6_id        /**/ = get_id("Brothel6");
    setup_id        /**/    = get_id("SetUp", "*Unused*");//

    SetButtonNavigation(back_id, "<back>");
    SetButtonNavigation(slavemarket_id, "Slave Market");
    SetButtonNavigation(prison_id, "Prison");
    SetButtonNavigation(bank_id, "Bank");
    SetButtonNavigation(mayor_id, "Mayor");
    SetButtonNavigation(setup_id, "Property Management");

    SetButtonCallback(brothel0_id, [this]() {check_building(0); });
    SetButtonCallback(brothel1_id, [this]() {check_building(1); });
    SetButtonCallback(brothel2_id, [this]() {check_building(2); });
    SetButtonCallback(brothel3_id, [this]() {check_building(3); });
    SetButtonCallback(brothel4_id, [this]() {check_building(4); });
    SetButtonCallback(brothel5_id, [this]() {check_building(5); });
    SetButtonCallback(brothel6_id, [this]() {check_building(6); });
    SetButtonCallback(centre_id, [this]() {check_building(7); });
    SetButtonCallback(farm_id, [this]() {check_building(8); });
    SetButtonCallback(arena_id, [this]() {check_building(9); });
    SetButtonCallback(studio_id, [this]() {check_building(10); });
    SetButtonCallback(clinic_id, [this]() {check_building(11); });
    SetButtonCallback(house_id, [this]() {
        set_active_building(g_Game->buildings().building_with_type(BuildingType::HOUSE));
        push_window("Player House");
    });
    SetButtonCallback(shop_id, [this]() {
        g_AllTogle = false;
        push_window("Item Management");
    });
    SetButtonCallback(walk_id, [this]() {
        do_walk();
        if (!g_Game->allow_cheats()) g_WalkAround = true;
        init(false);
    });
}

IBuilding& init_building(const static_brothel_data* data) {
    IBuilding& building = g_Game->buildings().AddBuilding( data->data);
    g_Game->gold().brothel_cost(data->price);
    return building;
}

void cScreenTown::init(bool back)
{
    if (gold_id >= 0)
    {
        std::stringstream ss; ss << "Gold: " << g_Game->gold().ival();
        EditTextItem(ss.str(), gold_id);
    }

    Focused();

    // buttons enable/disable
    DisableWidget(walk_id, g_WalkAround);

    int num_brothels = g_Game->buildings().num_buildings(BuildingType::BROTHEL);
    HideWidget(brothel2_id, num_brothels < 2);
    HideWidget(brothel3_id, num_brothels < 3);
    HideWidget(brothel4_id, num_brothels < 4);
    HideWidget(brothel5_id, num_brothels < 5);
    HideWidget(brothel6_id, num_brothels < 6);


    std::string brothel = "Current Brothel: ";
    brothel += active_building().name();
    EditTextItem(brothel, curbrothel_id);
}

void cScreenTown::process()
{
    // TODO figure out a way to display the event graphics!
    if (girlimage_id != -1) HideWidget(girlimage_id, true);
}

std::string cScreenTown::walk_no_luck()
{
    if (m_first_walk)
    {
        m_first_walk = false;
        return    R"(Your father once called this "talent spotting" - wandering though town, looking for new girls. "Willing, desperate or vulnerable" is how he put it. Not that any of those qualities are in evidence today, reminding you of another of your father's sayings - the one about patience. It's probably the only time he ever used the word "virtue".)";
    }
    switch (g_Dice % 8)
    {
    case 1: return  "Married. Married. Bodyguard. Already works for you. Married. Hideous. Not a woman. Married. Escorted. Married... Might as well go home, there's nothing happening out here.";
    case 2: return    "It's not a bad life, if you can get paid for hanging around on street corners and eyeing up the pretty girls. Not a single decent prospect in the bunch of them, mind. Every silver lining has a cloud...";
    case 3: return    "You've walked and walked and walked, and the prettiest thing you've seen all day turned out not to be female. It's time to go home...";
    case 4: return    "When the weather is bad, the hunting is good. Get them cold and wet enough and girls too proud to spread their legs suddenly can't get their knickers off fast enough, if the job only comes with room and board. The down side is that you spend far too much time walking in the rain when everyone sane is warm inside. Time to head home for a mug of cocoa and a nice hot trollop.";
    case 5: return    "There's a bit of skirt over there with a lovely figure, and had a face that was pretty, ninety years ago. Over yonder, a sweet young thing frolicking through the marketplace. She's being ever so daring, spending her daddy's gold, and hasn't yet realised that there's a dozen of her daddy's goons keeping a discreet eye on her.  It's like that everywhere today. Maybe tomorrow will be better.";
    default:return    "The city is quiet. The same old streets; the same old faces.";
    }
    // I don't think this should happen, hence the overly dramatic prose
    return "The sky is filled with thunder, and portals are opening all over Crossgate. You've seen five rains of frogs so far and three madmen speaking in tongues. In the marketplace a mechanical contraption materialised which walked like a man and declaimed alien poetry for half an hour before bursting into flames and collapsing. And the only thing all day that looked female and and attractive turned out to be some otherwordly species of carnivorous plant, and had to be destroyed by the town guard. The only good thing about this day is that it's over. It's time to go home.";
}

void cScreenTown::do_walk()
{
    if (g_WalkAround)
    {
        push_message("You can only do this once per week.", COLOR_RED);
        return;
    }
    auto girl = g_Game->GetRandomGirl();                        // let's get a girl for the player to meet
    if (girl == nullptr)                                                // if there's no girl, no meeting
    {
        push_message(walk_no_luck(), COLOR_RED);
        return;
    }
    // most of the time, you're not going to find anyone unless you're cheating, of course.
    if (!g_Dice.percent(g_Game->settings().get_percent(settings::WORLD_ENCOUNTER_CHANCE)) && !g_Game->allow_cheats())
    {
        push_message(walk_no_luck(), COLOR_BLUE);
        return;
    }

    if (girlimage_id != -1)
    {
        PrepareImage(girlimage_id, girl.get(), IMGTYPE_PROFILE, true, ImageNum);
        HideWidget(girlimage_id, false);
    }

    girl->TriggerEvent("girl:meet:town");
}

bool cScreenTown::buy_building(static_brothel_data* bck)
{
    std::locale syslocale("");
    std::stringstream ss;
    ss.imbue(syslocale);

    if (!g_Game->gold().afford(bck->price) || g_Game->gang_manager().GetNumBusinessExtorted() < bck->business)
    {    // can't buy it
        ss << ("This building costs ") << bck->price << (" gold and you need to control at least ") << bck->business << (" businesses.");
        if (!g_Game->gold().afford(bck->price))
            ss << "\n" << ("You need ") << (bck->price - g_Game->gold().ival()) << (" more gold to afford it.");
        if (g_Game->gang_manager().GetNumBusinessExtorted() < bck->business)
            ss << "\n" << ("You need to control ") << (bck->business - g_Game->gang_manager().GetNumBusinessExtorted()) << (" more businesses.");
        push_message(ss.str(), 0);
        return false;
    }
    else    // can buy it
    {
        ss << ("Do you wish to purchase this building for ") << bck->price << (" gold? It has ") << bck->data.rooms << (" rooms.");
        push_message(ss.str(), 2);
        input_choice("", std::vector<std::string>{"Buy It", "Don't Buy It"}, [this, bck](int selection) {
            if(selection != 0) return;
            if (bck->data.type == BuildingType::BROTHEL) {
                push_message("Enter a name for your new brothel.", 0);
                input_string([this, bck](const std::string& name){
                    if (g_Game->get_objective() && g_Game->get_objective()->m_Objective == OBJECTIVE_GETNEXTBROTHEL)
                        g_Game->objective_manager().PassObjective();
                    init_building(bck).set_name(name);
                    init(false);
                });

            } else {
                init_building(bck);
                init(false);
            }
        });
        return true;
    }
}

// player clicked on one of the brothels
void cScreenTown::check_building(int BrothelNum)
{
    auto data = brothel_data[BrothelNum];
    auto type = data.data.type;
    int num = BrothelNum;
    if(type != BuildingType::BROTHEL) {
        num = 0;
    }

    if (g_Game->buildings().num_buildings(type) == num)    // player doesn't own this brothel... can he buy it?
    {
        buy_building(brothel_data + BrothelNum);

    }
    else    // player owns this brothel... go to it
    {
        set_active_building(g_Game->buildings().building_with_type(type, num));
        replace_window("Building Management");
    }
}

