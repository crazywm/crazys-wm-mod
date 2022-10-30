#ifndef WM_CTRAITSMANAGER_H
#define WM_CTRAITSMANAGER_H

#include <unordered_map>
#include "ITraitsManager.h"
#include "ITraitSpec.h"

class cTraitSpec;

struct CaseInsensitiveHash {
    std::size_t operator()(const std::string& foo) const;
    bool operator()(const std::string& a, const std::string& b) const;
};

struct sLoadData {
    std::vector<std::string> exclusions;
    std::vector<std::pair<std::string, int>> modifiers;
};

class cTraitsManager : public ITraitsManager {
public:
    cTraitsManager() ;
    ~cTraitsManager() noexcept override;
    cTraitsManager(cTraitsManager&&) noexcept;

    void load_xml(const tinyxml2::XMLElement& doc) override;

    void load_traits(const tinyxml2::XMLElement& doc);
    void load_modifiers(const tinyxml2::XMLElement& root, std::string prefix) override;
    bool load_modifier(const tinyxml2::XMLElement& root, std::string name) override;

    std::unique_ptr<ITraitsCollection> create_collection() const override;
    const ITraitSpec* lookup(const char* name) const override;

    std::vector<std::string> get_all_traits() const override;
    void iterate(std::function<void(const ITraitSpec&)> callback) const override;
private:
    // helper functions
    void add_trait(std::unique_ptr<cTraitSpec> spec);

    // these are caches used to break circular dependencies while loading
    std::unordered_map<std::string, sLoadData, CaseInsensitiveHash, CaseInsensitiveHash> m_LoaderCache;

    // The actual trait data
    std::unordered_map<std::string, std::unique_ptr<cTraitSpec>, CaseInsensitiveHash, CaseInsensitiveHash> m_Traits;

};

#endif //WM_CTRAITSMANAGER_H
