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


#include "doctest.h"
#include "utils/doctest_utils.h"
#include "conditions.h"

TEST_CASE("parse comparison") {
    SUBCASE("parsing comparison condition: >=") {
        auto condition = sCompareCondition::from_string("exp >=50");
                CHECK(condition->Comparison == sCompareCondition::GEQ);
                CHECK(condition->ValueRef == "exp");
                CHECK(condition->ReferenceNumber == 50);
    }

    SUBCASE("parsing comparison condition: >") {
        auto condition = sCompareCondition::from_string("\texp > -23");
                CHECK(condition->Comparison == sCompareCondition::GREATER);
                CHECK(condition->ValueRef == "exp");
                CHECK(condition->ReferenceNumber == -23);
    }

    SUBCASE("parsing comparison condition: ==") {
        std::unique_ptr<sCompareCondition> condition;
                SUBCASE("=") {
            condition = sCompareCondition::from_string("exp = 13");
        }
                SUBCASE("==") {
            condition = sCompareCondition::from_string("exp == 13");
        }
                CHECK(condition->Comparison == sCompareCondition::EQUAL);
                CHECK(condition->ValueRef == "exp");
                CHECK(condition->ReferenceNumber == 13);
    }

    SUBCASE("parsing comparison condition: <") {
        auto condition = sCompareCondition::from_string(" exp < +12 ");
                CHECK(condition->Comparison == sCompareCondition::LESS);
                CHECK(condition->ValueRef == "exp");
                CHECK(condition->ReferenceNumber == 12);
    }

    SUBCASE("parsing comparison condition: <=") {
        auto condition = sCompareCondition::from_string(" exp <= 14254");
                CHECK(condition->Comparison == sCompareCondition::LEQ);
                CHECK(condition->ValueRef == "exp");
                CHECK(condition->ReferenceNumber == 14254);
    }

    SUBCASE("compare condition invalid") {
                CHECK_THROWS(sCompareCondition::from_string("exp>"));
                CHECK_THROWS(sCompareCondition::from_string("<54"));
                CHECK_THROWS(sCompareCondition::from_string("12<exp"));
                CHECK_THROWS(sCompareCondition::from_string("exp<2exp"));
                CHECK_THROWS(sCompareCondition::from_string("exp<1.56"));

                CHECK_THROWS(sCompareCondition::from_string("exp <== 5"));
    }
}

TEST_CASE("evaluate conditions") {
    struct Mock : public IInteractionInterface {
        bool LookupBoolean(const std::string& name) const final {
            if(name == "TRUE") {
                return true;
            } else if (name == "FALSE") {
                return false;
            }
            FAIL("invalid bool lookup");
        }
        int LookupNumber(const std::string& name) const final {
            if(name == "TEN") {
                return 10;
            }
            FAIL("invalid number lookup");
        };

        // actions
        void TriggerEvent(const std::string& name) const final {}
        void SetVariable(const std::string& name, int value) const final {}
    };

    Mock mock;

    SUBCASE("true comparisons") {
        for(auto parse : {"TEN < 15", "TEN <= 10", "TEN = 10", "TEN >= 10", "TEN > 5",
                          "TRUE", "TEN < 5 | TRUE", " TRUE & TEN = 10", "TRUE & TEN = 10 | FALSE"}) {
            CAPTURE(parse);
            auto cc = parse_conditions(parse);
            CHECK(cc->check(mock));
        }
    }

    SUBCASE("false comparisons") {
        for(auto parse : {"TEN > 15", "TEN = 8", "TEN < 5", "FALSE", "TEN < 15 & FALSE", "FALSE | TEN > 15",
                          "FALSE & TRUE | TRUE & FALSE"}) {
            CAPTURE(parse);
            auto cc = parse_conditions(parse);
            CHECK_FALSE(cc->check(mock));
        }
    }
}