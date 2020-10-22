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
#include "utils/piecewise_linear.h"
#include <algorithm>

void PiecewiseLinearFunction::add_vertex(float x, float y) {
    auto lb = std::upper_bound(begin(m_Vertices), end(m_Vertices), sVertex{x, 0.f});
    m_Vertices.insert(lb, {x, y});
}

float PiecewiseLinearFunction::operator()(float x) const {
    if(m_Vertices.empty()) {
        return 0.f;
    } else if (m_Vertices.size() == 1) {
        return m_Vertices.front().y;
    }

    auto lb = std::lower_bound(begin(m_Vertices), end(m_Vertices), sVertex{x, 0.f});
    if(lb == end(m_Vertices)) {
        // extrapolate right
        float dx = (lb-2)->x - (lb-1)->x;
        float dy = (lb-2)->y - (lb-1)->y;
        if(dx == 0.f) {
            return (lb-1)->y;
        }
        float slope = dy / dx;
        return (lb-1)->y - ((lb-1)->x - x) * slope;
    } else if(lb->x == x) {
        return lb->y;
    } else if(lb == begin(m_Vertices)) {
        // extrapolate left
        float dx = (lb+1)->x - lb->x;
        float dy = (lb+1)->y - lb->y;
        if(dx == 0.f) {
            return lb->y;
        }
        float slope = dy / dx;
        return lb->y - (lb->x - x) * slope;
    } else {
        float dx = lb->x - (lb-1)->x;
        float dy = lb->y - (lb-1)->y;
        float slope = dy / dx;
        return lb->y - (lb->x - x) * slope;
    }
}
bool PiecewiseLinearFunction::sVertex::operator<(const PiecewiseLinearFunction::sVertex& other) const {
    return x < other.x;
}

#include "doctest.h"

TEST_CASE("piecewise linear regular") {
    PiecewiseLinearFunction f;
    CHECK(f(1.f) == 0.f);

    f.add_vertex(1.f, 2.f);
    CHECK(f(3.f) == 2.f);

    // insert at the end
    f.add_vertex(2.f, 3.f);
    CHECK(f(0.f) == 1.f);
    CHECK(f(1.f) == 2.f);
    CHECK(f(1.5f) == 2.5f);
    CHECK(f(2.f) == 3.f);
    CHECK(f(3.f) == 4.f);

    // insert at the beginning
    f.add_vertex(0.f, 0.f);
    CHECK(f(-1.f) == -2.f);
    CHECK(f(0.f) == 0.f);
    CHECK(f(1.f) == 2.f);
    CHECK(f(1.5) == 2.5f);
    CHECK(f(2.f) == 3.f);
    CHECK(f(3.f) == 4.f);
}

TEST_CASE("piecewise linear with duplicate") {
    PiecewiseLinearFunction f;
    f.add_vertex(1.f, 1.f);
    f.add_vertex(1.f, 1.f);
    
    CHECK(f(-1.f) == 1.f);
    CHECK(f(1.f) == 1.f);
    CHECK(f(2.f) == 1.f);

    f.add_vertex(2.f, 2.f);
    CHECK(f(1.5f) == 1.5f);
}

TEST_CASE("piecewise linear with jumps") {
    // if we have jumps, the order of insertion matters
    PiecewiseLinearFunction fa;
    fa.add_vertex(1.f, 1.f);
    fa.add_vertex(1.f, 2.f);
    CHECK(fa(-1.f) == 1.f);
    CHECK(fa(2.f) == 2.f);

    fa.add_vertex(0.f, 0.f);
    CHECK(fa(0.5f) == .5f);

    PiecewiseLinearFunction fb;
    fb.add_vertex(1.f, 2.f);
    fb.add_vertex(1.f, 1.f);
    CHECK(fb(-1.f) == 2.f);
    CHECK(fb(2.f) == 1.f);

    fb.add_vertex(0.f, 0.f);
    CHECK(fb(0.5f) == 1.f);
}