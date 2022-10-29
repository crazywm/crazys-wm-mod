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

#include <IGame.h>
#include <cGirls.h>
#include "pregnancy.h"
#include "ICharacter.h"
#include "buildings/IBuilding.h"
#include "sGirl.h"
#include "predicates.h"
#include "cTariff.h"
#include "sStorage.h"
#include "CLog.h"
#include "character/traits/ITraitsCollection.h"
#include "character/traits/ITraitSpec.h"
#include "cGangs.h"
#include "cGangManager.hpp"
#include "buildings/cDungeon.h"
#include "character/cPlayer.h"
#include "utils/algorithms.hpp"
#include <sstream>

namespace settings {
    extern const char* PREG_CONTRA_FAIL;
    extern const char* USER_KEEP_TIPS_SLAVE;
    extern const char* USER_KEEP_TIPS_FREE;
    extern const char* USER_PAY_SLAVE;
    extern const char* USER_ACCOMODATION_SLAVE;
    extern const char* USER_ACCOMODATION_FREE;
    extern const char* USER_HOUSE_PERCENT_SLAVE;
    extern const char* USER_HOUSE_PERCENT_FREE;
    extern const char* PREG_MULTI_CHANCE;
    extern const char* PREG_GOOD_FACTOR;
    extern const char* PREG_CHANCE_PLAYER;
    extern const char* PREG_CHANCE_CUST;
    extern const char* PREG_CHANCE_BEAST;
    extern const char* PREG_DURATION_HUMAN;
    extern const char* PREG_DURATION_MONSTER;
    extern const char* PREG_GIRL_CHILD;
    extern const char* PREG_COOL_DOWN;
    extern const char* PREG_MISS_HUMAN;
    extern const char* PREG_MISS_MONSTER;
    extern const char* PREG_WEEKS_GROW;
    extern const char* PREG_CHANCE_GIRL;
}

int calc_abnormal_pc(const sGirl& mom, sGirl& sprog, bool is_players);
bool child_is_due(sGirl& girl, sChild& child, std::string& summary, bool PlayerControlled = true);
void handle_son(sGirl& mom, std::string& summary, bool PlayerControlled);
void handle_daughter(sGirl& mom, const sChild& child, std::string& summary);

// ----- Pregnancy, insemination & children

bool UseAntiPreg(const sGirl& girl)
{
    if (!girl.m_UseAntiPreg) return false;
    /*
    *    anti-preg potions, we probably should allow
    *    on-the-fly restocks. You can imagine someone
    *    noticing things are running low and
    *    sending a girl running to the shops to get
    *    a restock
    *
    *    that said, there's a good argument here for
    *    making this the matron's job, and giving it a
    *    chance dependent on skill level. Could have a
    *    comedy event where the matron forgets, or the
    *    girl forgets (or disobeys) and half a dozen
    *    girls get knocked up.
    *
    *    'course, we could do that anyway.. :)
    *
    */
    // TODO base this on a status effect!
    auto building = girl.m_Building;
    if(building && building->provide_anti_preg()) {
        return !g_Dice.percent( g_Game->settings().get_percent(settings::PREG_CONTRA_FAIL) );
    }

    return false;
}


int fertility(const sGirl& girl) {
    if (girl.get_trait_modifier("tag:sterile") > 0) {
        g_LogFile.debug("pregnancy", girl.FullName(), " is sterile.");
        return 0;
    }
    if (girl.is_pregnant()) {
        g_LogFile.debug("pregnancy", girl.FullName(), " is already pregnant.");
        return 0;
    }
    if (girl.m_PregCooldown > 0) {
        g_LogFile.debug("pregnancy", girl.FullName(), " is was recently pregnant (", girl.m_PregCooldown, ").");
        return 0;
    }
    if(UseAntiPreg(girl)) {
        g_LogFile.debug("pregnancy", girl.FullName(), " succesfully used contraceptives.");
        return 0;
    }

    int chance = 100.f * g_Game->settings().get_percent(settings::PREG_CHANCE_GIRL);
    chance += girl.get_trait_modifier("fertility");
    g_LogFile.debug("pregnancy", girl.FullName(), "'s fertility: ", chance);
    return chance;
}

