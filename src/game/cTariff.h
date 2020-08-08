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

struct sGirl;
class IBuilding;

class cTariff {
    double slave_base_price(sGirl& girl) const;
public:
    cTariff() = default;

    int goon_weapon_upgrade(int level);
    int goon_mission_cost(int mission);
    int healing_price(int n);
    int nets_price(int n);
    int anti_preg_price(int n);

    int empty_room_cost(IBuilding& brothel);

    int advertising_costs(int budget);
    int add_room_cost(int n);

    double slave_price(sGirl& girl, bool buying) const;
    int slave_buy_price(sGirl& girl) const;
    int slave_sell_price(sGirl& girl) const;

    int male_slave_sales();
    int creature_sales();
};
