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

#include <type_traits>

// based on https://stackoverflow.com/a/31836401
template<typename C>
class EnumIterator {
    using val_t = typename std::underlying_type<C>::type;
    val_t val;
public:
    constexpr explicit EnumIterator(const C & f) : val(static_cast<val_t>(f)) {}
    EnumIterator operator++() {
        ++val;
        return *this;
    }
    C operator*() { return static_cast<C>(val); }
    bool operator!=(const EnumIterator& i) { return val != i.val; }
};

// note: endVal has to be one past the last useful enum value
template<typename C, C beginVal, C endVal>
class EnumRange {
    using iter_t = EnumIterator<C>;
public:
    constexpr EnumRange() = default;
    iter_t begin() const { return iter_t(beginVal); } //default ctor is good
    iter_t end() const {
        return iter_t(endVal);
    }
};