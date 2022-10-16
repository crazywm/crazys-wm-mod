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

#ifndef CRAZYS_WM_MOD_SCHARACTER_HPP
#define CRAZYS_WM_MOD_SCHARACTER_HPP

#include <fstream>
#include <tinyxml2.h>
#include "Constants.h"
#include "sAttribute.h"
#include <memory>
#include <vector>
#include <boost/variant.hpp>
#include <unordered_map>

namespace tinyxml2 {
    class XMLElement;
}

using StatSkill = boost::variant<STATS, SKILLS>;

class Inventory;
class sInventoryItem;
class sTraitInfo;
class ITraitsCollection;

/*!
 * \brief base class for all characters. Characters are girls, customers, the player, children.
 *
*/
class ICharacter {
public:
    explicit ICharacter(std::unique_ptr<ITraitsCollection> tc, bool unique=false);
    virtual ~ICharacter();

    ICharacter(ICharacter&&) noexcept;

    // -----------------------------------------------------------------------------------------------------------------
    //                              GENERIC CHARACTER DATA
    // -----------------------------------------------------------------------------------------------------------------
    const std::string& FullName() const { return m_FullName; }
    const std::string& FirstName() const { return m_FirstName; }
    const std::string& MiddleName() const { return m_MiddleName; }
    const std::string& Surname() const { return m_Surname; }

    void SetName(std::string first, std::string middle, std::string last);
    void SetSurname(std::string surname);

    int BirthYear() const     { return m_BirthYear; }
    int BirthMonth() const    { return m_BirthMonth; }
    int BirthDay() const      { return m_BirthDay; }

    int SetBirthYear(int n);
    int SetBirthMonth(int n);
    int SetBirthDay(int n);

    bool IsUnique() const;
    /// Returns an ID that uniquely identifies the character. The returned number is always positive,
    /// so an ID of 0 can be used to signify an empty value.
    std::uint64_t GetID() const { return m_CharacterID; }

    // -----------------------------------------------------------------------------------------------------------------
    //                                        Inventory
    // -----------------------------------------------------------------------------------------------------------------
    Inventory& inventory();
    const Inventory& inventory() const;

    // shortcuts for common operations
    bool has_item(const sInventoryItem* item, int amount=1) const;
    const sInventoryItem* has_item(const std::string& name, int amount=1) const;
    bool add_item(const sInventoryItem* item, int amount=1);
    int remove_item(const sInventoryItem* item, int amount=1);

    // equipping
    virtual bool can_equip(const sInventoryItem* item) const { return false; };
    virtual bool unequip(const sInventoryItem* item);
    virtual bool equip(const sInventoryItem* item, bool force) { return false; };

    // -----------------------------------------------------------------------------------------------------------------
    //                                        Traits
    // -----------------------------------------------------------------------------------------------------------------
    int get_trait_modifier(const char* type) const;

    // normal way of character trait changes
    /// Adds a new trait to the list of permanent traits.
    /// Returns true if the trait was added, False if not (because it already existed)
    bool gain_trait(const char* trait_name, float chance=100);

    /// Removes a trait from the list of permanent traits.
    /// If hide==true, then the trait is only disabled.
    /// If the trait is inborn (inherent), then it is not removed but only disabled.
    bool lose_trait(const char* trait_name, int chance) { return lose_trait(trait_name, false, chance); }
    bool lose_trait(const char* trait_name, bool hide=false, int chance=100);

    /// gains a trait only temporarily.
    void add_temporary_trait(const char* trait_name, int duration);

    // check for traits
    /// Returns whether the character has a dormant (inherent or permanent trait that is set to inactive) trait.
    /// Traits that are blocked by dynamic traits are not considered dormant. If there is both a dormant and an
    /// active trait of this type (i.e. inherent: dormant, permanent: active or vice versa) the trait is also not
    /// considered dormant.
    bool has_dormant_trait(const char* trait_name) const;

    /// Returns whether the given trait is an active trait. It does not matter whether the trait is iherent, permanent
    /// or dynamic.
    bool has_active_trait(const char* trait_name) const;

    std::vector<sTraitInfo> get_trait_info() const;

