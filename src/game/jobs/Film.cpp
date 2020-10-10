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

#include "cJobManager.h"
#include "cGirls.h"
#include "character/sGirl.h"
#include "character/cPlayer.h"
#include "buildings/cMovieStudio.h"
#include <sstream>
#include "CLog.h"
#include "Game.hpp"
#include "Film.h"
#include "Films.h"
#include "character/predicates.h"


bool GenericFilmJob::DoWork(sGirl& girl, bool is_night) {
    Reset();
    return WorkFilm(girl);
}

bool GenericFilmJob::WorkFilm(sGirl& girl) {
    auto brothel = dynamic_cast<sMovieStudio*>(girl.m_Building);
    if(!brothel) {
        g_LogFile.error("jobs", girl.FullName(), " was not at the movie studio when doing movie job.");
        return false;
    }

    result.wages = m_FilmData.Wages;
    result.bonus = m_FilmData.Bonus;
    result.performance = GetPerformance(girl, false);

    ss << "\n ";

    cGirls::UnequipCombat(girl);

    DoScene(girl);

    // remaining modifiers are in the AddScene function --PP
    // `J` do job based modifiers
    result.bonus += girl.get_skill(m_FilmData.SceneSkill) / 5;
    int finalqual = brothel->AddScene(girl, job(), result.bonus, m_FilmData.Type, m_FilmData.SceneName);
    ss << "Her scene is valued at: " << finalqual << " gold.\n";

    girl.AddMessage(ss.str(), m_FilmData.Image, EVENT_DAYSHIFT);

    result.wages += finalqual * 2;
    // You own her so you don't have to pay her.
    if(girl.is_unpaid())
    {
        result.wages = 0;
    }

    // no tips at the movie set
    girl.m_Tips = 0;
    girl.m_Pay = std::max(0, result.wages);

    // Improve stats
    int xp = 10, skill = 3;

    if (girl.has_active_trait("Quick Learner"))        { skill += 1; xp += 3; }
    else if (girl.has_active_trait("Slow Learner"))    { skill -= 1; xp -= 3; }

    girl.exp(xp);
    girl.performance(uniform(0, skill));
    girl.upd_skill(m_FilmData.TrainSkill, uniform(0, skill + 1));

    if(m_FilmData.Action != Action_Types::NUM_ACTIONTYPES)
        girl.upd_Enjoyment(m_FilmData.Action, result.enjoy);

    girl.upd_Enjoyment(ACTION_WORKMOVIE, result.enjoy);

    // gain simple traits
    gain_traits(girl);

    if(m_FilmData.Type == FilmJobData::EVIL) {
        EvilMovieGirlUpdate(girl);
    } else if (m_FilmData.Type == FilmJobData::NICE) {
        NiceMovieGirlUpdate(girl);
    }

    // customized trait gains
    GainTraits(girl, result.performance);

    return false;
}

bool GenericFilmJob::CheckCanWork(sGirl& girl) {
    auto brothel = girl.m_Building;
    // No film crew.. then go home    // `J` this will be taken care of in building flow, leaving it in for now
    if (brothel->num_girls_on_job(JOB_CAMERAMAGE, SHIFT_NIGHT) == 0 || brothel->num_girls_on_job(JOB_CRYSTALPURIFIER, SHIFT_NIGHT) == 0)
    {
        girl.AddMessage("There was no crew to film the scene, so she took the day off", IMGTYPE_PROFILE, EVENT_NOWORK);
        return false;
    }
    return true;
}

bool GenericFilmJob::CheckRefuseWork(sGirl& girl) {
    if (girl.disobey_check(ACTION_WORKMOVIE, job()))
    {
        ss << get_text("refuse");
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    } else {
        ss << get_text("work") << "\n ";
        return false;
    }
}

void GenericFilmJob::EvilMovieGirlUpdate(sGirl& girl) const {
    //BONUS - evil jobs damage her body, break her spirit and make her hate you

    int MrEvil = uniform(0, 8), MrNasty = uniform(0, 8);
    MrEvil = (MrEvil + MrNasty) / 2;                //Should come out around 3 most of the time.

    girl.confidence(-MrEvil);
    girl.spirit(-MrEvil);
    girl.dignity(-MrEvil);
    girl.pclove(-MrEvil);
    girl.pchate(MrEvil);
    girl.pcfear(MrEvil);
    g_Game->player().disposition(-MrEvil);
}


