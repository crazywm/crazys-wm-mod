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

#include "GenericJob.h"
#include "cJobManager.h"
#include "cGirls.h"
#include "character/sGirl.h"
#include "buildings/IBuilding.h"
#include <sstream>
#include "character/predicates.h"

extern const char* const CarePointsBasicId;
extern const char* const CarePointsGoodId;

struct sExcludeTrait {
    const char* Trait;
    const char* Message;
};

struct sSurgeryData {
    const char*  SurgeryMessage;    //!< The message used to indicate that the surgery was performed
    int          Duration;          //!< How many days until finished.
    std::vector<sExcludeTrait> TraitExcludes;   //!< IF she has any trait in this list, she will not receive surgery
};


struct SurgeryJob : public IGenericJob {
    // TODO is_valid exists already as `is_job_valid`
public:
    explicit SurgeryJob(JOBS id, const char* short_name, sSurgeryData data) : IGenericJob(id), m_SurgeryData(data) {
        m_Info.ShortName = short_name;
        m_Info.FullTime = true;
    }

    bool DoWork(sGirl& girl, bool is_night) final;
    sJobValidResult is_job_valid(const sGirl& girl) const override;
    eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;
protected:
    // common data
    sSurgeryData m_SurgeryData;

    virtual void success(sGirl& girl) = 0;

private:
    bool nursing_effect(sGirl& girl);
};

bool SurgeryJob::DoWork(sGirl& girl, bool is_night) {
    auto brothel = girl.m_Building;
    JOBS job_id = job();

    auto msgtype = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;
    cGirls::UnequipCombat(girl);    // not for patient

    if (girl.m_YesterDayJob != job_id) { girl.m_WorkingDay = girl.m_PrevWorkingDay = 0; }

    ss << " " << m_SurgeryData.SurgeryMessage << "\n \n";

    // update progress
    if (!is_night)    // the Doctor works on her during the day
    {
        if(girl.health() < 50) {
            int bp = uniform(4, 7);
            if(brothel->TryConsumeResource(CarePointsBasicId, bp)) {
                int req_gp = uniform(2, 4);
                if(brothel->TryConsumeResource(CarePointsGoodId, req_gp)) {
                    ss << "${name}'s health condition does not allow surgery to be performed. "
                          "Your highly qualified nursing staff are making sure she get well enough for her treatment soon.";
                    girl.health( uniform(5, 10) );
                    girl.happiness(1);
                } else {
                    ss << "Your caretakers are trying to nurse ${name} back to health so that the doctor can perform the surgery.";
                    girl.health( uniform(3, 5) );
                }
            } else {
                ss << "${name}'s health condition does not allow surgery to be performed, and with no nurses around"
                      " it may take a long time for her to get better. All she can do is rest.";
                girl.health( uniform(1, 3) );
            }
            girl.tiredness(-uniform(5, 10));
            if(girl.strength() > 50) {
                girl.strength(-uniform(0, 2));
            }
            girl.AddMessage(ss.str(), IMGTYPE_PROFILE, msgtype);
            return false;
        }

        girl.m_WorkingDay++;
        nursing_effect(girl);


    } else    // and if there are nurses on duty, they take care of her at night
    {
        int req_gp = uniform(3, 6);
        if(brothel->TryConsumeResource(CarePointsGoodId, req_gp)) {
            brothel->ConsumeResource(CarePointsBasicId, req_gp);
            girl.m_WorkingDay++;
            ss << "Your professional nurses ensure a speedy recovery.";
            girl.health( uniform(3, 5) );
        } else {
            if(!nursing_effect(girl) && chance(50)) {
                girl.m_WorkingDay -= 1;
                ss << " These complications will prolong her stay in the hospital.";
            }
        }
    }

    // process progress
    if (girl.m_WorkingDay < m_SurgeryData.Duration || !is_night) {
        int wdays = (m_SurgeryData.Duration - girl.m_WorkingDay);
        ss << "The operation is in progress" << " (" << wdays << " day remaining).\n";
    } else {
        msgtype = EVENT_GOODNEWS;
        girl.m_WorkingDay = girl.m_PrevWorkingDay = 0;
        success(girl);
    }

    girl.AddMessage(ss.str(), IMGTYPE_PROFILE, msgtype);

    // Improve girl
    int libido = 0;
    if (girl.has_active_trait("Masochist")) libido += 1;
    girl.upd_temp_stat(STAT_LIBIDO, libido);

    if (chance(10.f))
        girl.medicine(1);    // `J` she watched what the doctors and nurses were doing
    return false;
}

