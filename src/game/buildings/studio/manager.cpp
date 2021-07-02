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
#include "xml/getattr.h"
#include "tinyxml2.h"
#include "cRng.h"
#include <cmath>
#include <map>
#include "utils/algorithms.hpp"
#include "utils/string.hpp"
#include "IGame.h"
#include "CLog.h"
#include "buildings/IBuilding.h"

extern cRng g_Dice;

namespace settings {
    extern const char* MOVIES_SATURATION_DECAY;
    extern const char* MOVIES_HYPE_DECAY;
    extern const char* MOVIES_AUDIENCE_UPDATE_CHANCE;
    extern const char* MOVIES_REPEATED_SCENE_FACTOR;
    extern const char* MOVIES_RUNNING_WEEKS;
    extern const char* MOVIES_AUDIENCE_BASE_FRACTION;
    extern const char* MOVIES_AUDIENCE_SATED_CHANCE;
}


namespace {
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

    const float multi_scene_factor = 1.f - g_Game->settings().get_float(settings::MOVIES_REPEATED_SCENE_FACTOR);

    for(const auto& scene : movie.Scenes) {
        float desired = audience.Desires[(int)scene.Category];

        auto found = type_counts.find(scene.Type);
        if(found == type_counts.end()) {
            found = type_counts.insert(std::make_pair(scene.Type, 0)).first;
        }
        // desire for scene type decreases for repetitions
        desired *= std::pow(multi_scene_factor, found->second);

        if(scene.Type == audience.Favourite) {
            desired = 1.0;
        }

        float tec_bonus = TECHNICAL_QUALITY_BONUS[(int)scene.Category] * scene.TechnicalQuality;
        float scene_quality = scene.ContentQuality * sigmoid(scene.TechnicalQuality / 10.f) + tec_bonus;
        total_score += scene_quality * desired;
        type_counts[scene.Type] += 1;
    }

    float price_fraction = static_cast<float>(movie.TicketPrice) / audience.SpendingPower;
    float can_afford = sigmoid(7.f * (1.f - price_fraction));

    const float log_factor = std::log(0.9f) / std::log(0.5f);
    float quality_threshold = audience.RequiredScore * std::max(std::min(1.f, std::exp(std::log(price_fraction) * log_factor)), 0.8f);

    // we want a contribution from relative quality, so that for low quality movies the curves are sharp,
    // but also a contribution from absolute quality, so the curves don't become too smooth (and never reach 100%)
    // for the high-quality segment
    float relative_quality = total_score / quality_threshold - 1.f;
    float absolute_quality = (total_score - quality_threshold) / 25.f;
    float quality_factor = sigmoid(7.f * relative_quality + absolute_quality);

    float base_hype = g_Game->settings().get_float(settings::MOVIES_AUDIENCE_BASE_FRACTION) * (1 + sigmoid(movie.Hype - 5));
    base_hype = std::min(base_hype, 1.f);
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

    auto read_movie = [](tinyxml2::XMLElement& el) {
        const char* req = el.Attribute("Requires");
        return sMovieName{el.GetText(), req ? scene_type_from_string(req) : SceneType::COUNT};
    };

    for(auto& el : IterateChildElements(*doc->RootElement(), "Tease")) {
        m_DefaultNames[(int)SceneCategory::TEASE].push_back( read_movie(el) );
    }
    for(auto& el : IterateChildElements(*doc->RootElement(), "Soft")) {
        m_DefaultNames[(int)SceneCategory::SOFT].push_back( read_movie(el) );
    }
    for(auto& el : IterateChildElements(*doc->RootElement(), "Hard")) {
        m_DefaultNames[(int)SceneCategory::HARD].push_back( read_movie(el) );
    }
    for(auto& el : IterateChildElements(*doc->RootElement(), "Extreme")) {
        m_DefaultNames[(int)SceneCategory::EXTREME].push_back( read_movie(el) );
    }

    m_Audiences.clear();
    for(auto& s : m_TargetGroupSpecs) {
        m_Audiences.emplace_back();
        update_group_by_spec(m_Audiences.back(), s);
    }
}

