/*
 * Copyright 2022, The Pink Petal Development Team.
 * The Pink Petal Development Team are defined as the game's coders
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

#ifndef WM_TREATMENT_H
#define WM_TREATMENT_H

#include "IGenericJob.h"

class ITextRepository;
class cJobTextInterface;

class ITreatmentJob : public IGenericJob {
public:
    ITreatmentJob(JOBS job, std::string xml_file);
    ~ITreatmentJob() override;
    sWorkJobResult DoWork(sGirl& girl, bool is_night) override;

protected:
    const std::string& get_text(const std::string& prompt) const;
    bool has_text(const std::string& prompt) const;
    std::stringstream& add_text(const std::string& prompt);

private:
    virtual const char* specific_config_element() const { return nullptr; }
    void load_from_xml_internal(const tinyxml2::XMLElement& job_data, const std::string& file_name) override;

    // For reading the custom data in the <Treatment> tag
    virtual void load_from_xml_callback(const tinyxml2::XMLElement& job_element) {};

    virtual void ReceiveTreatment(sGirl& girl, bool is_night) = 0;

    std::unique_ptr<ITextRepository> m_TextRepo;
    std::unique_ptr<cJobTextInterface> m_Interface;
};

#endif //WM_TREATMENT_H
