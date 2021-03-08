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
#include "IGame.h"
#include <sstream>
#include "character/predicates.h"

extern const char* const CarePointsBasicId;
extern const char* const CarePointsGoodId;
extern const char* const DoctorInteractionId;

namespace settings {
    extern const char* PREG_COOL_DOWN;
}

struct sTraitAndMessage {
    const char* Trait;
    const char* Message;
};

struct sSurgeryData {
    const char*  SurgeryMessage;                //!< The message used to indicate that the surgery was performed
    int          Duration;                      //!< How many days until finished.
    std::vector<sTraitAndMessage> TraitExcludes;   //!< If she has any trait in this list, she will not receive surgery
};

struct sStatGain {
    STATS Stat;
    const char* Message;
};


struct sSurgeryDataII {
    // Trait Gains
    std::vector<sTraitAndMessage> GainTrait;
    std::vector<sTraitAndMessage> LoseTrait;
    std::vector<sStatGain> GainStat;
};

class ITreatmentJob : public IGenericJob {
public:
    ITreatmentJob(JOBS job, const char* short_name) : IGenericJob(job) {
        m_Info.ShortName = short_name;
        m_Info.FullTime = true;
        m_Info.Consumes.emplace_back(DoctorInteractionId);
        m_Info.Consumes.emplace_back(CarePointsBasicId);
        m_Info.Consumes.emplace_back(CarePointsGoodId);
    }
    sWorkJobResult DoWork(sGirl& girl, bool is_night) override;

private:
    virtual void ReceiveTreatment(sGirl& girl, bool is_night) = 0;
};

sWorkJobResult ITreatmentJob::DoWork(sGirl& girl, bool is_night) {
    cGirls::UnequipCombat(girl);    // not for patient
    ReceiveTreatment(girl, is_night);
    return {false, 0, 0, 0};
}

struct SurgeryJob : public ITreatmentJob {
public:
    explicit SurgeryJob(JOBS id, const char* short_name, sSurgeryData data) : ITreatmentJob(id, short_name), m_SurgeryData(data) {
    }

    void ReceiveTreatment(sGirl& girl, bool is_night) final;
    sJobValidResult is_job_valid(const sGirl& girl) const override;
    eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;
protected:
    // common data
    sSurgeryData m_SurgeryData;

    virtual void success(sGirl& girl) = 0;

private:
    bool nursing_effect(sGirl& girl);
};

void SurgeryJob::ReceiveTreatment(sGirl& girl, bool is_night) {
    auto brothel = girl.m_Building;
    JOBS job_id = job();

    auto msgtype = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

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
            return;
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

    if (brothel->RequestInteraction(DoctorInteractionId)) {
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
        jobperformance += (100 - girl.beauty());
    }
    return jobperformance;
}

void CosmeticSurgery::success(sGirl& girl) {
    ss << "The surgery is a success.\n";

    girl.beauty(rng().bell(5, 12));


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

    // Fat
    if (girl.lose_trait( "Plump"))
    {
        ss << "Thanks to the surgery she is no longer Plump.\n";
    }
    else if (girl.lose_trait( "Fat"))
    {
        ss << "Thanks to the surgery she is no longer Fat.\n";
    }
    else if (girl.gain_trait( "Great Figure"))
    {
        ss << "Thanks to the surgery she now has a Great Figure.\n";
    }

    if (girl.has_active_trait("Great Figure"))
    {
        ss << "She has been released from the Clinic.\n \n";
        girl.FullJobReset(JOB_RESTING);
    }
}

