/*
* Copyright 2009, 2010, The Pink Petal Development Team.
* The Pink Petal Devloment Team are defined as the game's coders
* who meet on http://pinkpetal.org     // old site: http://pinkpetal .co.cc
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
#pragma once

#include "cGameWindow.h"

class cScreenTransfer : public cGameWindow
{
private:
    int    labelleft_id;
    int    labelright_id;
    int    brothelleft_id;
    int    listleft_id;
    int    listright_id;
    int    brothelright_id;
    int    shiftright_id;
    int    shiftleft_id;
    int    back_id;

    void set_ids() override;
public:
    cScreenTransfer();

    void init(bool back) override;
    void process() override { };
    void TransferGirlsRightToLeft(bool rightfirst, int rightBrothel, int leftBrothel);
    int checkjobcolor(const sGirl& temp);

    IBuilding * getBuilding(int index) const;

    enum Side {
        Left = 0,
        Right = 1
    };
    void select_brothel(Side side, int selected);
};