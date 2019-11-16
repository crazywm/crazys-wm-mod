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
#include "character/sGirl.hpp"
#include "IBuilding.hpp"
#include <sstream>
#include "character/predicates.h"

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
public:
    explicit SurgeryJob(JOBS id, sSurgeryData data) : IGenericJob(id), m_SurgeryData(data) {}

    bool DoWork(sGirl& girl, bool is_night) final;
protected:
    // common data
    sSurgeryData m_SurgeryData;

    int NumNurses;

    void EndSurgery(sGirl& girl);

    virtual bool is_valid(sGirl& girl);
    virtual void success(sGirl& girl) = 0;
};

void SurgeryJob::EndSurgery(sGirl& girl) {
    if (NumNurses > 2)
    {
        ss << "The Nurses kept her healthy and happy during her recovery.\n";
        girl.health(rng().bell(0, 20));
        girl.happiness(rng().bell(0, 10));
        girl.spirit(rng().bell(0, 10));
        girl.mana(rng().bell(0, 20));
        girl.beauty(rng().bell(0, 2));
        girl.charisma(rng().bell(0, 2));
    }
    else if (NumNurses > 0)
    {
        ss << "The Nurse" << (NumNurses > 1 ? "s" : "") << " helped her during her recovery.\n";
        girl.health(rng().bell(0, 10));
        girl.happiness(rng().bell(0, 5));
        girl.spirit(rng().bell(0, 5));
        girl.mana(rng().bell(0, 10));
        girl.beauty(uniform(0, 2));
        girl.charisma(uniform(0, 2));
    }
    else
    {
        ss << "She is sad and has lost some health during the operation.\n";
        girl.health(rng().bell(-20, 2));
        girl.happiness(rng().bell(-10, 1));
        girl.spirit(rng().bell(-5, 1));
        girl.mana(rng().bell(-20, 3));
        girl.beauty(rng().bell(-1, 1));
        girl.charisma(rng().bell(-1, 1));
    }
}

bool SurgeryJob::DoWork(sGirl& girl, bool is_night) {
    auto brothel = girl.m_Building;
    JOBS job_id = job();

    if (girl.m_YesterDayJob != job_id) { girl.m_WorkingDay = girl.m_PrevWorkingDay = 0; }
    girl.m_DayJob = girl.m_NightJob = job_id;    // it is a full time job

    // check validity of the job
    bool valid = is_valid(girl);
    for(auto& t : m_SurgeryData.TraitExcludes) {
        if(girl.has_active_trait(t.Trait)) {
            valid = false;
            ss << t.Message;
            break;
        }
    }
    if (!valid) {
        girl.FullJobReset(JOB_CLINICREST);
        girl.m_WorkingDay = girl.m_PrevWorkingDay = 0;
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
        return false;    // not refusing
    }

    bool hasDoctor = brothel->num_girls_on_job(JOB_DOCTOR, is_night) > 0;
    NumNurses = brothel->num_girls_on_job(JOB_NURSE, is_night);
    if (!hasDoctor) {
        ss << "${name} does nothing. You don't have any Doctors working. (require 1) ";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
        return false;    // not refusing
    }
    ss << " " << m_SurgeryData.SurgeryMessage << "\n \n";

    int msgtype = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;
    cGirls::UnequipCombat(&girl);    // not for patient

    // update progress
    if (!is_night)    // the Doctor works on her during the day
    {
        girl.m_WorkingDay++;
    } else    // and if there are nurses on duty, they take care of her at night
    {
        if (NumNurses > 0) {
            girl.m_WorkingDay++;
            //!!!!!!!!! ABORTION !!!!!!!!!!
            girl.happiness(5);
            girl.mana(5);
            //!!!!!!!!! Cosmetic Surgery !!!
            girl.health(10);
            girl.happiness(10);
            girl.mana(10);
        }
    }

    // process progress
    if (girl.m_WorkingDay < m_SurgeryData.Duration || !is_night) {
        int wdays = (m_SurgeryData.Duration - girl.m_WorkingDay);

        if (NumNurses > 0)
        {
            wdays = (wdays - 1) / 2 + 1;
        }
        ss << "The operation is in progress" << " (" << wdays << " day remaining).\n";
        if (NumNurses > 1)		 { ss << "The Nurses are taking care of her at night."; }
        else if (NumNurses > 0)  { ss << "The Nurse is taking care of her at night."; }
        else					 { ss << "Having a Nurse on duty will speed up her recovery."; }
    } else {
        msgtype = EVENT_GOODNEWS;
        girl.m_WorkingDay = girl.m_PrevWorkingDay = 0;
        success(girl);
    }

    girl.AddMessage(ss.str(), IMGTYPE_PROFILE, msgtype);

    // Improve girl
    int libido = 0;  // -8 for abortion!
    if (girl.has_active_trait("Lesbian"))   libido += NumNurses;
    if (girl.has_active_trait("Masochist")) libido += 1;
    girl.upd_temp_stat(STAT_LIBIDO, libido);

    if (chance(10.f))
        girl.medicine(1);	// `J` she watched what the doctors and nurses were doing
    return false;
}

