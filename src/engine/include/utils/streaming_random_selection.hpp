#ifndef CRAZYS_WM_MOD_STREAMING_RANDOM_SELECTION_HPP
#define CRAZYS_WM_MOD_STREAMING_RANDOM_SELECTION_HPP


#include "cRng.h"
#include <boost/optional.hpp>
#include <random>

extern cRng g_Dice;

/*!
 * \brief Random selection from a stream of data.
 * \tparam T Type of the selected object.
 * \details This class allows to randomly select an element from a stream of data without knowing beforehand
 * how many elements there will be. This is useful because it allows e.g. to select uniformly from all elements
 * of a list that fulfill a certain criterion without having to build an intermediate container for all elements
 * that fulfil the predicate. In total, this algorithm has O(1) memory and O(N) computational cost instead of
 * O(N) memory O(1) computation as in the case of building a buffer.
 * \note For forward iterators there might be a sometimes more efficient implementation based on iterating the
 * sequence twice. Which algorithm would be more efficient depends on whether the rng or the predicate are more
 * expensive.
 */
template<class T>
class RandomSelector {
public:
    void process(T* element, float weight = 1) {
        // streaming uniform selection. n'th element has chance w[n]/sum(w[i]) for being selected.
        m_TotalWeight += weight;
        double p = 100.0 * weight / m_TotalWeight;
        // TODO this is a bad rng; percentages are rounded
        if (g_Dice.percent(p)) {
            m_CurrentSelection = element;
        }
    }

    T* selection() const {
        return m_CurrentSelection;
    }

    void reset() {
        m_TotalWeight = 0;
        m_CurrentSelection = nullptr;
    }
private:
    float m_TotalWeight    = 0.0;
    T* m_CurrentSelection  = nullptr;
};

template<class T>
class RandomSelectorV2 {
public:
    template<class Rng>
    void process(Rng& rng, const T& element, int priority = 0, float weight = 1) {
        // skip when using lower priority
        if(priority < m_Priority) return;

        // reset when reaching higher priority
        if(priority > m_Priority) {
            m_Priority = priority;
            m_TotalWeight = 0.f;
        }

        // streaming uniform selection. n'th element has chance w[n]/sum(w[i]) for being selected.
        m_TotalWeight += weight;
        double p = weight / m_TotalWeight;
        if (m_Dist(rng) < p) {
            m_CurrentSelection = element;
        }
    }

    const boost::optional<T>& selection() const {
        return m_CurrentSelection;
    }

    int priority() const {
        return m_Priority;
    }

    float weight() const {
        return m_TotalWeight;
    }

    void reset() {
        m_TotalWeight = 0;
        m_CurrentSelection = boost::none;
    }
private:
    float m_TotalWeight    = 0.0;
    int m_Priority = std::numeric_limits<int>::min();
    boost::optional<T> m_CurrentSelection = boost::none;
    std::uniform_real_distribution<double> m_Dist = std::uniform_real_distribution<double>(0.0, 1.0);
};

#endif //CRAZYS_WM_MOD_STREAMING_RANDOM_SELECTION_HPP
