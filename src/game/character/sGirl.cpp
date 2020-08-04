#include "utils/algorithms.hpp"
#include "character/sGirl.h"
#include "CLog.h"
#include "XmlMisc.h"
#include "cJobManager.h"
#include "cTariff.h"
#include "buildings/IBuilding.h"
#include "Game.hpp"
#include "cGangs.h"
#include "character/cCustomers.h"
#include "cInventory.h"
#include "character/cPlayer.h"
#include "buildings/cBuildingManager.h"
#include "scripting/cScriptManager.h"
#include "xml/util.h"
#include "xml/getattr.h"
#include "sConfig.h"
#include "Inventory.h"
#include "traits/ITraitsCollection.h"
#include "predicates.h"
#include "pregnancy.h"
#include "jobs/GenericJob.h"
#include "buildings/cDungeon.h"

extern cRng g_Dice;
extern cConfig cfg;

bool is_Actress_Job(int testjob);
bool CrewNeeded(const IBuilding& building);

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

/*
* MOD: DocClox: Stuff for the XML loader code
*
* first: static members need declaring
*/
bool sGirl::m_maps_setup = false;
std::map<std::string, unsigned int> sGirl::jobs_lookup;

const char *sGirl::children_type_names[] =
                   {
                           "Total_Births", "Beasts", "All_Girls", "All_Boys", "Customer_Girls",
                           "Customer_Boys", "Your_Girls", "Your_Boys", "Miscarriages", "Abortions"
                   };

//  When modifying Training types, search for "Change-Traning-Types"  :  found in >> cGirls.cpp > training_jobs[]
const char *sGirl::training_jobs[] = {
        "puppy training",
        "pony training",
        "general training"
};

sGirl::sGirl(bool unique) : ICharacter( g_Game->create_traits_collection(), unique ),
        m_EventMapping(g_Game->script_manager().CreateEventMapping("GirlEventMapping", "DefaultGirl"))
{
    // Names
    m_Name = m_FullName = m_FirstName = m_MiddleName = m_Surname = m_MotherName = m_FatherName = m_Desc = "";

    // Time
    m_BDay = 0;        m_WeeksPast = 0;

    // Jobs and money
    m_DayJob = m_NightJob = JOB_UNSET;
    m_WorkingDay = m_PrevWorkingDay = 0;

    m_Refused_To_Work_Day = m_Refused_To_Work_Night = false;
    m_Money = m_Pay = m_Tips = 0;
    m_NumCusts = m_NumCusts_old = 0;

    // Sex
    m_UseAntiPreg = true;
    m_WeeksPreg = 0;
    m_JustGaveBirth = false;
    m_PregCooldown = 0;

    // Health and happiness
    m_Tort = false;
    m_AccLevel = 0;
    m_Withdrawals = 0;
    m_DaysUnhappy = 0;
    m_RunAway = 0;
    m_Spotted = 0;

    // Enjoyment
    for (int i = 0; i < NUM_ACTIONTYPES; i++)    // `J` Added m_Enjoyment here to zero out any that are not specified
        m_Enjoyment[i] = m_EnjoymentMods[i] = m_EnjoymentTemps[i] = 0;
    for (u_int i = 0; i < NUM_ACTIONTYPES; i++)    // `J` randomize starting likes -10 to 10 most closer to 0
        m_Enjoyment[i] = (g_Dice.bell(-10, 10));

    // Training
    for (int i = 0; i < NUM_TRAININGTYPES; i++)    // Added m_Training here to zero out any that are not specified
        m_Training[i] = m_TrainingMods[i] = m_TrainingTemps[i] = 0;

    // Others
    for (int & i : m_ChildrenCount)        { i = 0; }
    m_States = 0;
    m_FetishTypes.clear();



    // Other things that I'm not sure how their defaults would be set
    //    cEvents m_Events;
    //    cTriggerList m_Triggers;
    //    cChildList m_Children;
    //    vector<string> m_Canonical_Daughters;
}
sGirl::~sGirl()        // destructor
{
    m_Events.Clear();
}

std::string stringtolowerj(std::string name)
{
    std::string s;
    for (char i : name)
    {
        if (tolower(i) != tolower(' ') && tolower(i) != tolower('.') && tolower(i) != tolower(','))
        {
            s += tolower(i);
        }
    }
    return s;
}

bool sGirl::disobey_check(int action, JOBS job)
{
    int diff;
    int chance_to_obey = 0;                            // high value - more likely to obey
    chance_to_obey = -cGirls::GetRebelValue(*this, false, job);    // let's start out with the basic rebelliousness
    chance_to_obey += 100;                            // make it range from 0 to 200
    chance_to_obey /= 2;                            // get a conventional percentage value
    /*
    *    OK, let's factor in having a matron: normally this is done in GetRebelValue
    *    but matrons have shifts now, so really we want twice the effect for a matron
    *    on each shift as we'd get from just one. //corrected:(Either that, or we need to make this
    *    check shift dependent.)//
    *
    *    Anyway, the old version added 15 for a matron either shift. Let's add
    *    10 for each shift. Full coverage gets you 20 points
    */
    if (m_Building)
    { // `J` added building checks
        if (m_Building->matron_on_shift(SHIFT_DAY)) chance_to_obey += 10;
        if (m_Building->matron_on_shift(SHIFT_NIGHT)) chance_to_obey += 10;
    }
    /*
    *    This is still confusing - at least it still confuses me
    *    why not normalise the rebellion -100 to 100 value so it runs
    *    0 to 100, and invert it so it's basically an obedience check
    */

    switch (action)
    {
    case ACTION_COMBAT:
        // WD use best stat as many girls have only one stat high
        diff = std::max(combat(), magic()) - 50;
        diff /= 3;
        chance_to_obey += diff;
        break;
    case ACTION_SEX:
        // Let's do the same thing here
        diff = libido();
        diff /= 5;
        chance_to_obey += diff;
        break;
    case ACTION_WORKCLEANING:
        //
        break;
    default:
        break;
    }
    chance_to_obey += m_Enjoyment[action];            // add in her enjoyment level
    chance_to_obey += pclove() / 10;                    // let's add in some mods for love, fear and hate
    chance_to_obey += pcfear() / 10;
    chance_to_obey -= pchate() / 10;
    chance_to_obey += 30;                                    // Let's add a blanket 30% to all of that
    int roll = g_Dice.d100();                                // let's get a percentage roll
    diff = chance_to_obey - roll;
    bool girl_obeys = (diff >= 0);
    if (girl_obeys)                                            // there's a price to be paid for relying on love or fear
    {
        if (diff < (pclove() / 10)) pclove(-1);    // if the only reason she obeys is love it wears away that love
        if (diff < (pcfear() / 10)) pcfear(-1);    // just a little bit. And if she's only doing it out of fear
    }
    /*
    *    do we need any more than this, really?
    *    we can add in some shaping factors if desired
    */

    return !girl_obeys;
}




