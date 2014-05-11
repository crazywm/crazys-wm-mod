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
#include "cScriptManager.h"

cScriptManagerInner *cScriptManager::instance = 0;

static bool ends_with(string s, string suff)
{
	int s_siz = s.size();
	int suff_siz = suff.size();
	if(suff_siz > s_siz) {
		return false;
	}
	string end = s.substr(s_siz - suff_siz, suff_siz);
	return end == suff;
}


void cScriptManagerInner::Load(string filename, sGirl *girl)
{
/*
 *	so - if it doesn't end with ".lua" we assume it's a GameScript
 */
	if(!ends_with(filename, ".lua")) {
		m_Script.Load(filename);
		m_Script.Prepare(girl);
		return;
	}
/*
 *	So we're in LuaLand. 
 */
 	cLuaScript *lpt = new cLuaScript();
	lpt->load(filename, girl);

	lq.push(lpt);
}
