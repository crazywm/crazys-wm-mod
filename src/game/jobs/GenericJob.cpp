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
#include <boost/throw_exception.hpp>
#include <CLog.h>
#include "xml/util.h"
#include "xml/getattr.h"
#include "utils/string.hpp"
#include "cGirls.h"
#include "cRng.h"
#include "character/sGirl.h"
#include "IGame.h"
#include "character/traits/ITraitsManager.h"

class sBrothel;


sWorkJobResult IGenericJob::Work(sGirl& girl, bool is_night, cRng& rng) {
    ss.str("");
    m_Rng = &rng;
    m_ActiveGirl = &girl;
    m_CurrentShift = is_night;

    if(m_Info.FullTime && girl.m_DayJob != girl.m_NightJob) {
        g_LogFile.error("jobs", "Full time job was assigned for a single shift!");
        girl.m_DayJob = job();
        girl.m_NightJob = job();
    }

    if(is_night) {
        girl.m_Refused_To_Work_Night = false;
    } else {
        girl.m_Refused_To_Work_Day = false;
    }

    InitWork();
    switch (CheckWork(girl, is_night)) {
        case eCheckWorkResult::ACCEPTS:
            return DoWork(girl, is_night);
        case eCheckWorkResult::REFUSES:
            if(is_night) {
                girl.m_Refused_To_Work_Night = true;
            } else {
                girl.m_Refused_To_Work_Day = true;
            }
            return {true, 0, 0, 0};
        case eCheckWorkResult::IMPOSSIBLE:
            return {false, 0, 0, 0};
    }
    assert(false);
}

int IGenericJob::d100() const {
    return m_Rng->d100();
}

bool IGenericJob::chance(float percent) const {
    return m_Rng->percent(percent);
}