void create_pregnancy(sGirl& girl, int num_children, int type, const ICharacter& father) {
    // if there is somehow leftover pregnancy data, clear it
    girl.clear_pregnancy();
    girl.set_status((STATUS)type);

    if (girl.has_active_trait(traits::BROODMOTHER))
    {
        if (g_Dice.percent(g_Game->settings().get_percent(settings::PREG_MULTI_CHANCE))) num_children++;
        if (g_Dice.percent(g_Game->settings().get_percent(settings::PREG_MULTI_CHANCE))) num_children++;
    }
    auto child = std::make_unique<sChild>(unsigned(type) == STATUS_PREGNANT_BY_PLAYER, sChild::None, num_children);

    // `J` average the mother's and father's stats and skills
    for (auto stat : StatsRange)        child->m_Stats[stat] = (father.get_base_stat(stat) + girl.get_base_stat(stat)) / 2;
    /// TODO this doesn't make sense, learned skills should not be inherited
    for (auto skill: SkillsRange)    child->m_Skills[skill] = (father.get_base_skill(skill) + girl.get_base_skill(skill)) / 2;
    child->m_Stats[STAT_HEALTH] = 100;
    child->m_Stats[STAT_HAPPINESS] = 100;

    girl.m_Children.push_back(std::move(child));
}

