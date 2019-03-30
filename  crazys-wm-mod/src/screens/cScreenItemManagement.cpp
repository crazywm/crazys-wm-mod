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
#include "buildings/cBrothel.h"
#include "cScreenItemManagement.h"
#include "widgets/cListBox.h"
#include "widgets/cTextItem.h"
#include "cTariff.h"
#include "cWindowManager.h"
#include "InterfaceGlobals.h"
#include "InterfaceProcesses.h"
#include "sConfig.h"
#include "cGirls.h"
#include "main.h"
#include "Game.hpp"
#include "cInventory.h"


static bool AutoUseItems = false;


struct cInventoryProviderPlayer : public IInventoryProvider {
    std::array<std::string, 3> get_data(int filter) const override {
        stringstream ss, ss2;
        int num_items = g_Game.player().inventory().get_num_items();
        if (num_items > 0) ss << num_items;
        int numtype = g_Game.player().inventory().get_num_classes(filter);
        if (numtype > 0) ss2 << numtype;

        return std::array<std::string, 3> {"Player", ss.str(), ss2.str()};
    }

    void enumerate_items(const std::function<void(sInventoryItem *, int)>& callback) const override
    {
        for(auto entry : g_Game.player().inventory().all_items()) {
            callback(entry.first, entry.second);
        }
    }

    int take_item(sInventoryItem * item, int amount) override {
        return g_Game.player().inventory().remove_item(item, amount);
    }

    int give_item(sInventoryItem* item, int amount) override {
        // Add the item to players inventory
        int got = g_Game.player().inventory().add_item(item, amount);
        if (got == amount)
        {
            g_Game.push_message("Your inventory is full.", 1);
        }
        return amount - got;
    }
};

struct cInventoryProviderShop : public IInventoryProvider {
    std::array<std::string, 3> get_data(int filter) const override {
        return std::array<std::string, 3> {"Shop", "", ""};
    }

    void enumerate_items(const std::function<void(sInventoryItem *, int)>& callback) const override
    {
        for (int i = 0; i < NUM_SHOPITEMS; i++)
        {
            /// TODO shop item count
            sInventoryItem * item = g_Game.inventory_manager().GetShopItem(i);
            if (item) { callback(item, 1);  }
        }
    }

    int take_item(sInventoryItem * item, int amount) override {
        int cost = item->m_Cost;
        int bought = 0;
        if (g_Game.gold().afford(cost*amount))
        {
            while(bought < amount)
            {
                int got = g_Game.inventory_manager().BuyShopItem(item, amount);
                g_Game.gold().item_cost(got * cost);
                bought += got;
                if(got == 0)
                    break;
            }
        }
        return bought;
    }

    int give_item(sInventoryItem* item, int amount) override {
        g_Game.gold().item_sales(amount * item->m_Cost);
        return 0;
    }
};



struct cInventoryProviderGirl : public IInventoryProvider {
    explicit cInventoryProviderGirl(sGirl* girl) : m_Girl(girl) { }
    std::array<std::string, 3> get_data(int filter) const override {
        std::array<std::string, 3> data;

        data[0] = m_Girl->m_Realname;

        if (m_Girl->m_NumInventory > 0)
            data[1] = std::to_string(m_Girl->m_NumInventory);

        int numtype = cGirls::GetNumItemType(m_Girl, filter);
        if (numtype > 0) data[2] = std::to_string(numtype);

        return data;
    }

    std::string get_details(int font_size) const override
    {
        return cGirls::GetSimpleDetails(m_Girl, font_size);
    }

    void enumerate_items(const std::function<void(sInventoryItem *, int)>& callback) const override
    {
        for (auto & item : m_Girl->m_Inventory) {
            if (item) { callback(item, 1); }
        }
    }

