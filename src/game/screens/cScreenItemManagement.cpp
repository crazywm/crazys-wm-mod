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
#include <sstream>
#include <xml/util.h>
#include "interface/cColor.h"
#include "cScreenItemManagement.h"
#include "buildings/cBuildingManager.h"
#include "buildings/cDungeon.h"
#include "character/cPlayer.h"
#include "widgets/IListBox.h"
#include "widgets/cTextItem.h"
#include "cTariff.h"
#include "interface/cWindowManager.h"
#include "interface/CGraphics.h"
#include "cGirls.h"
#include "IGame.h"
#include "cInventory.h"
#include "cShop.h"
#include "sConfig.h"
#include "CLog.h"

namespace settings {
    extern const char* MONEY_SELL_ITEM;
}


// TODO make this a config
static bool AutoUseItems = false;


struct cInventoryProviderPlayer: public IInventoryProvider {
    FormattedCellData get_data(int filter, const std::string& column) const override {
        if(column == "Name") {
            return mk_text("Player");
        } else if (column == "Number") {
            int num_items = g_Game->player().inventory().get_num_items();
            return mk_num(num_items);
        } else if (column == "CatNum") {
            int numtype = g_Game->player().inventory().get_num_of_type(filter);
            return mk_num(numtype);
        }
        assert(false);
    }

    void enumerate_items(const std::function<void(const sInventoryItem *, int)> &callback) const override
    {
        for(auto entry : g_Game->player().inventory().all_items()) {
            callback(entry.first, entry.second.count);
        }
    }

    int take_item(const sInventoryItem *item, int amount) override {
        return g_Game->player().inventory().remove_item(item, amount);
    }

    int give_item(const sInventoryItem *item, int amount) override {
        // Add the item to players inventory
        int got = g_Game->player().inventory().add_item(item, amount);
        if (got != amount)
        {
            g_Game->push_message("Your inventory is full.", 1);
        }
        return amount - got;
    }

    bool equippable(const sInventoryItem *item, bool equip) const override { return false; }
};

struct cInventoryProviderShop : public IInventoryProvider {
    FormattedCellData get_data(int filter, const std::string& column) const override {
        if(column == "Name") {
            return mk_text("Shop");
        } else if (column == "Number") {
            int num_items = g_Game->shop().GetInventory().get_num_items();
            return mk_num(num_items);
        } else if (column == "CatNum") {
            int numtype = g_Game->shop().GetInventory().get_num_of_type(filter);
            return mk_num(numtype);
        }
        assert(false);
    }

    void enumerate_items(const std::function<void(const sInventoryItem*, int)> &callback) const override
    {
        g_Game->shop().IterateItems(callback);
    }

    int take_item(const sInventoryItem *item, int amount) override {
        int cost = item->m_Cost;
        int bought = 0;
        if (g_Game->gold().afford(cost*amount))
        {
            while(bought < amount)
            {
                int got = g_Game->shop().BuyItem(item, amount);
                g_Game->gold().item_cost(got * cost);
                bought += got;
                if(got == 0)
                    break;
            }
        }
        return bought;
    }

    int give_item(const sInventoryItem *item, int amount) override {
        g_Game->gold().item_sales(amount * item->m_Cost);
        return 0;
    }

    bool equippable(const sInventoryItem *item, bool equip) const override { return false; }
};



struct cInventoryProviderGirl : public IInventoryProvider {
    explicit cInventoryProviderGirl(std::shared_ptr<sGirl> girl) : m_Girl(girl) { }
    FormattedCellData get_data(int filter, const std::string& column) const override {
        if(column == "Name") {
            return mk_text(m_Girl->FullName());
        } else if (column == "Number") {
            int num_items = m_Girl->inventory().get_num_items();
            return mk_num(num_items);
        } else if (column == "CatNum") {
            int numtype = m_Girl->inventory().get_num_of_type(filter);
            return mk_num(numtype);
        }
        assert(false);
    }

    std::string get_details() const override
    {
        return cGirls::GetSimpleDetails(*m_Girl);
    }

