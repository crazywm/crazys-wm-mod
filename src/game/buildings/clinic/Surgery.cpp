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

#include "jobs/IGenericJob.h"
#include "jobs/Treatment.h"
#include "cJobManager.h"
#include "cGirls.h"
#include "character/sGirl.h"
#include "buildings/IBuilding.h"
#include "IGame.h"
#include <sstream>
#include <utility>
#include "character/predicates.h"
#include "text/repo.h"
#include "CLog.h"
#include "xml/getattr.h"
#include "xml/util.h"

extern const char* const CarePointsBasicId;
extern const char* const CarePointsGoodId;
extern const char* const DoctorInteractionId;

namespace settings {
    extern const char* PREG_COOL_DOWN;
}

struct sTraitAndMessage {
    std::string Trait;
    std::string Message;
};

struct sSurgeryData {
    int Duration = 0;                               //!< How many days until finished.
    std::vector<sTraitAndMessage> TraitExcludes;    //!< If she has any trait in this list, she will not receive surgery
};

class IMedicalJob : public ITreatmentJob {
public:
    IMedicalJob(JOBS job, std::string xml_file) : ITreatmentJob(job, std::move(xml_file)) {
        m_Info.Consumes.emplace_back(DoctorInteractionId);
        m_Info.Consumes.emplace_back(CarePointsBasicId);
        m_Info.Consumes.emplace_back(CarePointsGoodId);
    }
    void PreShift(sGirl& girl, bool is_night, cRng& rng) const override;
};

void IMedicalJob::PreShift(sGirl& girl, bool is_night, cRng& rng) const {
    // check validity of the job
    auto valid = is_job_valid(girl);
    if (!valid) {
        girl.FullJobReset(JOB_RESTING);
        girl.AddMessage(valid.Reason + " She was sent to the waiting room.", EImageBaseType::PROFILE, EVENT_WARNING);
    }
}

struct SurgeryJob : public IMedicalJob {
public:
    explicit SurgeryJob(JOBS id, std::string xml_file) : IMedicalJob(id, std::move(xml_file)) {
    }

    void ReceiveTreatment(sGirl& girl, bool is_night) final;
    sJobValidResult is_job_valid(const sGirl& girl) const override;
    eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;
protected:
    // common data
    sSurgeryData m_SurgeryData;

    virtual void success(sGirl& girl) = 0;

    const char* specific_config_element() const override { return "Surgery"; }
    void load_from_xml_callback(const tinyxml2::XMLElement& job_element) override;
private:
    bool nursing_effect(sGirl& girl);
};

void SurgeryJob::load_from_xml_callback(const tinyxml2::XMLElement& job_element) {
    m_SurgeryData.Duration = GetIntAttribute(job_element, "Duration");
    for (auto& exclude_el : IterateChildElements(job_element, "TraitExclude")) {
        m_SurgeryData.TraitExcludes.emplace_back(sTraitAndMessage{GetStringAttribute(exclude_el, "Trait"),
                                                                  exclude_el.GetText()});
    }
}

