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

#include <string>
#include <map>
#include <iomanip>
#include <math.h>
#include "CLog.h"
#include "tinyxml.h"

using namespace std;

extern CLog g_LogFile;

struct sBrothel;

string intstring(int);

class cGoldBase
{
	struct in {
		double brothel_work;

		double street_work;
		double movie_income;
		double bar_income;
		double gambling_profits;
		double item_sales;
		double slave_sales;
		double creature_sales;
		double extortion;
		double plunder;
		double petty_theft;
		double grand_theft;
		double catacomb_loot;
		double objective_reward;
		double bank_interest;
		double misc;
		double clinic_income;
		double arena_income;
		double farm_income;

		//01234567890123456789012345678901234567890123456789012345678901234567890123456789
		// --- Whores ---                              --- Sales ---
		// Brothel  Street   Movie     Bar  Casino   Items  Monster Loc'Biz   Raids P.Theft G.Theft C'combs  Reward Intr'st    Misc
		//  123456 1234567 1234567 1234567 1234567 1234567  1234567 1234567 1234567 1234567 1234567 1234567 1234567 1234567 1234567
		string str(int brothel_no = -1) {
			stringstream ss;
			ss << "  --- Whores ---                              --- Sales ---";
			ss << "# Brothel  Street   Movie     Bar  Casino   Items  "
				"Monster Loc'Biz   Raids P.Theft G.Theft C'combs  "
				"Reward Intr'st    Misc      Clinic		Arena"
				;
			if (brothel_no == -1) {
				ss << brothel_no << " ";
			}
			else {
				ss << "  ";
			}
			ss << setw(7) << brothel_work << " ";
			ss << setw(7) << street_work << " ";
			ss << setw(7) << movie_income << " ";
			ss << setw(7) << bar_income << " ";
			ss << setw(7) << gambling_profits << " ";
			ss << setw(7) << item_sales << " ";
			ss << setw(7) << slave_sales << " ";
			ss << setw(7) << creature_sales << " ";
			ss << setw(7) << extortion << " ";
			ss << setw(7) << plunder << " ";
			ss << setw(7) << petty_theft << " ";
			ss << setw(7) << grand_theft << " ";
			ss << setw(7) << catacomb_loot << " ";
			ss << setw(7) << objective_reward << " ";
			ss << setw(7) << bank_interest << " ";
			ss << setw(7) << misc << " ";
			ss << setw(7) << clinic_income << " ";
			ss << setw(7) << arena_income << " ";
			ss << setw(7) << farm_income << " ";
			ss << endl;
			return ss.str();
		}
	} detail_in;

	struct out {
		double brothel_cost;
		double slave_cost;
		double item_cost;
		double consumable_cost;
		double movie_cost;
		double goon_wages;
		double staff_wages;
		double girl_support;
		double girl_training;
		double building_upkeep;
		double bar_upkeep;
		double casino_upkeep;
		double advertising_costs;
		double centre_costs;
		double arena_costs;
		double clinic_costs;
		double bribes;
		double fines;
		double tax;
		double rival_raids;
		double misc_debit;
	} detail_out;

	//   -------     Purchase    -------  Making --  Support  --    Girl ------   Upkeep  ------
	// # Brothel  Slaves   Items Consume  Movies    Goon    Girl Train'g Build'g     Bar  Casino Adverts  Bribes   Fines Raiders
	//   1234567 1234567 1234567 1234567 1234567 1234567 1234567 1234567 1234567 1234567 1234567 1234567 1234567 1234567 1234567

protected:

	/*
	*	Two types of transaction here: instant and delayed.
	*	Instant transactions show up in the gold total immediatly.
	*	Delayed transactions are settled at the end of the turn.
	*	Instant transactions are sometimes called cash transactions
	*	because they're effectively things the PC buys with the cash in
	*	his pocket, or where he sells something and pockets the cash
	*
	*	Delayed expenditure gets paid whether you have the gold or not,
	*	possibly driving you into debt. Instant expenditure can only happen
	*	if you have the cash in hand.
	*
	*	we're using doubles here because a lot of prices are going to be
	*	floating point values after config factors are applied: might as well
	*	track the decimals.
	*/
	double	m_initial_value;
	double	m_value;
	double	m_upkeep;
	double	m_income;
	/*
	*	cash transactions are applied directly to value
	*	but we need to record how much came in and went out
	*	so we can calculate total earnings and expenditure
	*	for the turn
	*/
	double	m_cash_in;
	double	m_cash_out;
	/*
	*	this lets us combine the "if pc can afford it" test
	*	with the actual debit, streamlining cash expediture
	*/
	bool	debit_if_ok(double price, bool force = false);
	// `J` added interest rate for .06.03.01
	double m_interest_rate;

public:
	cGoldBase();
	cGoldBase(int initial);
	/*
	*	use these to save and load
	*	they save "value income upkeep" all on one line by itself
	*
	*	If I expand the variables tracked here, I'll mod the stream
	*	operators accodingly.
	*/
	friend	istream &operator>>(istream& is, cGoldBase &g);
	/*
	*	save and load methods
	*/
	TiXmlElement* saveGoldXML(TiXmlElement* pRoot);
	bool loadGoldXML(TiXmlHandle hGold);
	/*
	*	type conversion methods
	*/
	string	sval();
	int	ival();

