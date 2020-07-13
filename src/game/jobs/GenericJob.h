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
#include <boost/variant.hpp>


using StatSkill = boost::variant<STATS, SKILLS>;

class sGirl;
class cRng;

struct sTraitChange {
    // explicit constructor needed for mingw
    constexpr sTraitChange(bool g, const char* tn, int th, Action_Types a, const char* m, int e = EVENT_GOODNEWS) :
            Gain(g), TraitName(tn), Threshold(th), Action(a), Message(m), EventType(e) {}
    bool Gain;
    const char* TraitName;
    int Threshold;
    Action_Types Action;
    const char* Message;
    int EventType = EVENT_GOODNEWS;
};

class IGenericJob {
public:
    IGenericJob(JOBS j) : m_JobId(j) {}
    virtual ~IGenericJob() noexcept = default;
    virtual double GetPerformance(const sGirl& girl, bool estimate) const = 0;
    bool Work(sGirl& girl, bool is_night, cRng& rng);
    JOBS job() const { return m_JobId; }
protected:
    std::stringstream ss;

    // random functions
    cRng& rng() { return *m_Rng; }
    int d100() const;
    bool chance(float percent) const;
    int uniform(int min, int max) const;
private:
    virtual bool DoWork(sGirl& girl, bool is_night) = 0;

    cRng* m_Rng;
    JOBS m_JobId;
};

struct sJobPerformance {
    const char*  TraitMod;          //!< Name of the trait modifier
    std::vector<StatSkill> PrimaryGains;    // primary skill and stat gains
    std::vector<StatSkill> SecondaryGains;  // primary skill and stat gains
};

class cBasicJob : public IGenericJob {
public:
    using IGenericJob::IGenericJob;
    double GetPerformance(const sGirl& girl, bool estimate) const;

protected:

    void set_performance_data(const char* mod, std::vector<StatSkill> primary, std::vector<StatSkill> secondary);

private:
    sJobPerformance m_PerformanceData;
};

void RegisterCraftingJobs(cJobManager& mgr);
void RegisterSurgeryJobs(cJobManager& mgr);
void RegisterWrappedJobs(cJobManager& mgr);
void RegisterManagerJobs(cJobManager& mgr);
void RegisterFilmJobs(cJobManager& mgr);
void RegisterTherapyJobs(cJobManager& mgr);

#endif //WM_GENERICJOB_H