// ----- Lookups
void sGirl::setup_maps()
{
    g_LogFile.log(ELogLevel::INFO, "[sGirl::setup_maps] Setting up Stats, Skills and Status codes.");
    m_maps_setup = true;

    // `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >> cGirls.cpp > jobs_lookup
    jobs_lookup["Adv"] = JOB_ADVERTISING;
    jobs_lookup["AMng"] = JOB_ANGER;
    jobs_lookup["TOff"] = JOB_ARENAREST;
    jobs_lookup["AssJ"] = JOB_ASSJOB;
    jobs_lookup["Bkr"] = JOB_BAKER;
    jobs_lookup["BrCk"] = JOB_BARCOOK;
    jobs_lookup["BrMd"] = JOB_BARMAID;
    jobs_lookup["SSrp"] = JOB_BARSTRIPPER;
    jobs_lookup["SWhr"] = JOB_BARWHORE;
    jobs_lookup["BCpt"] = JOB_BEASTCAPTURE;
    jobs_lookup["BstC"] = JOB_BEASTCARER;
    jobs_lookup["Bksm"] = JOB_BLACKSMITH;
    jobs_lookup["BbJb"] = JOB_BOOBJOB;
    jobs_lookup["BRS"] = JOB_BREASTREDUCTION;
    jobs_lookup["Brwr"] = JOB_BREWER;
    jobs_lookup["BStp"] = JOB_BROTHELSTRIPPER;
    jobs_lookup["Bchr"] = JOB_BUTCHER;
    jobs_lookup["CM"] = JOB_CAMERAMAGE;
    jobs_lookup["CRnc"] = JOB_CATACOMBRANCHER;
    jobs_lookup["CMgr"] = JOB_CENTREMANAGER;
    jobs_lookup["TOff"] = JOB_CENTREREST;
    jobs_lookup["Crmn"] = JOB_CHAIRMAN;
    jobs_lookup["CGrd"] = JOB_CITYGUARD;
    jobs_lookup["GKpr"] = JOB_CLEANARENA;
    jobs_lookup["ClnC"] = JOB_CLEANCENTRE;
    jobs_lookup["ClnH"] = JOB_CLEANHOUSE;
    jobs_lookup["Cln"] = JOB_CLEANING;
    jobs_lookup["TOff"] = JOB_CLINICREST;
    jobs_lookup["Cblr"] = JOB_COBBLER;
    jobs_lookup["CmSr"] = JOB_COMUNITYSERVICE;
    jobs_lookup["CosS"] = JOB_COSMETICSURGERY;
    jobs_lookup["Cnsl"] = JOB_COUNSELOR;
    jobs_lookup["CP"] = JOB_CRYSTALPURIFIER;
    jobs_lookup["Cure"] = JOB_CUREDISEASES;
    jobs_lookup["CS"] = JOB_CUSTOMERSERVICE;
    jobs_lookup["Dlr"] = JOB_DEALER;
    jobs_lookup["Dir"] = JOB_DIRECTOR;
    jobs_lookup["Dtre"] = JOB_DOCTORE;
    jobs_lookup["Doc"] = JOB_DOCTOR;
    jobs_lookup["Entn"] = JOB_ENTERTAINMENT;
    jobs_lookup["Scrt"] = JOB_ESCORT;
    jobs_lookup["ExC"] = JOB_EXPLORECATACOMBS;
    jobs_lookup["EThr"] = JOB_EXTHERAPY;
    jobs_lookup["FLft"] = JOB_FACELIFT;
    jobs_lookup["FOEx"] = JOB_FAKEORGASM;
    jobs_lookup["Frmr"] = JOB_FARMER;
    jobs_lookup["FHnd"] = JOB_FARMHAND;
    jobs_lookup["FMgr"] = JOB_FARMMANGER;
    jobs_lookup["TOff"] = JOB_FARMREST;
    jobs_lookup["Feed"] = JOB_FEEDPOOR;
    jobs_lookup["FrtT"] = JOB_FERTILITY;
    jobs_lookup["Cage"] = JOB_FIGHTARENAGIRLS;
    jobs_lookup["FiBs"] = JOB_FIGHTBEASTS;
    jobs_lookup["CT"] = JOB_FIGHTTRAIN;
    jobs_lookup["FAct"] = JOB_FILMACTION;
    jobs_lookup["FAnl"] = JOB_FILMANAL;
    jobs_lookup["FBst"] = JOB_FILMBEAST;
    jobs_lookup["FBnd"] = JOB_FILMBONDAGE;
    jobs_lookup["FBuk"] = JOB_FILMBUKKAKE;
    jobs_lookup["FChf"] = JOB_FILMCHEF;
    jobs_lookup["FFac"] = JOB_FILMFACEFUCK;
    jobs_lookup["FFJ"] = JOB_FILMFOOTJOB;
    jobs_lookup["TOff"] = JOB_FILMFREETIME;
    jobs_lookup["FGrp"] = JOB_FILMGROUP;
    jobs_lookup["FHJ"] = JOB_FILMHANDJOB;
    jobs_lookup["FLes"] = JOB_FILMLESBIAN;
    jobs_lookup["FMst"] = JOB_FILMMAST;
    jobs_lookup["FMus"] = JOB_FILMMUSIC;
    jobs_lookup["FOrl"] = JOB_FILMORAL;
    jobs_lookup["FTor"] = JOB_FILMPUBLICBDSM;
    jobs_lookup["FRnd"] = JOB_FILMRANDOM;
    jobs_lookup["FSex"] = JOB_FILMSEX;
    jobs_lookup["FStp"] = JOB_FILMSTRIP;
    jobs_lookup["FTea"] = JOB_FILMTEASE;
    jobs_lookup["FTit"] = JOB_FILMTITTY;
    jobs_lookup["Fluf"] = JOB_FLUFFER;
    jobs_lookup["Grdn"] = JOB_GARDENER;
    jobs_lookup["Abrt"] = JOB_GETABORT;
    jobs_lookup["Heal"] = JOB_GETHEALING;
    jobs_lookup["Repr"] = JOB_GETREPAIRS;
    jobs_lookup["HGrl"] = JOB_HEADGIRL;
    jobs_lookup["Hcok"] = JOB_HOUSECOOK;
    jobs_lookup["Hpet"] = JOB_HOUSEPET;
    jobs_lookup["TOff"] = JOB_HOUSEREST;
    jobs_lookup["Dngn"] = JOB_INDUNGEON;
    jobs_lookup["Ntrn"] = JOB_INTERN;
    jobs_lookup["Jntr"] = JOB_JANITOR;
    jobs_lookup["Jwlr"] = JOB_JEWELER;
    jobs_lookup["Lipo"] = JOB_LIPO;
    jobs_lookup["MkIt"] = JOB_MAKEITEM;
    jobs_lookup["MkPt"] = JOB_MAKEPOTIONS;
    jobs_lookup["Mrkt"] = JOB_MARKETER;
    jobs_lookup["Msus"] = JOB_MASSEUSE;
    jobs_lookup["Mtrn"] = JOB_MATRON;
    jobs_lookup["Mech"] = JOB_MECHANIC;
    jobs_lookup["Mlkr"] = JOB_MILKER;
    jobs_lookup["Mlkd"] = JOB_MILK;
    jobs_lookup["Nurs"] = JOB_NURSE;
    jobs_lookup["Peep"] = JOB_PEEP;
    jobs_lookup["BdWm"] = JOB_PERSONALBEDWARMER;
    jobs_lookup["PTrn"] = JOB_PERSONALTRAINING;
    jobs_lookup["Pno"] = JOB_PIANO;
    jobs_lookup["Prmt"] = JOB_PROMOTER;
    jobs_lookup["Rncr"] = JOB_RANCHER;
    jobs_lookup["Rcrt"] = JOB_RECRUITER;
    jobs_lookup["Rehb"] = JOB_REHAB;
    jobs_lookup["Rsrc"] = JOB_RESEARCH;
    jobs_lookup["TOff"] = JOB_RESTING;
    jobs_lookup["RunA"] = JOB_RUNAWAY;
    jobs_lookup["Sec"] = JOB_SECURITY;
    jobs_lookup["SHrd"] = JOB_SHEPHERD;
    jobs_lookup["Sngr"] = JOB_SINGER;
    jobs_lookup["SBmd"] = JOB_SLEAZYBARMAID;
    jobs_lookup["SWtr"] = JOB_SLEAZYWAITRESS;
    jobs_lookup["SOBi"] = JOB_SO_BISEXUAL;
    jobs_lookup["SOLe"] = JOB_SO_LESBIAN;
    jobs_lookup["SOSt"] = JOB_SO_STRAIGHT;
    jobs_lookup["StgH"] = JOB_STAGEHAND;
    jobs_lookup["Talr"] = JOB_TAILOR;
    jobs_lookup["Thrp"] = JOB_THERAPY;
    jobs_lookup["Trtr"] = JOB_TORTURER;
    jobs_lookup["Prtc"] = JOB_TRAINING;
    jobs_lookup["TTid"] = JOB_TUBESTIED;
    jobs_lookup["VagR"] = JOB_VAGINAREJUV;
    jobs_lookup["Vet"] = JOB_VETERINARIAN;
    jobs_lookup["Wtrs"] = JOB_WAITRESS;
    jobs_lookup["BWhr"] = JOB_WHOREBROTHEL;
    jobs_lookup["HWhr"] = JOB_WHOREGAMBHALL;
    jobs_lookup["StWr"] = JOB_WHORESTREETS;
    jobs_lookup["XXXE"] = JOB_XXXENTERTAINMENT;
    jobs_lookup["255"] = 255;
}

JOBS sGirl::lookup_jobs_code(std::string s)
{
    if (!m_maps_setup)    // only need to do this once
        setup_maps();

    // be useful to be able to log unrecognised type names here
    if (jobs_lookup.find(s) == jobs_lookup.end())
    {
        // `J` added check for missing jobs_lookup
        for (int i = 0; i < NUM_JOBS; i++)
        {
            if (g_Game->job_manager().JobData[i].brief == s || g_Game->job_manager().JobData[i].name == s)
                return static_cast<JOBS>(i);
        }
        // if still not found, send original error message
        g_LogFile.log(ELogLevel::ERROR,  "[sGirl::jobs_enjoy_code] Error: unknown Job: ", s);
        return static_cast<JOBS>(-1);
    }
    return static_cast<JOBS>(jobs_lookup[s]);
}

// END MOD


bool sGirl::equip(const sInventoryItem* item, bool force) {
    if (force || can_equip(item))
    {
        g_Game->inventory_manager().Equip(*this, item, force);
        return true;
    }
    return false;
}

