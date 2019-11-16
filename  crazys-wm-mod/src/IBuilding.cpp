/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders
 * who meet on http://pinkpetal.org     // old site: http://pinkpetal .co.cc
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

#include "IBuilding.hpp"

#include <memory>
#include "src/buildings/cBrothel.h"
#include "cTariff.h"
#include "src/Game.hpp"
#include "src/sStorage.hpp"
#include "cGangs.h"

extern cConfig cfg;

void do_food_and_digs(IBuilding * brothel, sGirl * girl);
void updateGirlTurnBrothelStats(sGirl* girl);

sGirl* IBuilding::find_girl_by_name(std::string name) const
{
    return find_first_girl(HasName(std::move(name)));
}

const char* building_type_to_str(BuildingType type) {
    switch(type) {
    case BuildingType::BROTHEL:
        return "Brothel";
    case BuildingType::ARENA:
        return "Arena";
    case BuildingType::CLINIC:
        return "Clinic";
    case BuildingType::FARM:
        return "Farm";
    case BuildingType::HOUSE:
        return "House";
    case BuildingType::CENTRE:
        return "Centre";
    case BuildingType::STUDIO:
        return "MovieStudio";
    }
}

const char* IBuilding::type_str() const
{
    return building_type_to_str(type());
}

int IBuilding::get_girl_index(const sGirl& girl) const
{
    auto found = std::find(begin(m_Girls), end(m_Girls), &girl);
    //! \todo this is a rather inefficient implementation for std::list
    if(found != end(m_Girls)) {
        return std::distance(begin(m_Girls), found);
    }
    return -1;
}

// ----- Matron  // `J` added building checks
bool IBuilding::matron_on_shift(int shift) const
{
    return num_girls_on_job(m_MatronJob, shift) > 0;
}

//int sBrothel::matron_count
int IBuilding::matron_count() const
{
    int sum = 0;
    for (int i = 0; i < 2; i++)
    {
        if(matron_on_shift(i)) {
            ++sum;
        }
    }
    return sum;
}


void IBuilding::BeginWeek()
{
    std::vector<sGirl*> dead_girls;

    stringstream ss;
    for(auto& cgirl : m_Girls)
    {
        m_Filthiness++;

        if (cgirl->is_dead())			// Remove any dead bodies from last week
        {
            m_Filthiness++; // `J` Death is messy
            // increase all the girls fear and hate of the player for letting her die (weather his fault or not)
            for(auto& girl : m_Girls)
            {
                girl->upd_stat(STAT_PCFEAR, 2);
                girl->upd_stat(STAT_PCHATE, 1);
            }

            ss.str(""); ss << cgirl->m_Realname << " has died from her injuries, the other girls all fear and hate you a little more.";
            cgirl->m_Events.AddMessage(ss.str(), IMGTYPE_DEATH, EVENT_DANGER);
            g_Game->push_message(ss.str(), COLOR_RED);
            ss.str(""); ss << cgirl->m_Realname << " has died from her injuries.  Her body will be removed by the end of the week.";
            cgirl->m_Events.AddMessage(ss.str(), IMGTYPE_DEATH, EVENT_SUMMARY);
            dead_girls.push_back(cgirl);
            continue;
        }
        else
        {
            cgirl->m_Building = this;
            cgirl->m_Tort = false;

            cgirl->m_Events.Clear();                // Clear the girls' events from the last turn
            cgirl->m_Pay = cgirl->m_Tips = 0;


            // `J` Check for out of building jobs
            if (cgirl->m_DayJob	  < m_FirstJob || cgirl->m_DayJob   > m_LastJob)	cgirl->m_DayJob = m_RestJob;
            if (cgirl->m_NightJob < m_FirstJob || cgirl->m_NightJob > m_LastJob)	cgirl->m_NightJob = m_RestJob;
            if (cgirl->m_PrevDayJob	< m_FirstJob || cgirl->m_PrevDayJob > m_LastJob)        cgirl->m_PrevDayJob = 255;
            if (cgirl->m_PrevNightJob < m_FirstJob || cgirl->m_PrevNightJob > m_LastJob)	cgirl->m_PrevNightJob = 255;

            // set yesterday jobs for everyone
            cgirl->m_YesterDayJob = cgirl->m_DayJob;
            cgirl->m_YesterNightJob = cgirl->m_NightJob;
            cgirl->m_Refused_To_Work_Day = cgirl->m_Refused_To_Work_Night = false;
            cgirl->m_NumCusts_old = cgirl->m_NumCusts;// prepare for this week
            string summary;

            cgirl->add_tiredness();			        // `J` moved all girls add tiredness to one place
            do_food_and_digs(this, cgirl);		    // Brothel only update for girls accommodation level
            cGirls::CalculateGirlType(cgirl);		// update the fetish traits
            cGirls::updateGirlAge(cgirl, true);		// update birthday counter and age the girl
            g_Game->girl_pool().HandleChildren(cgirl, summary);	// handle pregnancy and children growing up
            cGirls::updateSTD(cgirl);				// health loss to STD's				NOTE: Girl can die
            cGirls::updateHappyTraits(cgirl);		// Update happiness due to Traits	NOTE: Girl can die
            updateGirlTurnBrothelStats(cgirl);		// Update daily stats				Now only runs once per day
            cGirls::updateGirlTurnStats(cgirl);		// Stat Code common to Dugeon and Brothel


            if (cgirl->m_JustGaveBirth)		// if she gave birth, let her rest this week
            {
                if (cgirl->m_DayJob != m_RestJob)		cgirl->m_PrevDayJob = cgirl->m_DayJob;
                if (cgirl->m_NightJob != m_RestJob)	cgirl->m_PrevNightJob = cgirl->m_NightJob;
                cgirl->m_DayJob = cgirl->m_NightJob = m_RestJob;
            }
        }
    }

    for(auto& dead : dead_girls) {
        m_Girls.remove(dead);
        delete dead;
    }
}

void IBuilding::HandleRestingGirls(bool is_night, bool has_matron, const char * matron_name)
{
    std::stringstream ss;
    for(auto& current : girls())
    {
        unsigned int sw = (is_night ? current->m_NightJob : current->m_DayJob);
        if (current->is_dead() || sw != m_RestJob)
        {	// skip dead girls and anyone not resting
            continue;
        }
        int sum = EVENT_SUMMARY;
        std::string summary;
        ss.str("");
        const auto& girlName = current->m_Realname;

        if (current->m_PregCooldown == cfg.pregnancy.cool_down())
        {
            ss << girlName << " is on maternity leave.";
        }
        else if (handle_resting_girl(*current, is_night, has_matron, ss)) {
            // nothing to do her, handle_resting_girl did all the work.
        }
        else if (current->health() < 80 || current->tiredness() > 20)
        {
            g_Game->job_manager().JobFunc[m_RestJob](current, is_night, summary, g_Dice);
        }
        else if (has_matron)	// send her back to work
        {
            int psw = (is_night ? current->m_PrevNightJob : current->m_PrevDayJob);
            if (psw != m_RestJob && psw != 255)
            {	// if she had a previous job, put her back to work.
                if(!handle_back_to_work(*current, ss, is_night)) {
                    if (is_night == SHIFT_DAY)
                    {
                        current->m_DayJob = current->m_PrevDayJob;
                        if (current->m_NightJob == m_RestJob && current->m_PrevNightJob != m_RestJob && current->m_PrevNightJob != 255)
                            current->m_NightJob = current->m_PrevNightJob;
                    }
                    else
                    {
                        if (current->m_DayJob == m_RestJob && current->m_PrevDayJob != m_RestJob && current->m_PrevDayJob != 255)
                            current->m_DayJob = current->m_PrevDayJob;
                        current->m_NightJob = current->m_PrevNightJob;
                    }
                    ss << "The " << matron_name << " puts " << girlName << " back to work.\n";
                }
            }
            else if (current->m_DayJob == m_RestJob && current->m_NightJob == m_RestJob)
            {
                auto_assign_job(current, ss, is_night);
            }
            current->m_PrevDayJob = current->m_PrevNightJob = 255;
            sum = EVENT_BACKTOWORK;
        }
        else if (current->health() < 100 || current->tiredness() > 0)	// if there is no matron to send her somewhere just do resting
        {
            g_Game->job_manager().JobFunc[m_RestJob](current, is_night, summary, g_Dice);
        }
        else	// no one to send her back to work
        {
            ss << "WARNING " << girlName << " is doing nothing!\n";
            sum = EVENT_WARNING;
        }

        if (ss.str().length() > 0) current->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, sum);
    }
}