    int take_item(sInventoryItem * item, int amount) override {
        // find the item
        auto found = std::find(begin(m_Girl->m_Inventory), end(m_Girl->m_Inventory), item);
        if(found == end(m_Girl->m_Inventory))
            return 0;
        int selection = std::distance(begin(m_Girl->m_Inventory), found);

        if (m_Girl->m_EquipedItems[selection] == 1)	// unequip item if it is equiped
            g_Game.inventory_manager().Unequip(m_Girl, selection);
        auto sold_item = m_Girl->m_Inventory[selection];
        if (sold_item->m_Badness >= 20)	// happy to get rid of bad items
            m_Girl->happiness(5);
        else	// sad to see good items go
        {
            int happiness = g_Game.inventory_manager().HappinessFromItem(sold_item);
            m_Girl->happiness(-happiness);
        }

        // remove the item
        m_Girl->m_Inventory[selection] = nullptr;
        m_Girl->m_EquipedItems[selection] = 0;
        m_Girl->m_NumInventory--;

        return 1;
    }

    int give_item(sInventoryItem* item, int amount) override {
        while(amount > 0) {
            if (cGirls::IsInvFull(m_Girl)) {
                g_Game.push_message("Her inventory is full", 0);
                return amount;
            }

            int   goodbad  = item->m_Badness;
            u_int type     = item->m_Type;
            int   HateLove = m_Girl->pclove() - m_Girl->pchate();
            g_Game.push_message(cScreenItemManagement::GiveItemText(goodbad, HateLove, m_Girl), 0);

            if (goodbad < 20) {
                int happiness = g_Game.inventory_manager().HappinessFromItem(item);

                m_Girl->obedience(1);
                m_Girl->happiness(happiness);
                m_Girl->pchate(-2);
                m_Girl->pclove(happiness - 1);
                m_Girl->pcfear(-1);
            }

            if (!AutoUseItems && (type == INVFOOD || type == INVMAKEUP))
                m_Girl->add_inv(item);
            else
                g_Game.inventory_manager().Equip(m_Girl, m_Girl->add_inv(item), false);

            --amount;
        }
        return 0;
    }

    sGirl* m_Girl;
};

extern bool g_AllTogle;
extern cConfig cfg;
extern bool playershopinventory;
extern sGirl *g_selected_girl;
extern cTariff tariff;

extern	bool	g_AltKeys;	// New hotkeys --PP

cScreenItemManagement::cScreenItemManagement() : cInterfaceWindowXML("itemmanagement_screen.xml")
{

}

static int filter = 0;
static int filterpos = 0;

int HateLove = 0;

static SDL_Color* RarityColor[9];

void cScreenItemManagement::load_ids(sItemTransferSide& target, Side side)
{
    std::string side_str = side == Side::Left ? "Left" : "Right";
    Side opposite = side == Side::Left ? Side::Right : Side::Left;
    target.sell10_id    = get_id("Sell10" + side_str + "Button");
    target.sellall_id   = get_id("SellAll" + side_str + "Button");
    target.buy10_id     = get_id("Buy10" + side_str + "Button");
    target.shift_id     = get_id("Shift" + side_str + "Button");
    target.equip_id     = get_id("Equip" + side_str + "Button");
    target.unequip_id   = get_id("Unequip" + side_str + "Button");
    target.owners_id    = get_id("Owners" + side_str + "List");
    target.items_id     = get_id("Items" + side_str + "List");
    target.detail_id     = get_id("Owners" + side_str + "Details", "*Optional*");

    string ORColumns[] = { "ORLName", "ORLNumber", "ORLCatNum" };
    string OLColumns[] = { "OLLName", "OLLNumber", "OLLCatNum" };
    if(side == Side::Left) {
        SortColumns(target.owners_id, OLColumns, 3);
    } else {
        SortColumns(target.owners_id, ORColumns, 3);
    }

    SetButtonCallback(target.buy10_id, [this, opposite]() { attempt_transfer(opposite, 10); });
    SetButtonCallback(target.sell10_id, [this, side]() { attempt_transfer(side, 10); });
    SetButtonCallback(target.sellall_id, [this, side]() { attempt_transfer(side, 999); });
    SetButtonCallback(target.shift_id, [this, opposite]() { attempt_transfer(opposite); });
    SetButtonCallback(target.equip_id, [this, side]() { change_equip(side, true); });
    SetButtonCallback(target.unequip_id, [this, side]() { change_equip(side, false); });
    SetListBoxSelectionCallback(target.owners_id, [this, side](int selection) { refresh_item_list(side); });
    SetListBoxSelectionCallback(target.items_id, [this, side](int selection) { on_select_item(side, selection); });
}


