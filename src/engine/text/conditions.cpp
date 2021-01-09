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

#include "conditions.h"
#include "CLog.h"

#include <algorithm>
#include <cassert>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <utility>

namespace {
    struct GetValueVisitor {
        const IInteractionInterface* Lookup;

        int operator()(int val) const {
            return val;
        }

        int operator()(const std::string& val) const {
            return Lookup->LookupNumber(val);
        }

        using result_type = int;
    };

    sCompareCondition::ValueRef val_from_string(const std::string& source) {
        char* endptr = nullptr;
        int num_candidate = std::strtol(source.c_str(), &endptr, 10);
        if(endptr == source.c_str()) {
            assert(num_candidate == 0);
            return source;
        } else {
            return num_candidate;
        }
    }
}

bool sCompareCondition::check(const IInteractionInterface& lookup) const {

    GetValueVisitor visitor{&lookup};
    int left = Left.apply_visitor(visitor);
    int right = Right.apply_visitor(visitor);

    switch (Comparison) {
        case sCompareCondition::LESS:
            return left < right;
        case sCompareCondition::LEQ:
            return left <= right;
        case sCompareCondition::EQUAL:
            return left == right;
        case sCompareCondition::GEQ:
            return left >= right;
        case sCompareCondition::GREATER:
            return left > right;
    }
    assert(false);
}

std::unique_ptr<sCompareCondition> sCompareCondition::from_string(const std::string& source) {
    auto cmp = std::find_if(begin(source), end(source), [](char c){ return c == '<' || c == '=' || c == '>'; });
    if(cmp == end(source)) {
        g_LogFile.error("text", "Trying to parse invalid condition '", source, "'");
        throw std::runtime_error("invalid action: no comparison operator");
    }

    auto lhs = std::string(begin(source), cmp);
    boost::algorithm::trim(lhs);
    if(lhs.empty()) {
        g_LogFile.error("text", "Trying to parse invalid condition '", source, "'");
        throw std::runtime_error("invalid action: lhs of comparison is empty");
    }

    auto rhs_start = cmp + 1;
    sCompareCondition::ECompare compare;
    if(*cmp == '<') {
        if(*(cmp+1) == '=') {
            compare = sCompareCondition::LEQ;
            ++rhs_start;
        } else {
            compare = sCompareCondition::LESS;
        }
    } else if(*cmp == '>') {
        if(*(cmp+1) == '=') {
            compare = sCompareCondition::GEQ;
            ++rhs_start;
        } else {
            compare = sCompareCondition::GREATER;
            ++rhs_start;
        }
    } else if(*cmp == '=') {
        compare = sCompareCondition::EQUAL;
        // TODO should we require ==?
        if(*rhs_start == '=') {
            ++rhs_start;
        }
    } else {
        assert(false);
    }
    auto rhs = std::string(rhs_start, end(source));
    boost::trim(rhs);
    if(rhs.empty()) {
        g_LogFile.error("text", "Trying to parse invalid condition '", source, "'");
        throw std::runtime_error("invalid action: rhs of condition is empty");
    }

    return std::make_unique<sCompareCondition>(compare, val_from_string(lhs), val_from_string(rhs));
}

sCompareCondition::sCompareCondition(sCompareCondition::ECompare cmp, ValueRef left, ValueRef right) :
    Comparison(cmp), Left(std::move(left)), Right(std::move(right)) {
}

std::unique_ptr<ICondition> sCompareCondition::clone() const {
    return std::make_unique<sCompareCondition>(Comparison, Left, Right);
}

namespace {

    template<class T>
    std::vector<std::unique_ptr<ICondition>> split_and_parse(const std::string& source, char sep, T&& parser) {
        std::vector<std::string> conditions;
        boost::split(conditions, source, [&](const char c) { return c == sep; });
        std::vector<std::unique_ptr<ICondition> > result;
        result.reserve(conditions.size());
        std::for_each(begin(conditions), end(conditions), [](std::string& c) { boost::trim(c); });
        std::transform(begin(conditions), end(conditions), std::back_inserter(result), parser);
        return result;
    }

