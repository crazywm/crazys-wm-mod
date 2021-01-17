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
#include "cScreenGangs.h"
#include "interface/cWindowManager.h"
#include "cGold.h"
#include "cTariff.h"
#include "InterfaceProcesses.h"
#include "cGangs.h"
#include "Game.hpp"
#include "CLog.h"

static std::stringstream ss;

cScreenGangs::cScreenGangs() : cInterfaceWindowXML("gangs_screen.xml")
{
}

void cScreenGangs::set_ids()
{
    ganghire_id        = get_id("GangHireButton");
    gangfire_id        = get_id("GangFireButton");
    totalcost_id       = get_id("TotalCost");
    gold_id            = get_id("Gold");
    ganglist_id        = get_id("GangList");
    missionlist_id     = get_id("MissionList");
    gangdesc_id        = get_id("GangDescription","*Unused*");//
    missiondesc_id     = get_id("MissionDescription");
    weaponlevel_id     = get_id("WeaponDescription");
    weaponup_id        = get_id("WeaponUpButton");
    netdesc_id         = get_id("NetDescription");
    netbuy_id          = get_id("BuyNetsButton");
    netbuy10_id        = get_id("BuyNetsButton10");
    netbuy20_id        = get_id("BuyNetsButton20");
    netautobuy_id      = get_id("AutoBuyNetsToggle");
    healdesc_id        = get_id("HealPotDescription");
    healbuy_id         = get_id("BuyHealPotButton");
    healbuy10_id       = get_id("BuyHealPotButton10");
    healbuy20_id       = get_id("BuyHealPotButton20");
    healautobuy_id     = get_id("AutoBuyHealToggle");
    recruitlist_id     = get_id("RecruitList");

    // `J` added for .06.01.10
    controlcatacombs_id = get_id("ControlCatacombs");
    catacombslabel_id   = get_id("Catacombs");
    ganggetsgirls_id    = get_id("GangGetsGirls");
    ganggetsitems_id    = get_id("GangGetsItems");
    ganggetsbeast_id    = get_id("GangGetsBeast");
    girlspercslider_id  = get_id("GirlsPercSlider");
    itemspercslider_id  = get_id("ItemsPercSlider");

    //Set the default sort order for columns, so listboxes know the order in which data will be sent
    std::vector<std::string> RecruitColumns{ "GangName", "Number", "Combat", "Magic", "Intelligence", "Agility", "Constitution", "Charisma", "Strength", "Service" };
    SortColumns(recruitlist_id, RecruitColumns);
    std::vector<std::string> GangColumns{ "GangName", "Number", "Mission", "Combat", "Magic", "Intelligence", "Agility", "Constitution", "Charisma", "Strength", "Service" };
    SortColumns(ganglist_id, GangColumns);

    // set button callbacks
    SetButtonCallback(gangfire_id, [this](){
        int selection = GetLastSelectedItemFromList(ganglist_id);
        if (selection != -1)
        {
            g_Game->gang_manager().FireGang(selection);
            init(false);
        };
    });

    SetButtonCallback(ganghire_id, [this]() {
        hire_recruitable();
    });

    SetButtonCallback(weaponup_id, [this]() {
        ForAllSelectedItems(ganglist_id, [&](int selection) {
            sGang* gang = g_Game->gang_manager().GetGang(selection);
            if(gang) {
                int wlev = gang->weapon_level();
                int cost = g_Game->tariff().goon_weapon_upgrade(wlev);
                if (g_Game->gold().item_cost(cost) && wlev < 3)
                {
                    gang->set_weapon_level(wlev + 1);                }
            }
        });
        init(false);
    });

    SetButtonCallback(netbuy_id,    [this]() { buy_nets(1);});
    SetButtonCallback(netbuy10_id,  [this]() { buy_nets(10);});
    SetButtonCallback(netbuy20_id,  [this]() { buy_nets(20);});
    SetButtonCallback(healbuy_id,   [this]() { buy_potions(1);});
    SetButtonCallback(healbuy10_id, [this]() { buy_potions(10);});
    SetButtonCallback(healbuy20_id, [this]() { buy_potions(20);});

    SetListBoxSelectionCallback(missionlist_id, [this](int sel) { on_select_mission(); });
    SetListBoxHotKeys(missionlist_id, SDLK_w, SDLK_s);
    SetListBoxSelectionCallback(ganglist_id, [this](int sel) { on_select_gang(sel); });
    SetListBoxHotKeys(ganglist_id, SDLK_a, SDLK_d);
    SetListBoxSelectionCallback(recruitlist_id, [this](int sel) {
        update_recruit_btn();
    });
    SetListBoxDoubleClickCallback(recruitlist_id, [this](int sel) {hire_recruitable(); });
    SetListBoxHotKeys(recruitlist_id, SDLK_q, SDLK_e);

    AddKeyCallback(SDLK_SPACE, [this](){ hire_recruitable(); });

    SetCheckBoxCallback(netautobuy_id, [this](bool on) {
        int nets = g_Game->gang_manager().GetNets();
        g_Game->gang_manager().KeepNetStocked(on ? nets : 0);
    });

    SetCheckBoxCallback(healautobuy_id, [this](bool on) {
        int potions = g_Game->gang_manager().GetHealingPotions();
        g_Game->gang_manager().KeepHealStocked(on ? potions : 0);
    });

    SetSliderCallback(girlspercslider_id, [this](int value) {
        int s1 = value;
        int s2 = SliderValue(itemspercslider_id);
        if (s2 < s1)
        {
            s2 = s1;
            SliderRange(itemspercslider_id, 0, 100, s2, 1);
        }
        update_sliders();
    });

    SetSliderCallback(itemspercslider_id, [this](int value) {
        int s1 = value;
        int s2 = SliderValue(girlspercslider_id);
        if (s1 < s2)
        {
            s2 = s1;
            SliderRange(girlspercslider_id, 0, 100, s2, 1);
        }
        update_sliders();
    });
}