bool child_is_due(sGirl& girl, sChild& child, std::string& summary, bool PlayerControlled) {
    if (child.m_MultiBirth < 1) child.m_MultiBirth = 1; // `J` fix old code
    if (child.m_MultiBirth > 5 && girl.carrying_human())
        child.m_MultiBirth = 5; // `J` fix old code - maximum 5 human children - no max for beasts
    /*
    *    clock on the count and see if she's due
    *    if not, return false (meaning "do not remove this child yet)
    */
    girl.m_WeeksPreg++;
    if (girl.m_WeeksPreg < girl.get_preg_duration())
        return false;
    /*
    *    OK, it's time to give birth
    *    start with some basic bookkeeping.
    */
    std::stringstream ss;

    girl.m_WeeksPreg = 0;
    child.m_Unborn = 0;
    girl.m_PregCooldown = g_Game->settings().get_integer(settings::PREG_COOL_DOWN);

    //ADB low health is risky for pregnancy!
    //80 health will add 2 to percent chance of sterility and death, 10 health will add 9 percent!
    int healthFactor = (100 - girl.health()) / 10;
    /*
    *    the human-baby case is marginally easier than the
    *    tentacle-beast-monstrosity one, so we do that first
    */
    bool is_broodmother = girl.has_active_trait(traits::BROODMOTHER);
    bool is_fertile = girl.has_active_trait(traits::FERTILE);
    if (girl.carrying_human())
    {
        /*
        *        first things first - clear the pregnancy bit
        *        this is a human birth, so add the MILF trait
        */
        girl.clear_pregnancy();
        girl.m_JustGaveBirth = true;
        girl.gain_trait(traits::MILF);

        girl.tiredness(100);
        girl.happiness(10 + g_Dice % 91);
        girl.health(-(child.m_MultiBirth + g_Dice % 10 + healthFactor));

        // `J` If/when the baby gets moved somewhere else in the code, then the maother can die from giving birth
        // For now don't kill her, it causes too many problems with the baby.
        if (girl.health() < 1) girl.set_stat(STAT_HEALTH, 1);

        if (child.m_MultiBirth == 1)    // only 1 baby so use the old code
        {
            if (g_Dice.percent(g_Game->settings().get_percent(settings::PREG_MISS_HUMAN)))    // the baby dies
            {
                // format a message
                girl.m_ChildrenCount[CHILD08_MISCARRIAGES]++;
                ss << "She has given birth to " << child.boy_girl_str() << " but it did not survive the birth.\n \nYou grant her the week off to grieve.";
                //check for sterility
                if (g_Dice.percent(5 + healthFactor))
                {
                    // `J` updated old code to use new traits from new code
                    ss << "It was a difficult birth and ";
                    if (is_broodmother)
                    {
                        ss << "her womb has been damaged.\n";
                        girl.health(-(1 + g_Dice % 2));
                        if (girl.health() < 1) girl.set_stat(STAT_HEALTH, 1);    // don't kill her now, it causes all the babies to go away.
                    }
                    else if (is_fertile)    // loose fertile
                    {
                        ss << "her womb has been damaged reducing her fertility.\n";
                        cGirls::AdjustTraitGroupFertility(girl, -1);
                    }
                    else    // add sterile
                    {
                        ss << "she has lost the ability to have children.\n";
                        cGirls::AdjustTraitGroupFertility(girl, -1);
                    }
                }
                if (PlayerControlled) girl.AddMessage(ss.str(), EImageBaseType::PROFILE, EVENT_DANGER);
                return true;
            }
            else    // the baby lives
            {
                girl.m_ChildrenCount[CHILD00_TOTAL_BIRTHS]++;
                if (child.is_girl())
                {
                    girl.m_ChildrenCount[CHILD02_ALL_GIRLS]++;
                    if (child.m_IsPlayers) girl.m_ChildrenCount[CHILD06_YOUR_GIRLS]++;
                    else                    girl.m_ChildrenCount[CHILD04_CUSTOMER_GIRLS]++;
                }
                else
                {
                    girl.m_ChildrenCount[CHILD03_ALL_BOYS]++;
                    if (child.m_IsPlayers) girl.m_ChildrenCount[CHILD07_YOUR_BOYS]++;
                    else                    girl.m_ChildrenCount[CHILD05_CUSTOMER_BOYS]++;
                }
                // format a message
                ss << "She has given birth to " << child.boy_girl_str() << ".\n \nYou grant her the week off for maternity leave.";
                //check for sterility
                if (g_Dice.percent(healthFactor))
                {
                    // `J` updated old code to use new traits from new code
                    ss << "It was a difficult birth and ";
                    if (is_broodmother)
                    {
                        ss << "her womb has been damaged.\n";
                        girl.health(-(1 + g_Dice % 2));
                        if (girl.health() < 1) girl.set_stat(STAT_HEALTH, 1);    // don't kill her now, it causes all the babies to go away.
                    }
                    else if (is_fertile)    // loose fertile
                    {
                        ss << "her womb has been damaged reducing her fertility.\n";
                        cGirls::AdjustTraitGroupFertility(girl, -1);
                    }
                    else    // add sterile
                    {
                        ss << "she has lost the ability to have children.\n";
                        cGirls::AdjustTraitGroupFertility(girl, -1);
                    }
                }
                // queue the message and return false because we need to see this one grow up
                if (PlayerControlled) girl.AddMessage(ss.str(), EImageBaseType::PROFILE, EVENT_DANGER);
                return false;
            }
        }
        else    // multiple births
        {
            int unbornbabies = child.m_MultiBirth;
            healthFactor += child.m_MultiBirth;
            int t = 0;    // total
            int m = 0;    // miscarriages
            int g = 0;    // girls born live
            int b = 0;    // boys born live
            int s = 0;    // sterility count
            int c = 0;    // current baby, 0=girl, 1=boy
            while (unbornbabies > 0)
            {
                c = (unbornbabies > child.m_GirlsBorn ? 1 : 0);
                t++;
                if (g_Dice.percent(100.f * g_Game->settings().get_percent(settings::PREG_MISS_HUMAN)            // the baby dies
                                   + child.m_MultiBirth))                                        // more likely for multiple births
                {
                    m++; girl.m_ChildrenCount[CHILD08_MISCARRIAGES]++;            // add to miscarriage count
                    child.m_MultiBirth--; if (c == 0) child.m_GirlsBorn--;    // and remove the baby from the counts
                    if (g_Dice.percent(5 + healthFactor)) s++;                    // check for sterility
                }
                else    // the baby lives
                {
                    girl.m_ChildrenCount[CHILD00_TOTAL_BIRTHS]++;
                    if (c == 0)
                    {
                        g++;
                        girl.m_ChildrenCount[CHILD02_ALL_GIRLS]++;
                        if (child.m_IsPlayers) girl.m_ChildrenCount[CHILD06_YOUR_GIRLS]++;
                        else                    girl.m_ChildrenCount[CHILD04_CUSTOMER_GIRLS]++;
                    }
                    else
                    {
                        b++;
                        girl.m_ChildrenCount[CHILD03_ALL_BOYS]++;
                        if (child.m_IsPlayers) girl.m_ChildrenCount[CHILD07_YOUR_BOYS]++;
                        else                    girl.m_ChildrenCount[CHILD05_CUSTOMER_BOYS]++;
                    }
                    if (g_Dice.percent(healthFactor)) s++;                        //check for sterility
                }
                unbornbabies--; // count down and check the next one
            }

            ss << girl.FullName() << " has given birth";
            if (g + b > 0)        ss << " to ";
            if (g > 0)            ss << g << " girl" << (g > 1 ? "s" : "");
            if (g > 0 && b > 0)    ss << " and ";
            if (b > 0)            ss << b << " boy" << (b > 1 ? "s" : "");
            if (m > 0)
            {
                ss << ".\nShe lost ";
                if (m < t) ss << m << " of her " << t << " babies";
                else ss << "all of her babies, she is very distraught";
            }

            ss << ".\n \nYou grant her the week off ";
            if (g + b > 0)            ss << "for maternity leave";
            if (g + b > 0 && m > 0)    ss << " and ";
            if (m > 0)                ss << "to mourn her lost child" << (m > 1 ? "ren" : "");
            ss << ".\n \n";

            if (s > 0)
            {
                ss << "It was a difficult birth and ";
                if (is_broodmother)
                {
                    if (s > 1)    // loose broodmother only if 2 or more miscarriages
                    {
                        ss << "her womb has been damaged reducing her fertility.\n";
                        cGirls::AdjustTraitGroupFertility(girl, (s == 5 ? -2 : -1));
                    }
                    else        // otherwise take more damage
                    {
                        ss << "her womb has been damaged.\n";
                        girl.health(-(s + g_Dice % (s * 2)));
                        if (girl.health() < 1) girl.set_stat(STAT_HEALTH, 1);    // don't kill her now, it causes all the babies to go away.
                    }
                }
                else if (is_fertile)    // loose fertile
                {
                    ss << "her womb has been damaged " << (s > 3 ? "leaving her sterile" : "reducing her fertility") << ".\n";
                    cGirls::AdjustTraitGroupFertility(girl, (s > 3 ? -2 : -1));
                }
                else    // add sterile
                {
                    ss << "she has lost the ability to have children.\n";
                    cGirls::AdjustTraitGroupFertility(girl, -1);
                }
            }
            else if (!is_broodmother && t > 1 && g_Dice.percent(t * 5))
            {
                ss << "She has given birth to so many children, her womb has gotten used to carrying babies.\n";
                cGirls::AdjustTraitGroupFertility(girl, 1);
            }

            if (PlayerControlled) girl.AddMessage(ss.str(), EImageBaseType::PROFILE, EVENT_DANGER);

            if (child.m_MultiBirth > 0) return false;    // there are some babies that survived so we need to keep them
            else return true;                            // or they all died so we can remove this pregnancy
        }
    }
    /*
    *    It's monster time!
    *
    *    How much does one of these suckers bring on the open market
    *    anyway?
    *
    *    might as well record the transaction and clear the preggo bit
    *    while we're at it
    */


    if (PlayerControlled)
    {
        int number = child.m_MultiBirth;
        ss << "The ";
        if (number > 1)    { ss << number << " creatures within her have"; }
        else { number = 1;    ss << "creature within her has"; }
        ss << " matured and emerged from her womb.\n";

        int died = 0; int add = 0; int sell = 0; long gold = 0;
        for (int i = 0; i < number; i++)
        {
            if (g_Dice.percent(100.f * g_Game->settings().get_percent(settings::PREG_MISS_MONSTER) + number - 1))    died++;        // some may die
            else if (g_Dice.percent(child.m_Stats[STAT_BEAUTY]))                    add++;        // keep the good looking ones
            else if (g_Dice.percent(child.m_Stats[STAT_CONSTITUTION]))                add++;        // and the realy healthy ones
            else sell++;                                                                        // sell the rest
        }
        girl.m_ChildrenCount[CHILD01_ALL_BEASTS] += add + sell;
        girl.m_ChildrenCount[CHILD08_MISCARRIAGES] += died;
        if (died > 0)
        {
            healthFactor += died;
            if (died > 2) healthFactor += died;    // the more that died the worse off she will be
        }
        if (add > 0)
        {
            g_Game->storage().add_to_beasts(add);
        }
        if (sell > 0)
        {
            gold = sell * g_Game->tariff().creature_sales();
            g_Game->gold().creature_sales(gold);
        }

        if (died > 0)
        {
            ss << "\nUnfortunately ";
            if (add + sell < 1)
            {
                /* */if (died == 1)    ss << "it did not survive.";
                else if (died == 2)    ss << "neither of them survived.";
                else /*          */    ss << "none of them survived.";
            }
            else
            {
                if (died == 1) ss << "one";
                else ss << died;
                ss << " of them did not survive.";
            }
        }

        if (add > 0)
        {
            ss << "\n";
            if (died + sell < 1)    // all added
            {
                if (add == 1) ss << "It was";
                else ss << "They were " << (add == 2 ? "both" : "all");
                ss << " added to your stable of beasts.";
            }
            else
            {
                if (add == 1) ss << "One of them was";
                else ss << add << " of them were";
                ss << " good enough to be added to your stable of beasts.";
            }
        }

        if (sell > 0)
        {
            ss << "\n";
            if (died + add < 1)    // all sold
            {
                if (sell == 1) ss << "It was";
                else ss << "They were " << (sell ==2 ? "both" : "all");
                ss << " sold for " << g_Game->tariff().creature_sales() << "gold" << (sell > 1 ? " each" : "") << ".";
            }
            else
            {
                if (sell == 1) ss << "One of them was";
                else ss << sell << " of them were";
                ss << " sold for " << g_Game->tariff().creature_sales() << " gold" << (sell > 1 ? " each" : "") << ".";
            }
            if (sell > 1) ss << "\nYou made " << int(gold) << " gold for selling " << (sell == 2 ? "both" : "all") << " of them.";
        }

        ss << "\n \nYou grant her the week off for her body to recover.";
    }
    girl.clear_pregnancy();
    /*
    *    check for death
    */
    if (g_Dice.percent(healthFactor))
    {
        //summary += "And died from it. ";
        ss << "\nSadly, the girl did not survive the experience.";
        girl.set_stat(STAT_HEALTH, 0);
    }
        /*
        *    and sterility
        *    slight mod: 1% death, 5% sterility for monster births
        *    as opposed to other way around. Seems better this way.
        */
    else if (g_Dice.percent(5 + healthFactor))
    {
        // `J` updated old code to use new traits from new code
        ss << "It was a difficult birth and ";
        if (is_broodmother)
        {
            ss << "her womb has been damaged.\n";
            girl.health(-(1 + g_Dice % 2));
            if (girl.health() < 1) girl.set_stat(STAT_HEALTH, 1);    // don't kill her now, it causes all the babies to go away.
        }
        else if (is_fertile)    // loose fertile
        {
            ss << "her womb has been damaged reducing her fertility.\n";
            cGirls::AdjustTraitGroupFertility(girl, -1);
        }
        else    // add sterile
        {
            ss << "she has lost the ability to have children.\n";
            cGirls::AdjustTraitGroupFertility(girl, -1);
        }
    }
    /*
    *    queue the message and return TRUE
    *    because we're not interested in watching
    *    little tentacles grow to adulthood
    */
    if (PlayerControlled) girl.AddMessage(ss.str(), EImageBaseType::BIRTH_HUMAN, EVENT_DANGER);
    girl.m_JustGaveBirth = true;
    return true;
}