bool SurgeryJob::is_valid(sGirl& girl) {
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------

struct CosmeticSurgery: public SurgeryJob {
    CosmeticSurgery();
    void success(sGirl& girl) override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
};

CosmeticSurgery::CosmeticSurgery() : SurgeryJob(JOB_COSMETICSURGERY, {"${name} is in the Clinic to get general surgery.", 5}) {

}

double CosmeticSurgery::GetPerformance(const sGirl& girl, bool estimate) const {
    double jobperformance = 0.0;
    if (estimate)	// for third detail string - how much do they need this?
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

    EndSurgery(girl);

    if (NumNurses > 2)
    {
        girl.beauty(rng().bell(10, 20));
        girl.charisma(rng().bell(1, 10));
    }
    else if (NumNurses > 0)
    {
        girl.beauty(rng().bell(8, 15));
        girl.charisma(rng().bell(1, 5));
    }
    else
    {
        girl.beauty(rng().bell(5, 12));
        girl.charisma(rng().bell(0, 3));
    }

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
        girl.FullJobReset(JOB_CLINICREST);
    }
}

struct Liposuction: public SurgeryJob {
    Liposuction();
    void success(sGirl& girl) override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
};

Liposuction::Liposuction() : SurgeryJob(JOB_LIPO, {"${name} is in the Clinic to get fat removed.", 5,
                                        {{"Great Figure", "${name} already has a Great Figure so she was sent to the waiting room."}}}) {
}


void Liposuction::success(sGirl& girl) {
    ss << "The surgery is a success.\n";

    EndSurgery(girl);

    if (NumNurses > 2)
    {
        girl.beauty(rng().bell(5, 20));
        girl.charisma(rng().bell(1, 6));
    }
    else if (NumNurses > 0)
    {
        girl.beauty(rng().bell(2, 15));
        girl.charisma(rng().bell(1, 3));
    }
    else
    {
        girl.beauty(rng().bell(0, 10));
        girl.charisma(rng().bell(-1, 2));
    }

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
        girl.FullJobReset(JOB_CLINICREST);
    }
}

double Liposuction::GetPerformance(const sGirl& girl, bool estimate) const {
    if (girl.is_pregnant())					    return 80;		// D - not recommended while pregnant
    if (girl.has_active_trait("Plump"))		    return 400;		// I - do it
    if (girl.has_active_trait("Great Figure"))	return -1000;	// X - not needed
    return 200;													// A - can improve
}

struct BreastReduction: public SurgeryJob {
    BreastReduction();
    void success(sGirl& girl) override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
};

BreastReduction::BreastReduction() : SurgeryJob(JOB_BREASTREDUCTION, {"${name} is in the Clinic to get her breasts reduced.", 1,
                                                 {{"Flat Chest", "${name} already has a Flat Chest so she was sent to the waiting room."}}}) {
}

