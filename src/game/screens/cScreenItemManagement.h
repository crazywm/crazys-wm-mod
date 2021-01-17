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

class sInventoryItem;

struct sItemTransferSide {
    int sell10_id;
    int sellall_id;
    int buy10_id;
    int shift_id;
    int owners_id;
    int items_id;
    int equip_id;
    int unequip_id;
    int detail_id;      // `J` Added for .06.02.39

    std::vector<const sInventoryItem*> items;
    int selected_owner = -1;
    int selected_item = -2;
};

class IInventoryProvider {
    // everything to which an inventory item can be transferred
public:
    virtual ~IInventoryProvider() = default;
    virtual std::vector<FormattedCellData> get_data(int filter) const = 0;
    virtual std::string get_details() const { return "-"; }
    virtual void enumerate_items(const std::function<void(const sInventoryItem *, int)> &callback) const = 0;

    virtual int take_item(const sInventoryItem *item, int amount) = 0;
    virtual int give_item(const sInventoryItem *item, int amount) = 0;
    virtual bool equippable(const sInventoryItem *item, bool equip) const = 0;
};

class cScreenItemManagement : public cGameWindow
{
private:
    enum Side {
        Left = 0,
        Right = 1
    };

    int curbrothel_id;    // Current Brothel text
    int gold_id;        // Player Gold text
    int desc_id;        // Item Description text
    int filter_id;        // Filter list
    int autouse_id;        // Autouse items checkbox

    void load_ids(sItemTransferSide& target, Side side);
    void init_side(sItemTransferSide& target, int owner, int item);

    sItemTransferSide m_LeftData;
    sItemTransferSide m_RightData;

    void set_ids() override;

    std::vector<std::unique_ptr<IInventoryProvider>> m_OwnerList;

    void AddGirlsFromBuilding(IBuilding * brothel);
public:
    cScreenItemManagement();

    void init(bool back) override;
    void process() override {};
    void update_button_states(Side side);

    void attempt_transfer(Side transfer_from, int num = 1);
    void refresh_item_list(Side which_list);
    static std::string GiveItemText(int goodbad, int HateLove, const sGirl& targetgirl, const std::string& ItemName = "");

    void write_item_text(const sInventoryItem *item, int owner, int target);

    void change_equip(Side, bool equip);

    void on_select_filter(int selection);
    void on_select_item(Side side, int selected);

    void sync_owner_selection(const sItemTransferSide& side);

    void update_details(const sItemTransferSide& target);
};
