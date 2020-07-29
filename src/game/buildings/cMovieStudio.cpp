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

#include <sstream>
#include "queries.hpp"

#include <vector>
#include <algorithm>
#include "cMovieStudio.h"
#include "cGangs.h"
#include "XmlMisc.h"
#include "buildings/cBuildingManager.h"
#include "Game.hpp"
#include "xml/util.h"
#include "interface/constants.h"
#include "scripting/GameEvents.h"

extern cRng             g_Dice;

namespace settings
{
    extern const char* USER_MOVIES_AUTO;
}

// // ----- Strut sMovieStudio Create / destroy
sMovieStudio::sMovieStudio() : IBuilding(BuildingType::STUDIO, "Studio")
{
    m_CurrFilm = nullptr;
    m_MovieRunTime = 0;

    m_RestJob = JOB_FILMFREETIME;
    m_FirstJob = JOB_FILMFREETIME;
    m_LastJob = JOB_FILMRANDOM;
}

sMovieStudio::~sMovieStudio()            // destructor
{
    delete m_CurrFilm;
}

// Run the shifts
void sMovieStudio::UpdateGirls(bool is_night)            // Start_Building_Process_B
{
    if(!is_night)
        return;

    // `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >> cMovieStudio.cpp
    stringstream ss;

    bool camera = false, crystal = false;

    //////////////////////////////////////////////////////
    //  Handle the start of shift stuff for all girls.  //
    //////////////////////////////////////////////////////
    BeginShift();
    bool matron = SetupMatron(SHIFT_NIGHT, "Director");
    HandleRestingGirls(is_night, matron, "Director");

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Pre-Process Crew to make sure there is at least one camera mage and crystal purifier. //
    ///////////////////////////////////////////////////////////////////////////////////////////
    m_Girls->apply([&]( sGirl& current){
        if (current.is_dead() || (current.m_NightJob != JOB_CAMERAMAGE && current.m_NightJob != JOB_CRYSTALPURIFIER))
        {    // skip dead girls and anyone not working the jobs we are processing
            return;
        }
        auto sum = EVENT_SUMMARY;
        ss.str("");

        // will they do their job?
        if (current.disobey_check(ACTION_WORKMOVIE, (JOBS)current.m_NightJob))
        {
            // if there is a Director and a camera or crystal refuses to work the director will not be happy.
            if (matron)
            {
                int test = 0; 
                auto sw = JOB_FILMRANDOM;
                // setting day job as an easy way of tracking what the director makes the refuser do
                current.m_DayJob = current.m_NightJob;
                ss << "Annoyed by her refusal to " << (current.m_DayJob == JOB_CAMERAMAGE ? "film" : "edit");
                ss << " the scenes, the Director ordered ${name} to ";
                // currently this is random but when the morality system gets added, this will be decided by the director's morality
                do // now roll a random number and make sure it is allowed
                {
                    int testa = g_Dice % 12;
                    if (testa == 0 && !is_sex_type_allowed(SKILL_BEASTIALITY))    testa++;
                    if (testa == 1 && !is_sex_type_allowed(SKILL_BDSM))            testa++;
                    if (testa == 2 && !is_sex_type_allowed(SKILL_GROUP))            testa++;
                    if (testa == 3 && !is_sex_type_allowed(SKILL_ANAL))            testa++;
                    if (testa == 4 && !is_sex_type_allowed(SKILL_NORMALSEX))        testa++;
                    if (testa == 5 && !is_sex_type_allowed(SKILL_LESBIAN))        testa++;
                    if (testa == 6 && !is_sex_type_allowed(SKILL_FOOTJOB))        testa++;
                    if (testa == 7 && !is_sex_type_allowed(SKILL_HANDJOB))        testa++;
                    if (testa == 8 && !is_sex_type_allowed(SKILL_ORALSEX))        testa++;
                    if (testa == 9 && !is_sex_type_allowed(SKILL_TITTYSEX))        testa++;
                    if (testa == 10 && !is_sex_type_allowed(SKILL_STRIP))            testa++;
                    switch (testa)    // if that skill is lower than the test number have her do that as punishment
                    {
                        case 0:        if (test <= current.get_skill(SKILL_BEASTIALITY))    { sw = JOB_FILMBEAST;    ss << "have sex with a beast"; }    break;
                        case 1:        if (test <= current.get_skill(SKILL_BDSM))            { sw = JOB_FILMBONDAGE;    ss << "get tied up and whipped"; }    break;
                        case 2:        if (test <= current.get_skill(SKILL_GROUP))        { sw = JOB_FILMGROUP;    ss << "have sex with all the extras"; }    break;
                        case 3:        if (test <= current.get_skill(SKILL_ANAL))            { sw = JOB_FILMANAL;    ss << "have anal sex with one of the extras"; }    break;
                        case 4:        if (test <= current.get_skill(SKILL_NORMALSEX))    { sw = JOB_FILMSEX;        ss << "have sex with one of the extras"; }    break;
                        case 5:        if (test <= current.get_skill(SKILL_LESBIAN))        { sw = JOB_FILMLESBIAN;    ss << "have sex with one of the other girls"; }    break;
                        case 6:        if (test <= current.get_skill(SKILL_FOOTJOB))        { sw = JOB_FILMFOOTJOB;    ss << "use her feet on one of the extras"; }    break;
                        case 7:        if (test <= current.get_skill(SKILL_HANDJOB))        { sw = JOB_FILMHANDJOB;    ss << "jack off one of the extras"; }    break;
                        case 8:        if (test <= current.get_skill(SKILL_ORALSEX))        { sw = JOB_FILMORAL;    ss << "blow one of the extras"; }    break;
                        case 9:        if (test <= current.get_skill(SKILL_TITTYSEX))        { sw = JOB_FILMTITTY;    ss << "let one of the extras fuck her tits"; }    break;
                        case 10:    if (test <= current.get_skill(SKILL_STRIP))        { sw = JOB_FILMSTRIP;    ss << "take her clothes off"; }    break;
                        case 11:    if (test <= current.get_skill(SKILL_PERFORMANCE))    { sw = JOB_FILMMAST;    ss << "get herself off"; }    break;
                        default: break;
                    }
                    test += 5;    // after each roll, lower the test number and roll again
                } while (sw == JOB_FILMRANDOM);    // until something is assigned or the test number gets too low
                current.m_NightJob = sw;    // when done set her job (random if the loop failed)

                ss << " while someone else filmed it.";
            }
            else
            {
                current.m_Refused_To_Work_Night = true;
                m_Fame -= current.fame()/10;
                ss << "${name} refused to work so made no money.";
            }
        }
        else    // if she did not refuse to work...
        {
            if (current.m_NightJob == JOB_CAMERAMAGE) camera = true;
            if (current.m_NightJob == JOB_CRYSTALPURIFIER) crystal = true;
        }
        if (ss.str().length() > 0) current.AddMessage(ss.str(), IMGTYPE_PROFILE, sum);
    });

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // If after the camera and crystal are processed, one of those jobs is vacant, try to have the Director fill it.  //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    m_Girls->apply([&](sGirl& girl) {
        if(!(matron && (!camera || !crystal)))
            return;
        // skip dead girls, the director and anyone resting (the director would have assigned them a new job already if they were able to work)
        if (girl.is_dead() || girl.m_NightJob == m_MatronJob || girl.m_NightJob == m_RestJob ||
            // skip JOB_CAMERAMAGE and JOB_CRYSTALPURIFIER if there is only one of them
            (girl.m_NightJob == JOB_CAMERAMAGE && num_girls_on_job(JOB_CAMERAMAGE, 1) < 2) ||
            (girl.m_NightJob == JOB_CRYSTALPURIFIER && num_girls_on_job(JOB_CRYSTALPURIFIER, 1) < 2) ||
            // skip anyone if they have refused already
            (girl.m_DayJob != m_RestJob))
        {
            return;
        }
        //        Summary Messages
        if (!girl.disobey_check(ACTION_WORKMOVIE, (JOBS)girl.m_NightJob))    // if she did not refuse to work...
        {
            // setting day job as an easy way of tracking what the girl was doing before the director tries to reassign her
            girl.m_DayJob = girl.m_NightJob;
            if (!camera)
            {
                girl.m_NightJob = JOB_CAMERAMAGE;
                camera = true;
            }
            else if (!crystal)
            {
                girl.m_NightJob = JOB_CRYSTALPURIFIER;
                crystal = true;
            }
        }
    });

    // last check, is there a crew to film?
    bool readytofilm = (camera && crystal && matron);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // If the filming can not procede even after trying to fill the jobs (or there is no Director to fill the jobs)  //
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if(!readytofilm) {
        m_Girls->apply([this](sGirl& girl) {
            if (girl.is_dead() || girl.m_NightJob == m_RestJob || girl.m_NightJob == m_MatronJob) {    // skip dead girls, resting girls and the director (if there is one)
                return;
            }
            if (girl.m_NightJob == JOB_STAGEHAND || girl.m_NightJob == JOB_PROMOTER) { // these two can still work
                g_Game->job_manager().handle_simple_job(girl, SHIFT_NIGHT);
            } else {
                girl.AddMessage("There was no crew to film the scene, so ${name} took the day off", IMGTYPE_PROFILE, EVENT_NOWORK);
            }
        });
    } else {

        ////////////////////////
        // Process the Crew.  //
        ////////////////////////
        m_Girls->apply([](sGirl& girl) {
            if (girl.is_dead() || (girl.m_NightJob != JOB_PROMOTER && girl.m_NightJob != JOB_STAGEHAND &&
                                       girl.m_NightJob != JOB_FLUFFER && girl.m_NightJob != JOB_CAMERAMAGE &&
                                       girl.m_NightJob != JOB_CRYSTALPURIFIER)
                // skip dead girls and anyone not working the jobs we are processing
                || (girl.m_NightJob == JOB_CAMERAMAGE && girl.m_Refused_To_Work_Night)
                // and skip any camera or crystal who refused to work in the first check and were not reassigned
                || (girl.m_NightJob == JOB_CRYSTALPURIFIER && girl.m_Refused_To_Work_Night)) {
                return;
            }

            g_Game->job_manager().handle_simple_job(girl, SHIFT_NIGHT);
        });


        /////////////////////
        // Process Stars.  //
        /////////////////////
        m_Girls->apply([&](sGirl& girl) {
            auto sw = girl.m_NightJob;
            if (girl.is_dead() || sw == m_RestJob || sw == JOB_FLUFFER || sw == JOB_CAMERAMAGE ||
                sw == JOB_CRYSTALPURIFIER || sw == JOB_DIRECTOR || sw == JOB_PROMOTER ||
                sw == JOB_STAGEHAND) {    // skip dead girls and already processed jobs
                return;
            }
            g_Game->job_manager().handle_simple_job(girl, is_night);
        });
    }


    EndShift("Director", true, matron);
}

