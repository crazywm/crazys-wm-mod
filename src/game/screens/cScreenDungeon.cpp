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
#include <string>
#include <sstream>
#include <algorithm>
#include "widgets/cListBox.h"
#include "cScreenDungeon.h"
#include "buildings/cBuildingManager.h"
#include "buildings/cDungeon.h"
#include "character/cPlayer.h"
#include "cTariff.h"
#include "cGirlGangFight.h"
#include "cGirlTorture.h"
#include "Game.hpp"

extern cRng                    g_Dice;
extern int                    g_TalkCount;

static int                    ImageNum = -1;
static std::vector<int>            select_girls;
static std::stringstream ss;

cScreenDungeon::cScreenDungeon() : cGameWindow("dungeon_screen.xml")
{
    selection = -1;
}

void cScreenDungeon::set_ids()
{
    back_id         = get_id("BackButton", "Back");
    header_id       = get_id("DungeonHeader");
    gold_id         = get_id("Gold");
    girllist_id     = get_id("GirlList");
    girlimage_id    = get_id("GirlImage");
    brandslave_id   = get_id("BrandSlaveButton");
    release_id      = get_id("ReleaseButton");
    allowfood_id    = get_id("AllowFoodButton");
    torture_id      = get_id("TortureButton");
    stopfood_id     = get_id("StopFeedingButton");
    interact_id     = get_id("InteractButton");
    interactc_id    = get_id("InteractCount");
    releaseall_id   = get_id("ReleaseAllButton");
    releasecust_id  = get_id("ReleaseCustButton");
    viewdetails_id  = get_id("DetailsButton");
    sellslave_id    = get_id("SellButton");

    releaseto_id    = get_id("ReleaseTo");
    roomsfree_id    = get_id("RoomsFree");
    brothel0_id     = get_id("Brothel0");
    brothel1_id     = get_id("Brothel1");
    brothel2_id     = get_id("Brothel2");
    brothel3_id     = get_id("Brothel3");
    brothel4_id     = get_id("Brothel4");
    brothel5_id     = get_id("Brothel5");
    brothel6_id     = get_id("Brothel6");
    house_id        = get_id("House");
    clinic_id       = get_id("Clinic");
    studio_id       = get_id("Studio");
    arena_id        = get_id("Arena");
    centre_id       = get_id("Centre");
    farm_id         = get_id("Farm");

    struct RelBtn {
        const char* name;
        BuildingType type;
        int index;
    };

    RelBtn btns[] = {
            {"Brothel0", BuildingType::BROTHEL, 0},
            {"Brothel1", BuildingType::BROTHEL, 1},
            {"Brothel2", BuildingType::BROTHEL, 2},
            {"Brothel3", BuildingType::BROTHEL, 3},
            {"Brothel4", BuildingType::BROTHEL, 4},
            {"Brothel5", BuildingType::BROTHEL, 5},
            {"Brothel6", BuildingType::BROTHEL, 6},
            {"House", BuildingType::HOUSE, 0},
            {"Clinic", BuildingType::CLINIC, 0},
            {"Studio", BuildingType::STUDIO, 0},
            {"Arena", BuildingType::ARENA, 0},
            {"Centre", BuildingType::CENTRE, 0},
            {"Farm", BuildingType::FARM, 0},
    };

    for(const auto& btn : btns) {
        SetButtonCallback(get_id(btn.name), [this, btn]() {
            change_release(btn.type, btn.index);
        });
    }

    SetButtonNavigation(back_id, "<back>");
    SetButtonCallback(brandslave_id, [this]() { enslave(); });
    SetButtonCallback(releasecust_id, [this]() { release_all_customers(); });
    SetButtonCallback(sellslave_id, [this]() { sell_slaves(); });
    SetButtonCallback(viewdetails_id, [this]() { view_girl(); });
    SetButtonHotKey(viewdetails_id, SDLK_SPACE);
    SetButtonCallback(releaseall_id, [this]() {
        release_all_girls();
        selection = -1;
    });

    SetButtonCallback(stopfood_id, [this]() {
        stop_feeding();
        selection = -1;
    });

    SetButtonCallback(allowfood_id, [this]() {
        start_feeding();
        selection = -1;
    });

    SetButtonCallback(interact_id, [this]() {
        if (selection == -1) return;
        talk();
    });

    SetButtonCallback(torture_id, [this]() {
        torture();
        selection = -1;
    });

    SetButtonCallback(release_id, [this]() {
        release();
        selection = -1;
        init(false);
    });

    SetListBoxSelectionCallback(girllist_id, [this](int selected) {
        selection_change();
        if (IsMultiSelected(girllist_id))         // disable buttons based on multiselection
        {
            DisableWidget(interact_id, true);
        }
        update_image();
    });
    SetListBoxDoubleClickCallback(girllist_id, [this](int selected) {
        view_girl();
    });
    SetListBoxHotKeys(girllist_id, SDLK_a, SDLK_d);
}