int sGirl::upd_base_stat(int stat_id, int amount, bool usetraits) {
    u_int stat = stat_id;
    switch (stat) {

    case STAT_HEALTH:
    case STAT_TIREDNESS:
        if (!usetraits) break;

        if (has_active_trait("Fragile"))    // 20% health and 10% tired penalties
        {
            if (stat == STAT_HEALTH) {
                if (amount > 0)            // gain health - a little less than normal
                {    // reduce the intended amount healed by 20% or at least 1 except keeping at least 1 point healed
                    int mod = amount / 5;
                    if (mod < 1) mod = 1;
                    amount -= mod;
                    if (amount < 1) amount = 1;
                } else if (amount < 0)    // lose health - a little more than normal
                {    // increase the amound of damage taken by 20% or at least 1 extra point lost
                    int mod = -amount / 5;
                    if (mod < 1) mod = 1;
                    amount -= mod;
                } else // (amount == 0)    // no change intended but possibly injure her anyway
                {    // based on her constitution
                    if (!g_Dice.percent(constitution()))
                        amount -= 1;
                }
            } else    // STAT_TIREDNESS
            {
                if (amount > 0)            // gain tiredness
                {    // increase the amound of tiredness gained by 10% (+0 ok)
                    int mod = amount / 10;
                    amount += mod;
                } else if (amount < 0)    // lose tiredness
                {    // reduce the intended rest gained by 10% (-0 ok)
                    int mod = -amount / 10;
                    amount += mod;
                } else // (amount == 0)    // no change intended but possibly tire her anyway
                {    // based on her constitution but only 50% of the time
                    if (!g_Dice.percent(constitution()) && g_Dice.percent(50))
                        amount += 1;
                }
            }
        }
        if (has_active_trait("Delicate"))    // 20% tired and 10% health penalties
        {
            if (stat == STAT_HEALTH) {
                if (amount > 0)            // gain health - a little less than normal
                {    // reduce the intended amount healed by 10% (-0 ok) keeping at least 1 point healed
                    int mod = amount / 10;
                    amount -= mod;
                    if (amount < 1) amount = 1;
                } else if (amount < 0)    // lose health - a little more than normal
                {    // increase the amound of damage taken by 10% (+0 ok)
                    int mod = -amount / 10;
                    amount -= mod;
                } else // (amount == 0)    // no change intended but possibly injure her anyway
                {    // based on her constitution but only 50% of the time
                    if (!g_Dice.percent(constitution()) && g_Dice.percent(50))
                        amount -= 1;
                }
            } else    // STAT_TIREDNESS
            {
                if (amount > 0)            // gain tiredness
                {    // increase the amound of tiredness gained by 20% and at least 1
                    int mod = amount / 5;
                    if (mod < 1) mod = 1;
                    amount += mod;
                } else if (amount < 0)    // lose tiredness
                {    // decrease the amound of tiredness recovered by 20% and at least 1 but recovering at least 1
                    int mod = -amount / 5;
                    if (mod < 1) mod = 1;
                    amount += mod;
                    if (amount > -1) amount = -1;
                } else // (amount == 0)    // no change intended but possibly tire her anyway
                {    // based on her constitution
                    if (!g_Dice.percent(constitution()))
                        amount += 1;
                }
            }
        }
        if (has_active_trait("Tough"))    // 20% health and 10% tired bonuses
        {
            if (stat == STAT_HEALTH) {
                if (amount > 0)            // gain health
                {    // increase the amount of health gained by 20% and at least 1 point
                    int mod = amount / 5;
                    if (mod < 1) mod = 1;
                    amount += mod;
                } else if (amount < 0)    // lose health
                {    // reduce the amount of health lost by 20% and at least 1 but lose at least 1
                    int mod = -amount / 5;
                    if (mod < 1) mod = 1;
                    amount += mod;
                    if (amount > -1) amount = -1;
                } else // (amount == 0)    // no change intended but possibly heal her anyway
                {    // based on her constitution
                    if (g_Dice.percent(constitution()))
                        amount += 1;
                }
            } else    // STAT_TIREDNESS
            {
                if (amount > 0)            // gain tiredness
                {    // decrease the amount of tiredness gained by 10% (-0 ok) but gain at least 1
                    int mod = amount / 10;
                    amount -= mod;
                    if (amount < 1) amount = 1;
                } else if (amount < 0)    // lose tiredness
                {    // increase the amount of tiredness lost by 10% (+0 ok)
                    int mod = -amount / 10;
                    amount -= mod;
                } else // (amount == 0)    // no change intended but possibly rest her anyway
                {    // based on her constitution but only 50% of the time
                    if (g_Dice.percent(constitution()) && g_Dice.percent(50))
                        amount -= 1;
                }
            }
        }
        if (has_active_trait("Construct") && ((stat == STAT_HEALTH && amount < 0) || (stat == STAT_TIREDNESS && amount > 0)))
            amount = (int) ceil((float) amount * 0.1); // constructs take 10% damage
        break;

    case STAT_HAPPINESS:
        if (has_active_trait("Pessimist") && g_Dice % 5 == 1 && usetraits) amount -= 1; // `J` added
        if (has_active_trait("Optimist") && g_Dice % 5 == 1 && usetraits) amount += 1; // `J` added
        break;
    }

    return ICharacter::upd_base_stat(stat_id, amount, true);
}

// ----- Pregnancy, insemination & children
// returns false if she becomes pregnant or true if she does not
bool sGirl::calc_pregnancy(int chance, int type, const ICharacter& father)
{
    std::string text = "She has";
    if (g_Dice.percent(100 - chance)) return true;
    /*
    *    narrative depends on what it was that Did The Deed
    *    specifically, was it human or not?
    */
    switch (type)
    {
    case STATUS_INSEMINATED:
        text += " been inseminated.";
        break;
    case STATUS_PREGNANT_BY_PLAYER:
        text += " become pregnant with your child.";
        break;
    case STATUS_PREGNANT:
    default:
        type = STATUS_PREGNANT;        // `J` rearranged and added default to make sure there are no complications
        text += " become pregnant.";
        break;
    }

    m_Events.AddMessage(text, IMGTYPE_PREGNANT, EVENT_DANGER);
    create_pregnancy(*this, 1, type, father);
    return false;
}

void sGirl::add_tiredness()
{
    if (has_active_trait("Incorporeal") || // Sanity check
                                     has_active_trait("Skeleton") ||
        has_active_trait("Zombie"))
    {
        set_stat(STAT_TIREDNESS, 0);    return;
    }
    int change = 10;
    if (constitution() > 0)
        change -= (constitution()) / 10;
    if (change <= 0)    change = 0;

  upd_base_stat(STAT_TIREDNESS, change, false);

    if (tiredness() == 100)
    {
      upd_base_stat(STAT_HAPPINESS, -1, false);
      upd_base_stat(STAT_HEALTH, -1, false);
    }
}

bool sGirl::fights_back()
{
    if (health() < 10 || tiredness() > 90)/* */    return false;

    // TODO not sure what this check does
    if (disobey_check(ACTION_COMBAT))/*            */    return true;
    int chance = get_trait_modifier("fight-back-chance");
    return g_Dice.percent(chance);
}

int sGirl::get_stat(int stat_id) const
{
    if (stat_id < 0) return 0;
    if (stat_id == STAT_HEALTH && has_active_trait("Incorporeal"))    return 100;
    else if (stat_id == STAT_TIREDNESS &&
             (has_active_trait("Incorporeal") ||
              has_active_trait("Skeleton") ||
              has_active_trait("Zombie")))
        return 0;
    // Generic calculation
    return ICharacter::get_stat(stat_id);

}

void sGirl::run_away()
{
    if(m_Building)
        g_Game->AddGirlToRunaways(m_Building->remove_girl(this));
    if (m_NightJob == JOB_INDUNGEON)
        g_Game->AddGirlToRunaways(g_Game->dungeon().RemoveGirl(this));
    m_RunAway = 6;        // player has 6 weeks to retreive
    m_NightJob = m_DayJob = JOB_RUNAWAY;
}

bool sGirl::keep_tips() const {
    if(is_slave()) {
        return g_Game->settings().get_bool(settings::USER_KEEP_TIPS_SLAVE);
    } else {
        return g_Game->settings().get_bool(settings::USER_KEEP_TIPS_FREE);
    }
}

bool sGirl::is_unpaid() const {
    return is_slave() && g_Game->settings().get_bool(settings::USER_PAY_SLAVE);
}

// This load

