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
#include <fstream>
#include <algorithm>
#include "cTariff.h"
#include "cGirls.h"
#include <tinyxml2.h>
#include "utils/algorithms.hpp"
#include "cGangs.h"
#include "cGangManager.hpp"
#include "buildings/IBuilding.h"
#include "IGame.h"
#include "sStorage.h"
#include "character/sGirl.h"
#include "character/cCustomers.h"
#include "cInventory.h"
#include "CLog.h"
#include "xml/util.h"
#include "scripting/GameEvents.h"
#include "cNameList.h"
#include "buildings/cDungeon.h"
#include "cJobManager.h"

#include "utils/DirPath.h"
#include "utils/FileList.h"
#include "utils/string.hpp"
#include "character/traits/ITraitSpec.h"
#include "Inventory.h"
#include "character/traits/ITraitsCollection.h"
#include "character/predicates.h"
#include "character/cPlayer.h"
#include "character/pregnancy.h"
#include "character/cGirlPool.h"
#include "cGirlGangFight.h"

namespace settings {
    extern const char* USER_HOUSE_PERCENT_FREE;
    extern const char* USER_HOUSE_PERCENT_SLAVE;
    extern const char* USER_ACCOMODATION_FREE;
    extern const char* USER_ACCOMODATION_SLAVE;
    extern const char* USER_ITEMS_AUTO_EQUIP_COMBAT;
    extern const char* USER_SHOW_NUMBERS;
    extern const char* USER_HOROSCOPE;
    extern const char* PREG_WEEKS_GROW;
    extern const char* PREG_MULTI_CHANCE;
    extern const char* PREG_MISS_HUMAN;
    extern const char* PREG_MISS_MONSTER;
    extern const char* PREG_COOL_DOWN;
}

extern cRng g_Dice;

extern cNameList g_GirlNameList;
extern cNameList g_BoysNameList;
extern cNameList g_SurnameList;

using namespace std;

// ----- Create / destroy

cGirls::cGirls() : m_Girls(std::make_unique<cGirlPool>())
{
}
cGirls::~cGirls() = default;

// ----- Misc

void cGirls::CalculateGirlType(sGirl& girl)
{
    std::array<int, NUM_FETISH> ratings;
    for(int i = 0; i < NUM_FETISH; ++i) {
        ratings[i] = girl.get_trait_modifier( (std::string("fetish:") + get_fetish_name((Fetishs)i)).c_str() );
    }
    girl.m_FetishTypes.clear();
    // can have either BIGBOOBS or SMALLBOOBS fetish
    if (ratings[FETISH_BIGBOOBS] > ratings[FETISH_SMALLBOOBS])
    {
        ratings[FETISH_SMALLBOOBS] = 0;
    }
    else
    {
        ratings[FETISH_BIGBOOBS] = 0;
    }

    // skip over FETISH_TRYANYTHING and FETISH_SPECIFICGIRL, as they do not apply here
    for(unsigned fetish = FETISH_BIGBOOBS; fetish < NUM_FETISH; ++fetish) {
        if(ratings[fetish] >= 50 ) {
            girl.m_FetishTypes.insert((Fetishs)fetish);
        }
    }
}

bool cGirls::CheckGirlType(const sGirl& girl, Fetishs type)
{
    return type == FETISH_TRYANYTHING || girl.m_FetishTypes.count(type);
}

void cGirls::UpdateAskPrice(sGirl& girl, bool vari)
{
    int askPrice = (int)(((girl.beauty() + girl.charisma()) / 2)*0.6f);    // Initial price
    askPrice += girl.confidence() / 10;        // their confidence will make them think they are worth more
    askPrice += girl.intelligence() / 10;        // if they are smart they know they can get away with a little more
    askPrice += girl.fame() / 2;                // And lastly their fame can be quite useful too
    if (girl.level() > 0)    askPrice += girl.level() * 10;  // MYR: Was * 1

    if (askPrice > 100) askPrice = 100;
    if (askPrice < 0) askPrice = 0;
    girl.set_stat(STAT_ASKPRICE, askPrice);
    if (vari)    // vari is used when calculating for jobs so she can charge customers a little more
    {
        // `J` changed so there is better variation but with a chance of negatives
        int lowend = 0;    int highend = 0;
        // dumb girls will charge less, smart girls will charge more
        lowend    += (girl.intelligence() / 10) - 7;    // -7 to +3
        highend    += (girl.intelligence() / 8);        // +0 to +12
        // timid girls will charge less, confident girls will charge more
        lowend    += (girl.confidence() / 10) - 7;    // -7 to +3
        highend    += (girl.confidence() / 7);        // +0 to +14

        askPrice += g_Dice.bell(lowend, highend);

        if (askPrice < 1) askPrice = 1;
        girl.set_stat(STAT_ASKPRICE, askPrice);
    }

}

// `J` moved exp check into levelup to reduce coding
void cGirls::LevelUp(sGirl& girl)
{
    int level = girl.level();
    int xp = girl.exp();
    int xpneeded = GetRequiredXP(girl);

    if (xp < xpneeded) return;

    girl.set_stat(STAT_EXP, xp - xpneeded);
    girl.level(1);

    if (girl.level() <= 20)    LevelUpStats(girl);

    stringstream ss;
    ss << "${name} levelled up to " << girl.level() << ".";
    girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_LEVELUP);
    ss.str("");

    // add traits
    // MYR: One chance to get a new trait every five levels.
    if (girl.level() % 5 == 0)
    {
        int addedtrait = girl.level() + 5;
        while (addedtrait > 0)
        {
            int chance = g_Dice % 12;
            string trait;
            switch (chance)
            {
            case 1:        trait = "Agile";                break;
            case 2:        trait = "Charismatic";            break;
            case 3:        trait = "Charming";                break;
            case 4:        trait = "Cool Person";            break;
            case 5:        trait = "Fake Orgasm Expert";    break;
            case 6:        trait = "Fleet of Foot";        break;
            case 7:        trait = "Good Kisser";            break;
            case 8:        trait = "Nimble Tongue";        break;
            case 9:        trait = "Nymphomaniac";            break;
            case 10:    trait = "Open Minded";            break;
            case 11:    trait = "Sexy Air";                break;
            default: break;
            }
            if (!trait.empty() && girl.gain_trait(trait.c_str()))
            {
                addedtrait = 0;
                ss << " She has gained the " << trait << " trait.";
                girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);
            }
            addedtrait--;
        }
    }

    // check if the girl managed to jump a level.
    LevelUp(girl);
}

void cGirls::LevelUpStats(sGirl& girl)
{
    int DiceSize = 3;
    if (girl.has_active_trait("Quick Learner")) DiceSize = 4;
    else if (girl.has_active_trait("Slow Learner")) DiceSize = 2;

    // level up stats (only first 8 advance in levelups)
    for (int i = 0; i < 8; i++) girl.upd_base_stat((STATS)i, g_Dice % DiceSize);

    // level up skills
    for (int i = 0; i < NUM_SKILLS; i++)    girl.upd_skill(i, g_Dice%DiceSize);
}

/*
*   `J` degrade the girls skills at the end of the turn.
*    if a sex type is banned, 10% chance she will lose 1 point in it
*   all other skills have a 5% chance to lose 1 point
*/
void cGirls::EndDayGirls(IBuilding& brothel, sGirl& girl)
{
    stringstream goodnews;
    /* */if (girl.m_NumCusts == girl.m_NumCusts_old)    {}    // no customers
    else if (girl.m_NumCusts < girl.m_NumCusts_old)    {}    // lost customers??
    else if (girl.m_NumCusts_old == 0 && girl.m_NumCusts > 0)
    {
        goodnews << "${name} has serviced her first ";
        /* */if (girl.m_NumCusts == 1)            goodnews << " customer.";
        else if (girl.m_NumCusts == 2)        goodnews << " pair of customers.";
        else if (girl.m_NumCusts == 12)    goodnews << " dozen customers.";
        else        goodnews << (int)girl.m_NumCusts << " customers. ";
        goodnews << " She is sure to service more as long as she works for you.";
    }
    else if (girl.m_NumCusts_old < 100 && girl.m_NumCusts >= 100)
    {
        goodnews << "${name} serviced her first hundred customers.";
        if (girl.has_active_trait("Optimist") && girl.happiness() > 80) goodnews << " She seems pleased with her accomplishment and looks forward to reaching the next level.";
        else goodnews << " You see great potential in this one.";
        girl.fame(1);
    }
    else if (girl.m_NumCusts_old < 500 && girl.m_NumCusts >= 500)
    {
        goodnews << "${name} serviced five hundred customers.";
        if (girl.has_active_trait("Optimist") && girl.happiness() > 80) goodnews << " She seems pleased with her accomplishment and looks forward to reaching the next level.";
        girl.gain_trait("Whore");
        girl.fame(5);
    }
    else if (girl.m_NumCusts_old < 1000 && girl.m_NumCusts >= 1000)
    {
        goodnews << "${name} has slept with 1000 people.. You gotta wonder if its like throwing a hot dog down a hallway at this point.";
        girl.fame(10);

    }
    if (goodnews.str().length() > 2)    girl.AddMessage(goodnews.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);


    girl.m_NumCusts_old = girl.m_NumCusts;            // prepare for next week

    int E_mana = 0, E_libido = 0, E_lactation = 0;

    /* */if (girl.has_active_trait("Muggle")) E_mana = girl.magic() / 50;    // max 2 per day
    else if (girl.has_active_trait("Weak Magic")) E_mana = girl.magic() / 20;    // max 5 per day
    else if (girl.has_active_trait("Strong Magic")) E_mana = girl.magic() / 5;        // max 20 per day
    else if (girl.has_active_trait("Powerful Magic")) E_mana = girl.magic() / 2;        // max 50 per day
    else /*                                 */    E_mana = girl.magic() / 10;    // max 10 per day
    girl.mana(E_mana);


    // `J` update the girls base libido
    int total_libido = girl.libido();                // total_libido
    int base_libido = girl.libido();    // base_libido
    if (total_libido > (base_libido*1.5)) E_libido++;
    if (total_libido > 90)    E_libido++;
    if (total_libido < 10)    E_libido--;
    if (total_libido < (base_libido / 3)) E_libido--;
    girl.libido(E_libido);


    /* `J` lactation is not really thought out fully
    *    lactation traits affect how quickly she refills
    *    breast size affects how much she produces
    *    pregnancy doubles lactation
    *    pregnant cow girl will alwasy be ready to milk
    //*/
    if (!girl.has_active_trait("No Nipples"))    // no nipples = no lactation
    {
        if      (girl.has_active_trait("Dry Milk")) E_lactation = 1;
        else if (girl.has_active_trait("Scarce Lactation")) E_lactation = 5;
        else if (girl.has_active_trait("Abundant Lactation")) E_lactation = 25;
        else if (girl.has_active_trait("Cow Tits")) E_lactation = 50;
        else E_lactation = 0;

        // *Add* (not set!) E_lactation
        girl.lactation(girl.is_pregnant() || girl.m_PregCooldown > 0 ? E_lactation * 2 + 1 : E_lactation);
    }

    for(auto skill : SkillsRange) {
        int val = girl.get_base_skill(skill);
        // in the lowest range, skills don't decay
        if(val < 10) continue;

        // otherwise, a skill might decay with a low base probability (1%)
        // or with a larger probability if it is close to the cap (5%)
        // a high level skill very close to the cap has a large decay chance (10%)
        int cap = g_Game->get_skill_cap(skill, girl);
        int chance = g_Dice.d100();
        if(chance < 1 || (chance < 5 && val + 10 > cap) || (chance < 10 && val + 5 > cap && val >= 60)) {
            girl.upd_skill(skill, -1);
        }
    }
}

// ----- Add remove

void cGirls::AddGirl(shared_ptr<sGirl> girl)
{
    m_Girls->AddGirl(std::move(girl));
}

shared_ptr<sGirl> cGirls::TakeGirl(const sGirl* girl)
{
    return m_Girls->TakeGirl(girl);
}

void cGirls::GiveGirl(std::shared_ptr<sGirl> girl)
{
    if (girl->IsUnique()) {
        m_Girls->AddGirl(std::move(girl)); // only add unique girls back to main pool
    }
}

string cGirls::GetGirlMood(const sGirl& girl)
{
    stringstream ss;
    ss << girl.FullName();

    int HateLove = girl.pclove();
    ss << " feels the player ";

    if (girl.has_active_trait("Your Daughter"))
    {
        /* */if (HateLove <= -80)    ss << "should die ";
        else if (HateLove <= -60)    ss << "is better off dead ";
        else if (HateLove <= -40)    ss << "is mean ";
        else if (HateLove <= -20)    ss << "isn't nice ";
        else if (HateLove <= 0)        ss << "is annoying ";
        else if (HateLove <= 20)    ss << "is ok ";
        else if (HateLove <= 40)    ss << "is easy going ";
        else if (HateLove <= 60)    ss << "is a good dad ";
        else if (HateLove <= 80)    ss << "is a great dad ";
        else                         ss << "is an awesome daddy ";
    }
    else if (girl.has_active_trait("Lesbian"))//lesbian shouldn't fall in love with you
    {
        /* */if (HateLove <= -80)    ss << "should die ";
        else if (HateLove <= -60)    ss << "is better off dead ";
        else if (HateLove <= -40)    ss << "is mean ";
        else if (HateLove <= -20)    ss << "isn't nice ";
        else if (HateLove <= 0)        ss << "is annoying ";
        else if (HateLove <= 20)    ss << "is ok ";
        else if (HateLove <= 40)    ss << "is easy going ";
        else if (HateLove <= 60)    ss << "is good ";
        else if (HateLove <= 80)    ss << "is a nice guy ";
        else                         ss << "is an awesome guy ";
    }
    else
    {
        /* */if (HateLove <= -80)    ss << "should die ";
        else if (HateLove <= -60)    ss << "is better off dead ";
        else if (HateLove <= -40)    ss << "is mean ";
        else if (HateLove <= -20)    ss << "isn't nice ";
        else if (HateLove <= 0)        ss << "is annoying ";
        else if (HateLove <= 20)    ss << "is ok ";
        else if (HateLove <= 40)    ss << "is easy going ";
        else if (HateLove <= 60)    ss << "is good ";
        else if (HateLove <= 80)    ss << "is attractive ";
        else                         ss << "is her true love ";
    }

    if (girl.pcfear() > 20)
    {
        if (HateLove > 0)    ss << "but she is also ";
        else                ss << "and she is ";
        /* */if (girl.pcfear() < 40)    ss << "afraid of him." << (girl.is_dead() ? " (for good reasons)." : ".");
        else if (girl.pcfear() < 60)    ss << "fearful of him." << (girl.is_dead() ? " (for good reasons)." : ".");
        else if (girl.pcfear() < 80)    ss << "afraid he will hurt her" << (girl.is_dead() ? " (and she was right)." : ".");
        else                                        ss << "afraid he will kill her" << (girl.is_dead() ? " (and she was right)." : ".");

    }
    else    ss << "and he isn't scary.";

    int happy = girl.happiness();
    ss << "\nShe is ";
    if (girl.health() < 1)    ss << "dead.";
    else if (happy > 90)    ss << "happy.";
    else if (happy > 80)    ss << "joyful.";
    else if (happy > 60)    ss << "reasonably happy.";
    else if (happy > 40)    ss << "unhappy.";
    else        ss << "showing signs of depression.";


    int morality = girl.morality(); //zzzzz FIXME needs better text
    ss << "\nShe " << (girl.health() < 1 ? "was " : "is ");
    /* */if (morality <= -80)    ss << "pure evil";
    else if (morality <= -60)    ss << "evil";
    else if (morality <= -40)    ss << "mean";
    else if (morality <= -20)    ss << "not nice";
    else if (morality <= 0)        ss << "neutral";
    else if (morality <= 20)    ss << "lawful";
    else if (morality <= 40)    ss << "nice";
    else if (morality <= 60)    ss << "good";
    else if (morality <= 80)    ss << "very good";
    else                         ss << "holy";
    ss << ".";

    int sanity = girl.sanity();    // `bsin` added - `J` adjusted
    ss << "\nShe " << (girl.health() < 1 ? "was " : "is ");

    /* */if (sanity <= -75)
    {
        /* */if (morality < -66)        ss << "psychopathic";
        else if (morality < -33)        ss << "a total bunny-boiler";
        else if (morality <     33)        ss << "dangerous";
        else if (morality <     66)        ss << "a raving fundamentalist";
        else                            ss << "a religious extremist";
    }
    else if (sanity <= -45)
    {
        /* */if (morality < -33)        ss << "a bit of a psycho";
        else if (morality <     33)        ss << "scarily out-there";
        else                            ss << "scarily over-zealous";
    }
    else if (sanity <= -15)
    {
        if (morality < 0)        ss << "disturbed";
        else                    ss << "nuts";
    }
    else if (sanity <= 15)    ss << "mentally stable";
    else if (sanity <= 45)    ss << "well-adjusted";
    else if (sanity <= 75)    ss << "completely rational";
    else ss << "rational to the extreme";

    ss << '.';

    return ss.str();
}

string cGirls::GetDetailsString(sGirl& girl, bool purchase)
{
    stringstream ss;

    // `J` When modifying Stats or Skills, search for "J-Change-Stats-Skills"  :  found in >> cGirls.cpp > GetDetailsString
    string basestr[] = { "Age : \t", "Rebelliousness : \t", "Looks : \t", "Constitution : \t", "Health : \t", "Happiness : \t", "Tiredness : \t", "Worth : \t" };

    SKILLS skillnum[] = { SKILL_MAGIC, SKILL_COMBAT, SKILL_SERVICE, SKILL_MEDICINE, SKILL_PERFORMANCE, SKILL_CRAFTING, SKILL_HERBALISM,
                          SKILL_FARMING, SKILL_BREWING, SKILL_ANIMALHANDLING, SKILL_COOKING,
                          SKILL_STRIP, SKILL_ORALSEX, SKILL_TITTYSEX, SKILL_HANDJOB, SKILL_FOOTJOB,
                          SKILL_ANAL, SKILL_NORMALSEX, SKILL_LESBIAN, SKILL_BDSM, SKILL_BEASTIALITY, SKILL_GROUP
                          };
    string skillstr[] = { "Magic Ability : \t", "Combat Ability : \t", "Service Skills : \t", "Medicine Skill : \t", "Performance Skill : \t",
                          "Crafting Skill : \t", "Herbalism Skill : \t", "Farming Skill : \t", "Brewing Skill : \t", "Animal Handling : \t", "Cooking : \t",
                          "Stripping : \t", "Oral Sex : \t", "Titty Sex : \t", "Hand Job : \t", "Foot Job : \t",
                          "Anal Sex : \t", "Normal Sex : \t", "Lesbian Sex : \t", "BDSM Sex : \t", "Bestiality Sex : \t", "Group Sex : \t"};

    string levelstr[] = { "Level : \t", "Exp : \t", "Exp to level : \t", "Needs : \t" };

    const int level = girl.level();
    const int exp = girl.exp();
    const int exptolv = GetRequiredXP(girl);
    const int expneed = exptolv - exp;

    // display level and exp
    ss << levelstr[0] << girl.stat_with_change_str(STAT_LEVEL);
    if (!purchase)
    {
        ss << '\n' << levelstr[1] << girl.stat_with_change_str(STAT_EXP);
        ss << '\n' << levelstr[2] << exptolv;
        ss << "\t | " << levelstr[3] << expneed << '\n';
    }

    // display Age, Looks, Rebel, Constitution
    ss << "\n\n" << basestr[0]; if (girl.age() == 100) ss << "Unknown"; else ss << girl.age();
    ss << '\n'   << basestr[2] << (girl.beauty() + girl.charisma()) / 2;
    ss << '\n'   << basestr[1] << girl.rebel();
    ss << '\n'   << basestr[3] << girl.stat_with_change_str(STAT_CONSTITUTION);

    // display HHT and money
    if (!purchase)
    {
        ss << '\n' << basestr[4] << girl.stat_with_change_str(STAT_HEALTH);
        ss << '\n' << basestr[5] << girl.stat_with_change_str(STAT_HAPPINESS);
        ss << '\n' << basestr[6] << girl.stat_with_change_str(STAT_TIREDNESS);
    }
    int cost = int(g_Game->tariff().slave_price(girl, purchase));
    ss << '\n' << basestr[7] << cost << " Gold\t";
    UpdateAskPrice(girl, false);
    cost = girl.askprice();
    ss << "\nAvg Pay per Customer : " << cost << " gold\n";

    // display status
    /* */if (girl.is_slave())    ss << "Is Branded a Slave\n";
    else ss << '\n';

    /* */if (is_virgin(girl))            ss << "She is a Virgin\n";
    else ss << '\n';

    if (!purchase)
    {
        int to_go = girl.get_preg_duration() - girl.m_WeeksPreg;
        /* */if (girl.has_status(STATUS_PREGNANT))                { ss << "Is pregnant, due: " << to_go << " weeks\n"; }
        else if (girl.has_status(STATUS_PREGNANT_BY_PLAYER))    { ss << "Is pregnant with your child, due: " << to_go << " weeks\n"; }
        else if (girl.has_status(STATUS_INSEMINATED))            { ss << "Is inseminated, due: " << to_go << " weeks\n"; }
        else if (girl.m_PregCooldown != 0)                            { ss << "Cannot get pregnant for: " << girl.m_PregCooldown << " weeks\n"; }
        else ss << '\n';
        // `J` moved the rest of children lines to second detail list
    }

    bool addict = is_addict(girl);
    bool diseased = has_disease(girl);
    /* */if (addict && !diseased) ss << "Has an addiciton\n";
    else if (!addict && diseased)    ss << "Has a disease\n";
    else if (addict && diseased)    ss << "Has an addiciton and a disease\n";
    else                                                ss << '\n';

    if (!purchase)
    {
        if (girl.has_status(STATUS_BADLY_POISONED))   ss << "Is badly poisoned\n";
        else if (girl.has_status(STATUS_POISONED))    ss << "Is poisoned\n";
        else                                                ss << '\n';
    }

    if (!purchase)
    {
        int cust = girl.m_NumCusts;
        ss << "\nShe has slept with " << cust << " Customers.";
    }

    // display Skills
    ss << "\n\nSKILLS";

    for (int i = 0; i < NUM_SKILLS; i++)
    {
        if (i == 11)
        {
            ss << "\n\nSEX SKILLS";
        }
        int cap = g_Game->get_skill_cap(skillnum[i], girl);
        ss << '\n' << skillstr[i] << girl.skill_with_change_str(skillnum[i]);
        if(cap < get_all_skills()[i].Max) {
           ss << " \t [" << g_Game->get_skill_cap(skillnum[i], girl) << "]";
        }
    }
    return ss.str();
}

