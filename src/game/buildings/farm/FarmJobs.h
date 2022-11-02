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

#include "jobs/SimpleJob.h"

class cFarmJob : public cSimpleJob {
public:
    using cSimpleJob::cSimpleJob;
protected:
    // a version of handle gains that leaves fame unaffected
    void HandleGains(sGirl& girl);
};

class cFarmJobFarmer : public cFarmJob {
public:
    cFarmJobFarmer();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
};

class cFarmJobMarketer : public cFarmJob {
public:
    cFarmJobMarketer();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
};

class cFarmJobVeterinarian : public cFarmJob {
public:
    cFarmJobVeterinarian();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
private:
};

class cFarmJobShepherd : public cFarmJob {
public:
    cFarmJobShepherd();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
};

class cFarmJobRancher : public cFarmJob {
public:
    cFarmJobRancher();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
};

class cFarmJobMilker : public cFarmJob {
public:
    cFarmJobMilker();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night);
};

class cFarmJobGetMilked : public cFarmJob {
public:
    cFarmJobGetMilked();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
private:
    double toOz(int ml)            { return (0.0338 * ml); }
};

class cFarmJobCatacombRancher : public cFarmJob {
public:
    cFarmJobCatacombRancher();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
};

class cFarmJobBeastCapture : public cFarmJob {
public:
    cFarmJobBeastCapture();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night);
};

class cFarmJobResearch : public cFarmJob {
public:
    cFarmJobResearch();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
};

#endif //WM_FARMJOBS_H
