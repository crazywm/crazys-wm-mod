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

#ifndef CRAZYS_WM_MOD_SGIRL_HPP
#define CRAZYS_WM_MOD_SGIRL_HPP

#include <string>
#include <map>
#include <vector>
#include <set>
#include <tinyxml2.h>

#include "cEvents.h"
#include "Constants.h"
#include "ICharacter.h"
#include "scripting/fwd.hpp"
#include "scripting/sLuaParameter.hpp"
#include "pregnancy.h"
#include "utils/DirPath.h"

#include "interface/TableCells.h"


class TraitSpec;
class sInventoryItem;
class sChild;
class IBuilding;
class cPlayer;
class sCustomer;
class sGirl;

typedef struct sChild
{
    int m_MultiBirth;
    std::string multibirth_str()
    {
        if (m_MultiBirth == 2) return "Twins";
        if (m_MultiBirth == 3) return "Triplets";
        if (m_MultiBirth == 4) return "Quads";
        if (m_MultiBirth == 5) return "Quints";
        // `J` anything else is single
        m_MultiBirth = 1;
        return "Single";
    };
    enum Gender {
        None = -1,
        Girl = 0,
        Boy = 1
    };
    Gender m_Sex;
    int m_GirlsBorn;            // if multiple births, how many are girls
    std::string boy_girl_str()
    {
        if (m_MultiBirth == 2)    return "twins";
        if (m_MultiBirth == 3)    return "triplets";
        if (m_MultiBirth > 3)    return "a litter";
        if (m_Sex == Boy)        return "a baby boy";
        if (m_Sex == Girl)        return "a baby girl";
        return "a baby";
    }
    bool is_boy()    { return m_Sex == Boy; }
    bool is_girl()    { return m_Sex == Girl; }

    static Gender RandomGender();

    int m_Age;    // grows up at 60 weeks
    bool m_IsPlayers;    // 1 when players
    unsigned char m_Unborn;    // 1 when child is unborn (for when stats are inherited from customers)

    // skills and stats from the father
    int m_Stats[NUM_STATS];
    int m_Skills[NUM_SKILLS];

    sChild* m_Next;
    sChild* m_Prev;

    sChild(bool is_players = false, Gender gender = None, int MultiBirth = 1);
    ~sChild(){ m_Prev = nullptr; if (m_Next)delete m_Next; m_Next = nullptr; }

    tinyxml2::XMLElement& SaveChildXML(tinyxml2::XMLElement& elRoot);
    bool LoadChildXML(const tinyxml2::XMLElement* pChild);

} sChild;

// Represents a single girl
struct sGirl : public ICharacter, public std::enable_shared_from_this<sGirl>
{
    sGirl(bool unique);
    ~sGirl() override;

    const DirPath& GetImageFolder() const;
    void SetImageFolder(DirPath p);

    std::string m_Name;                                 // The girls name
    std::string m_MotherName;                           //    `J` added mother and father names
    std::string m_FatherName;                           //    `J` added mother and father names
    std::string m_Desc;                                 // Short story about the girl

    int house() const                               { return m_HousePercent; }                /* It's NOT lupus! */
    void house(int n)                                { m_HousePercent = std::min(std::max(0 , n), 100); }
    int m_HousePercent = 0;

    /// gets the girls job for the day or night shift
    JOBS get_job(bool night_shift) const;
    void FullJobReset(JOBS job);
    JOBS m_DayJob;                            // id for what job the girl is currently doing
    JOBS m_NightJob;                          // id for what job the girl is currently doing
    JOBS m_PrevDayJob     = JOB_UNSET;        // id for what job the girl was doing
    JOBS m_PrevNightJob   = JOB_UNSET;        // id for what job the girl was doing
    JOBS m_YesterDayJob   = JOB_UNSET;        // id for what job the girl did yesterday
    JOBS m_YesterNightJob = JOB_UNSET;        // id for what job the girl did yesternight

    int m_Enjoyment[NUM_ACTIONTYPES];            // these values determine how much a girl likes an action
    int m_EnjoymentMods[NUM_ACTIONTYPES];        // `J` added perminant modifiers to stats
    int m_EnjoymentTemps[NUM_ACTIONTYPES];        // `J` added these go down (or up) by 30% each week until they reach 0
    // (-100 is hate, +100 is loves)

    bool m_UseAntiPreg;                            // if true she will use anit preg measures

    unsigned char m_Withdrawals;                // if she is addicted to something this counts how many weeks she has been off

    int m_Money;                                // amount of cash this chick has

    int m_AccLevel;                    // how good her Accommodation is, 0 is slave like and non-slaves will really hate it

    int m_Training[NUM_TRAININGTYPES];            // these values determine how far a girl is into her training CRAZY
    int m_TrainingMods[NUM_TRAININGTYPES];        // 
    int m_TrainingTemps[NUM_TRAININGTYPES];        // 
    // (starts at 0, 100 if fully trained)

    int m_RunAway;                    // if 0 then off, if 1 then girl is removed from list,
    // otherwise will count down each week
    unsigned char m_Spotted;                    // if 1 then she has been seen stealing but not punished yet

    int m_WeeksPast;                            // number of weeks in your service
    int m_BDay;                                    // number of weeks in your service since last aging

    int m_NumCusts;                                // number of customers this girl has slept with
    int m_NumCusts_old;                            // number of customers this girl has slept with before this week

    bool m_Tort;                                // if true then have already tortured today
    bool m_JustGaveBirth;                        // did she give birth this current week?