bool sGirl::LoadGirlXML(const tinyxml2::XMLElement* pGirl)
{
    //this is always called after creating a new girl, so let's not init sGirl again
    if (pGirl == nullptr) return false;
    int tempInt = 0;

    // load the name
    m_Name = pGirl->Attribute("Name");        // the name the girl is based on, also the name of the image folder
    m_Desc = (pGirl->Attribute("Desc") ? pGirl->Attribute("Desc") : "-");    // get the description

    // load the amount of days they are unhappy in a row
    pGirl->QueryIntAttribute("DaysUnhappy", &tempInt); m_DaysUnhappy = tempInt; tempInt = 0;

    // load their states
    pGirl->QueryAttribute("States", &m_States);

    // load their stats
    LoadXML(*pGirl);

    // load enjoyment values
    LoadActionsXML(pGirl->FirstChildElement("Actions"), m_Enjoyment, m_EnjoymentMods, m_EnjoymentTemps);

    // load training values
    LoadTrainingXML(pGirl->FirstChildElement("Training"), m_Training, m_TrainingMods, m_TrainingTemps);

    // load using antipreg
    pGirl->QueryAttribute("UseAntiPreg", &m_UseAntiPreg);

    // load withdrawals
    pGirl->QueryIntAttribute("Withdrawals", &tempInt); m_Withdrawals = tempInt; tempInt = 0;

    // load money
    pGirl->QueryIntAttribute("Money", &m_Money);

    // load working day counter
    pGirl->QueryIntAttribute("WorkingDay", &m_WorkingDay);
    pGirl->QueryIntAttribute("PrevWorkingDay", &m_PrevWorkingDay);    // `J` added
    if (m_WorkingDay < 0)        m_WorkingDay = 0;
    if (m_PrevWorkingDay < 0)    m_PrevWorkingDay = 0;

    // load acom level
    pGirl->QueryIntAttribute("AccLevel", &m_AccLevel);
    // load house percent
    pGirl->QueryIntAttribute("HousePercent", &m_HousePercent);

    // `J` changeing jobs to save as quick codes in stead of numbers so if new jobs are added they don't shift jobs
    std::string tempst = pGirl->Attribute("DayJob");            m_DayJob = lookup_jobs_code(tempst);
    tempst = pGirl->Attribute("NightJob");                m_NightJob = lookup_jobs_code(tempst);
    tempst = pGirl->Attribute("PrevDayJob");            m_PrevDayJob = lookup_jobs_code(tempst);
    tempst = pGirl->Attribute("PrevNightJob");            m_PrevNightJob = lookup_jobs_code(tempst);
    tempst = pGirl->Attribute("YesterDayJob");            m_YesterDayJob = lookup_jobs_code(tempst);
    tempst = pGirl->Attribute("YesterNightJob");        m_YesterNightJob = lookup_jobs_code(tempst);

    if (m_YesterDayJob < 0)        m_YesterDayJob = JOB_UNSET;
    if (m_YesterNightJob < 0)    m_YesterNightJob = JOB_UNSET;

    // load runnayway value
    pGirl->QueryIntAttribute("RunAway", &tempInt); m_RunAway = tempInt; tempInt = 0;

    // load spotted
    pGirl->QueryIntAttribute("Spotted", &tempInt); m_Spotted = tempInt; tempInt = 0;

    // load weeks past, birth day, and pregant time
    pGirl->QueryAttribute("WeeksPast", &m_WeeksPast);
    pGirl->QueryAttribute("BDay", &m_BDay);
    pGirl->QueryIntAttribute("WeeksPreg", &tempInt); m_WeeksPreg = tempInt; tempInt = 0;

    // load number of customers slept with
    pGirl->QueryAttribute("NumCusts", &m_NumCusts);
    m_NumCusts_old = m_NumCusts;

    // load their torture value
    pGirl->QueryAttribute("Tort", &m_Tort);

    // Load their children
    pGirl->QueryIntAttribute("PregCooldown", &tempInt); m_PregCooldown = tempInt; tempInt = 0;

    // load number of children
    auto* pChildren = pGirl->FirstChildElement("Children");
    for (int i = 0; i < CHILD_COUNT_TYPES; i++)        // `J` added
    {
        pChildren->QueryIntAttribute(children_type_names[i], &tempInt);
        if (tempInt < 0 || tempInt>1000) tempInt = 0;
        m_ChildrenCount[i] = tempInt;
        tempInt = 0;
    }
    if (pChildren)
    {
        for (auto* pChild = pChildren->FirstChildElement("Child"); pChild != nullptr; pChild = pChild->NextSiblingElement("Child"))
        {
            auto child = std::make_unique<sChild>(false, sChild::Girl, 0);                // `J` prepare a minimal new child
            bool success = child->LoadChildXML(pChild);                    // because this will load over top of it
            if (success) { m_Children.push_back(std::move(child)); }                // add it if it loaded
        }
    }


    // TODO load their triggers
    if (age() < 18) set_stat(STAT_AGE, 18);    // `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live

    cGirls::CalculateGirlType(*this);

    // get the number of daugher names
    /*ifs  >>  temp >> ws;
    for(int i = 0; i < temp; i++) {
    string s;

    ifs >> s >> ws;
    current->m_Canonical_Daughters.push_back(s);*/

    return true;
}

// This save

tinyxml2::XMLElement& sGirl::SaveGirlXML(tinyxml2::XMLElement& elRoot)
{
    auto& elGirl = PushNewElement(elRoot, "Girl");
    elGirl.SetAttribute("Name", m_Name.c_str());                        // save the name
    elGirl.SetAttribute("Desc", m_Desc.c_str());                        // save the description
    elGirl.SetAttribute("DaysUnhappy", m_DaysUnhappy);            // save the amount of days they are unhappy

    elGirl.SetAttribute("States", m_States);                    // save their states
    SaveXML(elGirl);        // Save their stats

    elGirl.SetAttribute("UseAntiPreg", m_UseAntiPreg);            // save using antipreg
    elGirl.SetAttribute("Withdrawals", m_Withdrawals);            // save withdrawals
    elGirl.SetAttribute("Money", m_Money);                        // save money
    elGirl.SetAttribute("AccLevel", m_AccLevel);                // save acom level
    elGirl.SetAttribute("HousePercent", m_HousePercent);                // save acom level

    // save working day counter
    elGirl.SetAttribute("WorkingDay", m_WorkingDay);
    elGirl.SetAttribute("PrevWorkingDay", m_PrevWorkingDay);    // `J` added

    // `J` changed jobs to save as quick codes in stead of numbers so if new jobs are added they don't shift jobs
    // save day/night jobs
    if (m_DayJob < 0 || m_DayJob > NUM_JOBS) elGirl.SetAttribute("DayJob", "255");
    else elGirl.SetAttribute("DayJob", g_Game->job_manager().JobData[m_DayJob].brief.c_str());
    if (m_NightJob < 0 || m_NightJob > NUM_JOBS) elGirl.SetAttribute("NightJob", "255");
    else elGirl.SetAttribute("NightJob", g_Game->job_manager().JobData[m_NightJob].brief.c_str());

    // save prev day/night jobs
    if (m_PrevDayJob < 0 || m_PrevDayJob > NUM_JOBS) elGirl.SetAttribute("PrevDayJob", "255");
    else elGirl.SetAttribute("PrevDayJob", g_Game->job_manager().JobData[m_PrevDayJob].brief.c_str());
    if (m_PrevNightJob < 0 || m_PrevNightJob > NUM_JOBS) elGirl.SetAttribute("PrevNightJob", "255");
    else elGirl.SetAttribute("PrevNightJob", g_Game->job_manager().JobData[m_PrevNightJob].brief.c_str());

    // save prev day/night jobs
    if (m_YesterDayJob < 0 || m_YesterDayJob > NUM_JOBS) elGirl.SetAttribute("YesterDayJob", "255");
    else elGirl.SetAttribute("YesterDayJob", g_Game->job_manager().JobData[m_YesterDayJob].brief.c_str());
    if (m_YesterNightJob < 0 || m_YesterNightJob > NUM_JOBS) elGirl.SetAttribute("YesterNightJob", "255");
    else elGirl.SetAttribute("YesterNightJob", g_Game->job_manager().JobData[m_YesterNightJob].brief.c_str());

    elGirl.SetAttribute("RunAway", m_RunAway);                    // save runnayway vale
    elGirl.SetAttribute("Spotted", m_Spotted);                    // save spotted

    // save weeks past, birth day, and pregant time
    elGirl.SetAttribute("WeeksPast", m_WeeksPast);
    elGirl.SetAttribute("BDay", m_BDay);
    elGirl.SetAttribute("WeeksPreg", m_WeeksPreg);

    elGirl.SetAttribute("NumCusts", m_NumCusts);                // number of customers slept with
    elGirl.SetAttribute("Tort", m_Tort);                        // save their torture value

    // save their children
    elGirl.SetAttribute("PregCooldown", m_PregCooldown);
    auto& elChildren = PushNewElement(elGirl, "Children");
    for (int i = 0; i < CHILD_COUNT_TYPES; i++)
    {
        if (m_ChildrenCount[i] < 0 || m_ChildrenCount[i] > 1000)    m_ChildrenCount[i] = 0;        // to correct girls without these
        elChildren.SetAttribute(children_type_names[i], m_ChildrenCount[i]);
    }

    for(auto& child : m_Children) {
        child->SaveChildXML(elChildren);
    }

    SaveActionsXML(elGirl, m_Enjoyment);                            // save their enjoyment values
    SaveTrainingXML(elGirl, m_Training, m_TrainingMods, m_TrainingTemps);                        // save their training values
    /// TODO save triggers

    return elGirl;
}

