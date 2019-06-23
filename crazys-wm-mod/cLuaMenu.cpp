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
#include "cLuaMenu.h"
#include "cChoiceMessage.h"

extern CGraphics g_Graphics;
extern cChoiceManager g_ChoiceManager;

extern cConfig cfg;
/*
 * OK: this is a little bit complicated
 *
 * The menu is defined in Lua where there's a call like this:
 *
 *	menu.show {
 *		x = 100, y = 100,
 *		w = 100, h = 100,
 *		font_file = "neuropol.ttf",
 *		font_size = 24,
 *		captions = {
 *			"Do something",
 *			"Do something else",
 *			"Do what John?",
 *		},
 *		callback = lua_function
 *	}
 *
 * Most of that can be defaulted. Really, only the options are
 * needed. Everythign else can be calculated on the fly or 
 * taken from the config file.
 *
 * There could also be some color options, although I'll leave them
 * to the config file for now
 * 
 * ******
 *
 * The sequence of events bears documenting too:
 *
 *	1 : user triggers menu
 *	2 : menu displayed, as described above
 *	3 : user clicks on menu option
 *	4 : choice manager calls C callback function
 *	5 : callback frees menu resources
 *	6 : callback calls lua callback function
 */

cLuaMenuInner *cLuaMenu::instance = 0;

int cLuaMenuInner::get_int(const char *name, int def_val)
{
	int rv = def_val;
/*
 *	assuming the arg table is on top
 *	push the key name on top of that
 */
	lua_pushstring(l, name);
/*
 *	get the value that goes with the key
 */
	lua_gettable(l, -2);
/*
 *	if the new top value is not nil
 *	it must be our number
 */
 	if(!lua_isnil(l, -1)) {
		rv = lua_tointeger(l, -1);
	}
/*
 *	pop the value off the stack, putting it back to the
 *	state it was in when we found it
 */
	lua_pop(l, 1);
/*
 *	and return the return value
 */
 	return rv;
}

/*
 * same again, but with a string value
 */
string cLuaMenuInner::get_string(const char *name, const char *def_val)
{
	string str = def_val;
/*
 *	assuming the arg table is on top
 *	push the key name on top of that
 */
	lua_pushstring(l, name);
/*
 *	get the value that goes with the key
 */
	lua_gettable(l, -2);
/*
 *	if the new top value is not nil
 *	it must be our number
 */
 	if(!lua_isnil(l, -1)) {
		str = lua_tostring(l, -1);
	}
/*
 *	pop the value off the stack, putting it back to the
 *	state it was in when we found it
 */
	lua_pop(l, 1);
/*
 *	and return the return value
 */
 	return str;
}

static void c_callback(int chosen);

void cLuaMenuInner::whoops(string msg)
{
	lua_pushstring(l, msg.c_str());
	lua_error(l);	// never returns
}

/*
 * OK. we need the user to pass us a lua handler.
 *
 * we could ask for a function name, but if we do that we need
 * to get the namespace passed, and then worry about nested namespaces
 * and in general it gets messy.
 *
 * or - they could just pass the function, functions being first
 * class citizens in Lua. The problem there is storing the function between
 * invocations. Which is what the reference system is for, of course
 */
int cLuaMenuInner::get_ref(const char *name)
{
/*
 *	assuming the arg table is on top
 *	push the key name on top of that
 */
	lua_pushstring(l, name);
/*
 *	get the value that goes with the key
 */
	lua_gettable(l, -2);
/*
 *	slight change from the established pattern here:
 *	if the new top value is nil
 *	then pop the value and return -1
 *
 *	this is OK - it just means the menu will do nothing
 */
 	if(lua_isnil(l, -1)) {
		lua_pop(l, 1);
		return -1;
	}
/*
 *	a bit of type checking is in order here
 */
 	if(lua_type(l, -1) != LUA_TFUNCTION) {
		whoops("cLuaMenuInner::show requires a function for 'callback'");
	}
/*
 *	OK - to store the function we need to convert it into a reference
 *	A reference is just an integer index into the registry
 */
	log.ss() << "storing lua callback as reference";
	log.ssend();
	int index = luaL_ref(l, LUA_REGISTRYINDEX);
	log.ss() << "storED lua callback as reference";
	log.ssend();
/*
 *	that clears the callback off the stack
 *	so we just need to return the reference value
 */
 	return index;
}


