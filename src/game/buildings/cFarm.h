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

#ifndef __CFARM_H
#define __CFARM_H

#include "IBuilding.hpp"

// defines a single farm
struct sFarm : public IBuilding
{
    sFarm();
    ~sFarm();

    void save_additional_xml(tinyxml2::XMLElement& root) const override {};
    void auto_assign_job(sGirl& target, std::stringstream& message, bool is_night) override;
    void UpdateGirls(bool is_night) override;
};


#endif  /* __CFARM_H */
