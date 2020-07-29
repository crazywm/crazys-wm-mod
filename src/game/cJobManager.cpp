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
#include "buildings/queries.hpp"
#include "character/cCustomers.h"
#include "cRng.h"
#include "cInventory.h"
#include "sConfig.h"
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
#include "Inventory.hpp"
#include "cRival.h"
#include "buildings/cDungeon.h"
#include "character/predicates.h"
#include "character/cPlayer.h"
#include "buildings/cBuildingManager.h"
#include "cGirlGangFight.h"

extern cRng g_Dice;
extern cConfig cfg;

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

    JobData[JOB_RESTING] = sJobData("Free Time", "TOff");
    JobData[JOB_RESTING].description = "She will take some time off, maybe do some shopping or walk around town. If the girl is unhappy she may try to escape.";
    JobData[JOB_TRAINING] = sJobData("Practice Skills", "Prtc");
    JobData[JOB_TRAINING].description = "She will train either alone or with others to improve her skills.";
    JobData[JOB_CLEANING] = sJobData("Cleaning", "Cln");
    JobData[JOB_CLEANING].description = "She will clean the building, as filth will put off some customers.";
    JobData[JOB_SECURITY] = sJobData("Security", "Sec");
    JobData[JOB_SECURITY].description = "She will patrol the building, stopping mis-deeds.";
    JobData[JOB_ADVERTISING] = sJobData("Advertising", "Adv");
    JobData[JOB_ADVERTISING].description = "She will advertise the building's features in the city.";
    JobData[JOB_CUSTOMERSERVICE] = sJobData("Customer Service", "CS");
    JobData[JOB_CUSTOMERSERVICE].description = "She will look after customer needs.";
    JobData[JOB_MATRON] = sJobData("Matron", "Mtrn");
    JobData[JOB_MATRON].description = "This girl will look after the other girls. Only non-slave girls can have this position and you must pay them 300 gold per week. Also, it takes up both shifts. (max 1)";
    JobData[JOB_TORTURER] = sJobData("Torturer", "Trtr");
    JobData[JOB_TORTURER].description = "She will torture the prisoners in addition to your tortures, she will also look after them to ensure they don't die. (max 1 for all brothels)";
    JobData[JOB_EXPLORECATACOMBS] = sJobData("Explore Catacombs", "ExC");
    JobData[JOB_EXPLORECATACOMBS].description = "She will explore the catacombs looking for treasure and capturing monsters and monster girls. Needless to say, this is a dangerous job.";
    JobData[JOB_BEASTCARER] = sJobData("Beast Carer", "BstC");
    JobData[JOB_BEASTCARER].description = "She will look after the needs of the beasts in your Brothel.";
    // - Bar Jobs
    JobFilters[JOBFILTER_BAR] = sJobFilter{"Bar", "These are jobs for running a bar."};
    register_filter(JOBFILTER_BAR, JOB_BARMAID, JOB_BARCOOK);
    JobData[JOB_BARMAID] = sJobData("Barmaid", "BrMd");
    JobData[JOB_BARMAID].description = "She will staff the bar and serve drinks.";
    JobData[JOB_WAITRESS] = sJobData("Waitress", "Wtrs");
    JobData[JOB_WAITRESS].description = "She will bring drinks and food to customers at the tables.";
    JobData[JOB_SINGER] = sJobData("Singer", "Sngr");
    JobData[JOB_SINGER].description = "She will sing for the customers.";
    JobData[JOB_PIANO] = sJobData("Piano", "Pno");
    JobData[JOB_PIANO].description = "She will play the piano for the customers.";
    JobData[JOB_ESCORT] = sJobData("Escort", "Scrt");
    JobData[JOB_ESCORT].description = "She will be an escort.";
    JobData[JOB_BARCOOK] = sJobData("Bar Cook", "BrCk");
    JobData[JOB_BARCOOK].description = "She will cook food for the bar.";
    // - Gambling Hall Jobs
    JobFilters[JOBFILTER_GAMBHALL] = sJobFilter{"Gambling Hall", "These are jobs for running a gambling hall."};
    register_filter(JOBFILTER_GAMBHALL, JOB_DEALER, JOB_WHOREGAMBHALL);
    JobData[JOB_DEALER] = sJobData("Game Dealer", "Dlr");
    JobData[JOB_DEALER].description = "She will manage a game in the gambling hall.";
    JobData[JOB_ENTERTAINMENT] = sJobData("Entertainer", "Entn");
    JobData[JOB_ENTERTAINMENT].description = "She will provide entertainment to the customers.";
    JobData[JOB_XXXENTERTAINMENT] = sJobData("XXX Entertainer", "XXXE");
    JobData[JOB_XXXENTERTAINMENT].description = "She will provide sexual entertainment to the customers.";
    JobData[JOB_WHOREGAMBHALL] = sJobData("Hall Whore", "HWhr");
    JobData[JOB_WHOREGAMBHALL].description = "She will give her sexual favors to the customers.";
    // - Sleazy Bar Jobs        // Changing all references to Strip Club, just sounds better to me and more realistic. -PP
    JobFilters[JOBFILTER_SLEAZYBAR] = sJobFilter{"Strip Club", "These are jobs for running a Strip Club."};
    register_filter(JOBFILTER_SLEAZYBAR, JOB_SLEAZYBARMAID, JOB_BARWHORE);
    JobData[JOB_SLEAZYBARMAID] = sJobData("Strip Club Barmaid", "SBmd");
    JobData[JOB_SLEAZYBARMAID].description = "She will staff the bar and serve drinks while dressed in lingerie or fetish costumes.";
    JobData[JOB_SLEAZYWAITRESS] = sJobData("Strip Club Waitress", "SWtr");
    JobData[JOB_SLEAZYWAITRESS].description = "She will bring drinks and food to the customers at the tables while dressed in lingerie or fetish costumes.";
    JobData[JOB_BARSTRIPPER] = sJobData("Strip Club Stripper", "SSrp");
    JobData[JOB_BARSTRIPPER].description = "She will strip on the tables and stage for the customers.";
    JobData[JOB_BARWHORE] = sJobData("Strip Club Whore", "SWhr");
    JobData[JOB_BARWHORE].description = "She will provide sex to the customers.";
    // - Brothel Jobs
    JobFilters[JOBFILTER_BROTHEL] = sJobFilter{"Brothel", "These are jobs for running a brothel."};
    register_filter(JOBFILTER_BROTHEL, JOB_MASSEUSE, JOB_WHORESTREETS);
    JobData[JOB_MASSEUSE] = sJobData("Masseuse", "Msus");
    JobData[JOB_MASSEUSE].description = "She will give massages to the customers.";
    JobData[JOB_BROTHELSTRIPPER] = sJobData("Brothel Stripper", "BStp");
    JobData[JOB_BROTHELSTRIPPER].description = "She will strip for the customers.";
    JobData[JOB_PEEP] = sJobData("Peep Show", "Peep");
    JobData[JOB_PEEP].description = "She will let people watch her change and maybe more...";
    JobData[JOB_WHOREBROTHEL] = sJobData("Brothel Whore", "BWhr");
    JobData[JOB_WHOREBROTHEL].description = "She will whore herself to customers within the building's walls. This is safer but a little less profitable.";
    JobData[JOB_WHORESTREETS] = sJobData("Whore on Streets", "StWr");
    JobData[JOB_WHORESTREETS].description = "She will whore herself on the streets. It is more dangerous than whoring inside but more profitable.";





    // Studio Crew
    JobFilters[JOBFILTER_STUDIOCREW] = sJobFilter{"Studio Crew", "These are jobs for running a movie studio."};
    register_filter(JOBFILTER_STUDIOCREW, JOB_FILMFREETIME, JOB_STAGEHAND);
    JobData[JOB_FILMFREETIME] = sJobData("Time off", "TOff");
    JobData[JOB_FILMFREETIME].description = "She takes time off resting and recovering.";
    JobData[JOB_DIRECTOR] = sJobData("Director", "Dir");
    JobData[JOB_DIRECTOR].description = "She directs the filming, and keeps the girls in line. (max 1)";
    JobData[JOB_PROMOTER] = sJobData("Promoter", "Prmt");
    JobData[JOB_PROMOTER].description = "She advertises the movies. (max 1)";
    JobData[JOB_CAMERAMAGE] = sJobData("Camera Mage", "CM");
    JobData[JOB_CAMERAMAGE].description = "She will film the scenes. (requires at least 1 to create a scene)";
    JobData[JOB_CRYSTALPURIFIER] = sJobData("Crystal Purifier", "CP");
    JobData[JOB_CRYSTALPURIFIER].description = "She will clean up the filmed scenes. (requires at least 1 to create a scene)";
    //JobData[JOB_SOUNDTRACK] = sJobData("Sound Track", "SndT", WorkSoundTrack, JP_SoundTrack);
    //JobData[JOB_SOUNDTRACK].description = ("She will clean up the audio and add music to the scenes. (not required but helpful)");
    JobData[JOB_FLUFFER] = sJobData("Fluffer", "Fluf");
    JobData[JOB_FLUFFER].description = "She will keep the porn stars aroused.";
    JobData[JOB_STAGEHAND] = sJobData("Stagehand", "StgH");
    JobData[JOB_STAGEHAND].description = "She helps setup equipment, and keeps the studio clean.";


    // `J` When adding new Studio Scenes, search for "J-Add-New-Scenes"  :  found in >> cJobManager.cpp > Setup

    // Studio - Non-Sex Scenes
    JobFilters[JOBFILTER_STUDIONONSEX] = sJobFilter{"Non-Sex Scenes", "These are scenes without sex."};
    register_filter(JOBFILTER_STUDIONONSEX, JOB_FILMACTION, JOB_FILMMUSIC);
    JobData[JOB_FILMACTION] = sJobData("Action", "FAct");
    JobData[JOB_FILMACTION].description = "She will be the scantily clad heroine in fighting/magical action scenes.";
    JobData[JOB_FILMCHEF] = sJobData("The Naked Chef", "FChf");
    JobData[JOB_FILMCHEF].description = "She will be filmed preparing food. And talking slowly. Sensuously. In sexy outfits. Because no reason.";
    //JobData[JOB_FILMCOMEDY] = sJobData("Comedy Scene", "FCmd", WorkFilmComedy, JP_FilmComedy);
    //JobData[JOB_FILMCOMEDY].description = ("She will perform a scene intended to make people laugh.");
    //JobData[JOB_FILMDRAMA] = sJobData("Drama Scene", "FDrm", WorkFilmDrama, JP_FilmDrama);
    //JobData[JOB_FILMDRAMA].description = ("She will film a Dramatic scene.");
    //JobData[JOB_FILMHORROR] = sJobData("Horror Scene", "FHrr", WorkFilmHorror, JP_FilmHorror);
    //JobData[JOB_FILMHORROR].description = ("She will film a Horror scene.");
    //JobData[JOB_FILMIDOL] = sJobData("Idol (Aidoru)", "FIdl", WorkFilmIdol, JP_FilmIdol);
    //JobData[JOB_FILMIDOL].description = ("She will be presented to the public as an idol: a cute, sexy woman.");
    JobData[JOB_FILMMUSIC] = sJobData("Music", "FMus");
    JobData[JOB_FILMMUSIC].description = "She will make a music video.";

    // Studio - Softcore Porn
    JobFilters[JOBFILTER_STUDIOSOFTCORE] = sJobFilter{"Softcore Scenes", "These are scenes with just the one girl."};
    register_filter(JOBFILTER_STUDIOSOFTCORE, JOB_FILMMAST, JOB_FILMTEASE);
    JobData[JOB_FILMMAST] = sJobData("Film Masturbation", "FMst");
    JobData[JOB_FILMMAST].description = "She will perform in a masturbation scene.";
    JobData[JOB_FILMSTRIP] = sJobData("Film Strip tease", "FStp");
    JobData[JOB_FILMSTRIP].description = "She will perform in a strip tease scene.";
    JobData[JOB_FILMTEASE] = sJobData("Teaser Video", "FTea");
    JobData[JOB_FILMTEASE].description = "She will flirt and remove her clothes and tease seductively, but won't show all. If you freeze-frame you might just catch a nipple.";

    // Studio - Porn
    JobFilters[JOBFILTER_STUDIOPORN] = sJobFilter{"Porn Scenes", "These are regular sex scenes."};
    register_filter(JOBFILTER_STUDIOPORN, JOB_FILMANAL, JOB_FILMTITTY);
    JobData[JOB_FILMANAL] = sJobData("Film Anal", "FAnl");
    JobData[JOB_FILMANAL].description = "She will film an anal scenes.";
    JobData[JOB_FILMFOOTJOB] = sJobData("Film Foot Job", "FFJ");
    JobData[JOB_FILMFOOTJOB].description = "She will perform in a foot job scene.";
