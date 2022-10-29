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
    return num_diseases(character) > 0;
}

int num_diseases(const ICharacter& character) {
    return character.get_trait_modifier("tag:disease");
}

bool is_virgin(const ICharacter& character) {
    return character.has_active_trait(traits::VIRGIN);
}

bool is_nonhuman(const ICharacter& character) {
    return character.has_active_trait(traits::NOT_HUMAN);
}

bool is_your_daughter(const ICharacter& character) {
    return character.has_active_trait(traits::YOUR_DAUGHTER);
}

bool is_futa(const ICharacter& character) {
    return character.has_active_trait(traits::FUTANARI);
}

int get_sex_openness(const ICharacter& character) {
    int trait = character.get_trait_modifier("sex.openness");
    int open = trait + character.libido() / 2 - std::max(0, character.dignity() / 3);
    // specific modifiers
    // very high dignity or very low confidence
    if(character.dignity() > 80)  open -= 5;
    if(character.confidence() < 20) open -= 5;
    // low dignity and sufficient confidence
    if(character.dignity() < 0 && character.confidence() > 35) {
        open += 10;
    }
    // virgin and sick of it
    if(is_virgin(character) && (character.libido() > 80 || character.age() > 22)) {
        open += 10;
    }
    // unhappy or unhealthy or too tired
    if(character.health() < 25 || character.happiness() < 25 || character.tiredness() > 80) {
        open -= 15;
    }
    return open;
}

bool is_sex_crazy(const ICharacter& character) {
    return character.any_active_trait({traits::NYMPHOMANIAC, traits::SUCCUBUS, traits::SLUT});
}

bool likes_women(const ICharacter& character) {
    return character.any_active_trait({traits::LESBIAN, traits::BISEXUAL});
}

bool dislikes_women(const ICharacter& character) {
    if (character.has_active_trait(traits::STRAIGHT)) return true;
    return false;
}

bool likes_men(const ICharacter& character) {
    return !character.has_active_trait(traits::LESBIAN);
}

bool dislikes_men(const ICharacter& character) {
    if (character.has_active_trait(traits::LESBIAN)) return true;
    return false;
}