void handle_son(sGirl& mom, std::string& summary, bool PlayerControlled)
{
    std::stringstream ss;

    summary += "A son has grown of age. ";
    ss << "Her son has grown of age";
    sGang * gang = g_Game->gang_manager().GetGangNotFull(1, false);
    if (gang && gang->m_Num < 15) {
        gang->m_Num++;
        ss << " and was sent to join your gang " << gang->name() << ".\n";
    } else if (PlayerControlled)    // get the going rate for a male slave and sell the poor sod
    {
        int gold = g_Game->tariff().male_slave_sales();
        g_Game->gold().slave_sales(gold);
        ss << " and has been sold into slavery.\n";
        ss << "You make " << gold << " gold selling the boy.\n";
    } else    // or send him on his way
    {
        int roll = g_Dice % 4;
        ss << " and ";
        if (roll == 0) ss << "moved away";
        else if (roll == 1) ss << "joined the army";
        else ss << "got his own place in town";
        ss << ".\n";
    }
    mom.AddMessage(ss.str(), EImageBaseType::PROFILE, EVENT_GOODNEWS);
}

int calc_abnormal_pc(const sGirl& mom, sGirl& sprog, bool is_players)
{
    if (!is_players)     // the non-pc-daughter case is simpler
    {
        if (mom.has_active_trait(traits::YOUR_DAUGHTER)) return 0;        // if the mom is your daughter then any customer is a safe dad - genetically speaking, anyway
        if (g_Dice.percent(98)) return 0;                    // so what are the odds that this customer fathered both mom and sprog. Let's say 2%
        sprog.raw_traits().add_inherent_trait(traits::INCEST);                    // that's enough to give the sprog the incest trait
        if (!mom.has_active_trait(traits::INCEST)) return 0;    // but there's only a risk of abnormality if mom is herself incestuous
        return 5;                                            // If we get past all that lot, there's a 5% chance of abnormality
    }
    sprog.raw_traits().add_inherent_trait(traits::YOUR_DAUGHTER);                // OK. The sprog is the player's get
    if (!mom.has_active_trait(traits::YOUR_DAUGHTER)) return 0;    // if mom isn't the player's then there is no problem
    sprog.raw_traits().add_inherent_trait(traits::INCEST);                        // she IS, so we add the incest trait
    if (mom.has_active_trait(traits::INCEST)) return 10;                // if mom is also incestuous, that adds 5% to the odds
    return 5;
}