    void enumerate_items(const std::function<void(const sInventoryItem *, int)> &callback) const override
    {
        for (auto & item : m_Girl->inventory().all_items()) {
            callback(item.first, item.second.count);
        }
    }

    int take_item(const sInventoryItem *item, int amount) override {
        // find the item
        int num_taken = m_Girl->remove_item(item, amount);
        if (item->m_Badness >= 20)    // happy to get rid of bad items
            m_Girl->happiness(5);
        else    // sad to see good items go
        {
            int happiness = g_Game->inventory_manager().HappinessFromItem(item);
            m_Girl->happiness(-happiness);
        }

        return num_taken;
    }

    int give_item(const sInventoryItem *item, int amount) override {
        while (amount > 0) {
            int   goodbad  = item->m_Badness;
            int   type     = item->m_Type;
            int   HateLove = m_Girl->pclove() - m_Girl->pchate();
            g_Game->push_message(cScreenItemManagement::GiveItemText(goodbad, HateLove, *m_Girl), 0);

            if (goodbad < 20) {
                int happiness = g_Game->inventory_manager().HappinessFromItem(item);

                m_Girl->obedience(1);
                m_Girl->happiness(happiness);
                m_Girl->pchate(-2);
                m_Girl->pclove(happiness - 1);
                m_Girl->pcfear(-1);
            }

            m_Girl->add_item(item);
            if (AutoUseItems && (type == sInventoryItem::Food || type == sInventoryItem::Makeup)) {
                m_Girl->equip(item, false);
            }

            --amount;
        }
        return 0;
    }

    bool equippable(const sInventoryItem *item, bool equip) const override {
        if(!g_Game->inventory_manager().IsItemEquipable(item)) {
            return false;
        }
        if(equip) {
            return m_Girl->can_equip(item);
        } else {
            return m_Girl->inventory().num_equipped(item) > 0;
        }
    }

    std::shared_ptr<sGirl> m_Girl;
};

extern bool g_AllTogle;
extern cConfig cfg;
extern bool playershopinventory;
static cColor ColorConvert;

cScreenItemManagement::cScreenItemManagement() : cGameWindow("itemmanagement_screen.xml")
{

}

static int filter = 0;

static SDL_Color RarityColor[9];

void cScreenItemManagement::load_ids(sItemTransferSide& target, Side side)
{
    std::string side_str = side == Side::Left ? "Left" : "Right";
    Side opposite = (side == Side::Left) ? Side::Right : Side::Left;
    target.sell10_id    = get_id("Sell10" + side_str + "Button");
    target.sellall_id   = get_id("SellAll" + side_str + "Button");
    target.buy10_id     = get_id("Buy10" + side_str + "Button");
    target.shift_id     = get_id("Shift" + side_str + "Button");
    target.equip_id     = get_id("Equip" + side_str + "Button");
    target.unequip_id   = get_id("Unequip" + side_str + "Button");
    target.owners_list  = GetListBox(get_id("Owners" + side_str + "List"));
    target.items_id     = get_id("Items" + side_str + "List");
    target.detail_id     = get_id("Owners" + side_str + "Details", "*Optional*");

    // even though it seems weird that buy and sell have the same source, note that when the left
    // sell button is visible, only the right buy button will be shown and vice versa, so in practice
    // these buttons really do have opposite effect
    SetButtonCallback(target.buy10_id, [this, opposite]() { attempt_transfer(opposite, 10); });
    SetButtonCallback(target.sell10_id, [this, opposite]() { attempt_transfer(opposite, 10); });
    SetButtonCallback(target.sellall_id, [this, opposite]() { attempt_transfer(opposite, 999); });
    SetButtonCallback(target.shift_id, [this, opposite]() { attempt_transfer(opposite); });
    SetButtonCallback(target.equip_id, [this, side]() { change_equip(side, true); });
    SetButtonCallback(target.unequip_id, [this, side]() { change_equip(side, false); });
    target.owners_list->SetSelectionCallback([this, side](int selection) { refresh_item_list(side); });
    SetListBoxSelectionCallback(target.items_id, [this, side](int selection) { on_select_item(side, selection); });
}


