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

#ifndef WM_REPO_H
#define WM_REPO_H

#include <string>
#include <memory>

namespace tinyxml2 {
    class XMLElement;
}

//! \brief Interface that defines how to do lookups when testing a condition, and how to trigger events.
class IInteractionInterface {
public:
    virtual ~IInteractionInterface() = default;

    // conditions
    virtual bool LookupBoolean(const std::string& name) const = 0;
    virtual int LookupNumber(const std::string& name) const = 0;

    // actions
    virtual void TriggerEvent(const std::string& name) const = 0;
    virtual void SetVariable(const std::string& name, int value) const = 0;
    virtual void SetVariable(const std::string& name, std::string value) const = 0;
};

///! \brief Base class for all actions that may be triggered.
class IAction {
public:
    virtual ~IAction() = default;
    virtual void apply(const IInteractionInterface& target) const = 0;
};

///! \brief Base class for all conditions that may be checked.
class ICondition {
public:
    virtual ~ICondition() = default;
    virtual bool check(const IInteractionInterface& target) const = 0;
    virtual std::unique_ptr<ICondition> clone() const = 0;
};

//! Allows to look up named text templates that may be filtered by conditions.
class ITextRepository {
public:
    virtual ~ITextRepository() = default;
    virtual void load(const tinyxml2::XMLElement& root) = 0;

    // builder interface
    virtual void add_text(const std::string& prompt, const std::string& tmplate, int Pri, int Chance,
                          std::unique_ptr<ICondition> condition, std::unique_ptr<IAction> action) = 0;

    // lookup interface
    /*! Looks up a text identified by `prompt`. The `lookup` object is passed
     * through to the any evaluation of a condition.
     * \param prompt An identifier for the text to be looked up.
     * \param lookup Generic argument to be supplied to conditions and actions.
     * \return A reference to the text with the given name. If `promp` has registered texts, but none of
     * them fulfill their conditions, an empty string is returned.
     */
    virtual const std::string& get_text(const std::string& prompt, const IInteractionInterface& lookup) = 0;

    /// Checks whether an entry for the given prompt exists.
    virtual bool has_text(const std::string& prompt) = 0;

    /// Checks that all provided text options do have non-empty text
    virtual bool verify() const = 0;

    /// Creates a text repository. This will return a unique_ptr to a concrete implementation of a text
    /// repo as supplied by the engine.
    static std::unique_ptr<ITextRepository> create();
};

#endif //WM_REPO_H
