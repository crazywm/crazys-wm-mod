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

#include "manager.h"
#include "utils/DirPath.h"
#include "xml/util.h"
#include "tinyxml2.h"
#include "cRng.h"
#include <cmath>
#include <map>
#include "utils/algorithms.hpp"
#include "utils/string.hpp"
#include "Game.hpp"
#include "CLog.h"
#include "buildings/IBuilding.h"

extern cRng g_Dice;


namespace {
    // maybe turn these into game constants later
    sPercent CHANCE_TO_UPDATE_GROUP = sPercent(2);
    float SATURATION_DECAY = 0.05;
    float HYPE_DECAY = 0.1;

    void update_group_by_spec(sTargetGroup& group, const sTargetGroupSpec& spec) {
        group.Name = spec.Name;
        group.Favourite = spec.Favourites[g_Dice.random(spec.Favourites.size())];

        // spending power and score requirement correlate
        float x = g_Dice.randomd(1.0);
        group.RequiredScore = spec.RequiredScoreMin + x * (spec.RequiredScoreMax - spec.RequiredScoreMin);
        group.SpendingPower = spec.SpendingPowerMin + x * (spec.SpendingPowerMax - spec.SpendingPowerMin);


        group.ForcedIsTurnOff = spec.ForcedIsTurnOff;
        group.Amount = g_Dice.in_range(spec.MinAmount, spec.MaxAmount);
        for(int i = 0; i < (int)SceneCategory::NUM_TYPES; ++i) {
            group.TurnOffs[i] = spec.TurnOffs[i];
            group.Desires[i] = g_Dice.in_range(100.f * spec.DesiresMin[i], 100.f * spec.DesiresMax[i]) / 100.f;
        }
    }

    // how much improvement can a scene get based on technical quality
    float TECHNICAL_QUALITY_BONUS[] = {
            0.25, 0.2, 0.1, 0.05, 0.05
    };


    float sigmoid(float v) {
        return std::exp(v) / (std::exp(v) + 1.f);
    }
}

auto cMovieManager::rate_movie_for_audience(const sTargetGroup& audience, const Movie& movie) -> RatingResult {
    float chance_to_consider = 1.f;
    // check the turn offs
    for(const auto& scene : movie.Scenes) {
        chance_to_consider *= 1.f - audience.TurnOffs[(int)scene.Category];
    }

    float total_score = 0;

    std::map<SceneType, int> type_counts;

    for(const auto& scene : movie.Scenes) {
        float desired = audience.Desires[(int)scene.Category];

        auto found = type_counts.find(scene.Type);
        if(found == type_counts.end()) {
            found = type_counts.insert(std::make_pair(scene.Type, 0)).first;
        }
        // desire for scene type decreases for repetitions
        desired *= std::pow(0.8, found->second);

        if(scene.Type == audience.Favourite) {
            desired = 1.0;
        }

        float tec_bonus = TECHNICAL_QUALITY_BONUS[(int)scene.Category] * scene.TechnicalQuality;
        float scene_quality = scene.ContentQuality * sigmoid(scene.TechnicalQuality / 10.f) + tec_bonus;
        total_score += scene_quality * desired;
        type_counts[scene.Type] += 1;
    }

    float price_fraction = static_cast<float>(movie.TicketPrice) / audience.SpendingPower;
    float can_afford = sigmoid(6.f * (1.f - price_fraction));

    const float log_factor = std::log(0.9f) / std::log(0.5f);
    float quality_threshold = audience.RequiredScore * std::max(std::min(1.f, std::exp(std::log(price_fraction) * log_factor)), 0.8f);
    float quality_factor = sigmoid(6.f * (total_score / quality_threshold - 1.f));

    float base_hype = 0.05f + 0.05f * sigmoid(movie.Hype - movie.Age / 2 - 5);
    base_hype *= std::exp(-float(movie.Age) / 10.f);

    float satisfaction = total_score / audience.RequiredScore - price_fraction;

    assert(0.f <= can_afford && can_afford <= 1.f);
    assert(0.f <= base_hype && base_hype <= 1.f);
    assert(0.f <= chance_to_consider && chance_to_consider <= 1.f);
    assert(0.f <= quality_factor && quality_factor <= 1.f);

    return{satisfaction, total_score, quality_factor, can_afford, base_hype, chance_to_consider};
}

