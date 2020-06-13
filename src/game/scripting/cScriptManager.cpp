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

#include "fwd.hpp"
#include <DirPath.h>
#include "cScriptManager.h"
#include "cEventMapping.h"
#include "cLuaScript.h"
#include <tinyxml2.h>
#include "xml/util.h"
#include "xml/getattr.h"
#include "CLog.h"
#include "sLuaParameter.hpp"


using namespace scripting;


sScriptValue cScriptManager::RunEvent(const sEventTarget& event, sGirl &girl) const {
    return RunEvent(event, {sLuaParameter(girl)});
}

sScriptValue cScriptManager::RunEvent(const sEventTarget& event) const {
    return RunEvent(event, {});
}

sScriptValue cScriptManager::RunEvent(const sEventTarget& event, std::initializer_list<sLuaParameter> params) const
{
    cLuaScript& script = *m_Scripts.at(event.script);
    return script.RunEvent(event.function, params);
}

pEventMapping cScriptManager::CreateEventMapping(std::string name, const std::string& fallback) const
{
    if(name.empty())
        throw std::logic_error("The EventMapping must have a (non-empty) name");

    pEventMapping fb;
    if(!fallback.empty()) {
        fb = m_EventMappings.at(fallback);
    }
    return std::make_unique<cEventMapping>(std::move(name), this, fb);
}

void cScriptManager::LoadScript(std::string name, const std::string& file)
{
    auto ls = std::make_unique<cLuaScript>();
    ls->LoadSource(DirPath() << "Resources" << "Scripts" << "API.lua");
    ls->LoadSource(DirPath() << "Resources" << "Scripts" << file);
    m_Scripts[std::move(name)] = std::move(ls);
}

void cScriptManager::RegisterEventMapping(pEventMapping mapping)
{
    m_EventMappings[mapping->GetName()] = std::move(mapping);
}

const pEventMapping& cScriptManager::GetGlobalEventMapping()
{
    return m_GlobalEventMapping;
}

const sEventTarget& cScriptManager::GetGlobalEvent(const sEventID& event) const
{
    return m_GlobalEventMapping->GetEvent(event);
}

void cScriptManager::LoadEventMapping(IEventMapping& ev, tinyxml2::XMLElement * source)
{
    // First, load all scripts
    for(auto& el : IterateChildElements(*source, "Script")) {
        // Load the script
        /// TODO named scripts
    }

    for(auto& el : IterateChildElements(*source, "Event")) {
        try {
            const char* name   = GetStringAttribute(el, "Name");
            std::string script = GetStringAttribute(el, "Script");
            const char* fn     = GetStringAttribute(el, "Function");
            // Verify integrity of "Script"
            if (m_Scripts.count(script) == 0) {
                // If the file name ends in .lua, this is fine
                if (script.length() > 4 && script.substr(script.size() - 4) == ".lua") {
                    // Load the script by its name
                    LoadScript(script, script);
                } else {
                    g_LogFile.error("lua", "Invalid script file name (", script, ") for event ", name);
                }
            }
            ev.SetEventHandler(name, script, fn);
        } catch (std::runtime_error& error) {
            g_LogFile.log(ELogLevel::ERROR, "Could not load event handler from line ", el.GetLineNum(), ":", error.what());
        }
    }
}

void cScriptManager::LoadEventMapping(IEventMapping& ev, const std::string& source_file)
{
    auto doc = LoadXMLDocument(source_file);
    auto root = doc->FirstChildElement();
    LoadEventMapping(ev, root);
}

cScriptManager::cScriptManager() {
    m_GlobalEventMapping = CreateEventMapping("global", "");
}

cScriptManager::~cScriptManager() = default;

