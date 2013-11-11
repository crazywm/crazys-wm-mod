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

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "CLog.h"
#include "DirPath.h"

using namespace std;

class cInterfaceWindowXML;

class cLuaStateInner {
public:
	lua_State       *L;

	void set_lua_path();
static  const std::string sandbox;
	cLuaStateInner();
	~cLuaStateInner() {
		if(L) lua_close(L);
	}
	inline operator lua_State*() {
		return L;
	}
};

class cLuaState {
	static cLuaStateInner *instance;
public:
	cLuaState() {
		if(!instance) instance = new cLuaStateInner();
	}
	inline operator lua_State*() {
		return instance->L;
	}
};

struct sGirl;

class cLuaScript {
	cLuaState	l;
	string		m_file;
	bool		running;
	CLog		log;
/*
 *	these are going to hold lua references
 *	pointers to the script's init() and run() methods
 */
	int		init_ref;
	int		run_ref;
/*
 *	the girl who is the subject (if any) of the event 
 */
	sGirl		*girl;

	string slurp(string path);
	void get_param_table();
	bool get_from_space(const char *func);
public:
	cLuaScript() {
		running = false;
		girl	= 0;
		init_ref= -1;
		run_ref	= -1;
	}
	void log_error();

	void set_param(const char *name, void *pointer);

	bool load(string filename, sGirl *girl);

	bool run(const char *func = "script");
	bool process(cInterfaceWindowXML *window);
	bool refresh(cInterfaceWindowXML *window);
	bool call_handler(cInterfaceWindowXML *window, string handler_name);
	int get_ref(const char *name);
	bool run_by_ref(int ref);
	void set_wm_girl(sGirl *girl);
	void set_wm_player();

	~cLuaScript() {}
};