    std::vector<std::unique_ptr<ICondition>> clone_conditions(const std::vector<std::unique_ptr<ICondition>>& source) {
        std::vector<std::unique_ptr<ICondition>> copy;
        copy.reserve(source.size());
        std::transform(begin(source), end(source), std::back_inserter(copy),
                       [](const std::unique_ptr<ICondition>& c){
                           return c->clone();
                       });
        return copy;
    }

}

bool sConjunctCondition::check(const IInteractionInterface& lookup) const {
    return std::all_of(begin(Conditions), end(Conditions), [&](const auto& c){ return c->check(lookup); });
}

sConjunctCondition::sConjunctCondition(std::vector<std::unique_ptr<ICondition>> conditions) :
    Conditions(std::move(conditions))
{
    assert(std::all_of(begin(Conditions), end(Conditions), [](const auto& c){ return c!= nullptr; }));
}

std::unique_ptr<ICondition> sConjunctCondition::clone() const {
    return std::make_unique<sConjunctCondition>(clone_conditions(Conditions));
}

bool sDisjunctCondition::check(const IInteractionInterface& lookup) const {
    return std::any_of(begin(Conditions), end(Conditions), [&](const auto& c){ return c->check(lookup); });
}

sDisjunctCondition::sDisjunctCondition(std::vector<std::unique_ptr<ICondition>> conditions) :
        Conditions(std::move(conditions)) {
    assert(std::all_of(begin(Conditions), end(Conditions), [](const auto& c){ return c!= nullptr; }));
}

std::unique_ptr<ICondition> sDisjunctCondition::clone() const {
    return std::make_unique<sDisjunctCondition>(clone_conditions(Conditions));
}


bool sBoolCondition::check(const IInteractionInterface& lookup) const {
    return lookup.LookupBoolean(ValueRef);
}

sBoolCondition::sBoolCondition(std::string val) : ValueRef(std::move(val)) {

}

std::unique_ptr<ICondition> sBoolCondition::clone() const {
    return std::make_unique<sBoolCondition>(ValueRef);
}

namespace {

    std::unique_ptr<ICondition> parse_inner(const std::string& content) {
        auto cmp = std::find_if(begin(content), end(content), [&](char c){ return c == '<' || c == '=' || c == '>'; });
        if(cmp == end(content)) {
            return std::make_unique<sBoolCondition>(boost::algorithm::trim_copy(content));
        } else {
            return sCompareCondition::from_string(std::move(content));
        }
    }

    std::unique_ptr<ICondition> parse_and(const std::string& source) {
        auto ands = split_and_parse(source, '^', parse_inner);
        if(ands.size() == 1) {
            return std::move(ands.front());
        }
        return std::make_unique<sConjunctCondition>(std::move(ands));
    }

    std::unique_ptr<ICondition> parse_or(const std::string& source) {
        auto ors = split_and_parse(source, '|', parse_and);
        if(ors.size() == 1) {
            return std::move(ors.front());
        }
        return std::make_unique<sDisjunctCondition>(std::move(ors));
    }
}

std::unique_ptr<ICondition> parse_conditions(const std::string& source) {
    return parse_or(source);
}

#include "doctest.h"

TEST_CASE("inner parse") {
    std::unique_ptr<ICondition> cond;
    cond = parse_inner("x < 12");
    CHECK(typeid(*cond) == typeid(sCompareCondition));

    cond = parse_inner(" boolcond");
    CHECK(typeid(*cond) == typeid(sBoolCondition));
}

TEST_CASE("parse and") {
    std::unique_ptr<ICondition> cond;
    cond = parse_and("x < 12");
    CHECK(typeid(*cond) == typeid(sCompareCondition));

    cond = parse_and("a ^ b");
    CHECK(typeid(*cond) == typeid(sConjunctCondition));
}

TEST_CASE("parse or") {
    std::unique_ptr<ICondition> cond;
    cond = parse_or("x < 12");
    CHECK(typeid(*cond) == typeid(sCompareCondition));

    cond = parse_or("a | x < 5");
    CHECK(typeid(*cond) == typeid(sDisjunctCondition));
}

// TODO check evaluation of conditions!