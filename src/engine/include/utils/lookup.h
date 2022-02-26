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

#ifndef WM_LOOKUP_H
#define WM_LOOKUP_H

#include <unordered_map>
#include "CLog.h"
#include <cassert>
#include "string.hpp"

template<class T>
using id_lookup_t = std::unordered_map<std::string, T, sCaseInsensitiveHash, sCaseInsensitiveEqual>;

template<class T, std::size_t N>
inline id_lookup_t<T> create_lookup_table(const std::array<const char*, N>& names) {
    id_lookup_t<T> lookup;
    for(std::size_t i = 0; i < N; ++i) {
        auto inserted = lookup.insert(std::make_pair(names[i], static_cast<T>(i)));
        assert(inserted.second);
    }
    return std::move(lookup);
}

template<class T>
inline auto lookup_with_error(const T& map, const std::string& name, const char* error_msg) {
    try {
        return map.at(name);
    } catch (const std::out_of_range& oor ) {
        g_LogFile.error("lookup", error_msg, ": '", name, "'");
        throw;
    }
}

#endif //WM_LOOKUP_H