string cGirls::GetMoreDetailsString(const sGirl& girl, bool purchase)
{
    stringstream ss;

    // `J` When modifying Stats or Skills, search for "J-Change-Stats-Skills"  :  found in >> cGirls.cpp > GetMoreDetailsString
    ss << "STATS";
    const int statnum[] = { STAT_CHARISMA, STAT_BEAUTY, STAT_LIBIDO, STAT_MANA, STAT_INTELLIGENCE, STAT_CONFIDENCE, STAT_OBEDIENCE,
                            STAT_SPIRIT, STAT_AGILITY, STAT_STRENGTH, STAT_FAME, STAT_LACTATION ,STAT_PCFEAR, STAT_PCLOVE };
    const int statnumsize = 15;
    const string statstr[] = { "Charisma : \t", "Beauty : \t", "Libido : \t", "Mana : \t", "Intelligence : \t", "Confidence : \t",
                         "Obedience : \t", "Spirit : \t", "Agility : \t", "Strength : \t", "Fame : \t", "Lactation : \t",
                         "PCFear : \t", "PCLove : \t", "Gold : \t" };

    const int show = statnumsize - 3;

    for (int i = 0; i < show; i++) ss << '\n' << statstr[i] << girl.stat_with_change_str((STATS) statnum[i]);

    if (!purchase)
    {
        ss << '\n' << statstr[14];
        if (g_Game->gang_manager().GetGangOnMission(MISS_SPYGIRLS))
        {
            ss << girl.m_Money;
        }
        else
        {
            ss << "Unknown";
        }
    }
    else ss << '\n';

    if (!purchase)
    {
        ss << "\n \nAccommodation: \t";
        ss << Accommodation(girl.m_AccLevel);
        ss << "\nCost per turn: \t" << ((girl.is_slave() ? 5 : 20) * (girl.m_AccLevel + 1)) << " gold.\n";

        //ss << "\nDetails:\n";
        //ss << AccommodationDetails(girl, girl.m_AccLevel);
        //ss << cGirls::AccommodationDetails(girl, girl.m_AccLevel);

        // added from Dagoth
        if (girl.is_resting() && !girl.was_resting() && girl.m_PrevDayJob != 255 && girl.m_PrevNightJob != 255)
        {
            ss << "\n \nOFF WORK, RESTING DUE TO TIREDNESS.";
            ss << "\nStored Day Job:   " << g_Game->job_manager().get_job_name(girl.m_PrevDayJob);
            ss << "\nStored Night Job: " << g_Game->job_manager().get_job_name(girl.m_PrevNightJob);
            ss << '\n';
        }
        int to_go = girl.get_preg_duration() - girl.m_WeeksPreg;
        // first line is current pregnancy
        /* */if (girl.has_status(STATUS_PREGNANT))                { ss << "Is pregnant, due: " << to_go << " weeks\n"; }
        else if (girl.has_status(STATUS_PREGNANT_BY_PLAYER))    { ss << "Is pregnant with your child, due: " << to_go << " weeks\n"; }
        else if (girl.has_status(STATUS_INSEMINATED))            { ss << "Is inseminated, due: " << to_go << " weeks\n"; }
        else if (girl.m_PregCooldown != 0)                            { ss << "Cannot get pregnant for: " << girl.m_PregCooldown << " weeks\n"; }
        else ss << '\n';
        // count the total births
        if (girl.m_ChildrenCount[CHILD00_TOTAL_BIRTHS] > 0)
            ss << "She has given birth to " << girl.m_ChildrenCount[CHILD00_TOTAL_BIRTHS]
                << " child" << (girl.m_ChildrenCount[CHILD00_TOTAL_BIRTHS] > 1 ? "ren" : "") << ":\n";
        // count the girls born
        if (girl.m_ChildrenCount[CHILD02_ALL_GIRLS] > 0)
        {
            ss << girl.m_ChildrenCount[CHILD02_ALL_GIRLS] << " girl" << (girl.m_ChildrenCount[CHILD02_ALL_GIRLS] > 1 ? "s" : "") << "\n   ";
            if (girl.m_ChildrenCount[CHILD02_ALL_GIRLS] == girl.m_ChildrenCount[CHILD06_YOUR_GIRLS])
            {
                if (girl.m_ChildrenCount[CHILD02_ALL_GIRLS] == 1)
                    ss << "She is your daughter.";
                else ss << "They are all yours.";
            }
            else if (girl.m_ChildrenCount[CHILD02_ALL_GIRLS] == girl.m_ChildrenCount[CHILD04_CUSTOMER_GIRLS])
            {
                if (girl.m_ChildrenCount[CHILD02_ALL_GIRLS] == 1)
                    ss << "She is not your daughter.";
                else ss << "They are all from other men.";
            }
            else
            {
                if (girl.m_ChildrenCount[CHILD06_YOUR_GIRLS] == 1)
                    ss << "One is yours and ";
                else ss << girl.m_ChildrenCount[CHILD06_YOUR_GIRLS] << " of them are yours and ";
                ss << "\n   ";
                if (girl.m_ChildrenCount[CHILD04_CUSTOMER_GIRLS] == 1)
                    ss << "One is from another man.";
                else ss << girl.m_ChildrenCount[CHILD04_CUSTOMER_GIRLS] << " of them are from other men.";
            }
            ss << '\n';
        }

        // count the boys born
        if (girl.m_ChildrenCount[CHILD03_ALL_BOYS] > 0)
        {
            ss << girl.m_ChildrenCount[CHILD03_ALL_BOYS] << " boy" << (girl.m_ChildrenCount[CHILD03_ALL_BOYS] > 1 ? "s" : "") << "\n   ";
            if (girl.m_ChildrenCount[CHILD03_ALL_BOYS] == girl.m_ChildrenCount[CHILD07_YOUR_BOYS])
            {
                if (girl.m_ChildrenCount[CHILD03_ALL_BOYS] == 1)
                    ss << "He is your son.";
                else ss << "They are all yours.";
            }
            else if (girl.m_ChildrenCount[CHILD03_ALL_BOYS] == girl.m_ChildrenCount[CHILD05_CUSTOMER_BOYS])
            {
                if (girl.m_ChildrenCount[CHILD03_ALL_BOYS] == 1)
                    ss << "He is not your son.";
                else ss << "They are all from other men.";
            }
            else
            {
                if (girl.m_ChildrenCount[CHILD07_YOUR_BOYS] == 1)
                    ss << "One is yours and ";
                else ss << girl.m_ChildrenCount[CHILD07_YOUR_BOYS] << " of them are yours and ";
                ss << "\n   ";
                if (girl.m_ChildrenCount[CHILD05_CUSTOMER_BOYS] == 1)
                    ss << "One is from another man.";
                else ss << girl.m_ChildrenCount[CHILD05_CUSTOMER_BOYS] << " of them are from other men.";
            }
            ss << '\n';
        }

        if (girl.m_ChildrenCount[CHILD01_ALL_BEASTS] > 0)        ss << "She has given birth to " << girl.m_ChildrenCount[CHILD01_ALL_BEASTS] << " Beast" << (girl.m_ChildrenCount[CHILD01_ALL_BEASTS] > 1 ? "s" : "") << ".\n";
        if (girl.m_ChildrenCount[CHILD08_MISCARRIAGES] > 0)    ss << "She has had " << girl.m_ChildrenCount[CHILD08_MISCARRIAGES] << " Miscarriage" << (girl.m_ChildrenCount[CHILD08_MISCARRIAGES] > 1 ? "s" : "") << ".\n";
        if (girl.m_ChildrenCount[CHILD09_ABORTIONS] > 0)        ss << "She has had " << girl.m_ChildrenCount[CHILD09_ABORTIONS] << " Abortion" << (girl.m_ChildrenCount[CHILD09_ABORTIONS] > 1 ? "s" : "") << ".\n";
    }

    ss << "\n \nFETISH CATEGORIES\n";
    if (CheckGirlType(girl, FETISH_BIGBOOBS))        ss << " |Big Boobs| ";
    if (CheckGirlType(girl, FETISH_CUTEGIRLS))        ss << " |Cute Girl| ";
    if (CheckGirlType(girl, FETISH_DANGEROUSGIRLS))    ss << " |Dangerous| ";
    if (CheckGirlType(girl, FETISH_COOLGIRLS))        ss << " |Cool| ";
    if (CheckGirlType(girl, FETISH_NERDYGIRLS))        ss << " |Nerd| ";
    if (CheckGirlType(girl, FETISH_NONHUMAN))        ss << " |Non or part human| ";
    if (CheckGirlType(girl, FETISH_LOLITA))            ss << " |Lolita| ";
    if (CheckGirlType(girl, FETISH_ELEGANT))        ss << " |Elegant| ";
    if (CheckGirlType(girl, FETISH_SEXY))            ss << " |Sexy| ";
    if (CheckGirlType(girl, FETISH_FIGURE))            ss << " |Nice Figure| ";
    if (CheckGirlType(girl, FETISH_ARSE))            ss << " |Nice Arse| ";
    if (CheckGirlType(girl, FETISH_SMALLBOOBS))        ss << " |Small Boobs| ";
    if (CheckGirlType(girl, FETISH_FREAKYGIRLS))    ss << " |Freaky| ";
    if (CheckGirlType(girl, FETISH_FUTAGIRLS))        ss << " |Futa| ";
    if (CheckGirlType(girl, FETISH_TALLGIRLS))        ss << " |Tall| ";
    if (CheckGirlType(girl, FETISH_SHORTGIRLS))        ss << " |Short| ";
    if (CheckGirlType(girl, FETISH_FATGIRLS))        ss << " |Fat| ";
//    if (CheckGirlType(girl, FETISH_OBEDIENCE))        ss << " |Obedient| ";
//    if (CheckGirlType(girl, FETISH_REBELLIOUS))        ss << " |Rebellious| ";
//    if (CheckGirlType(girl, FETISH_DEGRADATION))    ss << " |Degraded| ";
//    if (CheckGirlType(girl, FETISH_MUTILATION))        ss << " |Mutilated| ";
//    if (CheckGirlType(girl, FETISH_BONDAGE))        ss << " |Restrained| ";


    if (!purchase)
    {
        ss << "\n \nJOB PREFERENCES";
        ss << '\n';
        string base = "She";
        string text;
        int enjcount = 0;
        for (int i = 0; i < NUM_ACTIONTYPES; ++i)
        {
            int e = girl.get_enjoyment((Action_Types)i);
            /* */if (e < -70)    { text = " hates "; }
            else if (e < -50)    { text = " really dislikes "; }
            else if (e < -30)    { text = " dislikes "; }
            else if (e < -20)    { text = " doesn't particularly enjoy "; }
            // if she's indifferent, why specify it? Let's instead skip it.
            else if (e < 15)    { continue; }
            else if (e < 30)    { text = " is happy enough with "; }
            else if (e < 50)    { text = " likes "; }
            else if (e < 70)    { text = " really enjoys "; }
            else                { text = " loves "; }
            ss << base << text << get_action_descr((Action_Types)i) << ".";
            ss << '\n';
            enjcount++;
        }
        if (enjcount > 0) ss << "\nShe is indifferent to all other tasks.\n \n";
        else              ss << "At the moment, she is indifferent to all tasks.\n \n";

        int tricount = 0;
        for (int i = 0; i < NUM_TRAININGTYPES; ++i)
        {
            if (strcmp(sGirl::training_jobs[i], "") == 0)            continue;
            int e = girl.get_training(i);
            /* */if (e < 0)    { text = " hasn't started "; }
            // if she's indifferent, why specify it? Let's instead skip it.
            else if (e < 15)    { continue; }
            else if (e < 30)    { text = " has started "; }
            else if (e < 50)    { text = " knows the basics of "; }
            else if (e < 70)    { text = " has knowledge of "; }
            else                { text = " performs well in"; }
            ss << base << text << sGirl::training_jobs[i] << ".";
            ss << '\n';
            tricount++;
        }
        if (tricount > 0)                        { ss << "\nShe hasn't started any other training.\n \n"; }
        else                                     { ss << "At the moment, she hasn't started any special training.\n \n"; }
    }

    ss << "\n \n\nBased on:  " << girl.m_Name;

    return ss.str();
}

const char* JobRatingLetter(double value)
{
    /* */if (value < -500)        return "X    ";    // Can not do this job
    else if (value == 0)        return "0    ";    // Bad input
    else if (value >= 350)        return "   I  ";    // Incomparable
    else if (value >= 245)        return "  S  ";    // Superior
    else if (value >= 185)        return " A   ";    // Amazing
    else if (value >= 145)        return "B    "; // Better
    else if (value >= 100)        return "C    ";    // Can do it
    else if (value >= 70)        return "D    ";    // Don't bother
    else                        return "E    "; // Expect Failure
}

namespace {
    struct sJD {
        JOBS job;
        char mark = '-';
    };

    void JobRating(std::stringstream& jr, const sGirl& girl, std::initializer_list<sJD> jobs) {
        for(auto& job : jobs) {
            double value = girl.job_performance(job.job, true);

            jr << JobRatingLetter(value) << "  " << job.mark << "  " << g_Game->job_manager().get_job_name(job.job);
            if (g_Game->settings().get_bool(settings::USER_SHOW_NUMBERS)) jr << "   ( " << (int) value << " )";
            jr << '\n';
        }
        jr << '\n';
    }
}

string cGirls::GetThirdDetailsString(const sGirl& girl)    // `J` bookmark - Job ratings
{
    // `J` bookmark - Job Ratings list

    // `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >> cGirls.cpp > GetThirdDetailsString

    // `J` spiltting the buildings so they can be sorted
    string div = "\n------------------------------------\n\n";
    std::stringstream Brothel_Data;
    Brothel_Data << "Brothel Job Ratings\n";
    JobRating(Brothel_Data, girl, {{JOB_MATRON, '-'}, {JOB_SECURITY, '-'}, {JOB_EXPLORECATACOMBS, '-'},
                              {JOB_ADVERTISING, '-'}, {JOB_CUSTOMERSERVICE, '-'}, {JOB_BEASTCARER, '-'},
                              {JOB_CLEANING, '?'}});
    JobRating(Brothel_Data, girl, {{JOB_BARMAID, '-'}, {JOB_WAITRESS, '-'}, {JOB_SINGER, '-'}, {JOB_PIANO, '-'}, {JOB_ESCORT, '?'}});
    JobRating(Brothel_Data, girl, {{JOB_DEALER, '-'}, {JOB_ENTERTAINMENT, '-'}, {JOB_XXXENTERTAINMENT, '-'}, {JOB_WHOREGAMBHALL, '?'}});
    JobRating(Brothel_Data, girl, {{JOB_SLEAZYBARMAID, '-'}, {JOB_SLEAZYWAITRESS, '-'}, {JOB_BARSTRIPPER, '-'}, {JOB_BARWHORE, '?'}});
    JobRating(Brothel_Data, girl, {{JOB_MASSEUSE, '-'}, {JOB_BROTHELSTRIPPER, '-'}, {JOB_PEEP, '-'}, {JOB_WHOREBROTHEL, '?'}, {JOB_WHORESTREETS, '?'}});
    Brothel_Data << div;

    //STUDIO
    stringstream Studio_Data;
    if (g_Game->has_building(BuildingType::STUDIO))
    {
        Studio_Data << "Studio Job Ratings\n";
        JobRating(Studio_Data, girl, {{JOB_DIRECTOR, '?'}, {JOB_CAMERAMAGE, '?'}, {JOB_CRYSTALPURIFIER, '?'}, {JOB_FLUFFER, '?'}, {JOB_STAGEHAND, '?'}});
        JobRating(Studio_Data, girl, {{JOB_FILMACTION, ' '}, {JOB_FILMMUSIC, ' '}, {JOB_FILMCHEF, ' '}, {JOB_FILMTEASE, ' '}});
        JobRating(Studio_Data, girl, {{JOB_FILMORAL, ' '}});
        JobRating(Studio_Data, girl, {{JOB_FILMFACEFUCK, ' '}, {JOB_FILMBUKKAKE, ' '}, {JOB_FILMBONDAGE, ' '}, {JOB_FILMPUBLICBDSM, ' '}, {JOB_FILMBEAST, ' '}});
        // TODO the other jobs don't have a useful rating yet
        Studio_Data << div;
    }
    //ARENA
    stringstream Arena_Data;
    if (g_Game->has_building(BuildingType::ARENA))
    {
        Arena_Data << "Arena Job Ratings\n";
        JobRating(Arena_Data, girl, {{JOB_DOCTORE, '-'}, {JOB_CITYGUARD, '?'}, {JOB_BLACKSMITH, '-'}, {JOB_COBBLER, '-'}, {JOB_JEWELER, '-'}});
        JobRating(Arena_Data, girl, {{JOB_FIGHTBEASTS, '-'}, {JOB_FIGHTARENAGIRLS, '?'}, {JOB_FIGHTTRAIN, '!'}});
        Arena_Data << div;
    }
    //CENTRE
    stringstream Centre_Data;
    if (g_Game->has_building(BuildingType::CENTRE))
    {
        Centre_Data << "Centre Job Ratings\n";
        JobRating(Centre_Data, girl, {{JOB_CENTREMANAGER, '-'}, {JOB_FEEDPOOR, '-'}, {JOB_COMUNITYSERVICE, '-'}});
        JobRating(Centre_Data, girl, {{JOB_COUNSELOR, '-'}, {JOB_REHAB, '!'}, {JOB_THERAPY, '!'}, {JOB_EXTHERAPY, '!'}, {JOB_ANGER, '!'}});
        Centre_Data << div;
    }
    //CLINIC
    stringstream Clinic_Data;
    if (g_Game->has_building(BuildingType::CLINIC))
    {
        Clinic_Data << "Clinic Job Ratings\n";
        JobRating(Clinic_Data, girl, {{JOB_CHAIRMAN, '-'}, {JOB_DOCTOR, '-'}, {JOB_NURSE, '!'}, {JOB_INTERN, '!'}});
        JobRating(Clinic_Data, girl, {{JOB_GETHEALING, '!'}, {JOB_CUREDISEASES, '!'}, {JOB_GETABORT, '!'},
                                       {JOB_COSMETICSURGERY, '!'}, {JOB_LIPO, '!'}, {JOB_BREASTREDUCTION, '!'}, {JOB_BOOBJOB, '!'},
                                       {JOB_VAGINAREJUV, '!'}, {JOB_FACELIFT, '!'}, {JOB_ASSJOB, '!'}, {JOB_TUBESTIED, '!'}, {JOB_FERTILITY, '!'}});
        Clinic_Data << div;
    }
    //FARM
    stringstream Farm_Data;
    if (g_Game->has_building(BuildingType::FARM))
    {
        Farm_Data << "Farm Job Ratings\n";
        JobRating(Farm_Data, girl, {{JOB_FARMMANGER, '-'}, {JOB_VETERINARIAN, '-'}, {JOB_MARKETER, '-'}, {JOB_RESEARCH, '!'}, {JOB_FARMHAND, '-'}});
        JobRating(Farm_Data, girl, {{JOB_FARMER, '-'}, {JOB_GARDENER, '-'}, {JOB_SHEPHERD, '-'}, {JOB_RANCHER, '-'}, {JOB_CATACOMBRANCHER, '-'},
                                     {JOB_BEASTCAPTURE, '-'}, {JOB_MILKER, '-'}, {JOB_MILK, '?'}});
        JobRating(Farm_Data, girl, {{JOB_BUTCHER, '-'}, {JOB_BAKER, '-'}, {JOB_BREWER, '-'}, {JOB_TAILOR, '-'}, {JOB_MAKEITEM, '-'}, {JOB_MAKEPOTIONS, '-'}});
        Farm_Data << div;
    }
    //HOUSE
    stringstream House_Data;
    House_Data << "House Job Ratings\n";
    JobRating(House_Data, girl, {{JOB_HEADGIRL, '-'}, {JOB_RECRUITER, '-'}, {JOB_PERSONALTRAINING, '!'}, {JOB_TRAINING, '!'},
                                 {JOB_FAKEORGASM, '!'}, {JOB_SO_STRAIGHT, '!'}, {JOB_SO_BISEXUAL, '!'}, {JOB_SO_LESBIAN, '!'}});

    // House_Data += JobRating(*girl, m_JobManager.JP_PersonalBedWarmer(girl, true), "* PersonalBedWarmer");
    House_Data << div;

    // `J` Show the current building first
    string data = Brothel_Data.str();
    BuildingType btype = BuildingType::BROTHEL;
    if(girl.m_Building) {
        btype = girl.m_Building->type();
        switch(girl.m_Building->type()) {
        case BuildingType::ARENA:
            data = Arena_Data.str();
            break;
        case BuildingType::STUDIO:
            data = Studio_Data.str();
            break;
        case BuildingType::CENTRE:
            data = Centre_Data.str();
            break;
        case BuildingType::CLINIC:
            data = Clinic_Data.str();
            break;
        case BuildingType::FARM:
            data = Farm_Data.str();
            break;
        case BuildingType::HOUSE:
            data = House_Data.str();
            break;
        case BuildingType::BROTHEL:
            data = Brothel_Data.str();
            break;
        }
    }


    // `J` show all the other buildings
    data += div;
    if (btype != BuildingType::BROTHEL) data += Brothel_Data.str();
    if (btype != BuildingType::STUDIO)    data += Studio_Data.str();
    if (btype != BuildingType::ARENA)    data += Arena_Data.str();
    if (btype != BuildingType::CENTRE)    data += Centre_Data.str();
    if (btype != BuildingType::CLINIC)    data += Clinic_Data.str();
    if (btype != BuildingType::FARM)    data += Farm_Data.str();
    if (btype != BuildingType::HOUSE)    data += House_Data.str();

    // `J` finish with the explanation
    data += div;
    data += "Job Ratings range from\n'I' The absolute best, 'S' Superior,\n";
    data += "Then 'A'-'E' with 'E' being the worst.\n'X' means they can not do the job.\n \n";
    data += "Jobs marked with ? do not really use job performance directly and is an estimate.\n";
    data += "Jobs marked with ! are how much the girl is in need of the service of that job.\n";
    return data;

}