void GenericFilmJob::NiceMovieGirlUpdate(sGirl& girl) const {
    //BONUS - Nice jobs show off her assets and make her happy, increasing fame and love.

    int MrNiceGuy = uniform(0, 6), MrFair = uniform(0, 6);
    MrNiceGuy = (MrNiceGuy + MrFair) / 3;                //Should come out around 1-2 most of the time.

    girl.happiness(MrNiceGuy);
    girl.fame(MrNiceGuy);
    girl.pclove(MrNiceGuy);
    girl.pchate(-MrNiceGuy);
    girl.pcfear(-MrNiceGuy);
    g_Game->player().disposition(MrNiceGuy);

}

void GenericFilmJob::Reset() {
    result = SceneResult();
}

struct SimpleFilmJob : public GenericFilmJob
{
    SimpleFilmJob(JOBS job, const char* xml, FilmJobData data);
    void DoScene(sGirl& girl) override;

    virtual int handle_events(sGirl& girl) { return 0; }
    virtual void narrate(const sGirl& girl, int roll) = 0;
    double GetPerformance(const sGirl& girl, bool estimate) const override { return 0; }
};

SimpleFilmJob::SimpleFilmJob(JOBS id, const char* xml, FilmJobData data) :
    GenericFilmJob(id, data)
    {
    load_from_xml(xml);
}

void SimpleFilmJob::DoScene(sGirl& girl) {
    int roll = d100();
    if (roll <= 10) { result.enjoy -= uniform(1, 4);    }
    else if (roll >= 90) { result.enjoy += uniform(1, 4); }
    else /*            */{ result.enjoy += uniform(0, 2); }

    narrate(girl, roll);
    ss << "\n \n";
    result.bonus = result.enjoy;
    handle_events(girl);
}

struct FilmTitty : public SimpleFilmJob
{
    FilmTitty() : SimpleFilmJob(JOB_FILMTITTY, "FilmTitty.xml", {
            IMGTYPE_TITTY, ACTION_SEX, SKILL_TITTYSEX, 50, -5,
            FilmJobData::NORMAL, SKILL_TITTYSEX, "Titty"
    })
    {
    };

    void narrate(const sGirl& girl, int roll) override {
        if (roll <= 10) { ss << "She used her breasts on his cock, but didn't like it."; }
        else if (roll >= 90) { ss << "She loved having his cock slide between her breasts."; }
        else { ss << "She had a pleasant day using her tits to get her co-star off."; }
    }
};

struct FilmStrip : public SimpleFilmJob
{
    FilmStrip() : SimpleFilmJob(JOB_FILMSTRIP, "FilmStrip.xml", {
         IMGTYPE_STRIP, ACTION_WORKSTRIP, SKILL_STRIP, 50, 0,
         FilmJobData::NICE, SKILL_STRIP, "Stripping"})
         {};

    void GainTraits(sGirl& girl, int performance) const override {
        if (girl.performance() > 50 && girl.strip() > 50 && chance(25))
        {
            cGirls::PossiblyGainNewTrait(girl, "Sexy Air", 80, ACTION_WORKSTRIP, "${name} has been stripping for so long, when she walks, it seems her clothes just want to fall off.", false);
        }
    }

    void narrate(const sGirl& girl, int roll) override {
        if (roll <= 10) { ss << "She stripped on film today, but didn't like it."; }
        else if (roll >= 90) { ss << "She loved stripping for the camera."; }
        else { ss << "She had a pleasant day stripping today."; }
    }
};

struct FilmLesbian : public SimpleFilmJob
{
    FilmLesbian() : SimpleFilmJob( JOB_FILMLESBIAN, "FilmLes.xml", {
           IMGTYPE_LESBIAN, ACTION_SEX, SKILL_LESBIAN, 50, 10,
           FilmJobData::NORMAL, SKILL_LESBIAN, "Lesbian"
    }) {
    };

