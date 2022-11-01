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
#include <algorithm>
#include "cJobManager.h"
#include "buildings/IBuilding.h"
#include "buildings/queries.h"
#include "character/cCustomers.h"
#include "cRng.h"
#include "cInventory.h"
#include <sstream>
#include "CLog.h"
#include "cTariff.h"
#include "cGold.h"
#include "cGangs.h"
#include "cGangManager.hpp"
#include "IGame.h"
#include "scripting/GameEvents.h"
#include "combat/combat.h"
#include "jobs/Crafting.h"
#include "Inventory.h"
#include "cRival.h"
#include "buildings/cDungeon.h"
#include "character/predicates.h"
#include "character/cPlayer.h"
#include "buildings/cBuildingManager.h"
#include "cGirlGangFight.h"
#include "cShop.h"

extern cRng g_Dice;

void RegisterCraftingJobs(cJobManager& mgr);
void RegisterSurgeryJobs(cJobManager& mgr);
void RegisterWrappedJobs(cJobManager& mgr);
void RegisterManagerJobs(cJobManager& mgr);
void RegisterTherapyJobs(cJobManager& mgr);
void RegisterBarJobs(cJobManager& mgr);
void RegisterFarmJobs(cJobManager& mgr);
void RegisterClinicJobs(cJobManager& mgr);
void RegisterFilmCrewJobs(cJobManager& mgr);
void RegisterFilmingJobs(cJobManager& mgr);
void RegisterOtherStudioJobs(cJobManager& mgr);
void RegisterTrainingJobs(cJobManager& mgr);
void RegisterArenaJobs(cJobManager& mgr);
void RegisterCleaningJobs(cJobManager& mgr);
void RegisterHouseJobs(cJobManager& mgr);
void RegisterCentreJobs(cJobManager& mgr);
void RegisterStripClubJobs(cJobManager& mgr);

namespace settings {
    extern const char* WORLD_RAPE_STREETS;
    extern const char* WORLD_RAPE_BROTHEL;
}

cJobManager::cJobManager() = default;
cJobManager::~cJobManager() = default;

void cJobManager::Setup()
{
    m_OOPJobs.resize(NUM_JOBS);

    auto register_filter = [&](JOBFILTER filter, JOBS first, JOBS last, std::initializer_list<JOBS> extra) {
        for(int i = first; i <= last; ++i) {
            JobFilters[filter].Contents.push_back((JOBS)i);
        }
        for(auto& j : extra)  {
            JobFilters[filter].Contents.push_back(j);
        }
    };

    // General Jobs
    JobFilters[JOBFILTER_GENERAL] = sJobFilter{"General", "These are general jobs that work individually in any building."};
    register_filter(JOBFILTER_GENERAL, JOB_RESTING, JOB_BEASTCARER, {});

    // - Bar Jobs
    JobFilters[JOBFILTER_BAR] = sJobFilter{"Bar", "These are jobs for running a bar."};
    register_filter(JOBFILTER_BAR, JOB_BARMAID, JOB_BARCOOK, {});
    // - Gambling Hall Jobs
    JobFilters[JOBFILTER_GAMBHALL] = sJobFilter{"Gambling Hall", "These are jobs for running a gambling hall."};
    register_filter(JOBFILTER_GAMBHALL, JOB_DEALER, JOB_WHOREGAMBHALL, {});
    // - Sleazy Bar Jobs        // Changing all references to Strip Club, just sounds better to me and more realistic. -PP
    JobFilters[JOBFILTER_SLEAZYBAR] = sJobFilter{"Strip Club", "These are jobs for running a Strip Club."};
    register_filter(JOBFILTER_SLEAZYBAR, JOB_SLEAZYBARMAID, JOB_BARWHORE, {});
    // - Brothel Jobs
    JobFilters[JOBFILTER_BROTHEL] = sJobFilter{"Brothel", "These are jobs for running a brothel."};
    register_filter(JOBFILTER_BROTHEL, JOB_MASSEUSE, JOB_WHORESTREETS, {});

    // Studio Crew
    JobFilters[JOBFILTER_STUDIOCREW] = sJobFilter{"Studio Crew", "These are jobs for running a movie studio."};
    register_filter(JOBFILTER_STUDIOCREW, JOB_EXECUTIVE, JOB_STAGEHAND, {JOB_RESTING});
    //JobData[JOB_SOUNDTRACK] = sJobData("Sound Track", "SndT", WorkSoundTrack, JP_SoundTrack);
    //JobData[JOB_SOUNDTRACK].description = ("She will clean up the audio and add music to the scenes. (not required but helpful)");

    // Studio - Non-Sex Scenes
    JobFilters[JOBFILTER_STUDIOTEASE] = sJobFilter{"Teasing Scenes", "These are scenes without sex."};
    register_filter(JOBFILTER_STUDIOTEASE, JOB_FILMACTION, JOB_FILMTEASE, {});

    // Studio - Softcore Porn
    JobFilters[JOBFILTER_STUDIOSOFTCORE] = sJobFilter{"Softcore Scenes", "These are scenes without any penetration."};
    register_filter(JOBFILTER_STUDIOSOFTCORE, JOB_FILMMAST, JOB_FILMTITTY, {});

    // Studio - Porn
    JobFilters[JOBFILTER_STUDIOPORN] = sJobFilter{"Porn Scenes", "These are regular sex scenes."};
    register_filter(JOBFILTER_STUDIOPORN, JOB_FILMANAL, JOB_FILMGROUP, {});

    // Studio - Hardcore porn
    JobFilters[JOBFILTER_STUDIOHARDCORE] = sJobFilter{"Hardcore Scenes", "These are rough scenes that not all girls would be comfortable with."};
    register_filter(JOBFILTER_STUDIOHARDCORE, JOB_FILMBEAST, JOB_FILMPUBLICBDSM, {});

    // Studio - Random
    JobFilters[JOBFILTER_RANDSTUDIO] = sJobFilter{"Random Scenes", "Let the game choose what scene the girl will film."};
    register_filter(JOBFILTER_RANDSTUDIO, JOB_FILMRANDOM, JOB_FILMRANDOM, {});


    // - Arena Jobs
    JobFilters[JOBFILTER_ARENA] = sJobFilter{"Arena", "These are jobs for running an arena."};
    register_filter(JOBFILTER_ARENA, JOB_FIGHTBEASTS, JOB_FIGHTTRAIN, {});
    //- Arena Staff
    JobFilters[JOBFILTER_ARENASTAFF] = sJobFilter{"Arena Staff", "These are jobs that help run an arena."};
    register_filter(JOBFILTER_ARENASTAFF, JOB_DOCTORE, JOB_CLEANARENA, {JOB_RESTING});

    // - Community Centre Jobs
    JobFilters[JOBFILTER_COMMUNITYCENTRE] = sJobFilter{"Community Centre", "These are jobs for running a community centre."};
    register_filter(JOBFILTER_COMMUNITYCENTRE, JOB_CENTREMANAGER, JOB_CLEANCENTRE, {JOB_RESTING});
    // - Counseling Centre Jobs
    JobFilters[JOBFILTER_COUNSELINGCENTRE] = sJobFilter{"Counseling Centre", "These are jobs for running a counseling centre."};
    register_filter(JOBFILTER_COUNSELINGCENTRE, JOB_COUNSELOR, JOB_ANGER, {});

    // - Clinic Surgeries
    JobFilters[JOBFILTER_CLINIC] = sJobFilter{"Medical Clinic", "These are procerures the girls can have done in the Medical Clinic."};
    register_filter(JOBFILTER_CLINIC, JOB_GETHEALING, JOB_FERTILITY, {});
    // Clinic staff
    JobFilters[JOBFILTER_CLINICSTAFF] = sJobFilter{"Clinic Staff", "These are jobs that help run a medical clinic."};
    register_filter(JOBFILTER_CLINICSTAFF, JOB_CHAIRMAN, JOB_JANITOR, {JOB_RESTING});

    // - Farm Staff
    JobFilters[JOBFILTER_FARMSTAFF] = sJobFilter{"Farm Staff", "These are jobs for running a farm"};
    register_filter(JOBFILTER_FARMSTAFF, JOB_FARMMANGER, JOB_FARMHAND, {JOB_RESTING});

    // - Laborers
    JobFilters[JOBFILTER_LABORERS] = sJobFilter{"Laborers", "These are jobs your girls can do at your farm."};
    register_filter(JOBFILTER_LABORERS, JOB_FARMER, JOB_MILK, {});
    // - Producers
    JobFilters[JOBFILTER_PRODUCERS] = sJobFilter{"Producers", "These are jobs your girls can do at your farm."};
    register_filter(JOBFILTER_PRODUCERS, JOB_BUTCHER, JOB_MAKEPOTIONS, {});

    // - House
    JobFilters[JOBFILTER_HOUSE] = sJobFilter{"Player House", "These are jobs your girls can do at your house."};
    register_filter(JOBFILTER_HOUSE, JOB_HEADGIRL, JOB_CLEANHOUSE, {JOB_RESTING});
    //JobData[JOB_PONYGIRL].name = ("Pony Girl");
    //JobData[JOB_PONYGIRL].brief = "PGil";
    //JobData[JOB_PONYGIRL].description = ("She will be trained to become a pony girl.");
    //JobFunc[JOB_PONYGIRL] = &WorkFarmPonyGirl;

    JobFilters[JOBFILTER_HOUSETTRAINING] = sJobFilter{"Sex Training", "Training the girl in sexual matters."};
    register_filter(JOBFILTER_HOUSETTRAINING, JOB_MISTRESS, JOB_HOUSEPET, {});

    RegisterCraftingJobs(*this);
    RegisterSurgeryJobs(*this);
    RegisterWrappedJobs(*this);
    RegisterManagerJobs(*this);
    RegisterFilmingJobs(*this);
    RegisterFilmCrewJobs(*this);
    RegisterOtherStudioJobs(*this);
    RegisterTherapyJobs(*this);
    RegisterBarJobs(*this);
    RegisterFarmJobs(*this);
    RegisterClinicJobs(*this);
    RegisterTrainingJobs(*this);
    RegisterArenaJobs(*this);
    RegisterCleaningJobs(*this);
    RegisterHouseJobs(*this);
    RegisterCentreJobs(*this);
}

