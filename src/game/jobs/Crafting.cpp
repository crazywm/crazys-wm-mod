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

#include "Crafting.h"
#include "Game.hpp"
#include "cGirls.h"
#include "character/sGirl.hpp"
#include "IBuilding.hpp"
#include "cInventory.h"
#include "character/cPlayer.h"

namespace settings {
    extern const char* MONEY_SELL_ITEM;
}

bool GenericCraftingJob::DoWork(sGirl& girl, bool is_night) {
    return WorkCrafting(girl, is_night);
}

bool GenericCraftingJob::WorkCrafting(sGirl& girl, bool is_night) {
    auto brothel = girl.m_Building;
#pragma region //    Job setup                //
    Action_Types actiontype = m_CraftingData.Action;
    if (girl.disobey_check(actiontype, job()))            // they refuse to work
    {
        ss << "${name} refused to work during the " << (is_night ? "night" : "day") << " shift.";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    }
    ss << m_CraftingData.MsgWork << "\n \n";

    cGirls::UnequipCombat(girl);    // put that shit away, you'll scare off the customers!

    int imagetype = m_CraftingData.Image;
    int msgtype = is_night;

#pragma endregion
#pragma region //    Job Performance            //

    jobperformance = GetPerformance(girl, false);
    craftpoints = jobperformance;

    int dirtyloss = brothel->m_Filthiness / 10;        // craftpoints lost due to repairing equipment
    if (dirtyloss > 0)
    {
        craftpoints -= dirtyloss * 2;
        brothel->m_Filthiness -= dirtyloss * 10;
        ss << m_CraftingData.MsgRepair << "\n";
    }

    int wages = m_CraftingData.Wages * (1.0 + (jobperformance - 70) / 100.0);
    performance_msg();
    ss << "\n \n";

#pragma endregion
#pragma region    //    Enjoyment and Tiredness        //
    DoWorkEvents(girl);
    if(girl.is_dead())
        return false;    // not refusing, she is dead
    // TODO tiredness
#pragma endregion

    // not receiving money reduces motivation
    float girl_pay = girl.is_unpaid() ? 0.f : 1.f - girl.house() / 100.f;
    craftpoints *= min(1.0, girl_pay / 2 + 0.66);

    if (craftpoints > 0)
    {
        int points_remaining = (int)craftpoints;
        int numitems = 0;

        while (points_remaining > 0 && numitems < (1 + girl.crafting() / 15))
        {
            auto item = g_Game->inventory_manager().GetCraftableItem(girl, job(), points_remaining);
            if(!item) {
                // try something easier. Get craftable item does not return items which need less than
                // points_remaining / 3 crafting points
                item = g_Game->inventory_manager().GetCraftableItem(girl, job(), points_remaining / 2);
            }
            if(!item) {
                points_remaining -= 10;
                continue;
            }

            points_remaining -= item->m_Crafting.craft_cost();
            girl.mana(-item->m_Crafting.mana_cost());
            msgtype = EVENT_GOODNEWS;
            if (numitems == 0)    ss << "\n \n" << m_CraftingData.MsgProduce << ":";
            ss << "\n" << item->m_Name;
            g_Game->player().add_item(item);
            numitems++;

            // add item sell worth to wages
            wages += min(g_Game->settings().get_percent(settings::MONEY_SELL_ITEM) * item->m_Cost * girl_pay, 100.f);
        }
    }

    // Push out the turn report
    girl.AddMessage(ss.str(), imagetype, msgtype);

    // Money
    girl.m_Tips = 0;
    girl.m_Pay = max(0, wages);

    // Base Improvement and trait modifiers
    int xp = 5, skill = 3;
    if (girl.has_active_trait("Quick Learner"))        { skill += 1; xp += 3; }
    else if (girl.has_active_trait("Slow Learner"))    { skill -= 1; xp -= 3; }
    // EXP and Libido
    girl.exp(uniform(1, 1+xp));

    // primary improvement (+2 for single or +1 for multiple)
    if(m_CraftingData.PrimaryGains.size() == 1) {
        girl.update_attribute(m_CraftingData.PrimaryGains.front(), uniform(2, 2 + skill));
    } else {
        for(auto& att : m_CraftingData.PrimaryGains) {
            girl.update_attribute(att, uniform(1, skill + 1));
        }
    }
    // secondary improvement (-1 for one then -2 for others)
    int sub = 1;
    for(auto& att : m_CraftingData.SecondaryGains) {
        girl.update_attribute(att, max(0,(uniform(0, skill)-sub)));
        sub = -2;
    }

    // Update Enjoyment
    girl.upd_Enjoyment(actiontype, enjoy);
    // Gain Traits
    // gain simple traits
    for(auto& trait : m_CraftingData.TraitChanges) {
        if(trait.Gain) {
            cGirls::PossiblyGainNewTrait(girl, trait.TraitName, trait.Threshold, trait.Action,
                                         trait.Message, is_night, trait.EventType);
        } else {
            cGirls::PossiblyLoseExistingTrait(girl, trait.TraitName, trait.Threshold, trait.Action,
                                              trait.Message, is_night);
        }
    }

#pragma endregion
    return false;
}

