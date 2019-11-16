#include "cDynamicStat.h"
#include "ICharacter.h"

int cDynamicStat::operator()(const ICharacter& target) {
    float result = 0;
    for(int i = 0; i < NUM_STATS; ++i) {
        result += m_Stats[i] * target.get_stat(i);
    }
    for(int i = 0; i < NUM_SKILLS; ++i) {
        result += m_Skills[i] * target.get_skill(i);
    }
    for(auto& mod : m_Modifiers) {
        result += target.get_trait_modifier(mod.c_str());
    }

    return result;
}

void cDynamicStat::set(STATS stat, float factor) {
    m_Stats[stat] = factor;
}

void cDynamicStat::set(SKILLS skill, float factor) {
    m_Skills[skill] = factor;
}

void cDynamicStat::set(std::string mod) {
    m_Modifiers.push_back(std::move(mod));
}