void sMovieStudio::auto_assign_job(sGirl& target, std::stringstream& message, bool is_night)
{
    std::stringstream& ss = message;
    ss << "The Director assigns " << target.FullName() << " to ";
    bool assign_actress = false;
    target.m_DayJob = m_RestJob;

    // first, if there are not enough girls in the studio to film a scene, put them to work at something else.
    if (num_girls() < 4)    // Director plus only 1 or 2 others
    {
        // if there are movies being sold and noone promoting them, assign a promoter.
        if (!m_Movies.empty() && target.is_free()
            && num_girls_on_job(JOB_PROMOTER, 1) < 1)
        {
            target.m_NightJob = JOB_PROMOTER;
            ss << "promote the movies being sold.";
        }
        else // otherwise assign her to clean
        {
            target.m_NightJob = JOB_STAGEHAND;
            ss << "clean the building and take care of the equipment.";
        }
    }
        // second, make sure there is at least 1 camera, 1 crystal and 1 actress
    else if (num_girls_on_job(JOB_CAMERAMAGE, 1) < 1)
    {
        target.m_NightJob = JOB_CAMERAMAGE;
        ss << "film the scenes.";
    }
    else if (num_girls_on_job(JOB_CRYSTALPURIFIER, 1) < 1)
    {
        target.m_NightJob = JOB_CRYSTALPURIFIER;
        ss << "clean up the filmed scenes.";
    }
    else if (Num_Actress(*this) < 1)
    {
        assign_actress = true;
    }

        // if there are a lot of girls in the studio, assign more to camera and crystal
    else if (num_girls_on_job(JOB_CAMERAMAGE, 1) < (num_girls() / 20) + 1)
    {
        target.m_NightJob = JOB_CAMERAMAGE;
        ss << "film the scenes.";
    }
    else if (num_girls_on_job(JOB_CRYSTALPURIFIER, 1) < (num_girls() / 20) + 1)
    {
        target.m_NightJob = JOB_CRYSTALPURIFIER;
        ss << "clean up the filmed scenes.";
    }
        // if there are more than 20 girls and no promoter, assign one
    else if (num_girls_on_job(JOB_PROMOTER, 1) < 1 && target.is_free() && num_girls() > 20)
    {
        target.m_NightJob = JOB_PROMOTER;
        ss << "advertise the movies.";
    }
        // assign at least 1 stagehand if there are 10 or more girls and 1 more for every 20 after that
    else if (num_girls() >= 10 && num_girls_on_job(JOB_STAGEHAND, 1) < (num_girls() / 20) + 1)
    {
        target.m_NightJob = JOB_STAGEHAND;
        ss << "setup equipment and keep the studio clean.";
    }
        // assign a fluffer if there are more than 20 and 1 more for every 20 after that
    else if (num_girls_on_job(JOB_FLUFFER, 1) < (num_girls() / 20))
    {
        target.m_NightJob = JOB_FLUFFER;
        ss << "keep the porn stars aroused.";
    }
    else assign_actress = true;

    if (assign_actress)        // everyone else gets assigned to film something they are good at
    {
        JOBS sw = JOB_FILMRANDOM;
        int test = 80;
        do // now roll a random number and if that skill is higher than the test number set that as her job
        {
            int testa = g_Dice % 12;
            if (testa == 0 && !is_sex_type_allowed(SKILL_BEASTIALITY))    testa++;
            if (testa == 1 && !is_sex_type_allowed(SKILL_BDSM))            testa++;
            if (testa == 2 && !is_sex_type_allowed(SKILL_GROUP))        testa++;
            if (testa == 3 && !is_sex_type_allowed(SKILL_ANAL))            testa++;
            if (testa == 4 && !is_sex_type_allowed(SKILL_NORMALSEX))    testa++;
            if (testa == 5 && !is_sex_type_allowed(SKILL_LESBIAN))        testa++;
            if (testa == 6 && !is_sex_type_allowed(SKILL_FOOTJOB))        testa++;
            if (testa == 7 && !is_sex_type_allowed(SKILL_HANDJOB))        testa++;
            if (testa == 8 && !is_sex_type_allowed(SKILL_ORALSEX))        testa++;
            if (testa == 9 && !is_sex_type_allowed(SKILL_TITTYSEX))        testa++;
            if (testa == 10 && !is_sex_type_allowed(SKILL_STRIP))        testa++;
            switch (testa)
            {
            case 0:        if (test <= target.get_skill(SKILL_BEASTIALITY))    { sw = JOB_FILMBEAST;    ss << "perform in bestiality scenes."; }    break;
            case 1:        if (test <= target.get_skill(SKILL_BDSM))            { sw = JOB_FILMBONDAGE;    ss << "perform in bondage scenes."; }        break;
            case 2:        if (test <= target.get_skill(SKILL_GROUP))        { sw = JOB_FILMGROUP;    ss << "perform in group sex scenes."; }        break;
            case 3:        if (test <= target.get_skill(SKILL_ANAL))            { sw = JOB_FILMANAL;    ss << "perform in anal scenes."; }            break;
            case 4:        if (test <= target.get_skill(SKILL_NORMALSEX))    { sw = JOB_FILMSEX;        ss << "perform in normal sex scenes."; }    break;
            case 5:        if (test <= target.get_skill(SKILL_LESBIAN))        { sw = JOB_FILMLESBIAN;    ss << "perform in lesbian scenes."; }        break;
            case 6:        if (test <= target.get_skill(SKILL_FOOTJOB))        { sw = JOB_FILMFOOTJOB;    ss << "perform in foot job scenes."; }        break;
            case 7:        if (test <= target.get_skill(SKILL_HANDJOB))        { sw = JOB_FILMHANDJOB;    ss << "perform in hand job scenes."; }        break;
            case 8:        if (test <= target.get_skill(SKILL_ORALSEX))        { sw = JOB_FILMORAL;    ss << "perform in oral sex scenes."; }        break;
            case 9:        if (test <= target.get_skill(SKILL_TITTYSEX))        { sw = JOB_FILMTITTY;    ss << "perform in titty fuck scenes."; }    break;
            case 10:    if (test <= target.get_skill(SKILL_STRIP))        { sw = JOB_FILMSTRIP;    ss << "perform in strip tease scenes."; }    break;
            case 11:    if (test <= target.get_skill(SKILL_PERFORMANCE))    { sw = JOB_FILMMAST;    ss << "perform in masturbation scenes."; }    break;
            default: break;
            }
            test -= 5;    // after each roll, lower the test number and roll again
        } while (sw == JOB_FILMRANDOM);    // until something is assigned or the test number gets too low
        target.m_NightJob = sw;    // when done set her job (random if the loop failed)
    }
}