	void	reset();
	void	cheat()	{ m_value = 999999; }
	void	zero()	{ m_upkeep = m_income = m_cash_in = m_cash_out = m_value = 0.0; }

	/*
	*	these are bools - if you can't afford it,
	*	the transaction will fail and return false
	*/
	bool brothel_cost(double price);
	bool slave_cost(double price);
	bool item_cost(double cost);
	bool consumable_cost(double cost, bool force = false);
	void movie_cost(double cost);
	/*
	*	these are paid whether or not you can
	*	afford it. You just go into debt.
	*/
	void goon_wages(double cost);
	void staff_wages(double cost);
	void girl_support(double cost);
	void girl_training(double cost);
	void building_upkeep(double cost);
	void bar_upkeep(double cost);
	void casino_upkeep(double cost);
	void advertising_costs(double cost);
	void centre_costs(double cost);
	void arena_costs(double cost);
	void clinic_costs(double cost);
	void bribes(double cost);
	void fines(double cost);
	void tax(double cost);
	void rival_raids(double cost);
	/*
	*	income methods
	*/
	void brothel_work(double income);
	void item_sales(double income);
	void slave_sales(double income);
	void creature_sales(double income);
	void movie_income(double income);
	void clinic_income(double income);
	void arena_income(double income);
	void farm_income(double income);
	void bar_income(double income);
	void gambling_profits(double income);
	void extortion(double income);
	void objective_reward(double income);
	void plunder(double income);		// from raiding rivals
	void petty_theft(double income);
	void grand_theft(double income);
	void catacomb_loot(double income);
	/*
	*	this doesn't get added to the player's
	*	cash in hand - it's just here for
	*	accounting purposes
	*/
	void bank_interest(double income);
	// `J` added interest rate for .06.03.01
	double interest_rate();
	double reset_interest_rate();
	double increase_interest_rate();
	double decrease_interest_rate();


	/*
	*	the "misc" methods never get factored
	*	they're for gold that's already in the system
	*	for instance bank transactions, or taking gold
	*	from your girls
	*/
	void misc_credit(double amount);
	bool misc_debit(double amount);

	bool afford(double amount) {
		return amount <= m_value;
	}

	cGoldBase &operator -=(const cGoldBase &rhs)
	{
		m_value -= rhs.m_value;
		m_income -= rhs.m_income;
		m_upkeep -= rhs.m_upkeep;
		return *this;
	}

	cGoldBase &operator +=(const cGoldBase &rhs)
	{
		m_value += rhs.m_value;
		m_income += rhs.m_income;
		m_upkeep += rhs.m_upkeep;
		return *this;
	}

	/*
	*	some convienience methods
	*/
	int total_income()	{ return int(m_income); }
	int total_upkeep()	{ return int(m_upkeep); }
	int total_earned()	{ return int(m_income + m_cash_in); }
	int total_profit()	{
		double d = total_earned() - total_upkeep();
		return int(d);
	}
};

class cGold : public cGoldBase
{
	map<int, cGoldBase *> brothels;
	cGoldBase *find_brothel_account(int id) {
		cGoldBase *ac_pt = brothels[id];
		if (ac_pt == 0) {
			ac_pt = new cGoldBase(0);
			brothels[id] = ac_pt;
		}
		return ac_pt;
	}
public:
	cGold() : cGoldBase()		 { }
	cGold(int initial) : cGoldBase(initial) { }
	/*
	*	take the brothel's cGold field (m_Finance - replaces
	*	m_Upkeep and m_Income) and add it to the record
	*	Passing the brothel struct so we can record the
	*	transactions against the brothel ID
	*
	*	(Better would be to pass m_Finance and ID separately
	*	and loosen the module coupling)
	*/
	void	brothel_accounts(cGold &g, int brothel_id);
	void	week_end();
	int	total_income();
	int	total_upkeep();
	/*
	*	I'm not sure whether it's easier to make the total funcs
	*	virtual, or just to repeat the inlines for the base class
	*
	*	as long as it's just the two, repeating the inlines
	*	might be better
	*/
	int total_earned();//	{ return m_income + m_cash_in; }
	int total_profit()	{
		return	int(total_earned() - total_upkeep());
	}
	void gen_report(int month);
};