void cScreenItemManagement::set_ids()
{
	curbrothel_id	= get_id("CurrentBrothel");
	back_id			= get_id("BackButton","Back");
    gold_id			= get_id("PlayerGold");
    desc_id			= get_id("ItemDesc");
    filter_id		= get_id("FilterList");
    autouse_id		= get_id("AutoUseItems");
    load_ids(m_LeftData, Side::Left);
    load_ids(m_RightData, Side::Right);

	for (int i = 0; i < 9; i++) RarityColor[i] = cfg.items.rarity_color(i);

    SetButtonNavigation(back_id, "<back>");

    SetListBoxHotKeys(m_LeftData.owners_id, SDLK_t, SDLK_g);
    SetListBoxHotKeys(m_RightData.owners_id, SDLK_y, SDLK_h);
    SetListBoxHotKeys(m_LeftData.items_id, SDLK_u, SDLK_j);
    SetListBoxHotKeys(m_RightData.items_id, SDLK_i, SDLK_k);
    SetListBoxSelectionCallback(filter_id, [this](int selection) { on_select_filter(selection); });
    SetListBoxHotKeys(filter_id, SDLK_r, SDLK_f);

    SetCheckBoxCallback(autouse_id, [this](bool on) { AutoUseItems = on; });
}

void cScreenItemManagement::init_side(sItemTransferSide& target, int owner, int item)
{
    ClearListBox(target.owners_id);
    ClearListBox(target.items_id);

    for(std::size_t i = 0; i < m_OwnerList.size(); ++i) {
        /// TODO define colors
        AddToListBox(target.owners_id, i, m_OwnerList[i]->get_data(filter).data(), 3, COLOR_BLUE);
    }

    if (m_ListBoxes[target.owners_id]->GetSelected() != owner)		SetSelectedItemInList(target.owners_id, owner);
    SetSelectedItemInList(target.items_id, item);

    update_details(target);

    // disable the equip/unequip buttons
    DisableButton(target.equip_id, true);
    DisableButton(target.unequip_id, true);
}

void cScreenItemManagement::update_details(const sItemTransferSide& target)
{
    if (target.detail_id > -1)
    {
        int index = GetSelectedItemFromList(target.owners_id);
        if(index >= 0) {
            EditTextItem(m_OwnerList.at(index)->get_details(m_TextItems[target.detail_id]->m_FontHeight),
                         target.detail_id);
        } else {
            EditTextItem("", target.detail_id);
        }
    }
}