int cMovieManager::step(IBuilding& studio) {
    float total_income = 0.0;
    const float sated_chance = g_Game->settings().get_float(settings::MOVIES_AUDIENCE_SATED_CHANCE);

    for(auto& movie : m_Movies) {
        int income = 0;
        float hype = 0.f;
        int total_viewers = 0;
        for(auto& audience : m_Audiences) {
            auto rating = rate_movie_for_audience(audience, movie);
            float factor = rating.QualityFactor * rating.PriceFactor * rating.HypeFactor * rating.NoTurnOff;
            int viewers = static_cast<int>((audience.Amount - audience.Saturation) * factor);
            income += movie.TicketPrice * viewers;
            audience.Saturation += viewers * sated_chance;
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
        studio.AddMessage(movie_report.str());
    }

    // remove old movies and notify the player
    erase_if(m_Movies, [&](const Movie& m)
    {
        if(m.Age >= g_Game->settings().get_integer(settings::MOVIES_RUNNING_WEEKS)) {
            std::stringstream movie_end_report;
            movie_end_report << "Your movie " << m.Name << " finished its run with a total revenue of " << m.TotalEarnings
                             << " and has now been taken out of the programming. It generated a total hype of " << int(100 * m.Hype) << ".";
            studio.AddMessage(movie_end_report.str(), EVENT_GOODNEWS);
            return true;
        }
        return false;
    });

    // decay hype and saturation
    const float hype_decay_factor = 1.f - g_Game->settings().get_float(settings::MOVIES_HYPE_DECAY);
    for(auto& movie : m_Movies) {
        movie.Hype -= g_Dice.in_range(25, 75) / 100.f;
        movie.Hype *= hype_decay_factor;
    }

    update_audience();

    // update market research / promotion points
    // decay the market research / promotion points, except for the last one
    m_MarketResearchPoints = m_MarketResearchPoints - std::max(0, m_MarketResearchPoints / 10 - 8);
    m_PromotionPoints = m_PromotionPoints - std::max(0, m_PromotionPoints / 10 - 8);

    const float saturation_decay_factor = 1.f - g_Game->settings().get_float(settings::MOVIES_SATURATION_DECAY);
    for(auto& aud : m_Audiences) {
        if(aud.Saturation > 0.5 * aud.Amount) {
            std::stringstream tg_wn;
            tg_wn << "It appears that people in the target group " << aud.Name << " have been to the movies so often lately"
                     " that their desire for cinema is mostly sated.";
            studio.AddMessage(tg_wn.str(), EVENT_WARNING);
        }

        // a small fraction of people become saturated even if you aren't showing any movies
        // but only in some weeks, to increase the variance between groups
        if(g_Dice.percent(5)) {
            // TODO note these changes here in the design document!
            // TODO make this GAME-CONFIG changeable
            aud.Saturation += g_Dice.in_range(0, 10) * (aud.Amount - aud.Saturation) / 100;
        }
        // and some of the saturated once want to watch movies again
        aud.Saturation *= saturation_decay_factor;
    }

    studio.AddMessage("In total, your Studio earned " + std::to_string((int)total_income) +
                                " gold this week from ticket sales of " + std::to_string(m_Movies.size()) + " movies.",
                               EVENT_SUMMARY);

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

    m_MarketResearchPoints = GetIntAttribute(element, "MarketResearch");
    m_PromotionPoints = GetIntAttribute(element, "Promotion");
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

    element.SetAttribute("MarketResearch", m_MarketResearchPoints);
    element.SetAttribute("Promotion", m_PromotionPoints);
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
    RandomSelector<sMovieName> select;
    for(auto& n : namelist) {
        if(n.Requires != SceneType::COUNT) {
            for(auto& scene : scenes) {
                if(scene->Type == n.Requires) {
                    select.process(&n);
                    break;
                }
            }
        } else {
            select.process(&n);
        }
    }

    if(select.selection()) {
        return select.selection()->Name;
    }

    return "";
}

namespace {
    auto lookup_spec(const std::string& name, const std::vector<sTargetGroupSpec>& groups) {
        auto found = std::find_if(groups.begin(), groups.end(),
                                  [&](const sTargetGroupSpec& spec){
                                      return spec.Name == name;
                                  });
        return found;
    }

    sTargetGroup known_data(const sTargetGroup& true_data, const sTargetGroupSpec& spec) {
        sTargetGroup group;
        group.Name = spec.Name;
        if(true_data.Knowledge >= sTargetGroup::KnowledgeForFavScene) {
            group.Favourite = true_data.Favourite;
        } else {
            group.Favourite = SceneType::COUNT;
        }

        if(true_data.Knowledge >= sTargetGroup::KnowledgeForSpendingPower) {
            group.SpendingPower = true_data.SpendingPower;
        } else {
            group.SpendingPower = (spec.SpendingPowerMax + spec.SpendingPowerMin) / 2;
        }

        if(true_data.Knowledge >= sTargetGroup::KnowledgeForReqScore) {
            group.RequiredScore = true_data.RequiredScore;
        } else {
            group.RequiredScore = (spec.RequiredScoreMax + spec.RequiredScoreMin) / 2;
        }

        if(true_data.Knowledge >= sTargetGroup::KnowledgeForSize) {
            group.Amount = true_data.Amount;
        } else {
            group.Amount = (spec.MinAmount + spec.MaxAmount) / 2;
        }

        if(true_data.Knowledge >= sTargetGroup::KnowledgeForSaturation) {
            group.Saturation = true_data.Saturation;
        } else {
            group.Saturation = 0.25f * group.Amount;
        }

        group.ForcedIsTurnOff = spec.ForcedIsTurnOff;
        for(int i = 0; i < (int)SceneCategory::NUM_TYPES; ++i) {
            group.TurnOffs[i] = spec.TurnOffs[i];
            if(true_data.Knowledge >= sTargetGroup::KnowledgeForDesires) {
                group.Desires[i] = true_data.Desires[i];
            } else {
                group.Desires[i] = 0.5 * (spec.DesiresMin[i] + spec.DesiresMax[i]);
            }
        }

        return group;
    }
}

void cMovieManager::update_audience() {
    const sPercent update_chance = g_Game->settings().get_percent(settings::MOVIES_AUDIENCE_UPDATE_CHANCE);

    for(auto& aud : m_Audiences) {
        // decline of knowledge
        aud.Knowledge = std::max(0, aud.Knowledge - 1);

        if(g_Dice.percent(update_chance)) {
            auto found = lookup_spec(aud.Name, m_TargetGroupSpecs);
            if(found != m_TargetGroupSpecs.end()) {
                // update the target group, but preserve the fraction of saturation
                float sat_fac = static_cast<float>(aud.Saturation) / aud.Amount;
                update_group_by_spec(aud, *found);
                aud.Saturation = static_cast<int>(sat_fac * aud.Amount);
                aud.Knowledge /= 2;
            } else {
                g_LogFile.error("movies", "Could not find audience spec for ", aud.Name);
            }
        }
    }
}

auto cMovieManager::estimate_revenue(const sTargetGroup& audience, const Movie& movie) -> sRevenueEstimate {
    auto spec_iter = lookup_spec(audience.Name, m_TargetGroupSpecs);
    if(spec_iter == end(m_TargetGroupSpecs)) {
        g_LogFile.error("movies", "Could not find audience spec for ", audience.Name);
        return {{}, 0, 0};
    }
    sTargetGroup tg_est = known_data(audience, *spec_iter);
    auto rating = rate_movie_for_audience(tg_est, movie);
    float factor = rating.QualityFactor * rating.PriceFactor * rating.HypeFactor * rating.NoTurnOff;
    int viewers = static_cast<int>((tg_est.Amount - tg_est.Saturation) * factor);

    return {rating, viewers, viewers * movie.TicketPrice, (int)tg_est.RequiredScore};
}


int cMovieManager::estimate_revenue(const Movie& movie) {
    int total_expected = 0;
    for(auto& a : m_Audiences) {
        total_expected += estimate_revenue(a, movie).Revenue;
    }
    return total_expected;
}

int cMovieManager::auto_detect_ticket_price(const Movie& movie) {
    Movie cp(movie);
    auto TicketPrices = {1, 2, 4, 5, 6, 8, 10, 12, 15, 20, 25, 30, 40, 50};
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

void cMovieManager::gain_knowledge(const sTargetGroup* group, int gain) {
    int index = group - m_Audiences.data();
    m_Audiences.at(index).Knowledge += gain;
}

void cMovieManager::gain_promo_point() {
    m_PromotionPoints += 100;
}

int cMovieManager::promotion_points() const {
    return (m_PromotionPoints + 50) / 100;
}

void cMovieManager::run_ad_campaign(int target_movie) {
    int points = std::min(m_PromotionPoints, 500);
    if(g_Game->gold().misc_debit(500)) {
        auto& movie = m_Movies.at(target_movie);
        movie.Hype += points * g_Dice.in_range(90, 110) / 10000.f;
        m_PromotionPoints -= points;
        movie.TotalCost += 500;
    } else {
        g_Game->push_message("The ad campaign costs 500 gold!", COLOR_RED);
    }
}

void cMovieManager::gain_market_research_point() {
    m_MarketResearchPoints += 100;
}

void cMovieManager::make_survey(int target_audience) {
    int points = std::min(m_MarketResearchPoints, 500);
    if(g_Game->gold().misc_debit(500)) {
        auto& audience = m_Audiences.at(target_audience);
        audience.Knowledge += (points * g_Dice.in_range(10, 20)) / 1000;
        m_MarketResearchPoints -= points;
    }else {
        g_Game->push_message("The survey costs 500 gold!", COLOR_RED);
    }
}

int cMovieManager::market_research_points() const {
    return (m_MarketResearchPoints + 50) / 100;
}