void cScreenGangs::init(bool back)
{
    Focused();

    int selection = GetLastSelectedItemFromList(ganglist_id);

    ClearListBox(missionlist_id);
    AddToListBox(missionlist_id, 0, "GUARDING");
    AddToListBox(missionlist_id, 1, "SABOTAGE");
    AddToListBox(missionlist_id, 2, "SPY ON GIRLS");
    AddToListBox(missionlist_id, 3, "RECAPTURE");
    AddToListBox(missionlist_id, 4, "ACQUIRE TERRITORY");
    AddToListBox(missionlist_id, 5, "PETTY THEFT");
    AddToListBox(missionlist_id, 6, "GRAND THEFT");
    AddToListBox(missionlist_id, 7, "KIDNAPPING");
    AddToListBox(missionlist_id, 8, "CATACOMBS");
    AddToListBox(missionlist_id, 9, "TRAINING");
    AddToListBox(missionlist_id, 10, "RECRUITING");
    AddToListBox(missionlist_id, 11, "SERVICE");

    SliderRange(girlspercslider_id, 0, 100, g_Game->gang_manager().Gang_Gets_Girls(), 1);
    SliderRange(itemspercslider_id, 0, 100, g_Game->gang_manager().Gang_Gets_Girls() + g_Game->gang_manager().Gang_Gets_Items(), 1);
    ss.str("");    ss << "Girls : " << g_Game->gang_manager().Gang_Gets_Girls() << "%";    EditTextItem(ss.str(), ganggetsgirls_id);
    ss.str("");    ss << "Items : " << g_Game->gang_manager().Gang_Gets_Items() << "%";    EditTextItem(ss.str(), ganggetsitems_id);
    ss.str("");    ss << "Beasts : " << g_Game->gang_manager().Gang_Gets_Beast() << "%";    EditTextItem(ss.str(), ganggetsbeast_id);

    SetCheckBox(netautobuy_id, (g_Game->gang_manager().GetNetRestock() > 0));
    SetCheckBox(healautobuy_id, (g_Game->gang_manager().GetHealingRestock() > 0));

    // weapon upgrades
    update_wpn_info();

    int nets = g_Game->gang_manager().GetNets();
    ss.str(""); ss << "Nets (" << g_Game->tariff().nets_price(1) << "g each): " << nets;
    EditTextItem(ss.str(), netdesc_id);
    DisableWidget(netbuy_id, nets >= 60);
    DisableWidget(netbuy10_id, nets >= 60);
    DisableWidget(netbuy20_id, nets >= 60);
    DisableWidget(netautobuy_id, nets < 1);

    int potions = g_Game->gang_manager().GetHealingPotions();
    ss.str(""); ss << "Heal Potions (" << g_Game->tariff().healing_price(1) << "g each): " << potions;
    EditTextItem(ss.str(), healdesc_id);
    DisableWidget(healbuy_id, potions >= 200);
    DisableWidget(healbuy10_id, potions >= 200);
    DisableWidget(healbuy20_id, potions >= 200);
    DisableWidget(healautobuy_id, potions < 1);

    int cost = 0;
    if (g_Game->gang_manager().GetNumGangs() > 0)
    {
        for(auto& gang : g_Game->gang_manager().GetPlayerGangs()) {
            cost += g_Game->tariff().goon_mission_cost(gang->m_MissionID);
        }
    }
    ss.str(""); ss << "Weekly Cost: " << cost;
    EditTextItem(ss.str(), totalcost_id);
    if (gold_id >= 0)
    {
        ss.str(""); ss << "Gold: " << g_Game->gold().ival();
        EditTextItem(ss.str(), gold_id);
    }

    ClearListBox(ganglist_id);
    int num = 0;

    // loop through the gangs, populating the list box
    g_LogFile.log(ELogLevel::DEBUG, "Setting gang mission descriptions\n");
    for(auto& gang : g_Game->gang_manager().GetPlayerGangs())
    {
        // format the string with the gang name, mission and number of men
        std::vector<FormattedCellData> Data(11);

        Data[0]  = mk_text(gang->name());
        Data[1]  = mk_num(gang->m_Num);
        Data[2]  = mk_text(short_mission_desc(gang->m_MissionID));

        Data[3]  = mk_percent(gang->m_Skills[SKILL_COMBAT]);
        Data[4]  = mk_percent(gang->m_Skills[SKILL_MAGIC]);
        Data[5]  = mk_percent(gang->m_Stats[STAT_INTELLIGENCE]);
        Data[6]  = mk_percent(gang->m_Stats[STAT_AGILITY]);
        Data[7]  = mk_percent(gang->m_Stats[STAT_CONSTITUTION]);
        Data[8]  = mk_percent(gang->m_Stats[STAT_CHARISMA]);
        Data[9]  = mk_percent(gang->m_Stats[STAT_STRENGTH]);
        Data[10] = mk_percent(gang->m_Skills[SKILL_SERVICE]);

        //        cerr << "Gang:\t" << Data[0] << "\t" << Data[1] << "\t" << Data[2]
        //            << "\t" << Data[3] << "\t" << Data[4] << "\t" << Data[5] << "\t" << Data[6] << endl;

        /*
        *            add the box to the list; red highlight gangs that are low on numbers
        */
        int color = (gang->m_Num < 6 ? COLOR_RED : COLOR_BLUE);
        if (gang->m_Num < 6 && (gang->m_MissionID == MISS_SERVICE || gang->m_MissionID == MISS_TRAINING)) color = COLOR_YELLOW;
        AddToListBox(ganglist_id, num++, std::move(Data), color);
    }

    ClearListBox(recruitlist_id);
    num = 0;

    // loop through the gangs, populating the list box
    g_LogFile.log(ELogLevel::DEBUG, "Setting recruitable gang info\n");
    for (auto& current : g_Game->gang_manager().GetHireableGangs())
    {
        // format the string with the gang name, mission and number of men
        std::vector<FormattedCellData> Data(10);
        Data[0] = mk_text(current->name());
        Data[1] = mk_num(current->m_Num);
        Data[2] = mk_percent(current->combat());
        Data[3] = mk_percent(current->magic());
        Data[4] = mk_percent(current->intelligence());
        Data[5] = mk_percent(current->agility());
        Data[6] = mk_percent(current->constitution());
        Data[7] = mk_percent(current->charisma());
        Data[8] = mk_percent(current->strength());
        Data[9] = mk_percent(current->service());

        //           add the box to the list
        int color = current->m_Num < 6 ? COLOR_RED : COLOR_BLUE;
        AddToListBox(recruitlist_id, num++, std::move(Data), color);
    }

    if (selection == -1 && GetListBoxSize(ganglist_id) >= 1) selection = 0;

    if (selection >= 0)
    {
        while (selection > GetListBoxSize(ganglist_id) && selection != -1) selection--;
    }
    if (selection >= 0) SetSelectedItemInList(ganglist_id, selection);

    update_recruit_btn();
    DisableWidget(gangfire_id, (g_Game->gang_manager().GetNumGangs() <= 0) || (selection == -1));
}

