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

#ifndef CRAZYS_WM_MOD_SATTRIBUTE_H
#define CRAZYS_WM_MOD_SATTRIBUTE_H

#include "Constants.h"

/*!
 * \brief Represents the global data of an attribute.
 * \details Attributes are Stats and Skills. For each attribute, it defines a long and a short name,
 * a description, and minimum, maximum and default value.
 * Minimum and maximum are the absolute extreme values that a character can have effectively. This means that
 *  1) Depending on circumstances, a character might have a lower cap or higher minimum for an attribute.
 *  2) The basic value of such an attribute always has to be between min and max
 *  3) After applying all modifiers, the attributes effective value is clamped to [min, max].
 */
struct sAttribute {
	// textural representation
	const char* name;
	const char* short_name;
	const char* description;

	// value info
	int min = 0;			//!< Absolute minimum value this attribute can have.
	int max = 100;			//!< Absolute maximum value this attribute can have.
	int default_value = 0;

	// metadata
	bool permanent = true;	//!< TODO figure out a good name for this. Set to false for attributes like HitPoints, happiness, tiredness, mana
};

struct sAttributeValue {
	int m_Value          = 0;			//!< the inherent base value of the attribute
	int m_PermanentMods  = 0;			//!< bonus and mali to the attribute that are due to items, traits etc. This value should be reconstructible from other data, and thus need not be saved.
	int m_TempMods       = 0;			//!< temporary modifiers (spells, potions etc)
};


#endif //CRAZYS_WM_MOD_SATTRIBUTE_H
