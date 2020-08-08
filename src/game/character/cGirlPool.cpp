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

#include "cGirlPool.h"
#include <algorithm>
#include "xml/util.h"
#include "character/sGirl.h"
#include "utils/algorithms.hpp"
#include "utils/streaming_random_selection.hpp"

/// use this in predicate calling functions, so that they cannot modify the collection while we're iterating
#define ITERATING_FUNCTION sIterationGuard _dbg_helper_var(this);

std::size_t cGirlPool::num() const {
    return m_Pool.size();
}

sGirl* cGirlPool::get_girl(int index) {
    if(index < 0 || index >= m_Pool.size())
        return nullptr;

    return m_Pool[index].get();
}

const sGirl* cGirlPool::get_girl(int index) const {
    if(index < 0 || index >= m_Pool.size())
        return nullptr;

    return m_Pool[index].get();
}

void cGirlPool::AddGirl(std::shared_ptr<sGirl> girl) {
    if(girl == nullptr)
        throw std::invalid_argument("Trying to add nullptr to girl pool.");
    if(m_IsIterating) {
        m_WaitingToBeAdded.emplace_back(std::move(girl));
    } else {
        m_Pool.emplace_back(std::move(girl));
    }
}

void cGirlPool::LoadXML(const tinyxml2::XMLElement& source) {
    for (auto& pGirl : IterateChildElements(source, "Girl"))
    {
        auto girl = std::make_shared<sGirl>(false);
        if (girl->LoadGirlXML(&pGirl))
        {
            AddGirl(std::move(girl));
        }
    }
}

void cGirlPool::SaveXML(tinyxml2::XMLElement& target) const {
    for(auto& girl : m_Pool)
    {
        girl->SaveGirlXML(target);
    }
}

std::shared_ptr<sGirl> cGirlPool::TakeGirl(const sGirl* girl) {
    auto iter = std::find_if(begin(m_Pool), end(m_Pool),
                             [&](auto& ptr) { return ptr.get() == girl; });
    if (iter == m_Pool.end())
        return nullptr;

    auto result = *iter;

    // if we're not in an iteration, do a direct delete, otherwise
    // we need to set to zero and will clean up after the iteration
    // is completed
    if(m_IsIterating == 0) {
        m_Pool.erase(iter);
    } else {
        *iter = nullptr;
    }
    return std::move(result);
}

std::shared_ptr<sGirl> cGirlPool::TakeGirl(int index) {
    if(index < 0 || index >= num())
        return nullptr;
    auto iter = m_Pool.begin() + index;

    auto result = *iter;

    if(m_IsIterating == 0) {
        m_Pool.erase(iter);
    } else {
        *iter = nullptr;
    }

    return std::move(result);
}

bool cGirlPool::empty() const {
    return m_Pool.empty();
}

sGirl* cGirlPool::get_random_girl(const std::function<bool(const sGirl&)>& predicate) {
    ITERATING_FUNCTION;
    if(!predicate) {
        if(m_Pool.empty()) return nullptr;
        int index = g_Dice % m_Pool.size();
        if(sGirl* candidate = m_Pool[index].get()) {
            return candidate;
        }

        // OK, we seem to be in iteration mode and have picked a deleted girl.
        // Have to do this the hard way
        return get_random_girl([](const sGirl& g){ return true; });
    }

    RandomSelector<sGirl> selector;
    for(auto& girl : m_Pool)
    {
        if(!girl) continue;
        if (predicate(*girl))    selector.process(girl.get());
    }
    return selector.selection();
}

const sGirl* cGirlPool::get_random_girl(const std::function<bool(const sGirl&)>& predicate) const {
    return const_cast<cGirlPool*>(this)->get_random_girl(predicate);
}

sGirl* cGirlPool::get_first_girl(const std::function<bool(const sGirl&)>& predicate) {
    ITERATING_FUNCTION;
    for(auto& girl : m_Pool)
    {
        if(!girl) continue;
        if (predicate(*girl))    return girl.get();
    }
    return nullptr;
}

const sGirl* cGirlPool::get_first_girl(const std::function<bool(const sGirl&)>& predicate) const {
    return const_cast<cGirlPool*>(this)->get_first_girl(predicate);
}


std::size_t cGirlPool::count(const std::function<bool(const sGirl&)>& predicate) const {
    ITERATING_FUNCTION;
    return std::count_if(begin(m_Pool), end(m_Pool), [&](const auto& ptr) {
        if(!ptr) return false;
        return predicate(*ptr);
    });
}

bool cGirlPool::has_any(const std::function<bool(const sGirl&)>& predicate) const {
    ITERATING_FUNCTION;
    return std::any_of(begin(m_Pool), end(m_Pool), [&](const auto& ptr) {
        if(!ptr) return false;
        return predicate(*ptr);
    });
}

int cGirlPool::get_index(const sGirl* girl) const {
    // this iterates, but has no predicate so it is save
    auto found = std::find_if(begin(m_Pool), end(m_Pool), [&](auto& ptr){ return ptr.get() == girl; });
    if(found != end(m_Pool)) {
        return std::distance(begin(m_Pool), found);
    }
    return -1;
}

void cGirlPool::visit(const std::function<void(const sGirl&)>& handler) const {
    ITERATING_FUNCTION;
    for(auto& g : m_Pool) {
        if (g) {
            handler(*g);
        }
    }
}

void cGirlPool::apply(const std::function<void(sGirl&)>& handler) {
    ITERATING_FUNCTION;
    for(auto& g : m_Pool) {
        if (g) {
            handler(*g);
        }
    }
}

std::shared_ptr<sGirl> cGirlPool::get_ref_counted(const sGirl* source) {
    auto iter = std::find_if(begin(m_Pool), end(m_Pool),
                             [&](auto& ptr){ return ptr.get() == source; });
    if(iter == m_Pool.end())
        return nullptr;

    return *iter;
}

void cGirlPool::finalize_after_iteration() {
    // get rid of all deleted girls
    erase_if(m_Pool, [](const auto& gptr){ return gptr == nullptr; });

    // add the new girls
    std::move(begin(m_WaitingToBeAdded), end(m_WaitingToBeAdded), std::back_inserter(m_Pool));
    m_WaitingToBeAdded.clear();
}

cGirlPool::sIterationGuard::~sIterationGuard() {
    --(gp->m_IsIterating);
    // if no one is iterating anymore, clean up
    if(gp->m_IsIterating == 0) {
        const_cast<cGirlPool*>(gp)->finalize_after_iteration();
    }
}