IGenericJob::eCheckWorkResult SurgeryJob::CheckWork(sGirl& girl, bool is_night) {
    auto brothel = girl.m_Building;
    // check validity of the job
    auto valid = is_job_valid(girl);
    if (!valid) {
        girl.FullJobReset(JOB_RESTING);
        girl.m_WorkingDay = girl.m_PrevWorkingDay = 0;
        ss << valid.Reason << " She was sent to the waiting room.";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
        return IGenericJob::eCheckWorkResult::IMPOSSIBLE;    // not refusing
    }

    bool hasDoctor = brothel->num_girls_on_job(JOB_DOCTOR, is_night) > 0;
    if (!hasDoctor) {
        ss << "${name} does nothing. You don't have any Doctors working. (require 1) ";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
        return IGenericJob::eCheckWorkResult::IMPOSSIBLE;    // not refusing
    }

    return IGenericJob::eCheckWorkResult::ACCEPTS;
}

sJobValidResult SurgeryJob::is_job_valid(const sGirl& girl) const {
    for(auto& t : m_SurgeryData.TraitExcludes) {
        if(girl.has_active_trait(t.Trait)) {
            return {false, t.Message};
        }
    }
    return IGenericJob::is_job_valid(girl);
}

bool SurgeryJob::nursing_effect(sGirl& girl) {
    int health_dmg = uniform(4, 7);
    health_dmg -= girl.m_Building->ConsumeResource(CarePointsBasicId, health_dmg);

    if(health_dmg > 0) {
        if(girl.gain_trait("Small Scars", 2)) {
            ss << "Due to a lack of care by the nurses, ${name}'s wounds have become infected. This surgery will "
                  "leave Scars.";
            girl.health(-10);
            girl.tiredness(10);
            girl.happiness(-10);
            girl.strength(-uniform(0, 5));
            girl.spirit(-2);
        } else {
            ss << "You seem to have too few nurses that provide care for your patients after their operation. "
                  "This has negative effects on {name}'s health.";
            girl.health(-health_dmg);
            girl.tiredness(2 * health_dmg);
            girl.happiness(-5);
            girl.beauty(chance(20) ? -1 : 0);
        }
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

struct CosmeticSurgery: public SurgeryJob {
    CosmeticSurgery();
    void success(sGirl& girl) override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
};

CosmeticSurgery::CosmeticSurgery() : SurgeryJob(JOB_COSMETICSURGERY, "CosS",
                                                {"${name} is in the Clinic to get general surgery.", 5}) {
    m_Info.Description = "She will undergo magical surgery to \"enhance\" her appearance.\n*(Takes up to 5 days, less if a Nurse is on duty)";
}

double CosmeticSurgery::GetPerformance(const sGirl& girl, bool estimate) const {
    double jobperformance = 0.0;
    if (estimate)    // for third detail string - how much do they need this?
    {
        if (!girl.has_active_trait("Sexy Air")) jobperformance += 100;
        if (!girl.has_active_trait("Cute"))     jobperformance += 100;
        jobperformance += (100 - girl.charisma());
        jobperformance += (100 - girl.beauty());
    }
    return jobperformance;
}

void CosmeticSurgery::success(sGirl& girl) {
    ss << "The surgery is a success.\n";

    girl.beauty(rng().bell(5, 12));
    girl.charisma(rng().bell(0, 3));


    if (girl.gain_trait("Sexy Air"))
    {
        ss << "She gains Sexy Air trait.\n";
    }
    else if (girl.gain_trait( "Cute"))
    {
        ss << "She gains Cute trait.\n";
    }

    if (girl.has_active_trait("Sexy Air") && girl.has_active_trait("Cute") && girl.beauty() > 99)
    {
        ss << "\n \nShe has been released from the Clinic.";
        girl.FullJobReset(JOB_RESTING);
    }
}

struct Liposuction: public SurgeryJob {
    Liposuction();
    void success(sGirl& girl) override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
};

Liposuction::Liposuction() : SurgeryJob(JOB_LIPO, "Lipo", {"${name} is in the Clinic to get fat removed.", 5,
                                        {{"Great Figure", "${name} already has a Great Figure."}}})
{
    m_Info.Description = "She will undergo liposuction to \"enhance\" her figure.\n*(Takes up to 5 days, less if a Nurse is on duty)";
}


void Liposuction::success(sGirl& girl) {
    ss << "The surgery is a success.\n";

    girl.beauty(rng().bell(0, 10));
    girl.charisma(rng().bell(-1, 2));


    if (girl.lose_trait( "Plump"))
    {
        ss << "Thanks to the surgery she is no longer Plump.\n";
    }
    else if (girl.gain_trait( "Great Figure"))
    {
        ss << "Thanks to the surgery she now has a Great Figure.\n";
    }

    if (girl.has_active_trait("Great Figure") && !girl.has_active_trait("Plump"))
    {
        ss << "She has been released from the Clinic.\n \n";
        girl.FullJobReset(JOB_RESTING);
    }
}

double Liposuction::GetPerformance(const sGirl& girl, bool estimate) const {
    if (girl.is_pregnant())                        return 80;        // D - not recommended while pregnant
    if (girl.has_active_trait("Plump"))            return 400;        // I - do it
    if (girl.has_active_trait("Great Figure"))    return -1000;    // X - not needed
    return 200;                                                    // A - can improve
}

struct BreastReduction: public SurgeryJob {
    BreastReduction();
    void success(sGirl& girl) override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
};

BreastReduction::BreastReduction() : SurgeryJob(JOB_BREASTREDUCTION, "BRS",
                                                {"${name} is in the Clinic to get her breasts reduced.", 1,
                                                 {{"Flat Chest", "${name} already has a Flat Chest."}}}) {
    m_Info.Description = "She will undergo breast reduction surgery.\n*(Takes up to 5 days, less if a Nurse is on duty)";
}

void BreastReduction::success(sGirl& girl) {
    ss << "The surgery is a success.\n";

    ss << cGirls::AdjustTraitGroupBreastSize(girl, -1, false) << "\n \n";

    if (girl.has_active_trait("Flat Chest"))
    {
        ss << "${name}'s breasts are as small as they can get so she was sent to the waiting room.";
        girl.FullJobReset(JOB_RESTING);
    }
}

double BreastReduction::GetPerformance(const sGirl& girl, bool estimate) const {
    double jobperformance = 0.0;
    if (estimate)    // for third detail string - how much do they need this?
    {
        int values[] = {-1000, 0, 70, 100, 100, 150, 200, 250, 350, 400};
        return values[girl.breast_size() - 1];
    }
    return jobperformance;
}

struct BoobJob: public SurgeryJob {
    BoobJob();
    void success(sGirl& girl) override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
};

BoobJob::BoobJob() : SurgeryJob(JOB_BOOBJOB, "BbJb", {"${name} is in the Clinic to get her breasts enlarged.", 1,
                                 {{"Titanic Tits", "${name} already has Titanic Tits."}}}) {
    m_Info.Description = "She will undergo surgery to \"enhance\" her bust.\n*(Takes up to 5 days, less if a Nurse is on duty)";
}

void BoobJob::success(sGirl& girl) {
    ss << "The surgery is a success.\n";

    ss << cGirls::AdjustTraitGroupBreastSize(girl, 1, false) << "\n \n";

    if (girl.has_active_trait("Titanic Tits"))
    {
        ss << "${name}'s breasts are as large as they can get so she was sent to the waiting room.";
        girl.FullJobReset(JOB_RESTING);
    }
}

double BoobJob::GetPerformance(const sGirl& girl, bool estimate) const {
    double jobperformance = 0.0;
    if (estimate)    // for third detail string - how much do they need this?
    {
        int values[] = {400, 250, 250, 200, 200, 150, 100, 70, 0, -1000};
        return values[girl.breast_size() - 1];
    }
    return jobperformance;
}

struct VaginalRejuvenation: public SurgeryJob {
    VaginalRejuvenation();
    void success(sGirl& girl) override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
};

VaginalRejuvenation::VaginalRejuvenation() : SurgeryJob(JOB_VAGINAREJUV, "VagR",
                                                        {"${name} is in the Clinic to get her vagina tightened.", 5,
                                                         {{"Virgin", "${name} is already a Virgin."}}}) {
    m_Info.Description = "She will undergo surgery to make her a virgin again.\n*(Takes up to 5 days, less if a Nurse is on duty)";
}

void VaginalRejuvenation::success(sGirl& girl) {
    ss << "The surgery is a success.\nShe is a 'Virgin' again.\n";

    girl.gain_trait("Virgin");
    girl.FullJobReset(JOB_RESTING);
    ss << "\n \nShe has been released from the Clinic.";
}

double VaginalRejuvenation::GetPerformance(const sGirl& girl, bool estimate) const {
    if (is_virgin(girl))            return -1000;    // X - not needed
    if (girl.is_pregnant())            return 80;        // D - is her name Mary?
    return 400;                                            // I - needs it
}

struct FaceLift: public SurgeryJob {
    FaceLift();
    sJobValidResult is_job_valid(const sGirl& girl) const override;
    void success(sGirl& girl) override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
};

FaceLift::FaceLift() : SurgeryJob(JOB_FACELIFT, "FLft", {"${name} is in the Clinic to get a face lift.", 5}) {
    m_Info.Description = "She will undergo surgery to make her younger.\n*(Takes up to 5 days, less if a Nurse is on duty)";
}

sJobValidResult FaceLift::is_job_valid(const sGirl& girl) const {
    if (girl.age() <= 21)
    {
        return {false, "${name} is too young to get a Face Lift."};
    }
    return SurgeryJob::is_job_valid(girl);
}

void FaceLift::success(sGirl& girl) {
    ss << "The surgery is a success.\nShe looks a few years younger.\n";

    girl.beauty(rng().bell(4, 10));
    girl.charisma(rng().bell(-1, 1));
    girl.age(rng().bell(-2, -1));

    if (girl.age() <= 18) girl.set_stat(STAT_AGE, 18);
    if (girl.age() <= 21)
    {
        ss << "\n \nShe has been released from the Clinic.";
        girl.FullJobReset(JOB_RESTING);
    }
}

double FaceLift::GetPerformance(const sGirl& girl, bool estimate) const {
    double jobperformance = 0.0;
    if (estimate)    // for third detail string - how much do they need this?
    {
        if (girl.age() <= 21)    return -1000;            // X - not needed
        if (girl.age() == 100) return 0;                // E - unknown age?

        // this probably needs to be reworked
        jobperformance += 50 + girl.age() * 5;
        jobperformance -= girl.charisma();
        jobperformance -= girl.beauty();
    }
    return jobperformance;
}

struct AssJob: public SurgeryJob {
    AssJob();
    void success(sGirl& girl) override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
};

AssJob::AssJob(): SurgeryJob(JOB_ASSJOB, "AssJ",
                             {"${name} is in the Clinic to get her ass worked on.", 5,
                              {{"Great Arse", "${name} already has a Great Arse."}}}) {
    m_Info.Description = "She will undergo surgery to \"enhance\" her ass.\n*(Takes up to 5 days, less if a Nurse is on duty)";
}

void AssJob::success(sGirl& girl) {
    ss << "The surgery is a success.\n";
    if (girl.gain_trait( "Great Arse"))
    {
        ss << "Thanks to the surgery she now has a Great Arse.\n";
        ss << "\n \nShe has been released from the Clinic.";
        girl.FullJobReset(JOB_RESTING);
    }
}

double AssJob::GetPerformance(const sGirl& girl, bool estimate) const {
    if (girl.has_active_trait("Great Arse")) return -1000;    // X - not needed
    return 200;
}

struct TubesTied : public SurgeryJob {
    TubesTied();
    sJobValidResult is_job_valid(const sGirl& girl) const override;
    void success(sGirl& girl) override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
};

TubesTied::TubesTied(): SurgeryJob(JOB_TUBESTIED, "TTid", {"${name} is in the Clinic to get her tubes tied.", 5,
                                    {{"Sterile", "${name} is already Sterile."}}}) {
    m_Info.Description = "She will undergo surgery to make her sterile.\n*(Takes up to 5 days, less if a Nurse is on duty)";
}

sJobValidResult TubesTied::is_job_valid(const sGirl& girl) const {
    if (girl.is_pregnant()) {
        return {false, "${name} is pregnant.\nShe must either have her baby or get an abortion before she can get her Tubes Tied."};
    }
    return SurgeryJob::is_job_valid(girl);
}

void TubesTied::success(sGirl& girl) {
    ss << "The surgery is a success.\n";
    girl.FullJobReset(JOB_RESTING);
    ss << cGirls::AdjustTraitGroupFertility(girl, -10, false);
}

double TubesTied::GetPerformance(const sGirl& girl, bool estimate) const {
    if (girl.has_active_trait("Sterile"))        return -1000;    // X - not needed
    if (girl.is_pregnant())                    return 0;        // E - needs abortion or birth first
    if (girl.has_active_trait("Broodmother"))    return 200;        // A
    if (girl.has_active_trait("Fertile"))        return 100;        // B
    return 150;                                                    // C
}

struct Fertility: public SurgeryJob {
    Fertility();
    sJobValidResult is_job_valid(const sGirl& girl) const override;
    void success(sGirl& girl) override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
};

Fertility::Fertility(): SurgeryJob(JOB_FERTILITY, "FrtT", {"${name} is in the Clinic to get fertility treatment.", 5,
                                    {{"Broodmother", "${name} is already as Fertile as she can be."}}}) {
    m_Info.Description = "She will undergo surgery to make her fertile.\n*(Takes up to 5 days, less if a Nurse is on duty)";
}

sJobValidResult Fertility::is_job_valid(const sGirl& girl) const {
    if (girl.is_pregnant()) {
        return {false, "${name} is pregnant.\n"
                       "She must either have her baby or get an abortion before She can get receive any more fertility treatments."};
    }

    return SurgeryJob::is_job_valid(girl);
}

void Fertility::success(sGirl& girl) {
    ss << "The surgery is a success.\n";
    ss << cGirls::AdjustTraitGroupFertility(girl, 1, false);
    if (girl.has_active_trait("Broodmother"))
    {
        ss << "\n \nShe has been released from the Clinic.";
        girl.FullJobReset(JOB_RESTING);
    }
}

double Fertility::GetPerformance(const sGirl& girl, bool estimate) const {
    if (girl.has_active_trait("Broodmother"))    return -1000;    // X - not needed
    if (girl.is_pregnant())                    return 0;        // E - needs abortion or birth first
    if (girl.has_active_trait("Sterile"))        return 200;        // A - needs it to have a baby
    if (girl.has_active_trait("Fertile"))        return 100;        // C - would improve chances
    return 150;                                                    // B - would improve chances greatly
}

void RegisterSurgeryJobs(cJobManager& mgr) {
    mgr.register_job(std::make_unique<CosmeticSurgery>());
    mgr.register_job(std::make_unique<Liposuction>());
    mgr.register_job(std::make_unique<BreastReduction>());
    mgr.register_job(std::make_unique<BoobJob>());
    mgr.register_job(std::make_unique<VaginalRejuvenation>());
    mgr.register_job(std::make_unique<FaceLift>());
    mgr.register_job(std::make_unique<AssJob>());
    mgr.register_job(std::make_unique<TubesTied>());
    mgr.register_job(std::make_unique<Fertility>());
}