sCustomer cJobManager::GetMiscCustomer(IBuilding& brothel)
{
    sCustomer cust = g_Game->GetCustomer(brothel);
    brothel.m_MiscCustomers+=1;
    return cust;
}

// ----- Job related

bool cJobManager::FullTimeJob(JOBS Job)
{
    assert(m_OOPJobs.at(Job) != nullptr);
    return m_OOPJobs.at(Job)->get_info().FullTime;
}

bool cJobManager::is_job_Paid_Player(JOBS Job)
{
/*
 *    WD:    Tests for jobs that the player pays the wages for
 *
 *    Jobs that earn money for the player are commented out
 *
 */

    // `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >> cJobManager::is_job_Paid_Player
    return (
        // ALL JOBS

        // - General
        //Job ==    JOB_RESTING                ||    // relaxes and takes some time off
        Job ==    JOB_CLEANING            ||    // cleans the building
        Job ==    JOB_SECURITY            ||    // Protects the building and its occupants
        Job ==    JOB_ADVERTISING            ||    // Goes onto the streets to advertise the buildings services
        Job ==    JOB_CUSTOMERSERVICE        ||    // looks after customers needs (customers are happier when people are doing this job)
        Job ==    JOB_MATRON                ||    // looks after the needs of the girls (only 1 allowed)
        Job ==    JOB_TORTURER            ||    // Tortures the people in the dungeons to help break their will (this is in addition to player torture) (only 1 allowed)
        //Job ==    JOB_EXPLORECATACOMBS    ||    // goes adventuring in the catacombs
        Job ==    JOB_BEASTCAPTURE        ||    // captures beasts for use (only 2 allowed) (creates beasts resource)
        Job ==    JOB_BEASTCARER            ||    // cares for captured beasts (only 1 allowed) (creates beasts resource and lowers their loss)

        // Clinic
        Job ==    JOB_DOCTOR                ||    // becomes a doctor (requires 1) (will make some extra cash for treating locals)
        Job ==    JOB_CHAIRMAN            ||    //
        Job ==    JOB_JANITOR                ||    //

        // - Movie Crystal Studio
        Job == JOB_FILMACTION            ||    // films this sort of scene in the movie
        Job == JOB_FILMCHEF                ||    // films this sort of scene in the movie
        Job == JOB_FILMMUSIC            ||    // films this sort of scene in the movie
        Job == JOB_FILMMAST                ||    // films this sort of scene in the movie
        Job == JOB_FILMSTRIP            ||    // films this sort of scene in the movie
        Job == JOB_FILMTEASE            ||    // films this sort of scene in the movie
        Job == JOB_FILMANAL                ||    // films this sort of scene in the movie
        Job == JOB_FILMFOOTJOB            ||    // films this sort of scene in the movie
        Job == JOB_FILMHANDJOB            ||    // films this sort of scene in the movie
        Job == JOB_FILMLESBIAN            ||    // films this sort of scene in the movie
        Job == JOB_FILMORAL                ||    // films this sort of scene in the movie
        Job == JOB_FILMSEX                ||    // films this sort of scene in the movie
        Job == JOB_FILMTITTY            ||    // films this sort of scene in the movie
        Job == JOB_FILMBEAST            ||    // films this sort of scene in the movie (uses beast resource)
        Job == JOB_FILMBONDAGE            ||    // films this sort of scene in the movie
        Job == JOB_FILMBUKKAKE            ||    // films this sort of scene in the movie
        Job == JOB_FILMFACEFUCK            ||    // films this sort of scene in the movie
        Job == JOB_FILMGROUP            ||    // films this sort of scene in the movie
        Job == JOB_FILMPUBLICBDSM        ||    // films this sort of scene in the movie
        Job == JOB_FILMRANDOM            ||    // films this sort of scene in the movie

        Job ==    JOB_DIRECTOR            ||    // Direcets the movies
        Job ==    JOB_PROMOTER            ||    // Advertises the studio's films
        Job ==    JOB_CAMERAMAGE            ||    // Uses magic to record the scenes to crystals (requires at least 1)
        Job ==    JOB_CRYSTALPURIFIER        ||    // Post editing to get the best out of the film (requires at least 1)
        Job ==    JOB_FLUFFER                ||    // Keeps the porn stars and animals aroused
        Job ==    JOB_STAGEHAND            ||    // Sets up equipment, and keeps studio clean

        // - Community Centre
        Job ==    JOB_FEEDPOOR            ||    // work in a soup kitchen
        Job ==    JOB_COMUNITYSERVICE        ||    // Goes around town helping where they can
        Job ==    JOB_CENTREMANAGER        ||    // work in a soup kitchen
        Job ==    JOB_CLEANCENTRE            ||    // Goes around town helping where they can

        //arena
        Job ==    JOB_DOCTORE                ||    //
        Job ==    JOB_CLEANARENA            ||    //
        Job ==    JOB_FIGHTTRAIN            ||    //
        Job ==    JOB_FIGHTARENAGIRLS        ||    //
        Job ==    JOB_FIGHTBEASTS            ||    //

        //farm
        Job ==    JOB_FARMMANGER            ||    //
        Job ==    JOB_FARMHAND            ||    //

        //house
        Job ==    JOB_HEADGIRL            ||    //
        Job ==    JOB_RECRUITER            ||    //
        Job ==    JOB_PERSONALBEDWARMER    ||    //
        Job ==    JOB_HOUSECOOK            ||    //
        Job ==    JOB_CLEANHOUSE            ||    //
        Job ==    JOB_PERSONALTRAINING    ||    //
        Job ==    JOB_FAKEORGASM            ||    //
        Job ==    JOB_SO_STRAIGHT            ||    //
        Job ==    JOB_SO_BISEXUAL            ||    //
        Job ==    JOB_SO_LESBIAN            ||    //
        Job ==    JOB_TRAINING            ||    // trains skills at a basic level
        Job ==    JOB_HOUSEPET            ||    //

        // - Brothel
        //Job ==    JOB_WHOREBROTHEL        ||    // whore herself inside the building
        //Job ==    JOB_WHORESTREETS        ||    // whore self on the city streets
        //Job ==    JOB_BROTHELSTRIPPER        ||    // strips for customers and sometimes sex
        //Job ==    JOB_MASSEUSE            ||    // gives massages to patrons and sometimes sex

        // - Gambling Hall - Using WorkHall() or WorkWhore()
        //Job ==    JOB_WHOREGAMBHALL        ||    // looks after customers sexual needs
        //Job ==    JOB_DEALER                ||    // dealer for gambling tables
        //Job ==    JOB_ENTERTAINMENT        ||    // sings, dances and other shows for patrons
        //Job ==    JOB_XXXENTERTAINMENT    ||    // naughty shows for patrons

        // - Bar Using - WorkBar() or WorkWhore()
        //Job ==    JOB_BARMAID                ||    // serves at the bar (requires 1 at least)
        //Job ==    JOB_WAITRESS            ||    // waits on the tables (requires 1 at least)
        //Job ==    JOB_STRIPPER            ||    // strips for the customers and gives lapdances
        //Job ==    JOB_WHOREBAR            ||    // offers sexual services to customers
        //Job ==    JOB_SINGER                ||    // sings int the bar (girl needs singer trait)

/*
 *        Not Implemented
 */

#if 0

        // - Community Centre
        Job ==    JOB_COLLECTDONATIONS    ||    // collects money to help the poor
        Job ==    JOB_ARTISAN                ||    // makes cheap crappy handmade items for selling to raise money (creates handmade item resource)
        Job ==    JOB_SELLITEMS            ||    // goes out and sells the made items (sells handmade item resource)

        // - Drug Lab (these jobs gain bonus if in same building as a clinic)
        Job ==    JOB_VIRASPLANTFUCKER    ||    // the plants need to inseminate with other humanoids to proper-gate, creates vira blood items
        Job ==    JOB_SHROUDGROWER        ||    // They require lots of care, and may explode. Produces shroud mushroom item.
        Job ==    JOB_FAIRYDUSTER            ||    // captures and pounds faries to dust, produces fairy dust item
        Job ==    JOB_DRUGDEALER            ||    // goes out onto the streets to sell the items made with the other jobs

        // - Alchemist lab (these jobs gain bonus if in same building as a drug lab)
        Job ==    JOB_FINDREGENTS            ||    // goes around and finds ingredients for potions, produces ingredients resource.
        Job ==    JOB_BREWPOTIONS            ||    // will randomly produce any items with the word "potion" in the name, uses ingredients resource
        Job ==    JOB_POTIONTESTER        ||    // Potion tester tests potions, they may die doing this or random stuff may happen. (requires 1)

        // - Arena (these jobs gain bonus if in same building as a clinic)
        Job ==    JOB_FIGHTBEASTS            ||    // customers come to place bets on who will win, girl may die (uses beasts resource)
        Job ==    JOB_WRESTLE                ||    // as above no chance of dying
        Job ==    JOB_FIGHTTODEATH        ||    // as above but against other girls (chance of dying)
        Job ==    JOB_FIGHTVOLUNTEERS        ||    // fight against customers who volunteer for prizes of gold
        Job ==    JOB_COLLECTBETS            ||    // collects the bets from customers (requires 1)

        // - Skills Centre
        Job ==    JOB_TEACHBDSM            ||    // teaches BDSM skills
        Job ==    JOB_TEACHSEX            ||    // teaches general sex skills
        Job ==    JOB_TEACHBEAST            ||    // teaches beastiality skills
        Job ==    JOB_TEACHMAGIC            ||    // teaches magic skills
        Job ==    JOB_TEACHCOMBAT            ||    // teaches combat skills
        Job ==    JOB_DAYCARE                ||    // looks after the local kids (non sex stuff of course)
        Job ==    JOB_SCHOOLING            ||    // teaches the local kids (non sex stuff of course)
        Job ==    JOB_TEACHDANCING        ||    // teaches dancing and social skills
        Job ==    JOB_TEACHSERVICE        ||    // teaches misc skills
        Job ==    JOB_TRAIN                ||    // trains the girl in all the disicplince for which there is a teacher

        // - Clinic
        Job ==    JOB_DOCTOR                ||    // becomes a doctor (requires 1) (will make some extra cash for treating locals)
        Job ==    JOB_GETABORT            ||    // gets an abortion (takes 2 days)
        Job ==    JOB_COSMETICSURGERY        ||    // magical plastic surgery (takes 5 days)
        Job ==    JOB_GETHEALING            ||    // takes 1 days for each wound trait received.
        Job ==    JOB_GETREPAIRS            ||    // construct girls can get repaired quickly
#endif

        // - extra unassignable
        //Job ==    JOB_INDUNGEON            ||
        //Job ==    JOB_RUNAWAY                ||

        false );


}