//    JobData[JOB_FILMFUTA] = sJobData("Film Futa Scene", "FFta", WorkFilmFuta, JP_FilmFuta);
    //    JobData[JOB_FILMFUTA].description = ("She will perform in a Futa scene.");
    JobData[JOB_FILMHANDJOB] = sJobData("Film Hand Job", "FHJ");
    JobData[JOB_FILMHANDJOB].description = "She will perform in a hand job scene.";
    JobData[JOB_FILMLESBIAN] = sJobData("Film Lesbian", "FLes");
    JobData[JOB_FILMLESBIAN].description = "She will do a lesbian scene.";
    JobData[JOB_FILMORAL] = sJobData("Film Oral Sex", "FOrl");
    JobData[JOB_FILMORAL].description = ("She will perform in an oral sex scene.");
    JobData[JOB_FILMSEX] = sJobData("Film Sex", "FSex");
    JobData[JOB_FILMSEX].description = "She will film a normal sex scenes.";
    JobData[JOB_FILMTITTY] = sJobData("Film Titty Fuck", "FTit");
    JobData[JOB_FILMTITTY].description = "She will perform in a titty fuck scene.";

    // Studio - Hardcore porn
    JobFilters[JOBFILTER_STUDIOHARDCORE] = sJobFilter{"Hardcore Scenes", "These are rough scenes that not all girls would be comfortable with."};
    register_filter(JOBFILTER_STUDIOHARDCORE, JOB_FILMBEAST, JOB_FILMPUBLICBDSM);
    JobData[JOB_FILMBEAST] = sJobData("Film Bestiality", "FBst");
    JobData[JOB_FILMBEAST].description = ("She will film a bestiality scenes.");
    JobData[JOB_FILMBONDAGE] = sJobData("Film Bondage", "FBnd");
    JobData[JOB_FILMBONDAGE].description = ("She will perform a in bondage scenes.");
    JobData[JOB_FILMBUKKAKE] = sJobData("Cumslut/Bukkake", "FBuk");
    JobData[JOB_FILMBUKKAKE].description = ("A large group of men will cum on, over and inside her.");
    JobData[JOB_FILMFACEFUCK] = sJobData("Face-fuck", "FFac");
    JobData[JOB_FILMFACEFUCK].description = ("She will be used in gag and face-fuck scenes.");
    JobData[JOB_FILMGROUP] = sJobData("Film Group", "FGrp");
    JobData[JOB_FILMGROUP].description = ("She will film a group sex scene.");
    JobData[JOB_FILMPUBLICBDSM] = sJobData("Public Torture", "FTor");
    JobData[JOB_FILMPUBLICBDSM].description = ("She will be tied up in a public place, where she will be stripped, whipped, tortured and fully-used. On film.");

    //JobData[JOB_FILMDOM].name = ("Film Dominatrix");
    //JobData[JOB_FILMDOM].brief = "FDom";                                            //update in cGirls.cpp
    //JobData[JOB_FILMDOM].description = ("She will perform in a Dominatrix scene.");
    //JobFunc[JOB_FILMDOM] = &WorkFilmDominatrix;
    //JobPerf[JOB_FILMDOM] = &JP_FilmDom;

    // Studio - Random
    JobFilters[JOBFILTER_RANDSTUDIO] = sJobFilter{"Random Scenes", "Let the game choose what scene the girl will film."};
    register_filter(JOBFILTER_RANDSTUDIO, JOB_FILMRANDOM, JOB_FILMRANDOM);
    JobData[JOB_FILMRANDOM] = sJobData("Film a random scene", "FRnd");
    // This job is handled different than others, it is in cMovieStudio.cpp UpdateGirls()
    JobData[JOB_FILMRANDOM].description = "She will perform in a random sex scene.";


    // - Arena Jobs
    JobFilters[JOBFILTER_ARENA] = sJobFilter{"Arena", "These are jobs for running an arena."};
    register_filter(JOBFILTER_ARENA, JOB_FIGHTBEASTS, JOB_FIGHTTRAIN);
    JobData[JOB_FIGHTBEASTS] = sJobData("Fight Beasts", "FiBs");
    JobData[JOB_FIGHTBEASTS].description = ("She will fight to the death against beasts you own. Dangerous.");
    JobData[JOB_FIGHTARENAGIRLS] = sJobData("Cage Match", "Cage");
    JobData[JOB_FIGHTARENAGIRLS].description = ("She will fight against other girls. Dangerous.");
    JobData[JOB_FIGHTTRAIN] = sJobData("Combat Training", "CT");
    JobData[JOB_FIGHTTRAIN].description = ("She will practice combat.");
    /*JobData[JOB_JOUSTING] = sJobData("Jousting", "JstM", WorkArenaJousting, JP_ArenaJousting);
    JobData[JOB_JOUSTING].description = ("She will put on a Jousting match.");*/
    //- Arena Staff
    JobFilters[JOBFILTER_ARENASTAFF] = sJobFilter{"Arena Staff", "These are jobs that help run an arena."};
    register_filter(JOBFILTER_ARENASTAFF, JOB_ARENAREST, JOB_CLEANARENA);
    JobData[JOB_ARENAREST] = sJobData("Time off", "TOff");
    JobData[JOB_ARENAREST].description = ("She will rest.");
    JobData[JOB_DOCTORE] = sJobData("Doctore", "Dtre");
    JobData[JOB_DOCTORE].description = ("She will watch over the girls in the arena.");
    JobData[JOB_CITYGUARD] = sJobData("City Guard", "CGrd");
    JobData[JOB_CITYGUARD].description = ("She will help keep crossgate safe.");
    JobData[JOB_BLACKSMITH] = sJobData("Blacksmith", "Bksm");
    JobData[JOB_BLACKSMITH].description = ("She will make weapons and armor.");
    JobData[JOB_COBBLER] = sJobData("Cobbler", "Cblr");
    JobData[JOB_COBBLER].description = ("She will make shoes and leather items.");
    JobData[JOB_JEWELER] = sJobData("Jeweler", "Jwlr");
    JobData[JOB_JEWELER].description = ("She will make jewelery.");
    JobData[JOB_CLEANARENA] = sJobData("Grounds Keeper", "GKpr");
    JobData[JOB_CLEANARENA].description = ("She will clean the arena.");

    // - Community Centre Jobs
    JobFilters[JOBFILTER_COMMUNITYCENTRE] = sJobFilter{"Community Centre", "These are jobs for running a community centre."};
    register_filter(JOBFILTER_COMMUNITYCENTRE, JOB_CENTREREST, JOB_CLEANCENTRE);
    JobData[JOB_CENTREREST] = sJobData("Time off", "TOff");
    JobData[JOB_CENTREREST].description = ("She will rest.");
    JobData[JOB_CENTREMANAGER] = sJobData("Centre Manager", "CMgr");
    JobData[JOB_CENTREMANAGER].description = ("She will look after the girls working in the centre.");
    JobData[JOB_FEEDPOOR] = sJobData("Feed Poor", "Feed");
    JobData[JOB_FEEDPOOR].description = ("She will work in a soup kitchen.");
    JobData[JOB_COMUNITYSERVICE] = sJobData("Community Service", "CmSr");
    JobData[JOB_COMUNITYSERVICE].description = ("She will go around town and help out where she can.");
    JobData[JOB_CLEANCENTRE] = sJobData("Clean Centre", "ClnC");
    JobData[JOB_CLEANCENTRE].description = ("She will clean the centre.");
    // - Counseling Centre Jobs
    JobFilters[JOBFILTER_COUNSELINGCENTRE] = sJobFilter{"Counseling Centre", "These are jobs for running a counseling centre."};
    register_filter(JOBFILTER_COUNSELINGCENTRE, JOB_COUNSELOR, JOB_ANGER);
    JobData[JOB_COUNSELOR] = sJobData("Counselor", "Cnsl");
    JobData[JOB_COUNSELOR].description = ("She will help girls get over their addictions and problems.");
    JobData[JOB_REHAB] = sJobData("Rehab", "Rehb");
    JobData[JOB_REHAB].description = ("She will go to rehab to get over her addictions.");
    JobData[JOB_THERAPY] = sJobData("Therapy", "Thrp");
    JobData[JOB_THERAPY].description = ("She will go to therapy to get over her mental problems. (Nervous, Dependant, Pessimist)");
    JobData[JOB_EXTHERAPY] = sJobData("Extreme Therapy", "EThr");
    JobData[JOB_EXTHERAPY].description = ("She will go to extreme therapy to get over her hardcore mental problems. (Mind Fucked, Broken Will)");
    JobData[JOB_ANGER] = sJobData("Anger Management", "AMng");
    JobData[JOB_ANGER].description = ("She will go to anger management to get over her anger problems. (Aggressive, Tsundere, Yandere)");

    // - Clinic Surgeries
    JobFilters[JOBFILTER_CLINIC] = sJobFilter{"Medical Clinic", "These are procerures the girls can have done in the Medical Clinic."};
    register_filter(JOBFILTER_CLINIC, JOB_GETHEALING, JOB_FERTILITY);
    JobData[JOB_GETHEALING] = sJobData("Get Healing", "Heal");
    JobData[JOB_GETHEALING].description = ("She will have her wounds attended.");
    JobData[JOB_GETREPAIRS] = sJobData("Get Repaired", "Repr");
    JobData[JOB_GETREPAIRS].description = ("Construct girls will be quickly repaired here.");
    JobData[JOB_CUREDISEASES] = sJobData("Cure Diseases", "Cure");
    JobData[JOB_CUREDISEASES].description = ("She will try to get her diseases cured.");
    JobData[JOB_GETABORT] = sJobData("Abortion", "Abrt");
    JobData[JOB_GETABORT].description = ("She will get an abortion, removing pregnancy and/or insemination.\n*(Takes 2 days or 1 if a Nurse is on duty)");
    JobData[JOB_COSMETICSURGERY] = sJobData("Cosmetic Surgery", "CosS");
    JobData[JOB_COSMETICSURGERY].description = ("She will undergo magical surgery to \"enhance\" her appearance.\n*(Takes up to 5 days, less if a Nurse is on duty)");
    JobData[JOB_LIPO] = sJobData("Liposuction", "Lipo");
    JobData[JOB_LIPO].description = ("She will undergo liposuction to \"enhance\" her figure.\n*(Takes up to 5 days, less if a Nurse is on duty)");
    JobData[JOB_BREASTREDUCTION] = sJobData("Breast Reduction Surgery", "BRS");
    JobData[JOB_BREASTREDUCTION].description = ("She will undergo breast reduction surgery.\n*(Takes up to 5 days, less if a Nurse is on duty)");
    JobData[JOB_BOOBJOB] = sJobData("Boob Job", "BbJb");
    JobData[JOB_BOOBJOB].description = ("She will undergo surgery to \"enhance\" her bust.\n*(Takes up to 5 days, less if a Nurse is on duty)");
    JobData[JOB_VAGINAREJUV] = sJobData("Vaginal Rejuvenation", "VagR");
    JobData[JOB_VAGINAREJUV].description = ("She will undergo surgery to make her a virgin again.\n*(Takes up to 5 days, less if a Nurse is on duty)");
    JobData[JOB_FACELIFT] = sJobData("Face Lift", "FLft");
    JobData[JOB_FACELIFT].description = ("She will undergo surgery to make her younger.\n*(Takes up to 5 days, less if a Nurse is on duty)");
    JobData[JOB_ASSJOB] = sJobData("Arse Job", "AssJ");
    JobData[JOB_ASSJOB].description = ("She will undergo surgery to \"enhance\" her ass.\n*(Takes up to 5 days, less if a Nurse is on duty)");
    JobData[JOB_TUBESTIED] = sJobData("Tubes Tied", "TTid");
    JobData[JOB_TUBESTIED].description = ("She will undergo surgery to make her sterile.\n*(Takes up to 5 days, less if a Nurse is on duty)");
    JobData[JOB_FERTILITY] = sJobData("Fertility Treatmeant", "FrtT");
    JobData[JOB_FERTILITY].description = ("She will undergo surgery to make her fertile.\n*(Takes up to 5 days, less if a Nurse is on duty)");
    // Clinic staff
    JobFilters[JOBFILTER_CLINICSTAFF] = sJobFilter{"Clinic Staff", "These are jobs that help run a medical clinic."};
    register_filter(JOBFILTER_CLINICSTAFF, JOB_CLINICREST, JOB_JANITOR);
    JobData[JOB_CLINICREST] = sJobData("Time off", "TOff");
    JobData[JOB_CLINICREST].description = ("She will rest");
    JobData[JOB_CHAIRMAN] = sJobData("Chairman", "Crmn");
    JobData[JOB_CHAIRMAN].description = ("She will watch over the staff of the clinic");
    JobData[JOB_DOCTOR] = sJobData("Doctor", "Doc");
    JobData[JOB_DOCTOR].description = ("She will become a doctor. Doctors earn extra cash from treating locals. (requires at least 1 to perform surgeries)");
    JobData[JOB_NURSE] = sJobData("Nurse", "Nurs");
    JobData[JOB_NURSE].description = ("Will help the doctor and heal sick people.");
    JobData[JOB_MECHANIC] = sJobData("Mechanic", "Mech");
    JobData[JOB_MECHANIC].description = ("Will help the doctor and repair Constructs.");
    JobData[JOB_INTERN] = sJobData("Intern", "Ntrn");
    JobData[JOB_INTERN].description = ("Will train in how to be a nurse.");
    JobData[JOB_JANITOR] = sJobData("Janitor", "Jntr");
    JobData[JOB_JANITOR].description = ("She will clean the clinic");

    // - Farm Staff
    JobFilters[JOBFILTER_FARMSTAFF] = sJobFilter{"Farm Staff", "These are jobs for running a farm"};
    register_filter(JOBFILTER_FARMSTAFF, JOB_FARMREST, JOB_FARMHAND);
    JobData[JOB_FARMREST] = sJobData("Time off", "TOff");
    JobData[JOB_FARMREST].description = "She will take time off and rest.";
    JobData[JOB_FARMMANGER] = sJobData("Farm Manager", "FMgr");
    JobData[JOB_FARMMANGER].description = "She will watch over the farm and girls working there.";
    JobData[JOB_VETERINARIAN] = sJobData("Veterinarian", "Vet");
    JobData[JOB_VETERINARIAN].description = "She will tend to sick animals.";
    JobData[JOB_MARKETER] = sJobData("Marketer", "Mrkt");
    JobData[JOB_MARKETER].description = "She will buy and sell things.";
    JobData[JOB_RESEARCH] = sJobData("Researcher", "Rsrc");
    JobData[JOB_RESEARCH].description = "She will research how to improve various things.";
    JobData[JOB_FARMHAND] = sJobData("Farm Hand", "FHnd");
    JobData[JOB_FARMHAND].description = "She will clean up around the farm.";
    // - Laborers
    JobFilters[JOBFILTER_LABORERS] = sJobFilter{"Laborers", "These are jobs your girls can do at your farm."};
    register_filter(JOBFILTER_LABORERS, JOB_FARMER, JOB_MILK);
    JobData[JOB_FARMER] = sJobData("Farmer", "Frmr");
    JobData[JOB_FARMER].description = ("She will tend to your crops.");
    JobData[JOB_GARDENER] = sJobData("Gardener", "Grdn");
    JobData[JOB_GARDENER].description = ("She will produce herbs.");
    JobData[JOB_SHEPHERD] = sJobData("Shepherd", "SHrd");
    JobData[JOB_SHEPHERD].description = ("She will tend to food animals.");
    JobData[JOB_RANCHER] = sJobData("Rancher", "Rncr");
    JobData[JOB_RANCHER].description = ("She will tend to larger animals.");
    JobData[JOB_CATACOMBRANCHER] = sJobData("Catacombs Rancher", "CRnc");
    JobData[JOB_CATACOMBRANCHER].description = ("She will tend to strange beasts.");
    JobData[JOB_BEASTCAPTURE] = sJobData("Beast Capture", "BCpt");
    JobData[JOB_BEASTCAPTURE].description = "She will go out into the wilds to capture beasts.";
    JobData[JOB_MILKER] = sJobData("Milker", "Mlkr");
    JobData[JOB_MILKER].description = ("She will milk the various animals and girls you own.");
    JobData[JOB_MILK] = sJobData("Get Milked", "Mlkd");
    JobData[JOB_MILK].description = "She will have her breasts milked";
    // - Producers
    JobFilters[JOBFILTER_PRODUCERS] = sJobFilter{"Producers", "These are jobs your girls can do at your farm."};
    register_filter(JOBFILTER_PRODUCERS, JOB_BUTCHER, JOB_MAKEPOTIONS);
    JobData[JOB_BUTCHER] = sJobData("Butcher", "Bchr");
    JobData[JOB_BUTCHER].description = ("She will produce food from animals you own.");
    JobData[JOB_BAKER] = sJobData("Baker", "Bkr");
    JobData[JOB_BAKER].description = ("She will produce food from the crops you own.");
    JobData[JOB_BREWER] = sJobData("Brewer", "Brwr");
    JobData[JOB_BREWER].description = ("She will make various beers and wines.");
    JobData[JOB_TAILOR] = sJobData("Tailor", "Talr");
    JobData[JOB_TAILOR].description = ("She will make clothes and other items from fabrics.");
    JobData[JOB_MAKEITEM] = sJobData("Make Items", "MkIt");
    JobData[JOB_MAKEITEM].description = ("She will make various items.");
    JobData[JOB_MAKEPOTIONS] = sJobData("Make Potions", "MkPt");
    JobData[JOB_MAKEPOTIONS].description = ("She will make various potions.");

    // - House
    JobFilters[JOBFILTER_HOUSE] = sJobFilter{"Player House", "These are jobs your girls can do at your house."};
    register_filter(JOBFILTER_HOUSE, JOB_HOUSEREST, JOB_CLEANHOUSE);
    JobData[JOB_HOUSEREST] = sJobData("Time off", "TOff");
    JobData[JOB_HOUSEREST].description = ("She takes time off resting and recovering.");
    JobData[JOB_HEADGIRL] = sJobData("Head Girl", "HGrl");
    JobData[JOB_HEADGIRL].description = ("She takes care of the girls in your house.");
    JobData[JOB_RECRUITER] = sJobData("Recruiter", "Rcrt");
