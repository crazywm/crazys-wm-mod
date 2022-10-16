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

class IAssignmentAction : public IAction {
public:
    enum EAssign {
        SET, ADD, SUB
    };

    explicit IAssignmentAction(std::string tgt) : Target(std::move(tgt)) {}

    const std::string& getTarget() const { return Target; }

    static std::unique_ptr<IAssignmentAction> from_string(const std::string& source);
protected:
    std::string Target;
};


struct sSequenceAction : public IAction {
    explicit sSequenceAction(std::vector<std::unique_ptr<IAction>> a) : Actions(std::move(a)) {}
    void apply(const IInteractionInterface& target) const override;

    std::vector<std::unique_ptr<IAction>> Actions;
};

#endif //WM_ACTIONS_H
