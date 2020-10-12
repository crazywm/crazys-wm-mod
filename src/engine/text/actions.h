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

#ifndef WM_ACTIONS_H
#define WM_ACTIONS_H

#include "text/repo.h"
#include <vector>

struct sAssignmentAction : public IAction {
    enum EAssign {
        SET, ADD, SUB
    } Mode;
    std::string Target;
    int Value;

    sAssignmentAction(EAssign mode, std::string tgt, int value);
    void apply(const IInteractionInterface& target) const override;

    static std::unique_ptr<sAssignmentAction> from_string(const std::string& source);
};

struct sSequenceAction : public IAction {
    explicit sSequenceAction(std::vector<std::unique_ptr<IAction>> a) : Actions(std::move(a)) {}
    void apply(const IInteractionInterface& target) const override;

    std::vector<std::unique_ptr<IAction>> Actions;
};

#endif //WM_ACTIONS_H
