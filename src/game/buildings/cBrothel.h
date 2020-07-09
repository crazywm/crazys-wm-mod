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

#ifndef __CBROTHEL_H
#define __CBROTHEL_H

#include "IBuilding.hpp"

// ---------------------------------------------------------------------------------------------------------------------

// defines a single brothel
struct sBrothel : public IBuilding
{
    explicit sBrothel();        // constructor
    virtual ~sBrothel();        // destructor

    unsigned char    m_Bar;                        // level of bar: 0 => none
    unsigned char    m_GamblingHall;                // as above

    // For keeping track of any shows currently being produced here
    int                m_ShowTime;            // when reaches 0 then the show is ready
    int                m_ShowQuality;        // Determined by the average fame and skill of the girls in the show
    unsigned char    m_HasGambStaff;        // gambling hall or
    unsigned char    m_HasBarStaff;        // Bar staff. Not as good as girls but consistent

    void save_additional_xml(tinyxml2::XMLElement& root) const override;
    virtual bool LoadXML(tinyxml2::XMLElement * pBrothel);
    // currently unused for brothel
    void auto_assign_job(sGirl* target, std::stringstream& message, bool is_night) override {};

    void Update() override;
    void UpdateGirls(bool is_night) override;

    bool runaway_check(sGirl * girl);
};

#endif  /* __CBROTHEL_H */