void sMovieStudio::load_xml(const tinyxml2::XMLElement& root)
{
    IBuilding::load_xml(root);

    root.QueryIntAttribute("MovieRunTime", &m_MovieRunTime);

    auto* pMovies = root.FirstChildElement("Movies");
    if (pMovies)
    {
        for (auto* pMovie = pMovies->FirstChildElement("Movie");
             pMovie != nullptr;
             pMovie = pMovie->NextSiblingElement("Movie"))
        {
            string Name        = (pMovie->Attribute("Name") ? pMovie->Attribute("Name") : "");
            string Director = (pMovie->Attribute("Director") ? pMovie->Attribute("Director") : "");
            string Cast        = (pMovie->Attribute("Cast") ? pMovie->Attribute("Cast") : "");
            string Crew        = (pMovie->Attribute("Crew") ? pMovie->Attribute("Crew") : "");
            int init_quality = 0;
            int quality = 0;
            int promo_quality = 0;
            int money_made = 0;
            int runweeks = -1;
            pMovie->QueryAttribute("Init_Qual", &init_quality);
            pMovie->QueryAttribute("Qual", &quality);
            pMovie->QueryAttribute("Promo_Qual", &promo_quality);
            pMovie->QueryAttribute("Money_Made", &money_made);
            pMovie->QueryAttribute("RunWeeks", &runweeks);
            if (runweeks < 0)
            {
                runweeks = m_MovieRunTime - m_Movies.size();
            }
            //when you create a new movie, you set m_Quality to quality*0.5
            //but you directly save m_Quality, so this undoes the division
            // --PP Changed quality to be equal instead of half, to increase movie value.
            // quality *= 2;
            NewMovie(Name, Director, Cast, Crew, init_quality, quality, promo_quality, money_made, runweeks);

        }
    }
    // `J` load scenes added for .06.02.55
    auto* pScenes = root.FirstChildElement("Scenes");
    if (pScenes)
    {
        for (auto* pScene = pScenes->FirstChildElement("Scene");
             pScene != nullptr;
             pScene = pScene->NextSiblingElement("Scene"))
        {
            int scenenum; int mscenenum; int job;
            int init_quality = 0; int quality = 0; int promo_quality = 0; int money_made = 0; int runweeks = -1;

            pScene->QueryIntAttribute("SceneNumber", &scenenum);
            if (pScene->Attribute("MovieSceneNumber"))
            { pScene->QueryIntAttribute("MovieSceneNumber", &mscenenum); }
            else { mscenenum = -1; }
            string name = (pScene->Attribute("Name") ? pScene->Attribute("Name") : "");
            string actress = (pScene->Attribute("Actress") ? pScene->Attribute("Actress") : "");
            string director = (pScene->Attribute("Director") ? pScene->Attribute("Director") : "");
            string cm = (pScene->Attribute("CM") ? pScene->Attribute("CM") : "");
            string cp = (pScene->Attribute("CP") ? pScene->Attribute("CP") : "");
            job = (pScene->Attribute("Job") ? sGirl::lookup_jobs_code(pScene->Attribute("Job")) : JOB_FILMRANDOM);
            pScene->QueryAttribute("Init_Quality", &init_quality);
            pScene->QueryAttribute("Quality", &quality);
            pScene->QueryAttribute("Promo_Quality", &promo_quality);
            pScene->QueryAttribute("Money_Made", &money_made);
            pScene->QueryAttribute("RunWeeks", &runweeks);
            LoadScene(scenenum, name, actress, director, job, init_quality, quality, promo_quality, money_made, runweeks, mscenenum, cm, cp);
        }
    }
}