string cGirls::GetSimpleDetails(const sGirl& girl)
{
    stringstream ss;
    int w, h, size = 0;

    // `J` When modifying Stats or Skills, search for "J-Change-Stats-Skills"  :  found in >> cGirls.cpp > GetDetailsString
    string basestr[] = { "Age : \t", "Rebelliousness : \t", "Looks : \t", "Constitution : \t", "Health : \t", "Happiness : \t", "Tiredness : \t", "Level : \t", "Exp : \t", "Location : \t", "Day Job : \t", "Night Job : \t" };
    int basecount = 12;
    int skillnum[] = { SKILL_MAGIC, SKILL_COMBAT, SKILL_SERVICE, SKILL_MEDICINE, SKILL_PERFORMANCE, SKILL_CRAFTING, SKILL_HERBALISM, SKILL_FARMING, SKILL_BREWING, SKILL_ANIMALHANDLING, SKILL_COOKING, SKILL_ANAL, SKILL_BDSM, SKILL_NORMALSEX, SKILL_BEASTIALITY, SKILL_GROUP, SKILL_LESBIAN, SKILL_ORALSEX, SKILL_TITTYSEX, SKILL_HANDJOB, SKILL_STRIP, SKILL_FOOTJOB };
    string skillstr[] = { "Magic : \t", "Combat : \t", "Service : \t", "Medicine : \t", "Performance : \t", "Crafting : \t", "Herbalism : \t", "Farming : \t", "Brewing : \t", "Animal Handling : \t", "Cooking : \t", "Anal : \t", "BDSM : \t", "Normal : \t", "Bestiality : \t", "Group : \t", "Lesbian : \t", "Oral : \t", "Titty : \t", "Hand Job : \t", "Stripping : \t", "Foot Job : \t" };
    int skillcount = 22;
    STATS statnum[] = { STAT_CHARISMA, STAT_BEAUTY, STAT_LIBIDO, STAT_MANA, STAT_INTELLIGENCE, STAT_CONFIDENCE, STAT_OBEDIENCE, STAT_SPIRIT, STAT_AGILITY, STAT_STRENGTH, STAT_FAME, STAT_LACTATION };
    string statstr[] = { "Charisma : \t", "Beauty : \t", "Libido : \t", "Mana : \t", "Intelligence : \t", "Confidence : \t", "Obedience : \t", "Spirit : \t", "Agility : \t", "Strength : \t", "Fame : \t", "Lactation : \t" };
    int statcount = 12;

    ss << basestr[9];
    if(girl.m_Building)
        ss << girl.m_Building->name();

    ss << '\n' << basestr[10] << g_Game->job_manager().get_job_name(girl.m_DayJob);
    ss << '\n' << basestr[11] << g_Game->job_manager().get_job_name(girl.m_NightJob);
    ss << '\n' << basestr[2] << (girl.beauty() + girl.charisma()) / 2;
    ss << '\n' << statstr[0] << girl.charisma();
    ss << '\n' << statstr[1] << girl.beauty();
    ss << '\n' << basestr[7] << girl.level();
    ss << '\n' << basestr[8] << girl.exp();
    ss << '\n' << basestr[0]; if (girl.age() == 100) ss << "Unknown"; else ss << girl.age();
    ss << '\n' << basestr[1] << const_cast<sGirl&>(girl).rebel();
    ss << '\n' << basestr[3] << girl.constitution();
    ss << '\n' << basestr[4] << girl.health();
    ss << '\n' << basestr[5] << girl.happiness();
    ss << '\n' << basestr[6] << girl.tiredness();
    for (int i = 2; i < statcount; i++)    { ss << '\n' << statstr[i] << girl.get_stat(statnum[i]); }
    ss << '\n';    if (girl.is_slave())                { ss << "Is Branded a Slave"; }
    ss << '\n';    if (is_virgin(girl))    { ss << "She is a Virgin"; }
    int to_go = girl.get_preg_duration() - girl.m_WeeksPreg;
    ss << '\n';    if (girl.has_status(STATUS_PREGNANT))        { ss << "Is pregnant " << "(" << to_go << ")"; }
    else if (girl.has_status(STATUS_PREGNANT_BY_PLAYER))    { ss << "Is pregnant with your child " << "(" << to_go << ")"; }
    else if (girl.has_status(STATUS_INSEMINATED))            { ss << "Is inseminated " << "(" << to_go << ")"; }
    ss << '\n';    if (is_addict(girl) && !has_disease(girl))    { ss << "Has an addiciton"; }
    else if (!is_addict(girl) && has_disease(girl))            { ss << "Has a disease"; }
    else if (is_addict(girl) && has_disease(girl))            { ss << "Has an addiciton and a disease"; }
    for (int i = 0; i < skillcount; i++)    { ss << '\n' << skillstr[i] << girl.get_skill(skillnum[i]); }
    ss << "\n \n";    int trait_count = 0;
    auto all_traits = const_cast<sGirl&>(girl).raw_traits().get_trait_info();
    for (auto& t : all_traits)
    {
        if(!t.active) continue;
        trait_count++;
        if (trait_count > 1) ss << ",   ";
        ss << t.trait->display_name();
        if (t.remaining_time > 0) ss << " (" << t.remaining_time << ")";
    }
    return ss.str();
}

// added human check: -1 does not matter, 0 not human, 1 human
std::shared_ptr<sGirl> cGirls::GetUniqueYourDaughterGirl(int Human0Monster1)
{
    if (GetNumYourDaughterGirls() == 0) return nullptr;
    auto ptr = m_Girls->get_random_girl([&](const sGirl& girl) {
        return is_your_daughter(girl) && (Human0Monster1 == -1 ||
                (Human0Monster1 == 1 && !girl.is_human()) ||
                (Human0Monster1 == 0 && girl.is_human()));
    });
    return m_Girls->TakeGirl(ptr);
}

std::shared_ptr<sGirl> cGirls::GetUniqueGirl(bool slave, bool catacomb, bool arena, bool daughter, bool isdaughter)
{
    int num_girls = m_Girls->num();
    int num_monster = m_Girls->count([](const sGirl& girl) { return girl.is_monster(); });
    int num_arena = m_Girls->count([](const sGirl& girl) { return girl.is_arena(); });
    int num_daughter = m_Girls->count([](const sGirl& girl) { return girl.is_isdaughter(); });
    int num_slave = m_Girls->count([](const sGirl& girl) { return girl.is_slave(); });

    auto choice = m_Girls->get_random_girl([&](const sGirl& girl) {
        if(!girl.IsUnique()) {
            return false;
        }
        return  girl.is_slave() == slave
                &&    girl.is_monster() == catacomb
                &&    girl.is_arena() == arena
                &&    is_your_daughter(girl) == daughter
                &&    girl.is_isdaughter() == isdaughter;});
    return m_Girls->TakeGirl(choice);
}

sGirl* cGirls::GetGirl(int girl)
{
    return m_Girls->get_girl(girl);
}

int cGirls::GetRebelValue(const sGirl& girl, JOBS job)
{
    /*
    *    WD:    Added test to ignore STAT_HOUSE value
    *    if doing a job that the player is paying
    *    only when processing Day or Night Shift
    *
    *    This is to make it so that the jobs that
    *    cost the player support where the house take
    *    has no effect has no impact on the chance of
    *    refusal.
    */

    if (girl.has_active_trait("Broken Will"))    return -100;
    int chanceNo = 0;
    int houseStat = girl.house();
    int happyStat = girl.happiness();
    bool girlIsSlave = girl.is_slave();

    chanceNo -= girl.pclove() / 5;
    chanceNo += girl.spirit() / 2;
    chanceNo -= girl.obedience() / 5;

    // having a guarding gang will enforce order
    sGang* gang = g_Game->gang_manager().GetGangOnMission(MISS_GUARDING);
    if (gang)    chanceNo -= 10;

    chanceNo += std::max(0, girl.tiredness() / 10 - 3);    // Tired girls increase Rebel

    if (happyStat < 50)                                // Unhappy girls increase Rebel
    {
        chanceNo += (50 - happyStat) / 5;
        if (happyStat < 10)                            // WD:    Fixed missing case Happiness < 10
            chanceNo += 10 - happyStat;                // WD:    Rebel increases by an additional point if happy < 10
    }
    else    chanceNo -= (happyStat - 50) / 10;        // happy girls are less cranky, less Rebel

    // House Take has no effect on slaves
    if (girlIsSlave)    chanceNo -= 15;                // Slave Girl lowers rebelinous of course
    else
    {
        chanceNo += 15;                                // Free girls are a little more rebelious
        // WD    House take of gold has no affect on rebellion if
        //        job is paid by player. eg Matron / cleaner
        if (job == NUM_JOBS || g_Game->job_manager().is_job_Paid_Player(job))
            houseStat = 0;

        if (houseStat < 60)                            // Take less money than normal, lower Rebel
            chanceNo -= (60 - houseStat) / 2;
        else
        {
            chanceNo += (houseStat - 60) / 2;        // Take more money than normal, more Rebel
            if (houseStat >= 100) chanceNo += 10;    // Take all the money, more Rebel
        }
    }

    /*
    *    `J` "Kidnapped" and "Emprisoned Customer" are factored in twice, before and after mental trait modifiers
    *    This will allow them to have at least some effect on "Mind Fucked", "Dependant" or "Meek" girls
    */

    // these are factoring in twice before and after mental trait modifiers
    if (girl.has_active_trait("Kidnapped") || girl.has_active_trait("Emprisoned Customer")) chanceNo += 10;
    /// TODO (traits) consider remaining time
    //int kep = girl.has_temp_trait("Kidnapped") + girl.has_temp_trait("Emprisoned Customer");
    //if (kep > 20) kep += 20; else if (kep > 10) kep += 10;

    // guarantee certain rebelliousness values for specific traits
    if (girl.has_active_trait("Retarded")) chanceNo -= 30;
    if (girl.has_active_trait("Mind Fucked") && chanceNo > -50) chanceNo = -50;
    if (girl.has_active_trait("Dependant") && chanceNo > -40) chanceNo = -40;
    if (girl.has_active_trait("Meek") && chanceNo > 20) chanceNo = 20;

    // chanceNo += kep;

    // `J` What type of accommodations she is held in will affect her a lot.
    int accommod = girl.m_AccLevel - cGirls::PreferredAccom(girl);
    if (accommod < -4)
    {
        chanceNo -= accommod * 2;
    }
    else if (accommod < -1)
    {
        chanceNo -= accommod;
    }
    else if (accommod > 1)
    {
        chanceNo -= accommod * 2;
    }


    // Normalise
    if (chanceNo < -100)        chanceNo = -100;
    else if (chanceNo > 100)    chanceNo = 100;
    return chanceNo;
}

int cGirls::GetNumYourDaughterGirls()
{
    return m_Girls->count(is_your_daughter);
}

// ----- Stat

void cGirls::updateTemp(sGirl& girl)    // `J` group all the temp updates into one area
{
    girl.DecayTemp();        // update temp stats
    updateTempEnjoyment(girl);        // update temp enjoyment
}

// ----- Skill

double cGirls::GetAverageOfAllSkills(const sGirl& girl)
{
    return ((girl.anal() + girl.animalhandling() + girl.bdsm() + girl.beastiality() + girl.brewing()
        + girl.combat() + girl.cooking()+ girl.crafting() + girl.farming() + girl.footjob() + girl.group() + girl.handjob()
        + girl.herbalism() + girl.lesbian() + girl.magic() + girl.medicine() + girl.normalsex() + girl.oralsex()
        + girl.performance() + girl.service() + girl.strip() + girl.tittysex()) / 22.0);
}
double cGirls::GetAverageOfNSxSkills(const sGirl& girl)
{
    return ((girl.animalhandling() + girl.brewing() + girl.combat() + girl.cooking() + girl.crafting() + girl.farming()
        + girl.herbalism() + girl.magic() + girl.medicine() + girl.performance() + girl.service()
        ) / 11.0);
}
double cGirls::GetAverageOfSexSkills(const sGirl& girl)
{
    return ((girl.anal() + girl.bdsm() + girl.beastiality() + girl.footjob() + girl.group() + girl.handjob()
        + girl.lesbian() + girl.normalsex() + girl.oralsex() + girl.strip() + girl.tittysex()) / 11.0);
}


// total of all skills
int cGirls::GetSkillWorth(const sGirl& girl)
{
    int num = 0;
    for (int i = 0; i < NUM_SKILLS; ++i) num += girl.get_skill(i);
    return num;
}

// ----- Load save

// this function throws if the XML file itself cannot be opened or parsed, but
// all errors that happen during parsing of any <Girl> elements are only logged
// and the corresponding Girl ignored, but the other Girls will be loaded.
void cGirls::LoadRandomGirl(const string& filename, const std::string& base_path,
                            const std::function<void(const std::string&)>& error_handler)
{
    m_RandomGirls.load_from_file(filename, base_path, error_handler);
}

// this function throws if the XML file itself cannot be opened or parsed, but
// all errors that happen during parsing of any <Girl> elements are only logged
// and the corresponding Girl ignored, but the other Girls will be loaded.
void cGirls::LoadGirlsXML(const std::string& file_path, const std::string& base_path,
                          const std::function<void(const std::string&)>& error_handler)
{
    auto doc = LoadXMLDocument(file_path);
    auto root = doc->RootElement();
    if(!root) {
        g_LogFile.error("girls", "No XML root found in girl file ", file_path);
        throw std::runtime_error("ERROR: No XML root element");
    }

    // loop over the elements attached to the root
    for (auto& el : IterateChildElements(*root, "Girl"))
    {
        try {
            auto girl = sGirl::LoadFromTemplate(el);
            girl->SetImageFolder(DirPath(base_path.c_str()) << girl->m_Name);

            if (girl->age() < 18) girl->set_stat(STAT_AGE, 18);    // `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live

            // TODO load triggers if the girl has any
            CalculateGirlType(*girl);            // Fetish list for customer happiness
            AddGirl(girl);                        // add the girl to the list
        } catch (const std::exception& ex) {
            g_LogFile.error("girls", "Could not load girl from file '", file_path, "': ", ex.what());
            if(error_handler)
                error_handler("ERROR: Could not load girl from file " + file_path + ": " + ex.what());
        }
    }
}

bool cGirls::LoadGirlsXML(const tinyxml2::XMLElement* pGirls)
{
    if (pGirls == nullptr) return false;
    std::shared_ptr<sGirl> current = nullptr;                    // load the number of girls
    for (auto* pGirl = pGirls->FirstChildElement("Girl"); pGirl != nullptr; pGirl = pGirl->NextSiblingElement("Girl"))
    {
        current = std::make_shared<sGirl>(false);            // load each girl and add her
        bool success = current->LoadGirlXML(pGirl);
        if (success) AddGirl(std::move(current));
    }
    return true;
}

tinyxml2::XMLElement& cGirls::SaveGirlsXML(tinyxml2::XMLElement& elRoot)
{
    auto& elGirls = PushNewElement(elRoot, "Girls");
    m_Girls->SaveXML(elGirls);
    elGirls.SetAttribute("NumberofGirls", m_Girls->num());
    return elGirls;
}

// ----- Equipment & inventory
void cGirls::EquipCombat(sGirl& girl)
{
    // girl makes sure best armor and weapons are equipped, ready for combat
    if (!g_Game->settings().get_bool(settings::USER_ITEMS_AUTO_EQUIP_COMBAT)) return;    // is this feature disabled in config?
    int refusal = 0;
    if (girl.has_active_trait("Retarded")) refusal += 30;    // if she's retarded, she might refuse or forget
    if (g_Dice.percent(refusal)) return;
    
    const sInventoryItem* Armor = nullptr, *Helm=nullptr, *Shield=nullptr, *Boot=nullptr, *Weap1=nullptr, *Weap2=nullptr;
    for(auto& entry : girl.inventory().all_items()) {
        auto item = entry.first;
        if (item->m_Type == sInventoryItem::Weapon)
        {
            girl.unequip(item);
            if(!Weap1) Weap1 = item;
            else if (!Weap2) Weap2 = item;
            else if (item->m_Cost > Weap1->m_Cost)
            {
                Weap2 = Weap1;
                Weap1 = item;
            }
            else if (item->m_Cost > Weap2->m_Cost)
                Weap2 = item;
        }
        if (item->m_Type == sInventoryItem::Armor)
        {
            girl.unequip(item);
            if (!Armor || item->m_Cost > Armor->m_Cost) Armor = item;
        }
        if (item->m_Type == sInventoryItem::Helmet)
        {
            girl.unequip(item);
            if (!Helm || item->m_Cost > Helm->m_Cost) Helm = item;
        }
        if (item->m_Type == sInventoryItem::Combatshoes)
        {
            girl.unequip(item);
            if (!Boot || item->m_Cost > Boot->m_Cost) Boot = item;

        }
        if (item->m_Type == sInventoryItem::Shield)
        {
            girl.unequip(item);
            if (!Shield || item->m_Cost > Shield->m_Cost) Shield = item;

        }
    }
    
    // unequip hats and shoes if boots and helms were found
    if (Helm || Boot)
    {
        for(auto& entry : girl.inventory().all_items()) {
            auto item = entry.first;
            if (Helm && item->m_Type == sInventoryItem::Hat)                    girl.unequip(item);
            if (Boot && item->m_Type == sInventoryItem::Shoes)                    girl.unequip(item);
        }
    }

    if (Armor)        girl.equip(Armor, false);
    if (Weap1)        girl.equip(Weap1, false);
    if (Weap2)        girl.equip(Weap2, false);
    if (Helm)        girl.equip(Helm, false);
    if (Boot)        girl.equip(Boot, false);
    if (Shield)        girl.equip(Shield, false);
}

void cGirls::UnequipCombat(sGirl& girl)
{  // girl unequips armor and weapons, ready for brothel work or other non-aggressive jobs
    if (!g_Game->settings().get_bool(settings::USER_ITEMS_AUTO_EQUIP_COMBAT)) return; // is this feature disabled in config?
    // if she's a really rough or crazy bitch, she might just keep combat gear equipped
    int refusal = girl.get_trait_modifier("refuse-unequip-combat");
    if (g_Dice.percent(refusal))            return;

    for(auto& entry : girl.inventory().all_items()) {
        auto item = entry.first;
        if (is_in(item->m_Type, {sInventoryItem::Weapon, sInventoryItem::Armor, sInventoryItem::Helmet, sInventoryItem::Combatshoes, sInventoryItem::Shield}))
            girl.unequip(item);
    }
    // reequip shoes and hats
    for(auto& entry : girl.inventory().all_items()) {
        auto item = entry.first;
        if (item->m_Type == sInventoryItem::Shoes || item->m_Type == sInventoryItem::Hat)
            girl.equip(item, false);
    }


}

struct sStatChange {
    STATS stat;
    int change;
};

bool HandleDrug(sGirl &girl, const char *drug_trait, const char* drug, std::initializer_list<const char*> items, int withdrawal_duration,
                bool& withdraw, std::initializer_list<sStatChange> withdrawals) {
    if (girl.has_active_trait(drug_trait))
    {
        const sInventoryItem* temp = nullptr;
        for(auto item : items) {
            temp = girl.has_item(item);
            if(temp) break;
        }

        if (!temp)    // withdrawals for a week
        {
            if (girl.m_Withdrawals >= withdrawal_duration)
            {
                girl.lose_trait(drug_trait, true);
                girl.gain_trait("Former Addict");
                stringstream goodnews;
                goodnews << "Good News, ${name} has overcome her addiction to " << drug << ".";
                girl.AddMessage(goodnews.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);
            }
            else
            {
                for(auto& w : withdrawals) {
                    girl.upd_base_stat(w.stat, w.change);
                }
                if (!withdraw)
                {
                    girl.m_Withdrawals++;
                    withdraw = true;
                }
            }
        }
        else
        {
            girl.equip(temp, false);
            girl.m_Withdrawals = 0;
            return true;
        }
    }
    return false;
}