void cScreenItemManagement::set_ids()
{
    curbrothel_id    = get_id("CurrentBrothel");
    gold_id            = get_id("PlayerGold");
    desc_id            = get_id("ItemDesc");
    filter_id        = get_id("FilterList");
    autouse_id        = get_id("AutoUseItems");
    load_ids(m_LeftData, Side::Left);
    load_ids(m_RightData, Side::Right);


    // check interface for colors
    DirPath dpi = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "InterfaceColors.xml";
    tinyxml2::XMLDocument doci;
    if (doci.LoadFile(dpi.c_str()) == tinyxml2::XML_SUCCESS)
    {
        for (auto& el : IterateChildElements(*doci.RootElement()))
        {
            std::string tag = el.Value();
            /// TODO move this to all the other interface stuff
            if (tag == "Color")
            {
                int r, g, b;
                if(el.QueryAttribute("R", &r) == tinyxml2::XML_SUCCESS && el.QueryAttribute("G", &g) == tinyxml2::XML_SUCCESS &&
                   el.QueryAttribute("B", &b) == tinyxml2::XML_SUCCESS) {
                    std::string name = el.Attribute("Name");
                    /* */if (name == "ItemRarity0") ColorConvert.RGBToSDLColor(&RarityColor[0], r, g, b);
                    else if (name == "ItemRarity1") ColorConvert.RGBToSDLColor(&RarityColor[1], r, g, b);
                    else if (name == "ItemRarity2") ColorConvert.RGBToSDLColor(&RarityColor[2], r, g, b);
                    else if (name == "ItemRarity3") ColorConvert.RGBToSDLColor(&RarityColor[3], r, g, b);
                    else if (name == "ItemRarity4") ColorConvert.RGBToSDLColor(&RarityColor[4], r, g, b);
                    else if (name == "ItemRarity5") ColorConvert.RGBToSDLColor(&RarityColor[5], r, g, b);
                    else if (name == "ItemRarity6") ColorConvert.RGBToSDLColor(&RarityColor[6], r, g, b);
                    else if (name == "ItemRarity7") ColorConvert.RGBToSDLColor(&RarityColor[7], r, g, b);
                    else if (name == "ItemRarity8") ColorConvert.RGBToSDLColor(&RarityColor[8], r, g, b);
                } else {
                    g_LogFile.error("interface", "Error reading Color definition from '", dpi.c_str(), "': ",  el.GetLineNum());
                }

            }
        }
    }

    m_LeftData.owners_list->SetArrowHotKeys(SDLK_t, SDLK_g);
    m_RightData.owners_list->SetArrowHotKeys(SDLK_y, SDLK_h);
    SetListBoxHotKeys(m_LeftData.items_id, SDLK_u, SDLK_j);
    SetListBoxHotKeys(m_RightData.items_id, SDLK_i, SDLK_k);
    SetListBoxSelectionCallback(filter_id, [this](int selection) { on_select_filter(selection); });
    SetListBoxHotKeys(filter_id, SDLK_r, SDLK_f);

    SetCheckBoxCallback(autouse_id, [this](bool on) { AutoUseItems = on; });
}

void cScreenItemManagement::init_side(sItemTransferSide& target, int owner, int item)
{
    target.owners_list->Clear();
    ClearListBox(target.items_id);

    for(std::size_t i = 0; i < m_OwnerList.size(); ++i) {
        /// TODO define colors
        target.owners_list->AddRow(i, [&](const std::string& col){ return m_OwnerList[i]->get_data(filter, col); }, COLOR_BLUE);
    }

    /// this works because here ID == index
    if(owner >= 0) {
        target.owners_list->SetSelectedIndex(owner);
        SetSelectedItemInList(target.items_id, item);
    }

    update_details(target);

    // disable the equip/unequip buttons
    DisableWidget(target.equip_id, true);
    DisableWidget(target.unequip_id, true);
}