JobData[JOB_RECRUITER].description = "She will go out and try and recruit girls for you.";
    JobData[JOB_PERSONALBEDWARMER] = sJobData("Bed Warmer", "BdWm");
    JobData[JOB_PERSONALBEDWARMER].description = ("She will stay in your bed at night with you.");
    JobData[JOB_HOUSECOOK] = sJobData("House Cook", "Hcok");
    JobData[JOB_HOUSECOOK].description = ("She will cook for your house.");
    //JobData[JOB_PONYGIRL].name = ("Pony Girl");
    //JobData[JOB_PONYGIRL].brief = "PGil";
    //JobData[JOB_PONYGIRL].description = ("She will be trained to become a pony girl.");
    //JobFunc[JOB_PONYGIRL] = &WorkFarmPonyGirl;
    JobData[JOB_CLEANHOUSE] = sJobData("Clean House", "ClnH");
    JobData[JOB_CLEANHOUSE].description = ("She will clean your house.");

    JobFilters[JOBFILTER_HOUSETTRAINING] = sJobFilter{"Sex Training", "You will take a personal interest in training the girl in sexual matters."};
    register_filter(JOBFILTER_HOUSETTRAINING, JOB_PERSONALTRAINING, JOB_HOUSEPET);
    JobData[JOB_PERSONALTRAINING] = sJobData("Personal Training", "PTrn");
    JobData[JOB_PERSONALTRAINING].description = ("You will oversee her training personal.");
    JobData[JOB_HOUSEPET] = sJobData("House Pet", "Hpet");
    JobData[JOB_HOUSEPET].description = ("She will be trained to become the house pet.");
    JobData[JOB_SO_STRAIGHT] = sJobData("SO Straight", "SOSt");
    JobData[JOB_SO_STRAIGHT].description = ("You will make sure she only likes having sex with men.");
    JobData[JOB_SO_BISEXUAL] = sJobData("SO Bisexual", "SOBi");
    JobData[JOB_SO_BISEXUAL].description = ("You will make sure she likes having sex with both men and women.");
    JobData[JOB_SO_LESBIAN] = sJobData("SO Lesbian", "SOLe");
    JobData[JOB_SO_LESBIAN].description = ("You will make sure she only likes having sex with women.");
    JobData[JOB_FAKEORGASM] = sJobData("Fake Orgasm Expert", "FOEx");
    JobData[JOB_FAKEORGASM].description = ("You will teach her how to fake her orgasms.");



    JobData[JOB_INDUNGEON].name    = ("In the Dungeon");
    JobData[JOB_INDUNGEON].brief    = "Dngn";
    JobData[JOB_INDUNGEON].description    = ("She is languishing in the dungeon.");
    JobData[JOB_RUNAWAY].name    = ("Runaway");
    JobData[JOB_RUNAWAY].brief    = "RunA";
    JobData[JOB_RUNAWAY].description    = ("She has run away.");