void sMovieStudio::save_additional_xml(tinyxml2::XMLElement& root) const
{
    root.SetAttribute("MovieRunTime", m_MovieRunTime);

    auto& elMovies = PushNewElement(root, "Movies");
    for(auto& movie: m_Movies)
    {
        auto& elMovie = PushNewElement(elMovies, "Movie");
        elMovie.SetAttribute("Name", movie.m_Name.c_str());
        elMovie.SetAttribute("Director", movie.m_Director.c_str());
        elMovie.SetAttribute("Cast", movie.m_Cast.c_str());
        elMovie.SetAttribute("Crew", movie.m_Crew.c_str());
        elMovie.SetAttribute("Init_Qual", movie.m_Init_Quality);
        elMovie.SetAttribute("Qual", movie.m_Quality);
        elMovie.SetAttribute("Promo_Qual", movie.m_Promo_Quality);
        elMovie.SetAttribute("Money_Made", movie.m_Money_Made);
        elMovie.SetAttribute("RunWeeks", movie.m_RunWeeks);
    }
    // `J` Save Scenes added for .06.02.55
    auto& elScenes = PushNewElement(root, "Scenes");
    for (auto& scene : m_availableScenes)
    {
        auto& elScene = PushNewElement(elScenes, "Scene");
        elScene.SetAttribute("SceneNumber", scene->m_SceneNum);
        elScene.SetAttribute("Name", scene->m_Name.c_str());
        elScene.SetAttribute("Actress", scene->m_Actress.c_str());
        elScene.SetAttribute("Director", scene->m_Director.c_str());
        elScene.SetAttribute("CM", scene->m_CM.c_str());
        elScene.SetAttribute("CP", scene->m_CP.c_str());
        elScene.SetAttribute("Job", g_Game->job_manager().JobData[scene->m_Job].brief.c_str());
        elScene.SetAttribute("Init_Quality", scene->m_Init_Quality);
        elScene.SetAttribute("Quality", scene->m_Quality);
        elScene.SetAttribute("Promo_Quality", scene->m_Promo_Quality);
        elScene.SetAttribute("Money_Made", scene->m_Money_Made);
        elScene.SetAttribute("RunWeeks", scene->m_RunWeeks);
    }
    for (auto& scene : m_movieScenes)
    {
        auto& elScene = PushNewElement(elScenes, "Scene");
        elScene.SetAttribute("SceneNumber", scene->m_SceneNum);
        elScene.SetAttribute("Name", scene->m_Name.c_str());
        elScene.SetAttribute("Actress", scene->m_Actress.c_str());
        elScene.SetAttribute("Director", scene->m_Director.c_str());
        elScene.SetAttribute("CM", scene->m_CM.c_str());
        elScene.SetAttribute("CP", scene->m_CP.c_str());
        elScene.SetAttribute("Job", g_Game->job_manager().JobData[scene->m_Job].brief.c_str());
        elScene.SetAttribute("Init_Quality", scene->m_Init_Quality);
        elScene.SetAttribute("Quality", scene->m_Quality);
        elScene.SetAttribute("Promo_Quality", scene->m_Promo_Quality);
        elScene.SetAttribute("Money_Made", scene->m_Money_Made);
        elScene.SetAttribute("RunWeeks", scene->m_RunWeeks);
    }
}