    int handle_events(sGirl& girl) override {
        if (is_virgin(girl)) {
            ss << "No 'Man' has been with her, She is still a virgin.\n";
            return 20;
        }
        return 0;
    }

    void narrate(const sGirl& girl, int roll) override {
        if (roll <= 10) { ss << "She didn't like doing it, but but she made the actress come."; }
        else if (roll >= 90) { ss << "She was amazed how good it felt to have her pussy eaten, and gave as good in return."; }
        else { ss << "She had a pleasant day playing with her girlfriend."; }
    }
};

struct FilmHandJob : public SimpleFilmJob
{
    FilmHandJob() : SimpleFilmJob(JOB_FILMHANDJOB, "FilmHand.xml", {
          IMGTYPE_HAND, ACTION_SEX, SKILL_HANDJOB, 50, -5,
          FilmJobData::NORMAL, SKILL_HANDJOB, "Handjob"
    }) {
    }

    void narrate(const sGirl& girl, int roll) override {
        if (roll <= 10) { ss << "She used her hand on his cock, but didn't like it."; }
        else if (roll >= 90) { ss << "She loved having his cock slide between her hands."; }
        else { ss << "She had a pleasant day using her hands to get her co-star off."; }
    }
};

struct FilmFootJob : public SimpleFilmJob
{
    FilmFootJob() : SimpleFilmJob(JOB_FILMFOOTJOB, "FilmFoot.xml", {
          IMGTYPE_FOOT, ACTION_SEX, SKILL_FOOTJOB, 50, -5,
          FilmJobData::NORMAL, SKILL_FOOTJOB, "Footjob",
    }) {
    }

    void narrate(const sGirl& girl, int roll) override {
        if (roll <= 10) { ss << "She used her feet on his cock, but didn't like it."; }
        else if (roll >= 90) { ss << "She loved having his cock slide between her feet."; }
        else { ss << "She had a pleasant day using her feet to get her co-star off."; }
    }
};

struct FilmAnal : public SimpleFilmJob
{
    FilmAnal() : SimpleFilmJob(JOB_FILMANAL, "FilmAnal.xml", {
           IMGTYPE_ANAL, ACTION_SEX, SKILL_ANAL, 50, 5,
           FilmJobData::NORMAL, SKILL_ANAL, "Anal"
    }) {
    }

    int handle_events(sGirl& girl) override {
        if (is_virgin(girl)) {
            ss << "She is a virgin.\n";
            return 20;
        }
        return 0;
    }

    void narrate(const sGirl& girl, int roll) override {
        if (roll <= 10) { ss << "She didn't like having a cock up her ass today."; }
        else if (roll >= 90) { ss << "She loved having her ass pounded today."; }
        else { ss << "She had a pleasant day letting her co-star slip his cock into her butt."; }
    }
};

struct FilmMast : public SimpleFilmJob
{
    FilmMast() : SimpleFilmJob(JOB_FILMMAST, "FilmMast.xml", {
           IMGTYPE_MAST, ACTION_SEX, SKILL_SERVICE, 50, -10,
           FilmJobData::NICE, SKILL_PERFORMANCE, "Masturbation"
    }) {
    }

    int handle_events(sGirl& girl) override {
        if (is_virgin(girl))
        {
            ss << "Her pussy is so tight she is undeniably a virgin.\n";
            return 20;
        }
        else if (chance(33))
        {
            const char* dildos[] = {"Compelling Dildo", "Dreidel Dildo", "Double Dildo"};
            for(auto& dildo : dildos) {
                if(girl.has_item(dildo)) {
                    ss << "${name} finished the scene by pounding herself with her ";
                    ss << dildo << " until she came again.\n";
                    return 20;
                }
            }
        }
        return 0;
    }