void handle_daughter(sGirl& mom, const sChild& child, std::string& summary) {
    std::stringstream ss;
    bool playerfather = child.m_IsPlayers;        // is 1 if father is player
    summary += "A daughter has grown of age. ";

    // create a new girl for the barn
    std::shared_ptr<sGirl> sprog = g_Game->girl_pool().CreateDaughter(mom, playerfather);

    // check for incest, get the odds on abnormality
    int abnormal_pc = calc_abnormal_pc(mom, *sprog, child.m_IsPlayers);
    if (g_Dice.percent(abnormal_pc)) {
        if (g_Dice.percent(50)) sprog->raw_traits().add_inherent_trait(traits::MALFORMED);
        else sprog->raw_traits().add_inherent_trait(traits::RETARDED);
    }
    // loop through the mom's traits, inheriting where appropriate
    auto moms_traits = mom.raw_traits().get_trait_info();
    for (auto& info : moms_traits) {
        if (info.type == sTraitInfo::INHERENT) {
            if (g_Dice.percent(info.trait->get_properties().get_percent("inherit:chance"))) {
                sprog->raw_traits().add_inherent_trait(info.trait);
            }
        }
    }
    if (playerfather) {
        sprog->raw_traits().add_inherent_trait(traits::YOUR_DAUGHTER);
    }

    sprog->raw_traits().update();

    // inherit stats
    for (auto stat : StatsRange) {
        int min = 0, max = 100;
        if (mom.get_base_stat(stat) < child.m_Stats[stat]) {
            min = mom.get_base_stat(stat);
            max = child.m_Stats[stat];
        }
        else {
            max = mom.get_base_stat(stat);
            min = child.m_Stats[stat];
        }
        sprog->set_stat(stat, (g_Dice % (max - min)) + min);
    }

    // set age to 18, fix health
    sprog->set_stat(STAT_AGE, 18);    // `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live
    sprog->set_stat(STAT_HEALTH, 100);
    sprog->set_stat(STAT_HAPPINESS, 100);
    sprog->set_stat(STAT_TIREDNESS, 0);
    sprog->set_stat(STAT_LEVEL, 0);
    sprog->set_stat(STAT_EXP, 0);

    // `J` set her birthday
    int m0 = g_Game->date().month;
    int d0 = g_Game->date().day;
    d0 -= g_Dice % 7;
    if (d0 < 1) {
        d0 += 30;
        m0--;
    }
    if (m0 < 1) { m0 = 12; }
    sprog->SetBirthMonth(m0);
    sprog->SetBirthDay(d0);

    // inherit skills
    for (auto skill : SkillsRange) {
        int s = 0;
        if (mom.get_base_skill(skill) < child.m_Skills[skill]) s = child.m_Skills[skill];
        else s = mom.get_base_skill(skill);
        sprog->set_skill_direct(static_cast<SKILLS>(skill), g_Dice % std::min(s, 20));
    }

    // calling update makes sure that the new skills respect the caps
    for (auto skill : SkillsRange) {
        sprog->upd_skill(skill, 0);
    }


    // new code to add first and last name to girls

    // at this point the sprog should have temporary firstname, surname, and realname
    std::string prevsurname = sprog->Surname();        // save the temporary surname incase it is needed later
    std::string biography;
    if (playerfather) {
        sprog->SetSurname(g_Game->player().Surname());
        biography = "Daughter of " + mom.FullName() + " and the Brothel owner, Mr. " + g_Game->player().FullName();
    } else {
        if (!mom.Surname().empty()) { // mom has a surname already
            sprog->SetSurname(mom.Surname());
        }
        biography = "Daughter of " + mom.FullName() + " and an anonymous brothel client";
    }
    sprog->m_Desc = sprog->m_Desc + "\n \n" + biography + ".";

    // make sure slave daughters have house perc. set to 100, otherwise 60
    sprog->set_default_house_percent();

    // TODO decide where to send the new girl.
    g_Game->dungeon().AddGirl(sprog, DUNGEON_KID);
    mom.AddMessage(ss.str(), EImageBaseType::PROFILE, EVENT_GOODNEWS);
}

