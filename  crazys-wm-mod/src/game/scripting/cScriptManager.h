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

#ifndef CRAZYS_WM_MOD_CSCRIPTMANAGER_H
#define CRAZYS_WM_MOD_CSCRIPTMANAGER_H

#include <unordered_map>
#include <memory>
#include "fwd.hpp"

class sGirl;

namespace scripting {
    class cLuaScript;
    class sLuaParameter;

    class cScriptManager {
    public:
        cScriptManager();
        ~cScriptManager();

        // running events
        sScriptValue RunEvent(const sEventTarget& event) const;
        sScriptValue RunEvent(const sEventTarget& event, sGirl& girl) const;
        sScriptValue RunEvent(const sEventTarget& event, std::initializer_list<sLuaParameter> params) const;

        // registering
        /// Load the script from `file` and save under the name `name`.
        void LoadScript(std::string name, const std::string& file);

        pEventMapping CreateEventMapping(std::string name, const std::string& fallback) const;
        void RegisterEventMapping(pEventMapping mapping);

        // global event mapping
        const pEventMapping& GetGlobalEventMapping();
        const sEventTarget& GetGlobalEvent(const sEventID& event) const;

        void LoadEventMapping(IEventMapping& ev, tinyxml2::XMLElement* source);
        void LoadEventMapping(IEventMapping& ev, const std::string& source_file);
    private:
        std::unordered_map<std::string, std::unique_ptr<cLuaScript>> m_Scripts;
        std::unordered_map<std::string, pEventMapping> m_EventMappings;

        pEventMapping m_GlobalEventMapping;
    };
}

#endif //CRAZYS_WM_MOD_CSCRIPTMANAGER_H