void cScreenGangs::update_sliders()
{
    int s1 = SliderValue(girlspercslider_id);
    int s2 = SliderValue(itemspercslider_id);
    g_Game->gang_manager().Gang_Gets_Girls(s1);
    g_Game->gang_manager().Gang_Gets_Items(s2 - s1);
    g_Game->gang_manager().Gang_Gets_Beast(100 - s2);
    ss.str("");
    ss << "Girls : " << g_Game->gang_manager().Gang_Gets_Girls() << "%";
    EditTextItem(ss.str(), ganggetsgirls_id);
    ss.str("");
    ss << "Items : " << g_Game->gang_manager().Gang_Gets_Items() << "%";
    EditTextItem(ss.str(), ganggetsitems_id);
    ss.str("");
    ss << "Beasts : " << g_Game->gang_manager().Gang_Gets_Beast() << "%";
    EditTextItem(ss.str(), ganggetsbeast_id);
}

void cScreenGangs::on_select_gang(int selection)
{
    if (selection != -1)
    {
        sGang* gang = g_Game->gang_manager().GetGang(selection);
        ss.str(""); ss << "Name: " << gang->name() << "\n" << "Number: " << gang->m_Num << "\n" << "Combat: " << gang->m_Skills[SKILL_COMBAT] << "%\n" << "Magic: " << gang->m_Skills[SKILL_MAGIC] << "%\n" << "Intelligence: " << gang->m_Stats[STAT_INTELLIGENCE] << "%\n";
        EditTextItem(ss.str(), gangdesc_id);
        SetSelectedItemInList(missionlist_id, gang->m_MissionID, false);
        set_mission_desc(gang->m_MissionID);        // set the long description for the mission
    }

    update_wpn_info();
}

