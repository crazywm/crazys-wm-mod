#include "sGirl.hpp"
#include "CLog.h"
#include "XmlMisc.h"
#include "cJobManager.h"
#include "cTariff.h"
#include "IBuilding.hpp"
#include "Game.hpp"
#include "cGangs.h"
#include "cTraits.h"
#include "cCustomers.h"
#include "cInventory.h"
#include "cPlayer.h"
#include "src/buildings/cBrothel.h"

extern cRng g_Dice;
extern cConfig cfg;

bool is_Actress_Job(int testjob);
bool CrewNeeded(const IBuilding& building);

/*
* MOD: DocClox: Stuff for the XML loader code
*
* first: static members need declaring
*/
bool sGirl::m_maps_setup = false;
map<string, unsigned int> sGirl::jobs_lookup;

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

sGirl::sGirl()				// constructor
{
    // sGirl stuff
    m_newRandomFixed = -1;

    // Names
    m_Name = m_Realname = m_FirstName = m_MiddleName = m_Surname = m_MotherName = m_FatherName = m_Desc = "";

    // Time
    BirthMonth = BirthDay = 0;		m_BDay = 0;		m_WeeksPast = 0;

    // Jobs and money
    m_DayJob = m_NightJob = m_PrevDayJob = m_PrevNightJob = m_YesterDayJob = m_YesterNightJob = 255;
    m_WorkingDay = m_PrevWorkingDay = m_SpecialJobGoal = 0;

    m_Refused_To_Work_Day = m_Refused_To_Work_Night = false;
    m_Money = m_Pay = m_Tips = 0;
    m_NumCusts = m_NumCusts_old = 0;

    // Sex
    m_Virgin = -1;
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

    // Inventory
    m_NumInventory = 0;
    for (int i = 0; i < MAXNUM_GIRL_INVENTORY; i++)	{ m_EquipedItems[i] = 0; m_Inventory[i] = nullptr; }

    //Traits
    m_NumRememTraits = m_NumTraits = 0;
    for (int i = 0; i < MAXNUM_TRAITS; i++)			{ m_Traits[i] = nullptr; m_TempTrait[i] = 0; }
    for (int i = 0; i < MAXNUM_TRAITS * 2; i++)		{ m_RememTraits[i] = nullptr; }

    // Stats and skills
    for (u_int i = 0; i < NUM_SKILLS; i++)		// Added m_Skills here to zero out any that are not specified -- PP
        m_Skills[i] = m_SkillTemps[i] = m_SkillMods[i] = m_SkillTr[i] = 0;
    for (int i = 0; i < NUM_STATS; i++)			// Added m_Stats here to zero out any that are not specified -- PP
        m_Stats[i] = m_StatTemps[i] = m_StatMods[i] = m_StatTr[i] = 0;
    m_Stats[STAT_HEALTH] = 100;
    m_Stats[STAT_HAPPINESS] = 100;
    m_Stats[STAT_HOUSE] = 60;  // Moved from above so it is not zero'd out by above changes --PP

    // Enjoyment
    for (int i = 0; i < NUM_ACTIONTYPES; i++)	// `J` Added m_Enjoyment here to zero out any that are not specified
        m_Enjoyment[i] = m_EnjoymentTR[i] = m_EnjoymentMods[i] = m_EnjoymentTemps[i] = 0;
    for (u_int i = 0; i < NUM_ACTIONTYPES; i++)	// `J` randomize starting likes -10 to 10 most closer to 0
        m_Enjoyment[i] = (g_Dice.bell(-10, 10));

    // Training
    for (int i = 0; i < NUM_TRAININGTYPES; i++)	// Added m_Training here to zero out any that are not specified
        m_Training[i] = m_TrainingMods[i] = m_TrainingTemps[i] = 0;

    // Others
    for (int i = 0; i < NUM_GIRLFLAGS; i++)			{ m_Flags[i] = 0; }
    for (int i = 0; i < CHILD_COUNT_TYPES; i++)		{ m_ChildrenCount[i] = 0; }
    m_States = m_BaseStates = 0;
    m_FetishTypes.clear();



    // Other things that I'm not sure how their defaults would be set
    //	cEvents m_Events;
    //	cTriggerList m_Triggers;
    //	cChildList m_Children;
    //	vector<string> m_Canonical_Daughters;
}
sGirl::~sGirl()		// destructor
{
    m_Events.Free();
}

int sGirl::has_item(const std::string& item)
{
    for (int i = 0; i < MAXNUM_GIRL_INVENTORY; i++)
    {
        if (m_Inventory[i])
        {
            if (m_Inventory[i]->m_Name == item) return i;
        }
    }
    return -1;
}

string stringtolowerj(string name)
{
    string s;
    for (char i : name)
    {
        if (tolower(i) != tolower(' ') && tolower(i) != tolower('.') && tolower(i) != tolower(','))
        {
            s += tolower(i);
        }
    }
    return s;
}


int sGirl::has_item_j(const std::string& item)
{
    string s = stringtolowerj(item);
    for (int i = 0; i < MAXNUM_GIRL_INVENTORY; i++)
    {
        if (m_Inventory[i])
        {
            string t = stringtolowerj(m_Inventory[i]->m_Name);
            if (t == s)	return i;
        }
    }
    return -1;
}

int sGirl::add_inv(const sInventoryItem *item)
{
    if (!item)	return -1;
    int i;
    for (i = 0; i < MAXNUM_GIRL_INVENTORY; i++)
    {
        if (m_Inventory[i] == nullptr)
        {
            m_Inventory[i] = const_cast<sInventoryItem*>(item);
            m_NumInventory++;
            if (item->m_Type == INVMISC) equip(i, true);
            return i;  // MYR: return i for success, -1 for failure
        }
    }
    return -1;
}