void IBuilding::Update()
{
    m_Finance.zero();
    m_AntiPregUsed = 0;

    BeginWeek();
    UpdateGirls(false);	// Run the Day Shift
    UpdateGirls(true);	    // Run the Night Shift

    g_Game->gold().brothel_accounts(m_Finance, m_id);

    for(auto cgirl : girls())
    {
        cGirls::updateTemp(cgirl);			// update temp stuff
        cGirls::EndDayGirls(*this, cgirl);
    }

    // cap filthiness and security at 0
    if (m_Filthiness < 0)		m_Filthiness = 0;
    if (m_SecurityLevel < 0)	m_SecurityLevel = 0;
}


void IBuilding::EndShift(const string& matron_title, bool Day0Night1, bool has_matron)
{
    for(auto& current : girls())
    {
        if (current->is_dead())
        {	// skip dead girls
            continue;
        }
        string girlName = current->m_Realname;
        int sum = EVENT_SUMMARY;
        stringstream ss;
        ss.str("");

        // update for girls items that are not used up
        do_daily_items(*current);					// `J` added

        // Level the girl up if nessessary
        cGirls::LevelUp(current);
        // Natural healing, 2% health and 2% tiredness per day
        current->upd_stat(STAT_HEALTH, 2, false);
        current->upd_stat(STAT_TIREDNESS, -2, false);

        u_int sw = (Day0Night1 ? current->m_NightJob : current->m_DayJob);
        if (current->happiness()< 40)
        {
            if (sw != m_MatronJob && has_matron && num_girls() > 1 && g_Dice.percent(70))
            {
                ss << "The " << matron_title << " helps cheer up " << girlName << " when she is feeling sad.\n";
                current->happiness(g_Dice % 10 + 5);
            }
            else if (num_girls() > 10 && g_Dice.percent(50))
            {
                ss << "Some of the other girls help cheer up " << girlName << " when she is feeling sad.\n";
                current->happiness(g_Dice % 8 + 3);
            }
            else if (num_girls() > 1 && g_Dice.percent(max((int)num_girls(), 50)))
            {
                ss << "One of the other girls helps cheer up " << girlName << " when she is feeling sad.\n";
                current->happiness(g_Dice % 6 + 2);
            }
            else if (num_girls() == 1 && g_Dice.percent(70))
            {
                ss << girlName << " plays around in the empty building until she feels better.\n";
                current->happiness(g_Dice % 10 + 10);
            }
            else if (current->health()< 20) // no one helps her and she is really unhappy
            {
                ss << girlName << " is looking very depressed. You may want to do something about that before she does something drastic.\n";
                sum = EVENT_WARNING;
            }
        }

        int t = current->tiredness();
        int h = current->health();
        if (sw == m_MatronJob && (t > 60 || h < 40))
        {
            ss << "As " << matron_title << ", " << girlName << " has the keys to the store room.\nShe used them to 'borrow' ";
            if (t > 50 && h < 50)
            {
                ss << "some potions";
                current->upd_stat(STAT_HEALTH, 20 + g_Dice % 20, false);
                current->upd_stat(STAT_TIREDNESS, -(20 + g_Dice % 20), false);
                g_Game->gold().consumable_cost(20, true);
            }
            else if (t > 50)
            {
                ss << "a resting potion";
                current->upd_stat(STAT_TIREDNESS, -(20 + g_Dice % 20), false);
                g_Game->gold().consumable_cost(10, true);
            }
            else if (h < 50)
            {
                ss << "a healing potion";
                current->upd_stat(STAT_HEALTH, 20 + g_Dice % 20, false);
                g_Game->gold().consumable_cost(10, true);
            }
            else
            {
                ss << "a potion";
                current->upd_stat(STAT_HEALTH, 10 + g_Dice % 10, false);
                current->upd_stat(STAT_TIREDNESS, -(10 + g_Dice % 10), false);
                g_Game->gold().consumable_cost(5, true);
            }
            ss << " for herself.\n";
        }
        else if (t > 80 || h < 40)
        {
            if (!has_matron)	// do no matron first as it is the easiest
            {
                ss << "WARNING! " << girlName;
                /* */if (t > 80 && h < 20)	ss << " is in real bad shape, she is tired and injured.\nShe should go to the Clinic.\n";
                else if (t > 80 && h < 40)	ss << " is in bad shape, she is tired and injured.\nShe should rest or she may die!\n";
                else if (t > 80)/*      */	ss << " is desparatly in need of rest.\nGive her some free time\n";
                else if (h < 20)/*      */	ss << " is badly injured.\nShe should rest or go to the Clinic.\n";
                else if (h < 40)/*      */	ss << " is hurt.\nShe should rest and recuperate.\n";
                sum = EVENT_WARNING;
            }
            else	// do all other girls with a matron working
            {
                if (current->m_PrevNightJob == 255 && current->m_PrevDayJob == 255) // the girl has been working
                {
                    current->m_PrevDayJob = current->m_DayJob;
                    current->m_PrevNightJob = current->m_NightJob;
                    current->m_DayJob = current->m_NightJob = m_RestJob;
                    ss << "The " << matron_title << " takes " << girlName << " off duty to rest due to her ";
                    if (t > 80 && h < 40)	ss << "exhaustion.\n";
                    else if (t > 80)		ss << "tiredness.\n";
                    else if (h < 40)		ss << "low health.\n";
                    else /*       */		ss << "current state.\n";
                    sum = EVENT_WARNING;
                }
                else	// the girl has already been taken off duty by the matron
                {
                    if (g_Dice.percent(70))
                    {
                        ss << "The " << matron_title << "helps ";
                        if (t > 80 && h < 40)
                        {
                            ss << girlName << " recuperate.\n";
                            current->upd_stat(STAT_HEALTH, 2 + g_Dice % 4, false);
                            current->upd_stat(STAT_TIREDNESS, -(2 + g_Dice % 4), false);
                        }
                        else if (t > 80)
                        {
                            ss << girlName << " to relax.\n";
                            current->upd_stat(STAT_TIREDNESS, -(5 + g_Dice % 5), false);
                        }
                        else if (h < 40)
                        {
                            ss << " heal " << girlName << ".\n";
                            current->upd_stat(STAT_HEALTH, 5 + g_Dice % 5, false);
                        }
                    }
                }
            }
        }

        if (ss.str().length() > 0)	current->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, sum);
    }
}


IBuilding::~IBuilding()
{
    for(sGirl* girl : m_Girls)
    {
        delete girl;
    }
}

int IBuilding::num_girls() const
{
    return m_Girls.size();
}

int IBuilding::num_girls_on_job(JOBS jobID, int is_night) const
{
    return num_girls_with(HasJob(jobID, is_night));
}

void IBuilding::add_girl(sGirl* girl, bool keep_job)
{
    girl->m_Building = this;
    m_Girls.push_back(girl);
    if (!keep_job) {
        girl->m_DayJob = girl->m_NightJob = m_RestJob;
    }
    girl->FixFreeTimeJobs();
}

void IBuilding::remove_girl(sGirl* girl)
{
    m_Girls.remove(girl);
    girl->m_Building = nullptr;
}

void IBuilding::save_girls_xml(TiXmlElement& target) const
{
    TiXmlElement* pGirls = new TiXmlElement("Girls");
    target.LinkEndChild(pGirls);
    for(sGirl* girl : m_Girls)
    {
        girl->SaveGirlXML(pGirls);
    }
}

void IBuilding::load_girls_xml(TiXmlHandle root)
{
    TiXmlHandle pGirls = root.FirstChildElement("Girls");
    for (TiXmlElement* pGirl = pGirls.FirstChildElement("Girl").ToElement();
         pGirl != nullptr;
         pGirl = pGirl->NextSiblingElement("Girl"))// load each girl and add her
    {
        auto girl = std::make_unique<sGirl>();
        if (girl->LoadGirlXML(TiXmlHandle(pGirl)))
        {
            add_girl(girl.release(), true);
        }
    }
}

IBuilding::IBuilding(BuildingType type, std::string name) : m_Type(type), m_Finance(0), m_Name(std::move(name))
{

}

bool IBuilding::provide_anti_preg() {
    int cost = g_Game->tariff().anti_preg_price(1);

    if (m_KeepPotionsStocked) {
        if (m_AntiPregPotions < m_AntiPregUsed) cost *= 5;
        g_Game->gold().consumable_cost(cost);
        m_AntiPregUsed++;
        return true;
    }
    if (m_AntiPregPotions > 0) {
        m_AntiPregUsed++;
        m_AntiPregPotions--;
        return true;
    }
    return false;
}

