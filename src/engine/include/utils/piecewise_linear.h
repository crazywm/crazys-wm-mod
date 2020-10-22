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

#ifndef WM_PIECEWISE_LINEAR_H
#define WM_PIECEWISE_LINEAR_H

#include <vector>

/// A piecewise linear function mapping floats to floats. Specified by a set of vertices. The function continues
/// linearly through the leftmost and rightmost vertices, i.e. the function specified by the points (0, 0) and (1, 1)
/// is `y=x` for all values of x. Discontinuities are supported, in that case the vertex inserted first determines
/// the value on the left of the jump location.
class PiecewiseLinearFunction {
public:
    /// inserts a new vertex into the piecewise linear function. If two vertices are inserted with the same x
    /// coordinate, the function makes a jump and the value to the left is determined by the y value of the vertex
    /// that was inserted first.
    void add_vertex(float x, float y);

    /// evaluates the function at point x
    float operator()(float x) const;
private:
    struct sVertex {
        float x;
        float y;

        bool operator<(const sVertex& other) const;
    };

    std::vector<sVertex> m_Vertices;
};

#endif //WM_PIECEWISE_LINEAR_H
