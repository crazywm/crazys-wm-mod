#include "utils/string.hpp"
#include "cTraitsManager.h"
#include "cTraitsCollection.h"
#include "cTraitSpec.h"

#include <tinyxml2.h>
#include <CLog.h>
#include <xml/getattr.h>
#include "xml/util.h"

std::unique_ptr<ITraitsCollection> cTraitsManager::create_collection() const {
    return std::make_unique<cTraitsCollection>(this);
}

const ITraitSpec *cTraitsManager::lookup(const char* name) const {
    return m_Traits.at(name).get();
}

void cTraitsManager::load_traits(const tinyxml2::XMLElement& root) {
    std::unique_ptr<cTraitProps> def = {};

    // First load the default settings for this file.
    if(auto def_el = root.FirstChildElement("Default"))
    {
        def = std::make_unique<cTraitProps>();
        def->load_xml(*def_el);
    }

    // Then load all the traits
    for (auto& el : IterateChildElements(root, "Trait"))
    {
        add_trait(cTraitSpec::from_xml(el, def.get()));
    }

    // check exclusion groups
    for (auto& el : IterateChildElements(root, "MutexGroup"))
    {
        std::vector<std::string> mutex_group;
        for(auto& t : IterateChildElements(el, "Trait")) {
            mutex_group.emplace_back(t.GetText());
        }
        for(int i = 0; i < mutex_group.size(); ++i) {
            for(int j = 0; j < mutex_group.size(); ++j) {
                if(i != j) {
                    dynamic_cast<cTraitSpec*>(m_Traits.at(mutex_group[i]).get())->add_exclude(mutex_group[j]);
                    dynamic_cast<cTraitSpec*>(m_Traits.at(mutex_group[j]).get())->add_exclude(mutex_group[i]);
                }
            }
        }
    }
}

void cTraitsManager::load_xml(const tinyxml2::XMLElement& root) {
    if(std::strcmp(root.Value(), "Traits") == 0) {
        load_traits(root);
    } else if(std::strcmp(root.Value(), "TraitMods") == 0) {
        load_mods(root);
    } else {
        throw std::runtime_error("Unknown root element: " + std::string(root.Value()));
    }
}

void cTraitsManager::load_mods(const tinyxml2::XMLElement& root) {
    for (auto& modifier_el : IterateChildElements(root, "Modifier")) {
        const char* modifier = GetStringAttribute(modifier_el, "Name");
        bool has_modifiers = false;
        for (auto& el : IterateChildElements(modifier_el, "Trait")) {
            const char* trait_name = GetStringAttribute(el, "Name");
            int value = GetIntAttribute(el, "Value");
            auto trait = m_Traits.find(trait_name);
            if (trait != m_Traits.end()) {
                trait->second->add_modifier(modifier, value);
            } else {
                m_LoaderCache[trait_name].modifiers.emplace_back(modifier, value);
            }
            has_modifiers = true;
        }
        if(!has_modifiers) {
            g_LogFile.warning("traits", "Did not find any <Trait> element for modifier ", modifier);
        }
    }
}

void cTraitsManager::add_trait(std::unique_ptr<cTraitSpec> spec) {
    auto found = m_LoaderCache.find(spec->name());

    // if there is data already declared for this trait, add that data
    if(found != m_LoaderCache.end()) {
        for(auto& excl : found->second.exclusions) {
            spec->add_exclude(std::move(excl));
        }

        for(auto& mod : found->second.modifiers) {
            spec->add_modifier(std::move(mod.first), mod.second);
        }

        m_LoaderCache.erase(found);
    }
    m_Traits.emplace(spec->name(), std::move(spec));
}

cTraitsManager::cTraitsManager() = default;
cTraitsManager::~cTraitsManager() noexcept = default;
cTraitsManager::cTraitsManager(cTraitsManager&&) noexcept = default;

std::size_t CaseInsensitiveHash::operator()(const std::string& name) const {
    return case_insensitive_hash(name);
}

bool CaseInsensitiveHash::operator()(const std::string& a, const std::string& b) const {
    return iequals(a, b);
}
