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
#include "cGold.h"
#include "CLog.h"
#include <sstream>
#include <iomanip>
#include <tinyxml2.h>
#include "xml/util.h"
#include "Game.hpp"

namespace settings {
    extern const char* INITIAL_GOLD;
    extern const char* MONEY_SELL_ITEM;
}

/*
* Two types of transaction here: instant and delayed.
* Instant transactions show up in the gold total immediatly.
* Delayed transactions are settled at the end of the turn.
*
* They also break down by income and expenditure, which should
* be obvious.
*
* Delayed expenditure gets paid whether you have the gold or not,
* possibly driving you into debt. Instant expenditure can only happen
* ifyou have the cash in hand.
*/

cGoldBase::cGoldBase(int init_val)
{
    m_value = init_val;
    m_upkeep = 0;
    m_income = 0;
    m_cash_in = 0;
    m_cash_out = 0;
}

void cGoldBase::reset()
{
    m_value = g_Game->settings().get_integer(settings::INITIAL_GOLD);
    m_upkeep = 0;
    m_income = 0;
    m_cash_in = 0;
    m_cash_out = 0;
}

// for stuff sold in the marketplace - goes straight into the PC's pocket
void cGoldBase::item_sales(double income)
{
    income = g_Game->settings().get_percent(settings::MONEY_SELL_ITEM) * income;

    m_value += income;
    m_cash_in += income;
}

// ditto at the slave market
void cGoldBase::slave_sales(double income)
{
    m_value += income;
    m_cash_in += income;
}

/*
* This is an "untaxed" way to credit the PC. It is primarily for such
* things as cash withdrawals from the bank, which isn't income and
* therefore shouldn't be taxed.
*/
void cGoldBase::misc_credit(double income)
{
    m_value += income;
}

// this is for girls working at the brothel - goes into income
void cGoldBase::brothel_work(double income)
{
    m_income += income;
}

// for when a girl gives birth to a monster -  The cash goes to the brothel, so we add this to m_income
void cGoldBase::creature_sales(double income)
{
    m_income += income;
}

// income from movie crystals
void cGoldBase::movie_income(double income)
{
    m_income += income;
}

// income from the clinic
void cGoldBase::clinic_income(double income)
{
    m_income += income;
}

// income from the arena
void cGoldBase::arena_income(double income)
{
    m_income += income;
}

// income from the farm
void cGoldBase::farm_income(double income)
{
    m_income += income;
}

// income from the bar
void cGoldBase::bar_income(double income)
{
    m_income += income;
}

// income from gambling halls
void cGoldBase::gambling_profits(double income)
{
    m_income += income;
}

// income from businesess under player control
void cGoldBase::extortion(double income)
{
    m_income += income;
}

// These happen at end of turn anyway - so let's do them as delayed transactions
void cGoldBase::objective_reward(double income)
{
    m_income += income;
}
void cGoldBase::plunder(double income)
{
    m_income += income;
}
void cGoldBase::petty_theft(double income)
{
    m_income += income;
}
void cGoldBase::grand_theft(double income)
{
    m_income += income;
}
void cGoldBase::catacomb_loot(double income)
{
    m_income += income;
}

/*
* this is for cash purchases. It lets us test and
* debit in one function call
*
* some transactions can be executed as instant or delayed
* depending on how they happen. Healing potions, for instance
* are cash transactions from the upgrade screen, but if
* they're auto-restocked, that's a delayed cost.
* For such cases the force flag lets us execute this as
* a delayed transaction.
*
* This is a protected func and can't be called from outside the class.
*/
bool cGoldBase::debit_if_ok(double price, bool force)
{
    if (price > m_value || force) return false;
    m_value -= price;
    m_cash_out -= price;
    return true;
}

// weekly cost of making a movie - delayed transaction
void cGoldBase::movie_cost(double price)
{
    m_upkeep += price;
}

/*
* costs associated with building maintenance and upgread
* (but not bar and casino costs which have their own
* cost category
*/
void cGoldBase::building_upkeep(double price)
{
    m_upkeep += price;
}

// buying stuff from the market - instant
bool cGoldBase::item_cost(double price)
{
    if (debit_if_ok(price))
    {
        return true;
    }
    return false;
}

// counterpart of misc_credit - should probably also go away
bool cGoldBase::misc_debit(double price)
{
    if (debit_if_ok(price))
    {
        return true;
    }
    return false;
}

/*
* consumables are things like potions, nets and booze
* which tend to be auto-stockable
*
* so we have a force flag here for the restock call
*/
bool cGoldBase::consumable_cost(double price, bool force)
{

    if (debit_if_ok(price, force))
    {
        return true;
    }
    return false;

}

// buying at the slave market - you need the cash
bool cGoldBase::slave_cost(double price)
{
    if (debit_if_ok(price))
    {
        return true;
    }
    return false;
}

// buying a new building - cash transactions only please
bool cGoldBase::brothel_cost(double price)
{
    if (debit_if_ok(price))
    {
        return true;
    }
    return false;
}

// training is a delayed cost
void cGoldBase::girl_training(double cost)
{
    m_upkeep += cost;
}

void cGoldBase::girl_support(double cost)
{
    m_upkeep += cost;
}

void cGoldBase::bribes(double cost)
{
    m_upkeep += cost;
}

void cGoldBase::fines(double cost)
{
    m_value -= cost;
    m_cash_out -= cost;
}

