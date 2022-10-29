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
#include <buildings/studio/cMovieStudio.h>
#include "buildings/IBuilding.h"
#include "buildings/queries.h"
#include "cGirls.h"
#include "jobs/GenericJob.h"
#include "StudioJobs.h"

extern const char* const FluffPointsId;
extern const char* const DirectorInteractionId;
extern const char* const CamMageInteractionId;
extern const char* const CrystalPurifierInteractionId;
extern const char* const StageHandPtsId;

class cJobCameraMage : public cCrewJob {
public:
    cJobCameraMage();
    void HandleUpdate(sGirl& girl, float performance) override {
        ProvideInteraction(CamMageInteractionId, 3);
    }
};

class cJobCrystalPurifier : public cCrewJob {
public:
    cJobCrystalPurifier();
    void HandleUpdate(sGirl& girl, float performance) override {
        ProvideInteraction(CrystalPurifierInteractionId, 3);
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
        ProvideInteraction(DirectorInteractionId, 3);
    };

};

cCrewJob::eCheckWorkResult cCrewJob::CheckWork(sGirl& girl, bool is_night) {
    auto brothel = girl.m_Building;
    if (brothel->num_girls_on_job(JOB_CAMERAMAGE, SHIFT_NIGHT) == 0 || brothel->num_girls_on_job(JOB_CRYSTALPURIFIER, SHIFT_NIGHT) == 0)
    {
        add_text("no-crew");
        girl.AddMessage(ss.str(), EImageBaseType::PROFILE, EVENT_NOWORK);
        return eCheckWorkResult::IMPOSSIBLE;    // not refusing
    }
    else if (GetNumberActresses(*brothel) < 1)
    {
        add_text("no-actress");
        girl.AddMessage(ss.str(), EImageBaseType::PROFILE, EVENT_NOWORK);
        return eCheckWorkResult::IMPOSSIBLE;    // not refusing
    }

    return cSimpleJob::CheckWork(girl, is_night);
}

