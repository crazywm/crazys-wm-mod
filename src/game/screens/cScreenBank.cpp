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
#include "cScreenBank.h"
#include "cGold.h"
#include "Game.hpp"
#include <sstream>

cScreenBank::cScreenBank() : cInterfaceWindowXML("bank_screen.xml")
{
}


void cScreenBank::set_ids()
{
    back_id            = get_id("BackButton", "Back");
    details_id        = get_id("BankDetails");
    header_id        = get_id("ScreenHeader");
    deposit_id        = get_id("DepositButton");
    depositall_id    = get_id("DepositAllButton");
    withdraw_id        = get_id("WithdrawButton");
    withdrawall_id    = get_id("WithdrawAllButton");

    // setup callbacks
    SetButtonNavigation(back_id, "<back>");
    SetButtonCallback(depositall_id, [this](){ deposit_all(); });
    SetButtonCallback(withdrawall_id, [this](){ withdraw_all(); });
    SetButtonCallback(deposit_id, [this]() {
        input_integer([this](int amount){
            deposit(amount);
        });
    });

    SetButtonCallback(withdraw_id, [this]() {
        input_integer([this](int amount){
            withdraw(amount);
        });
    });
}

void cScreenBank::init(bool back)
{
    Focused();

    stringstream ss;
    ss << "Bank account: " << g_Game->GetBankMoney() << " gold" << endl;
    ss << "On hand: " << g_Game->gold().ival() << " gold";
    EditTextItem(ss.str(), details_id);

    // disable/enable Withdraw button depending on whether player has money in bank
    DisableWidget(withdraw_id, (g_Game->GetBankMoney() == 0));
    DisableWidget(withdrawall_id, (g_Game->GetBankMoney() == 0));
    // likewise, if player has no money on hand, disable deposit buttons
    DisableWidget(depositall_id, (g_Game->gold().ival() <= 0));
    DisableWidget(deposit_id, (g_Game->gold().ival() <= 0));
}

void cScreenBank::withdraw_all()
{
    if (g_Game->GetBankMoney() <= 0)
    {
        push_message("You have no money to withdraw", COLOR_RED);
        return;
    }
    withdraw(g_Game->GetBankMoney());
}

void cScreenBank::deposit_all()
{
    if (g_Game->gold().ival() <= 0)
    {
        push_message("You have no money to deposit.", COLOR_RED);
        return;
    }
    deposit(g_Game->gold().ival());
}

void cScreenBank::deposit(int amount)
{
    if (g_Game->gold().misc_debit(amount)) g_Game->DepositInBank(amount);
    else push_message("You don't have that much!", COLOR_RED);

    init(false);
}

void cScreenBank::withdraw(int amount)
{
    if (amount <= g_Game->GetBankMoney())
    {
        g_Game->WithdrawFromBank(amount);
        g_Game->gold().misc_credit(amount);
    }
    else push_message("There isn't that much in your account", COLOR_RED);

    init(false);
}