bool sChild::LoadChildXML(const tinyxml2::XMLElement* pChild)
{
    if (pChild == nullptr)
    {
        return false;
    }

    int tempInt = 0;
    pChild->QueryIntAttribute("Age", &tempInt); m_Age = tempInt; tempInt = 0;
    pChild->QueryBoolAttribute("IsPlayers", &m_IsPlayers);
    pChild->QueryIntAttribute("Sex", &tempInt);    m_Sex = sChild::Gender(tempInt); tempInt = 0;
    pChild->QueryIntAttribute("Unborn", &tempInt); m_Unborn = tempInt; tempInt = 0;
    pChild->QueryIntAttribute("MultiBirth", &tempInt); m_MultiBirth = tempInt; tempInt = 0;    // `J` added
    if (m_MultiBirth < 1) m_MultiBirth = 1;
    if (m_MultiBirth > 5) m_MultiBirth = 5;            // `J` limited
    pChild->QueryIntAttribute("GirlsBorn", &tempInt); m_GirlsBorn = tempInt; tempInt = 0;    // `J` added
    if (m_GirlsBorn > m_MultiBirth) m_GirlsBorn = m_MultiBirth;                                // `J` limited
    if (m_GirlsBorn < 0)    m_GirlsBorn = 0;                                                // `J` limited

    LoadStatsXML(pChild->FirstChildElement("Stats"), m_Stats);        // load their stats
    LoadSkillsXML(pChild->FirstChildElement("Skills"), m_Skills);    // load their skills
    return true;
}

tinyxml2::XMLElement& sChild::SaveChildXML(tinyxml2::XMLElement& elRoot)
{
    auto& elChild = PushNewElement(elRoot, "Child");
    elChild.SetAttribute("Age", m_Age);
    elChild.SetAttribute("IsPlayers", m_IsPlayers);
    elChild.SetAttribute("Sex", m_Sex);
    elChild.SetAttribute("Unborn", m_Unborn);
    elChild.SetAttribute("MultiBirth", m_MultiBirth);    // `J` added
    elChild.SetAttribute("GirlsBorn", m_GirlsBorn);        // `J` added
    SaveStatsXML(elChild, m_Stats);                        // Save their stats
    SaveSkillsXML(elChild, m_Skills);                    // save their skills
    return elChild;
}


bool sGirl::can_equip(const sInventoryItem* item) const
{
    if(!item) return false;
    int max = cInventory::NumItemSlots(item);
    int cur = inventory().num_equipped_of_type(item->m_Type);
    return cur < max;
}

sChild::sChild(bool is_players, Gender gender, int MultiBirth)
{
    // set all the basics
    m_Unborn = 1;
    m_Age = 0;
    m_IsPlayers = is_players;
    if (gender == None && MultiBirth != 0)
        m_Sex = RandomGender();
    else m_Sex = gender;
    m_MultiBirth = 1;
    m_GirlsBorn = (m_Sex == Girl ? 1 : 0);
    m_Next = m_Prev = nullptr;
    for (int & m_Skill : m_Skills)        // Added m_Skills here to zero out any that are not specified -- PP
        m_Skill = 0;
    for (int & m_Stat : m_Stats)            // Added m_Stats here to zero out any that are not specified -- PP
        m_Stat = 0;
    m_Stats[STAT_HEALTH] = 100;
    m_Stats[STAT_HAPPINESS] = 100;

    if (MultiBirth == 0) return;    // 0 means we are creating a new child in order to load one so we can skip the rest

    // so now the first baby is ready, check if there are more
    m_MultiBirth = MultiBirth;

    int trycount = 1;
    double multichance = 100.f * g_Game->settings().get_percent(settings::PREG_MULTI_CHANCE);
    while (g_Dice.percent(multichance) && m_MultiBirth < 5)
    {
        m_MultiBirth++;
        if (RandomGender() == Girl) m_GirlsBorn++;    // check if the new one is a girl
        trycount++;
        multichance /= trycount;
    }
}

sChild::Gender sChild::RandomGender() {
    auto chance = g_Game->settings().get_percent(settings::PREG_GIRL_CHANCE);
    return g_Dice.percent(chance) ? Girl : Boy;
}

bool sGirl::calc_pregnancy(cPlayer *player, double factor, bool nomessage)
{
    float girl_chance = fertility(*this);
    sPercent guy_chance = g_Game->settings().get_percent(settings::PREG_CHANCE_PLAYER);
    float chance = girl_chance * guy_chance * factor;
    //    now do the calculation
    bool result = calc_pregnancy(int(chance), STATUS_PREGNANT_BY_PLAYER, *player);
    if(!result && !nomessage) {
        g_Game->push_message(FullName() + " has gotten pregnant", 0);
    }
    return result;
}

bool sGirl::calc_group_pregnancy(const sCustomer& cust, bool good, double factor)
{
    double girl_chance = fertility(*this);
    sPercent guy_chance = g_Game->settings().get_percent(settings::PREG_CHANCE_CUST);
    float chance = girl_chance * (1.f - std::pow(1.f - float(guy_chance), cust.m_Amount)) * factor;

    // now do the calculation
    return calc_pregnancy(int(chance), STATUS_PREGNANT, cust);
}

bool sGirl::calc_pregnancy(const sCustomer& cust, double factor)
{
    double girl_chance = fertility(*this);
    sPercent guy_chance = g_Game->settings().get_percent(settings::PREG_CHANCE_CUST);
    float chance = girl_chance * float(guy_chance) * factor;

    return calc_pregnancy(int(chance), STATUS_PREGNANT, cust);
}

bool sGirl::calc_insemination(const sCustomer& cust, bool good, double factor)
{
    double girl_chance = fertility(*this);
    sPercent guy_chance = g_Game->settings().get_percent(settings::PREG_CHANCE_BEAST);
    float chance = girl_chance * float(guy_chance) * factor;
    return calc_pregnancy(int(chance), STATUS_INSEMINATED, cust);
}

bool sGirl::carrying_monster() const    { return(m_States & (1u << STATUS_INSEMINATED)) != 0; }
bool sGirl::carrying_human() const         { return carrying_players_child() || carrying_customer_child(); }
bool sGirl::carrying_players_child() const    { return(m_States & (1u << STATUS_PREGNANT_BY_PLAYER)) != 0; }
bool sGirl::carrying_customer_child() const    { return(m_States & (1u << STATUS_PREGNANT)) != 0; }
bool sGirl::is_pregnant() const            { return(m_States & (1u << STATUS_PREGNANT) || m_States & (1 << STATUS_PREGNANT_BY_PLAYER) || m_States & (1 << STATUS_INSEMINATED)); }
bool sGirl::is_mother()    const            { return m_ChildrenCount[CHILD00_TOTAL_BIRTHS] > 0; }
bool sGirl::is_poisoned() const            { return(has_status(STATUS_POISONED) || has_status(STATUS_BADLY_POISONED)); }
void sGirl::clear_dating()                { m_States &= ~(1 << STATUS_DATING_PERV); m_States &= ~(1 << STATUS_DATING_MEAN); m_States &= ~(1 << STATUS_DATING_NICE); }
void sGirl::clear_pregnancy()
{
    m_States &= ~(1u << STATUS_PREGNANT);
    m_States &= ~(1u << STATUS_PREGNANT_BY_PLAYER);
    m_States &= ~(1u << STATUS_INSEMINATED);
    m_WeeksPreg = 0;
    erase_if(m_Children, [](auto& child){ return child->m_Unborn > 0;});
}

int sGirl::breast_size() const// `J` Breast size number, normal is 4, 1 is flat, max is 10
{
    return 4 + get_trait_modifier("breast-size");
}

bool sGirl::is_dead() const
{
    return health() <= 0;
}

bool sGirl::is_fighter(bool canbehelped) const
{
    if (canbehelped)
    {
        return has_active_trait("Aggressive") ||
               has_active_trait("Yandere") ||
               has_active_trait("Tsundere");
    }
    return has_active_trait("Aggressive") ||
           has_active_trait("Assassin") ||
           has_active_trait("Yandere") ||
           has_active_trait("Brawler") ||
           has_active_trait("Tsundere");
}