bool cCrewJob::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) {
    m_Wages = 50;

    // slave girls not being paid for a job that normally you would pay directly for do less work
    if (girl.is_unpaid())
    {
        m_Performance *= 0.9;
    }
    else    // work out the pay between the house and the girl
    {
        // `J` zzzzzz - need to change pay so it better reflects how well she filmed the films
        int roll_max = m_Performance;
        roll_max /= 4;
        m_Wages += uniform(10, 10 + roll_max);
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

    girl.AddMessage(ss.str(), m_ImageType, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

    HandleUpdate(girl, m_Performance);

    // Improve stats
    apply_gains(girl, m_Performance);

    return false;
}


cJobCameraMage::cJobCameraMage() : cCrewJob(JOB_CAMERAMAGE, "CameraMage.xml", {ACTION_MOVIECREW, 50, EImageBaseType::CAMERA_MAGE}) {
    m_Info.Provides.emplace_back(CamMageInteractionId);
}

cJobCrystalPurifier::cJobCrystalPurifier() : cCrewJob(JOB_CRYSTALPURIFIER, "CrystalPurifier.xml", {ACTION_MOVIECREW, 50, EImageBaseType::PURIFIER}) {
    m_Info.Provides.emplace_back(CrystalPurifierInteractionId);
}

cJobFluffer::cJobFluffer() : cCrewJob(JOB_FLUFFER, "Fluffer.xml", {ACTION_MOVIECREW, 50, EImagePresets::BLOWJOB}) {
    m_Info.Provides.emplace_back(FluffPointsId);
}

void cJobFluffer::HandleUpdate(sGirl& girl, float performance) {
    ProvideResource(FluffPointsId, (int)performance);
}

cJobDirector::cJobDirector() : cCrewJob(JOB_DIRECTOR, "Director.xml", {ACTION_MOVIECREW, 50, EImageBaseType::DIRECTOR}) {
    m_Info.Provides.emplace_back(DirectorInteractionId);
}

class cJobStageHand : public cBasicJob {
public:
    cJobStageHand() : cBasicJob(JOB_STAGEHAND) {
        m_Info.Provides.emplace_back(StageHandPtsId);
    };
    eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;
    sWorkJobResult DoWork(sGirl& girl, bool is_night) override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
};

IGenericJob::eCheckWorkResult cJobStageHand::CheckWork(sGirl& girl, bool is_night) {
    int roll_a = d100();
    if (roll_a <= 50 && (girl.disobey_check(ACTION_MOVIECREW, JOB_STAGEHAND) || girl.disobey_check(ACTION_WORKCLEANING, JOB_STAGEHAND)))
    {
        ss << "${name} refused to work as a stagehand today.";
        girl.AddMessage(ss.str(), EImageBaseType::REFUSE, EVENT_NOWORK);
        return eCheckWorkResult::REFUSES;
    }
    return eCheckWorkResult::ACCEPTS;
}

sWorkJobResult cJobStageHand::DoWork(sGirl& girl, bool is_night) {
    auto brothel = dynamic_cast<sMovieStudio*>(girl.m_Building);
    int roll_a = d100();
    ss << "${name} worked as a stagehand.\n \n";

    cGirls::UnequipCombat(girl);    // not for studio crew
    int enjoyc = 0, enjoym = 0;
    m_Wages = 50;
    EImageBaseType imagetype = EImageBaseType::STAGEHAND;
    bool filming = true;


    // `J` - jobperformance and CleanAmt need to be worked out specially for this job.
    double jobperformance = 0;
    double CleanAmt = ((girl.service() / 10.0) + 5) * 5;
    CleanAmt += girl.get_trait_modifier(traits::modifiers::WORK_STAGEHAND_CLEAN_AMOUNT);
    jobperformance += girl.get_trait_modifier(traits::modifiers::WORK_STAGEHAND_PERFORMANCE);

    if (brothel->num_girls_on_job(JOB_CAMERAMAGE, SHIFT_NIGHT) == 0 ||
        brothel->num_girls_on_job(JOB_CRYSTALPURIFIER, SHIFT_NIGHT) == 0 ||
        GetNumberActresses(*brothel) < 1)
    {
        ss << "There were no scenes being filmed, so she just cleaned the set.\n \n";
        filming = false;
        imagetype = EImageBaseType::MAID;
    }

    if (roll_a <= 10)
    {
        enjoyc -= uniform(1, 3); if (filming) enjoym -= uniform(1, 3);
        CleanAmt *= 0.8;
        ss << "She did not like working in the studio today.";
    }
    else if (roll_a >= 90)
    {
        enjoyc += uniform(1, 3); if (filming) enjoym += uniform(1, 3);
        CleanAmt *= 1.1;
        ss << "She had a great time working today.";
    }
    else
    {
        enjoyc += std::max(0, uniform(-1, 2)); if (filming) enjoym += std::max(0, uniform(-1, 2));
        ss << "Otherwise, the shift passed uneventfully.";
    }
    jobperformance += enjoyc + enjoym;
    ss << "\n \n";

    CleanAmt = std::min((int)CleanAmt, brothel->m_Filthiness);

    if (filming)
    {
        jobperformance += (girl.crafting() / 5) + (girl.constitution() / 10) + (girl.service() / 10);
        jobperformance += girl.level();
        jobperformance += uniform(-1, 3);    // should add a -1 to +3 random element --PP

        // Cleaning reduces the points remaining for actual stage hand work
        jobperformance -= CleanAmt / 2;
        if(jobperformance < 0 && CleanAmt > 0) {
            ss << "Your studio was so messy that ${name} spent the entire shift cleaning up, and had no time to "
                  "assist in movie production. She improved the cleanliness rating by " << (int)CleanAmt << ".";
            jobperformance = 0;
        } else {
            ss << "She assisted the crew in movie production and provided " << (int)jobperformance << " stage hand points.";
        }
    }


    // slave girls not being paid for a job that normally you would pay directly for do less work
    if (girl.is_unpaid())
    {
        CleanAmt *= 0.9;
        m_Wages = 0;
    }
    else if (filming)
    {
        m_Wages += int(CleanAmt + jobperformance);
    }
    else
    {
        m_Wages += int(CleanAmt);
    }

    if (!filming && brothel->m_Filthiness < CleanAmt / 2) // `J` needs more variation
    {
        ss << "\n \n${name} finished her cleaning early so she hung out around the Studio a bit.";
        girl.upd_temp_stat(STAT_LIBIDO, uniform(1, 3), true);
        girl.happiness(uniform(1, 3));
    }


    girl.AddMessage(ss.str(), imagetype, EVENT_NIGHTSHIFT);

    ProvideResource(StageHandPtsId, int(jobperformance));
    brothel->m_Filthiness = std::max(0, brothel->m_Filthiness - int(CleanAmt));

    // Improve girl
    int xp = filming ? 15 : 10, skill = 3;
    if (enjoyc + enjoym > 2)                            { xp += 1; skill += 1; }
    if (girl.has_active_trait(traits::QUICK_LEARNER))        { skill += 1; xp += 3; }
    else if (girl.has_active_trait(traits::SLOW_LEARNER))    { skill -= 1; xp -= 3; }

    girl.exp(xp);
    girl.service(uniform(2, skill+1));

    if (filming) girl.upd_Enjoyment(ACTION_MOVIECREW, enjoym);
    girl.upd_Enjoyment(ACTION_WORKCLEANING, enjoyc);
    // Gain Traits
    if (chance(girl.service()))
        cGirls::PossiblyGainNewTrait(girl, "Maid", 90, ACTION_WORKCLEANING, "${name} has cleaned enough that she could work professionally as a Maid anywhere.", is_night);
    //lose traits
    cGirls::PossiblyLoseExistingTrait(girl, traits::CLUMSY, 30, ACTION_WORKCLEANING, "It took her spilling hundreds of buckets, and just as many reprimands, but ${name} has finally stopped being so Clumsy.", is_night);

    return {false, 0, 0, m_Wages};
}

double cJobStageHand::GetPerformance(const sGirl& girl, bool estimate) const {
    //SIN - standardizing job performance calc per J's instructs
    double jobperformance =
            //main stat - first 100
            girl.service() +
            //secondary stats - second 100
            ((girl.morality() + girl.obedience() + girl.agility()) / 3) +
            //add level
            girl.level();

    //tiredness penalty
    if (!estimate)
    {
        int t = girl.tiredness() - 80;
        if (t > 0)
            jobperformance -= (t + 2) * (t / 3);
    }

    jobperformance += girl.get_trait_modifier(traits::modifiers::WORK_CLEANING);

    return jobperformance;
}


void RegisterFilmCrewJobs(cJobManager& mgr) {
    mgr.register_job(std::make_unique<cJobCameraMage>());
    mgr.register_job(std::make_unique<cJobFluffer>());
    mgr.register_job(std::make_unique<cJobCrystalPurifier>());
    mgr.register_job(std::make_unique<cJobDirector>());
    mgr.register_job(std::make_unique<cJobStageHand>());
}