#if 0
    // - Community Centre
    JobFunc[JOB_COLLECTDONATIONS] = &WorkVoid;    // ************** TODO
    JobFunc[JOB_ARTISAN] = &WorkMakeItem;    // ************** TODO
    JobFunc[JOB_SELLITEMS] = &WorkVoid;    // ************** TODO
    // - Drug Lab (these jobs gain bonus if in same building as a clinic)
    JobFunc[JOB_VIRASPLANTFUCKER] = &WorkVoid;    // ************** TODO
    JobFunc[JOB_SHROUDGROWER] = &WorkVoid;    // ************** TODO
    JobFunc[JOB_FAIRYDUSTER] = &WorkVoid;    // ************** TODO
    JobFunc[JOB_DRUGDEALER] = &WorkVoid;    // ************** TODO
    // - Alchemist lab (these jobs gain bonus if in same building as a drug lab)
    JobFunc[JOB_FINDREGENTS] = &WorkVoid;    // ************** TODO
    JobFunc[JOB_BREWPOTIONS] = &WorkVoid;    // ************** TODO
    JobFunc[JOB_POTIONTESTER] = &WorkVoid;    // ************** TODO
    // - Arena (these jobs gain bonus if in same building as a clinic)
    JobFunc[JOB_FIGHTBEASTS] = &WorkVoid;    // ************** TODO
    JobFunc[JOB_WRESTLE] = &WorkVoid;    // ************** TODO
    JobFunc[JOB_FIGHTTODEATH] = &WorkVoid;    // ************** TODO
    JobFunc[JOB_FIGHTVOLUNTEERS] = &WorkVoid;    // ************** TODO
    JobFunc[JOB_COLLECTBETS] = &WorkVoid;    // ************** TODO
    // - Skills Centre
    JobFunc[JOB_TEACHBDSM] = &WorkVoid;    // ************** TODO
    JobFunc[JOB_TEACHSEX] = &WorkVoid;    // ************** TODO
    JobFunc[JOB_TEACHBEAST] = &WorkVoid;    // ************** TODO
    JobFunc[JOB_TEACHMAGIC] = &WorkVoid;    // ************** TODO
    JobFunc[JOB_TEACHCOMBAT] = &WorkVoid;    // ************** TODO
    JobFunc[JOB_DAYCARE] = &WorkVoid;    // ************** TODO
    JobFunc[JOB_SCHOOLING] = &WorkVoid;    // ************** TODO
    JobFunc[JOB_TEACHDANCING] = &WorkVoid;    // ************** TODO
    JobFunc[JOB_TEACHSERVICE] = &WorkVoid;    // ************** TODO
    JobFunc[JOB_TRAIN] = &WorkVoid;    // ************** TODO