void cGirls::UseItems(sGirl& girl)
{
    bool withdraw = false;
    // uses drugs first
    HandleDrug(girl, "Viras Blood Addict", "Vira Blood", {"Vira Blood"}, 30, withdraw,
            {{STAT_HAPPINESS, -30}, {STAT_OBEDIENCE, -30}, {STAT_HEALTH, -4}});
    HandleDrug(girl, "Fairy Dust Addict", "Fairy Dust", {"Fairy Dust"}, 20, withdraw,
            {{STAT_HAPPINESS, -30}, {STAT_OBEDIENCE, -30}, {STAT_HEALTH, -4}});
    HandleDrug(girl, "Shroud Addict", "Shroud Mushrooms", {"Shroud Mushroom"}, 20, withdraw,
            {{STAT_HAPPINESS, -30}, {STAT_OBEDIENCE, -30}, {STAT_HEALTH, -4}});
    HandleDrug(girl, "Alcoholic", "Alcohol", {"Alcohol"}, 15, withdraw,
                {{STAT_HAPPINESS, -10}, {STAT_OBEDIENCE, -10}, {STAT_HEALTH, -1}});
    if (girl.has_active_trait("Smoker")) // `Gondra` added this since this seemed to be missing IMPORTANT: requires the item
    {
        if (auto item = girl.has_item("Stop Smoking Now Patch"))
        {
            girl.equip(item, false);
            girl.m_Withdrawals = 0;
        }
        else if (auto item = girl.has_item("Stop Smoking Patch"))
        {
            girl.equip(item, false);
            girl.m_Withdrawals = 0;
        } else {
            if(HandleDrug(girl, "Smoker", "Nicotine",
                       {"Magic Carton of Cigarettes", "Magic Pack of Cigarettes", "Carton of Cigarettes",
                        "Pack of Cigarettes", "Small pack of Cigarettes", "Cigarette"},
                       15, withdraw, {{STAT_HAPPINESS,    -10},
                            {STAT_OBEDIENCE,    -5},
                            {STAT_HEALTH,       -1},
                            {STAT_INTELLIGENCE, -2},
                            {STAT_TIREDNESS,    5}})) {
                if (girl.is_dead()) {
                    stringstream cancer;
                    cancer << "${name} has died of cancer from smoking.";
                    girl.AddMessage(cancer.str(), IMGTYPE_PROFILE, EVENT_WARNING);
                    return;
                }
            }
        }
    }

    // sell crappy items
    for(auto& element : girl.inventory().all_items())
    {
        int max = cInventory::NumItemSlots(element.first);
        if(max < 10) {
            if (cGirls::GetNumItemType(girl, element.first->m_Type) > max) // MYR: Bug fix, was >=
            {
                auto nicerThan = cGirls::GetWorseItem(girl, element.first->m_Type, element.first->m_Cost);    // find a worse item of the same type
                if (nicerThan)
                    // `J` zzzzzzzz Add an option to have the girls put the item into "Store Room" instead of selling it
                    cGirls::SellInvItem(girl, nicerThan);
            }
        }
    }


    int usedFood = (g_Dice % 3) + 1;
    int usedFoodCount = 0;
    std::vector<const sInventoryItem*> use_items;
    for(auto& element : girl.inventory().all_items())     // use a food item if it is in stock, and remove any bad things if disobedient
    {
        const sInventoryItem* curItem = element.first;
        if ((curItem->m_Type == sInventoryItem::Food || curItem->m_Type == sInventoryItem::Makeup) && usedFoodCount < usedFood)
        {
            if (!g_Dice.percent(curItem->m_GirlBuyChance)) continue;   // make sure she'd want it herself

            int checktouseit = curItem->m_Effects.size();
            for (int j = 0; j < curItem->m_Effects.size(); j++)
            {
                const sEffect* curEffect = &curItem->m_Effects[j];
                if (curEffect->m_Affects == sEffect::Nothing)
                {  // really? it does nothing? sure, just for the hell of it
                    checktouseit--;
                }
                else if (curEffect->m_Affects == sEffect::GirlStatus)
                {
                    switch (curEffect->m_EffectID)
                    {
                        // these should not be used for items so skip them
                    case STATUS_NONE:
                    case STATUS_CATACOMBS:
                    case STATUS_ARENA:
                    case STATUS_ISDAUGHTER:
                        break;
                        // these statuses need to be tested individually
                    case STATUS_PREGNANT:
                        if (curEffect->m_Amount == 0 && girl.has_status(STATUS_PREGNANT))
                        {
                            if (g_Dice.percent(5)) checktouseit--;        // 5% chance she wants to get rid of the baby
                        }
                        if (curEffect->m_Amount == 1 && !girl.is_pregnant())
                        {
                            if (g_Dice.percent(50)) checktouseit--;        // you gave it to her so she will consider using it
                        }
                        break;
                    case STATUS_PREGNANT_BY_PLAYER:
                        if (curEffect->m_Amount == 1 && !girl.is_pregnant())
                        {
                            if (girl.pclove()>80)    // she love you and wants to have your child
                                checktouseit--;
                        }
                        if (curEffect->m_Amount == 0 && girl.has_status(STATUS_PREGNANT_BY_PLAYER))
                        {
                            if (girl.pclove()<-90)    // she hates you and doesn't want to have your child
                                checktouseit--;
                        }
                        break;
                    case STATUS_INSEMINATED:
                        if (curEffect->m_Amount == 0 && girl.has_status((STATUS)curEffect->m_EffectID))
                        {
                            if (g_Dice.percent(50)) checktouseit--;    // she might not want give birth to a beast
                        }
                        if (curEffect->m_Amount == 1 && !girl.is_pregnant())
                        {
                            if (g_Dice.percent(50)) checktouseit--;    // she might want give birth to a beast
                        }
                        break;
                        // if she has these statuses and the item removes it she will use it
                    case STATUS_POISONED:
                    case STATUS_BADLY_POISONED:
                    case STATUS_SLAVE:
                    case STATUS_CONTROLLED:
                        if (curEffect->m_Amount == 0 && girl.has_status((STATUS)curEffect->m_EffectID))
                        {
                            checktouseit--;
                        }
                        break;
                    }

                }
                else if (curEffect->m_Affects == sEffect::Trait)
                {
                    if ((curEffect->m_Amount >= 1) != girl.has_active_trait(curEffect->m_Trait.c_str()))
                    {  // girl has trait and item removes it, or doesn't have trait and item adds it
                        checktouseit--;
                    }
                }
                else if (curEffect->m_Affects == sEffect::Stat)
                {
                    STATS Stat{(STATS)curEffect->m_EffectID};

                    if ((curEffect->m_Amount > 0) && is_in(Stat, {STAT_CHARISMA, STAT_HAPPINESS, STAT_FAME, STAT_LEVEL,
                                                                  STAT_ASKPRICE, STAT_EXP, STAT_BEAUTY}))
                    {  // even if this stat can't be increased further, she still wants it (call it vanity, greed, whatever)
                        checktouseit--;
                    }
                    if (curEffect->m_Amount > 0 && girl.get_stat(Stat) < 100 &&
                        is_in(Stat, {STAT_LIBIDO, STAT_CONSTITUTION, STAT_INTELLIGENCE, STAT_CONFIDENCE, STAT_MANA,
                               STAT_AGILITY, STAT_SPIRIT, STAT_HEALTH}))
                    {  // this stat increase would be good
                        checktouseit--;
                    }
                    if ((curEffect->m_Amount < 0) && (girl.get_stat(Stat) > 0) &&
                        (
                        Stat == STAT_AGE || Stat == STAT_TIREDNESS
                        )
                        )
                    {  // decreasing this stat would actually be good
                        checktouseit--;
                    }
                }
                else if (curEffect->m_Affects == sEffect::Skill)
                {
                    if ((curEffect->m_Amount > 0) && (girl.get_stat((STATS)curEffect->m_EffectID) < 100))
                    {  // skill would actually increase (wouldn't want to lose any skills)
                        checktouseit--;
                    }
                }
                else if (curEffect->m_Affects == sEffect::Enjoy)
                {
                    if ((curEffect->m_Amount > 0) && (girl.get_enjoyment((Action_Types)curEffect->m_EffectID) < 100))
                    {  // enjoyment would actually increase (wouldn't want to lose any enjoyment)
                        checktouseit--;
                    }
                }
            }

            if (checktouseit < (int)curItem->m_Effects.size() / 2) // if more than half of the effects are useful, use it
            {  // hey, this consumable item might actually be useful... gobble gobble gobble
                use_items.push_back(curItem);
            }
        }

        // MYR: Girls shouldn't be able (IMHO) to take off things like control bracelets
        //else if(curItem->m_Badness > 20 && DisobeyCheck(girl, ACTION_GENERAL) && girl.m_EquipedItems[i] == 1)
        //{
        //    g_Game->inventory_manager().Unequip(girl, i);
        //}
    }

    for(auto& item : use_items) {
        girl.equip(item, false);
        usedFoodCount++;
    }

    // add the selling of items that are no longer needed here
}

void cGirls::SellInvItem(sGirl& girl, const sInventoryItem* item)
{
    girl.m_Money += (int)((float)item->m_Cost * 0.5f);
    girl.unequip(item);
}

const sInventoryItem* cGirls::GetWorseItem(const sGirl& girl, int type, int cost)
{
    for(auto& inv : girl.inventory().all_items()) {
        if(inv.first->m_Type == type && inv.first->m_Cost < cost) {
            return inv.first;
        }
    }
    return nullptr;
}

int cGirls::GetNumItemType(const sGirl& girl, int Type, bool splitsubtype)
{
    int num = girl.inventory().get_num_of_type(Type);
    if(!splitsubtype && Type == sInventoryItem::Food) {
        num += girl.inventory().get_num_of_type(sInventoryItem::Makeup);
    }
    return num;
}

// ----- Traits

// If a girl enjoys a job enough, she has a chance of gaining traits associated with it
bool cGirls::PossiblyGainNewTrait(sGirl& girl, string Trait, int Threshold, int ActionType, string Message, bool Day0Night1, EventType eventtype)
{
    if (girl.m_Enjoyment[ActionType] > Threshold)
    {
        int chance = (girl.m_Enjoyment[ActionType] - Threshold);
        if (girl.gain_trait(Trait.c_str(), chance))
        {
            girl.AddMessage(Message, IMGTYPE_PROFILE, eventtype);
            return true;
        }
    }
    return false;
}

// If a girl enjoys a job enough, she has a chance of losing bad traits associated with it
bool cGirls::PossiblyLoseExistingTrait(sGirl& girl, string Trait, int Threshold, int ActionType, string Message, bool Day0Night1)
{
    if (girl.m_Enjoyment[ActionType] > Threshold && girl.has_active_trait(Trait.c_str()))
    {
        int chance = (girl.m_Enjoyment[ActionType] - Threshold);
        if (girl.lose_trait(Trait.c_str(), chance))
        {
            girl.AddMessage(Message, IMGTYPE_PROFILE, EVENT_GOODNEWS);
            return true;
        }
    }
    return false;
}

// `J` When adding new traits, search for "J-Add-New-Traits"  :  found in >> cGirls.cpp > AdjustTraitGroup

std::string AdjustTraitGroup(sGirl& girl, int adjustment, std::initializer_list<const char*> traits, bool event,
        const char* lose_message, const char* gain_message, const char* notrait_name=nullptr) {
    int group = -1;
    int count = 0;
    const char* lost = nullptr;
    for(auto t : traits) {
        // nullptr == no-trait
        if(!t) {
            group = count;
            count += 1;
            continue;
        }
        if (girl.has_active_trait(t)) {
            group = count;
            lost = t;
            break;
        }
        count += 1;
    }

    int new_group = std::min(std::max(0, group + adjustment), (int)traits.size() - 1);
    if(group == new_group)
        return "";

    std::stringstream ss;

    ss << girl.FullName();
    const char* lose_name = notrait_name;
    if(lost) {
        girl.lose_trait(lost);
        lose_name = lost;
    }
    if(lose_name)
        ss << lose_message << " '" << lose_name << "'";

    auto gained = begin(traits);
    std::advance(gained, new_group);
    const char* gain_name = notrait_name;
    if(*gained) {
        gain_name = *gained;
        girl.gain_trait(*gained);
    }

    if(gain_name)
        ss << gain_message << " '" << gain_name << "'.";

    // only send a message if called for
    if (event)    girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);

    return ss.str();
}

// `J` adding these to allow single step adjustment of linked traits
string cGirls::AdjustTraitGroupGagReflex(sGirl& girl, int adjustment, bool showmessage)
{
    if (adjustment == 0) return "";    // no girl or not changing anything so quit
    return AdjustTraitGroup(girl, adjustment,
            {"Strong Gag Reflex", "Gag Reflex", nullptr, "No Gag Reflex", "Deep Throat"}, showmessage,
            " has lost the trait", " has gained the trait");
}

string cGirls::AdjustTraitGroupBreastSize(sGirl& girl, int adjustment, bool showmessage)
{
    if (adjustment == 0) return "";    // no girl or not changing anything so quit
    return AdjustTraitGroup(girl, adjustment,
                            {"Flat Chest", "Petite Breasts", "Small Boobs", nullptr, "Busty Boobs", "Big Boobs", "Giant Juggs",
                             "Massive Melons", "Abnormally Large Boobs", "Titanic Tits"}, showmessage,
                            "'s breast size has changed from", " to", "Average");
}

string cGirls::AdjustTraitGroupFertility(sGirl& girl, int steps, bool showmessage)
{
    if (steps == 0) return "";    // not changing anything so quit
    return AdjustTraitGroup(girl, steps,
                            {"Sterile", nullptr, "Fertile", "Broodmother"}, showmessage,
                            " has lost the trait", " has gained the trait");
}

// Update happiness for trait affects
void cGirls::updateHappyTraits(sGirl& girl)
{
    if (girl.is_dead()) return;    // Sanity check. Abort on dead girl
    if (girl.has_active_trait("Optimist")) girl.happiness(5);

    if (girl.has_active_trait("Pessimist"))
    {
        girl.happiness(-5);
        if (girl.happiness() <= 0 && g_Dice.percent(50))
        {
            stringstream ss;
            string stopper;
            if(girl.m_Building) {
                auto bt = girl.m_Building->type();
                if (bt == BuildingType::ARENA && girl.m_Building->num_girls_on_job(JOB_DOCTORE, 0) > 0)
                    stopper = "the Doctore";
                else if (bt == BuildingType::STUDIO && girl.m_Building->num_girls_on_job(JOB_DIRECTOR, 1) > 0)
                    stopper = "the Director";
                else if (bt == BuildingType::CLINIC && girl.m_Building->num_girls_on_job(JOB_CHAIRMAN, 0) > 0)
                    stopper = "the Chairman";
                else if (bt == BuildingType::CENTRE &&
                        (girl.m_DayJob == JOB_REHAB || girl.m_PrevDayJob == JOB_REHAB) &&
                        girl.m_Building->num_girls_on_job(JOB_COUNSELOR, 0) > 0)
                    stopper = "her Counselor";
                else if (bt == BuildingType::CENTRE && girl.m_Building->num_girls_on_job(JOB_CENTREMANAGER, 0) > 0)
                    stopper = "the Centre Manager";
                else if (bt == BuildingType::HOUSE && g_Dice.percent(50))
                    stopper = "You";
                else if (bt == BuildingType::HOUSE &&  girl.m_Building->num_girls_on_job(JOB_HEADGIRL, 0) > 0)
                    stopper = "your Head Girl";
                else if (bt == BuildingType::FARM &&  girl.m_Building->num_girls_on_job(JOB_FARMMANGER, 0) > 0)
                    stopper = "the Farm Manger";
                else if ( girl.m_Building->num_girls_on_job(JOB_MATRON, 0) > 0)
                    stopper = "the Matron";

            }

            int Schance = g_Dice.d100();
            if (!stopper.empty())
            {
                ss << "${name} tried to killed herself but " << stopper;
                if (Schance < 50)        { ss << " talked her out of it."; }
                else if (Schance < 90)    { ss << " stopped her."; }
                else    { girl.set_stat(STAT_HEALTH, 1);    ss << " revived her."; }
                girl.AddMessage(ss.str(), IMGTYPE_DEATH, EVENT_DANGER);
            }
            else
            {
                string msg = girl.FullName() + " has killed herself since she was unhappy and depressed.";
                girl.AddMessage(msg, IMGTYPE_DEATH, EVENT_DANGER);
                g_Game->push_message(msg, COLOR_RED);
                girl.health(-1000);
            }
        }
    }
}

// ----- Sex

