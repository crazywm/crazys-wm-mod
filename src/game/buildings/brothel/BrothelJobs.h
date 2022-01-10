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

#ifndef WM_BROTHELJOBS_H
#define WM_BROTHELJOBS_H

#include "jobs/SimpleJob.h"

class cBarJob : public cSimpleJob {
public:
    using cSimpleJob::cSimpleJob;
protected:
    eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;
};

class cEscortJob : public cSimpleJob {
public:
    cEscortJob();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
protected:
    eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;

private:
    int m_Escort;
    int m_Prepare;

    enum class SexType {
        NONE, ANY, SEX, ANAL, ORAL, HAND, TITTY
    };

    struct sClientData {
        bool TittyLover = false;
        bool AssLover = false;
        SexType SexOffer = SexType::NONE;
    };

    SexType choose_sex(const std::string& prefix, const sGirl& girl, const sClientData& client);
    Image_Types handle_sex(const std::string& prefix, int& fame, sGirl& girl, SexType type);
};

class cWhoreJob : public cSimpleJob {
public:
    cWhoreJob(JOBS job, const char* short_name, const char* description);

    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;

private:
    void HandleCustomer(sGirl& girl, IBuilding& brothel, bool is_night);
    int m_NumSleptWith = 0;
    int m_OralCount = 0;
    std::stringstream m_FuckMessage;
};

#endif //WM_BROTHELJOBS_H