void cMovieManager::init() {
    auto doc = LoadXMLDocument((DirPath() << "Resources" << "Data" << "Audiences.xml").str());
    if(!doc->RootElement()) {
        throw std::runtime_error("Audiences.xml is missing Root Element");
    }

    for(auto& tg : IterateChildElements(*doc->RootElement(), "TargetGroup")) {
        m_TargetGroupSpecs.emplace_back();
        m_TargetGroupSpecs.back().load_xml(tg);
    }

    // load default names
    doc = LoadXMLDocument((DirPath() << "Resources" << "Data" << "MovieNames.xml").str());
    if(!doc->RootElement()) {
        throw std::runtime_error("MovieNames.xml is missing Root Element");
    }
    for(auto& el : IterateChildElements(*doc->RootElement(), "Tease")) {
        m_DefaultNames[(int)SceneCategory::TEASE].push_back( el.GetText() );
    }
    for(auto& el : IterateChildElements(*doc->RootElement(), "Soft")) {
        m_DefaultNames[(int)SceneCategory::SOFT].push_back( el.GetText() );
    }
    for(auto& el : IterateChildElements(*doc->RootElement(), "Hard")) {
        m_DefaultNames[(int)SceneCategory::HARD].push_back( el.GetText() );
    }
    for(auto& el : IterateChildElements(*doc->RootElement(), "Extreme")) {
        m_DefaultNames[(int)SceneCategory::EXTREME].push_back( el.GetText() );
    }

    m_Audiences.clear();
    for(auto& s : m_TargetGroupSpecs) {
        m_Audiences.emplace_back();
        update_group_by_spec(m_Audiences.back(), s);
    }
}

int cMovieManager::step(IBuilding& studio) {
    float total_income = 0.0;
    for(auto& movie : m_Movies) {
        int income = 0;
        float hype = 0.f;
        int total_viewers = 0;
        for(auto& audience : m_Audiences) {
            auto rating = rate_movie_for_audience(audience, movie);
            float factor = rating.QualityFactor * rating.PriceFactor * rating.HypeFactor * rating.NoTurnOff;
            int viewers = static_cast<int>((audience.Amount - audience.Saturation) * factor);
            income += movie.TicketPrice * viewers;
            audience.Saturation += viewers * 0.5f;
            if(rating.Satisfaction > 0.5) {
                hype += (rating.Satisfaction - 0.5f) * rating.PriceFactor * viewers;
            }
            total_viewers += viewers;
        }

        // weighted mean of hype
        if(total_viewers > 0) {
            hype /= total_viewers;
        } else {
            hype = 0;
        }

        movie.Age += 1;
        movie.Hype += hype;
        movie.TotalEarnings += income;
        movie.EarningsLastWeek = income;
        total_income += income;

        std::stringstream movie_report;
        movie_report << "Your movie " << movie.Name << " was seen by " <<total_viewers << " people who paid a total of "
                     << income << " gold for tickets. It has been in theatres for " << movie.Age << " weeks now.";
        if(hype > 0.1) {
            movie_report << " It was well received, and its hype-rating increased by " << int(hype*100) << " points. ";
        }
        studio.m_Events.AddMessage(movie_report.str(), IMGTYPE_PROFILE, EVENT_BROTHEL);
    }

    // remove old movies and notify the player
    erase_if(m_Movies, [&](const Movie& m)
    {
        if(m.Age >= 25) {
            std::stringstream movie_end_report;
            movie_end_report << "Your movie " << m.Name << " finished its run with a total revenue of " << m.TotalEarnings
                             << " and has now been taken out of the programming. It generated a total hype of " << int(100 * m.Hype) << ".";
            studio.m_Events.AddMessage(movie_end_report.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);
            return true;
        }
        return false;
    });

    // decay hype and saturation
    for(auto& movie : m_Movies) {
        movie.Hype *= 1.f - HYPE_DECAY;
    }

    update_audience();

    for(auto& aud : m_Audiences) {
        if(aud.Saturation > 0.5 * aud.Amount) {
            std::stringstream tg_wn;
            tg_wn << "It appears that people in the target group " << aud.Name << " have been to the movies so often lately"
                     " that their desire for cinema is mostly sated.";
            studio.m_Events.AddMessage(tg_wn.str(), IMGTYPE_PROFILE, EVENT_WARNING);
        }

        // a small fraction of people become saturated even if you aren't showing any movies
        aud.Saturation += 0.025f * (aud.Amount - aud.Saturation);
        // and some of the saturated once want to watch movies again
        aud.Saturation *= 1.f - SATURATION_DECAY;
    }

    studio.m_Events.AddMessage("In total, your Studio earned " + std::to_string((int)total_income) +
                                " gold this week from ticket sales of " + std::to_string(m_Movies.size()) + " movies.",
                               IMGTYPE_PROFILE, EVENT_SUMMARY);

    return static_cast<int>(total_income);
}

cMovieManager::cMovieManager() {
    init();
}

void cMovieManager::load_xml(const tinyxml2::XMLElement& element) {
    m_Audiences.clear();
    for(auto& el : IterateChildElements(element, "TargetGroup")) {
        m_Audiences.emplace_back();
        m_Audiences.back().load_xml(el);
    }

    m_Scenes.clear();
    for(auto& el : IterateChildElements(element, "Scene")) {
        m_Scenes.emplace_back();
        m_Scenes.back().load_xml(el);
    }

    m_Movies.clear();
    for(auto& el : IterateChildElements(element, "Movie")) {
        m_Movies.emplace_back();
        m_Movies.back().load_xml(el);
    }
}

