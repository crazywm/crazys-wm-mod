#include <algorithm>
#include <CLog.h>
#include "xml/util.h"
#include "xml/getattr.h"
#include <tinyxml2.h>
#include "ITraitSpec.h"
#include "cTraitsCollection.h"
#include "utils/algorithms.hpp"
#include "ITraitsManager.h"
#include "cTraitSpec.h"
#include "hDynamicTraitHandle.h"


bool cTraitsCollection::_add_inherent_trait(sTraitID id, bool active) {
    const ITraitSpec* trait = id_to_spec(id);

    auto existing = std::find_if(begin(m_InherentTraits), end(m_InherentTraits),
                                 [trait](const PermanentTrait& entry){ return entry.spec == trait; });

    bool has_added = false;
    // update an existing trait or add a new one
    if(existing != end(m_InherentTraits)) {
        // if either the existing one or the added one are active, make this trait active.
        existing->active |= active;
        has_added = !existing->active;
        active = existing->active;
    } else {
        m_InherentTraits.emplace_back(trait, active);
        has_added = true;
    }

    // TODO check whether this trait is blocked by an existing permanent trait. How do we handle that?
    // this is only relevant in cases where the exclusions are non-symmetric.
    if(active) {
        // make dormant all inherent traits that conflict with this newly acquired one.
        for (auto& inh : m_InherentTraits) {
            if (inh.active && trait->excludes(inh.spec->name())) {
                inh.active = false;
            }
        }

        // remove all permanent traits that conflict with this newly acquire one.
        for(auto it = begin(m_PermanentTraits); it != end(m_PermanentTraits); ++it) {
            if(trait->excludes(it->spec->name())) {
                it = m_PermanentTraits.erase(it);
                if(it == end(m_PermanentTraits)) {
                    break;
                }
            }
        }
    }

    if(has_added)
        m_DirtyFlag = true;

    return has_added;
}

bool cTraitsCollection::_add_permanent_trait(sTraitID id, bool active) {
    const ITraitSpec* trait = id_to_spec(id);

    auto existing = std::find_if(begin(m_PermanentTraits), end(m_PermanentTraits),
                                 [trait](const PermanentTrait& entry){ return entry.spec == trait; });

    // update an existing
    bool has_added = false;
    if(existing != end(m_PermanentTraits)) {
        // if either the existing one or the added one are active, make this trait active.
        existing->active |= active;
        has_added = !existing->active;
        active = existing->active;
    } else {
        m_PermanentTraits.emplace_back(trait, active);
        has_added = true;
    }

    // handle trait exclusions
    if(active) {
        // any inherent trait that is excluded by this trait will be deactivated
        for(auto& inh : m_InherentTraits) {
            if(trait->excludes(inh.spec->name()))
            {
                inh.active = false;
            }
        }

        // any permanent trait that is excluded by this trait will be removed
        for(auto it = begin(m_PermanentTraits); it != end(m_PermanentTraits); ++it) {
            if(trait->excludes(it->spec->name()))
            {
                it = m_PermanentTraits.erase(it);
                if(it == end(m_PermanentTraits))
                    break;
            }
        }
    }

    if(has_added)
        m_DirtyFlag = true;

    return has_added;
}

hDynamicTraitHandle cTraitsCollection::_add_dynamic_trait(sTraitID id, bool block) {
    const ITraitSpec* trait = id_to_spec(id);
    ++m_UUID_Counter;
    // dynamic traits can be stacked arbitrarily, so there are no checks required here
    m_DynamicTraits.emplace_back(trait, -1, m_UUID_Counter, block);
    m_DirtyFlag = true;

    return {this, m_UUID_Counter};
}

void cTraitsCollection::_add_temporary_trait(sTraitID id, bool block, int duration) {
    const ITraitSpec* trait = id_to_spec(id);

    auto result = find_if(m_DynamicTraits, [&](auto& t){ return t.spec == trait && t.block == block; });
    if(result != end(m_DynamicTraits)) {
        duration = std::max(duration, result->remaining_time);
        m_DynamicTraits.erase(result);
    }

    // in any case, the trait will be moved to the end of the list, so that it will overwrite any existing.
    m_DynamicTraits.emplace_back(trait, duration, -1, block);

    m_DirtyFlag = true;
}

bool cTraitsCollection::_remove_inherent_trait(sTraitID id, bool deactivate) {
    return _remove_trait_from_list(id, m_InherentTraits, deactivate);
}

bool cTraitsCollection::_remove_permanent_trait(sTraitID id, bool deactivate) {
    return _remove_trait_from_list(id, m_PermanentTraits, deactivate);
}

bool cTraitsCollection::_remove_trait_from_list(sTraitID id, std::vector<PermanentTrait> &tl, bool deactivate)
{
    const ITraitSpec* trait = id_to_spec(id);

    for(auto it = begin(tl); it != end(tl); ++it) {
        if(it->spec == trait) {
            // if the trait was not active beforehand, there was no change so dirty flag needs not be set
            if(it->active)
                m_DirtyFlag = true;

            if(deactivate) {
                if(it->active) {
                    it->active = false;
                    return true;
                }
                return false;
            } else {
                tl.erase(it);
                return true;
            }
        }
    }
    return false;
}

