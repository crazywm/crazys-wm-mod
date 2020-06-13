#ifndef WM_CDYNAMICSTAT_H
#define WM_CDYNAMICSTAT_H

class ICharacter;
#include <array>
#include <vector>
#include "Constants.h"

class cDynamicStat {
public:
    int operator()(const ICharacter& target);

    void set(STATS stat, float factor);
    void set(SKILLS skill, float factor);
    void set(std::string mod);
private:
    std::array<float, NUM_STATS> m_Stats;
    std::array<float, NUM_SKILLS> m_Skills;
    std::vector<std::string> m_Modifiers;
};


#endif //WM_CDYNAMICSTAT_H
