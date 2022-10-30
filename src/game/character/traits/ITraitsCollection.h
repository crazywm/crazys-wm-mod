#ifndef WM_ITRAITSCOLLECTION_H
#define WM_ITRAITSCOLLECTION_H

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "Constants.h"
#include "hDynamicTraitHandle.h"

namespace tinyxml2 {
    class XMLElement;
}

class ITraitSpec;
class ITraitsCollection;

struct sTraitID {
    /* implicit */ sTraitID(const ITraitSpec* p) : pointer(p) {  };
    /* implicit */ sTraitID(const char* n) : name(n) {  };

    const ITraitSpec* pointer = nullptr;
    const char* name = nullptr;
};

struct sTraitInfo {
    const ITraitSpec* trait;
    bool active;
    bool block;
    int remaining_time;
    enum ETraitType {
        INHERENT, PERMANENT, DYNAMIC
    } type;
};

/*!
 * \brief Collection of traits.
 * \details This class manages the traits of a girl. Traits are organised in three lists.
 * The first manages the inborn traits, that are (mostly) immutable, and define which traits will
 * be inherited by children. The second manages traits that are the result of permanent changes to
 * the girl. This may be something learned, or a modification from the clinic. Finally, the third
 * stack manages traits that are only temporary. These are either limited to a fixed amount of time,
 * or tied to an item and removed once the item is removed. These different kinds of traits are handled in
 * the following way:
 * Inherent Traits:
 *   These can be added, removed, and disabled. A disabled inherent trait is dormant, which means that it
 *   can be inherited by a child but are disabled for all other purposes.
 * Permanent Traits:
 *   These can be added, removed, and disabled. A disabled permanent trait is dormant, which can be used e.g. for
 *   overcome drug additions that might be restarted. Permanent traits are not inherent by children. If a permanent
 *   trait is added that excludes an existing permanent trait, this permanent trait is removed. If it excludes an
 *   existing inherent trait, the inherent trait is made dormant. That means that when the permanent trait is removed
 *   again, the inherent trait will not be re-activated automatically. If such behaviour is desired, the trait should be
 *   added as a non-temporary, dynamic trait.
 * Dynamic Traits:
 *   These can be added and removed, but there are no disabled. Dynamic traits might be temporary, which means that
 *   they will automatically be removed after a certain amount of time. Any inherent or permanent trait that is excluded
 *   by a dynamic trait is added to the set of blocked traits. Furthermore, it is possible to add a dynamic trait in an
 *   explicit blocking mode, which means that instead of adding that trait to the active traits, it is added to the
 *   set of blocked traits.
 *
 * Active Traits:
 *   These are the traits that are actually applied. These cannot be set explicitly, but are calculated in `update`
 *   based on inherent, permanent, and dynamic traits. These are all the traits that are neither dormant (not active),
 *   nor in the set of blocked traits.
 *
 * Blocked Traits:
 *   The set of traits that are blocked, i.e. the traits that will not be active. This included all traits that are
 *   explicitly set to blocked by a dynamic trait, and implicitly blocked by the exclusions of dynamical traits. The
 *   difficulty thing to handle is if one dynamical trait blocks another, because the blocked dynamical trait should
 *   not contribute to the blocking list. Therefore, the list of blocked traits is constructed as follows:
 *   The dynamical traits are processed right-to-left (most recently added first). Every trait that is not in the
 *   blocking set is processed. If it is a blocking trait, it is added to the set of blocked traits, otherwise the
 *   traits it excludes are added. The same processing is applied to permanent and inherited traits.
 * TODO should we allow/how to handle asymmetrical exclusion events?
 */
class ITraitsCollection {
public:
    enum ETraitState {
        TRAIT_MISSING,      // a trait that is not present
        TRAIT_ACTIVE,       // a trait that is present
        TRAIT_INACTIVE      // a trait that is present, but deactivated
    };

    virtual ~ITraitsCollection() = default;

    virtual void load_from_xml(const tinyxml2::XMLElement& root) = 0;
    virtual void save_to_xml(tinyxml2::XMLElement& root)   = 0;

