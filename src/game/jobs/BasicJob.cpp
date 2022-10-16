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

#include "BasicJob.h"
#include "xml/util.h"
#include "character/traits/ITraitsManager.h"
#include "character/sGirl.h"
#include "cGirls.h"
#include "CLog.h"
#include "utils/string.hpp"
#include "IGame.h"

double cBasicJob::GetPerformance(const sGirl& girl, bool estimate) const {
    if(m_Info.FreeOnly && girl.is_slave()) return -1000;
    return m_PerformanceData.eval(girl, estimate);
}

cBasicJob::cBasicJob(JOBS job, std::string xml_file) : IGenericJob(job, std::move(xml_file)), m_Interface(this) {
    RegisterVariable("Performance", m_Performance);
    RegisterVariable("Tips", m_Tips);
    RegisterVariable("Wages", m_Wages);
    RegisterVariable("Earnings", m_Earnings);
}

void cBasicJob::apply_gains(sGirl& girl, int performance) {
    m_Gains.apply(girl, performance);
}

void cBasicJob::load_from_xml_internal(const tinyxml2::XMLElement& job_data, const std::string& file_name) {
    std::string prefix = "job." + file_name;

    // Performance Criteria
    const auto* performance_el = job_data.FirstChildElement("Performance");
    if(performance_el) {
        m_PerformanceData.load(*performance_el, prefix);
    }

    // Gains
    const auto* gains_el = job_data.FirstChildElement("Gains");
    if(gains_el) {
        m_Gains.load(*gains_el);
    }

    // Modifiers
    const auto* modifiers_el = job_data.FirstChildElement("Modifiers");
    if(modifiers_el) {
        // TODO automatically prefix with the jobs name, and allow for loading "local" modifiers
        // which start with .
        g_Game->traits().load_modifiers(*modifiers_el, prefix);
    }

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

    const auto* config_el = job_data.FirstChildElement("Config");
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
    if(!m_TextRepo) return false;
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
    m_Earnings = 0;
    m_Wages = 0;
    m_Tips = 0;
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

void cBasicJobTextInterface::SetVariable(const std::string& name, std::string value) const {
    throw std::logic_error("String variables are not implemented yet");
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
        girl.AddMessage(ss.str(), EImageBaseType::REFUSE, EVENT_NOWORK);
        return eCheckWorkResult::REFUSES;
    }
    return eCheckWorkResult::ACCEPTS;
}

void cBasicJob::add_performance_text() {
    if (m_Performance >= 245)
    {
        add_text("work.perfect");
    }
    else if (m_Performance >= 185)
    {
        add_text("work.great");
    }
    else if (m_Performance >= 145)
    {
        add_text("work.good");
    }
    else if (m_Performance >= 100)
    {
        add_text("work.ok");
    }
    else if (m_Performance >= 70)
    {
        add_text("work.bad");
    }
    else
    {
        add_text("work.worst");
    }
    ss << "\n\n";
}