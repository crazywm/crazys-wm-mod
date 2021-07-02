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
#include "buildings/queries.h"

#include <vector>
#include <utils/string.hpp>
#include "cMovieStudio.h"
#include "cGangs.h"
#include "buildings/cBuildingManager.h"
#include "IGame.h"
#include "xml/util.h"
#include "interface/constants.h"
#include "scripting/GameEvents.h"
#include "cGirls.h"
#include "manager.h"

extern const char* const FluffPointsId = "FluffPoints";
extern const char* const StageHandPtsId = "StageHandPoints";
extern const char* const DirectorInteractionId = "DirectorInteraction";
extern const char* const CamMageInteractionId = "CamMageInteraction";
extern const char* const CrystalPurifierInteractionId = "CrystalPurifierInteraction";
extern cRng             g_Dice;

namespace settings
{
    extern const char* USER_MOVIES_AUTO;
}

// // ----- Strut sMovieStudio Create / destroy
sMovieStudio::sMovieStudio() : IBuilding(BuildingType::STUDIO, "Studio")
{
    m_FirstJob = JOB_EXECUTIVE;
    m_LastJob = JOB_FILMRANDOM;
    m_MatronJob = JOB_EXECUTIVE;
    m_MeetGirlData.Event = EDefaultEvent::MEET_GIRL_STUDIO;
    declare_resource(FluffPointsId);
    declare_resource(StageHandPtsId);
    declare_interaction(DirectorInteractionId);
    declare_interaction(CamMageInteractionId);
    declare_interaction(CrystalPurifierInteractionId);
}

sMovieStudio::~sMovieStudio() = default;

// Run the shifts
void sMovieStudio::UpdateGirls(bool is_night)
{
    if(!is_night)
        return;

    std::stringstream ss;

    //  Handle the start of shift stuff for all girls.  //
    BeginShift(is_night);

    IterateGirls(is_night, {JOB_CAMERAMAGE, JOB_CRYSTALPURIFIER, JOB_PROMOTER, JOB_DIRECTOR, JOB_MARKET_RESEARCH},
                 [&]( sGirl& current){
        g_Game->job_manager().handle_simple_job(current, SHIFT_NIGHT);
    });

    // last check, is there a crew to film?
    bool ready_to_film = HasInteraction(CamMageInteractionId) && HasInteraction(CrystalPurifierInteractionId) && HasInteraction(DirectorInteractionId);
    std::stringstream summary;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // If the filming can not proceed even after trying to fill the jobs (or there is no Director to fill the jobs)  //
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if(!ready_to_film) {
        summary << "No filming took place at the studio today. In order to film a scene, you need at least a camera mage, "
                   "crystal purifier and a director. Your scenes will be better if you also employ sufficiently many stage hands "
                   "and fluffers.";
        m_Girls->apply([this](sGirl& girl) {
            if (girl.is_dead() || girl.m_NightJob == JOB_RESTING || girl.m_NightJob == m_MatronJob ||
                girl.m_NightJob == JOB_PROMOTER || girl.m_NightJob == JOB_MARKET_RESEARCH) {    // skip dead girls, resting girls and the director (if there is one)
                return;
            }
            if (girl.m_NightJob == JOB_STAGEHAND) { // these two can still work
                g_Game->job_manager().handle_simple_job(girl, SHIFT_NIGHT);
            } else {
                girl.AddMessage("There was no crew to film the scene, so ${name} took the day off", IMGTYPE_PROFILE, EVENT_NOWORK);
            }
        });
    } else {
        // Process the Crew.  //
        IterateGirls(is_night, {JOB_STAGEHAND, JOB_FLUFFER}, [](sGirl& girl) {
            g_Game->job_manager().handle_simple_job(girl, SHIFT_NIGHT);
        });

        // Add an event with info
        summary << "You have " << NumInteractors(CrystalPurifierInteractionId) << " crystal purifiers, " << NumInteractors(CamMageInteractionId) << " camera mages ";
        summary << "and " << NumInteractors(DirectorInteractionId) << " working.\n";
        summary << "Your stagehands provide a total of " << GetResourceAmount(StageHandPtsId) << " stagehand points.\n";
        summary << "Your fluffers provide a total of " << GetResourceAmount(FluffPointsId) << " fluffer points.\n";

        int num_scenes_before = g_Game->movie_manager().get_scenes().size();
        // Process Stars.  //
        m_Girls->apply([&](sGirl& girl) {
            auto sw = girl.m_NightJob;
            if (girl.is_dead() || sw == JOB_RESTING || sw == JOB_FLUFFER || sw == JOB_CAMERAMAGE ||
                sw == JOB_CRYSTALPURIFIER || sw == JOB_DIRECTOR || sw == JOB_EXECUTIVE || sw == JOB_PROMOTER || sw == JOB_MARKET_RESEARCH ||
                sw == JOB_STAGEHAND) {    // skip dead girls and already processed jobs
                return;
            }
            g_Game->job_manager().handle_simple_job(girl, is_night);
        });
        int num_scenes_after = g_Game->movie_manager().get_scenes().size();
        summary << "A total of " << num_scenes_after - num_scenes_before << " scenes were filmed at the studio today.\n";
    }

    // Finally, check if we overused our production staff
    check_overuse(CamMageInteractionId,
                  "You need more Camera Mages. You have ${workers} Camera Mages who can film up to "
                  "${possible} scenes each week, but you wanted ${total} scenes this week.");
    check_overuse(CrystalPurifierInteractionId,
                  "You need more Crystal Purifiers. You have ${workers} Crystal Purifiers who can process up to "
                  "${possible} scenes each week, but you wanted ${total} scenes this week.");
    check_overuse(DirectorInteractionId,
                  "You need more Directors. You have ${workers} Directors who can direct up to "
                  "${possible} scenes each week, but you wanted ${total} scenes this week.");

    EndShift(is_night);

    AddMessage(summary.str(), EVENT_SUMMARY);
}

