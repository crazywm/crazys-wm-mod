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
#include <functional>
#include "cGirls.h"
#include "cRng.h"
#include "character/sGirl.h"

class sBrothel;


bool IGenericJob::Work(sGirl& girl, bool is_night, cRng& rng) {
    ss.str("");
    m_Rng = &rng;
    return DoWork(girl, is_night);
}

int IGenericJob::d100() const {
    return m_Rng->d100();
}

bool IGenericJob::chance(float percent) const {
    return m_Rng->percent(percent);
}

int IGenericJob::uniform(int min, int max) const {
    return m_Rng->in_range(min, max);
}

class cJobWrapper: public IGenericJob {
public:
    cJobWrapper(JOBS j, std::function<bool(sGirl&, bool, cRng&)> w, std::function<double(const sGirl&, bool)> p) :
            IGenericJob(j), m_Work(std::move(w)), m_Perf(std::move(p)) {}
private:
    double GetPerformance(const sGirl& girl, bool estimate) const override { return m_Perf(girl, estimate); }
    bool DoWork(sGirl& girl, bool is_night) override { return m_Work(girl, is_night, rng()); }

    std::function<bool(sGirl&, bool, cRng&)> m_Work;
    std::function<double(const sGirl&, bool)> m_Perf;
};


// `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >> cJobManager.h > class cJobManager

#define DECL_JOB(Fn) bool Work##Fn(sGirl& girl, bool Day0Night1, cRng& rng); \
double JP_##Fn(const sGirl& girl, bool estimate)

// - General
DECL_JOB(Freetime);
DECL_JOB(Cleaning);
DECL_JOB(Training);
DECL_JOB(Security);
DECL_JOB(Advertising);
DECL_JOB(CustService);
DECL_JOB(Torturer);
DECL_JOB(ExploreCatacombs);
DECL_JOB(BeastCare);

// - Bar
DECL_JOB(Barmaid);
DECL_JOB(BarWaitress);
DECL_JOB(BarSinger);
DECL_JOB(BarPiano);
DECL_JOB(Escort);
DECL_JOB(BarCook);

// - Gambling Hall
DECL_JOB(HallDealer);
DECL_JOB(HallEntertainer);
DECL_JOB(HallXXXEntertainer);

// - Sleazy Bar
DECL_JOB(SleazyBarmaid);
DECL_JOB(SleazyWaitress);
DECL_JOB(BarStripper);

// - Brothel
DECL_JOB(BrothelMasseuse);
DECL_JOB(BrothelStripper);
DECL_JOB(PeepShow);
DECL_JOB(Whore);

// - Movie Studio
DECL_JOB(FilmDirector);
DECL_JOB(FilmPromoter);
DECL_JOB(CameraMage);
DECL_JOB(CrystalPurifier);
DECL_JOB(Fluffer);
DECL_JOB(FilmStagehand);
DECL_JOB(FilmRandom);

// - Arena - Fighting
DECL_JOB(FightBeast);
DECL_JOB(FightArenaGirls);
DECL_JOB(CombatTraining);
DECL_JOB(ArenaJousting);
DECL_JOB(ArenaRacing);

// - Arena - Staff
DECL_JOB(CityGuard);
bool WorkCleanArena(sGirl& girl, bool Day0Night1, cRng& rng);

//Comunity Centre
DECL_JOB(FeedPoor);
DECL_JOB(ComunityService);
bool WorkCleanCentre(sGirl& girl, bool Day0Night1, cRng& rng);
DECL_JOB(Counselor);

// - Clinic - Surgery
DECL_JOB(Healing);
DECL_JOB(RepairShop);
DECL_JOB(CureDiseases);
DECL_JOB(GetAbort);

// - Clinic - Staff
DECL_JOB(Doctor);
DECL_JOB(Nurse);
DECL_JOB(Mechanic);
DECL_JOB(Intern);
bool WorkJanitor(sGirl& girl, bool Day0Night1, cRng& rng);

// - Farm - Staff
DECL_JOB(FarmVeterinarian);
DECL_JOB(FarmMarketer);
DECL_JOB(FarmResearch);
DECL_JOB(FarmHand);

// - Farm - Laborers
DECL_JOB(Farmer);
DECL_JOB(Shepherd);
DECL_JOB(Rancher);
DECL_JOB(CatacombRancher);
DECL_JOB(BeastCapture);
DECL_JOB(Milker);
DECL_JOB(Milk);

