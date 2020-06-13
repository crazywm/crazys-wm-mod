#ifndef WM_ITRAITSPEC_H
#define WM_ITRAITSPEC_H

#include <string>
#include <array>
#include <vector>
#include "Constants.h"
#include "IKeyValueStore.h"
#include <cassert>

class ITraitSpec {
public:
    virtual ~ITraitSpec() = default;

    virtual const std::string& name() const = 0;
    virtual const std::string& display_name() const = 0;
    virtual const std::string& desc() const = 0;

    virtual const std::vector<std::string>& exclusions() const = 0;

    /// Returns `true` if this trait excludes the trait with the given `trait_name`.
    virtual bool excludes(const std::string& trait_name) const = 0;

    virtual IKeyValueStore& get_properties() const = 0;
};


#endif //WM_ITRAITSPEC_H