bool IBuilding::is_sex_type_allowed(unsigned int sex_type) const
{
    return !((sex_type == SKILL_ANAL && m_RestrictAnal) ||
             (sex_type == SKILL_BDSM && m_RestrictBDSM) ||
             (sex_type == SKILL_BEASTIALITY && m_RestrictBeast) ||
             (sex_type == SKILL_FOOTJOB && m_RestrictFoot) ||
             (sex_type == SKILL_GROUP && m_RestrictGroup) ||
             (sex_type == SKILL_HANDJOB && m_RestrictHand) ||
             (sex_type == SKILL_LESBIAN && m_RestrictLesbian) ||
             (sex_type == SKILL_NORMALSEX && m_RestrictNormal) ||
             (sex_type == SKILL_ORALSEX && m_RestrictOral) ||
             (sex_type == SKILL_STRIP && m_RestrictStrip) ||
             (sex_type == SKILL_TITTYSEX && m_RestrictTitty));
}

bool IBuilding::nothing_banned() const
{
    return !(m_RestrictAnal || m_RestrictBDSM || m_RestrictBeast || m_RestrictFoot ||
             m_RestrictGroup || m_RestrictHand || m_RestrictLesbian || m_RestrictNormal ||
             m_RestrictOral || m_RestrictStrip || m_RestrictTitty);
}

void IBuilding::set_sex_type_allowed(SKILLS sex_type, bool is_allowed)
{
    switch(sex_type) {
    case SKILL_ANAL:
        m_RestrictAnal = !is_allowed;
        break;
    case SKILL_BDSM:
        m_RestrictBDSM = !is_allowed;
        break;
    case SKILL_BEASTIALITY:
        m_RestrictBeast = !is_allowed;
        break;
    case SKILL_FOOTJOB:
        m_RestrictFoot = !is_allowed;
        break;
    case SKILL_GROUP:
        m_RestrictGroup = !is_allowed;
        break;
    case SKILL_HANDJOB:
        m_RestrictHand = !is_allowed;
        break;
    case SKILL_LESBIAN:
        m_RestrictLesbian = !is_allowed;
        break;
    case SKILL_NORMALSEX:
        m_RestrictNormal = !is_allowed;
        break;
    case SKILL_ORALSEX:
        m_RestrictOral = !is_allowed;
        break;
    case SKILL_STRIP:
        m_RestrictStrip = !is_allowed;
        break;
    case SKILL_TITTYSEX:
        m_RestrictTitty = !is_allowed;
        break;
    default:
        throw runtime_error("Invalid sex type!");
    }
}

void IBuilding::save_settings_xml(TiXmlElement& root) const
{
    root.SetAttribute("Name", m_Name);
    root.SetAttribute("BackgroundImage", m_BackgroundImage);

    root.SetAttribute("id", m_id);
    root.SetAttribute("NumRooms", m_NumRooms);
    root.SetAttribute("MaxNumRooms", m_MaxNumRooms);
    root.SetAttribute("Fame", m_Fame);
    root.SetAttribute("Happiness", m_Happiness);
    root.SetAttribute("Filthiness", m_Filthiness);
    root.SetAttribute("SecurityLevel", m_SecurityLevel);
    root.SetAttribute("AdvertisingBudget", m_AdvertisingBudget);
    root.SetAttribute("AntiPregPotions", m_AntiPregPotions);
    root.SetAttribute("AntiPregUsed", m_AntiPregUsed);
    root.SetAttribute("KeepPotionsStocked", m_KeepPotionsStocked);

    root.SetAttribute("RestrictAnal", m_RestrictAnal);
    root.SetAttribute("RestrictBDSM", m_RestrictBDSM);
    root.SetAttribute("RestrictBeast", m_RestrictBeast);
    root.SetAttribute("RestrictFoot", m_RestrictFoot);
    root.SetAttribute("RestrictGroup", m_RestrictGroup);
    root.SetAttribute("RestrictHand", m_RestrictHand);
    root.SetAttribute("RestrictLesbian", m_RestrictLesbian);
    root.SetAttribute("RestrictNormal", m_RestrictNormal);
    root.SetAttribute("RestrictOral", m_RestrictOral);
    root.SetAttribute("RestrictStrip", m_RestrictStrip);
    root.SetAttribute("RestrictTitty", m_RestrictTitty);
}

void IBuilding::load_settings_xml(TiXmlElement& root)
{
    const char* name_attribute = root.Attribute("Name");
    if (name_attribute)
    {
        m_Name = name_attribute;
    }

    const char* bg_attribute = root.Attribute("BackgroundImage");
    if(bg_attribute) {
        m_BackgroundImage = bg_attribute;
    }

    root.QueryIntAttribute("id", &m_id);

    // load variables for sex restrictions
    root.QueryValueAttribute<bool>("RestrictAnal", &m_RestrictAnal);
    root.QueryValueAttribute<bool>("RestrictBDSM", &m_RestrictBDSM);
    root.QueryValueAttribute<bool>("RestrictBeast", &m_RestrictBeast);
    root.QueryValueAttribute<bool>("RestrictFoot", &m_RestrictFoot);
    root.QueryValueAttribute<bool>("RestrictGroup", &m_RestrictGroup);
    root.QueryValueAttribute<bool>("RestrictHand", &m_RestrictHand);
    root.QueryValueAttribute<bool>("RestrictLesbian", &m_RestrictLesbian);
    root.QueryValueAttribute<bool>("RestrictNormal", &m_RestrictNormal);
    root.QueryValueAttribute<bool>("RestrictOral", &m_RestrictOral);
    root.QueryValueAttribute<bool>("RestrictStrip", &m_RestrictStrip);
    root.QueryValueAttribute<bool>("RestrictTitty", &m_RestrictTitty);

    root.QueryValueAttribute<unsigned short>("AdvertisingBudget", &m_AdvertisingBudget);
    root.QueryIntAttribute("AntiPregPotions", &m_AntiPregPotions);
    root.QueryIntAttribute("AntiPregUsed", &m_AntiPregUsed);
    root.QueryValueAttribute<bool>("KeepPotionsStocked", &m_KeepPotionsStocked);

    root.QueryIntAttribute("Filthiness", &m_Filthiness);
    root.QueryIntAttribute("SecurityLevel", &m_SecurityLevel);
    root.QueryValueAttribute<unsigned short>("Happiness", &m_Happiness);

    int tmp = 0;
    root.QueryIntAttribute("Fame", &tmp);
    m_Fame = tmp;

    root.QueryIntAttribute("NumRooms", &tmp); m_NumRooms = tmp; tmp = 0;
    root.QueryIntAttribute("MaxNumRooms", &tmp); m_MaxNumRooms = tmp; tmp = 0;
    if (m_MaxNumRooms < 200)		m_MaxNumRooms = 200;
    else if (m_MaxNumRooms > 600)	m_MaxNumRooms = 600;
}

int IBuilding::filthiness() const
{
    return m_Filthiness;
}

int IBuilding::security() const
{
    return m_SecurityLevel;
}

void IBuilding::load_xml(TiXmlElement& root)
{
    load_settings_xml(root);
    load_girls_xml(TiXmlHandle(&root));
}

void IBuilding::save_xml(TiXmlElement& root) const
{
    save_settings_xml(root);
    save_girls_xml(root);
    save_additional_xml(root);
}

sGirl* IBuilding::get_girl(int index)
{
    if(m_Girls.empty())
        return nullptr;

    if(index >= m_Girls.size())
        return nullptr;

    auto girl = m_Girls.begin();
    std::advance(girl, index);
    return *girl;
}

void IBuilding::BeginShift()
{
    for(auto& girl : girls()) {
        if (girl->is_dead())		// skip dead girls
        {
            continue;
        }
        else
        {
            girl->use_items();
            cGirls::CalculateGirlType(girl);		// update the fetish traits
            cGirls::CalculateAskPrice(girl, true);	// Calculate the girls asking price
        }
    }
}

//void cBrothelManager::AddAntiPreg(int amount)
void IBuilding::AddAntiPreg(int amount) // unused
{
    m_AntiPregPotions += amount;
    if (m_AntiPregPotions > 700)
        m_AntiPregPotions = 700;
}