void sMovieStudio::Update()
{
    // Cannot use UpdateBase here currently, as this expects two shifts.
    stringstream ss;
    string girlName;

    m_Finance.zero();
    m_AntiPregUsed = 0;

    // `J` autocreatemovies added for .06.02.57
    if (GetNumScenes() < 10){}        // don't worry about it if there are less than 10 scenes
    // TODO decide whether this belongs to the settings, or whether we want to handle this as part of the MovieStudio
    else if (g_Game->settings().get_bool(settings::USER_MOVIES_AUTO))    { ReleaseCurrentMovie(true); }
    else if (GetNumScenes() > 0)
    {
        ss.str("");    ss << "You have " << GetNumScenes() << " unused scenes in the Movie Studio ready to be put into movies.";
        g_Game->push_message(ss.str(), COLOR_GREEN);
    }

    BeginWeek();

    m_FlufferQuality = m_CameraQuality = m_PurifierQaulity = m_DirectorQuality = 0;

    UpdateGirls(true);        // Run the Nighty Shift

    // Update movies currently being sold
    if (!m_Movies.empty())
    {
        long income = 0;
        double bonusperc = m_PromoterBonus / 100;
        m_MovieRunTime++;    //    track how long the studio has been putting out movies

        for(auto& movie: m_Movies)
        {
            if (movie.m_RunWeeks > 34)
            {
                movie.m_Promo_Quality = 0;
                movie.m_Quality = 0;
                movie.m_RunWeeks++;
            }
            else
            {
                movie.m_Promo_Quality = (int)(movie.m_Quality * bonusperc);
                movie.m_Money_Made += movie.m_Promo_Quality;
                income += movie.m_Promo_Quality;
                int degrade = (int)((float)(movie.m_Quality)*0.15);
                movie.m_Quality -= degrade;
                movie.m_RunWeeks++;        // `J` each movie will have its own run tracked
            }
        }
        m_Finance.movie_income(income);
        ss.str("");
        ss << "You earn " << income << " gold from movie income, at your " << name();
        g_Game->push_message(ss.str(), COLOR_GREEN);
    }

    g_Game->gold().brothel_accounts(m_Finance, m_id);

    m_Girls->apply([this](sGirl& g){
        cGirls::updateTemp(g);            // update temp stuff
        cGirls::EndDayGirls(*this, g);
    });
    if (m_Filthiness < 0)        m_Filthiness = 0;
    if (m_SecurityLevel < 0)    m_SecurityLevel = 0;
}

// ----- Get / Set
sMovieScene* sMovieStudio::GetScene(int num)
{
    return m_availableScenes.at(num);
}

int sMovieStudio::GetNumScenes() const
{
    return m_availableScenes.size();
}

sMovieScene* sMovieStudio::GetMovieScene(int num)
{
    return m_movieScenes.at(num);
}

int sMovieStudio::GetNumMovieScenes() const
{
    return m_movieScenes.size();
}

vector<int> sMovieStudio::AddSceneToMovie(int num)
{
    if (num >= 0)
    {
        sMovieScene* scene = GetScene(num);
        scene->m_RowM = GetNumMovieScenes();
        scene->m_MovieSceneNum = GetNumMovieScenes() + 1;
        m_availableScenes.erase(m_availableScenes.begin() + num);
        m_movieScenes.push_back(scene);
        SortMovieScenes();
    }
    return{ (num >= GetNumScenes() ? GetNumScenes() - 1 : num), GetNumMovieScenes() - 1 };
}
vector<int> sMovieStudio::RemoveSceneFromMovie(int num)
{
    if (num >= 0)
    {
        sMovieScene* scene = GetMovieScene(num);
        scene->m_RowM = -1;
        scene->m_MovieSceneNum = -1;
        m_movieScenes.erase(m_movieScenes.begin() + num);
        m_availableScenes.push_back(scene);
        SortMovieScenes();
    }
    return { GetNumScenes() - 1, num - 1 };
}
int sMovieStudio::MovieSceneUp(int num)
{
    if (num > 0)
    {
        m_movieScenes[num]->m_RowM--;
        m_movieScenes[num]->m_MovieSceneNum--;
        m_movieScenes[num - 1]->m_RowM++;
        m_movieScenes[num - 1]->m_MovieSceneNum++;
        SortMovieScenes();
    }
    return num - 1;
}
int sMovieStudio::MovieSceneDown(int num)
{
    if (num >= 0 && num < GetNumMovieScenes() - 1)
    {
        m_movieScenes[num]->m_RowM++;
        m_movieScenes[num]->m_MovieSceneNum++;
        m_movieScenes[num + 1]->m_RowM--;
        m_movieScenes[num + 1]->m_MovieSceneNum--;
        SortMovieScenes();
    }
    return num + 1;
}
int sMovieStudio::DeleteScene(int num)
{
    if (num >= 0)
    {
        m_availableScenes.erase(m_availableScenes.begin() + num);
    }
    if (num >= GetNumScenes()) num = GetNumScenes();
    return num;
}
bool SortMovieScene(sMovieScene* a, sMovieScene* b)
{
    return (a->m_RowM < b->m_RowM);
}
void sMovieStudio::SortMovieScenes()
{
    if (GetNumMovieScenes() < 1) { return; }                                        // if there are none we don't need to sort it.
    std::sort(m_movieScenes.begin(), m_movieScenes.end(), SortMovieScene);

    for (int i = 0; i < GetNumMovieScenes(); i++)                                    // we need to go through the scenes and find out what order they are in
    {
        sMovieScene* scene = GetMovieScene(i);
        scene->m_RowM = i;
        scene->m_MovieSceneNum = i + 1;
    }

}


void sMovieScene::OutputSceneRow(vector<string>& Data, const vector<string>& columnNames)
{
    Data.resize(columnNames.size());
    for (unsigned int x = 0; x < columnNames.size(); ++x)
    {
        //for each column, write out the statistic that goes in it
        OutputSceneDetailString(Data[x], columnNames[x]);
    }
}