    /// provide direct access to the underlying traits collection
    ITraitsCollection& raw_traits() { return *m_Traits; }

    // -----------------------------------------------------------------------------------------------------------------
    //                                        STATS
    // -----------------------------------------------------------------------------------------------------------------
    int get_attribute(StatSkill id) const;
    int update_attribute(StatSkill id, int amount);
    /// Updates the attribute with an amount between min and max, if its current value is less than target.
    /// If the current value exceeds target, the update amount is decreased.
    /// In the future, this is the function that should be used for most stat/skill gains
    int gain_attribute(StatSkill id, int min, int max, int target);

    virtual int get_stat(STATS stat_id) const;
    /// gets the stat without any modifiers
    int get_base_stat(STATS stat_id) const { return m_Stats[(int)stat_id].m_Value; }
    int get_temp_stat(STATS stat_id) const { return m_Stats[(int)stat_id].m_TempMods; }
    int get_mod_stat(STATS stat_id) const { return m_Stats[(int)stat_id].m_PermanentMods; }

    /// Sets the stat's base value, which is the characters inherent value for that stat.
    virtual void set_stat(STATS stat, int amount);
    virtual int upd_base_stat(STATS stat, int amount, bool usetraits = true);
    void upd_mod_stat(STATS stat, int amount);
    virtual void upd_temp_stat(STATS stat, int amount, bool usetraits=false);

    // higher level behaviour
    void DecayTemp();

    // convenience methods
    // `J` When modifying Stats or Skills, search for "J-Change-Stats-Skills"  :  found in >> cGirls.h
    int charisma() const                            { return get_stat(STATS::STAT_CHARISMA); }
    int charisma(int n)                             { return upd_base_stat(STATS::STAT_CHARISMA, n, true); }
    int happiness() const                           { return get_stat(STATS::STAT_HAPPINESS); }
    int happiness(int n)                            { return upd_base_stat(STATS::STAT_HAPPINESS, n, true); }
    int libido() const                              { return get_stat(STATS::STAT_LIBIDO); }
    int libido(int n)                               { return upd_base_stat(STATS::STAT_LIBIDO, n, true); }
    int constitution() const                        { return get_stat(STATS::STAT_CONSTITUTION); }
    int constitution(int n)                         { return upd_base_stat(STATS::STAT_CONSTITUTION, n, true); }
    int intelligence() const                        { return get_stat(STATS::STAT_INTELLIGENCE); }
    int intelligence(int n)                         { return upd_base_stat(STATS::STAT_INTELLIGENCE, n, true); }
    int confidence() const                          { return get_stat(STATS::STAT_CONFIDENCE); }
    int confidence(int n)                           { return upd_base_stat(STATS::STAT_CONFIDENCE, n, true); }
    int mana() const                                { return get_stat(STATS::STAT_MANA); }
    int mana(int n)                                 { return upd_base_stat(STATS::STAT_MANA, n, true); }
    int agility() const                             { return get_stat(STATS::STAT_AGILITY); }
    int agility(int n)                              { return upd_base_stat(STATS::STAT_AGILITY, n, true); }
    int strength() const                            { return get_stat(STATS::STAT_STRENGTH); }
    int strength(int n)                             { return upd_base_stat(STATS::STAT_STRENGTH, n, true); }
    int fame() const                                { return get_stat(STATS::STAT_FAME); }
    int fame(int n)                                 { return upd_base_stat(STATS::STAT_FAME, n, true); }
    int level() const                               { return get_stat(STATS::STAT_LEVEL); }
    int level(int n)                                { return upd_base_stat(STATS::STAT_LEVEL, n, true); }
    int askprice() const                            { return get_stat(STATS::STAT_ASKPRICE); }
    int askprice(int n)                             { return upd_base_stat(STATS::STAT_ASKPRICE, n, true); }
    int exp() const                                 { return get_stat(STATS::STAT_EXP); }
    int exp(int n)                                  { return upd_base_stat(STATS::STAT_EXP, n, true); }
    int age() const                                 { return get_stat(STATS::STAT_AGE); }
    int age(int n)                                  { return upd_base_stat(STATS::STAT_AGE, n, true); }
    int obedience() const                           { return get_stat(STATS::STAT_OBEDIENCE); }
    int obedience(int n)                            { return upd_base_stat(STATS::STAT_OBEDIENCE, n, true); }
    int spirit() const                              { return get_stat(STATS::STAT_SPIRIT); }
    int spirit(int n)                               { return upd_base_stat(STATS::STAT_SPIRIT, n, true); }
    int beauty() const                              { return get_stat(STATS::STAT_BEAUTY); }
    int beauty(int n)                               { return upd_base_stat(STATS::STAT_BEAUTY, n, true); }
    int tiredness() const                           { return get_stat(STATS::STAT_TIREDNESS); }
    int tiredness(int n)                            { return upd_base_stat(STATS::STAT_TIREDNESS, n, true); }
    int health() const                              { return get_stat(STATS::STAT_HEALTH); }
    int health(int n)                               { return upd_base_stat(STATS::STAT_HEALTH, n, true); }
    int pcfear() const                              { return get_stat(STATS::STAT_PCFEAR); }
    int pcfear(int n)                               { return upd_base_stat(STATS::STAT_PCFEAR, n, true); }
    int pclove() const                              { return get_stat(STATS::STAT_PCLOVE); }
    int pclove(int n)                               { return upd_base_stat(STATS::STAT_PCLOVE, n, true); }
    int morality() const                            { return get_stat(STATS::STAT_MORALITY); }
    int morality(int n)                             { return upd_base_stat(STATS::STAT_MORALITY, n, true); }
    int refinement() const                          { return get_stat(STATS::STAT_REFINEMENT); }
    int refinement(int n)                           { return upd_base_stat(STATS::STAT_REFINEMENT, n, true); }
    int dignity() const                             { return get_stat(STATS::STAT_DIGNITY); }
    int dignity(int n)                              { return upd_base_stat(STATS::STAT_DIGNITY, n, true); }
    int lactation() const                           { return get_stat(STATS::STAT_LACTATION); }
    int lactation(int n)                            { return upd_base_stat(STATS::STAT_LACTATION, n, true); }
    int sanity() const                              { return get_stat(STATS::STAT_SANITY); }
    int sanity(int n)                               { return upd_base_stat(STATS::STAT_SANITY, n, true); }