bool IBuilding::SetupMatron(bool is_night, const string& title)
{
    int has_matron = num_girls_on_job(m_MatronJob, is_night);
    for(auto& current : girls())
    {
        if (current->is_dead() ||
            (has_matron && (current->m_DayJob != m_MatronJob || current->m_NightJob != m_MatronJob)) ||
            (!has_matron && (current->m_PrevDayJob != m_MatronJob || current->m_PrevNightJob != m_MatronJob)))
        {	// Sanity check! Don't process dead girls and only process those with matron jobs
            continue;
        }
        // `J` so someone is or was a matron

        string girlName = current->m_Realname;
        // if there is no matron on duty, we see who was on duty previously
        if (has_matron < 1)
        {
            // if a matron was found and she is healthy, not tired and not on maternity leave... send her back to work
            if (current->health() >= 50 && current->tiredness() <= 50 && current->m_PregCooldown < cfg.pregnancy.cool_down())
                // Matron job is more important so she will go back to work at 50% instead of regular 80% health and 20% tired
            {
                current->m_DayJob = current->m_NightJob = m_MatronJob;
                current->m_PrevDayJob = current->m_PrevNightJob = 255;
                current->m_Events.AddMessage("The " + title + " puts herself back to work.", IMGTYPE_PROFILE,
                                             EVENT_BACKTOWORK);
            }
            else { continue; }
        }

        // `J` Now we have a matron so lets see if she will work
        stringstream ss;
        int sum = EVENT_SUMMARY;

        // `J` she can refuse the first shift then decide to work the second shift
        if (!current->m_Refused_To_Work_Day && is_night)	// but if she worked the first shift she continues the rest of the night
        {
            ss << girlName << " continued to help the other girls throughout the night.";
            current->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_SUMMARY);
            return true;
        }
        else if (current->disobey_check(ACTION_WORKMATRON, JOB_MATRON))
        {
            if(is_night) {
                current->m_Refused_To_Work_Night = true;
            } else {
                current->m_Refused_To_Work_Day = true;
            }
            m_Fame -= current->fame();
            ss << girlName << " refused to work as the " << title << ".";
            current->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
            return false;
        }
        else	// so there is less chance of a matron refusing the entire turn
        {
            int totalGold = 0;
            string summary;
            // TODO use a global JobManager here
            cJobManager jobs;
            jobs.Setup();
            jobs.JobFunc[m_MatronJob](current, is_night ? 1 : 0, summary, g_Dice);
            totalGold += current->m_Pay + current->m_Tips;

            // She does not get paid for the first shift and gets docked some pay from the second shift if she refused once
            if (is_night) totalGold /= 3;

            // TODO this line is useless!
            current->m_Pay += max(0, totalGold);
            current->m_Pay = current->m_Tips = 0;

            m_Fame += current->fame();
            if (totalGold > 0)			{ ss << girlName << " earned a total of " << totalGold << " gold directly from you. She gets to keep it all."; }
            else if (totalGold == 0)	{ ss << girlName << " made no money."; }
            else if (totalGold < 0)		{
                sum = EVENT_DEBUG;
                ss << "ERROR: She has a loss of " << totalGold << " gold.\n \nPlease report this to the Pink Petal Devloment Team at http://pinkpetal.org\n\n";
                ss << "Girl Name: " << current->m_Realname << "\nJob: " << jobs.JobName[(is_night ? current->m_NightJob : current->m_DayJob)];
                ss << "\nPay:     " << current->m_Pay << "\nTips:   " << current->m_Tips << "\nTotal: " << totalGold;

            }
            current->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, sum);
            return true;
        }
    }
}

std::vector<sGirl*> IBuilding::girls_on_job(JOBS jobID, int is_night) const
{
    return find_all_girls(HasJob(jobID, is_night));
}

void IBuilding::update_all_girls_stat(STATS stat, int amount)
{
    for (auto& girl : girls()) {
        girl->upd_stat(stat, amount);
    }
}

int IBuilding::total_fame() const
{
    int total_fame = 0;
    for(auto& current : girls())
    {
        total_fame += current->fame();
    }
    return total_fame;
}

bool is_she_cleaning(sGirl& girl)
{
    if (girl.m_DayJob == JOB_CLEANING || girl.m_NightJob == JOB_CLEANING ||
        girl.m_DayJob == JOB_CLEANARENA || girl.m_NightJob == JOB_CLEANARENA ||
        girl.m_DayJob == JOB_STAGEHAND || girl.m_NightJob == JOB_STAGEHAND ||
        girl.m_DayJob == JOB_JANITOR || girl.m_NightJob == JOB_JANITOR ||
        girl.m_DayJob == JOB_CLEANCENTRE || girl.m_NightJob == JOB_CLEANCENTRE ||
        girl.m_DayJob == JOB_FARMHAND || girl.m_NightJob == JOB_FARMHAND ||
        girl.m_DayJob == JOB_CLEANHOUSE || girl.m_NightJob == JOB_CLEANHOUSE)
    {
        return true;
    }
    return false;
}

