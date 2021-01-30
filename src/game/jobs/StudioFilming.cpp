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

#include "StudioJobs.h"
#include "character/sGirl.h"
#include "cGirls.h"
#include "character/cPlayer.h"
#include "character/cCustomers.h"
#include "buildings/cMovieStudio.h"
#include "IGame.h"
#include "CLog.h"
#include "xml/getattr.h"

extern const char* const DirectorInteractionId;
extern const char* const CamMageInteractionId;
extern const char* const CrystalPurifierInteractionId;

auto cFilmSceneJob::CheckWork(sGirl& girl, bool is_night) -> eCheckWorkResult {
    if(!CheckCanWork(girl)) {
        return IGenericJob::eCheckWorkResult::IMPOSSIBLE;
    }

    if(CheckRefuseWork(girl)) {
        return IGenericJob::eCheckWorkResult::REFUSES;
    }

    return IGenericJob::eCheckWorkResult::ACCEPTS;
}

bool cFilmSceneJob::CheckCanWork(sGirl& girl) {
    auto* brothel = dynamic_cast<sMovieStudio*>(girl.m_Building);
    if(!brothel) {
        g_LogFile.error("jobs", girl.FullName(), " was not at the movie studio when doing movie job.");
        return false;
    }

    // No film crew.. then go home
    if (!brothel->HasInteraction(DirectorInteractionId) ||
        !brothel->HasInteraction(CamMageInteractionId)  ||
        !brothel->HasInteraction(CrystalPurifierInteractionId) )
    {
        if(brothel->NumInteractors(DirectorInteractionId) != 0 && brothel->NumInteractors(CamMageInteractionId) != 0 &&
        brothel->NumInteractors(CrystalPurifierInteractionId) != 0) {
            girl.AddMessage("There were more scenes scheduled for filming today than you crew could handle. ${name} took the day off.",
                            IMGTYPE_PROFILE, EVENT_NOWORK);
        } else {
            girl.AddMessage("There was no crew to film the scene, so she took the day off. You need at least a Director, a Camera Mage,"
                            "and a Crystal Purifier to film a scene.", IMGTYPE_PROFILE,EVENT_NOWORK);
        }
        // still, we notify the building that we wanted these interactions.
        // TODO maybe have a separate function for this.
        brothel->RequestInteraction(DirectorInteractionId);
        brothel->RequestInteraction(CamMageInteractionId);
        brothel->RequestInteraction(CrystalPurifierInteractionId);
        return false;
    }

    // other conditions in which she cannot work:
    if (girl.health() < m_MinimumHealth)
    {
        add_text("crew.refuse.health");
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return false;
    }

    if(m_RefuseIfPregnant && girl.is_pregnant()) {
        add_text("crew.refuse.pregnant");
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return false;
    }

    // check sex type
    if(m_PleasureFactor.Skill != NUM_SKILLS) {
        if(!brothel->is_sex_type_allowed(m_PleasureFactor.Skill)) {
            girl.AddMessage("A scene of this type cannot be filmed, because you have forbidden the corresponding sex type.",
                            IMGTYPE_PROFILE, EVENT_NOWORK);
            return false;
        }
    }

    return true;
}

namespace {
    int libido_influence(const sFilmPleasureData& data, const sGirl& girl) {
        if(data.Skill == NUM_SKILLS)
            return 0;

        // calculate the percentage of the skill between SkillMin and 100
        int skill = girl.get_skill(data.Skill) - data.SkillMin;
        float skill_factor = static_cast<float>(skill) / static_cast<float>(100 - data.SkillMin);
        if(skill_factor < 0) {
            skill_factor = 0.f;
        }
        // a non-linear transformation
        // the 0.09 makes the curve a bit smoother
        skill_factor = std::sqrt(0.09f + skill_factor) / std::sqrt(1.09f);

        float lib_value = girl.libido() * (data.Factor + data.BaseValue) * skill_factor;
        return lib_value / 100 - data.BaseValue;
    }
}


sFilmObedienceData cFilmSceneJob::CalcChanceToObey(const sGirl& girl) const {
    int base_chance = 100 - cGirls::GetRebelValue(girl, false, job());
    base_chance /= 2;      // get a conventional percentage value

    int libido = libido_influence(m_PleasureFactor, girl);
    int enjoy = (2 * girl.get_enjoyment(m_PrimaryAction) + girl.get_enjoyment(m_SecondaryAction)) / 3;
    int love_hate = (girl.pclove() + girl.pcfear() - girl.pchate()) / 10;

    return {base_chance, libido, enjoy, love_hate};
}

