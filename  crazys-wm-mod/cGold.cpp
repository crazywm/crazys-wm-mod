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
#include "sConfig.h"
#include <sstream>
#include "libintl.h"

extern CLog g_LogFile;

extern cConfig cfg;

//Function to handle conversions of an int to a string cMessageQue can accept,
string intstring(int input)
{
	stringstream ss;
	ss << input;
	return ss.str();
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

cGoldBase::cGoldBase()
{
	m_initial_value = m_value = cfg.initial.gold();
	m_upkeep = 0;
	m_income = 0;
	m_cash_in = 0;
	m_cash_out = 0;
}

cGoldBase::cGoldBase(int init_val)
{
	m_initial_value = m_value = init_val;
	m_upkeep = 0;
	m_income = 0;
	m_cash_in = 0;
	m_cash_out = 0;
}

void cGoldBase::reset()
{
	m_value = m_initial_value;
	m_upkeep = 0;
	m_income = 0;
	m_cash_in = 0;
	m_cash_out = 0;
}

// for stuff sold in the marketplace - goes straight into the PC's pocket
void cGoldBase::item_sales(double income)
{
	income = cfg.in_fact.item_sales() * income;

	m_value += income;
	m_cash_in += income;
	detail_in.item_sales += income;
}

// ditto at the slave market
void cGoldBase::slave_sales(double income)
{
	m_value += income;
	m_cash_in += income;
	detail_in.slave_sales += income;
}

/*
* This is an "untaxed" way to credit the PC. It is primarily for such
* things as cash withdrawals from the bank, which isn't income and
* therefore shouldn't be taxed.
*/
void cGoldBase::misc_credit(double income)
{
	m_value += income;
	detail_in.misc += income;
}

// this is for girls working at the brothel - goes into income
void cGoldBase::brothel_work(double income)
{
	m_income += income;
	detail_in.brothel_work += income;
}

// for when a girl gives birth to a monster -  The cash goes to the brothel, so we add this to m_income
void cGoldBase::creature_sales(double income)
{
	m_income += income;
	detail_in.creature_sales += income;
}

// income from movie crystals
void cGoldBase::movie_income(double income)
{
	m_income += income;
	detail_in.movie_income += income;
}

// income from the clinic
void cGoldBase::clinic_income(double income)
{
	m_income += income;
	detail_in.clinic_income += income;
}

// income from the arena
void cGoldBase::arena_income(double income)
{
	m_income += income;
	detail_in.arena_income += income;
}

// income from the farm
void cGoldBase::farm_income(double income)
{
	m_income += income;
	detail_in.farm_income += income;
}

// income from the bar
void cGoldBase::bar_income(double income)
{
	m_income += income;
	detail_in.bar_income += income;
}

// income from gambling halls
void cGoldBase::gambling_profits(double income)
{
	m_income += income;
	detail_in.gambling_profits += income;
}

// income from businesess under player control
void cGoldBase::extortion(double income)
{
	m_income += income;
	detail_in.extortion += income;
}

// These happen at end of turn anyway - so let's do them as delayed transactions
void cGoldBase::objective_reward(double income)
{
	m_income += income;
	detail_in.objective_reward += income;
}
void cGoldBase::plunder(double income)
{
	m_income += income;
	detail_in.plunder += income;
}
void cGoldBase::petty_theft(double income)
{
	m_income += income;
	detail_in.petty_theft += income;
}
void cGoldBase::grand_theft(double income)
{
	m_income += income;
	detail_in.grand_theft += income;
}
void cGoldBase::catacomb_loot(double income)
{
	m_income += income;
	detail_in.catacomb_loot += income;
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
	detail_out.movie_cost += price;
}

/*
* costs associated with building maintenance and upgread
* (but not bar and casino costs which have their own
* cost category
*/
void cGoldBase::building_upkeep(double price)
{
	m_upkeep += price;
	detail_out.building_upkeep += price;
}

// buying stuff from the market - instant
bool cGoldBase::item_cost(double price)
{
	if (debit_if_ok(price))
	{
		detail_out.item_cost += price;
		return true;
	}
	return false;
}

// counterpart of misc_credit - should probably also go away
bool cGoldBase::misc_debit(double price)
{
	if (debit_if_ok(price))
	{
		detail_out.misc_debit += price;
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
		detail_out.consumable_cost += price;
		return true;
	}
	return false;

}

// buying at the slave market - you need the cash
bool cGoldBase::slave_cost(double price)
{
	if (debit_if_ok(price))
	{
		detail_out.slave_cost += price;
		return true;
	}
	return false;
}

// buying a new building - cash transactions only please
bool cGoldBase::brothel_cost(double price)
{
	if (debit_if_ok(price))
	{
		detail_out.brothel_cost += price;
		return true;
	}
	return false;
}

// training is a delayed cost
void cGoldBase::girl_training(double cost)
{
	m_upkeep += cost;
	detail_out.girl_training += cost;
}

void cGoldBase::girl_support(double cost)
{
	m_upkeep += cost;
	detail_out.girl_support += cost;
}

void cGoldBase::bribes(double cost)
{
	m_upkeep += cost;
	detail_out.bribes += cost;
}

void cGoldBase::fines(double cost)
{
	m_value -= cost;
	m_cash_out -= cost;
	detail_out.fines += cost;
}

void cGoldBase::tax(double cost)
{
	m_value -= cost;
	m_cash_out -= cost;
	detail_out.tax += cost;
}

void cGoldBase::goon_wages(double cost)
{
	m_upkeep += cost;
	detail_out.goon_wages += cost;
}

void cGoldBase::staff_wages(double cost)
{
	m_upkeep += cost;
	detail_out.staff_wages += cost;
}

void cGoldBase::bar_upkeep(double cost)
{
	m_upkeep += cost;
	detail_out.bar_upkeep += cost;
}

void cGoldBase::casino_upkeep(double cost)
{
	m_upkeep += cost;
	detail_out.casino_upkeep += cost;
}

void cGoldBase::advertising_costs(double cost)
{
	m_upkeep += cost;
	detail_out.advertising_costs += cost;
}

void cGoldBase::centre_costs(double cost)
{
	m_upkeep += cost;
	detail_out.centre_costs += cost;
}

void cGoldBase::arena_costs(double cost)
{
	m_upkeep += cost;
	detail_out.arena_costs += cost;
}
void cGoldBase::clinic_costs(double cost)
{
	m_upkeep += cost;
	detail_out.clinic_costs += cost;
}

void cGoldBase::rival_raids(double cost)
{
	m_value -= cost;
	m_cash_out -= cost;
	detail_out.rival_raids += cost;
}

void cGoldBase::bank_interest(double income)
{
	// do nothing for now - placeholder until accounting
	// stuff is added
}
// `J` added interest rate for .06.03.01
double cGoldBase::interest_rate()
{
	if (m_interest_rate > 0.01)	m_interest_rate = 0.01;
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
	if (m_interest_rate > 0.01)	m_interest_rate = 0.01;
	return m_interest_rate;
}
double cGoldBase::decrease_interest_rate()
{
	m_interest_rate -= 0.0005;
	if (m_interest_rate < 0.0000)	m_interest_rate = 0.0000;
	return m_interest_rate;
}

string cGoldBase::sval()
{
	stringstream ss;
	ss << ival();
	return ss.str();
}

int cGoldBase::ival()
{
	return int(floor(m_value));
}

TiXmlElement* cGoldBase::saveGoldXML(TiXmlElement* pRoot)
{
	TiXmlElement* pGold = new TiXmlElement("Gold");
	pRoot->LinkEndChild(pGold);
	pGold->SetDoubleAttribute("value", m_value);
	pGold->SetDoubleAttribute("income", m_income);
	pGold->SetDoubleAttribute("upkeep", m_upkeep);
	pGold->SetDoubleAttribute("cash_in", m_cash_in);
	pGold->SetDoubleAttribute("cash_out", m_cash_out);
	pGold->SetDoubleAttribute("interest_rate", m_interest_rate);
	return pGold;
}

bool cGoldBase::loadGoldXML(TiXmlHandle hGold)
{
	//load default values just in case
	reset();

	TiXmlElement* pGold = hGold.ToElement();
	if (pGold == 0) return false;

	pGold->QueryDoubleAttribute("value", &m_value);
	pGold->QueryDoubleAttribute("income", &m_income);
	pGold->QueryDoubleAttribute("upkeep", &m_upkeep);
	pGold->QueryDoubleAttribute("cash_in", &m_cash_in);
	pGold->QueryDoubleAttribute("cash_out", &m_cash_out);
	pGold->QueryDoubleAttribute("interest_rate", &m_interest_rate);
	return true;
}

istream &operator>>(istream& is, cGoldBase &g)
{
	double val, inc, up, cashin, cashout;
	is >> val >> inc >> up >> cashin >> cashout;
	g.m_value = val;
	g.m_income = inc;
	g.m_upkeep = up;
	g.m_cash_in = cashin;
	g.m_cash_out = cashout;
	is.ignore(100, '\n');
	return is;
}

void cGold::brothel_accounts(cGold &g, int brothel_id)
{
	cGoldBase *br_gold = find_brothel_account(brothel_id);
	if (br_gold)
	{
		(*br_gold) += g;
		return;
	}
	g_LogFile.ss() << "Error: can't find account record " << "for brothel id " << brothel_id << endl;
	g_LogFile.ssend();
}

void cGold::week_end()
{
	cGoldBase *bpt;
	ostream &ss = g_LogFile.ss();

	ss << "Week End begins: value    = " << m_value << endl;
	ss << "               : income   = " << m_income << endl;
	ss << "               : upkeep   = " << m_upkeep << endl;
	ss << "               : cash in  = " << m_cash_in << endl;
	ss << "               : cash out = " << m_cash_out << endl;

	for (int i = 0; (bpt = brothels[i]); i++)
	{
		(*this) += (*bpt);

		ss << "Added Bothel " << i << ": value    = " << m_value << endl;
		ss << "               : income   = " << m_income << endl;
		ss << "               : upkeep   = " << m_upkeep << endl;
		ss << "               : cash in  = " << m_cash_in << endl;
		ss << "               : cash out = " << m_cash_out << endl;
		bpt->zero();
	}
	m_value += m_income;
	m_value -= m_upkeep;
	m_income = m_upkeep = m_income = m_cash_in = m_cash_out = 0;

	ss << "Week End Final : value    = " << m_value << endl;
	ss << "               : income   = " << m_income << endl;
	ss << "               : upkeep   = " << m_upkeep << endl;
	ss << "               : cash in  = " << m_cash_in << endl;
	ss << "               : cash out = " << m_cash_out << endl;
	g_LogFile.ssend();
}

int cGold::total_income()
{
	cGoldBase *bpt;
	double sum = cGoldBase::total_income();
	for (int i = 0; (bpt = brothels[i]); i++) sum += bpt->total_income();
	return int(floor(sum));
}

int cGold::total_upkeep()
{
	cGoldBase *bpt;
	double sum = cGoldBase::total_upkeep();
	for (int i = 0; (bpt = brothels[i]); i++) sum += bpt->total_upkeep();
	return int(floor(sum));
}

int cGold::total_earned()
{
	cGoldBase *bpt;
	double sum = cGoldBase::total_earned();
	for (int i = 0; (bpt = brothels[i]); i++) sum += bpt->total_earned();
	return int(floor(sum));
}

void cGold::gen_report(int month)
{
}