void cScreenItemManagement::init(bool back)	// `J` bookmark
{
	Focused();

	string brothel = "Current Brothel: ";
	brothel += active_building().name();
	EditTextItem(brothel, curbrothel_id);

	if (playershopinventory)	// `J` to set player and shop when pressing ctrl-I to get to inventory
	{
		if (m_LeftData.selected_owner != 0)								                m_LeftData.selected_item = -1;
		if (m_LeftData.selected_item < 0 && !g_Game.player().inventory().empty())		m_LeftData.selected_item = 0;
		if (m_RightData.selected_owner != 1)								m_RightData.selected_item = -1;
		if (m_RightData.selected_item < 0 && g_Game.inventory_manager().GetShopItem(0))	m_RightData.selected_item = 0;
		m_LeftData.selected_owner = 0;	m_RightData.selected_owner = 1;
	}
	playershopinventory = false;

    if (m_RightData.selected_owner < 0)	m_RightData.selected_owner = 1;
    if (m_LeftData.selected_owner < 0)	m_LeftData.selected_owner = (m_RightData.selected_owner == 0 ? 1 : 0);

	// clear list boxes
	ClearListBox(filter_id);

	// setup the filter
	AddToListBox(filter_id, 0, "All");
	AddToListBox(filter_id, INVFOOD, "Consumables");
	AddToListBox(filter_id, INVMISC, "Misc");
	AddToListBox(filter_id, INVHAT, "Hats");
	AddToListBox(filter_id, INVGLASSES, "Glasses");
	AddToListBox(filter_id, INVNECKLACE, "Necklaces");
	AddToListBox(filter_id, INVARMBAND, "Armbands");
	AddToListBox(filter_id, INVRING, "Rings");
	AddToListBox(filter_id, INVDRESS, "Dress");
	AddToListBox(filter_id, INVUNDERWEAR, "Underwear");
	AddToListBox(filter_id, INVSWIMSUIT, "Swimsuits");
	AddToListBox(filter_id, INVSHOES, "Shoes");
	AddToListBox(filter_id, INVSMWEAPON, "Small Weapons");
	AddToListBox(filter_id, INVWEAPON, "Large Weapons");
	AddToListBox(filter_id, INVHELMET, "Helmets");
	AddToListBox(filter_id, INVARMOR, "Armor");
	AddToListBox(filter_id, INVSHIELD, "Shields");
	AddToListBox(filter_id, INVCOMBATSHOES, "Combat Shoes");

	if (filter < 0) filter = 0;
	SetSelectedItemInList(filter_id, filter, false);
	SetListTopPos(filter_id, filterpos);
	
    // shop and player
    m_OwnerList.clear();
    m_OwnerList.push_back(std::make_unique<cInventoryProviderPlayer>());
    m_OwnerList.push_back(std::make_unique<cInventoryProviderShop>());

    // and girls from current brothel to list
    for(auto& b : g_Game.buildings().buildings()) {
        AddGirlsFromBuilding(b.get());
    }

	// add current dungeon girls to list
	for(sDungeonGirl& girl : g_Game.dungeon().girls())
	{
	    int i = m_OwnerList.size();
	    if (g_AllTogle && g_selected_girl == girl.m_Girl.get()) m_RightData.selected_owner = i;
		m_OwnerList.push_back(std::make_unique<cInventoryProviderGirl>(girl.m_Girl.get()));
	}

    init_side(m_LeftData, 0, 0);
    init_side(m_RightData, 0, 0);

	g_AllTogle = false;

    update_button_states(Right);
}

void cScreenItemManagement::AddGirlsFromBuilding(IBuilding * brothel) {
    for(auto& temp : brothel->girls()) {
        int i = m_OwnerList.size();
        if (g_AllTogle && g_selected_girl == temp) m_RightData.selected_owner = i;
        m_OwnerList.push_back(std::make_unique<cInventoryProviderGirl>(temp));
    }
}

void cScreenItemManagement::write_item_text(sInventoryItem * item, int owner, int target)
{
	stringstream ss;
	stringstream iName;
	stringstream iCost;
	stringstream iSell;
	stringstream iType;
	stringstream iDesc;

	if (item == nullptr || owner < 0) {}
	else
	{
		iName << item->m_Name;
		iCost << item->m_Cost << " gold";
		iSell << int(((float)item->m_Cost) * cfg.in_fact.item_sales()) << " gold";
		iType << item->m_Type;
		iDesc << item->m_Desc;
	}
	if (owner != 1 && owner >= 0 && target == 1)
	{
		cFont check; int w, h, size = int(m_TextItems[desc_id]->GetWidth()*0.25);
		check.LoadFont(cfg.fonts.normal(), cfg.fonts.detailfontsize());
		check.GetSize(iCost.str(), w, h); while (w < size) { iCost << " "; check.GetSize(iCost.str(), w, h); }
	}
	ss << "Item Name:      " << iName.str();
	ss << "\nCost:  " << iCost.str();
	if (owner != 1 && owner >= 0 && target == 1) ss << "Sell for:  " << iSell.str();
	ss << "\nType:  " << iType.str();
	ss << "\n \n" << iDesc.str();

	EditTextItem(ss.str(), desc_id);
};

