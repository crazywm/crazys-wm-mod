#ifndef WM_CTRAITSPEC_H
#define WM_CTRAITSPEC_H

#include "ITraitSpec.h"
#include <memory>
#include "utils/cKeyValueStore.h"

namespace tinyxml2 {
    class XMLElement;
}

struct TraitEffect
{
    enum Type {
        STAT, SKILL, MODIFIER
    } type;

    unsigned target;
    std::string modifier;
    int value;

    static TraitEffect from_xml(const tinyxml2::XMLElement& el);
};

class ICharacter;

class cTraitProps : public cSimpleKeyValue  {
public:
    cTraitProps();

    std::unique_ptr<cTraitProps> clone() const;
};


class cTraitSpec : public ITraitSpec {
public:
    cTraitSpec(std::string name, std::string description, std::unique_ptr<IKeyValueStore>);

    static std::unique_ptr<cTraitSpec> from_xml(const tinyxml2::XMLElement& el,
            cTraitProps* default_properties = nullptr);

    const std::string& name() const override { return m_Name; }
    const std::string& display_name() const override { return m_Name; }
    const std::string& desc() const override { return m_Desc; }

    IKeyValueStore& get_properties() const override;

    const std::vector<TraitEffect>& effects() const { return m_Effects; }
    const std::vector<std::string>& exclusions() const override;
    /// Returns `true` if this trait excludes the trait with the given `trait_name`.
    bool excludes(const std::string& trait_name) const override;


    void add_exclude(std::string c);
    void add_modifier(std::string mod, int value);

private:
    std::string m_Name;				// the name and unique ID of the trait
    std::string m_Desc;				// a description of the trait

    std::vector<TraitEffect> m_Effects;

    std::vector<std::string> m_Exclusions;

    std::unique_ptr<IKeyValueStore> m_Properties;
};

#endif //WM_CTRAITSPEC_H
