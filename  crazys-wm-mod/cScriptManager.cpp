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
#include "cLuaScript.h"
#include "cGameScript.h"

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
    void RunScript();
};

cScriptManagerInner *cScriptManager::instance = nullptr;

cScriptManager::cScriptManager()
{
    if(!instance) instance = new cScriptManagerInner();
}

void cScriptManager::Load(ScriptPath& dp, sGirl * girl)
{
    instance->Load(string(dp.c_str()), girl);
}

void cScriptManager::Load(string filename, sGirl * girl)
{
    instance->Load(filename, girl);
}

void cScriptManager::Release()
{
    instance->Release();
}

bool cScriptManager::IsActive()
{
    return instance->IsActive();
}

void cScriptManager::RunScript()
{
    instance->RunScript();
}

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

void cScriptManagerInner::RunScript()
{
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
