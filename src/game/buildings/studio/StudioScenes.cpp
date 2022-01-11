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

#include "StudioJobs.h"
#include "character/sGirl.h"
#include "cGirls.h"
#include "IGame.h"
#include "sStorage.h"
#include "buildings/IBuilding.h"
#include "utils/streaming_random_selection.hpp"

namespace {
    struct FilmOral : cFilmSceneJob {
        FilmOral() : cFilmSceneJob(JOB_FILMORAL, "FilmOral.xml", IMGTYPE_ORAL,
                                   SceneType::ORAL) {

        }

        void PreFilmCallback(sGirl& girl) override {
            cFilmSceneJob::PreFilmCallback(girl);
            if (girl.has_active_trait("Cum Addict")) m_Enjoyment += 2;
        }

        void PostFilmCallback(sGirl& girl) override {
            cFilmSceneJob::PostFilmCallback(girl);
            if (chance(5) && (girl.happiness() > 80) && (girl.get_enjoyment(ACTION_PORNSTAR) > 75))
                cGirls::AdjustTraitGroupGagReflex(girl, 1, true);
        }
    };

    struct FilmThroat : cFilmSceneJob {
        FilmThroat() : cFilmSceneJob(JOB_FILMFACEFUCK, "FilmFaceFuck.xml", IMGTYPE_ORAL,
                                   SceneType::THROAT) {

        }

        void PreFilmCallback(sGirl& girl) override {
            cFilmSceneJob::PreFilmCallback(girl);
            if (girl.has_active_trait("Cum Addict")) m_Enjoyment += 2;
        }

        void PostFilmCallback(sGirl& girl) override {
            cFilmSceneJob::PostFilmCallback(girl);

            int impact = uniform(0, 10);
            if (girl.has_active_trait("Strong Gag Reflex"))
            {
                ss << "She was gagging and retching the whole scene, and was violently sick. She was exhausted and felt awful afterward.\n \n";
                girl.health((10 + impact));
                girl.tiredness((10 + impact));
                girl.pclove(-2);
                girl.pcfear(1);
            }
            else if (girl.has_active_trait("Gag Reflex"))
            {
                ss << "She gagged and retched a lot. It was exhausting and left her feeling sick.\n \n";
                girl.health(2 + impact);
                girl.tiredness(5 + impact);
                girl.pclove(-1);
            }

            if (chance(5) && (girl.happiness() > 80) && (girl.get_enjoyment(ACTION_PORNSTAR) > 75))
                cGirls::AdjustTraitGroupGagReflex(girl, 1, true);
        }
    };

    struct FilmBeast : cFilmSceneJob {
        FilmBeast() : cFilmSceneJob(JOB_FILMBEAST, "FilmBeast.xml",IMGTYPE_BEAST,
                                    SceneType::BEAST, cFilmSceneJob::SexAction::MONSTER) {
        }

        bool CheckCanWork(sGirl& girl) override {
            if(cFilmSceneJob::CheckCanWork(girl)) {
                // no beasts = no scene
                if (g_Game->storage().beasts() < 1)
                {
                    girl.AddMessage("You have no beasts for this scene, so she had the day off.", IMGTYPE_PROFILE, EVENT_NOWORK);
                    return false;
                }
                return true;
            }
            return false;
        }
    };

    struct FilmBuk : cFilmSceneJob {
        FilmBuk() : cFilmSceneJob(JOB_FILMBUKKAKE, "FilmBuk.xml", IMGTYPE_ORAL, SceneType::BUK) {}
        void Narrate(sGirl& girl) override {
            if(m_IsForced) {
                PrintForcedSceneEval();
            } else {
                add_text("work.description");
                if (m_IsForced) {
                    add_text("work.description.tied");
                } else {
                    add_text("work.description.untied");
                }

                PrintPerfSceneEval();
            }
        }
    };
    struct FilmPubBDSM : cFilmSceneJob {
        FilmPubBDSM() : cFilmSceneJob(JOB_FILMPUBLICBDSM, "FilmPublicBDSM.xml", IMGTYPE_BDSM, SceneType::PUB_BDSM) {}
        void Narrate(sGirl& girl) override {
            if(m_IsForced) {
                PrintForcedSceneEval();
            } else {
                add_text("scene.position");
                add_text("scene.action") << "\n";
                PrintPerfSceneEval();
            }
        }
    };

    struct FilmAction : cFilmSceneJob {
        FilmAction() : cFilmSceneJob(JOB_FILMACTION, "FilmAction.xml", IMGTYPE_COMBAT, SceneType::ACTION) {}
        void Narrate(sGirl& girl) override {
            if (m_Performance >= 350)
            {
                ss << "${name} made an outstanding action scene, " << get_text("story");
                ss << " It will definitely win her some fans.";
            }
            else if (m_Performance >= 245)
            {
                ss << "${name} made a great action scene, " << get_text("story");
                ss << " It should win her some fans.";
            }
            else if (m_Performance >= 185)
            {
                ss << "${name} made a fairly good action scene, " << get_text("story");
                ss << " Her fans will enjoy it.";
            }
            else if (m_Performance >= 145)
            {
                ss << "${name} made an uninspired action scene, " << get_text("story");
                ss << " Her diehard fans might enjoy it.";
            }
            else if (m_Performance >= 100)
            {
                ss << "${name} made a weak action scene, " << get_text("story");
            }
            else
            {
                ss << "${name} made an awful action scene, " << get_text("story");
                ss << " Even her fans will hate it.";
            }
        }
    };