void cGirls::GirlFucks(sGirl* girl, bool Day0Night1, sCustomer* customer, bool group, string& message, SKILLS &SexType, bool first)
{
    int check = girl->get_skill(SexType);
    string girlName = girl->FullName();

    //SIN: let's add a problem...
    if (g_Dice.percent(33) && (girl->happiness() < 40) && (girl->intelligence() < 50)
        && is_addict(*girl, true))
    {
        stringstream runawaymsg;
        if (is_your_daughter(*girl)) runawaymsg << "Your daughter ";
        runawaymsg << girlName << " ran away with your rival!\nExploiting her desperate drug cravings, he claimed to be ";

        //the con
        runawaymsg << g_Dice.select_text({"her true love, ", "a Prince, ", "the Spy who Loves her, ",
                                          "her long-lost brother, ", "a Priest, ", "her old school friend, "});
        runawaymsg << "promising her a better life and everything she needs if she escaped with him.\nShe did. By now she will probably be ";

        //speculate
        runawaymsg << g_Dice.select_text({"tied over a park-bench being gang-raped by ",
                                          "chained to a rack being ram-raided by ",
                                          "tied up in a dumpster giving blowjobs to ",
                                          "folded over a fence being 'used' by ",
                                          "naked in the town arena being 'conquered' by ",
                                          "stripped and locked in the town's public-stocks, being 'punished' by "
                                          });

        runawaymsg << g_Dice.select_text({"rival gang-members.", "bums.", "horny street kids.", "mistreated slaves.",
                                          "wild animals.", "pumped-up gladiators.", "the town's gentlemen.", "aristocrats."});
        runawaymsg << '\n';

        //What do you do...
        /* */if (g_Game->player().disposition() < -33) runawaymsg << "She's where she deserves. Why waste a gang's time going to fetch her? Unless you want to punish personally?";
        else if (g_Game->player().disposition() < 33) runawaymsg << "You could send a gang to retrieve her. Or you could leave her. No hurry.";
        else runawaymsg << "You should send a gang right away to rescue the poor girl.";

        runawaymsg << " (When you find her, she may be... changed.)";

        //If she was a virgin, she won't be now...
        girl->lose_trait("Virgin");

        //What damage?
        int harm = g_Dice.d100();
        if (harm > 95) //5% multi STDS
        {
            harm = g_Dice.d100();
            if (harm == 100)    girl->gain_trait("AIDS"),     girl->gain_trait("Syphilis"), girl->gain_trait("Herpes"), girl->gain_trait("Chlamydia");
            else if (harm > 95) girl->gain_trait("AIDS"),        girl->gain_trait("Syphilis");
            else if (harm > 85) girl->gain_trait("AIDS"),        girl->gain_trait("Herpes");
            else if (harm > 70) girl->gain_trait("Syphilis"),    girl->gain_trait("Herpes");
            else if (harm > 50) girl->gain_trait("Syphilis"),    girl->gain_trait("Chlamydia");
            else                girl->gain_trait("Herpes"),    girl->gain_trait("Chlamydia");
        }
        else if (harm > 90)  //5% an STD
        {
            harm = g_Dice.d100();
            if (harm > 95)        girl->gain_trait("AIDS");
            else if (harm > 80)    girl->gain_trait("Syphilis");
            else if (harm > 50) girl->gain_trait("Herpes");
            else                girl->gain_trait("Chlamydia");
        }
        else if (harm > 85)  //10% scars
        {
            if (!girl->has_active_trait("Small Scars") && !girl->has_active_trait("Cool Scars") && !girl->has_active_trait(
                    "Horrific Scars")) girl->gain_trait("Small Scars");
            else if (girl->has_active_trait("Small Scars")) girl->gain_trait("Cool Scars");
            else if (girl->has_active_trait("Cool Scars")) girl->gain_trait("Horrific Scars");
        }
        else if (harm > 75)  //10% traumatised
        {
            girl->gain_trait("Mind Fucked");
        }
        else if (harm > 50)  //25% chance
        {
            //overused face
            girl->gain_trait("Missing Teeth");
            AdjustTraitGroupGagReflex(*girl, +1);

            girl->lose_trait("Optimist");
            girl->upd_base_stat(STAT_DIGNITY, -10);
            girl->upd_base_stat(STAT_SPIRIT, -10);
            girl->upd_skill(SKILL_ORALSEX, 5);
        }
        else if (harm > 25)  //25% chance
        {
            //overused behind
            girl->gain_trait("Whore");

            girl->lose_trait("Optimist");
            girl->upd_base_stat(STAT_HEALTH, -5);
            girl->upd_base_stat(STAT_SPIRIT, -5);
            girl->upd_temp_stat(STAT_LIBIDO, -50, true);
            girl->upd_skill(SKILL_NORMALSEX, 5);
        }
        else if (harm > 15)  //10% chance
        {
            girl->gain_trait("Broken Will");
            girl->gain_trait("Branded on the Ass");
        }
        else //15% no damage
        {

        }


        //does she get knocked up?
        bool antiPregStatus = girl->m_UseAntiPreg;
        girl->m_UseAntiPreg = false;                    //won't have access to this
        girl->calc_group_pregnancy(*customer, 2);
        girl->m_UseAntiPreg = antiPregStatus;            //afterwards she'll go back to normal

        // player has 6 weeks to retrieve
        girl->run_away();

        //Warn the user
        g_Game->push_message(runawaymsg.str(), COLOR_RED);
        return;
    }


    bool contraception = false;
    double STDchance = 0.001;        // `J` added new percent that allows 3 decimal check so setting a 0.001% base chance
    int happymod = 0;    // Start the customers unhappiness/happiness bad sex decreases, good sex inceases

    if (customer->m_Fetish == FETISH_FUTAGIRLS && !girl->has_active_trait("Futanari"))
    {
        happymod -= 10;
    }

    happymod += girl->get_trait_modifier("sex:*");
    happymod += girl->get_trait_modifier((std::string("sex:") + get_fetish_name((Fetishs)customer->m_Fetish)).c_str());

    girl->m_NumCusts += (int)customer->m_Amount;
    if (group && (customer->m_SexPref != SKILL_GROUP || customer->m_SexPref != SKILL_STRIP))
    {
        // the customer will be an average in all skills for the customers involved in the sex act
        SexType = SKILL_GROUP;
    }

    // If the girls skill < 50 then it will be unsatisfying otherwise it will be satisfying
    happymod += (girl->get_skill(SexType) - 50) / 5;
    // If the girl is famous then he will be slightly happier
    happymod += girl->fame() / 5;

    // her service ability will also make him happier (I.e. does she help clean him well)
    happymod += girl->service() / 10;

    customer->happiness(happymod);            // `J` now set customers happiness

    // her magic ability can make him think he enjoyed it more if she has mana

    int happycost = 3 - int(girl->magic() / 40);    // `J` how many mana will each point of happy cost her
    if (happycost < 1) happycost = 1;        // so [magic:cost] [<10:can't] [10-39:3] [40-79:2] [80+:1] (probably, I hate math)
    if (customer->happiness() < 100 &&            // If they are not fully happy
        girl->mana() >= happycost &&        // If she has enough mana to actually try
        girl->magic() > 9)                // If she has at least 10 magic
    {
        int happymana = girl->mana();                    // check her mana
        if (happymana > 20) happymana = 20;                            // and only max of 20 will be used
        int happygain = happymana / happycost;                        // check how much she can increase happiness with 20 mana
        if (happygain > 10) happygain = 10;                            // can only increase happy by 10
        int lesshappy = 100 - customer->happiness();                // how much can she charm him before it is wasted?
        if (happygain > lesshappy) happygain = lesshappy;            // can only increase happy by 10
        happymana = happygain * happycost;                            // check how much mana she actually spends
        if (happymana > 20) happymana = 20;                            // correct incase more than 20
        if (happymana < 0) happymana = 1;                            // will spend at least 1 mana just for trying

        customer->happiness(happygain);
          girl->upd_base_stat(STAT_MANA, -happymana);                    // and apply mana
    }


    string introtext = girlName;
    // `J` not sure if all of the options will come up in appropriate order but it is a good start.
    int intro = g_Dice % 15;
    intro += girl->libido() / 20;
    intro += check / 20;

    bool z = false;
    if (girl->has_active_trait("Zombie"))
    {
        z = true;
        introtext += " follows";
    }
    else
    {
        // need to add more traits
        if (!customer->m_IsWoman && girl->has_active_trait("Cum Addict")) intro += 4;
        if (customer->m_IsWoman && girl->has_active_trait("Lesbian")) intro += 3;
        if (customer->m_IsWoman && girl->has_active_trait("Straight")) intro -= 3;
        intro += girl->get_trait_modifier("sex.eagerness");

        //SIN: Fix ordering and wording - delete old if this okay
        /* */if (intro < 2)        introtext += " reluctantly leads";
        else if (intro < 4)        introtext += " hesitantly leads";
        else if (intro < 8)        introtext += " leads";
        else if (intro < 12)    introtext += " quickly leads";
        else if (intro < 18)    introtext += " eagerly leads";
        else if (intro < 22)    introtext += " excitedly leads";
        else if (intro < 25)    introtext += " almost drags";
        else                    introtext += " eagerly drags";
    }

    if (SexType == SKILL_GROUP)    introtext += " her customers ";
    else introtext += " her customer ";

    int currentjob = girl->get_job(Day0Night1);
    if (currentjob == JOB_WHOREBROTHEL || currentjob == JOB_BARWHORE || currentjob == JOB_WHOREGAMBHALL)
    {
        //SIN - added some variety here
        //How many options do we need for a litttle randomness below? State it here.
        const int OPTS = 4;

        if (SexType == SKILL_GROUP && g_Dice.percent(40))
        {
            introtext += "to the ";
            introtext += g_Dice.select_text({"pirate-themed 'Cap'n's Orrrrgy Cabin.'\n", "college-themed dorm room.\n",
                                             "prison-themed communal shower.\n", "secluded orgy garden.\n"});
        }
        else if (SexType == SKILL_BDSM && g_Dice.percent(40))
        {
            // now, what kind of person are you?
            if (g_Game->player().disposition() < -40)    //You're related to satan, right?
            {
                introtext += "deep underground to your infamous dungeons.\n";
                introtext += g_Dice.select_text({
                    "\"This is really it, isn't it?!\" he smiles, wonderstruck. ",
                    "He stops, crouching, putting his fingers to a dark stain on the concrete floor: \"I've heard so much about this place,\" he whispers. ",
                    "\"So it's true. All of it!\" he glances between the stock, the chains, the whips and cages. ",
                    "\"They say you can do ANYTHING here,\" he hisses. "
                });
                introtext += "\"" + g_Game->player().FullName() + "'s Dungeon.\"\n";
                if (g_Dice.percent(20)) introtext += "\nHe laughs nervously. \"I do get to come out again, right?\"\n";
                customer->happiness(10);    //Evil customer likes evil
            }
            else if (g_Game->player().disposition() < 0)        //You're bad, you're bad, you know it.
            {
                introtext += "to your ominous dungeons.\n";
                introtext += g_Dice.select_text({
                    "The customer looks around: \"This is it... ",
                    R"("I've heard about this place," he says evenly. ")",
                    R"("I heard the rumours but I never believed them," he says. ")",
                    "He grins: \"So you'll be my toy here in "
                });
                introtext += g_Game->player().FullName() + "'s Dungeon.\"\n";
            }
            else if (g_Game->player().disposition() < 40)        //You're good
            {
                introtext += "to the dungeons.\n";
            }
            else                                    //You're nice, bless your little cotton socks.
            {
                introtext += "to an aseptic, custom-made and well-equipped Bondage room";
                introtext += g_Dice.select_text({" right next to the first aid station.\n",
                                                 " within earshot of the guard station. Just in case.\n",
                                                 " designed for safety first.\n\"Some interesting toys here\" he says.",
                                                 ".\nThe customer nods approvingly.\n"
                                                 });
                if (g_Dice.percent(20)) introtext += " \"Nice set up.\"\n";
            }
        }
        else if (SexType == SKILL_BEASTIALITY && g_Dice.percent(40))
        {
            introtext += g_Dice.select_text({"to the beasts' den.\n", "to the animal pit.\n", "to the monster's lair.\n",
                                             "to a small cave in the garden.\n"});
        }
        else    // nice room = nicer for customer too, right? (Good customer likes Good)
        {

            introtext += "back to her ";
            if (girl->m_AccLevel < 2)
            {
                introtext += "cramped little hovel.";
                customer->happiness(-5);
                if (SexType == SKILL_GROUP && g_Dice.percent(50))
                {
                    introtext += " It was awkward getting this many customers in her tiny room.";
                    customer->happiness(-5);
                }
            }
            else if (girl->m_AccLevel < 4) introtext += "modest little room.";
            else if (girl->m_AccLevel < 6) introtext += "nice, spacious room.",                customer->happiness(5);
            else if (girl->m_AccLevel < 8) introtext += "large, elegant room.",                customer->happiness(10);
            else /*                     */ introtext += "huge, extravagant suite.",            customer->happiness(20);
            introtext += '\n';
        }
    }
    else if (currentjob == JOB_WHORESTREETS)
        introtext += "to a secluded alley way.\n";
    else if (currentjob == JOB_PEEP)
        introtext = "";        // intro is handled in peep show job
    else
        introtext += "to a secluded spot.\n";

    if (z)
    {
        introtext += "She blankly stares at them as they proceed to have their way with her.\n";
    }
    else
    {
        if (currentjob == JOB_PEEP){}
        else if ((girl->has_active_trait("Nervous") && girl->m_Enjoyment[ACTION_SEX] < 10) ||
            girl->m_Enjoyment[ACTION_SEX] < -20)
        {
            introtext += "She is clearly uncomfortable with the arrangement, and it makes the customer feel uncomfortable.\n";
            customer->happiness(-5);
        }
        else if (g_Dice.percent(10))
        {
            introtext += "She smiles slightly and makes the effort to arouse the customer.\n";
        }
        else
        {
            introtext += '\n';
        }
    }
    introtext += '\n';
    message += introtext;



    int choice = g_Dice.d100(); //Gondra: initializing a choice variable here because god this is a mess of different ways to handle this
    stringstream sexMessage; //Gondra: using this as a temporary storage so I have less problems when there shouldn't be girlname at the start.

    //SIN- Adding will for willfull chars - they can refuse jobs they are bad at.
    //First, a var to store her willfullness. Max (full spirit, no obed) = 50% refusal; Min (all obedience, no spirit) = 0%
    int willfullness = ((100 + (girl->spirit() - girl->obedience())) / 2);
    // next a couple of reasons why refuse
    bool pigHeaded = girl->has_active_trait("Iron Will");
    bool highStatus = (girl->has_active_trait("Princess") || girl->has_active_trait("Queen") || girl->has_active_trait("Noble"));

    // now the implementation...
    if ((check < 40) && !z && !girl->has_active_trait("Mute"))  //if she's bad at this sex-type (and not a zombie or mute!), pride kicks in
    {
        //if she's pigheaded, or thinks this is beneath her - she refuses
        if (g_Dice.percent(willfullness) && (pigHeaded || highStatus))
        {
            //Initiate locally used stuff
            SKILLS newSexType = SexType;            //What will she change it to?
            bool resisting = true;        //Does she even resist? Low-level non-intrusive requests will be honored
            bool forced = false;        //in case the customer overpowers her
            bool askedForHerTopSkill = false;    //in case what she's refusing is already her top skill.
            bool angry = false;            //has he actively pissed her off

            //Why is she refusing - let's put it in a string now
            string refusesbecause;
            if (pigHeaded)
            {
                refusesbecause = g_Dice.select_text({"Whatever anyone says, she's nobody's sex-slave.",
                                                     "It's her body: she will not let him do this to her.",
                                                     "No way. He'll have to kill her first.",
                                                     "She still has some sense of self-worth.",
                                                     "If she doesn't want to, she doesn't want to. End of."
                                                     });
                refusesbecause += '\n';
            }
            else
            {
                refusesbecause = g_Dice.select_text({"This is beneath her.", "Does he think she's some common street whore?",
                                                     "A Lady does NOT do this.", "She will leave this kind of thing for the common-folk.",
                                                     "She was raised for Greatness, not this."});
                refusesbecause += '\n';
            }

            //Find top skill - what 'skill' is she most comfortable with? Working from the most extreme down...
            SKILLS TopSkillID = NUM_SKILLS;
            int TopSkillLev = 0;

            /// TODO this code is simply wrong. This will always result in strip.
            if (TopSkillLev < girl->beastiality())    TopSkillID = SKILL_BEASTIALITY;
            if (TopSkillLev < girl->anal())            TopSkillID = SKILL_ANAL;
            if (TopSkillLev < girl->normalsex())    TopSkillID = SKILL_NORMALSEX;
            if (TopSkillLev < girl->oralsex())        TopSkillID = SKILL_ORALSEX;
            if (TopSkillLev < girl->tittysex())        TopSkillID = SKILL_TITTYSEX;
            if (TopSkillLev < girl->handjob())        TopSkillID = SKILL_HANDJOB;
            if (TopSkillLev < girl->footjob())        TopSkillID = SKILL_FOOTJOB;
            if (TopSkillLev < girl->strip())        TopSkillID = SKILL_STRIP;
            TopSkillLev = girl->get_skill(TopSkillID);

            //is the thing she's being asked for already her top skill?
            if (TopSkillID == SexType) askedForHerTopSkill = true;


            //She's getting willful, time to explain why and what is being refused
    switch (SexType)
    {
            case SKILL_BEASTIALITY:
                if (choice < 33)
                {
                    sexMessage << girlName << " will not be fucked by animals for this slithery pervert's entertainment. He only wants to see her degraded.\n"
                        << refusesbecause
                        << R"("No," she says. "Not doing that.")";
                }
                else if (choice < 66)
                {
                    sexMessage << "The customer brings in a unicorn-like creature, with a large horn on it's head and the hugest, weirdest animal-cock " << girlName
                        << " has ever seen. He smirks at her with a shit-eating grin.\n"
                        << refusesbecause
                        << "\"No,\" she says simply.";
                }
                else
                {
                    sexMessage << "The customer want to see " << girlName << " fucked by large tentacle creatures.\n"
                        << refusesbecause
                        << "She refuses to let the beast near her.";
                }
                break;

            case SKILL_BDSM:
                if (choice < 33)
                {
                    sexMessage << "Without warning, the customer grabs " << girlName << " by the throat and slams her back onto a table.\n"
                        << "\"You're mine, whore,\" he growls, climbing onto her. The last straw is when he spits in her mouth.\n"
                        << refusesbecause
                        << "She resists";
                    angry = true;
                    if (g_Dice.percent(girl->strength())) sexMessage << " and in her fury, overpowers him.";
                    else if (g_Dice.percent(girl->combat())) sexMessage << ", and in her fury fights him off.";
                    else
                    {
                        sexMessage << ", but he is too strong, and with his arm locked on her throat, she can't even scream. "
                            << "As he tortures and rapes her, her blood runs cold with impotent fury.";
                        forced = true;
                                          girl->upd_base_stat(STAT_HAPPINESS, -5);    //sad
                                          girl->upd_base_stat(STAT_SPIRIT, 2);        //angry
                        //girl->upd_stat(STAT_SANITY, -4);        //and a bit crazy
                    }
                }
                else if (choice < 67)
                {
                    sexMessage << "As " << girlName << " bends down easing off her panties, giving the customer a great show, he unexpectedly whips her ass/pussy. "
                        << "Hard. With an actual whip - three-headed, metal-tipped.\nShe yelps in pain, as he howls laughing.\n"
                        << refusesbecause
                        << "She cooly locks the BDSM tools away.";
                }
                else
                {
                    sexMessage << "As customer starts to paw through the whips, clamps, and insertions, " << girlName << " decides she doesn't feel like being anyone's bitch right now.\n"
                        << refusesbecause
                        << "\"We'll do something else.\"\nHe looks crest-fallen.";
                }
                break;

            case SKILL_ANAL:
                if (choice < 33)
                {
                    sexMessage << "Without warning, the customer slips a finger up " << girlName << "'s asshole. She squeals and spins away.\n"
                        << refusesbecause
                        << R"("THAT," she says. "Is out of bounds.")";
                }
                else if (choice < 67)
                {
                    sexMessage << "The customer pats " << girlName << "'s ass, and grins with a wink: \"After THIS don't reckon you gonna be sittin' down for a while.\"\n"
                        << refusesbecause
                        << "She just shakes her head: \"Not happening.\"\nHis face drops.";
                }
                else
                {
                    sexMessage << girlName << " is still removing her clothes when the customer pushes her over a desk, yanks apart her butt-cheeks, and spits on her asshole. "
                        << "She spins round like a demon and shoves him away.\n"
                        << refusesbecause
                        << "\"NOT that.\"";
                    angry = true;
                }
                break;

            case SKILL_NORMALSEX:
                if (highStatus && customer->m_Class == 1 && g_Dice.percent(20))
                {
                    sexMessage << "The customer, a minor noble, smirked when he saw " << girlName << ".\n\"" << girlName
                        << "! A lady or breeding!\" he says pulling away her clothes. \"Okay, let's breed.\"\n"
                        << refusesbecause
                        << "\"I was intimate with three beggars before,\" she says coldly, shaking her head. \"But that's as low-class as I'm willing to go.\"";
                    angry = true;
                }
                else if (choice < 33)
                {
                    sexMessage << "Without warning, the customer shoves three fingers up " << girlName << "'s pussy. She squeals and pulls away.\n"
                        << refusesbecause
                        << "\"That is NOT how you ask,\" she tells him angrily.";
                }
                else if (choice < 67)
                {
                    sexMessage << "The customer nods approvingly at " << girlName << "'s pussy.\n\"Damn,\" he grins. \"I am gonna tear that apart.\"\n"
                        << refusesbecause
                        << "She shakes her head.\nHis face drops.";
                }
                else
                {
                    sexMessage << "As " << girlName << " makes a show of easing off her panties, bending way down to give the customer a view, he pounces slamming his cock hard into her pussy.\n"
                        << refusesbecause
                        << "She thrashes to shake him off";
                    angry = true;
                    if (g_Dice.percent(girl->strength())) sexMessage << ", and in a frenzy, throws him clean across the room.";
                    else if (g_Dice.percent(girl->combat())) sexMessage << ", a hammer-fist between her legs catching him right in the balls. He falls back with a whimper.";
                    else
                    {
                        sexMessage << "But he is too strong, and pinned under all his weight, she can't even wriggle away. "
                            << "As his filthy cock slides into her, she refuses to give him any pleasure.";
                        forced = true;
                                          girl->upd_base_stat(STAT_HAPPINESS, -5);    //sad
                                          girl->upd_base_stat(STAT_SPIRIT, 2);        //angry
                        //girl->upd_stat(STAT_SANITY, -2);        //crazy
                        check -= 40;                            //deliberately underperforms
                    }
                }
                break;

            case SKILL_ORALSEX:
                if (highStatus && customer->m_Class == 1 && g_Dice.percent(20))
                {
                    sexMessage << "The customer, a minor noble, smirked when he saw " << girlName << ".\n\"" << girlName
                        << "!\" he smiles pulling out his cock. \"Get on your knees. Remember when you were 'too good' for me?!\"\n"
                        << refusesbecause
                        << "\"I still am,\" she says coldly.";
                    angry = true;
                }
                else if (choice < 33)
                {
                    sexMessage << "Without warning, the customer waves his cock in " << girlName << "'s face. She pats it away, shaking her head.\n"
                        << refusesbecause;
                }
                else if (choice < 67)
                {
                    sexMessage << "The customer suddenly puts his hand in " << girlName << "'s mouth, grabbing her by the jaw and pinching her lips.\n";
                    if (girl->has_active_trait("Dick-Sucking Lips")) sexMessage << "\"Those lips!\" he says, shoving her head down. \"Made for dick-sucking.\"\n";
                    else sexMessage << "\"On your knees, whore,\" he says. \"And suck this.\"\n"
                        << refusesbecause
                        << "She shakes her head: \"Not if you wanna keep it.\"";
                }
                else
                {
                    sexMessage << girlName << " is still locking the door when the customer grabs her head and starts pulling it down toward his cock.\n"
                        << refusesbecause
                        << "She grabs his balls with a sharp squeeze. He freezes.\n\"I wouldn't do that,\" she says.";
                }
                break;

            default:
                resisting = false;    //if they're asking for anything else, it is okay
                break;
            }
            if (resisting)
            {
                sexMessage << '\n';

                //if she's resisted what they asked for, and has not been forced she "chooses"...
                if (!forced)
                {
                    //If she's pissed, she won't give more than an angry handjob
                    if (angry)
                    {
                        newSexType = SKILL_HANDJOB;
                        check -= 30;
                    }
                    else
                    {
                        //she will give her best skill - or if that's what they already asked for...
                        if (!askedForHerTopSkill) newSexType = TopSkillID;
                        else
                        {
                            //she will statically downgrade to something (K.I.S.S.)
                            if (SexType == SKILL_BEASTIALITY)        newSexType = SKILL_NORMALSEX;
                            else if (SexType == SKILL_BDSM)            newSexType = SKILL_NORMALSEX;
                            else if (SexType == SKILL_ANAL)            newSexType = SKILL_NORMALSEX;
                            else if (SexType == SKILL_NORMALSEX)    newSexType = SKILL_HANDJOB;
                            else if (SexType == SKILL_ORALSEX)        newSexType = SKILL_HANDJOB;
                        }
                    }

                    if (SexType != newSexType) //if these aren't the same, report the change
                    {
                        //report the old
                        switch (SexType)
                        {
                        case SKILL_BEASTIALITY:
                            sexMessage << "\"I won't fuck some filthy animal for you,\" " << girlName << " says";
                            break;
                        case SKILL_BDSM:
                            sexMessage << "\"You do not get to abuse me. You do not get to hurt me.\" " << girlName << " says";
                            break;
                        case SKILL_ANAL:
                            sexMessage << "\"There is no way That is going in my ass ";
                            Day0Night1 ? sexMessage << "today,\" " : sexMessage << "tonight,\" ";
                            sexMessage << girlName << " says";
                            break;
                        case SKILL_NORMALSEX:
                            sexMessage << "\"I will not have you in my pussy,\" " << girlName << " says";
                            break;
                        case SKILL_ORALSEX:
                            sexMessage << "\"Your cock is not going in my mouth ";
                            Day0Night1 ? sexMessage << "today,\" " : sexMessage << "tonight,\" ";
                            sexMessage << girlName << " says";
                            break;
                        default: // no way, if it doesn't match these types, it should not change.
                            sexMessage << "(E)";
                            break;
                        }
                        //report the new
                        switch (newSexType)
                        {
                        case SKILL_BEASTIALITY:
                            sexMessage << ". \"I'd sooner fuck a beast than that... Though I guess you can watch if you want.\"";
                            break;
                        case SKILL_ANAL:
                            sexMessage << ". \"But you can have my ass, if you want.\"";
                            break;
                        case SKILL_NORMALSEX:
                            sexMessage << ". \"But if you're man enough, I guess you can fuck me.\"";
                            break;
                        case SKILL_ORALSEX:
                            sexMessage << ", adding with a smile. \"But if you want your dick sucked";
                            if (highStatus)
                            {
                                if (girl->has_active_trait("Queen")) sexMessage << " by a Queen";
                                else if (girl->has_active_trait("Princess")) sexMessage << " by a Princess";
                                else sexMessage << " by Nobility";
                            }
                            else sexMessage << " by a girl who doesn't stop";
                            sexMessage << "... That can still happen.\"";
                            break;
                        case SKILL_TITTYSEX:
                            sexMessage << ". She slips off her gown exposing her chest, and sits back on the bed, rubbing her nipples and squeezing ";
                            if (girl->breast_size() <= 3) sexMessage << "her small, under-developed breasts";
                            else if (girl->breast_size() >= 8) sexMessage << "her enormous breasts";
                            else if (girl->breast_size() >= 5) sexMessage << "her large, round breasts";
                            else sexMessage << "her breasts";
                            sexMessage << ". \"But we can oil these up and have some fun.\"";
                            break;
                        case SKILL_HANDJOB:
                            if (angry) sexMessage << ". Without a word she grabs his cock and starts giving an angry handjob.";
                            else sexMessage << ", grabbing his penis and rubbing it. \"But this is okay.\"";
                            break;
                        case SKILL_FOOTJOB:
                            sexMessage << ". \"But have you ever tried a girl who's good with her feet?\"";
                            break;
                        case SKILL_STRIP:
                            sexMessage << ". \"But if you promise to good from here, I'll give you a lapdance.\"";
                            break;
                        default: // no way, if it doesn't match these types, it should not change.
                            sexMessage << "(E)";
                            break;
                        }
                        //finally update variable for next bit
                        SexType = newSexType;
                        sexMessage << '\n';
                    }

                }
                //Forced to do what they want, so no changes
                //let's reset choice to avoid outcomes above being always paired with ones below
                choice = g_Dice.d100();
            }
            //She didn't have a problem with what they asked for
        }
        //else she doesn't refuse - so move along, nothing to see here
    }

    EDefaultEvent event = EDefaultEvent::GIRL_SEX_STRIP;
    switch (SexType)
    {
    case SKILL_ANAL:        event = EDefaultEvent::GIRL_SEX_ANAL; break;
    case SKILL_BDSM:        event = EDefaultEvent::GIRL_SEX_BDSM; break;
    case SKILL_NORMALSEX:   event = EDefaultEvent::GIRL_SEX_NORMAL; break;
    case SKILL_ORALSEX:     event = EDefaultEvent::GIRL_SEX_ORAL; break;
    case SKILL_TITTYSEX:    event = EDefaultEvent::GIRL_SEX_TITTY; break;
    case SKILL_HANDJOB:     event = EDefaultEvent::GIRL_SEX_HAND; break;
    case SKILL_FOOTJOB:     event = EDefaultEvent::GIRL_SEX_FOOT; break;
    case SKILL_BEASTIALITY: event = EDefaultEvent::GIRL_SEX_BEAST; break;
    case SKILL_GROUP:       event = EDefaultEvent::GIRL_SEX_GROUP; break;
    case SKILL_LESBIAN:     event = EDefaultEvent::GIRL_SEX_LESBIAN; break;
    case SKILL_STRIP:       event = EDefaultEvent::GIRL_SEX_STRIP; break;
    default: /* did some non-sex skill sneak in? */ break;
    }    //end switch

    auto result = girl->CallScriptFunction(event, customer);
    message += interpolate_string(boost::get<std::string>(result), [girl](const std::string& pattern) -> std::string {
        if(pattern == "name") {
            return girl->FullName();
        }
        throw std::runtime_error("Invalid pattern " + pattern);
    }, g_Dice) + '\n';


    message += (SexType == SKILL_GROUP) ? "\nThe customers " : "\nThe customer ";
    /* */if (customer->happiness() > 99)    message += "swore it was the best ever.";
    else if (customer->happiness() > 80)    message += "swore they would come back.";
    else if (customer->happiness() > 60)    message += "enjoyed the experience.";
    else if (customer->happiness() > 50)    message += "has had a better experience before."; //added this CRAZY
    else if (customer->happiness() > 40)    message += "thought it was okay."; //added this CRAZY
    else if (customer->happiness() > 30)    message += "didn't enjoy it.";
    else /*                                       */    message += "thought it was crap.";

    // WD: update Fame based on Customer HAPPINESS
    girl->upd_base_stat(STAT_FAME, (customer->happiness() - 1) / 33);

    // The girls STAT_CONSTITUTION and STAT_AGILITY modify how tired she gets
    girl->add_tiredness();

    // if the girl likes sex and the sex type then increase her happiness otherwise decrease it
    if (girl->has_active_trait("Succubus"))
    {
        message += "\nIt seems that she lives for this sort of thing.";//succubus does live for sex lol.. Idk if this will work like i want it to CRAZY
        girl->health(10);//Idk where I should put this really but succubus gain live force or whatever from sex
        girl->happiness(5);
    }
    else if (girl->libido() > 5)
    {
        /* */if (check < 20)    message += "\nThough she had a tough time with it, she was horny and still managed to gain some little enjoyment.";
        else if (check < 40)    message += "\nShe considered it a learning experience and enjoyed it a bit.";
        else if (check < 60)    message += "\nShe enjoyed it a lot and wanted more.";
        else if (check < 80)    message += "\nIt was nothing new for her, but she really does appreciate such work.";//girl->upd_stat(STAT_SANITY, 1);
        else /*           */    message += "\nIt seems that she lives for this sort of thing.";//girl->upd_stat(STAT_SANITY, 2);
        girl->happiness(girl->libido() / 5);
    }
    else
    {
        if (check < 30)
        {
            message += "\nShe wasn't in the mood at all and didn't enjoy being used this way.";
                  girl->upd_base_stat(STAT_HAPPINESS, -2);
            //girl->upd_stat(STAT_SANITY, -1);
        }
        else if (check < 60)
        {
            message += "\nShe wasn't really in the mood.";
                  girl->upd_base_stat(STAT_HAPPINESS, -1);
        }
        else
        {
            message += "\nShe didn't need this right now, but was happy enough to do it for the ";
            message += (customer->m_IsWoman ? "girl." : "guy.");
        }
    }

    // special cases for certain sex types
    switch (SexType)
    {
    case SKILL_ANAL:
    {
        if (z)
        {
            message += "(Z text not done)\n";
            //break;
        }
        if (check <= 20)    // if unexperienced then will get hurt
        {
            if (g_Dice.percent(30)) message += "\nHer inexperience hurt her a little.";
            else /*              */    message += "\nHer inexperience hurt her. It's now quite painful to sit down.";
                  girl->happiness(-3);
                  girl->confidence(-1);
                  girl->spirit(-3);
                  girl->health(-3);
            //girl->upd_stat(STAT_SANITY, -3);
        }
        girl->upd_temp_stat(STAT_LIBIDO, -10, true);
        girl->upd_base_stat(STAT_SPIRIT, -1);
        STDchance += 30;

     //SIN - GIFT DROP
        if (g_Dice.percent(5) && customer->happiness() > 75)
        {
            bool keep = false;
            message += ("\n \nAfterwards he stuffed a toy up " + girlName + "'s cum lubricated ass, telling her it was a 'gift.' ");
            if (girl->has_active_trait("Nymphomaniac"))
            {
                message += "She kept it in while she dressed and tidied up. She'll definitely keep ";
                keep = true;
            }
            else if (g_Dice.percent(girl->dignity())) //higher dig = higher chance
            {
                message += "As soon as he was gone, she pulled out the stinking thing and threw it in the trash.";
            }
            else
            {
                message += "She decided to keep ";
                keep = true;
            }
            if (keep)
            {

                if (g_Dice.percent(66)) girl->add_item(g_Game->inventory_manager().GetItem("Buttplug")), message += "this buttplug.";
                else girl->add_item(g_Game->inventory_manager().GetItem("Anal Beads")), message += "these anal beads.";
            }
        }
    }break;

    case SKILL_BDSM:
    {
        if (z)
        {
            message += "(Z text not done)\n";
            //break;
        }
        if (check <= 30)    // if unexperienced then will get hurt
        {
            if (g_Dice.percent(30)) message += "\nHer inexperience hurt her a little.";
            else /*              */    message += "\nHer inexperience hurt her a little. She's not used to having pain in those places.";
                  girl->upd_base_stat(STAT_HAPPINESS, -2);
                  girl->upd_base_stat(STAT_SPIRIT, -3);
                  girl->upd_base_stat(STAT_CONFIDENCE, -1);
                  girl->upd_base_stat(STAT_HEALTH, -3);
            //girl->upd_stat(STAT_SANITY, -5);
        }
        if (!customer->m_IsWoman)
        {
            contraception = girl->calc_pregnancy(*customer, 0.75);
            STDchance += (contraception ? 2 : 20);
        }

        girl->upd_temp_stat(STAT_LIBIDO, -5, true);
          girl->upd_base_stat(STAT_SPIRIT, -1);

     //SIN - GIFT DROP
        if (g_Dice.percent(5) && customer->happiness() > 75)
        {
            bool keep = false;
            message += "\n \nAs he untied " + girlName + ", he told her she could keep the collar. ";
            if (girl->has_active_trait("Masochist") || girl->has_active_trait("Slut"))
            {
                message += "She agreed. It's exactly what she deserves: a ";
                keep = true;
            }
            else if (g_Dice.percent(girl->dignity())) //higher dig = higher chance
            {
                message += "\nA collar?! As soon as he was gone, she ripped it off and threw it in the trash.";
            }
            else
            {
                message += "She decided to keep this ";
                keep = true;
            }
            if (keep)
            {
                if (g_Dice.percent(66)) girl->add_item(g_Game->inventory_manager().GetItem("Spiked Collar")), message += "spiked collar.";
                else girl->add_item(g_Game->inventory_manager().GetItem("Slut Collar")), message += "'slut' collar.";
            }
        }
    }break;

    case SKILL_NORMALSEX:
    {
        if (z)
        {
            message += "(Z text not done)\n";
            //break;
        }
        if (check < 15)
        {
            if (g_Dice.percent(30)) message += "\nHer inexperience hurt her a little.";
            else /*              */    message += "\nShe's inexperienced and got poked in the eye.";/*Wouldnt this work better in oral? CRAZY*/
                  girl->upd_base_stat(STAT_HAPPINESS, -2);
                  girl->upd_base_stat(STAT_SPIRIT, -3);
                  girl->upd_base_stat(STAT_CONFIDENCE, -1);
                  girl->upd_base_stat(STAT_HEALTH, -3);
            //girl->upd_stat(STAT_SANITY, -2);
        }
        if (girl->has_active_trait("Sterile"))
        {
            contraception = false;                            // none needed
            STDchance += 16;                                // less chance than not using but more chance than using
        }
        //Trait modifications
        else if (girl->has_active_trait("Cum Addict") && girl->m_UseAntiPreg &&
                 g_Dice.percent(girl->libido()) && !g_Dice.percent(girl->intelligence()))
        {
            message += "\n \nShe got over-excited by her desire for cum, and failed to use her anti-preg. ";
            girl->m_UseAntiPreg = false;    // turn off anti
            contraception = girl->calc_pregnancy(*customer);
            STDchance += (contraception ? 4 : 40);
            if (contraception) message += "Luckily she didn't get pregnant.\n";
            else message += "Sure enough, she got pregnant.";
            girl->m_UseAntiPreg = true;        // return to normal (as checked in initial if condition)
        }
        else
        {
            contraception = girl->calc_pregnancy(*customer);
            STDchance += (contraception ? 4 : 40);
        }
        girl->upd_temp_stat(STAT_LIBIDO, -15, true);

     //SIN - GIFT DROP
        if (g_Dice.percent(5) && customer->happiness() > 75)
        {
            bool keep = false;
            message += "\n \nAfterwards he squeezed a toy into " + girlName + "'s cummy cunt, leaving it there as a 'gift.' ";
            if (girl->has_active_trait("Nymphomaniac"))
            {
                message += "While he was getting dressed, she noisily tried out the new ";
                keep = true;
            }
            else if (g_Dice.percent(girl->dignity())) //higher dig = higher chance
            {
                message += "As soon as he was gone, she pulled out the filthy thing and threw it in the trash.";
            }
            else
            {
                message += "She decided to keep ";
                keep = true;
            }
            if (keep)
            {
                if (g_Dice.percent(66)) girl->add_item(g_Game->inventory_manager().GetItem("Dildo")), message += "dildo.";
                else girl->add_item(g_Game->inventory_manager().GetItem("Studded Dildo")), message += "studded dildo.";
            }
        }
    }break;

    case SKILL_ORALSEX:
    {
        if (z)
        {
            message += "(Z text not done)\n";
            //break;
        }
        if (girl->get_skill(SexType) <= 20)    // if unexperienced then will get hurt
        {
            if (girl->has_active_trait("Gag Reflex") || girl->has_active_trait("Strong Gag Reflex"))
            {
                message += "\nHer throat is raw from gagging on the customer's cock. She was nearly sick.";
                          girl->upd_base_stat(STAT_HAPPINESS, -4);
                          girl->upd_base_stat(STAT_SPIRIT, -3);
                          girl->upd_base_stat(STAT_CONFIDENCE, -1);
                          girl->upd_base_stat(STAT_HEALTH, -3);
                //girl->upd_stat(STAT_SANITY, -3);
            }
            else
            {
                if (g_Dice.percent(40)) message += "\nHer inexperience caused her some embarrassment."; // Changed... being new at oral doesn't hurt, but can be embarrasing. --PP
                else /*              */    message += "\nShe's inexperienced and got poked in the eye.";/*CRAZY*/
                          girl->upd_base_stat(STAT_HAPPINESS, -2);
                          girl->upd_base_stat(STAT_SPIRIT, -3);
                          girl->upd_base_stat(STAT_CONFIDENCE, -1);
                //girl->upd_stat(STAT_SANITY, -1);
            }
        }
        STDchance += 10;
        girl->upd_temp_stat(STAT_LIBIDO, -2, true);

     //SIN - GIFT DROP
        if (g_Dice.percent(5) && customer->happiness() > 75)
        {
            message += "\n \nAfterwards he gave " + girlName + " a gift to help her give head in future. She got ";
            if (g_Dice.percent(50)) girl->add_item(g_Game->inventory_manager().GetItem("Oral Sex Candy")), message += "some delicious oral sex candies.";
            else girl->add_item(g_Game->inventory_manager().GetItem("Knee Pads")), message += "some comfortable knee pads.";
        }
    }break;

    case SKILL_TITTYSEX:
    {
        if (z)
        {
            message += "(Z text not done)\n";
            //break;
        }
        if (check <= 20)    // if unexperienced then will get hurt
        {
            message += "\nHer inexperience caused her some embarrassment.";    // Changed... being new at oral doesn't hurt, but can be embarrasing. --PP
                  girl->upd_base_stat(STAT_HAPPINESS, -2);
                  girl->upd_base_stat(STAT_SPIRIT, -3);
                  girl->upd_base_stat(STAT_CONFIDENCE, -1);
            //girl->upd_stat(STAT_SANITY, -1);
        }
        STDchance += 1;
        girl->upd_temp_stat(STAT_LIBIDO, -2, true);
    }break;

    case SKILL_HANDJOB:
    {
        if (z)
        {
            message += "(Z text not done)\n";
            //break;
        }
        if (check <= 20)    // if unexperienced then will get hurt
        {
            message += "\nHer inexperience caused her some embarrassment.";    // Changed... being new at handjob doesn't hurt, but can be embarrasing. --PP
                  girl->upd_base_stat(STAT_HAPPINESS, -2);
                  girl->upd_base_stat(STAT_SPIRIT, -3);
                  girl->upd_base_stat(STAT_CONFIDENCE, -1);
        }
        STDchance += 1;
        girl->upd_temp_stat(STAT_LIBIDO, -1, true);
    }break;

    case SKILL_FOOTJOB:
    {
        if (z)
        {
            message += "(Z text not done)\n";
            //break;
        }
        if (check <= 20)    // if unexperienced then will get hurt
        {
            message += "\nHer inexperience caused her some embarrassment.";    // Changed... being new at footjob doesn't hurt, but can be embarrasing. --PP
                  girl->upd_base_stat(STAT_HAPPINESS, -2);
                  girl->upd_base_stat(STAT_SPIRIT, -3);
                  girl->upd_base_stat(STAT_CONFIDENCE, -1);
        }
        STDchance += 1;
        girl->upd_temp_stat(STAT_LIBIDO, -1, true);
    }break;

    case SKILL_BEASTIALITY:
    {
        if (z)
        {
            message += "(Z text not done)\n";
            //break;
        }
        if (check <= 30)    // if unexperienced then will get hurt
        {
            message += "\nHer inexperience hurt her a little.";
                  girl->upd_base_stat(STAT_HAPPINESS, -2);
                  girl->upd_base_stat(STAT_SPIRIT, -3);
                  girl->upd_base_stat(STAT_CONFIDENCE, -1);
                  girl->upd_base_stat(STAT_HEALTH, -3);
            //girl->upd_stat(STAT_SANITY, -4);
        }
          girl->upd_base_stat(STAT_SPIRIT, -1);    // is pretty degrading
        // mod: added check for number of beasts owned; otherwise, fake beasts could somehow inseminate the girl
        if (g_Game->storage().beasts() > 0)
        {
            contraception = girl->calc_insemination(GetBeast());
            STDchance += (contraception ? 2 : 20);
        }
        girl->upd_temp_stat(STAT_LIBIDO, -10, true);

    //SIN - GIFT DROP
        if (g_Dice.percent(5) && customer->happiness() > 50)
        {
            bool keep = true;
            if (g_Dice.percent(90))
            {
                message += "\n \nAfterward the customer gave " + girlName + " a pet collar as a gift.";
                if (g_Dice.percent(girl->dignity())) //higher dig = higher chance
                {
                    message += "Annoyed, she later threw out this demeaning trash.";
                    keep = false;
                }
                if (keep) girl->add_item(g_Game->inventory_manager().GetItem("Pet Collar"));
            }
            else
            {
                message += "\n \nAfterward the customer gave " + girlName + " some cute Paw-Print Teddy lingerie as a gift.";
                girl->add_item(g_Game->inventory_manager().GetItem("Paw-Print Teddy"));
            }
        }
    }break;

    case SKILL_GROUP:
    {
        if (z)
        {
            message += "(Z text not done)\n";
            //break;
        }
        if (check <= 30)    // if unexperienced then will get hurt
        {
            if (g_Dice.percent(30)) message += "\nHer inexperience hurt her a little.";
            else /*              */    message += "\nShe's inexperienced and got rubbed pretty raw.";
                  girl->upd_base_stat(STAT_HAPPINESS, -2);
                  girl->upd_base_stat(STAT_SPIRIT, -3);
                  girl->upd_base_stat(STAT_CONFIDENCE, -1);
                  girl->upd_base_stat(STAT_HEALTH, -3);
            //girl->upd_stat(STAT_SANITY, -3);
        }
        if (girl->has_active_trait("Sterile"))
        {
            contraception = false;                            // none needed
            STDchance += ((4 + customer->m_Amount) * 4);    // less chance than not using but more chance than using
        }
        //trait variation
        else if (girl->has_active_trait("Cum Addict") && girl->m_UseAntiPreg &&
                 g_Dice.percent(girl->libido()) && !g_Dice.percent(girl->intelligence()))
        {
            message += "\n \nShe got over-excited by her desire for cum, and failed to use her anti-preg. ";
            girl->m_UseAntiPreg = false;    // turn off anti
            contraception = girl->calc_group_pregnancy(*customer, 1.5);
            STDchance += ((4 + customer->m_Amount) * (contraception ? 1 : 10));
            if (contraception) message += "Luckily she didn't get pregnant.\n";
            else message += "Sure enough, she got pregnant.";
            girl->m_UseAntiPreg = true;        // return to normal (as checked in initial if condition)
        }
        else
        {
            //50% bonus to the chance of pregnancy since there's more than one partner involved
            contraception = girl->calc_group_pregnancy(*customer, 1.5);
            STDchance += ((4 + customer->m_Amount) * (contraception ? 1 : 10));
        }
     //GIFT DROP
        if (g_Dice.percent(5) && customer->happiness() > 50)
        {
            if (g_Dice.percent(90))
            {
                message += "\n \nAfter the group had left, " + girlName + " found they had filled her decorative goblet with their cum. ";
                if (girl->has_active_trait("Cum Addict"))
                {
                    message += "She immediately swallowed it all down.";
                }
                else if (g_Dice.percent(girl->dignity())) //higher dig = higher chance
                {
                    message += "Annoyed, she threw the goblet, and its disgusting contents, in the trash.";
                }
                else
                {
                    message += "She decided to keep it.";
                    girl->add_item(g_Game->inventory_manager().GetItem("Goblet of Cum"));
                }
            }
            else if (customer->happiness() > 90)
            {
                girl->add_item(g_Game->inventory_manager().GetItem("Ring of the Hivemind"));
                message += "\n \nThe grateful group were so exhausted they forgot to take their Ring of Hivemind. It's hers now.";
            }
            else
            {
                girl->add_item(g_Game->inventory_manager().GetItem("Herpes Cure"));
                message += "\n \nWorryingly, as she tidied up she found a Herpes Cure dropped under the bed. It's hers now.";
            }
        }
        girl->upd_temp_stat(STAT_LIBIDO, -20, true);
    }break;

    case SKILL_LESBIAN:
    {
        if (z)
        {
            message += "(Z text not done)\n";
            //break;
        }
        if (check <= 20)    // if unexperienced then will get hurt
        {
            message += "\nHer inexperience caused her some embarrassment.";    // Changed... being new at lesbian doesn't hurt, but can be embarrasing. --PP
                  girl->upd_base_stat(STAT_HAPPINESS, -2);
                  girl->upd_base_stat(STAT_SPIRIT, -3);
                  girl->upd_base_stat(STAT_CONFIDENCE, -1);
            //girl->upd_stat(STAT_SANITY, -1);
        }
        STDchance += 5;
        girl->upd_temp_stat(STAT_LIBIDO, -10, true);

     //SIN - GIFT DROP
        if (g_Dice.percent(5) && customer->happiness() > 75)
        {
            bool keep = true;
            if (g_Dice.percent(90))
            {
                message += "\n \nAfterwards the woman let " + girlName + " keep the double-dildo they had shared.";
                if (girl->has_active_trait("Straight"))
                {
                    message += "\nBeing straight, she had no desire to keep it and threw it out later.";
                    keep = false;
                }
                if (keep) girl->add_item(g_Game->inventory_manager().GetItem("Dildo"));
            }
            else
            {
                message += "\n \nThe woman gave " + girlName + " an expensive, illustrated book on lesbian sex.";
                if (girl->has_active_trait("Straight"))
                {
                    message += "\nBeing straight, she found it kinda gross and threw it out.";
                    keep = false;
                }
                if (keep) girl->add_item(g_Game->inventory_manager().GetItem("Manual of Two Roses"));
            }
        }
    }break;

    case SKILL_STRIP:
    default:
    {
        if (z)
        {
            message += "(Z text not done)\n";
            //break;
        }
        if (check <= 20)    // if inexperienced then will get hurt
        {
            message += "\nShe got tangled in her clothes and fell on her face.";
                  girl->upd_base_stat(STAT_HAPPINESS, -2);
                  girl->upd_base_stat(STAT_SPIRIT, -3);
                  girl->upd_base_stat(STAT_CONFIDENCE, -1);
                  girl->upd_base_stat(STAT_HEALTH, -3);
        }
        STDchance += 0;
        girl->upd_temp_stat(STAT_LIBIDO, 0, true);
    }break;
    }    // end switch

    // lose virginity unless it was anal sex -- or lesbian, or Oral also customer is happy no matter what. -PP
    if (is_virgin(*girl))
    {
        bool virgincheck = false;
        int chappy = 0;
        if (SexType == SKILL_ORALSEX || SexType == SKILL_TITTYSEX || SexType == SKILL_HANDJOB || SexType == SKILL_FOOTJOB)
        {
            // virginity does not matter at all
            chappy = 2;    // but lets give a little anyway
        }
        else if (SexType == SKILL_STRIP)
        {
            message += " The customer was excited to have a virgin stripping for them.";
            chappy = 10;
            virgincheck = true;
        }
        else if (SexType == SKILL_BEASTIALITY)
        {
            message += " The customer was excited that she was a virgin as he pushed the beast onto her.";
            chappy = 25;
            virgincheck = true;
        }
        else if (SexType == SKILL_ANAL)
        {
            message += " The customer was overjoyed that she was a virgin, but that was not what he was into.";
            chappy = 50;
        }
        else if (SexType == SKILL_LESBIAN)
        {
            message += " The customer was happy that she had never been with a \"man\" and was still a virgin.";
            chappy = 69;
        }
        else if (SexType == SKILL_GROUP)
        {
            message += " The customers were overjoyed that she was a virgin. Well... the first one was at least.";
            chappy = 80;
            virgincheck = true;
        }
        else if (SexType == SKILL_BDSM)
        {
            message += " The customer was overjoyed that she was a virgin.";
            chappy = 90;
            if (g_Dice.percent(70)) virgincheck = true;
        }
        else if (SexType == SKILL_NORMALSEX)
        {
            message += " The customer was overjoyed that she was a virgin.";
            chappy = 100;
            virgincheck = true;
        }
        if (chappy > 0) customer->happiness(chappy);
        if (girl->is_pregnant()) virgincheck = true;
        if (virgincheck) girl->lose_trait("Virgin");
    }

    //SIN - poor accomodation minus...
    if (g_Dice.percent(3) && girl->m_AccLevel < 2)
    {
        message += "\n \nOn the way out, the customer mentioned " + girlName + "'s \"awful personal hygiene\" to one of your staff.\n";
        message += g_Dice.select_text(
                {"\"She has cum-dreadlocks in her hair!\"", "\"The stink when I took her panties off...\"",
                 "\"Her breath stank of cock.\"", "\"There were stains all over her clothes and bed.\"",
                 "\"Her kiss tasted like horse cum. And do NOT ask how I know what that tastes like...\""
                });
        message += "\nIt's not really her fault: the room/closet you have her living in doesn't even have a sink.";
        customer->happiness(-15);
    }

    // Now calculate other skill increases
    int skillgain = 3;    int exp = 4;
    if(first) {
        // more gains for first time per night, and  quick learner only has an effect once
        skillgain = 4;
        if (girl->has_active_trait("Quick Learner"))        { skillgain += 1; exp += 1; }
        else if (girl->has_active_trait("Slow Learner"))    { skillgain -= 1; exp -= 1; }
    }
    if (SexType == SKILL_GROUP)
    {
        girl->upd_skill(SKILL_ANAL, max(0, g_Dice % skillgain + 1));
        girl->upd_skill(SKILL_BDSM, max(0, g_Dice % skillgain - 1));
        girl->upd_skill(SKILL_NORMALSEX, max(0, g_Dice % skillgain + 1));
        girl->upd_skill(SKILL_BEASTIALITY, max(0, g_Dice % skillgain - 3));
        girl->upd_skill(SKILL_GROUP, max(2, g_Dice % skillgain + 2));
        girl->upd_skill(SKILL_LESBIAN, max(0, g_Dice % skillgain - 2));
        girl->upd_skill(SKILL_STRIP, max(0, g_Dice % skillgain - 2));
        girl->upd_skill(SKILL_ORALSEX, max(0, g_Dice % skillgain + 0));
        girl->upd_skill(SKILL_TITTYSEX, max(0, g_Dice % skillgain - 1));
        girl->upd_skill(SKILL_HANDJOB, max(0, g_Dice % skillgain + 0));
        girl->upd_skill(SKILL_FOOTJOB, max(0, g_Dice % skillgain - 1));
    }
    else    // single sex act focus gets more base gain
    {
        girl->upd_skill(SexType, g_Dice.closed_uniform(1, skillgain + 2));
    }
    girl->upd_skill(SKILL_SERVICE, max(0, g_Dice % skillgain - 1));    // everyone gets some service gain
    if(first) {
        girl->upd_base_stat(STAT_EXP, g_Dice.closed_uniform(1, exp * 3));
    } else {
        // reduced xp gain
        girl->upd_base_stat(STAT_EXP, g_Dice.closed_uniform(1, exp * 2));
    }

    int enjoy = 1;
    if (girl->has_active_trait("Nymphomaniac"))
    {
        girl->upd_temp_stat(STAT_LIBIDO, 15);        // she just had sex and she wants more
        switch (SexType)
        {
        case SKILL_GROUP:            enjoy += 3; break;
        case SKILL_ANAL:            enjoy += 2; break;
        case SKILL_NORMALSEX:        enjoy += 2; break;
        case SKILL_BDSM:
        case SKILL_BEASTIALITY:
        case SKILL_LESBIAN:            enjoy += 1; break;
            // Nymphomaniac would rather have something inside her so if she can't, she does not enjoy it as much
        case SKILL_STRIP:            enjoy -= 2; break;
        case SKILL_TITTYSEX:        enjoy -= 1; break;
        case SKILL_HANDJOB:            enjoy -= 1; break;
        case SKILL_FOOTJOB:            enjoy -= 1; break;
        case SKILL_ORALSEX:
        default:
            break;
        }
    }
    if (girl->has_active_trait("Lesbian"))
    {
        switch (SexType)
        {
        case SKILL_LESBIAN:            enjoy += 3; break;
        case SKILL_STRIP:            enjoy += 1; break;
            // Lesbian would rather not have sex with a male
        case SKILL_NORMALSEX:        enjoy -= 3; break;
        case SKILL_TITTYSEX:
        case SKILL_ORALSEX:            enjoy -= 1; break;
        case SKILL_HANDJOB:
        case SKILL_FOOTJOB:            enjoy -= 1; break;
        case SKILL_ANAL:
        case SKILL_GROUP:            enjoy -= 2; break;
        default:
            break;
        }
    }
    if (girl->has_active_trait("Straight"))
    {
        switch (SexType)
        {
        case SKILL_NORMALSEX:        enjoy += 2; break;
        case SKILL_LESBIAN:            enjoy -= 1; break;
        default:
            break;
        }
    }
    if (girl->has_active_trait("Cum Addict"))
    {
        switch (SexType)
        {
        case SKILL_ORALSEX:            enjoy += 5; break;
        case SKILL_GROUP:            enjoy += 5; break;
        default:
            break;
        }
    }

    if (girl->happiness() > 50)            enjoy += 2;
    else if (girl->happiness() <= 5)    enjoy -= 2;

    int health = girl->health();
    /* */if (health > 95)    { STDchance = 1; }
    else if (health > 30)    { STDchance /= (health - 25) / 5; }
    else /*if (health < 30)*/    { STDchance *= (35 - health) / 10; } //SIN: check not required
    if (STDchance < 0.1)    STDchance = 0.1;

    if (girl->has_active_trait("AIDS") && customer->gain_trait("AIDS", STDchance))
    {
        girl->AddMessage("${name} gave the customer AIDS! They are not happy about this.", IMGTYPE_PROFILE, EVENT_DANGER);
        customer->happiness(-100);
        enjoy -= 3;
    }
    else if (customer->has_active_trait("AIDS") && girl->gain_trait("AIDS", STDchance))
    {
        girl->AddMessage("${name} has caught the disease AIDS! She will likely die, but a rare cure can sometimes be found in the shop.", IMGTYPE_PROFILE, EVENT_DANGER);
        girl->happiness(-50);
        enjoy -= 30;
    }
    if (girl->has_active_trait("Chlamydia") && customer->gain_trait("Chlamydia", STDchance))
    {
        girl->AddMessage("${name} gave the customer Chlamydia! They are not happy about this.", IMGTYPE_PROFILE, EVENT_DANGER);
        customer->happiness(-40);
        enjoy -= 3;
    }
    else if (customer->has_active_trait("Chlamydia") && girl->gain_trait("Chlamydia", STDchance))
    {
        girl->AddMessage("${name} has caught the disease Chlamydia! A cure can sometimes be found in the shop.", IMGTYPE_PROFILE, EVENT_DANGER);
        girl->happiness(-30);
        enjoy -= 30;
    }

    if (girl->has_active_trait("Syphilis") && customer->gain_trait("Syphilis", STDchance))
    {
        girl->AddMessage("${name} gave the customer Syphilis! They are not happy about this.", IMGTYPE_PROFILE, EVENT_DANGER);
        customer->happiness(-50);
        enjoy -= 3;
    }
    else if (customer->has_active_trait("Syphilis") && girl->gain_trait("Syphilis", STDchance))
    {
        girl->AddMessage("${name} has caught the disease Syphilis! This can be deadly, but a cure can sometimes be found in the shop.", IMGTYPE_PROFILE, EVENT_DANGER);
        girl->happiness(-30);
        enjoy -= 30;
    }

    if (girl->has_active_trait("Herpes") && customer->gain_trait("Herpes", STDchance))
    {
        girl->AddMessage("${name} gave the customer Herpes! They are not happy about this.", IMGTYPE_PROFILE, EVENT_DANGER);
        customer->happiness(-30);
        enjoy -= 3;
    }
    else if (customer->has_active_trait("Herpes") && girl->gain_trait("Herpes", STDchance))
    {
        girl->AddMessage("${name} has caught the disease Herpes! A cure can sometimes be found in the shop.", IMGTYPE_PROFILE, EVENT_DANGER);
        girl->happiness(-30);
        enjoy -= 30;
    }

    girl->upd_Enjoyment(ACTION_SEX, enjoy);

    g_LogFile.log(ELogLevel::DEBUG, "STD Debug ::: Sex Type : ", get_skill_name((SKILLS)SexType)
            , " :: Contraception: ", (contraception ? "True" : "False")
            , " :: Health: ", health
            , " :: STD Chance: ", STDchance);
}

