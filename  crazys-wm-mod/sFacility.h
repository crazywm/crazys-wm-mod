/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders 
 * who meet on http://pinkpetal.co.cc
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

#include <math.h>
#include "cTariff.h"

using namespace std;

/*
 * I don't generally see the point of using chars over ints
 * for stat values - most PCs have more than enough memory to make the
 * space considerations trivial.
 *
 * that said, this would be an easy change to a template class
 * if we did want a char based version. Then a sBoundedVar would
 * use three times the memory of a char rather than 12 times, which
 * could be important if we're looking at porting to other platforms
 *
 * We could use static constants for the min and max values as well,
 * although we'd need a new subclass for each set of bounds and then to 
 * override the constants. Then of course we'd probably need a virtual accessor
 * to make sure we got the right constants ... and then the vtable would 
 * wipe out any savings in memory from the two ints (or chars).
 *
 * probably easier to stick with three values, all told.
 */
struct sBoundedVar
{
	int	m_min;
	int	m_max;
	int	m_curr;
/*
 *	defaults are set up for facility adjusters
 *	so range is 0-9, default to zero
 */
	sBoundedVar() {
		m_min = 0;
		m_max = 9;
		m_curr = 0;
	}
/*
 *	but we could create one with any range
 */
	sBoundedVar(int min, int max, int def=0) {
		m_min = min;
		m_max = max;
		m_curr = def;
	}
/*
 *	methods for adjuster buttons - simple increment with
 *	bounds checking
 */
	void up() { if(m_curr < m_max) m_curr ++; }
	void down() { if(m_curr > m_min) m_curr --; }
/*
 *	operators = += -= 
 */
	int operator=(int val) { m_curr = val;  return bound(); }
	int operator+=(int val) { m_curr += val;  return bound(); }
	int operator-=(int val) { m_curr -= val; return bound(); }
	int bound() {
		if(m_curr < m_min) m_curr = m_min;
		if(m_curr > m_max) m_curr = m_max;
		return m_curr;
	}
/*
 *	rather than a save method, just return an XML element
 *	that can be linked into a larger tree
 *
 *	The XML is going to look something like this:
 *
 *	<BoundedVar
 *		Name	= "Glitz"
 *		Min	= "0"
 *		Max	= "9"
 *		Curr	= "3"
 *	/>
 */
	TiXmlElement	*to_xml(string name);
	bool		from_xml(TiXmlElement *el);
};

struct sFacility;

/*
 * this is a bit of a mess from a model-view-controller point of view
 * but I'm not clear how to untangle it right now.
 *
 * So I'll make it work for now, and untangle it later
 */
struct sBoundedVar_Provides : public sBoundedVar
{
	int m_inc;			// increment - this per bump
	int m_space;			// space taken
	double m_slots_per_space;	// how many slots for one space?

/*
 *	we need to know how much extra space a bump would consume
 */
 	int space_needed() {
/*
 *		we always use a whole space
 *		if we get 4 kennels per space, then we bump
 *		in bundles of 4 slots and one space
 */
		if(m_slots_per_space >= 1) {
			return 1;
		}
/*
 *		otherwise we return the spaces needed for the
 *		next whole slot. Fractional slots are dropped
 */
 		return int(floor(
			(m_curr + 1) / m_slots_per_space	
		)) - m_space;
	}
/*
 *	same exercise from a slot perspective
 */
	int slots_needed() {
		if(m_slots_per_space < 1) {
			return 1;
		}
		return int(ceil(
			(m_space + 1) * m_slots_per_space
		)) - m_curr;
	}

	void init(sFacility *fac);

	TiXmlElement	*to_xml(string name);
	bool		from_xml(TiXmlElement *el);

