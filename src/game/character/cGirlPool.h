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
#ifndef WM_CGIRLPOOL_H
#define WM_CGIRLPOOL_H

#include <memory>
#include <vector>
#include <functional>

class sGirl;
namespace tinyxml2 {
    class XMLElement;
}

/*!
 * Manages a container of girls using shared_ptr.
 */
class cGirlPool {
public:
    /// Adds a new girl to the pool
    void AddGirl(std::shared_ptr<sGirl> girl);

    /// Takes the girl identified by the given pointer out of the pool
    std::shared_ptr<sGirl> TakeGirl(const sGirl* girl);
    /// Takes the girl identified by the given index out of the pool
    std::shared_ptr<sGirl> TakeGirl(int index);

    /// Gets the number of girls in the pool
    std::size_t num() const;
    /// returns whether the pool is empty
    bool empty() const;
    /// Gets the number of girls matching a predicate
    std::size_t count(const std::function<bool(const sGirl&)>& ptr) const;
    /// Checks if any girl matches the predicate
    bool has_any(const std::function<bool(const sGirl&)>& ptr) const;

    /// Get by number. Returns nullptr if index is not valid.
    sGirl* get_girl(int index);
    const sGirl* get_girl(int index) const;

    sGirl* find_by_id(std::uint64_t id);
    const sGirl* find_by_id(std::uint64_t id) const;

    /// Gets a shared_ptr
    std::shared_ptr<sGirl> get_ref_counted(const sGirl* source);

    /// Gets an index correspodnign to a girl
    int get_index(const sGirl* girl_ptr) const;

    /// returns a pointer to a random girl that fulfills the given predicate. If no predicate is given, chooses
    /// uniformly at random.
    sGirl* get_random_girl(const std::function<bool(const sGirl&)>& predicate = {});
    const sGirl* get_random_girl(const std::function<bool(const sGirl&)>& predicate) const;
    /// gets a pointer to the first girl that fulfills the predicate
    sGirl* get_first_girl(const std::function<bool(const sGirl&)>& predicate);
    const sGirl* get_first_girl(const std::function<bool(const sGirl&)>& predicate) const;

    /// visits all girls
    void visit(const std::function<void(const sGirl&)>& handler) const;
    void apply(const std::function<void(sGirl&)>& handler);

    // io
    void LoadXML(const tinyxml2::XMLElement& source);
    void SaveXML(tinyxml2::XMLElement& target) const;
private:
    std::vector<std::shared_ptr<sGirl>> m_Pool;
    /// these are the girls that will be added after all iterations are finalized.
    std::vector<std::shared_ptr<sGirl>> m_WaitingToBeAdded;

    /// This keeps track of how many nested iteration loops we're in.
    mutable int m_IsIterating = 0;

    struct sIterationGuard {
        const cGirlPool* gp;
        explicit sIterationGuard(const cGirlPool* g) : gp(g) { ++(gp->m_IsIterating); }
        ~sIterationGuard();
    };
    friend class sIterationGuard;

    void finalize_after_iteration();
};

#endif //WM_CGIRLPOOL_H
