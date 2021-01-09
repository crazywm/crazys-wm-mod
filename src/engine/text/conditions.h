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

#ifndef WM_CONDITIONS_H
#define WM_CONDITIONS_H

#include "text/repo.h"
#include <vector>
#include <boost/variant.hpp>

/// A condition that is the logical AND of its sub-conditions
struct sConjunctCondition : public ICondition {
    explicit sConjunctCondition(std::vector<std::unique_ptr<ICondition>> conditions);
    bool check(const IInteractionInterface& lookup) const override;
    std::unique_ptr<ICondition> clone() const override;

    std::vector<std::unique_ptr<ICondition>> Conditions;
};

/// A condition that is the logical OR of its sub-conditions
struct sDisjunctCondition : public ICondition {
    explicit sDisjunctCondition(std::vector<std::unique_ptr<ICondition>> conditions);

    bool check(const IInteractionInterface& lookup) const override;
    std::unique_ptr<ICondition> clone() const override;

    std::vector<std::unique_ptr<ICondition>> Conditions;
};

/// A condition that compares the value of a variable to a constant
struct sCompareCondition : public ICondition {
    bool check(const IInteractionInterface& lookup) const override;
    std::unique_ptr<ICondition> clone() const override;

    enum ECompare {
        LESS,
        LEQ,
        EQUAL,
        GEQ,
        GREATER
    } Comparison;

    using ValueRef = boost::variant<int, std::string>;
    ValueRef Left;
    ValueRef Right;

    sCompareCondition(ECompare cmp, ValueRef left, ValueRef right);

    static std::unique_ptr<sCompareCondition> from_string(const std::string& source);
};

/// A condition that looks up a boolean from the game
struct sBoolCondition : public ICondition {
    explicit sBoolCondition(std::string str);

    bool check(const IInteractionInterface& lookup) const override;
    std::unique_ptr<ICondition> clone() const override;

    std::string ValueRef;
};

std::unique_ptr<ICondition> parse_conditions(const std::string& source);


#endif //WM_CONDITIONS_H
