#ifndef CRAZYS_WM_MOD_STREAMING_RANDOM_SELECTION_HPP
#define CRAZYS_WM_MOD_STREAMING_RANDOM_SELECTION_HPP


#include "cRng.h"

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
private:
    float m_TotalWeight    = 0.0;
    T* m_CurrentSelection  = nullptr;
};

#endif //CRAZYS_WM_MOD_STREAMING_RANDOM_SELECTION_HPP
