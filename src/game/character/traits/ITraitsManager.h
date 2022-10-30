#ifndef WM_ITRAITSMANAGER_H
#define WM_ITRAITSMANAGER_H

#include <memory>
#include <vector>

namespace tinyxml2 {
    class XMLElement;
}

class ITraitsCollection;
class ITraitSpec;

// Manages and loads the traits file
class ITraitsManager
{
public:
    virtual ~ITraitsManager() = default;

    virtual void load_xml(const tinyxml2::XMLElement& root) = 0;
    virtual void load_modifiers(const tinyxml2::XMLElement& root, std::string prefix) = 0;
    virtual bool load_modifier(const tinyxml2::XMLElement& root, std::string name) = 0;

    virtual std::unique_ptr<ITraitsCollection> create_collection() const = 0;
    virtual const ITraitSpec* lookup(const char* name) const = 0;

    virtual std::vector<std::string> get_all_traits() const = 0;
    virtual void iterate(std::function<void(const ITraitSpec&)> callback) const = 0;
};

#endif //WM_ITRAITSMANAGER_H