// ----- Combat

bool cGirls::GirlInjured(sGirl& girl, unsigned int unModifier, std::function<void(std::string)> handler)
{  // modifier: 5 = 5% chance, 10 = 10% chance
    /*
    *    WD    Injury was only possible if girl is pregnant or
    *        hasn't got the required traits.
    *
    *        Now check for injury first
    *        Use usigned int so can't pass negative chance
    */
    //    bool injured = false;
    string message;
    int nMod = static_cast<int>(unModifier);

    if(!handler)
        handler = [&](std::string message){
            girl.AddMessage(std::move(message), IMGTYPE_PROFILE, EVENT_WARNING);
    };

    // Sanity check, Can't get injured
    if (girl.has_active_trait("Incorporeal")) return false;
    if (girl.has_active_trait("Fragile")) nMod += nMod;
    if (girl.has_active_trait("Tough")) nMod /= 2;

    // Did the girl get injured
    if (!g_Dice.percent(nMod))
        return false;
    /*
    *    INJURY PROCESSING
    *    Only injured girls continue past here
    */

    // getting hurt badly could lead to scars
    if (
            g_Dice.percent(nMod * 2) &&
            !girl.has_active_trait("Small Scars") &&
            !girl.has_active_trait("Cool Scars") &&
            !girl.has_active_trait("Horrific Scars")
        )
    {
        //injured = true;
        int chance = g_Dice % 6;
        if (chance == 0)
        {
            girl.gain_trait("Horrific Scars");
            if (g_Dice.percent(50))
            {
                handler("She was horribly injured, and now is now covered with Horrific Scars.");
            }
            else
            {
                handler("She was badly injured, and now has to deal with Horrific Scars.");
            }
        }
        else if (chance <= 2)
        {
            girl.gain_trait("Small Scars");
            handler("She was injured and now has a couple of Small Scars.");
        }
        else
        {
            girl.gain_trait("Cool Scars");
            handler("She was injured and scarred. As scars go however, at least they are pretty Cool Scars.");
        }
    }

    // in rare cases, she might even lose an eye
    if (
            g_Dice.percent((nMod / 2)) &&
            !girl.has_active_trait("One Eye") &&
            !girl.has_active_trait("Eye Patch")
        )
    {
        //injured = true;
        int chance = g_Dice % 3;
        if (chance == 0)
        {
            girl.gain_trait("One Eye");
            handler("Oh, no! She was badly injured, and now only has One Eye!");
        }
        else
        {
            girl.gain_trait("Eye Patch");
            handler("She was injured and lost an eye, but at least she has a cool Eye Patch to wear.");
        }
    }

    if (girl.has_active_trait("Tough"))
    {
        if (girl.gain_trait("Tough", nMod))
        {
            handler("Due to ${name}'s injuries her body has become less Tough.\n");
        }
    } else
    // or become fragile
    if (girl.gain_trait("Fragile", nMod / 2))
    {
        handler("Due to ${name}'s injuries her body has become Fragile.\n");
    }
    

    // and if pregnant, she might lose the baby; I'll assume inseminations can't be aborted so easily
    if (girl.carrying_human() && g_Dice.percent((nMod * 2)))
    {  // unintended abortion time
        //injured = true;
        girl.m_ChildrenCount[CHILD08_MISCARRIAGES]++;
        girl.clear_pregnancy();
        girl.happiness(-20);
        girl.spirit(-5);
        handler("Her unborn child has been lost due to the injuries she sustained, leaving her quite distraught.");
    }
    if (girl.carrying_monster() && g_Dice.percent((nMod)))
    {  // unintended abortion time
        //injured = true;
        girl.m_ChildrenCount[CHILD08_MISCARRIAGES]++;
        girl.clear_pregnancy();
        girl.happiness(-10);
        girl.spirit(-5);
        handler("The creature growing inside her has been lost due to the injuries she sustained, leaving her distraught.");
    }

    // Lose between 5 - 14 hp
    girl.health(-5 - g_Dice % 10);

    return true;
}