void SurgeryJob::ReceiveTreatment(sGirl& girl, bool is_night) {
    auto msgtype = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

    add_text("surgery") << "\n\n";

    // update progress
    if (!is_night)    // the Doctor works on her during the day
    {
        if(girl.health() < 50) {
            int bp = uniform(4, 7);
            if(TryConsumeResource(CarePointsBasicId, bp)) {
                int req_gp = uniform(2, 4);
                if(TryConsumeResource(CarePointsGoodId, req_gp)) {
                    add_text("too-sick.quality-nurses");
                    girl.health( uniform(5, 10) );
                    girl.happiness(1);
                } else {
                    add_text("too-sick.basic-nurses");
                    girl.health( uniform(3, 5) );
                }
            } else {
                add_text("too-sick.no-nurses");
                girl.health( uniform(1, 3) );
                girl.make_treatment_progress( -uniform(2, 8) );
            }
            girl.tiredness(-uniform(5, 10));
            if(girl.strength() > 50) {
                girl.strength(-uniform(0, 2));
            }
            girl.AddMessage(ss.str(), EImageBaseType::PROFILE, msgtype);
            return;
        }

        sGirl* doctor = RequestInteraction(DoctorInteractionId);
        girl.make_treatment_progress(g_Dice.closed_uniform(90, 110) / m_SurgeryData.Duration);
        nursing_effect(girl);
        doctor->AddMessage(girl.Interpolate(get_text("doctor.work")), EImageBaseType::NURSE, EventType::EVENT_DAYSHIFT);
        doctor->exp(5);
    } else    // and if there are nurses on duty, they take care of her at night
    {
        int req_gp = uniform(3, 6);
        if(TryConsumeResource(CarePointsGoodId, req_gp)) {
            ConsumeResource(CarePointsBasicId, req_gp);
            girl.make_treatment_progress(g_Dice.closed_uniform(3, 8));
            add_text("recovery.great");
            girl.health( g_Dice.closed_uniform(25, 50) / m_SurgeryData.Duration );
        } else {
            if(!nursing_effect(girl) && chance(50)) {
                girl.make_treatment_progress(-g_Dice.closed_uniform(5, 10));
                add_text("recovery.complications");
            }
        }
    }

    // process progress
    if (girl.get_treatment_progress() < 100 && !is_night) {
        ss << "\nThe operation is in progress" << " (" << girl.get_treatment_progress() << "%).\n";
    } else if(girl.get_treatment_progress() >= 100 && is_night) {
        msgtype = EVENT_GOODNEWS;
        girl.finish_treatment();
        ss << "\n";
        add_text("surgery.success") << "\n";
        success(girl);
    }

    girl.AddMessage(ss.str(), EImageBaseType::PROFILE, msgtype);

    // Improve girl
    int libido = 0;
    if (girl.has_active_trait(traits::MASOCHIST)) libido += 1;
    girl.upd_temp_stat(STAT_LIBIDO, libido);

    if (chance(10.f)) {
        // `J` she watched what the doctors and nurses were doing
        girl.gain_attribute(SKILL_MEDICINE, 0, 2, 15);
    }
}

IGenericJob::eCheckWorkResult SurgeryJob::CheckWork(sGirl& girl, bool is_night) {
    if (!HasInteraction(DoctorInteractionId)) {
        // calling request-interaction because we still want to count how many interactions where requested.
        RequestInteraction(DoctorInteractionId);
        int dp = girl.m_Building->GetInteractionProvided(DoctorInteractionId);
        if(dp > 0) {
            ss << "${name} does nothing. You don't have enough Doctors working. All " << dp << " Doctor Interactions have already been used up.";
        } else {
            ss << "${name} does nothing. You don't have any Doctors working.";
        }

        girl.AddMessage(ss.str(), EImageBaseType::PROFILE, EVENT_WARNING);
        return IGenericJob::eCheckWorkResult::IMPOSSIBLE;    // not refusing
    }

    return IGenericJob::eCheckWorkResult::ACCEPTS;
}

sJobValidResult SurgeryJob::is_job_valid(const sGirl& girl) const {
    for(auto& t : m_SurgeryData.TraitExcludes) {
        if(girl.has_active_trait(t.Trait.c_str())) {
            return {false, t.Message};
        }
    }
    return IGenericJob::is_job_valid(girl);
}