bool cFilmSceneJob::CheckRefuseWork(sGirl& girl) {
    // since a scene job combines multiple actions, we cannot use the normal disobey_check code
    auto obey = CalcChanceToObey(girl);

    m_Dbg_Msg << "Obedience:\n  Basic Value " << obey.Base << "\n";
    m_Dbg_Msg << "  Libido " << obey.Libido << "\n";
    m_Dbg_Msg << "  Enjoy " << obey.Enjoy << "\n";
    m_Dbg_Msg << "  Love/Hate " << obey.LoveHate << "\n";

    int chance_to_obey = obey.total();


    // TODO add trait based values

    int roll = g_Dice.d100();                                // let's get a percentage roll
    m_Dbg_Msg << "           Total " << chance_to_obey << " " << roll << "\n";
    int diff = chance_to_obey - roll;
    bool girl_obeys = (diff >= 0);
    if (girl_obeys)                                            // there's a price to be paid for relying on love or fear
    {
        if (diff < (girl.pclove() / 10)) girl.pclove(-1);    // if the only reason she obeys is love it wears away that love
        if (diff < (girl.pcfear() / 10)) girl.pchate(+1);    // just a little bit. And if she's only doing it out of fear, she will hate you more
    }

    // if she doesn't want to do it, but still works, her enjoyment decreases
    m_Dbg_Msg << "Enjoyment: \n  Init " << m_Enjoyment << "\n";
    m_Dbg_Msg << "  Libido " << libido_influence(m_PleasureFactor, girl) << "\n";
    m_Dbg_Msg << "  Base " << (2*(2 * girl.get_enjoyment(m_PrimaryAction) + girl.get_enjoyment(m_SecondaryAction)) / 3) / 3 << "\n";
    m_Enjoyment += libido_influence(m_PleasureFactor, girl);
    m_Enjoyment += (2*(2 * girl.get_enjoyment(m_PrimaryAction) + girl.get_enjoyment(m_SecondaryAction)) / 3) / 3;
    if(chance_to_obey < 60) {
        m_Enjoyment += (chance_to_obey - 60) / 10;
        m_Dbg_Msg << "  Obey " << (chance_to_obey - 60) / 10 << "\n";
    }

    if (!girl_obeys)
    {
        if(m_CanBeForced) {
            return RefusedTieUp(girl);
        } else {
            add_text("refuse");
            girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
            produce_debug_message(girl);
        }
        return true;
    } else {
        add_text("work");
        return false;
    }
}

bool cFilmSceneJob::RefusedTieUp(sGirl& girl) {
    if (girl.is_slave())
    {
        if (g_Game->player().disposition() > 30)  // nice
        {
            add_text("disobey.slave.nice");
            girl.pclove(1);
            girl.pcfear(-1);
            girl.pchate(-1);
            girl.obedience(-1);
            girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
            return true;
        }
        else if (g_Game->player().disposition() > -30) //pragmatic
        {
            add_text("disobey.slave.neutral") << "\n \n";
            girl.pclove(-1);
            girl.pchate(2);
            girl.pcfear(1);
            g_Game->player().disposition(-1);
            m_Enjoyment -= 5;
            m_IsForced = true;
        }
        else
        {
            add_text("disobey.slave.evil")<< "\n \n";
            girl.pclove(-4);
            girl.pchate(+5);
            girl.pcfear(+5);
            girl.spirit(-1);
            g_Game->player().disposition(-2);
            m_Enjoyment -= 10;
            m_IsForced = true;
        }
    }
    else // not a slave
    {
        add_text("disobey.free");
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    }

    if(m_IsForced) {
        girl.upd_temp_skill(SKILLS::SKILL_PERFORMANCE, -100);
        girl.upd_temp_stat(STATS::STAT_CHARISMA, -100);
        m_Dbg_Msg << "Girl is tied. Job performance reduced from " << m_Performance << " to ";
        m_Performance = GetPerformance(girl, false) - 10;
        m_Dbg_Msg << m_Performance << "\n";
        girl.upd_temp_skill(SKILLS::SKILL_PERFORMANCE, 100);
        girl.upd_temp_stat(STATS::STAT_CHARISMA, 100);
    }

    return false;
}