void cMovieManager::save_xml(tinyxml2::XMLElement& element) const {
    for(auto& group : m_Audiences) {
        auto& el = PushNewElement(element, "TargetGroup");
        group.save_xml(el);
    }

    for(auto& scene : m_Scenes) {
        auto& el = PushNewElement(element, "Scene");
        scene.save_xml(el);
    }

    for(auto& movie : m_Movies) {
        auto& el = PushNewElement(element, "Movie");
        movie.save_xml(el);
    }
}

void cMovieManager::add_scene(MovieScene scene) {
    m_Scenes.push_back(std::move(scene));
}

const std::vector<MovieScene>& cMovieManager::get_scenes() const {
    return m_Scenes;
}

bool cMovieManager::delete_scene(const MovieScene* scene) {
    return erase_if(m_Scenes, [&](const auto& elem){ return &elem == scene; }) != 0;
}

Movie& cMovieManager::create_movie(const std::vector<const MovieScene*>& scenes, std::string name) {
    m_Movies.emplace_back();
    Movie& mv = m_Movies.back();
    mv.Name = std::move(name);

    int cost = 0;
    for(const MovieScene* scene: scenes) {
        cost += scene->Budget;
        mv.Scenes.push_back(*scene);
        // TODO check that scene really is part of the vector.
        const_cast<MovieScene*>(scene)->Category = SceneCategory::NUM_TYPES;        // mark for deletion
    }
    // now remove the used-up scenes
    erase_if(m_Scenes, [&](const auto& elem){ return elem.Category == SceneCategory::NUM_TYPES; });
    mv.Age = 0;
    mv.Hype = 0;
    mv.TicketPrice = auto_detect_ticket_price(mv);
    mv.TotalEarnings = 0;
    mv.TotalCost = cost;
    return mv;
}

const std::vector<Movie>& cMovieManager::get_movies() const {
    return m_Movies;
}

void cMovieManager::set_ticket_price(int index, int price) {
    if(price <= 0) {
        throw std::invalid_argument("Ticket price must be positive");
    }
    m_Movies.at(index).TicketPrice = price;
}

const std::vector<sTargetGroup>& cMovieManager::get_audience() const {
    return m_Audiences;
}

void cMovieManager::hype_movie(int index, float hype, int cost) {
    m_Movies.at(index).Hype += hype;
    m_Movies.at(index).TotalCost += cost;
}

int cMovieManager::hype_audience(int index, float hype) {
    float change = sigmoid(hype) * 0.1;
    int primed = m_Audiences.at(index).Saturation * change;
    m_Audiences.at(index).Saturation -= primed;
    return primed;
}

std::string cMovieManager::auto_create_name(const std::vector<const MovieScene*>& scenes) {
    int most_extreme = 0;
    for(auto& scene : scenes) {
        most_extreme = std::max(most_extreme, (int)scene->Category);
    }
    auto& namelist = m_DefaultNames.at(most_extreme);
    auto candidate = namelist.at(g_Dice.random(namelist.size()));
    // TODO automatic counter?
    return candidate;
}

void cMovieManager::update_audience() {
    for(auto& aud : m_Audiences) {
        if(g_Dice.percent(CHANCE_TO_UPDATE_GROUP)) {
            auto found = std::find_if(m_TargetGroupSpecs.begin(), m_TargetGroupSpecs.end(),
                                      [&](const sTargetGroupSpec& spec){
                return spec.Name == aud.Name;
            });
            if(found != m_TargetGroupSpecs.end()) {
                // update the target group, but preserve the fraction of saturation
                float sat_fac = static_cast<float>(aud.Saturation) / aud.Amount;
                update_group_by_spec(aud, *found);
                aud.Saturation = static_cast<int>(sat_fac * aud.Amount);
            } else {
                g_LogFile.warning("movies", "Could not find audience spec for ", aud.Name);
            }
        }
    }
}

int cMovieManager::estimate_revenue(const Movie& movie) {
    int total_expected = 0;
    for(auto& a : m_Audiences) {
        auto rating = rate_movie_for_audience(a, movie);
        float factor = rating.QualityFactor * rating.PriceFactor * rating.HypeFactor * rating.NoTurnOff;
        int viewers = static_cast<int>((a.Amount - a.Saturation) * factor);
        total_expected += viewers * movie.TicketPrice;
    }
    return total_expected;
}

int cMovieManager::auto_detect_ticket_price(const Movie& movie) {
    Movie cp(movie);
    auto TicketPrices = {1, 2, 4, 5, 6, 8, 10, 15, 20, 25, 30, 40, 50};
    int best = -1;
    int best_rev = -1;
    for(auto& price : TicketPrices) {
        cp.TicketPrice = price;
        int rev = estimate_revenue(cp);
        if(rev > best_rev) {
            best = price;
            best_rev = rev;
        }
    }
    return best;
}