void GenericCraftingJob::DoWorkEvents(sGirl& girl) {
    int roll = d100();
    //enjoyed the work or not
    if (roll <= 5)
    {
        ss << "\nSome of the patrons abused her during the shift.";
        enjoy -= 1;
    }
    else if (roll <= 25)
    {
        ss << "\nShe had a pleasant time working.";
        enjoy += 3;
    }
    else
    {
        ss << "\nOtherwise, the shift passed uneventfully.";
        enjoy += 1;
    }
}

void GenericCraftingJob::performance_msg() {
    if (jobperformance >= 245)
    {
        ss << " She must be the perfect at this.";
    }
    else if (jobperformance >= 185)
    {
        ss << " She's unbelievable at this.";;
    }
    else if (jobperformance >= 145)
    {
        ss << " She's good at this job.";
    }
    else if (jobperformance >= 100)
    {
        ss << " She made a few mistakes but overall she is okay at this.";
    }
    else if (jobperformance >= 70)
    {
        ss << " She was nervous and made a few mistakes. She isn't that good at this.";
    }
    else
    {
        ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.";
    }
}

struct cBlacksmithJob : GenericCraftingJob {
    cBlacksmithJob();
    void DoWorkEvents(sGirl& girl) override;
};

cBlacksmithJob::cBlacksmithJob() :
        GenericCraftingJob(JOB_BLACKSMITH,
                {IMGTYPE_CRAFT, ACTION_WORKMAKEITEMS, "work.blacksmith",
                 {SKILL_CRAFTING, STAT_STRENGTH},
                 {STAT_CONSTITUTION, SKILL_COMBAT, SKILL_MAGIC, STAT_INTELLIGENCE},
                 40,
                 "${name} worked as a blacksmith at the arena.",
                 "She spent some of her time repairing the Arena's equipment instead of making new stuff.",
                 "${name} made:",
                 {{true, "Tough", 50, ACTION_WORKMAKEITEMS, "Working in the heat of the forge has made ${name} rather Tough."}}
                }
        ) {

}