// returns false if the child is not grown up, returns true when the child grows up
bool child_is_grown(sGirl& girl, sChild& child, std::string& summary, bool PlayerControlled)
{
    if (child.m_MultiBirth < 1) child.m_MultiBirth = 1; // `J` fix old code
    if (child.m_MultiBirth > 5) child.m_MultiBirth = 5; // `J` fix old code

    // bump the age - if it's still not grown, go home
    child.m_Age++;        if (child.m_Age < g_Game->settings().get_integer(settings::PREG_WEEKS_GROW))    return false;

    if (child.m_MultiBirth == 1)    // `J` only 1 child so use the old code
    {
        // we need a coming of age ceremony
        if (child.is_boy())
        {
            handle_son(girl, summary, PlayerControlled);
            return true;
        }

        handle_daughter(girl, child, summary);
        return true;
    } else {
        // prepare for the checks
        int numchildren = child.m_MultiBirth;
        int aregirls[5] = { -1, -1, -1, -1, -1 };
        for (int i = 0; i < child.m_MultiBirth; i++)    aregirls[i] = 1;
        for (int i = 0; i < child.m_GirlsBorn; i++)    aregirls[i] = 0;

        for (int i = 0; i < numchildren; i++)
        {
            if (aregirls[i] == 1)    // boys first
            {
                handle_son(girl, summary, PlayerControlled);
            }
            else if (aregirls[i] == 0)    // girls next
            {
                handle_daughter(girl, child, summary);
            }
        }
    }
    return true;
}

void handle_children(sGirl& girl, std::string& summary, bool PlayerControlled) {
    girl.m_JustGaveBirth = false;
    /*
    *    start by advancing pregnancy cooldown time
    */
    if (girl.m_PregCooldown > 0) girl.m_PregCooldown--;

    /*
    *    loop through the girl's children, and divide them into those growing up and those still to be born
    */
    erase_if(girl.m_Children, [&](auto& child){
        if(child->m_Unborn) {
            // some births (monsters) we do not track to adulthood these need removing from the list
            return child_is_due(girl, *child, summary, PlayerControlled);
        } else {
            // the child has been born already if it comes of age we remove it from the list
            return child_is_grown(girl, *child, summary, PlayerControlled);
        }
    });
}