bool cFilmSceneJob::DoWork(sGirl& girl, bool is_night) {
    auto brothel = dynamic_cast<sMovieStudio*>(girl.m_Building);
    assert(brothel);

    // pre-work processing
    PreFilmCallback(girl);

    int roll = d100();
    if (roll <= 10) { m_Enjoyment -= uniform(1, 4);    }
    else if (roll >= 90) { m_Enjoyment += uniform(1, 4); }
    else { m_Enjoyment += uniform(0, 2); }

    int bonus_enjoy = 0;
    if (m_Performance >= 200)
    {
        bonus_enjoy = uniform(9, 14);
    }
    else if (m_Performance >= 100)
    {
        int offset = (m_Performance - 100) / 10;
        bonus_enjoy = uniform(offset / 2, 2 + offset);
    }
    else
    {
        bonus_enjoy = -uniform(3, 6);
    }
    m_Enjoyment += bonus_enjoy;
    m_Dbg_Msg << "  Perf base Enjoy: " << bonus_enjoy << "\n";

    ss << "\n ";

    cGirls::UnequipCombat(girl);

    Narrate(girl);

    m_Dbg_Msg << "Performance: " << m_Performance << "\n";

    int quality = m_Performance * 85 / 300;
    quality += girl.get_trait_modifier("movie.quality");

    // post-work text
    PostFilmCallback(girl);


    if(m_SexAction != SexAction::NONE) {
        if(girl.lose_trait("Virgin")) {
            ss << "She is no longer a virgin.\n";
            quality += 10;
        }

        if(m_SexAction == SexAction::HUMAN) {
            sCustomer Cust = g_Game->GetCustomer(*brothel);
            Cust.m_Amount = 1;
            Cust.m_IsWoman = false;
            if (!girl.calc_pregnancy(Cust, 1.0)) {
                g_Game->push_message(girl.FullName() + " has gotten pregnant", 0);
            }
        } else if (m_SexAction == SexAction::MONSTER) {
            if (!girl.calc_insemination(cGirls::GetBeast(), 1.0))
                g_Game->push_message(girl.FullName() + " has gotten inseminated", 0);
        }
    }



    ss << "\n";

    // consequences of forcing are loss of iron will and potential gain of Mind Fucked
    if (m_IsForced) {
        if(girl.has_active_trait("Iron Will")) {
            if (girl.lose_trait("Iron Will", 2))
            {
                girl.AddMessage("${name}'s unwilling degradation has shattered her iron will.", IMGTYPE_TORTURE, EVENT_GOODNEWS);
                return true;
            }
        } else {
            if (girl.gain_trait("Mind Fucked", 2))
            {
                girl.AddMessage("${name} has become Mind Fucked from the forced degradation.", IMGTYPE_TORTURE, EVENT_WARNING);
                return true;
            }
        }
    }

    // independent of the scene description, charisma and beauty change the rating a bit
    quality += (girl.charisma() - 50) / 10;
    quality += (girl.beauty() - 50) / 10;
    quality += girl.fame() / 10;
    quality += girl.level();

    if(girl.is_unpaid()) {
        quality = quality * 90 / 100;
    }

    // TODO better fame mechanics based on how well the movie does
    if(girl.fame() < quality / 2 && chance(50)) {
        girl.fame(1);
    }

    auto& scene = film_scene(g_Game->movie_manager(), girl, quality, m_SceneType, m_IsForced);

    girl.AddMessage(ss.str(), m_EventImage, EVENT_DAYSHIFT);

    // Improve stats and gain traits
    apply_gains(girl, m_Performance);
    update_enjoyment(girl);

    produce_debug_message(girl);

    return false;
}

void cFilmSceneJob::produce_debug_message(sGirl& girl) const { girl.AddMessage(m_Dbg_Msg.str(), IMGTYPE_PROFILE, EVENT_DEBUG); }

