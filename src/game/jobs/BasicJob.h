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

#ifndef WM_BASICJOB_H
#define WM_BASICJOB_H

#include <unordered_map>

#include "GenericJob.h"
#include "JobData.h"
#include "text/repo.h"


class cBasicJob;

class cBasicJobTextInterface : public IInteractionInterface {
public:
    cBasicJobTextInterface() = delete;
    explicit cBasicJobTextInterface(cBasicJob* job) : m_Job(job) {}

    bool LookupBoolean(const std::string& name) const final;

    int LookupNumber(const std::string& name) const final;

    void TriggerEvent(const std::string& name) const final;
    void SetVariable(const std::string& name, int value) const final;

    void RegisterVariable(std::string name, int& value);
private:
    std::unordered_map<std::string, int*> m_MappedValues;
    cBasicJob* m_Job;
};


class cBasicJob : public IGenericJob {
public:
    cBasicJob(JOBS job, const char* xml_file = nullptr);
    double GetPerformance(const sGirl& girl, bool estimate) const override;

protected:
    void apply_gains(sGirl& girl, int performance);

    void load_from_xml(const char* xml_file);
    const std::string& get_text(const std::string& prompt) const;
    bool has_text(const std::string& prompt) const;
    std::stringstream& add_text(const std::string& prompt);

    // processing variables
    void InitWork() override;
    void RegisterVariable(std::string name, int& value);

    void SetSubstitution(std::string key, std::string replace);

    int m_Performance;

    // payment data
    int m_Earnings;
    int m_Wages;
    int m_Tips;

    eCheckWorkResult SimpleRefusalCheck(sGirl& girl, Action_Types action);

private:
    cJobPerformance m_PerformanceData;
    cJobGains       m_Gains;

    std::unique_ptr<ITextRepository> m_TextRepo;
    cBasicJobTextInterface m_Interface;

    void load_from_xml_internal(const char* xml_file);
    virtual void load_from_xml_callback(const tinyxml2::XMLElement& job_element) {};

    friend class cBasicJobTextInterface;

    std::unordered_map<std::string, std::string> m_Replacements;
};

#endif //WM_BASICJOB_H
