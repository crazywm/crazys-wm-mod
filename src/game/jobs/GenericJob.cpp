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
#include <CLog.h>
#include "xml/util.h"
#include "xml/getattr.h"
#include "utils/string.hpp"
#include "cGirls.h"
#include "cRng.h"
#include "character/sGirl.h"
#include "TextRepo.h"

class sBrothel;


bool IGenericJob::Work(sGirl& girl, bool is_night, cRng& rng) {
    ss.str("");
    m_Rng = &rng;
    m_ActiveGirl = &girl;
    m_CurrentShift = is_night;

    if(m_Info.FullTime && girl.m_DayJob != girl.m_NightJob) {
        g_LogFile.error("jobs", "Full time job was assigned for a single shift!");
    }

    switch (CheckWork(girl, is_night)) {
        case eCheckWorkResult::ACCEPTS:
            return DoWork(girl, is_night);
        case eCheckWorkResult::REFUSES:
            return true;
        case eCheckWorkResult::IMPOSSIBLE:
            return false;
    }
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

IGenericJob::IGenericJob(JOBS j) : m_Info{j, get_job_name(j)} {}

sJobValidResult IGenericJob::is_job_valid(const sGirl& girl) const {
    if(m_Info.FreeOnly && girl.is_slave()) {
        return {false, "Slaves cannot work as " + m_Info.Name + "!"};
    }

    return {true, {}};
}

void IGenericJob::OnRegisterJobManager(const cJobManager& manager) {
    assert(m_JobManager == nullptr);
    m_JobManager = &manager;
}

const sGirl& IGenericJob::active_girl() const {
    assert(m_ActiveGirl);
    return *m_ActiveGirl;
}

bool IGenericJob::is_night_shift() const {
    return m_CurrentShift;
};

class cJobWrapper: public IGenericJob {
public:
    cJobWrapper(JOBS j, std::function<bool(sGirl&, bool, cRng&)> w, std::function<double(const sGirl&, bool)> p,
                std::string brief, std::string desc) :
            IGenericJob(j), m_Work(std::move(w)), m_Perf(std::move(p)) {
        m_Info.ShortName = std::move(brief);
        m_Info.Description = std::move(desc);
    }

    cJobWrapper& full_time() { m_Info.FullTime = true; return *this; };
    cJobWrapper& free_only() { m_Info.FreeOnly = true; return *this; };

    eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override {
        if(!is_job_valid(girl)) {
            return eCheckWorkResult::IMPOSSIBLE;
        }
        return eCheckWorkResult::ACCEPTS;
    }
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

namespace {
  bool WorkNullJob(sGirl&, bool, cRng&) { return false; }
  double JP_NullJob(const sGirl&, bool) { return 0.0; }
}

#define REGISTER_JOB_MANUAL(J, Wf, Pf, Brief, Desc)                                     \
    [&]() -> auto& {                                                                    \
    auto new_job = std::make_unique<cJobWrapper>(J, Work##Wf, JP_##Pf, Brief, Desc);    \
    auto ptr = new_job.get();                                                           \
    mgr.register_job(std::move(new_job));                                               \
    return *ptr;                                                                        \
    }()
#define REGISTER_JOB(J, Fn, Brief, Desc) REGISTER_JOB_MANUAL(J, Fn, Fn, Brief, Desc)


void RegisterWrappedJobs(cJobManager& mgr) {
    REGISTER_JOB(JOB_RESTING, Freetime, "TOff", "She will take some time off, maybe do some shopping or walk around town. If the girl is unhappy she may try to escape.");
    REGISTER_JOB(JOB_CLEANING, Cleaning, "Prtc", "She will train either alone or with others to improve her skills.");
    REGISTER_JOB(JOB_TRAINING, Training, "Cln", "She will clean the building, as filth will put off some customers.");
    REGISTER_JOB(JOB_SECURITY, Security, "Sec", "She will patrol the building, stopping mis-deeds.");
    REGISTER_JOB(JOB_ADVERTISING, Advertising, "Adv", "She will advertise the building's features in the city.");
    REGISTER_JOB(JOB_CUSTOMERSERVICE, CustService, "CS", "She will look after customer needs.");
    REGISTER_JOB(JOB_TORTURER, Torturer, "Trtr", "She will torture the prisoners in addition to your tortures, she will also look after them to ensure they don't die. (max 1 for all brothels)").full_time().free_only();
    REGISTER_JOB(JOB_EXPLORECATACOMBS, ExploreCatacombs, "ExC", "She will explore the catacombs looking for treasure and capturing monsters and monster girls. Needless to say, this is a dangerous job.");
    REGISTER_JOB(JOB_BEASTCARER, BeastCare, "BstC", "She will look after the needs of the beasts in your Brothel.");

    REGISTER_JOB(JOB_ESCORT, Escort, "Scrt", "She will be an escort.");

    REGISTER_JOB(JOB_DEALER, HallDealer, "Dlr", "She will manage a game in the gambling hall.");
    REGISTER_JOB(JOB_ENTERTAINMENT, HallEntertainer, "Entn", "She will provide entertainment to the customers.");
    REGISTER_JOB(JOB_XXXENTERTAINMENT, HallXXXEntertainer, "XXXE", "She will provide sexual entertainment to the customers.");

    REGISTER_JOB(JOB_SLEAZYBARMAID, SleazyBarmaid, "SBmd", "She will staff the bar and serve drinks while dressed in lingerie or fetish costumes.");
    REGISTER_JOB(JOB_SLEAZYWAITRESS, SleazyWaitress, "SWtr", "She will bring drinks and food to the customers at the tables while dressed in lingerie or fetish costumes.");
    REGISTER_JOB(JOB_BARSTRIPPER, BarStripper, "SSrp", "She will strip on the tables and stage for the customers.");
    REGISTER_JOB(JOB_MASSEUSE, BrothelMasseuse, "Msus", "She will give massages to the customers.");
    REGISTER_JOB(JOB_BROTHELSTRIPPER, BrothelStripper, "BStp", "She will strip for the customers.");
    REGISTER_JOB(JOB_PEEP, PeepShow, "Peep",  "She will let people watch her change and maybe more...");

    REGISTER_JOB(JOB_WHOREGAMBHALL, Whore, "HWhr", "She will give her sexual favors to the customers.");
    REGISTER_JOB(JOB_BARWHORE, Whore, "SWhr", "She will provide sex to the customers.");
    REGISTER_JOB(JOB_WHOREBROTHEL, Whore, "BWhr", "She will whore herself to customers within the building's walls. This is safer but a little less profitable.");
    REGISTER_JOB(JOB_WHORESTREETS, Whore, "StWr", "She will whore herself on the streets. It is more dangerous than whoring inside but more profitable.");

// - Movie Studio - Crew
    REGISTER_JOB(JOB_DIRECTOR, FilmDirector, "Dir", "She directs the filming, and keeps the girls in line. (max 1)").free_only();
    REGISTER_JOB(JOB_PROMOTER, FilmPromoter, "Prmt", "She advertises the movies. (max 1)");
    REGISTER_JOB(JOB_CAMERAMAGE, CameraMage, "CM", "She will film the scenes. (requires at least 1 to create a scene)");
    REGISTER_JOB(JOB_CRYSTALPURIFIER, CrystalPurifier, "CP",  "She will clean up the filmed scenes. (requires at least 1 to create a scene)");
    REGISTER_JOB(JOB_FLUFFER, Fluffer, "Fluf", "She will keep the porn stars aroused.");
    REGISTER_JOB_MANUAL(JOB_STAGEHAND, FilmStagehand, Cleaning, "StgH", "She helps setup equipment, and keeps the studio clean.");

    //Rand
    REGISTER_JOB(JOB_FILMRANDOM, FilmRandom, "FRnd", "She will perform in a random sex scene.");

// - Arena - Fighting
    REGISTER_JOB(JOB_FIGHTBEASTS, FightBeast, "FiBs", "She will fight to the death against beasts you own. Dangerous.");
    REGISTER_JOB(JOB_FIGHTARENAGIRLS, FightArenaGirls, "Cage", "She will fight against other girls. Dangerous.");
    REGISTER_JOB(JOB_FIGHTTRAIN, CombatTraining, "CT", "She will practice combat.");

// - Arena - Staff
    REGISTER_JOB(JOB_CITYGUARD, CityGuard, "CGrd", "She will help keep Crossgate safe.").free_only();
    REGISTER_JOB_MANUAL(JOB_CLEANARENA, CleanArena, Cleaning, "GKpr", "She will clean the arena.");

//Comunity Centre
    REGISTER_JOB(JOB_FEEDPOOR, FeedPoor, "Feed", "She will work in a soup kitchen.");
    REGISTER_JOB(JOB_COMUNITYSERVICE, ComunityService, "CmSr", "She will go around town and help out where she can.");
    REGISTER_JOB_MANUAL(JOB_CLEANCENTRE, CleanCentre, Cleaning, "ClnC", "She will clean the centre.");

// Counseling Centre
    REGISTER_JOB(JOB_COUNSELOR, Counselor, "Cnsl", "She will help girls get over their addictions and problems.").full_time().free_only();

// - Clinic - Surgery
    REGISTER_JOB(JOB_GETHEALING, Healing, "Heal", "She will have her wounds attended.").full_time();
    REGISTER_JOB(JOB_GETREPAIRS, RepairShop, "Repr", "Construct girls will be quickly repaired here.").full_time();
    REGISTER_JOB(JOB_CUREDISEASES, CureDiseases, "Cure", "She will try to get her diseases cured.").full_time();
    REGISTER_JOB(JOB_GETABORT, GetAbort, "Abrt", "She will get an abortion, removing pregnancy and/or insemination.\n*(Takes 2 days or 1 if a Nurse is on duty)").full_time();

// house
    REGISTER_JOB(JOB_RECRUITER, Recruiter, "Rcrt", "She will go out and try and recruit girls for you.").full_time();
    REGISTER_JOB(JOB_PERSONALTRAINING, PersonalTraining, "PTrn", "You will oversee her training personally.");
    REGISTER_JOB(JOB_PERSONALBEDWARMER, PersonalBedWarmer, "BdWm", "She will stay in your bed at night with you.");
    //REGISTER_JOB(JOB_HOUSEVAC, HouseVacation);
    REGISTER_JOB_MANUAL(JOB_CLEANHOUSE, CleanHouse, Cleaning, "ClnH", "She will clean your house.");
    REGISTER_JOB(JOB_HOUSECOOK, HouseCook, "Hcok", "She will cook for your house.");
    REGISTER_JOB(JOB_HOUSEPET, HousePet, "Hpet", "She will be trained to become the house pet.").full_time();
    REGISTER_JOB(JOB_SO_STRAIGHT, SOStraight, "SOSt", "You will make sure she only likes having sex with men.").full_time();
    REGISTER_JOB(JOB_SO_BISEXUAL, SOBisexual, "SOBi", "You will make sure she likes having sex with both men and women.").full_time();
    REGISTER_JOB(JOB_SO_LESBIAN, SOLesbian, "SOLe", "You will make sure she only likes having sex with women.").full_time();
    REGISTER_JOB(JOB_FAKEORGASM, FakeOrgasm, "FOEx", "You will teach her how to fake her orgasms.").full_time();

    // Some pseudo-jobs
    REGISTER_JOB(JOB_INDUNGEON, NullJob, "", "She is languishing in the dungeon.");
    REGISTER_JOB(JOB_RUNAWAY, NullJob, "", "She has escaped.");
}

double cBasicJob::GetPerformance(const sGirl& girl, bool estimate) const {
    return m_PerformanceData.eval(girl, estimate);
}

cBasicJob::cBasicJob(JOBS job, const char* xml_file) : IGenericJob(job) {
    load_from_xml(xml_file);
}

void cBasicJob::apply_gains(sGirl& girl) {
    m_Gains.apply(girl);
}

void cBasicJob::load_from_xml(const char* xml_file) {
    try {
        load_from_xml_internal(xml_file);
    } catch (std::exception& error) {
        g_LogFile.error("job", "Error loading job xml '", xml_file, "': ", error.what());
        throw;
    }
}

void cBasicJob::load_from_xml_internal(const char* xml_file) {
    DirPath path = DirPath() << "Resources" << "Data" << "Jobs" << xml_file;
    auto doc = LoadXMLDocument(path.c_str());
    auto job_data = doc->FirstChildElement("Job");
    if(!job_data) {
        throw std::runtime_error("Job xml does not contain <Job> element!");
    }

    // Performance Criteria
    const auto* performance_el = job_data->FirstChildElement("Performance");
    if(performance_el) {
        m_PerformanceData.load(*performance_el);
    }

    // Gains
    const auto* gains_el = job_data->FirstChildElement("Gains");
    if(gains_el) {
        m_Gains.load(*gains_el);
    }

    // Info
    m_Info.ShortName = GetStringAttribute(*job_data, "ShortName");
    if(const auto* desc_el = job_data->FirstChildElement("Description")) {
        if(const char* description = desc_el->GetText()) {
            m_Info.Description = description;
        } else {
            g_LogFile.error("jobs", "<Description> element does not contain text. File: ", xml_file);
        }
    } else {
        g_LogFile.error("jobs", "<Job> element does not contain <Description>. File: ", xml_file);
    }

    // Texts
    const auto* text_el = job_data->FirstChildElement("Messages");
    if(text_el) {
        m_TextRepo = std::make_unique<cTextRepository>();
        m_TextRepo->load(*text_el);
    }
}

const std::string& cBasicJob::get_text(const std::string& prompt) const {
    assert(m_TextRepo);
    return m_TextRepo->get_text(prompt, [this](const std::string& c) -> bool {
        return active_girl().has_active_trait(c.c_str());
    });
}

std::stringstream& cBasicJob::add_text(const std::string& prompt) {
    auto& tpl = get_text(prompt);
    interpolate_string(ss, tpl, [&](const std::string& var) -> std::string {
        if(var == "name") {
            return active_girl().FullName();
        } else if (var == "shift") {
            return is_night_shift() ? "night" : "day";
        }
        assert(false);
        }, rng());
    return ss;
}
