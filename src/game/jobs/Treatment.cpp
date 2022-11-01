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

#include "Treatment.h"
#include "character/sGirl.h"
#include "text/repo.h"
#include "IGame.h"
#include "CLog.h"
#include "cGirls.h"
#include "TextInterface.h"
#include "utils/string.hpp"
#include <tinyxml2.h>

ITreatmentJob::ITreatmentJob(JOBS job, std::string xml_file) : IGenericJob(job, std::move(xml_file), EJobClass::TREATMENT),
    m_Interface(std::make_unique<cJobTextInterface>(this)) {
    m_Info.FullTime = true;
}

ITreatmentJob::~ITreatmentJob() = default;

void ITreatmentJob::load_from_xml_internal(const tinyxml2::XMLElement& job_data, const std::string& file_name) {
    // Texts
    const auto* text_el = job_data.FirstChildElement("Messages");
    if(text_el) {
        m_TextRepo = ITextRepository::create();
        m_TextRepo->load(*text_el);
        if(!m_TextRepo->verify()) {
            g_LogFile.error("jobs", "Detected some problems when loading ", file_name);
            g_Game->error("Detected some problems when loading " + file_name);
        }
    }

    const auto* config_el = job_data.FirstChildElement(specific_config_element());
    if(config_el) {
        load_from_xml_callback(*config_el);
    }
}

const std::string& ITreatmentJob::get_text(const std::string& prompt) const {
    assert(m_TextRepo);
    try {
        return m_TextRepo->get_text(prompt, *m_Interface);
    } catch (const std::out_of_range& oor) {
        g_LogFile.error("job", "Trying to get missing text '", prompt, "\' in job ", m_Info.Name);
        throw;
    }
}

bool ITreatmentJob::has_text(const std::string& prompt) const {
    if(!m_TextRepo) return false;
    return m_TextRepo->has_text(prompt);
}

std::stringstream& ITreatmentJob::add_text(const std::string& prompt) {
    auto& tpl = get_text(prompt);
    interpolate_string(ss, tpl, [&](const std::string& var) -> std::string {
        if(var == "name") {
            return active_girl().FullName();
        }
        assert(false);
    }, rng());
    return ss;
}

sWorkJobResult ITreatmentJob::DoWork(sGirl& girl, bool is_night) {
    cGirls::UnequipCombat(girl);    // not for patient
    if(girl.get_active_treatment() != job()) {
        girl.start_treatment(job());
    }
    ReceiveTreatment(girl, is_night);
    return {false, 0, 0, 0};
}
