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

#ifndef CRAZYS_WM_MOD_CEVENTMAPPING_H
#define CRAZYS_WM_MOD_CEVENTMAPPING_H

#include <unordered_map>
#include "fwd.hpp"

namespace scripting
{
    struct EventIDHash {
        std::size_t operator()(const sEventID& foo) const {
            return foo.hash();
        }
    };

    class cScriptManager;

    class cEventMapping final : public IEventMapping
    {
    public:
        explicit cEventMapping(std::string name, const cScriptManager * mgr, pEventMapping fallback);
        ~cEventMapping() final;

        const std::string& GetName() const final;

        void SetEventHandler(sEventID id, std::string script, std::string function) final;
        bool HasEvent(const sEventID& event) const final;
        const sEventTarget& GetEvent(const sEventID& event) const final;

        sScriptValue RunEvent(const sEventID& event) const final;
        sScriptValue RunEvent(const sEventID& event, sGirl& girl) const final;
        sScriptValue RunEvent(const sEventID& event, std::initializer_list<sLuaParameter> params) const final;
    private:
        sScriptValue RunEventWithParams(const sEventID& event, std::initializer_list<sLuaParameter> params) const;
        std::string m_Name;
        const cScriptManager* m_Manager = nullptr;
        pEventMapping  m_Fallback = nullptr;

        std::unordered_map<sEventID, sEventTarget, EventIDHash> m_Events;
        std::unordered_map<sEventID, std::vector<sEventTarget>, EventIDHash> m_Triggers;
    };
}

#endif //CRAZYS_WM_MOD_CEVENTMAPPING_H
