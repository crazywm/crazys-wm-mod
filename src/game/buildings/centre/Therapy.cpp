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

#include "jobs/Treatment.h"
#include "cJobManager.h"
#include "cGirls.h"
#include "character/sGirl.h"
#include "buildings/IBuilding.h"
#include <sstream>
#include <utility>
#include "character/predicates.h"
#include "IGame.h"
#include "cGirlGangFight.h"
#include "buildings/cBuildingManager.h"
#include "xml/getattr.h"
#include "xml/util.h"

extern const char* const CounselingInteractionId;

struct sRemoveTrait {
    const char* Trait;
    const char* Message;
};

class TherapyJob : public ITreatmentJob {
public:
    explicit TherapyJob(JOBS id, std::string xml_file) : ITreatmentJob(id, std::move(xml_file)) {
    }

    void ReceiveTreatment(sGirl& girl, bool is_night) final;
    sJobValidResult is_job_valid(const sGirl& girl) const override;
protected:
    // common data
    std::string TreatmentName;     //!< Therapy or Rehab
    int         Duration;          //!< How many days until finished.
    int         BasicFightChance;  //!< Chance to fight with the counselor
    int         SuccessBonus;      //!< Bonus for enjoyment and happiness once therapy was a success
    int         HealthDanger;      //!< How much health does she lose TODO does it make sense that she loses health in therapy but not without therapy???
    std::vector<sRemoveTrait> TraitRemove;   //!< The traits that this therapy can remove

    eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;

    virtual void FightEvent(sGirl& girl, bool is_night);
    virtual void OnFinish(sGirl& girl) {}
    bool needs_therapy(const sGirl& girl) const;
    const char* specific_config_element() const override { return "Therapy"; }
    void load_from_xml_callback(const tinyxml2::XMLElement& job_element) override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
};

void TherapyJob::load_from_xml_callback(const tinyxml2::XMLElement& job_element) {
    Duration = GetIntAttribute(job_element, "Duration");
    BasicFightChance = GetIntAttribute(job_element, "FightChance");
    TreatmentName = GetStringAttribute(job_element, "Title");
    SuccessBonus = GetIntAttribute(job_element, "SuccessBonus");
    HealthDanger = GetIntAttribute(job_element, "HealthDanger");

    for (auto& remove_el : IterateChildElements(job_element, "RemoveTrait")) {
        TraitRemove.emplace_back(sRemoveTrait{GetStringAttribute(remove_el, "Trait"),
                                                            remove_el.GetText()});
    }
}

void TherapyJob::ReceiveTreatment(sGirl& girl, bool is_night) {
    Action_Types actiontype = ACTION_WORKTHERAPY;

    sGirl* counselor = RequestInteraction(CounselingInteractionId);

    if (chance(BasicFightChance) || girl.disobey_check(actiontype, job()))    // `J` - yes, OR, not and.
    {
        FightEvent(girl, is_night);
    }
    add_text("therapy") << "\n\n";

    int enjoy = 0;
    auto msgtype = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

#pragma region //    Count the Days                //

    if (!is_night) {
        girl.make_treatment_progress(uniform(90, 110) / Duration);
    }

    girl.happiness(uniform(-20, 10));
    girl.spirit(uniform(-5, 5));
    girl.mana(uniform(-5, 5));

    // `J` % chance a counselor will save her if she almost dies
    int healthmod = uniform(-HealthDanger, 2);
    if (girl.health() + healthmod < 1 && chance(95 + (girl.health() + healthmod)))
    {    // Don't kill the girl from therapy if a Counselor is on duty
        girl.set_stat(STAT_HEALTH, 1);
        girl.pcfear(5);
        girl.pclove(-20);
        ss << "She almost died in " << TreatmentName << " but the Counselor saved her.\n";
        ss << "She hates you a little more for forcing this on her.\n \n";
        msgtype = EVENT_DANGER;
        enjoy -= 2;
    }
    else
    {
        girl.health(healthmod);
        enjoy += (healthmod / 5) + 1;
    }

    if (girl.health() < 1)
    {
        add_text("death");
        msgtype = EVENT_DANGER;
    }

    if (girl.get_treatment_progress() >= 100 && is_night)
    {
        enjoy += uniform(0, SuccessBonus);
        girl.upd_Enjoyment(ACTION_WORKCOUNSELOR, uniform(-2, 4));    // `J` She may want to help others with their problems
        girl.happiness(uniform(0, SuccessBonus));

        ss << "The " << TreatmentName << " is a success.\n";
        msgtype = EVENT_GOODNEWS;

        RandomSelector<sRemoveTrait> selector;
        for(auto& t : TraitRemove) {
            if(girl.has_active_trait(t.Trait)) {
                selector.process(&t);
            }
        }

        if(auto sel = selector.selection()) {
            if (girl.lose_trait(sel->Trait))
            {
                ss << sel->Message << "\n";
            }
        }

        OnFinish(girl);

        if (needs_therapy(girl))
        {
            ss << "\n";
            add_text("continue-therapy");
        }
        else // get out of therapy
        {
            ss << "\n";
            add_text("release");
            girl.FullJobReset(JOB_RESTING);
        }
    }
    else
    {
        ss << "The " << TreatmentName << " is in progress (" << girl.get_treatment_progress() << "%).";
    }

    // Improve girl
    girl.AddMessage(ss.str(), EImageBaseType::PROFILE, msgtype);
    girl.upd_Enjoyment(actiontype, enjoy);
}