void cScreenItemManagement::on_select_item(Side side, int selected)
{
    auto& own_side = side == Side::Left ? m_LeftData : m_RightData;
    auto& other_side = side == Side::Right ? m_LeftData : m_RightData;
    own_side.selected_item = GetLastSelectedItemFromList(own_side.items_id);

    //DisableButton(own_side.shift_id, (selected < 0) || (own_side.selected_owner == 1 && !g_Game.gold().afford(g_Game.inventory_manager().GetShopItem(selected)->m_Cost)));

    bool disablebuy10L = true;
    if (other_side.selected_owner == 0 && own_side.selected_owner == 1 && selected > -1 &&
        g_Game.inventory_manager().GetShopItem(selected) &&
        g_Game.gold().afford(g_Game.inventory_manager().GetShopItem(selected)->m_Cost * 10))
    {
        disablebuy10L = !g_Game.inventory_manager().GetShopItem(selected)->m_Infinite;
    }
    DisableButton(own_side.buy10_id, disablebuy10L);

    if (selected != -1)
    {
        /*
        sInventoryItem * item;
        if (own_side.selected_owner == 0) // Shop
        {
            item =  own_side.items[selected];
            DisableButton(own_side.equip_id, true);
            DisableButton(own_side.unequip_id, true);
        }
        else if (own_side.selected_owner == 1) // Player
        {
            item = g_Game.inventory_manager().GetShopItem(selected);
            DisableButton(own_side.equip_id, true);
            DisableButton(own_side.unequip_id, true);
        }
        else // Girl
        {
            sGirl* targetGirl = GirlSelectedFromList(own_side.selected_owner);
            item = targetGirl->m_Inventory[selected];
            HateLove = targetGirl->pclove() - targetGirl->pchate();

            if (g_Game.inventory_manager().IsItemEquipable(targetGirl->m_Inventory[selected]))
            {
                DisableButton(own_side.equip_id, targetGirl->m_EquipedItems[selected] == 1);
                DisableButton(own_side.unequip_id, targetGirl->m_EquipedItems[selected] != 1);
            }
            else
            {
                DisableButton(own_side.equip_id, true);
                DisableButton(own_side.unequip_id, true);
            }
        }

        write_item_text(item, own_side.selected_owner, other_side.selected_owner);
         */
    }
    else
    {
        DisableButton(own_side.equip_id, true);
        DisableButton(own_side.unequip_id, true);
    }
}

void cScreenItemManagement::on_select_filter(int selection)
{
    filter    = selection;
    filterpos = m_ListBoxes[filter_id]->m_Position;
    sync_owner_selection(m_LeftData);
    sync_owner_selection(m_RightData);
    init(false);
}

void cScreenItemManagement::sync_owner_selection(const sItemTransferSide& side)
{
    if (m_ListBoxes[side.owners_id]->GetSelected() != side.selected_owner)
        SetSelectedItemInList(side.owners_id, side.selected_owner);
}

void cScreenItemManagement::change_equip(Side side, bool equip)
{
    sItemTransferSide& own_side = side == Side::Left ? m_LeftData : m_RightData;
    sItemTransferSide& other_side = side == Side::Right ? m_LeftData : m_RightData;

    sGirl * targetGirl = dynamic_cast<cInventoryProviderGirl&>(*m_OwnerList.at(own_side.owners_id)).m_Girl;

    HateLove = targetGirl->pclove() - targetGirl->pchate();

    int item = GetLastSelectedItemFromList(own_side.items_id);
    if (item != -1)
    {
        if(equip) {
            g_Game.inventory_manager().Equip(targetGirl, item, true);
        } else {
            g_Game.inventory_manager().Unequip(targetGirl, item);
        }
        DisableButton(own_side.equip_id, equip);
        DisableButton(own_side.unequip_id, !equip);
        sync_owner_selection(own_side);
        sync_owner_selection(other_side);
    }
    refresh_item_list(side);
    update_button_states(Right);
}