// ----- Update

void cGirls::UpdateEnjoymentMod(sGirl& girl, int whatSheEnjoys, int amount)
{
    girl.m_EnjoymentMods[whatSheEnjoys] += amount;
}

// Normalise to zero by 30%
void cGirls::updateTempEnjoyment(sGirl& girl)
{
    // Sanity check. Abort on dead girl
    if (girl.is_dead()) return;

    for (int i = 0; i < NUM_ACTIONTYPES; i++)
    {
        if (girl.m_EnjoymentTemps[i] != 0)
        {                                            // normalize towards 0 by 30% each week
            int newEnjoy = (int)(float(girl.m_EnjoymentTemps[i]) * 0.7);
            if (newEnjoy != girl.m_EnjoymentTemps[i])    girl.m_EnjoymentTemps[i] = newEnjoy;
            else
            {                                        // if 30% did nothing, go with 1 instead
                /* */if (girl.m_EnjoymentTemps[i] > 0)    girl.m_EnjoymentTemps[i]--;
                else if (girl.m_EnjoymentTemps[i] < 0)    girl.m_EnjoymentTemps[i]++;
            }
        }
    }
}


// Increment birthday counter and update Girl's age if needed
void cGirls::updateGirlAge(sGirl& girl, bool inc_inService)
{
    // Sanity check. Abort on dead girl
    if (girl.is_dead()) return;
    if (inc_inService)
    {
        girl.m_WeeksPast++;
        girl.m_BDay++;
    }
    if (girl.m_BDay >= 52)                    // Today is girl's birthday
    {
        girl.m_BDay = 0;
        girl.age(1);
        if (girl.age() > 20) girl.lose_trait("Lolita");
        if (girl.age() >= 50)
        {
            girl.beauty(-(g_Dice % 3 + 1));
        }
    }
}

// Update health and other things for STDs
void cGirls::updateSTD(sGirl& girl)
{
    // Sanity check. Abort on dead girl
    if (girl.is_dead()) return;

    bool matron = girl_has_matron(girl, SHIFT_DAY);

    int Dhea = 0, Dhap = 0, Dtir = 0, Dint = 0, Dcha = 0;
    if (girl.has_active_trait("AIDS"))
    {
        const char* cureitem = "AIDS Cure";
        if (matron)
        {
            stringstream cure;
            cure << girl.FullName() << " was given an " << cureitem << " from the brothel's stock to cure the disease.";
            g_Game->player().AutomaticFoodItemUse(girl, cureitem, cure.str());
        }
        else
        {
            Dhea += g_Dice % 15 + 5;
            Dhap += g_Dice % 5 + 5;
            Dtir += g_Dice % 2 + 1;
        }
    }
    if (girl.has_active_trait("Herpes"))
    {
        const char* cureitem = "Herpes Cure";
        if (matron)
        {
            stringstream cure;
            cure << girl.FullName() << " was given a " << cureitem << " from the brothel's stock to cure the disease.";
            g_Game->player().AutomaticFoodItemUse(girl, cureitem, cure.str());
        }
        else
        {
            Dhea += max(0, g_Dice % 4 - 2);
            Dhap += max(1, g_Dice % 5 - 2);
            Dcha += max(0, g_Dice % 4 - 1);
        }
    }
    if (girl.has_active_trait("Chlamydia"))
    {
        const char* cureitem = "Chlamydia Cure";
        if (matron)
        {
            stringstream cure;
            cure << girl.FullName() << " was given a " << cureitem << " from the brothel's stock to cure the disease.";
            g_Game->player().AutomaticFoodItemUse(girl, cureitem, cure.str());
        }
        else
        {
            Dhea += g_Dice % 3 + 1;
            Dhap += g_Dice % 3 + 1;
            Dtir += g_Dice % 2 + 1;
        }
    }
    if (girl.has_active_trait("Syphilis"))
    {
        const char* cureitem = "Syphilis Cure";
        if (matron)
        {
            stringstream cure;
            cure << girl.FullName() << " was given a " << cureitem << " from the brothel's stock to cure the disease.";
            g_Game->player().AutomaticFoodItemUse(girl, cureitem, cure.str());
        }
        else
        {
            Dhea += g_Dice % 10 + 5;
            Dhap += g_Dice % 5 + 5;
            Dint += max(0, g_Dice % 4 - 2);
        }
    }

    girl.health(-Dhea);
    girl.happiness(-Dhap);
    girl.tiredness(Dtir);
    girl.charisma(-Dcha);
    girl.intelligence(-Dint);



    if (girl.is_dead())
    {
        string msg = "${name} has died from STDs.";
        girl.AddMessage(msg, IMGTYPE_DEATH, EVENT_DANGER);
        g_Game->push_message(msg, COLOR_RED);
    }
}

// Stat update code that is to be run every turn
void cGirls::updateGirlTurnStats(sGirl& girl)
{
    if (girl.is_dead()) return;        // Sanity check. Abort on dead girl

    // TIREDNESS Really tired girls get unhappy fast
    int bonus = girl.tiredness() - 90;
    int b = 0;
    if (bonus > 0)                            // bonus is 1-10
    {
        girl.obedience(-1);                // Base loss for being tired
        girl.pclove(-1);
        b = bonus / 3 + 1;
        girl.happiness(-b);                // 1-4
        if (girl.health() - bonus < 10)    // Don't kill the girl from tiredness
        {
            b = bonus / 2 + 1;                // halve the damage
            girl.health(-b);                // Girl will hate player more if badly hurt from being tired
            girl.pclove(-2);
        }
        else girl.health(-bonus);            // Really tired girls lose more health
    }

    // HEALTH hurt girls get tired fast
    bonus = 40 - girl.health();
    if (bonus > 0)                            // bonus is 1-40
    {
        // Base loss for being hurt
        girl.pclove(-2);
        girl.happiness(-1);

        bonus = bonus / 8 + 1;                // bonus vs health values 1: 33-39, 2: 25-32, 3: 17-24, 4: 09-16 5: 01-08
        girl.tiredness(bonus);
    }

    // LOVE love is updated only if happiness is >= 100 or < 50
    if (girl.happiness() >= 100)
    {
        girl.pclove(2);                    // Happy girls love player more
        girl.obedience(g_Dice%2);            // `J` added
    }
    if (!girl.is_slave() && girl.happiness() < 50)
    {
        girl.pclove(-2);                    // Unhappy FREE girls love player less
        girl.obedience(-(g_Dice % 2));        // `J` added
    }
}

// ----- Stream operators

/*
* While I'm here, let's defined the stream operator for the girls,
* random and otherwise.
*
* This looks big and complex, but it's just printing out the random
* girl data from the load. It's so I can say cout << *rgirl << endl;
* and get a something sensible printed
*/

ostream& operator<<(ostream& os, sGirl &g)
{
    os << g.FullName() << endl;
    os << g.m_Desc << endl;
    os << endl;

    for (auto stat: StatsRange)
    {
        os.width(20);
        os.flags(ios::left);
        os << get_stat_name(stat) << "\t: " << g.get_base_stat(stat) << endl;
    }
    os << endl;

    for (auto skill: SkillsRange)
    {
        os.width(20);
        os.flags(ios::left);
        os << get_skill_name(skill) << "\t: " << g.get_base_skill(skill) << endl;
    }
    os << endl;

    os << endl;
    return os;
}

