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

#if !defined(SEEN_UTILS_DOCTEST_UTILS_2020_10_15_)
#define SEEN_UTILS_DOCTEST_UTILS_2020_10_15_ 1

#include <vector>
#include <string>
#include <sstream>

#include "doctest.h"

// Tell doctest how to print std::vector<>
namespace doctest
{
   template<typename T, typename Alloc>
   struct StringMaker<std::vector<T, Alloc>>
   {
      static String convert(std::vector<T, Alloc> const& vec)
      {
         std::ostringstream os;

         std::string sep = "";

         os << "{";
         for(auto const& el : vec)
         {
            os << sep << el;
            sep = ", ";
         }
         os << "}";

         return os.str().c_str();
      }
   };
}

#endif