    // -----------------------------------------------------------------------------------------------------------------
    //                                            SKILLS
    // -----------------------------------------------------------------------------------------------------------------
    virtual int get_skill(int skill_id) const;
    /// gets the skill without any modifiers
    int get_base_skill(int skill_id) const { return m_Skills[skill_id].m_Value; }
    int get_temp_skill(int stat_id) const { return m_Skills[stat_id].m_TempMods; }
    int get_mod_skill(int stat_id) const { return m_Skills[stat_id].m_PermanentMods; }
    void set_skill(SKILLS skill_id, int amount);

    /// Sets the skill, bypassing any cap checks. Use only in loading code.
    void set_skill_direct(SKILLS skill_id, int amount);

    virtual int upd_skill(int skill_id, int amount, bool usetraits = true);
    virtual void upd_temp_skill(int stat, int amount);
    virtual void upd_mod_skill(int stat, int amount);

    int anal() const                                { return get_skill(SKILL_ANAL); }
    int anal(int n)                                 { return upd_skill(SKILL_ANAL, n, true); }
    int bdsm() const                                { return get_skill(SKILL_BDSM); }
    int bdsm(int n)                                 { return upd_skill(SKILL_BDSM, n, true); }
    int beastiality() const                         { return get_skill(SKILL_BEASTIALITY); }
    int beastiality(int n)                          { return upd_skill(SKILL_BEASTIALITY, n, true); }
    int combat() const                              { return get_skill(SKILL_COMBAT); }
    int combat(int n)                               { return upd_skill(SKILL_COMBAT, n, true); }
    int group() const                               { return get_skill(SKILL_GROUP); }
    int group(int n)                                { return upd_skill(SKILL_GROUP, n, true); }
    int lesbian() const                             { return get_skill(SKILL_LESBIAN); }
    int lesbian(int n)                              { return upd_skill(SKILL_LESBIAN, n, true); }
    int magic() const                               { return get_skill(SKILL_MAGIC); }
    int magic(int n)                                { return upd_skill(SKILL_MAGIC, n, true); }
    int normalsex() const                           { return get_skill(SKILL_NORMALSEX); }
    int normalsex(int n)                            { return upd_skill(SKILL_NORMALSEX, n, true); }
    int oralsex() const                             { return get_skill(SKILL_ORALSEX); }
    int oralsex(int n)                              { return upd_skill(SKILL_ORALSEX, n, true); }
    int tittysex() const                            { return get_skill(SKILL_TITTYSEX); }
    int tittysex(int n)                             { return upd_skill(SKILL_TITTYSEX, n, true); }
    int handjob() const                             { return get_skill(SKILL_HANDJOB); }
    int handjob(int n)                              { return upd_skill(SKILL_HANDJOB, n, true); }
    int footjob() const                             { return get_skill(SKILL_FOOTJOB); }
    int footjob(int n)                              { return upd_skill(SKILL_FOOTJOB, n, true); }
    int service() const                             { return get_skill(SKILL_SERVICE); }
    int service(int n)                              { return upd_skill(SKILL_SERVICE, n, true); }
    int strip() const                               { return get_skill(SKILL_STRIP); }
    int strip(int n)                                { return upd_skill(SKILL_STRIP, n, true); }
    int medicine() const                            { return get_skill(SKILL_MEDICINE); }
    int medicine(int n)                             { return upd_skill(SKILL_MEDICINE, n, true); }
    int performance() const                         { return get_skill(SKILL_PERFORMANCE); }
    int performance(int n)                          { return upd_skill(SKILL_PERFORMANCE, n, true); }
    int crafting() const                            { return get_skill(SKILL_CRAFTING); }
    int crafting(int n)                             { return upd_skill(SKILL_CRAFTING, n, true); }
    int herbalism() const                           { return get_skill(SKILL_HERBALISM); }
    int herbalism(int n)                            { return upd_skill(SKILL_HERBALISM, n, true); }
    int farming() const                             { return get_skill(SKILL_FARMING); }
    int farming(int n)                              { return upd_skill(SKILL_FARMING, n, true); }
    int brewing() const                             { return get_skill(SKILL_BREWING); }
    int brewing(int n)                              { return upd_skill(SKILL_BREWING, n, true); }
    int animalhandling() const                      { return get_skill(SKILL_ANIMALHANDLING); }
    int animalhandling(int n)                       { return upd_skill(SKILL_ANIMALHANDLING, n, true); }
    int cooking() const                             { return get_skill(SKILL_COOKING); }
    int cooking(int n)                              { return upd_skill(SKILL_COOKING, n, true); }