// Crazy found some old code to allow Canonical_Daughters
sGirl* cGirls::find_girl_by_name(const std::string& name)
{
    return m_Girls->get_first_girl(
            [&](const sGirl& girl){ return girl.m_Name == name; });
}

void cGirls::UncontrolledPregnancies()
{
    /*string summary;
    for(auto current : m_Girls)
    {
        handle_children(*current, summary, false);
    }*/
}

string cGirls::GetHoroscopeName(int month, int day)
{
    // There are 2 types of of horoscopes, 1: Tropical (Western) and 2: Sideral (Hindu)
    int ZodiacType = g_Game->settings().get_integer(settings::USER_HOROSCOPE);

    if (ZodiacType == 2)    //    Sideral
    {
        switch (month)
        {
        case 1:        if (day <= 14) return "Sagittarius";    else return "Capricorn";
        case 2:        if (day <= 14) return "Capricorn";        else return "Aquarius";
        case 3:        if (day <= 14) return "Aquarius";        else return "Pisces";
        case 4:        if (day <= 14) return "Pisces";            else return "Aries";
        case 5:        if (day <= 15) return "Aries";            else return "Taurus";
        case 6:        if (day <= 15) return "Taurus";            else return "Gemini";
        case 7:        if (day <= 15) return "Gemini";            else return "Cancer";
        case 8:        if (day <= 15) return "Cancer";            else return "Leo";
        case 9:        if (day <= 15) return "Leo";            else return "Virgo";
        case 10:    if (day <= 15) return "Virgo";            else return "Libra";
        case 11:    if (day <= 15) return "Libra";            else return "Scorpio";
        case 12:    if (day <= 15) return "Scorpio";        else return "Sagittarius";
        default:    return "";    break;
        }
    }
    else    //    Tropical
    {
        switch (month)
        {
        case 1:        if (day <= 19) return "Capricorn";        else return "Aquarius";
        case 2:        if (day <= 18) return "Aquarius";        else return "Pisces";
        case 3:        if (day <= 20) return "Pisces";            else return "Aries";
        case 4:        if (day <= 19) return "Aries";            else return "Taurus";
        case 5:        if (day <= 20) return "Taurus";            else return "Gemini";
        case 6:        if (day <= 20) return "Gemini";            else return "Cancer";
        case 7:        if (day <= 22) return "Cancer";            else return "Leo";
        case 8:        if (day <= 22) return "Leo";            else return "Virgo";
        case 9:        if (day <= 22) return "Virgo";            else return "Libra";
        case 10:    if (day <= 22) return "Libra";            else return "Scorpio";
        case 11:    if (day <= 21) return "Scorpio";        else return "Sagittarius";
        case 12:    if (day <= 21) return "Sagittarius";    else return "Capricorn";
        default:    return "";    break;
        }
    }

    return "";
}

bool cGirls::girl_has_matron(const sGirl& girl, int shift)
{
    if(girl.m_Building) {
        return girl.m_Building->matron_on_shift(shift);
    }
    return false;
}

string cGirls::Accommodation(int acc)
{
    static std::array<std::string, 10> accomodations = {
            "Bare Bones", "Very Poor", "Poor", "Adequate", "Comfortable", "Nice", "Good", "Great", "Wonderful",
            "High Class"
    };
    return accomodations.at(acc);
}

// The accommodation level the girl expects/demands
int cGirls::PreferredAccom(const sGirl& girl)
{
    double preferredaccom = (girl.is_slave() ? 1.0 : 2.5);
    preferredaccom += girl.level() * (girl.is_slave() ? 0.1 : 0.3);
    if (girl.is_pregnant()) preferredaccom += 1.5;

    preferredaccom += girl.get_trait_modifier("pref-accommodation");

    // TODO I think iteration would be more efficient here!
    if (!girl.inventory().all_items().empty())    // only bother checking items if the girl has at least 1
    {
        if (girl.has_item("Chrono Bed"))                        preferredaccom -= 2.0;    // She gets a great night sleep so she is happier when she wakes up
        else if (girl.has_item("Rejuvenation Bed"))            preferredaccom -= 1.0;    // She gets a good night sleep so she is happier when she wakes up
        if (girl.has_item("150 Piece Drum Kit"))                preferredaccom += 0.5;    // Though she may annoy her neighbors and it takes a lot of space, it it fun
        if (girl.has_item("Android, Assistance"))            preferredaccom -= 0.5;    // This little guy cleans up for her
        if (girl.has_item("Anger Management Tapes"))            preferredaccom -= 0.1;    // When she listens to these it takes her mind off other things
        if (girl.has_item("Appreciation Trophy"))            preferredaccom -= 0.1;    // Something nice to look at
        if (girl.has_item("Art Easel"))                        preferredaccom -= 1.0;    // She can make her room nicer by herself.
        if (girl.has_item("Black Cat"))                        preferredaccom -= 0.3;    // Small and soft, it mostly cares for itself
        if (girl.has_item("Cat"))                            preferredaccom -= 0.3;    // Small and soft, it mostly cares for itself
        if (girl.has_item("Claptrap"))                        preferredaccom -= 0.1;    // An annoying little guy but he does help a little
        if (girl.has_item("Computer"))                        preferredaccom -= 1.5;    // Something to do but it takes up a little room
        if (girl.has_item("Death Bear"))                        preferredaccom += 2.0;    // Having a large bear living with her she needs a little more room.
        if (girl.has_item("Deathtrap"))                        preferredaccom += 1.0;    // Having a large robot guarding her her she needs a little more room.
        if (girl.has_item("Free Weights"))                    preferredaccom += 0.2;    // She may like the workout but it takes up a lot of room
        if (girl.has_item("Guard Dog"))                        preferredaccom += 0.2;    // Though she loves having a pet, a large dog takes up some room
        if (girl.has_item("Happy Orb"))                        preferredaccom -= 0.5;    // She has happy dreams
        if (girl.has_item("Relaxation Orb"))                    preferredaccom -= 0.5;    // She can relax anywhere
        if (girl.has_item("Library Card"))                    preferredaccom -= 0.5;    // She has somewhere else to go and she can bring books back, they keep her mind off other things
        if (girl.has_item("Lovers Orb"))                        preferredaccom -= 0.5;    // She really enjoys her dreams
        if (girl.has_item("Nightmare Orb"))                    preferredaccom += 0.2;    // She does not sleep well
        if (girl.has_item("Pet Spider"))                        preferredaccom -= 0.1;    // A little spider, she may be afraid of it but it takes her mind off her room
        if (girl.has_item("Room Decorations"))                preferredaccom -= 0.5;    // They make her like her room more.
        if (girl.has_item("Safe by Marcus"))                    preferredaccom -= 0.3;    // Somewhere to keep her stuff where ske knows no one can get to it.
        if (girl.has_item("Smarty Pants"))                    preferredaccom -= 0.2;    // A little stuffed animal to hug and squeeze
        if (girl.has_item("Stick Hockey Game"))                preferredaccom += 0.3;    // While fun, it takes a lot of room to not break things
        if (girl.has_item("Stripper Pole"))                    preferredaccom += 0.1;    // She may like the workout but it takes up a lot of room
        if (girl.has_item("Television Set"))                    preferredaccom -= 2.0;    // When she stares at this, she doesn't notice anything else
        if (girl.has_item("The Realm of Darthon"))            preferredaccom -= 0.1;    // She and her friends can have fun together but they need some space to play it
        if (girl.has_item("Weekly Social Therapy Session"))    preferredaccom -= 0.1;    // She has somewhere to go and get her troubles off her chest.
    }

    if (preferredaccom <= 0.0) return 0;
    if (preferredaccom >= 9.0) return 9;
    return (int)preferredaccom;
}

// `J` the girl will check the customer for diseases before continuing.
bool cGirls::detect_disease_in_customer(IBuilding * brothel, sGirl& girl, sCustomer * Cust, double mod)
{
    stringstream ss;
    if (g_Dice.percent(0.1))    // 0.001 chance of false positive
    {
        ss << girl.FullName() << " thought she detected that her customer had a disease and refused to allow them to touch her just to be safe.";
        g_Game->push_message(ss.str(), COLOR_RED);
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
        return true;
    }
    // if the customer is clean, then it will return false
    if (!has_disease(*Cust)) return false;
    // 10% chance to miss it
    if (g_Dice.percent(10))    return false;

    double detectdisease = 1.0;                                                // base 1% chance
    detectdisease += mod;                                                    // add mod
    detectdisease += girl.medicine() / 2.0;                                // +50 medicine
    detectdisease += girl.intelligence() / 5.0;                            // +20 intelligence
    detectdisease += girl.magic() / 5.0;                                    // +20 magic
    detectdisease -= girl.libido() / 2.0;                                    // -50 libido

    if (has_disease(girl))                        detectdisease += 20;    // has it so know what to look for
    detectdisease += girl.get_trait_modifier("detect-disease");

    const char* found_disease = nullptr;
    // these need better texts
    if (Cust->has_active_trait("AIDS") && g_Dice.percent(min(90.0, detectdisease*0.5)))    // harder to detect
    {
        found_disease = "AIDS";
    } else if (Cust->has_active_trait("Syphilis") && g_Dice.percent(detectdisease*0.8))    // harder to detect
    {
        found_disease = "Syphilis";
    } else if (Cust->has_active_trait("Chlamydia") && g_Dice.percent(detectdisease)) {
        found_disease = "Chlamydia";
    } else if (Cust->has_active_trait("Herpes") && g_Dice.percent(detectdisease)) {
        found_disease = "Herpes";
    }

    if(found_disease) {
        ss << girl.FullName() << " detected that her customer has " << found_disease <<
            " and refused to allow them to touch her.";
        g_Game->push_message(ss.str(), COLOR_RED);
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
        brothel->m_RejectCustomersDisease++;
    }

    return found_disease;
}

string cGirls::catacombs_look_for(int girls, int items, int beast)
{
    stringstream ss;
    ss << "You tell them to ";


    // over 99%
    if (girls >= 99 || items >= 99 || beast >= 99)
    {
        ss << "only bring back ";
        if (girls >= 99)    ss << "girls";
        if (items >= 99)    ss << "items";
        if (beast >= 99)    ss << "beasts";
    }

    // over 80%
    else if (girls >= 80 || items >= 80 || beast >= 80)
    {
        ss << "mostly try to only bring back ";
        if (girls >= 80)
        {
            ss << "girls but bring back ";
            /* */if (items < 1) ss << "any beasts";
            else if (beast < 1) ss << "any items";
            else ss << "anything else";
        }
        else if (items >= 80)
        {
            ss << "items but bring back ";
            /* */if (girls < 1) ss << "any beasts";
            else if (beast < 1) ss << "any girls";
            else ss << "anything else";
        }
        else if (beast >= 80)
        {
            ss << "beasts but bring back ";
            /* */if (items < 1) ss << "any girls";
            else if (girls < 1) ss << "any items";
            else ss << "anything else";
        }
        ss << " they find as well";
    }

    // over 50%
    else if (girls >= 50 || items >= 50 || beast >= 50)
    {
        ss << "bring back any ";
        if (girls >= 50)
        {
            ss << "girls and ";
            /* */if (beast > 40) ss << "beasts";
            else if (items > 40) ss << "items";
            else ss << "anything else they find";
        }
        else if (items >= 50)
        {
            ss << "items and ";
            /* */if (beast > 40)    ss << "beasts";
            else if (girls > 40)    ss << "girls";
            else ss << "anything else they find";
        }
        else if (beast >= 50)
        {
            ss << "beasts and ";
            /* */if (girls > 40) ss << "girls";
            else if (items > 40) ss << "items";
            else ss << "anything else they find";
        }
    }
    else ss << "bring back anything they can find";

    //else  ss << "have fun. (error code::  CLF01|" << girls << "|" << items << "|" << beast << "  ::  Please report it to pinkpetal.org so it can be fixed)";
    ss << ".\n";
    return ss.str();
}

// `J` for use with beast jobs where the girl has sex with a beast
sCustomer cGirls::GetBeast()
{
    sCustomer beast;
    beast.m_Amount = 1;
    beast.m_IsWoman = false;
    // get their stats generated
    for(auto stat: StatsRange)  beast.set_stat(stat, g_Dice % 100);
    for(auto skill : SkillsRange)  beast.set_skill_direct(skill, g_Dice % 10);

    beast.set_skill_direct(SKILL_BEASTIALITY, 40 + g_Dice % 61);
    beast.set_skill_direct(SKILL_COMBAT, 40 + g_Dice % 61);
    beast.set_skill_direct(SKILL_FARMING, g_Dice % 20);
    beast.set_skill_direct(SKILL_ANIMALHANDLING, 40 + g_Dice % 61);
    beast.set_stat(STAT_INTELLIGENCE, g_Dice % 30);
    beast.set_stat(STAT_LEVEL, 0);
    beast.set_stat(STAT_AGE, g_Dice % 20);
    beast.set_stat(STAT_STRENGTH, 20+g_Dice % 81);

    beast.m_SexPref = beast.m_SexPrefB = SKILL_BEASTIALITY;

    beast.gain_trait("AIDS", 0.5);
    beast.gain_trait("Chlamydia", 1.0);
    beast.gain_trait("Syphilis", 1.5);
    beast.gain_trait("Herpes", 2.5);
    beast.m_Money = 0;

    return beast;
}

bool do_take_gold(sGirl& girl, string &message)    // returns TRUE if the girl won
{
    const int GIRL_LOSES = false;
    const int GIRL_WINS = true;
    bool girl_win_flag = GIRL_WINS;

    // she thinks about escape
    auto result = AttemptEscape(girl);
    if (result == EGirlEscapeAttemptResult::STOPPED_BY_GOONS)
    {        // put her in the dungeon
        message += "She puts up a fight but your goons manage to subdue her and you take her gold anyway.";
        return girl_win_flag;
    } else if (result == EGirlEscapeAttemptResult::SUBMITS ) {
        message += "She quietly allows you to take her gold (" + std::to_string(girl.m_Money) + ").";
        return GIRL_LOSES;    // no fight -> girl lose
    } else if(result == EGirlEscapeAttemptResult::STOPPED_BY_PLAYER) {
        /*
        *    from here on down, the girl won against the goons
        */
        // TODO need to know whether there was a GANG fight
        message += "She puts up a fight and your goons cannot stop her, ";
        message += "but you defeat her yourself and take her gold.";
        return false;    // girl did not win, after all
    } else {
        /*
        *    Looks like she won: put her out of the brothel
        *    and post her as a runaway
        */
        message += "She puts up a fight and your goons cannot stop her. "
                   "After defeating you as well she escapes to the outside.\n";


        girl.run_away();

        stringstream smess;
        smess << girl.FullName() << " has run away";
        g_Game->push_message(smess.str(), 1);
        return true;    // the girl still won
    }
}

void cGirls::TakeGold(sGirl& girl) {
    string message;
    bool girl_win = do_take_gold(girl, message);
    /*
    *    if the girl won, then we're pretty much sorted
    *    display the message and return
    */
    if (girl_win)
    {
        g_Game->push_message(message, 0);
        return;
    }
    /*
    *    so the girl lost: take away her money now
    */
    g_Game->gold().misc_credit(girl.m_Money);
    girl.m_Money = 0;
    /*
    *    she isn't going to like this much
    *    but it's not so bad for slaves
    */
    if (girl.is_slave())
    {
        girl.confidence(-1);
        girl.obedience(5);
        girl.spirit(-2);
        girl.pclove(-10);
        girl.pcfear(5);
        girl.happiness(-20);
    }
    else
    {
        girl.confidence(-5);
        girl.obedience(5);
        girl.spirit(-10);
        girl.pclove(-60);
        girl.pcfear(10);
        girl.happiness(-50);
    }

    g_Game->push_message(message, 0);
}

std::shared_ptr<sGirl> cGirls::GetDaughterByName(const string& name, bool player_dad) {
    auto current = find_girl_by_name(name);
    // did we get a girl?
    if (current)    return TakeGirl(current);        // yes, we did!

    //    OK, we need to search for a random girl
    return m_RandomGirls.spawn(player_dad ? SpawnReason::PLAYER_DAUGHTER : SpawnReason::BIRTH, 18, name);
}

std::shared_ptr<sGirl> cGirls::CreateDaughter(sGirl& mom, bool player_dad) {
    std::shared_ptr<sGirl> sprog = nullptr;
    bool slave = mom.is_slave();
    bool non_human = !mom.is_human();

    /*
     *    Check canonical daughters
     */
    while (!mom.m_Canonical_Daughters.empty()) {
        int index = g_Dice.random(mom.m_Canonical_Daughters.size());
        string name = mom.m_Canonical_Daughters[index];

        sprog = GetDaughterByName(name, player_dad);
        mom.m_Canonical_Daughters.erase(mom.m_Canonical_Daughters.begin() + index);

        if(sprog)
            return sprog;
    }

    // If the player is the father, check that shortlist
    if(player_dad && GetNumYourDaughterGirls() > 0)                // this should check all your daughter girls that apply
    {
        sprog = GetUniqueYourDaughterGirl(non_human);                        // first try to get the same human/nonhuman as mother
        if (!sprog && non_human)
            sprog = GetUniqueYourDaughterGirl(false);    // next, if mom is nonhuman, try to get a human daughter
    }

    /*
    *    Did not find a girl, so back to the random girls
    */
    if(player_dad) {
        return CreateRandomGirl(SpawnReason::PLAYER_DAUGHTER, 18);
    }
    return CreateRandomGirl(SpawnReason::BIRTH, 18);
}

void cGirls::FireGirls(const std::vector<sGirl*>& targets) {
    std::stringstream ss;
    ss << "You fire ";

    for(int i = 0; i < targets.size(); ++i) {
        auto girl = targets[i];
        if(i == 0) {
            ss << girl->FullName();
        } else if (i == targets.size() - 2) {
            ss << ", and " << girl->FullName();
        } else {
            ss << ", " << girl->FullName();
        }
        assert(girl->m_Building);
        g_Game->girl_pool().GiveGirl(girl->m_Building->remove_girl(girl));
    }
    ss << ".";

    g_Game->push_message(ss.str(), COLOR_BLUE);
}

void cGirls::FreeGirls(const std::vector<sGirl*>& targets, bool party) {
    std::stringstream ss;
    ss << "You grant ";

    if(party) {
        if(!g_Game->gold().misc_debit(100 * targets.size())) {
            g_Game->push_message("You cannot afford the freedom party", COLOR_RED);
            party = false;
        }
    }

    for(int i = 0; i < targets.size(); ++i) {
        auto girl = targets[i];
        assert(girl->is_slave());
        if(i == 0) {
            ss << girl->FullName();
        } else if (i == targets.size() - 1) {
            ss << ", and " << girl->FullName();
        } else {
            ss << ", " << girl->FullName();
        }
        assert(girl->m_Building);

        girl->remove_status(STATUS_SLAVE);
        girl->m_AccLevel = g_Game->settings().get_integer(settings::USER_ACCOMODATION_FREE);
        girl->set_default_house_percent();

        if(party) {
            g_Game->player().disposition(7);
            // if she really hates you, a freedom party alone won't sway her
            if(girl->pclove() < -50) {
                girl->pcfear(-10);
                girl->pclove(25);
                girl->happiness(20);
                girl->dignity(5);
            } else {
                girl->pclove(35);
                girl->pcfear(-20);
                girl->obedience(5);
                girl->happiness(50);
                girl->obedience(10);
                girl->dignity(5);
            }
            girl->confidence(5);
            girl->tiredness(5);
        } else {
            g_Game->player().disposition(5);
            girl->pclove(35);
            girl->pcfear(-20);
            girl->happiness(35);
        }
    }

    if(targets.size() == 1) {
        ss << " her freedom";
    } else {
        ss << " their freedom";
    }

    if(party) {
        ss << " and celebrate with a big party.";
    } else {
        ss << ".";
    }

    g_Game->push_message(ss.str(), COLOR_BLUE);
}

void cGirls::SellSlaves(const std::vector<sGirl*>& target) {
    int sellsize = target.size();
    std::stringstream ss;
    ss << "You sell ";

    auto price_of = [&](int i) {
        return g_Game->tariff().slave_sell_price(*target.at(i));
    };

    if (sellsize == 1) ss << target.front()->FullName() << " for " << price_of(0) << " gold";
    else if (sellsize == 2) {
        ss << "two slaves:\n" << target.front()->FullName() << " for " << price_of(0) << " gold and\n"
           << target.at(1)->FullName() << " for " << price_of(1) << " gold";
    } else {
        ss << sellsize << " slaves:\n";
        for (int i = 0; i < sellsize; i++) {
            if (i != 0) ss << ",\n";
            else if (i == sellsize - 1) ss << ", and\n";
            ss << target.at(i)->FullName() << " for " << price_of(i) << " gold";
        }
    }
    if (sellsize > 1) {
        int total = 0;
        for (int i = 0; i < (int) sellsize; i++) total += price_of(i);
        ss << ".\nYour total take was " << total << " gold";
    }
    ss << ".";

    // now do the actual sale
    for (sGirl* girl : target) {
        assert(girl->is_slave());
        g_Game->gold().slave_sales(g_Game->tariff().slave_sell_price(*girl));
        g_Game->girl_pool().GiveGirl(girl->m_Building->remove_girl(girl));
    }

    g_Game->push_message(ss.str(), COLOR_BLUE);
}

void cGirls::SetSlaveStats(sGirl& girl) {
    girl.set_slave();
    girl.obedience(-10);
    girl.pcfear(10);
    girl.pclove(-35);
    girl.happiness(-50);
    girl.spirit(-5);
    girl.dignity(-5);
    girl.set_default_house_percent();
}

std::shared_ptr<sGirl> cGirls::CreateRandomGirl(SpawnReason reason, int age) {
    return m_RandomGirls.spawn(reason, age);
}

int cGirls::GetRequiredXP(const sGirl& girl) {
    int level = girl.level();
    return min(32000, (level + 1) * 100);
}