void BreastReduction::success(sGirl& girl) {
    ss << "The surgery is a success.\n";

    ss << cGirls::AdjustTraitGroupBreastSize(&girl, -1, false) << "\n \n";

    EndSurgery(girl);

    if (girl.has_active_trait("Flat Chest"))
    {
        ss << "${name}'s breasts are as small as they can get so she was sent to the waiting room.";
        girl.FullJobReset(JOB_CLINICREST);
    }
}

double BreastReduction::GetPerformance(const sGirl& girl, bool estimate) const {
    double jobperformance = 0.0;
    if (estimate)	// for third detail string - how much do they need this?
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

BoobJob::BoobJob() : SurgeryJob(JOB_BOOBJOB, {"${name} is in the Clinic to get her breasts enlarged.", 1,
                                 {{"Titanic Tits", "${name} already has Titanic Tits so she was sent to the waiting room."}}}) {

}

void BoobJob::success(sGirl& girl) {
    ss << "The surgery is a success.\n";

    ss << cGirls::AdjustTraitGroupBreastSize(&girl, 1, false) << "\n \n";

    EndSurgery(girl);

    if (girl.has_active_trait("Titanic Tits"))
    {
        ss << "${name}'s breasts are as large as they can get so she was sent to the waiting room.";
        girl.FullJobReset(JOB_CLINICREST);
    }
}

double BoobJob::GetPerformance(const sGirl& girl, bool estimate) const {
    double jobperformance = 0.0;
    if (estimate)	// for third detail string - how much do they need this?
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

VaginalRejuvenation::VaginalRejuvenation() : SurgeryJob(JOB_VAGINAREJUV, {"${name} is in the Clinic to get her vagina tightened.", 5,
                                                         {{"Virgin", "${name} is already a Virgin so she was sent to the waiting room."}}}) {
}

void VaginalRejuvenation::success(sGirl& girl) {
    ss << "The surgery is a success.\nShe is a 'Virgin' again.\n";

    EndSurgery(girl);

    girl.gain_trait("Virgin");
    girl.FullJobReset(JOB_CLINICREST);
    ss << "\n \nShe has been released from the Clinic.";
}

double VaginalRejuvenation::GetPerformance(const sGirl& girl, bool estimate) const {
    if (is_virgin(girl))		    return -1000;	// X - not needed
    if (girl.is_pregnant())			return 80;		// D - is her name Mary?
    return 400;											// I - needs it
}

struct FaceLift: public SurgeryJob {
    FaceLift();
    bool is_valid(sGirl& girl) override;
    void success(sGirl& girl) override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
};

FaceLift::FaceLift() : SurgeryJob(JOB_FACELIFT, {"${name} is in the Clinic to get a face lift.", 5}) {

}

bool FaceLift::is_valid(sGirl& girl) {
    if (girl.age() <= 21)
    {
        ss << "${name} is too young to get a Face Lift so she was sent to the waiting room.";
        return false;
    }
    return true;
}

void FaceLift::success(sGirl& girl) {
    ss << "The surgery is a success.\nShe looks a few years younger.\n";
    EndSurgery(girl);

    if (NumNurses > 2)
    {
        girl.beauty(rng().bell(8, 16));
        girl.charisma(rng().bell(0, 2));
        girl.age(rng().bell(-4, -1));
    }
    else if (NumNurses > 0)
    {
        girl.beauty(rng().bell(6, 12));
        girl.charisma(uniform(0, 2));
        girl.age(rng().bell(-3, -1));
    }
    else
    {
        girl.beauty(rng().bell(4, 10));
        girl.charisma(rng().bell(-1, 1));
        girl.age(rng().bell(-2, -1));
    }

    if (girl.age() <= 18) girl.set_stat(STAT_AGE, 18);
    if (girl.age() <= 21)
    {
        ss << "\n \nShe has been released from the Clinic.";
        girl.FullJobReset(JOB_CLINICREST);
    }
}

double FaceLift::GetPerformance(const sGirl& girl, bool estimate) const {
    double jobperformance = 0.0;
    if (estimate)	// for third detail string - how much do they need this?
    {
        if (girl.age() <= 21)	return -1000;			// X - not needed
        if (girl.age() == 100) return 0;				// E - unknown age?

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

AssJob::AssJob(): SurgeryJob(JOB_ASSJOB, {"${name} is in the Clinic to get her ass worked on.", 5,
                              {{"Great Arse", "${name} already has a Great Arse so she was sent to the waiting room."}}}) {

}

void AssJob::success(sGirl& girl) {
    ss << "The surgery is a success.\n";
    EndSurgery(girl);
    if (girl.gain_trait( "Great Arse"))
    {
        ss << "Thanks to the surgery she now has a Great Arse.\n";
        ss << "\n \nShe has been released from the Clinic.";
        girl.FullJobReset(JOB_CLINICREST);
    }
}

double AssJob::GetPerformance(const sGirl& girl, bool estimate) const {
    if (girl.has_active_trait("Great Arse")) return -1000;	// X - not needed
    return 200;
}

struct TubesTied : public SurgeryJob {
    TubesTied();
    bool is_valid(sGirl& girl) override;
    void success(sGirl& girl) override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
};

TubesTied::TubesTied(): SurgeryJob(JOB_TUBESTIED, {"${name} is in the Clinic to get her tubes tied.", 5,
                                    {{"Sterile", "${name} is already Sterile so she was sent to the waiting room."}}}) {
}

bool TubesTied::is_valid(sGirl& girl) {
    if (girl.is_pregnant()) {
        ss << "${name} is pregnant.\nShe must either have her baby or get an abortion before She can get her Tubes Tied.";
    } else {
        return true;
    }

    return false;
}

void TubesTied::success(sGirl& girl) {
    ss << "The surgery is a success.\n";
    EndSurgery(girl);
    girl.FullJobReset(JOB_CLINICREST);
    ss << cGirls::AdjustTraitGroupFertility(&girl, -10, false);
}

double TubesTied::GetPerformance(const sGirl& girl, bool estimate) const {
    if (girl.has_active_trait("Sterile"))		return -1000;	// X - not needed
    if (girl.is_pregnant())					return 0;		// E - needs abortion or birth first
    if (girl.has_active_trait("Broodmother"))	return 200;		// A
    if (girl.has_active_trait("Fertile"))		return 100;		// B
    return 150;													// C
}

struct Fertility: public SurgeryJob {
    Fertility();
    bool is_valid(sGirl& girl) override;
    void success(sGirl& girl) override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
};

Fertility::Fertility(): SurgeryJob(JOB_FERTILITY, {"${name} is in the Clinic to get fertility treatment.", 5,
                                    {{"Broodmother", "${name} is already as Fertile as she can be so she was sent to the waiting room."}}}) {

}

bool Fertility::is_valid(sGirl& girl) {
    if (girl.is_pregnant()) {
        ss << "${name} is pregnant.\nShe must either have her baby or get an abortion before She can get receive any more fertility treatments.";
    } else {
        return true;
    }

    return false;
}

void Fertility::success(sGirl& girl) {
    ss << "The surgery is a success.\n";
    EndSurgery(girl);
    ss << cGirls::AdjustTraitGroupFertility(&girl, 1, false);
    if (girl.has_active_trait("Broodmother"))
    {
        ss << "\n \nShe has been released from the Clinic.";
        girl.FullJobReset(JOB_CLINICREST);
    }
}

double Fertility::GetPerformance(const sGirl& girl, bool estimate) const {
    if (girl.has_active_trait("Broodmother"))	return -1000;	// X - not needed
    if (girl.is_pregnant())					return 0;		// E - needs abortion or birth first
    if (girl.has_active_trait("Sterile"))		return 200;		// A - needs it to have a baby
    if (girl.has_active_trait("Fertile"))		return 100;		// C - would improve chances
    return 150;													// B - would improve chances greatly
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