void cScreenItemManagement::update_details(const sItemTransferSide& target)
{
    if (target.detail_id > -1)
    {
        int index = target.owners_list->GetSelectedID();
        if(index >= 0 && index < m_OwnerList.size()) {
            EditTextItem(m_OwnerList[index]->get_details(), target.detail_id, true);
        } else {
            EditTextItem("", target.detail_id);
        }
    }
}

void cScreenItemManagement::init(bool back)    // `J` bookmark
{
    Focused();

    std::string brothel = "Current Brothel: ";
    brothel += active_building().name();
    EditTextItem(brothel, curbrothel_id);

    if (playershopinventory)    // `J` to set player and shop when pressing ctrl-I to get to inventory
    {
        if (m_LeftData.selected_owner != 0)                                                m_LeftData.selected_item = -1;
        if (m_LeftData.selected_item < 0 && !g_Game->player().inventory().empty())        m_LeftData.selected_item = 0;
        if (m_RightData.selected_owner != 1)                                            m_RightData.selected_item = -1;
        if (m_RightData.selected_item < 0 && !g_Game->shop().empty())                    m_RightData.selected_item = 0;
        m_LeftData.selected_owner = 0;    m_RightData.selected_owner = 1;
    }
    playershopinventory = false;

    if (m_RightData.selected_owner < 0)    m_RightData.selected_owner = 1;
    if (m_LeftData.selected_owner < 0)    m_LeftData.selected_owner = (m_RightData.selected_owner == 0 ? 1 : 0);

    // clear list boxes
    ClearListBox(filter_id);

    // setup the filter
    AddToListBox(filter_id, 0, "All");
    AddToListBox(filter_id, sInventoryItem::Food, "Consumables");
    AddToListBox(filter_id, sInventoryItem::Misc, "Misc");
    AddToListBox(filter_id, sInventoryItem::Hat, "Hats");
    AddToListBox(filter_id, sInventoryItem::Glasses, "Glasses");
    AddToListBox(filter_id, sInventoryItem::Necklace, "Necklaces");
    AddToListBox(filter_id, sInventoryItem::Armband, "Armbands");
    AddToListBox(filter_id, sInventoryItem::Ring, "Rings");
    AddToListBox(filter_id, sInventoryItem::Dress, "Dress");
    AddToListBox(filter_id, sInventoryItem::Underwear, "Underwear");
    AddToListBox(filter_id, sInventoryItem::Swimsuit, "Swimsuits");
    AddToListBox(filter_id, sInventoryItem::Shoes, "Shoes");
    AddToListBox(filter_id, sInventoryItem::SmWeapon, "Small Weapons");
    AddToListBox(filter_id, sInventoryItem::Weapon, "Large Weapons");
    AddToListBox(filter_id, sInventoryItem::Helmet, "Helmets");
    AddToListBox(filter_id, sInventoryItem::Armor, "Armor");
    AddToListBox(filter_id, sInventoryItem::Shield, "Shields");
    AddToListBox(filter_id, sInventoryItem::Combatshoes, "Combat Shoes");

    if (filter < 0) filter = 0;
    SetSelectedItemInList(filter_id, filter, false);
    
    // shop and player
    m_OwnerList.clear();
    m_OwnerList.push_back(std::make_unique<cInventoryProviderPlayer>());
    m_OwnerList.push_back(std::make_unique<cInventoryProviderShop>());

    // and girls from current brothel to list
    for(auto& b : g_Game->buildings().buildings()) {
        AddGirlsFromBuilding(b.get());
    }

    // add current dungeon girls to list
    for(sDungeonGirl& girl : g_Game->dungeon().girls())
    {
        int i = m_OwnerList.size();
        if (g_AllTogle && selected_girl().get() == girl.m_Girl.get()) m_RightData.selected_owner = i;
        m_OwnerList.push_back(std::make_unique<cInventoryProviderGirl>(girl.m_Girl));
    }

    if(!back) {
        init_side(m_LeftData, 0, 0);
        init_side(m_RightData, 0, 0);
    }

    g_AllTogle = false;

    update_button_states(Left);
    update_button_states(Right);
}

