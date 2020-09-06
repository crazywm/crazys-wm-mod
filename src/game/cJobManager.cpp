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
#include "buildings/cBrothel.h"
#include "buildings/queries.h"
#include "character/cCustomers.h"
#include "cRng.h"
#include "cInventory.h"
#include <sstream>
#include "CLog.h"
#include "cTrainable.h"
#include "cTariff.h"
#include "cGold.h"
#include "cGangs.h"
#include "Game.hpp"
#include "scripting/GameEvents.h"
#include "combat/combat.h"
#include "jobs/Film.h"
#include "jobs/Crafting.h"
#include "Inventory.h"
#include "cRival.h"
#include "buildings/cDungeon.h"
#include "character/predicates.h"
#include "character/cPlayer.h"
#include "buildings/cBuildingManager.h"
#include "cGirlGangFight.h"

extern cRng g_Dice;

namespace settings {
    extern const char* WORLD_RAPE_STREETS;
    extern const char* WORLD_RAPE_BROTHEL;
}

cJobManager::cJobManager() = default;
cJobManager::~cJobManager() = default;

void cJobManager::Setup()
{
    m_OOPJobs.resize(NUM_JOBS);

    auto register_filter = [&](JOBFILTER filter, JOBS first, JOBS last) {
        for(int i = first; i <= last; ++i) {
            JobFilters[filter].Contents.push_back((JOBS)i);
        }
    };

    // `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >> cJobManager.cpp > Setup

    // much simplier then trying to get the sJob working with this (plus a smaller memory footprint...?maybe)
    // General Jobs
    JobFilters[JOBFILTER_GENERAL] = sJobFilter{"General", "These are general jobs that work individually in any building."};
    register_filter(JOBFILTER_GENERAL, JOB_RESTING, JOB_BEASTCARER);

    // - Bar Jobs
    JobFilters[JOBFILTER_BAR] = sJobFilter{"Bar", "These are jobs for running a bar."};
    register_filter(JOBFILTER_BAR, JOB_BARMAID, JOB_BARCOOK);
    // - Gambling Hall Jobs
    JobFilters[JOBFILTER_GAMBHALL] = sJobFilter{"Gambling Hall", "These are jobs for running a gambling hall."};
    register_filter(JOBFILTER_GAMBHALL, JOB_DEALER, JOB_WHOREGAMBHALL);
    // - Sleazy Bar Jobs        // Changing all references to Strip Club, just sounds better to me and more realistic. -PP
    JobFilters[JOBFILTER_SLEAZYBAR] = sJobFilter{"Strip Club", "These are jobs for running a Strip Club."};
    register_filter(JOBFILTER_SLEAZYBAR, JOB_SLEAZYBARMAID, JOB_BARWHORE);
    // - Brothel Jobs
    JobFilters[JOBFILTER_BROTHEL] = sJobFilter{"Brothel", "These are jobs for running a brothel."};
    register_filter(JOBFILTER_BROTHEL, JOB_MASSEUSE, JOB_WHORESTREETS);





    // Studio Crew
    JobFilters[JOBFILTER_STUDIOCREW] = sJobFilter{"Studio Crew", "These are jobs for running a movie studio."};
    register_filter(JOBFILTER_STUDIOCREW, JOB_FILMFREETIME, JOB_STAGEHAND);
    //JobData[JOB_SOUNDTRACK] = sJobData("Sound Track", "SndT", WorkSoundTrack, JP_SoundTrack);
    //JobData[JOB_SOUNDTRACK].description = ("She will clean up the audio and add music to the scenes. (not required but helpful)");


    // `J` When adding new Studio Scenes, search for "J-Add-New-Scenes"  :  found in >> cJobManager.cpp > Setup

    // Studio - Non-Sex Scenes
    JobFilters[JOBFILTER_STUDIONONSEX] = sJobFilter{"Non-Sex Scenes", "These are scenes without sex."};
    register_filter(JOBFILTER_STUDIONONSEX, JOB_FILMACTION, JOB_FILMMUSIC);

    // Studio - Softcore Porn
    JobFilters[JOBFILTER_STUDIOSOFTCORE] = sJobFilter{"Softcore Scenes", "These are scenes with just the one girl."};
    register_filter(JOBFILTER_STUDIOSOFTCORE, JOB_FILMMAST, JOB_FILMTEASE);

    // Studio - Porn
    JobFilters[JOBFILTER_STUDIOPORN] = sJobFilter{"Porn Scenes", "These are regular sex scenes."};
    register_filter(JOBFILTER_STUDIOPORN, JOB_FILMANAL, JOB_FILMTITTY);

    // Studio - Hardcore porn
    JobFilters[JOBFILTER_STUDIOHARDCORE] = sJobFilter{"Hardcore Scenes", "These are rough scenes that not all girls would be comfortable with."};
    register_filter(JOBFILTER_STUDIOHARDCORE, JOB_FILMBEAST, JOB_FILMPUBLICBDSM);

    // Studio - Random
    JobFilters[JOBFILTER_RANDSTUDIO] = sJobFilter{"Random Scenes", "Let the game choose what scene the girl will film."};
    register_filter(JOBFILTER_RANDSTUDIO, JOB_FILMRANDOM, JOB_FILMRANDOM);


    // - Arena Jobs
    JobFilters[JOBFILTER_ARENA] = sJobFilter{"Arena", "These are jobs for running an arena."};
    register_filter(JOBFILTER_ARENA, JOB_FIGHTBEASTS, JOB_FIGHTTRAIN);
    //- Arena Staff
    JobFilters[JOBFILTER_ARENASTAFF] = sJobFilter{"Arena Staff", "These are jobs that help run an arena."};
    register_filter(JOBFILTER_ARENASTAFF, JOB_ARENAREST, JOB_CLEANARENA);

    // - Community Centre Jobs
    JobFilters[JOBFILTER_COMMUNITYCENTRE] = sJobFilter{"Community Centre", "These are jobs for running a community centre."};
    register_filter(JOBFILTER_COMMUNITYCENTRE, JOB_CENTREREST, JOB_CLEANCENTRE);
    // - Counseling Centre Jobs
    JobFilters[JOBFILTER_COUNSELINGCENTRE] = sJobFilter{"Counseling Centre", "These are jobs for running a counseling centre."};
    register_filter(JOBFILTER_COUNSELINGCENTRE, JOB_COUNSELOR, JOB_ANGER);

    // - Clinic Surgeries
    JobFilters[JOBFILTER_CLINIC] = sJobFilter{"Medical Clinic", "These are procerures the girls can have done in the Medical Clinic."};
    register_filter(JOBFILTER_CLINIC, JOB_GETHEALING, JOB_FERTILITY);
    // Clinic staff
    JobFilters[JOBFILTER_CLINICSTAFF] = sJobFilter{"Clinic Staff", "These are jobs that help run a medical clinic."};
    register_filter(JOBFILTER_CLINICSTAFF, JOB_CLINICREST, JOB_JANITOR);

    // - Farm Staff
    JobFilters[JOBFILTER_FARMSTAFF] = sJobFilter{"Farm Staff", "These are jobs for running a farm"};
    register_filter(JOBFILTER_FARMSTAFF, JOB_FARMREST, JOB_FARMHAND);

    // - Laborers
    JobFilters[JOBFILTER_LABORERS] = sJobFilter{"Laborers", "These are jobs your girls can do at your farm."};
    register_filter(JOBFILTER_LABORERS, JOB_FARMER, JOB_MILK);
    // - Producers
    JobFilters[JOBFILTER_PRODUCERS] = sJobFilter{"Producers", "These are jobs your girls can do at your farm."};
    register_filter(JOBFILTER_PRODUCERS, JOB_BUTCHER, JOB_MAKEPOTIONS);

    // - House
    JobFilters[JOBFILTER_HOUSE] = sJobFilter{"Player House", "These are jobs your girls can do at your house."};
    register_filter(JOBFILTER_HOUSE, JOB_HOUSEREST, JOB_CLEANHOUSE);
    //JobData[JOB_PONYGIRL].name = ("Pony Girl");
    //JobData[JOB_PONYGIRL].brief = "PGil";
    //JobData[JOB_PONYGIRL].description = ("She will be trained to become a pony girl.");
    //JobFunc[JOB_PONYGIRL] = &WorkFarmPonyGirl;

    JobFilters[JOBFILTER_HOUSETTRAINING] = sJobFilter{"Sex Training", "You will take a personal interest in training the girl in sexual matters."};
    register_filter(JOBFILTER_HOUSETTRAINING, JOB_PERSONALTRAINING, JOB_HOUSEPET);


/*
    JobData[JOB_INDUNGEON].name    = ("In the Dungeon");
    JobData[JOB_INDUNGEON].brief    = "Dngn";
    JobData[JOB_INDUNGEON].description    = ("She is languishing in the dungeon.");
    JobData[JOB_RUNAWAY].name    = ("Runaway");
    JobData[JOB_RUNAWAY].brief    = "RunA";
    JobData[JOB_RUNAWAY].description    = ("She has run away.");
*/

    RegisterCraftingJobs(*this);
    RegisterSurgeryJobs(*this);
    RegisterWrappedJobs(*this);
    RegisterManagerJobs(*this);
    RegisterFilmJobs(*this);
    RegisterTherapyJobs(*this);
    RegisterBarJobs(*this);
    RegisterFarmJobs(*this);
    RegisterClinicJobs(*this);
}

