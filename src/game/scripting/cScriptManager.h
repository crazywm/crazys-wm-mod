/*
 * Copyright 2019-2022, The Pink Petal Development Team.
 * The Pink Petal Development Team are defined as the game's coders
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

        /// Runs an event in async mode.
        sAsyncScriptHandle RunEventAsync(const sEventTarget& event, std::initializer_list<sLuaParameter> params) const;

        /// Runs an event in synchronous mode
        sScriptValue RunEventSync(const sEventTarget& event, std::initializer_list<sLuaParameter> params) const;

        // registering
        /// Load the script from `file` and save under the name `name`.
        void LoadScript(std::string name, const std::string& file);

        /// Ensures that `script` is loaded and checks that `function` exists. Generates error messages if
        /// that is not the case, and also returns whether the script was valid.
        bool VerifyScript(const std::string& script, const std::string& function);

        pEventMapping CreateEventMapping(std::string name, const std::string& fallback);
        void RegisterEventMapping(pEventMapping mapping);

        // global event mapping
        const pEventMapping& GetGlobalEventMapping();
        const sEventTarget& GetGlobalEvent(const sEventID& event) const;

        void LoadEventMapping(IEventMapping& ev, const tinyxml2::XMLElement& source);
        void LoadEventMapping(IEventMapping& ev, const std::string& source_file);
    private:
        std::unordered_map<std::string, std::unique_ptr<cLuaScript>> m_Scripts;
        std::unordered_map<std::string, pEventMapping> m_EventMappings;

        pEventMapping m_GlobalEventMapping;
    };
}

#endif //CRAZYS_WM_MOD_CSCRIPTMANAGER_H
