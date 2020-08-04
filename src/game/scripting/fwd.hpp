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

#ifndef CRAZYS_WM_MOD_SCRIPTING_FWD_HPP
#define CRAZYS_WM_MOD_SCRIPTING_FWD_HPP

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <boost/variant/variant_fwd.hpp>

enum class EDefaultEvent : int;
class sGirl;
namespace tinyxml2
{
    class XMLElement;
}

namespace scripting {
    class sLuaParameter;
    using sScriptValue = boost::variant<boost::blank, bool, float, std::string>;

    /// Specifies what to execute for a given event: The name of the target script (lua file) and the function's name.
    struct sEventTarget {
        std::string script;
        std::string function;
    };

    /// An identifier for an event.
    struct sEventID {
        sEventID(const char* n);
        sEventID(std::string n);
        sEventID(EDefaultEvent event);

        std::string name;

        bool operator==(const sEventID& other) const;
        std::size_t hash() const;

        bool is_specific() const { return name.back() != '*'; }
    };

    /// This is returned from an async script run
    class IAsyncScriptHandle {
    public:
        virtual ~IAsyncScriptHandle() = default;
        virtual bool is_finished() const = 0;
        virtual const sScriptValue& result() const = 0;

        /// sets a callback that is triggered when the thread finishes
        virtual void SetDoneCallback(std::function<void(const sScriptValue& val)> callback) = 0;
    };

    using sAsyncScriptHandle = std::shared_ptr<IAsyncScriptHandle>;

    /*!
     * \brief Maps event names to event targets.
     * \details This class is responsible for determining which scripts to execute when a certain event or trigger is
     * run. There are three different kinds of events, those for which exactly one script is executed, those for which
     * one out of a set of equivalent scripts is executed and those for which a set of scripts have to be executed.
     *
     * There is also the possibility to set a fallback mapping, i.e. when defining the event mapping for a specific
     * girl, the default event mapping would be set as the fallback.
     */
    class IEventMapping {
        template<class... T>
        struct _id {
            using type = sScriptValue;
        };
    public:
        using shared_ptr_t = std::shared_ptr<IEventMapping>;

        virtual ~IEventMapping() = default;

        virtual const std::string& GetName() const = 0;

        /// Sets the handler for the given event. If a previous one exists, it is overwritten.
        virtual void SetEventHandler(sEventID id, std::string script, std::string function) = 0;

        /// Returns whether this `IEventMapping` has an entry for the specified event.
        virtual bool HasEvent(const sEventID& event) const = 0;

        /// Returns the specific event
        virtual const sEventTarget& GetEvent(const sEventID& event) const = 0;

        // Run functions
        virtual sAsyncScriptHandle RunAsync(const sEventID& event, std::initializer_list<sLuaParameter> params) const = 0;

        template<class... T>
        sAsyncScriptHandle RunAsync(const sEventID& event, T&&... params) {
            return RunAsync(event, {sLuaParameter(std::forward<T>(params))...});
        };

        virtual sScriptValue RunSynchronous(const sEventID& event, std::initializer_list<sLuaParameter> params) const = 0;

        template<class... T>
        auto RunSynchronous(const sEventID& event, T&&... params) -> typename _id<T...>::type {
            return RunSynchronous(event, {sLuaParameter(std::forward<T>(params))...});
        };
    };

    using pEventMapping = std::shared_ptr<IEventMapping>;

    class cScriptManager;
}

#endif //CRAZYS_WM_MOD_SCRIPTING_FWD_HPP
