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

#include "ICharacter.h"
#include "predicates.h"

bool is_addict(const ICharacter& character, bool onlyhard) {
    if (onlyhard)
    {
        return character.get_trait_modifier("tag:hard-addiction") > 0;
    }
    return character.get_trait_modifier("tag:addiction") > 0;
}

bool has_disease(const ICharacter& character) {
    return character.get_trait_modifier("tag:disease") > 0;
}

bool is_virgin(const ICharacter& character) {
    return character.has_active_trait("Virgin");
}

bool is_nonhuman(const ICharacter& character) {
    return character.has_active_trait("Not Human");
}

bool is_your_daughter(const ICharacter& character) {
    return character.has_active_trait("Your Daughter");
}
