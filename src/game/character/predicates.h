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

#ifndef WM_PREDICATES_H
#define WM_PREDICATES_H

class ICharacter;

/// returns whether the character is an addict. If `onlyhard`, consider only hard drugs.
bool is_addict(const ICharacter& character, bool onlyhard = false);

/// returns whether the character has a disease.
bool has_disease(const ICharacter& character);

/// returns the number of diseases a character has
int num_diseases(const ICharacter& character);

/// returns whether the character is a futanari
bool is_futa(const ICharacter& character);

/// returns whether the character is a virgin.
bool is_virgin(const ICharacter& character);

/// check whether the character is a non-human
bool is_nonhuman(const ICharacter& character);

/// check whether the character is daughter of the player
bool is_your_daughter(const ICharacter& character);

/// gets her openness for sex, i.e. her willingness to try out things / perform sexual acts in
/// unusual situations. Mostly trait based, but slightly influenced by libido.
int get_sex_openness(const ICharacter& character);

/// Does she have a trait that makes her "need" sex. These are nympho, succubus, and slut
bool is_sex_crazy(const ICharacter& character);

/// Does the character like to have sex with women
bool likes_women(const ICharacter& character);

/// Does the character like to have sex with men
bool likes_men(const ICharacter& character);

#endif //WM_PREDICATES_H
