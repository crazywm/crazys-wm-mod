#include "cTraitSpec.h"
#include "xml/getattr.h"
#include "xml/util.h"
#include <tinyxml2.h>
#include <utils/cKeyValueStore.h>
#include "CLog.h"
#include <boost/algorithm/string.hpp>
#include <regex>


TraitEffect TraitEffect::from_xml(const tinyxml2::XMLElement& el)
{
    TraitEffect effect;
    if(el.QueryAttribute("Value", &effect.value) != tinyxml2::XML_SUCCESS)
        throw std::runtime_error("No 'value' specified for TraitEffect!");

    std::string target = GetStringAttribute(el, "Name");
    if(boost::algorithm::istarts_with(target, "stat:")) {
        effect.type = TraitEffect::STAT;
        effect.target = get_stat_id(target.substr(5));
    } else if(boost::algorithm::istarts_with(target, "skill:")) {
        effect.type = TraitEffect::SKILL;
        effect.target = get_skill_id(target.substr(6));
    } else if(boost::algorithm::istarts_with(target, "skill-cap:")) {
        effect.type = TraitEffect::SKILL_CAP;
        effect.target = get_skill_id(target.substr(10));
    } else {
        effect.type = TraitEffect::MODIFIER;
        effect.modifier = target;
    }
    return effect;
}

namespace {
    std::regex valid_trait{R"([[:alpha:]][\w -]*[[:alnum:]])", std::regex_constants::optimize};
}


std::unique_ptr<cTraitSpec> cTraitSpec::from_xml(const tinyxml2::XMLElement& el, cTraitProps* default_properties) {
    std::string name = GetStringAttribute(el, "Name");
    if(name.empty()) {
        throw std::runtime_error("Trait name cannot be empty!");
    } else if( !std::regex_match(name, valid_trait) ) {

        throw std::runtime_error("Invalid Trait name '" + name + '\'');
    }

    auto props = default_properties ? default_properties->clone() : std::make_unique<cTraitProps>();
    auto pptr = props.get();

    auto trait = std::make_unique<cTraitSpec>(name, "", std::move(props));

    for (auto& sub : IterateChildElements(el))
    {
        std::string tag = sub.Value();
        if(tag == "description") {
            trait->m_Desc = sub.GetText();
        }
        else if(tag == "Modifiers") {

            for (auto& effect_el : IterateChildElements(sub))
            {
                trait->m_Effects.push_back(TraitEffect::from_xml(effect_el));
            }
        }
        else if(tag == "Properties") {
            // this is safe, because `trait` now owns the properties object.
            pptr->load_xml(sub);
        }
        else if(tag == "Excludes") {
            // this is safe, because `trait` now owns the properties object.
            trait->m_Exclusions.emplace_back(sub.GetText() );
        }
        else {
            g_LogFile.log(ELogLevel::WARNING, "Encountered unexpected XML Element '", tag, "' when loading trait '", name , '\'');
        }
    }

    return trait;
}

cTraitSpec::cTraitSpec(std::string name, std::string description, std::unique_ptr<IKeyValueStore> properties) :
        m_Name(std::move(name)),
        m_Desc(std::move(description)),
        m_Properties(std::move(properties))
{
}

const std::vector<std::string>& cTraitSpec::exclusions() const {
    return m_Exclusions;
}

bool cTraitSpec::excludes(const std::string& trait_name) const {
    return false;
}

IKeyValueStore& cTraitSpec::get_properties() const {
    return *m_Properties;
}

void cTraitSpec::add_exclude(std::string c) {
    m_Exclusions.push_back(std::move(c));
}

void cTraitSpec::add_modifier(std::string mod, int value) {
    m_Effects.push_back(TraitEffect{TraitEffect::MODIFIER, 0, std::move(mod), value});
}

cTraitProps::cTraitProps() : cSimpleKeyValue("Setting", "Name", "Value") {
    // inheritance
    add_setting(traits::properties::INHERIT_CHANCE, "Inherit Chance", sPercent(50), "Base chance for inheriting this trait.");
    add_setting(traits::properties::INHERIT_CHANCE_FROM_DORMANT, "Inherit chance (from dormant)", boost::blank(),
               "Chance that this trait will be inherited from a dormant trait.", "inherit:chance");

    add_setting("inherit:dormant", "Dormant Chance (inherited)", boost::blank(),
            "Chance that this trait will be dormant when inherited.", "random:inherent:dormant");
    add_setting("inherit:dormant_from_dormant", "Dormant Chance (inherited from dormant)", sPercent(75),
                "Chance that this trait will be dormant when inherited from dormant.");
    add_setting("inherit:from_acquired", "Transmission Chance", sPercent(0),
                "Chance that an active acquired trait (such as a disease) is passed on to offspring as an acquired trait.");

    // random spawning
    add_setting("random:chance", "Random Chance", sPercent(1), "Chance for this trait to appear in a randomly generated girl if not specified in the template.");
    add_setting("random:inherent", "Inherent Chance (random)", sPercent(50), "Chance that this trait will be generated as an inherent trait.");
    add_setting("random:inherent:dormant", "Dormant Chance (random inherent)", sPercent(5),
            "Chance that this trait will be generated dormant when randomly generated as an inherent trait.");
    add_setting("random:acquired:dormant", "Dormant Chance (random acquired)", sPercent(0),
            "Chance that this trait will be generated dormant when randomly generated as an acquired trait.");
}

std::unique_ptr<cTraitProps> cTraitProps::clone() const {
    auto copy = std::make_unique<cTraitProps>();
    copy->m_Settings = m_Settings;
    return std::move(copy);
}
