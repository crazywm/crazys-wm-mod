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

#include <sstream>
#include <buildings/cMovieStudio.h>
#include "buildings/IBuilding.h"
#include "buildings/queries.h"
#include "cGirls.h"
#include "GenericJob.h"
#include "StudioJobs.h"

extern const char* const FluffPointsId;
extern const char* const DirectorInteractionId;
extern const char* const CamMageInteractionId;
extern const char* const CrystalPurifierInteractionId;

class cJobCameraMage : public cCrewJob {
public:
    cJobCameraMage();
    void HandleUpdate(sGirl& girl, float performance) override {
        girl.m_Building->ProvideInteraction(CamMageInteractionId, &girl, 3);
    }
};

class cJobCrystalPurifier : public cCrewJob {
public:
    cJobCrystalPurifier();
    void HandleUpdate(sGirl& girl, float performance) override {
        girl.m_Building->ProvideInteraction(CrystalPurifierInteractionId, &girl, 3);
    }
};

class cJobFluffer : public cCrewJob {
public:
    cJobFluffer();
    void HandleUpdate(sGirl& girl, float performance) override;

};

class cJobDirector : public cCrewJob {
public:
    cJobDirector();
    void HandleUpdate(sGirl& girl, float performance) override {
        girl.m_Building->ProvideInteraction(DirectorInteractionId, &girl, 3);
    };

};

cCrewJob::eCheckWorkResult cCrewJob::CheckWork(sGirl& girl, bool is_night) {
    auto brothel = girl.m_Building;
    if (brothel->num_girls_on_job(JOB_CAMERAMAGE, SHIFT_NIGHT) == 0 || brothel->num_girls_on_job(JOB_CRYSTALPURIFIER, SHIFT_NIGHT) == 0)
    {
        add_text("no-crew");
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return eCheckWorkResult::IMPOSSIBLE;    // not refusing
    }
    else if (GetNumberActresses(*brothel) < 1)
    {
        add_text("no-actress");
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return eCheckWorkResult::IMPOSSIBLE;    // not refusing
    }

    if (girl.disobey_check(ACTION_MOVIECREW, job()))
    {
        add_text("refuse");
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return IGenericJob::eCheckWorkResult::REFUSES;
    }
    return IGenericJob::eCheckWorkResult::ACCEPTS;
}

bool cCrewJob::DoWork(sGirl& girl, bool is_night) {
    add_text("work") << "\n";
    cGirls::UnequipCombat(girl);    // not for studio crew
    int wages = 50;

    // slave girls not being paid for a job that normally you would pay directly for do less work
    if (girl.is_unpaid())
    {
        m_Performance *= 0.9;
        wages = 0;
    }
    else    // work out the pay between the house and the girl
    {
        // `J` zzzzzz - need to change pay so it better reflects how well she filmed the films
        int roll_max = m_Performance;
        roll_max /= 4;
        wages += uniform(10, 10 + roll_max);
    }

    if (m_Performance >= 166)
    {
        add_text("work.perfect");
    }
    else if (m_Performance >= 133)
    {
        add_text("work.great");
    }
    else if (m_Performance >= 100)
    {
        add_text("work.good");
    }
    else if (m_Performance >= 66)
    {
        add_text("work.ok");
    }
    else if (m_Performance >= 33)
    {
        add_text("work.bad");
    }
    else
    {
        add_text("work.worst");
    }

    girl.AddMessage(ss.str(), m_EventImage, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
    girl.m_Tips = 0;
    girl.m_Pay = std::max(0, wages);

    HandleUpdate(girl, m_Performance);

    // Improve stats
    apply_gains(girl, m_Performance);

    return false;
}


cJobCameraMage::cJobCameraMage() : cCrewJob(JOB_CAMERAMAGE, "CameraMage.xml"){
}

cJobCrystalPurifier::cJobCrystalPurifier() : cCrewJob(JOB_CRYSTALPURIFIER, "CrystalPurifier.xml"){
}

cJobFluffer::cJobFluffer() : cCrewJob(JOB_FLUFFER, "Fluffer.xml") {
    m_EventImage = IMGTYPE_ORAL;
}

void cJobFluffer::HandleUpdate(sGirl& girl, float performance) {
    girl.m_Building->ProvideResource(FluffPointsId, (int)performance);
}

cJobDirector::cJobDirector() : cCrewJob(JOB_DIRECTOR, "Director.xml") {
    m_EventImage = IMGTYPE_FORMAL;
}

void RegisterFilmCrewJobs(cJobManager& mgr) {
    mgr.register_job(std::make_unique<cJobCameraMage>());
    mgr.register_job(std::make_unique<cJobFluffer>());
    mgr.register_job(std::make_unique<cJobCrystalPurifier>());
    mgr.register_job(std::make_unique<cJobDirector>());
}