double Liposuction::GetPerformance(const sGirl& girl, bool estimate) const {
    if (girl.is_pregnant())                        return 80;        // D - not recommended while pregnant
    if (girl.has_active_trait("Plump"))           return 400;        // I - do it
    if (girl.has_active_trait("Fat"))             return 400;        // I - do it
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
    double performance = 0.0;
    if (estimate)    // for third detail string - how much do they need this?
    {
        int values[] = {-1000, 0, 70, 100, 100, 150, 200, 250, 350, 400};
        return values[girl.breast_size() - 1];
    }
    return performance;
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


class CureDiseases : public ITreatmentJob {
public:
    CureDiseases();
private:
    void ReceiveTreatment(sGirl& girl, bool is_night) final;
    double GetPerformance(const sGirl& girl, bool estimate) const final;
    eCheckWorkResult CheckWork(sGirl& girl, bool is_night) final;
};

CureDiseases::CureDiseases() : ITreatmentJob(JOB_CUREDISEASES, "Cure") {
    m_Info.Description = "She will try to get her diseases cured.";
}

void CureDiseases::ReceiveTreatment(sGirl& girl, bool is_night) {
    auto brothel = girl.m_Building;

    // if she was not in JOB_CUREDISEASES yesterday, reset working days to 0 before proceeding
    if (girl.m_YesterDayJob != JOB_CUREDISEASES) girl.m_PrevWorkingDay = girl.m_WorkingDay = 0;
    if (girl.m_WorkingDay < 0) girl.m_WorkingDay = 0;
    girl.m_DayJob = girl.m_NightJob = JOB_CUREDISEASES;    // it is a full time job

    auto doctor = brothel->RequestInteraction(DoctorInteractionId);

    int cost = 0;
    std::vector<std::string> diseases;
    if (girl.has_active_trait("Herpes"))        { diseases.emplace_back("Herpes"); cost += 25; }
    if (girl.has_active_trait("Chlamydia"))    { diseases.emplace_back("Chlamydia"); cost += 50; }
    if (girl.has_active_trait("Syphilis"))    { diseases.emplace_back("Syphilis"); cost += 75; }
    if (girl.has_active_trait("AIDS"))        { diseases.emplace_back("AIDS"); cost += 100; }
    int num_diseases = diseases.size();

    if (num_diseases > 1)    ss << "${name} is in the Clinic to get her diseases treated";
    else/*            */    ss << "${name} is in the Clinic to get treatment for " << diseases[0];
    ss << ".\n \n";

    auto msgtype = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

    if(!doctor) {
        // do you have high-quality nursing
        if(brothel->TryConsumeResource(CarePointsGoodId, 3)) {
            ss << "There were no Doctors available ${name}, but you highly qualified nurses made sure that ${name}'s situation "
                  "did not deteriorate.";
        } else {
            ss << "There were no Doctors available ${name} just lay in bed getting sicker.";
            girl.m_WorkingDay -= uniform(1, 10);
            cost = 0;    // noone to give her the medicine
        }
    } else {
        ss << "${name} was treated by " << doctor->FullName() << ".";
        doctor->AddMessage("${name} treated " + girl.FullName() + "'s " + diseases[0],
                           IMGTYPE_NURSE, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
        int doc_pts = std::min(5, int(doctor->job_performance(JOB_DOCTOR, false) / 20));
        int nurse_pts = brothel->ConsumeResource(CarePointsBasicId, 4) + 2 * brothel->ConsumeResource(CarePointsGoodId, 2);
        if(nurse_pts == 0) {
            ss << " Your clinic has not enough Nurses to provide adequate care, so her recovery will be slower.";
        }
        girl.m_WorkingDay += doc_pts + nurse_pts;
    }

    girl.m_WorkingDay += girl.constitution() / 10 + girl.get_trait_modifier("disease.recovery");

    if (is_night && girl.m_WorkingDay >= 100)
    {
        msgtype = EVENT_GOODNEWS;
        girl.m_WorkingDay = girl.m_PrevWorkingDay = 0;

        std::string disease_cured = diseases[uniform(0, num_diseases - 1)];
        girl.lose_trait(disease_cured.c_str());
        num_diseases--;
        ss << "You pay " << cost << " gold for last dose of the medicine used in her treatment.\n \nThe treatment is a success, ${name} no longer has " << disease_cured << "!\n \n";

        if (!has_disease(girl))
        {
            ss << "She has been released from the Clinic";
            girl.FullJobReset(JOB_RESTING);
        }
        else
        {
            ss << "Her " << (doctor ? "Doctor" : "Nurse");
            ss << " keeps her in bed so they can treat her other disease" << (num_diseases > 1 ? "s" : "");
        }
    }
    else
    {
        ss << "You pay " << cost << " gold for the medicine and equipment used in her treatment";
    }
    ss << ".\n \n";

    brothel->m_Finance.clinic_costs(cost);    // pay for it

    girl.AddMessage(ss.str(), IMGTYPE_PROFILE, msgtype);
}

double CureDiseases::GetPerformance(const sGirl& girl, bool estimate) const {
    int numdiseases = 0;
    if (girl.has_active_trait("AIDS"))            numdiseases++;
    if (girl.has_active_trait("Herpes"))            numdiseases++;
    if (girl.has_active_trait("Chlamydia"))        numdiseases++;
    if (girl.has_active_trait("Syphilis"))        numdiseases++;

    if (numdiseases == 0)    return -1000;
    return numdiseases * 100;
}

auto CureDiseases::CheckWork(sGirl& girl, bool is_night) -> eCheckWorkResult {
    if (!has_disease(girl)) {
        ss << "${name} has no diseases";
        JOBS new_job = JOB_RESTING;
        if (girl.health() < 80 || girl.tiredness() > 20)
        {
            ss << ". She was not feeling well so she checked herself in for treatment instead.";
            new_job = JOB_GETHEALING;
        } else { ss << " so she was sent to the waiting room."; }
        girl.FullJobReset(new_job);
        girl.m_PrevWorkingDay = girl.m_WorkingDay = 0;
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
        return IGenericJob::eCheckWorkResult::IMPOSSIBLE;    // not refusing
    }

    return IGenericJob::eCheckWorkResult::ACCEPTS;
}


class Abortion : public ITreatmentJob {
public:
    Abortion();
private:
    void ReceiveTreatment(sGirl& girl, bool is_night) final;
    double GetPerformance(const sGirl& girl, bool estimate) const final;
    eCheckWorkResult CheckWork(sGirl& girl, bool is_night) final;
};

Abortion::Abortion() : ITreatmentJob(JOB_GETABORT, "Abrt") {
    m_Info.Description = "She will get an abortion, removing pregnancy and/or insemination.";
}

namespace
{
    void process_happiness(std::stringstream& ss, sGirl& girl, int happy, int& hate, std::array<const char*, 7> messages) {
        if (happy < -50)
        {
            ss << messages[0];
            hate += 10;
            girl.add_temporary_trait("Pessimist", 20);
        }
        else if (happy < -25)
        {
            ss << messages[1];
            hate += 5;
        }
        else if (happy < -5)
        {
            ss << messages[2];
        }
        else if (happy < 10)
        {
            ss << messages[3];
        }
        else if (happy < 25)
        {
            ss << messages[4];
        }
        else if (happy < 50)
        {
            ss << messages[5];
        }
        else
        {
            ss << messages[6];
            hate -= 5;
            girl.add_temporary_trait("Optimist", 20);
        }

        ss << "\n";
    }
}

void Abortion::ReceiveTreatment(sGirl& girl, bool is_night) {
    auto brothel = girl.m_Building;
    // if she was not in surgery last turn, reset working days to 0 before proceeding
    if (girl.m_YesterDayJob != JOB_GETABORT) { girl.m_WorkingDay = girl.m_PrevWorkingDay = 0; }
    girl.m_DayJob = girl.m_NightJob = JOB_GETABORT;    // it is a full time job

    sGirl* doctor = brothel->RequestInteraction(DoctorInteractionId);
    if (!doctor)
    {
        ss << "There is no doctor available to perform ${name}'s abortion!";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
        return;    // not refusing
    }
    ss << "${name} is in the Clinic to get an abortion.\n \n";

    auto msgtype = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

    if (!is_night)    // the Doctor works on her durring the day
    {
        girl.m_WorkingDay++;
    }
    else    // and if there are nurses on duty, they take care of her at night
    {
        if (brothel->num_girls_on_job(JOB_NURSE, 1) > 0)
        {
            girl.m_WorkingDay++;
            girl.happiness(5);
            girl.mana(5);
        }
    }

    if (girl.m_WorkingDay >= 2 && is_night)
    {
        girl.m_WorkingDay = girl.m_PrevWorkingDay = 0;
        ss << "The girl had an abortion.\n";
        msgtype = EVENT_GOODNEWS;

        // `J` first set the base stat modifiers
        int happy = -10, health = -20, mana = -20, spirit = -5, love = -5, hate = 5;

        if (brothel->TryConsumeResource(CarePointsBasicId, 3))
        {
            ss << "The Nurse tried to keep her healthy and happy during her recovery.\n";
            // `J` then adjust if a nurse helps her through it
            happy += 10;    health += 10;    mana += 10;    spirit += 5;    love += 1;    hate -= 1;
        }
        else
        {
            ss << "She is sad and has lost some health during the operation.\n";
        }

        happy += girl.get_trait_modifier("preg.abort.happy");
        love += girl.get_trait_modifier("preg.abort.love");
        hate += girl.get_trait_modifier("preg.abort.hate");

        // `J` next, check traits
        if (girl.has_active_trait("Fragile"))        // natural adj
        {
            health -= 5;
        }
        if (girl.has_active_trait("Tough"))        // natural adj
        {
            health += 5;
        }

        // `J` finally see what type of pregnancy it is and get her reaction to the abortion.
        if (girl.has_status(STATUS_PREGNANT))
        {
            process_happiness(ss, girl, happy, hate,
                              {"She is very distraught about the loss of her baby.",
                               "She is distraught about the loss of her baby.",
                               "She is sad about the loss of her baby.",
                               "She accepts that she is not ready to have her baby.",
                               "She is glad she is not going to have her baby.",
                               "She is happy she is not going to have her baby.",
                               "She is overjoyed not to be forced to carry her baby."
                              });
        }
        else if (girl.has_status(STATUS_PREGNANT_BY_PLAYER))
        {
            // `J` adjust her happiness by her hate-love for you
            happy += int(((girl.pchate() + hate) - (girl.pclove() + love)) / 2);
            if (girl.has_active_trait("Your Wife"))// "Why?"
            {
                happy -= 20;    spirit -= 1;    love -= 3;    hate += 0;
            }

            process_happiness(ss, girl, happy, hate,
                              {"She is very distraught about the loss of your baby.",
                               "She is distraught about the loss of your baby.",
                               "She is sad about the loss of your baby.",
                               "She accepts that she is not ready to have your baby.",
                               "She is glad she is not going to have your baby.",
                               "She is happy she is not going to have your baby.",
                               "She is overjoyed not to be forced to carry your hellspawn."
                              });
        }
        else if (girl.has_status(STATUS_INSEMINATED))
        {
            // `J` Some traits would react differently to non-human pregnancies.
            happy += girl.get_trait_modifier("inseminated.abort.happy");
            love += girl.get_trait_modifier("inseminated.abort.love");
            hate += girl.get_trait_modifier("inseminated.abort.hate");

            if (girl.has_active_trait("Angel"))        // "DEAR GOD, WHAT WAS THAT THING?"
            {
                spirit -= 5;    mana -= 5;
            }

            process_happiness(ss, girl, happy, hate,
                              {"She is very distraught about the loss of the creature growing inside her.",
                               "She is distraught about the loss of the creature growing inside her.",
                               "She is sad about the loss of the creature growing inside her.",
                               "She accepts that she is not ready to bring a strange creature into this world.",
                               "She is glad she is not going to have to carry that strange creature inside her.",
                               "She is happy she is not going to have to carry that strange creature inside her.",
                               "She is overjoyed not to be forced to carry that hellspawn anymore."
                              });
        }
        // `J` now apply all the stat changes and finalize the transaction
        girl.happiness(happy);
        girl.health(health);
        girl.mana(mana);
        girl.spirit(spirit);
        girl.pclove(love);
        girl.pchate(hate);

        girl.m_ChildrenCount[CHILD09_ABORTIONS]++;
        girl.clear_pregnancy();
        girl.m_PregCooldown = g_Game->settings().get_integer(settings::PREG_COOL_DOWN);
        girl.FullJobReset(JOB_RESTING);
        girl.m_WorkingDay = girl.m_PrevWorkingDay = 0;
    }
    else
    {
        ss << "The abortion is in progress (1 day remaining).";
    }

    girl.AddMessage(ss.str(), IMGTYPE_PROFILE, msgtype);

    // Improve girl
    int libido = -8;
    girl.upd_temp_stat(STAT_LIBIDO, libido);
}

double Abortion::GetPerformance(const sGirl& girl, bool estimate) const {
    if (!girl.is_pregnant())              return -1000;    // X - not needed
    if (girl.carrying_players_child())    return 1;        // E - its yours
    if (girl.carrying_monster())          return 150;      // B - Beast
    return 100;                                            // C - customer's child
}

IGenericJob::eCheckWorkResult Abortion::CheckWork(sGirl& girl, bool is_night) {
    if (!girl.is_pregnant())
    {
        ss << "${name} is not pregnant so she was sent to the waiting room.";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
        girl.FullJobReset(JOB_RESTING);
        girl.m_WorkingDay = girl.m_PrevWorkingDay = 0;
        return eCheckWorkResult::IMPOSSIBLE;    // not refusing
    }
    return eCheckWorkResult::ACCEPTS;
}


class Healing : public ITreatmentJob {
public:
    Healing();
private:
    void ReceiveTreatment(sGirl& girl, bool is_night) final;
    double GetPerformance(const sGirl& girl, bool estimate) const final;
    eCheckWorkResult CheckWork(sGirl& girl, bool is_night) final;
};

Healing::Healing() : ITreatmentJob(JOB_GETHEALING, "Heal") {
    m_Info.Description = "She will have her wounds attended.";
}

double Healing::GetPerformance(const sGirl& girl, bool estimate) const {
    double performance = 1.0;
    performance += (100 - girl.health());
    performance += (100 - girl.happiness());
    performance += girl.tiredness();
    return performance;
}

IGenericJob::eCheckWorkResult Healing::CheckWork(sGirl& girl, bool is_night) {
    return eCheckWorkResult::ACCEPTS;
}

void Healing::ReceiveTreatment(sGirl& girl, bool is_night) {
    auto brothel = girl.m_Building;

    sGirl* doctor = brothel->RequestInteraction(DoctorInteractionId);

    // `J` base recovery copied free time recovery
    int health = 10 + (girl.constitution() / 10);
    int tiredness = uniform(10, 30);    // build up as positive then apply as negative
    int happy = uniform(10, 20);
    int mana = 5 + (girl.magic() / 5);
    int libido = (girl.has_active_trait("Nymphomaniac") ? 15 : 4);

    if (doctor)
    {
        ss << " Doctor" << doctor->FullName() << " takes care of her.";
        if (girl.has_active_trait("Half-Construct") || girl.has_active_trait("Construct")) {
            health += 20;    // Less healing for constructs
        } else {
            health += 30;
        }

        if(brothel->TryConsumeResource(CarePointsBasicId, 3)) {
            health += 10;
        }
    } else {
        if(brothel->TryConsumeResource(CarePointsBasicId, 3)) {
            ss << "You don't have enough doctors on duty, so ${name} is just cared for by the nurses.";
            health += 10;
        } else {
            ss << "You don't have sufficient Doctors or Nurses on duty so ${name} just rests in her hospital bed.";
            happy -= 5;
        }
    }

    girl.upd_base_stat(STAT_HEALTH, health, false);
    girl.upd_base_stat(STAT_TIREDNESS, -tiredness, false);
    girl.happiness(happy);
    girl.mana(mana);
    girl.upd_temp_stat(STAT_LIBIDO, libido);

    // send her to the waiting room when she is healthy
    if (girl.health() > 90 && girl.tiredness() < 10)
    {
        ss << "\n \nShe has been released from the Clinic.";
        girl.m_DayJob = JOB_RESTING;
        girl.m_NightJob = JOB_RESTING;
    }

    girl.AddMessage(ss.str(), IMGTYPE_PROFILE, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
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
    mgr.register_job(std::make_unique<CureDiseases>());
    mgr.register_job(std::make_unique<Abortion>());
    mgr.register_job(std::make_unique<Healing>());
}