void IBuilding::do_daily_items(sGirl& girl)
{
    if (girl.m_NumInventory < 1) return;	// no items so skip it

    stringstream ss;
    string girlName = girl.m_Realname;
    int HateLove = girl.pclove() - girl.pchate();

    int mast = false;
    int strip = false;
    int combat = false;
    int formal = false;
    int swim = false;
    int cook = false;
    int maid = false;

    // `J` zzzzzz - This list needs to be sorted into groups


    // Clothing
#if 1

    // Always worn
    if (girl.has_item_j("Disguised Slave Band") != -1)
    {
        if (g_Dice.percent(20))		// it always works but doesn't always say anything
            ss << girlName << " went around wearing her Disguised Slave Band having no idea of what it really does to her.\n \n";
        girl.obedience(1);
        girl.pchate(-1);
    }


    if (girl.has_item_j("Compelling Buttplug") != -1)
    {
        if (g_Dice.percent(20))		// it always works but doesn't always say anything
            ss << girlName << " went around with her Compelling Buttplug in.\n \n";
        girl.anal(1);
        girl.refinement(-1);
        girl.dignity(-1);
    }
    else if (girl.has_item_j("Buttplug") != -1 && g_Dice.percent(10))
    {
        ss << girlName << " went around with her Buttplug in.\n \n";
        girl.anal(1);
        girl.refinement(-1);
        girl.dignity(-1);
    }
    else if (girl.has_item_j("Large Buttplug") != -1 && g_Dice.percent(10))
    {
        ss << girlName << " went around with her Large Buttplug in.\n \n";
        girl.anal(1);
        girl.refinement(-1);
        girl.dignity(-1);
    }

    // Dress
    if (g_Dice.percent(90)){}	// don't bother mentioning it most of the time
    else if (girl.has_item_j("Noble Gown") != -1)
    {
        ss << girlName << " went around wearing her Noble Gown today making her look quite formal.\n \n";
        formal = true;
        girl.confidence(1);
        girl.refinement(1);
        girl.dignity(1);
    }
    else if (girl.has_item_j("Liquid Dress") != -1)
    {
        ss << girlName << " went around wearing her Liquid Dress today making her squirm in delight at its inner workings.\n \n";
        girl.libido(1 + g_Dice % 10);
        girl.bdsm(1);
        girl.dignity(-1);
    }
    else if (girl.has_item_j("Dark Liquid Dress") != -1)
    {
        ss << girlName << " went around wearing her Dark Liquid Dress today making her squirm at its inner workings.\n \n";
        girl.libido(1 + g_Dice % 10);
        girl.bdsm(1 + g_Dice % 2);
        girl.dignity(-1);
    }
    else if (girl.has_item_j("Enchanted Dress") != -1)
    {
        ss << girlName << " went around wearing her Enchanted Dress today making her look quite formal.\n \n";
        formal = true;
        girl.magic(g_Dice % 2);
        girl.beauty(g_Dice % 2);
        girl.refinement(1);
        girl.dignity(1);
    }
    else if (girl.has_item_j("Gemstone Dress") != -1)
    {
        ss << girlName << " went around wearing her Gemstone Dress today making her look sparkely.\n \n";
        girl.constitution(g_Dice % 2);
        girl.refinement(1);
        girl.dignity(1);
    }
    else if (girl.has_item_j("Hime Dress") != -1)
    {
        ss << girlName << " went around wearing her Hime Dress today making her look quite cute.\n \n";
        girl.confidence(1);
        girl.refinement(1);
        girl.dignity(1);
        formal = true;
    }
    else if (girl.has_item_j("Linen Dress") != -1)
    {
        ss << girlName << " went around wearing her Linen Dress today making her look casual.\n \n";
        girl.service(1);
    }
    else if (girl.has_item_j("Chinese Dress") != -1)
    {
        ss << girlName << " went around wearing her Chinese Dress today making her look formal.\n \n";
        girl.confidence(1);
    }
    else if (girl.has_item_j("Lounge Dress") != -1)
    {
        ss << girlName << " went around wearing her Lounge Dress today making her look sultry.\n \n";
        girl.confidence(1);
    }
    else if (girl.has_item_j("Silken Dress") != -1)
    {
        ss << girlName << " went around wearing her Silken Dress today making her look quite sleek.\n \n";
        girl.beauty(1);
        girl.refinement(1);
        girl.dignity(1);
    }
    else if (girl.has_item_j("Trashy Dress") != -1)
    {
        ss << girlName << " went around wearing her Trashy Dress today making her feel dirty.\n \n";
        girl.refinement(-1);
        girl.refinement(-1);
        girl.dignity(-1);
    }
    else if (girl.has_item_j("Velvet Dress") != -1)
    {
        ss << girlName << " went around wearing her Velvet Dress today making her look quite elegant.\n \n";
        girl.charisma(1);
        girl.refinement(1);
        girl.dignity(1);
    }
    else if (girl.has_item_j("Oiran Dress") != -1)
    {
        ss << girlName << " went around wearing her Oiran Dress today making her feel quite tough.\n \n";
        girl.combat(1);
        girl.confidence(1);
    }
    else if (girl.has_item_j("Ladonna's Dress") != -1)
    {
        ss << girlName << " went around wearing Ladonna's Dress today making her look fashonable but hideous.\n \n";
        girl.pchate(g_Dice % 2);
        girl.refinement(1);
        girl.dignity(-1);
    }
    else if (girl.has_item_j("Jessica's Dress") != -1)
    {
        ss << girlName << " went around wearing Jessica's Dress today making her look extremely sexy.\n \n";
        girl.libido(g_Dice % 10);
        girl.beauty(1);
        girl.strip(1);
        girl.refinement(-1);
        girl.dignity(1);
    }
    else if (girl.has_item_j("Minerva's Dress") != -1)
    {
        ss << girlName << " went around wearing Minerva's Dress today making her look quite slinky.\n \n";
        girl.libido(g_Dice % 10);
        girl.beauty(1);
        girl.strip(1);
        girl.refinement(-1);
        girl.dignity(1);
    }

























    if (g_Dice.percent(15) && (
            girl.has_item_j("Chainmail Bikini") != -1 ||
            girl.has_item_j("Goron Blessed Swimsuit") != -1 ||
            girl.has_item_j("Kokiri Blessed Swimsuit") != -1 ||
            girl.has_item_j("Red Swimsuit") != -1 ||
            girl.has_item_j("Zora Blessed Swimsuit") != -1 ||
            girl.has_item_j("White String Bikini") != -1 ||
            girl.has_item_j("Black String Bikini") != -1))
    {
#if 1
        string wearwhat; string wearwhere;
        while (wearwhat.empty())
        {
            switch (g_Dice % 7)
            {
            case 0:		if (girl.has_item_j("Chainmail Bikini") != -1)			wearwhat = "Chainmail Bikini";			break;
            case 1:		if (girl.has_item_j("Goron Blessed Swimsuit") != -1)		wearwhat = "Goron Blessed Swimsuit";	break;
            case 2:		if (girl.has_item_j("Kokiri Blessed Swimsuit") != -1)	wearwhat = "Kokiri Blessed Swimsuit";	break;
            case 3:		if (girl.has_item_j("Red Swimsuit") != -1)				wearwhat = "Red Swimsuit";				break;
            case 4:		if (girl.has_item_j("Zora Blessed Swimsuit") != -1)		wearwhat = "Zora Blessed Swimsuit";		break;
            case 5:		if (girl.has_item_j("White String Bikini") != -1)		wearwhat = "White String Bikini";		break;
            default:	if (girl.has_item_j("Black String Bikini") != -1)		wearwhat = "Black String Bikini";		break;
            }
        }
        while (wearwhere.empty())
        {
            switch (g_Dice % 4)
            {
            case 0:		wearwhere = "down to the beach";			break;
            case 1:		wearwhere = "down to the pool";				break;
            case 2:		wearwhere = "up to the roof to sunbathe";	break;
            default:	wearwhere = "around town";					break;
            }
        }
        ss << girlName << " went " << wearwhere << " wearing her " << wearwhat << " today.\n \n";
        girl.happiness(g_Dice % 10);
        swim = true;
#endif
    }
    if (girl.has_item_j("Maid Uniform") != -1 && g_Dice.percent(5) && girl.is_resting())
    {
        ss << "She put on her Maid Uniform and cleaned up.\n \n";
        m_Filthiness -= 5;
        maid = true;
    }

    // Legs
    if (girl.has_item_j("Rainbow Underwear") != -1)
    {
    }
    if (girl.has_item_j("Fishnet Stocking") != -1)
    {
        ss << girlName << " went around wearing her Fishnet Stockings today making her look sexier even if it did make her feel a litte trashy.\n \n";
    }




    // Part time use
    if (girl.has_item_j("Apron") != -1 && g_Dice.percent(10))
    {
        ss << "She put on her Apron and cooked a meal for some of the girls.\n \n";
        girl.upd_Enjoyment(ACTION_WORKCOOKING, 1);
        girl.happiness(5);
        cook = true;
    }







#endif


    // Robots, Decorations and things that get used without her
#if 1
    if (girl.has_item_j("Android, Assistance") != -1 && g_Dice.percent(50))
    {
        ss << "Her Assistance Android swept up and took out the trash for her.\n \n";
        m_Filthiness -= 10;
    }
    if (girl.has_item_j("Claptrap") != -1 && g_Dice.percent(10))
    {
        ss << "Thanks to Claptrap's sense of humor she is a better mood.\n \n";
        girl.happiness(5);
    }
    if (girl.has_item_j("Pet Spider") != -1 && g_Dice.percent(15))
    {
        ss << girlName;
        if (girl.has_trait( "Nerd"))
        {
            ss << " watches her Pet Spider, studying it and occasionally jotting down notes.\n \n";
            girl.happiness(1 + g_Dice % 3);
        }
        else if (girl.has_trait( "Meek"))
        {
            ss << "'s Meek nature makes her cover her Pet Spiders cage so it doesn't scare her.\n \n";
            girl.happiness(1);
        }
        else if (girl.has_trait("Aggressive") || girl.has_trait("Assassin") || girl.has_trait("Merciless"))
        {
            ss << " throws in some food to her Pet Spider and smiles while she watchs it kill its prey.\n \n";
            girl.happiness(1 + g_Dice % 5);
        }
        else
        {
            ss << " plays with her pet spider.\n \n";
            girl.happiness(1 + g_Dice % 5);
        }
    }
    if ((girl.has_item_j("Cat") != -1 || girl.has_item_j("Black Cat") != -1) && g_Dice.percent(10))
    {
        if (girl.is_resting())
        {
            ss << "Spent her time off with her pet Cat.\n \n";
            girl.happiness(1 + g_Dice % 10);
        }
        else
        {
            ss << "She cuddles with her Cat as she falls asleep.\n \n";
            girl.happiness(1 + g_Dice % 5);
            girl.tiredness(-1);
        }
    }
    if (girl.has_item_j("Guard Dog") != -1 && g_Dice.percent(15))
    {
        if (girl.has_trait( "Meek"))
        {
            ss << girlName << "'s Meek nature makes her glad she has her Guard Dog to protect her.\n \n";
            girl.pcfear(-1);
        }
        else if (girl.has_trait( "Aggressive"))
        {
            ss << girlName << " seeks her Guard Dog on some random patrons and laughs while they run scared.\n \n";
        }
        else
        {
            ss << girlName << " plays with her pet Guard Dog.\n \n";
        }
        girl.confidence(g_Dice % 2);
        girl.happiness(1 + g_Dice % 5);
    }

    if (girl.has_item_j("Room Decorations") != -1 && g_Dice.percent(3))
    {
        ss << "She looks around at her Room Decorations and smiles, she really likes that her room is a little better then most the other girls.\n \n";
        girl.happiness(5);
    }
    if (girl.has_item_j("Appreciation Trophy") != -1 && g_Dice.percent(5))
    {
#if 1
        if (is_she_cleaning(girl))
        {
            ss << "While cleaning, " << girlName << " came across her Appreciation Trophy";
            if (HateLove > 60)
            {
                ss << ". She stopped and took it down off the shelf, polished it and placed it back front and center";
                girl.pclove(g_Dice.bell(1, 4));
                girl.pclove(g_Dice.bell(-4, -1));
                girl.happiness(g_Dice.bell(1, 6));
            }
            else if (HateLove > 10)
            {
                ss << " and smiled";
                girl.pclove(1);
                girl.pchate(-1);
                girl.happiness(g_Dice % 4);
            }
            else if (HateLove > 30)
            {
                ss << ". She looked at it for a bit then continued cleaning";
            }
            else if (HateLove > 80)
            {
                ss << ". She looked at it incredulously for a bit then pushed it back behind some other things on the shelf";
                girl.pclove(g_Dice.bell(-2, 0));
                girl.pchate(g_Dice.bell(0, 2));
            }
            else
            {
                ss << ". She looked at it angerly for a bit then threw it in the trash";
                girl.pclove(g_Dice.bell(-5, -1));
                girl.pchate(g_Dice.bell(2, 5));
                girl.remove_inv(girl.has_item_j("Appreciation Trophy"));
            }
        }
        else
        {
            ss << girlName << " took her Appreciation Trophy off the shelf";
            if (HateLove > 50)
            {
                ss << ", polished it and placed it back front and center";
                girl.pclove(g_Dice.bell(1, 6));
                girl.pclove(g_Dice.bell(-6, -1));
                girl.happiness(g_Dice.bell(1, 10));
            }
            else if (HateLove > 0)
            {
                ss << ", looked at it for a while and smiled";
                girl.pclove(g_Dice.bell(0, 3));
                girl.pclove(g_Dice.bell(-3, 0));
                girl.happiness(g_Dice.bell(0, 6));
            }
            else if (HateLove > 70)
            {
                ss << ". She looked at it incredulously for a bit then pushed it back behind some other things on the shelf";
                girl.pclove(g_Dice.bell(-4, 1));
                girl.pchate(g_Dice.bell(-1, 4));
            }
            else
            {
                ss << ". She looked at it angerly for a bit then threw it in the trash";
                girl.pclove(g_Dice.bell(-7, -2));
                girl.pchate(g_Dice.bell(2, 7));
                girl.remove_inv(girl.has_item_j("Appreciation Trophy"));
            }
        }
        ss << ".\n \n";
#endif
    }

    // Dream Orbs
#if 1
    if (girl.has_item_j("Nightmare Orb") != -1 && g_Dice.percent(50))
    {
        if (girl.pclove() > girl.pcfear() && g_Dice.percent(girl.pclove()))
        {
            if (g_Dice.percent(50))	ss << girlName << " comes to you and tells you she had a scary dream about you.\n \n";
            girl.pcfear(g_Dice % 2);
            girl.happiness(-1);
            girl.tiredness(1);
        }
        else if (girl.has_trait("Masochist") || girl.has_trait("Twisted"))	// she liked it
        {
            girl.pcfear(g_Dice % 2);
            girl.happiness(g_Dice % 3);
            girl.tiredness(g_Dice % 2);
        }
        else	// everyone else
        {
            girl.pcfear(g_Dice % 3);
            girl.happiness(-(g_Dice % 3));
            girl.tiredness(g_Dice % 4);
        }
    }
    if (girl.has_item_j("Lovers Orb") != -1 && g_Dice.percent(50))
    {
        if (girl.pclove() > girl.pchate() && g_Dice.percent(girl.pclove()))
        {
            girl.pclove(1 + g_Dice % 3);
            girl.pcfear(-(g_Dice % 3));
            girl.pchate(-(g_Dice % 3));
            girl.happiness(3 + g_Dice % 3);
            girl.tiredness(1 + g_Dice % 3);
            girl.npclove(-(1 + g_Dice % 3));
            if (g_Dice.percent(50))
            {
                ss << girlName << " comes to you and tells you she had a sexy dream about you.";
                if (girl.has_trait("Lesbian") && g_Game->player().Gender() >= GENDER_HERMFULL && g_Dice.percent(girl.pclove() / 10))
                {
                    girl.remove_trait("Lesbian");
                    girl.add_trait("Bisexual");
                    ss << "  \"Normally I don't like men but for you I'll make an exception.\"";
                }
                if (girl.has_trait("Straight") && g_Game->player().Gender() <= GENDER_FUTAFULL && g_Dice.percent(girl.pclove() / 10))
                {
                    girl.remove_trait("Straight");
                    girl.add_trait("Bisexual");
                    ss << "  \"Normally I don't like women but for you I'll make an exception.\"";
                }
                ss << "\n \n";
            }
        }
        else	// everyone else
        {
            girl.pclove(g_Dice % 3);
            girl.pcfear(-(g_Dice % 2));
            girl.pchate(-(g_Dice % 2));
            girl.happiness(1 + g_Dice % 3);
            girl.tiredness(1 + g_Dice % 3);
        }
    }
    if (girl.has_item_j("Happy Orb") != -1 && g_Dice.percent(50))
    {
        if (girl.pclove() > girl.pcfear() && g_Dice.percent(girl.pclove()))
        {
            if (g_Dice.percent(50))
                ss << girlName << " comes to you and tells you she had a happy dream about you.\n \n";
            girl.happiness(4 + g_Dice % 5);
            girl.pclove(g_Dice % 2);
            girl.pcfear(-(g_Dice % 2));
            girl.pchate(-(g_Dice % 2));
        }
        else	// everyone else
        {
            girl.happiness(3 + g_Dice % 3);
        }
    }
    if (girl.has_item_j("Chastity Orb") != -1 && g_Dice.percent(50))
    {
        if (girl.pclove() > girl.pcfear() && g_Dice.percent(girl.pclove()))
        {
            if (g_Dice.percent(50))
            {
                ss << girlName << " comes to you and tells you ";
                if (girl.is_havingsex())	ss << "she does not like having sex anymore and wants to do something more constructive with her life.\n \n";
                else
                {
                    ss << "is glad you are not making her have sex with anyone";
                    if (girl.pclove() > 80) ss << " but you";
                    ss << ".";
                    girl.pclove(g_Dice % 2);
                    girl.happiness(5 + g_Dice % 6);
                }
            }
            girl.pcfear(-(g_Dice % 2));
            girl.pchate(-(g_Dice % 2));
        }
        else	// everyone else
        {
            girl.happiness(3 + g_Dice % 3);
        }
        girl.morality(g_Dice % 3);
        girl.dignity(g_Dice % 2);
        girl.libido(-(5 + g_Dice % 20));
    }
    if (girl.has_item_j("Relaxation Orb") != -1 && g_Dice.percent(50))
    {
        if (g_Dice.percent(50))		ss << girlName << " looks extremely relaxed.\n \n";
        girl.happiness(2 + g_Dice % 4);
        girl.tiredness(-(g_Dice % 10));
        girl.pcfear(-(g_Dice % 2));
        girl.pchate(-(g_Dice % 2));
    }
#endif

#endif


    // Books and reading materials
#if 1
    // first list all books to see if she has any
    if (girl.has_item_j("Manual of Sex") != -1 ||
        girl.has_item_j("Manual of Bondage") != -1 ||
        girl.has_item_j("Manual of Two Roses") != -1 ||
        girl.has_item_j("Manual of Arms") != -1 ||
        girl.has_item_j("Manual of the Dancer") != -1 ||
        girl.has_item_j("Manual of Magic") != -1 ||
        girl.has_item_j("Manual of Health") != -1 ||
        girl.has_item_j("Library Card") != -1)
    {
#if 1
        int numbooks = girl.intelligence() / 30;	// how many books can she read?
        if (girl.has_trait( "Blind"))				numbooks = 1;
        else
        {
            if (girl.has_trait( "Nerd"))				numbooks += 1;
            if (girl.has_trait( "Quick Learner"))	numbooks += 1;
            if (girl.has_trait( "Slow Learner"))		numbooks -= 2;
            if (girl.has_trait( "Bimbo"))			numbooks -= 1;
        }
        if (numbooks < 1)				numbooks = 1;

        // then see if she wants to read any
        if (girl.has_item_j("Manual of Sex") != -1 && girl.normalsex() < 100 && g_Dice.percent(5) && numbooks > 0)
        {
            ss << "Spent her time off reading her Manual of Sex.\n \n";
            girl.normalsex(2);
            numbooks--;
        }
        if (girl.has_item_j("Manual of Bondage") != -1 && girl.bdsm() < 100 && g_Dice.percent(5) && numbooks > 0)
        {
            ss << "Spent her time off reading her Manual of Bondage.\n \n";
            girl.bdsm(2);
            numbooks--;
        }
        if (girl.has_item_j("Manual of Two Roses") != -1 && girl.lesbian() < 100 && g_Dice.percent(5) && numbooks > 0)
        {
            ss << "Spent her time off reading her Manual of Two Roses.\n \n";
            girl.lesbian(2);
            numbooks--;
        }
        if (girl.has_item_j("Manual of Arms") != -1 && girl.combat() < 100 && g_Dice.percent(5) && numbooks > 0)
        {
            ss << "Spent her time off reading her Manual of Arms.\n \n";
            girl.combat(2);
            numbooks--;
        }
        if (girl.has_item_j("Manual of the Dancer") != -1 && girl.strip() < 100 && g_Dice.percent(5) && numbooks > 0)
        {
            ss << "Spent her time off reading her Manual of the Dacer.\n \n";
            girl.strip(2);
            numbooks--;
        }
        if (girl.has_item_j("Manual of Magic") != -1 && girl.magic() < 100 && g_Dice.percent(5) && numbooks > 0)
        {
            ss << "Spent her time off reading her Manual of Magic.\n \n";
            girl.magic(2);
            numbooks--;
        }
        if (girl.has_item_j("Manual of Health") != -1 && girl.constitution() < 100 && g_Dice.percent(5) && numbooks > 0)
        {
            ss << "Spent her time off reading her Manual of Health.\n \n";
            girl.constitution(1);
            numbooks--;
        }

        // She may go to the library if she runs out of books to read
        if (girl.has_item_j("Library Card") != -1 && numbooks > 0 && (g_Dice.percent(20) || (girl.has_trait("Nerd") && g_Dice.percent(50))))
        {
            if (girl.has_trait( "Nymphomaniac"))
            {
                ss << "She spent the day at the Library looking at porn making her become horny.\n \n";
                girl.upd_temp_stat(STAT_LIBIDO, 15);
            }
            else
            {
                ss << "She spent her free time at the library reading.";
                if (g_Dice.percent(5))		girl.intelligence(1);
                if (g_Dice.percent(5))
                {
                    int upskill = g_Dice%NUM_SKILLS;
                    int upskillg = g_Dice % 4 - 1;
                    ss << " She found a book on " << get_skill_name((SKILLS)upskill);
                    if (upskillg > 0)
                    {
                        ss << " and gained " << upskillg << " points in it.";
                        girl.upd_skill(upskill, upskillg);
                    }
                    else ss << "but didn't find it very useful.";
                }
                ss << "\n \n";
            }
        }
#endif
    }

    if (girl.has_item_j("Journal") != -1 && g_Dice.percent(15))
    {
#if 1
        if (girl.has_trait( "Nerd") && g_Dice.percent(30))
        {
            ss << "She decide to write on her novel some today.\n \n";
            girl.happiness(g_Dice % 2);
            girl.intelligence(g_Dice % 2);
        }
        else if (girl.has_trait( "Bimbo") && g_Dice.percent(30))
        {
            ss << "She doodled silly pictures in her journal.\n \n";
            girl.happiness(g_Dice % 3);
        }
        else if (girl.carrying_human() && g_Dice.percent(30))
        {
            ss << "She wrote baby names in her journal";
            if (girl.carrying_players_child())
            {
                if (HateLove > 60)
                {
                    ss << ", all of them with your last name";
                    girl.pclove(g_Dice % 5);
                    girl.pchate(-(g_Dice % 5));
                    girl.pcfear(-(g_Dice % 3));
                    girl.happiness(g_Dice % 10);
                }
                else if (HateLove > 20)
                {
                    ss << ", sometimes with your last name";
                    girl.pclove(g_Dice % 2);
                    girl.pchate(-(g_Dice % 2));
                    girl.happiness(g_Dice % 5);
                }
                else if (HateLove > -20)
                {
                    ss << ", sometimes with your last name but usually with her last name or none at all";
                    girl.happiness(g_Dice % 3);
                }
                else if (HateLove > -60)
                {
                    ss << ", sometimes when she uses your last name she scratches it out";
                    girl.pclove(-(g_Dice % 5));
                    girl.pchate(g_Dice % 5);
                    girl.happiness(g_Dice.bell(-3, 3));
                }
                else
                {
                    ss << ", she never uses your last name or any name similar to yours";
                    girl.pclove(-(g_Dice % 10));
                    girl.pchate(g_Dice % 10);
                    girl.pcfear(g_Dice % 5);
                    girl.happiness(-(g_Dice % 5));
                }
            }
            else
            {
                if (HateLove > 60)
                {
                    ss << ", sometimes she would use your last name hopeing you would take her child as your own";
                    girl.pclove(g_Dice % 3);
                    girl.pchate(-(g_Dice % 3));
                    girl.pcfear(-(g_Dice % 2));
                }
                girl.happiness(g_Dice % 5);
            }

            ss << ".\n \n";
        }
        else
        {
            string thoughts;
            switch (g_Dice % 20)
            {
            case 0:		girl.happiness(1);					thoughts = " happy";		break;
            case 1:		girl.happiness(-1);				thoughts = " sad";			break;
            case 2:		girl.happiness(1 + g_Dice % 3);	thoughts = " fun";			break;
            case 3:		girl.intelligence(1);				thoughts = " interesting";	break;
            case 4:		girl.spirit(1);					thoughts = " positive";		break;
            case 5:		girl.spirit(-1);					thoughts = " negative";		break;
            case 6:		girl.obedience(1);					thoughts = " helpful";		break;
            case 7:		girl.obedience(-1);				thoughts = " anoying";		break;
            case 8:		girl.pclove(1);					thoughts = " loving";		break;
            case 9:		girl.pclove(-1);					thoughts = " unloving";		break;
            case 10:	girl.pchate(1);					thoughts = " hateful";		break;
            case 11:	girl.pchate(-1);					thoughts = " carefree";		break;
            case 12:	girl.pcfear(1);					thoughts = " fearful";		break;
            case 13:	girl.pcfear(-1);					thoughts = " fearless";		break;
            case 14:	girl.dignity(1);					thoughts = " proper";		break;
            case 15:	girl.dignity(-1);					thoughts = " slutty";		break;
            case 16:	girl.libido(-1);					thoughts = " tame";			break;
            case 17:	girl.libido(1);					thoughts = " sexy";			break;
            default:	break;
            }
            ss << "She used her Journal to write some of her" << thoughts << " thoughts down today.\n \n";
        }
#endif
    }

#endif	// End Books and reading materials

    ////////////////////
    // Unsorted items //
    ////////////////////


    if (girl.has_item_j("Free Weights") != -1 && g_Dice.percent(15))
    {
        ss << girl.m_Realname << " spent some time working out with her Free Weights.\n \n";
        if (g_Dice.percent(5))	girl.beauty(1);		// working out will help her look better
        if (g_Dice.percent(10))	girl.constitution(1);	// working out will make her healthier
        if (g_Dice.percent(50))	girl.strength(1);		// working out will make her stronger
    }
    if (girl.has_item_j("Television Set") != -1)
    {
        if (girl.is_resting())
        {
            ss << girl.m_Realname << " spent most of her day lounging in front of her Television Set.\n \n";
            girl.tiredness(-5);
            if (g_Dice.percent(5))		girl.intelligence(1);
        }
        else
        {
            ss << "At the end of her long day, " << girlName << " flopped down in front of her Television Set and relaxed.\n \n";
            girl.tiredness(-3);
        }
    }
    if (girl.has_item_j("Art Easel") != -1 && g_Dice.percent(girl.fame() / 10))
    {
#if 1
        string paintingtype;
        int sale = ((1 + g_Dice % 30) * max(1, (girl.fame()))) / 10;
        switch (g_Dice % 50)											// start with simple painting types
        {
        case 0:		girl.happiness(1);					paintingtype = "happy";			break;
        case 1:		girl.happiness(-1);				paintingtype = "sad";			break;
        case 2:		girl.happiness(1 + g_Dice % 3);	paintingtype = "fun";			break;
        case 3:		girl.intelligence(1);				paintingtype = "interesting";	break;
        case 4:		girl.spirit(1);					paintingtype = "positive";		break;
        case 5:		girl.spirit(-1);					paintingtype = "negative";		break;
        case 6:		girl.animalhandling(1);			paintingtype = "animal";		break;
        case 7:		girl.combat(1);					paintingtype = "combat";		break;
        case 8:		girl.pclove(1);					paintingtype = "loving";		break;
        case 9:		girl.pclove(-1);					paintingtype = "angry";			break;
        case 10:	girl.pchate(1);					paintingtype = "hateful";		break;
        case 11:	girl.pchate(-1);					paintingtype = "carefree";		break;
        case 12:	girl.pcfear(1);					paintingtype = "fearful";		break;
        case 13:	girl.pcfear(-1);					paintingtype = "fearless";		break;
        case 14:	girl.dignity(1);					paintingtype = "proper";		break;
        case 15:	girl.dignity(-1);					paintingtype = "slutty";		break;
        case 16:	girl.libido(-1);					paintingtype = "tame";			break;
        case 17:	girl.libido(1);					paintingtype = "sexy";			break;
        default:	break;
        }
        if (!paintingtype.empty())	paintingtype += " paintings";
        else															// if no simple type is chosen, do special types
        {
            if (HateLove > 50 && g_Dice.percent(10))
            {
                paintingtype = "protraits of you";
                girl.pclove(g_Dice.bell(1, 4));
                girl.happiness(g_Dice.bell(3, 10));
            }
            else if (girl.magic() > 50 && g_Dice.percent(10))
            {
                paintingtype = "magical moving paintings";
                sale *= 3;
                girl.magic(1);
            }
            else if (g_Dice.percent(10))
            {
                paintingtype = "self protraits";
                girl.happiness(g_Dice.bell(3, 10));
                sale += girl.beauty();
            }
            else if (g_Dice.percent(10))
            {
                paintingtype = "nude self protraits";
                sale += girl.beauty() * 2;
            }
        }
        if (paintingtype.empty())	paintingtype = "paintings";				// if no type is chosen, just a regular painting

        ss << girl.m_Realname << " managed to sell one of her " << paintingtype << " for " << sale << " gold.\n \n";
        girl.m_Money += sale;
        girl.happiness(sale / 10);
        girl.confidence(sale / 25);
        girl.fame(sale / 50);
#endif
    }
    if (girl.has_item_j("The Realm of Darthon") != -1 && girl.is_resting() && g_Dice.percent(5))
    {
        ss << "Spent her time playing The Realm of Darthon with some of the other girls.\n \n";
        girl.happiness(5);
    }
    if (girl.has_item_j("Compelling Dildo") != -1)
    {
        if (girl.libido() > 65 && girl.is_resting())
        {
            ss << girlName << "'s lust got the better of her and she spent the day using her Compelling Dildo.\n \n";
            girl.upd_temp_stat(STAT_LIBIDO, -20);
            mast = true;
        }
    }
    if (girl.has_item_j("Stripper Pole") != -1 && g_Dice.percent(10))
    {
        if (girl.is_resting())
        {
            ss << "Spent her time off practicing on her Stripper Pole.\n \n";
            girl.strip(2);
            strip = true;
        }
    }
    if (girl.has_item_j("Android, Combat MK I") != -1 && g_Dice.percent(5))
    {
        if (girl.is_resting())
        {
            ss << "Spent her time off training with her Android, Combat MK I.\n \n";
            girl.combat(1);
            combat = true;
        }
    }
    if (girl.has_item_j("Android, Combat MK II") != -1 && g_Dice.percent(10))
    {
        if (girl.is_resting())
        {
            ss << "Spent her time off training with her Android, Combat MK II.\n \n";
            girl.combat(2);
            combat = true;
        }
    }
    if (girl.has_item_j("Computer") != -1 && g_Dice.percent(15) && girl.is_resting())
    {
        if (girl.has_trait( "Nymphomaniac"))
        {
            if (girl.libido() > 65)
            {
                ss << girlName << "'s lust got the better of her while she was on the her Computer looking at porn.\n \n";
                girl.upd_temp_stat(STAT_LIBIDO, -20);
                mast = true;
            }
            else
            {
                ss << "She spent the day on her Computer looking at porn making her become horny.\n \n";
                girl.upd_temp_stat(STAT_LIBIDO, 15);
            }
        }
        else
        {
            ss << "She spent her free time playing on her Computer.\n \n";
            if (g_Dice.percent(5))		girl.intelligence(1);
        }
    }

    if (girl.has_item_j("Anger Management Tapes") != -1 && g_Dice.percent(2))
    {
        if (girl.is_resting())
        {
            ss << "Spent her time off listen to her Anger Management Tapes.\n \n";
            girl.spirit(-2);
        }
    }
    if (girl.has_item_j("Short Sword") != -1 && g_Dice.percent(5))
    {
        if (girl.is_resting())
        {
            if (girl.intelligence() > 65)
            {
                ss << girlName << " sharpened her Short Sword making it more ready for combat.\n \n";
                girl.upd_temp_skill(SKILL_COMBAT, 2);
            }
            else
            {
                ss << girlName << " tried to sharpen her Short Sword but doesn't have the brains to do it right.\n \n";
                girl.upd_temp_skill(SKILL_COMBAT, -2);
            }
        }
    }




    if (girl.has_item_j("Chrono Bed") != -1)
    {
        ss << "Thanks to her Chrono Bed she got a great nights sleep and woke up feeling wonderful.\n \n";
        girl.health(25);
        girl.tiredness(-50);
    }
    else if (girl.has_item_j("Rejuvenation Bed") != -1)
    {
        ss << "Thanks to her Rejuvenation Bed she got a great nights sleep and woke up feeling better.\n \n";
        girl.health(10);
        girl.tiredness(-10);
    }

    if (ss.str().length()>0)		// only pass the summary if she has any of the items listed
    {
        int imagetype = IMGTYPE_PROFILE;
        /* */if (mast)		imagetype = IMGTYPE_MAST;
        else if (strip)		imagetype = IMGTYPE_STRIP;
        else if (combat)	imagetype = IMGTYPE_COMBAT;
        else if (formal)	imagetype = IMGTYPE_FORMAL;
        else if (swim)		imagetype = IMGTYPE_SWIM;
        else if (cook)		imagetype = IMGTYPE_COOK;
        else if (maid)		imagetype = IMGTYPE_MAID;

        girl.m_Events.AddMessage(ss.str(), imagetype, EVENT_SUMMARY);

    }
}

