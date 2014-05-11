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

#include <string>
#include <vector>

#include "cChoiceMessage.h"
#include "cLuaScript.h"
#include "CLog.h"
#include "sConfig.h"
#include "CGraphics.h"

using namespace std;

typedef vector<string *> str_vec;

class cLuaMenuInner 
{
/*
 *	these are all singletons, so having them in the class
 *	is cheap
 */
	cLuaState	l;
	CLog		log;
	cConfig		cfg;
/*
 *	parameters to the initial show method
 */
	int		x, y;
	int		h, w;
	int		maxw, maxh;
	int		num_options;
	string		font_file;
	int		font_size;
	vector<string*>	captions;
	int		lua_callback_ref;

	int get_int(const char *name, int def_val);
	string get_string(const char *name, const char *def_val);
	void whoops(string msg);
	int get_ref(const char *);
	str_vec get_caption_strings();
	str_vec traverse_caption_table();
	void calc_size_from_font(str_vec &v);
	void calc_co_ords(str_vec &v);
	int get_menu_x(int maxw);
	int get_menu_y(int maxh);
	void write_captions();
public:
	void show();
	void clicked(int option_number);
};

class cLuaMenu {
static	cLuaMenuInner *instance;
public:
	void show() {
		instance->show();
	}
	void clicked(int option_number) {
		instance->clicked(option_number);
	}
	cLuaMenu() {
		if(!instance) instance = new cLuaMenuInner();
	}
};