void cScreenDungeon::init(bool back)
{
    if(!back)
        m_ReleaseBuilding = &active_building();

    Focused();
    ClearListBox(girllist_id);                // clear the lists
    //get a list of all the column names, so we can find which data goes in that column
    std::vector<std::string> columnNames = GetListBox(girllist_id)->GetColumnNames();

    if (gold_id >= 0)
    {
        ss.str(""); ss << "Gold: " << g_Game->gold().ival();
        EditTextItem(ss.str(), gold_id);
    }

    ss.str("");    ss << "Your Dungeon where " << g_Game->dungeon().GetNumDied() << " people have died.";
    EditTextItem(ss.str(), header_id);
    // Fill the list box
    selection = g_Game->dungeon().GetNumGirls() > 0 ? 0 : -1;
    for (int i = 0; i < g_Game->dungeon().GetNumGirls(); i++)                                                // add girls
    {
        std::vector<std::string> Data;
        sGirl *girl = g_Game->dungeon().GetGirl(i)->m_Girl.get();                                            // get the i-th girl
        if (selected_girl().get() == girl) selection = i;                                                            // if selected_girl is this girl, update selection
        girl->m_DayJob = girl->m_NightJob = JOB_INDUNGEON;
        int col = ((girl->health() <= 30) || (girl->happiness() <= 30)) ? COLOR_RED : COLOR_BLUE;            // if she's low health or unhappy, flag her entry to display in red // Anon21
        g_Game->dungeon().OutputGirlRow(i, Data, columnNames);                                                // add her to the list
        AddToListBox(girllist_id, i, std::move(Data), col);
    }
    // now add the customers
    int offset = g_Game->dungeon().GetNumGirls();
    for (int i = 0; i < g_Game->dungeon().GetNumCusts(); i++)    // add customers
    {
        std::vector<std::string> Data;
        int col = (g_Game->dungeon().GetCust(i)->m_Health <= 30) ? COLOR_RED : COLOR_BLUE;
        g_Game->dungeon().OutputCustRow(i, Data, columnNames);
        AddToListBox(girllist_id, i + offset, std::move(Data), col);
    }

    // disable some buttons
    DisableWidget(allowfood_id);
    DisableWidget(stopfood_id);
    DisableWidget(interact_id);
    if (interactc_id >= 0)
    {
        ss.str(""); ss << "Interactions Left: ";
        if (g_Game->allow_cheats()) ss << "Infinate Cheat";
        else if (g_TalkCount <= 0) ss << "0 (buy in House screen)";
        else ss << g_TalkCount;
        EditTextItem(ss.str(), interactc_id);
    }
    DisableWidget(release_id);
    DisableWidget(brandslave_id);
    DisableWidget(torture_id);
    DisableWidget(sellslave_id);
    //    cerr << "::init: disabling torture" << endl;    // `J` commented out
    DisableWidget(viewdetails_id);
    DisableWidget(releaseall_id, (g_Game->dungeon().GetNumGirls() <= 0));    // only enable "release all girls" if there are girls to release
    DisableWidget(releasecust_id, (g_Game->dungeon().GetNumCusts() <= 0));    // similarly...

    HideWidget(girlimage_id, g_Game->dungeon().GetNumGirls() <= 0);

    ss.str("");    ss << "Release Girl to: " << m_ReleaseBuilding->name();
    EditTextItem(ss.str(), releaseto_id);
    ss.str("");    ss << "Room for " << m_ReleaseBuilding->free_rooms() << " more girls.";
    EditTextItem(ss.str(), roomsfree_id);

    int mum_brothels = g_Game->buildings().num_buildings(BuildingType::BROTHEL);
    HideWidget(brothel1_id, mum_brothels < 2);
    HideWidget(brothel2_id, mum_brothels < 3);
    HideWidget(brothel3_id, mum_brothels < 4);
    HideWidget(brothel4_id, mum_brothels < 5);
    HideWidget(brothel5_id, mum_brothels < 6);
    HideWidget(brothel6_id, mum_brothels < 7);
    HideWidget(clinic_id, !g_Game->has_building(BuildingType::CLINIC));
    HideWidget(studio_id, !g_Game->has_building(BuildingType::STUDIO));
    HideWidget(arena_id, !g_Game->has_building(BuildingType::ARENA));
    HideWidget(centre_id, !g_Game->has_building(BuildingType::CENTRE));
    HideWidget(farm_id, !g_Game->has_building(BuildingType::FARM));

    // if a selection of girls is stored, try to re-select them
    if (!select_girls.empty())
    {
        selection = select_girls.back();
        for (int i = 0; i < (int)select_girls.size(); i++)
        {
            SetSelectedItemInList(girllist_id, select_girls[i], (select_girls[i] == select_girls.back()), false);
        }
        select_girls.clear();
    }
    else if (selection >= 0) SetSelectedItemInList(girllist_id, selection);
}