    // Stat Changes: Stat/Skill values from last week
    std::unordered_map<STATS,  int> m_last_stats;
    std::unordered_map<SKILLS, int> m_last_skills;
    void save_statistics();
    // Stat Changes: Generate Strings to show stat / skill value changes with
    std::string stat_with_change_str(STATS stat) const;
    std::string skill_with_change_str(SKILLS skill) const;
private:
    template <typename T>
    std::string generate_change_string(const std::unordered_map<T, int>& last_values, T stat, int value_now) const;

protected:
    void SaveXML(tinyxml2::XMLElement& elRoot) const;
    void LoadXML(const tinyxml2::XMLElement& elRoot);

    std::array<sAttributeValue, NUM_STATS> m_Stats;
    std::array<sAttributeValue, NUM_SKILLS> m_Skills;

    bool m_IsUnique = false;
    /// This is an id that is unique for each character that is generated in the game.
    std::uint64_t m_CharacterID = 0;

    // Birth Data
    int m_BirthYear  = 1190;    // the game starts in year 1209 so default start age is 18
    int m_BirthMonth = 1;        // there are 12 month in the year
    int m_BirthDay   = 1;        // there are 30 days in every month

    // name info
    std::string m_FullName;

    std::string m_FirstName;
    std::string m_MiddleName;
    std::string m_Surname;

    // traits
    std::unique_ptr<ITraitsCollection> m_Traits;

    // inventory -- needs to go after traits, because items may generate traits
    std::unique_ptr<Inventory> m_Inventory;
};

#endif //CRAZYS_WM_MOD_SCHARACTER_HPP