sCustomer cJobManager::GetMiscCustomer(IBuilding& brothel)
{
    sCustomer cust = g_Game->GetCustomer(brothel);
    brothel.m_MiscCustomers+=1;
    return cust;
}

// ----- Job related

void cJobManager::do_advertising(IBuilding& brothel, bool Day0Night1)
{  // advertising jobs are handled before other jobs, more particularly before customer generation
    brothel.m_AdvertisingLevel = 1.0;  // base multiplier
    brothel.girls().apply([&](sGirl& current){
        // Added test for current shift, was running each shift twice -PP
        if ((current.m_DayJob == JOB_ADVERTISING) && (Day0Night1 == SHIFT_DAY))
        {
            current.m_Refused_To_Work_Day = do_job(current, false);
        }
        if ((current.m_NightJob == JOB_ADVERTISING) && (Day0Night1 == SHIFT_NIGHT))
        {
            current.m_Refused_To_Work_Night = do_job(current, true);
        }
    });
}

void cJobManager::do_whorejobs(IBuilding& brothel, bool Day0Night1)
{ // Whores get first crack at any customers.
    brothel.girls().apply([&](sGirl& current)
    {
        switch (current.get_job(Day0Night1))
        {
        case JOB_WHOREBROTHEL:
        case JOB_BARWHORE:
        case JOB_WHOREGAMBHALL:
            do_job(current, Day0Night1);
            break;
        default:
            break;
        }
    });
}

