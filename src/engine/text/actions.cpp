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

struct cIntAssignmentAction : public IAssignmentAction {
    EAssign Mode;
    int Value;

    cIntAssignmentAction(EAssign mode, std::string tgt, int value);
    void apply(const IInteractionInterface& lookup) const override;
};

struct cStringAssignmentAction : public IAssignmentAction {
    std::string Value;

    cStringAssignmentAction(std::string tgt, std::string value);
    void apply(const IInteractionInterface& lookup) const override;
};

namespace {
    cIntAssignmentAction::EAssign read_mode(char c) {
        switch (c) {
            case '+':
                return cIntAssignmentAction::ADD;
            case '-':
                return cIntAssignmentAction::SUB;
            case '*':
            case '/':
                throw std::runtime_error("Multiplication currently not supported");
            default:
                return cIntAssignmentAction::SET;
        }
    }
}

std::unique_ptr<IAssignmentAction> IAssignmentAction::from_string(const std::string& content) {
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
    cIntAssignmentAction::EAssign mode = read_mode(*(cmp-1));
    auto left = cmp;
    if(mode != cIntAssignmentAction::SET)
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

    if(rhs.front() == '\'') {
        if(mode != SET) {
            g_LogFile.error("text", "Trying to parse invalid string assignment action. Strings only support direct assignment. Input was '", content, "'");
            throw std::runtime_error("String type variables only support assignment using '='");
        }
        auto end_str = rhs.find('\'', 1);
        if(end_str == std::string::npos) {
            throw std::runtime_error("Could not find matching ' in string assignment");
        }
        return std::make_unique<cStringAssignmentAction>(std::move(lhs), rhs.substr(1, end_str - 1));
    } else {
        int rhs_val = boost::lexical_cast<int>(rhs.data());
        return std::make_unique<cIntAssignmentAction>(mode, std::move(lhs), rhs_val);
    }
}

cIntAssignmentAction::cIntAssignmentAction(cIntAssignmentAction::EAssign mode, std::string tgt, int value) :
    IAssignmentAction(std::move(tgt)),
    Mode(mode), Value(value) {

}

cStringAssignmentAction::cStringAssignmentAction(std::string tgt, std::string value) :
        IAssignmentAction(std::move(tgt)), Value(std::move(value)) {

}

void cStringAssignmentAction::apply(const IInteractionInterface& lookup) const {
    lookup.SetVariable(Target, Value);
}

void cIntAssignmentAction::apply(const IInteractionInterface& lookup) const {
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


void sSequenceAction::apply(const IInteractionInterface& target) const {
    for(auto& action : Actions) {
        action->apply(target);
    }
}


#include "doctest.h"

TEST_CASE("parse assignment validation") {
    CHECK_THROWS(IAssignmentAction::from_string("=5"));
    CHECK_THROWS(IAssignmentAction::from_string("20-45"));
    CHECK_THROWS(IAssignmentAction::from_string("a=b"));    // currently not supported, maybe later
    CHECK_THROWS(IAssignmentAction::from_string("a*=2"));   // currently not supported, maybe later
    CHECK_THROWS(IAssignmentAction::from_string("b/=2"));   // currently not supported, maybe later
    CHECK_THROWS(IAssignmentAction::from_string("b=c=2"));
    CHECK_THROWS(IAssignmentAction::from_string("a?b=2"));  // invalid identifier
    CHECK_THROWS(IAssignmentAction::from_string("a=\t"));
    CHECK_THROWS(IAssignmentAction::from_string(" =5"));

    CHECK_THROWS(IAssignmentAction::from_string("a += 'test'"));
    CHECK_THROWS(IAssignmentAction::from_string("a -= 'test'"));
    CHECK_THROWS(IAssignmentAction::from_string("a = 'test"));
}

TEST_CASE("parse assignment to int") {
    std::unique_ptr<cIntAssignmentAction> assign;
    auto create = [](const char* source) {
        return std::unique_ptr<cIntAssignmentAction>{dynamic_cast<cIntAssignmentAction*>(IAssignmentAction::from_string(source).release())};
    };
    SUBCASE("set") {
        assign = create("a=5");
        CHECK(assign->Mode == cIntAssignmentAction::SET);
    }
    SUBCASE("add") {
        assign = create("a += 5");
        CHECK(assign->Mode == cIntAssignmentAction::ADD);
    }
    SUBCASE("sub") {
        assign = create("\ta -= 5 ");
        CHECK(assign->Mode == cIntAssignmentAction::SUB);
    }
    CHECK(assign->Value == 5);
    CHECK(assign->getTarget() == "a");
}

TEST_CASE("parse assignment to string") {
    std::unique_ptr<cStringAssignmentAction> assign;
    auto create = [](const char* source) {
        return std::unique_ptr<cStringAssignmentAction>{dynamic_cast<cStringAssignmentAction*>(IAssignmentAction::from_string(source).release())};
    };
    assign = create(" a =\t'test' ");
    CHECK(assign->Value == "test");
    CHECK(assign->getTarget() == "a");
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

        void SetVariable(const std::string& name, std::string value) const final {
            if(name != "s") {
                FAIL("wrong name");
            }
            const_cast<std::string&>(b) = value;
        }

        int a = 0;
        std::string b;
    };

    Mock mock;

    cIntAssignmentAction::from_string("a = 5")->apply(mock);
    CHECK(mock.a == 5);

    cIntAssignmentAction::from_string("a += 5")->apply(mock);
    CHECK(mock.a == 10);

    cIntAssignmentAction::from_string("a -= 8")->apply(mock);
    CHECK(mock.a == 2);

    cIntAssignmentAction::from_string("s = 'test'")->apply(mock);
    CHECK(mock.b == "test");
}
