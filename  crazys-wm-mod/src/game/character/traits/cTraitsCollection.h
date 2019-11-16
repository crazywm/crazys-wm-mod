#ifndef WM_CTRAITSCOLLECTION_H
#define WM_CTRAITSCOLLECTION_H

#include "ITraitsCollection.h"
#include "hDynamicTraitHandle.h"
#include <vector>
#include <list>
#include <unordered_set>
#include <unordered_map>

class ITraitsManager;
class TraitEffect;

class cTraitsCollection : public ITraitsCollection {
public:
    explicit cTraitsCollection(const ITraitsManager* mgr);

    std::vector<sTraitInfo> get_trait_info() const override;
    ETraitState has_inherent_trait(sTraitID trait) const override;
    ETraitState has_permanent_trait(sTraitID trait) const override;
    bool has_active_trait(sTraitID trait) const override;

    const std::unordered_set<const ITraitSpec*>& get_active_traits() const override;

    void update() override;
    void tick() override;

    void load_from_xml(const tinyxml2::XMLElement& root) override;
    void save_to_xml(tinyxml2::XMLElement& root) override;

    void decay_temporary_trait(sTraitID trait, int decay) override;

    /// gets the fetish rating that the active traits produce
    const std::array<int, NUM_STATS>& stat_effects() const override { return m_StatEffect; }
    const std::array<int, NUM_SKILLS>& skill_effects() const override { return m_SkillEffect; }

    int get_modifier(const char* name) const override;

private:
    const ITraitSpec* id_to_spec(sTraitID id) const;

    bool _add_inherent_trait(sTraitID trait, bool active) override;
    bool _add_permanent_trait(sTraitID trait, bool active) override;
    hDynamicTraitHandle _add_dynamic_trait(sTraitID trait, bool block) override;
    void _add_temporary_trait(sTraitID trait, bool block, int duration) override;

    bool _remove_inherent_trait(sTraitID trait, bool deactivate) override;
    bool _remove_permanent_trait(sTraitID trait, bool deactivate) override;
    void _remove_dynamic_trait(std::uint64_t h) override;

    struct DynamicTrait {
        DynamicTrait(const ITraitSpec* s, int t, std::uint64_t uuid, bool b) :
        spec(s), remaining_time(t), id(uuid), block(b) {
            if(!s)
                throw std::logic_error("Invalid dynamic trait! No 'TraitSpec' specified");
        }

        const ITraitSpec* spec;

        //! If this is a temporary trait, remaining time tells how many weeks the trait will remain.
        //! For permanent traits this is set to -1.
        int remaining_time;

        //! A unique number (within this collection) used to track traits.
        std::uint64_t id;

        //! If this is set to true, the specified trait is not added, but removed from the set of active traits
        bool block;
    };

    //! A structure that gathers the data for an inherent (inborn) or permanent (learned) trait.
    //! This is just the corresponding `TraitSpec` and whether the trait is active or dormant.
    //! A structure that gathers the data for an inherent (inborn) or permanent (learned) trait.
    //! This is just the corresponding `TraitSpec` and whether the trait is active or dormant.
    struct PermanentTrait {
        explicit PermanentTrait(const ITraitSpec* s, bool a = true) : spec(s), active(a) {
            if(!s)
                throw std::logic_error("Invalid permanent trait! No 'TraitSpec' specified");
        }

        //! TraitSpec of this trait.
        const ITraitSpec* spec;
        //! Whether this trait is active (true) or dormant(false)
        bool active;
    };

    std::vector<PermanentTrait> m_InherentTraits;
    std::vector<PermanentTrait> m_PermanentTraits;
    std::list<DynamicTrait> m_DynamicTraits;
    std::uint64_t m_UUID_Counter = 0;

    /// The set of traits that are blocked.
    std::unordered_set<std::string> m_BlockedTraits;

    /// This set collects all the traits that are actually active.
    std::unordered_set<const ITraitSpec*> m_ActiveTraits;

    /// Cache for the fetish effects of the traits
    std::array<int, NUM_STATS> m_StatEffect;
    std::array<int, NUM_SKILLS> m_SkillEffect;
    std::unordered_map<std::string, int> m_Modifiers;

    bool _remove_trait_from_list(sTraitID trait, std::vector<PermanentTrait>& tl, bool deactivate);
    ETraitState _find_trait_status(sTraitID trait, const std::vector<PermanentTrait>& tl) const;

    bool m_DirtyFlag = false;
    void check_dirty() const;

    const ITraitsManager* m_TraitsManager;

    void apply_effect(const TraitEffect& effect);
};

#endif //WM_CTRAITSCOLLECTION_H