    void narrate(const sGirl& girl, int roll) override {
        if (roll <= 10) { ss << "She didn't want to make the film, but the director persuaded her."; }
        else if (roll >= 90) {
            if (girl.has_active_trait("Futanari")) {
                ss << "She cummed hard while playing with her cock!";
            } else {
                ss << "She had intense orgasms while playing with her pussy!";
            }
        } else {
            if (girl.has_active_trait("Futanari")) {
                ss << "She spent the session rubbing her dick until she came.";
            } else {
                ss << "She spent the afternoon fingering herself.";
            }
        }
    }
};

// film registry
void RegisterFilmJobs(cJobManager& mgr) {
    mgr.register_job(std::make_unique<FilmTitty>());
    mgr.register_job(std::make_unique<FilmMast>());
    mgr.register_job(std::make_unique<FilmAnal>());
    mgr.register_job(std::make_unique<FilmFootJob>());
    mgr.register_job(std::make_unique<FilmHandJob>());
    mgr.register_job(std::make_unique<FilmLesbian>());
    mgr.register_job(std::make_unique<FilmStrip>());
    mgr.register_job(std::make_unique<FilmAction>());
    mgr.register_job(std::make_unique<FilmBeast>());
    mgr.register_job(std::make_unique<FilmBdsm>());
    mgr.register_job(std::make_unique<FilmBuk>());
    mgr.register_job(std::make_unique<FilmChef>());
    mgr.register_job(std::make_unique<FilmGroup>());
    mgr.register_job(std::make_unique<FilmMusic>());
    mgr.register_job(std::make_unique<FilmOral>());
    mgr.register_job(std::make_unique<FilmPubBDSM>());
    mgr.register_job(std::make_unique<FilmSex>());
    mgr.register_job(std::make_unique<FilmTease>());
    mgr.register_job(std::make_unique<FilmThroat>());
}

void GenericFilmJob::PerformanceToEnjoyment(const char* good_message, const char* neutral_message, const char* bad_message) {
    //Enjoyed? If she performed well, she'd should have enjoyed it.
    if (result.performance >= 200)
    {
        result.enjoy += uniform(1, 4);
        ss << good_message;
    }
    else if (result.performance >= 100)
    {
        result.enjoy += uniform(0, 2);
        ss << neutral_message;
    }
    else
    {
        result.enjoy -= uniform(2, 5);
        ss << bad_message;
    }

    ss << "\n \n";
}

void GenericFilmJob::PrintPerfSceneEval() {
    if (result.performance >= 350)
    {
        ss << "It was an excellent scene.";
        result.bonus += 12;
    }
    else if (result.performance >= 245)
    {
        ss << "It was mostly an excellent scene.";
        result.bonus += 6;
    }
    else if (result.performance >= 185)
    {
        ss << "Overall, it was an solid scene.";
        result.bonus += 4;
    }
    else if (result.performance >= 145)
    {
        ss << "Overall, it wasn't a bad scene.";
        result.bonus += 2;
    }
    else if (result.performance >= 100)
    {
        ss << "It wasn't a great scene.";
        result.bonus++;
    }
    else
    {
        ss << "It was a poor scene.";
    }
}

sTraitChange GenericFilmJob::GainPornStar = {true, "Porn Star", 80, ACTION_WORKMOVIE, "She has performed in enough sex scenes that she has become a well known Porn Star."};
sTraitChange GenericFilmJob::GainFaker = {true, "Fake Orgasm Expert", 50, ACTION_SEX, "She has become quite the faker."};
sTraitChange GenericFilmJob::GainSlut = {true, "Slut", 80, ACTION_SEX, "${name} has turned into quite a slut.", EVENT_WARNING};
sTraitChange GenericFilmJob::GainMasochist = {true, "Masochist", 65, ACTION_SEX, "${name} has turned into a Masochist from filming so many BDSM scenes."};

IGenericJob::eCheckWorkResult GenericFilmJob::CheckWork(sGirl& girl, bool is_night) {
    if(!CheckCanWork(girl)) {
        return IGenericJob::eCheckWorkResult::IMPOSSIBLE;
    }

    if(CheckRefuseWork(girl)) {
        return IGenericJob::eCheckWorkResult::REFUSES;
    }
    
    return IGenericJob::eCheckWorkResult::ACCEPTS;
}