void cScreenDungeon::selection_change()
{
    selection = GetSelectedItemFromList(girllist_id);
    // if nothing is selected, then we just need to disable some buttons and we're done
    if (selection == -1)
    {
        set_active_girl(nullptr);
        DisableWidget(brandslave_id);
        DisableWidget(allowfood_id);
        DisableWidget(stopfood_id);
        DisableWidget(interact_id);
        DisableWidget(release_id);
        DisableWidget(torture_id);
        //        cerr << "selection = " << selection << " (-1) - disabling torture" << endl;    // `J` commented out
        DisableWidget(viewdetails_id);
        DisableWidget(sellslave_id);
        return;
    }
    // otherwise, we need to enable some buttons...
    DisableWidget(sellslave_id);
    DisableWidget(torture_id, !torture_possible());
    //    cerr << "selection = " << selection << " - enabling torture" << endl;    // `J` commented out
    DisableWidget(interact_id, g_TalkCount == 0);
    EnableWidget(release_id);
    DisableWidget(brandslave_id);
    // and then decide if this is a customer selected, or a girl customer is easiest, so we do that first
    if ((selection - g_Game->dungeon().GetNumGirls()) >= 0)
    {
        // It's a customer! All we need to do is toggle some buttons
        int num = (selection - g_Game->dungeon().GetNumGirls());
        sDungeonCust* cust = g_Game->dungeon().GetCust(num);
        if(cust == nullptr) {
            push_message("Could not select the target. If you encounter this error, please"
                         "provide us a description / save game of how you managed this.", COLOR_RED);
            return;
        }
        DisableWidget(viewdetails_id);
        DisableWidget(interact_id);
        DisableWidget(allowfood_id, cust->m_Feeding);
        DisableWidget(stopfood_id, !cust->m_Feeding);
        return;
    }
    // Not a customer then. Must be a girl...
    int num = selection;
    auto dgirl = g_Game->dungeon().GetGirl(num);
    auto girl = dgirl->m_Girl;
    // again, we're just enabling and disabling buttons
    EnableWidget(viewdetails_id);

    DisableWidget(allowfood_id, dgirl->m_Feeding);
    DisableWidget(stopfood_id, !dgirl->m_Feeding);
    // some of them partly depend upon whether she's a slave or not
    if (dgirl->m_Girl->is_slave())
    {
        EnableWidget(sellslave_id);
        DisableWidget(brandslave_id);
    }
    else
    {
        EnableWidget(brandslave_id);
        DisableWidget(sellslave_id);
    }
    set_active_girl(std::move(girl));
}