    struct FilmRandom : cBasicJob {
        FilmRandom() : cBasicJob(JOB_FILMRANDOM) {
            m_Info.ShortName = "FRnd";
            m_Info.Description = "She will perform in a random scene, chosen according to her skills.";
        }

        eCheckWorkResult CheckWork(sGirl& girl, bool is_night) override {
            return eCheckWorkResult::ACCEPTS;
        }

        sWorkJobResult DoWork(sGirl& girl, bool is_night) override {
            cJobManager& mgr = g_Game->job_manager();
            RandomSelector<void> select_job;

            std::initializer_list<JOBFILTER> filters = {JOBFILTER_STUDIOTEASE, JOBFILTER_STUDIOSOFTCORE, JOBFILTER_STUDIOPORN, JOBFILTER_STUDIOHARDCORE};
            for(auto& filter : filters) {
                auto jobs = mgr.JobFilters[filter];
                // note: we really need to capture by reference here, because the selector works with pointers!
                for(auto& job : jobs.Contents) {
                    auto& job_class = dynamic_cast<cFilmSceneJob&>(*mgr.m_OOPJobs[job]);
                    auto st = job_class.GetSexType();
                    if(girl.m_Building->is_sex_type_allowed(st)) {
                        auto o = job_class.CalcChanceToObey(girl);
                        // scale performance with chance to obey. If larger than 80%, don't differentiate further
                        float f = o.total() / 80.f;
                        f = std::min(1.f, f);
                        // make it less likely to choose a job type with bad performance, or which the girl is unlikely to do
                        select_job.process((void*)job, std::exp(f * girl.job_performance(job, true) / 15.f) / 10.f);
                    }
                }
            }

            if(select_job.selection()) {
                JOBS chosen = static_cast<JOBS>((std::ptrdiff_t)select_job.selection());
                return mgr.do_job(chosen, girl, SHIFT_NIGHT);
            }

            girl.AddMessage("Could not find a valid scene to film for the random job.", IMGTYPE_PROFILE, EVENT_DEBUG);
            return {false, 0, 0, 0};
        }
    };
}

// film registry
void RegisterFilmingJobs(cJobManager& mgr) {
    mgr.register_job(std::make_unique<cFilmSceneJob>(JOB_FILMTITTY, "FilmTitty.xml",
                                                     IMGTYPE_TITTY, SceneType::TITTY));

    mgr.register_job(std::make_unique<cFilmSceneJob>(JOB_FILMHANDJOB, "FilmHand.xml",
                                                     IMGTYPE_HAND, SceneType::HAND));

    mgr.register_job(std::make_unique<cFilmSceneJob>(JOB_FILMFOOTJOB, "FilmFoot.xml",
                                                     IMGTYPE_FOOT, SceneType::FOOT));

    mgr.register_job(std::make_unique<cFilmSceneJob>(JOB_FILMMAST, "FilmMast.xml",
                                                     IMGTYPE_MAST, SceneType::MAST));

    mgr.register_job(std::make_unique<cFilmSceneJob>(JOB_FILMANAL, "FilmAnal.xml",
                                                     IMGTYPE_ANAL, SceneType::ANAL));

    mgr.register_job(std::make_unique<cFilmSceneJob>(JOB_FILMLESBIAN, "FilmLes.xml",
                                                     IMGTYPE_LESBIAN, SceneType::LES));

    mgr.register_job(std::make_unique<cFilmSceneJob>(JOB_FILMSTRIP, "FilmStrip.xml",
                                                     IMGTYPE_STRIP, SceneType::STRIP));

    mgr.register_job(std::make_unique<cFilmSceneJob>(JOB_FILMTEASE, "FilmTease.xml",
                                                     IMGTYPE_ECCHI, SceneType::TEASE));

    mgr.register_job(std::make_unique<cFilmSceneJob>(JOB_FILMMUSIC, "FilmMusic.xml",
                                                     IMGTYPE_SING, SceneType::MUSIC));

    mgr.register_job(std::make_unique<cFilmSceneJob>(JOB_FILMSEX, "FilmSex.xml",
                                                     IMGTYPE_SEX, SceneType::SEX, cFilmSceneJob::SexAction::HUMAN));

    mgr.register_job(std::make_unique<cFilmSceneJob>(JOB_FILMCHEF, "FilmChef.xml",
                                                     IMGTYPE_COOK, SceneType::CHEF));

    mgr.register_job(std::make_unique<cFilmSceneJob>(JOB_FILMBONDAGE, "FilmBondage.xml",
                                                     IMGTYPE_BDSM, SceneType::BDSM, cFilmSceneJob::SexAction::HUMAN));

    mgr.register_job(std::make_unique<cFilmSceneJob>(JOB_FILMGROUP, "FilmGroup.xml",
                                                     IMGTYPE_GROUP, SceneType::GROUP, cFilmSceneJob::SexAction::HUMAN));

    mgr.register_job(std::make_unique<FilmOral>());
    mgr.register_job(std::make_unique<FilmThroat>());
    mgr.register_job(std::make_unique<FilmBeast>());
    mgr.register_job(std::make_unique<FilmBuk>());
    mgr.register_job(std::make_unique<FilmAction>());
    mgr.register_job(std::make_unique<FilmPubBDSM>());
    mgr.register_job(std::make_unique<FilmRandom>());
}