#endif




# if 0

    //- Community Centre
    JobFilterName[JOBFILTER_COMMUNITYCENTRE] = ("Community Centre");
    JobFilterDesc[JOBFILTER_COMMUNITYCENTRE] = ("These are jobs for running a community centre.");
    JobFilterIndex[JOBFILTER_COMMUNITYCENTRE] = JOB_COLLECTDONATIONS;
    JobData[JOB_COLLECTDONATIONS].name = ("Collect Donations");
    JobData[JOB_COLLECTDONATIONS].description = ("She will collect money to help the poor.");
    JobData[JOB_ARTISAN].name = ("Make Crafts");
    JobData[JOB_ARTISAN].description = ("She will craft cheap handmade items.");
    JobData[JOB_SELLITEMS].name = ("Sell Crafts");
    JobData[JOB_SELLITEMS].description = ("She will go out and sell previously crafted items.");

    JobFilterName[JOBFILTER_DRUGLAB] = ("Drug Lab");
    JobFilterDesc[JOBFILTER_DRUGLAB] = ("These are jobs for running a drug lab.");
    JobFilterIndex[JOBFILTER_DRUGLAB] = JOB_VIRASPLANTFUCKER;
    JobData[JOB_VIRASPLANTFUCKER].name = ("Fuck Viras Plants");
    JobData[JOB_VIRASPLANTFUCKER].description = ("She will allow Viras plants to inseminate her, creating Vira Blood.");
    JobData[JOB_SHROUDGROWER].name = ("Grow Shroud Mushrooms");
    JobData[JOB_SHROUDGROWER].description = ("She will grow Shroud Mushrooms, which require a lot of care and may explode.");
    JobData[JOB_FAIRYDUSTER].name = ("Fairy Duster");
    JobData[JOB_FAIRYDUSTER].description = ("She will capture fairies and grind them into a fine powder known as Fairy Dust.");
    JobData[JOB_DRUGDEALER].name = ("Drug Dealer");
    JobData[JOB_DRUGDEALER].description = ("She will go out and sell your drugs on the streets.");

    JobFilterName[JOBFILTER_ALCHEMISTLAB] = ("Alchemist Lab");
    JobFilterDesc[JOBFILTER_ALCHEMISTLAB] = ("These are jobs for running an alchemist lab.");
    JobFilterIndex[JOBFILTER_ALCHEMISTLAB] = JOB_FINDREGENTS;
    JobData[JOB_FINDREGENTS].name = ("Find Reagents");
    JobData[JOB_FINDREGENTS].description = ("She will go out and look for promising potion ingredients.");
    JobData[JOB_BREWPOTIONS].name = ("Brew Potions");
    JobData[JOB_BREWPOTIONS].description = ("She will brew a random potion using available ingredients.");
    JobData[JOB_POTIONTESTER].name = ("Guinea Pig");
    JobData[JOB_POTIONTESTER].description = ("She will test out random potions. Who knows what might happen? (requires 1)");

    JobFilterName[JOBFILTER_ARENA] = ("Arena");
    JobFilterDesc[JOBFILTER_ARENA] = ("These are jobs for running an arena.");
    JobFilterIndex[JOBFILTER_ARENA] = JOB_FIGHTBEASTS;
    JobData[JOB_FIGHTBEASTS].name = ("Fight Beasts");
    JobData[JOB_FIGHTBEASTS].description = ("She will fight to the death against beasts you own. Dangerous.");
    JobData[JOB_WRESTLE].name = ("Wrestle");
    JobData[JOB_WRESTLE].description = ("She will take part in a wrestling match. Not dangerous.");
    JobData[JOB_FIGHTTODEATH].name = ("Cage Match");
    JobData[JOB_FIGHTTODEATH].description = ("She will fight to the death against other girls. Dangerous.");
    JobData[JOB_FIGHTVOLUNTEERS].name = ("Fight Volunteers");
    JobData[JOB_FIGHTVOLUNTEERS].description = ("She will fight against customer volunteers for gold prizes. Not dangerous.");
    JobData[JOB_COLLECTBETS].name = ("Collect Bets");
    JobData[JOB_COLLECTBETS].description = ("She will collect bets from the customers. (requires 1)");

    JobFilterName[JOBFILTER_TRAININGCENTRE] = ("Training Centre");
    JobFilterDesc[JOBFILTER_TRAININGCENTRE] = ("These are jobs for running a training centre.");
    JobFilterIndex[JOBFILTER_TRAININGCENTRE] = JOB_TEACHBDSM;
    JobData[JOB_TEACHBDSM].name = ("Teach BDSM");
    JobData[JOB_TEACHBDSM].description = ("She will teach other girls in BDSM skills.");
    JobData[JOB_TEACHSEX].name = ("Teach Sex");
    JobData[JOB_TEACHSEX].description = ("She will teach other girls the ins and outs of regular sex.");
    JobData[JOB_TEACHBEAST].name = ("Teach Bestiality");
    JobData[JOB_TEACHBEAST].description = ("She will teach other girls how to safely fuck beasts.");
    JobData[JOB_TEACHMAGIC].name = ("Teach Magic");
    JobData[JOB_TEACHMAGIC].description = ("She will teach other girls how to wield magic.");
    JobData[JOB_TEACHCOMBAT].name = ("Teach Combat");
    JobData[JOB_TEACHCOMBAT].description = ("She will teach other girls how to fight.");
    JobData[JOB_DAYCARE].name = ("Daycare");
    JobData[JOB_DAYCARE].description = ("She will look after local children. (non-sexual)");
    JobData[JOB_SCHOOLING].name = ("Schooling");
    JobData[JOB_SCHOOLING].description = ("She will teach the local children. (non-sexual)");
    JobData[JOB_TEACHDANCING].name = ("Teach Dancing");
    JobData[JOB_TEACHDANCING].description = ("She will teach other girls dancing and social skills.");
    JobData[JOB_TEACHSERVICE].name = ("Teach Service");
    JobData[JOB_TEACHSERVICE].description = ("She will teach other girls a variety of service skills.");
    JobData[JOB_TRAIN].name = ("Student");
    JobData[JOB_TRAIN].description = ("She will learn from all of the other girls who are teaching here.");

#endif

    RegisterCraftingJobs(*this);
    RegisterSurgeryJobs(*this);
    RegisterWrappedJobs(*this);
    RegisterManagerJobs(*this);
    RegisterFilmJobs(*this);
    RegisterTherapyJobs(*this);
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

bool cJobManager::FullTimeJob(u_int Job)
{
    return (
        Job == JOB_MATRON ||
        Job == JOB_TORTURER ||
        // - Arena
        Job == JOB_DOCTORE ||
        // - Clinic
        Job == JOB_GETHEALING ||
        Job == JOB_GETREPAIRS ||
        Job == JOB_GETABORT ||
        Job == JOB_COSMETICSURGERY ||
        Job == JOB_LIPO ||
        Job == JOB_BREASTREDUCTION ||
        Job == JOB_BOOBJOB ||
        Job == JOB_VAGINAREJUV ||
        Job == JOB_FACELIFT ||
        Job == JOB_ASSJOB ||
        Job == JOB_TUBESTIED ||
        Job == JOB_FERTILITY ||
        // - clinic staff
        Job == JOB_CHAIRMAN ||
        Job == JOB_DOCTOR ||
        Job == JOB_NURSE ||
        Job == JOB_MECHANIC ||
        // - Community Centre
        Job == JOB_CENTREMANAGER ||
        // - drug centre
        Job == JOB_COUNSELOR ||
        Job == JOB_REHAB ||
        Job == JOB_ANGER ||
        Job == JOB_EXTHERAPY ||
        Job == JOB_THERAPY ||
        // - House
        Job == JOB_HEADGIRL ||
        Job == JOB_RECRUITER ||
        Job == JOB_FAKEORGASM ||
        Job == JOB_SO_STRAIGHT ||
        Job == JOB_SO_BISEXUAL ||
        Job == JOB_SO_LESBIAN ||
        Job == JOB_HOUSEPET ||
        // - Farm
        Job == JOB_FARMMANGER ||
        //Job == JOB_PONYGIRL ||
        false);
}

