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

#ifndef WM_IGENERICJOB_H
#define WM_IGENERICJOB_H

#include <vector>
#include <memory>
#include <sstream>
#include "cJobManager.h"
#include "Constants.h"
#include <boost/variant.hpp>

namespace tinyxml2 {
    class XMLElement;
}


using StatSkill = boost::variant<STATS, SKILLS>;

class sGirl;
class cRng;

struct sJobInfo {
    JOBS        JobId;
    std::string Name;
    std::string ShortName;
    std::string Description;

    bool FullTime = false;
    bool FreeOnly = false;

    std::vector<std::string> Consumes;
    std::vector<std::string> Provides;
};

struct sJobValidResult {
    bool IsValid;
    std::string Reason;
    explicit operator bool() const { return IsValid; }
};

class IGenericJob {
public:
    enum class EJobClass {
        REGULAR_JOB, TREATMENT
    };
    explicit IGenericJob(JOBS j, std::string xml_file = {}, EJobClass job_class = EJobClass::REGULAR_JOB);
    virtual ~IGenericJob() noexcept = default;

    // queries
    const sJobInfo& get_info() const { return m_Info; }
    JOBS job() const { return m_Info.JobId; }

    /// Gets an estimate or actual value of how well the girl performs at this job
    virtual double GetPerformance(const sGirl& girl, bool estimate) const = 0;

    /// Checks whether the given girl can do this job.
    virtual sJobValidResult is_job_valid(const sGirl& girl) const;

    /// Handles simple pre-shift setup, before any actual jobs are run.
    /// Note: This function cannot handle any
    /// stateful job processing. Multiple `PreShift` calls for different
    /// girls might happen before the corresponding `Work` calls.
    virtual void PreShift(sGirl& girl, bool is_night, cRng& rng) const {};

    /// Lets the girl do the job
    sWorkJobResult Work(sGirl& girl, bool is_night, cRng& rng);

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

    sGirl& active_girl();
    const sGirl& active_girl() const;
    bool is_night_shift() const;

    // resources
    //  bulk resources
    /// Consumes up to `amount` points of the given resource. If less is available,
    /// that amount will we consumed. Returns the amount of actual consumption.
    int ConsumeResource(const std::string& name, int amount);

    /// Provides `amount` points of the given resource.
    void ProvideResource(const std::string& name, int amount);

    /// Tries to consume `amount` of the given resource. If not enough is available,
    /// no resource is consumed and false is returned.
    bool TryConsumeResource(const std::string& name, int amount);

    //  one-on-one interactions
    void ProvideInteraction(const std::string& name, int amount);
    sGirl* RequestInteraction(const std::string& name);

    bool HasInteraction(const std::string& name) const;

private:
    virtual void InitWork() {}
    virtual sWorkJobResult DoWork(sGirl& girl, bool is_night) = 0;

    /*! Checks whether the girl will work. There are two reasons why she might not:
        She could refuse, or the job could not be possible because of external
        circumstances. This function should report which reason applies.
    */
    virtual eCheckWorkResult CheckWork(sGirl& girl, bool is_night) = 0;

    cRng* m_Rng;
    sGirl* m_ActiveGirl;
    bool m_CurrentShift;

    const cJobManager* m_JobManager = nullptr;

    /// If the job has specified an xml file, this function will load the job data from there. If no file is
    /// specified, nothing happens.
    /// Note: Since this may call virtual functions, we cannot do this in the constructor.
    /// Therefore, this is called when the job is registered to the JobManager
    void load_job();
    virtual void load_from_xml_internal(const tinyxml2::XMLElement& source, const std::string& file_name) { };
    std::string m_XMLFile;
    EJobClass m_JobClass;
protected:
    sJobInfo m_Info;

    friend class cJobTextInterface;
};

#endif //WM_IGENERICJOB_H
