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

#include "jobs/BasicJob.h"
#include "character/sGirl.h"
#include "character/cPlayer.h"
#include "character/predicates.h"
#include "character/cCustomers.h"
#include "cGirls.h"
#include "IGame.h"
#include "cGirlGangFight.h"
#include "sStorage.h"
#include "cInventory.h"
#include "combat/combat.h"
#include "combat/combatant.h"
#include "buildings/IBuilding.h"
#include "buildings/cDungeon.h"

namespace {
    class CityGuard : public cBasicJob {
    public:
        CityGuard();

        sWorkJobResult DoWork(sGirl& girl, bool is_night) override;
        eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;
    };

    class FightBeasts : public cBasicJob {
    public:
        FightBeasts();

        sWorkJobResult DoWork(sGirl& girl, bool is_night) override;
        eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;
    };

    class FightGirls : public cBasicJob {
    public:
        FightGirls();

        sWorkJobResult DoWork(sGirl& girl, bool is_night) override;
        eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;
    };

    class FightTraining : public cBasicJob {
    public:
        FightTraining();

        sWorkJobResult DoWork(sGirl& girl, bool is_night) override;
        eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override;
        double GetPerformance(const sGirl& girl, bool estimate) const override;
    };
}

CityGuard::CityGuard() : cBasicJob(JOB_CITYGUARD, "CityGuard.xml") {
    m_Info.FullTime = true;
}

IGenericJob::eCheckWorkResult CityGuard::CheckWork(sGirl& girl, bool is_night) {
    return SimpleRefusalCheck(girl, ACTION_WORKSECURITY);
}

