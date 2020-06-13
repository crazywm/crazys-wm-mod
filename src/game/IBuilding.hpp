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

#pragma once

#ifndef CRAZYS_WM_MOD_IBUILDING_HPP
#define CRAZYS_WM_MOD_IBUILDING_HPP

#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <unordered_set>
#include "utils/streaming_random_selection.hpp"

#include "cEvents.h"
#include "Constants.h"
#include "cGold.h"
#include "cGirls.h"

#include "tinyxml2.h"
#include "character/sGirl.hpp"

class IBuilding
{
public:
    explicit IBuilding(BuildingType type, std::string name);
    virtual ~IBuilding();

    IBuilding(const IBuilding&) = delete;

    virtual void auto_assign_job(sGirl* target, std::stringstream& message, bool is_night) = 0;

    // meeting new girls
    virtual sGirl* meet_girl() const;

    // info functions
    BuildingType type() const { return m_Type; };
    const char* type_str() const;
    const std::string& name() const { return m_Name; }
    void set_name(std::string new_name) { m_Name = std::move(new_name); }

    // display data
    const std::string& background_image() const { return m_BackgroundImage; }
    void set_background_image(std::string img);

    int num_girls() const;

    int num_rooms() const;
    int free_rooms() const;

    int filthiness() const;
    int security() const;

    bool matron_on_shift(int shift) const; // `J` added building checks
    int matron_count() const;

    void update_all_girls_stat(STATS stat, int amount);

    // xml helpers
    void save_girls_xml(tinyxml2::XMLElement& target) const;
    void save_settings_xml(tinyxml2::XMLElement& root) const;
    virtual void save_additional_xml(tinyxml2::XMLElement& root) const {};
    void load_girls_xml(tinyxml2::XMLElement& root);
    void load_settings_xml(tinyxml2::XMLElement& root);

    virtual void load_xml(tinyxml2::XMLElement& root);
    virtual void save_xml(tinyxml2::XMLElement& root) const;

    // girl management
    const std::list<sGirl*>& girls() const { return m_Girls; }
    sGirl* get_girl(int index);
    int get_girl_index(const sGirl& girl) const;
    void add_girl(sGirl* girl, bool keep_job = false);
    void remove_girl(sGirl* girl);

    // predicate based lookup. Options are: find first, find random, find all, count
    // the methods named find_ all return a sGirl* and may return nullptr if no girl
    // is found.
    template<class F>
    sGirl* find_first_girl(F&& predicate) const {
        auto found = std::find_if(begin(m_Girls), end(m_Girls), predicate);
        if(found == end(m_Girls)) {
            return nullptr;
        } else {
            return *found;
        }
    }

    template<class F>
    sGirl* find_random_girl_with(F&& predicate) const {
        RandomSelector<sGirl> selector;
        for(auto& girl : m_Girls) {
            if(predicate(girl)) {
                selector.process(girl);
            }
        }
        return selector.selection();
    }

    template<class F>
    int num_girls_with(F predicate) const {
        return std::count_if(begin(m_Girls), end(m_Girls), predicate);
    }

    template<class F>
    std::vector<sGirl*> find_all_girls(F&& predicate) const {
        std::vector<sGirl*> found;
        std::copy_if(begin(m_Girls), end(m_Girls), std::back_inserter(found), std::forward<F>(predicate));
        return std::move(found);
    }

    // utilities: common uses of the functions above
    sGirl* find_random_girl() const;
    sGirl* find_girl_by_name(std::string name) const;
    int    num_girls_on_job(JOBS jobID, int is_night = 0) const;
    std::vector<sGirl*> girls_on_job(JOBS jobID, int is_night = 0) const;


    int total_fame() const;

    // private:

    // potions
    int  GetNumPotions()                    { return m_AntiPregPotions; }
    void KeepPotionsStocked(bool stocked)    { m_KeepPotionsStocked = stocked; }
    bool GetPotionRestock()                    { return m_KeepPotionsStocked; }
    void AddAntiPreg(int amount);