void cFilmSceneJob::update_enjoyment(sGirl& girl) const {
    m_Dbg_Msg << "Enjoyment: " << m_Enjoyment << " [" << girl.get_enjoyment(m_PrimaryAction) << "]\n";
    int old_enjoyment = girl.m_Enjoyment[m_PrimaryAction];
    if (m_Enjoyment > old_enjoyment + 2) {
        int delta = m_Enjoyment - old_enjoyment;
        if(chance(25 + 5 * delta)) {
            girl.upd_Enjoyment(m_PrimaryAction, uniform(1, delta));
            std::stringstream enjoy_message;
            enjoy_message << "${name} had fun working today (" << m_Enjoyment
                          << "), and now enjoys this job a little more (" << old_enjoyment << " -> " << girl.get_enjoyment(m_PrimaryAction) << ").";
            girl.AddMessage(enjoy_message.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);
        }
    } else if (m_Enjoyment < old_enjoyment - 6) {
        int delta = old_enjoyment - 4 - m_Enjoyment;
        if(chance(25 + 5 * delta)) {
            girl.upd_Enjoyment(m_PrimaryAction, -uniform(1, 1 + delta));
            std::stringstream enjoy_message;
            enjoy_message << "${name} disliked working today (" << m_Enjoyment
                          << "), and now enjoys this job a little less (" << old_enjoyment << " -> " << girl.get_enjoyment(m_PrimaryAction) << ").";
            girl.AddMessage(enjoy_message.str(), IMGTYPE_PROFILE, EVENT_WARNING);
        }
    }
}

void cFilmSceneJob::PrintPerfSceneEval() {
    if (m_Performance >= 300)
    {
        add_text("work.perfect");
    }
    else if (m_Performance >= 220)
    {
        add_text("work.great");
    }
    else if (m_Performance >= 175)
    {
        add_text("work.good");
    }
    else if (m_Performance >= 135)
    {
        add_text("work.ok");
    }
    else if (m_Performance >= 90)
    {
        add_text("work.bad");
    }
    else
    {
        add_text("work.worst");
    }

    ss << "\n";
}

void cFilmSceneJob::PrintForcedSceneEval() {
    add_text("forced-filming");
}

cFilmSceneJob::cFilmSceneJob(JOBS job, const char* xml, Image_Types event_image, SceneType scene, SexAction sex) :
    cBasicJob(job), m_EventImage(event_image), m_SceneType(scene), m_SexAction(sex) {
    load_from_xml(xml);
}

void cFilmSceneJob::load_from_xml_callback(const tinyxml2::XMLElement& job_element) {
    m_PrimaryAction = get_action_id( GetStringAttribute(job_element, "PrimaryAction") );
    m_SecondaryAction = get_action_id( GetStringAttribute(job_element, "SecondaryAction") );
    m_MinimumHealth = job_element.IntAttribute("MinimumHealth", -100);
    m_RefuseIfPregnant = job_element.IntAttribute("RefuseIfPregnant", 0);
    m_CanBeForced = job_element.IntAttribute("CanBeForced", 0);

    auto li_el = job_element.FirstChildElement("LustInfluence");
    if(li_el) {
        m_PleasureFactor.Factor = GetIntAttribute(*li_el, "Amount");
        m_PleasureFactor.SkillMin = li_el->IntAttribute("SkillMin", 0);
        m_PleasureFactor.BaseValue = GetIntAttribute(*li_el, "BaseValue");
        m_PleasureFactor.Skill = get_skill_id(GetStringAttribute(*li_el, "Skill"));
    }

    // check that we have the corresponding text elements
    if(m_CanBeForced) {
        if(!has_text("disobey.free") || !has_text("disobey.slave.nice") || !has_text("disobey.slave.neutral") ||
           !has_text("disobey.slave.evil") || !has_text("forced-filming")) {
            g_LogFile.warning("job", "Job ", get_job_name(job()), " is missing disobey texts");
        }
    }
}

void cFilmSceneJob::PostFilmCallback(sGirl& girl) {
    if(has_text("post-work-event")) {
        add_text("post-work-event");
    }
}

void cFilmSceneJob::PreFilmCallback(sGirl& girl) {
    if(has_text("pre-work-event")) {
        add_text("pre-work-event");
    }
}

void cFilmSceneJob::Narrate(sGirl& girl) {
    if(m_IsForced) {
        PrintForcedSceneEval();
    } else {
        PrintPerfSceneEval();
    }
}

void cFilmSceneJob::InitWork() {
    cBasicJob::InitWork();
    RegisterVariable("Enjoy", m_Enjoyment);
    m_IsForced = false;
    m_Enjoyment = 0;
    m_Dbg_Msg.str("");
}

SKILLS cFilmSceneJob::GetSexType() const {
    return m_PleasureFactor.Skill;
}