void cScreenItemManagement::AddGirlsFromBuilding(IBuilding * brothel) {
    brothel->girls().visit([&](const sGirl& girl){
        int i = m_OwnerList.size();
        if (g_AllTogle && selected_girl().get() == &girl) m_RightData.selected_owner = i;
        m_OwnerList.push_back(std::make_unique<cInventoryProviderGirl>(brothel->girls().get_ref_counted(&girl)));
    });
}

void cScreenItemManagement::write_item_text(const sInventoryItem* item, int owner, int target)
{
    std::stringstream ss;
    std::stringstream iCost;
    std::stringstream iSell;
    std::stringstream iType;

    if (item == nullptr || owner < 0) {
        EditTextItem("", desc_id);
        return;
    }
    else
    {
        iCost << item->m_Cost << " gold";
        iSell << int(((float)item->m_Cost) * g_Game->settings().get_percent(settings::MONEY_SELL_ITEM)) << " gold";
        iType << item->m_Type;
    }
    ss << "Item Name:      " << item->m_Name;
    ss << "\nCost:  " << iCost.str();
    if (target == 1) ss << "Sell for:  " << iSell.str();
    ss << "\nType:  " << iType.str();
    ss << "\n \n" << item->m_Desc;

    EditTextItem(ss.str(), desc_id);
};

void cScreenItemManagement::on_select_item(Side side, int selected)
{
    auto& own_side = side == Side::Left ? m_LeftData : m_RightData;
    auto& other_side = side == Side::Right ? m_LeftData : m_RightData;
    own_side.selected_item = GetLastSelectedItemFromList(own_side.items_id);

    if (selected != -1)
    {
        const sInventoryItem * item = own_side.items.at(selected);
        DisableWidget(own_side.equip_id, !m_OwnerList[own_side.selected_owner]->equippable(item, true));
        DisableWidget(own_side.unequip_id, !m_OwnerList[own_side.selected_owner]->equippable(item, false));
        write_item_text(item, own_side.selected_owner, other_side.selected_owner);
    }
    else
    {
        DisableWidget(own_side.equip_id, true);
        DisableWidget(own_side.unequip_id, true);
    }

    update_button_states(Side::Left);
    update_button_states(Side::Right);
}

void cScreenItemManagement::on_select_filter(int selection)
{
    filter    = selection;
    int filterpos = GetListBox(filter_id)->GetTopPosition();
    sync_owner_selection(m_LeftData);
    sync_owner_selection(m_RightData);
    init_side(m_LeftData, m_LeftData.selected_owner, 0);
    init_side(m_RightData, m_RightData.selected_owner, 0);
    update_button_states(Left);
    update_button_states(Right);

    GetListBox(filter_id)->SetTopPosition(filterpos);
}

void cScreenItemManagement::sync_owner_selection(const sItemTransferSide& side)
{
    if (side.owners_list->GetSelectedID() != side.selected_owner) {
        side.owners_list->SetSelectedID(side.selected_owner);
    }
}

