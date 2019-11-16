/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders 
 * who meet on http://pinkpetal.co.cc
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

#include <climits>
#include <cstdlib>
#include <cstring>
#include <cstdio>

#ifdef LINUX
inline void itoa(int ival, char *buff, int) { sprintf(buff, "%d", ival); }
inline void ltoa(long lval, char *buff, int) { sprintf(buff, "%ld", lval); }
inline void _itoa(int ival, char *buff, int) { sprintf(buff, "%d", ival); }
inline void _ltoa(long lval, char *buff, int) { sprintf(buff, "%ld", lval); }
inline char *_strdup(const char *buff) { return strdup(buff); }
#endif