ITraitsCollection::ETraitState
cTraitsCollection::_find_trait_status(sTraitID id, const std::vector<PermanentTrait> &tl) const {
    auto trait = id_to_spec(id);
    for(auto& t : tl) {
        if(t.spec == trait) {
            if(t.active)
                return ITraitsCollection::TRAIT_ACTIVE;
            return ITraitsCollection::TRAIT_INACTIVE;
        }
    }
    return ITraitsCollection::TRAIT_MISSING;
}

ITraitsCollection::ETraitState cTraitsCollection::has_inherent_trait(sTraitID trait) const {
    check_dirty();
    return _find_trait_status(trait, m_InherentTraits);
}

ITraitsCollection::ETraitState cTraitsCollection::has_permanent_trait(sTraitID trait) const {
    check_dirty();
    return _find_trait_status(trait, m_PermanentTraits);
}

void cTraitsCollection::update() {
    if(!m_DirtyFlag)
        return;

    m_DirtyFlag = false;

    m_ActiveTraits.clear();
    m_BlockedTraits.clear();

    // start with the dynamic traits, as they are responsible for blocking
    // dynamics traits
    for(auto& trait : m_DynamicTraits) {
        // skip all traits that are blocked
        if(m_BlockedTraits.count(trait.spec->name()))
            continue;

        if(trait.block)
        {
            m_BlockedTraits.insert(trait.spec->name());
        }
        else
        {
            m_ActiveTraits.insert(trait.spec);
            auto& ex = trait.spec->exclusions();
            m_BlockedTraits.insert(begin(ex), end(ex));
        }
    }

    // permanent traits
    for(auto& trait : m_PermanentTraits) {
        if(trait.active  && !m_BlockedTraits.count(trait.spec->name()))
        {
            m_ActiveTraits.insert(trait.spec);
            auto& ex = trait.spec->exclusions();
            m_BlockedTraits.insert(begin(ex), end(ex));
        }
    }

    // inherent traits
    for(auto& trait : m_InherentTraits) {
        // ignore dormant traits
        if(trait.active && !m_BlockedTraits.count(trait.spec->name())) {
            m_ActiveTraits.insert(trait.spec);
            auto& ex = trait.spec->exclusions();
            m_BlockedTraits.insert(begin(ex), end(ex));
        }
    }


    // ok, at this point we have a list of traits that are actually active
    m_StatEffect.fill(0);
    m_SkillEffect.fill(0);
    m_Modifiers.clear();
    for(auto& spec : m_ActiveTraits) {
        for(auto& effect : dynamic_cast<const cTraitSpec*>(spec)->effects()) {
            apply_effect(effect);
        }
    }
}

void cTraitsCollection::tick() {
    // by construction, only dynamic traits can have a time limit.
    for(auto& data : m_DynamicTraits) {
        if(data.remaining_time > 0)
            data.remaining_time -= 1;
    }

    // remove expired traits. These are the traits with remaining_time == 0.
    // negative remaining time corresponds to permanent traits!
    auto new_end = std::remove_if(begin(m_DynamicTraits), end(m_DynamicTraits),
                                  [](const DynamicTrait& e) { return e.remaining_time == 0; });
    m_DirtyFlag |= new_end != end(m_DynamicTraits);
    m_DynamicTraits.erase(new_end, end(m_DynamicTraits));
    // if at least one trait was removed, recalculate trait effects.
    update();
}

const ITraitSpec *cTraitsCollection::id_to_spec(sTraitID id) const {
    if(id.pointer) return id.pointer;
    try {
        return m_TraitsManager->lookup(id.name);
    } catch(std::out_of_range& error) {
        g_LogFile.error("traits", "Could not find trait: ", id.name);
        throw std::out_of_range(std::string("Could not find trait: ") + id.name);
    }
}

bool cTraitsCollection::has_active_trait(sTraitID id) const {
    check_dirty();
    return m_ActiveTraits.count(id_to_spec(id));
}

const std::unordered_set<const ITraitSpec *> &cTraitsCollection::get_active_traits() const {
    check_dirty();
    return m_ActiveTraits;
}

void cTraitsCollection::load_from_xml(const tinyxml2::XMLElement &root) {
    for (const auto& elItem : IterateChildElements(root, "Trait"))
    {
        try {
            if (elItem.Attribute("Type", "Inherent")) {
                bool active = GetBoolAttribute(elItem, "Active");
                const char* name = GetStringAttribute(elItem, "Name");
                add_inherent_trait(name, active);
            } else if (elItem.Attribute("Type", "Permanent")) {
                bool active = GetBoolAttribute(elItem, "Active");
                const char* name = GetStringAttribute(elItem, "Name");
                add_permanent_trait(name, active);
            } else if (elItem.Attribute("Type", "Dynamic")) {
                bool block = GetBoolAttribute(elItem, "Block");
                const char* name = GetStringAttribute(elItem, "Name");
                int remaining_time = GetIntAttribute(elItem, "Remaining");
                add_temporary_trait(name, remaining_time, block);
            }
        } catch (std::out_of_range& oor) {
            g_LogFile.error("traits", "Ignoring unknown trait ", elItem.Attribute("Name"), " (found on line ", elItem.GetLineNum(), ")");
            // ignore the error
        }
    }
    m_DirtyFlag = true;
    update();
}