// `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >> cJobManager.cpp > HandleSpecialJobs
bool cJobManager::HandleSpecialJobs(sGirl& Girl, JOBS JobID, JOBS OldJobID, bool Day0Night1, bool fulltime)
{
    bool MadeChanges = true;  // whether a special case applies to specified job or not

    /*    `J` added to reset working days only if her job actually changes
     * m_WorkingDay is only used for full time jobs that take more than 1 day to complete
     * such as getting surgerys or rehab
    */
    if (Girl.m_WorkingDay > 0)    // `J` Save it and reset it to 0 so it is only backed up once...
    {
        if ((Girl.m_WorkingDay < 2 && (Girl.m_YesterDayJob == JOB_GETABORT )) ||
            (Girl.m_WorkingDay < 3 && (Girl.m_YesterDayJob == JOB_REHAB)) ||
            (Girl.m_WorkingDay < 5 && Girl.m_YesterDayJob != JOB_GETABORT && cJobManager::is_Surgery_Job(Girl.m_YesterDayJob)) ||
            // `J` training jobs use m_WorkingDay as a percent learned
            (Girl.m_YesterDayJob == JOB_FAKEORGASM || Girl.m_YesterDayJob == JOB_SO_STRAIGHT || Girl.m_YesterDayJob == JOB_SO_BISEXUAL || Girl.m_YesterDayJob == JOB_SO_LESBIAN)
            )
        {
            Girl.m_PrevWorkingDay = Girl.m_WorkingDay;
        }
        Girl.m_WorkingDay = 0;
    }
    if (Girl.m_YesterDayJob == JobID)    // `J` ...so that if you decide to put her back onto the job...
    {
        Girl.m_WorkingDay = Girl.m_PrevWorkingDay;    // `J` ...it will restore the previous days
    }


    assert(m_OOPJobs[JobID] != nullptr);
    auto check = m_OOPJobs[JobID]->is_job_valid(Girl);
    if(!check) {
        g_Game->push_message(check.Reason, 0);
        return false;
    }

    // rest jobs
    if (JobID == JOB_RESTING)
    {
        /*   */if (fulltime)    Girl.m_NightJob = Girl.m_DayJob = JobID;
        else if (Day0Night1)    Girl.m_NightJob = JobID;
        else/*            */    Girl.m_DayJob = JobID;
    }
// Special Brothel Jobs
    else if (JobID == JOB_MATRON)
    {
        if (Girl.m_Building->num_girls_on_job(JOB_MATRON, Day0Night1) > 0)
            g_Game->push_message("You can only have one matron per brothel.", 0);
        else
            Girl.m_NightJob = Girl.m_DayJob = JOB_MATRON;
    }
    else if (JobID == JOB_TORTURER)
    {
        if (random_girl_on_job(g_Game->buildings(), JOB_TORTURER, 0))
            g_Game->push_message("You can only have one torturer among all of your brothels.", 0);
        else
            Girl.m_NightJob = Girl.m_DayJob = JOB_TORTURER;
    }
    // Special House Jobs
    else if (JobID == JOB_HEADGIRL)
    {
        if (Girl.m_Building->num_girls_on_job(JOB_HEADGIRL, Day0Night1) > 0)    g_Game->push_message("There can be only one Head Girl!", 0);
        else /*                                 */    Girl.m_NightJob = Girl.m_DayJob = JOB_HEADGIRL;
    }
    else if (JobID == JOB_RECRUITER)
    {
        Girl.m_NightJob = Girl.m_DayJob = JOB_RECRUITER;
    }
    else if (JobID == JOB_FAKEORGASM)
    {
        if (Girl.has_active_trait(traits::FAKE_ORGASM_EXPERT))    g_Game->push_message("She already has \"Fake Orgasm Expert\".", 0);
        else /*                                 */    Girl.m_DayJob = Girl.m_NightJob = JOB_FAKEORGASM;
    }
    else if (JobID == JOB_SO_BISEXUAL)
    {
        if (Girl.has_active_trait(traits::BISEXUAL))/*      */    g_Game->push_message("She is already Bisexual.", 0);
        else /*                                 */    Girl.m_DayJob = Girl.m_NightJob = JOB_SO_BISEXUAL;
    }
    else if (JobID == JOB_SO_LESBIAN)
    {
        if (Girl.has_active_trait(traits::LESBIAN))/*       */    g_Game->push_message("She is already a Lesbian.", 0);
        else /*                                 */    Girl.m_DayJob = Girl.m_NightJob = JOB_SO_LESBIAN;
    }
    else if (JobID == JOB_SO_STRAIGHT)
    {
        if (Girl.has_active_trait(traits::STRAIGHT))/*      */    g_Game->push_message("She is already Straight.", 0);
        else /*                                 */    Girl.m_DayJob = Girl.m_NightJob = JOB_SO_STRAIGHT;
    }
    else if (JobID == JOB_HOUSEPET)
    {
        if (Girl.is_slave())/*                 */    Girl.m_NightJob = Girl.m_DayJob = JOB_HOUSEPET;
        else /*                                 */    g_Game->push_message("Only slaves can take this training.", 0);
    }
//    else if (JobID == JOB_PONYGIRL)
//    {
//        if (Girl.is_slave())    Girl.m_NightJob = Girl.m_DayJob = JOB_PONYGIRL;
//        else                    g_Game->push_message(("Only slaves can take this training."), 0);
//    }
    // Special Farm Jobs
    else if (JobID == JOB_FARMMANGER)
    {
        if (Girl.m_Building->num_girls_on_job(JOB_FARMMANGER, Day0Night1) > 0)
            g_Game->push_message("There can be only one Farm Manager!", 0);
        else
            Girl.m_NightJob = Girl.m_DayJob = JOB_FARMMANGER;
    }
    else if (JobID == JOB_MARKETER)
    {
        if (Day0Night1 == SHIFT_DAY || fulltime)
        {
            if (Girl.m_Building->num_girls_on_job(JOB_MARKETER, SHIFT_DAY) > 0)
                g_Game->push_message("There can be only one Farm Marketer on each shift!", 0);
            else Girl.m_DayJob = JOB_MARKETER;
        }
        if (Day0Night1 == SHIFT_NIGHT || fulltime)
        {
            if (Girl.m_Building->num_girls_on_job(JOB_MARKETER, SHIFT_NIGHT) > 0)
                g_Game->push_message("There can be only one Farm Marketer on each shift!", 0);
            else Girl.m_NightJob = JOB_MARKETER;
        }
    }
    // Special Arena Jobs
    else if (JobID == JOB_DOCTORE)
    {
        if (Girl.m_Building->num_girls_on_job(JOB_DOCTORE, Day0Night1) > 0)
            g_Game->push_message("There can be only one Doctore!", 0);
        else
            Girl.m_NightJob = Girl.m_DayJob = JOB_DOCTORE;
    }
    else if (JobID == JOB_FIGHTTRAIN && (Girl.combat() > 99 && Girl.magic() > 99 && Girl.agility() > 99 && Girl.constitution() > 99))
    {    // `J` added then modified
        g_Game->push_message("There is nothing more she can learn here.", 0);
        if (Girl.m_DayJob == JOB_FIGHTTRAIN)    Girl.m_DayJob = JOB_RESTING;
        if (Girl.m_NightJob == JOB_FIGHTTRAIN)    Girl.m_NightJob = JOB_RESTING;
    }
    // Special Clinic Jobs
    else if (JobID == JOB_CHAIRMAN)
    {
        if (Girl.m_Building->num_girls_on_job(JOB_CHAIRMAN, Day0Night1)>0)
            g_Game->push_message("There can be only one Chairman!", 0);
        else
            Girl.m_NightJob = Girl.m_DayJob = JOB_CHAIRMAN;
    }

    else if (Girl.has_active_trait(traits::AIDS) && (JobID == JOB_DOCTOR || JobID == JOB_NURSE || JobID == JOB_INTERN))
    {
        g_Game->push_message("Health laws prohibit anyone with AIDS from working in the Medical profession", 0);
        if (Girl.m_DayJob == JOB_INTERN || Girl.m_DayJob == JOB_NURSE || Girl.m_DayJob == JOB_DOCTOR)
            Girl.m_DayJob = JOB_RESTING;
        if (Girl.m_NightJob == JOB_INTERN || Girl.m_NightJob == JOB_NURSE || Girl.m_NightJob == JOB_DOCTOR)
            Girl.m_NightJob = JOB_RESTING;
    }
    else if (JobID == JOB_DOCTOR)
    {
        if (Girl.medicine() < 50 || Girl.intelligence() < 50)
        {
            std::stringstream ss;
            ss << Girl.FullName() << " does not have enough training to work as a Doctor. Doctors require 50 Medicine and 50 Intelligence.";
            g_Game->push_message(ss.str(), 0);
        }
        else Girl.m_NightJob = Girl.m_DayJob = JOB_DOCTOR;
    }
    else if (JobID == JOB_INTERN && Girl.medicine() > 99 && Girl.intelligence() > 99 && Girl.charisma() > 99)
    {
        std::stringstream ss;
        ss << "There is nothing more she can learn here.\n";
        if (Girl.is_free())
        {
            Girl.m_DayJob = Girl.m_NightJob = JOB_DOCTOR;
            ss << Girl.FullName() << " has been assigned as a Doctor instead.";
        }
        else
        {
            if (fulltime)
                Girl.m_DayJob = Girl.m_NightJob = JOB_NURSE;
            else if (Day0Night1 == SHIFT_DAY)
                Girl.m_DayJob = JOB_NURSE;
            else
                Girl.m_NightJob = JOB_NURSE;
            ss << Girl.FullName() << " has been assigned as a Nurse instead.";
        }
        g_Game->push_message(ss.str(), 0);
    }
    // Special Centre Jobs
    else if (JobID == JOB_CENTREMANAGER)
    {
        if (Girl.m_Building->num_girls_on_job(JOB_CENTREMANAGER, Day0Night1) >0)
            g_Game->push_message(("There can be only one Centre Manager!"), 0);
        else
            Girl.m_NightJob = Girl.m_DayJob = JOB_CENTREMANAGER;
    }
    else if (JobID == JOB_COUNSELOR)
    {
        Girl.m_NightJob = Girl.m_DayJob = JOB_COUNSELOR;
    }
    else if (JobID == JOB_REHAB)
    {
        if (Girl.m_Building->num_girls_on_job(JOB_COUNSELOR, Day0Night1) < 1)
            g_Game->push_message(("You must have a counselor for rehab."), 0);
        else if (!is_addict(Girl))
            g_Game->push_message(("She has no addictions."), 0);
        else
            Girl.m_DayJob = Girl.m_NightJob = JOB_REHAB;
    }
    else if (JobID == JOB_ANGER)
    {
        if (Girl.m_Building->num_girls_on_job(JOB_COUNSELOR, Day0Night1) < 1)
            g_Game->push_message(("You must have a counselor for anger management."), 0);
        else if (!Girl.any_active_trait({traits::AGGRESSIVE, traits::TSUNDERE, traits::YANDERE}))
            g_Game->push_message(("She has no anger issues."), 0);
        else
            Girl.m_DayJob = Girl.m_NightJob = JOB_ANGER;
    }
    else if (JobID == JOB_EXTHERAPY)
    {
        if (Girl.m_Building->num_girls_on_job(JOB_COUNSELOR, Day0Night1) < 1)
            g_Game->push_message(("You must have a counselor for extreme therapy."), 0);
        else if (!Girl.any_active_trait({traits::MIND_FUCKED, traits::BROKEN_WILL}))
            g_Game->push_message(("She has no extreme issues."), 0);
        else
            Girl.m_DayJob = Girl.m_NightJob = JOB_EXTHERAPY;
    }
    else if (JobID == JOB_THERAPY)
    {
        if (Girl.m_Building->num_girls_on_job(JOB_COUNSELOR, Day0Night1) < 1)
            g_Game->push_message(("You must have a counselor for therapy."), 0);
        else if (!Girl.any_active_trait({traits::NERVOUS, traits::DEPENDENT, traits::PESSIMIST}))
            g_Game->push_message(("She has no need of therapy."), 0);
        else
            Girl.m_DayJob = Girl.m_NightJob = JOB_THERAPY;
    }

// Special cases were checked and don't apply, just set the studio job as requested
#if 1
    else if (Girl.m_Building && Girl.m_Building->type() == BuildingType::STUDIO)
    {
        MadeChanges = false;
        Girl.m_DayJob = JOB_RESTING;
        Girl.m_NightJob = JobID;
    }
    else
    {
        MadeChanges = false;
        if (fulltime)/*                */    Girl.m_DayJob = Girl.m_NightJob = JobID;
        else if (Day0Night1 == SHIFT_DAY)    Girl.m_DayJob = JobID;
        else/*                         */    Girl.m_NightJob = JobID;
    }
#endif
// handle instances where special job has been removed, specifically where it actually matters
    if (JobID != OldJobID)
    {
        // if old job was full time but new job is not, switch leftover day or night job back to resting
        if (!fulltime && FullTimeJob(OldJobID) && !FullTimeJob(JobID))        // `J` greatly simplified the check
            (Day0Night1 ? Girl.m_DayJob = JOB_RESTING : Girl.m_NightJob = JOB_RESTING);

    }

    return MadeChanges;
}