bool cJobManager::is_job_Paid_Player(u_int Job)
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
bool cJobManager::HandleSpecialJobs(sGirl& Girl, JOBS JobID, int OldJobID, bool Day0Night1, bool fulltime)
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
#if 1
    else if (u_int(JobID) == JOB_MATRON)
    {
        if (Girl.m_Building->num_girls_on_job(JOB_MATRON, Day0Night1) > 0)
            g_Game->push_message(("You can only have one matron per brothel."), 0);
        else if (Girl.is_slave())
            g_Game->push_message(("The matron cannot be a slave."), 0);
        else
            Girl.m_NightJob = Girl.m_DayJob = JOB_MATRON;
    }
    else if (u_int(JobID) == JOB_TORTURER)
    {
        if (random_girl_on_job(g_Game->buildings(), JOB_TORTURER, 0))
            g_Game->push_message(("You can only have one torturer among all of your brothels."), 0);
        else if (Girl.is_slave())
            g_Game->push_message(("The torturer cannot be a slave."), 0);
        else
            Girl.m_NightJob = Girl.m_DayJob = JOB_TORTURER;
    }
#endif
    // Special House Jobs
#if 1
    else if (u_int(JobID) == JOB_HEADGIRL)
    {
        if (Girl.m_Building->num_girls_on_job(JOB_HEADGIRL, Day0Night1) > 0)    g_Game->push_message(("There can be only one Head Girl!"), 0);
        else if (Girl.is_slave())/*            */    g_Game->push_message(("The Head Girl cannot be a slave."), 0);
        else /*                                 */    Girl.m_NightJob = Girl.m_DayJob = JOB_HEADGIRL;
    }
    else if (u_int(JobID) == JOB_RECRUITER)
    {
        if (Girl.is_slave())/*                 */    g_Game->push_message(("The recruiter cannot be a slave."), 0);
        else /*                                 */    Girl.m_NightJob = Girl.m_DayJob = JOB_RECRUITER;
    }
    else if (u_int(JobID) == JOB_FAKEORGASM)
    {
        if (Girl.has_active_trait("Fake Orgasm Expert"))    g_Game->push_message(("She already has \"Fake Orgasm Expert\"."), 0);
        else /*                                 */    Girl.m_DayJob = Girl.m_NightJob = JOB_FAKEORGASM;
    }
    else if (u_int(JobID) == JOB_SO_BISEXUAL)
    {
        if (Girl.has_active_trait("Bisexual"))/*      */    g_Game->push_message(("She is already Bisexual."), 0);
        else /*                                 */    Girl.m_DayJob = Girl.m_NightJob = JOB_SO_BISEXUAL;
    }
    else if (u_int(JobID) == JOB_SO_LESBIAN)
    {
        if (Girl.has_active_trait("Lesbian"))/*       */    g_Game->push_message(("She is already a Lesbian."), 0);
        else /*                                 */    Girl.m_DayJob = Girl.m_NightJob = JOB_SO_LESBIAN;
    }
    else if (u_int(JobID) == JOB_SO_STRAIGHT)
    {
        if (Girl.has_active_trait("Straight"))/*      */    g_Game->push_message(("She is already Straight."), 0);
        else /*                                 */    Girl.m_DayJob = Girl.m_NightJob = JOB_SO_STRAIGHT;
    }
    else if (u_int(JobID) == JOB_HOUSEPET)
    {
        if (Girl.is_slave())/*                 */    Girl.m_NightJob = Girl.m_DayJob = JOB_HOUSEPET;
        else /*                                 */    g_Game->push_message(("Only slaves can take this training."), 0);
    }
//    else if (u_int(JobID) == JOB_PONYGIRL)
//    {
//        if (Girl.is_slave())    Girl.m_NightJob = Girl.m_DayJob = JOB_PONYGIRL;
//        else                    g_Game->push_message(("Only slaves can take this training."), 0);
//    }
#endif
    // Special Farm Jobs
#if 1
    else if (u_int(JobID) == JOB_FARMMANGER)
    {
        if (Girl.m_Building->num_girls_on_job(JOB_FARMMANGER, Day0Night1) > 0)
            g_Game->push_message(("There can be only one Farm Manager!"), 0);
        else if (Girl.is_slave())
            g_Game->push_message(("The Farm Manager cannot be a slave."), 0);
        else
            Girl.m_NightJob = Girl.m_DayJob = JOB_FARMMANGER;
    }
    else if (u_int(JobID) == JOB_MARKETER)
    {
        if (Girl.is_slave())
            g_Game->push_message(("The Farm Marketer cannot be a slave."), 0);
        else
        {
            if (Day0Night1 == SHIFT_DAY || fulltime)
            {
                if (Girl.m_Building->num_girls_on_job(JOB_MARKETER, SHIFT_DAY) > 0)
                    g_Game->push_message(("There can be only one Farm Marketer on each shift!"), 0);
                else Girl.m_DayJob = JOB_MARKETER;
            }
            if (Day0Night1 == SHIFT_NIGHT || fulltime)
            {
                if (Girl.m_Building->num_girls_on_job(JOB_MARKETER, SHIFT_NIGHT) > 0)
                    g_Game->push_message(("There can be only one Farm Marketer on each shift!"), 0);
                else Girl.m_NightJob = JOB_MARKETER;
            }

        }
    }
#endif
    // Special Arena Jobs
#if 1
    else if (u_int(JobID) == JOB_DOCTORE)
    {
        if (Girl.m_Building->num_girls_on_job(JOB_DOCTORE, Day0Night1) > 0)
            g_Game->push_message(("There can be only one Doctore!"), 0);
        else if (Girl.is_slave())
            g_Game->push_message(("The Doctore cannot be a slave."), 0);
        else
            Girl.m_NightJob = Girl.m_DayJob = JOB_DOCTORE;
    }
    else if (u_int(JobID) == JOB_CITYGUARD && Girl.is_slave())
    {
        g_Game->push_message(("Slaves can not work as City Guards."), 0);
    }
    else if (u_int(JobID) == JOB_FIGHTTRAIN && (Girl.combat() > 99 && Girl.magic() > 99 && Girl.agility() > 99 && Girl.constitution() > 99))
    {    // `J` added then modified
        g_Game->push_message(("There is nothing more she can learn here."), 0);
        if (Girl.m_DayJob == JOB_FIGHTTRAIN)    Girl.m_DayJob = rest;
        if (Girl.m_NightJob == JOB_FIGHTTRAIN)    Girl.m_NightJob = rest;
    }
#endif
    // Special Clinic Jobs