sWorkJobResult CityGuard::DoWork(sGirl& girl, bool is_night) {
    add_text("work") << "\n\n";

    cGirls::EquipCombat(girl);    // ready armor and weapons!

    int roll_a = d100(), roll_b = d100();
    m_Wages = 150;
    int enjoy = 0, enjoyc = 0, sus = 0;
    int imagetype = IMGTYPE_PROFILE;

    int agl = (girl.agility() / 2 + uniform(0, girl.combat() / 2));

    if (roll_a >= 50)
    {
        ss << "${name} didn't find any trouble today.";
        sus -= 5;
    }
    else if (roll_a >= 25)
    {
        ss << "${name} spotted a thief and ";
        if (agl >= 90)
        {
            ss << "was on them before they could blink.  Putting a stop to the theft.";
            sus -= 20;
            enjoy += 3;
        }
        else if (agl >= 75)
        {
            ss << "was on them before they could get away.  She is quick.";
            sus -= 15;
            enjoy += 1;
        }
        else if (agl >= 50)
        {
            ss << "was able to keep up, ";
            if (roll_b >= 50)    { sus += 5; ss << "but they ended up giving her the slip."; }
            else /*        */    { sus -= 10; ss << "and was able to catch them."; }
        }
        else
        {
            ss << "was left eating dust. Damn is she slow.";
            sus += 10;
            enjoy -= 3;
        }
    }
    else
    {
        auto tempgirl = g_Game->CreateRandomGirl(SpawnReason::ARENA);
        if (tempgirl)        // `J` reworked incase there are no Non-Human Random Girls
        {
            auto outcome = GirlFightsGirl(girl, *tempgirl);
            imagetype = IMGTYPE_COMBAT;
            if (outcome == EFightResult::VICTORY)    // she won
            {
                enjoy += 3; enjoyc += 3;
                ss << "${name} ran into some trouble and ended up in a fight. She was able to win.";
                sus -= 20;
            }
            else  // she lost or it was a draw
            {
                ss << "${name} ran into some trouble and ended up in a fight. She was unable to win the fight.";
                enjoy -= 1; enjoyc -= 1;
                sus += 10;
            }
        }
        else
        {
            g_LogFile.log(ELogLevel::ERROR, "You have no Random Girls for your girls to fight");
            ss << "There were no criminals around for her to fight.\n \n";
            ss << "(Error: You need a Random Girl to allow WorkCityGuard randomness)";
        }
    }

    girl.AddMessage(ss.str(), imagetype, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
    g_Game->player().suspicion(sus);


    // Improve girl
    girl.upd_Enjoyment(ACTION_WORKSECURITY, enjoy);
    girl.upd_Enjoyment(ACTION_COMBAT, enjoyc);

    apply_gains(girl, m_Performance);

    return sWorkJobResult{false, 0, m_Wages, 0};
}

FightBeasts::FightBeasts() : cBasicJob(JOB_FIGHTBEASTS, "FightBeasts.xml") {

}

IGenericJob::eCheckWorkResult FightBeasts::CheckWork(sGirl& girl, bool is_night) {
    if (g_Game->storage().beasts() < 1)
    {
        ss << "${name} had no beasts to fight.";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, is_night? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
        return eCheckWorkResult::IMPOSSIBLE;    // not refusing
    }
    if (girl.disobey_check(ACTION_COMBAT, JOB_FIGHTBEASTS))
    {
        ss << "${name} refused to fight beasts today.\n";
        girl.AddMessage(ss.str(), IMGTYPE_REFUSE, EVENT_NOWORK);
        return eCheckWorkResult::REFUSES;
    }
    return eCheckWorkResult::ACCEPTS;
}

sWorkJobResult FightBeasts::DoWork(sGirl& girl, bool is_night) {
    auto brothel = girl.m_Building;

    cGirls::EquipCombat(girl);
    bool has_armor = girl.get_num_item_equiped(sInventoryItem::Armor);
    bool has_wpn = girl.get_num_item_equiped(sInventoryItem::Weapon) + girl.get_num_item_equiped(sInventoryItem::SmWeapon);

    if (!has_armor)
    {
        ss << "The crowd can't believe you sent ${name} out to fight without armor";
        if (!has_wpn)
        {
            ss << " or a weapon.";
        }
        else
        {
            ss << ". But at least she had a weapon.";
        }
    }
    else
    {
        ss << "${name} came out in armor";
        if (has_wpn)
        {
            ss << " but didn't have a weapon.";
        }
        else
        {
            ss << " and with a weapon in hand. The crowd felt she was ready for battle.";
        }
    }
    ss << "\n \n";

    m_Wages = 175;
    int enjoy = 0;

    // TODO need better dialog
    Combat combat(ECombatObjective::KILL, ECombatObjective::KILL);
    combat.add_combatant(ECombatSide::ATTACKER, girl);
    auto beast = std::make_unique<Combatant>("Beast", 100, 0, 0,
                                             g_Dice.in_range(40, 80), g_Dice.in_range(40, 80), 0,
                                             g_Dice.in_range(40, 80), g_Dice.in_range(40, 80));
    combat.add_combatant(ECombatSide::DEFENDER, std::move(beast));

    auto result = combat.run(15);
    auto report = std::make_shared<CombatReport>();
    report->rounds = combat.round_summaries();
    girl.GetEvents().AddMessage(combat.round_summaries().back(), EVENT_GANG, report);

    if (result == ECombatResult::VICTORY)    // she won
    {
        ss << " fights against a beast. She won the fight.";//was confusing
        enjoy += 3;
        girl.AddMessage(ss.str(), IMGTYPE_COMBAT, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
        int roll_max = girl.fame() + girl.charisma();
        roll_max /= 4;
        m_Wages += uniform(10, 10+roll_max);
        girl.fame(2);
    }
    else  // she lost or it was a draw
    {
        ss << " was unable to win the fight.";
        enjoy -= 1;
        //Crazy i feel there needs be more of a bad outcome for losses added this... Maybe could use some more
        if (brothel->is_sex_type_allowed(SKILL_BEASTIALITY) && !is_virgin(girl))
        {
            ss << " So as punishment you allow the beast to have its way with her."; enjoy -= 1;
            girl.upd_temp_stat(STAT_LIBIDO, -50, true);
            girl.beastiality(2);
            girl.AddMessage(ss.str(), IMGTYPE_BEAST, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
            if (!girl.calc_insemination(cGirls::GetBeast(), 1.0))
            {
                g_Game->push_message(girl.FullName() + " has gotten inseminated", 0);
            }
        }
        else
        {
            ss << " So you send your men in to cage the beast before it can harm her.";
            girl.AddMessage(ss.str(), IMGTYPE_COMBAT, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
            girl.fame(-1);
        }
    }

    int kills = uniform(0, 2);                 // `J` how many beasts she kills 0-2
    if (g_Game->storage().beasts() < kills)    // or however many there are
        kills = g_Game->storage().beasts();
    if (kills < 0) kills = 0;                // can't gain any
    g_Game->storage().add_to_beasts(-kills);

    if (girl.is_pregnant())
    {
        if (girl.strength() >= 60)
        {
            ss << "\n \nAll that fighting proved to be quite exhausting for a pregnant girl, even for one as strong as ${name} .\n";
        }
        else
        {
            ss << "\n \nAll that fighting proved to be quite exhausting for a pregnant girl like ${name} .\n";
        }
        girl.tiredness(10 - girl.strength() / 20 );
    }


    int roll = d100();
    if (roll <= 15)
    {
        ss << " didn't like fighting beasts today.";
        enjoy -= 3;
    }
    else if (roll >= 90)
    {
        ss << " loved fighting beasts today.";
        enjoy += 3;
    }
    else
    {
        ss << " had a pleasant time fighting beasts today.";
        enjoy += 1;
    }
    ss << "\n \n";

    if (girl.is_unpaid())
    {
        m_Wages = 0;
    }

    int earned = 0;
    for (int i = 0; i < m_Performance; i++)
    {
        earned += uniform(5, 15); // 5-15 gold per customer  This may need tweaked to get it where it should be for the pay
    }
    brothel->m_Finance.arena_income(earned);
    ss.str("");
    ss << "${name} drew in " << m_Performance << " people to watch her and you earned " << earned << " from it.";
    girl.AddMessage(ss.str(), IMGTYPE_PROFILE, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

    girl.upd_Enjoyment(ACTION_COMBAT, enjoy);

    apply_gains(girl, m_Performance);

    if (chance(25) && girl.strength() >= 60 && girl.combat() >= girl.magic())
    {
        cGirls::PossiblyGainNewTrait(girl, "Strong", 60, ACTION_COMBAT, "${name} has become pretty Strong from all of the fights she's been in.", is_night);
    }

    return {false, m_Tips, m_Wages, 0};
}

FightGirls::FightGirls() : cBasicJob(JOB_FIGHTARENAGIRLS, "FightGirls.xml"){

}

IGenericJob::eCheckWorkResult FightGirls::CheckWork(sGirl& girl, bool is_night) {
    return SimpleRefusalCheck(girl, ACTION_COMBAT);
}

sWorkJobResult FightGirls::DoWork(sGirl& girl, bool is_night) {
    auto brothel = girl.m_Building;
    ss << "${name} was assigned to fight other girls in the arena.\n \n";

    int enjoy = 0, fame = 0;

    int imagetype = IMGTYPE_COMBAT;

    cGirls::EquipCombat(girl);        // ready armor and weapons!
    auto tempgirl = g_Game->CreateRandomGirl(SpawnReason::ARENA);
    if (tempgirl) {
        auto fight_outcome = GirlFightsGirl(girl, *tempgirl);
        if (fight_outcome == EFightResult::VICTORY)    // she won
        {
            enjoy = uniform(1, 3);
            fame = uniform(1, 3);
            std::shared_ptr<sGirl> ugirl = nullptr;
            if (chance(10))        // chance of getting unique girl
            {
                ugirl = g_Game->GetRandomUniqueGirl(false, false, true);
            }
            if (ugirl)
            {
                std::stringstream msg;    // goes to the girl and the g_MessageQue
                std::stringstream Umsg;    // goes to the new girl
                std::stringstream Tmsg;    // temp msg
                ugirl->set_stat(STAT_HEALTH, uniform(1, 50));
                ugirl->set_stat(STAT_HAPPINESS, uniform(1, 80));
                ugirl->set_stat(STAT_TIREDNESS, uniform(50, 100));
                ugirl->set_status(STATUS_ARENA);
                msg << "${name} won her fight against " << ugirl->FullName() << ".\n \n";
                Umsg << ugirl->FullName() << " lost her fight against your girl ${name}.\n \n";
                Tmsg << ugirl->FullName();
                if (chance(10))
                {
                    ugirl->set_status(STATUS_SLAVE);
                    Tmsg << "'s owner could not afford to pay you your winnings so he gave her to you instead.\n \n";
                }
                else
                {
                    Tmsg << " put up a good fight so you let her live as long as she came work for you.\n \n";
                    m_Wages = uniform(100, 100 + girl.fame() + girl.charisma());
                }
                msg << Tmsg.str();
                Umsg << Tmsg.str();
                ss << msg.str();
                g_Game->push_message(msg.str(), 0);
                ugirl->AddMessage(Umsg.str(), IMGTYPE_PROFILE, EVENT_DUNGEON);

                g_Game->dungeon().AddGirl(ugirl, DUNGEON_NEWARENA);
            }
            else
            {
                add_text("victory") << "\n";
                m_Wages = uniform(100, 100 + girl.fame() + girl.charisma());
            }
        }
        else if (fight_outcome == EFightResult::DEFEAT) // she lost
        {
            enjoy = -uniform(1, 3);
            fame = -uniform(1, 3);
            add_text("defeat") << "\n";
            int cost = 150;
            brothel->m_Finance.arena_costs(cost);
            ss << " You had to pay " << cost << " gold cause your girl lost.";
            /*that should work but now need to make if you lose the girl if you dont have the gold zzzzz FIXME*/
        }
        else  // it was a draw
        {
            enjoy = uniform(-2, 2);
            fame = uniform(-2, 2);
            add_text("draw");
        }
    }
    else {
        g_LogFile.log(ELogLevel::ERROR, "You have no Arena Girls for your girls to fight\n");
        ss << "There were no Arena Girls for her to fight.\n \n(Error: You need an Arena Girl to allow WorkFightArenaGirls randomness)";
        imagetype = IMGTYPE_PROFILE;
    }

    if (girl.is_pregnant())
    {
        if (girl.strength() >= 60)
        {
            ss << "\n \nAll that fighting proved to be quite exhausting for a pregnant girl, even for one as strong as ${name} .\n";
        }
        else
        {
            ss << "\n \nAll that fighting proved to be quite exhausting for a pregnant girl like ${name} .\n";
        }
        girl.tiredness(10 - girl.strength() / 20 );
    }

    if (girl.has_active_trait("Exhibitionist") && chance(15))
    {
        ss << "A flamboyant fighter, ${name} fights with as little armor and clothing as possible, and sometimes takes something off in the middle of a match, to the enjoyment of many fans.\n";
    }

    if (girl.has_active_trait("Idol") && chance(15))
    {
        ss << "${name} has quite the following, and the Arena is almost always packed when she fights.  People just love to watch her in action.\n";
    }

    // Improve girl
    if (girl.is_unpaid())
    {
        m_Wages = 0;
    }

    girl.AddMessage(ss.str(), imagetype, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
    girl.fame(fame);
    girl.upd_Enjoyment(ACTION_COMBAT, enjoy);

    /* `J` this will be a place holder until a better payment system gets done
    *
    */
    int earned = 0;
    for (int i = 0; i < m_Performance; i++)
    {
        earned += uniform(5, 15); // 5-15 gold per customer  This may need tweaked to get it where it should be for the pay
    }
    brothel->m_Finance.arena_income(earned);
    ss.str("");
    ss << "${name} drew in " << m_Performance << " people to watch her and you earned " << earned << " from it.";
    girl.AddMessage(ss.str(), IMGTYPE_PROFILE, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);


    //gain traits
    if (chance(25) && girl.strength() >= 65 && girl.combat() > girl.magic())
    {
        cGirls::PossiblyGainNewTrait(girl, "Strong", 60, ACTION_COMBAT, "${name} has become pretty Strong from all of the fights she's been in.", is_night);
    }
    if (chance(25) && girl.combat() >= 60 && girl.combat() > girl.magic())
    {
        cGirls::PossiblyGainNewTrait(girl, "Brawler", 60, ACTION_COMBAT, "${name} has become pretty good at fighting.", is_night);
    }

#pragma endregion
    return {false, m_Tips, 0, m_Wages};
}

FightTraining::FightTraining() : cBasicJob(JOB_FIGHTTRAIN, "FightTrain.xml") {
}

double FightTraining::GetPerformance(const sGirl& girl, bool estimate) const {
    if (estimate) {
        return (100 - girl.combat()) +
               (100 - girl.magic()) +
               (100 - girl.agility()) +
               (100 - girl.constitution()) +
               (100 - girl.strength());
    }

    return 0.0;
}

IGenericJob::eCheckWorkResult FightTraining::CheckWork(sGirl& girl, bool is_night) {
    if (girl.combat() + girl.magic() + girl.agility() +
        girl.constitution() + girl.strength() >= 500)
    {
        ss << "There is nothing more she can learn here so ${name} takes the rest of the day off.";
        girl.m_NightJob = girl.m_DayJob = JOB_RESTING;
        return eCheckWorkResult::IMPOSSIBLE;    // not refusing
    }
    return SimpleRefusalCheck(girl, ACTION_COMBAT);
}

sWorkJobResult FightTraining::DoWork(sGirl& girl, bool is_night) {
    auto brothel = girl.m_Building;
    add_text("work") << "\n\n";

    cGirls::EquipCombat(girl);    // Ready for combat training

    int enjoy = 0;                                                //
    int train = 0;                                                // main skill trained
    int tcom = girl.combat();                                    // Starting level - train = 1
    int tmag = girl.magic();                                    // Starting level - train = 2
    int tagi = girl.agility();                                    // Starting level - train = 3
    int tcon = girl.constitution();                            // Starting level - train = 4
    int tstr = girl.strength();                                // Starting level - train = 5
    bool gaintrait = false;                                        // posibility of gaining a trait
    int skill = 0;                                                // gian for main skill trained
    int dirtyloss = brothel->m_Filthiness / 100;                // training time wasted with bad equipment
    int sgCmb = 0, sgMag = 0, sgAgi = 0, sgCns = 0, sgStr = 0;    // gains per skill
    int roll_a = d100();                                    // roll for main skill gain
    int roll_b = d100();                                    // roll for main skill trained
    int roll_c = d100();                                    // roll for enjoyment



    /* */if (roll_a <= 5)    skill = 7;
    else if (roll_a <= 15)    skill = 6;
    else if (roll_a <= 30)    skill = 5;
    else if (roll_a <= 60)    skill = 4;
    else /*             */    skill = 3;
    /* */if (girl.has_active_trait("Quick Learner"))    { skill += 1; }
    else if (girl.has_active_trait("Slow Learner"))    { skill -= 1; }
    skill -= dirtyloss;
    ss << "The Arena is ";
    if (dirtyloss <= 0) ss << "clean and tidy";
    if (dirtyloss == 1) ss << "dirty and the equipment has not been put back in its place";
    if (dirtyloss == 2) ss << "messy. The equipment is damaged and strewn about the building";
    if (dirtyloss == 3) ss << "filthy and some of the equipment is broken";
    if (dirtyloss >= 4) ss << "in complete disarray and the equipment barely usable";
    ss << ".\n \n";
    if (skill < 1) skill = 1;    // always at least 1

    do{        // `J` New method of selecting what job to do
        /* */if (roll_b < 20  && tcom < 100)    train = 1;    // combat
        else if (roll_b < 40  && tmag < 100)    train = 2;    // magic
        else if (roll_b < 60  && tagi < 100)    train = 3;    // agility
        else if (roll_b < 80  && tcon < 100)    train = 4;    // constitution
        else if (roll_b < 100 && tstr < 100)    train = 5;    // strength
        roll_b -= 10;
    } while (train == 0 && roll_b > 0);
    if (train == 0 || chance(5)) gaintrait = true;

    if (train == 1) { sgCmb = skill; ss << "She trains in general combat techniques.\n"; }    else sgCmb = uniform(0, 1);
    if (train == 2) { sgMag = skill; ss << "She trains to control her magic.\n"; }            else sgMag = uniform(0, 1);
    if (train == 3) { sgAgi = skill; ss << "She trains her agility.\n"; }                    else sgAgi = uniform(0, 1);
    if (train == 4) { sgCns = skill; ss << "She works on her constitution.\n"; }            else sgCns = uniform(0, 1);
    if (train == 5) { sgStr = skill; ss << "She lifts weights to get stronger.\n"; }        else sgStr = uniform(0, 1);

    if (sgCmb + sgMag + sgAgi + sgCns + sgStr > 0)
    {
        ss << "She managed to gain:\n";
        if (sgCmb > 0) { ss << sgCmb << " Combat\n";        girl.combat(sgCmb); }
        if (sgMag > 0) { ss << sgMag << " Magic\n";            girl.magic(sgMag); }
        if (sgAgi > 0) { ss << sgAgi << " Agility\n";        girl.agility(sgAgi); }
        if (sgCns > 0) { ss << sgCns << " Constitution\n";    girl.constitution(sgCns); }
        if (sgStr > 0) { ss << sgStr << " Strength\n";        girl.strength(sgStr); }
    }

    int trycount = 20;
    while (gaintrait && trycount > 0)    // `J` Try to add a trait
    {
        trycount--;
        switch (uniform(0, 9))
        {
            case 0:
                if (girl.lose_trait( "Fragile"))
                {
                    ss << "She has had to heal from so many injuries you can't say she is fragile anymore.";
                    gaintrait = false;
                }
                else if (girl.gain_trait( "Tough"))
                {
                    ss << "She has become pretty Tough from her training.";
                    gaintrait = false;
                }
                break;
            case 1:
                if (girl.gain_trait( "Adventurer"))
                {
                    ss << "She has been in enough tough spots to consider herself an Adventurer.";
                    gaintrait = false;
                }
                break;
            case 2:
                if (girl.has_active_trait("Nervous") || girl.has_active_trait("Meek") || girl.has_active_trait("Dependant"))
                {
                    if (girl.lose_trait( "Nervous",  50))
                    {
                        ss << "She seems to be getting over her Nervousness with her training.";
                        gaintrait = false;
                    }
                    else if (girl.lose_trait( "Meek", 50))
                    {
                        ss << "She seems to be getting over her Meakness with her training.";
                        gaintrait = false;
                    }
                    else if (girl.lose_trait( "Dependant", 50))
                    {
                        ss << "She seems to be getting over her Dependancy with her training.";
                        gaintrait = false;
                    }
                }
                else
                {
                    if (girl.gain_trait( "Aggressive", 50))
                    {
                        ss << "She is getting rather Aggressive from her enjoyment of combat.";
                        gaintrait = false;
                    }
                    else if (girl.gain_trait( "Fearless", 50))
                    {
                        ss << "She is getting rather Fearless from her enjoyment of combat.";
                        gaintrait = false;
                    }
                    else if (girl.gain_trait( "Audacity", 50))
                    {
                        ss << "She is getting rather Audacious from her enjoyment of combat.";
                        gaintrait = false;
                    }
                }
                break;
            case 3:
                if (girl.gain_trait( "Strong"))
                {
                    ss << "She is getting rather Strong from handling heavy weapons and armor.";
                    gaintrait = false;
                }
                break;
            case 4:
                break;
            case 5:

                break;
            case 6:

                break;

            default:    break;    // no trait gained
        }
    }


    /*

    Small Scars
    Cool Scars
    Horrific Scars
    Bruises
    Idol
    Agile
    Fleet of Foot
    Clumsy
    Strong
    Merciless
    Delicate
    Brawler
    Assassin
    Masochist
    Sadistic
    Tsundere
    Twisted
    Yandere


    Missing Nipple

    Muggle
    Weak Magic
    Strong Magic
    Powerful Magic

    Broken Will
    Iron Will

    Eye Patch
    One Eye

    Shy
    Missing Teeth


    No Arms
    One Arm
    No Hands
    One Hand
    Missing Finger
    Missing Fingers

    No Feet
    No Legs
    One Foot
    One Leg
    Missing Toe
    Missing Toes


    Muscular
    Plump
    Great Figure



    */

    if (girl.is_pregnant())
    {
        if (girl.strength() >= 60)
        {
            ss << "\n \nAll that training proved to be quite exhausting for a pregnant girl, even for one as strong as ${name} .\n";
        }
        else
        {
            ss << "\n \nAll that training proved to be quite exhausting for a pregnant girl like ${name} .\n";
        }
        girl.tiredness(10 - girl.strength() / 20 );
    }


    //enjoyed the work or not
    /* */if (roll_c <= 10)    { enjoy -= uniform(1, 3);    ss << "\n"; }
    else if (roll_c >= 90)    { enjoy += uniform(1, 3);    ss << "\n"; }
    else /*             */    { enjoy += uniform(0, 1);        ss << "\nOtherwise, the shift passed uneventfully."; }
    girl.upd_Enjoyment(ACTION_COMBAT, enjoy);
    girl.upd_Enjoyment(ACTION_WORKTRAINING, enjoy);

    girl.AddMessage(ss.str(), IMGTYPE_COMBAT, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
    brothel->m_Filthiness += 2;    // fighting is dirty
    if (girl.is_unpaid()) { m_Wages = 0; }
    else { m_Wages = 25 + (skill * 5); } // `J` Pay her more if she learns more

    // Improve stats
    int xp = 5 + skill;

    if (girl.has_active_trait("Quick Learner"))        { xp += 2; }
    else if (girl.has_active_trait("Slow Learner"))    { xp -= 2; }

    girl.exp(uniform(1, xp));
    girl.upd_temp_stat(STAT_LIBIDO, int(skill / 2));

    return {false, 0, 0, m_Wages};
}


void RegisterArenaJobs(cJobManager& mgr) {
    mgr.register_job(std::make_unique<CityGuard>());
    mgr.register_job(std::make_unique<FightBeasts>());
    mgr.register_job(std::make_unique<FightGirls>());
    mgr.register_job(std::make_unique<FightTraining>());
}