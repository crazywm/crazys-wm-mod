#ifndef WM_PREDICATES_H
#define WM_PREDICATES_H

class ICharacter;

/// returns whether the character is an addict. If `onlyhard`, consider only hard drugs.
bool is_addict(const ICharacter& character, bool onlyhard = false);

/// returns whether the character has a disease.
bool has_disease(const ICharacter& character);

/// returns whether the character is a virgin.
bool is_virgin(const ICharacter& character);

#endif //WM_PREDICATES_H
