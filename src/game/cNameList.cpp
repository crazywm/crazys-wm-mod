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
#include <fstream>
#include "Constants.h"
#include "cNameList.h"
#include "CLog.h"
#include "cRng.h"
#include "utils/string.hpp"

using namespace std;

extern cRng g_Dice;

cNameList::cNameList(string file)
{
    load(file);
}

void cNameList::load(string file)
{
   /*
    *    open the file
    */
    ifstream in(file.c_str());
    if (!in)
    {
        g_LogFile.log(ELogLevel::ERROR, "Unable to open file '", file, "'");
        return;
    }

    std::vector<std::string> names;

    readline(in);         // ignore the first line (it's a line count)
    while(in.good())      // read until EOF
    {
       auto name = readline(in);
       if(!name.empty())
          names.emplace_back(std::move(name));
    }

    m_names = std::move(names);

    /*
    *    quick sanity check
    */
    if (m_names.empty())
    {
        g_LogFile.log(ELogLevel::ERROR, "No names found in file '", file, "'");
        return;
    }
}

string cNameList::random()
{
    if (m_names.empty())
    {
        g_LogFile.log(ELogLevel::ERROR, "No names in cNameList: Returning name '<ERROR>'");
        /*
        *        make sure we see the bug reports ...
        */
        return "<ERROR>";
    }
    /*
    *    otherwise, pick a random name
    */
    unsigned size = m_names.size();
    return m_names[g_Dice.random(size - 1)];  // edited from size to size-1 since I got an OOB vector crash once
}