bool sGirl::disobey_check(int action, JOBS job)
{
    int diff;
    int chance_to_obey = 0;							// high value - more likely to obey
    chance_to_obey = -cGirls::GetRebelValue(this, false, job);	// let's start out with the basic rebelliousness
    chance_to_obey += 100;							// make it range from 0 to 200
    chance_to_obey /= 2;							// get a conventional percentage value
    /*
    *	OK, let's factor in having a matron: normally this is done in GetRebelValue
    *	but matrons have shifts now, so really we want twice the effect for a matron
    *	on each shift as we'd get from just one. //corrected:(Either that, or we need to make this
    *	check shift dependent.)//
    *
    *	Anyway, the old version added 15 for a matron either shift. Let's add
    *	10 for each shift. Full coverage gets you 20 points
    */
    if (m_Building)
    { // `J` added building checks
        if (m_Building->matron_on_shift(SHIFT_DAY)) chance_to_obey += 10;
        if (m_Building->matron_on_shift(SHIFT_NIGHT)) chance_to_obey += 10;
    }
    /*
    *	This is still confusing - at least it still confuses me
    *	why not normalise the rebellion -100 to 100 value so it runs
    *	0 to 100, and invert it so it's basically an obedience check
    */

    switch (action)
    {
    case ACTION_COMBAT:
        // WD use best stat as many girls have only one stat high
        diff = max(combat(), magic()) - 50;
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
    chance_to_obey += m_Enjoyment[action];			// add in her enjoyment level
    chance_to_obey += pclove() / 10;					// let's add in some mods for love, fear and hate
    chance_to_obey += pcfear() / 10;
    chance_to_obey -= pchate() / 10;
    chance_to_obey += 30;									// Let's add a blanket 30% to all of that
    int roll = g_Dice.d100();								// let's get a percentage roll
    diff = chance_to_obey - roll;
    bool girl_obeys = (diff >= 0);
    if (girl_obeys)											// there's a price to be paid for relying on love or fear
    {
        if (diff < (pclove() / 10)) pclove(-1);	// if the only reason she obeys is love it wears away that love
        if (diff < (pcfear() / 10)) pcfear(-1);	// just a little bit. And if she's only doing it out of fear
    }
    /*
    *	do we need any more than this, really?
    *	we can add in some shaping factors if desired
    */

    return !girl_obeys;
}




// ----- Lookups
void sGirl::setup_maps()
{
    g_LogFile.os() << "[sGirl::setup_maps] Setting up Stats, Skills and Status codes." << endl;
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

int sGirl::lookup_jobs_code(string s)
{
    if (!m_maps_setup)	// only need to do this once
        setup_maps();

    // be useful to be able to log unrecognised type names here
    if (jobs_lookup.find(s) == jobs_lookup.end())
    {
        // `J` added check for missing jobs_lookup
        for (int i = 0; i < NUM_JOBS; i++)
        {
            if (g_Game->job_manager().JobQkNm[i] == s || g_Game->job_manager().JobName[i] == s)
                return i;
        }
        // if still not found, send original error message
        g_LogFile.os() << "[sGirl::jobs_enjoy_code] Error: unknown Job: " << s << endl;
        return -1;
    }
    return jobs_lookup[s];
}

string sGirl::lookup_where_she_is()
{
    if(m_Building)
        return m_Building->name();
    return "";
}

// END MOD


bool sGirl::equip(int slot, bool force) {
    if (can_equip(slot, force))
    {
        g_Game->inventory_manager().Equip(this, slot, force);
        return true;
    }
    return false;
}


int sGirl::get_skill(int skill_id) const
{
    int value = (m_Skills[skill_id]) + m_SkillTemps[skill_id] + m_SkillMods[skill_id] + m_SkillTr[skill_id];
    if (value > 100)	value = 100;
    // TODO do we want this? It requires some sort of global tracking of current shift
    /*
    if (has_trait("Vampire"))
    {
        if (g_Brothels.m_Processing_Shift == 0)			value -= 10;
        else if (g_Brothels.m_Processing_Shift == 1)	value += 10;
    }
     */
    if (value < 0)			value = 0;
    else if (value > 100)	value = 100;
    return value;
}

int sGirl::upd_stat(int stat_id, int amount, bool usetraits) {
    u_int stat = stat_id;
    int value = 0, min = 0, max = 100;
    switch (stat)
    {
        // `J` first do special cases that do not follow standard stat 0 to 100 rule
    case STAT_AGE:
        if (m_Stats[STAT_AGE] != 100 && amount != 0)	// WD: Dont change imortal age = 100
        {
            value = m_Stats[stat] + amount;
            if (value > 80)			value = 80;
            else if (value < 18)	value = 18;	// `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live
            m_Stats[stat] = value;
        }
        return get_stat(stat_id);
        break;
        // now for the stats with modifiers
    case STAT_HEALTH:
    case STAT_TIREDNESS:
        if (has_trait("Incorporeal"))
        {
            m_Stats[STAT_HEALTH] = 100;	// WD: Sanity - Incorporeal health should allways be at 100%
            m_Stats[STAT_TIREDNESS] = 0;	// WD: Sanity - Incorporeal Tiredness should allways be at 0%
            return get_stat(stat_id);
        }
        if (stat == STAT_TIREDNESS && (has_trait("Skeleton") || has_trait("Zombie")))
        {
            m_Stats[STAT_TIREDNESS] = 0;	// `J` Sanity - Zombie Tiredness should allways be at 0%
            return get_stat(stat_id);
        }
        if (!usetraits) break;

        if (has_trait("Fragile"))	// 20% health and 10% tired penalties
        {
            if (stat == STAT_HEALTH)
            {
                if (amount > 0)			// gain health - a little less than normal
                {	// reduce the intended amount healed by 20% or at least 1 except keeping at least 1 point healed
                    int mod = amount / 5;
                    if (mod < 1) mod = 1;
                    amount -= mod;
                    if (amount < 1) amount = 1;
                }
                else if (amount < 0)	// lose health - a little more than normal
                {	// increase the amound of damage taken by 20% or at least 1 extra point lost
                    int mod = -amount / 5;
                    if (mod < 1) mod = 1;
                    amount -= mod;
                }
                else // (amount == 0)	// no change intended but possibly injure her anyway
                {	// based on her constitution
                    if (!g_Dice.percent(constitution()))
                        amount -= 1;
                }
            }
            else	// STAT_TIREDNESS
            {
                if (amount > 0)			// gain tiredness
                {	// increase the amound of tiredness gained by 10% (+0 ok)
                    int mod = amount / 10;
                    amount += mod;
                }
                else if (amount < 0)	// lose tiredness
                {	// reduce the intended rest gained by 10% (-0 ok)
                    int mod = -amount / 10;
                    amount += mod;
                }
                else // (amount == 0)	// no change intended but possibly tire her anyway
                {	// based on her constitution but only 50% of the time
                    if (!g_Dice.percent(constitution()) && g_Dice.percent(50))
                        amount += 1;
                }
            }
        }
        if (has_trait("Delicate"))	// 20% tired and 10% health penalties
        {
            if (stat == STAT_HEALTH)
            {
                if (amount > 0)			// gain health - a little less than normal
                {	// reduce the intended amount healed by 10% (-0 ok) keeping at least 1 point healed
                    int mod = amount / 10;
                    amount -= mod;
                    if (amount < 1) amount = 1;
                }
                else if (amount < 0)	// lose health - a little more than normal
                {	// increase the amound of damage taken by 10% (+0 ok)
                    int mod = -amount / 10;
                    amount -= mod;
                }
                else // (amount == 0)	// no change intended but possibly injure her anyway
                {	// based on her constitution but only 50% of the time
                    if (!g_Dice.percent(constitution()) && g_Dice.percent(50))
                        amount -= 1;
                }
            }
            else	// STAT_TIREDNESS
            {
                if (amount > 0)			// gain tiredness
                {	// increase the amound of tiredness gained by 20% and at least 1
                    int mod = amount / 5;
                    if (mod < 1) mod = 1;
                    amount += mod;
                }
                else if (amount < 0)	// lose tiredness
                {	// decrease the amound of tiredness recovered by 20% and at least 1 but recovering at least 1
                    int mod = -amount / 5;
                    if (mod < 1) mod = 1;
                    amount += mod;
                    if (amount > -1) amount = -1;
                }
                else // (amount == 0)	// no change intended but possibly tire her anyway
                {	// based on her constitution
                    if (!g_Dice.percent(constitution()))
                        amount += 1;
                }
            }
        }
        if (has_trait("Tough"))	// 20% health and 10% tired bonuses
        {
            if (stat == STAT_HEALTH)
            {
                if (amount > 0)			// gain health
                {	// increase the amount of health gained by 20% and at least 1 point
                    int mod = amount / 5;
                    if (mod < 1) mod = 1;
                    amount += mod;
                }
                else if (amount < 0)	// lose health
                {	// reduce the amount of health lost by 20% and at least 1 but lose at least 1
                    int mod = -amount / 5;
                    if (mod < 1) mod = 1;
                    amount += mod;
                    if (amount > -1) amount = -1;
                }
                else // (amount == 0)	// no change intended but possibly heal her anyway
                {	// based on her constitution
                    if (g_Dice.percent(constitution()))
                        amount += 1;
                }
            }
            else	// STAT_TIREDNESS
            {
                if (amount > 0)			// gain tiredness
                {	// decrease the amount of tiredness gained by 10% (-0 ok) but gain at least 1
                    int mod = amount / 10;
                    amount -= mod;
                    if (amount < 1) amount = 1;
                }
                else if (amount < 0)	// lose tiredness
                {	// increase the amount of tiredness lost by 10% (+0 ok)
                    int mod = -amount / 10;
                    amount -= mod;
                }
                else // (amount == 0)	// no change intended but possibly rest her anyway
                {	// based on her constitution but only 50% of the time
                    if (g_Dice.percent(constitution()) && g_Dice.percent(50))
                        amount -= 1;
                }
            }
        }
        /*		`J` need another trait that will cover this
        if (has_trait("????????????"))		// 20% tired and 10% health bonuses
        {
        if (stat == STAT_HEALTH)
        {
        if (amount > 0)			// gain health
        {

        }
        else if (amount < 0)	// lose health
        {

        }
        else // (amount == 0)	// no change
        {

        }
        amount -= 2;
        }
        else	// STAT_TIREDNESS
        {
        if (amount > 0)			// gain tiredness
        {

        }
        else if (amount < 0)	// lose tiredness
        {

        }
        else // (amount == 0)	// no change
        {

        }
        amount += 2;
        }
        }
        //*/
        if (has_trait("Construct") && ((stat == STAT_HEALTH && amount < 0) || (stat == STAT_TIREDNESS && amount > 0)))
            amount = (int)ceil((float)amount * 0.1); // constructs take 10% damage
        break;

    case STAT_HAPPINESS:
        if (has_trait("Pessimist") && g_Dice % 5 == 1 && usetraits)	amount -= 1; // `J` added
        if (has_trait("Optimist") && g_Dice % 5 == 1 && usetraits)		amount += 1; // `J` added
        break;

    case STAT_LEVEL:
        max = 255;
        break;
    case STAT_EXP:
        max = 32000;
        break;
    case STAT_PCFEAR:
    case STAT_PCLOVE:
    case STAT_PCHATE:
    case STAT_MORALITY:
    case STAT_REFINEMENT:
    case STAT_DIGNITY:
    case STAT_LACTATION:
        min = -100;
        break;
        // and the rest
    default:
        break;
    }
    if (amount != 0)
    {
        value = m_Stats[stat] + amount;
        if (value > max)		value = max;
        else if (value < min)	value = min;
        m_Stats[stat] = value;
    }
    return get_stat(stat_id);
}

int sGirl::upd_skill(int skill_id, int amount, bool usetraits) {
    if (amount >= 0)
    {
        m_Skills[skill_id] = min(100, amount + m_Skills[skill_id]);
    }
    else
    {
        m_Skills[skill_id] = max(0, amount + m_Skills[skill_id]);
    }
    return get_skill(skill_id);
}

bool sGirl::check_virginity() {
    if (has_trait("Virgin") && m_Virgin == 1) // `J` if already correct settings then return true
    {
        return true;
    }
    else if (has_trait("Virgin"))	// `J` if not set correctly, set it correctly and return true
    {
        m_Virgin = 1;
        return true;
    }
    else if (m_Virgin == 1)	// `J` if not set correctly, set it correctly and return true
    {
        add_trait("Virgin");
        return true;
    }
    else if (m_Virgin == 0) // `J` if already correct settings then return false
    {
        return false;
    }
    else if (m_Stats[STAT_AGE] < 18)	// `J` If she just turned 18 she should not legally have had sex yet
    {
        m_Stats[STAT_AGE] = 18;	// `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live
        m_Virgin = 1;
        add_trait("Virgin");
        remove_trait("MILF");
        return true;
    }
    else	// `J` average all sex skills plus age
    {
        int totalsex = m_Stats[STAT_AGE];
        int div = 1;
        for (u_int i = 0; i < NUM_SKILLS; i++)
        {
            // `J` removed nonsex from virginity check
            if (i != SKILL_SERVICE && i != SKILL_MAGIC && i != SKILL_COMBAT && i != SKILL_MEDICINE && i != SKILL_PERFORMANCE && i != SKILL_COOKING &&
                i != SKILL_CRAFTING && i != SKILL_HERBALISM && i != SKILL_FARMING && i != SKILL_BREWING && i != SKILL_ANIMALHANDLING)
            {
                totalsex += m_Skills[i];
                div++;	// `J` added to allow new skills
            }
        }
        int avg = totalsex / div;	// `J` fixed to allow new skills
        if (avg < 20)
        {
            m_Virgin = 1;
            add_trait("Virgin");
            return true;
        }
        else
        {
            m_Virgin = 0;
            return false;
        }
    }
    if (m_Virgin == 1) add_trait("Virgin");
    else { m_Virgin = 0; remove_trait("Virgin"); }
    return (m_Virgin == 1);
}

bool sGirl::regain_virginity() {
    /*  Very similar to (and uses) AddTrait(). Added since trait "Virgin" created 04/14/2013.
    *	This includes capability for items, magic or other processes
    *	to have a "remove but remember" effect, like a "Belt of False Defloration"
    *	that provides a magical substitute vagina, preserving the original while worn.
    *	Well, why not?		DustyDan
    */

    bool traitOpSuccess = false;
    m_Virgin = 1;
    //	Let's avoid re-inventing the wheel
    traitOpSuccess = add_trait("Virgin", false, false, false);
    return traitOpSuccess;
}

// ----- Pregnancy, insemination & children

bool UseAntiPreg(sGirl *girl)
//	bool use, bool isClinic, bool isStudio, bool isArena, bool isCentre, bool isHouse, bool isFarm, int whereisshe)
{
    if (!girl->m_UseAntiPreg) return false;
    /*
    *	anti-preg potions, we probably should allow
    *	on-the-fly restocks. You can imagine someone
    *	noticing things are running low and
    *	sending a girl running to the shops to get
    *	a restock
    *
    *	that said, there's a good argument here for
    *	making this the matron's job, and giving it a
    *	chance dependent on skill level. Could have a
    *	comedy event where the matron forgets, or the
    *	girl forgets (or disobeys) and half a dozen
    *	girls get knocked up.
    *
    *	'course, we could do that anyway.. :)
    *
    *	`J` adjusted it so it uses your existing stock first
    *	before it buys extras at a higher cost as emergency stock
    *
    */
    auto building = girl->m_Building;
    if(building && building->provide_anti_preg()) {
        return !g_Dice.percent(cfg.pregnancy.anti_preg_failure());
    }

    return false;
}

// if contraception is TRUE, then she can't get pregnant which makes sense
static bool has_contraception(sGirl *girl)
{
    // `J` rearranged to speed up checks
    if (girl->has_trait("Skeleton") || girl->has_trait("Zombie"))	return true;	// Skeletons and Zombies can't get pregnant
    if (girl->has_trait("Sterile"))		return true;	// If she's Sterile, she can't get pregnant
    if (girl->is_pregnant())			return true;	// If she's pregnant, she shouldn't get pregnant
    if (girl->m_PregCooldown > 0) 		return true;	// If she's in her cooldown period after giving birth
    if (girl->m_DayJob == JOB_INDUNGEON || girl->m_NightJob == JOB_INDUNGEON)	// `J`
    {
        girl->m_Building = nullptr;
    }
    return UseAntiPreg(girl);
}

// returns false if she becomes pregnant or true if she does not
bool sGirl::calc_pregnancy(int chance, int type, const int stats[NUM_STATS], const int skills[NUM_SKILLS]) {
    /*
	*	If there's a condition that would stop her getting preggers
	*	then we get to go home early
	*
	*	return TRUE to indicate that pregnancy is FALSE
	*	(actually, supposed to mean that contraception is true,
	*	but it also applies for things like being pregnant,
	*	or just blowing the dice roll. That gets confusing too.
	*/
    if (has_contraception(this)) return true;

    string text = "She has";
    /*
    *	for reasons I do not understand, but nevertheless think
    *	are kind of cool, virgins have a +10 to their pregnancy
    *	chance
    */
    if (check_virginity() && chance > 0) chance += 10;
    /*
    *	the other effective form of contraception, of course,
    *	is failing the dice roll. Let's check the chance of
    *	her NOT getting preggers here
    */
    if (has_trait( "Broodmother") && chance > 0)	chance += 60;//this should work CRAZY
    else if (has_trait( "Fertile") && chance > 0)	chance += 30;//this should work CRAZY
    if (g_Dice.percent(100 - chance)) return true;
    /*
    *	narrative depends on what it was that Did The Deed
    *	specifically, was it human or not?
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
        type = STATUS_PREGNANT;		// `J` rearranged and added default to make sure there are no complications
        text += " become pregnant.";
        break;
    }

    m_Events.AddMessage(text, IMGTYPE_PREGNANT, EVENT_DANGER);
    cGirls::CreatePregnancy(this, 1, type, stats, skills);
    return false;
}

void sGirl::use_items()
{
    cGirls::UseItems(this);
}

void sGirl::add_tiredness()
{
    if (has_trait( "Incorporeal") || // Sanity check
        has_trait( "Skeleton") ||
        has_trait( "Zombie"))
    {
        set_stat(STAT_TIREDNESS, 0);	return;
    }
    int change = 10;
    if (constitution() > 0)
        change -= (constitution()) / 10;
    if (change <= 0)	change = 0;

    upd_stat(STAT_TIREDNESS, change, false);

    if (tiredness() == 100)
    {
        upd_stat(STAT_HAPPINESS, -1, false);
        upd_stat(STAT_HEALTH, -1, false);
    }
}

bool sGirl::fights_back()
{
    // `J` When adding new traits, search for "J-Add-New-Traits"  :  found in >> cBrothel > cBrothelManager::FightsBack
    if (health() < 10 || tiredness() > 90)/* */	return false;
    if (has_trait("Broken Will"))/*                */	return false;
    if (has_trait("Mind Fucked"))/*                */	return false;

    if (disobey_check(ACTION_COMBAT))/*            */	return true;
    int chance = 0;
    if (has_trait("Adventurer"))/*                 */	chance += 5;
    if (has_trait("Aggressive"))/*                 */	chance += 10;
    if (has_trait("Agile"))/*                      */	chance += 2;
    if (has_trait("Assassin"))/*                   */	chance += 10;
    if (has_trait("Audacity"))/*                   */	chance += 10;
    if (has_trait("Brawler"))/*                    */	chance += 5;
    if (has_trait("Canine"))/*                     */	chance += 2;
    if (has_trait("Cat Girl"))/*                   */	chance += 2;
    if (has_trait("Country Gal"))/*                */	chance += 2;
    if (has_trait("Demon"))/*                      */	chance += 5;
    if (has_trait("Dominatrix"))/*                 */	chance += 5;
    if (has_trait("Emprisoned Customer"))/*        */	chance += 10;
    if (has_trait("Fearless"))/*                   */	chance += 10;
    if (has_trait("Fleet of Foot"))/*              */	chance += 2;
    if (has_trait("Heroine"))/*                    */	chance += 5;
    if (has_trait("Hunter"))/*                     */	chance += 5;
    if (has_trait("Incorporeal"))/*                */	chance += 10;
    if (has_trait("Iron Will"))/*                  */	chance += 20;
    if (has_trait("Kidnapped"))/*                  */	chance += 15;
    if (has_trait("Manly"))/*                      */	chance += 5;
    if (has_trait("Merciless"))/*                  */	chance += 5;
    if (has_trait("Muscular"))/*                   */	chance += 5;
    if (has_trait("Open Minded"))/*                */	chance += 2;
    if (has_trait("Optimist"))/*                   */	chance += 2;
    if (has_trait("Pessimist"))/*                  */	chance += 2;
    if (has_trait("Powerful Magic"))/*             */	chance += 10;
    if (has_trait("Sadistic"))/*                   */	chance += 5;
    if (has_trait("Strong Magic"))/*               */	chance += 5;
    if (has_trait("Strong"))/*                     */	chance += 5;
    if (has_trait("Tomboy"))/*                     */	chance += 2;
    if (has_trait("Tough"))/*                      */	chance += 5;
    if (has_trait("Tsundere"))/*                   */	chance += 5;
    if (has_trait("Twisted"))/*                    */	chance += 5;
    if (has_trait("Yandere"))/*                    */	chance += 5;

    if (has_trait("Bad Eyesight"))/*               */	chance -= 2;
    if (has_trait("Bimbo"))/*                      */	chance -= 5;
    if (has_trait("Blind"))/*                      */	chance -= 5;
    if (has_trait("Bruises"))/*                    */	chance -= 2;
    if (has_trait("Clumsy"))/*                     */	chance -= 2;
    if (has_trait("Deaf"))/*                       */	chance -= 5;
    if (has_trait("Delicate"))/*                   */	chance -= 10;
    if (has_trait("Dependant"))/*                  */	chance -= 20;
    if (has_trait("Elegant"))/*                    */	chance -= 5;
    if (has_trait("Fragile"))/*                    */	chance -= 10;
    if (has_trait("Malformed"))/*                  */	chance -= 2;
    if (has_trait("Masochist"))/*                  */	chance -= 10;
    if (has_trait("Meek"))/*                       */	chance -= 20;
    if (has_trait("Nerd"))/*                       */	chance -= 5;
    if (has_trait("Nervous"))/*                    */	chance -= 5;
    if (has_trait("Retarded"))/*                   */	chance -= 10;
    if (has_trait("Shy"))/*                        */	chance -= 10;
    if (has_trait("One Eye"))/*                    */	chance -= 1;
    if (has_trait("No Hands"))/*                   */	chance -= 8;
    if (has_trait("One Arm"))/*                    */	chance -= 3;
    if (has_trait("No Arms"))/*                    */	chance -= 10;
    if (has_trait("One Foot"))/*                   */	chance -= 3;
    if (has_trait("No Feet"))/*                    */	chance -= 6;
    if (has_trait("One Leg"))/*                    */	chance -= 4;
    if (has_trait("No Legs"))/*                    */	chance -= 10;
    if (has_trait("Clipped Tendons"))/*            */	chance -= 8;

    return g_Dice.percent(chance);
}

int sGirl::get_stat(int stat_id) const
{
    // returns the total of stat + statmod + tempstat + stattr

    if (stat_id < 0) return 0;
    u_int stat = stat_id;
    int value = 0, min = 0, max = 100;
    /* */if (stat == STAT_AGE) min = 18;	// `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live
    else if (stat == STAT_EXP) max = 32000;
    else if (stat == STAT_LEVEL) max = 255;
    else if (stat == STAT_HEALTH	&& has_trait( "Incorporeal"))	return 100;
    else if (stat == STAT_TIREDNESS &&
             (has_trait( "Incorporeal") ||
              has_trait( "Skeleton") ||
              has_trait( "Zombie")))
        return 0;
    else if (stat == STAT_PCLOVE || stat == STAT_PCFEAR || stat == STAT_PCHATE || stat == STAT_MORALITY ||
             stat == STAT_REFINEMENT || stat == STAT_DIGNITY || stat == STAT_LACTATION) min = -100;
    // Generic calculation
    value = m_Stats[stat] + m_StatMods[stat] + m_StatTemps[stat] + m_StatTr[stat];

    if (value < min) value = min;
    else if (value > max) value = max;
    return value;
}

bool sGirl::remove_inv(int slot) {

    // Girl inventories don't stack items
    if (m_Inventory[slot] != nullptr)
    {
        unequip(slot);
        m_Inventory[slot] = nullptr;
        m_NumInventory--;
        return true;
    }
    return false;

}


sGirl *sGirl::run_away()
{
    if(m_Building)
        m_Building->remove_girl(this);
    if (m_NightJob == JOB_INDUNGEON)
        g_Game->dungeon().RemoveGirl(this);
    m_RunAway = 6;		// player has 6 weeks to retreive
    m_NightJob = m_DayJob = JOB_RUNAWAY;
    g_Game->AddGirlToRunaways(this);
    return nullptr;
}


// This load

bool sGirl::LoadGirlXML(TiXmlHandle hGirl)
{
    //this is always called after creating a new girl, so let's not init sGirl again
    TiXmlElement* pGirl = hGirl.ToElement();
    if (pGirl == nullptr) return false;
    int tempInt = 0;

    // load the name
    m_Name = pGirl->Attribute("Name");		// the name the girl is based on, also the name of the image folder

    // m_Realname = the name the girl is called in the game // `J` used to set to m_Name but now gets built from F+M+S names
    m_Realname = (pGirl->Attribute("Realname") ? pGirl->Attribute("Realname") : "");
    m_FirstName = (pGirl->Attribute("FirstName") ? pGirl->Attribute("FirstName") : "");	// `J` New
    m_MiddleName = (pGirl->Attribute("MiddleName") ? pGirl->Attribute("MiddleName") : "");	// `J` New
    m_Surname = (pGirl->Attribute("Surname") ? pGirl->Attribute("Surname") : "");	// `J` New
    if (m_Realname.empty() || (m_FirstName.empty() && m_MiddleName.empty() && m_Surname.empty())) cGirls::BuildName(this);

    m_Desc = (pGirl->Attribute("Desc") ? pGirl->Attribute("Desc") : "-");	// get the description

    // load the amount of days they are unhappy in a row
    pGirl->QueryIntAttribute("DaysUnhappy", &tempInt); m_DaysUnhappy = tempInt; tempInt = 0;

    // Load their traits
    g_Game->traits().LoadTraitsXML(hGirl.FirstChild("Traits"), m_NumTraits, m_Traits, m_TempTrait);
    if (m_NumTraits > MAXNUM_TRAITS)
        g_LogFile.write("--- ERROR - Loaded more traits than girls can have??");

    cGirls::MutuallyExclusiveTraits(this, true);	// cleanup traits
    cGirls::RemoveAllRememberedTraits(this);	// and clear any thing left after the cleanup

    // Load their remembered traits
    g_Game->traits().LoadTraitsXML(hGirl.FirstChild("Remembered_Traits"), m_NumRememTraits, m_RememTraits);
    if (m_NumRememTraits > MAXNUM_TRAITS * 2)
        g_LogFile.write("--- ERROR - Loaded more remembered traits than girls can have??");

    // Load inventory items
    LoadInventoryXML(hGirl.FirstChild("Inventory"), m_Inventory, m_NumInventory, m_EquipedItems);

    // load their states
    pGirl->QueryValueAttribute<long>("States", &m_States);

    // load their stats
    LoadStatsXML(hGirl.FirstChild("Stats"), m_Stats, m_StatMods, m_StatTemps);

    // load their skills
    LoadSkillsXML(hGirl.FirstChild("Skills"), m_Skills, m_SkillMods, m_SkillTemps);

    // load enjoyment values
    LoadActionsXML(hGirl.FirstChildElement("Actions"), m_Enjoyment, m_EnjoymentMods, m_EnjoymentTemps);

    // load training values
    LoadTrainingXML(hGirl.FirstChildElement("Training"), m_Training, m_TrainingMods, m_TrainingTemps);

    // load virginity
    pGirl->QueryIntAttribute("Virgin", &m_Virgin);

    // load using antipreg
    pGirl->QueryValueAttribute<bool>("UseAntiPreg", &m_UseAntiPreg);

    // load withdrawals
    pGirl->QueryIntAttribute("Withdrawals", &tempInt); m_Withdrawals = tempInt; tempInt = 0;

    // load money
    pGirl->QueryIntAttribute("Money", &m_Money);

    // load working day counter
    pGirl->QueryIntAttribute("WorkingDay", &m_WorkingDay);
    pGirl->QueryIntAttribute("PrevWorkingDay", &m_PrevWorkingDay);	// `J` added
    pGirl->QueryIntAttribute("SpecialJobGoal", &m_SpecialJobGoal);	// `J` added
    if (m_WorkingDay < 0)		m_WorkingDay = 0;
    if (m_PrevWorkingDay < 0)	m_PrevWorkingDay = 0;
    if (m_SpecialJobGoal < 0)	m_SpecialJobGoal = 0;

    // load acom level
    pGirl->QueryIntAttribute("AccLevel", &tempInt); m_AccLevel = tempInt; tempInt = 0;

    // `J` changeing jobs to save as quick codes in stead of numbers so if new jobs are added they don't shift jobs
    string tempst = pGirl->Attribute("DayJob");			m_DayJob = lookup_jobs_code(tempst);
    tempst = pGirl->Attribute("NightJob");				m_NightJob = lookup_jobs_code(tempst);
    tempst = pGirl->Attribute("PrevDayJob");			m_PrevDayJob = lookup_jobs_code(tempst);
    tempst = pGirl->Attribute("PrevNightJob");			m_PrevNightJob = lookup_jobs_code(tempst);
    tempst = pGirl->Attribute("YesterDayJob");			m_YesterDayJob = lookup_jobs_code(tempst);
    tempst = pGirl->Attribute("YesterNightJob");		m_YesterNightJob = lookup_jobs_code(tempst);

    // `J` now check if any of the jobs failed the lookup and see if they are numbers - aka use the old code
    // load day/night jobs
    if (m_DayJob < 0 || m_DayJob > 255)					{ pGirl->QueryIntAttribute("DayJob", &tempInt);			m_DayJob = tempInt; tempInt = 0; }
    if (m_NightJob < 0 || m_NightJob >255)				{ pGirl->QueryIntAttribute("NightJob", &tempInt);		m_NightJob = tempInt; tempInt = 0; }
    if (m_PrevDayJob < 0 || m_PrevDayJob >255)			{ pGirl->QueryIntAttribute("PrevDayJob", &tempInt);		m_PrevDayJob = tempInt; tempInt = 0; }
    if (m_PrevNightJob < 0 || m_PrevNightJob >255)		{ pGirl->QueryIntAttribute("PrevNightJob", &tempInt);	m_PrevNightJob = tempInt; tempInt = 0; }
    if (m_YesterDayJob < 0 || m_YesterDayJob >255)		{ pGirl->QueryIntAttribute("YesterDayJob", &tempInt);	m_YesterDayJob = tempInt; tempInt = 0; }
    if (m_YesterNightJob < 0 || m_YesterNightJob >255)	{ pGirl->QueryIntAttribute("YesterNightJob", &tempInt);	m_YesterNightJob = tempInt; tempInt = 0; }

    if (m_YesterDayJob < 0)		m_YesterDayJob = 255;
    if (m_YesterNightJob < 0)	m_YesterNightJob = 255;

    // load runnayway value
    pGirl->QueryIntAttribute("RunAway", &tempInt); m_RunAway = tempInt; tempInt = 0;

    // load spotted
    pGirl->QueryIntAttribute("Spotted", &tempInt); m_Spotted = tempInt; tempInt = 0;

    // load newRandomFixed
    tempInt = -1;
    pGirl->QueryIntAttribute("NewRandomFixed", &tempInt); m_newRandomFixed = tempInt; tempInt = 0;

    // load weeks past, birth day, and pregant time
    pGirl->QueryValueAttribute<unsigned long>("WeeksPast", &m_WeeksPast);
    pGirl->QueryValueAttribute<unsigned int>("BDay", &m_BDay);
    pGirl->QueryIntAttribute("WeeksPreg", &tempInt); m_WeeksPreg = tempInt; tempInt = 0;

    // load number of customers slept with
    pGirl->QueryValueAttribute<unsigned long>("NumCusts", &m_NumCusts);
    m_NumCusts_old = m_NumCusts;

    // load girl flags
    TiXmlElement* pFlags = pGirl->FirstChildElement("Flags");
    if (pFlags)
    {
        std::string flagNumber;
        for (int i = 0; i < NUM_GIRLFLAGS; i++)
        {
            flagNumber = "Flag_";
            std::stringstream stream;
            stream << i;
            flagNumber.append(stream.str());
            pFlags->QueryIntAttribute(flagNumber, &tempInt); m_Flags[i] = tempInt; tempInt = 0;
        }
    }

    // load their torture value
    pGirl->QueryValueAttribute<bool>("Tort", &m_Tort);

    // Load their children
    pGirl->QueryIntAttribute("PregCooldown", &tempInt); m_PregCooldown = tempInt; tempInt = 0;

    // load number of children
    TiXmlElement* pChildren = pGirl->FirstChildElement("Children");
    for (int i = 0; i < CHILD_COUNT_TYPES; i++)		// `J` added
    {
        pChildren->QueryIntAttribute(children_type_names[i], &tempInt);
        if (tempInt < 0 || tempInt>1000) tempInt = 0;
        m_ChildrenCount[i] = tempInt;
        tempInt = 0;
    }
    if (pChildren)
    {
        for (TiXmlElement* pChild = pChildren->FirstChildElement("Child"); pChild != nullptr; pChild = pChild->NextSiblingElement("Child"))
        {
            sChild* child = new sChild(false, sChild::Girl, 0);				// `J` prepare a minimal new child
            bool success = child->LoadChildXML(TiXmlHandle(pChild));	// because this will load over top of it
            if (success) { m_Children.add_child(child); }		        // add it if it loaded
            else { delete child; continue; }							// or delete the failed load
        }
    }


    // load their triggers
    m_Triggers.LoadTriggersXML(hGirl.FirstChildElement("Triggers"));
    m_Triggers.SetGirlTarget(this);



    cGirls::ApplyTraits(this);
    if (m_Stats[STAT_AGE] < 18) m_Stats[STAT_AGE] = 18;	// `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live

    cGirls::CalculateGirlType(this);

    // get the number of daugher names
    /*ifs  >>  temp >> ws;
    for(int i = 0; i < temp; i++) {
    string s;

    ifs >> s >> ws;
    current->m_Canonical_Daughters.push_back(s);*/

    return true;
}

// This save

TiXmlElement* sGirl::SaveGirlXML(TiXmlElement* pRoot)
{
    TiXmlElement* pGirl = new TiXmlElement("Girl");
    pRoot->LinkEndChild(pGirl);
    pGirl->SetAttribute("Name", m_Name);						// save the name
    pGirl->SetAttribute("Realname", m_Realname);				// save the real name
    pGirl->SetAttribute("FirstName", m_FirstName);				// save the first name
    pGirl->SetAttribute("MiddleName", m_MiddleName);			// save the middle name
    pGirl->SetAttribute("Surname", m_Surname);					// save the surname
    pGirl->SetAttribute("Desc", m_Desc);						// save the description
    pGirl->SetAttribute("DaysUnhappy", m_DaysUnhappy);			// save the amount of days they are unhappy

    // Save their traits
    if (m_NumTraits > MAXNUM_TRAITS) g_LogFile.write("---- ERROR - Saved more traits then girls can have");
    SaveTraitsXML(pGirl, "Traits", MAXNUM_TRAITS, m_Traits, m_TempTrait);

    // Save their remembered traits
    if (m_NumRememTraits > MAXNUM_TRAITS * 2) g_LogFile.write("---- ERROR - Saved more remembered traits then girls can have");
    SaveTraitsXML(pGirl, "Remembered_Traits", MAXNUM_TRAITS * 2, m_RememTraits, nullptr);

    // Save inventory items
    TiXmlElement* pInventory = new TiXmlElement("Inventory");
    pGirl->LinkEndChild(pInventory);
    SaveInventoryXML(pInventory, m_Inventory, 40, m_EquipedItems);

    pGirl->SetAttribute("States", m_States);					// save their states
    SaveStatsXML(pGirl, m_Stats, m_StatMods, m_StatTemps);		// Save their stats
    SaveSkillsXML(pGirl, m_Skills, m_SkillMods, m_SkillTemps);	// save their skills

    pGirl->SetAttribute("Virgin", m_Virgin);					// save virginity
    pGirl->SetAttribute("UseAntiPreg", m_UseAntiPreg);			// save using antipreg
    pGirl->SetAttribute("Withdrawals", m_Withdrawals);			// save withdrawals
    pGirl->SetAttribute("Money", m_Money);						// save money
    pGirl->SetAttribute("AccLevel", m_AccLevel);				// save acom level

    // save working day counter
    pGirl->SetAttribute("WorkingDay", m_WorkingDay);
    pGirl->SetAttribute("PrevWorkingDay", m_PrevWorkingDay);	// `J` added
    pGirl->SetAttribute("SpecialJobGoal", m_SpecialJobGoal);	// `J` added

    // `J` changed jobs to save as quick codes in stead of numbers so if new jobs are added they don't shift jobs
    // save day/night jobs
    if (m_DayJob < 0 || m_DayJob > NUM_JOBS) pGirl->SetAttribute("DayJob", "255");
    else pGirl->SetAttribute("DayJob", g_Game->job_manager().JobQkNm[m_DayJob]);
    if (m_NightJob < 0 || m_NightJob > NUM_JOBS) pGirl->SetAttribute("NightJob", "255");
    else pGirl->SetAttribute("NightJob", g_Game->job_manager().JobQkNm[m_NightJob]);

    // save prev day/night jobs
    if (m_PrevDayJob < 0 || m_PrevDayJob > NUM_JOBS) pGirl->SetAttribute("PrevDayJob", "255");
    else pGirl->SetAttribute("PrevDayJob", g_Game->job_manager().JobQkNm[m_PrevDayJob]);
    if (m_PrevNightJob < 0 || m_PrevNightJob > NUM_JOBS) pGirl->SetAttribute("PrevNightJob", "255");
    else pGirl->SetAttribute("PrevNightJob", g_Game->job_manager().JobQkNm[m_PrevNightJob]);

    // save prev day/night jobs
    if (m_YesterDayJob < 0 || m_YesterDayJob > NUM_JOBS) pGirl->SetAttribute("YesterDayJob", "255");
    else pGirl->SetAttribute("YesterDayJob", g_Game->job_manager().JobQkNm[m_YesterDayJob]);
    if (m_YesterNightJob < 0 || m_YesterNightJob > NUM_JOBS) pGirl->SetAttribute("YesterNightJob", "255");
    else pGirl->SetAttribute("YesterNightJob", g_Game->job_manager().JobQkNm[m_YesterNightJob]);

    pGirl->SetAttribute("RunAway", m_RunAway);					// save runnayway vale
    pGirl->SetAttribute("Spotted", m_Spotted);					// save spotted

    if (m_newRandomFixed >= 0)	pGirl->SetAttribute("NewRandomFixed", m_newRandomFixed);

    // save weeks past, birth day, and pregant time
    pGirl->SetAttribute("WeeksPast", m_WeeksPast);
    pGirl->SetAttribute("BDay", m_BDay);
    pGirl->SetAttribute("WeeksPreg", m_WeeksPreg);

    pGirl->SetAttribute("NumCusts", m_NumCusts);				// number of customers slept with

    // girl flags
    TiXmlElement* pFlags = new TiXmlElement("Flags");
    pGirl->LinkEndChild(pFlags);
    std::string flagNumber;
    for (int i = 0; i < NUM_GIRLFLAGS; i++)
    {
        flagNumber = "Flag_";
        std::stringstream stream;
        stream << i;
        flagNumber.append(stream.str());
        pFlags->SetAttribute(flagNumber, m_Flags[i]);
    }

    pGirl->SetAttribute("Tort", m_Tort);						// save their torture value

    // save their children
    pGirl->SetAttribute("PregCooldown", m_PregCooldown);
    TiXmlElement* pChildren = new TiXmlElement("Children");
    for (int i = 0; i < CHILD_COUNT_TYPES; i++)
    {
        if (m_ChildrenCount[i] < 0 || m_ChildrenCount[i] > 1000)	m_ChildrenCount[i] = 0;		// to correct girls without these
        pChildren->SetAttribute(children_type_names[i], m_ChildrenCount[i]);
    }

    pGirl->LinkEndChild(pChildren);
    sChild* child = m_Children.m_FirstChild;
    while (child)
    {
        child->SaveChildXML(pChildren);
        child = child->m_Next;
    }

    SaveActionsXML(pGirl, m_Enjoyment);							// save their enjoyment values
    SaveTrainingXML(pGirl, m_Training, m_TrainingMods, m_TrainingTemps);						// save their training values
    m_Triggers.SaveTriggersXML(pGirl);							// save their triggers

    return pGirl;

    /*unsigned int lim = current->m_Canonical_Daughters.size();
    ofs << lim << "\n";
    for(unsigned int i = 0; i < lim; i++) {
    ofs << current->m_Canonical_Daughters[i] << "\n";*/
}

bool sChild::LoadChildXML(TiXmlHandle hChild)
{
    TiXmlElement* pChild = hChild.ToElement();
    if (pChild == nullptr)
    {
        return false;
    }

    int tempInt = 0;
    pChild->QueryIntAttribute("Age", &tempInt); m_Age = tempInt; tempInt = 0;
    pChild->QueryValueAttribute<bool>("IsPlayers", &m_IsPlayers);
    pChild->QueryIntAttribute("Sex", &tempInt);	m_Sex = sChild::Gender(tempInt); tempInt = 0;
    pChild->QueryIntAttribute("Unborn", &tempInt); m_Unborn = tempInt; tempInt = 0;
    pChild->QueryIntAttribute("MultiBirth", &tempInt); m_MultiBirth = tempInt; tempInt = 0;	// `J` added
    if (m_MultiBirth < 1) m_MultiBirth = 1; if (m_MultiBirth > 5) m_MultiBirth = 5;			// `J` limited
    pChild->QueryIntAttribute("GirlsBorn", &tempInt); m_GirlsBorn = tempInt; tempInt = 0;	// `J` added
    if (m_GirlsBorn > m_MultiBirth) m_GirlsBorn = m_MultiBirth;								// `J` limited
    if (m_GirlsBorn < 0)	m_GirlsBorn = 0;												// `J` limited

    LoadStatsXML(hChild.FirstChild("Stats"), m_Stats);		// load their stats
    LoadSkillsXML(hChild.FirstChild("Skills"), m_Skills);	// load their skills
    return true;
}

TiXmlElement* sChild::SaveChildXML(TiXmlElement* pRoot)
{
    TiXmlElement* pChild = new TiXmlElement("Child");
    pRoot->LinkEndChild(pChild);
    pChild->SetAttribute("Age", m_Age);
    pChild->SetAttribute("IsPlayers", m_IsPlayers);
    pChild->SetAttribute("Sex", m_Sex);
    pChild->SetAttribute("Unborn", m_Unborn);
    pChild->SetAttribute("MultiBirth", m_MultiBirth);	// `J` added
    pChild->SetAttribute("GirlsBorn", m_GirlsBorn);		// `J` added
    SaveStatsXML(pChild, m_Stats);						// Save their stats
    SaveSkillsXML(pChild, m_Skills);					// save their skills
    return pChild;
}

// and an xml loader for sGirl
void sGirl::load_from_xml(TiXmlElement *el)
{
    int ival;
    const char *pt;
    // get the simple fields
    if (pt = el->Attribute("Name"))
    {
        m_Name = pt;
        m_Realname = pt;
        g_LogFile.os() << "Loading Girl : " << m_Realname << endl;
    }
    else
    {
        g_LogFile.os() << "Error: can't find name when loading girl." << "XML = " << (*el) << endl;
        return;
    }
    m_newRandomFixed = -1;

    if (pt = el->Attribute("Desc"))			m_Desc = pt;
    if (pt = el->Attribute("Gold", &ival))	m_Money = ival;
    if (pt = el->Attribute("Virgin"))		m_Virgin = (strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0) ? 1 : 0;
    if (pt = el->Attribute("Catacombs"))	m_States |= (strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0) ? (1 << STATUS_CATACOMBS) : (0 << STATUS_CATACOMBS);
    if (pt = el->Attribute("Slave"))		m_States |= (strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0) ? (1 << STATUS_SLAVE) : (0 << STATUS_SLAVE);
    if (pt = el->Attribute("Arena"))		m_States |= (strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0) ? (1 << STATUS_ARENA) : (0 << STATUS_ARENA);
    if (pt = el->Attribute("YourDaughter"))	m_States |= (strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0) ? (1 << STATUS_YOURDAUGHTER) : (0 << STATUS_YOURDAUGHTER);
    if (pt = el->Attribute("IsDaughter"))	m_States |= (strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0) ? (1 << STATUS_ISDAUGHTER) : (0 << STATUS_ISDAUGHTER);

    for (int i = 0; i < NUM_STATS; i++) // loop through stats
    {
        int ival;
        const char *stat_name = get_stat_name((STATS)i);
        pt = el->Attribute(stat_name, &ival);

        ostream& os = g_LogFile.os();
        if (pt == nullptr)
        {
            os << "Error: Can't find stat '" << stat_name << "' for girl '" << m_Realname << "' - Setting it to default." << endl;
            continue;
        }
        m_Stats[i] = ival;
        if (cfg.debug.log_girls())	os << "Debug: Girl='" << m_Realname << "'; Stat='" << stat_name << "'; Value='" << pt << "'; Ival = " << int(m_Stats[i]) << "'" << endl;
    }

    for (u_int i = 0; i < NUM_SKILLS; i++)	//	loop through skills
    {
        int ival;
        if (el->Attribute(get_skill_name((SKILLS)i), &ival))	m_Skills[i] = ival;
    }

    if (pt = el->Attribute("Status"))
    {
        /* */if (strcmp(pt, "Catacombs") == 0)		m_States |= (1u << STATUS_CATACOMBS);
        else if (strcmp(pt, "Slave") == 0)			m_States |= (1u << STATUS_SLAVE);
        else if (strcmp(pt, "Arena") == 0)			m_States |= (1u << STATUS_ARENA);
        else if (strcmp(pt, "Your Daughter") == 0)	m_States |= (1u << STATUS_YOURDAUGHTER);
        else if (strcmp(pt, "Is Daughter") == 0)	m_States |= (1u << STATUS_ISDAUGHTER);
        //		else	m_States = 0;
    }

    TiXmlElement * child;
    for (child = el->FirstChildElement(); child; child = child->NextSiblingElement())
    {
        if (child->ValueStr() == "Canonical_Daughters")
        {
            string s = child->Attribute("Name");
            m_Canonical_Daughters.push_back(s);
        }
        if (child->ValueStr() == "Trait")	//get the trait name
        {
            pt = child->Attribute("Name");
            m_Traits[m_NumTraits] = g_Game->traits().GetTrait(pt);
            m_NumTraits++;
        }
        if (child->ValueStr() == "Item")	//get the item name
        {
            pt = child->Attribute("Name");
            sInventoryItem* item = g_Game->inventory_manager().GetItem(pt);
            if (item)
            {
                m_Inventory[m_NumInventory] = item;
                if (item->m_Type != INVFOOD && item->m_Type != INVMAKEUP)
                {
                    equip(m_NumInventory, false);
                }
                m_NumInventory++;
            }
            else
            {
                g_LogFile.os() << "Error: Can't find Item: '" << pt << "' - skipping it." << endl;
                return;		// do as much as we can without crashing
            }

        }
    }

    if (el->Attribute("Accomm", &ival)) m_AccLevel = ival;
    else m_AccLevel = (is_slave() ? cfg.initial.slave_accom() : cfg.initial.girls_accom());

}

bool sGirl::can_equip(int num, bool force)
{
    if (force) return true;
    switch (m_Inventory[num]->m_Type)
    {
    case INVRING:			// worn on fingers (max 8)
        if (get_num_item_equiped(m_Inventory[num]->m_Type) >= 8) return false;
        break;

    case INVWEAPON:			// equiped on body, (max 2)
    case INVSMWEAPON:		// hidden on body, (max 2)
    case INVARMBAND:		// (max 2), worn around arms
        if (get_num_item_equiped(m_Inventory[num]->m_Type) >= 2) return false;
        break;

    case INVDRESS:			// Worn on body, (max 1)
    case INVUNDERWEAR:		// Worn on body, (max 1)
    case INVSHOES:			// worn on feet, (max 1)
    case INVNECKLACE:		// worn on neck, (max 1)
    case INVARMOR:			// worn on body over dresses (max 1)
    case INVHAT:			//
    case INVHELMET:			//
    case INVGLASSES:		//
    case INVSWIMSUIT:		//
    case INVCOMBATSHOES:	//
    case INVSHIELD:			//
        if (get_num_item_equiped(m_Inventory[num]->m_Type) >= 1) return false;
        break;

    case INVFOOD:	// Eaten, single use
    case INVMAKEUP:	// worn on face, single use
    case INVMISC:	// these items don't usually do anything just random stuff girls might buy. The ones that do, cause a constant effect without having to be equiped
    default:
        return true;
        break;
    }
    return true;
}

bool sGirl::lose_virginity()	{
    /*  Very similar to (and uses) RemoveTrait(). Added since trait "Virgin" created 04/14/2013.
    *	This includes capability for items, magic or other processes
    *	to have a "remove but remember" effect, like a "Belt of False Defloration"
    *	that provides a magical substitute vagina, preserving the original while worn.
    *	Well, why not?		DustyDan
    */

    bool traitOpSuccess = false;
    m_Virgin = 0;
    traitOpSuccess = remove_trait("Virgin");
    return traitOpSuccess;
}


bool sGirl::calc_pregnancy(int chance, sCustomer *cust)
{
    return calc_pregnancy(chance, STATUS_PREGNANT, cust->m_Stats, cust->m_Skills);
}

sChild::sChild(bool is_players, Gender gender, int MultiBirth)
{
    // set all the basics
    m_Unborn = 1;
    m_Age = 0;
    m_IsPlayers = is_players;
    if (gender == None && MultiBirth != 0)
        m_Sex = (g_Dice.is_girl() ? Girl : Boy);
    else m_Sex = gender;
    m_MultiBirth = 1;
    m_GirlsBorn = (m_Sex == Girl ? 1 : 0);
    m_Next = m_Prev = nullptr;
    for (u_int i = 0; i < NUM_SKILLS; i++)		// Added m_Skills here to zero out any that are not specified -- PP
        m_Skills[i] = 0;
    for (int i = 0; i < NUM_STATS; i++)			// Added m_Stats here to zero out any that are not specified -- PP
        m_Stats[i] = 0;
    m_Stats[STAT_HEALTH] = 100;
    m_Stats[STAT_HAPPINESS] = 100;

    if (MultiBirth == 0) return;	// 0 means we are creating a new child in order to load one so we can skip the rest

    // so now the first baby is ready, check if there are more
    m_MultiBirth = MultiBirth;

    int trycount = 1;
    double multichance = cfg.pregnancy.multi_birth_chance();
    while (g_Dice.percent(multichance) && m_MultiBirth < 5)
    {
        m_MultiBirth++;
        if (g_Dice.is_girl()) m_GirlsBorn++;	// check if the new one is a girl
        trycount++;
        multichance /= trycount;
    }
}


int sGirl::preg_chance(int base_pc, bool good, double factor)
{
    g_LogFile.ss() << "sGirl::preg_chance: " << "base %chance = " << base_pc << ", " << "good flag = " << good << ", " << "factor = " << factor;
    g_LogFile.ssend();
    double chance = base_pc;
    // factor is used to pass situational modifiers. For instance BDSM has a 25% reduction in chance
    chance *= factor;
    // if the sex was good, the chance is modded, again from the config file
    if (good) chance *= cfg.pregnancy.good_sex_factor();
    return int(chance);
}

bool sGirl::calc_pregnancy(cPlayer *player, bool good, double factor)
{
    double chance = preg_chance(cfg.pregnancy.player_chance(), good, factor);
    //	now do the calculation
    return calc_pregnancy(int(chance), STATUS_PREGNANT_BY_PLAYER, player->m_Stats, player->m_Skills);
}
bool sGirl::calc_group_pregnancy(cPlayer *player, bool good, double factor)
{
    double chance = preg_chance(cfg.pregnancy.player_chance(), good, factor);
    // player has 25% chance to be father (4 men in the group)
    int father = STATUS_PREGNANT;
    if (g_Dice.percent(25)) father = STATUS_PREGNANT_BY_PLAYER;
    // now do the calculation
    return calc_pregnancy(int(chance), father, player->m_Stats, player->m_Skills);
}
bool sGirl::calc_group_pregnancy(const sCustomer& cust, bool good, double factor)
{
    double chance = preg_chance(cfg.pregnancy.customer_chance(), good, factor);
    chance += cust.m_Amount;
    // now do the calculation
    return calc_pregnancy(int(chance), STATUS_PREGNANT, cust.m_Stats, cust.m_Skills);
}
bool sGirl::calc_pregnancy(const sCustomer& cust, bool good, double factor)
{
    double chance = preg_chance(cfg.pregnancy.customer_chance(), good, factor);
    return calc_pregnancy(int(chance), STATUS_PREGNANT, cust.m_Stats, cust.m_Skills);
}
bool sGirl::calc_insemination(const sCustomer& cust, bool good, double factor)
{
    double chance = preg_chance(cfg.pregnancy.monster_chance(), good, factor);
    return calc_pregnancy(int(chance), STATUS_INSEMINATED, cust.m_Stats, cust.m_Skills);
}
bool sGirl::calc_insemination(cPlayer *player, bool good, double factor)
{
    double chance = preg_chance(cfg.pregnancy.monster_chance(), good, factor);
    return calc_pregnancy(int(chance), STATUS_INSEMINATED, player->m_Stats, player->m_Skills);
}


bool sGirl::carrying_monster()			{ return(m_States & (1 << STATUS_INSEMINATED)) != 0; }
bool sGirl::carrying_human()			{ return carrying_players_child() || carrying_customer_child(); }
bool sGirl::carrying_players_child()	{ return(m_States & (1 << STATUS_PREGNANT_BY_PLAYER)) != 0; }
bool sGirl::carrying_customer_child()	{ return(m_States & (1 << STATUS_PREGNANT)) != 0; }
bool sGirl::is_pregnant()				{ return(m_States & (1 << STATUS_PREGNANT) || m_States & (1 << STATUS_PREGNANT_BY_PLAYER) || m_States & (1 << STATUS_INSEMINATED)); }
bool sGirl::is_mother()					{ return(m_States&(1 << STATUS_HAS_DAUGHTER) || m_States&(1 << STATUS_HAS_SON)); }
bool sGirl::is_poisoned()				{ return(m_States&(1 << STATUS_POISONED) || m_States&(1 << STATUS_BADLY_POISONED)); }
bool sGirl::has_weapon()		// `J` added .06.02.22
{
    if (this->m_NumInventory <= 0) return false;	// she has no items
    for (int i = 0; i < MAXNUM_GIRL_INVENTORY; i++)	// check all her items
    {
        if (this->m_Inventory[i] != nullptr && this->m_EquipedItems[i] == 1 &&
            (this->m_Inventory[i]->m_Type == INVWEAPON || this->m_Inventory[i]->m_Type == INVSMWEAPON))
            return true;							// and return true if she has a weapon equiped
    }
    return false;
}
void sGirl::clear_dating()				{ m_States &= ~(1 << STATUS_DATING_PERV); m_States &= ~(1 << STATUS_DATING_MEAN); m_States &= ~(1 << STATUS_DATING_NICE); }
void sGirl::clear_pregnancy()
{
    m_States &= ~(1 << STATUS_PREGNANT);
    m_States &= ~(1 << STATUS_PREGNANT_BY_PLAYER);
    m_States &= ~(1 << STATUS_INSEMINATED);
    m_WeeksPreg = 0;
    sChild* leftover = (this)->m_Children.m_FirstChild;
    while (leftover)
    {
        leftover = (this)->next_child(leftover, (leftover->m_Unborn > 0));
    }

}


bool sGirl::add_trait(string name, int temptime, bool removeitem, bool remember)
{
    /*
*	WD: Added logic for remembered trait
*
*		removeitem = true Will add to Remember
*		trait list if the trait is already active
*		Used with items / efects may be removed
*		later eg items - rings
*
*		inrememberlist = true only add trait if
*		exists in the remember list and remove
*		from the list. Use mainly with unequiping
*		items and Trait overiding in ApplyTraits()
*
*		Returns true if trait made active or added
*		trait remember list.
*
*		This should fix items with duplicate
*		traits and loss of original trait if
*		overwritten by a trait from an item
*		that is later removed
*
*/

    if (has_trait(name))
    {
        if (removeitem)								//	WD: Overwriting existing trait with removable item / effect
            cGirls::AddRememberedTrait(this, name);			//	WD:	Save trait for when item is removed
        return true;
    }

    if (remember)								// WD: Add trait only if it is in the Remember List
    {
        if (cGirls::HasRememberedTrait(this, name)) cGirls::RemoveRememberedTrait(this, name);
        else return false;							//	WD:	No trait to add
    }

    for (int i = 0; i < MAXNUM_TRAITS; i++)				// add the trait
    {
        if (m_Traits[i] == nullptr)
        {
            if (temptime>0) m_TempTrait[i] = temptime;
            m_NumTraits++;
            m_Traits[i] = g_Game->traits().GetTrait(name);
            // if we did not find the trait in the traits list, there is nothing we can do here.
            if(m_Traits[i] == nullptr) {
                return false;
            }

            cGirls::MutuallyExclusiveTrait(this, true, m_Traits[i], removeitem);
            cGirls::ApplyTraits(this, m_Traits[i]);

            return true;
        }
    }
    return false;
}
bool sGirl::remove_trait(string name,  bool addrememberlist, bool force, bool keepinrememberlist)
{
    /*
*	WD: Added logic for remembered trait
*
*		addrememberlist = true = will move the trait from active to the remember list
*		addrememberlist = false = will remove the trait from active but not add it to the remember list
*
*		force = false = will ignore this check
*		force = true = will remove the trait entirely
*
*		keepinrememberlist = false = will ignore this check
*		keepinrememberlist = true will add it to the remember list even if it is completely removed
*		`J` added - this is for when an item tries to add a trait that the girl can not possible get because she has a trait that precludes it
*			ie. adding "Perky Nipples" to a girl who has "No Nipples", if "No Nipples" gets removed the "Perky Nipples" will show themselves
*
*		Returns true if there was a active trait or remember list entry removed
*
*		This should fix items with duplicate traits and loss of original trait if
*		overwritten by a trait from an item that is later removed
*/

    bool hasRemTrait = cGirls::HasRememberedTrait(this, name);

    if (!has_trait(name))							// WD:	no active trait to remove
    {
        if (hasRemTrait && !keepinrememberlist)
        {	// WD:	try remembered trait // `J` only if we want to remove it
            cGirls::RemoveRememberedTrait(this, name);
            return true;
            // `J` explain - she had the trait removed before and it is getting removed again so remove it for good
        }
        if (!hasRemTrait && keepinrememberlist)
        {	// `J` if she does not have it at all but we want her to remember trying to get it
            cGirls::AddRememberedTrait(this, name);
        }
        return false;	// otherwise just return false
    }
    // `J` - so she has the trait active at this point...

    if (!force && hasRemTrait)	//	WD:	has remembered trait so don't touch active trait unless we are forcing removal of active trait
    {
        cGirls::RemoveRememberedTrait(this, name);
        return true;
        // `J` explain - she had the trait in both active and remembered so instead of removing active and replacing with remembered, just remove remembered
    }

    //	WD:	save trait to remember list before removing
    if (addrememberlist || keepinrememberlist) cGirls::AddRememberedTrait(this, name);

    //	WD: Remove trait
    TraitSpec* trait = g_Game->traits().GetTrait(name);
    for (int i = 0; i < MAXNUM_TRAITS; i++)			// remove the traits
    {
        if (m_Traits[i] && m_Traits[i] == trait)
        {
            m_NumTraits--;

            cGirls::MutuallyExclusiveTrait(this, false, m_Traits[i]);
            cGirls::ApplyTraits(this);

            if (m_TempTrait[i] > 0) m_TempTrait[i] = 0;
            m_Traits[i] = nullptr;
            return true;
        }
    }
    return false;
}

bool sGirl::has_trait(string trait) const
{
    for (int i = 0; i < MAXNUM_TRAITS; i++)
    {
        if (m_Traits[i])
        {
            if (trait.compare(m_Traits[i]->name()) == 0) return true;
        }
    }
    return false;
}

int sGirl::breast_size()	// `J` Breast size number, normal is 4, 1 is flat, max is 10
{
    /* */if (this->has_trait("Flat Chest"))				return 1;	// No bra needed
    else if (this->has_trait("Petite Breasts"))			return 2;	// A cup
    else if (this->has_trait("Small Boobs"))			return 3;	// B cup
    else if (this->has_trait("Busty Boobs"))			return 5;	// D cup
    else if (this->has_trait("Big Boobs"))				return 6;	// E-F cup
    else if (this->has_trait("Giant Juggs"))			return 7;	// G-H cup
    else if (this->has_trait("Massive Melons"))			return 8;	// I-J cup
    else if (this->has_trait("Abnormally Large Boobs"))	return 9;	// K-M cup
    else if (this->has_trait("Titanic Tits"))			return 10;	// N+ cup
    return 4;														// C cup
}
bool sGirl::is_dead(bool sendmessage) const
{
    if (this->health() <= 0)
    {
        if (sendmessage)
        {
            stringstream ss; ss << this->m_Realname << " is dead. She isn't going to work anymore and her body will be removed by the end of the week.";
            g_Game->push_message(ss.str(), 1);
        }
        return true;
    }
    return false;

}
bool sGirl::is_addict(bool onlyhard)	// `J` added bool onlyhard to allow only hard drugs to be checked for
{
    if (onlyhard)
    {
        return	has_trait("Shroud Addict") ||
                  has_trait("Fairy Dust Addict") ||
                  has_trait("Viras Blood Addict");
    }
    return	has_trait("Shroud Addict") ||
              has_trait("Fairy Dust Addict") ||
              has_trait("Smoker") ||
              has_trait("Alcoholic") ||
              has_trait("Cum Addict") ||
              has_trait("Viras Blood Addict");
}
bool sGirl::has_disease()
{
    return	has_trait("AIDS") ||
              has_trait("Herpes") ||
              has_trait("Chlamydia") ||
              has_trait("Syphilis");
}
bool sGirl::is_fighter(bool canbehelped)
{
    if (canbehelped)
    {
        return	has_trait("Aggressive") ||
                  has_trait("Yandere") ||
                  has_trait("Tsundere");
    }
    return	has_trait("Aggressive") ||
              has_trait("Assassin") ||
              has_trait("Yandere") ||
              has_trait("Brawler") ||
              has_trait("Tsundere");
}

sChild* sGirl::next_child(sChild* child, bool remove)
{
    if (!remove)
    {
        return child->m_Next;
    }
    return m_Children.remove_child(child, this);
}

int sGirl::preg_type(int image_type)
{
    int new_type = image_type + PREG_OFFSET;
    /*
    *		if the new image type is >=  NUM_IMGTYPES
    *		then it was one of the types that doesn't have
    *		an equivalent pregnant form
    */
    if (new_type >= NUM_IMGTYPES)
    {
        return image_type;
    }
    return new_type;
}

bool sGirl::is_resting()
{
    return ((m_DayJob == JOB_FILMFREETIME	&& m_NightJob == JOB_FILMFREETIME) ||
            (m_DayJob == JOB_ARENAREST		&& m_NightJob == JOB_ARENAREST) ||
            (m_DayJob == JOB_CENTREREST		&& m_NightJob == JOB_CENTREREST) ||
            (m_DayJob == JOB_CLINICREST		&& m_NightJob == JOB_CLINICREST) ||
            (m_DayJob == JOB_HOUSEREST		&& m_NightJob == JOB_HOUSEREST) ||
            (m_DayJob == JOB_FARMREST		&& m_NightJob == JOB_FARMREST) ||
            (m_DayJob == JOB_RESTING		&& m_NightJob == JOB_RESTING));
}
bool sGirl::is_havingsex()
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
bool sGirl::was_resting()
{
    return ((m_PrevDayJob == JOB_FILMFREETIME	&& m_PrevNightJob == JOB_FILMFREETIME) ||
            (m_PrevDayJob == JOB_ARENAREST		&& m_PrevNightJob == JOB_ARENAREST) ||
            (m_PrevDayJob == JOB_CENTREREST		&& m_PrevNightJob == JOB_CENTREREST) ||
            (m_PrevDayJob == JOB_CLINICREST		&& m_PrevNightJob == JOB_CLINICREST) ||
            (m_PrevDayJob == JOB_HOUSEREST		&& m_PrevNightJob == JOB_HOUSEREST) ||
            (m_PrevDayJob == JOB_FARMREST		&& m_PrevNightJob == JOB_FARMREST) ||
            (m_PrevDayJob == JOB_RESTING		&& m_PrevNightJob == JOB_RESTING));
}


void sGirl::OutputGirlRow(vector<string>& Data, const vector<string>& columnNames)
{
    Data.resize(columnNames.size());
    for (unsigned int x = 0; x < columnNames.size(); ++x)
    {
        //for each column, write out the statistic that goes in it
        OutputGirlDetailString(Data[x], columnNames[x]);
    }
}

void sGirl::OutputGirlDetailString(string& Data, const string& detailName)
{
    //given a statistic name, set a string to a value that represents that statistic
    static stringstream ss;
    ss.str("");

    bool interrupted = false;	// `J` added
    if (this->m_YesterDayJob != (this)->m_DayJob &&
        (cJobManager::is_Surgery_Job((this)->m_YesterDayJob) || (this)->m_YesterDayJob == JOB_REHAB) &&
        (((this)->m_WorkingDay > 0) || (this)->m_PrevWorkingDay > 0))
        interrupted = true;

    /* */if (detailName == "Name")				{ ss << m_Realname; }
    else if (detailName == "Health")			{ if (get_stat(STAT_HEALTH) <= 0) ss << "DEAD"; else ss << get_stat(STAT_HEALTH) << "%"; }
    else if (detailName == "Age")				{ if (get_stat(STAT_AGE) == 100) ss << "???"; else ss << get_stat(STAT_AGE); }
    else if (detailName == "Libido")			{ ss << libido(); }
    else if (detailName == "Rebel")				{ ss << rebel(); }
    else if (detailName == "Looks")				{ ss << ((get_stat(STAT_BEAUTY) + get_stat(STAT_CHARISMA)) / 2) << "%"; }
    else if (detailName == "Tiredness")			{ ss << get_stat(STAT_TIREDNESS) << "%"; }
    else if (detailName == "Happiness")			{ ss << get_stat(STAT_HAPPINESS) << "%"; }
    else if (detailName == "Virgin")			{ ss << (this->check_virginity() ? "Yes" : "No"); }
    else if (detailName == "Weeks_Due")
    {
        if (is_pregnant())
        {
            int to_go = ((this)->m_States&(1 << STATUS_INSEMINATED) ? cfg.pregnancy.weeks_monster_p() : cfg.pregnancy.weeks_pregnant()) - (this)->m_WeeksPreg;
            ss << to_go;
        }
        else
        {
            ss << "---";
        }
    }
    else if (detailName == "PregCooldown")		{ ss << m_PregCooldown; }
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
    else if (detailName == "Pay")				{ ss << m_Pay; }

        // 'J' Added for .06.03.01
    else if (detailName == "DayJobShort" || detailName == "NightJobShort")
    {
        ss << g_Game->job_manager().JobQkNm[(detailName == "DayJobShort" ? m_DayJob : m_NightJob)];
    }

        // 'J' Girl Table job text
    else if (detailName == "DayJob" || detailName == "NightJob")
    {
        bool DN_Day = detailName == "NightJob";
        int DN_Job = (DN_Day ? m_NightJob : m_DayJob);

        // `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >>
        if (DN_Job >= NUM_JOBS)
        {
            ss << "None";
        }
        else if (DN_Job == JOB_FAKEORGASM || DN_Job == JOB_SO_STRAIGHT || DN_Job == JOB_SO_BISEXUAL || DN_Job == JOB_SO_LESBIAN)
        {
            ss << g_Game->job_manager().JobName[DN_Job] << " (" << m_WorkingDay << "%)";
        }
        else if (DN_Job == JOB_CUREDISEASES)
        {
            if (m_Building->num_girls_on_job(JOB_DOCTOR, DN_Day) > 0)
            {
                ss << g_Game->job_manager().JobName[DN_Job] << " (" << m_WorkingDay << "%)";
            }
            else
            {
                ss << g_Game->job_manager().JobName[DN_Job] << " (" << m_WorkingDay << "%) **";
            }
        }
        else if (DN_Job == JOB_REHAB || DN_Job == JOB_ANGER || DN_Job == JOB_EXTHERAPY || DN_Job == JOB_THERAPY)
        {
            if (m_Building->num_girls_on_job(JOB_COUNSELOR, DN_Day) > 0)
            {
                ss << g_Game->job_manager().JobName[DN_Job] << " (" << 3 - m_WorkingDay << ")";
            }
            else
            {
                ss << g_Game->job_manager().JobName[DN_Job] << " (?)***";
            }
        }
        else if (DN_Job == JOB_GETHEALING)
        {
            if (m_Building->num_girls_on_job(JOB_DOCTOR, DN_Day) > 0)
            {
                ss << g_Game->job_manager().JobName[DN_Job];
            }
            else
            {
                ss << g_Game->job_manager().JobName[DN_Job] << " ***";
            }
        }
        else if (DN_Job == JOB_GETREPAIRS)
        {
            if (m_Building->num_girls_on_job(JOB_MECHANIC, DN_Day) > 0 &&
                (this->has_trait("Construct") || this->has_trait("Half-Construct")))
            {
                ss << g_Game->job_manager().JobName[DN_Job];
            }
            else if (this->has_trait("Construct"))
            {
                ss << g_Game->job_manager().JobName[DN_Job] << " ****";
            }
            else
            {
                ss << g_Game->job_manager().JobName[DN_Job] << " !!";
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
                ss << g_Game->job_manager().JobName[DN_Job] << " (" << wdays << ")*";
            }
            else
            {
                ss << g_Game->job_manager().JobName[DN_Job] << " (?)***";
            }
        }
        else if (cJobManager::is_Surgery_Job(DN_Job))
        {
            int wdays = (5 - (this)->m_WorkingDay);
            if (m_Building->num_girls_on_job(JOB_NURSE, DN_Day) > 0)
            {
                if (wdays >= 3)		{ wdays = 3; }
                else if (wdays > 1)	{ wdays = 2; }
                else				{ wdays = 1; }
            }
            if (m_Building->num_girls_on_job(JOB_DOCTOR, DN_Day) > 0)
            {
                ss << g_Game->job_manager().JobName[DN_Job] << " (" << wdays << ")*";
            }
            else
            {
                ss << g_Game->job_manager().JobName[DN_Job] << " (?)***";
            }
        }
        else if (is_Actress_Job(DN_Job) && CrewNeeded(*m_Building))
        {
            ss << g_Game->job_manager().JobName[DN_Job] << " **";
        }
        else if (is_resting() && !was_resting() && m_PrevDayJob != 255 && m_PrevNightJob != 255)
        {
            ss << g_Game->job_manager().JobName[DN_Job];
            ss << " (" << g_Game->job_manager().JobQkNm[(DN_Day == 0 ? m_PrevDayJob : m_PrevNightJob)] << ")";
        }
        else
        {
            ss << g_Game->job_manager().JobName[DN_Job];
        }
        if (interrupted)
        {
            ss << " **";
        }
    }

    else if (detailName.find("STAT_") != string::npos)
    {
        string stat = detailName;
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
    else if (detailName.find("SKILL_") != string::npos)
    {
        string skill = detailName;
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
    else if (detailName.find("TRAIT_") != string::npos)
    {
        string trait = detailName;
        trait.replace(0, 6, "");
        if (this->has_trait(trait))
        {
            ss << "Yes";
        }
        else
        {
            ss << "No";
        }
    }
    else if (detailName.find("STATUS_") != string::npos)
    {
        string status = detailName;
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
        if (this->check_virginity()) ss << "Vg.";
        else if (is_pregnant())
        {
            int to_go = ((this)->m_States&(1u << STATUS_INSEMINATED) ? cfg.pregnancy.weeks_monster_p() :
                         cfg.pregnancy.weeks_pregnant()) - (this)->m_WeeksPreg;
            if (carrying_players_child())	ss << "Yours";
            else if (carrying_monster())	ss << "Beast";
            else /*                      */	ss << "Yes";
            if (has_trait("Sterile") || has_trait("Zombie") || has_trait("Skeleton"))
                ss << "?" << to_go << "?";	// how?
            else
                ss << "(" << to_go << ")";
        }
        else if (m_PregCooldown > 0)
        {
            ss << "No";
            if (has_trait("Sterile") || has_trait("Zombie") || has_trait("Skeleton"))
                ss << "!" << m_PregCooldown << "!";
            else
                ss << "(" << m_PregCooldown << ")";
        }
        else if (has_trait("Zombie") || has_trait("Skeleton")) ss << "Ud.";
        else if (has_trait("Sterile"))	    ss << "St.";
        else if (has_trait("Fertile"))      ss << "No+";
        else if (has_trait("Broodmother"))  ss << "No++";
        else                                ss << "No";
    }
    else if (detailName == "is_slave")			{ ss << (is_slave() ? "Yes" : "No"); }
    else if (detailName == "carrying_human")	{ ss << (carrying_human() ? "Yes" : "No"); }
    else if (detailName == "is_addict")			{ ss << (is_addict() ? "Yes" : "No"); }
    else if (detailName == "has_disease")		{ ss << (has_disease() ? "Yes" : "No"); }
    else if (detailName == "is_mother")			{ ss << (is_mother() ? "Yes" : "No"); }
    else if (detailName == "is_poisoned")		{ ss << (is_poisoned() ? "Yes" : "No"); }
    else if (detailName == "Value")
    {
        cGirls::CalculateAskPrice(this, false);
        ss << (int)g_Game->tariff().slave_price(this, false);
    }
    else if (detailName == "SO")
    {
        /* */if (has_trait("Lesbian"))	ss << "L";
        else if (has_trait("Straight"))	ss << "S";
        else if (has_trait("Bisexual"))	ss << "B";
        else/*                       */	ss << "-";
    }
    else if (detailName == "SexAverage")
    {
        ss << (int)cGirls::GetAverageOfSexSkills(this);
    }
    else if (detailName == "NonSexAverage")
    {
        ss << (int)cGirls::GetAverageOfNSxSkills(this);
    }
    else if (detailName == "SkillAverage")
    {
        ss << (int)cGirls::GetAverageOfAllSkills(this);
    }
    else /*                            */		{ ss << "Not found"; }
    Data = ss.str();
}

int sGirl::rebel()
{
    // return cGirls::GetRebelValue(this, this->m_DayJob == JOB_MATRON); // `J` old version
    if (this->m_DayJob == JOB_INDUNGEON)	// `J` Dungeon "Matron" can be a Torturer from any brothel
        return cGirls::GetRebelValue(this, random_girl_on_job(g_Game->buildings(), JOB_TORTURER, 0));
    if(m_Building)
        return cGirls::GetRebelValue(this, m_Building->matron_count() > 0);
    g_LogFile.ss() << "Error: Getting rebel value of girl that is not associated with any building!\n";
    g_LogFile.ssend();
    return 0;
}

// `J` .06.01.17 condensed and added log_extradetails
string sGirl::JobRating(double value, string type, string name)
{
    stringstream jr;
    jr << JobRatingLetter(value) << "  " << type << "  " << name;
    if (cfg.debug.log_extradetails()) jr << "   ( " << (int)value << " )";
    jr << "\n";
    return jr.str();
}

string sGirl::JobRatingLetter(double value)
{
    /* */if (value < -500)		return "X    ";	// Can not do this job
    else if (value == 0)		return "0    ";	// Bad input
    else if (value >= 350)		return "   I  ";	// Incomparable
    else if (value >= 245)		return "  S  ";	// Superior
    else if (value >= 185)		return " A   ";	// Amazing
    else if (value >= 145)		return "B    "; // Better
    else if (value >= 100)		return "C    ";	// Can do it
    else if (value >= 70)		return "D    ";	// Don't bother
    else						return "E    "; // Expect Failure
}

bool sGirl::FixFreeTimeJobs()
{
    bool fixedD = false;
    bool fixedN = false;
    if (this->m_DayJob == JOB_FILMFREETIME || this->m_DayJob == JOB_ARENAREST || this->m_DayJob == JOB_CENTREREST || this->m_DayJob == JOB_CLINICREST ||
        this->m_DayJob == JOB_HOUSEREST || this->m_DayJob == JOB_FARMREST || this->m_DayJob == JOB_RESTING || this->m_DayJob == 255)
    {
        auto old_job = m_DayJob;
        if(m_Building) {
            m_DayJob = m_Building->m_RestJob;
        } else {
            m_DayJob = JOB_RESTING;
        }


        fixedD = old_job != m_DayJob;
    }
    if (this->m_NightJob == JOB_FILMFREETIME || this->m_NightJob == JOB_ARENAREST || this->m_NightJob == JOB_CENTREREST || this->m_NightJob == JOB_CLINICREST ||
        this->m_NightJob == JOB_HOUSEREST || this->m_NightJob == JOB_FARMREST || this->m_NightJob == JOB_RESTING || this->m_NightJob == 255)
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

int sGirl::upd_temp_stat(int stat_id, int amount, bool usetraits)
{
    if (usetraits)
    {
        if (stat_id == STAT_LIBIDO)
        {
            if (has_trait("Nymphomaniac"))	{ amount = int((double)amount * (amount > 0 ? 1.5 : 0.5));	if (amount == 0)	amount = 1; }
            else if (has_trait("Chaste"))		{ amount = int((double)amount * (amount > 0 ? 0.5 : 1.5));	if (amount == 0)	amount = -1; }
        }
    }
    // TODO Does it really make sense to silently change some stats non temporarily in upd_temp_stat?
    if (stat_id == STAT_HEALTH || stat_id == STAT_HAPPINESS || stat_id == STAT_TIREDNESS || stat_id == STAT_EXP ||
        stat_id == STAT_LEVEL || stat_id == STAT_HOUSE || stat_id == STAT_ASKPRICE)
    {
        upd_stat(stat_id, amount);
        return get_stat(stat_id);
    }
    m_StatTemps[stat_id] += amount;

    return get_stat(stat_id);
}

int sGirl::upd_temp_Enjoyment(int stat_id, int amount)
{
    m_EnjoymentTemps[stat_id] += amount;
    return get_enjoyment(stat_id);
}

int sGirl::upd_Enjoyment(int stat_id, int amount, bool usetraits)
{
    m_Enjoyment[amount] += amount;
    /* */if (m_Enjoyment[amount] > 100) 	m_Enjoyment[amount] = 100;
    else if (m_Enjoyment[amount] < -100) 	m_Enjoyment[amount] = -100;

    return get_enjoyment(stat_id);
}

int sGirl::upd_Training(int stat_id, int amount, bool usetraits)
{
    m_Training[stat_id] += amount;
    /* */if (m_Training[stat_id] > 100) 	m_Training[stat_id] = 100;
    else if (m_Training[stat_id] < 0) 		m_Training[stat_id] = 0;
    return get_training(stat_id);
}

int sGirl::get_num_item_equiped(int Type)
{
    if (m_NumInventory == 0) return 0;
    int num = 0;
    for (int i = 0; i < MAXNUM_GIRL_INVENTORY; i++)
    {
        if (m_Inventory[i])
        {
            if (m_Inventory[i]->m_Type == Type && m_EquipedItems[i] == 1) num++;
        }
    }
    return num;
}

bool sGirl::has_temp_trait(std::string trait) const
{
    for (int i = 0; i < MAXNUM_TRAITS; i++)
    {
        if (m_Traits[i] && m_TempTrait[i] > 0)
        {
            if (trait == m_Traits[i]->name())
            {
                return m_TempTrait[i];
            }
        }
    }
    return false;
}

void sGirl::set_stat(int stat, int amount)
{
    int amt = amount;       // Modifying amount directly isn't a good idea
    int min = 0, max = 100;
    switch (stat)
    {
    case STAT_AGE:		// age is a special case so we start with that
        if (m_Stats[STAT_AGE] > 99)		m_Stats[stat] = 100;
        else if (m_Stats[stat] > 80)	m_Stats[stat] = 80;
        else if (m_Stats[stat] < 18)	m_Stats[stat] = 18;	// `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live
        else							m_Stats[stat] = amt;
        return; break;	// and just return instead of going to the end
    case STAT_HEALTH:
        if (has_trait( "Incorporeal"))	// Health and tiredness need the incorporeal sanity check
        {
            m_Stats[stat] = 100;
            return;
        }
        break;
    case STAT_TIREDNESS:
        if (has_trait( "Incorporeal") ||	// Health and tiredness need the incorporeal sanity check
            has_trait( "Skeleton") ||
            has_trait( "Zombie"))
        {
            m_Stats[stat] = 0;
            return;
        }
        break;
    case STAT_EXP:
        max = 32000;
        break;
    case STAT_LEVEL:
        max = 255;
        break;
    case STAT_PCFEAR:
    case STAT_PCHATE:
    case STAT_PCLOVE:
    case STAT_MORALITY:
    case STAT_REFINEMENT:
    case STAT_DIGNITY:
    case STAT_LACTATION:
        min = -100;
        break;
    default:
        break;
    }
    if (amt > max) amt = max;
    else if (amt < min) amt = min;
    m_Stats[stat] = amt;
}

int sGirl::get_trait_modifier(const std::string& type) const
{
    int mod = 0;
    for(auto& trait : m_Traits) {
        if(trait)
            mod += trait->get_modifier(type);
    }
    return mod;
}

int sGirl::has_item(const sInventoryItem &item) const {
    for(auto& inv : m_Inventory) {
        if(inv == &item) {
            return true;
        }
    }
    return false;
}

void sGirl::unequip(int slot) {
    if (m_EquipedItems[slot] == 0) return;	// if already unequiped do nothing
    // unapply the effects
    for (u_int i = 0; i < m_Inventory[slot]->m_Effects.size(); i++)
    {
        int eff_id = m_Inventory[slot]->m_Effects[i].m_EffectID;
        int affects = m_Inventory[slot]->m_Effects[i].m_Affects;
        int amount = m_Inventory[slot]->m_Effects[i].m_Amount;

        /* */if (affects == sEffect::Skill)	cGirls::UpdateSkillMod(this, eff_id, -amount);
        else if (affects == sEffect::Stat)	cGirls::UpdateStatMod(this, eff_id, -amount);
        else if (affects == sEffect::Enjoy)	cGirls::UpdateEnjoymentMod(this, eff_id, -amount);
        else if (affects == sEffect::GirlStatus)	// adds/removes status
        {
            if (amount == 1) m_States &= ~(1 << eff_id);		// add status
            else if (amount == 0) m_States |= (1 << eff_id);	// remove status
        }
        else if (affects == sEffect::Trait)	// trait
        {
            /*
             *	WD:	New logic for remembering traits
             *		moved to AddTrait() RemoveTrait() fn's
             *
             *		UNEQUIP
             */
            if (amount == 0 && m_Inventory[slot]->m_Effects[i].m_Trait == "Virgin")
            {
                m_Virgin = 0; // `J` unequiping an item will not make her a virgin again
            }
            else if (amount == 0)					// possibly add remembered trait from unequiping an item
                add_trait(m_Inventory[slot]->m_Effects[i].m_Trait, false, false, true);	// inrememberlist = true Add trait only if it is in the rememebered list

            else if (amount == 1)				// remove item trait from unequiping an item
                remove_trait(m_Inventory[slot]->m_Effects[i].m_Trait);
        }
    }
    // set it as unequiped
    m_EquipedItems[slot] = 0;

    cGirls::CalculateGirlType(this);
}