void cScreenItemManagement::update_button_states(Side side)
{
    sItemTransferSide& data = (side == Side::Left) ? m_LeftData : m_RightData;
    sItemTransferSide& other_data = (side == Side::Right) ? m_LeftData : m_RightData;
    int item        = other_data.selected_item;
    
	// check the shop for infinite items and check Player for multiples of the same item
	int disable_shift   = 0;		// 0 = hidden
	int disable_buy10   = 0;		// 1 = off
	int disable_sell10  = 0;		// 2 = on
	int disable_sellall = 0;	//
	
	disable_shift = (item > -1 ? 2 : 1);

	if (data.selected_owner == 0/*Player*/ && other_data.selected_owner == 1/*Shop*/)
	{
        disable_buy10 = (item > -1 && g_Game.inventory_manager().GetShopItem(item) &&
                         g_Game.inventory_manager().GetShopItem(item)->m_Infinite &&
                         g_Game.gold().afford(g_Game.inventory_manager().GetShopItem(item)->m_Cost * 10) ? 2 : 1);
        disable_shift = (item > -1 && g_Game.inventory_manager().GetShopItem(item) &&
                         g_Game.gold().afford(g_Game.inventory_manager().GetShopItem(item)->m_Cost) ? 2 : 1);
	}
	if (data.selected_owner == 1/*Shop*/ && other_data.selected_owner == 0/*Player*/)
	{
        disable_sell10  = (item > -1 ? 2 : 1);
        if(item < 0 || other_data.items.size() <= item) {
            disable_sellall = 1;
        } else {
            disable_sellall = (g_Game.player().inventory().get_num_items(other_data.items.at(item)) > 0 ? 2 : 1);
        }
	}
	DisableButton(data.shift_id, disable_shift <= 1);
	HideButton(data.buy10_id, disable_buy10 == 0);			DisableButton(data.buy10_id, disable_buy10 <= 1);
	HideButton(data.sell10_id, disable_sell10 == 0);		DisableButton(data.sell10_id, disable_sell10 <= 1);
	HideButton(data.sellall_id, disable_sellall == 0);		DisableButton(data.sellall_id, disable_sellall <= 1);

    if (GetSelectedItemFromList(data.items_id) < 0)
    {
        DisableButton(data.equip_id, true);
        DisableButton(data.unequip_id, true);
    }
}

void cScreenItemManagement::refresh_item_list(Side which_list)
{
	// good enough place as any to update the cost shown on the screen
	string temp = "PLAYER GOLD: ";
	temp += g_Game.gold().sval();
	EditTextItem(temp, gold_id);

    sItemTransferSide& data = (which_list == Side::Left) ? m_LeftData : m_RightData;
    sItemTransferSide& other = (which_list == Side::Right) ? m_LeftData : m_RightData;

	data.items.clear();

	update_details(m_LeftData);
	update_details(m_RightData);

	int sel_pos = GetSelectedItemFromList(data.items_id);
	std::string sel_name;

	ClearListBox(data.items_id);
	int selection = GetSelectedItemFromList(data.owners_id);
	int& owner = data.selected_owner;
	if (selection == other.selected_owner) SetSelectedItemInList(data.owners_id, owner);
	else if (selection != -1)
	{
	    int i = 0;
        m_OwnerList[selection]->enumerate_items([this, &data, &i](sInventoryItem * item, int amount) {
            int          ItemColor = -1;
            stringstream it;
            it << item->m_Name;
            if(amount > 1 && amount < 999) {
                it << " (" << amount << ")";
            }
            int item_type = item->m_Type;
            if ((filter == 0)  // unfiltered?
                || (item_type == filter)  // matches filter exactly?
                || ((filter == INVFOOD) && (item_type == INVMAKEUP))  // passes "consumable" filter?
                    ) {  // passed the filter, so add it

                // TODO fix this
                //if (sel_name == item->m_Name) {
                //    sel_pos = i;  // if we just transferred this item here, might want to select it
                //}
                AddToListBox(data.items_id, i, it.str());
                ItemColor = item->m_Rarity;
                ++i;
            }
            // todo shouldn't this only happen if the item is added to the list box?
            data.items.push_back(item);

            if (ItemColor > -1) SetSelectedItemTextColor(data.items_id, i, RarityColor[ItemColor]);
            i++;
        });
	}
	SortListItems(data.items_id, "");

	SetSelectedItemInList(data.items_id, sel_pos);
    data.selected_item = GetSelectedItemFromList(data.items_id);

	if (GetLastSelectedItemFromList(data.items_id) < 0)
	{
		write_item_text(nullptr, -1, -1);
		DisableButton(data.shift_id, true);
	}

    update_button_states(Left);
    update_button_states(Right);
}

