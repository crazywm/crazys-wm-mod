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

#ifndef WM_BARJOBS_H
#define WM_BARJOBS_H

#include "jobs/GenericJob.h"
#include "utils/piecewise_linear.h"

struct sBarJobData {
    Action_Types Action;
};

class cBarJob : public cBasicJob {
public:
    cBarJob(JOBS job, const char* xml, sBarJobData data);
    sWorkJobResult DoWork(sGirl& girl, bool is_night) override;
    virtual bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) = 0;

protected:
    eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;
    void HandleGains(sGirl& girl, int enjoy, int fame);
    sBarJobData m_Data;

    PiecewiseLinearFunction PerformanceToWages;

    void load_from_xml_callback(const tinyxml2::XMLElement& job_element) override;
    void perf_text();

    int m_Wages;
    int m_Tips;
};

#endif //WM_BARJOBS_H