// ------ Work Related Violence fns

// MYR: Rewrote the work_related_violence to add the security guard job.
//      Girls and gangs now fight using the attack/defense/agility style combat.
//      This will have to be changed to the new style of combat eventually.
//      A new kind of weaker gang not associated with any of the enemy organizations.
//      This "gang" doesn't have weapon levels or access to healing potions.
//      They're just a bunch of toughs out to make life hard for our working girls.

/*
 * return true if violence was committed against the girl
 * false if nothing happened, or if violence was committed
 * against the customer.
 */
bool cJobManager::work_related_violence(sGirl& girl, bool Day0Night1, bool streets)
{
    // the base chance of an attempted rape is higher on the streets
    float rape_chance = (float)g_Game->settings().get_percent(streets ? settings::WORLD_RAPE_STREETS : settings::WORLD_RAPE_BROTHEL);

    IBuilding * Brothl = girl.m_Building;

    // `J` adjusted this a bit so gangs spying on the girl can help also
    std::vector<sGang *> gangs_guarding = g_Game->gang_manager().gangs_watching_girls();

    int gang_coverage = guard_coverage(&gangs_guarding);

    // night time doubles the chance of attempted rape and reduces the chance for a gang to catch it by 20%
    if (Day0Night1) { rape_chance *= 2.f; gang_coverage = (int)((float)gang_coverage*0.8f); }

    // if the player has a -ve disposition, this can scare the would-be rapist into behaving himself
    if (g_Dice.percent(g_Game->player().disposition() * -1)) rape_chance = 1;

    if (rape_chance < 1) rape_chance = 1;    // minimum of 1%
    if (g_Dice.percent(rape_chance))
    {
        // Get a random weak gang. These are street toughs looking to abuse a girl.
        // They have low stats and smaller numbers. Define it here so that even if
        // the security girl or defending gang is defeated, any casualties they inflicts
        // carry over to the next layer of defense.
        sGang enemy_gang = g_Game->gang_manager().GetTempGang(-20);
        enemy_gang.give_potions(g_Dice.in_range(1, 10));
        enemy_gang.m_Skills[SKILL_MAGIC] = 0;
        // There is also between 1 and 10 of them, not 10 every time
        enemy_gang.m_Num = std::max(1, g_Dice.bell(-5, 10));

        // Three more lines of defense

        // first subtract 10 security point per gang member that is attacking
        Brothl->m_SecurityLevel = Brothl->m_SecurityLevel - enemy_gang.m_Num * 10;    // `J` moved and doubled m_SecurityLevel loss
        // 1. Brothel security
        if (security_stops_rape(girl, &enemy_gang, Day0Night1)) return false;
        // 2. Defending gangs
        if (gang_stops_rape(girl, gangs_guarding, &enemy_gang, gang_coverage, Day0Night1)) return false;
        // 3. The attacked girl herself
        if (girl_fights_rape(girl, &enemy_gang, Day0Night1)) return false;
        // If all defensive measures fail...
        // subtract 10 security points per gang member left
        Brothl->m_SecurityLevel = Brothl->m_SecurityLevel - enemy_gang.m_Num * 10;    // `J` moved and doubled m_SecurityLevel loss
        customer_rape(girl, enemy_gang.m_Num);
        return true;
    }
    girl.upd_Enjoyment(ACTION_SEX, +1);
    /*
     *    the fame thing could work either way.
     *    of course, that road leads to us keeping reputation for
     *    different things - beating the customer would be a minus
     *    for the straight sex crowd, but a big pull for the
     *    femdom lovers...
     *
     *    But let's get what we have working first
     */
    if (girl.any_active_trait({traits::YANDERE, traits::TSUNDERE, traits::AGGRESSIVE}) && g_Dice.percent(30))
    {
        switch (g_Dice % 5)
            {
            case 0:
                girl.AddMessage(("She beat the customer silly."), EImageBaseType::COMBAT, Day0Night1 ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
                break;
            case 1:
                girl.AddMessage(("The customer's face annoyed her, so she punched it until it went away."), EImageBaseType::COMBAT, Day0Night1 ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
                break;
            case 2:
                girl.AddMessage(("The customer acted like he owned her - so she pwned him."), EImageBaseType::COMBAT, Day0Night1 ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
                break;
            case 3:
                girl.AddMessage(("The customer's attitude was bad. She corrected it."), EImageBaseType::COMBAT, Day0Night1 ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
                break;
            case 4:
                girl.AddMessage(("He tried to insert a bottle into her, so she 'gave it' to him instead."), EImageBaseType::COMBAT, Day0Night1 ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
                break;
            default:
                girl.AddMessage(("Did som(E)thing violent."), EImageBaseType::COMBAT, Day0Night1 ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
                break;
            }
        girl.fame(-1);
    }
    return false;
}

// We need a cGuards guard manager. Or possible a cGuardsGuards manager.
int cJobManager::guard_coverage(std::vector<sGang*> *vpt)
{
    int pc = 0;
    std::vector<sGang*> v = g_Game->gang_manager().gangs_on_mission(MISS_GUARDING);
    if (vpt != nullptr) *vpt = v;
    for (int i = 0; i < v.size(); i++)
    {
        sGang *gang = v[i];
        // our modifier is (gang int - 50) so that gives us a value from -50 to + 50
        float mod = (float)gang->intelligence() - 50;
        // divide by 50 to make it -1.0 to +1.0
        mod /= 50;
        // add one so we have a value from 0.0 to 2.0
        mod += 1;
        /*
         *        and that's the multiplier
         *
         *        a base 6% per gang member give a full strength gang
         *        with average int a 90% chance of stopping a rape
         *        at 100 int they get 180.
         */
        pc += int(6 * gang->m_Num * mod);
    }
    /*
     *    with 6 brothels, that one gang gives a 15% chance to stop
     *    any given rape - rising to 30 for 100 intelligence
     *
     *    so 3 gangs on guard duty, at full strength, get you 90%
     *    coverage. And 4 get you 180 which should be bulletproof
     */
    return pc / g_Game->buildings().num_buildings(BuildingType::BROTHEL);
}

// True means security intercepted the perp(s)
bool cJobManager::security_stops_rape(sGirl& girl, sGang *enemy_gang, int day_night)
{
    IBuilding* Brothl = girl.m_Building;
    int SecLev = Brothl->m_SecurityLevel, OrgNumMem = enemy_gang->m_Num;

    int p_seclev = 90 + (SecLev / 1000);
    if (p_seclev > 99) p_seclev = 99;
    // A gang takes 5 security points per member to stop
    if (SecLev > OrgNumMem * 5 && g_Dice.percent(p_seclev) &&
        (Brothl->num_girls_on_job(JOB_SECURITY, day_night == SHIFT_DAY) > 0 ||
        !g_Game->gang_manager().gangs_on_mission(MISS_GUARDING).empty()))
        return true;

    // Get a random guard ready to fight
    sGirl* SecGuard = Brothl->girls().get_random_girl([day_night](const sGirl& girl){
        return girl.get_job(day_night) == JOB_SECURITY && girl.health() > 50;
    });

    // If all the security guards are too wounded to fight
    if (!SecGuard) return false;

    std::string SecName = SecGuard->FullName();

    // Most of the rest of this is a copy-paste from customer_rape
    Combat combat(ECombatObjective::CAPTURE, ECombatObjective::KILL);
    combat.add_combatants(ECombatSide::ATTACKER, *enemy_gang);
    combat.add_combatant(ECombatSide::DEFENDER, *SecGuard);
    auto result = combat.run(10);

    // Earn xp for all kills, even if defeated
    int xp = 3;

    if (SecGuard->has_active_trait(traits::QUICK_LEARNER)) xp += 2;
    else if (SecGuard->has_active_trait(traits::SLOW_LEARNER)) xp -= 2;

    int num = OrgNumMem - enemy_gang->m_Num;
    SecGuard->exp(num * xp);

    if (result == ECombatResult::DEFEAT)  // Security guard wins
    {
        SecGuard->combat(1);
        SecGuard->magic(1);
        SecGuard->upd_temp_stat(STAT_LIBIDO, num, true);  // There's nothing like killin ta make ya horny!
        SecGuard->confidence(num);
        SecGuard->fame(num);
        girl.upd_Enjoyment(ACTION_COMBAT, num);
        girl.upd_Enjoyment(ACTION_WORKSECURITY, num);

        std::stringstream Gmsg;
        std::stringstream SGmsg;

        // I decided to not say gang in the text. It can be confused with a player or enemy organization's
        // gang, when it is neither.
        if (OrgNumMem == 1)
        {
            bool female = g_Dice.percent(30);
            Gmsg << "A customer tried to attack " << girl.FullName() << ", but " << SecName << " intercepted and beat ";
            SGmsg << "Security Report:\nA customer tried to attack " << girl.FullName() << ", but " << SecName << " intercepted and beat ";
            if (female)
            {
                Gmsg << "her.";
                SGmsg << "her.";
                std::string item;
                if (g_Game->player().inventory().has_item("Brainwashing Oil"))
                {
                    item = "Brainwashing Oil";
                    SGmsg << "\n \n" << SecName << " forced a bottle of Brainwashing Oil down her throat. After a few minutes of struggling, your new slave, ";
                }
                else if (g_Game->player().inventory().has_item("Necklace of Control"))
                {
                    item = "Necklace of Control";
                    SGmsg << "\n \n" << SecName << " placed a Necklace of Control around her neck. After a few minutes of struggling, the magic in the necklace activated and your new slave, ";
                }
                else if (g_Game->player().inventory().has_item("Slave Band"))
                {
                    item = "Slave Band";
                    SGmsg << "\n \n" << SecName << " placed a Slave Band on her arm. After a few minutes of struggling, the magic in the Slave Band activated and your new slave, ";
                }
                if (!item.empty())
                {
                    std::stringstream CGmsg;

                    // `J` create the customer
                    auto custgirl = g_Game->CreateRandomGirl(SpawnReason::CUSTOMER, g_Dice % 40 + 18);
                    cGirls::SetSlaveStats(*custgirl);

                    // `J` and adjust her stats
                    const sInventoryItem* item_p = g_Game->inventory_manager().GetItem(item);
                    custgirl->add_item(item_p);
                    custgirl->equip(item_p, true);
                    g_Game->player().inventory().remove_item(item_p);
                    custgirl->add_temporary_trait(traits::EMPRISONED_CUSTOMER, std::max(5, g_Dice.bell(0, 20)));
                    custgirl->pclove(-(g_Dice % 100 + 100));
                    custgirl->pcfear(g_Dice % 50 + 50);
                    custgirl->m_Enjoyment[ACTION_COMBAT] -= (g_Dice % 50 + 20);
                    custgirl->m_Enjoyment[ACTION_SEX] -= (g_Dice % 50 + 20);
                    g_Game->player().suspicion(g_Dice % 10);
                    g_Game->player().disposition(-(g_Dice % 10));
                    g_Game->player().customerfear(g_Dice % 10);

                    // `J` do all the messages
                    SGmsg << custgirl->FullName() << " was sent to your dungeon.";
                    Gmsg << "\n" << girl.FullName() << " escorted " << custgirl->FullName() << " to the dungeon after "
                         << SecName << " gave her attacker a " << item << ", all the while scolding her for her actions.";
                    CGmsg << custgirl->FullName() << " was caught attacking a girl under your employ. She was given a "
                          << item << " and sent to the dungeon as your newest slave.";
                    custgirl->AddMessage(CGmsg.str(), EImageBaseType::DEATH, EVENT_WARNING);
                    // `J` add the customer to the dungeon
                    g_Game->dungeon().AddGirl(custgirl, DUNGEON_CUSTBEATGIRL);
                }
            }
            else
            {
                Gmsg << "him.";
                SGmsg << "him.";
                int dildo = 0;
                /* */if (SecGuard->has_item("Compelling Dildo"))    dildo = 1;
                else if (SecGuard->has_item("Dreidel Dildo"))    dildo = 2;
                else if (SecGuard->has_item("Double Dildo"))        dildo = 3;
                if (dildo > 0)
                {
                    SGmsg << "\n \n" << SecName << " decided to give this customer a taste of his own medicine and shoved her ";
                    /* */if (dildo == 1) SGmsg << "Compelling Dildo";
                    else if (dildo == 2) SGmsg << "Dreidel Dildo";
                    else if (dildo == 3) SGmsg << "Double Dildo";
                    SGmsg << " up his ass.";
                    g_Game->player().suspicion(g_Dice % 2);
                    g_Game->player().disposition(-(g_Dice % 2));
                    g_Game->player().customerfear(g_Dice % 3);
                }
            }
        }
        else
        {
            std::stringstream Tmsg;
            Gmsg << "A group of ";
            SGmsg << "Security Report:\n" << "A group of ";
            if (enemy_gang->m_Num == 0)
                Tmsg << "customers tried to attack " << girl.FullName() << ". " << SecName << " intercepted and thrashed all " << OrgNumMem;
            else
                Tmsg << OrgNumMem << " customers tried to attack " << girl.FullName() << ". They fled after " << SecName << " intercepted and thrashed " << num;
            Tmsg << " of them.";
            Gmsg << Tmsg.str();
            SGmsg << Tmsg.str();
        }
        SecGuard->AddMessage(SGmsg.str(), EImageBaseType::COMBAT, EVENT_WARNING);
        girl.AddMessage(Gmsg.str(), EImageBaseType::COMBAT, EVENT_WARNING);
        return true;
    }
    else if (result == ECombatResult::DRAW) {
        std::stringstream ss;
        ss << "Security Problem:\n" << "Trying to defend " << girl.FullName() << ". You defeated "
           << num << " of " << OrgNumMem << ". By now the guarding gangs have arrived, and will deal with the offenders.";
        SecGuard->AddMessage(ss.str(), EImageBaseType::DEATH, EVENT_DANGER);
        return false;
    }
    else  // Loss
    {
        int attacktype = SKILL_COMBAT;                                                // can be anything
        if (enemy_gang->m_Num > 1 && g_Dice.percent(40)) attacktype = SKILL_GROUP;        // standard group attack
        else if (enemy_gang->m_Num > 1 && g_Dice.percent(40)) attacktype = SKILL_BDSM;    // special group attack
        else if (g_Dice.percent(20)) attacktype = SKILL_LESBIAN;                        // female attacker
        else if (g_Dice.percent(40)) attacktype = SKILL_NORMALSEX;                        // single male attacker


        std::stringstream ss;
        ss << ("Security Problem:\n") << ("Trying to defend ") << girl.FullName() << (". You defeated ")
           << num << (" of ") << OrgNumMem << (" before:\n") << SecGuard->FullName() << GetGirlAttackedString(attacktype);

        SecGuard->AddMessage(ss.str(), EImageBaseType::DEATH, EVENT_DANGER);

        // Bad stuff
        SecGuard->happiness(-40);
        SecGuard->confidence(-40);
        SecGuard->obedience(-10);
        SecGuard->spirit(-40);
        SecGuard->libido(-4);
        SecGuard->upd_temp_stat(STAT_LIBIDO, -40, true);
        SecGuard->tiredness(60);
        SecGuard->pcfear(20);
        SecGuard->pclove(-40);
        cGirls::GirlInjured(*SecGuard, 10); // MYR: Note
        SecGuard->upd_Enjoyment(ACTION_WORKSECURITY, -30);
        SecGuard->upd_Enjoyment(ACTION_COMBAT, -30);
        return false;
    }
}

bool cJobManager::gang_stops_rape(sGirl& girl, std::vector<sGang *> gangs_guarding, sGang *enemy_gang, int coverage, int day_night)
{
    if (!g_Dice.percent(coverage)) return false;

    sGang *guarding_gang = gangs_guarding.at(g_Dice%gangs_guarding.size());
    int start_num = enemy_gang->m_Num;

    // GangVsGang returns true if enemy_gang loses
    auto result = GangBrawl(*guarding_gang, *enemy_gang);

    if (result == EFightResult::DEFEAT) {
        std::stringstream gang_s, girl_s;
        gang_s << guarding_gang->name() << " was defeated defending " << girl.FullName() << ".";
        girl_s << guarding_gang->name() << " was defeated defending you from a gang of rapists.";
        guarding_gang->AddMessage(gang_s.str(), EVENT_WARNING);
        girl.AddMessage(girl_s.str(), EImageBaseType::DEATH, EVENT_WARNING);
        return false;
    }

    /*
     * options: smart guards send them off with a warning
     * dead customers should impact disposition and suspicion
     * might also need a bribe to cover it up
     */
    std::stringstream gang_ss, girl_ss;
    if(result == EFightResult::VICTORY) {
        int roll = g_Dice.d100();
        if (roll < guarding_gang->intelligence() / 2) {
            gang_ss << "One of the " << guarding_gang->name() << " found a customer getting violent with "
                    << girl.FullName() << " and sent him packing, in fear for his life.";
            girl_ss << ("Customer attempt to rape her, but guards ") << guarding_gang->name() << (" scared him off.");
        } else if (roll < guarding_gang->intelligence()) {
            gang_ss << ("One of the ") << guarding_gang->name() << (" found a customer hurting  ") << girl.FullName()
                    << (" and beat him bloody before sending him away.");
            girl_ss << ("Customer attempt to rape her, but guards ") << guarding_gang->name()
                    << (" gave him a beating.");
        } else {
            gang_ss << ("One of the ") << guarding_gang->name() << (" killed a customer who was trying to rape ")
                    << girl.FullName() << (".");
            girl_ss << ("Customer attempt to rape her, but guards ") << guarding_gang->name() << (" killed them.");
        }
    } else
    {
        gang_ss << guarding_gang->name() << " found " << (start_num == 1 ? "a customer" : "customers") <<  " getting violent with "
                << girl.FullName() << " and intervened. They were able to defend her until the approaching city guard scared off "
                << (start_num == 1 ? "the rapist" : "the attackers");
        girl_ss << "Customer attempt to rape her, and guards " << guarding_gang->name()
                << " barely managed to hold him off until the city guard arrived.";
    }

    girl.AddMessage(girl_ss.str(), EImageBaseType::PROFILE, EVENT_WARNING);
    guarding_gang->AddMessage(gang_ss.str());

    return true;
}

// true means she won

bool cJobManager::girl_fights_rape(sGirl& girl, sGang *enemy_gang, int day_night)
{
    int OrgNumMem = enemy_gang->m_Num;

    Combat combat(ECombatObjective::CAPTURE, ECombatObjective::KILL);
    combat.add_combatants(ECombatSide::ATTACKER, *enemy_gang);
    combat.add_combatant(ECombatSide::DEFENDER, girl);

    auto result = combat.run(10);
    bool res = result != ECombatResult::VICTORY;

    auto report = std::make_shared<CombatReport>();
    report->rounds = combat.round_summaries();
    girl.GetEvents().AddMessage(combat.round_summaries().back(), EVENT_DANGER, report);

    // Earn xp for all kills, even if defeated
    int xp = 3;
    if (girl.has_active_trait(traits::QUICK_LEARNER)) xp += 2;
    else if (girl.has_active_trait(traits::SLOW_LEARNER)) xp -= 2;

    const int num = OrgNumMem - enemy_gang->m_Num;

    girl.exp(num * xp);

    if (res)  // She wins.  Yay!
    {
        girl.combat(1);
        girl.magic(1);
        girl.agility(1);
        girl.upd_temp_stat(STAT_LIBIDO, num, true);  // There's nothing like killin ta make ya horny!
        girl.confidence(num);
        girl.fame(num);

        girl.upd_Enjoyment(ACTION_COMBAT, num);

        std::stringstream msg;

        // MYR: I decided to not say gang in the text. It can be confused with a player or enemy organization's
        //     gang, when it is neither.
        if (OrgNumMem == 1)
        {
          msg << ("A customer tried to attack her. She killed ") << (g_Dice.percent(20) ? ("her.") : ("him."));
        }
        else
        {
          if (enemy_gang->m_Num == 0)
            msg << ("A group of customers tried to assault her. She thrashed all ") << OrgNumMem << (" of them.");
          else
            msg << ("A group of ") << OrgNumMem << (" customers tried to assault her. They fled after she killed ")
            << num << (" of them.");
        }
        girl.AddMessage(msg.str(), EImageBaseType::COMBAT, EVENT_WARNING);
    }

    // Losing is dealt with later in customer_rapes (called from work_related_violence)
    return res;
}

/*
 * I think these next three could use a little detail
 * MYR: Added the requested detail (in GetGirlAttackedString() below)
 */
void cJobManager::customer_rape(sGirl& girl, int numberofattackers)
{
    int attacktype = SKILL_COMBAT;                                                // can be anything
    /* */if (numberofattackers > 1 && g_Dice.percent(40)) attacktype = SKILL_GROUP;    // standard group attack
    else if (numberofattackers > 1 && g_Dice.percent(40)) attacktype = SKILL_BDSM;    // special group attack
    else if (g_Dice.percent(20)) attacktype = SKILL_LESBIAN;                        // female attacker
    else if (g_Dice.percent(40)) attacktype = SKILL_NORMALSEX;                        // single male attacker

    std::stringstream ss;
    ss << girl.FullName() << GetGirlAttackedString(attacktype);

    girl.AddMessage(ss.str(), EImageBaseType::DEATH, EVENT_DANGER);

    // Made this more harsh, so the player hopefully notices it
    //girl.health(-(g_Dice%10 + 5));  // Oops, can drop health below zero after combat is considered
    girl.happiness(-40);
    girl.confidence(-40);
    girl.obedience(-10);
    girl.spirit(-40);
    girl.libido(-4);
    girl.upd_temp_stat(STAT_LIBIDO, -40, true);
    girl.tiredness(60);
    girl.pcfear(20);
    girl.pclove(-40);
    cGirls::GirlInjured(girl, 10); // MYR: Note
    girl.upd_Enjoyment(ACTION_SEX, -30);

    // `J` do Pregnancy and/or STDs
    bool preg = false, std = false, a = false, c = false, h = false, s = false;
    sCustomer Cust = GetMiscCustomer(*girl.m_Building);
    Cust.m_Amount = numberofattackers;

    if (attacktype == SKILL_GROUP || attacktype == SKILL_NORMALSEX)
    {
        Cust.m_IsWoman = false;
        preg = !girl.calc_group_pregnancy(Cust, false);
    }
    if (attacktype == SKILL_LESBIAN)
    {
        Cust.m_IsWoman = true;
    }

    if (Cust.has_active_trait(traits::AIDS))      a = true;
    if (Cust.has_active_trait(traits::CHLAMYDIA)) c = true;
    if (Cust.has_active_trait(traits::SYPHILIS))  s = true;
    if (Cust.has_active_trait(traits::HERPES))    h = true;
    std = a || c || s || h;
    if (!std && g_Dice.percent(5))
    {
        std = true;
        if      (g_Dice.percent(20)) a = true;
        else if (g_Dice.percent(20)) c = true;
        else if (g_Dice.percent(20)) s = true;
        else /*                   */    h = true;
    }

    if (preg || std) {
        ss.str("");
        ss << girl.FullName() << "'s rapist";
        if (numberofattackers > 1) ss << "s left their";
        else ss << " left " << (Cust.m_IsWoman ? "her" : "his");
        ss << " calling card behind, ";
        if (preg)            { ss << "a baby in her belly"; }
        if (preg && std)    { ss << " and "; }
        else if (preg)        { ss << ".\n \n"; }
        if (a || c || s || h)    { bool _and = false;
            if (a)    { girl.gain_trait(traits::AIDS);        ss << "AIDS"; }
            if (a && (c || s || h))                            {    ss << " and ";        _and = true; }
            if (c)    { girl.gain_trait(traits::CHLAMYDIA);    ss << "Chlamydia";    _and = false; }
            if (!_and && (a || c) && (s || h))                {    ss << " and ";        _and = true; }
            if (s)    { girl.gain_trait(traits::SYPHILIS);    ss << "Syphilis";    _and = false; }
            if (!_and && (a || c || s) && h)                {    ss << " and "; }
            if (h)    { girl.gain_trait(traits::HERPES);        ss << "Herpes"; }
            ss << ".\n \n";
        }
        girl.AddMessage(ss.str(), EImageBaseType::DEATH, EVENT_DANGER);
    }
}

// MYR: Lots of different ways to say the girl had a bad day
std::string cJobManager::GetGirlAttackedString(int attacktype)
{
    int roll1 = 0, roll2 = 0, roll3 = 0, rolla = 0;
    int maxroll1 = 21, maxroll2 = 20, maxroll3 = 21;

    std::stringstream ss;
    ss << (" was ");

    if (attacktype == SKILL_GROUP)            // standard group attack
    {
        roll1 = g_Dice % maxroll1 + 1;
        roll2 = g_Dice % maxroll2 + 1;
        rolla = g_Dice % 5 + 1;
        switch (rolla)
        {
        case 1:        roll3 = 1;    break;    // "customers."
        case 2:        roll3 = 8;    break;    // "priests."
        case 3:        roll3 = 9;    break;    // "orcs. (What?)"
        case 4:        roll3 = 18;    break;    // "members of the militia."
        case 5:        roll3 = 20;    break;    // "tentacles from the sewers."
        default:    roll3 = g_Dice % maxroll3 + 1;    break;    // fallback just in case
        }
    }
    if (attacktype == SKILL_BDSM)            // special group attack
    {
        roll1 = g_Dice % maxroll1 + 1;    // if roll1 is a BDSM text...
        if (roll1 == 3    ||    // "put on the wood horse"
            roll1 == 4    ||    // "tied to the bed"
            roll1 == 7    ||    // "tied up"
            roll1 == 8    ||    // "wrestled to the floor"
            roll1 == 9    ||    // "slapped in irons"
            roll1 == 10    ||    // "cuffed"
            roll1 == 11    ||    // "put in THAT machine"
            roll1 == 14    ||    // "hand-cuffed during kinky play"
            roll1 == 18    ||    // "chained to the porch railing"
            roll1 == 19    ||    // "tied up BDSM-style"
            roll1 == 20    ||    // "stretched out on the torture table"
            roll1 == 21    )    // "tied up and hung from the rafters"
        {
            roll2 = g_Dice % maxroll2 + 1;    // ...then roll2 can be anything
        }
        else    // ...otherwise roll2 needs to be BDSM
        {
            rolla = g_Dice % 6 + 1;
            switch (rolla)
            {
            case 1:        roll2 = 1;    break;    // "abused"
            case 2:        roll2 = 2;    break;    // "whipped"
            case 3:        roll2 = 13;    break;    // "had her fleshy bits pierced"
            case 4:        roll2 = 15;    break;    // "penetrated by some object"
            case 5:        roll2 = 17;    break;    // "tortured"
            case 6:        roll2 = 20;    break;    // "her skin was pierced by sharp things"
            default:    roll2 = g_Dice % maxroll2 + 1;    break;    // fallback just in case
            }
        }
        roll3 = g_Dice % maxroll3 + 1;
    }
    if (attacktype == SKILL_LESBIAN)        // female attacker
    {
        roll1 = g_Dice % maxroll1 + 1;
        roll2 = g_Dice % maxroll2 + 1;
        rolla = g_Dice % 13 + 1;
        switch (rolla)
        {
        case 1:        roll3 = 1;        break;    // "customers."
        case 2:     roll3 = 4;        break;    // "the other girls."
        case 3:     roll3 = 6;        break;    // "a ghost (She claims.)"
        case 4:     roll3 = 7;        break;    // "an enemy gang member."
        case 5:     roll3 = 10;        break;    // "a doppleganger disguised as a human."
        case 6:     roll3 = 11;        break;    // "a jealous wife."
        case 7:     roll3 = 13;        break;    // "a public health official."
        case 8:     roll3 = 15;        break;    // "some raving loonie."
        case 9:     roll3 = 16;        break;    // "a ninja!"
        case 10:     roll3 = 17;        break;    // "a pirate. (Pirates rule! Ninjas drool!)"
        case 11:     roll3 = 18;        break;    // "members of the militia."
        case 12:     roll3 = 19;        break;    // "your mom (It runs in the family.)"
        case 13:     roll3 = 21;        break;    // "a vengeful family member."
        default:    roll3 = g_Dice % maxroll3 + 1;    break;    // fallback just in case
        }
    }
    if (attacktype == SKILL_NORMALSEX)        // single male attacker
    {
        roll1 = g_Dice % maxroll1 + 1;
        roll2 = g_Dice % maxroll2 + 1;
        rolla = g_Dice % 12 + 1;
        switch (rolla)
        {
        case 1:     roll3 = 2;        break;    // "some sadistic monster."
        case 2:     roll3 = 3;        break;    // "a police officer."
        case 3:     roll3 = 5;        break;    // "Batman! (Who knew?)"
        case 4:     roll3 = 6;        break;    // "a ghost (She claims.)"
        case 5:     roll3 = 7;        break;    // "an enemy gang member."
        case 6:     roll3 = 10;        break;    // "a doppleganger disguised as a human."
        case 7:     roll3 = 12;        break;    // "a jealous husband."
        case 8:     roll3 = 13;        break;    // "a public health official."
        case 9:     roll3 = 15;        break;    // "some raving loonie."
        case 10:     roll3 = 16;        break;    // "a ninja!"
        case 11:     roll3 = 17;        break;    // "a pirate. (Pirates rule! Ninjas drool!)"
        case 12:     roll3 = 21;        break;    // "a vengeful family member."
        default:    roll3 = g_Dice % maxroll3 + 1;    break;    // fallback just in case
        }
    }
    // if nothing above worked then just do the default anything goes
    if (attacktype == SKILL_COMBAT || roll1 == 0 || roll2 == 0 || roll3 == 0)    // can be anything
    {
        roll1 = g_Dice % maxroll1 + 1;
        roll2 = g_Dice % maxroll2 + 1;
        roll3 = g_Dice % maxroll3 + 1;
    }

    switch (roll1)
    {
      case 1: ss << ("cornered"); break;
      case 2: ss << ("hauled to the dungeon"); break;
      case 3: ss << ("put on the wood horse"); break;
      case 4: ss << ("tied to the bed"); break;
      case 5: ss << ("locked in the closet"); break;
      case 6: ss << ("drugged"); break;
      case 7: ss << ("tied up"); break;
      case 8: ss << ("wrestled to the floor"); break;
      case 9: ss << ("slapped in irons"); break;
      case 10: ss << ("cuffed"); break;
      case 11: ss << ("put in THAT machine"); break;
      case 12: ss << ("konked on the head"); break;
      case 13: ss << ("calmly sat down"); break;
      case 14: ss << ("hand-cuffed during kinky play"); break;
      case 15: ss << ("caught off guard during fellatio"); break;
      case 16: ss << ("caught cheating at cards"); break;
      case 17: ss << ("found sleeping on the job"); break;
      case 18: ss << ("chained to the porch railing"); break;
      case 19: ss << ("tied up BDSM-style"); break;
      case 20: ss << ("stretched out on the torture table"); break;
      case 21: ss << ("tied up and hung from the rafters"); break;
    }

    ss << (" and ");

    switch (roll2)
    {
      case 1: ss << ("abused"); break;
      case 2: ss << ("whipped"); break;
      case 3: ss << ("yelled at"); break;
      case 4: ss << ("assaulted"); break;
      case 5: ss << ("raped"); break;
      case 6: ss << ("her cavities were explored"); break;
      case 7: ss << ("her hair was done"); break;
      case 8: ss << ("her feet were licked"); break;
      case 9: ss << ("relentlessly tickled"); break;
      case 10: ss << ("fisted"); break;
      case 11: ss << ("roughly fondled"); break;
      case 12: ss << ("lectured to"); break;
      case 13: ss << ("had her fleshy bits pierced"); break;
      case 14: ss << ("slapped around"); break;
      case 15: ss << ("penetrated by some object"); break;
      case 16: ss << ("shaved"); break;
      case 17: ss << ("tortured"); break;
      case 18: ss << ("forced outside"); break;
      case 19: ss << ("forced to walk on a knotted rope"); break;
      case 20: ss << ("her skin was pierced by sharp things"); break;
    }

    ss << (" by ");

    switch (roll3)
    {
      case 1: ss << ("customers."); break;
      case 2: ss << ("some sadistic monster."); break;
      case 3: ss << ("a police officer."); break;
      case 4: ss << ("the other girls."); break;
      case 5: ss << ("Batman! (Who knew?)"); break;
      case 6: ss << ("a ghost (She claims.)"); break;
      case 7: ss << ("an enemy gang member."); break;
      case 8: ss << ("priests."); break;
      case 9: ss << ("orcs. (What?)"); break;
      case 10: ss << ("a doppleganger disguised as a human."); break;
      case 11: ss << ("a jealous wife."); break;
      case 12: ss << ("a jealous husband."); break;
      case 13: ss << ("a public health official."); break;
      case 14: ss << ("you!"); break;
      case 15: ss << ("some raving loonie."); break;
      case 16: ss << ("a ninja!"); break;
      case 17: ss << ("a pirate. (Pirates rule! Ninjas drool!)"); break;
      case 18: ss << ("members of the militia."); break;
      case 19: ss << ("your mom (It runs in the family.)"); break;
      case 20: ss << ("tentacles from the sewers."); break;
      case 21: ss << ("a vengeful family member."); break;
    }

    return ss.str();
}

double calc_pilfering(sGirl& girl)
{
    double factor = 0.0;
    if (is_addict(girl) && girl.m_Money < 100)            // on top of all other factors, an addict will steal to feed her habit
        factor += (is_addict(girl, true) ? 0.5 : 0.1);        // hard drugs will make her steal more
    // let's work out what if she is going steal anything
    if (girl.pclove() >= 50 || girl.obedience() >= 50) return factor;            // love or obedience will keep her honest
    if (girl.pcfear() > -girl.pclove()) return factor;                            // if her fear is greater than her hate, she won't dare steal
    // `J` yes they do // if (girl.is_slave()) return factor;                    // and apparently, slaves don't steal
    if (girl.pclove() < -40) return factor + 0.15;                                // given all the above, if she hates him enough, she'll steal
    if (girl.confidence() > 70 && girl.spirit() > 50) return factor + 0.15;    // if she's not motivated by hatred, she needs to be pretty confident
    return factor;    // otherwise, she stays honest (aside from addict factored-in earlier)
}

sPaymentData cJobManager::CalculatePay(sGirl& girl, sWorkJobResult result)
{
    sPaymentData retval{0, 0, 0, 0, 0};
    // no pay or tips, no need to continue
    if(result.Wages == 0 && result.Tips == 0 && result.Earnings == 0) return retval;

    if(girl.is_unpaid()) {
        result.Wages = 0;
    }

    retval.Wages = result.Wages;
    retval.Earnings = result.Earnings;
    retval.Tips = result.Tips;

    if (result.Tips > 0)        // `J` check tips first
    {
        if (girl.keep_tips())
        {
            girl.m_Money += result.Tips;    // give her the tips directly
            retval.GirlGets += result.Tips;
        }
        else    // otherwise add tips into pay
        {
            result.Earnings += result.Tips;
        }
    }

    // TODO when can this be false? How do we handle that case?
    if(girl.m_Building) {
        // TODO check where we are handling the money processing for girl's payment
        girl.m_Building->m_Finance.girl_support(result.Wages);
    }

    retval.PlayerGets -= result.Wages;
    girl.m_Money += result.Wages;    // she gets it all
    retval.GirlGets += result.Wages;


    // work out how much gold (if any) she steals
    double steal_factor = calc_pilfering(girl);
    int stolen = int(steal_factor * result.Earnings);
    result.Earnings -= stolen;
    retval.Earnings -= stolen;
    girl.m_Money += stolen;

    // so now we are to the house percent.
    int house = (girl.house() * result.Earnings) / 100;       // the house takes its cut of whatever's left
    retval.PlayerGets += house;

    girl.m_Money += result.Earnings - house;               // The girl collects her part of the pay
    retval.GirlGets += result.Earnings - house;
    if(girl.m_Building) {
        // TODO ditto
        girl.m_Building->m_Finance.brothel_work(house);                         // and add the rest to the brothel finances
    }

    if (!stolen) return retval;                                    // If she didn't steal anything, we're done
    sGang* gang = g_Game->gang_manager().GetGangOnMission(MISS_SPYGIRLS);    // if no-one is watching for theft, we're done
    if (!gang) return retval;
    int catch_pc = g_Game->gang_manager().chance_to_catch(girl);            // work out the % chance that the girl gets caught
    if (!g_Dice.percent(catch_pc)) return retval;                    // if they don't catch her, we're done

    // OK: she got caught. Tell the player
    std::stringstream gmess; gmess << "Your Goons spotted " << girl.FullName() << " taking more gold then she reported.";
    gang->AddMessage(gmess.str());
    return retval;
}

void cJobManager::handle_simple_job(sGirl& girl, bool is_night)
{
    auto sw = girl.get_job(is_night);;
    auto brothel = girl.m_Building;
    if(!brothel) {
        g_LogFile.error("jobs", "Could not handle simple job, because girl '", girl.FullName(), "' is not in any building");
        return;
    }

    // do their job
    auto result = do_job(girl, is_night);

    //        Summary Messages
    if (result.Refused)
    {
        brothel->m_Fame -= girl.fame();
        girl.AddMessage("${name} refused to work so she made no money.", EImageBaseType::PROFILE, EVENT_SUMMARY);
    }
    else
    {
        brothel->m_Fame += girl.fame();
        std::stringstream ss;
        auto money_data = CalculatePay(girl, result);
        ss << "${name} made " << money_data.Earnings;
        if(money_data.Tips != 0) {
            ss << " and " << money_data.Tips << " in tips. ";
        } else {
            ss << " gold. ";
        }
        if (money_data.Wages > 0) ss << "You paid her a salary of " << money_data.Wages << ". ";
        ss << "In total, she got " << money_data.GirlGets << " gold and you ";
        if(money_data.PlayerGets > 0) {
           ss << "got " << money_data.PlayerGets << " gold.";
        } else {
            ss << "spent " << -money_data.PlayerGets << " gold.";
        }

        girl.AddMessage(ss.str(), EImageBaseType::PROFILE, EVENT_SUMMARY);
    }
}

sWorkJobResult cJobManager::do_job(sGirl& girl, bool is_night)
{
    return do_job(girl.get_job(is_night), girl, is_night);
}

sWorkJobResult cJobManager::do_job(JOBS job_id, sGirl& girl, bool is_night)
{
    auto ctx{g_Game->push_error_context("job: " + get_job_name(job_id))};
    assert(m_OOPJobs[job_id] != nullptr);
    auto result = m_OOPJobs[job_id]->Work(girl, is_night, g_Dice);
    if(is_night) {
        girl.m_Refused_To_Work_Night = result.Refused;
    } else {
        girl.m_Refused_To_Work_Day = result.Refused;
    }
    return result;
}

void cJobManager::handle_pre_shift(sGirl& girl, bool is_night) {
    auto job_id = girl.get_job(is_night);
    auto ctx{g_Game->push_error_context("pre@job: " + get_job_name(job_id))};
    assert(m_OOPJobs[job_id] != nullptr);
    m_OOPJobs[job_id]->PreShift(girl, is_night, g_Dice);
}


// `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >> cClinic.cpp >> is_Surgery_Job
bool cJobManager::is_Surgery_Job(int testjob) {
    if (testjob == JOB_CUREDISEASES ||
        testjob == JOB_GETABORT ||
        testjob == JOB_COSMETICSURGERY ||
        testjob == JOB_LIPO ||
        testjob == JOB_BREASTREDUCTION ||
        testjob == JOB_BOOBJOB ||
        testjob == JOB_VAGINAREJUV ||
        testjob == JOB_TUBESTIED ||
        testjob == JOB_FERTILITY ||
        testjob == JOB_FACELIFT ||
        testjob == JOB_ASSJOB)
        return true;
    return false;
}

void cJobManager::CatchGirl(sGirl& girl, std::stringstream& fuckMessage, const sGang* guardgang) {// try to find an item
    const sInventoryItem* item = nullptr;
    if(g_Game->player().inventory().has_item("Brainwashing Oil")) {
        item = g_Game->inventory_manager().GetItem("Brainwashing Oil");
    } else if(g_Game->player().inventory().has_item("Necklace of Control")) {
        item = g_Game->inventory_manager().GetItem("Necklace of Control");
    } else if(g_Game->player().inventory().has_item("Slave Band")) {
        item = g_Game->inventory_manager().GetItem("Slave Band");
    }
    if (item || g_Dice.percent(10)) {
        std::string itemname;
        std::stringstream itemtext;
        itemtext << "Your gang " << (guardgang ? guardgang->name() : "");
        std::stringstream CGmsg;
        // `J` create the customer
        auto custgirl = g_Game->CreateRandomGirl(SpawnReason::CUSTOMER,
                                                 18 + (std::max(0, g_Dice % 40 - 10)));
        cGirls::SetSlaveStats(*custgirl);
        int emprisontraittime = 1;
        custgirl->pclove(-(g_Dice % 100 + 100));
        custgirl->pcfear(g_Dice % 50 + 50);
        custgirl->m_Enjoyment[ACTION_COMBAT] -= (g_Dice % 50 + 20);
        custgirl->m_Enjoyment[ACTION_SEX] -= (g_Dice % 50 + 20);

        if (!item)    // no item
        {
            itemtext << " beat up " << custgirl->FullName();
            fuckMessage << itemtext.str() << " and dragged her to the dungeon.";
            itemtext
                    << " until she stops fighting back. They place your brand on her and bring your new slave to the dungeon.";
            emprisontraittime = 5 + g_Dice % 21;
            custgirl->set_stat(STAT_HEALTH, 5 + g_Dice % 70);
            custgirl->set_stat(STAT_HAPPINESS, g_Dice % 50);
            custgirl->set_stat( STAT_TIREDNESS, 50 + g_Dice % 51);
            girl.obedience(g_Dice % 10);
            girl.pcfear(g_Dice % 10);
            girl.pclove(-(g_Dice % 20));
            girl.spirit(-(g_Dice % 10));
        } else        // item was found
        {
            itemname = item->m_Name;
            custgirl->add_item(item);
            custgirl->equip(item, true);
            g_Game->player().inventory().remove_item(item);
            if (itemname == "Brainwashing Oil") {
                emprisontraittime += g_Dice % 5;
                itemtext << " forced a bottle of Brainwashing Oil down her throat";
                fuckMessage << itemtext.str() << " and dragged her to the dungeon.";
                itemtext << ". After a few minutes of struggling,";
            } else if (itemname == "Necklace of Control") {
                emprisontraittime += g_Dice % 10;
                itemtext << " placed a Necklace of Control around her neck";
                fuckMessage << itemtext.str() << " and dragged her to the dungeon.";
                itemtext << ". After a few minutes of struggling, the magic in the necklace activated and";
            } else if (itemname == "Slave Band") {
                emprisontraittime += g_Dice % 15;
                itemtext << " placed a Slave Band on her arm";
                fuckMessage << itemtext.str() << " and dragged her to the dungeon.";
                itemtext << ". After a few minutes of struggling, the magic in the Slave Band activated and";
            }
            itemtext << " your new slave, " << custgirl->FullName() << ", was taken to the dungeon.";

        }
        // `J` do all the messages
        CGmsg << "${name} was caught trying to run out without paying for services provided by "
              << girl.FullName() << ".\n \n" << itemtext.str();
        custgirl->add_temporary_trait(traits::EMPRISONED_CUSTOMER, emprisontraittime);    // add temp trait
        if (g_Dice.percent(75)) custgirl->lose_trait(traits::VIRGIN);                // most of the time she will not be a virgin
        g_Game->player().suspicion(g_Dice % 10);
        g_Game->player().disposition(-(g_Dice % 10));
        g_Game->player().customerfear(g_Dice % 10);
        custgirl->m_Money = 0;

        custgirl->AddMessage(CGmsg.str(), EImageBaseType::DEATH, EVENT_WARNING);
        // `J` add the customer to the dungeon
        g_Game->dungeon().AddGirl(custgirl, DUNGEON_CUSTNOPAY);
    } else {
        g_Game->PushEvent(EDefaultEvent::CUSTOMER_NOPAY);    // if not taken as a slave, use the old code
    }
}

void cJobManager::register_job(std::unique_ptr<IGenericJob> job) {
    assert(job != nullptr);
    job->OnRegisterJobManager(*this);
    m_OOPJobs[job->job()] = std::move(job);
}

const IGenericJob* cJobManager::get_job(JOBS job) const {
    if(job < 0 || job >= m_OOPJobs.size()) {
      g_LogFile.error("jobmgr",
		      "Job ", job, " is outside the (0..", m_OOPJobs.size(), "( range.");
      throw std::out_of_range("cJobManager::get_job()");
    }

    auto& ptr = m_OOPJobs[job];
    if(!ptr) {
      g_LogFile.error("jobmgr",
		      "Job ", job, " has not been registered.");
      throw std::invalid_argument("cJobManager::get_job()");
    }

    return ptr.get();
}

const std::string& cJobManager::get_job_name(JOBS job) const {
    return get_job(job)->get_info().Name;
}

const std::string& cJobManager::get_job_brief(JOBS job) const {
    return get_job(job)->get_info().ShortName;
}

const std::string& cJobManager::get_job_description(JOBS job) const {
    return get_job(job)->get_info().Description;
}

bool cJobManager::is_free_only(JOBS job) const {
    return get_job(job)->get_info().FreeOnly;
}

bool cJobManager::job_filter(int Filter, JOBS job) const {
    auto& filter = JobFilters[Filter];
    return std::count(begin(filter.Contents), end(filter.Contents), job) > 0;
}

bool cJobManager::AddictBuysDrugs(std::string Addiction, std::string Drug, sGirl& girl, IBuilding * brothel, bool Day0Night1)
{
    int avail = g_Game->shop().CountItem(Drug);
    if(avail == 0) return false;                        // quit if the shop does not have the item

    auto item = g_Game->inventory_manager().GetItem(Drug);
    // try to buy the item
    if(!g_Game->shop().GirlBuyItem(girl, *item))   return false;

    // If a matron is on shift, she may catch the girl buying drugs
    if ((brothel->num_girls_on_job(JOB_MATRON, true) >= 1 || brothel->num_girls_on_job(JOB_MATRON, false) >= 1)
        && g_Dice.percent(70))
    {
        girl.AddMessage("Matron confiscates drugs", EImageBaseType::PROFILE, EVENT_WARNING);
        return girl.remove_item(item) == 0;
        return false;
    }
    return true;
}
