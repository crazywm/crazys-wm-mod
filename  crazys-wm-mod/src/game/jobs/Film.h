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

#ifndef WM_JOBS_FILM_H
#define WM_JOBS_FILM_H

#include "GenericJob.h"
#include <sstream>
#include <utility>
#include <vector>
#include <memory>
#include "cRng.h"
#include "Constants.h"

struct FilmJobData {
    enum SceneType { NICE=0, NORMAL=1, EVIL=2 };
    Image_Types  Image;             //!< The image type used in the job event.
    Action_Types Action;            //!< Secondary action (besides WORKMOVIE) that can gain enjoyment. Set to NUM_ACTIONTYPES to disable
    SKILLS       TrainSkill;        //!< Skill that gets increased, alongside SKILL_PERFORMANCE
    int          Wages;             //!< Default wages for that job
    int          Bonus;             //!< Default bonus of the scene
    SceneType    Type;              //!< The type of the scene. NICE scenes give bonuses to the girl, evil ones give mali
    SKILLS       SceneSkill;        //!< Skill that influences scene quality
    const char*  SceneName;         //!< Display name for a scene of this type.
    const char*  MsgWork;           //!< Default message for doing the work
    const char*  MsgRefuse;         //!< Default message refusing to work
    std::vector<sTraitChange> TraitChanges;
};

struct SceneResult {
    int enjoy       = 0;
    int bonus       = 0;
    int performance = 0;
    int wages       = 0;
};

class GenericFilmJob : public cBasicJob {
public:
    explicit GenericFilmJob(JOBS id, FilmJobData data) : cBasicJob(id), m_FilmData(std::move(data)) {}

    bool DoWork(sGirl& girl, bool is_night) final;
    bool WorkFilm(sGirl& girl);

protected:
    virtual void Reset();
    virtual void DoScene(sGirl& girl) = 0;
    virtual void GainTraits(sGirl& girl, int performance) const {};
    virtual bool CheckCanWork(sGirl& girl);
    virtual bool CheckRefuseWork(sGirl& girl);

    // some helpers for common code
    void PerformanceToEnjoyment(const char* good_message, const char* neutral_message, const char* bad_message);
    void PrintPerfSceneEval();
    double JobPerformanceCalculation(const sGirl& girl, bool estimate, std::initializer_list<STATS> primary,
            std::initializer_list<SKILLS> secondary, const char* modifier) const;

    // common data
    std::stringstream ss;
    SceneResult result;

    FilmJobData m_FilmData;

    // common trait changes
    static constexpr sTraitChange GainPornStar = {true, "Porn Star", 80, ACTION_WORKMOVIE, "She has performed in enough sex scenes that she has become a well known Porn Star."};
    static constexpr sTraitChange GainFaker = {true, "Fake Orgasm Expert", 50, ACTION_SEX, "She has become quite the faker."};
    static constexpr sTraitChange GainSlut = {true, "Slut", 80, ACTION_SEX, "${name} has turned into quite a slut.", EVENT_WARNING};
    static constexpr sTraitChange GainMasochist = {true, "Masochist", 65, ACTION_SEX, "${name} has turned into a Masochist from filming so many BDSM scenes."};

private:
    void NiceMovieGirlUpdate(sGirl& girl) const;
    void EvilMovieGirlUpdate(sGirl& girl) const;
};


#endif //WM_JOBS_FILM_H