bool sGirl::is_resting() const
{
    return ((m_DayJob == JOB_FILMFREETIME    && m_NightJob == JOB_FILMFREETIME) ||
            (m_DayJob == JOB_ARENAREST        && m_NightJob == JOB_ARENAREST) ||
            (m_DayJob == JOB_CENTREREST        && m_NightJob == JOB_CENTREREST) ||
            (m_DayJob == JOB_CLINICREST        && m_NightJob == JOB_CLINICREST) ||
            (m_DayJob == JOB_HOUSEREST        && m_NightJob == JOB_HOUSEREST) ||
            (m_DayJob == JOB_FARMREST        && m_NightJob == JOB_FARMREST) ||
            (m_DayJob == JOB_RESTING        && m_NightJob == JOB_RESTING));
}
bool sGirl::is_havingsex() const
{
    return (
            m_DayJob == JOB_TRAINING || m_NightJob == JOB_TRAINING ||
            m_DayJob == JOB_ESCORT || m_NightJob == JOB_ESCORT ||
            m_DayJob == JOB_WHOREGAMBHALL || m_NightJob == JOB_WHOREGAMBHALL ||
            m_DayJob == JOB_BARWHORE || m_NightJob == JOB_BARWHORE ||
            m_DayJob == JOB_WHOREBROTHEL || m_NightJob == JOB_WHOREBROTHEL ||
            m_DayJob == JOB_WHORESTREETS || m_NightJob == JOB_WHORESTREETS ||
            m_DayJob == JOB_FILMANAL || m_NightJob == JOB_FILMANAL ||
            m_DayJob == JOB_FILMFOOTJOB || m_NightJob == JOB_FILMFOOTJOB ||
            m_DayJob == JOB_FILMHANDJOB || m_NightJob == JOB_FILMHANDJOB ||
            m_DayJob == JOB_FILMLESBIAN || m_NightJob == JOB_FILMLESBIAN ||
            m_DayJob == JOB_FILMORAL || m_NightJob == JOB_FILMORAL ||
            m_DayJob == JOB_FILMSEX || m_NightJob == JOB_FILMSEX ||
            m_DayJob == JOB_FILMTITTY || m_NightJob == JOB_FILMTITTY ||
            m_DayJob == JOB_FILMBEAST || m_NightJob == JOB_FILMBEAST ||
            m_DayJob == JOB_FILMBONDAGE || m_NightJob == JOB_FILMBONDAGE ||
            m_DayJob == JOB_FILMBUKKAKE || m_NightJob == JOB_FILMBUKKAKE ||
            m_DayJob == JOB_FILMFACEFUCK || m_NightJob == JOB_FILMFACEFUCK ||
            m_DayJob == JOB_FILMGROUP || m_NightJob == JOB_FILMGROUP ||
            m_DayJob == JOB_FILMPUBLICBDSM || m_NightJob == JOB_FILMPUBLICBDSM ||
            m_DayJob == JOB_FILMRANDOM || m_NightJob == JOB_FILMRANDOM ||
            m_DayJob == JOB_PERSONALBEDWARMER || m_NightJob == JOB_PERSONALBEDWARMER ||
            m_DayJob == JOB_PERSONALTRAINING || m_NightJob == JOB_PERSONALTRAINING ||
            m_DayJob == JOB_FAKEORGASM || m_NightJob == JOB_FAKEORGASM ||
            m_DayJob == JOB_SO_STRAIGHT || m_NightJob == JOB_SO_STRAIGHT ||
            m_DayJob == JOB_SO_BISEXUAL || m_NightJob == JOB_SO_BISEXUAL ||
            m_DayJob == JOB_SO_LESBIAN || m_NightJob == JOB_SO_LESBIAN ||
            m_DayJob == JOB_HOUSEPET || m_NightJob == JOB_HOUSEPET
    );
}
bool sGirl::was_resting() const
{
    return ((m_PrevDayJob == JOB_FILMFREETIME    && m_PrevNightJob == JOB_FILMFREETIME) ||
            (m_PrevDayJob == JOB_ARENAREST        && m_PrevNightJob == JOB_ARENAREST) ||
            (m_PrevDayJob == JOB_CENTREREST        && m_PrevNightJob == JOB_CENTREREST) ||
            (m_PrevDayJob == JOB_CLINICREST        && m_PrevNightJob == JOB_CLINICREST) ||
            (m_PrevDayJob == JOB_HOUSEREST        && m_PrevNightJob == JOB_HOUSEREST) ||
            (m_PrevDayJob == JOB_FARMREST        && m_PrevNightJob == JOB_FARMREST) ||
            (m_PrevDayJob == JOB_RESTING        && m_PrevNightJob == JOB_RESTING));
}