void sMovieScene::OutputSceneDetailString(string& Data, const string& detailName)
{
    //given a statistic name, set a string to a value that represents that statistic
    static stringstream ss;
    ss.str("");
    /* */if (detailName == "SceneNumber")        { ss << m_SceneNum; }
    else if (detailName == "MovieSceneNumber")    { ss << m_MovieSceneNum; }
    else if (detailName == "Name")                { ss << m_Name; }
    else if (detailName == "Actress")            { ss << m_Actress; }
    else if (detailName == "Director")            { ss << m_Director; }
    else if (detailName == "Camera Mage")        { ss << m_CM; }
    else if (detailName == "CameraMage")        { ss << m_CM; }
    else if (detailName == "Camera_Mage")        { ss << m_CM; }
    else if (detailName == "Camera")            { ss << m_CM; }
    else if (detailName == "CM")                { ss << m_CM; }
    else if (detailName == "Crystal Purifier")    { ss << m_CP; }
    else if (detailName == "CrystalPurifier")    { ss << m_CP; }
    else if (detailName == "Crystal_Purifier")    { ss << m_CP; }
    else if (detailName == "Crystal")            { ss << m_CP; }
    else if (detailName == "CP")                { ss << m_CP; }
    else if (detailName == "Job")                { ss << g_Game->job_manager().JobData[m_Job].name; }
    else if (detailName == "Init_Quality")        { ss << m_Init_Quality; }
    else if (detailName == "Quality")            { ss << m_Quality; }
    else if (detailName == "Promo_Quality")        { ss << m_Promo_Quality; }
    else if (detailName == "Money_Made")        { ss << m_Money_Made; }
    else if (detailName == "RunWeeks")            { ss << m_RunWeeks; }
    else /*                               */    { ss << "Not found"; }
    Data = ss.str();
}

void sMovieStudio::NewMovie(string Name, string Director, string Cast, string Crew, int Init_Quality, int Quality,
                            int Promo_Quality, int Money_Made, int RunWeeks)
{
    m_Movies.emplace_back();
    m_Movies.back().m_Name = Name;
    m_Movies.back().m_Director = Director;
    m_Movies.back().m_Cast = Cast;
    m_Movies.back().m_Crew = Crew;
    m_Movies.back().m_Init_Quality = Init_Quality;
    m_Movies.back().m_Promo_Quality = Promo_Quality;
    m_Movies.back().m_Quality = Quality;
    m_Movies.back().m_Money_Made = Money_Made;
    m_Movies.back().m_RunWeeks = RunWeeks;
}

void sMovieStudio::EndMovie()
{
    if(!m_Movies.empty())
        m_Movies.erase(m_Movies.begin());
}

// ----- Add / remove
int sMovieStudio::AddScene(sGirl& girl, int Job, int Bonus, int jobType, const char* scene_name)
{
    stringstream ss;
    string girlName = girl.FullName();

    auto Director = random_girl_on_job(*girl.m_Building, JOB_DIRECTOR, SHIFT_NIGHT);
    auto CM = random_girl_on_job(*girl.m_Building, JOB_CAMERAMAGE, SHIFT_NIGHT);
    auto CP = random_girl_on_job(*girl.m_Building, JOB_CRYSTALPURIFIER, SHIFT_NIGHT);

    // NOTE i crazy added this to try and improve the movies before it only check for normalsex skill now it should check for each skill type i hope
    // Fixed so it will check for skill type being used --PP
    long quality = Bonus;
    quality += g_Dice % 4 - 1;    // should add a -1 to +3 random element --PP


    // Balance is...
    // Evil jobs should pay better and show higher 'quality' - but damage health, happiness, love and can possibly cause injury/disease
    // nice jobs should pay less, but be safe and boost fame, skills, happiness and health.
    // normal jobs should be normal.
    //
    // `J` When adding new Studio Scenes, search for "J-Add-New-Scenes"  :  found in >> cMovieStudio.cpp > AddScene
    //case JOB_FILMCOMEDY:        jobType = NICE;        quality -= 2;    Skill = SKILL_PERFORMANCE;    ss << "Comedy";            break;
    //case JOB_FILMDRAMA:        jobType = NICE;        quality -= 2;    Skill = SKILL_PERFORMANCE;    ss << "Drama";            break;
    //case JOB_FILMHORROR:        jobType = NORMAL;    quality -= 2;    Skill = SKILL_PERFORMANCE;    ss << "Horror";            break;
    //case JOB_FILMIDOL:        jobType = NICE;        quality -= 5;    Skill = SKILL_PERFORMANCE;    ss << "Idol";            break;
    //case JOB_FILMFUTA:        jobType = NORMAL;    quality += 5;    Skill = SKILL_NORMALSEX;    ss << "Futa";            break;
        //case JOB_FILMDOM:            jobType = EVIL;        quality += 8;    Skill = SKILL_BDSM;            ss << "Dominatrix";        break;

    ss << scene_name << " scene by " << girlName;


    // `J` When adding new traits, search for "J-Add-New-Traits"  :  found in >> cMovieStudioManager::AddScene

    //CRAZY added this to have traits play a bigger part in the movies
    quality += girl.get_trait_modifier("movie.quality.general");

    if(Job == JOB_FILMTITTY) {
        quality += girl.get_trait_modifier("movie.quality.titty");
    } else if(Job == JOB_FILMANAL) {
        quality += girl.get_trait_modifier("movie.quality.anal");
    } else if(Job == JOB_FILMBONDAGE) {
        quality += girl.get_trait_modifier("movie.quality.bondage");
    } else if(Job == JOB_FILMSEX) {
        quality += girl.get_trait_modifier("movie.quality.sex");
    } else if(Job == JOB_FILMBEAST) {
        quality += girl.get_trait_modifier("movie.quality.beast");
    } else if(Job == JOB_FILMGROUP) {
        quality += girl.get_trait_modifier("movie.quality.group");
    } else if(Job == JOB_FILMLESBIAN) {
        quality += girl.get_trait_modifier("movie.quality.lesbian");
    } else if(Job == JOB_FILMSTRIP) {
        quality += girl.get_trait_modifier("movie.quality.strip");
    } else if(Job == JOB_FILMORAL || Job == JOB_FILMFACEFUCK) {
        quality += girl.get_trait_modifier("movie.quality.oral");
    }

    if (girl.has_active_trait("Lesbian"))
    {
        //a lesbian would be more into it and give a better show I would think CRAZY
        // `J` and she would be less into doing it with a guy
        if (Job == JOB_FILMFOOTJOB)                    quality -= 2;
        if (Job == JOB_FILMHANDJOB)                    quality -= 2;
        if (Job == JOB_FILMMAST)                    quality -= 1;
    }

    //CRAZY added this better looking girls should make better quality movies
    // Changed to work with new job revision --PP
    quality += girl.performance() / 10;
    quality += (girl.charisma() - 50) / 10;
    quality += (girl.beauty() - 50) / 10;
    quality += girl.fame();
    quality += girl.level();

    int performance = 0;

    if (jobType <= 1)
    {
        performance = quality;
    }
    else
    {
        performance = quality;            //Performance rate set
        quality += (quality / 2);        //Quality increased 50% - FIRST GUESS - MAY NEED BALANCING!!
    }

    // Add bonus for Fluffer, CameraMage and CrystalPurifier --PP
    if (Job != JOB_FILMMAST && Job != JOB_FILMLESBIAN && Job != JOB_FILMSTRIP)    // No fluffers needed --PP no need for strip either CRAZY
    quality += m_FlufferQuality;

    quality += m_CameraQuality;
    quality += m_PurifierQaulity;
    quality += m_DirectorQuality;

    quality += m_StagehandQuality;


    LoadScene(
         /* int m_SceneNum            */    ++m_TotalScenesFilmed
        ,/* string m_Name             */    ss.str()
        ,/* string m_Actress         */    girl.FullName()
        ,/* string m_Director         */    Director->FullName()
        ,/* int m_Job                 */    Job
        ,/* long m_Init_Quality     */    quality
        ,/* long m_Quality             */    quality
        ,/* long m_Promo_Quality     */    0
        ,/* long m_Money_Made         */    0
        ,/* long m_RunWeeks         */    0
        ,/* int m_MovieSceneNum     */    -1
        ,/* string m_CM             */    CM->FullName()
        ,/* string m_CP                */    CP->FullName()
        );
    return performance;
}
void sMovieStudio::LoadScene(int m_SceneNum, string m_Name, string m_Actress, string m_Director, int m_Job, long m_Init_Quality, long m_Quality, long m_Promo_Quality, long m_Money_Made, long m_RunWeeks, int m_MovieSceneNum, string m_CM, string m_CP)
{
    sMovieScene* newScene = new sMovieScene();
    newScene->m_SceneNum = m_SceneNum;
    newScene->m_MovieSceneNum = m_MovieSceneNum;
    newScene->m_Name = m_Name;
                                   
    newScene->m_Actress = m_Actress;
    newScene->m_Director = m_Director;
    newScene->m_CM = m_CM;
    newScene->m_CP = m_CP;
    newScene->m_Job = m_Job;
    newScene->m_Init_Quality = m_Init_Quality;
    newScene->m_Quality = m_Quality;
    newScene->m_Promo_Quality = m_Promo_Quality;
    newScene->m_Money_Made = m_Money_Made;
    newScene->m_RunWeeks = m_RunWeeks;
    m_availableScenes.push_back(newScene);
}