int cScreenDungeon::view_girl()
{
    selection = GetSelectedItemFromList(girllist_id);

    if (selection == -1) return Continue;                            // nothing selected, nothing to do.
    if ((selection - g_Game->dungeon().GetNumGirls()) >= 0) return Continue;    // if this is a customer, we're not interested
    reset_cycle_list();

    if(IsMultiSelected(girllist_id)) {
        // if multiple girls are selected, put them into the selection list
        int pos = 0;
        int sel = GetNextSelectedItemFromList(girllist_id, 0, pos);
        while (sel != -1 && sel < g_Game->dungeon().GetNumGirls())
        {
            add_to_cycle_list(g_Game->dungeon().GetGirl(sel)->m_Girl);
            sel = GetNextSelectedItemFromList(girllist_id, pos + 1, pos);
        }
    } else {
        auto girl = g_Game->dungeon().GetGirl(selection)->m_Girl;
        // TODO can this happen?
        if(!girl) return Continue;
        else if(girl->is_dead()) {
            push_message("This is a dead girl. She has ceased to be.", COLOR_RED);
            return Return;
        }
        // if only a single girl is selected, allow iterating over all
        for(auto& g : g_Game->dungeon().girls()) {
            if (!g.m_Girl->is_dead()) {
                add_to_cycle_list(g.m_Girl);
            }
        }
        cycle_to(girl.get());
    }
    push_window("Girl Details");
    return Return;
}

int cScreenDungeon::enslave_customer(int girls_removed, int custs_removed)
{
    /*
    *    mod - docclox - nerfed the cash for selling a customer.
    *    a fat smelly brothel creeper probably shouldn't raise as much as
    *    a sexy young slavegirl. Feel free to un-nerf if you disagree.
    */
    long gold = (g_Dice % 200) + 63;
    g_Game->gold().slave_sales(gold);
    ss.str("");    ss << "You force the customer into slavery lawfully for committing a crime against your business and sell them for " << gold << " gold.";
    push_message(ss.str(), 0);
    g_Game->player().evil(1);
    int customer_index = selection - g_Game->dungeon().GetNumGirls();    // get the index of the about-to-be-sold customer
    customer_index += girls_removed;
    customer_index -= custs_removed;
    sDungeonCust* cust = g_Game->dungeon().GetCust(customer_index);        // get the customer record
    g_Game->dungeon().RemoveCust(cust);        // remove the customer from the dungeon room for an overload here
    return 0;
}

void cScreenDungeon::set_slave_stats(sGirl *girl)
{
    girl->set_slave();
    girl->obedience(-10);
    girl->pcfear(5);
    girl->pclove(-10);
    girl->pchate(5);
    girl->set_default_house_percent();
}