void cBlacksmithJob::DoWorkEvents(sGirl& girl) {
    int tired = (300 - (int)jobperformance);    // this gets divided in roll_a by (8, 10 or 12) so it will end up around 0-40 tired
    int roll_a = uniform(0, 100) + (jobperformance - 75) / 20;
    int roll_b = uniform(0, 100);
    if (roll_a <= 10)
    {
        tired /= 8;
        enjoy -= uniform(0, 3);
        if (roll_b < 10)    // fire
        {
            int fire = max(0, rng().bell(-2, 10));
            girl.m_Building->m_Filthiness += fire * 2;
            craftpoints *= (1 - fire * 0.1);
            if (girl.pcfear() > 20) girl.pcfear(fire / 2);    // she is afraid you will get mad at her
            ss << "She accidentally started a fire";
            /* */if (fire < 3)    ss << " but it was quickly put out.";
            else if (fire < 6)    ss << " that destroyed several racks of equipment.";
            else if (fire < 10)    ss << " that destroyed most of the equipment she had made.";
            else /*          */    ss << " destroying everything she had made.";

            if (fire > 5) g_Game->push_message(girl.FullName() + " accidently started a large fire while working as a Blacksmith at the Arena.", COLOR_RED);
        }
        else if (roll_b < 30)    // injury
        {
            girl.health(-(1 + uniform(0, 5)));
            craftpoints *= 0.8;
            if (girl.magic() > 50 && girl.mana() > 20)
            {
                girl.mana(-uniform(10, 20));
                ss << "While trying to enchant an item, the magic rebounded on her.";
            }
            else
                ss << "She burnt herself in the heat of the forge.";
            if (girl.is_dead())
            {
                ss << " It killed her.";
                g_Game->push_message(girl.FullName() + " was killed in an accident while working as a Blacksmith at the Arena.", COLOR_RED);
            }
            else ss << ".";
        }

        else    // unhappy
        {
            ss << "She did not like working in the arena today.";
            girl.happiness(-uniform(0, 11));
        }
    }
    else if (roll_a >= 90)
    {
        tired /= 12;
        craftpoints *= 1.1;
        enjoy += uniform(0, 3);
        /* */if (roll_b < 50)    ss << "She kept a steady pace of hammer blows by humming a pleasant tune.";
        else /*            */    ss << "She had a great time working today.";
    }
    else
    {
        tired /= 10;
        enjoy += uniform(0, 2);
        ss << "The shift passed uneventfully.";
    }
    ss << "\n \n";
}

struct cCobblerJob : GenericCraftingJob {
    cCobblerJob();
    void DoWorkEvents(sGirl& girl) override;
};

cCobblerJob::cCobblerJob() :
    GenericCraftingJob(JOB_COBBLER,
            {IMGTYPE_CRAFT, ACTION_WORKMAKEITEMS, "work.cobbler",
             {SKILL_CRAFTING},
             {SKILL_SERVICE, STAT_INTELLIGENCE, SKILL_MAGIC},
             20,
             "${name} worked making shoes and other leather items at the arena.",
             "She spent some of her time repairing the Arena's equipment instead of making new stuff.",
             "${name} made:",
             {}
            }
    ) {
}

void cCobblerJob::DoWorkEvents(sGirl& girl) {
    int tired = (300 - (int)jobperformance);    // this gets divided in roll_a by (10, 12 or 14) so it will end up around 0-23 tired
    int roll_a = uniform(0, 100) + (jobperformance - 75) / 20;
    int roll_b = uniform(0, 100);
    if (roll_a <= 10)
    {
        tired /= 10;
        enjoy -= uniform(0, 3);
        if (roll_b < 20)    // injury
        {
            girl.health(-uniform(1, 5));
            craftpoints *= 0.8;
            if (girl.magic() > 50 && girl.mana() > 20)
            {
                girl.mana(-uniform(10, 20));
                ss << "While trying to enchant an item, the magic rebounded on her";
            }
            else
                ss << "She hurt herself while making items";
            if (girl.is_dead())
            {
                ss << " killing her.";
                g_Game->push_message(girl.FullName() + " was killed in an accident while working as a Cobbler at the Arena.", COLOR_RED);
            }
            else ss << ".";
        }

        else    // unhappy
        {
            ss << "She did not like";
            ss << rng().select_text({" making shoes today.", " working with animal hides today."});
            girl.happiness(-uniform(0, 11));
        }
    }
    else if (roll_a >= 90)
    {
        tired /= 14;
        craftpoints *= 1.1;
        enjoy += uniform(0, 3);
        /* */if (roll_b < 50)    ss << "She kept a steady pace with her needle work by humming a pleasant tune.";
        else /*            */    ss << "She had a great time working today.";
    }
    else
    {
        tired /= 12;
        enjoy += uniform(0, 2);
        ss << "The shift passed uneventfully.";
    }
    ss << "\n \n";
}


struct cMakeItemJob : GenericCraftingJob {
    cMakeItemJob();

    void DoWorkEvents(sGirl& girl) override;
};