void cJobManager::do_custjobs(IBuilding& brothel, bool Day0Night1)
{ // Customer taking jobs get first crack at any customers before customer service.
    brothel.girls().apply([&](sGirl& current)
    {
        switch (current.get_job(Day0Night1))
        {
        case JOB_BARMAID:
        case JOB_WAITRESS:
        case JOB_SINGER:
        case JOB_PIANO:
        case JOB_DEALER:
        case JOB_ENTERTAINMENT:
        case JOB_XXXENTERTAINMENT:
        case JOB_SLEAZYBARMAID:
        case JOB_SLEAZYWAITRESS:
        case JOB_BARSTRIPPER:
        case JOB_MASSEUSE:
        case JOB_BROTHELSTRIPPER:
        case JOB_PEEP:
            do_job(current, Day0Night1);
            break;
        default:
            break;
        }
    });
}

bool cJobManager::FullTimeJob(JOBS Job)
{
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
        Job ==    JOB_TRAINING            ||    // trains skills at a basic level
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
        Job ==    JOB_FILMFREETIME        ||    // Taking a day off

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
    JOBS rest = JOB_RESTING;
    if(Girl.m_Building)
        rest = Girl.m_Building->m_RestJob;

    auto check = m_OOPJobs[JobID]->is_job_valid(Girl);
    if(!check) {
        g_Game->push_message(check.Reason, 0);
        return false;
    }

    // rest jobs
    if (JobID == JOB_FILMFREETIME)
    {
        Girl.m_NightJob = Girl.m_DayJob = JobID;
    }
    else if (JobID == rest)
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
        if (Girl.has_active_trait("Fake Orgasm Expert"))    g_Game->push_message("She already has \"Fake Orgasm Expert\".", 0);
        else /*                                 */    Girl.m_DayJob = Girl.m_NightJob = JOB_FAKEORGASM;
    }
    else if (JobID == JOB_SO_BISEXUAL)
    {
        if (Girl.has_active_trait("Bisexual"))/*      */    g_Game->push_message("She is already Bisexual.", 0);
        else /*                                 */    Girl.m_DayJob = Girl.m_NightJob = JOB_SO_BISEXUAL;
    }
    else if (JobID == JOB_SO_LESBIAN)
    {
        if (Girl.has_active_trait("Lesbian"))/*       */    g_Game->push_message("She is already a Lesbian.", 0);
        else /*                                 */    Girl.m_DayJob = Girl.m_NightJob = JOB_SO_LESBIAN;
    }
    else if (JobID == JOB_SO_STRAIGHT)
    {
        if (Girl.has_active_trait("Straight"))/*      */    g_Game->push_message("She is already Straight.", 0);
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
        if (Girl.m_DayJob == JOB_FIGHTTRAIN)    Girl.m_DayJob = rest;
        if (Girl.m_NightJob == JOB_FIGHTTRAIN)    Girl.m_NightJob = rest;
    }
    // Special Clinic Jobs
    else if (JobID == JOB_CHAIRMAN)
    {
        if (Girl.m_Building->num_girls_on_job(JOB_CHAIRMAN, Day0Night1)>0)
            g_Game->push_message("There can be only one Chairman!", 0);
        else
            Girl.m_NightJob = Girl.m_DayJob = JOB_CHAIRMAN;
    }

    else if (Girl.has_active_trait("AIDS") && (JobID == JOB_DOCTOR || JobID == JOB_NURSE || JobID == JOB_INTERN))
    {
        g_Game->push_message("Health laws prohibit anyone with AIDS from working in the Medical profession", 0);
        if (Girl.m_DayJob == JOB_INTERN || Girl.m_DayJob == JOB_NURSE || Girl.m_DayJob == JOB_DOCTOR)
            Girl.m_DayJob = rest;
        if (Girl.m_NightJob == JOB_INTERN || Girl.m_NightJob == JOB_NURSE || Girl.m_NightJob == JOB_DOCTOR)
            Girl.m_NightJob = rest;
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
    else if (JobID == JOB_MECHANIC)
    {
        Girl.m_NightJob = Girl.m_DayJob = JOB_MECHANIC;
    }
    else if (JobID == JOB_GETHEALING && Girl.has_active_trait("Construct"))
    {
        if (fulltime)
            Girl.m_DayJob = Girl.m_NightJob = JOB_GETREPAIRS;
        else if (Day0Night1 == SHIFT_DAY)
            Girl.m_DayJob = JOB_GETREPAIRS;
        else
            Girl.m_NightJob = JOB_GETREPAIRS;
        g_Game->push_message(("The Doctor does not work on Constructs so she sends ") + Girl.FullName() + (" to the Mechanic."), 0);
    }
    else if (JobID == JOB_GETREPAIRS && !Girl.has_active_trait("Construct") && !Girl.has_active_trait(
            "Half-Construct"))
    {
        if (fulltime)
            Girl.m_DayJob = Girl.m_NightJob = JOB_GETHEALING;
        else if (Day0Night1 == SHIFT_DAY)
            Girl.m_DayJob = JOB_GETHEALING;
        else
            Girl.m_NightJob = JOB_GETHEALING;
        g_Game->push_message(("The Mechanic only works on Constructs so she sends ") + Girl.FullName() + (" to the Doctor."), 0);
    }
    // `J` condensed clinic surgery jobs into one check
    else if (
        JobID == JOB_CUREDISEASES ||
        JobID == JOB_GETABORT ||
        JobID == JOB_COSMETICSURGERY ||
        JobID == JOB_BREASTREDUCTION ||
        JobID == JOB_BOOBJOB ||
        JobID == JOB_ASSJOB ||
        JobID == JOB_FACELIFT ||
        JobID == JOB_VAGINAREJUV ||
        JobID == JOB_LIPO ||
        JobID == JOB_TUBESTIED ||
        JobID == JOB_FERTILITY)
    {
        bool jobgood = true;
        if (Girl.m_Building->num_girls_on_job(JOB_DOCTOR, Day0Night1) == 0)
        {
            g_Game->push_message(("You must have a Doctor on duty to perform surgery."), 0);
            jobgood = false;
        }
        else if (JobID == JOB_CUREDISEASES && !has_disease(Girl))
        {
            g_Game->push_message(("Oops, the girl does not have any diseases."), 0);
            jobgood = false;
        }
        else if (JobID == JOB_GETABORT && !Girl.is_pregnant())
        {
            g_Game->push_message(("Oops, the girl is not pregant."), 0);
            jobgood = false;
        }
        else if (JobID == JOB_COSMETICSURGERY)
        {
        }
        else if (JobID == JOB_BREASTREDUCTION && Girl.has_active_trait("Flat Chest"))
        {
            g_Game->push_message(("Her boobs can't get no smaller."), 0);
            jobgood = false;
        }
        else if (JobID == JOB_BOOBJOB && Girl.has_active_trait("Titanic Tits"))
        {
            g_Game->push_message(("Her boobs can't get no bigger."), 0);
            jobgood = false;
        }
        else if (JobID == JOB_ASSJOB && Girl.has_active_trait("Great Arse"))
        {
            g_Game->push_message(("Her ass can't get no better."), 0);
            jobgood = false;
        }
        else if (JobID == JOB_FACELIFT && Girl.age() <= 21)
        {
            g_Game->push_message(("She is to young for a face lift."), 0);
            jobgood = false;
        }
        else if (JobID == JOB_VAGINAREJUV && is_virgin(Girl))
        {
            g_Game->push_message(("She is a virgin and has no need of this operation."), 0);
            jobgood = false;
        }
        else if (JobID == JOB_LIPO && Girl.has_active_trait("Great Figure"))
        {
            g_Game->push_message(("She already has a great figure and doesn't need this."), 0);
            jobgood = false;
        }
        else if (JobID == JOB_TUBESTIED)
        {
            if (Girl.is_pregnant())
            {
                g_Game->push_message(Girl.FullName() + (" is pregant.\nShe must either have her baby or get an abortion before She can get her Tubes Tied."), 0);
                jobgood = false;
            }
            else if (Girl.has_active_trait("Sterile"))
            {
                g_Game->push_message(("She is already Sterile and doesn't need this."), 0);
                jobgood = false;
            }
        }
        else if (JobID == JOB_FERTILITY)
        {
            if (Girl.is_pregnant())
            {
                g_Game->push_message(Girl.FullName() +
                                     " is pregant.\nShe must either have her baby or get an abortion before She can get recieve any more fertility treatments.", 0);
                jobgood = false;
            }
            else if (Girl.has_active_trait("Broodmother"))
            {
                g_Game->push_message("She is already as Fertile as she can be and doesn't need any more fertility treatments.", 0);
                jobgood = false;
            }
        }
        Girl.m_DayJob = Girl.m_NightJob = jobgood ? JobID : rest;
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
        else if (is_addict(Girl))
            g_Game->push_message(("She has no addictions."), 0);
        else
            Girl.m_DayJob = Girl.m_NightJob = JOB_REHAB;
    }
    else if (JobID == JOB_ANGER)
    {
        if (Girl.m_Building->num_girls_on_job(JOB_COUNSELOR, Day0Night1) < 1)
            g_Game->push_message(("You must have a counselor for anger management."), 0);
        else if (!Girl.has_active_trait("Aggressive")
            && !Girl.has_active_trait("Tsundere")
            && !Girl.has_active_trait("Yandere"))
            g_Game->push_message(("She has no anger issues."), 0);
        else
            Girl.m_DayJob = Girl.m_NightJob = JOB_ANGER;
    }
    else if (JobID == JOB_EXTHERAPY)
    {
        if (Girl.m_Building->num_girls_on_job(JOB_COUNSELOR, Day0Night1) < 1)
            g_Game->push_message(("You must have a counselor for extreme therapy."), 0);
        else if (!Girl.has_active_trait("Mind Fucked")
            && !Girl.has_active_trait("Broken Will"))
            g_Game->push_message(("She has no extreme issues."), 0);
        else
            Girl.m_DayJob = Girl.m_NightJob = JOB_EXTHERAPY;
    }
    else if (JobID == JOB_THERAPY)
    {
        if (Girl.m_Building->num_girls_on_job(JOB_COUNSELOR, Day0Night1) < 1)
            g_Game->push_message(("You must have a counselor for therapy."), 0);
        else if (!Girl.has_active_trait("Nervous")
            && !Girl.has_active_trait("Dependant")
            && !Girl.has_active_trait("Pessimist"))
            g_Game->push_message(("She has no need of therapy."), 0);
        else
            Girl.m_DayJob = Girl.m_NightJob = JOB_THERAPY;
    }
    // Special Movie Studio Jobs
#if 1
    else if (JobID == JOB_DIRECTOR && Girl.m_Building->num_girls_on_job(JOB_DIRECTOR, SHIFT_NIGHT) >0)
    {
        g_Game->push_message(("There can be only one Director!"), 0);
    }
    else if (JobID == JOB_PROMOTER && Girl.m_Building->num_girls_on_job(JOB_PROMOTER, SHIFT_NIGHT) > 0)
    {
        g_Game->push_message(("There can be only one Promoter."), 0);
    }
    else if (is_Actress_Job(JobID) &&
        (Girl.m_Building->num_girls_on_job(JOB_CAMERAMAGE, SHIFT_NIGHT) < 1 ||
                Girl.m_Building->num_girls_on_job(JOB_CRYSTALPURIFIER, SHIFT_NIGHT) < 1))
    {
        g_Game->push_message("You must have one cameramage and one crystal purifier.", 0);
        Girl.m_DayJob = Girl.m_NightJob = rest;
    }
#endif

// Special cases were checked and don't apply, just set the studio job as requested
#if 1
    else if (Girl.m_Building && Girl.m_Building->type() == BuildingType::STUDIO)
    {
        MadeChanges = false;
        Girl.m_DayJob = rest;
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
            (Day0Night1 ? Girl.m_DayJob = rest : Girl.m_NightJob = rest);

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
        sGang *enemy_gang = g_Game->gang_manager().GetTempWeakGang();
        enemy_gang->give_potions(g_Dice.in_range(1, 10));
        enemy_gang->m_Skills[SKILL_MAGIC] = 0;
        // There is also between 1 and 10 of them, not 10 every time
        enemy_gang->m_Num = std::max(1, g_Dice.bell(-5, 10));

        // Three more lines of defense

        // first subtract 10 security point per gang member that is attacking
        Brothl->m_SecurityLevel = Brothl->m_SecurityLevel - enemy_gang->m_Num * 10;    // `J` moved and doubled m_SecurityLevel loss
        // 1. Brothel security
        if (security_stops_rape(girl, enemy_gang, Day0Night1)) return false;
        // 2. Defending gangs
        if (gang_stops_rape(girl, gangs_guarding, enemy_gang, gang_coverage, Day0Night1)) return false;
        // 3. The attacked girl herself
        if (girl_fights_rape(girl, enemy_gang, Day0Night1)) return false;
        // If all defensive measures fail...
        // subtract 10 security points per gang member left
        Brothl->m_SecurityLevel = Brothl->m_SecurityLevel - enemy_gang->m_Num * 10;    // `J` moved and doubled m_SecurityLevel loss
        customer_rape(girl, enemy_gang->m_Num);
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
    if ((girl.has_active_trait("Yandere") || girl.has_active_trait("Tsundere") || girl.has_active_trait("Aggressive")) && g_Dice.percent(30))
    {
        switch (g_Dice % 5)
            {
            case 0:
                girl.AddMessage(("She beat the customer silly."), IMGTYPE_COMBAT, Day0Night1 ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
                break;
            case 1:
                girl.AddMessage(("The customer's face annoyed her, so she punched it until it went away."), IMGTYPE_COMBAT, Day0Night1 ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
                break;
            case 2:
                girl.AddMessage(("The customer acted like he owned her - so she pwned him."), IMGTYPE_COMBAT, Day0Night1 ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
                break;
            case 3:
                girl.AddMessage(("The customer's attitude was bad. She corrected it."), IMGTYPE_COMBAT, Day0Night1 ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
                break;
            case 4:
                girl.AddMessage(("He tried to insert a bottle into her, so she 'gave it' to him instead."), IMGTYPE_COMBAT, Day0Night1 ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
                break;
            default:
                girl.AddMessage(("Did som(E)thing violent."), IMGTYPE_COMBAT, Day0Night1 ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
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

    if (SecGuard->has_active_trait("Quick Learner")) xp += 2;
    else if (SecGuard->has_active_trait("Slow Learner")) xp -= 2;

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
                    auto custgirl = g_Game->CreateRandomGirl(g_Dice % 40 + 18, true, false, (g_Dice % 3 == 1));    // `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live

                    // `J` and adjust her stats
                    sInventoryItem* item_p = g_Game->inventory_manager().GetItem(item);
                    custgirl->add_item(item_p);
                    custgirl->equip(item_p, true);
                    g_Game->player().inventory().remove_item(item_p);
                    custgirl->add_temporary_trait("Emprisoned Customer", std::max(5, g_Dice.bell(0, 20)));
                    custgirl->pclove(-(g_Dice % 50 + 50));
                    custgirl->pcfear(g_Dice % 50 + 50);
                    custgirl->pchate(g_Dice % 50 + 50);
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
                    custgirl->m_Events.AddMessage(CGmsg.str(), IMGTYPE_DEATH, EVENT_WARNING);
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
        SecGuard->m_Events.AddMessage(SGmsg.str(), IMGTYPE_COMBAT, EVENT_WARNING);
        girl.AddMessage(Gmsg.str(), IMGTYPE_COMBAT, EVENT_WARNING);
        return true;
    }
    else if (result == ECombatResult::DRAW) {
        std::stringstream ss;
        ss << "Security Problem:\n" << "Trying to defend " << girl.FullName() << ". You defeated "
           << num << " of " << OrgNumMem << ". By now the guarding gangs have arrived, and will deal with the offenders.";
        SecGuard->m_Events.AddMessage(ss.str(), IMGTYPE_DEATH, EVENT_DANGER);
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

        SecGuard->m_Events.AddMessage(ss.str(), IMGTYPE_DEATH, EVENT_DANGER);

        // Bad stuff
        SecGuard->happiness(-40);
        SecGuard->confidence(-40);
        SecGuard->obedience(-10);
        SecGuard->spirit(-40);
        SecGuard->libido(-4);
        SecGuard->upd_temp_stat(STAT_LIBIDO, -40, true);
        SecGuard->tiredness(60);
        SecGuard->pcfear(20);
        SecGuard->pclove(-20);
        SecGuard->pchate(20);
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
        guarding_gang->m_Events.AddMessage(gang_s.str(), IMGTYPE_PROFILE, EVENT_WARNING);
        girl.AddMessage(girl_s.str(), IMGTYPE_DEATH, EVENT_WARNING);
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

    girl.AddMessage(girl_ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
    guarding_gang->m_Events.AddMessage(gang_ss.str(), IMGTYPE_PROFILE, EVENT_GANG);

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
    girl.m_Events.AddMessage(combat.round_summaries().back(), EVENT_DANGER, report);

    // Earn xp for all kills, even if defeated
    int xp = 3;
    if (girl.has_active_trait("Quick Learner")) xp += 2;
    else if (girl.has_active_trait("Slow Learner")) xp -= 2;

    int num = OrgNumMem - enemy_gang->m_Num;

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
        girl.AddMessage(msg.str(), IMGTYPE_COMBAT, EVENT_WARNING);
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

    girl.m_Events.AddMessage(ss.str(), IMGTYPE_DEATH, EVENT_DANGER);

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
    girl.pclove(-20);
    girl.pchate(20);
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

    if (Cust.has_active_trait("AIDS"))            a = true;
    if (Cust.has_active_trait("Chlamydia"))    c = true;
    if (Cust.has_active_trait("Syphilis"))        s = true;
    if (Cust.has_active_trait("Herpes"))        h = true;
    std = a || c || s || h;
    if (!std && g_Dice.percent(5))
    {
        std = true;
        /* */if (g_Dice.percent(20))    a = true;
        else if (g_Dice.percent(20))    c = true;
        else if (g_Dice.percent(20))    s = true;
        else /*                   */    h = true;
    }

    if (preg || std)
    {
        ss.str("");
        ss << girl.FullName() << "'s rapist";
        if (numberofattackers > 1) ss << "s left their";
        else ss << " left " << (Cust.m_IsWoman ? "her" : "his");
        ss << " calling card behind, ";
        if (preg)            { ss << "a baby in her belly"; }
        if (preg && std)    { ss << " and "; }
        else if (preg)        { ss << ".\n \n"; }
        if (a || c || s || h)    { bool _and = false;
            if (a)    { girl.gain_trait("AIDS");        ss << "AIDS"; }
            if (a && (c || s || h))                            {    ss << " and ";        _and = true; }
            if (c)    { girl.gain_trait("Chlamydia");    ss << "Chlamydia";    _and = false; }
            if (!_and && (a || c) && (s || h))                {    ss << " and ";        _and = true; }
            if (s)    { girl.gain_trait("Syphilis");    ss << "Syphilis";    _and = false; }
            if (!_and && (a || c || s) && h)                {    ss << " and "; }
            if (h)    { girl.gain_trait("Herpes");        ss << "Herpes"; }
            ss << ".\n \n";
        }

        girl.AddMessage(ss.str(), IMGTYPE_DEATH, EVENT_DANGER);
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

// ------ Training

/*
 * let's look at this a little differently...
 */
void cJobManager::get_training_set(std::vector<sGirl*> &v, std::vector<sGirl*> &t_set)
{
    int max = 4;
    // empty out the trainable set
    t_set.clear();
    if(v.empty())
        return;                 // if there's nothing in the vector, return with t_set empty
    int v_siz = v.size();

    if (v_siz == 1)            // if there's just one, return with t_set containing that
    {
        t_set.push_back(v.back());
        v.pop_back();
        return;
    }
/*
 *    we want to split the girls into groups of four
 *    but if possible we don't want any singletons
 *
 *    So... if there would be one left over,
 *    we make the first group a group of three.
 *    subsequent groups will have modulus 2 and thus
 *    generate quads, until the end when we have a pair left.
 *
 *    Easier to explain in code than words, really.
 */
     if((v_siz % 4) == 1)
        max --;        // reduce it by one, this time only
/*
 *    get the limiting variable for the loop
 *    we don't compare against v.size() because
 *    that will change as the loop progresses
 */
    int lim = (max < v_siz ? max : v_siz);
/*
 *    now loop until we hit max, or run out of girls
 */
     for(int i = 0; i < lim; i++)
    {
        t_set.push_back(v.back());
        v.pop_back();
    }
}

bool WorkTraining(sGirl& girl, bool Day0Night1, cRng& rng)
{
    return false;
}
double JP_Training(const sGirl& girl, bool estimate)
{
    double jobperformance = 0.0;

    jobperformance +=
        (100 - cGirls::GetAverageOfSexSkills(girl)) +
        (100 - cGirls::GetAverageOfNSxSkills(girl))
        ;

    return jobperformance;
}

void cJobManager::do_solo_training(sGirl& girl, bool Day0Night1)
{
    TrainableGirl trainee(&girl);
    girl.AddMessage(("She trained during this shift by herself, so learning anything worthwhile was difficult."), IMGTYPE_PROFILE, Day0Night1 ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
/*
 *    50% chance nothing happens
 */
     if(g_Dice.percent(50))
    {
        girl.AddMessage(("Sadly, she didn't manage to improve herself in any way."), IMGTYPE_PROFILE, EVENT_SUMMARY);
        return;
    }
/*
 *    otherwise, pick a random attribute and raise it 1-3 points
 */
    int amt = 1 + g_Dice%3;
    std::string improved = trainee.update_random(amt);
    std::stringstream ss;
    ss.str("");
    ss << ("She managed to gain ") << amt << " " << improved << (".");
    girl.m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_SUMMARY);
}

void cJobManager::do_training_set(std::vector<sGirl*> girls, bool Day0Night1)
{
    std::stringstream ss;
    /*
    *    we're getting a vector of 1-4 girls here
    *    (the one is possible if only one girl trains)
    */
    if (girls.empty())
    {  // no girls? that shouldn't happen
        g_LogFile.log(ELogLevel::ERROR, "Logic Error in cJobManager::do_training_set: empty set passed for training!");
        return;
    }
    else if (girls.size() == 1)
    {  // special case for only one girl
        do_solo_training(*girls[0], Day0Night1);
        return;
    }
    /*
    *    OK. Now, as I was saying. We have an array of sGirl* pointers..
    *    We need that to be a list of TrainableGirl objects:
    */
    std::vector<TrainableGirl> set;
    /*
    *    4 is the maximum set size. I should probably consider
    *    making that a class constant - or a static class member
    *    initialised from the config file. Later for that.
    */
    for (auto& girl : girls)
    {
        if (girl == nullptr) break;
        set.emplace_back(girl);
    }
    /*
    *    now get an idealized composite of the girls in the set
    *
    *    and the get a vector of the indices of the most efficient
    *    three attributes for them to train
    */
    IdealGirl ideal(set);
    std::vector<int> indices = ideal.training_indices();
    /*
    *    OK. Loop over the girls, and then over the indices
    */
    for (int g_idx = 0; g_idx < set.size(); g_idx++)
    {
        TrainableGirl &trainee = set[g_idx];

        for (int i_idx = 0; i_idx < indices.size(); i_idx++)
        {
            int index = indices[i_idx];
            /*
            *            the base value for any increase is the difference
            *            between the value of girl's stat, and the value of best
            *            girl in the set
            */
            int aaa = ideal[index].value(); //    [index].value();
            int bbb = trainee[index].value();
            int diff = aaa - bbb;
            if (diff < 0)  // don't want a negative training value
                diff = 0;
            /*
            *            plus one - just so the teacher gets a chance to
            *            learn something
            */
            diff++;
            /*
            *            divide by 10 for the increase
            */
            int inc = (int)floor(1.0 * (double)diff / 10.0);
            /*
            *            if there's any left over (or if the diff
            *            was < 10 to begin with, there's a 10%
            *            point for each difference point
            */
            int mod = diff % 10;
            if (g_Dice.percent(mod * 10)) inc++;
            trainee[index].upd(inc);
        }

        sGirl *girl = trainee.girl();
        /*
        *        need to do the  "she trained hard with ..." stuff here
        */
        //int n_girls = set.size();
        ss.str("");
        ss << ("She trained during this shift in the following grouping: ");
        for (int i = 0; i < girls.size(); i++)
        {
            ss << girls[i]->FullName();
            if (i == 0 && girls.size() == 2)    ss << (" and ");
            else if (i < girls.size() - 2)        ss << (", ");
            else if (i == girls.size() - 2)    ss << (", and ");
        }
        ss << (".");

        girl->AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1 ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
        ss.str("");
        ss << ("She trained during this shift");

        bool raised = false;
        for (int i_idx = 0; i_idx < indices.size(); i_idx++)
        {
            int index = indices[i_idx];
            int gain = trainee[index].gain();
            if (gain == 0) continue;
            if (!raised)
            {
                raised = true;
                ss << (" and gained ");
            }
            else ss << (", ");

            ss << ("+") << gain << " " << trainee[index].name();
        }
        ss << (".");
        girl->AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_SUMMARY);
        if (girl->has_active_trait("Lesbian") && set.size() > 1) girl->upd_temp_stat(STAT_LIBIDO, set.size() - 1, true);
    }
}

void cJobManager::do_training(sBrothel* brothel, bool Day0Night1)
{
    std::vector<sGirl*> t_set;
    std::vector<sGirl*> girls = brothel->girls_on_job(JOB_TRAINING, Day0Night1);

    for (int i = girls.size(); i-- > 0;)
    {  // no girls sneaking in training if she gave birth
        if ((girls[i]->m_WeeksPreg > 0 &&
            girls[i]->m_WeeksPreg + 1 >= girls[i]->get_preg_duration())
            || (girls[i]->m_JustGaveBirth && Day0Night1 == SHIFT_NIGHT))
            girls.erase(girls.begin() + i);
    }
    random_shuffle(girls.begin(), girls.end());
    for (;;)
    {
        get_training_set(girls, t_set);
        if (t_set.empty()) break;
        do_training_set(t_set, Day0Night1);
    }
    /*
    *    a few bookkeeping details here
    *    dirt and training costs, for a start
    */
    brothel->m_Filthiness += girls.size();
    /*
    *    and then each girl gets to feel tired and horny
    *    as a result of training
    */
    for (auto& girl : girls)
    {
        int libido = (girl->has_active_trait("Nymphomaniac")) ? 4 : 2;
        girl->upd_temp_stat(STAT_LIBIDO, libido);
    }
}

// ----- Film & related

std::string cJobManager::GirlPaymentText(IBuilding * brothel, sGirl& girl, int totalTips, int totalPay, int totalGold,
                                    bool Day0Night1)
{
    std::stringstream ss;
    std::string girlName = girl.FullName();
    auto sw = girl.get_job(Day0Night1);

    // `J` if a slave does a job that is normally paid by you but you don't pay your slaves...
    if (girl.is_unpaid() && is_job_Paid_Player(sw))
    {
        ss << "\nYou own her and you don't pay your slaves.";
    }
    else if (totalGold > 0)
    {
        ss << girlName << " earned a total of " << totalGold << " gold";

        // if it is a player paid job and she is not a slave or she is a slave but you pay slaves out of pocket.
        if (is_job_Paid_Player(sw) && !girl.is_unpaid())
        {
            ss << " directly from you.\nShe gets to keep it all.";
        }
        else if (girl.house() <= 0)
        {
            ss << " and she gets to keep it all.";
        }
        else if (totalTips > 0 && girl.keep_tips())                                        // if there are tips
        {
            int hpay = int(double(totalPay * double(girl.house() * 0.01)));
            int gpay = totalPay - hpay;
            ss << ".\nShe keeps the " << totalTips << " she got in tips and her cut ("
                << 100 - girl.house() << "%) of the payment amounting to " << gpay
                << " gold.\n \nYou got " << hpay << " gold (" << girl.house() << "%).";
        }
        else
        {
            int hpay = int(double(totalGold * double(girl.house() * 0.01)));
            int gpay = totalGold - hpay;
            ss << ".\nShe keeps " << gpay << " gold. (" << 100 - girl.house()
                << "%)\nYou keep " << hpay << " gold (" << girl.house() << "%).";
        }
    }
    else if (totalGold == 0)    { ss << girlName << " made no money."; }
    else if (totalGold < 0)        {
        ss << "ERROR: She has a loss of " << totalGold << " gold\n \nPlease report this to the Pink Petal Devloment Team at http://pinkpetal.org\n \nGirl Name: " << girl.FullName() << "\nJob: " << get_job_name(Day0Night1 ? girl.m_NightJob : girl.m_DayJob) << "\nPay:     " << girl.m_Pay << "\nTips:   " << girl.m_Tips << "\nTotal: " << totalGold;
    }
    return ss.str();
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
    bool refused = do_job(girl, is_night);

    int totalPay  = girl.m_Pay;
    int totalTips = girl.m_Tips;
    brothel->CalculatePay(girl, sw);

    //        Summary Messages
    if (refused)
    {
        brothel->m_Fame -= girl.fame();
        girl.AddMessage("${name} refused to work so made no money.", IMGTYPE_PROFILE, EVENT_SUMMARY);
    }
    else
    {
        brothel->m_Fame += girl.fame();
        girl.AddMessage(GirlPaymentText(brothel, girl, totalTips, totalPay, totalTips + totalPay, is_night),
                IMGTYPE_PROFILE, EVENT_SUMMARY);
    }
}

bool cJobManager::do_job(sGirl& girl, bool is_night)
{
    return do_job(girl.get_job(is_night), girl, is_night);
}

bool cJobManager::do_job(JOBS job_id, sGirl& girl, bool is_night)
{
    auto refused = m_OOPJobs[job_id]->Work(girl, is_night, g_Dice);
    if(is_night) {
        girl.m_Refused_To_Work_Night = refused;
    } else {
        girl.m_Refused_To_Work_Day = refused;
    }
    return refused;
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
    sInventoryItem* item = nullptr;
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
        auto custgirl = g_Game->CreateRandomGirl(18 + (std::max(0, g_Dice % 40 - 10)), true, false,
                (g_Dice % 3 == 1));    // `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live
        int emprisontraittime = 1;
        custgirl->pclove(-(g_Dice % 50 + 50));
        custgirl->pcfear(g_Dice % 50 + 50);
        custgirl->pchate(g_Dice % 50 + 50);
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
            girl.pchate(g_Dice % 10);
            girl.pcfear(g_Dice % 10);
            girl.pclove(-(g_Dice % 10));
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
        CGmsg << custgirl->FullName() << " was caught trying to run out without paying for services provided by "
              << girl.FullName() << ".\n \n" << itemtext.str();
        custgirl->add_temporary_trait("Emprisoned Customer", emprisontraittime);    // add temp trait
        if (g_Dice.percent(75)) custgirl->lose_trait("Virgin");                // most of the time she will not be a virgin
        g_Game->player().suspicion(g_Dice % 10);
        g_Game->player().disposition(-(g_Dice % 10));
        g_Game->player().customerfear(g_Dice % 10);
        custgirl->m_Money = 0;

        custgirl->m_Events.AddMessage(CGmsg.str(), IMGTYPE_DEATH, EVENT_WARNING);
        // `J` add the customer to the dungeon
        g_Game->dungeon().AddGirl(custgirl, DUNGEON_CUSTNOPAY);
    } else {
        g_Game->PushEvent(EDefaultEvent::CUSTOMER_NOPAY);    // if not taken as a slave, use the old code
    }
}

void cJobManager::register_job(std::unique_ptr<IGenericJob> job) {
    m_OOPJobs[job->job()] = std::move(job);
}

const IGenericJob* cJobManager::get_job(JOBS job) const {
    return m_OOPJobs.at(job).get();
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


bool cJobManager::job_filter(int Filter, JOBS job) const {
    auto& filter = JobFilters[Filter];
    return std::count(begin(filter.Contents), end(filter.Contents), job) > 0;
}