void sGirl::OutputGirlDetailString(std::string& Data, const std::string& detailName) const
{
    //given a statistic name, set a string to a value that represents that statistic
    static std::stringstream ss;
    ss.str("");

    bool interrupted = false;    // `J` added
    if (m_YesterDayJob != m_DayJob &&
        (cJobManager::is_Surgery_Job(m_YesterDayJob) || m_YesterDayJob == JOB_REHAB) &&
        ((m_WorkingDay > 0) || m_PrevWorkingDay > 0))
        interrupted = true;

    /* */if (detailName == "Name")                { ss << FullName(); }
    else if (detailName == "Health")            { if (get_stat(STAT_HEALTH) <= 0) ss << "DEAD"; else ss << get_stat(STAT_HEALTH) << "%"; }
    else if (detailName == "Age")                { if (get_stat(STAT_AGE) == 100) ss << "???"; else ss << get_stat(STAT_AGE); }
    else if (detailName == "Libido")            { ss << libido(); }
    else if (detailName == "Rebel")                { ss << rebel(); }
    else if (detailName == "Looks")                { ss << ((get_stat(STAT_BEAUTY) + get_stat(STAT_CHARISMA)) / 2) << "%"; }
    else if (detailName == "Tiredness")            { ss << get_stat(STAT_TIREDNESS) << "%"; }
    else if (detailName == "Happiness")            { ss << get_stat(STAT_HAPPINESS) << "%"; }
    else if (detailName == "Virgin")            { ss << (is_virgin(*this) ? "Yes" : "No"); }
    else if (detailName == "Weeks_Due")
    {
        if (is_pregnant())
        {
            ss << get_preg_duration() - m_WeeksPreg;
        }
        else
        {
            ss << "---";
        }
    }
    else if (detailName == "PregCooldown")        { ss << m_PregCooldown; }
    else if (detailName == "Accommodation")
    {
        ss << cGirls::Accommodation(m_AccLevel);
    }
    else if (detailName == "Gold")
    {
        if (g_Game->gang_manager().GetGangOnMission(MISS_SPYGIRLS))
        {
            ss << m_Money;
        }
        else
        {
            ss << "???";
        }
    }
    else if (detailName == "Pay")                { ss << m_Pay; }

        // 'J' Added for .06.03.01
    else if (detailName == "DayJobShort" || detailName == "NightJobShort")
    {
        ss << g_Game->job_manager().JobData[(detailName == "DayJobShort" ? m_DayJob : m_NightJob)].brief;
    }

        // 'J' Girl Table job text
    else if (detailName == "DayJob" || detailName == "NightJob")
    {
        bool DN_Day = detailName == "NightJob";
        int DN_Job = get_job(DN_Day);

        // `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >>
        if (DN_Job >= NUM_JOBS)
        {
            ss << "None";
        }
        else if (DN_Job == JOB_FAKEORGASM || DN_Job == JOB_SO_STRAIGHT || DN_Job == JOB_SO_BISEXUAL || DN_Job == JOB_SO_LESBIAN)
        {
            ss << g_Game->job_manager().JobData[DN_Job].name << " (" << m_WorkingDay << "%)";
        }
        else if (DN_Job == JOB_CUREDISEASES)
        {
            if (m_Building->num_girls_on_job(JOB_DOCTOR, DN_Day) > 0)
            {
                ss << g_Game->job_manager().JobData[DN_Job].name << " (" << m_WorkingDay << "%)";
            }
            else
            {
                ss << g_Game->job_manager().JobData[DN_Job].name << " (" << m_WorkingDay << "%) **";
            }
        }
        else if (DN_Job == JOB_REHAB || DN_Job == JOB_ANGER || DN_Job == JOB_EXTHERAPY || DN_Job == JOB_THERAPY)
        {
            if (m_Building->num_girls_on_job(JOB_COUNSELOR, DN_Day) > 0)
            {
                ss << g_Game->job_manager().JobData[DN_Job].name << " (" << 3 - m_WorkingDay << ")";
            }
            else
            {
                ss << g_Game->job_manager().JobData[DN_Job].name << " (?)***";
            }
        }
        else if (DN_Job == JOB_GETHEALING)
        {
            if (m_Building->num_girls_on_job(JOB_DOCTOR, DN_Day) > 0)
            {
                ss << g_Game->job_manager().JobData[DN_Job].name;
            }
            else
            {
                ss << g_Game->job_manager().JobData[DN_Job].name << " ***";
            }
        }
        else if (DN_Job == JOB_GETREPAIRS)
        {
            if (m_Building->num_girls_on_job(JOB_MECHANIC, DN_Day) > 0 &&
                (has_active_trait("Construct") || has_active_trait("Half-Construct")))
            {
                ss << g_Game->job_manager().JobData[DN_Job].name;
            }
            else if (has_active_trait("Construct"))
            {
                ss << g_Game->job_manager().JobData[DN_Job].name << " ****";
            }
            else
            {
                ss << g_Game->job_manager().JobData[DN_Job].name << " !!";
            }
        }
        else if (DN_Job == JOB_GETABORT)
        {
            int wdays = (2 - (this)->m_WorkingDay);
            if (m_Building->num_girls_on_job(JOB_NURSE, DN_Day) > 0)
            {
                wdays = 1;
            }
            if (m_Building->num_girls_on_job( JOB_DOCTOR, DN_Day) > 0)
            {
                ss << g_Game->job_manager().JobData[DN_Job].name << " (" << wdays << ")*";
            }
            else
            {
                ss << g_Game->job_manager().JobData[DN_Job].name << " (?)***";
            }
        }
        else if (cJobManager::is_Surgery_Job(DN_Job))
        {
            int wdays = (5 - (this)->m_WorkingDay);
            if (m_Building->num_girls_on_job(JOB_NURSE, DN_Day) > 0)
            {
                if (wdays >= 3)        { wdays = 3; }
                else if (wdays > 1)    { wdays = 2; }
                else                { wdays = 1; }
            }
            if (m_Building->num_girls_on_job(JOB_DOCTOR, DN_Day) > 0)
            {
                ss << g_Game->job_manager().JobData[DN_Job].name << " (" << wdays << ")*";
            }
            else
            {
                ss << g_Game->job_manager().JobData[DN_Job].name << " (?)***";
            }
        }
        else if (is_Actress_Job(DN_Job) && CrewNeeded(*m_Building))
        {
            ss << g_Game->job_manager().JobData[DN_Job].name << " **";
        }
        else if (is_resting() && !was_resting() && m_PrevDayJob != 255 && m_PrevNightJob != 255)
        {
            ss << g_Game->job_manager().JobData[DN_Job].name;
            ss << " (" << g_Game->job_manager().JobData[(DN_Day == 0 ? m_PrevDayJob : m_PrevNightJob)].brief << ")";
        }
        else
        {
            ss << g_Game->job_manager().JobData[DN_Job].name;
        }
        if (interrupted)
        {
            ss << " **";
        }
    }

    else if (detailName.find("STAT_") != std::string::npos)
    {
        std::string stat = detailName;
        stat.replace(0, 5, "");
        int code = get_stat_id(stat);
        if (code != -1)
        {
            ss << get_stat(code);
        }
        else
        {
            ss << "Error";
        }
    }
    else if (detailName.find("SKILL_") != std::string::npos)
    {
        std::string skill = detailName;
        skill.replace(0, 6, "");
        int code = get_skill_id(skill);
        if (code != -1)
        {
            ss << get_skill(code);
        }
        else
        {
            ss << "Error";
        }
    }
    else if (detailName.find("TRAIT_") != std::string::npos)
    {
        std::string trait = detailName;
        trait.replace(0, 6, "");
        if (this->has_active_trait(trait.c_str()))
        {
            ss << "Yes";
        }
        else
        {
            ss << "No";
        }
    }
    else if (detailName.find("STATUS_") != std::string::npos)
    {
        std::string status = detailName;
        status.replace(0, 7, "");
        int code = get_status_id(status);
        if (code != -1)
        {
            ss << (m_States&(1u << code) ? "Yes" : "No");
        }
        else
        {
            ss << "Error";
        }
    }
    else if (detailName == "is_pregnant")
    {
        if (is_virgin(*this)) ss << "Vg.";
        else if (is_pregnant())
        {
            int to_go = get_preg_duration() - (this)->m_WeeksPreg;
            if (carrying_players_child())    ss << "Yours";
            else if (carrying_monster())    ss << "Beast";
            else /*                      */    ss << "Yes";
            if (has_active_trait("Sterile") || has_active_trait("Zombie") || has_active_trait("Skeleton"))
                ss << "?" << to_go << "?";    // how?
            else
                ss << "(" << to_go << ")";
        }
        else if (m_PregCooldown > 0)
        {
            ss << "No";
            if (has_active_trait("Sterile") || has_active_trait("Zombie") || has_active_trait("Skeleton"))
                ss << "!" << m_PregCooldown << "!";
            else
                ss << "(" << m_PregCooldown << ")";
        }
        else if (has_active_trait("Zombie") || has_active_trait("Skeleton")) ss << "Ud.";
        else if (has_active_trait("Sterile"))        ss << "St.";
        else if (has_active_trait("Fertile"))      ss << "No+";
        else if (has_active_trait("Broodmother"))  ss << "No++";
        else                                ss << "No";
    }
    else if (detailName == "is_slave")            { ss << (is_slave() ? "Yes" : "No"); }
    else if (detailName == "carrying_human")    { ss << (carrying_human() ? "Yes" : "No"); }
    else if (detailName == "is_addict")            { ss << (is_addict(*this) ? "Yes" : "No"); }
    else if (detailName == "has_disease")        { ss << (has_disease(*this) ? "Yes" : "No"); }
    else if (detailName == "is_mother")            { ss << (is_mother() ? "Yes" : "No"); }
    else if (detailName == "is_poisoned")        { ss << (is_poisoned() ? "Yes" : "No"); }
    else if (detailName == "Value")
    {
        // TODO this should not be modifying the girl
        cGirls::UpdateAskPrice(*const_cast<sGirl*>(this), false);
        ss << (int)g_Game->tariff().slave_price(*const_cast<sGirl*>(this), false);
    }
    else if (detailName == "SO")
    {
        /* */if (has_active_trait("Lesbian"))    ss << "L";
        else if (has_active_trait("Straight"))    ss << "S";
        else if (has_active_trait("Bisexual"))    ss << "B";
        else/*                       */    ss << "-";
    }
    else if (detailName == "SexAverage")
    {
        ss << (int)cGirls::GetAverageOfSexSkills(*this);
    }
    else if (detailName == "NonSexAverage")
    {
        ss << (int)cGirls::GetAverageOfNSxSkills(*this);
    }
    else if (detailName == "SkillAverage")
    {
        ss << (int)cGirls::GetAverageOfAllSkills(*this);
    }
    else /*                            */        { ss << "Not found"; }
    Data = ss.str();
}

int sGirl::rebel() const
{
    if (this->m_DayJob == JOB_INDUNGEON)    // `J` Dungeon "Matron" can be a Torturer from any brothel
        return cGirls::GetRebelValue(*this, random_girl_on_job(g_Game->buildings(), JOB_TORTURER, 0));
    if(m_Building)
        return cGirls::GetRebelValue(*this, m_Building->matron_count() > 0);
    g_LogFile.log(ELogLevel::ERROR, "Getting rebel value of girl '", FullName(), "' that is not associated with any building!");
    return 0;
}

bool sGirl::FixFreeTimeJobs()
{
    bool fixedD = false;
    bool fixedN = false;
    unsigned int dj = m_DayJob;
    if (dj == JOB_FILMFREETIME || dj == JOB_ARENAREST || dj == JOB_CENTREREST || dj == JOB_CLINICREST ||
        dj == JOB_HOUSEREST || dj == JOB_FARMREST || dj == JOB_RESTING || dj == 255)
    {
        auto old_job = m_DayJob;
        if(m_Building) {
            m_DayJob = m_Building->m_RestJob;
        } else {
            m_DayJob = JOB_RESTING;
        }


        fixedD = old_job != m_DayJob;
    }
    unsigned int nj = m_NightJob;
    if (nj == JOB_FILMFREETIME || nj == JOB_ARENAREST || nj == JOB_CENTREREST || nj == JOB_CLINICREST ||
        nj == JOB_HOUSEREST || nj == JOB_FARMREST || nj == JOB_RESTING || nj == 255)
    {
        auto old_job = m_NightJob;
        if(m_Building) {
            m_NightJob = m_Building->m_RestJob;
        } else {
            m_NightJob = JOB_RESTING;
        }

        fixedN = old_job != m_DayJob;
    }

    return fixedD || fixedN;
}

void sGirl::upd_temp_stat(int stat_id, int amount, bool usetraits)
{
    if (usetraits)
    {
        if (stat_id == STAT_LIBIDO)
        {
            if (has_active_trait("Nymphomaniac"))    { amount = int((double)amount * (amount > 0 ? 1.5 : 0.5));    if (amount == 0) amount = 1; }
            else if (has_active_trait("Chaste"))        { amount = int((double)amount * (amount > 0 ? 0.5 : 1.5));    if (amount == 0) amount = -1; }
        }
    }

    ICharacter::upd_temp_stat(stat_id, amount, false);
}

int sGirl::upd_temp_Enjoyment(Action_Types stat_id, int amount)
{
    m_EnjoymentTemps[stat_id] += amount;
    return get_enjoyment(stat_id);
}

int sGirl::upd_Enjoyment(Action_Types stat_id, int amount)
{
    assert(stat_id >= 0);
    m_Enjoyment[stat_id] += amount;
    /* */if (m_Enjoyment[stat_id] > 100)     m_Enjoyment[stat_id] = 100;
    else if (m_Enjoyment[stat_id] < -100)     m_Enjoyment[stat_id] = -100;

    return get_enjoyment(stat_id);
}

int sGirl::upd_Training(int stat_id, int amount, bool usetraits)
{
    m_Training[stat_id] += amount;
    /* */if (m_Training[stat_id] > 100)     m_Training[stat_id] = 100;
    else if (m_Training[stat_id] < 0)         m_Training[stat_id] = 0;
    return get_training(stat_id);
}

