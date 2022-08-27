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

#ifndef WM_SIMAGESPEC_H
#define WM_SIMAGESPEC_H

#include <cstdint>
#include "ids.h"
#include <cassert>

namespace tinyxml2 {
    class XMLElement;
}


struct sImageSpec {
    sImageSpec() = default;
    sImageSpec(EImageBaseType type, ESexParticipants parts,
               ETriValue preg, ETriValue futa, ETriValue tied);
    EImageBaseType BasicImage;
    ESexParticipants Participants = ESexParticipants::ANY;
    ETriValue IsPregnant = ETriValue::Maybe;
    ETriValue IsFuta = ETriValue::Maybe;
    ETriValue IsTied = ETriValue::Maybe;

    static sImageSpec load_from_xml(const tinyxml2::XMLElement& element);
};

bool operator<(const sImageSpec& a, const sImageSpec& b);

struct sImagePreset {
    sImagePreset(EImageBaseType p) : Value((int)p) {};
    sImagePreset(EImagePresets p) : Value((int)p + (int)EImageBaseType::NUM_TYPES) {};
    EImageBaseType base_image() const;
    ESexParticipants participants() const;
    ETriValue tied_up() const;

    constexpr bool operator==(const sImagePreset& other) const { return Value == other.Value; }
private:
    int Value;
};


#endif //WM_SIMAGESPEC_H
