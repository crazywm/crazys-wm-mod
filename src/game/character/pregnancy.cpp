#include <Game.hpp>
#include <cGirls.h>
#include "pregnancy.h"
#include "ICharacter.h"
#include "IBuilding.hpp"
#include "sGirl.hpp"
#include "predicates.h"

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
    extern const char* PREG_GIRL_CHANCE;
}


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
    if (girl.get_trait_modifier("tag:sterile") > 0)          return 0;
    if (girl.is_pregnant())                                  return 0;
    if (girl.m_PregCooldown > 0)                             return 0;
    if(UseAntiPreg(girl)) {
        return 0;
    }

    int chance = 0;
    if (is_virgin(girl)) chance += 10;
    chance += girl.get_trait_modifier("fertility");
    return chance;
}

void create_pregnancy(sGirl& girl, int num_children, int type, const ICharacter& father) {
    // if there is somehow leftover pregnancy data, clear it
    girl.clear_pregnancy();
    girl.set_status((STATUS)type);

    if (girl.has_active_trait("Broodmother"))
    {
        if (g_Dice.percent(g_Game->settings().get_percent(settings::PREG_MULTI_CHANCE))) num_children++;
        if (g_Dice.percent(g_Game->settings().get_percent(settings::PREG_MULTI_CHANCE))) num_children++;
    }
    auto child = std::make_unique<sChild>(unsigned(type) == STATUS_PREGNANT_BY_PLAYER, sChild::None, num_children);

    // `J` average the mother's and father's stats and skills
    for (int i = 0; i < NUM_STATS; i++)        child->m_Stats[i] = (father.get_base_stat(i) + girl.get_base_stat(i)) / 2;
    /// TODO this doesn't make sense, learned skills should not be inherited
    for (u_int i = 0; i < NUM_SKILLS; i++)    child->m_Skills[i] = (father.get_base_skill(i) + girl.get_base_stat(i)) / 2;
    child->m_Stats[STAT_HEALTH] = 100;
    child->m_Stats[STAT_HAPPINESS] = 100;

    girl.m_Children.push_back(std::move(child));
}