cMakeItemJob::cMakeItemJob() :
        GenericCraftingJob(JOB_MAKEITEM,
                {IMGTYPE_CRAFT, ACTION_WORKMAKEITEMS, "work.makeitem",
                 {SKILL_CRAFTING},
                 {SKILL_SERVICE, STAT_INTELLIGENCE, SKILL_MAGIC},
                 20,
                 "${name} was assigned to make items at the farm.",
                 "She spent some of her time repairing the Farm's equipment instead of making new stuff.",
                 "${name} made:",
                 {}
                }
        ) {
}

void cMakeItemJob::DoWorkEvents(sGirl& girl) {
    int tired = (300 - (int)jobperformance);    // this gets divided in roll_a by (8, 10 or 12) so it will end up around 0-40 tired
    int roll_a = uniform(0, 100) + (jobperformance - 75) / 20;
    int roll_b = uniform(0, 100);
    if (roll_a <= 10)
    {
        tired /= 14;
        enjoy -= uniform(0, 3);
        if (roll_b < 30)    // injury
        {
            girl.health(-uniform(1, 5));
            craftpoints *= 0.8;
            if (girl.magic() > 50 && girl.mana() > 20)
            {
                girl.mana(-uniform(10, 20));
                ss << "While trying to enchant an item, the magic rebounded on her";
            }
            else
                ss << "She injured herself with the " << (chance(40) ? "sharp" : "heavy") << " tools";
            if (girl.is_dead())
            {
                ss << " killing her.";
                g_Game->push_message(girl.FullName() + " was killed in an accident while making items at the Farm.", COLOR_RED);
            }
            else ss << ".";
        }
        else    // unhappy
        {
            ss << "She did not like making things today.";
            girl.happiness(-uniform(0, 11));
        }
    }
    else if (roll_a >= 90)
    {
        tired /= 20;
        craftpoints *= 1.1;
        enjoy += uniform(0, 3);
        /* */if (roll_b < 50)    ss << "She kept a steady pace by humming a pleasant tune.";
        else /*            */    ss << "She had a great time working today.";
    }
    else
    {
        tired /= 17;
        enjoy += uniform(0, 2);
        ss << "The shift passed uneventfully.";
    }
    ss << "\n \n";
}

struct cMakePotionsJob : GenericCraftingJob {
    cMakePotionsJob();

    void DoWorkEvents(sGirl& girl) override;
};


cMakePotionsJob::cMakePotionsJob() :
        GenericCraftingJob(JOB_MAKEPOTIONS,
                {IMGTYPE_CRAFT, ACTION_WORKMAKEPOTIONS, "work.makepotions",
                 {SKILL_BREWING, SKILL_HERBALISM},
                 {STAT_INTELLIGENCE, SKILL_COOKING, SKILL_MAGIC},
                 20,
                 "${name} worked as a potions maker on the farm.",
                 "She spent some of her time repairing the Farm's equipment instead of making new stuff.",
                 "${name} made:",
                 {}
                }
        ) {
}

void cMakePotionsJob::DoWorkEvents(sGirl& girl) {
    int roll = uniform(0, 100) + (jobperformance - 75) / 20;
    //enjoyed the work or not
    if (roll >= 90)
    {
        enjoy += uniform(1, 4);
        ss << "She had a great time making potions today.";
    }
    else if (roll <= 10)
    {
        enjoy -= uniform(1, 6);
        ss << "Some potions blew up in her face today.";
        girl.health(-uniform(0, 10));
        girl.happiness(-uniform(0, 20));
        girl.beauty(-uniform(0, 3));

    }
    else if (roll <= 20)
    {
        enjoy -= uniform(1, 4);
        ss << "She did not like making potions today.";
    }
    else
    {
        enjoy += uniform(0, 2);
        ss << "The shift passed uneventfully.";
    }
    ss << "\n \n";
}

struct cTailorJob : GenericCraftingJob {
    cTailorJob();

    void DoWorkEvents(sGirl& girl) override;
};


