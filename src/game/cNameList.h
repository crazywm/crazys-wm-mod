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
#pragma once
#include <vector>
#include <string>

class cNameList {
    std::vector<std::string> m_names;
public:
    cNameList() = default;
    cNameList(std::string file);
    std::string random();
    void load(std::string file);
};

class cDoubleNameList {
    cNameList m_first;
    cNameList m_last;
public:
    cDoubleNameList() = default;
    cDoubleNameList(std::string first, std::string last)
    : m_first(first), m_last(last)
    {
    }
    void load(std::string first, std::string last) {
        m_last.load(last);
        m_first.load(first);
    }
    std::string random() {
        std::string s;
        s += m_first.random();
        s += " ";
        s += m_last.random();
        return s;
    }
};