// house
DECL_JOB(Recruiter);
DECL_JOB(PersonalTraining);
DECL_JOB(PersonalBedWarmer);
DECL_JOB(HouseVacation);
bool WorkCleanHouse(sGirl& girl, bool Day0Night1, cRng& rng);
DECL_JOB(HouseCook);
DECL_JOB(HousePet);
bool WorkFarmPonyGirl(sGirl& girl, bool Day0Night1, cRng& rng);
DECL_JOB(SOStraight);
DECL_JOB(SOBisexual);
DECL_JOB(SOLesbian);
DECL_JOB(FakeOrgasm);

#define REGISTER_JOB_MANUAL(J, Wf, Pf) mgr.register_job(std::make_unique<cJobWrapper>(J, Work##Wf, JP_##Pf))
#define REGISTER_JOB(J, Fn) REGISTER_JOB_MANUAL(J, Fn, Fn)

void RegisterWrappedJobs(cJobManager& mgr) {
    REGISTER_JOB(JOB_RESTING, Freetime);
    REGISTER_JOB(JOB_CLEANING, Cleaning);
    REGISTER_JOB(JOB_TRAINING, Training);
    REGISTER_JOB(JOB_SECURITY, Security);
    REGISTER_JOB(JOB_ADVERTISING, Advertising);
    REGISTER_JOB(JOB_CUSTOMERSERVICE, CustService);
    REGISTER_JOB(JOB_TORTURER, Torturer);
    REGISTER_JOB(JOB_EXPLORECATACOMBS, ExploreCatacombs);
    REGISTER_JOB(JOB_BEASTCARER, BeastCare);

    REGISTER_JOB(JOB_ESCORT, Escort);

    REGISTER_JOB(JOB_DEALER, HallDealer);
    REGISTER_JOB(JOB_ENTERTAINMENT, HallEntertainer);
    REGISTER_JOB(JOB_XXXENTERTAINMENT, HallXXXEntertainer);

    REGISTER_JOB(JOB_SLEAZYBARMAID, SleazyBarmaid);
    REGISTER_JOB(JOB_SLEAZYWAITRESS, SleazyWaitress);
    REGISTER_JOB(JOB_BARSTRIPPER, BarStripper);
    REGISTER_JOB(JOB_MASSEUSE, BrothelMasseuse);
    REGISTER_JOB(JOB_BROTHELSTRIPPER, BrothelStripper);
    REGISTER_JOB(JOB_PEEP, PeepShow);

    REGISTER_JOB(JOB_WHOREGAMBHALL, Whore);
    REGISTER_JOB(JOB_BARWHORE, Whore);
    REGISTER_JOB(JOB_WHOREBROTHEL, Whore);
    REGISTER_JOB(JOB_WHORESTREETS, Whore);

// - Movie Studio - Crew
    REGISTER_JOB(JOB_FILMFREETIME, Freetime);
    REGISTER_JOB(JOB_DIRECTOR, FilmDirector);
    REGISTER_JOB(JOB_PROMOTER, FilmPromoter);
    REGISTER_JOB(JOB_CAMERAMAGE, CameraMage);
    REGISTER_JOB(JOB_CRYSTALPURIFIER, CrystalPurifier);
    REGISTER_JOB(JOB_FLUFFER, Fluffer);
    REGISTER_JOB_MANUAL(JOB_STAGEHAND, FilmStagehand, Cleaning);

    //Rand
    REGISTER_JOB(JOB_FILMRANDOM, FilmRandom);

// - Arena - Fighting
    REGISTER_JOB(JOB_FIGHTBEASTS, FightBeast);
    REGISTER_JOB(JOB_FIGHTARENAGIRLS, FightArenaGirls);
    REGISTER_JOB(JOB_FIGHTTRAIN, CombatTraining);

// - Arena - Staff
    REGISTER_JOB(JOB_ARENAREST, Freetime);
    REGISTER_JOB(JOB_CITYGUARD, CityGuard);
    REGISTER_JOB_MANUAL(JOB_CLEANARENA, CleanArena, Cleaning);

//Comunity Centre
    REGISTER_JOB(JOB_CENTREREST, Freetime);
    REGISTER_JOB(JOB_FEEDPOOR, FeedPoor);
    REGISTER_JOB(JOB_COMUNITYSERVICE, ComunityService);
    REGISTER_JOB_MANUAL(JOB_CLEANCENTRE, CleanCentre, Cleaning);

// Counseling Centre
    REGISTER_JOB(JOB_COUNSELOR, Counselor);

// - Clinic - Surgery
    REGISTER_JOB(JOB_GETHEALING, Healing);
    REGISTER_JOB(JOB_GETREPAIRS, RepairShop);
    REGISTER_JOB(JOB_CUREDISEASES, CureDiseases);
    REGISTER_JOB(JOB_GETABORT, GetAbort);

// - Clinic - Staff
    REGISTER_JOB(JOB_CLINICREST, Freetime);
    REGISTER_JOB(JOB_DOCTOR, Doctor);
    REGISTER_JOB(JOB_NURSE, Nurse);
    REGISTER_JOB(JOB_MECHANIC, Mechanic);
    REGISTER_JOB(JOB_INTERN, Intern);
    REGISTER_JOB_MANUAL(JOB_JANITOR, Janitor, Cleaning);

// - Farm - Staff
    REGISTER_JOB(JOB_FARMREST, Freetime);
    REGISTER_JOB(JOB_VETERINARIAN, FarmVeterinarian);
    REGISTER_JOB(JOB_MARKETER, FarmMarketer);
    REGISTER_JOB(JOB_RESEARCH, FarmResearch);
    REGISTER_JOB_MANUAL(JOB_FARMHAND, FarmHand, Cleaning);

// - Farm - Laborers
    REGISTER_JOB(JOB_FARMER, Farmer);
    REGISTER_JOB(JOB_SHEPHERD, Shepherd);
    REGISTER_JOB(JOB_RANCHER, Rancher);
    REGISTER_JOB(JOB_CATACOMBRANCHER, CatacombRancher);
    REGISTER_JOB(JOB_BEASTCAPTURE, BeastCapture);
    REGISTER_JOB(JOB_MILKER, Milker);
    REGISTER_JOB(JOB_MILK, Milk);

// house
    REGISTER_JOB(JOB_HOUSEREST, Freetime);
    REGISTER_JOB(JOB_RECRUITER, Recruiter);
    REGISTER_JOB(JOB_PERSONALTRAINING, PersonalTraining);
    REGISTER_JOB(JOB_PERSONALBEDWARMER, PersonalBedWarmer);
    //REGISTER_JOB(JOB_HOUSEVAC, HouseVacation);
    REGISTER_JOB_MANUAL(JOB_CLEANHOUSE, CleanHouse, Cleaning);
    REGISTER_JOB(JOB_HOUSECOOK, HouseCook);
    REGISTER_JOB(JOB_HOUSEPET, HousePet);
    REGISTER_JOB(JOB_SO_STRAIGHT, SOStraight);
    REGISTER_JOB(JOB_SO_BISEXUAL, SOBisexual);
    REGISTER_JOB(JOB_SO_LESBIAN, SOLesbian);
    REGISTER_JOB(JOB_FAKEORGASM, FakeOrgasm);
}

