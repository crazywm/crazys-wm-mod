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

#ifndef WM_FARMJOBS_H
#define WM_FARMJOBS_H

#include "GenericJob.h"

struct sFarmJobData {
    Action_Types Action;
    int BaseWages;
};

class cFarmJob : public cBasicJob {
public:
    cFarmJob(JOBS job, sFarmJobData data);
    bool DoWork(sGirl& girl, bool is_night) override;
    eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;
    virtual bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night, double performance) = 0;

protected:
    cFarmJob(JOBS job, const char* xml, sFarmJobData data);
    void HandleGains(sGirl& girl, int enjoy);
    sFarmJobData m_Data;

    // cached data
    int wages;
};

#endif //WM_FARMJOBS_H