cTailorJob::cTailorJob() :
        GenericCraftingJob(JOB_TAILOR,
                {IMGTYPE_CRAFT, ACTION_WORKMAKEITEMS, "work.makepotions",
                 {SKILL_CRAFTING},
                 {STAT_INTELLIGENCE, SKILL_SERVICE, SKILL_MAGIC},
                 20,
                 "${name} worked making and mending clothes at the farm.",
                 "She spent some of her time repairing the Farm's equipment instead of making new stuff.",
                 "${name} made:",
                 {}
                }
        ) {
}

void cTailorJob::DoWorkEvents(sGirl& girl) {
    int tired = (300 - (int)jobperformance);    // this gets divided in roll_a by (8, 10 or 12) so it will end up around 0-40 tired
    int roll_a = uniform(0, 100) + (jobperformance - 75) / 20;
    int roll_b = uniform(0, 100);
    if (roll_a <= 10)
    {
        tired /= 8;
        enjoy -= uniform(0, 3);
        if (roll_b < 20)    // injury
        {
            girl.health(-uniform(1, 6));
            craftpoints *= 0.8;
            if (girl.magic() > 50 && girl.mana() > 20)
            {
                girl.mana(-uniform(10, 20));
                ss << "While trying to enchant an item, the magic rebounded on her";
            }
            else
                ss << "She stabbed herself while working";
            if (girl.is_dead())
            {
                ss << " killing her.";
                g_Game->push_message(girl.FullName() + " was killed in an accident while working as a Tailor at the Farm.", COLOR_RED);
            }
            else ss << ".";
        }

        else    // unhappy
        {
            ss << "She did not like working as a tailor today.";
            girl.happiness(-uniform(0, 11));
        }
    }
    else if (roll_a >= 90)
    {
        tired /= 12;
        craftpoints *= 1.1;
        enjoy += uniform(0, 3);
        /* */if (roll_b < 50)    ss << "She kept a steady pace with her needle work by humming a pleasant tune.";
        else /*            */    ss << "She had a great time working today.";
    }
    else
    {
        tired /= 10;
        enjoy += uniform(0, 2);
        ss << "The shift passed uneventfully.";
    }
    ss << "\n \n";
}


struct cGardenerJob : GenericCraftingJob {
    cGardenerJob();
};

cGardenerJob::cGardenerJob() :
        GenericCraftingJob(JOB_GARDENER,
                {IMGTYPE_FARM, ACTION_WORKFARM, "work.gardener",
                 {SKILL_HERBALISM},
                 {SKILL_FARMING, STAT_INTELLIGENCE, STAT_CONSTITUTION},
                 20,
                 "${name} worked as a gardener on the farm.",
                 "She spent some of her time repairing the Farm's equipment instead of gardening.",
                 "${name} was able to harvest:",
                 {}
                }
        ) {
}

struct cJewelerJob : GenericCraftingJob {
    cJewelerJob();
    void DoWorkEvents(sGirl& girl) override;
};

cJewelerJob::cJewelerJob() :
        GenericCraftingJob(JOB_JEWELER,
                {IMGTYPE_CRAFT, ACTION_WORKMAKEITEMS, "work.jeweler",
                 {SKILL_CRAFTING},
                 {STAT_AGILITY, STAT_CONFIDENCE, STAT_INTELLIGENCE, SKILL_MAGIC},
                 40,
                 "worked as a Jeweler at the arena.",
                 "She spent some of her time repairing the Arena's equipment instead of making new stuff.",
                 "${name} crafted:",
                 {{true, "Sharp-Eyed", 50, ACTION_WORKMAKEITEMS, "Working on such small items has made ${name} rather Sharp-Eyed."}}
                }
        ) {
}