void sMovieStudio::auto_assign_job(sGirl& target, std::stringstream& message, bool is_night)
{
    std::stringstream& ss = message;
    ss << "The Director assigns " << target.FullName() << " to ";
    target.m_DayJob = JOB_RESTING;

    int actresses = GetNumberActresses(*this);
    if (num_girls_on_job(JOB_CAMERAMAGE, 1) * 3 < actresses + 1) {
        target.m_NightJob = JOB_CAMERAMAGE;
        ss << "film the scenes.";
    } else if (num_girls_on_job(JOB_CRYSTALPURIFIER, 1) * 3 < actresses + 1) {
        target.m_NightJob = JOB_CRYSTALPURIFIER;
        ss << "clean up the filmed scenes.";
    } else if (num_girls_on_job(JOB_DIRECTOR, 1) * 3 < actresses + 1) {
        target.m_NightJob = JOB_DIRECTOR;
        ss << "direct the scenes.";
    } else if (m_Filthiness > 10 || num_girls_on_job(JOB_STAGEHAND, 1) * 3 < actresses) {
        target.m_NightJob = JOB_STAGEHAND;
        ss << "setup equipment and keep the studio clean.";
    } else if (num_girls_on_job(JOB_FLUFFER, 1) < actresses / 5) {
        target.m_NightJob = JOB_FLUFFER;
        ss << "keep the porn stars aroused.";
    } else if (g_Game->movie_manager().get_movies().size() > num_girls_on_job(JOB_PROMOTER, 1)) {
        target.m_NightJob = JOB_PROMOTER;
        ss << "promote the movies being sold.";
    } else {
        target.m_NightJob = JOB_FILMRANDOM;
        ss << "work as an actress.";
    }
}

void sMovieStudio::Update()
{
    // Cannot use UpdateBase here currently, as this expects two shifts.
    std::stringstream ss;
    std::string girlName;

    m_Finance.zero();
    m_AntiPregUsed = 0;

    // `J` autocreatemovies added for .06.02.57
    if (GetNumScenes() < 5) {}        // don't worry about it if there are less than 5 scenes
    // TODO decide whether this belongs to the settings, or whether we want to handle this as part of the MovieStudio
    else if (g_Game->settings().get_bool(settings::USER_MOVIES_AUTO)) {
        auto_create_movies();
    }
    else if (GetNumScenes() > 0)
    {
        ss.str("");    ss << "You have " << GetNumScenes() << " unused scenes in the Movie Studio ready to be put into movies.";
        g_Game->push_message(ss.str(), COLOR_GREEN);
    }

    BeginWeek();

    auto income = g_Game->movie_manager().step(*this);
    m_Finance.movie_income(income);

    UpdateGirls(true);        // Run the Nighty Shift

    g_Game->gold().brothel_accounts(m_Finance, m_id);

    m_Girls->apply([this](sGirl& g){
        cGirls::updateTemp(g);            // update temp stuff
        cGirls::EndDayGirls(*this, g);
    });
    if (m_Filthiness < 0)        m_Filthiness = 0;
    if (m_SecurityLevel < 0)    m_SecurityLevel = 0;
}

// ----- Get / Set
int sMovieStudio::GetNumScenes() const
{
    return g_Game->movie_manager().get_scenes().size();
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

void sMovieStudio::auto_create_movies() {
    while(GetNumScenes() >= 5) {
        std::vector<const MovieScene*> active_scenes;
        for (auto& scene : g_Game->movie_manager().get_scenes()) {
            active_scenes.push_back(&scene);
            // Make sure the movies don't exceed the target length
            if(active_scenes.size() == 5)  break;

            // TODO add a creation message
        }
        g_Game->movie_manager().create_movie(active_scenes, g_Game->movie_manager().auto_create_name(active_scenes));
    }
}

void sMovieStudio::check_overuse(const std::string& resource, const std::string& message) {
    int total_scenes = GetInteractionConsumed(resource);
    int possible_scenes = GetInteractionProvided(resource);


    if(total_scenes > possible_scenes) {
        auto lookup = [&](const std::string& key) {
            if(key == "workers") {
                return std::to_string(NumInteractors(resource));
            } else if (key == "possible") {
                return std::to_string(possible_scenes);
            } else if (key == "total") {
                return std::to_string(total_scenes);
            }
            assert(false);
        };
        ;
        AddMessage(interpolate_string(message, lookup, g_Dice),EventType::EVENT_WARNING);
    }
}
