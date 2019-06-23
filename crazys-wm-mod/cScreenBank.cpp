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
#include "cBrothel.h"
#include "cScreenBank.h"
#include "cWindowManager.h"
#include "cGold.h"
#include "cGetStringScreenManager.h"
#include "cScreenGetInput.h"
#include "InterfaceGlobals.h"

extern bool						g_InitWin;
extern int						g_CurrBrothel;
extern cGold					g_Gold;
extern cBrothelManager			g_Brothels;
extern cWindowManager			g_WinManager;
extern cInterfaceEventManager	g_InterfaceEvents;
extern long						g_IntReturn;
extern cScreenGetInput			g_GetInput;
extern	int						g_CurrentScreen;

bool cScreenBank::ids_set = false;
cScreenBank::cScreenBank()
{
	DirPath dp = DirPath() << "Resources" << "Interface" << cfg.resolution.resolution() << "bank_screen.xml";
	m_filename = dp.c_str();
	GetDeposit = GetWithdraw = false;
}
cScreenBank::~cScreenBank() {}

void cScreenBank::set_ids()
{
	ids_set			/**/ = true;
	g_LogFile.write("set_ids in cScreenBank");

	back_id			/**/ = get_id("BackButton", "Back");
	details_id		/**/ = get_id("BankDetails");
	header_id		/**/ = get_id("ScreenHeader");
	deposit_id		/**/ = get_id("DepositButton");
	depositall_id	/**/ = get_id("DepositAllButton");
	withdraw_id		/**/ = get_id("WithdrawButton");
	withdrawall_id	/**/ = get_id("WithdrawAllButton");
}

void cScreenBank::init()
{
	g_CurrentScreen = SCREEN_BANK;
	if (!g_InitWin) return;
	Focused();
	g_InitWin = false;

	if (GetDeposit)
	{
		if (g_Gold.misc_debit(g_IntReturn)) g_Brothels.DepositInBank(g_IntReturn);
		else g_MessageQue.AddToQue("You don't have that much!", COLOR_RED);
		GetDeposit = false;
	}

	if (GetWithdraw)
	{
		if (g_IntReturn <= g_Brothels.GetBankMoney())
		{
			g_Brothels.WithdrawFromBank(g_IntReturn);
			g_Gold.misc_credit(g_IntReturn);
		}
		else g_MessageQue.AddToQue("There isn't that much in your account", COLOR_RED);
		GetWithdraw = false;
	}

	stringstream ss;
	ss << "Bank account: " << g_Brothels.GetBankMoney() << " gold" << endl;
	ss << "On hand: " << g_Gold.ival() << " gold";
	EditTextItem(ss.str(), details_id);

	// disable/enable Withdraw button depending on whether player has money in bank
	DisableButton(withdraw_id, (g_Brothels.GetBankMoney() == 0));
	DisableButton(withdrawall_id, (g_Brothels.GetBankMoney() == 0));
	// likewise, if player has no money on hand, disable deposit buttons
	DisableButton(depositall_id, (g_Gold.ival() <= 0));
	DisableButton(deposit_id, (g_Gold.ival() <= 0));
}

void cScreenBank::process()
{
	if (!ids_set) set_ids();	// we need to make sure the ID variables are set
	init();						// set up the window if needed
	check_events();				// check to see if there's a button event needing handling
}

void cScreenBank::check_events()
{
	if (g_InterfaceEvents.GetNumEvents() == 0) return;	// no events means we can go home
	if (g_InterfaceEvents.CheckButton(back_id))			// if it's the back button, pop the window off the stack and we're done
	{
		g_InitWin = true;
		g_WinManager.Pop();
		return;
	}
	if (g_InterfaceEvents.CheckButton(depositall_id))
	{
		if (g_Gold.ival() <= 0)
		{
			g_MessageQue.AddToQue("You have no money to deposit.", COLOR_RED);
			return;
		}
		GetDeposit = true;
		g_IntReturn = g_Gold.ival();
		g_InitWin = true;
		return;
	}
	if (g_InterfaceEvents.CheckButton(deposit_id))
	{
		if (g_Gold.ival() <= 0)
		{
			g_MessageQue.AddToQue("You have no money to deposit.", COLOR_RED);
			return;
		}
		GetDeposit = true;
		g_GetInput.ModeGetInt();
		g_WinManager.push("GetInput");
		g_InitWin = true;
		return;
	}
	if (g_InterfaceEvents.CheckButton(withdraw_id))
	{
		GetWithdraw = true;
		g_GetInput.ModeGetInt();
		g_WinManager.push("GetInput");
		g_InitWin = true;
		return;
	}
	if (g_InterfaceEvents.CheckButton(withdrawall_id))
	{
		if (g_Brothels.GetBankMoney() <= 0)
		{
			g_MessageQue.AddToQue("You have no money to withdraw", COLOR_RED);
			return;
		}
		g_IntReturn = g_Brothels.GetBankMoney();
		GetWithdraw = true;
		g_InitWin = true;
		return;
	}
}