void cScreenGangs::on_select_mission()
{
    // get the index into the missions list
    int mission_id            = GetLastSelectedItemFromList(missionlist_id);
    set_mission_desc(mission_id);        // set the textfield with the long description and price for this mission
    ForAllSelectedItems(ganglist_id, [&](int selection) {
        sGang* gang = g_Game->gang_manager().GetGang(selection);
        /*
        *                make sure we found the gang - pretty catastrophic
        *                if not, so log it if we do
        */
        if (gang == nullptr)
        {
            g_LogFile.log(ELogLevel::ERROR, "No gang for index ", selection);
            return;
        }
        /*
        *                if the mission id is -1, nothing else to do
        *                (moving this to before the recruitment check
        *                since -1 most likely means nothing selected in
        *                the missions list)
        */
        if (mission_id == -1) { return; }
        /*
        *                if the gang is already doing <whatever>
        *                then let them get on with it
        */
        if (gang->m_MissionID == mission_id) { return; }
        /*
        *                if they were recruiting, turn off the
        *                auto-recruit flag
        */
        if (gang->m_MissionID == MISS_RECRUIT && gang->m_AutoRecruit)
        {
            gang->m_AutoRecruit = false;
            gang->m_LastMissID = -1;
        }
        gang->m_MissionID = mission_id;
        /*
        *                format the display line
        */
        init(false);
    });

    int cost = 0;
    if (g_Game->gang_manager().GetNumGangs() > 0)
    {
        for (int i = 0; i < g_Game->gang_manager().GetNumGangs(); i++)
        {
            sGang* g = g_Game->gang_manager().GetGang(i);
            cost += g_Game->tariff().goon_mission_cost(g->m_MissionID);
        }
    }
    ss.str("");
    ss << "Weekly Cost: " << cost;
    EditTextItem(ss.str(), totalcost_id);
    if (gold_id >= 0)
    {
        ss.str(""); ss << "Gold: " << g_Game->gold().ival();
        EditTextItem(ss.str(), gold_id);
    }
}