void cScreenItemManagement::change_equip(Side side, bool equip)
{
    sItemTransferSide& own_side = side == Side::Left ? m_LeftData : m_RightData;
    sItemTransferSide& other_side = side == Side::Right ? m_LeftData : m_RightData;

    auto targetGirl = dynamic_cast<cInventoryProviderGirl&>(*m_OwnerList.at(own_side.selected_owner)).m_Girl;

    int item = GetLastSelectedItemFromList(own_side.items_id);
    if (item != -1)
    {
        if(equip) {
            targetGirl->equip(own_side.items[item], true);
        } else {
            targetGirl->unequip(own_side.items[item]);
        }
        DisableWidget(own_side.equip_id, equip);
        DisableWidget(own_side.unequip_id, !equip);
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
    const sInventoryItem* item  = nullptr;
    if(other_data.selected_item >= 0) {
        item = other_data.items[other_data.selected_item];
    }
    
    // check the shop for infinite items and check Player for multiples of the same item
    int disable_shift   = 0;        // 0 = hidden
    int disable_buy10   = 0;        // 1 = off
    int disable_sell10  = 0;        // 2 = on
    int disable_sellall = 0;    //
    
    disable_shift = (item  ? 2 : 1);

    if (data.selected_owner == 0/*Player*/ && other_data.selected_owner == 1/*Shop*/)
    {
        disable_buy10 = (item  && item->m_Infinite && g_Game->gold().afford(item->m_Cost * 10) ? 2 : 1);
        disable_shift = (item  && g_Game->gold().afford(item->m_Cost) ? 2 : 1);
    }
    if (data.selected_owner == 1/*Shop*/ && other_data.selected_owner == 0/*Player*/)
    {
        disable_sell10  = (item ? 2 : 1);
        if(!item) {
            disable_sellall = 1;
        } else {
            disable_sellall = (g_Game->player().inventory().get_num_items(item) > 0 ? 2 : 1);
        }
    }
    DisableWidget(data.shift_id, disable_shift <= 1);
    HideWidget(data.buy10_id, disable_buy10 == 0);
    DisableWidget(data.buy10_id, disable_buy10 <= 1);
    HideWidget(data.sell10_id, disable_sell10 == 0);
    DisableWidget(data.sell10_id, disable_sell10 <= 1);
    HideWidget(data.sellall_id, disable_sellall == 0);
    DisableWidget(data.sellall_id, disable_sellall <= 1);

    if (GetSelectedItemFromList(data.items_id) < 0)
    {
        DisableWidget(data.equip_id, true);
        DisableWidget(data.unequip_id, true);
    }
}

void cScreenItemManagement::refresh_item_list(Side which_list)
{
    // good enough place as any to update the cost shown on the screen
    std::string temp = "PLAYER GOLD: ";
    temp += g_Game->gold().sval();
    EditTextItem(temp, gold_id);

    sItemTransferSide& data = (which_list == Side::Left) ? m_LeftData : m_RightData;
    sItemTransferSide& other = (which_list == Side::Right) ? m_LeftData : m_RightData;

    update_details(m_LeftData);
    update_details(m_RightData);

    int selected_item = GetListBox(data.items_id)->GetSelectedIndex();
    ClearListBox(data.items_id);
    data.items.clear();

    int selection = data.owners_list->GetSelectedID();

    // prevent selecting the same on both sides. in that case, just reset to old selection.
    if (selection == other.selected_owner) {
        if(selection == data.selected_owner)
            std::cout << "THIS IS WEIRD\n";
        else {
            data.owners_list->SetSelectedID(data.selected_owner);
        }
        return;
    } else {
        data.selected_owner = selection;
    }

    if (selection != -1)
    {
        int i = 0;
        m_OwnerList[selection]->enumerate_items([this, &data, &i](const sInventoryItem* item, int amount) {
            std::stringstream it;
            it << item->m_Name;
            if(amount > 1 && amount < 999) {
                it << " (" << amount << ")";
            }
            int item_type = item->m_Type;
            if ((filter == 0)  // unfiltered?
                || (item_type == filter)  // matches filter exactly?
                || ((filter == sInventoryItem::Food) && (item_type == sInventoryItem::Makeup))  // passes "consumable" filter?
                    ) {  // passed the filter, so add it
                AddToListBox(data.items_id, i, it.str());
                SetSelectedItemTextColor(data.items_id, i, RarityColor[item->m_Rarity]);

                data.items.push_back(item);
                ++i;
            }
        });
    }

    SortListItems(data.items_id, "");

    if(selected_item >= 0 && selected_item < GetListBox(data.items_id)->NumItems())
        GetListBox(data.items_id)->SetSelectedIndex(selected_item);
    data.selected_item = GetLastSelectedItemFromList(data.items_id);

    if (data.selected_item < 0)
    {
        write_item_text(nullptr, -1, -1);
        DisableWidget(data.shift_id, true);
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

    ForAllSelectedItems(from_data.items_id, [&](int selection) {
        // since items sold to shop are simply destroyed, no selection to track here
        const sInventoryItem* item = from_data.items[selection];
        int sold = m_OwnerList[from_data.selected_owner]->take_item(item, num);
        int remaining = m_OwnerList[to_data.selected_owner]->give_item(item, sold);
        // give back remaining items to original owner
        if(remaining > 0) {
            remaining = m_OwnerList[from_data.selected_owner]->give_item(item, remaining);
        }
    });

    // update the item lists
    refresh_item_list(Side::Left);
    refresh_item_list(Side::Right);
}

std::string cScreenItemManagement::GiveItemText(int goodbad, int HateLove, const sGirl& girl, const std::string& ItemName)
{
    std::string message;
    if (goodbad < 20)
    {
        if (HateLove < -80)         message = "She grudgingly accepts the gift, but makes it clear that she still thinks that you rate slightly below a toad in her worldview.";
        else if (HateLove < -60)    message = "She takes your gift in hand, looks at it, looks at you, than walks away without a word.";
        else if (HateLove < -40)
        {
            if (girl.has_active_trait("Your Daughter"))
            {
                message = "Are you trying to make up for being an ass dad?"; //hopefully this works.. will add more
            }
            else message = "You know, if you wanted to fuck, you shoulda just said so.  That way I'd have had more fun saying no.";
        }
        else if (HateLove < -20)
        {
            if (girl.has_active_trait("Your Daughter"))
            {
                message = "You still have a long way to go if you want me to like you dad."; //hopefully this works.. will add more CRAZY
            }
            else message = "If you think giving me pretty things will get you between my legs, you're wrong!";
        }
        else if (HateLove < 0)        message = "She is shocked you would give her anything nice.";
        else if (HateLove < 20)        message = "She is happy with the gift.";
        else if (HateLove < 40)        message = "She is happy with the gift and thanks you.";
        else if (HateLove < 60)        message = "She is happy with the gift and gives you a big hug.";
        else if (HateLove < 80)        message = "She is happy with the gift and gives you a big hug and a kiss on the cheek.";
        else
        {
            if (girl.has_active_trait("Your Daughter")) message = "She is happy with the gift and gives you a big hug and a kiss on the cheek saying she loves her daddy."; //hopefully this works.. will add more
            else if (girl.has_active_trait("Lesbian")) message = "She is happy with the gift and gives you a big hug and a kiss on the cheek and says that if you weren't a \"man\" she might have to show you how much she loved that gift.";
            else /*                                   */    message = "She is happy with the gift and gives you a big hug and kisses you hard.  After the kiss she whispers to you to see her later so she can thank you \"properly\".";
        }
    }
    else
    {
        if (HateLove < -80)            message = "'Wow, what's next?  A dead rat?  Your shit sucks almost as bad as your technique!";
        else if (HateLove < -60)    message = "She takes your gift in hand, looks at it, looks at you, than walks away without a word.";
        else if (HateLove < -40)    message = "Hey, present for you!' 'Fuck off and die!";
        else if (HateLove < -20)    message = "'Fuck off, and take your cheap garbage with you!";
        else if (HateLove < 0)        message = "She doesn't seem happy with the gift.  But has come to expect this kinda thing from you.";
        else if (HateLove < 20)        message = "She doesn't seem happy with the gift.";
        else if (HateLove < 40)        message = "She doesn't seem happy with the gift and looks a little sad.";
        else if (HateLove < 60)        message = "She doesn't seem happy with the gift and looks sad.";
        else if (HateLove < 80)        message = "She doesn't seem happy with the gift and tears can be seen in her eyes.";
        else
        {
            if (girl.has_active_trait("Your Daughter")) message = "She looks at you and says \"Why would you give me such a thing daddy?\"."; //hopefully this works.. will add more
            else /*                                   */    message = "She can't belive you would give her such a gift and runs off crying.";
        }
    }
    return message;
}