#if 1
    else if (u_int(JobID) == JOB_CHAIRMAN)
    {
        if (Girl.m_Building->num_girls_on_job(JOB_CHAIRMAN, Day0Night1)>0)
            g_Game->push_message(("There can be only one Chairman!"), 0);
        else if (Girl.is_slave())
            g_Game->push_message(("The Chairman cannot be a slave."), 0);
        else
            Girl.m_NightJob = Girl.m_DayJob = JOB_CHAIRMAN;
    }

    else if (Girl.has_active_trait("AIDS") && (u_int(JobID) == JOB_DOCTOR || u_int(JobID) == JOB_NURSE || u_int(JobID) == JOB_INTERN))
    {
        g_Game->push_message(("Health laws prohibit anyone with AIDS from working in the Medical profession"), 0);
        if (Girl.m_DayJob == JOB_INTERN || Girl.m_DayJob == JOB_NURSE || Girl.m_DayJob == JOB_DOCTOR)
            Girl.m_DayJob = rest;
        if (Girl.m_NightJob == JOB_INTERN || Girl.m_NightJob == JOB_NURSE || Girl.m_NightJob == JOB_DOCTOR)
            Girl.m_NightJob = rest;
    }
    else if (u_int(JobID) == JOB_DOCTOR)
    {
        if (Girl.is_slave())
        {
            g_Game->push_message(("The Doctor cannot be a slave."), 0);
        }
        else if (Girl.medicine() < 50 || Girl.intelligence() < 50)
        {
            stringstream ss;
            ss << Girl.FullName() << " does not have enough training to work as a Doctor. Doctors require 50 Medicine and 50 Intelligence.";
            g_Game->push_message(ss.str(), 0);
        }
        else Girl.m_NightJob = Girl.m_DayJob = JOB_DOCTOR;
    }
    else if (u_int(JobID) == JOB_INTERN && Girl.medicine() > 99 && Girl.intelligence() > 99 && Girl.charisma() > 99)
    {
        stringstream ss;
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
    else if (u_int(JobID) == JOB_MECHANIC)
    {
        Girl.m_NightJob = Girl.m_DayJob = JOB_MECHANIC;
    }
    else if (u_int(JobID) == JOB_GETHEALING && Girl.has_active_trait("Construct"))
    {
        if (fulltime)
            Girl.m_DayJob = Girl.m_NightJob = JOB_GETREPAIRS;
        else if (Day0Night1 == SHIFT_DAY)
            Girl.m_DayJob = JOB_GETREPAIRS;
        else
            Girl.m_NightJob = JOB_GETREPAIRS;
        g_Game->push_message(("The Doctor does not work on Constructs so she sends ") + Girl.FullName() + (" to the Mechanic."), 0);
    }
    else if (u_int(JobID) == JOB_GETREPAIRS && !Girl.has_active_trait("Construct") && !Girl.has_active_trait(
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
        u_int(JobID) == JOB_CUREDISEASES ||
        u_int(JobID) == JOB_GETABORT ||
        u_int(JobID) == JOB_COSMETICSURGERY ||
        u_int(JobID) == JOB_BREASTREDUCTION ||
        u_int(JobID) == JOB_BOOBJOB ||
        u_int(JobID) == JOB_ASSJOB ||
        u_int(JobID) == JOB_FACELIFT ||
        u_int(JobID) == JOB_VAGINAREJUV ||
        u_int(JobID) == JOB_LIPO ||
        u_int(JobID) == JOB_TUBESTIED ||
        u_int(JobID) == JOB_FERTILITY)
    {
        bool jobgood = true;
        if (Girl.m_Building->num_girls_on_job(JOB_DOCTOR, Day0Night1) == 0)
        {
            g_Game->push_message(("You must have a Doctor on duty to perform surgery."), 0);
            jobgood = false;
        }
        else if (u_int(JobID) == JOB_CUREDISEASES && !has_disease(Girl))
        {
            g_Game->push_message(("Oops, the girl does not have any diseases."), 0);
            jobgood = false;
        }
        else if (u_int(JobID) == JOB_GETABORT && !Girl.is_pregnant())
        {
            g_Game->push_message(("Oops, the girl is not pregant."), 0);
            jobgood = false;
        }
        else if (u_int(JobID) == JOB_COSMETICSURGERY)
        {
        }
        else if (u_int(JobID) == JOB_BREASTREDUCTION && Girl.has_active_trait("Flat Chest"))
        {
            g_Game->push_message(("Her boobs can't get no smaller."), 0);
            jobgood = false;
        }
        else if (u_int(JobID) == JOB_BOOBJOB && Girl.has_active_trait("Titanic Tits"))
        {
            g_Game->push_message(("Her boobs can't get no bigger."), 0);
            jobgood = false;
        }
        else if (u_int(JobID) == JOB_ASSJOB && Girl.has_active_trait("Great Arse"))
        {
            g_Game->push_message(("Her ass can't get no better."), 0);
            jobgood = false;
        }
        else if (u_int(JobID) == JOB_FACELIFT && Girl.age() <= 21)
        {
            g_Game->push_message(("She is to young for a face lift."), 0);
            jobgood = false;
        }
        else if (u_int(JobID) == JOB_VAGINAREJUV && is_virgin(Girl))
        {
            g_Game->push_message(("She is a virgin and has no need of this operation."), 0);
            jobgood = false;
        }
        else if (u_int(JobID) == JOB_LIPO && Girl.has_active_trait("Great Figure"))
        {
            g_Game->push_message(("She already has a great figure and doesn't need this."), 0);
            jobgood = false;
        }
        else if (u_int(JobID) == JOB_TUBESTIED)
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
        else if (u_int(JobID) == JOB_FERTILITY)
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
#endif
    // Special Centre Jobs
#if 1
    else if (u_int(JobID) == JOB_CENTREMANAGER)
    {
        if (Girl.m_Building->num_girls_on_job(JOB_CENTREMANAGER, Day0Night1) >0)
            g_Game->push_message(("There can be only one Centre Manager!"), 0);
        else if (Girl.is_slave())
            g_Game->push_message(("The Centre Manager cannot be a slave."), 0);
        else
            Girl.m_NightJob = Girl.m_DayJob = JOB_CENTREMANAGER;
    }
    else if (u_int(JobID) == JOB_COUNSELOR)
    {
        if (Girl.is_slave())
            g_Game->push_message(("The Counselor cannot be a slave."), 0);
        else
            Girl.m_NightJob = Girl.m_DayJob = JOB_COUNSELOR;
    }
    else if (u_int(JobID) == JOB_REHAB)
    {
        if (Girl.m_Building->num_girls_on_job(JOB_COUNSELOR, Day0Night1) < 1)
            g_Game->push_message(("You must have a counselor for rehab."), 0);
        else if (is_addict(Girl))
            g_Game->push_message(("She has no addictions."), 0);
        else
            Girl.m_DayJob = Girl.m_NightJob = JOB_REHAB;
    }
    else if (u_int(JobID) == JOB_ANGER)
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
    else if (u_int(JobID) == JOB_EXTHERAPY)
    {
        if (Girl.m_Building->num_girls_on_job(JOB_COUNSELOR, Day0Night1) < 1)
            g_Game->push_message(("You must have a counselor for extreme therapy."), 0);
        else if (!Girl.has_active_trait("Mind Fucked")
            && !Girl.has_active_trait("Broken Will"))
            g_Game->push_message(("She has no extreme issues."), 0);
        else
            Girl.m_DayJob = Girl.m_NightJob = JOB_EXTHERAPY;
    }
    else if (u_int(JobID) == JOB_THERAPY)
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
#endif
    // Special Movie Studio Jobs
#if 1
    else if (u_int(JobID) == JOB_DIRECTOR && Girl.m_Building->num_girls_on_job(JOB_DIRECTOR, SHIFT_NIGHT) >0)
    {
        g_Game->push_message(("There can be only one Director!"), 0);
    }
    else if (u_int(JobID) == JOB_DIRECTOR && Girl.is_slave())
    {
        g_Game->push_message(("The Director cannot be a slave."), 0);
    }
    else if (u_int(JobID) == JOB_PROMOTER && Girl.m_Building->num_girls_on_job(JOB_PROMOTER, SHIFT_NIGHT) > 0)
    {
        g_Game->push_message(("There can be only one Promoter."), 0);
    }
    else if (u_int(JobID) == JOB_PROMOTER && Girl.is_slave())
    {
        g_Game->push_message(("The Promoter cannot be a slave."), 0);
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
    int rape_chance = (streets ? (int)cfg.prostitution.rape_streets() : (int)cfg.prostitution.rape_brothel());

    IBuilding * Brothl = girl.m_Building;

    // `J` adjusted this a bit so gangs spying on the girl can help also
    vector<sGang *> gangs_guarding = g_Game->gang_manager().gangs_watching_girls();

    int gang_coverage = guard_coverage(&gangs_guarding);

    // night time doubles the chance of attempted rape and reduces the chance for a gang to catch it by 20%
    if (Day0Night1) { rape_chance *= 2; gang_coverage = (int)((float)gang_coverage*0.8f); }

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
        enemy_gang->m_Num = (max(1, g_Dice.bell(-5, 10)));

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
                girl.AddMessage(("She beat the customer silly."), IMGTYPE_COMBAT, Day0Night1);
                break;
            case 1:
                girl.AddMessage(("The customer's face annoyed her, so she punched it until it went away."), IMGTYPE_COMBAT, Day0Night1);
                break;
            case 2:
                girl.AddMessage(("The customer acted like he owned her - so she pwned him."), IMGTYPE_COMBAT, Day0Night1);
                break;
            case 3:
                girl.AddMessage(("The customer's attitude was bad. She corrected it."), IMGTYPE_COMBAT, Day0Night1);
                break;
            case 4:
                girl.AddMessage(("He tried to insert a bottle into her, so she 'gave it' to him instead."), IMGTYPE_COMBAT, Day0Night1);
                break;
            default:
                girl.AddMessage(("Did som(E)thing violent."), IMGTYPE_COMBAT, Day0Night1);
                break;
            }
        girl.fame(-1);
    }
    return false;
}

// We need a cGuards guard manager. Or possible a cGuardsGuards manager.
int cJobManager::guard_coverage(vector<sGang*> *vpt)
{
    int pc = 0;
    vector<sGang*> v = g_Game->gang_manager().gangs_on_mission(MISS_GUARDING);
    if (vpt != nullptr) *vpt = v;
    for (u_int i = 0; i < v.size(); i++)
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

    string SecName = SecGuard->FullName();

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

        stringstream Gmsg;
        stringstream SGmsg;

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
                string item;
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
                    stringstream CGmsg;

                    // `J` create the customer
                    auto custgirl = g_Game->CreateRandomGirl(g_Dice % 40 + 18, true, false, (g_Dice % 3 == 1));    // `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live

                    // `J` and adjust her stats
                    sInventoryItem* item_p = g_Game->inventory_manager().GetItem(item);
                    custgirl->add_item(item_p);
                    custgirl->equip(item_p, true);
                    g_Game->player().inventory().remove_item(item_p);
                    custgirl->add_temporary_trait("Emprisoned Customer", max(5, g_Dice.bell(0, 20)));
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
            stringstream Tmsg;
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
        stringstream ss;
        ss << "Security Problem:\n" << "Trying to defend " << girl.FullName() << ". You defeated "
           << num << " of " << OrgNumMem << ". By now the guarding gangs have arrived, and will deal with the offenders.";
        SecGuard->m_Events.AddMessage(ss.str(), IMGTYPE_DEATH, EVENT_DANGER);
        return false;
    }
    else  // Loss
    {
        u_int attacktype = SKILL_COMBAT;                                                // can be anything
        if (enemy_gang->m_Num > 1 && g_Dice.percent(40)) attacktype = SKILL_GROUP;        // standard group attack
        else if (enemy_gang->m_Num > 1 && g_Dice.percent(40)) attacktype = SKILL_BDSM;    // special group attack
        else if (g_Dice.percent(20)) attacktype = SKILL_LESBIAN;                        // female attacker
        else if (g_Dice.percent(40)) attacktype = SKILL_NORMALSEX;                        // single male attacker


        stringstream ss;
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

bool cJobManager::gang_stops_rape(sGirl& girl, vector<sGang *> gangs_guarding, sGang *enemy_gang, int coverage, int day_night)
{
    if (!g_Dice.percent(coverage)) return false;

    sGang *guarding_gang = gangs_guarding.at(g_Dice%gangs_guarding.size());
    int start_num = enemy_gang->m_Num;

    // GangVsGang returns true if enemy_gang loses
    auto result = GangBrawl(*guarding_gang, *enemy_gang);

    if (result == EFightResult::DEFEAT) {
        stringstream gang_s, girl_s;
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
    stringstream gang_ss, girl_ss;
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

        stringstream msg;

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
    u_int attacktype = SKILL_COMBAT;                                                // can be anything
    /* */if (numberofattackers > 1 && g_Dice.percent(40)) attacktype = SKILL_GROUP;    // standard group attack
    else if (numberofattackers > 1 && g_Dice.percent(40)) attacktype = SKILL_BDSM;    // special group attack
    else if (g_Dice.percent(20)) attacktype = SKILL_LESBIAN;                        // female attacker
    else if (g_Dice.percent(40)) attacktype = SKILL_NORMALSEX;                        // single male attacker

    stringstream ss;
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
// doc: let's have this return a string, instead of a stringstream:
// the caller doesn't need the stream and gcc is giving weird type coercion
// errors

string cJobManager::GetGirlAttackedString(u_int attacktype)
{
    int roll1 = 0, roll2 = 0, roll3 = 0, rolla = 0;
    int maxroll1 = 21, maxroll2 = 20, maxroll3 = 21;

    stringstream ss;
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
void cJobManager::get_training_set(vector<sGirl*> &v, vector<sGirl*> &t_set)
{
    u_int max = 4;
    u_int v_siz = v.size();

    // empty out the trainable set
    t_set.clear();
     if (v_siz == 0u) return;    // if there's nothing in the vector, return with t_set empty
    if (v_siz == 1u)            // if there's just one, return with t_set containing that
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
    u_int lim = (max < v_siz ? max : v_siz);
/*
 *    now loop until we hit max, or run out of girls
 */
     for(u_int i = 0; i < lim; i++)
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
    girl.AddMessage(("She trained during this shift by herself, so learning anything worthwhile was difficult."), IMGTYPE_PROFILE, Day0Night1);
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
    string improved = trainee.update_random(amt);
    stringstream ss;
    ss.str("");
    ss << ("She managed to gain ") << amt << " " << improved << (".");
    girl.m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_SUMMARY);
}

void cJobManager::do_training_set(vector<sGirl*> girls, bool Day0Night1)
{
    sGirl *girl;
    stringstream ss;
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
    vector<TrainableGirl> set;
    /*
    *    4 is the maximum set size. I should probably consider
    *    making that a class constant - or a static class member
    *    initialised from the config file. Later for that.
    */
    u_int num_girls = girls.size();
    for (u_int i = 0; i < num_girls; i++)
    {
        girl = girls[i];
        if (girl == nullptr) break;
        set.push_back(TrainableGirl(girl));
    }
    /*
    *    now get an idealized composite of the girls in the set
    *
    *    and the get a vector of the indices of the most efficient
    *    three attributes for them to train
    */
    IdealGirl ideal(set);
    vector<int> indices = ideal.training_indices();
    /*
    *    OK. Loop over the girls, and then over the indices
    */
    for (u_int g_idx = 0; g_idx < set.size(); g_idx++)
    {
        TrainableGirl &trainee = set[g_idx];

        for (u_int i_idx = 0; i_idx < indices.size(); i_idx++)
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
        for (u_int i = 0; i < num_girls; i++)
        {
            ss << girls[i]->FullName();
            if (i == 0 && num_girls == 2)    ss << (" and ");
            else if (i < num_girls - 2)        ss << (", ");
            else if (i == num_girls - 2)    ss << (", and ");
        }
        ss << (".");

        girl->AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);
        ss.str("");
        ss << ("She trained during this shift");

        bool raised = false;
        for (u_int i_idx = 0; i_idx < indices.size(); i_idx++)
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
    vector<sGirl*> t_set;
    vector<sGirl*> girls = brothel->girls_on_job(JOB_TRAINING, Day0Night1);

    for (u_int i = girls.size(); i-- > 0;)
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
    brothel->m_Finance.girl_training(g_Game->tariff().girl_training() * girls.size());
    /*
    *    and then each girl gets to feel tired and horny
    *    as a result of training
    */
    for (u_int i = 0; i < girls.size(); i++)
    {
        sGirl *girl = girls[i];
        int libido = (girl->has_active_trait("Nymphomaniac")) ? 4 : 2;
        girl->upd_temp_stat(STAT_LIBIDO, libido);
    }
}

// ----- Film & related

string cJobManager::GirlPaymentText(IBuilding * brothel, sGirl& girl, int totalTips, int totalPay, int totalGold,
                                    bool Day0Night1)
{
    stringstream ss;
    string girlName = girl.FullName();
    u_int sw = girl.get_job(Day0Night1);

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
        ss << "ERROR: She has a loss of " << totalGold << " gold\n \nPlease report this to the Pink Petal Devloment Team at http://pinkpetal.org\n \nGirl Name: " << girl.FullName() << "\nJob: " << JobData[(Day0Night1 ? girl.m_NightJob : girl.m_DayJob)].name << "\nPay:     " << girl.m_Pay << "\nTips:   " << girl.m_Tips << "\nTotal: " << totalGold;
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

void cJobManager::CatchGirl(sGirl& girl, stringstream& fuckMessage, const sGang* guardgang) {// try to find an item
    sInventoryItem* item = nullptr;
    if(g_Game->player().inventory().has_item("Brainwashing Oil")) {
        item = g_Game->inventory_manager().GetItem("Brainwashing Oil");
    } else if(g_Game->player().inventory().has_item("Necklace of Control")) {
        item = g_Game->inventory_manager().GetItem("Necklace of Control");
    } else if(g_Game->player().inventory().has_item("Slave Band")) {
        item = g_Game->inventory_manager().GetItem("Slave Band");
    }
    if (item || g_Dice.percent(10)) {
        string itemname;
        stringstream itemtext;
        itemtext << "Your gang " << (guardgang ? guardgang->name() : "");
        stringstream CGmsg;
        // `J` create the customer
        auto custgirl = g_Game->CreateRandomGirl(18 + (max(0, g_Dice % 40 - 10)), true, false,
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
        g_Game->RunEvent(EDefaultEvent::CUSTOMER_NOPAY);    // if not taken as a slave, use the old code
    }
}

void cJobManager::register_job(std::unique_ptr<IGenericJob> job) {
    m_OOPJobs[job->job()] = std::move(job);
}

bool cJobManager::job_filter(int Filter, JOBS job) const {
    auto& filter = JobFilters[Filter];
    return std::count(begin(filter.Contents), end(filter.Contents), job) > 0;
}