    // inherent traits -- these can be inherited (even if disabled)
    /*!
     * Adds a new inherent trait. If the trait is added as active (the default) any inherent trait that conflicts with
     * this one will be made dormant, and permanent trait that conflict with it will be removed.
     * \param trait Identifies the new trait to be added.
     * \param active Whether the new trait should be active.
     * \return
     */
    bool add_inherent_trait(sTraitID trait, bool active=true) { return _add_inherent_trait(trait, active); }
    bool remove_inherent_trait(sTraitID trait) { return _remove_inherent_trait(trait, false); };
    bool disable_inherent_trait(sTraitID trait) { return _remove_inherent_trait(trait, true); };

    // permanent traits -- traits that the character has gained
    bool add_permanent_trait(sTraitID trait, bool active=true) { return _add_permanent_trait(trait, active); }
    bool remove_permanent_trait(sTraitID trait) { return _remove_permanent_trait(trait, false); };
    bool disable_permanent_trait(sTraitID trait) { return _remove_permanent_trait(trait, true); };

    // traits bestowed by items, spells etc.
    hDynamicTraitHandle add_dynamic_trait(sTraitID trait, bool block=false) { return _add_dynamic_trait(trait, block); }
    void add_temporary_trait(sTraitID trait, int duration, bool block=false)  { _add_temporary_trait(trait, block, duration); }

    /// removes the given amount of time from the remaining time of the temporary trait, and removes the trait if
    /// the time goes below zero. In that case, all traits are recalculated, i.e. there is no need to manually
    /// call update.
    virtual void decay_temporary_trait(sTraitID trait, int decay) = 0;

    /// Checks whether the given trait is present in the trait collation, and is not blocked (i.e. it is active).
    virtual bool has_active_trait(sTraitID trait) const = 0;

    virtual ETraitState has_inherent_trait(sTraitID trait) const = 0;
    virtual ETraitState has_permanent_trait(sTraitID trait) const = 0;

    // export comprehensive trait info. This function may be expensive
    virtual std::vector<sTraitInfo> get_trait_info() const = 0;

    /// Gets the set of all traits that are active at the moment.
    virtual const std::unordered_set<const ITraitSpec*>& get_active_traits() const = 0;

    /// simulates a time step: decreases remaining time of temporary traits, and removes expired.
    virtual void tick() = 0;

    /// update the active traits. This needs to be called after traits have been added/removed for the changes
    /// to have any effect.
    virtual void update() = 0;

    virtual const std::array<int, NUM_STATS>& stat_effects() const = 0;
    virtual const std::array<int, NUM_SKILLS>& skill_effects() const = 0;
    virtual const std::array<int, NUM_SKILLS>& skill_cap_effects() const = 0;
    virtual int get_modifier(const char* name) const = 0;

    /// Gets all modifiers.
    virtual const std::unordered_map<std::string, int>& get_all_modifiers() const = 0;

protected:
    virtual bool _add_inherent_trait(sTraitID trait, bool active) = 0;
    virtual bool _add_permanent_trait(sTraitID trait, bool active) = 0;

    virtual bool _remove_inherent_trait(sTraitID trait, bool deactivate) = 0;
    virtual bool _remove_permanent_trait(sTraitID trait, bool deactivate) = 0;

    virtual hDynamicTraitHandle _add_dynamic_trait(sTraitID trait, bool block) = 0;
    virtual void _add_temporary_trait(sTraitID trait, bool block, int duration) = 0;
    virtual void _remove_dynamic_trait(std::uint64_t h) = 0;

    friend hDynamicTraitHandle::~hDynamicTraitHandle();
};

inline hDynamicTraitHandle::~hDynamicTraitHandle() {
    if(collection)
        collection->_remove_dynamic_trait(handle);
}

inline hDynamicTraitHandle::hDynamicTraitHandle(hDynamicTraitHandle&& source) :
        collection(source.collection), handle(source.handle) {
    source.collection = nullptr;
}


#endif //WM_ITRAITSCOLLECTION_H
