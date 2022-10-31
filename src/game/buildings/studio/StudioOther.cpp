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
#include "jobs/SimpleJob.h"
#include "character/sGirl.h"
#include "cGirls.h"
#include "IGame.h"
#include "manager.h"
#include "utils/streaming_random_selection.hpp"
#include <cmath>
#include "buildings/cBuildingManager.h"

namespace {
    class cJobMovieOther : public cSimpleJob {
    public:
        cJobMovieOther(JOBS job, const char* xml) : cSimpleJob(job, xml, {ACTION_MOVIECREW}) {}
    };

    class cJobMarketResearch : public cJobMovieOther {
    public:
        cJobMarketResearch();

        bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
    };


    cJobMarketResearch::cJobMarketResearch() : cJobMovieOther(JOB_MARKET_RESEARCH, "MarketResearch.xml") {
    }

    bool cJobMarketResearch::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) {
        m_Wages = 50;

        // slave girls not being paid for a job that normally you would pay directly for do less work
        if (girl.is_unpaid()) {
            m_Performance *= 0.9;
        } else    // work out the pay between the house and the girl
        {
            // `J` zzzzzz - need to change pay so it better reflects how well she filmed the films
            int roll_max = m_Performance / 4;
            m_Wages += uniform(10, 10 + roll_max);
        }

        int points = 0;
        if (m_Performance >= 166) {
            add_text("work.perfect");
            points = 6;
        } else if (m_Performance >= 133) {
            add_text("work.great");
            points = 5;
        } else if (m_Performance >= 100) {
            add_text("work.good");
            points = 4;
        } else if (m_Performance >= 66) {
            add_text("work.ok");
            points = 3;
        } else if (m_Performance >= 33) {
            add_text("work.bad");
            points = 2;
        } else {
            add_text("work.worst");
            points = 1;
        }

        // OK, now select a target group

        RandomSelector<const sTargetGroup> select;
        for (auto& group : g_Game->movie_manager().get_audience()) {
            select.process(&group, std::exp(-(float) group.Knowledge / 5));
        }

        if (select.selection()) {
            points = points + uniform(0, points);
            g_Game->movie_manager().gain_knowledge(select.selection(), points);
            ss << "\n${name} managed to collect " << points << " points of information about "
               << select.selection()->Name << "\n";
        }

        int point_pct = m_Performance / 2 + 10;
        if(chance(point_pct)) {
            ss << "\n\n${name} managed to generate a Market Research Point for targeted surveys.";
            g_Game->movie_manager().gain_market_research_point();
        }

        girl.AddMessage(ss.str(), EImageBaseType::PROFILE, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

        // Improve stats
        apply_gains(girl, m_Performance);

        return false;
    }
}

class cJobMoviePromoter : public cJobMovieOther {
public:
    cJobMoviePromoter();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
};

cJobMoviePromoter::cJobMoviePromoter() : cJobMovieOther(JOB_PROMOTER, "Promoter.xml") {
}

bool cJobMoviePromoter::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) {
    bool movies = !g_Game->movie_manager().get_movies().empty();
    if (!movies)    ss << "There were no movies for her to promote, so she just promoted the studio in general.\n \n";

    m_Wages = 50;
    int enjoy = 0;

    int roll = d100();
    if (roll <= 10 || (!movies && roll <= 15))
    {
        enjoy -= uniform(1, 3);
        ss << "She had difficulties working with advertisers and theater owners" << (movies ? "" : " without movies to sell them");
    }
    else if (roll >= 90)
    {
        enjoy += uniform(1, 3);
        ss << "She found it easier " << (movies ? "selling the movies" : "promoting the studio") << " today";
    }
    else
    {
        enjoy += uniform(0, 1);
    }
    ss << ".\n \n";
    m_Performance += enjoy * 2;

    // slave girls not being paid for a job that normally you would pay directly for do less work
    if (girl.is_unpaid())
    {
        m_Performance *= 0.9;
        m_Wages = 0;
    }
    else    // work out the pay between the house and the girl
    {
        // `J` zzzzzz - need to change pay so it better reflects how well she promoted the films
        int roll_max = girl.spirit() + girl.intelligence();
        roll_max /= 4;
        m_Wages += uniform(10, 10 + roll_max);
    }

    m_Performance = std::max(m_Performance, 1);
    int ad_money = brothel.m_AdvertisingBudget / brothel.num_girls_on_job(JOB_PROMOTER, is_night);

    for(int tries = 0; tries < 5; ++tries) {
        auto& mm = g_Game->movie_manager();
        float promotion_effect = m_Performance * std::log(ad_money / m_Performance + 2);
        if (movies && g_Dice.percent(66)) {
            int index = g_Dice.random(mm.get_movies().size());
            auto& movie = mm.get_movies().at(index);
            // HYPE FORMULA: Hype - Age / 2 - 5 => sufficient if Hype - 5 > 5 => Hype > 10
            if (movie.Hype > 10) {
                // not much chance in improving the movie, don't promote it
                continue;
            } else {
                mm.hype_movie(index, promotion_effect / 100.f, ad_money + m_Wages);
                ss << " She promoted your movie '" << movie.Name << "' and improved hype by " << int(promotion_effect)
                   << " points\n";
                break;
            }
        }

        // OK, no movie was promoted, then encourage people to go see movies in general
        auto& audience = mm.get_audience();
        int index = g_Dice.random(audience.size());
        if(audience.at(index).Saturation / (float)audience.at(index).Amount > 0.2f) {
            int convinced = mm.hype_audience(index, promotion_effect / 100.f);
            ss << " She started an ad campaign focussed on '" << audience.at(index).Name << "' and managed to "
               << "decrease market saturation by " << convinced << "\n";
            break;
        }
    }

    int point_pct = m_Performance / 2 + 10;
    if(chance(point_pct)) {
        ss << "\n\n${name} managed to generate a Promotion Point for targeted ad campaigns.";
        g_Game->movie_manager().gain_promo_point();
    }

    girl.AddMessage(ss.str(), EImageBaseType::PROFILE, EVENT_NIGHTSHIFT);


    // Improve girl
    apply_gains(girl, m_Performance);

    return false;
}

void RegisterOtherStudioJobs(cJobManager& mgr) {
    mgr.register_job(std::make_unique<cJobMarketResearch>());
    mgr.register_job(std::make_unique<cJobMoviePromoter>());
}