long sMovieStudio::calc_movie_quality(bool autoreleased)
{
    stringstream ss;
    long quality = 0;


    if (autoreleased)
    {
        int numscenes = m_availableScenes.size();
        int makerqual = g_Dice % 10 + 1;
        int thrownout = 0;
        const sGirl* maker = m_Girls->get_first_girl(HasJob(JOB_DIRECTOR, SHIFT_NIGHT));
        if (!maker)    { makerqual = g_Dice % 5 + 1;    maker = m_Girls->get_random_girl(HasJob(JOB_PROMOTER, SHIFT_NIGHT)); }
        if (!maker)    { makerqual = g_Dice % 3;        maker = random_girl_on_job(*this, JOB_CRYSTALPURIFIER, SHIFT_NIGHT); }
        if (!maker)    { makerqual = g_Dice % 3;        maker = random_girl_on_job(*this, JOB_CAMERAMAGE, SHIFT_NIGHT); }

        if (!maker)
        {
            makerqual = 0;
            ss << "A movie was automatically made from the scenes that are ready for use";
        }
        else
        {
            quality += makerqual;
            if (maker->m_NightJob == JOB_DIRECTOR)            ss << "Director ";
            if (maker->m_NightJob == JOB_PROMOTER)            ss << "Promoter ";
            ss << maker->FullName() << " produced a movie from the scenes shot last week";
            for (int i = 0; i < (int)m_availableScenes.size(); i++)
            {
                if (m_availableScenes[i]->m_Quality < 0)
                {
                    m_availableScenes[i]->m_Quality = 0;
                    thrownout++;
                }
            }
            if (thrownout >= numscenes * 8 / 10)
            {
                ss << ".\nThe movie was total crap but " << maker->FullName() << " chopped it up and put it out as a blooper reel";
                numscenes = thrownout + g_Dice%thrownout;
            }
            else if (thrownout>numscenes / 2)
            {
                ss << ".\nMore than half of the scenes were crap but " << maker->FullName() << " chopped them up and added them as a blooper reel at the end of the movie";
                numscenes -= thrownout;
                numscenes += g_Dice%thrownout;
            }
            else if (thrownout > 0)
            {
                ss << ".\nShe threw out ";
                /* */if (thrownout == 1)    ss << "one";
                else if (thrownout == 1)    ss << "two";
                else/*                */    ss << "several";
                ss << " scenes that didn't meet her standards";
                numscenes -= thrownout;
            }
        }
        ss << ".\n \n";
  
        for (int i = 0; i < (int)m_availableScenes.size(); i++)
        {
            quality += m_availableScenes[i]->m_Quality;
        }
        quality += numscenes * 10;
        ss << "This movie will sell at " << quality << " gold, for 35 weeks, but it's value will drop over time.\n \n";
        ss << (num_girls_on_job(JOB_PROMOTER, 0) > 0 ? "Your" : "A");
        ss << " promoter with an advertising budget will help it sell for more.";
    }
    else
    {
        int numscenes = m_movieScenes.size();
        for (int i = 0; i < numscenes; i++)
        {
            quality += m_movieScenes[i]->m_Quality;
        }
        quality += numscenes * 10;
        ss << "This movie will sell at " << quality << " gold, for 35 weeks, but it's value will drop over time.\n \n";
        ss << (num_girls_on_job(JOB_PROMOTER, 0) > 0 ? "Your" : "A");
        ss << " promoter with an advertising budget will help it sell for more.";

    }
    g_Game->push_message(ss.str(), COLOR_BLUE);
    // TODO what was the purpose of: g_InitWin = true;
    return quality;
}

