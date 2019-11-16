#include "ICharacter.h"
#include "predicates.h"

bool is_addict(const ICharacter& character, bool onlyhard) {
    if (onlyhard)
    {
        return character.get_trait_modifier("tag:hard-addiction") > 0;
    }
    return character.get_trait_modifier("tag:addiction") > 0;
}

bool has_disease(const ICharacter& character) {
    return character.get_trait_modifier("tag:disease") > 0;
}

bool is_virgin(const ICharacter& character) {
    return character.has_active_trait("Virgin");
}