    int     m_AntiPregPotions    = 0;          // `J` added so all buildings save their own number of potions
    int     m_AntiPregUsed       = 0;          // `J` number of potions used last turn
    bool m_KeepPotionsStocked = false;        // `J` and if they get restocked

    /// returns true if an anti-preg potion is available and subtracts that potion from the stock.
    /// also handles auto-buy if no potion is in stock.
    bool provide_anti_preg();

    // rules
    void set_sex_type_allowed(SKILLS sex_type, bool is_allowed=true);
    bool is_sex_type_allowed(SKILLS sex_type) const;
    bool nothing_banned() const;

    JOBS m_RestJob;
    JOBS m_FirstJob;
    JOBS m_LastJob;
    JOBS m_MatronJob;

    int             m_id   = 0;
    int    m_Filthiness       = 0;
    int    m_SecurityLevel    = 0;

    cEvents m_Events;

    unsigned short    m_AdvertisingBudget = 0;        // Budget player has set for weekly advertising
    double            m_AdvertisingLevel  = 0;        // multiplier for how far budget goes, based on girls working in advertising

    int                m_TotalCustomers = 0;            // the total number of customers for the last week
    int                m_RejectCustomersRestrict = 0;    // How many customers were turned away by your sex restrictions.
    int                m_RejectCustomersDisease  = 0;    // How many customers were turned away because of disease.
    int                m_MiscCustomers = 0;            // customers used for temp purposes but must still be taken into account

    unsigned char    m_Fame = 0;                        // How famous this brothel is
    unsigned short    m_Happiness   = 0;                // av. % happy customers last week
    int                m_NumRooms    = 0;                // How many rooms it has
    int                m_MaxNumRooms = 0;                // How many rooms it can have
    cGold            m_Finance;                      // for keeping track of how well the place is doing (for the last week)

    // helper functions
    void BeginWeek();
    void EndWeek();
    void BeginShift();
    void EndShift(const std::string& matron_title, bool Day0Night1, bool has_matron);

    /// Looks for a matron and decides whether she works.
    /// Returns true if the matron for this shift does work.
    bool SetupMatron(bool is_night, const std::string& title);

    virtual void Update();
    virtual void UpdateGirls(bool is_night) = 0;


    /// Handles all resting girls.
    void HandleRestingGirls(bool is_night, bool has_matron, const char * matron_name);

    /// This function is called for every resting girl that is not on maternity leave. If it returns
    /// true, processing for this girl is assuemd to be finished.
    virtual bool handle_resting_girl(sGirl& girl, bool is_night, bool has_matron, std::stringstream& ss)
    { return false; };

    /// This function is called when the matron wants to send a girl back to work. If this returns
    /// false, then the standard back to work code (reset job to old job + default message) is used.
    /// Override this function if you need special behaviour.
    virtual bool handle_back_to_work(sGirl& girl, std::stringstream& ss, bool is_night)
    { return false; }

    void do_daily_items(sGirl& girl);

    void CalculatePay(sGirl& girl, u_int Job);
protected:
    std::string m_Name;
    std::list<sGirl*> m_Girls;

private:
    std::unordered_set<SKILLS> m_ForbiddenSexType;

    BuildingType m_Type;
    std::string m_BackgroundImage;
};

// predicates
// ---------------------------------------------------------------------------------------------------------------------

inline bool is_nonhuman(const sGirl* girl) {
    return girl->has_active_trait("Not Human");
}

struct HasName {
    explicit HasName(std::string name) : m_Name(std::move(name)) {
    }

    bool operator()(const sGirl* girl) const {
        return m_Name == girl->FullName();
    }

    std::string m_Name;
};

struct HasJob {
    HasJob(JOBS job, bool at_night) : m_Job(job), m_DayNight(at_night) {

    }

    bool operator()(const sGirl* girl) const {
        return girl->get_job(m_DayNight) == m_Job;
    }

    JOBS m_Job;
    bool m_DayNight;
};

const char* building_type_to_str(BuildingType type);

#endif //CRAZYS_WM_MOD_IBUILDING_HPP
