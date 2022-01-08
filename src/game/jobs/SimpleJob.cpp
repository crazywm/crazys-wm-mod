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


#include "SimpleJob.h"
#include "cGirls.h"
#include "character/sGirl.h"
#include "xml/util.h"

sWorkJobResult cSimpleJob::DoWork(sGirl& girl, bool is_night)
{
    m_Wages = m_Data.BaseWages;
    auto brothel = girl.m_Building;
    if(has_text("work")) {
        add_text("work") << "\n\n";
    }
    cGirls::UnequipCombat(girl);  // put that shit away, you'll scare off the customers!

    return {JobProcessing(girl, *brothel, is_night), m_Tips, m_Earnings, m_Wages};
}

cSimpleJob::cSimpleJob(JOBS job, const char* xml, sSimpleJobData data) : cBasicJob(job, xml), m_Data(data) {
    RegisterVariable("Enjoyment", m_Enjoyment);
}

void cSimpleJob::HandleGains(sGirl& girl, int fame) {
    // Update Enjoyment
    girl.upd_Enjoyment(m_Data.Action, m_Enjoyment);

    if (girl.fame() < 10 && m_Performance >= 70)     { fame += 1; }
    if (girl.fame() < 20 && m_Performance >= 100)    { fame += 1; }
    if (girl.fame() < 40 && m_Performance >= 145)    { fame += 1; }
    if (girl.fame() < 60 && m_Performance >= 185)    { fame += 1; }

    girl.fame(fame);

    apply_gains(girl, m_Performance);
}

IGenericJob::eCheckWorkResult cSimpleJob::CheckWork(sGirl& girl, bool is_night) {
    return SimpleRefusalCheck(girl, m_Data.Action);
}

void cSimpleJob::load_from_xml_callback(const tinyxml2::XMLElement& job_element) {
    auto wages = job_element.FirstChildElement("WageFunction");
    if(wages) {
        m_PerformanceToEarnings = LoadLinearFunction(*wages, "Performance", "Wages");
    }
}

void cSimpleJob::shift_enjoyment() {
    ss << "\n";
    int roll = d100();
    if (roll <= 5)
    {
        ss << rng().select_text({
                                        "Some of the patrons abused her during the shift.",
                                        "Several patrons heckled her and made her shift generally unpleasant."
                                });
        m_Enjoyment -= 1;
    }
    else if (roll <= 25)
    {
        ss << "She had a pleasant time working.";
        m_Enjoyment += 3;
    }
    else
    {
        ss << "Otherwise, the shift passed uneventfully.";
        m_Enjoyment += 1;
    }

    if (m_Performance < 50)  m_Enjoyment -= 1;
    if (m_Performance < 0)   m_Enjoyment -= 1;          // if she doesn't do well at the job, she enjoys it less
    if (m_Performance > 200) m_Enjoyment *= 2;          // if she is really good at the job, her enjoyment (positive or negative) is doubled
}

int cSimpleJob::get_performance_class(int performance) {
    if (performance >= 245) { return 5; }
    else if (performance >= 185) { return 4; }
    else if (performance >= 145) { return 3; }
    else if (performance >= 100) { return 2; }
    else if (performance >= 70) { return 1; }
    else { return 0;}
}