bool TherapyJob::needs_therapy(const sGirl& girl) const {
    return std::any_of(begin(TraitRemove), end(TraitRemove),
     [&](const sRemoveTrait& t){
         return girl.has_active_trait(t.Trait);
     });
}

void TherapyJob::FightEvent(sGirl& girl, bool is_night) {
    ss << "${name} fought with her counselor and did not make any progress this week.";
    girl.AddMessage(ss.str(), EImageBaseType::REFUSE, EVENT_NOWORK);
    girl.upd_Enjoyment(ACTION_WORKTHERAPY, -1);
    if (is_night) {
        girl.make_treatment_progress(-uniform(10, 20));
    }
}

double TherapyJob::GetPerformance(const sGirl& girl, bool estimate) const {
    if(!needs_therapy(girl)) return -1000;
    double p = 100;
    for(auto& t : TraitRemove) {
        if(girl.has_active_trait(t.Trait)) p += 100;
    }
    return p;
}

IGenericJob::eCheckWorkResult TherapyJob::CheckWork(sGirl& girl, bool is_night) {
    auto brothel = girl.m_Building;
    if (!needs_therapy(girl))
    {
        add_text("no-need") << " She was sent to the waiting room.";
        if (!is_night)    girl.AddMessage(ss.str(), EImageBaseType::PROFILE, EVENT_WARNING);
        girl.FullJobReset(JOB_RESTING);
       // girl.m_PrevWorkingDay = girl.m_WorkingDay = 0;
        return eCheckWorkResult::IMPOSSIBLE; // not refusing
    }

    if (!brothel->HasInteraction(CounselingInteractionId))
    {
        add_text("no-counselor");
        girl.AddMessage(ss.str(), EImageBaseType::PROFILE, EVENT_WARNING);
        return eCheckWorkResult::IMPOSSIBLE;    // not refusing
    }

    return IGenericJob::eCheckWorkResult::ACCEPTS;
}

sJobValidResult TherapyJob::is_job_valid(const sGirl& girl) const {
    if (!needs_therapy(girl)) {
        return {false, get_text("no-need")};
    }
    return IGenericJob::is_job_valid(girl);
}

struct AngerManagement : public TherapyJob {
    using TherapyJob::TherapyJob;
    void FightEvent(sGirl& girl, bool is_night) override;
};

void AngerManagement::FightEvent(sGirl& girl, bool is_night) {
    girl.upd_Enjoyment(ACTION_WORKTHERAPY, -1);
    girl.make_treatment_progress(-uniform(10, 20));
    if (chance(10))
    {
        girl.upd_Enjoyment(ACTION_WORKTHERAPY, -5);
        bool runaway = false;
        // if there is no counselor, it should not get to here
        sGirl* counselor = girl.m_Building->girls().get_random_girl(HasJob(JOB_COUNSELOR, is_night));
        ss << "\n \n${name} fought hard with her counselor " << counselor->FullName();
        /// TODO Gangs and Security
        auto winner = GirlFightsGirl(*counselor, girl);
        if (winner != EFightResult::VICTORY)    // the patient won
        {
            ss << " and won.\n \n";
            girl.upd_Enjoyment(ACTION_COMBAT, 5);
            counselor->upd_Enjoyment(ACTION_WORKCOUNSELOR, -5);
            counselor->upd_Enjoyment(ACTION_COMBAT, -2);

            if (chance(10))    // and ran away
            {
                runaway = true;
                std::stringstream smess;
                smess << girl.FullName() << " fought with her counselor and ran away.\nSend your goons after her to attempt recapture.\nShe will escape for good after 6 weeks.\n";
                g_Game->push_message(smess.str(), COLOR_WARNING);
                girl.run_away();
                return;
            }
        }
        else    // the counselor won
        {
            ss << " and lost.\n \n";
            girl.upd_Enjoyment(ACTION_WORKTHERAPY, -5);
            girl.upd_Enjoyment(ACTION_COMBAT, -5);
            counselor->upd_Enjoyment(ACTION_WORKCOUNSELOR, -1);
            counselor->upd_Enjoyment(ACTION_COMBAT, 2);
        }
        std::stringstream ssc;
        ssc << "${name} had to defend herself from " << girl.FullName() << " who she was counseling.\n";
        if (runaway) ss << "${name} ran out of the Counceling Centre and has not been heard from since.";
        counselor->AddMessage(ssc.str(), EImageBaseType::COMBAT, EVENT_WARNING);
    }
    else
    {
        ss << "${name} fought with her counselor and did not make any progress this week.";
    }
    girl.AddMessage(ss.str(), EImageBaseType::PROFILE, EVENT_NOWORK);
}

struct Rehab : public TherapyJob {
    using TherapyJob::TherapyJob;
    void OnFinish(sGirl& girl) override;
};

void Rehab::OnFinish(sGirl& girl) {
    girl.add_temporary_trait(traits::FORMER_ADDICT, 40);
}

void RegisterTherapyJobs(cJobManager& mgr) {
    mgr.register_job(std::make_unique<AngerManagement>(JOB_ANGER, "AngerManagement.xml"));
    mgr.register_job(std::make_unique<TherapyJob>(JOB_EXTHERAPY, "ExtremeTherapy.xml"));
    mgr.register_job(std::make_unique<TherapyJob>(JOB_THERAPY, "Therapy.xml"));
    mgr.register_job(std::make_unique<Rehab>(JOB_REHAB, "Rehab.xml"));
}
