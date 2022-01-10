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
#include "CLog.h"
#include "cRng.h"
#include "character/sGirl.h"
#include "buildings/IBuilding.h"
#include "xml/util.h"
#include "xml/getattr.h"

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
    return m_Rng->closed_uniform(min, max);
}

IGenericJob::IGenericJob(JOBS j, std::string xml_file) :
    m_Info{j, get_job_name(j)}, m_XMLFile(std::move(xml_file))
{

}

sJobValidResult IGenericJob::is_job_valid(const sGirl& girl) const {
    if(m_Info.FreeOnly && girl.is_slave()) {
        return {false, "Slaves cannot work as " + m_Info.Name + "!"};
    }

    return {true, {}};
}

void IGenericJob::OnRegisterJobManager(const cJobManager& manager) {
    assert(m_JobManager == nullptr);
    m_JobManager = &manager;
    load_job();
}

sGirl& IGenericJob::active_girl() {
    assert(m_ActiveGirl);
    return *m_ActiveGirl;
}

const sGirl& IGenericJob::active_girl() const {
    assert(m_ActiveGirl);
    return *m_ActiveGirl;
}

bool IGenericJob::is_night_shift() const {
    return m_CurrentShift;
};

int IGenericJob::ConsumeResource(const std::string& name, int amount) {
    auto brothel = active_girl().m_Building;
    assert(brothel);
    return brothel->ConsumeResource(name, amount);
}

void IGenericJob::ProvideResource(const std::string& name, int amount) {
    auto brothel = active_girl().m_Building;
    assert(brothel);
    brothel->ProvideResource(name, amount);
}

bool IGenericJob::TryConsumeResource(const std::string& name, int amount) {
    auto brothel = active_girl().m_Building;
    assert(brothel);
    return brothel->TryConsumeResource(name, amount);
}

void IGenericJob::ProvideInteraction(const std::string& name, int amount) {
    auto brothel = active_girl().m_Building;
    assert(brothel);
    return brothel->ProvideInteraction(name, &active_girl(), amount);
}

sGirl* IGenericJob::RequestInteraction(const std::string& name) {
    auto brothel = active_girl().m_Building;
    assert(brothel);
    return brothel->RequestInteraction(name);
}

bool IGenericJob::HasInteraction(const std::string& name) const {
    auto brothel = active_girl().m_Building;
    assert(brothel);
    return brothel->HasInteraction(name);
}

void IGenericJob::load_job() {
    if(m_XMLFile.empty()) return;
    try {
        DirPath path = DirPath() << "Resources" << "Data" << "Jobs" << m_XMLFile;
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
                g_LogFile.error("jobs", "<Description> element does not contain text. File: ", m_XMLFile);
            }
        } else {
            g_LogFile.error("jobs", "<Job> element does not contain <Description>. File: ", m_XMLFile);
        }
        load_from_xml_internal(*job_data, path.str());
    } catch (std::exception& error) {
        g_LogFile.error("job", "Error loading job xml '", m_XMLFile, "': ", error.what());
        throw;
    }
}

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
DECL_JOB(Torturer);
DECL_JOB(ExploreCatacombs);

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
    REGISTER_JOB(JOB_TORTURER, Torturer, "Trtr", "She will torture the prisoners in addition to your tortures, she will also look after them to ensure they don't die. (max 1 for all brothels)").full_time().free_only();
    REGISTER_JOB(JOB_EXPLORECATACOMBS, ExploreCatacombs, "ExC", "She will explore the catacombs looking for treasure and capturing monsters and monster girls. Needless to say, this is a dangerous job.");

    // house
    REGISTER_JOB(JOB_PERSONALBEDWARMER, PersonalBedWarmer, "BdWm", "She will stay in your bed at night with you.");

    // Some pseudo-jobs
    REGISTER_JOB(JOB_INDUNGEON, NullJob, "", "She is languishing in the dungeon.");
    REGISTER_JOB(JOB_RUNAWAY, NullJob, "", "She has escaped.");
}