int cScreenDungeon::enslave()
{
    int numCustsRemoved = 0;
    int numGirlsRemoved = 0;
    int pos = 0, deadcount = 0;

    store_selected_girls();
    // roll on vectors!
    for (int selection = GetNextSelectedItemFromList(girllist_id, 0, pos);
         selection != -1;
         selection = GetNextSelectedItemFromList(girllist_id, pos + 1, pos))
    {
        if ((selection - (g_Game->dungeon().GetNumGirls() + numGirlsRemoved)) >= 0)    // it is a customer
        {
            enslave_customer(numGirlsRemoved, numCustsRemoved);
            numCustsRemoved++;
            continue;
        }
        // it is a girl
        sGirl *girl = g_Game->dungeon().GetGirl(selection - numGirlsRemoved)->m_Girl.get();
        if (girl->is_slave()) continue;                    // nothing to do if she's _already_ enslaved
        if (girl->is_dead()) { deadcount++; continue; }    // likewise, dead girls can't be enslaved

        auto result = AttemptEscape(*girl);
        switch(result) {
        case EGirlEscapeAttemptResult::SUBMITS:
            set_slave_stats(girl);
            ss.str("");
            ss << girl->FullName() << " submits the the enchanted slave tattoo being placed upon her.";
            push_message(ss.str(), 0);
            break;
        case EGirlEscapeAttemptResult::STOPPED_BY_GOONS:
            g_Game->player().evil(5);    // evil up the player for doing a naughty thing and adjust the girl's stats
            set_slave_stats(girl);
            ss.str("");
            ss << girl->FullName() << " puts up a fight "
               << "but your goons control her as the enchanted slave tattoo is placed upon her.";
            push_message(ss.str(), COLOR_RED);    // and queue the message
            break;
        case EGirlEscapeAttemptResult::STOPPED_BY_PLAYER:
            // adjust the girl's stats to reflect her new status and then evil up the player because he forced her into slavery
            g_Game->player().evil(5);
            set_slave_stats(girl);
            ss.str("");
            ss << girl->FullName()
               << " breaks free from your goons' control. You restrain her personally while the slave tattoo placed upon her.";
            push_message(ss.str(), COLOR_RED);
            break;
        case EGirlEscapeAttemptResult::SUCCESS:
            ss << "After defeating you goons and you, she escapes to the outside.\n";
            ss << "She will escape for good in 6 weeks if you don't send someone after her.";
            g_Game->dungeon().RemoveGirl(girl);
            numGirlsRemoved++;
            girl->run_away();
            g_Game->player().suspicion(15);                    // suspicion goes up
            g_Game->player().evil(15);                        // so does evil
            push_message(ss.str(), COLOR_RED);    // add to the message queue
        }
    }
    if (deadcount > 0) push_message("There's not much point in using a slave tattoo on a dead body.", 0);
    init(false);
    return Return;
}

void cScreenDungeon::release_all_customers()
{
    // loop until we run out of customers
    while (g_Game->dungeon().GetNumCusts() > 0)
    {
        sDungeonCust* cust = g_Game->dungeon().GetCust(0);        // get the first customer in the list
        g_Game->dungeon().RemoveCust(cust);                        // remove from brothel
        // de-evil the player for being nice suspicion drops too
        g_Game->player().evil(-5);
        g_Game->player().suspicion(-5);
    }
    selection = -1;
    init(false);
}

void cScreenDungeon::sell_slaves()
{
    int paid = 0, count = 0, deadcount = 0;
    std::vector<int> girl_array;
    get_selected_girls(&girl_array);  // get and sort array of girls/customers
    std::vector<std::string> girl_names;
    std::vector<int> sell_gold;
    for (int i = girl_array.size(); i-- > 0;)
    {
        selection = girl_array[i];
        if ((selection - g_Game->dungeon().GetNumGirls()) >= 0) continue;    // if this is a customer, we skip to the next list entry

        sGirl *girl = g_Game->dungeon().GetGirl(selection)->m_Girl.get();                                // and get the sGirl

        if (!girl->is_slave()) continue;                    // if she's not a slave, the player isn't allowed to sell her
        if (girl->is_dead())                                        // likewise, dead slaves can't be sold
        {
            deadcount++;
            continue;
        }
        // she's a living slave, she's out of here
        cGirls::UpdateAskPrice(*girl, false);
        int cost = g_Game->tariff().slave_sell_price(*girl);                    // get the sell price of the girl. This is a little on the occult side
        g_Game->gold().slave_sales(cost);
        paid += cost;
        count++;
        auto removed_girl = g_Game->dungeon().RemoveGirl(g_Game->dungeon().GetGirl(selection));    // remove her from the dungeon, add her back into the general pool
        girl_names.push_back(girl->FullName());
        sell_gold.push_back(cost);
        g_Game->girl_pool().GiveGirl(removed_girl);
    }
    if (deadcount > 0) push_message("Nobody is currently in the market for dead girls.", COLOR_YELLOW);
    if (count <= 0) return;

    ss.str(""); ss << "You sold ";
    if (count == 1)        { ss << girl_names[0] << " for " << sell_gold[0] << " gold."; }
    else
    {
        ss << count << " slaves:";
        for (int i = 0; i < count; i++)
        {
            ss << "\n    " << girl_names[i] << "   for " << sell_gold[i] << " gold";
        }
        ss << "\nFor a total of " << paid << " gold.";
    }
    push_message(ss.str(), 0);
    selection = -1;
    init(false);
}