void cJewelerJob::DoWorkEvents(sGirl& girl) {
    int tired = (300 - (int)jobperformance);    // this gets divided in roll_a by (8, 10 or 12) so it will end up around 0-40 tired
    int roll_a = uniform(0, 100) + (jobperformance - 75) / 20;
    int roll_b = uniform(0, 100);
    if (roll_a <= 10)
    {
        tired /= 8;
        enjoy -= uniform(0, 3);
        if (roll_b < 10)    // fire
        {
            int fire = max(0, rng().bell(-2, 10));
            girl.m_Building->m_Filthiness += fire * 2;
            craftpoints -= (craftpoints * (fire * 0.1));
            if (girl.pcfear() > 20) girl.pcfear(fire / 2);    // she is afraid you will get mad at her
            ss << "She accidently started a fire";
            /* */if (fire < 3)    ss << " but it was quickly put out.";
            else if (fire < 6)    ss << " that destroyed several racks of equipment.";
            else if (fire < 10)    ss << " that destroyed most of the equipment she had made.";
            else /*          */    ss << " destroying everything she had made.";

            if (fire > 5) g_Game->push_message(girl.FullName() + " accidently started a large fire while working as a Jeweler at the Arena.", COLOR_RED);
        }
        else if (roll_b < 30)    // injury
        {
            girl.health(-uniform(1, 6));
            craftpoints *= 0.8;
            if (girl.magic() > 50 && girl.mana() > 20)
            {
                girl.mana(-uniform(10, 20));
                ss << "While trying to enchant an item, the magic rebounded on her";
            }
            else
                ss << "She burnt herself in the heat of the forge";
            if (girl.is_dead())
            {
                ss << " killing her.";
                g_Game->push_message(girl.FullName() + " was killed in an accident while working as a Jeweler at the Arena.", COLOR_RED);
            }
            else ss << ".";
        }

        else    // unhappy
        {
            ss << "She did not like working in the arena today.";
            girl.happiness(-uniform(0, 11));
        }
    }
    else if (roll_a >= 90)
    {
        tired /= 12;
        craftpoints *= 1.1;
        enjoy += uniform(0, 3);
        /* */if (roll_b < 50)    ss << "She kept a steady pace of hammer blows by humming a pleasant tune.";
        else /*            */    ss << "She had a great time working today.";
    }
    else
    {
        tired /= 10;
        enjoy += uniform(0, 2);
        ss << "The shift passed uneventfully.";
    }
    ss << "\n \n";
}

void RegisterCraftingJobs(cJobManager& mgr) {
    mgr.register_job(std::make_unique<GenericCraftingJob>(
            JOB_BAKER, sCraftingJobData{
            IMGTYPE_COOK, ACTION_WORKCOOKING, "work.baker",
            {SKILL_COOKING},
            {SKILL_SERVICE, STAT_INTELLIGENCE, SKILL_HERBALISM}, 20,
            "${name} worked as a baker on the farm.",
            "She spent some of her time repairing the Farm's equipment instead of making new stuff.",
            "${name} made:",
            {{true, "Chef", 70, ACTION_WORKCOOKING, "${name} has prepared enough food to qualify as a Chef."}}
            }));
    mgr.register_job(std::make_unique<GenericCraftingJob>(
            JOB_BREWER, sCraftingJobData{
                 IMGTYPE_COOK, ACTION_WORKCOOKING, "work.brewer",
                 {SKILL_BREWING},
                 {SKILL_HERBALISM, SKILL_COOKING, STAT_INTELLIGENCE},
                 20,
                 "${name} worked as a brewer on the farm.",
                 "She spent some of her time repairing the Farm's equipment instead of making new stuff.",
                 "${name} made:",
                 {}
            }));
    mgr.register_job(std::make_unique<GenericCraftingJob>(
            JOB_BUTCHER, sCraftingJobData{
                    IMGTYPE_COOK, ACTION_WORKCOOKING, "work.butcher",
                    {SKILL_ANIMALHANDLING, STAT_STRENGTH},
                    {SKILL_MEDICINE, SKILL_COOKING, STAT_INTELLIGENCE},
                    20,
                    "${name} worked as a butcher on the farm.",
                    "She spent some of her time repairing the Farm's equipment instead of making new stuff.",
                    "${name} made:",
                    {}
            }));
    mgr.register_job(std::make_unique<cBlacksmithJob>());
    mgr.register_job(std::make_unique<cCobblerJob>());
    mgr.register_job(std::make_unique<cGardenerJob>());
    mgr.register_job(std::make_unique<cJewelerJob>());
    mgr.register_job(std::make_unique<cMakeItemJob>());
    mgr.register_job(std::make_unique<cMakePotionsJob>());
    mgr.register_job(std::make_unique<cTailorJob>());
}
