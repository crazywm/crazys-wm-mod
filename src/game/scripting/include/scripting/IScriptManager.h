/*
 * Copyright 2022 The Pink Petal Development Team.
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

#ifndef WM_ISCRIPTMANAGER_H
#define WM_ISCRIPTMANAGER_H

#include "scripting.h"

namespace scripting
{
    class cLuaScript;
    class sLuaParameter;

    class IScriptManager {
    public:
        using LuaParamsList = std::initializer_list<sLuaParameter>;

        virtual ~IScriptManager() = default;

        /// Runs an event in async mode.
        virtual sAsyncScriptHandle RunEventAsync(const sEventTarget& event, LuaParamsList params) const = 0;

        /// Runs an event in synchronous mode
        virtual sScriptValue RunEventSync(const sEventTarget& event, LuaParamsList params) const = 0;

        // registering
        /// Load the script from `file` and save under the name `name`.
        virtual void LoadScript(std::string name, const std::string& file) = 0;

        /// Ensures that `script` is loaded and checks that `function` exists. Generates error messages if
        /// that is not the case, and also returns whether the script was valid.
        virtual bool VerifyScript(const std::string& script, const std::string& function) = 0;

        static std::unique_ptr<IScriptManager> createScriptManager();

        virtual pEventMapping CreateEventMapping(std::string name, const std::string& fallback) = 0;
        virtual void RegisterEventMapping(pEventMapping mapping) = 0;

        // global event mapping
        virtual const pEventMapping& GetGlobalEventMapping() = 0;
        virtual const sEventTarget& GetGlobalEvent(const sEventID& event) const = 0;

        virtual void LoadEventMapping(IEventMapping& ev, const tinyxml2::XMLElement& source) = 0;
        virtual void LoadEventMapping(IEventMapping& ev, const std::string& source_file) = 0;
    };
}

#endif //WM_ISCRIPTMANAGER_H