void cScreenDungeon::release_all_girls()
{
    while (g_Game->dungeon().GetNumGirls() > 0)         // loop until there are no more girls to release
    {
        if (m_ReleaseBuilding->free_rooms() > 0)         // make sure there's room for another girl
        {
            /// TODO(performance) always removing the first element might be very inefficient
            g_Game->dungeon().ReleaseGirl(0, *m_ReleaseBuilding);
            continue;
        }
        // we only get here if we run out of space
        push_message("There is no more room in the current building.\nBuild more rooms, get rid of some girls, or change the building you are currently releasing girls to.", 0);
        break;
    }
}

void cScreenDungeon::stop_feeding()
{
    // and then loop using multi_first() and multi_next()
    for (int selection = multi_first(); selection != -1; selection = multi_next())
    {
        g_Game->dungeon().SetFeeding(selection, false);
    }
}

void cScreenDungeon::start_feeding()
{
    int pos = 0;
    selection = GetNextSelectedItemFromList(girllist_id, 0, pos);
    while (selection != -1)
    {
        g_Game->dungeon().SetFeeding(selection, true);
        selection = GetNextSelectedItemFromList(girllist_id, pos + 1, pos);
    }
}

void cScreenDungeon::torture_customer(int girls_removed)
{
    ss.str("Customer: ");
    int cust_index = selection - g_Game->dungeon().GetNumGirls() + girls_removed;    // get the index number for the customer
    sDungeonCust* cust = g_Game->dungeon().GetCust(cust_index);                        // get the customer record from the dungeon

    if (!cust) return;
    if (cust->m_Tort && !g_Game->allow_cheats())         // don't let the PL torture more than once a day (unless cheating is enabled)
    {
        ss << "You may only torture someone once per week.";
        push_message(ss.str(), 0);
        return;
    }
    cust->m_Tort = true;        // flag the customer as tortured, decrement his health
    cust->m_Health -= 6;
    ss << "Screams fill the dungeon ";
    if (cust->m_Health > 0)
    {
        ss << "until the customer is battered, bleeding and bruised.\nYou leave them sobbing "
            << (cust->m_Health >= 30 ? "uncontrollably." : "and near to death.");
    }
    else
    {
        cust->m_Health = 0;
        ss<<" gradually growing softer until it stops completely.\nThey are dead.";
        g_Game->player().evil(2);
    }
    push_message(ss.str(), 0);
}

/*
* If we have a multiple selection, then the torture button
* should be enabled if just one of the selected rows can
* be tortured
*/
bool cScreenDungeon::torture_possible()
{
    int nSelection;        // don't use selection for the loop - its a class global and can mess things up elsewhere
    int    nPosition = 0;
    int nNumGirls = g_Game->dungeon().GetNumGirls();
    for (nSelection = GetNextSelectedItemFromList(girllist_id, 0, nPosition); nSelection != -1; nSelection = GetNextSelectedItemFromList(girllist_id, nPosition + 1, nPosition))
    {
        bool not_yet_tortured;
        // get the customer or girl under selection and find out if they've been tortured this turn
        if (nSelection >= nNumGirls)
        {
            sDungeonCust* dcust = g_Game->dungeon().GetCust(nSelection - nNumGirls);
            if(dcust)
                not_yet_tortured = !dcust->m_Tort;
        }
        else
        {
            sDungeonGirl* dgirl = g_Game->dungeon().GetGirl(nSelection);
            if(dgirl)
                not_yet_tortured = !dgirl->m_Girl->m_Tort;
        }
        if (not_yet_tortured) return true;    // we only need one torturable prisoner so if we found one, we can go home
    }
    return false;                            // we only get here if no-one in the list was torturable
}