void sMovieStudio::ReleaseCurrentMovie(bool autoreleased, bool save)
{
    string Name;
    string Director = BuildDirectorList(autoreleased,true);
    string Cast = BuildCastList(autoreleased, true);
    string Crew = BuildCrewList(autoreleased, true);
    long init_quality = calc_movie_quality(autoreleased);
    long quality = init_quality;    // calculate movie quality
    long promo_quality = 0;
    long money_made = 0;
    long runweeks = 0;

    if (autoreleased) {    // old code
        m_availableScenes.clear();                    // clear scene list
    }
    else
    {
        m_movieScenes.clear();                        // clear scene list
                                                                                               
    }
    NewMovie(Name, Director, Cast, Crew, init_quality, quality, promo_quality,
             money_made, runweeks);    //add new movie

}
string sMovieStudio::BuildDirectorList(bool autoreleased, bool save)
{
    vector<string> names;
    vector<string> namesB;
    stringstream ss;
    if (autoreleased)
    {
        for (auto & m_availableScene : m_availableScenes)
        {
            
            if (!m_availableScene->m_Director.empty()) names.push_back(m_availableScene->m_Director);
        }
    }
    else
    {
        for (auto & m_movieScene : m_movieScenes)
        {
            if (!m_movieScene->m_Director.empty()) names.push_back(m_movieScene->m_Director);
        }
    }
    if (names.empty()) { return ""; }
    namesB = names;
    std::sort(names.begin(), names.end());
    names.resize(std::distance(names.begin(),std::unique(names.begin(), names.end())));

    if (!save)
    {
        ss << "Director" << (names.size() == 1 ? ":  " : "s:  ");
    }
    for (int i = 0; i < (int)names.size(); i++)
    {
        ss << names[i];
        if ((int)names.size() > 1 && i < (int)names.size() - 1) ss << ", ";
    }

    return ss.str();
}
string sMovieStudio::BuildCastList(bool autoreleased, bool save)
{
    vector<string> names;
    vector<string> namesB;
    stringstream ss;
    if (autoreleased)
    {
        for (auto & scene : m_availableScenes)
        {
            if (!scene->m_Actress.empty()) names.push_back(scene->m_Actress);
        }
    }
    else
    {
        for (auto & scene : m_movieScenes)
        {
            if (!scene->m_Actress.empty()) names.push_back(scene->m_Actress);
        }
    }
    if (names.empty()) { return ""; }

    namesB = names;
    std::sort(names.begin(), names.end());
    names.resize(std::distance(names.begin(), std::unique(names.begin(), names.end())));

    if (!save)
    {
        ss << "Actress" << (names.size() == 1 ? ":  " : "es:  ");
    }
    for (int i = 0; i < (int)names.size(); i++)
    {
        ss << names[i];
        if ((int)names.size() > 1 && i < (int)names.size() - 1) ss << ", ";
    }

    return ss.str();
}
string sMovieStudio::BuildCrewList(bool autoreleased, bool save)
{
    vector<string> names;
    vector<string> namesB;
    stringstream ss;
    if (autoreleased)
    {
        for (auto & scene : m_availableScenes)
        {
            if (!scene->m_CM.empty()) names.push_back(scene->m_CM);
            if (!scene->m_CP.empty()) names.push_back(scene->m_CP);
        }
    }
    else
    {
        for (auto & scene : m_movieScenes)
        {
            if (!scene->m_CM.empty()) names.push_back(scene->m_CM);
            if (!scene->m_CP.empty()) names.push_back(scene->m_CP);
        }
    }
    if (names.empty()) { return ""; }

    namesB = names;
    std::sort(names.begin(), names.end());
    names.resize(std::distance(names.begin(), std::unique(names.begin(), names.end())));

    if (!save)
    {
        ss << "Crew:  ";
    }
    for (int i = 0; i < (int)names.size(); i++)
    {
        ss << names[i];
        if ((int)names.size() > 1 && i < (int)names.size() - 1) ss << ", ";
    }

    return ss.str();
}

std::string sMovieStudio::meet_no_luck() const {
    return g_Dice.select_text(
            {
                "Your father once called this 'talent spotting' - and looking these girls over you see no talent for anything.",
                "Married. Married. Bodyguard. Already works for you. Married. Hideous. Not a woman. Married. Escorted. Married...\nMight as well go home, there's nothing happening here.",
                "It's not a bad life, if you can get paid to try pretty girls out before they start filming. But somedays there isn't a single decent prospect in the bunch of them.",
                "All seemed perfect she was pretty really wanting to be an actress...  Then you told her what kinda movies you planned to make and she stormed off cursing at you.",
                "When the weather is bad people just don't show up for this kinda thing.",
                "There's a bit of skirt over there with a lovely figure, and had a face that was pretty, ninety years ago. Over yonder, a sweet young thing but she's got daddy's gold.  Looks like nothing to gain here today. ",
                "There is not much going on here in the studio."
            }
            );
}

std::shared_ptr<sGirl> sMovieStudio::meet_girl() const {
    auto girl = g_Game->GetRandomGirl();
    if(girl) {
        g_Game->girl_pool().AddGirl(girl);
        girl->TriggerEvent(EDefaultEvent::MEET_GIRL_STUDIO);
    }
    return girl;
}
