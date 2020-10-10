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

#ifndef WM_GENERICJOB_H
#define WM_GENERICJOB_H

#include <utility>
#include <vector>
#include <memory>
#include <sstream>
#include <cJobManager.h>
#include "Constants.h"
#include "TextRepo.h"
#include <boost/variant.hpp>


using StatSkill = boost::variant<STATS, SKILLS>;

class sGirl;
class cRng;

struct sTraitChange {
    // explicit constructor needed for mingw
    sTraitChange(bool g, std::string trait, int th, Action_Types a, std::string message, EventType e = EVENT_GOODNEWS) :
            Gain(g), TraitName(std::move(trait)), Threshold(th), Action(a), Message(std::move(message)), EventType(e) {}
    bool Gain;
    std::string TraitName;
    int Threshold;
    Action_Types Action;
    std::string Message;
    ::EventType EventType = EVENT_GOODNEWS;
};

struct sJobInfo {
    JOBS        JobId;
    std::string Name;
    std::string ShortName;
    std::string Description;

    bool FullTime = false;
    bool FreeOnly = false;
};

struct sJobValidResult {
    bool IsValid;
    std::string Reason;
    operator bool() const { return IsValid; }
};

class IGenericJob {
public:
    IGenericJob(JOBS j);
    virtual ~IGenericJob() noexcept = default;

    // queries
    const sJobInfo& get_info() const { return m_Info; }
    JOBS job() const { return m_Info.JobId; }

    /// Gets an estimate or actual value of how well the girl performs at this job
    virtual double GetPerformance(const sGirl& girl, bool estimate) const = 0;

    /// Checks whether the given girl can do this job.
    virtual sJobValidResult is_job_valid(const sGirl& girl) const;

    /// Lets the girl do the job
    bool Work(sGirl& girl, bool is_night, cRng& rng);

    /// called by the job manager when the job gets registered.
    void OnRegisterJobManager(const cJobManager& manager);
protected:
    std::stringstream ss;

    // random functions
    cRng& rng() { return *m_Rng; }
    int d100() const;
    bool chance(float percent) const;
    int uniform(int min, int max) const;

    enum class eCheckWorkResult {
        REFUSES,
        ACCEPTS,
        IMPOSSIBLE
    };

    const sGirl& active_girl() const;

private:
    virtual bool DoWork(sGirl& girl, bool is_night) = 0;
    virtual eCheckWorkResult CheckWork(sGirl& girl, bool is_night) = 0;

    cRng* m_Rng;
    sGirl* m_ActiveGirl;
    const cJobManager* m_JobManager;

protected:
    sJobInfo m_Info;
};

struct sJobPerformance {
    std::string            TraitMod;        //!< Name of the trait modifier
    std::vector<StatSkill> PrimaryGains;    // primary skill and stat gains
    std::vector<StatSkill> SecondaryGains;  // primary skill and stat gains
};

struct sBasicJobData {
    int XP;
    int Skill;
};

class cBasicJob : public IGenericJob {
public:
    using IGenericJob::IGenericJob;
    double GetPerformance(const sGirl& girl, bool estimate) const override;

    cBasicJob(JOBS job, const char* xml_file);

protected:

    void set_performance_data(std::string mod, std::vector<StatSkill> primary, std::vector<StatSkill> secondary);
    const sJobPerformance& get_performance_data() const { return m_PerformanceData; }

    void add_trait_chance(sTraitChange c);
    void gain_traits(sGirl& girl);
    void apply_gains(sGirl& girl);

    void load_from_xml(const char* xml_file);
    const std::string& get_text(const std::string& prompt) const;
private:
    sJobPerformance m_PerformanceData;
    std::vector<sTraitChange> m_TraitChanges;
    sBasicJobData m_Data;

    std::unique_ptr<cTextRepository> m_TextRepo;

    void load_from_xml_internal(const char* xml_file);
};

void RegisterCraftingJobs(cJobManager& mgr);
void RegisterSurgeryJobs(cJobManager& mgr);
void RegisterWrappedJobs(cJobManager& mgr);
void RegisterManagerJobs(cJobManager& mgr);
void RegisterFilmJobs(cJobManager& mgr);
void RegisterTherapyJobs(cJobManager& mgr);
void RegisterBarJobs(cJobManager& mgr);
void RegisterFarmJobs(cJobManager& mgr);
void RegisterClinicJobs(cJobManager& mgr);

#endif //WM_GENERICJOB_H
