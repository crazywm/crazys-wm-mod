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
	cConfig cfg;

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

/*
 * for stuff sold in the marketplace
 *
 * goes straight into the PC's pocket
 */
void cGoldBase::item_sales(double income)
{
	m_value += income;
	m_cash_in += income;
	detail_in.item_sales += income;
}

/*
 * ditto at the slave market
 */
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
//	m_cash_in += income;  // m_cash_in is taxed, so let's not do that
}

/*
 * this is for girls working at the brothel
 *
 * goes into income
 */
void cGoldBase::brothel_work(double income)
{
	m_income += income;
	detail_in.brothel_work += income;
}

/*
 * for when a girl gives birth to a monster
 * The cash goes to the brothel, so we add this to m_income
 */
void cGoldBase::creature_sales(double income)
{
	m_income += income;
	detail_in.creature_sales += income;
}

/*
 * income from movie crystals
 */
void cGoldBase::movie_income(double income)
{
	m_income += income;
	detail_in.movie_income += income;
}

/*
 * income from the bar
 */
void cGoldBase::bar_income(double income)
{
	m_income += income;
	detail_in.movie_income += income;
}

/*
 * income from gambling halls
 */
void cGoldBase::gambling_profits(double income)
{
	m_income += income;
}

/*
 * income from businesess under player control
 */
void cGoldBase::extortion(double income)
{
	m_income += income;
}

/*
 * This happens at end of turn anyway - so let's do it as
 * a delayed transaction
 */
void cGoldBase::objective_reward(double income)
{
	m_income += income;
}

/*
 * same principle as above
 */
void cGoldBase::plunder(double income)
{
	m_income += income;
}

/*
 * ...and again for these three
 */
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
	if(price > m_value || force) {
		return false;
	}
	m_value -= price;
	m_cash_out -= price;
	return true;
}

/*
 * weekly cost of making a movie - delayed transaction
 */
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

/*
 * buying stuff from the market - instant
 */
bool cGoldBase::item_cost(double price)
{
	return debit_if_ok(price);
}
/*
 * counterpart of misc_credit - should probably also go away
 */
bool cGoldBase::misc_debit(double price)
{
	return debit_if_ok(price);
}

/*
 * consumables are things like potions, nets and booze
 * which tend to be auto-stockable
 *
 * so we have a force flag here for the restock call
 */
bool cGoldBase::consumable_cost(double price, bool force)
{
	return debit_if_ok(price, force);
}

/*
 * buying at the slave market - you need the cash
 */
bool cGoldBase::slave_cost(double price)
{
	return debit_if_ok(price);
}

/*
 * buying a new building - cash transactions only please
 */
bool cGoldBase::brothel_cost(double price)
{
	return debit_if_ok(price);
}

/*
 * training is a delayed cost
 */
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

void cGoldBase::bar_upkeep(double cost)
{
	m_upkeep += cost;
}

void cGoldBase::casino_upkeep(double cost)
{
	m_upkeep += cost;
}

void cGoldBase::advertising_costs(double cost)
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

	return pGold;
}

bool cGoldBase::loadGoldXML(TiXmlHandle hGold)
{
	//load default values just in case
	reset();

	TiXmlElement* pGold = hGold.ToElement();
	if (pGold == 0)
	{
		return false;
	}

	pGold->QueryDoubleAttribute("value", &m_value);
	pGold->QueryDoubleAttribute("income", &m_income);
	pGold->QueryDoubleAttribute("upkeep", &m_upkeep);
	pGold->QueryDoubleAttribute("cash_in", &m_cash_in);
	pGold->QueryDoubleAttribute("cash_out", &m_cash_out);
	return true;
}

istream &operator>>(istream& is, cGoldBase &g)
{
	double val, inc, up, cashin, cashout;

	/*
	is >> g.m_value
	   >> g.m_income
	   >> g.m_upkeep
	   >> g.m_cash_in	
	   >> g.m_cash_out
	;
	*/

	is >> val
	   >> inc
	   >> up
	   >> cashin
	   >> cashout
	;
	g.m_value	= val;
	g.m_income	= inc;
	g.m_upkeep	= up;
	g.m_cash_in	= cashin;
	g.m_cash_out	= cashout;

	is.ignore(100, '\n');
	return is;
}


void cGold::brothel_accounts(cGold &g, int brothel_id)
{
	cGoldBase *br_gold = find_brothel_account(brothel_id);
	if(br_gold) {
		(*br_gold) += g;
		return;
	}
	g_LogFile.ss()	<< "Error: can't find account record "
			<< "for brothel id "
			<< brothel_id
			<< endl
	;
	g_LogFile.ssend();
}

void cGold::week_end()
{
	cGoldBase *bpt;
	ostream &ss = g_LogFile.ss();

	ss << gettext("Week End begins: value    = ") << m_value << endl;
	ss << gettext("               : income   = ") << m_income << endl;
	ss << gettext("               : upkeep   = ") << m_upkeep << endl;
	ss << gettext("               : cash in  = ") << m_cash_in << endl;
	ss << gettext("               : cash out = ") << m_cash_out << endl;

	for(int i = 0; (bpt = brothels[i]); i++) {
		(*this) += (*bpt);

		ss << gettext("Added Bothel ")<< i << gettext(": value    = ") << m_value << endl;
		ss << gettext("               : income   = ") << m_income << endl;
		ss << gettext("               : upkeep   = ") << m_upkeep << endl;
		ss << gettext("               : cash in  = ") << m_cash_in << endl;
		ss << gettext("               : cash out = ") << m_cash_out << endl;
		bpt->zero();
	}
	m_value += m_income;
	m_value -= m_upkeep;
	m_income = m_upkeep = m_income = m_cash_in = m_cash_out = 0;

	ss << gettext("Week End Final : value    = ") << m_value << endl;
	ss << gettext("               : income   = ") << m_income << endl;
	ss << gettext("               : upkeep   = ") << m_upkeep << endl;
	ss << gettext("               : cash in  = ") << m_cash_in << endl;
	ss << gettext("               : cash out = ") << m_cash_out << endl;
	g_LogFile.ssend();
}

int cGold::total_income()
{
	cGoldBase *bpt;
	double sum = cGoldBase::total_income();

	for(int i = 0; (bpt = brothels[i]); i++) {
		sum += bpt->total_income();
	}
	return int(floor(sum));
}

int cGold::total_upkeep()
{
	cGoldBase *bpt;
	double sum = cGoldBase::total_upkeep();

	for(int i = 0; (bpt = brothels[i]); i++) {
		sum += bpt->total_upkeep();
	}
	return int(floor(sum));
}

int cGold::total_earned()
{
	cGoldBase *bpt;
	double sum = cGoldBase::total_earned();

	for(int i = 0; (bpt = brothels[i]); i++) {
		sum += bpt->total_earned();
	//	bpt->reset_income();

	}
	return int(floor(sum));
}

/*
 *

 */
void cGold::gen_report(int month)
{
}


/*

 *

 */
