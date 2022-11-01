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

#include "IGenericJob.h"
#include "JobData.h"


class cBasicJob;
class sImagePreset;
class ITextRepository;
class cJobTextInterface;

class cBasicJob : public IGenericJob {
public:
    explicit cBasicJob(JOBS job, std::string xml_file = {});
    ~cBasicJob() override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;

protected:
    void apply_gains(sGirl& girl, int performance);

    const std::string& get_text(const std::string& prompt) const;
    bool has_text(const std::string& prompt) const;
    std::stringstream& add_text(const std::string& prompt);

    void add_performance_text();

    // processing variables
    void InitWork() override;
    void RegisterVariable(std::string name, int& value);
    void RegisterVariable(std::string name, sImagePreset& preset);

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
    std::unique_ptr<cJobTextInterface> m_Interface;

    std::unordered_map<std::string, std::string> m_Replacements;
protected:
    // protected, so derived classes can call the base-class version
    void load_from_xml_internal(const tinyxml2::XMLElement& source, const std::string& file_name) override;
    virtual void load_from_xml_callback(const tinyxml2::XMLElement& job_element) {};
};

#endif //WM_BASICJOB_H
