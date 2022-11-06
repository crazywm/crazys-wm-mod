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

#include "cEventMapping.h"
#include "utils/string.hpp"
#include "scripting.h"
#include "GameEvents.h"
#include "cScriptManager.h"
#include <tinyxml2.h>
#include "CLog.h"
#include "sLuaParameter.h"
#include "utils/string.hpp"
#include "xml/util.h"


using namespace scripting;

bool sEventID::operator==(const scripting::sEventID& other) const
{
    return iequals(name, other.name);
}

std::size_t sEventID::hash() const
{
    return case_insensitive_hash(name);
}

sEventID::sEventID(const char* n) : name(n)
{
}

sEventID::sEventID(std::string n) : name(std::move(n))
{
}

std::string GetEventID(EDefaultEvent e) {
    switch(e) {
    case EDefaultEvent::CUSTOMER_GAMBLING_CHEAT:
        return "customer:gambling-cheat";
    case EDefaultEvent::CUSTOMER_NOPAY:
        return "customer:no-pay";
    case EDefaultEvent::NEXT_WEEK:
        return "next-week.*";
    case EDefaultEvent::RIVAL_LOST:
        return "rival-lost";
    case EDefaultEvent::GIRL_LOST_OWN_GAMBLING:
        return "girl:gambling-lost.own";
    case EDefaultEvent::GIRL_LOST_RIVAL_GAMBLING:
        return "girl:gambling-lost.rival";
    case EDefaultEvent::GIRL_SEX_ANAL:
        return "girl:sex:anal";
    case EDefaultEvent::GIRL_SEX_GROUP:
        return "girl:sex:group";
    case EDefaultEvent::GIRL_SEX_HAND:
        return "girl:sex:hand";
    case EDefaultEvent::GIRL_SEX_BDSM:
        return "girl:sex:bdsm";
    case EDefaultEvent::GIRL_SEX_NORMAL:
        return "girl:sex:normal";
    case EDefaultEvent::GIRL_SEX_ORAL:
        return "girl:sex:oral";
    case EDefaultEvent::GIRL_SEX_FOOT:
        return "girl:sex:foot";
    case EDefaultEvent::GIRL_SEX_BEAST:
        return "girl:sex:beast";
    case EDefaultEvent::GIRL_SEX_LESBIAN:
        return "girl:sex:lesbian";
    case EDefaultEvent::GIRL_SEX_STRIP:
        return "girl:sex:strip";
    case EDefaultEvent::GIRL_SEX_TITTY:
        return "girl:sex:titty";
    case EDefaultEvent::GIRL_INTERACT_BROTHEL:
        return "girl:interact:brothel";
    case EDefaultEvent::GIRL_INTERACT_DUNGEON:
        return "girl:interact:dungeon";
    case EDefaultEvent::MEET_GIRL_ARENA:
        return "girl:meet:arena";
    case EDefaultEvent::MEET_GIRL_TOWN:
        return "girl:meet:town";
    case EDefaultEvent::MEET_GIRL_CLINIC:
        return "girl:meet:clinic";
    case EDefaultEvent::MEET_GIRL_STUDIO:
        return "girl:meet:studio";
    }
    throw std::logic_error("GetEventID - Unknown event: " + std::to_string((int) e));
}

sEventID::sEventID(EDefaultEvent event) : name(GetEventID(event))
{

}

bool cEventMapping::HasEvent(const sEventID& event) const
{
    return m_Events.count(event);
}

const sEventTarget& cEventMapping::GetEvent(const sEventID& event) const
{
    if(!event.is_specific()) {
        throw std::logic_error("Cannot get non-specific event " + event.name);
    }

    auto found = m_Events.find(event);
    if(found != m_Events.end()) {
        return found->second;
    }

    // try fallback
    if(m_Fallback) {
        try {
            return m_Fallback->GetEvent(event);
        } catch (std::runtime_error& re) {
            throw std::runtime_error("Could not find event '" + event.name + "' in EventMapping '" + m_Name +
                                      "', and failed to find it in fallback '" + m_Fallback->GetName() + "'");
        }
    }

    throw std::runtime_error("Could not find event '" + event.name + "' in EventMapping '" + m_Name + "', and no fallback was specified.");
}

cEventMapping::cEventMapping(std::string name, cScriptManager* smgr, pEventMapping fallback) :
    m_Name(std::move(name)), m_Manager(smgr), m_Fallback(std::move(fallback)) {

}

const std::string& cEventMapping::GetName() const
{
    return m_Name;
}

sAsyncScriptHandle cEventMapping::RunAsync(const sEventID& event, std::initializer_list<sLuaParameter> params) const {
    return RunAsyncWithParams(event, params);
}

sScriptValue cEventMapping::RunSynchronous(const sEventID& event, std::initializer_list<sLuaParameter> params) const {
    return RunSyncWithParams(event, params);
}

sAsyncScriptHandle cEventMapping::RunAsyncWithParams(const sEventID& event, std::initializer_list<sLuaParameter> params) const
{
    if(!m_Manager) {
        throw std::logic_error("Trying to run event but no script manager has been set: " + m_Name);
    }

    if(event.is_specific()) {
        return m_Manager->RunEventAsync(GetEvent(event), params);
    } else {
        auto split_point = event.name.rfind('.');
        auto handlers = m_Triggers.at(event.name.substr(0, split_point));
        for(auto& handler : handlers) {
            m_Manager->RunEventAsync(handler, params);
        }
    }
    g_LogFile.warning("scripting", "Could not find event ", event.name, "to run!");
    return nullptr;
}

sScriptValue
cEventMapping::RunSyncWithParams(const sEventID& event, std::initializer_list<sLuaParameter> params) const {
    if(!m_Manager) {
        throw std::logic_error("Trying to run event but no script manager has been set: " + m_Name);
    }

    if(event.is_specific()) {
        return m_Manager->RunEventSync(GetEvent(event), params);
    } else {
        auto split_point = event.name.rfind('.');
        auto handlers = m_Triggers.at(event.name.substr(0, split_point));
        for(auto& handler : handlers) {
            m_Manager->RunEventSync(handler, params);
        }
    }
    g_LogFile.warning("scripting", "Could not find event ", event.name, "to run!");
    return boost::blank{};
}

void cEventMapping::SetEventHandler(sEventID id, std::string script, std::string function)
{
    if(!m_Manager) {
        throw std::logic_error("Trying to set event handler but no script manager has been set: " + m_Name);
    }
    // ensure that script is valid
    if(!m_Manager->VerifyScript(script, function)) {
        throw std::logic_error("Cannot set event handler for" + id.name + ", because the script " + function + "@" + script + "is not valid.");
    }

    // check that the id is a valid specific id
    if(!id.is_specific())
        throw std::logic_error("Cannot set event handler for generic ID " + id.name);

    // is it partitioned
    auto split_point = id.name.rfind('.');
    if(split_point == std::string::npos) {
        // single name, just add it
        m_Events[id] = sEventTarget{std::move(script), std::move(function)};
    } else {
        // add the specific event
        m_Events[id] = sEventTarget{script, function};
        // and the generic trigger
        m_Triggers[id.name.substr(0, split_point)].push_back( sEventTarget{std::move(script), std::move(function)} );
    }
}

void cEventMapping::SaveToXML(tinyxml2::XMLElement& target) const {
    for(const auto& event: m_Events) {
        auto& el = PushNewElement(target, "Event");
        el.SetAttribute("Name", event.first.name.c_str());
        el.SetAttribute("Script", event.second.script.c_str());
        el.SetAttribute("Function", event.second.function.c_str());
    }
}

cEventMapping::~cEventMapping() = default;
