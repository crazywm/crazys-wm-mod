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
#pragma once

#include "interface/cInterfaceWindowXML.h"
#include "cGameSettings.h"

class cScreenGameConfig: public cInterfaceWindowXML
{
private:
    cGameSettings m_Settings;
    std::vector<const sKeyValueEntry*> m_SettingsList;
    int back_id;
    int ok_id;
    int revert_id;
    int list_id;

    /// If this is set to true, the screen will acts in "in-game" mode, where only "user" settings can be edited.
    bool m_InGameMode;

public:
    explicit cScreenGameConfig(bool use_in_game_mode);
    void init(bool back) override;
    void process() override { };
    void set_ids() override;
};