void cGoldBase::tax(double cost)
{
    m_value -= cost;
    m_cash_out -= cost;
}

void cGoldBase::goon_wages(double cost)
{
    m_upkeep += cost;
}

void cGoldBase::staff_wages(double cost)
{
    m_upkeep += cost;
}

void cGoldBase::advertising_costs(double cost)
{
    m_upkeep += cost;
}

void cGoldBase::centre_costs(double cost)
{
    m_upkeep += cost;
}

void cGoldBase::arena_costs(double cost)
{
    m_upkeep += cost;
}
void cGoldBase::clinic_costs(double cost)
{
    m_upkeep += cost;
}

void cGoldBase::rival_raids(double cost)
{
    m_value -= cost;
    m_cash_out -= cost;
}

void cGoldBase::bank_interest(double income)
{
    // do nothing for now - placeholder until accounting
    // stuff is added
}
// `J` added interest rate for .06.03.01
double cGoldBase::interest_rate()
{
    if (m_interest_rate > 0.01)    m_interest_rate = 0.01;
    return m_interest_rate;
}
double cGoldBase::reset_interest_rate()
{
    m_interest_rate = 0.002;
    return m_interest_rate;
}
double cGoldBase::increase_interest_rate()
{
    m_interest_rate += 0.0005;
    if (m_interest_rate > 0.01)    m_interest_rate = 0.01;
    return m_interest_rate;
}
double cGoldBase::decrease_interest_rate()
{
    m_interest_rate -= 0.0005;
    if (m_interest_rate < 0.0000)    m_interest_rate = 0.0000;
    return m_interest_rate;
}

std::string cGoldBase::sval()
{
    std::stringstream ss;
    ss << ival();
    return ss.str();
}

int cGoldBase::ival()
{
    return int(floor(m_value));
}

tinyxml2::XMLElement& cGoldBase::saveGoldXML(tinyxml2::XMLElement& elRoot) const
{
    auto& elGold = PushNewElement(elRoot, "Gold");
    elGold.SetAttribute("value", m_value);
    elGold.SetAttribute("income", m_income);
    elGold.SetAttribute("upkeep", m_upkeep);
    elGold.SetAttribute("cash_in", m_cash_in);
    elGold.SetAttribute("cash_out", m_cash_out);
    elGold.SetAttribute("interest_rate", m_interest_rate);
    return elGold;
}

bool cGoldBase::loadGoldXML(const tinyxml2::XMLElement* pGold)
{
    //load default values just in case
    reset();

    if (pGold == nullptr) return false;

    pGold->QueryDoubleAttribute("value", &m_value);
    pGold->QueryDoubleAttribute("income", &m_income);
    pGold->QueryDoubleAttribute("upkeep", &m_upkeep);
    pGold->QueryDoubleAttribute("cash_in", &m_cash_in);
    pGold->QueryDoubleAttribute("cash_out", &m_cash_out);
    pGold->QueryDoubleAttribute("interest_rate", &m_interest_rate);
    return true;
}

void cGold::brothel_accounts(cGold &g, int brothel_id)
{
    cGoldBase *br_gold = find_brothel_account(brothel_id);
    if (br_gold)
    {
        (*br_gold) += g;
        return;
    }
    g_LogFile.log(ELogLevel::ERROR, "Can't find account record for brothel id ", brothel_id);
}

void cGold::week_end()
{
    cGoldBase *bpt;
    g_LogFile.log(ELogLevel::DEBUG, "Week End begins: value    = ", m_value,
            "\n               : income   = ", m_income,
            "\n               : upkeep   = ", m_upkeep,
            "\n               : cash in  = ", m_cash_in,
            "\n               : cash out = ", m_cash_out);

    for (int i = 0; (bpt = brothels[i]); i++)
    {
        (*this) += (*bpt);
        g_LogFile.log(ELogLevel::DEBUG, "Added Bothel ", i, ": value    = ", m_value,
                      "\n               : income   = ", m_income,
                      "\n               : upkeep   = ", m_upkeep,
                      "\n               : cash in  = ", m_cash_in,
                      "\n               : cash out = ", m_cash_out);
        bpt->zero();
    }
    m_value += m_income;
    m_value -= m_upkeep;
    m_income = m_upkeep = m_income = m_cash_in = m_cash_out = 0;

    g_LogFile.log(ELogLevel::DEBUG, "Week End Final : value    = " , m_value , '\n',
    "               : income   = " , m_income , '\n',
    "               : upkeep   = " , m_upkeep , '\n',
    "               : cash in  = " , m_cash_in , '\n',
    "               : cash out = " , m_cash_out , '\n');
}

int cGold::total_income() const
{
    double sum = cGoldBase::total_income();
    for(auto& bpt : brothels) {
        if(bpt.second) {
            sum += bpt.second->total_income();
        }
    }
    return int(floor(sum));
}

int cGold::total_upkeep() const
{
    double sum = cGoldBase::total_upkeep();
    for(auto& bpt : brothels) {
        if(bpt.second) {
            sum += bpt.second->total_upkeep();
        }
    }
    return int(floor(sum));
}

int cGold::total_earned() const
{
    double sum = cGoldBase::total_earned();
    for(auto& bpt : brothels) {
        if(bpt.second) {
            sum += bpt.second->total_earned();
        }
    }
    return int(floor(sum));
}

void cGold::gen_report(int month)
{
}