void cScreenDungeon::torture()
{
    int pos = 0;
    int numGirlsRemoved = 0;
    store_selected_girls();

    for (selection = GetNextSelectedItemFromList(girllist_id, 0, pos); selection != -1; selection = GetNextSelectedItemFromList(girllist_id, pos + 1, pos))
    {
        // if it's a customer, we have a separate routine
        if ((selection - (g_Game->dungeon().GetNumGirls() + numGirlsRemoved)) >= 0)
        {
            g_Game->player().evil(5);
            torture_customer(numGirlsRemoved);
            continue;
        }
        // If we get here, it's a girl
        sDungeonGirl* dgirl = g_Game->dungeon().GetGirl(selection - numGirlsRemoved);
        cGirlTorture gt(dgirl);
    }
}

void cScreenDungeon::change_release(BuildingType building, int index)
{
    m_ReleaseBuilding = g_Game->buildings().building_with_type(building, index);
    init(false);
}

void cScreenDungeon::release()
{
    std::vector<int> girl_array;
    get_selected_girls(&girl_array);            // get and sort array of girls/customers
    for (int i = girl_array.size(); i--> 0;)
    {
        selection = girl_array[i];
        // check in case its a customer
        if ((selection - g_Game->dungeon().GetNumGirls()) >= 0)
        {
            int num = selection - g_Game->dungeon().GetNumGirls();
            sDungeonCust* cust = g_Game->dungeon().GetCust(num);
            g_Game->dungeon().RemoveCust(cust);
            // player did a nice thing: suss and evil go down :)
            g_Game->player().suspicion(-5);
            g_Game->player().evil(-5);
            continue;
        }

        // if there's room, remove her from the dungeon and add her to the current brothel
        int num = selection;
        if ((m_ReleaseBuilding->free_rooms()) > 0)
        {
            g_Game->dungeon().ReleaseGirl(num, *m_ReleaseBuilding);
            continue;
        }
        // if we run out of space
        push_message("The current building has no more room.\nBuy a new one, get rid of some girls, or change the building you are currently releasing girls to.", 0);
        break;        // no point in running round the loop any further we're out of space
    }
}

void cScreenDungeon::talk()
{
    if (g_TalkCount <= 0) return;    // if we have no talks left, we can go home
    // customers are always last in the list, so we can determine if this is a customer by simple aritmetic
    if ((selection - g_Game->dungeon().GetNumGirls()) >= 0) return;        // it is a customer

    // OK, it wasn't a customer
    int num = selection;
    sDungeonGirl* girl = g_Game->dungeon().GetGirl(num);

    g_Game->TalkToGirl(*girl->m_Girl);

}

void cScreenDungeon::update_image()
{
    if ((selection - g_Game->dungeon().GetNumGirls()) >= 0)    // Makes it so when on a customer it doesnt keep the last girls pic up
    {
        HideWidget(girlimage_id, true);
    }
    else if (selected_girl() && !IsMultiSelected(girllist_id))
    {
        UpdateImage(selected_girl()->m_Tort ? IMGTYPE_TORTURE : IMGTYPE_JAIL);
    }
    else
    {
        HideWidget(girlimage_id, true);
    }
}

void cScreenDungeon::UpdateImage(int imagetype) {
    PrepareImage(girlimage_id, selected_girl().get(), imagetype, true, ImageNum);
    HideWidget(girlimage_id, false);
}

void cScreenDungeon::get_selected_girls(std::vector<int> *girl_array)
{  // take passed vector and fill it with sorted list of girl/customer IDs
    int pos = 0;
    int GSelection = GetNextSelectedItemFromList(girllist_id, 0, pos);
    while (GSelection != -1)
    {
        girl_array->push_back(GSelection);
        GSelection = GetNextSelectedItemFromList(girllist_id, pos + 1, pos);
    }
    sort(girl_array->begin(), girl_array->end());
}

void cScreenDungeon::store_selected_girls()
{  // save list of multi-selected girls
    select_girls.clear();
    get_selected_girls(&select_girls);
    if (select_girls.empty()) return;

    // we're not really interested in customers here
    while (select_girls.back() >= g_Game->dungeon().GetNumGirls())
    {
        select_girls.pop_back();
        if (select_girls.empty()) break;
    }
}