    std::set<Fetishs> m_FetishTypes;            // the types of fetishes this girl has

    void AddMessage(const std::string& message, int nImgType, EventType event);
    cEvents& GetEvents() { return m_Events; }
    const cEvents& GetEvents() const { return m_Events; }

    // triggers
    scripting::pEventMapping m_EventMapping;
    scripting::sAsyncScriptHandle TriggerEvent(scripting::sEventID id);
    template<class ...T>
    scripting::sAsyncScriptHandle TriggerEvent(scripting::sEventID id, T&&... args) {
        return m_EventMapping->RunAsync(id, *this, std::forward<T>(args)...);
    }

    template<class ...T>
    scripting::sScriptValue CallScriptFunction(scripting::sEventID id, T&&... args) {
        return m_EventMapping->RunSynchronous(id, *this, std::forward<T>(args)...);
    }

    unsigned char m_DaysUnhappy;                // used to track how many days they are really unhappy for

    int m_WeeksPreg;                            // number of weeks pregnant or inseminated
    int m_PregCooldown;                            // number of weeks until can get pregnant again
    std::vector<std::unique_ptr<sChild>> m_Children;
    int m_ChildrenCount[CHILD_COUNT_TYPES];

    std::vector<std::string> m_Canonical_Daughters;

    IBuilding* m_Building = nullptr;
    int m_PrevWorkingDay;                        // `J` save the last count of the number of working days
    int m_WorkingDay;                            // count the number of working day
    bool m_Refused_To_Work_Day;                    // `J` to track better if she refused to work her assigned job
    bool m_Refused_To_Work_Night;                // `J` to track better if she refused to work her assigned job

    static const char    *training_jobs[];
    static const char    *children_type_names[];    // `J` added

    tinyxml2::XMLElement& SaveGirlXML(tinyxml2::XMLElement& elRoot);
    bool LoadGirlXML(const tinyxml2::XMLElement* pGirl);
    static std::shared_ptr<sGirl> LoadFromTemplate(const tinyxml2::XMLElement& root);

    /*
    *    stream operator - used for debug
    */
    friend std::ostream& operator<<(std::ostream& os, sGirl &g);

    int get_stat(int stat_id) const override;
    void set_stat(int stat, int amount) override;

    void upd_temp_stat(int stat_id, int amount, bool usetraits=false) override;
    int upd_base_stat(int stat_id, int amount, bool usetraits = true) override;

    int upd_temp_Enjoyment(Action_Types stat_id, int amount);
    int upd_Enjoyment(Action_Types stat_id, int amount);
    int upd_Training(int stat_id, int amount, bool usetraits = true);

    int rebel() const;
    bool FixFreeTimeJobs();

    int get_enjoyment(Action_Types actiontype) const;
    int get_training(int actiontype) const;

    /*
    *    convenience func. Also easier to read like this
    */
    bool carrying_monster() const;
    bool carrying_human() const;
    bool carrying_players_child() const;
    bool carrying_customer_child() const;
    bool is_pregnant() const;
    int get_preg_duration() const;
    bool is_mother() const;
    bool is_poisoned() const;
    void clear_pregnancy();
    void clear_dating();

    bool calc_pregnancy(int chance, int type, const ICharacter& father);

    bool calc_pregnancy(cPlayer* player, double factor = 1.0, bool nomessage=false);
    bool calc_pregnancy(const sCustomer& cust, double factor = 1.0);
    bool calc_insemination(const sCustomer& cust, double factor = 1.0);
    bool calc_group_pregnancy(cPlayer* player, double factor = 1.0);
    bool calc_group_pregnancy(const sCustomer& cust, double factor = 1.0);
    /*
    *    let's overload that...
    *    should be able to do the same using sCustomer as well...
    */

    int breast_size() const;
    bool is_dead() const;        // `J` replaces a few DeadGirl checks
    bool is_fighter(bool canbehelped = false) const;

    void run_away();

    bool keep_tips() const;
    /// returns whether the girl receives payment. This is not the case if she is a slave and you don't pay your slaves
    bool is_unpaid() const;
    bool is_slave()    const;
    bool is_free() const;
    void set_slave();
    bool is_monster() const;
    bool is_human() const;
    bool is_arena() const;
    bool is_isdaughter() const;
    bool is_resting() const;
    bool is_havingsex() const;
    bool was_resting() const;

    FormattedCellData GetDetail(const std::string& detailName) const;
    FormattedCellData GetJobRating(JOBS job) const;

private:
    FormattedCellData GetDetail_Job(const std::string& detailName) const;

    cEvents m_Events;                            // Each girl keeps track of all her events that happened to her in the last turn

public:
    // END MOD

    double job_performance(JOBS job, bool estimate=true) const;

    // more useful functions
    int get_num_item_equiped(int Type) const;
    void set_default_house_percent();

    bool equip(const sInventoryItem* item, bool force);

    bool unequip(const sInventoryItem* item) override;
    bool can_equip(const sInventoryItem* item) const override;

    bool disobey_check(int action, JOBS job=NUM_JOBS);

    void add_tiredness();

    bool fights_back();

    void set_status(STATUS stat);
    bool has_status(STATUS stat) const;
    void remove_status(STATUS stat);

private:
    int m_States = 0;                                // Holds the states the girl has

    DirPath m_ImageFolder;
};

#endif //CRAZYS_WM_MOD_SGIRL_HPP
