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


#ifndef CRAZYS_WM_MOD_STRING_HPP
#define CRAZYS_WM_MOD_STRING_HPP

#include <cctype>
#include <string>
#include <algorithm>
#include <functional>
#include <cstring>
#include <istream>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
class cRng;

inline std::string tolower(std::string str) {
    std::transform(begin(str), end(str), begin(str), [](char c) { return std::tolower(c); });
    return std::move(str);
}

inline std::string toupper(std::string str) {
    std::transform(begin(str), end(str), begin(str), [](char c) { return std::toupper(c); });
    return std::move(str);
}

inline bool iequals(const std::string& a, const std::string& b)
{
    if(a.size() != b.size()) return false;
    return std::equal(a.begin(), a.end(),
                      b.begin(),
                      [](char a, char b) {
                          return tolower(a) == tolower(b);
                      });
}

inline std::size_t case_insensitive_hash(const std::string& str)
{
    std::size_t result = 0;
    for(auto& c : str) {
        result ^= std::tolower(c) + 0x9e3779b9 + (result << 6u) + (result >> 2u);
    }
    return result;
}

inline std::size_t case_insensitive_hash(const char* str)
{
    std::size_t result = 0;
    for(auto c = str; *c; ++c) {
        result ^= std::tolower(*c) + 0x9e3779b9 + (result << 6u) + (result >> 2u);
    }
    return result;
}

// Read one line from `is`. The terminating newline is '\n', '\r' (or
// '\r\n'. End-of-file also ends the line.
std::string readline(std::istream& is);


using boost::algorithm::starts_with;
using boost::algorithm::split;

/*!
 * Given a `text_template` that contains replacement specifiers of the form `${...}` and alternatives `$[...],
 * this template is output into `target` with the replacements chosen by `lookup`.
 * The return of lookup is allowed to contain a replacement again, which will be handled
 * recursively. However, it is not possible to form new replacements dynamically.
 */
void interpolate_string(std::ostream& target, const std::string& text_template,
                        const std::function<std::string(const std::string&)>& lookup, cRng& random);

/// interpolates a string and returns the result.
std::string interpolate_string(const std::string& text_template,
                               const std::function<std::string(const std::string&)>& lookup, cRng& random);

#endif //CRAZYS_WM_MOD_STRING_HPP
