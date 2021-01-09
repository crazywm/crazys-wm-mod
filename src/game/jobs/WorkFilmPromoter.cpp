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
#include "buildings/cBuildingManager.h"
#include "cRng.h"
#include "Game.hpp"
#include <sstream>
#include "cGirls.h"
#include "movies/manager.h"

// `J` Job Movie Studio - Crew
bool WorkFilmPromoter(sGirl& girl, bool Day0Night1, cRng& rng)
{
    auto brothel = girl.m_Building;

    std::stringstream ss;
    int roll = rng.d100();
    if (roll <= 20 && girl.disobey_check(ACTION_MOVIECREW, JOB_PROMOTER))
    {
        ss << "${name} refused to work as a promoter today.";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    }
    ss << "${name} worked to promote the sales of the studio's films.\n \n";
    // TODO
    bool movies = !g_Game->movie_manager().get_movies().empty();
    if (!movies)    ss << "There were no movies for her to promote, so she just promoted the studio in general.\n \n";

    cGirls::UnequipCombat(girl);    // not for studio crew

    int wages = 50, tips = 0;
    int enjoy = 0;

    if (roll <= 10 || (!movies && roll <= 15))
    {
        enjoy -= rng % 3 + 1;
        ss << "She had difficulties working with advertisers and theater owners" << (movies ? "" : " without movies to sell them");
    }
    else if (roll >= 90)
    {
        enjoy += rng % 3 + 1;
        ss << "She found it easier " << (movies ? "selling the movies" : "promoting the studio") << " today";
    }
    else
    {
        enjoy += rng % 2;
    }
    ss << ".\n \n";
    double jobperformance = girl.job_performance(JOB_PROMOTER, false);
    jobperformance += enjoy * 2;

    // slave girls not being paid for a job that normally you would pay directly for do less work
    if (girl.is_unpaid())
    {
        jobperformance *= 0.9;
        wages = 0;
    }
    else    // work out the pay between the house and the girl
    {
        // `J` zzzzzz - need to change pay so it better reflects how well she promoted the films
        int roll_max = girl.spirit() + girl.intelligence();
        roll_max /= 4;
        wages += 10 + rng%roll_max;
    }

    for(int tries = 0; tries < 5; ++tries) {
        auto& mm = g_Game->movie_manager();
        float promotion_effect = jobperformance * std::log(brothel->m_AdvertisingBudget / jobperformance + 2);
        if (movies && g_Dice.percent(66)) {
            int index = g_Dice.random(mm.get_movies().size());
            auto& movie = mm.get_movies().at(index);
            if (movie.Age - movie.Hype > promotion_effect / 100 + 2) {
                // not much chance in improving the movie, don't promote it
                continue;
            } else {
                mm.hype_movie(index, promotion_effect / 100.f, brothel->m_AdvertisingBudget + wages);
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

    girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NIGHTSHIFT);
    girl.m_Tips = std::max(0, tips);
    girl.m_Pay = std::max(0, wages);


    // Improve girl
    int xp = 10, skill = 3;

    if (girl.has_active_trait("Quick Learner"))        { skill += 1; xp += 3; }
    else if (girl.has_active_trait("Slow Learner"))    { skill -= 1; xp -= 3; }

    girl.exp(xp);
    girl.charisma(rng%skill);
    girl.service(rng%skill);

    return false;
}

double JP_FilmPromoter(const sGirl& girl, bool estimate)// not used
{
    double jobperformance = 0.0;
    if (estimate)// for third detail string
    {
        jobperformance =
            girl.performance() / 6.0 +
            girl.service() / 6.0 +
            girl.charisma() / 6.0 +
            girl.beauty() / 10.0 +
            girl.intelligence() / 6.0 +
            girl.confidence() / 10.0 +
            girl.fame() / 10.0;

        if (girl.is_slave()) jobperformance -= 1000;
    }
    else// for the actual check
    {
        // How much will she help stretch your advertising budget? Let's find out
        double cval = 0.0;
        cval = girl.service();
        if (cval > 0)
        {
            cval = g_Dice % (int)cval + (cval / 2);  // random 50%-150% range
            jobperformance += (cval / 3);  // add ~33% of service skill to jobperformance
        }
        cval = girl.charisma();
        if (cval > 0)
        {
            cval = g_Dice % (int)cval + (cval / 2);  // random 50%-150% range
            jobperformance += (cval / 6);  // add ~17% of charisma to jobperformance
        }
        cval = girl.beauty();
        if (cval > 0)
        {
            cval = g_Dice % (int)cval + (cval / 2);  // random 50%-150% range
            jobperformance += (cval / 10);  // add 10% of beauty to jobperformance
        }
        cval = girl.intelligence();
        if (cval > 0)
        {
            cval = g_Dice % (int)cval + (cval / 2);  // random 50%-150% range
            jobperformance += (cval / 6);  // add ~17% of intelligence to jobperformance
        }
        cval = girl.confidence();
        if (cval > 0)
        {
            cval = g_Dice % (int)cval + (cval / 2);  // random 50%-150% range
            jobperformance += (cval / 10);  // add 10% of confidence to jobperformance
        }
        cval = girl.fame();
        if (cval > 0)
        {
            cval = g_Dice % (int)cval + (cval / 2);  // random 50%-150% range
            jobperformance += (cval / 4);  // add 25% of fame to jobperformance
        }
        cval = girl.level();
        if (cval > 0)
        {
            jobperformance += (cval / 2);  // add 50% of level to jobperformance
        }
        int t = girl.tiredness() - 80;
        if (t > 0)
            jobperformance -= (t + 2) * (t / 3);
    }

    jobperformance += girl.get_trait_modifier("work.filmpromoter");

    return jobperformance;
}
