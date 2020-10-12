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

#include "actions.h"
#include "CLog.h"
#include <cassert>
#include <algorithm>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

void sAssignmentAction::apply(const IInteractionInterface& lookup) const {
    int value = lookup.LookupNumber(Target);
    switch (Mode) {
        case SET:
            value = Value;
            break;
        case ADD:
            value += Value;
            break;
        case SUB:
            value -= Value;
            break;
        default:
            assert(false);
    }
    lookup.SetVariable(Target, value);
}

namespace {
    sAssignmentAction::EAssign read_mode(char c) {
        switch (c) {
            case '+':
                return sAssignmentAction::ADD;
            case '-':
                return sAssignmentAction::SUB;
            default:
                return sAssignmentAction::SET;
        }
    }
}

std::unique_ptr<sAssignmentAction> sAssignmentAction::from_string(const std::string& content) {
    auto cmp = std::find_if(begin(content), end(content), [](char c){ return c == '='; });
    if(cmp == end(content)) {
        g_LogFile.error("text", "Trying to parse invalid assignment action '", content, "'");
        throw std::runtime_error("invalid assignment: missing '='");
    }
    if(cmp == begin(content)) {
        g_LogFile.error("text", "Trying to parse invalid assignment action '", content, "'");
        throw std::runtime_error("invalid assignment: starts with '='");
    }

    // at this point, we can safely decrement cmp.
    // assignment type action
    sAssignmentAction::EAssign mode = read_mode(*(cmp-1));
    auto left = cmp;
    if(mode != sAssignmentAction::SET)
        --left;

    auto lhs = std::string(begin(content), left);
    boost::trim(lhs);
    if(lhs.empty()) {
        g_LogFile.error("text", "Trying to parse invalid action '", content, "'");
        throw std::runtime_error("invalid action: lhs of assignment is empty");
    }

    auto rhs = std::string(cmp + 1, end(content));
    boost::trim(rhs);
    if(rhs.empty()) {
        g_LogFile.error("text", "Trying to parse invalid action '", content, "'");
        throw std::runtime_error("invalid action: rhs of assignment is empty");
    }
    int rhs_val = boost::lexical_cast<int>(rhs.data());

    return std::make_unique<sAssignmentAction>(mode, std::move(lhs), rhs_val);
}

sAssignmentAction::sAssignmentAction(sAssignmentAction::EAssign mode, std::string tgt, int value) :
    Mode(mode), Target(std::move(tgt)), Value(value){

}

void sSequenceAction::apply(const IInteractionInterface& target) const {
    for(auto& action : Actions) {
        action->apply(target);
    }
}

#include "doctest.h"

TEST_CASE("parse assignment validation") {
    CHECK_THROWS(sAssignmentAction::from_string("=5"));
    CHECK_THROWS(sAssignmentAction::from_string("20-45"));
    CHECK_THROWS(sAssignmentAction::from_string("a=b"));    // currently not supported, maybe later
    CHECK_THROWS(sAssignmentAction::from_string("a*=2"));   // currently not supported, maybe later
    CHECK_THROWS(sAssignmentAction::from_string("b/=2"));   // currently not supported, maybe later
    CHECK_THROWS(sAssignmentAction::from_string("b=c=2"));
    CHECK_THROWS(sAssignmentAction::from_string("a?b=2"));  // invalid identifier
    CHECK_THROWS(sAssignmentAction::from_string("a=\t"));
    CHECK_THROWS(sAssignmentAction::from_string(" =5"));
}

TEST_CASE("parse assignment") {
    std::unique_ptr<sAssignmentAction> assign;
    SUBCASE("set") {
        assign = sAssignmentAction::from_string("a=5");
        CHECK(assign->Mode == sAssignmentAction::SET);
    }
    SUBCASE("add") {
        assign = sAssignmentAction::from_string("a += 5");
        CHECK(assign->Mode == sAssignmentAction::ADD);
    }
    SUBCASE("sub") {
        assign = sAssignmentAction::from_string("\ta -= 5 ");
        CHECK(assign->Mode == sAssignmentAction::SUB);
    }
    CHECK(assign->Value == 5);
    CHECK(assign->Target == "a");
}

TEST_CASE("apply assignment") {
    struct Mock : public IInteractionInterface {
        bool LookupBoolean(const std::string& name) const final { return false; };
        int LookupNumber(const std::string& name) const final {
            if(name == "a") {
                return a;
            }
            FAIL("invalid number lookup");
        };

        // actions
        void TriggerEvent(const std::string& name) const final {}
        void SetVariable(const std::string& name, int value) const final {
            if(name != "a") {
                FAIL("wrong name");
            }
            const_cast<int&>(a) = value;
        }

        int a = 0;
    };

    Mock mock;

    sAssignmentAction::from_string("a = 5")->apply(mock);
    CHECK(mock.a == 5);

    sAssignmentAction::from_string("a += 5")->apply(mock);
    CHECK(mock.a == 10);

    sAssignmentAction::from_string("a -= 8")->apply(mock);
    CHECK(mock.a == 2);
}