void cTraitsCollection::save_to_xml(tinyxml2::XMLElement &root) {
    for(auto& t : m_InherentTraits) {
        auto& el = PushNewElement(root, "Trait");
        el.SetAttribute("Name", t.spec->name().c_str());
        el.SetAttribute("Active", t.active);
        el.SetAttribute("Type", "Inherent");
    }

    for(auto& t : m_PermanentTraits) {
        auto& el = PushNewElement(root, "Trait");
        el.SetAttribute("Name", t.spec->name().c_str());
        el.SetAttribute("Active", t.active);
        el.SetAttribute("Type", "Permanent");
    }

    for(auto& t : m_DynamicTraits) {
        // only save temporaries, because the others have to be restored by their
        // respective owners.
        if(t.remaining_time > 0) {
            auto& el = PushNewElement(root, "Trait");
            el.SetAttribute("Name", t.spec->name().c_str());
            el.SetAttribute("Block", t.block);
            el.SetAttribute("Remaining", t.remaining_time);
            el.SetAttribute("Type", "Dynamic");
        }
    }
}

std::vector<sTraitInfo> cTraitsCollection::get_trait_info() const {
    check_dirty();
    std::vector<sTraitInfo> result;
    for(auto& t : m_InherentTraits) {
        result.push_back( sTraitInfo{t.spec, (bool)m_ActiveTraits.count(t.spec), t.active,-1, sTraitInfo::INHERENT} );
    }

    for(auto& t : m_PermanentTraits) {
        result.push_back( sTraitInfo{t.spec, (bool)m_ActiveTraits.count(t.spec), t.active,-1, sTraitInfo::PERMANENT} );
    }

    for(auto& t : m_DynamicTraits) {
        result.push_back( sTraitInfo{t.spec, (bool)m_ActiveTraits.count(t.spec), t.block, t.remaining_time, sTraitInfo::DYNAMIC} );
    }

    return std::move(result);
}

void cTraitsCollection::decay_temporary_trait(sTraitID id, int decay) {
    auto trait = id_to_spec(id);
    for(auto& t : m_DynamicTraits) {
        if(t.spec == trait && t.remaining_time > 0) {
            t.remaining_time -= 1;
        }
    }

    // remove expired traits. These are the traits with remaining_time == 0.
    // negative remaining time corresponds to permanent traits!
    auto new_end = std::remove_if(begin(m_DynamicTraits), end(m_DynamicTraits),
                                  [](const DynamicTrait& e) { return e.remaining_time == 0; });
    if(erase_if(m_DynamicTraits, [](const DynamicTrait& e) { return e.remaining_time == 0; })) {
        m_DirtyFlag = true;
    }
    // if at least one trait was removed, recalculate trait effects.
    update();
}

void cTraitsCollection::_remove_dynamic_trait(std::uint64_t h) {
    if(erase_if(m_DynamicTraits, [&](const DynamicTrait& e) { return e.id == h; })) {
        m_DirtyFlag = true;
    }
}

cTraitsCollection::cTraitsCollection(const ITraitsManager* mgr) : m_TraitsManager(mgr) {
    m_StatEffect.fill(0);
    m_SkillEffect.fill(0);
}

void cTraitsCollection::apply_effect(const TraitEffect& effect) {
    switch(effect.type) {
        case TraitEffect::STAT:
        {
            auto stat = (STATS)effect.target;
            // it does not make any sense to change these stats with a trait.
            if (stat == STAT_HEALTH || stat == STAT_HAPPINESS || stat == STAT_TIREDNESS || stat == STAT_EXP ||
                stat == STAT_LEVEL || stat == STAT_ASKPRICE) {
                /*
                CLog l;
                l.ss() << "Error: Trait tried to change stat '"<< stat << endl;
                l.ssend();
                */
                return;
            }
            m_StatEffect[stat] += effect.value;
        }
            break;
        case TraitEffect::SKILL:
            m_SkillEffect[effect.target] += effect.value;
            break;
        case TraitEffect::MODIFIER:
            m_Modifiers[effect.modifier] += effect.value;
    }
}

void cTraitsCollection::check_dirty() const {
    if(m_DirtyFlag)
        g_LogFile.warning("traits", "Accessing dirty traits collection");
}

int cTraitsCollection::get_modifier(const char* name) const {
    auto lookup = m_Modifiers.find(name);
    if(lookup != m_Modifiers.end()) {
        return lookup->second;
    } else {
        return 0;
    }
}
