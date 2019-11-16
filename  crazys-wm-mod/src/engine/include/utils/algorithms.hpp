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


#ifndef CRAZYS_WM_MOD_ALGORITHMS_HPP
#define CRAZYS_WM_MOD_ALGORITHMS_HPP

#include <algorithm>

template<class T, class F>
std::ptrdiff_t erase_if(T& container, F&& predicate) {
    auto size_start = container.size();
    container.erase(std::remove_if(container.begin(), container.end(), std::forward<F>(predicate)), container.end());
    return container.size() - size_start;
}

template<class T, class V>
auto find(const T& container, V&& value)
{
    return std::find(begin(container), end(container), std::forward<V>(value));
}

template<class T, class F>
auto find_if(const T& container, F&& predicate)
{
    return std::find_if(begin(container), end(container), std::forward<F>(predicate));
}

template<class T>
bool is_in(T value, std::initializer_list<T> set) {
    return std::any_of(begin(set), end(set), [&](const T& ref){ return ref == value; });
}

#endif //CRAZYS_WM_MOD_ALGORITHMS_HPP