std::string cScreenGangs::mission_desc(int mid)
{
    switch (mid) {
    case MISS_GUARDING:        return "Your men will guard your property.";
    case MISS_SABOTAGE:        return "Your men will move about town and destroy and loot enemy businesses.";
    case MISS_SPYGIRLS:        return "Your men will spy on your working girls, looking for the ones who take extra for themselves.";
    case MISS_CAPTUREGIRL:    return "Your men will set out to re-capture any girls who have recently run away.";
    case MISS_EXTORTION:    return "Sends your men out to force local gangs out of their areas of town.";
    case MISS_PETYTHEFT:    return "Your men will mug people in the street.";
    case MISS_GRANDTHEFT:    return "Your men will attempt to rob a bank or other risky place with high rewards.";
    case MISS_KIDNAPP:        return "Your men will kidnap beggar, homeless or lost girls from the street and also lure other girls into working for you.";
    case MISS_CATACOMBS:    return "Your men will explore the catacombs for treasure.";
    case MISS_RECRUIT:        return "Your men will replace their missing men (up to 15).";
    case MISS_TRAINING:        return "Your men will improve their skills slightly (1-15 members ok).";
    case MISS_SERVICE:        return "Your men will help out in the community (1-15 members ok).";
    default:
        break;
    }
    ss.str(""); ss << "Error: unexpected mission ID: " << mid;
    return ss.str();
}

std::string cScreenGangs::short_mission_desc(int mid)
{
    switch (mid)
    {
    case MISS_GUARDING:        return "Guarding";
    case MISS_SABOTAGE:        return "Sabotaging";
    case MISS_SPYGIRLS:        return "Watching Girls";
    case MISS_CAPTUREGIRL:    return "Finding escaped girls";
    case MISS_EXTORTION:    return "Acquiring Territory";
    case MISS_PETYTHEFT:    return "Mugging people";
    case MISS_GRANDTHEFT:    return "Robbing places";
    case MISS_KIDNAPP:        return "Kidnapping Girls";
    case MISS_CATACOMBS:    return "Exploring Catacombs";
    case MISS_TRAINING:        return "Training Skills";
    case MISS_RECRUIT:        return "Recruiting Men";
    case MISS_SERVICE:        return "Serving the Community";
    default:                return "Error: Unknown";
    }
}

int cScreenGangs::set_mission_desc(int mid)
{
    int price = g_Game->tariff().goon_mission_cost(mid);            // OK: get the difficulty-adjusted price for this mission
    std::string desc = mission_desc(mid);                    // and get a description of the mission
    ss.str(""); ss << desc << " (" << price << "g)";                // stick 'em both together ...
    EditTextItem(ss.str(), missiondesc_id);                // ... and set the text field
    return price;                                        // return the mission price
}

void cScreenGangs::hire_recruitable()
{
    ForAllSelectedItems(recruitlist_id, [shift = 0](int sel_recruit) mutable {
        sel_recruit -= shift;
        if ((g_Game->gang_manager().GetNumGangs() >= g_Game->gang_manager().GetMaxNumGangs()) || (sel_recruit == -1)) return;
        g_Game->gang_manager().HireGang(sel_recruit);
        ++shift;
    });

    init(false);
}

void cScreenGangs::buy_potions(int buypots)
{
    g_Game->gang_manager().BuyHealingPotions(buypots, IsCheckboxOn(healautobuy_id));
    init(false);
}

void cScreenGangs::buy_nets(int buynets)
{
    g_Game->gang_manager().BuyNets(buynets, IsCheckboxOn(netautobuy_id));
    init(false);
}

void cScreenGangs::update_wpn_info() {
    ss.str("");    ss << "Weapon Level: ";
    int wpn_cost = 0;
    ForAllSelectedItems(ganglist_id, [&](int selection) {
        sGang* gang = g_Game->gang_manager().GetGang(selection);
        if(gang) {
            ss << gang->weapon_level() << " ";
            if ( gang->weapon_level() < 3)
            {
                wpn_cost += g_Game->tariff().goon_weapon_upgrade(gang->weapon_level());
            }
        }
    });

    if(wpn_cost == 0) {
        DisableWidget(weaponup_id);
    }
    else {
        EnableWidget(weaponup_id);
        ss << " Next: " << wpn_cost << "g";
    }
    EditTextItem(ss.str(), weaponlevel_id);
}

void cScreenGangs::update_recruit_btn() {
    int sel_recruit = GetLastSelectedItemFromList(recruitlist_id);
    DisableWidget(ganghire_id, (g_Game->gang_manager().GetNumHireableGangs() <= 0) ||
                               (g_Game->gang_manager().GetNumGangs() >= g_Game->gang_manager().GetMaxNumGangs()) ||
                               (sel_recruit == -1));
}