str_vec cLuaMenuInner::get_caption_strings()
{
	str_vec v;
	int arg_table = lua_gettop(l);
/*
 *	let's just make sure that really is a table on there
 */
 	if(lua_type(l, -1) != LUA_TTABLE) {
		log.ss() << "error: parameter table not no top of stack";
		log.ssend();
	}
/*
 *	the argument table contains a sub-table under the
 *	key "captions". We need that on top of the stack
 */
 	lua_pushstring(l, "captions");
	lua_gettable(l, arg_table);
/*
 *	now we need to traverse the caption table
 */
	v = traverse_caption_table();
/*
 *	before we return, since we put the caption table
 *	on the stack, we should pop it back off
 */
	lua_pop(l, 1);
/*
 *	and return the strings we collected
 */
 	return v;
}

str_vec cLuaMenuInner::traverse_caption_table()
{
	str_vec v;
	int choice_table = lua_gettop(l);
/*
 *	the table should be on top,
 *	we use lua_next to interate over it
 *	we start by pushing nil onto the stack
 *	to indicate that we need to start from the beginning
 */
 	lua_pushnil(l);
/*
 *	Now lua_next will walk us through the table
 */
 	for(;;) {
/*
 *		zero return means "iteration complete"
 */
		if(lua_next(l, choice_table) == 0) {
			break;
		}
/*
 *		otherwise, the next key will replace the
 *		old key (or nil) and the value will be
 *		on top of that
 */
 		string option_pt = lua_tostring(l, -1);
		//log.ss() << "traverse: '" << option_pt << "'";
		//log.ssend();

/*
 *		we need some storage for the string
 *		the doesn't depend on the Lua stack
 */
 		string *strpt = new string(" ");
		(*strpt) += option_pt;
		v.push_back(strpt);
/*
 *		we need to leave the key on top of the stack so lua_next
 *		can get the one after it. So we need to pop off the
 *		value which is currently on top of it
 */
		lua_pop(l, 1);
	}
	return v;
}

void cLuaMenuInner::calc_size_from_font(str_vec &v)
{
	unsigned int i;

	maxh = maxw = 0;

	TTF_Font *fontpt = TTF_OpenFont(font_file.c_str(), font_size);

	for(i = 0; i < v.size(); i++) {
		int loc_w, loc_h;
		const char *pt = v[i]->c_str();
/*
 *		for each menu caption, we want the height and width
 *		so we know how big and wide to make the menu
 */
		if(TTF_SizeText(fontpt, pt, &loc_w, &loc_h) == -1) {
			log.ss() << "Error: can't render '" << pt << "'";
			log.ssend();
			continue;
		}
		maxw = loc_w > maxw ? loc_w : maxw;
		maxh = loc_h > maxh ? loc_h : maxh;
		//g_ChoiceManager.AddChoice(0, pt, i);
	}
	log.ss() << "width  = " << maxw << "\n";
	log.ss() << "height = " << maxh << "\n";
	log.ssend();
}

void cLuaMenuInner::calc_co_ords(str_vec &v)
{
/*
 *	get the maximum w and h needed to fully display all the captions
 */
	calc_size_from_font(v);
/*
 *	set the width and height from the calculated values
 *	unless values were supplied as parameters
 */
	if(w == -1) w = maxw;
	if(h == -1) h = maxh;
/*
 *	now we have a width, calculate the x co-ord to center it
 *	unless of course an x co-ord was passed as a parameter
 */
 	if(x == -1) {
		x = get_menu_x(maxw);
	}
/*
 *	same for y axis
 */
 	if(y == -1) {
		y = get_menu_y(maxh);
	}
}

int cLuaMenuInner::get_menu_x(int maxw)
{
/*
 *	get the screen width first
 */
 	int screen_w = g_Graphics.GetWidth();
/*
 *	Now: if the menu is too wide for the screen
 *	we return 0 so as to give the string maximum room to display
 */
 	if(maxw >= screen_w) {
		return 0;
	}
/*
 *	if not, get the difference and divide by two
 */
	return (screen_w - maxw) / 2;
}

int cLuaMenuInner::get_menu_y(int maxh)
{
/*
 *	similarly, get the screen height 
 */
 	int screen_h = g_Graphics.GetHeight();
/*
 *	Now: if the menu is too wide for the screen
 *	we return 0 so as to give the string maximum room to display
 */
 	if(maxh >= screen_h) {
		return 0;
	}
/*
 *	if not, get the difference and divide by two
 */
	return (screen_h - maxh) / 2;
}