void cScreenItemManagement::attempt_transfer(Side transfer_from, int num)
{
    sItemTransferSide& from_data = (transfer_from == Side::Left) ? m_LeftData : m_RightData;
    sItemTransferSide& to_data = (transfer_from == Side::Right) ? m_LeftData : m_RightData;

	if (from_data.selected_owner == -1 || to_data.selected_owner == -1) return;

	m_LeftData.selected_item  = GetLastSelectedItemFromList(m_LeftData.items_id);
	m_RightData.selected_item = GetLastSelectedItemFromList(m_RightData.items_id);

    int pos = 0;
    int selection = GetNextSelectedItemFromList(from_data.items_id, 0, pos);
    while (selection != -1)
    {
        // since items sold to shop are simply destroyed, no selection to track here
        sInventoryItem* item = from_data.items[selection];
        int sold = m_OwnerList[from_data.selected_owner]->take_item(item, num);
        int remaining = m_OwnerList[to_data.selected_owner]->give_item(item, sold);
        // give back remaining items to original owner
        remaining = m_OwnerList[from_data.selected_owner]->give_item(item, remaining);
        selection = GetNextSelectedItemFromList(from_data.items_id, pos + 1, pos);
    }


	// update the item lists
	sync_owner_selection(from_data);
	sync_owner_selection(to_data);

    init(false);
}

string cScreenItemManagement::GiveItemText(int goodbad, int HateLove, sGirl* targetgirl, string ItemName)
{
	sGirl* girl = targetgirl;
	string message = "";
	if (goodbad < 20)
	{
		if (HateLove < -80)     	message = "She grudgingly accepts the gift, but makes it clear that she still thinks that you rate slightly below a toad in her worldview.";
		else if (HateLove < -60)	message = "She takes your gift in hand, looks at it, looks at you, than walks away without a word.";
		else if (HateLove < -40)
		{
			if (girl->has_trait( "Your Daughter"))
			{
				message = "Are you trying to make up for being an ass dad?"; //hopefully this works.. will add more
			}
			else message = "You know, if you wanted to fuck, you shoulda just said so.  That way I'd have had more fun saying no.";
		}
		else if (HateLove < -20)
		{
			if (girl->has_trait( "Your Daughter"))
			{
				message = "You still have a long way to go if you want me to like you dad."; //hopefully this works.. will add more CRAZY
			}
			else message = "If you think giving me pretty things will get you between my legs, you're wrong!";
		}
		else if (HateLove < 0)		message = "She is shocked you would give her anything nice.";
		else if (HateLove < 20)		message = "She is happy with the gift.";
		else if (HateLove < 40)		message = "She is happy with the gift and thanks you.";
		else if (HateLove < 60)		message = "She is happy with the gift and gives you a big hug.";
		else if (HateLove < 80)		message = "She is happy with the gift and gives you a big hug and a kiss on the cheek.";
		else
		{
			if (girl->has_trait( "Your Daughter"))	message = "She is happy with the gift and gives you a big hug and a kiss on the cheek saying she loves her daddy."; //hopefully this works.. will add more
			else if (girl->has_trait( "Lesbian"))		message = "She is happy with the gift and gives you a big hug and a kiss on the cheek and says that if you weren't a \"man\" she might have to show you how much she loved that gift.";
			else /*                                   */	message = "She is happy with the gift and gives you a big hug and kisses you hard.  After the kiss she whispers to you to see her later so she can thank you \"properly\".";
		}
	}
	else
	{
		if (HateLove < -80)	        message = "'Wow, what's next?  A dead rat?  Your shit sucks almost as bad as your technique!";
		else if (HateLove < -60)	message = "She takes your gift in hand, looks at it, looks at you, than walks away without a word.";
		else if (HateLove < -40)	message = "Hey, present for you!' 'Fuck off and die!";
		else if (HateLove < -20)	message = "'Fuck off, and take your cheap garbage with you!";
		else if (HateLove < 0)		message = "She doesn't seem happy with the gift.  But has come to expect this kinda thing from you.";
		else if (HateLove < 20)		message = "She doesn't seem happy with the gift.";
		else if (HateLove < 40)		message = "She doesn't seem happy with the gift and looks a little sad.";
		else if (HateLove < 60)		message = "She doesn't seem happy with the gift and looks sad.";
		else if (HateLove < 80)		message = "She doesn't seem happy with the gift and tears can be seen in her eyes.";
		else
		{
			if (girl->has_trait( "Your Daughter"))	message = "She looks at you and says \"Why would you give me such a thing daddy?\"."; //hopefully this works.. will add more
			else /*                                   */	message = "She can't belive you would give her such a gift and runs off crying.";
		}
	}
	return message;
}