bool SurgeryJob::nursing_effect(sGirl& girl) {
    int health_dmg = uniform(4, 7);
    health_dmg -= ConsumeResource(CarePointsBasicId, health_dmg);

    if(health_dmg > 0) {
        if(girl.gain_trait(traits::SMALL_SCARS, 2)) {
            add_text("surgery.lack-of-nurses.scars");
            girl.health(-10);
            girl.tiredness(10);
            girl.happiness(-10);
            girl.strength(-uniform(0, 5));
            girl.spirit(-2);
        } else {
            add_text("surgery.lack-of-nurses.health");
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

CosmeticSurgery::CosmeticSurgery() : SurgeryJob(JOB_COSMETICSURGERY, "CosmeticSurgery.xml") {
}

double CosmeticSurgery::GetPerformance(const sGirl& girl, bool estimate) const {
    double jobperformance = 0.0;
    if (estimate)    // for third detail string - how much do they need this?
    {
        if (!girl.has_active_trait(traits::SEXY_AIR)) jobperformance += 100;
        if (!girl.has_active_trait(traits::CUTE))     jobperformance += 100;
        jobperformance += (100 - girl.beauty());
    }
    return jobperformance;
}

void CosmeticSurgery::success(sGirl& girl) {
    girl.beauty(rng().bell(5, 12));

    if (girl.gain_trait(traits::SEXY_AIR))
    {
        ss << "She gains Sexy Air trait.\n";
    }
    else if (girl.gain_trait( traits::CUTE))
    {
        ss << "She gains Cute trait.\n";
    }

    if (girl.has_active_trait(traits::SEXY_AIR) && girl.has_active_trait(traits::CUTE) && girl.beauty() > 99)
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

Liposuction::Liposuction() : SurgeryJob(JOB_LIPO, "Liposuction.xml") {
}


void Liposuction::success(sGirl& girl) {
    // Fat
    if (girl.lose_trait( traits::PLUMP))
    {
        ss << "Thanks to the surgery she is no longer Plump.\n";
    }
    else if (girl.lose_trait( traits::FAT))
    {
        ss << "Thanks to the surgery she is no longer Fat.\n";
    }
    else if (girl.gain_trait( traits::GREAT_FIGURE))
    {
        ss << "Thanks to the surgery she now has a Great Figure.\n";
    }

    if (girl.has_active_trait(traits::GREAT_FIGURE))
    {
        ss << "She has been released from the Clinic.\n \n";
        girl.FullJobReset(JOB_RESTING);
    }
}

double Liposuction::GetPerformance(const sGirl& girl, bool estimate) const {
    if (girl.is_pregnant())                        return 80;        // D - not recommended while pregnant
    if (girl.has_active_trait(traits::PLUMP))           return 400;        // I - do it
    if (girl.has_active_trait(traits::FAT))             return 400;        // I - do it
    if (girl.has_active_trait(traits::GREAT_FIGURE))    return -1000;    // X - not needed
    return 200;                                                    // A - can improve
}

struct BreastReduction: public SurgeryJob {
    BreastReduction();
    void success(sGirl& girl) override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
};

BreastReduction::BreastReduction() : SurgeryJob(JOB_BREASTREDUCTION, "BreastReduction.xml") {
}

void BreastReduction::success(sGirl& girl) {
    ss << cGirls::AdjustTraitGroupBreastSize(girl, -1, false) << "\n \n";

    if (girl.has_active_trait(traits::FLAT_CHEST))
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

BoobJob::BoobJob() : SurgeryJob(JOB_BOOBJOB, "BoobJob.xml") {
}

void BoobJob::success(sGirl& girl) {
    ss << cGirls::AdjustTraitGroupBreastSize(girl, 1, false) << "\n \n";

    if (girl.has_active_trait(traits::TITANIC_TITS))
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

VaginalRejuvenation::VaginalRejuvenation() : SurgeryJob(JOB_VAGINAREJUV, "VaginalRejuvenation.xml") {
}

void VaginalRejuvenation::success(sGirl& girl) {
    girl.gain_trait(traits::VIRGIN);
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

FaceLift::FaceLift() : SurgeryJob(JOB_FACELIFT, "FaceLift.xml") {
}

sJobValidResult FaceLift::is_job_valid(const sGirl& girl) const {
    if (girl.age() <= 21)
    {
        return {false, "${name} is too young to get a Face Lift."};
    }
    return SurgeryJob::is_job_valid(girl);
}

void FaceLift::success(sGirl& girl) {
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

AssJob::AssJob(): SurgeryJob(JOB_ASSJOB, "AssJob.xml") {
}

void AssJob::success(sGirl& girl) {
    if (girl.gain_trait(traits::GREAT_ARSE))
    {
        ss << "Thanks to the surgery she now has a Great Arse.\n";
        ss << "\n \nShe has been released from the Clinic.";
        girl.FullJobReset(JOB_RESTING);
    }
}

double AssJob::GetPerformance(const sGirl& girl, bool estimate) const {
    if (girl.has_active_trait(traits::GREAT_ARSE)) return -1000;    // X - not needed
    return 200;
}

struct TubesTied : public SurgeryJob {
    TubesTied();
    sJobValidResult is_job_valid(const sGirl& girl) const override;
    void success(sGirl& girl) override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
};

TubesTied::TubesTied(): SurgeryJob(JOB_TUBESTIED, "TubesTied.xml") {
}

sJobValidResult TubesTied::is_job_valid(const sGirl& girl) const {
    if (girl.is_pregnant()) {
        return {false, "${name} is pregnant.\nShe must either have her baby or get an abortion before she can get her Tubes Tied."};
    }
    return SurgeryJob::is_job_valid(girl);
}

void TubesTied::success(sGirl& girl) {
    girl.FullJobReset(JOB_RESTING);
    ss << cGirls::AdjustTraitGroupFertility(girl, -10, false);
}

double TubesTied::GetPerformance(const sGirl& girl, bool estimate) const {
    if (girl.has_active_trait(traits::STERILE))        return -1000;    // X - not needed
    if (girl.is_pregnant())                    return 0;        // E - needs abortion or birth first
    if (girl.has_active_trait(traits::BROODMOTHER))    return 200;        // A
    if (girl.has_active_trait(traits::FERTILE))        return 100;        // B
    return 150;                                                    // C
}

struct Fertility: public SurgeryJob {
    Fertility();
    sJobValidResult is_job_valid(const sGirl& girl) const override;
    void success(sGirl& girl) override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
};

Fertility::Fertility(): SurgeryJob(JOB_FERTILITY, "Fertility.xml") {
}

sJobValidResult Fertility::is_job_valid(const sGirl& girl) const {
    if (girl.is_pregnant()) {
        return {false, "${name} is pregnant.\n"
                       "She must either have her baby or get an abortion before she can get receive any more fertility treatments."};
    }

    return SurgeryJob::is_job_valid(girl);
}

void Fertility::success(sGirl& girl) {
    ss << cGirls::AdjustTraitGroupFertility(girl, 1, false);
    if (girl.has_active_trait(traits::BROODMOTHER))
    {
        ss << "\n \nShe has been released from the Clinic.";
        girl.FullJobReset(JOB_RESTING);
    }
}

double Fertility::GetPerformance(const sGirl& girl, bool estimate) const {
    if (girl.has_active_trait(traits::BROODMOTHER))    return -1000;    // X - not needed
    if (girl.is_pregnant())                    return 0;        // E - needs abortion or birth first
    if (girl.has_active_trait(traits::STERILE))        return 200;        // A - needs it to have a baby
    if (girl.has_active_trait(traits::FERTILE))        return 100;        // C - would improve chances
    return 150;                                                    // B - would improve chances greatly
}


class CureDiseases : public IMedicalJob {
public:
    CureDiseases();
private:
    void ReceiveTreatment(sGirl& girl, bool is_night) final;
    double GetPerformance(const sGirl& girl, bool estimate) const final;
    eCheckWorkResult CheckWork(sGirl& girl, bool is_night) final;
    void PreShift(sGirl& girl, bool is_night, cRng& rng) const final;
};

CureDiseases::CureDiseases() : IMedicalJob(JOB_CUREDISEASES, "CureDiseases.xml") {
}

void CureDiseases::ReceiveTreatment(sGirl& girl, bool is_night) {
    auto brothel = girl.m_Building;

    auto doctor = RequestInteraction(DoctorInteractionId);

    int cost = 0;
    std::vector<std::string> diseases;
    if (girl.has_active_trait(traits::HERPES))      { diseases.emplace_back(traits::HERPES); cost += 25; }
    if (girl.has_active_trait(traits::CHLAMYDIA))   { diseases.emplace_back(traits::CHLAMYDIA); cost += 50; }
    if (girl.has_active_trait(traits::SYPHILIS))    { diseases.emplace_back(traits::SYPHILIS); cost += 75; }
    if (girl.has_active_trait(traits::AIDS))        { diseases.emplace_back(traits::AIDS); cost += 100; }
    int num_diseases = diseases.size();

    if (num_diseases > 1)    ss << "${name} is in the Clinic to get her diseases treated";
    else/*            */    ss << "${name} is in the Clinic to get treatment for " << diseases[0];
    ss << ".\n \n";

    auto msgtype = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

    if(!doctor) {
        // do you have high-quality nursing
        if(TryConsumeResource(CarePointsGoodId, 3)) {
            ss << "There were no Doctors available ${name}, but you highly qualified nurses made sure that ${name}'s situation "
                  "did not deteriorate.";
        } else {
            ss << "There were no Doctors available, so ${name} just lay in bed getting sicker.";
            girl.make_treatment_progress(-uniform(1, 10));
            cost = 0;    // noone to give her the medicine
        }
    } else {
        ss << "${name} was treated by " << doctor->FullName() << ".";
        doctor->AddMessage("${name} treated " + girl.FullName() + "'s " + diseases[0],
                           EImageBaseType::NURSE, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
        int doc_pts = std::min(5, int(doctor->job_performance(JOB_DOCTOR, false) / 20));
        int nurse_pts = ConsumeResource(CarePointsBasicId, 4) + 2 * ConsumeResource(CarePointsGoodId, 2);
        if(nurse_pts == 0) {
            ss << " Your clinic has not enough Nurses to provide adequate care, so her recovery will be slower.";
        }
        girl.make_treatment_progress(doc_pts + nurse_pts);
    }

    girl.make_treatment_progress(girl.constitution() / 10 + girl.get_trait_modifier(traits::modifiers::DISEASE_RECOVERY));

    if (is_night && girl.get_treatment_progress() >= 100)
    {
        msgtype = EVENT_GOODNEWS;
        girl.finish_treatment();

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

    girl.AddMessage(ss.str(), EImageBaseType::PROFILE, msgtype);
}

double CureDiseases::GetPerformance(const sGirl& girl, bool estimate) const {
    int diseases = num_diseases(girl);
    if (diseases == 0)    return -1000;
    return diseases * 100;
}

auto CureDiseases::CheckWork(sGirl& girl, bool is_night) -> eCheckWorkResult {
    return IGenericJob::eCheckWorkResult::ACCEPTS;
}

void CureDiseases::PreShift(sGirl& girl, bool is_night, cRng& rng) const{
    if (!has_disease(girl)) {
        std::stringstream ss;
        ss << "${name} has no diseases";
        JOBS new_job = JOB_RESTING;
        if (girl.health() < 80 || girl.tiredness() > 20)
        {
            ss << ". She was not feeling well so she checked herself in for treatment instead.";
            new_job = JOB_GETHEALING;
        } else { ss << " so she was sent to the waiting room."; }
        girl.FullJobReset(new_job);
        girl.AddMessage(ss.str(), EImageBaseType::PROFILE, EVENT_WARNING);
    }
}


class Abortion : public IMedicalJob {
public:
    Abortion();
private:
    void ReceiveTreatment(sGirl& girl, bool is_night) final;
    double GetPerformance(const sGirl& girl, bool estimate) const final;
    eCheckWorkResult CheckWork(sGirl& girl, bool is_night) final;
    void PreShift(sGirl& girl, bool is_night, cRng& rng) const final;
};

Abortion::Abortion() : IMedicalJob(JOB_GETABORT, "Abortion.xml") {
}

namespace
{
    void process_happiness(std::stringstream& ss, sGirl& girl, int happy, int& love, std::array<const char*, 7> messages) {
        if (happy < -50)
        {
            ss << messages[0];
            love -= 10;
            girl.add_temporary_trait(traits::PESSIMIST, 20);
        }
        else if (happy < -25)
        {
            ss << messages[1];
            love -= 5;
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
            love += 5;
            girl.add_temporary_trait(traits::OPTIMIST, 20);
        }

        ss << "\n";
    }
}

void Abortion::ReceiveTreatment(sGirl& girl, bool is_night) {
    auto brothel = girl.m_Building;

    sGirl* doctor = RequestInteraction(DoctorInteractionId);
    if (!doctor)
    {
        ss << "There is no doctor available to perform ${name}'s abortion!";
        girl.AddMessage(ss.str(), EImageBaseType::PROFILE, EVENT_WARNING);
        return;    // not refusing
    }
    ss << "${name} is in the Clinic to get an abortion.\n \n";

    auto msgtype = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

    if (!is_night)    // the Doctor works on her durring the day
    {
        girl.make_treatment_progress(33);
    }
    else    // and if there are nurses on duty, they take care of her at night
    {
        // TODO Consume interactions!
        if (brothel->num_girls_on_job(JOB_NURSE, 1) > 0)
        {
            girl.make_treatment_progress(25);
            girl.happiness(5);
            girl.mana(5);
        }
    }

    if (girl.get_treatment_progress() >= 100 && is_night)
    {
        ss << "The girl had an abortion.\n";
        msgtype = EVENT_GOODNEWS;

        // `J` first set the base stat modifiers
        int happy = -10, health = -20, mana = -20, spirit = -5, love = -10;

        if (TryConsumeResource(CarePointsBasicId, 3))
        {
            ss << "The Nurse tried to keep her healthy and happy during her recovery.\n";
            // `J` then adjust if a nurse helps her through it
            happy += 10;    health += 10;    mana += 10;    spirit += 5;    love += 2;
        }
        else
        {
            ss << "She is sad and has lost some health during the operation.\n";
        }

        happy += girl.get_trait_modifier(traits::modifiers::PREG_ABORT_HAPPY);
        love += girl.get_trait_modifier(traits::modifiers::PREG_ABORT_LOVE);
        spirit += girl.get_trait_modifier(traits::modifiers::PREG_ABORT_SPIRIT);

        // `J` next, check traits
        if (girl.has_active_trait(traits::FRAGILE))        // natural adj
        {
            health -= 5;
        }
        if (girl.has_active_trait(traits::TOUGH))        // natural adj
        {
            health += 5;
        }

        // `J` finally see what type of pregnancy it is and get her reaction to the abortion.
        if (girl.has_status(STATUS_PREGNANT))
        {
            process_happiness(ss, girl, happy, love,
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
            happy -= (girl.pclove() + love) / 2;
            if (girl.has_active_trait(traits::YOUR_WIFE))// "Why?"
            {
                happy -= 20;    spirit -= 1;    love -= 3;
            }

            process_happiness(ss, girl, happy, love,
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
            happy += girl.get_trait_modifier(traits::modifiers::INSEMINATED_ABORT_HAPPY);
            love += girl.get_trait_modifier(traits::modifiers::INSEMINATED_ABORT_LOVE);

            if (girl.has_active_trait(traits::ANGEL))        // "DEAR GOD, WHAT WAS THAT THING?"
            {
                spirit -= 5;    mana -= 5;
            }

            process_happiness(ss, girl, happy, love,
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

        girl.m_ChildrenCount[CHILD09_ABORTIONS]++;
        girl.clear_pregnancy();
        girl.m_PregCooldown = g_Game->settings().get_integer(settings::PREG_COOL_DOWN);
        girl.FullJobReset(JOB_RESTING);
        girl.finish_treatment();
    }
    else
    {
        ss << "The abortion is in progress (1 day remaining).";
    }

    girl.AddMessage(ss.str(), EImageBaseType::PROFILE, msgtype);

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

void Abortion::PreShift(sGirl& girl, bool is_night, cRng& rng) const {
    if (!girl.is_pregnant())
    {
        girl.AddMessage("${name} is not pregnant so she was sent to the waiting room.", EImageBaseType::PROFILE, EVENT_WARNING);
        girl.FullJobReset(JOB_RESTING);
    }
}

IGenericJob::eCheckWorkResult Abortion::CheckWork(sGirl& girl, bool is_night) {
    return eCheckWorkResult::ACCEPTS;
}


class Healing : public IMedicalJob {
public:
    Healing();
private:
    void ReceiveTreatment(sGirl& girl, bool is_night) final;
    double GetPerformance(const sGirl& girl, bool estimate) const final;
    eCheckWorkResult CheckWork(sGirl& girl, bool is_night) final;
};

Healing::Healing() : IMedicalJob(JOB_GETHEALING, "Healing.xml") {
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

    sGirl* doctor = RequestInteraction(DoctorInteractionId);

    // `J` base recovery copied free time recovery
    int health = 10 + (girl.constitution() / 10);
    int tiredness = uniform(10, 30);    // build up as positive then apply as negative
    int happy = uniform(10, 20);
    int mana = 5 + (girl.magic() / 5);
    int libido = (girl.has_active_trait(traits::NYMPHOMANIAC) ? 15 : 4);

    if (doctor)
    {
        ss << " Doctor " << doctor->FullName() << " takes care of her.";
        if (girl.has_active_trait(traits::HALF_CONSTRUCT) || girl.has_active_trait(traits::CONSTRUCT)) {
            health += 20;    // Less healing for constructs
        } else {
            health += 30;
        }

        if(TryConsumeResource(CarePointsBasicId, 3)) {
            health += 10;
        }
    } else {
        if(TryConsumeResource(CarePointsBasicId, 3)) {
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

    girl.AddMessage(ss.str(), EImageBaseType::PROFILE, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
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