int sGirl::get_num_item_equiped(int Type) const
{
    return inventory().num_equipped_of_type(Type);
}

void sGirl::set_stat(int stat, int amount)
{
    switch (stat) {
    case STAT_AGE:        // age can be a special case so we start with that
        if (age() > 99) amount = 100;
        else if (age() > 80) amount = 80;
        break;
    case STAT_HEALTH:
        if (has_active_trait("Incorporeal"))    // Health and tiredness need the incorporeal sanity check
        {
            amount = 100;
        }
        break;
    case STAT_TIREDNESS:
        if (has_active_trait("Incorporeal") ||    // Health and tiredness need the incorporeal sanity check
                                           has_active_trait("Skeleton") ||
            has_active_trait("Zombie")) {
            amount = 0;
            return;
        }
        break;
    }
    ICharacter::set_stat(stat, amount);
}

bool sGirl::unequip(const sInventoryItem* item) {
    if (!item) return false;    // if already unequiped do nothing
    if(!inventory().remove_from_equipment(item)) return false;        // nothing was unequipped
    // unapply the effects
    for (u_int i = 0; i < item->m_Effects.size(); i++)
    {
        int eff_id = item->m_Effects[i].m_EffectID;
        int affects = item->m_Effects[i].m_Affects;
        int amount = item->m_Effects[i].m_Amount;

        if (affects == sEffect::Skill)    upd_mod_skill(eff_id, -amount);
        else if (affects == sEffect::Stat)    upd_mod_stat(eff_id, -amount);
        else if (affects == sEffect::Enjoy)    cGirls::UpdateEnjoymentMod(*this, eff_id, -amount);
        else if (affects == sEffect::GirlStatus)    // adds/removes status
        {
            if (amount == 1) m_States &= ~(1 << eff_id);        // add status
            else if (amount == 0) m_States |= (1 << eff_id);    // remove status
        }
    }

    cGirls::CalculateGirlType(*this);

    return true;
}

scripting::sAsyncScriptHandle sGirl::TriggerEvent(scripting::sEventID id)
{
    return m_EventMapping->RunAsync(id, *this);
}

std::shared_ptr<sGirl> sGirl::LoadFromTemplate(const tinyxml2::XMLElement& root)
{
    auto girl = std::make_shared<sGirl>(true);            // walk the XML DOM to get the girl data
    const char *pt;
    // get the simple fields
    girl->m_Name = GetStringAttribute(root, "Name");
    /// TODO fix name handling here!
    girl->m_FullName = girl->m_Name;

    if (root.QueryStringAttribute("Desc", &pt))            girl->m_Desc = pt;
    girl->m_Money = root.IntAttribute("Gold", 0);
    if (pt = root.Attribute("Catacombs"))        girl->m_States |= (strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0) ? (1u << STATUS_CATACOMBS) : (0u << STATUS_CATACOMBS);
    if (pt = root.Attribute("Slave"))            girl->m_States |= (strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0) ? (1u << STATUS_SLAVE) : (0u << STATUS_SLAVE);
    if (pt = root.Attribute("Arena"))            girl->m_States |= (strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0) ? (1u << STATUS_ARENA) : (0u << STATUS_ARENA);
    if (pt = root.Attribute("IsDaughter"))    girl->m_States |= (strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0) ? (1u << STATUS_ISDAUGHTER) : (0u << STATUS_ISDAUGHTER);

    for (int i = 0; i < NUM_STATS; i++) // loop through stats
    {
        const char *stat_name = get_stat_name((STATS)i);
        auto error = root.QueryAttribute(stat_name, &girl->m_Stats[i].m_Value);

        if (error != tinyxml2::XML_SUCCESS)
        {
            g_LogFile.log(ELogLevel::ERROR, "Can't find stat '", stat_name, "' for girl '", girl->m_Name,
                    "' - Setting it to default(", girl->m_Stats[i].m_Value, ").");
            continue;
        }
    }

    for (u_int i = 0; i < NUM_SKILLS; i++)    //    loop through skills
    {
        root.QueryAttribute(get_skill_name((SKILLS)i), &girl->m_Skills[i].m_Value);
    }

    if (pt = root.Attribute("Status"))
    {
        /* */if (strcmp(pt, "Catacombs") == 0)        girl->m_States |= (1u << STATUS_CATACOMBS);
        else if (strcmp(pt, "Slave") == 0)            girl->m_States |= (1u << STATUS_SLAVE);
        else if (strcmp(pt, "Arena") == 0)            girl->m_States |= (1u << STATUS_ARENA);
        else if (strcmp(pt, "Is Daughter") == 0)    girl->m_States |= (1u << STATUS_ISDAUGHTER);
        //        else    m_States = 0;
    }

    for (auto& child : IterateChildElements(root))
    {
        std::string tag = child.Value();
        if (tag == "Canonical_Daughters")
        {
            std::string s = child.Attribute("Name");
            girl->m_Canonical_Daughters.push_back(s);
        }
        if (tag == "Trait")    //get the trait name
        {
            pt = child.Attribute("Name");
            /// TODO (traits) allow inherent / permanent / inactive
            girl->raw_traits().add_inherent_trait(pt);
        }
        if (tag == "Item")    //get the item name
        {
            pt = child.Attribute("Name");
            sInventoryItem* item = g_Game->inventory_manager().GetItem(pt);
            if (item)
            {
                girl->add_item(item);
                if (item->m_Type != sInventoryItem::Food && item->m_Type != sInventoryItem::Makeup)
                {
                    girl->equip(item, false);
                }
            }
            else
            {
                g_LogFile.log(ELogLevel::ERROR, "Can't find Item: '", pt, "' - skipping it.");
            }

        }
    }

    if (root.QueryAttribute("Accomm", &girl->m_AccLevel) != tinyxml2::XML_SUCCESS) {
        girl->m_AccLevel = girl->is_slave() ? g_Game->settings().get_integer(settings::USER_ACCOMODATION_SLAVE) :
                g_Game->settings().get_integer(settings::USER_ACCOMODATION_FREE);
    }

    girl->raw_traits().update();

    return std::move(girl);
}

JOBS sGirl::get_job(bool night_shift) const {
    return JOBS(night_shift ? m_NightJob : m_DayJob);

}

void sGirl::FullJobReset(JOBS job) {
    m_PrevDayJob = m_PrevNightJob = m_YesterDayJob = m_YesterNightJob = m_DayJob = m_NightJob = job;
}

int sGirl::get_preg_duration() const {
    if(is_pregnant())
        return g_Game->settings().get_integer(has_status(STATUS_INSEMINATED) ? settings::PREG_DURATION_MONSTER: settings::PREG_DURATION_HUMAN);
    return -1;
}

void sGirl::set_default_house_percent() {
    house(g_Game->settings().get_integer(is_slave() ? settings::USER_HOUSE_PERCENT_SLAVE : settings::USER_HOUSE_PERCENT_FREE));
}

int sGirl::get_enjoyment(Action_Types actiontype) const {
    if (actiontype < 0) return 0;
    // Generic calculation
    int value = m_Enjoyment[actiontype] + get_trait_modifier(("enjoy:" + std::string(get_action_name(actiontype))).c_str()) +
                m_EnjoymentMods[actiontype] + m_EnjoymentTemps[actiontype];

    if (value < -100) value = -100;
    else if (value > 100) value = 100;
    return value;
}

int sGirl::get_training(int actiontype) const {
    if (actiontype < 0) return 0;
    // Generic calculation
    int value = m_Training[actiontype] + m_TrainingMods[actiontype] + m_TrainingTemps[actiontype];

    if (value < 0) value = 0;
    else if (value > 100) value = 100;
    return value;
}

bool sGirl::is_slave() const { return has_status(STATUS_SLAVE); }

bool sGirl::is_free() const { return !is_slave(); }

bool sGirl::is_monster() const { return has_status(STATUS_CATACOMBS); }

bool sGirl::is_human() const { return !is_monster(); }

bool sGirl::is_arena() const { return has_status(STATUS_ARENA); }

bool sGirl::is_isdaughter() const { return has_status(STATUS_ISDAUGHTER); }

void sGirl::set_slave() { set_status(STATUS_SLAVE); }

void sGirl::set_status(STATUS stat) {
    m_States |= (1u << stat);
}
bool sGirl::has_status(STATUS stat) const {
    return m_States & (1u << stat);
}
void sGirl::remove_status(STATUS stat) {
    m_States &= ~(1u << stat);
}

double sGirl::job_performance(JOBS job, bool estimate) const {
    const auto& job_handler = g_Game->job_manager().m_OOPJobs.at(job);
    assert(job_handler);
    return job_handler->GetPerformance(*this, estimate);
}

extern std::string process_message(const sGirl& girl, std::string message);
void sGirl::AddMessage(std::string message, int nImgType, EventType event) {
    m_Events.AddMessage(process_message(*this, std::move(message)), nImgType, event);
}