int IGenericJob::uniform(int min_, int max_) const {
    int min = std::min(min_, max_);
    int max = std::max(min_, max_);
    return m_Rng->in_range(min, max + 1);
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
    cJobWrapper(JOBS j, std::function<sWorkJobResult(sGirl&, bool, cRng&)> w, std::function<double(const sGirl&, bool)> p,
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
    sWorkJobResult DoWork(sGirl& girl, bool is_night) override {
        return m_Work(girl, is_night, rng());
    }

    std::function<sWorkJobResult(sGirl&, bool, cRng&)> m_Work;
    std::function<double(const sGirl&, bool)> m_Perf;
};


// `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >> cJobManager.h > class cJobManager

#define DECL_JOB(Fn) sWorkJobResult Work##Fn(sGirl& girl, bool Day0Night1, cRng& rng); \
double JP_##Fn(const sGirl& girl, bool estimate)

// - General
DECL_JOB(Freetime);
DECL_JOB(Security);
DECL_JOB(Advertising);
DECL_JOB(CustService);
DECL_JOB(Torturer);
DECL_JOB(ExploreCatacombs);
DECL_JOB(BeastCare);

// - Bar
DECL_JOB(Escort);

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

//Comunity Centre
DECL_JOB(Counselor);

// house
DECL_JOB(PersonalBedWarmer);

namespace {
    sWorkJobResult WorkNullJob(sGirl&, bool, cRng&) { return {false}; }
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

// Counseling Centre
    REGISTER_JOB(JOB_COUNSELOR, Counselor, "Cnsl", "She will help girls get over their addictions and problems.").full_time().free_only();

// house
    REGISTER_JOB(JOB_PERSONALBEDWARMER, PersonalBedWarmer, "BdWm", "She will stay in your bed at night with you.");

    // Some pseudo-jobs
    REGISTER_JOB(JOB_INDUNGEON, NullJob, "", "She is languishing in the dungeon.");
    REGISTER_JOB(JOB_RUNAWAY, NullJob, "", "She has escaped.");
}

double cBasicJob::GetPerformance(const sGirl& girl, bool estimate) const {
    return m_PerformanceData.eval(girl, estimate);
}

cBasicJob::cBasicJob(JOBS job, const char* xml_file) : IGenericJob(job), m_Interface(this) {
    if(xml_file) {
        load_from_xml(xml_file);
    }

    RegisterVariable("Performance", m_Performance);
}

void cBasicJob::apply_gains(sGirl& girl, int performance) {
    m_Gains.apply(girl, performance);
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

    std::string prefix = "job." + std::string(xml_file);

    // Performance Criteria
    const auto* performance_el = job_data->FirstChildElement("Performance");
    if(performance_el) {
        m_PerformanceData.load(*performance_el, prefix);
    }

    // Gains
    const auto* gains_el = job_data->FirstChildElement("Gains");
    if(gains_el) {
        m_Gains.load(*gains_el);
    }

    // Modifiers
    const auto* modifiers_el = job_data->FirstChildElement("Modifiers");
    if(modifiers_el) {
        // TODO automatically prefix with the jobs name, and allow for loading "local" modifiers
        // which start with .
        g_Game->traits().load_modifiers(*modifiers_el, prefix);
    }

    // Texts
    const auto* text_el = job_data->FirstChildElement("Messages");
    if(text_el) {
        m_TextRepo = ITextRepository::create();
        m_TextRepo->load(*text_el);
    }
    const auto* config_el = job_data->FirstChildElement("Config");
    if(config_el) {
        load_from_xml_callback(*config_el);
    }
}

const std::string& cBasicJob::get_text(const std::string& prompt) const {
    assert(m_TextRepo);
    try {
        return m_TextRepo->get_text(prompt, m_Interface);
    } catch (const std::out_of_range& oor) {
        g_LogFile.error("job", "Trying to get missing text '", prompt, "\' in job ", m_Info.Name);
        throw;
    }
}

bool cBasicJob::has_text(const std::string& prompt) const {
    return m_TextRepo->has_text(prompt);
}

std::stringstream& cBasicJob::add_text(const std::string& prompt) {
    auto& tpl = get_text(prompt);
    interpolate_string(ss, tpl, [&](const std::string& var) -> std::string {
        if(var == "name") {
            return active_girl().FullName();
        } else if (var == "shift") {
            return is_night_shift() ? "night" : "day";
        } else if (m_Replacements.count(var) != 0) {
            return m_Replacements.at(var);
        }
        assert(false);
    }, rng());
    return ss;
}

void cBasicJob::SetSubstitution(std::string key, std::string replace) {
    m_Replacements[key] = replace;
}

void cBasicJob::InitWork() {
    m_Performance = GetPerformance(active_girl(), false);
}

void cBasicJob::RegisterVariable(std::string name, int& value) {
    m_Interface.RegisterVariable(std::move(name), value);
}

bool cBasicJobTextInterface::LookupBoolean(const std::string& name) const {
    return m_Job->active_girl().has_active_trait(name.c_str());
}

int cBasicJobTextInterface::LookupNumber(const std::string& name) const {
    auto split_point = name.find(':');
    auto type = name.substr(0, split_point);
    if(type == "stat") {
        return m_Job->active_girl().get_stat(get_stat_id(name.substr(split_point+1)));
    } else if(type == "skill") {
        return m_Job->active_girl().get_skill(get_skill_id(name.substr(split_point+1)));
    } else if (type.size() == name.size()) {
        try {
            return *m_MappedValues.at(name);
        } catch (const std::out_of_range& oor) {
            g_LogFile.error("job", "Unknown job variable '", name, '\'');
            BOOST_THROW_EXCEPTION(std::runtime_error("Unknown job variable: " + name));
        }
    } else {
        g_LogFile.error("job", "Unknown value category ", type, " of variable ", name);
        BOOST_THROW_EXCEPTION(std::runtime_error("Unknown value category: " + type));
    }
}

void cBasicJobTextInterface::SetVariable(const std::string& name, int value) const {
    int* looked_up = m_MappedValues.at(name);
    *looked_up = value;
}

void cBasicJobTextInterface::TriggerEvent(const std::string& name) const {
    throw std::logic_error("Event triggers are not implemented yet");
}

void cBasicJobTextInterface::RegisterVariable(std::string name, int& value) {
    m_MappedValues[std::move(name)] = &value;
}

IGenericJob::eCheckWorkResult cBasicJob::SimpleRefusalCheck(sGirl& girl, Action_Types action) {
    if (girl.disobey_check(action, job()))
    {
        add_text("refuse");
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return eCheckWorkResult::REFUSES;
    }
    return eCheckWorkResult::ACCEPTS;
}