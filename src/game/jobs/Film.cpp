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

GenericFilmJob::GenericFilmJob(JOBS id, FilmJobData data) : cBasicJob(id), m_FilmData(std::move(data))
{
    RegisterVariable("Enjoy", result.enjoy);
}


bool GenericFilmJob::DoWork(sGirl& girl, bool is_night) {
    return WorkFilm(girl);
}

void GenericFilmJob::InitWork() {
    this->cBasicJob::InitWork();
    result = SceneResult();
    result.wages = m_FilmData.Wages;
    result.bonus = m_FilmData.Bonus;
    result.performance = m_Performance;
    Reset();
}

bool GenericFilmJob::WorkFilm(sGirl& girl) {
    auto brothel = dynamic_cast<sMovieStudio*>(girl.m_Building);
    if(!brothel) {
        g_LogFile.error("jobs", girl.FullName(), " was not at the movie studio when doing movie job.");
        return false;
    }

    int roll = d100();
    if (roll <= 10) { result.enjoy -= uniform(1, 4);    }
    else if (roll >= 90) { result.enjoy += uniform(1, 4); }
    else { result.enjoy += uniform(0, 2); }

    if (result.performance >= 200)
    {
        result.enjoy += uniform(1, 4);
    }
    else if (result.performance >= 100)
    {
        result.enjoy += uniform(0, 2);
    }
    else
    {
        result.enjoy -= uniform(2, 5);
    }

    ss << "\n ";

    cGirls::UnequipCombat(girl);

    DoScene(girl);
    result.bonus += result.enjoy;

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
    apply_gains(girl);

    if(m_FilmData.Action != Action_Types::NUM_ACTIONTYPES)
        girl.upd_Enjoyment(m_FilmData.Action, result.enjoy);

    girl.upd_Enjoyment(ACTION_WORKMOVIE, result.enjoy);


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
}

struct SimpleFilmJob : public GenericFilmJob
{
    SimpleFilmJob(JOBS job, const char* xml, FilmJobData data);
    void DoScene(sGirl& girl) override;

    virtual int handle_events(sGirl& girl) { return 0; }
    double GetPerformance(const sGirl& girl, bool estimate) const override { return 0; }
};

SimpleFilmJob::SimpleFilmJob(JOBS id, const char* xml, FilmJobData data) :
    GenericFilmJob(id, data)
    {
    load_from_xml(xml);
}

void SimpleFilmJob::DoScene(sGirl& girl) {
    PrintPerfSceneEval();

    ss << "\n \n";
    result.bonus = result.enjoy;
    handle_events(girl);
}
struct FilmTitty : public SimpleFilmJob
{
    FilmTitty() : SimpleFilmJob(JOB_FILMTITTY, "FilmTitty.xml", {
            IMGTYPE_TITTY, ACTION_SEX, 50, -5,
            FilmJobData::NORMAL, SKILL_TITTYSEX, "Titty"
    })
    {
    };
};

struct FilmStrip : public SimpleFilmJob
{
    FilmStrip() : SimpleFilmJob(JOB_FILMSTRIP, "FilmStrip.xml", {
         IMGTYPE_STRIP, ACTION_WORKSTRIP, 50, 0,
         FilmJobData::NICE, SKILL_STRIP, "Stripping"})
         {};

    void GainTraits(sGirl& girl, int performance) const override {
        if (girl.performance() > 50 && girl.strip() > 50 && chance(25))
        {
            cGirls::PossiblyGainNewTrait(girl, "Sexy Air", 80, ACTION_WORKSTRIP, "${name} has been stripping for so long, when she walks, it seems her clothes just want to fall off.", false);
        }
    }
};

struct FilmLesbian : public SimpleFilmJob
{
    FilmLesbian() : SimpleFilmJob( JOB_FILMLESBIAN, "FilmLes.xml", {
           IMGTYPE_LESBIAN, ACTION_SEX, 50, 10,
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
};

struct FilmHandJob : public SimpleFilmJob
{
    FilmHandJob() : SimpleFilmJob(JOB_FILMHANDJOB, "FilmHand.xml", {
          IMGTYPE_HAND, ACTION_SEX, 50, -5,
          FilmJobData::NORMAL, SKILL_HANDJOB, "Handjob"
    }) {
    }
};

struct FilmFootJob : public SimpleFilmJob
{
    FilmFootJob() : SimpleFilmJob(JOB_FILMFOOTJOB, "FilmFoot.xml", {
          IMGTYPE_FOOT, ACTION_SEX, 50, -5,
          FilmJobData::NORMAL, SKILL_FOOTJOB, "Footjob",
    }) {
    }
};

struct FilmAnal : public SimpleFilmJob
{
    FilmAnal() : SimpleFilmJob(JOB_FILMANAL, "FilmAnal.xml", {
           IMGTYPE_ANAL, ACTION_SEX, 50, 5,
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
};

struct FilmMast : public SimpleFilmJob
{
    FilmMast() : SimpleFilmJob(JOB_FILMMAST, "FilmMast.xml", {
           IMGTYPE_MAST, ACTION_SEX, 50, -10,
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

void GenericFilmJob::PrintPerfSceneEval() {
    if (result.performance >= 350)
    {
        add_text("work.perfect");
        result.bonus = 12;
    }
    else if (result.performance >= 245)
    {
        add_text("work.great");
        result.bonus = 6;
    }
    else if (result.performance >= 185)
    {
        add_text("work.good");
        result.bonus = 4;
    }
    else if (result.performance >= 145)
    {
        add_text("work.ok");
        result.bonus = 2;
    }
    else if (result.performance >= 100)
    {
        add_text("work.bad");
        result.bonus = 1;
    }
    else
    {
        add_text("work.worst");
    }

    ss << "\n";
}

void GenericFilmJob::PrintEnjoyFeedback() {

    if (result.enjoy < -1) {
        add_text("summary.dislike");
    } else if (result.enjoy < 3) {
        add_text("summary.neural");
    }else {
        add_text("summary.like");
    }
}

IGenericJob::eCheckWorkResult GenericFilmJob::CheckWork(sGirl& girl, bool is_night) {
    if(!CheckCanWork(girl)) {
        return IGenericJob::eCheckWorkResult::IMPOSSIBLE;
    }

    if(CheckRefuseWork(girl)) {
        return IGenericJob::eCheckWorkResult::REFUSES;
    }
    
    return IGenericJob::eCheckWorkResult::ACCEPTS;
}

bool GenericFilmJob::RefusedTieUp(sGirl& girl) {
    if (girl.is_slave())
    {
        if (g_Game->player().disposition() > 30)  // nice
        {
            add_text("disobey.slave.nice");
            girl.pclove(2);
            girl.pchate(-1);
            girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
            return true;
        }
        else if (g_Game->player().disposition() > -30) //pragmatic
        {
            add_text("disobey.slave.neutral") << "\n \n";
            girl.pclove(-1);
            girl.pchate(2);
            girl.pcfear(2);
            g_Game->player().disposition(-1);
            result.enjoy -= 2;
        }
        else
        {
            add_text("disobey.slave.evil")<< "\n \n";
            girl.pclove(-4);
            girl.pchate(+5);
            girl.pcfear(+5);
            g_Game->player().disposition(-2);
            result.enjoy -= 6;
        }
    }
    else // not a slave
    {
        add_text("disobey.free");
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    }
    return false;
}