void cLuaMenuInner::write_captions()
{
	u_int i;

	for(i = 0; i < captions.size(); i++) {
		const char *pt = captions[i]->c_str();
		g_ChoiceManager.AddChoice(0, pt, i);
	}
}

void cLuaMenuInner::show()
{
/*
 *	we have a shedload of args to decode
 *	these are all passed from Lua in a table
 */
	x = get_int("x", -1);
	y = get_int("y", -1);
	w = get_int("w", -1);
	h = get_int("h", -1);
/*
 *	font details
 */
	font_size = get_int("font_size", 24);
	font_file = get_string("font_file", cfg.fonts.normal().c_str());
/*
 *	get a reference to the lua function we need to call 
 *	when the user makes a choice
 */
 	lua_callback_ref = get_ref("callback");
/*
 *	OK. that was reasonably efficient.
 *	now we need to get the option strings
 */
	log.ss() << "getting caption strings";
	log.ssend();
 	captions = get_caption_strings();
/*
 *	next we need to calculate co-rds for the values not
 *	specified
 */
	log.ss() << "calculating co-ords";
	log.ssend();
 	calc_co_ords(captions);
/*
 *	OK: now we should have everything we need to create 
 *	the menu
 *
 *	Actually, we should set the font first
	sRGBA text_rgba = cfg.interface.choice_manager.text();
 	g_ChoiceManager.set_font(font_file, font_size, text_rgba);
 */
/*
 *	now we can do the menu
 */
	log.ss() << "creating choice box";
	log.ssend();
	g_ChoiceManager.CreateChoiceBox(x, y, maxw + 20, maxh, 0, captions.size(), maxh, 0, 16);
/*
 *	Now: the choicemanager will call a C function when the
 *	user clicks, so we need to specify that. 
 */
 	g_ChoiceManager.set_callback(c_callback);
/* 
 *	we still need to write the captions on the menu boxes
 */
 	write_captions();
/*
 *	set the menu active, and we're done. For now :)
 */
	g_ChoiceManager.SetActive(0);
}

/*
 * this is the C callback. All it needs do is
 * recover the cLuaMenu instance (easy - it's a singleton)
 * and call the clicked method, passing the option number
 */
static void c_callback(int option_number)
{
	cLuaMenu lmen;
/*
 *	lua arrays count from one - so add one to the parameter
 */
	lmen.clicked(option_number + 1);
}

/*
 * this func needs to do two things.
 * it needs to free up the resources of the menu
 * and it needs to call the lua handler
 */
void cLuaMenuInner::clicked(int option_number)
{
	g_ChoiceManager.Free();
	g_ChoiceManager.set_callback(0);
/*
 *	we need to free up the string pointers used for captions
 */
	for(unsigned int i = 0; i < captions.size(); i++) {
		delete captions[i];
		captions[i] = 0;
	}
	captions.clear();
/*
 *	if the reference number is -1, we can go home
 */
 	if(lua_callback_ref == -1) {
		return;
	}
/*
 *	No? Then we need to de-reference the lua callback method
 *	first we get it out of the registry
 */
	lua_rawgeti(l, LUA_REGISTRYINDEX, lua_callback_ref);
/*
 *	OK - the callback function should now be on top of the stack
 *	ready to be called.
 *
 *	Second thoughts, let's check that...
 */
 	if(lua_type(l, -1) != LUA_TFUNCTION) {
		whoops("cLuaMenuInner::clicked: item in registry is not a function");
		log.ss() << "ERROR: cLuaMenuInner::whoops is not supposed to return!!!";
		log.ssend();
	}
/*
 *	if we get here, we have a function. Before calling it,
 *	we should release the reference 
 */
 	luaL_unref(l, LUA_REGISTRYINDEX, lua_callback_ref);
/*
 *	Almost there: we need to call the function with the option number
 *	as a parameter. So we need to push that onto the stack, first
 */
 	lua_pushinteger(l, option_number);
/*
 *	finally, we can call lua_pcall
 */
	int rc = lua_pcall(
		l,		// Lua state
		1,		// number of args we're passing in
		0,		// number of results we expect returned
		0		// optional error handler 
	);
	if(rc == 0) {
		return;
	}
	CLog log;
	string errstr = lua_tostring(l, -1);
	lua_pop(l, 1);
	log.ss()	<< "Script error in menu callback: "
			<< errstr
	;
	log.ssend();
}

/*

 *

 */