double cBasicJob::GetPerformance(const sGirl& girl, bool estimate) const {
    double jobperformance = 0.0;
    for(auto& att : m_PerformanceData.PrimaryGains) {
        jobperformance += girl.get_attribute(att) / (double)m_PerformanceData.PrimaryGains.size();
    }

    for(auto& att : m_PerformanceData.SecondaryGains) {
        jobperformance += girl.get_attribute(att) / (double)m_PerformanceData.SecondaryGains.size();
    }

    if(m_PerformanceData.PrimaryGains.size() == 0 || m_PerformanceData.SecondaryGains.size() == 0)
        jobperformance *= 2;

    jobperformance += girl.level();

    if (!estimate)
    {
        int t = girl.tiredness() - 80;
        if (t > 0)
            jobperformance -= (t + 2) * (t / 3);
    }

    jobperformance += girl.get_trait_modifier(m_PerformanceData.TraitMod);

    return jobperformance;
}

void cBasicJob::set_performance_data(const char* mod, std::vector<StatSkill> primary, std::vector<StatSkill> secondary) {
    m_PerformanceData = sJobPerformance{mod, std::move(primary), std::move(secondary)};
}

void cBasicJob::add_trait_chance(sTraitChange c) {
    m_TraitChanges.push_back(c);
}

void cBasicJob::gain_traits(sGirl& girl) {
    for(auto& trait : m_TraitChanges) {
        if(trait.Gain) {
            cGirls::PossiblyGainNewTrait(girl, trait.TraitName, trait.Threshold, trait.Action,
                                         trait.Message, false, trait.EventType);
        } else {
            cGirls::PossiblyLoseExistingTrait(girl, trait.TraitName, trait.Threshold, trait.Action,
                                              trait.Message, false);
        }
    }
}
