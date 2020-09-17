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

#ifndef WM_PASSIVEJOB_H
#define WM_PASSIVEJOB_H

#include "GenericJob.h"

/*!
 * Passive jobs are jobs where you ask the girl to get some treatment, surgery etc.
 * They are characterized by:
 *  1) There is no concept of Job Performance. Instead, some girls may benefit more from
 *     certain treatments.
 *  2) In order for the job to happen, another character must work on this girl.
 */
class IPassiveJob : public IGenericJob {
public:
    double GetPerformance(const sGirl& girl, bool estimate) const override;
    bool DoWork(sGirl& girl, bool is_night) override;

    bool GetTreated(sGirl& self, sGirl& actor, bool is_night);
private:

};

#endif //WM_PASSIVEJOB_H