double calc_pilfering(sGirl& girl)
{
    double factor = 0.0;
    if (girl.is_addict() && girl.m_Money < 100)			// on top of all other factors, an addict will steal to feed her habit
        factor += (girl.is_addict(true) ? 0.5 : 0.1);		// hard drugs will make her steal more
    // let's work out what if she is going steal anything
    if (girl.pclove() >= 50 || girl.obedience() >= 50) return factor;			// love or obedience will keep her honest
    if (girl.pcfear() > girl.pchate()) return factor;							// if her fear is greater than her hate, she won't dare steal
    // `J` yes they do // if (girl.is_slave()) return factor;					// and apparently, slaves don't steal
    if (girl.pchate() > 40) return factor + 0.15;								// given all the above, if she hates him enough, she'll steal
    if (girl.confidence() > 70 && girl.spirit() > 50) return factor + 0.15;	// if she's not motivated by hatred, she needs to be pretty confident
    return factor;	// otherwise, she stays honest (aside from addict factored-in earlier)
}


void IBuilding::CalculatePay(sGirl& girl, u_int Job)
{
// no pay or tips, no need to continue
    if (girl.m_Pay <= 0 && girl.m_Tips <= 0) { girl.m_Pay = girl.m_Tips = 0; return; }

    if (girl.m_Tips > 0)		// `J` check tips first
    {
        if ((cfg.initial.girls_keep_tips() && !girl.is_slave()) ||	// if free girls tips are counted sepreatly from pay
            (cfg.initial.slave_keep_tips() && girl.is_slave()))		// if slaves tips are counted sepreatly from pay
        {
            girl.m_Money += girl.m_Tips;	// give her the tips directly
        }
        else	// otherwise add tips into pay
        {
            girl.m_Pay += girl.m_Tips;
        }
    }
    girl.m_Tips = 0;
    // no pay, no need to continue
    if (girl.m_Pay <= 0) { girl.m_Pay = 0; return; }

    // if the house takes nothing		or if it is a player paid job and she is not a slave
    if (girl.m_Stats[STAT_HOUSE] == 0 || (g_Game->job_manager().is_job_Paid_Player(Job) && !girl.is_slave()) ||
        // or if it is a player paid job	and she is a slave		but you pay slaves out of pocket.
        (g_Game->job_manager().is_job_Paid_Player(Job) && girl.is_slave() && cfg.initial.slave_pay_outofpocket()))
    {
        girl.m_Money += girl.m_Pay;	// she gets it all
        girl.m_Pay = 0;
        return;
    }

    // so now we are to the house percent.
    float house_factor = float(girl.m_Stats[STAT_HOUSE]) / 100.0f;

    // work out how much gold (if any) she steals
    double steal_factor = calc_pilfering(girl);
    int stolen = int(steal_factor * girl.m_Pay);
    girl.m_Pay -= stolen;
    girl.m_Money += stolen;


    int house = int(house_factor * girl.m_Pay);			// the house takes its cut of whatever's left
    if (house > girl.m_Pay) house = girl.m_Pay;			// this shouldn't happen. That said...

    girl.m_Money += girl.m_Pay - house;					// The girl collects her part of the pay
    m_Finance.brothel_work(house);					// and add the rest to the brothel finances
    girl.m_Pay = 0;										// clear pay
    if (girl.m_Money < 0) girl.m_Money = 0;				// Not sure how this could happen - suspect it's just a sanity check

    if (!stolen) return;									// If she didn't steal anything, we're done
    sGang* gang = g_Game->gang_manager().GetGangOnMission(MISS_SPYGIRLS);	// if no-one is watching for theft, we're done
    if (!gang) return;
    int catch_pc = g_Game->gang_manager().chance_to_catch(&girl);			// work out the % chance that the girl gets caught
    if (!g_Dice.percent(catch_pc)) return;					// if they don't catch her, we're done

    // OK: she got caught. Tell the player
    stringstream gmess; gmess << "Your Goons spotted " << girl.m_Realname << " taking more gold then she reported.";
    gang->m_Events.AddMessage(gmess.str(), IMGTYPE_PROFILE, EVENT_GANG);
}

sGirl* IBuilding::find_random_girl() const
{
    if(m_Girls.empty())
        return nullptr;

    int index = g_Dice % m_Girls.size();
    return *std::next(m_Girls.begin(), index);
}

int IBuilding::free_rooms() const
{
    return m_NumRooms - m_Girls.size();
}

int IBuilding::num_rooms() const
{
    return m_NumRooms;
}

void IBuilding::set_background_image(std::string img)
{
    m_BackgroundImage = std::move(img);
}
