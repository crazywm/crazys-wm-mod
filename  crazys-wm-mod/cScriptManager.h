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

#include "CLog.h"
#include "cGameScript.h"
#include "cLuaScript.h"
#include "DirPath.h"
#include <queue>

/*
 * OK: I got into a bit of a mess the last time I tried this,
 * so this time I'm just going to wrap the current script manager
 * in another class
 *
 * let's see if we can get everything going through the wrapper
 * before we add the lua stuff in
 */
class cScriptManagerInner {
	cGameScript		m_Script;
	queue<cLuaScript *>	lq;
public:
	void Load(string filename, sGirl *girl);
	void Release() {
		cLuaScript *lpt;
/*
 *		clear down the Lua script queue
 */
		while(!lq.empty()) {
			lpt = lq.front();
			delete lpt;
			lq.pop();
		}
		m_Script.Release();
	}
	bool IsActive() {
		if(!lq.empty()) {
			return true;
		}
		return m_Script.IsActive();
	}
	void RunScript() {
		cLuaScript *lpt;
/*
 *		if there's nothing in the lua queue
 *		let m_Script do it's thing
 */
		if(lq.empty()) {
			m_Script.RunScript();
			return;
		}
/*
 *		get the front of the lua queue
 */
		lpt = lq.front();
/*
 *		let it run
 */
		bool rc = lpt->run();
/*
 *		if it returned true, leave it there so it can
 *		get another time slice
 */
 		CLog log;
		if(rc) {
			log.ss() << "RunScript: lua returned true: "
				 << "script left on queue"
				 ;
			log.ssend();
			return;
		}
		log.ss() << "RunScript: lua returned false: "
			 << "deleting from queue"
		;
/*
 *		if it returned false, or nothing at all,
 *		remove it from the queue and delete it
 */
		delete lpt;
		lq.pop();
	}
};

class cScriptManager {
static	cScriptManagerInner *instance;
public:
	cScriptManager() {
		if(!instance) instance = new cScriptManagerInner();
	}
	void Load(ScriptPath &dp, sGirl *girl) {
		instance->Load(string(dp.c_str()), girl);
	}
	void Load(string filename, sGirl *girl) {
		instance->Load(filename, girl);
	}
	void Release() {
		instance->Release();
	}
	bool IsActive() {
		return instance->IsActive();
	}
	void RunScript() {
		instance->RunScript();
	}
};
