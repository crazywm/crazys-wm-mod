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

#ifndef WM_JOBS_CRAFTING_H
#define WM_JOBS_CRAFTING_H

#include "SimpleJob.h"
#include <sstream>
#include <vector>


class GenericCraftingJob : public cSimpleJob {
public:
    explicit GenericCraftingJob(JOBS id, const char* xml, Action_Types action, int BaseWages, Image_Types image) :
        cSimpleJob(id, xml, {action, BaseWages}) {
    }

protected:
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
    eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;

    Image_Types m_Image;

    // shift processing data
    int craftpoints;
private:
    virtual void performance_msg();
    virtual void DoWorkEvents(sGirl& girl);

    float DoCrafting(sGirl& girl, int craft_points);
};

#endif //WM_JOBS_CRAFTING_H