	void up() {
		int slot_inc = slots_needed();
		if(m_curr + slot_inc > m_max) {
			return;
		}
		m_space += space_needed();
		m_curr += slot_inc;
	}

/*
 *	this is a little complicated
 *
 *	if a slot takes more than one space
 *	then we drop enough spaces to reduce the slot count
 *	so if the facility is a 4 space apartment suite,
 *	we drop by 4 spaces and one slot
 *
 *	on the other hand, if there are multiple slots per space
 *	we want to drop a space, and reduce the slot count to the maximum
 *	that fit in the new size. So if we get 4 kennels to the space,
 *	we reduce by 1 space and 4 kennel slots.
 */
	void down() {
		if(m_curr <= m_min) {
			return;
		}
/*
 *		The simplest case is if we get one slot per space
 *		this probably collapses elegantly into one of the
 *		other cases, but since I'm having trouble getting
 *		my head around the problem, I'm going to invoke the KISS principle.
 *		Keep It Simple, Stupid.
 */
 		if(m_slots_per_space == 1.0) {
			m_curr --;
			m_space --;
			return;
		}
/*
 *		if the slots-per-space count is more than one
 *		we can just drop a space and re-calculate
 */
 		if(m_slots_per_space > 1.0) {
			m_space --;
/*
 *			just because we get more than one
 *			that doesn't mean we get a whole number
 *			it might be a 3-for-two deal, for instance
 *
 *			so we use floor to drop any fractional slots
 *			from the calculation
 */
			m_curr = int(floor(m_slots_per_space * m_space));
			return;
		}
/*
 *		if we get here, we get less than one slot for a space
 *		so we drop the slot count by one, and then re-calculate space
 *		instead.
 */
 		m_curr --;
/*
 *		again, we might not get a whole number - dorms use 6 slots 
 *		in 2 spaces, for instance. So make sure the space requirement
 *		rounds UP
 */
		m_space = int(ceil(m_curr / m_slots_per_space));
	}
	int bound() {
		if(m_curr < m_min) m_curr = m_min;
		if(m_curr > m_max) m_curr = m_max;
/*
 *		we're setting the slot count here, 
 *		so calculate the space based on that
 */
		m_space = int(ceil(m_curr / m_slots_per_space));
		return m_curr;
	}
};

struct sFacility
{
	string		m_type_name;
	string		m_instance_name;
	string		m_desc;
	int		m_space_taken;
	int		m_slots;
	int		m_base_price;
	int		m_paid;
	sBoundedVar_Provides	m_provides;
	sBoundedVar	m_glitz;
	sBoundedVar	m_secure;
	sBoundedVar	m_stealth;
	bool		new_flag;
	cTariff		tariff;

	sFacility() {
		m_type_name = "";
		m_instance_name = "";
		m_desc = "";
		m_space_taken = 0;
		m_slots = 0;
		m_base_price = 0;
		new_flag = false;
	}
	sFacility(const sFacility& f) {
		m_base_price	= f.m_base_price;
		m_desc		= f.m_desc;
		m_instance_name	= f.m_instance_name;
		m_slots		= f.m_slots;
		m_space_taken	= f.m_space_taken;
		m_type_name	= f.m_type_name;
		new_flag	= f.new_flag;
	}

	void	commit()	{
		new_flag	= false;
		m_base_price	= 0;
	}

	string	name()		{
		if(m_instance_name != "") {
			return m_instance_name;
		}
		return m_type_name;
	}

	string	desc()		{ return m_desc; }
	string	type()		{ return m_type_name; }
	int	space_taken()	{ return m_space_taken; }
	int	slots()		{ return m_slots; }
	int	price()		{ return tariff.buy_facility(m_base_price); }
	int	glitz()		{ return m_glitz.m_curr; }
	void	glitz_up()	{ m_glitz.up(); }
	void	glitz_down()	{ m_glitz.down(); }

	int	secure()	{ return m_secure.m_curr; }
	void	secure_up()	{ m_secure.up(); }
	void	secure_down()	{ m_secure.down(); }

	int	stealth()	{ return m_stealth.m_curr; }
	void	stealth_up()	{ m_stealth.up(); }
	void	stealth_down()	{ m_stealth.down(); }

	void	load_from_xml(TiXmlElement *el);

	sFacility *clone()	{ return new sFacility(*this); }
};

/*

 *

 */
